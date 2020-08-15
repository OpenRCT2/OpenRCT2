/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "../interface/Dropdown.h"
#    include "CustomListView.h"
#    include "ScUi.hpp"
#    include "ScWindow.hpp"

#    include <limits>
#    include <openrct2-ui/interface/Widget.h>
#    include <openrct2-ui/windows/Window.h>
#    include <openrct2/drawing/Drawing.h>
#    include <openrct2/localisation/Language.h>
#    include <openrct2/localisation/Localisation.h>
#    include <openrct2/localisation/StringIds.h>
#    include <openrct2/scripting/Plugin.h>
#    include <openrct2/sprites.h>
#    include <openrct2/world/Sprite.h>
#    include <optional>
#    include <string>
#    include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

namespace OpenRCT2::Ui::Windows
{
    enum CUSTOM_WINDOW_WIDX
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CONTENT_PANEL,
        WIDX_TAB_0,
    };

    static rct_widget CustomDefaultWidgets[] = {
        { WWT_FRAME, 0, 0, 0, 0, 0, 0xFFFFFFFF, STR_NONE },                  // panel / background
        { WWT_CAPTION, 0, 1, 0, 1, 14, STR_STRING, STR_WINDOW_TITLE_TIP },   // title bar
        { WWT_CLOSEBOX, 0, 0, 0, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP }, // close x button
        { WWT_RESIZE, 1, 0, 0, 14, 0, 0xFFFFFFFF, STR_NONE },                // content panel
    };

    static void window_custom_close(rct_window* w);
    static void window_custom_mouseup(rct_window* w, rct_widgetindex widgetIndex);
    static void window_custom_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget);
    static void window_custom_resize(rct_window* w);
    static void window_custom_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
    static void window_custom_update(rct_window* w);
    static void window_custom_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height);
    static void window_custom_scrollmousedrag(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
    static void window_custom_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
    static void window_custom_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
    static void window_custom_invalidate(rct_window* w);
    static void window_custom_paint(rct_window* w, rct_drawpixelinfo* dpi);
    static void window_custom_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex);
    static void window_custom_update_viewport(rct_window* w);

    static rct_window_event_list window_custom_events = { window_custom_close,
                                                          window_custom_mouseup,
                                                          window_custom_resize,
                                                          window_custom_mousedown,
                                                          window_custom_dropdown,
                                                          nullptr,
                                                          window_custom_update,
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          window_custom_scrollgetsize,
                                                          window_custom_scrollmousedown,
                                                          window_custom_scrollmousedrag,
                                                          window_custom_scrollmouseover,
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          window_custom_invalidate,
                                                          window_custom_paint,
                                                          window_custom_scrollpaint };

    struct CustomWidgetDesc
    {
        // Properties
        std::string Type;
        int32_t X{};
        int32_t Y{};
        int32_t Width{};
        int32_t Height{};
        std::string Name;
        ImageId Image;
        std::string Text;
        std::string Tooltip;
        std::vector<std::string> Items;
        std::vector<ListViewItem> ListViewItems;
        std::vector<ListViewColumn> ListViewColumns;
        ScrollbarType Scrollbars{};
        int32_t SelectedIndex{};
        bool IsChecked{};
        bool IsDisabled{};
        bool IsPressed{};
        bool HasBorder{};
        bool ShowColumnHeaders{};
        bool IsStriped{};
        bool CanSelect{};

        // Event handlers
        DukValue OnClick;
        DukValue OnChange;
        DukValue OnIncrement;
        DukValue OnDecrement;
        DukValue OnHighlight;

        static CustomWidgetDesc FromDukValue(DukValue desc)
        {
            CustomWidgetDesc result;
            result.Type = desc["type"].as_string();
            result.X = desc["x"].as_int();
            result.Y = desc["y"].as_int();
            result.Width = desc["width"].as_int();
            result.Height = desc["height"].as_int();
            result.IsDisabled = AsOrDefault(desc["isDisabled"], false);
            result.Name = AsOrDefault(desc["name"], "");
            result.Tooltip = AsOrDefault(desc["tooltip"], "");
            if (result.Type == "button")
            {
                auto dukImage = desc["image"];
                if (dukImage.type() == DukValue::Type::NUMBER)
                {
                    auto img = dukImage.as_uint();
                    result.Image = ImageId::FromUInt32(img);
                    result.HasBorder = false;
                }
                else
                {
                    result.Text = ProcessString(desc["text"]);
                    result.HasBorder = true;
                }
                result.IsPressed = AsOrDefault(desc["isPressed"], false);
                result.OnClick = desc["onClick"];
            }
            else if (result.Type == "checkbox")
            {
                result.Text = ProcessString(desc["text"]);
                result.IsChecked = AsOrDefault(desc["isChecked"], false);
                result.OnChange = desc["onChange"];
            }
            else if (result.Type == "dropdown")
            {
                auto dukItems = desc["items"].as_array();
                for (const auto& dukItem : dukItems)
                {
                    result.Items.push_back(ProcessString(dukItem));
                }
                result.SelectedIndex = desc["selectedIndex"].as_int();
                result.OnChange = desc["onChange"];
            }
            else if (result.Type == "groupbox" || result.Type == "label")
            {
                result.Text = ProcessString(desc["text"]);
            }
            else if (result.Type == "listview")
            {
                result.ListViewColumns = FromDuk<std::vector<ListViewColumn>>(desc["columns"]);
                result.ListViewItems = FromDuk<std::vector<ListViewItem>>(desc["items"]);
                result.ShowColumnHeaders = AsOrDefault(desc["showColumnHeaders"], false);
                result.IsStriped = AsOrDefault(desc["isStriped"], false);
                result.OnClick = desc["onClick"];
                result.OnHighlight = desc["onHighlight"];
                result.CanSelect = AsOrDefault(desc["canSelect"], false);
                if (desc["scrollbars"].type() == DukValue::UNDEFINED)
                    result.Scrollbars = ScrollbarType::Vertical;
                else
                    result.Scrollbars = FromDuk<ScrollbarType>(desc["scrollbars"]);
            }
            else if (result.Type == "spinner")
            {
                result.Text = ProcessString(desc["text"]);
                result.OnIncrement = desc["onIncrement"];
                result.OnDecrement = desc["onDecrement"];
            }
            result.HasBorder = AsOrDefault(desc["border"], result.HasBorder);
            return result;
        }
    };

    struct CustomTabDesc
    {
        ImageId imageFrameBase;
        uint32_t imageFrameCount;
        uint32_t imageFrameDuration;
        ScreenCoordsXY offset;
        std::vector<CustomWidgetDesc> Widgets;

        static CustomTabDesc FromDukValue(const DukValue& desc)
        {
            CustomTabDesc result;
            auto dukImage = desc["image"];
            if (dukImage.type() == DukValue::Type::NUMBER)
            {
                result.imageFrameBase = ImageId::FromUInt32(static_cast<uint32_t>(dukImage.as_int()));
            }
            else if (dukImage.type() == DukValue::Type::OBJECT)
            {
                result.imageFrameBase = ImageId::FromUInt32(static_cast<uint32_t>(dukImage["frameBase"].as_int()));
                result.imageFrameCount = AsOrDefault(dukImage["frameCount"], 0);
                result.imageFrameDuration = AsOrDefault(dukImage["frameDuration"], 0);

                auto dukCoord = dukImage["offset"];
                if (dukCoord.type() == DukValue::Type::OBJECT)
                {
                    result.offset = { AsOrDefault(dukCoord["x"], 0), AsOrDefault(dukCoord["y"], 0) };
                }
            }
            if (desc["widgets"].is_array())
            {
                auto dukWidgets = desc["widgets"].as_array();
                std::transform(dukWidgets.begin(), dukWidgets.end(), std::back_inserter(result.Widgets), [](const DukValue& w) {
                    return CustomWidgetDesc::FromDukValue(w);
                });
            }
            return result;
        }
    };

    struct CustomWindowDesc
    {
        std::string Classification;
        std::optional<int32_t> X;
        std::optional<int32_t> Y;
        int32_t Width{};
        int32_t Height{};
        std::optional<int32_t> MinWidth;
        std::optional<int32_t> MinHeight;
        std::optional<int32_t> MaxWidth;
        std::optional<int32_t> MaxHeight;
        std::string Title;
        std::optional<int32_t> Id;
        std::vector<CustomWidgetDesc> Widgets;
        std::vector<colour_t> Colours;
        std::vector<CustomTabDesc> Tabs;

        // Event handlers
        DukValue OnClose;
        DukValue OnUpdate;
        DukValue OnTabChange;

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
            result.Title = language_convert_string(desc["title"].as_string());
            result.Id = GetOptionalInt(desc["id"]);

            if (desc["widgets"].is_array())
            {
                auto dukWidgets = desc["widgets"].as_array();
                std::transform(dukWidgets.begin(), dukWidgets.end(), std::back_inserter(result.Widgets), [](const DukValue& w) {
                    return CustomWidgetDesc::FromDukValue(w);
                });
            }

            if (desc["tabs"].is_array())
            {
                auto dukTabs = desc["tabs"].as_array();
                std::transform(dukTabs.begin(), dukTabs.end(), std::back_inserter(result.Tabs), [](const DukValue& w) {
                    return CustomTabDesc::FromDukValue(w);
                });
            }

            if (desc["colours"].is_array())
            {
                auto dukColours = desc["colours"].as_array();
                std::transform(dukColours.begin(), dukColours.end(), std::back_inserter(result.Colours), [](const DukValue& w) {
                    colour_t c = COLOUR_BLACK;
                    if (w.type() == DukValue::Type::NUMBER)
                    {
                        c = static_cast<colour_t>(std::clamp<int32_t>(w.as_int(), COLOUR_BLACK, COLOUR_COUNT - 1));
                    }
                    return c;
                });
            }

            result.OnClose = desc["onClose"];
            result.OnUpdate = desc["onUpdate"];
            result.OnTabChange = desc["onTabChange"];

            return result;
        }

        static std::optional<int32_t> GetOptionalInt(DukValue input)
        {
            return input.type() == DukValue::Type::NUMBER ? std::make_optional(input.as_int()) : std::nullopt;
        }
    };

    class CustomWindowInfo
    {
    public:
        std::shared_ptr<Plugin> Owner;
        CustomWindowDesc Desc;
        std::vector<rct_widget> Widgets;
        std::vector<size_t> WidgetIndexMap;
        std::vector<CustomListView> ListViews;

        CustomWindowInfo(std::shared_ptr<Plugin> owner, const CustomWindowDesc& desc)
            : Owner(owner)
            , Desc(desc)
        {
        }

        CustomWindowInfo(const CustomWindowInfo&) = delete;

        const CustomWidgetDesc* GetCustomWidgetDesc(rct_window* w, size_t widgetIndex) const
        {
            if (widgetIndex < WidgetIndexMap.size())
            {
                auto widgetDescIndex = WidgetIndexMap[widgetIndex];
                if (widgetDescIndex < Desc.Widgets.size())
                {
                    return &Desc.Widgets[widgetDescIndex];
                }
                else
                {
                    auto page = static_cast<size_t>(w->page);
                    if (Desc.Tabs.size() > page)
                    {
                        auto& widgets = Desc.Tabs[page].Widgets;
                        auto tabWidgetIndex = widgetDescIndex - Desc.Widgets.size();
                        if (tabWidgetIndex < widgets.size())
                        {
                            return &widgets[widgetDescIndex];
                        }
                    }
                }
            }
            return nullptr;
        }

        CustomWidgetDesc* GetCustomWidgetDesc(rct_window* w, size_t widgetIndex)
        {
            return const_cast<CustomWidgetDesc*>(std::as_const(*this).GetCustomWidgetDesc(w, widgetIndex));
        }
    };

    static rct_windownumber _nextWindowNumber;

    static CustomWindowInfo& GetInfo(rct_window* w);
    static rct_windownumber GetNewWindowNumber();
    static void RefreshWidgets(rct_window* w);
    static void InvokeEventHandler(const std::shared_ptr<Plugin>& owner, const DukValue& dukHandler);
    static void InvokeEventHandler(
        const std::shared_ptr<Plugin>& owner, const DukValue& dukHandler, const std::vector<DukValue>& args);

    rct_window* window_custom_open(std::shared_ptr<Plugin> owner, DukValue dukDesc)
    {
        auto desc = CustomWindowDesc::FromDukValue(dukDesc);

        uint16_t windowFlags = WF_RESIZABLE;

        rct_window* window{};
        if (desc.X && desc.Y)
        {
            window = window_create(
                { *desc.X, *desc.Y }, desc.Width, desc.Height, &window_custom_events, WC_CUSTOM, windowFlags);
        }
        else
        {
            window = window_create_auto_pos(desc.Width, desc.Height, &window_custom_events, WC_CUSTOM, windowFlags);
        }

        window->number = GetNewWindowNumber();
        window->custom_info = new CustomWindowInfo(owner, desc);
        window->enabled_widgets = (1 << WIDX_CLOSE);

        // Set window colours
        window->colours[0] = COLOUR_GREY;
        window->colours[1] = COLOUR_GREY;
        window->colours[2] = COLOUR_GREY;
        auto numColours = std::min(std::size(window->colours), std::size(desc.Colours));
        for (size_t i = 0; i < numColours; i++)
        {
            window->colours[i] = desc.Colours[i];
        }

        if (desc.IsResizable())
        {
            window->min_width = desc.MinWidth.value_or(0);
            window->min_height = desc.MinHeight.value_or(0);
            window->max_width = desc.MaxWidth.value_or(std::numeric_limits<uint16_t>::max());
            window->max_height = desc.MaxHeight.value_or(std::numeric_limits<uint16_t>::max());
        }
        RefreshWidgets(window);
        return window;
    }

    static void window_custom_close(rct_window* w)
    {
        auto info = static_cast<CustomWindowInfo*>(w->custom_info);
        if (info != nullptr)
        {
            InvokeEventHandler(info->Owner, info->Desc.OnClose);
            delete info;
            w->custom_info = nullptr;
        }
    }

    static void window_custom_change_tab(rct_window* w, size_t tabIndex)
    {
        const auto& info = GetInfo(w);

        w->page = static_cast<int16_t>(tabIndex);
        w->frame_no = 0;
        RefreshWidgets(w);

        w->Invalidate();
        window_event_resize_call(w);
        window_event_invalidate_call(w);
        window_init_scroll_widgets(w);
        w->Invalidate();

        InvokeEventHandler(info.Owner, info.Desc.OnTabChange);
    }

    static void window_custom_mouseup(rct_window* w, rct_widgetindex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                window_close(w);
                break;
            default:
            {
                const auto& info = GetInfo(w);
                if (widgetIndex >= WIDX_TAB_0 && widgetIndex < static_cast<rct_widgetindex>(WIDX_TAB_0 + info.Desc.Tabs.size()))
                {
                    window_custom_change_tab(w, widgetIndex - WIDX_TAB_0);
                    break;
                }

                const auto widgetDesc = info.GetCustomWidgetDesc(w, widgetIndex);
                if (widgetDesc != nullptr)
                {
                    if (widgetDesc->Type == "button")
                    {
                        InvokeEventHandler(info.Owner, widgetDesc->OnClick);
                    }
                    else if (widgetDesc->Type == "checkbox")
                    {
                        auto& widget = w->widgets[widgetIndex];
                        widget.flags ^= WIDGET_FLAGS::IS_PRESSED;
                        bool isChecked = widget.flags & WIDGET_FLAGS::IS_PRESSED;

                        widget_set_checkbox_value(w, widgetIndex, isChecked);

                        std::vector<DukValue> args;
                        auto ctx = widgetDesc->OnChange.context();
                        duk_push_boolean(ctx, isChecked);
                        args.push_back(DukValue::take_from_stack(ctx));
                        InvokeEventHandler(info.Owner, widgetDesc->OnChange, args);
                    }
                }
                break;
            }
        }
    }

    static void window_custom_resize(rct_window* w)
    {
        if (w->width < w->min_width)
        {
            w->Invalidate();
            w->width = w->min_width;
        }
        if (w->height < w->min_height)
        {
            w->Invalidate();
            w->height = w->min_height;
        }
        window_custom_update_viewport(w);
    }

    static void window_custom_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
    {
        const auto& info = GetInfo(w);
        const auto widgetDesc = info.GetCustomWidgetDesc(w, widgetIndex);
        if (widgetDesc != nullptr)
        {
            if (widgetDesc->Type == "dropdown")
            {
                widget--;
                auto selectedIndex = widgetDesc->SelectedIndex;
                const auto& items = widgetDesc->Items;
                const auto numItems = std::min<size_t>(items.size(), DROPDOWN_ITEMS_MAX_SIZE);
                for (size_t i = 0; i < numItems; i++)
                {
                    gDropdownItemsFormat[i] = selectedIndex == static_cast<int32_t>(i) ? STR_OPTIONS_DROPDOWN_ITEM_SELECTED
                                                                                       : STR_OPTIONS_DROPDOWN_ITEM;
                    auto sz = items[i].c_str();
                    std::memcpy(&gDropdownItemsArgs[i], &sz, sizeof(const char*));
                }
                window_dropdown_show_text_custom_width(
                    { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1,
                    w->colours[widget->colour], 0, DROPDOWN_FLAG_STAY_OPEN, numItems, widget->width() - 3);
            }
            else if (widgetDesc->Type == "spinner")
            {
                if (widget->text == STR_NUMERIC_DOWN)
                {
                    InvokeEventHandler(info.Owner, widgetDesc->OnDecrement);
                }
                else if (widget->text == STR_NUMERIC_UP)
                {
                    InvokeEventHandler(info.Owner, widgetDesc->OnIncrement);
                }
            }
        }
    }

    static void window_custom_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
    {
        if (dropdownIndex == -1)
            return;

        auto& info = GetInfo(w);
        auto widgetDesc = info.GetCustomWidgetDesc(w, widgetIndex);
        if (widgetDesc != nullptr)
        {
            if (widgetDesc->Type == "dropdown")
            {
                UpdateWidgetSelectedIndex(w, widgetIndex - 1, dropdownIndex);
            }
        }
    }

    static void window_custom_update(rct_window* w)
    {
        const auto& info = GetInfo(w);
        if (info.Desc.Tabs.size() > static_cast<size_t>(w->page))
        {
            const auto& tab = info.Desc.Tabs[w->page];
            if (tab.imageFrameCount != 0)
            {
                w->frame_no++;
                if (w->frame_no >= tab.imageFrameCount * tab.imageFrameDuration)
                {
                    w->frame_no = 0;
                }
                widget_invalidate(w, WIDX_TAB_0 + w->page);
            }
        }

        InvokeEventHandler(info.Owner, info.Desc.OnUpdate);

        // Since the plugin may alter widget positions and sizes during an update event,
        // we need to force an update for all list view scrollbars
        rct_widgetindex widgetIndex = 0;
        for (auto widget = w->widgets; widget->type != WWT_EMPTY; widget++)
        {
            if (widget->type == WWT_SCROLL)
            {
                widget_scroll_update_thumbs(w, widgetIndex);
            }
            widgetIndex++;
        }
    }

    static void window_custom_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
    {
        auto& info = GetInfo(w);
        if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
        {
            auto size = info.ListViews[scrollIndex].GetSize();
            *width = size.width;
            *height = size.height;
        }
    }

    static void window_custom_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
    {
        auto& info = GetInfo(w);
        if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
        {
            info.ListViews[scrollIndex].MouseDown(screenCoords);
        }
    }

    static void window_custom_scrollmousedrag(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
    {
        auto& info = GetInfo(w);
        if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
        {
            info.ListViews[scrollIndex].MouseOver(screenCoords, true);
        }
    }

    static void window_custom_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
    {
        auto& info = GetInfo(w);
        if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
        {
            info.ListViews[scrollIndex].MouseOver(screenCoords, false);
        }
    }

    static void window_custom_set_pressed_tab(rct_window* w)
    {
        const auto& info = GetInfo(w);
        auto numTabs = info.Desc.Tabs.size();
        if (numTabs != 0)
        {
            for (size_t i = 0; i < numTabs; i++)
            {
                w->pressed_widgets &= ~(1 << (WIDX_TAB_0 + i));
            }
            w->pressed_widgets |= 1LL << (WIDX_TAB_0 + w->page);
        }
    }

    static void window_custom_invalidate(rct_window* w)
    {
        w->widgets[WIDX_BACKGROUND].right = w->width - 1;
        w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
        w->widgets[WIDX_TITLE].right = w->width - 2;
        w->widgets[WIDX_CLOSE].left = w->width - 13;
        w->widgets[WIDX_CLOSE].right = w->width - 3;
        w->widgets[WIDX_CONTENT_PANEL].right = w->width - 1;
        w->widgets[WIDX_CONTENT_PANEL].bottom = w->height - 1;

        window_custom_set_pressed_tab(w);

        const auto& desc = GetInfo(w).Desc;
        auto ft = Formatter::Common();
        ft.Add<const char*>(desc.Title.c_str());

        auto& info = GetInfo(w);
        size_t scrollIndex = 0;
        for (auto widget = w->widgets; widget->type != WWT_LAST; widget++)
        {
            if (widget->type == WWT_SCROLL)
            {
                auto& listView = info.ListViews[scrollIndex];
                auto width = widget->width() + 1 - 2;
                auto height = widget->height() + 1 - 2;
                if (listView.GetScrollbars() == ScrollbarType::Horizontal || listView.GetScrollbars() == ScrollbarType::Both)
                {
                    height -= SCROLLBAR_WIDTH + 1;
                }
                if (listView.GetScrollbars() == ScrollbarType::Vertical || listView.GetScrollbars() == ScrollbarType::Both)
                {
                    width -= SCROLLBAR_WIDTH + 1;
                }
                listView.Resize({ width, height });
                scrollIndex++;
            }
        }
    }

    static void window_custom_draw_tab_images(rct_window* w, rct_drawpixelinfo* dpi)
    {
        const auto& customInfo = GetInfo(w);
        const auto& tabs = customInfo.Desc.Tabs;
        size_t tabIndex = 0;
        for (auto tab : tabs)
        {
            auto widgetIndex = static_cast<rct_widgetindex>(WIDX_TAB_0 + tabIndex);
            auto widget = &w->widgets[widgetIndex];
            if (widget_is_enabled(w, widgetIndex))
            {
                auto leftTop = w->windowPos + tab.offset + ScreenCoordsXY{ widget->left, widget->top };
                auto image = tab.imageFrameBase;
                if (static_cast<size_t>(w->page) == tabIndex && tab.imageFrameDuration != 0 && tab.imageFrameCount != 0)
                {
                    auto frame = w->frame_no / tab.imageFrameDuration;
                    auto imageOffset = frame % tab.imageFrameCount;
                    image = image.WithIndex(image.GetIndex() + imageOffset);
                }
                gfx_draw_sprite(dpi, image.ToUInt32(), leftTop, image.GetTertiary());
            }
            tabIndex++;
        }
    }

    static void window_custom_paint(rct_window* w, rct_drawpixelinfo* dpi)
    {
        window_draw_widgets(w, dpi);
        window_custom_draw_tab_images(w, dpi);
        if (w->viewport != nullptr)
        {
            window_draw_viewport(dpi, w);
        }
    }

    static void window_custom_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
    {
        const auto& info = GetInfo(w);
        if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
        {
            info.ListViews[scrollIndex].Paint(w, dpi, &w->scrolls[scrollIndex]);
        }
    }

    static std::optional<rct_widgetindex> GetViewportWidgetIndex(rct_window* w)
    {
        rct_widgetindex widgetIndex = 0;
        for (auto widget = w->widgets; widget->type != WWT_LAST; widget++)
        {
            if (widget->type == WWT_VIEWPORT)
            {
                return widgetIndex;
            }
            widgetIndex++;
        }
        return std::nullopt;
    }

    static void window_custom_update_viewport(rct_window* w)
    {
        auto viewportWidgetIndex = GetViewportWidgetIndex(w);
        if (viewportWidgetIndex)
        {
            auto viewportWidget = &w->widgets[*viewportWidgetIndex];
            auto& customInfo = GetInfo(w);
            auto widgetInfo = customInfo.GetCustomWidgetDesc(w, *viewportWidgetIndex);
            if (widgetInfo != nullptr)
            {
                auto left = w->windowPos.x + viewportWidget->left + 1;
                auto top = w->windowPos.y + viewportWidget->top + 1;
                auto width = viewportWidget->width() - 1;
                auto height = viewportWidget->height() - 1;
                auto viewport = w->viewport;
                if (viewport == nullptr)
                {
                    auto mapX = 0;
                    auto mapY = 0;
                    auto mapZ = 0;
                    viewport_create(
                        w, { left, top }, width, height, 0, { mapX, mapY, mapZ }, VIEWPORT_FOCUS_TYPE_COORDINATE,
                        SPRITE_INDEX_NULL);
                    w->flags |= WF_NO_SCROLLING;
                    w->Invalidate();
                }
                else
                {
                    if (viewport->pos.x != left || viewport->pos.y != top || viewport->width != width
                        || viewport->height != height)
                    {
                        viewport->pos.x = left;
                        viewport->pos.y = top;
                        viewport->width = width;
                        viewport->height = height;
                        viewport->view_width = width * viewport->zoom;
                        viewport->view_height = height * viewport->zoom;
                        w->Invalidate();
                    }
                }
            }
        }
    }

    static CustomWindowInfo& GetInfo(rct_window* w)
    {
        return *(static_cast<CustomWindowInfo*>(w->custom_info));
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

    static void CreateWidget(std::vector<rct_widget>& widgetList, const CustomWidgetDesc& desc)
    {
        rct_widget widget{};
        widget.colour = 1;
        widget.left = desc.X;
        widget.top = desc.Y;
        widget.right = desc.X + desc.Width - 1;
        widget.bottom = desc.Y + desc.Height - 1;
        widget.content = std::numeric_limits<uint32_t>::max();
        widget.tooltip = STR_NONE;
        if (!desc.Tooltip.empty())
        {
            widget.sztooltip = const_cast<utf8*>(desc.Tooltip.c_str());
            widget.flags |= WIDGET_FLAGS::TOOLTIP_IS_STRING;
        }
        widget.flags |= WIDGET_FLAGS::IS_ENABLED;
        if (desc.IsDisabled)
            widget.flags |= WIDGET_FLAGS::IS_DISABLED;

        if (desc.Type == "button")
        {
            if (desc.Image.HasValue())
            {
                widget.type = desc.HasBorder ? WWT_IMGBTN : WWT_FLATBTN;
                widget.image = desc.Image.ToUInt32();
            }
            else
            {
                widget.type = WWT_BUTTON;
                widget.string = const_cast<utf8*>(desc.Text.c_str());
                widget.flags |= WIDGET_FLAGS::TEXT_IS_STRING;
            }
            if (desc.IsPressed)
            {
                widget.flags |= WIDGET_FLAGS::IS_PRESSED;
            }
            widgetList.push_back(widget);
        }
        else if (desc.Type == "checkbox")
        {
            widget.type = WWT_CHECKBOX;
            widget.string = const_cast<utf8*>(desc.Text.c_str());
            widget.flags |= WIDGET_FLAGS::TEXT_IS_STRING;
            if (desc.IsChecked)
            {
                widget.flags |= WIDGET_FLAGS::IS_PRESSED;
            }
            widgetList.push_back(widget);
        }
        else if (desc.Type == "dropdown")
        {
            widget.type = WWT_DROPDOWN;
            if (desc.SelectedIndex >= 0 && static_cast<size_t>(desc.SelectedIndex) < desc.Items.size())
            {
                widget.string = const_cast<utf8*>(desc.Items[desc.SelectedIndex].c_str());
            }
            else
            {
                widget.string = const_cast<utf8*>("");
            }
            widget.flags |= WIDGET_FLAGS::TEXT_IS_STRING;
            widgetList.push_back(widget);

            // Add the dropdown button
            widget = {};
            widget.type = WWT_BUTTON;
            widget.colour = 1;
            widget.left = desc.X + desc.Width - 12;
            widget.right = desc.X + desc.Width - 2;
            widget.top = desc.Y + 1;
            widget.bottom = desc.Y + desc.Height - 2;
            widget.text = STR_DROPDOWN_GLYPH;
            widget.tooltip = STR_NONE;
            widget.flags |= WIDGET_FLAGS::IS_ENABLED;
            widgetList.push_back(widget);
        }
        else if (desc.Type == "groupbox")
        {
            widget.type = WWT_GROUPBOX;
            widget.string = const_cast<utf8*>(desc.Text.c_str());
            widget.flags |= WIDGET_FLAGS::TEXT_IS_STRING;
            widgetList.push_back(widget);
        }
        else if (desc.Type == "label")
        {
            widget.type = WWT_LABEL;
            widget.string = const_cast<utf8*>(desc.Text.c_str());
            widget.flags |= WIDGET_FLAGS::TEXT_IS_STRING;
            widgetList.push_back(widget);
        }
        else if (desc.Type == "listview")
        {
            widget.type = WWT_SCROLL;
            widget.content = 0;
            if (desc.Scrollbars == ScrollbarType::Horizontal)
                widget.content = SCROLL_HORIZONTAL;
            else if (desc.Scrollbars == ScrollbarType::Vertical)
                widget.content = SCROLL_VERTICAL;
            else if (desc.Scrollbars == ScrollbarType::Both)
                widget.content = SCROLL_BOTH;
            widgetList.push_back(widget);
        }
        else if (desc.Type == "spinner")
        {
            widget.type = WWT_SPINNER;
            widget.string = const_cast<utf8*>(desc.Text.c_str());
            widget.flags |= WIDGET_FLAGS::TEXT_IS_STRING;
            widgetList.push_back(widget);

            // Add the decrement button
            widget = {};
            widget.type = WWT_BUTTON;
            widget.colour = 1;
            widget.left = desc.X + desc.Width - 26;
            widget.right = widget.left + 12;
            widget.top = desc.Y + 1;
            widget.bottom = desc.Y + desc.Height - 2;
            widget.text = STR_NUMERIC_DOWN;
            widget.tooltip = STR_NONE;
            widget.flags |= WIDGET_FLAGS::IS_ENABLED;
            widgetList.push_back(widget);

            // Add the increment button
            widget.left = desc.X + desc.Width - 13;
            widget.right = widget.left + 11;
            widget.text = STR_NUMERIC_UP;
            widgetList.push_back(widget);
        }
        else if (desc.Type == "viewport")
        {
            widget.type = WWT_VIEWPORT;
            widget.text = STR_NONE;
            widgetList.push_back(widget);
        }
    }

    static void RefreshWidgets(rct_window* w)
    {
        w->enabled_widgets = 0;
        w->pressed_widgets = 0;
        w->disabled_widgets = 0;

        auto& info = GetInfo(w);
        auto& widgets = info.Widgets;

        widgets.clear();
        info.WidgetIndexMap.clear();
        info.ListViews.clear();

        // Add default widgets (window shim)
        widgets.insert(widgets.begin(), std::begin(CustomDefaultWidgets), std::end(CustomDefaultWidgets));
        for (size_t i = 0; i < widgets.size(); i++)
        {
            info.WidgetIndexMap.push_back(std::numeric_limits<size_t>::max());
        }
        w->enabled_widgets = 1ULL << WIDX_CLOSE;

        // Add window tabs
        if (info.Desc.Tabs.size() != 0)
        {
            widgets[WIDX_CONTENT_PANEL].top = 43;
        }
        for (size_t tabDescIndex = 0; tabDescIndex < info.Desc.Tabs.size(); tabDescIndex++)
        {
            rct_widget widget{};
            widget.type = WWT_TAB;
            widget.colour = 1;
            widget.left = static_cast<int16_t>(3 + (tabDescIndex * 31));
            widget.right = widget.left + 30;
            widget.top = 17;
            widget.bottom = 43;
            widget.image = IMAGE_TYPE_REMAP | SPR_TAB;
            widget.tooltip = STR_NONE;
            widgets.push_back(widget);
            info.WidgetIndexMap.push_back(std::numeric_limits<size_t>::max());
            w->enabled_widgets |= 1ULL << (widgets.size() - 1);
        }

        // Add custom widgets
        auto firstCustomWidgetIndex = widgets.size();
        auto totalWidgets = info.Desc.Widgets.size();
        auto tabWidgetsOffset = totalWidgets;
        if (info.Desc.Tabs.size() != 0)
        {
            totalWidgets += info.Desc.Tabs[w->page].Widgets.size();
        }
        for (size_t widgetDescIndex = 0; widgetDescIndex < totalWidgets; widgetDescIndex++)
        {
            const auto& widgetDesc = widgetDescIndex < info.Desc.Widgets.size()
                ? info.Desc.Widgets[widgetDescIndex]
                : info.Desc.Tabs[w->page].Widgets[widgetDescIndex - tabWidgetsOffset];
            auto preWidgetSize = widgets.size();
            CreateWidget(widgets, widgetDesc);
            auto numWidetsAdded = widgets.size() - preWidgetSize;
            for (size_t i = 0; i < numWidetsAdded; i++)
            {
                info.WidgetIndexMap.push_back(widgetDescIndex);
            }

            if (widgetDesc.Type == "listview")
            {
                CustomListView listView(w, info.ListViews.size());
                listView.SetScrollbars(widgetDesc.Scrollbars, true);
                listView.SetColumns(widgetDesc.ListViewColumns, true);
                listView.SetItems(widgetDesc.ListViewItems, true);
                listView.ShowColumnHeaders = widgetDesc.ShowColumnHeaders;
                listView.IsStriped = widgetDesc.IsStriped;
                listView.OnClick = widgetDesc.OnClick;
                listView.OnHighlight = widgetDesc.OnHighlight;
                listView.CanSelect = widgetDesc.CanSelect;
                info.ListViews.push_back(std::move(listView));
            }
        }

        for (size_t i = firstCustomWidgetIndex; i < widgets.size(); i++)
        {
            auto mask = 1ULL << i;
            auto flags = widgets[i].flags;
            if (flags & WIDGET_FLAGS::IS_ENABLED)
            {
                w->enabled_widgets |= mask;
            }
            if (flags & WIDGET_FLAGS::IS_PRESSED)
            {
                w->pressed_widgets |= mask;
            }
            if (flags & WIDGET_FLAGS::IS_DISABLED)
            {
                w->disabled_widgets |= mask;
            }
        }

        widgets.push_back({ WIDGETS_END });
        w->widgets = widgets.data();

        window_init_scroll_widgets(w);
        window_custom_update_viewport(w);
    }

    static void InvokeEventHandler(const std::shared_ptr<Plugin>& owner, const DukValue& dukHandler)
    {
        std::vector<DukValue> args;
        InvokeEventHandler(owner, dukHandler, args);
    }

    static void InvokeEventHandler(
        const std::shared_ptr<Plugin>& owner, const DukValue& dukHandler, const std::vector<DukValue>& args)
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        scriptEngine.ExecutePluginCall(owner, dukHandler, args, false);
    }

    std::string GetWindowTitle(rct_window* w)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            return customInfo.Desc.Title;
        }
        return {};
    }

    void UpdateWindowTitle(rct_window* w, const std::string_view& value)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            customInfo.Desc.Title = value;
        }
    }

    void UpdateWidgetText(rct_window* w, rct_widgetindex widgetIndex, const std::string_view& value)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                customWidgetInfo->Text = language_convert_string(value);
                w->widgets[widgetIndex].string = customWidgetInfo->Text.data();
                widget_invalidate(w, widgetIndex);
            }
        }
    }

    void UpdateWidgetItems(rct_window* w, rct_widgetindex widgetIndex, const std::vector<std::string>& items)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                customWidgetInfo->Items = items;
                UpdateWidgetSelectedIndex(w, widgetIndex, customWidgetInfo->SelectedIndex);
            }
        }
    }

    void UpdateWidgetSelectedIndex(rct_window* w, rct_widgetindex widgetIndex, int32_t selectedIndex)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                auto& widget = w->widgets[widgetIndex];

                auto lastSelectedIndex = customWidgetInfo->SelectedIndex;
                if (customWidgetInfo->Items.size() == 0)
                {
                    selectedIndex = -1;
                }
                else
                {
                    if (selectedIndex < 0 || static_cast<size_t>(selectedIndex) >= customWidgetInfo->Items.size())
                    {
                        selectedIndex = 0;
                    }
                }

                if (selectedIndex == -1)
                {
                    widget.string = const_cast<utf8*>("");
                }
                else
                {
                    widget.string = const_cast<utf8*>(customWidgetInfo->Items[selectedIndex].c_str());
                }
                customWidgetInfo->SelectedIndex = selectedIndex;

                widget_invalidate(w, widgetIndex);

                if (lastSelectedIndex != selectedIndex)
                {
                    std::vector<DukValue> args;
                    auto ctx = customWidgetInfo->OnChange.context();
                    duk_push_int(ctx, selectedIndex);
                    args.push_back(DukValue::take_from_stack(ctx));
                    InvokeEventHandler(customInfo.Owner, customWidgetInfo->OnChange, args);
                }
            }
        }
    }

    std::vector<std::string> GetWidgetItems(rct_window* w, rct_widgetindex widgetIndex)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                return customWidgetInfo->Items;
            }
        }
        return {};
    }

    int32_t GetWidgetSelectedIndex(rct_window* w, rct_widgetindex widgetIndex)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                return customWidgetInfo->SelectedIndex;
            }
        }
        return -1;
    }

    rct_window* FindCustomWindowByClassification(const std::string_view& classification)
    {
        for (auto w : g_window_list)
        {
            if (w->classification == WC_CUSTOM)
            {
                auto& customInfo = GetInfo(w.get());
                if (customInfo.Desc.Classification == classification)
                {
                    return w.get();
                }
            }
        }
        return nullptr;
    }

    std::optional<rct_widgetindex> FindWidgetIndexByName(rct_window* w, const std::string_view& name)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            for (size_t i = 0; i < customInfo.Widgets.size(); i++)
            {
                auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, i);
                if (customWidgetInfo != nullptr)
                {
                    if (customWidgetInfo->Name == name)
                    {
                        return static_cast<rct_widgetindex>(i);
                    }
                }
            }
        }
        return std::nullopt;
    }

    std::string GetWidgetName(rct_window* w, rct_widgetindex widgetIndex)
    {
        if (w->custom_info != nullptr)
        {
            const auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                return customWidgetInfo->Name;
            }
        }
        return {};
    }

    void SetWidgetName(rct_window* w, rct_widgetindex widgetIndex, const std::string_view& name)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                customWidgetInfo->Name = std::string(name);
            }
        }
    }

    CustomListView* GetCustomListView(rct_window* w, rct_widgetindex widgetIndex)
    {
        if (w->custom_info != nullptr)
        {
            auto& info = GetInfo(w);
            auto scrollIndex = window_get_scroll_data_index(w, widgetIndex);
            if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
            {
                return &info.ListViews[scrollIndex];
            }
        }
        return nullptr;
    }

} // namespace OpenRCT2::Ui::Windows

#endif
