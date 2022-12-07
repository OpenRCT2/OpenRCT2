/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "../UiContext.h"
#    include "../interface/Dropdown.h"
#    include "../interface/Widget.h"
#    include "../scripting/ScGraphicsContext.hpp"
#    include "../scripting/ScWidget.hpp"
#    include "../windows/Window.h"
#    include "CustomListView.h"
#    include "ScUi.hpp"
#    include "ScWindow.hpp"

#    include <limits>
#    include <openrct2/drawing/Drawing.h>
#    include <openrct2/interface/Window.h>
#    include <openrct2/localisation/Formatter.h>
#    include <openrct2/localisation/Language.h>
#    include <openrct2/localisation/Localisation.h>
#    include <openrct2/localisation/StringIds.h>
#    include <openrct2/scripting/Plugin.h>
#    include <openrct2/sprites.h>
#    include <optional>
#    include <string>
#    include <utility>
#    include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

namespace OpenRCT2::Ui::Windows
{
    enum CustomWindowWidx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CONTENT_PANEL,
        WIDX_TAB_0,
    };

    static rct_widget CustomDefaultWidgets[] = {
        { WindowWidgetType::Frame, 0, 0, 0, 0, 0, 0xFFFFFFFF, STR_NONE },                  // panel / background
        { WindowWidgetType::Caption, 0, 1, 0, 1, 14, STR_STRING, STR_WINDOW_TITLE_TIP },   // title bar
        { WindowWidgetType::CloseBox, 0, 0, 0, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP }, // close x button
        { WindowWidgetType::Resize, 1, 0, 0, 14, 0, 0xFFFFFFFF, STR_NONE },                // content panel
    };

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
        TextAlignment TextAlign{};
        colour_t Colour{};
        std::string Tooltip;
        std::vector<std::string> Items;
        std::vector<ListViewItem> ListViewItems;
        std::vector<ListViewColumn> ListViewColumns;
        ScrollbarType Scrollbars{};
        int32_t SelectedIndex{};
        int32_t MaxLength{};
        std::optional<RowColumn> SelectedCell;
        bool IsChecked{};
        bool IsDisabled{};
        bool IsVisible{};
        bool IsPressed{};
        bool HasBorder{};
        bool ShowColumnHeaders{};
        bool IsStriped{};
        bool CanSelect{};

        // Event handlers
        DukValue OnClick;
        DukValue OnChange;
        DukValue OnDraw;
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
            result.IsVisible = AsOrDefault(desc["isVisible"], true);
            result.Name = AsOrDefault(desc["name"], "");
            result.Tooltip = AsOrDefault(desc["tooltip"], "");
            if (result.Type == "button")
            {
                auto dukImage = desc["image"];
                if (dukImage.type() != DukValue::Type::UNDEFINED)
                {
                    result.Image = ImageId::FromUInt32(ImageFromDuk(dukImage));
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
            else if (result.Type == "colourpicker")
            {
                auto colour = AsOrDefault(desc["colour"], 0);
                if (colour < COLOUR_COUNT)
                {
                    result.Colour = colour;
                }
                result.OnChange = desc["onChange"];
            }
            else if (result.Type == "custom")
            {
                result.OnDraw = desc["onDraw"];
            }
            else if (result.Type == "dropdown")
            {
                if (desc["items"].is_array())
                {
                    auto dukItems = desc["items"].as_array();
                    for (const auto& dukItem : dukItems)
                    {
                        result.Items.push_back(ProcessString(dukItem));
                    }
                }
                result.SelectedIndex = AsOrDefault(desc["selectedIndex"], 0);
                result.OnChange = desc["onChange"];
            }
            else if (result.Type == "groupbox")
            {
                result.Text = ProcessString(desc["text"]);
            }
            else if (result.Type == "label")
            {
                result.Text = ProcessString(desc["text"]);
                if (ProcessString(desc["textAlign"]) == "centred")
                {
                    result.TextAlign = TextAlignment::CENTRE;
                }
            }
            else if (result.Type == "listview")
            {
                result.ListViewColumns = FromDuk<std::vector<ListViewColumn>>(desc["columns"]);
                result.ListViewItems = FromDuk<std::vector<ListViewItem>>(desc["items"]);
                result.SelectedCell = FromDuk<std::optional<RowColumn>>(desc["selectedCell"]);
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
                result.OnClick = desc["onClick"];
            }
            else if (result.Type == "textbox")
            {
                result.Text = ProcessString(desc["text"]);
                result.MaxLength = AsOrDefault(desc["maxLength"], 32);
                result.OnChange = desc["onChange"];
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
            if (dukImage.type() == DukValue::Type::OBJECT)
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
            else if (dukImage.type() != DukValue::Type::UNDEFINED)
            {
                result.imageFrameBase = ImageId::FromUInt32(ImageFromDuk(dukImage));
                result.imageFrameCount = 0;
                result.imageFrameDuration = 0;
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
        std::optional<int32_t> TabIndex;

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
            result.Title = desc["title"].as_string();
            result.Id = GetOptionalInt(desc["id"]);
            result.TabIndex = GetOptionalInt(desc["tabIndex"]);

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
                        c = std::clamp<int32_t>(BASE_COLOUR(w.as_int()), COLOUR_BLACK, COLOUR_COUNT - 1);
                        if (w.as_int() & COLOUR_FLAG_TRANSLUCENT)
                        {
                            c = TRANSLUCENT(c);
                        }
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

                auto page = static_cast<size_t>(w->page);
                if (Desc.Tabs.size() > page)
                {
                    auto& widgets = Desc.Tabs[page].Widgets;
                    auto tabWidgetIndex = widgetDescIndex - Desc.Widgets.size();
                    if (tabWidgetIndex < widgets.size())
                    {
                        return &widgets[tabWidgetIndex];
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

    static CustomWindowInfo& GetInfo(rct_window* w);
    static void InvokeEventHandler(const std::shared_ptr<Plugin>& owner, const DukValue& dukHandler);
    static void InvokeEventHandler(
        const std::shared_ptr<Plugin>& owner, const DukValue& dukHandler, const std::vector<DukValue>& args);

    class CustomWindow final : public Window
    {
    private:
        static rct_windownumber _nextWindowNumber;

    public:
        void Initialise(std::shared_ptr<Plugin> owner, const CustomWindowDesc& desc)
        {
            number = GetNewWindowNumber();
            custom_info = new CustomWindowInfo(owner, desc);

            // Set window tab
            page = desc.TabIndex.value_or(0);

            // Set window colours
            colours[0] = COLOUR_GREY;
            colours[1] = COLOUR_GREY;
            colours[2] = COLOUR_GREY;
            auto numColours = std::min(std::size(colours), std::size(desc.Colours));
            for (size_t i = 0; i < numColours; i++)
            {
                colours[i] = desc.Colours[i];
            }

            if (desc.IsResizable())
            {
                min_width = desc.MinWidth.value_or(0);
                min_height = desc.MinHeight.value_or(0);
                max_width = desc.MaxWidth.value_or(std::numeric_limits<uint16_t>::max());
                max_height = desc.MaxHeight.value_or(std::numeric_limits<uint16_t>::max());
            }
            RefreshWidgets();
        }

        void OnClose() override
        {
            auto info = static_cast<CustomWindowInfo*>(custom_info);
            if (info != nullptr)
            {
                InvokeEventHandler(info->Owner, info->Desc.OnClose);
                delete info;
                custom_info = nullptr;
            }
        }

        void OnResize() override
        {
            if (width < min_width)
            {
                Invalidate();
                width = min_width;
            }
            if (height < min_height)
            {
                Invalidate();
                height = min_height;
            }
            UpdateViewport();
        }

        void OnUpdate() override
        {
            const auto& info = GetInfo(this);
            if (info.Desc.Tabs.size() > static_cast<size_t>(page))
            {
                const auto& tab = info.Desc.Tabs[page];
                if (tab.imageFrameCount != 0)
                {
                    frame_no++;
                    if (frame_no >= tab.imageFrameCount * tab.imageFrameDuration)
                    {
                        frame_no = 0;
                    }
                    widget_invalidate(*this, WIDX_TAB_0 + this->page);
                }
            }

            InvokeEventHandler(info.Owner, info.Desc.OnUpdate);

            // Since the plugin may alter widget positions and sizes during an update event,
            // we need to force an update for all list view scrollbars
            WidgetIndex widgetIndex = 0;
            for (auto widget = widgets; widget->type != WindowWidgetType::Empty; widget++)
            {
                if (widget->type == WindowWidgetType::Scroll)
                {
                    WidgetScrollUpdateThumbs(*this, widgetIndex);
                }
                widgetIndex++;
            }
        }

        void OnPrepareDraw() override
        {
            widgets[WIDX_BACKGROUND].right = width - 1;
            widgets[WIDX_BACKGROUND].bottom = height - 1;
            widgets[WIDX_TITLE].right = width - 2;
            widgets[WIDX_CLOSE].left = width - 13;
            widgets[WIDX_CLOSE].right = width - 3;
            widgets[WIDX_CONTENT_PANEL].right = width - 1;
            widgets[WIDX_CONTENT_PANEL].bottom = height - 1;
            widgets[WIDX_CLOSE].text = (colours[0] & COLOUR_FLAG_TRANSLUCENT) ? STR_CLOSE_X_WHITE : STR_CLOSE_X;

            // Having the content panel visible for transparent windows makes the borders darker than they should be
            // For now just hide it if there are no tabs and the window is not resizable
            auto& info = GetInfo(this);
            auto canResize = (flags & WF_RESIZABLE) != 0 && (min_width != max_width || min_height != max_height);
            auto numTabs = info.Desc.Tabs.size();
            if (canResize || numTabs != 0)
            {
                widgets[WIDX_CONTENT_PANEL].flags &= ~WIDGET_FLAGS::IS_HIDDEN;
            }
            else
            {
                widgets[WIDX_CONTENT_PANEL].flags |= WIDGET_FLAGS::IS_HIDDEN;
            }

            SetPressedTab();

            const auto& desc = info.Desc;
            auto ft = Formatter::Common();
            ft.Add<const char*>(desc.Title.c_str());

            size_t scrollIndex = 0;
            for (auto widget = widgets; widget->type != WindowWidgetType::Last; widget++)
            {
                if (widget->type == WindowWidgetType::Scroll)
                {
                    auto& listView = info.ListViews[scrollIndex];
                    auto wwidth = widget->width() + 1 - 2;
                    auto wheight = widget->height() + 1 - 2;
                    if (listView.GetScrollbars() == ScrollbarType::Horizontal
                        || listView.GetScrollbars() == ScrollbarType::Both)
                    {
                        wheight -= SCROLLBAR_WIDTH + 1;
                    }
                    if (listView.GetScrollbars() == ScrollbarType::Vertical || listView.GetScrollbars() == ScrollbarType::Both)
                    {
                        wwidth -= SCROLLBAR_WIDTH + 1;
                    }
                    listView.Resize({ wwidth, wheight });
                    scrollIndex++;
                }
            }
        }

        void OnDraw(rct_drawpixelinfo& dpi) override
        {
            WindowDrawWidgets(*this, &dpi);
            DrawTabImages(dpi);
            if (viewport != nullptr)
            {
                auto widgetIndex = GetViewportWidgetIndex();
                if (WidgetIsVisible(*this, widgetIndex.value_or(false)))
                {
                    window_draw_viewport(&dpi, *this);
                }
            }
        }

        void OnDrawWidget(WidgetIndex widgetIndex, rct_drawpixelinfo& dpi) override
        {
            const auto& widget = widgets[widgetIndex];
            const auto& info = GetInfo(this);
            const auto widgetDesc = info.GetCustomWidgetDesc(this, widgetIndex);
            if (widgetDesc != nullptr && widgetDesc->Type == "custom")
            {
                auto& onDraw = widgetDesc->OnDraw;
                if (onDraw.is_function())
                {
                    rct_drawpixelinfo widgetDpi;
                    if (clip_drawpixelinfo(
                            &widgetDpi, &dpi, { windowPos.x + widget.left, windowPos.y + widget.top }, widget.width(),
                            widget.height()))
                    {
                        auto ctx = onDraw.context();
                        auto dukWidget = ScWidget::ToDukValue(ctx, this, widgetIndex);
                        auto dukG = GetObjectAsDukValue(ctx, std::make_shared<ScGraphicsContext>(ctx, widgetDpi));
                        auto& scriptEngine = GetContext()->GetScriptEngine();
                        scriptEngine.ExecutePluginCall(info.Owner, widgetDesc->OnDraw, dukWidget, { dukG }, false);
                    }
                }
            }
            else
            {
                Window::OnDrawWidget(widgetIndex, dpi);
            }
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    window_close(*this);
                    break;
                default:
                {
                    const auto& info = GetInfo(this);
                    if (widgetIndex >= WIDX_TAB_0 && widgetIndex < static_cast<WidgetIndex>(WIDX_TAB_0 + info.Desc.Tabs.size()))
                    {
                        ChangeTab(widgetIndex - WIDX_TAB_0);
                        break;
                    }

                    const auto widgetDesc = info.GetCustomWidgetDesc(this, widgetIndex);
                    if (widgetDesc != nullptr)
                    {
                        if (widgetDesc->Type == "button")
                        {
                            InvokeEventHandler(info.Owner, widgetDesc->OnClick);
                        }
                        else if (widgetDesc->Type == "checkbox")
                        {
                            auto& widget = widgets[widgetIndex];
                            widget.flags ^= WIDGET_FLAGS::IS_PRESSED;
                            bool isChecked = widget.flags & WIDGET_FLAGS::IS_PRESSED;

                            WidgetSetCheckboxValue(*this, widgetIndex, isChecked);

                            std::vector<DukValue> args;
                            auto ctx = widgetDesc->OnChange.context();
                            duk_push_boolean(ctx, isChecked);
                            args.push_back(DukValue::take_from_stack(ctx));
                            InvokeEventHandler(info.Owner, widgetDesc->OnChange, args);
                        }
                        else if (widgetDesc->Type == "spinner")
                        {
                            auto& widget = widgets[widgetIndex];
                            if (widget.text != STR_NUMERIC_DOWN && widget.text != STR_NUMERIC_UP)
                            {
                                InvokeEventHandler(info.Owner, widgetDesc->OnClick);
                            }
                        }
                    }
                    break;
                }
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            auto* widget = &widgets[widgetIndex];
            const auto& info = GetInfo(this);
            const auto widgetDesc = info.GetCustomWidgetDesc(this, widgetIndex);
            if (widgetDesc != nullptr)
            {
                if (widgetDesc->Type == "colourpicker")
                {
                    WindowDropdownShowColour(this, widget, colours[widget->colour], widgetDesc->Colour);
                }
                else if (widgetDesc->Type == "dropdown")
                {
                    widget--;
                    auto selectedIndex = widgetDesc->SelectedIndex;
                    const auto& items = widgetDesc->Items;
                    const auto numItems = std::min<size_t>(items.size(), Dropdown::ItemsMaxSize);
                    for (size_t i = 0; i < numItems; i++)
                    {
                        gDropdownItems[i].Format = selectedIndex == static_cast<int32_t>(i) ? STR_OPTIONS_DROPDOWN_ITEM_SELECTED
                                                                                            : STR_OPTIONS_DROPDOWN_ITEM;
                        auto sz = items[i].c_str();
                        std::memcpy(&gDropdownItems[i].Args, &sz, sizeof(const char*));
                    }
                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1,
                        colours[widget->colour], 0, Dropdown::Flag::StayOpen, numItems, widget->width() - 3);
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
                else if (widgetDesc->Type == "textbox")
                {
                    auto* text = const_cast<char*>(widgetDesc->Text.c_str());
                    window_start_textbox(*this, widgetIndex, STR_STRING, text, widgetDesc->MaxLength + 1);
                }
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (dropdownIndex == -1)
                return;

            auto& info = GetInfo(this);
            auto widgetDesc = info.GetCustomWidgetDesc(this, widgetIndex);
            if (widgetDesc != nullptr)
            {
                if (widgetDesc->Type == "colourpicker")
                {
                    UpdateWidgetColour(this, widgetIndex, dropdownIndex);
                }
                else if (widgetDesc->Type == "dropdown")
                {
                    UpdateWidgetSelectedIndex(this, widgetIndex - 1, dropdownIndex);
                }
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            auto& info = GetInfo(this);
            auto widgetDesc = info.GetCustomWidgetDesc(this, widgetIndex);
            if (widgetDesc != nullptr)
            {
                if (widgetDesc->Type == "textbox")
                {
                    UpdateWidgetText(this, widgetIndex, text);

                    std::vector<DukValue> args;
                    auto ctx = widgetDesc->OnChange.context();
                    duk_push_lstring(ctx, text.data(), text.size());
                    args.push_back(DukValue::take_from_stack(ctx));
                    InvokeEventHandler(info.Owner, widgetDesc->OnChange, args);
                }
            }
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            auto& info = GetInfo(this);
            if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
            {
                auto size = info.ListViews[scrollIndex].GetSize();
                return { size.width, size.height };
            }
            return {};
        }

        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto& info = GetInfo(this);
            if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
            {
                info.ListViews[scrollIndex].MouseDown(screenCoords);
            }
        }

        void OnScrollMouseDrag(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto& info = GetInfo(this);
            if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
            {
                info.ListViews[scrollIndex].MouseOver(screenCoords, true);
            }
        }

        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto& info = GetInfo(this);
            if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
            {
                info.ListViews[scrollIndex].MouseOver(screenCoords, false);
            }
        }

        void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
        {
            const auto& info = GetInfo(this);
            if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
            {
                info.ListViews[scrollIndex].Paint(this, &dpi, &scrolls[scrollIndex]);
            }
        }

        void ChangeTab(size_t tabIndex)
        {
            const auto& info = GetInfo(this);

            page = static_cast<int16_t>(tabIndex);
            frame_no = 0;
            RefreshWidgets();

            Invalidate();
            window_event_resize_call(this);
            window_event_invalidate_call(this);
            WindowInitScrollWidgets(*this);
            Invalidate();

            InvokeEventHandler(info.Owner, info.Desc.OnTabChange);
        }

    private:
        std::optional<WidgetIndex> GetViewportWidgetIndex()
        {
            WidgetIndex widgetIndex = 0;
            for (auto widget = widgets; widget->type != WindowWidgetType::Last; widget++)
            {
                if (widget->type == WindowWidgetType::Viewport)
                {
                    return widgetIndex;
                }
                widgetIndex++;
            }
            return std::nullopt;
        }

        void UpdateViewport()
        {
            auto viewportWidgetIndex = GetViewportWidgetIndex();
            if (viewportWidgetIndex)
            {
                auto viewportWidget = &widgets[*viewportWidgetIndex];
                auto& customInfo = GetInfo(this);
                auto widgetInfo = customInfo.GetCustomWidgetDesc(this, *viewportWidgetIndex);
                if (widgetInfo != nullptr)
                {
                    auto left = windowPos.x + viewportWidget->left + 1;
                    auto top = windowPos.y + viewportWidget->top + 1;
                    auto wwidth = viewportWidget->width() - 1;
                    auto wheight = viewportWidget->height() - 1;
                    if (viewport == nullptr)
                    {
                        viewport_create(this, { left, top }, wwidth, wheight, Focus(CoordsXYZ(0, 0, 0)));
                        flags |= WF_NO_SCROLLING;
                        Invalidate();
                    }
                    else
                    {
                        if (viewport->pos.x != left || viewport->pos.y != top || viewport->width != wwidth
                            || viewport->height != wheight)
                        {
                            viewport->pos.x = left;
                            viewport->pos.y = top;
                            viewport->width = wwidth;
                            viewport->height = wheight;
                            viewport->view_width = viewport->zoom.ApplyTo(wwidth);
                            viewport->view_height = viewport->zoom.ApplyTo(wheight);
                            Invalidate();
                        }
                    }
                }
                else
                {
                    RemoveViewport();
                }
            }
            else
            {
                RemoveViewport();
            }
        }

        void SetPressedTab()
        {
            const auto& info = GetInfo(this);
            auto numTabs = info.Desc.Tabs.size();
            if (numTabs != 0)
            {
                for (size_t i = 0; i < numTabs; i++)
                {
                    SetWidgetPressed(static_cast<WidgetIndex>(WIDX_TAB_0 + i), false);
                }
                SetWidgetPressed(WIDX_TAB_0 + page, true);
            }
        }

        void DrawTabImages(rct_drawpixelinfo& dpi)
        {
            const auto& customInfo = GetInfo(this);
            const auto& tabs = customInfo.Desc.Tabs;
            size_t tabIndex = 0;
            for (const auto& tab : tabs)
            {
                auto widgetIndex = static_cast<WidgetIndex>(WIDX_TAB_0 + tabIndex);
                auto widget = &widgets[widgetIndex];
                if (WidgetIsVisible(*this, widgetIndex))
                {
                    auto leftTop = windowPos + tab.offset + ScreenCoordsXY{ widget->left, widget->top };
                    auto image = tab.imageFrameBase;
                    if (static_cast<size_t>(page) == tabIndex && tab.imageFrameDuration != 0 && tab.imageFrameCount != 0)
                    {
                        auto frame = frame_no / tab.imageFrameDuration;
                        auto imageOffset = frame % tab.imageFrameCount;
                        image = image.WithIndex(image.GetIndex() + imageOffset);
                    }
                    gfx_draw_sprite(&dpi, image, leftTop);
                }
                tabIndex++;
            }
        }

        void RefreshWidgets()
        {
            auto& info = GetInfo(this);
            auto& widgetList = info.Widgets;

            widgetList.clear();
            info.WidgetIndexMap.clear();
            info.ListViews.clear();

            // Add default widgets (window shim)
            widgetList.insert(widgetList.begin(), std::begin(CustomDefaultWidgets), std::end(CustomDefaultWidgets));
            for (size_t i = 0; i < widgetList.size(); i++)
            {
                info.WidgetIndexMap.push_back(std::numeric_limits<size_t>::max());
            }

            // Add window tabs
            if (info.Desc.Tabs.size() != 0)
            {
                widgetList[WIDX_CONTENT_PANEL].top = 43;
            }
            for (size_t tabDescIndex = 0; tabDescIndex < info.Desc.Tabs.size(); tabDescIndex++)
            {
                rct_widget widget{};
                widget.type = WindowWidgetType::Tab;
                widget.colour = 1;
                widget.left = static_cast<int16_t>(3 + (tabDescIndex * 31));
                widget.right = widget.left + 30;
                widget.top = 17;
                widget.bottom = 43;
                widget.image = IMAGE_TYPE_REMAP | SPR_TAB;
                widget.tooltip = STR_NONE;
                widgetList.push_back(widget);
                info.WidgetIndexMap.push_back(std::numeric_limits<size_t>::max());
            }

            // Add custom widgets
            auto totalWidgets = info.Desc.Widgets.size();
            auto tabWidgetsOffset = totalWidgets;
            if (info.Desc.Tabs.size() != 0)
            {
                totalWidgets += info.Desc.Tabs[page].Widgets.size();
            }
            for (size_t widgetDescIndex = 0; widgetDescIndex < totalWidgets; widgetDescIndex++)
            {
                const auto& widgetDesc = widgetDescIndex < info.Desc.Widgets.size()
                    ? info.Desc.Widgets[widgetDescIndex]
                    : info.Desc.Tabs[page].Widgets[widgetDescIndex - tabWidgetsOffset];
                auto preWidgetSize = widgetList.size();
                CreateWidget(widgetList, widgetDesc);
                auto numWidetsAdded = widgetList.size() - preWidgetSize;
                for (size_t i = 0; i < numWidetsAdded; i++)
                {
                    info.WidgetIndexMap.push_back(widgetDescIndex);
                }

                if (widgetDesc.Type == "listview")
                {
                    CustomListView listView(this, info.ListViews.size());
                    listView.Owner = info.Owner;
                    listView.SetScrollbars(widgetDesc.Scrollbars, true);
                    listView.SetColumns(widgetDesc.ListViewColumns, true);
                    listView.SetItems(widgetDesc.ListViewItems, true);
                    listView.SelectedCell = widgetDesc.SelectedCell;
                    listView.ShowColumnHeaders = widgetDesc.ShowColumnHeaders;
                    listView.IsStriped = widgetDesc.IsStriped;
                    listView.OnClick = widgetDesc.OnClick;
                    listView.OnHighlight = widgetDesc.OnHighlight;
                    listView.CanSelect = widgetDesc.CanSelect;
                    info.ListViews.push_back(std::move(listView));
                }
            }

            widgetList.push_back(WIDGETS_END);
            widgets = widgetList.data();

            WindowInitScrollWidgets(*this);
            UpdateViewport();
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
            widget.sztooltip = const_cast<utf8*>(desc.Tooltip.c_str());
            widget.flags |= WIDGET_FLAGS::TOOLTIP_IS_STRING;
            if (desc.IsDisabled)
                widget.flags |= WIDGET_FLAGS::IS_DISABLED;
            if (!desc.IsVisible)
                widget.flags |= WIDGET_FLAGS::IS_HIDDEN;

            if (desc.Type == "button")
            {
                if (desc.Image.HasValue())
                {
                    widget.type = desc.HasBorder ? WindowWidgetType::ImgBtn : WindowWidgetType::FlatBtn;
                    widget.image = desc.Image.ToUInt32();
                }
                else
                {
                    widget.type = WindowWidgetType::Button;
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
                widget.type = WindowWidgetType::Checkbox;
                widget.string = const_cast<utf8*>(desc.Text.c_str());
                widget.flags |= WIDGET_FLAGS::TEXT_IS_STRING;
                if (desc.IsChecked)
                {
                    widget.flags |= WIDGET_FLAGS::IS_PRESSED;
                }
                widgetList.push_back(widget);
            }
            else if (desc.Type == "colourpicker")
            {
                widget.type = WindowWidgetType::ColourBtn;
                widget.image = GetColourButtonImage(desc.Colour).ToUInt32();
                widgetList.push_back(widget);
            }
            else if (desc.Type == "custom")
            {
                widget.type = WindowWidgetType::Custom;
                widgetList.push_back(widget);
            }
            else if (desc.Type == "dropdown")
            {
                widget.type = WindowWidgetType::DropdownMenu;
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
                widget.type = WindowWidgetType::Button;
                widget.colour = 1;
                widget.left = desc.X + desc.Width - 12;
                widget.right = desc.X + desc.Width - 2;
                widget.top = desc.Y + 1;
                widget.bottom = desc.Y + desc.Height - 2;
                widget.text = STR_DROPDOWN_GLYPH;
                widget.tooltip = STR_NONE;
                if (desc.IsDisabled)
                    widget.flags |= WIDGET_FLAGS::IS_DISABLED;
                widgetList.push_back(widget);
            }
            else if (desc.Type == "groupbox")
            {
                widget.type = WindowWidgetType::Groupbox;
                widget.string = const_cast<utf8*>(desc.Text.c_str());
                widget.flags |= WIDGET_FLAGS::TEXT_IS_STRING;
                widgetList.push_back(widget);
            }
            else if (desc.Type == "label")
            {
                widget.type = WindowWidgetType::Label;
                widget.string = const_cast<utf8*>(desc.Text.c_str());
                widget.flags |= WIDGET_FLAGS::TEXT_IS_STRING;
                if (desc.TextAlign == TextAlignment::CENTRE)
                {
                    widget.type = WindowWidgetType::LabelCentred;
                }
                widgetList.push_back(widget);
            }
            else if (desc.Type == "listview")
            {
                widget.type = WindowWidgetType::Scroll;
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
                widget.type = WindowWidgetType::Spinner;
                widget.string = const_cast<utf8*>(desc.Text.c_str());
                widget.flags |= WIDGET_FLAGS::TEXT_IS_STRING;
                widgetList.push_back(widget);

                // Add the decrement button
                widget = {};
                widget.type = WindowWidgetType::Button;
                widget.colour = 1;
                widget.left = desc.X + desc.Width - 26;
                widget.right = widget.left + 12;
                widget.top = desc.Y + 1;
                widget.bottom = desc.Y + desc.Height - 2;
                widget.text = STR_NUMERIC_DOWN;
                widget.tooltip = STR_NONE;
                if (desc.IsDisabled)
                    widget.flags |= WIDGET_FLAGS::IS_DISABLED;
                widget.flags |= WIDGET_FLAGS::IS_HOLDABLE;
                widgetList.push_back(widget);

                // Add the increment button
                widget.left = desc.X + desc.Width - 13;
                widget.right = widget.left + 11;
                widget.text = STR_NUMERIC_UP;
                widgetList.push_back(widget);
            }
            else if (desc.Type == "textbox")
            {
                widget.type = WindowWidgetType::TextBox;
                widget.string = const_cast<utf8*>(desc.Text.c_str());
                widget.flags |= WIDGET_FLAGS::TEXT_IS_STRING;
                widgetList.push_back(widget);
            }
            else if (desc.Type == "viewport")
            {
                widget.type = WindowWidgetType::Viewport;
                widget.text = STR_NONE;
                widgetList.push_back(widget);
            }
        }

        static rct_windownumber GetNewWindowNumber()
        {
            auto result = _nextWindowNumber++;
            while (window_find_by_number(WindowClass::Custom, result) != nullptr)
            {
                result++;
            }
            return result;
        }
    };

    rct_windownumber CustomWindow::_nextWindowNumber;

    rct_window* window_custom_open(std::shared_ptr<Plugin> owner, DukValue dukDesc)
    {
        auto desc = CustomWindowDesc::FromDukValue(dukDesc);
        uint16_t windowFlags = WF_RESIZABLE | WF_TRANSPARENT;
        CustomWindow* window{};
        if (desc.X && desc.Y)
        {
            window = WindowCreate<CustomWindow>(
                WindowClass::Custom, { *desc.X, *desc.Y }, desc.Width, desc.Height, windowFlags);
        }
        else
        {
            window = WindowCreate<CustomWindow>(WindowClass::Custom, desc.Width, desc.Height, windowFlags);
        }
        if (window != nullptr)
        {
            window->Initialise(owner, desc);
        }
        return window;
    }

    static CustomWindowInfo& GetInfo(rct_window* w)
    {
        return *(static_cast<CustomWindowInfo*>(w->custom_info));
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

    void UpdateWindowTitle(rct_window* w, std::string_view value)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            customInfo.Desc.Title = value;
        }
    }

    void UpdateWindowTab(rct_window* w, int32_t tabIndex)
    {
        if (w->classification == WindowClass::Custom && w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            if (tabIndex >= 0 && tabIndex < static_cast<int32_t>(customInfo.Desc.Tabs.size()))
            {
                static_cast<CustomWindow*>(w)->ChangeTab(tabIndex);
            }
        }
    }

    void UpdateWidgetText(rct_window* w, WidgetIndex widgetIndex, std::string_view value)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                customWidgetInfo->Text = value;
                w->widgets[widgetIndex].string = customWidgetInfo->Text.data();
                widget_invalidate(*w, widgetIndex);
            }
        }
    }

    void UpdateWidgetItems(rct_window* w, WidgetIndex widgetIndex, const std::vector<std::string>& items)
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

    void UpdateWidgetColour(rct_window* w, WidgetIndex widgetIndex, colour_t colour)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                auto& widget = w->widgets[widgetIndex];

                auto lastColour = customWidgetInfo->Colour;
                if (lastColour != colour && colour < COLOUR_COUNT)
                {
                    customWidgetInfo->Colour = colour;
                    widget.image = GetColourButtonImage(colour).ToUInt32();
                    widget_invalidate(*w, widgetIndex);

                    std::vector<DukValue> args;
                    auto ctx = customWidgetInfo->OnChange.context();
                    duk_push_int(ctx, colour);
                    args.push_back(DukValue::take_from_stack(ctx));
                    InvokeEventHandler(customInfo.Owner, customWidgetInfo->OnChange, args);
                }
            }
        }
    }

    void UpdateWidgetSelectedIndex(rct_window* w, WidgetIndex widgetIndex, int32_t selectedIndex)
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

                widget_invalidate(*w, widgetIndex);

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

    std::vector<std::string> GetWidgetItems(rct_window* w, WidgetIndex widgetIndex)
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

    colour_t GetWidgetColour(rct_window* w, WidgetIndex widgetIndex)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                return customWidgetInfo->Colour;
            }
        }
        return COLOUR_BLACK;
    }

    int32_t GetWidgetSelectedIndex(rct_window* w, WidgetIndex widgetIndex)
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

    rct_window* FindCustomWindowByClassification(std::string_view classification)
    {
        for (const auto& w : g_window_list)
        {
            if (w->classification == WindowClass::Custom)
            {
                const auto& customInfo = GetInfo(w.get());
                if (customInfo.Desc.Classification == classification)
                {
                    return w.get();
                }
            }
        }
        return nullptr;
    }

    std::optional<WidgetIndex> FindWidgetIndexByName(rct_window* w, std::string_view name)
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
                        return static_cast<WidgetIndex>(i);
                    }
                }
            }
        }
        return std::nullopt;
    }

    std::string GetWidgetName(rct_window* w, WidgetIndex widgetIndex)
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

    void SetWidgetName(rct_window* w, WidgetIndex widgetIndex, std::string_view name)
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

    std::string GetWidgetTooltip(rct_window* w, WidgetIndex widgetIndex)
    {
        if (w->custom_info != nullptr)
        {
            const auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                return customWidgetInfo->Tooltip;
            }
        }
        return {};
    }

    void SetWidgetTooltip(rct_window* w, WidgetIndex widgetIndex, std::string_view tooltip)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                customWidgetInfo->Tooltip = std::string(tooltip);
            }
        }
    }

    CustomListView* GetCustomListView(rct_window* w, WidgetIndex widgetIndex)
    {
        if (w->custom_info != nullptr)
        {
            auto& info = GetInfo(w);
            auto scrollIndex = window_get_scroll_data_index(*w, widgetIndex);
            if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
            {
                return &info.ListViews[scrollIndex];
            }
        }
        return nullptr;
    }

    int32_t GetWidgetMaxLength(rct_window* w, WidgetIndex widgetIndex)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                return customWidgetInfo->MaxLength;
            }
        }
        return 0;
    }

    void SetWidgetMaxLength(rct_window* w, WidgetIndex widgetIndex, int32_t value)
    {
        if (w->custom_info != nullptr)
        {
            auto& customInfo = GetInfo(w);
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                customWidgetInfo->MaxLength = value;
            }
        }
    }

    void CloseWindowsOwnedByPlugin(std::shared_ptr<Plugin> plugin)
    {
        // Get all the windows that need closing
        std::vector<std::shared_ptr<rct_window>> customWindows;
        for (const auto& window : g_window_list)
        {
            if (window->classification == WindowClass::Custom)
            {
                auto customWindow = reinterpret_cast<CustomWindow*>(window.get());
                auto customInfo = reinterpret_cast<CustomWindowInfo*>(customWindow->custom_info);
                if (customInfo != nullptr && customInfo->Owner == plugin)
                {
                    customWindows.push_back(window);
                }
            }
        }

        for (auto& window : customWindows)
        {
            window_close(*window.get());
        }
    }

} // namespace OpenRCT2::Ui::Windows

#endif
