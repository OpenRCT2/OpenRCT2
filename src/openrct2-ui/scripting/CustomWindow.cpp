/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <limits>
#include <optional>
#include <string>
#include <vector>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/scripting/Plugin.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include "ScUi.hpp"
#include "ScWindow.hpp"

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CONTENT_PANEL,
        WIDX_CUSTOM_BEGIN,
    };

    static rct_widget CustomDefaultWidgets[] = {
        { WWT_FRAME,    0, 0, 0, 0,  0,  0xFFFFFFFF,            STR_NONE             }, // panel / background
        { WWT_CAPTION,  0, 1, 0, 1,  14, STR_STRING,            STR_WINDOW_TITLE_TIP }, // title bar
        { WWT_CLOSEBOX, 0, 0, 0, 2,  13, STR_CLOSE_X,           STR_CLOSE_WINDOW_TIP }, // close x button
        { WWT_RESIZE,   1, 0, 0, 14, 0,  0xFFFFFFFF,            STR_NONE             }, // content panel
    };

    static void window_custom_close(rct_window *w);
    static void window_custom_mouseup(rct_window *w, rct_widgetindex widgetIndex);
    static void window_custom_resize(rct_window *w);
    static void window_custom_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
    static void window_custom_invalidate(rct_window *w);
    static void window_custom_paint(rct_window *w, rct_drawpixelinfo *dpi);
    static void window_custom_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

    static rct_window_event_list window_custom_events =
    {
        window_custom_close,
        window_custom_mouseup,
        window_custom_resize,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        window_custom_invalidate,
        window_custom_paint,
        nullptr
    };

    struct CustomWidgetDesc
    {
        // Properties
        std::string Type;
        sint32 X;
        sint32 Y;
        sint32 Width;
        sint32 Height;
        std::string Text;

        // Event handlers
        DukValue OnClick;

        static CustomWidgetDesc FromDukValue(DukValue desc)
        {
            CustomWidgetDesc result;
            result.Type = desc["type"].as_string();
            result.X = desc["x"].as_int();
            result.Y = desc["y"].as_int();
            result.Width = desc["width"].as_int();
            result.Height = desc["height"].as_int();
            result.Text = desc["text"].as_string();
            result.OnClick = desc["onClick"];
            return result;
        }
    };

    struct CustomWindowDesc
    {
        std::string Classification;
        std::optional<sint32> X;
        std::optional<sint32> Y;
        sint32 Width;
        sint32 Height;
        std::optional<sint32> MinWidth;
        std::optional<sint32> MinHeight;
        std::optional<sint32> MaxWidth;
        std::optional<sint32> MaxHeight;
        std::string Title;
        std::optional<sint32> Id;
        std::vector<CustomWidgetDesc> Widgets;

        CustomWindowDesc() = default;

        bool IsResizable() const
        {
            return MinWidth || MinHeight || MaxWidth || MaxHeight;
        }

        static CustomWindowDesc FromDukValue(DukValue desc)
        {
            CustomWindowDesc result;
            result.Classification = desc["classification"].as_string();
            result.X = GetOptionalInt(desc["x"]);
            result.Y = GetOptionalInt(desc["y"]);
            result.Width = desc["width"].as_int();
            result.Height = desc["height"].as_int();
            result.MinWidth = GetOptionalInt(desc["minWidth"]);
            result.MaxWidth = GetOptionalInt(desc["maxWidth"]);
            result.MinHeight = GetOptionalInt(desc["minHeight"]);
            result.MaxHeight = GetOptionalInt(desc["maxHeight"]);
            result.Title = desc["title"].as_string();
            result.Id = GetOptionalInt(desc["id"]);

            if (desc["widgets"].is_array())
            {
                auto dukWidgets = desc["widgets"].as_array();
                std::transform(dukWidgets.begin(), dukWidgets.end(), std::back_inserter(result.Widgets),
                    [](const DukValue& w) { return CustomWidgetDesc::FromDukValue(w); });
            }

            return std::move(result);
        }

        static std::optional<sint32> GetOptionalInt(DukValue input)
        {
            return input.type() == DukValue::Type::NUMBER ?
                std::make_optional(input.as_int()) :
                std::nullopt;
        }
    };

    class CustomWindowInfo
    {
    private:
        rct_windowclass _class;
        rct_windownumber _number;

    public:
        std::shared_ptr<Plugin> Owner;
        CustomWindowDesc Desc;
        std::vector<rct_widget> Widgets;

        CustomWindowInfo(
            rct_windowclass cls,
            rct_windownumber number,
            std::shared_ptr<Plugin> owner,
            const CustomWindowDesc& desc)
            : _class(cls),
            _number(number),
            Owner(owner),
            Desc(desc)
        {
        }

        CustomWindowInfo(const CustomWindowInfo&) = delete;
    };

    static rct_windownumber _nextWindowNumber;

    static CustomWindowInfo& GetInfo(rct_window * w);
    static rct_windownumber GetNewWindowNumber();
    static void RefreshWidgets(rct_window * w);
    static void InvokeEventHandler(std::shared_ptr<Plugin> owner, const DukValue& dukHandler);

    rct_window * window_custom_open(std::shared_ptr<Plugin> owner, DukValue dukDesc)
    {
        auto desc = CustomWindowDesc::FromDukValue(dukDesc);

        uint16 windowFlags = 0;
        if (desc.IsResizable())
        {
            windowFlags |= WF_RESIZABLE;
        }

        rct_window * window;
        if (desc.X && desc.Y)
        {
            window = window_create(
                desc.X.value(),
                desc.Y.value(),
                desc.Width,
                desc.Height,
                &window_custom_events,
                WC_CUSTOM,
                windowFlags);
        }
        else
        {
            window = window_create_auto_pos(
                desc.Width,
                desc.Height,
                &window_custom_events,
                WC_CUSTOM,
                windowFlags);
        }

        window->number = GetNewWindowNumber();
        window->custom_info = new CustomWindowInfo(window->classification, window->number, owner, desc);
        window->enabled_widgets = (1 << WIDX_CLOSE);
        window->colours[0] = COLOUR_GREY;
        window->colours[1] = COLOUR_GREY;
        window->colours[2] = COLOUR_GREY;
        if (desc.IsResizable())
        {
            window->min_width = desc.MinWidth.value_or(0);
            window->min_height = desc.MinHeight.value_or(0);
            window->max_width = desc.MaxWidth.value_or(std::numeric_limits<uint16>::max());
            window->max_height = desc.MaxHeight.value_or(std::numeric_limits<uint16>::max());
        }
        RefreshWidgets(window);
        window_init_scroll_widgets(window);
        return window;
    }

    static void window_custom_close(rct_window * w)
    {
        delete static_cast<CustomWindowInfo *>(w->custom_info);
        w->custom_info = nullptr;
    }

    static void window_custom_mouseup(rct_window * w, rct_widgetindex widgetIndex)
    {
        switch (widgetIndex) {
        case WIDX_CLOSE:
            window_close(w);
            break;
        default:
            {
                const auto& info = GetInfo(w);
                const auto& widgetDesc = info.Desc.Widgets[widgetIndex - WIDX_CUSTOM_BEGIN];
                InvokeEventHandler(info.Owner, widgetDesc.OnClick);
                break;
            }
        }
    }

    static void window_custom_resize(rct_window * w)
    {
        const auto& desc = GetInfo(w).Desc;
        if (desc.IsResizable())
        {
            if (w->width < w->min_width)
            {
                window_invalidate(w);
                w->width = w->min_width;
            }
            if (w->height < w->min_height)
            {
                window_invalidate(w);
                w->height = w->min_height;
            }
        }
    }

    static void window_custom_invalidate(rct_window * w)
    {
        w->widgets[WIDX_BACKGROUND].right = w->width - 1;
        w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
        w->widgets[WIDX_TITLE].right = w->width - 2;
        w->widgets[WIDX_CLOSE].left = w->width - 13;
        w->widgets[WIDX_CLOSE].right = w->width - 3;
        w->widgets[WIDX_CONTENT_PANEL].right = w->width - 1;
        w->widgets[WIDX_CONTENT_PANEL].bottom = w->height - 1;

        const auto& desc = GetInfo(w).Desc;
        set_format_arg(0, void *, desc.Title.c_str());
    }

    static void window_custom_paint(rct_window * w, rct_drawpixelinfo * dpi)
    {
        window_draw_widgets(w, dpi);
    }

    static CustomWindowInfo& GetInfo(rct_window * w)
    {
        return *(static_cast<CustomWindowInfo *>(w->custom_info));
    }

    static rct_windownumber GetNewWindowNumber()
    {
        auto result = _nextWindowNumber++;
        while (window_find_by_number(WC_CUSTOM, result) != nullptr)
        {
            result++;
        }
        return result;
    }

    static void RefreshWidgets(rct_window * w)
    {
        auto& info = GetInfo(w);
        auto& widgets = info.Widgets;

        widgets.clear();

        // Add default widgets (window shim)
        widgets.insert(widgets.begin(), std::begin(CustomDefaultWidgets), std::end(CustomDefaultWidgets));
        w->enabled_widgets = 1ULL << WIDX_CLOSE;

        // Add custom widgets
        for (const auto& widgetDesc : info.Desc.Widgets)
        {
            rct_widget widget{};
            widget.type = WWT_BUTTON;
            widget.left = widgetDesc.X;
            widget.top = widgetDesc.Y;
            widget.right = widgetDesc.X + widgetDesc.Width;
            widget.bottom = widgetDesc.Y + widgetDesc.Height;
            widget.string = (utf8*)widgetDesc.Text.c_str();
            widget.tooltip = STR_NONE;
            widget.flags = WIDGET_FLAGS::TEXT_IS_STRING;
            widgets.push_back(widget);

            auto widgetIndex = widgets.size() - 1;
            if (widgetIndex < 64)
            {
                w->enabled_widgets |= 1ULL << widgetIndex;
            }
        }

        widgets.push_back({ WIDGETS_END });
        w->widgets = widgets.data();
    }

    static void InvokeEventHandler(std::shared_ptr<Plugin> owner, const DukValue& dukHandler)
    {
        if (dukHandler.is_function())
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto& execInfo = scriptEngine.GetExecInfo();
            {
                ScriptExecutionInfo::PluginScope scope(execInfo, owner);
                dukHandler.push();
                duk_pcall(dukHandler.context(), 0);
                duk_pop(dukHandler.context());
            }
        }
    }
}
