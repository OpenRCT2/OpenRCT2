/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "../UiStringIds.h"
    #include "../interface/Dropdown.h"
    #include "../interface/Widget.h"
    #include "../scripting/ScGraphicsContext.hpp"
    #include "../scripting/ScWidget.hpp"
    #include "../windows/Windows.h"
    #include "CustomListView.h"
    #include "ScUi.hpp"
    #include "ScWindow.hpp"

    #include <limits>
    #include <openrct2/drawing/Drawing.h>
    #include <openrct2/interface/Window.h>
    #include <openrct2/localisation/Formatter.h>
    #include <openrct2/localisation/Language.h>
    #include <openrct2/scripting/Plugin.h>
    #include <openrct2/sprites.h>
    #include <optional>
    #include <string>
    #include <utility>
    #include <vector>

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

    static Widget CustomDefaultWidgets[] = {
        WINDOW_SHIM(STR_STRING, 50, 50),
        MakeWidget({ 0, 14 }, { 50, 36 }, WindowWidgetType::Resize, WindowColour::Secondary), // content panel
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
                if (dukImage.type() == DukValue::Type::STRING || dukImage.type() == DukValue::Type::NUMBER)
                {
                    result.Image = ImageId(ImageFromDuk(dukImage));
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
            if (dukImage.type() == DukValue::Type::STRING || dukImage.type() == DukValue::Type::NUMBER)
            {
                result.imageFrameBase = ImageId(ImageFromDuk(dukImage));
                result.imageFrameCount = 0;
                result.imageFrameDuration = 0;
            }
            else if (dukImage.type() == DukValue::Type::OBJECT)
            {
                result.imageFrameBase = ImageId(dukImage["frameBase"].as_uint());
                result.imageFrameCount = AsOrDefault(dukImage["frameCount"], 0);
                result.imageFrameDuration = AsOrDefault(dukImage["frameDuration"], 0);

                if (dukImage["primaryColour"].type() == DukValue::Type::NUMBER)
                {
                    result.imageFrameBase = result.imageFrameBase.WithPrimary(dukImage["primaryColour"].as_uint());

                    if (dukImage["secondaryColour"].type() == DukValue::Type::NUMBER)
                    {
                        result.imageFrameBase = result.imageFrameBase.WithSecondary(dukImage["secondaryColour"].as_uint());

                        if (dukImage["tertiaryColour"].type() == DukValue::Type::NUMBER)
                        {
                            result.imageFrameBase = result.imageFrameBase.WithTertiary(dukImage["tertiaryColour"].as_uint());
                        }
                    }
                }

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
        std::vector<ColourWithFlags> Colours;
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
                    ColourWithFlags c = { COLOUR_BLACK };
                    if (w.type() == DukValue::Type::NUMBER)
                    {
                        colour_t colour = w.as_uint() & ~kLegacyColourFlagTranslucent;
                        auto isTranslucent = (w.as_uint() & kLegacyColourFlagTranslucent);
                        c.colour = std::clamp<colour_t>(colour, COLOUR_BLACK, COLOUR_COUNT - 1);
                        c.flags = (isTranslucent ? EnumToFlag(ColourFlag::translucent) : 0);
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
        std::vector<Widget> Widgets;
        std::vector<size_t> WidgetIndexMap;
        std::vector<CustomListView> ListViews;

        CustomWindowInfo(std::shared_ptr<Plugin> owner, const CustomWindowDesc& desc)
            : Owner(owner)
            , Desc(desc)
        {
        }

        CustomWindowInfo(const CustomWindowInfo&) = delete;

        const CustomWidgetDesc* GetCustomWidgetDesc(WindowBase* w, size_t widgetIndex) const
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

        CustomWidgetDesc* GetCustomWidgetDesc(WindowBase* w, size_t widgetIndex)
        {
            return const_cast<CustomWidgetDesc*>(std::as_const(*this).GetCustomWidgetDesc(w, widgetIndex));
        }
    };

    class CustomWindow;
    static CustomWindowInfo& GetInfo(CustomWindow* w);
    static void InvokeEventHandler(const std::shared_ptr<Plugin>& owner, const DukValue& dukHandler);
    static void InvokeEventHandler(
        const std::shared_ptr<Plugin>& owner, const DukValue& dukHandler, const std::vector<DukValue>& args);

    class CustomWindow final : public Window
    {
    private:
        static rct_windownumber _nextWindowNumber;
        CustomWindowInfo _info;

    public:
        CustomWindow(std::shared_ptr<Plugin> owner, const CustomWindowDesc& desc)
            : _info(owner, desc)
        {
        }

        void OnOpen() override
        {
            number = GetNewWindowNumber();

            // Set window tab
            page = _info.Desc.TabIndex.value_or(0);

            // Set window colours
            colours[0] = COLOUR_GREY;
            colours[1] = COLOUR_GREY;
            colours[2] = COLOUR_GREY;
            auto numColours = std::min(std::size(colours), std::size(_info.Desc.Colours));
            for (size_t i = 0; i < numColours; i++)
            {
                colours[i] = _info.Desc.Colours[i];
            }

            if (_info.Desc.IsResizable())
            {
                min_width = _info.Desc.MinWidth.value_or(0);
                min_height = _info.Desc.MinHeight.value_or(0);
                max_width = _info.Desc.MaxWidth.value_or(std::numeric_limits<int16_t>::max());
                max_height = _info.Desc.MaxHeight.value_or(std::numeric_limits<int16_t>::max());
            }
            RefreshWidgets();
        }

        void OnClose() override
        {
            InvokeEventHandler(_info.Owner, _info.Desc.OnClose);
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
            if (_info.Desc.Tabs.size() > static_cast<size_t>(page))
            {
                const auto& tab = _info.Desc.Tabs[page];
                if (tab.imageFrameCount != 0)
                {
                    frame_no++;
                    if (frame_no >= tab.imageFrameCount * tab.imageFrameDuration)
                    {
                        frame_no = 0;
                    }

                    InvalidateWidget(WIDX_TAB_0 + this->page);
                }
            }

            InvokeEventHandler(_info.Owner, _info.Desc.OnUpdate);

            // Since the plugin may alter widget positions and sizes during an update event,
            // we need to force an update for all list view scrollbars
            WidgetIndex widgetIndex = 0;
            for (auto& widget : widgets)
            {
                if (widget.type == WindowWidgetType::Scroll)
                {
                    WidgetScrollUpdateThumbs(*this, widgetIndex);
                }
                widgetIndex++;
            }
        }

        void OnPrepareDraw() override
        {
            // This has to be called to ensure the window frame is correctly initialised - not doing this will
            // cause an assertion to be hit.
            ResizeFrameWithPage();
            widgets[WIDX_CLOSE].text = colours[0].hasFlag(ColourFlag::translucent) ? STR_CLOSE_X_WHITE : STR_CLOSE_X;

            // Having the content panel visible for transparent windows makes the borders darker than they should be
            // For now just hide it if there are no tabs and the window is not resizable
            auto canResize = (flags & WF_RESIZABLE) != 0 && (min_width != max_width || min_height != max_height);
            auto numTabs = _info.Desc.Tabs.size();
            if (canResize || numTabs != 0)
            {
                widgets[WIDX_CONTENT_PANEL].flags &= ~WIDGET_FLAGS::IS_HIDDEN;
            }
            else
            {
                widgets[WIDX_CONTENT_PANEL].flags |= WIDGET_FLAGS::IS_HIDDEN;
            }

            SetPressedTab();

            const auto& desc = _info.Desc;
            auto ft = Formatter::Common();
            ft.Add<const char*>(desc.Title.c_str());

            size_t scrollIndex = 0;
            for (const auto& widget : widgets)
            {
                if (widget.type == WindowWidgetType::Scroll)
                {
                    auto& listView = _info.ListViews[scrollIndex];
                    auto wwidth = widget.width() + 1 - 2;
                    auto wheight = widget.height() + 1 - 2;
                    if (listView.GetScrollbars() == ScrollbarType::Horizontal
                        || listView.GetScrollbars() == ScrollbarType::Both)
                    {
                        wheight -= kScrollBarWidth + 1;
                    }
                    if (listView.GetScrollbars() == ScrollbarType::Vertical || listView.GetScrollbars() == ScrollbarType::Both)
                    {
                        wwidth -= kScrollBarWidth + 1;
                    }
                    listView.Resize({ wwidth, wheight });
                    scrollIndex++;
                }
            }
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            WindowDrawWidgets(*this, dpi);
            DrawTabImages(dpi);
            if (viewport != nullptr)
            {
                auto widgetIndex = GetViewportWidgetIndex();
                if (WidgetIsVisible(*this, widgetIndex.value_or(false)))
                {
                    WindowDrawViewport(dpi, *this);
                }
            }
        }

        void OnDrawWidget(WidgetIndex widgetIndex, DrawPixelInfo& dpi) override
        {
            const auto& widget = widgets[widgetIndex];
            const auto widgetDesc = _info.GetCustomWidgetDesc(this, widgetIndex);
            if (widgetDesc != nullptr && widgetDesc->Type == "custom")
            {
                auto& onDraw = widgetDesc->OnDraw;
                if (onDraw.is_function())
                {
                    DrawPixelInfo widgetDpi;
                    if (ClipDrawPixelInfo(
                            widgetDpi, dpi, { windowPos.x + widget.left, windowPos.y + widget.top }, widget.width(),
                            widget.height()))
                    {
                        auto ctx = onDraw.context();
                        auto dukWidget = ScWidget::ToDukValue(ctx, this, widgetIndex);
                        auto dukG = GetObjectAsDukValue(ctx, std::make_shared<ScGraphicsContext>(ctx, widgetDpi));
                        auto& scriptEngine = GetContext()->GetScriptEngine();
                        scriptEngine.ExecutePluginCall(_info.Owner, widgetDesc->OnDraw, dukWidget, { dukG }, false);
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
                {
                    auto* windowMgr = Ui::GetWindowManager();
                    windowMgr->Close(*this);
                    break;
                }
                default:
                {
                    if (widgetIndex >= WIDX_TAB_0
                        && widgetIndex < static_cast<WidgetIndex>(WIDX_TAB_0 + _info.Desc.Tabs.size()))
                    {
                        ChangeTab(widgetIndex - WIDX_TAB_0);
                        break;
                    }

                    const auto widgetDesc = _info.GetCustomWidgetDesc(this, widgetIndex);
                    if (widgetDesc != nullptr)
                    {
                        if (widgetDesc->Type == "button")
                        {
                            InvokeEventHandler(_info.Owner, widgetDesc->OnClick);
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
                            InvokeEventHandler(_info.Owner, widgetDesc->OnChange, args);
                        }
                        else if (widgetDesc->Type == "spinner")
                        {
                            auto& widget = widgets[widgetIndex];
                            if (widget.text != STR_NUMERIC_DOWN && widget.text != STR_NUMERIC_UP)
                            {
                                InvokeEventHandler(_info.Owner, widgetDesc->OnClick);
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
            const auto widgetDesc = _info.GetCustomWidgetDesc(this, widgetIndex);
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
                    const auto numItems = std::min<size_t>(items.size(), Dropdown::kItemsMaxSize);
                    for (size_t i = 0; i < numItems; i++)
                    {
                        gDropdownItems[i].Format = STR_OPTIONS_DROPDOWN_ITEM;
                        if (selectedIndex == static_cast<int32_t>(i))
                            gDropdownItems[i].Format = STR_OPTIONS_DROPDOWN_ITEM_SELECTED;

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
                        InvokeEventHandler(_info.Owner, widgetDesc->OnDecrement);
                    }
                    else if (widget->text == STR_NUMERIC_UP)
                    {
                        InvokeEventHandler(_info.Owner, widgetDesc->OnIncrement);
                    }
                }
                else if (widgetDesc->Type == "textbox")
                {
                    WindowStartTextbox(*this, widgetIndex, widgetDesc->Text, widgetDesc->MaxLength + 1);
                }
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (dropdownIndex == -1)
                return;

            auto widgetDesc = _info.GetCustomWidgetDesc(this, widgetIndex);
            if (widgetDesc != nullptr)
            {
                if (widgetDesc->Type == "colourpicker")
                {
                    UpdateWidgetColour(this, widgetIndex, ColourDropDownIndexToColour(dropdownIndex));
                }
                else if (widgetDesc->Type == "dropdown")
                {
                    UpdateWidgetSelectedIndex(this, widgetIndex - 1, dropdownIndex);
                }
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            auto widgetDesc = _info.GetCustomWidgetDesc(this, widgetIndex);
            if (widgetDesc != nullptr)
            {
                if (widgetDesc->Type == "textbox")
                {
                    UpdateWidgetText(this, widgetIndex, text);

                    std::vector<DukValue> args;
                    auto ctx = widgetDesc->OnChange.context();
                    duk_push_lstring(ctx, text.data(), text.size());
                    args.push_back(DukValue::take_from_stack(ctx));
                    InvokeEventHandler(_info.Owner, widgetDesc->OnChange, args);
                }
            }
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            if (scrollIndex < static_cast<int32_t>(_info.ListViews.size()))
            {
                auto size = _info.ListViews[scrollIndex].GetSize();
                return { size.width, size.height };
            }
            return {};
        }

        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (scrollIndex < static_cast<int32_t>(_info.ListViews.size()))
            {
                _info.ListViews[scrollIndex].MouseDown(screenCoords);
            }
        }

        void OnScrollMouseDrag(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (scrollIndex < static_cast<int32_t>(_info.ListViews.size()))
            {
                _info.ListViews[scrollIndex].MouseOver(screenCoords, true);
            }
        }

        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (scrollIndex < static_cast<int32_t>(_info.ListViews.size()))
            {
                _info.ListViews[scrollIndex].MouseOver(screenCoords, false);
            }
        }

        void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
        {
            if (scrollIndex < static_cast<int32_t>(_info.ListViews.size()))
            {
                _info.ListViews[scrollIndex].Paint(this, dpi, &scrolls[scrollIndex]);
            }
        }

        void ChangeTab(size_t tabIndex)
        {
            page = static_cast<int16_t>(tabIndex);
            frame_no = 0;
            RefreshWidgets();

            Invalidate();
            OnResize();
            OnPrepareDraw();
            WindowInitScrollWidgets(*this);
            Invalidate();

            InvokeEventHandler(_info.Owner, _info.Desc.OnTabChange);
        }

        const CustomWindowInfo& GetInfo() const
        {
            return _info;
        }
        CustomWindowInfo& GetInfo()
        {
            return _info;
        }

    private:
        std::optional<WidgetIndex> GetViewportWidgetIndex()
        {
            WidgetIndex widgetIndex = 0;
            for (auto& widget : widgets)
            {
                if (widget.type == WindowWidgetType::Viewport)
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
                auto widgetInfo = _info.GetCustomWidgetDesc(this, *viewportWidgetIndex);
                if (widgetInfo != nullptr)
                {
                    auto left = windowPos.x + viewportWidget->left + 1;
                    auto top = windowPos.y + viewportWidget->top + 1;
                    auto wwidth = viewportWidget->width() - 1;
                    auto wheight = viewportWidget->height() - 1;
                    if (viewport == nullptr)
                    {
                        ViewportCreate(this, { left, top }, wwidth, wheight, Focus(CoordsXYZ(0, 0, 0)));
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
            auto numTabs = _info.Desc.Tabs.size();
            if (numTabs != 0)
            {
                for (size_t i = 0; i < numTabs; i++)
                {
                    SetWidgetPressed(static_cast<WidgetIndex>(WIDX_TAB_0 + i), false);
                }
                SetWidgetPressed(WIDX_TAB_0 + page, true);
            }
        }

        void DrawTabImages(DrawPixelInfo& dpi)
        {
            const auto& tabs = _info.Desc.Tabs;
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
                    GfxDrawSprite(dpi, image, leftTop);
                }
                tabIndex++;
            }
        }

        void RefreshWidgets()
        {
            auto& widgetList = _info.Widgets;

            widgetList.clear();
            _info.WidgetIndexMap.clear();
            _info.ListViews.clear();

            // Add default widgets (window shim)
            widgetList.insert(widgetList.begin(), std::begin(CustomDefaultWidgets), std::end(CustomDefaultWidgets));
            for (size_t i = 0; i < widgetList.size(); i++)
            {
                _info.WidgetIndexMap.push_back(std::numeric_limits<size_t>::max());
            }

            // Add window tabs
            if (_info.Desc.Tabs.size() != 0)
            {
                widgetList[WIDX_CONTENT_PANEL].top = 43;
            }
            for (size_t tabDescIndex = 0; tabDescIndex < _info.Desc.Tabs.size(); tabDescIndex++)
            {
                Widget widget{};
                widget.type = WindowWidgetType::Tab;
                widget.colour = 1;
                widget.left = static_cast<int16_t>(3 + (tabDescIndex * 31));
                widget.right = widget.left + 30;
                widget.top = 17;
                widget.bottom = 43;
                widget.image = ImageId(SPR_TAB, FilterPaletteID::PaletteNull);
                widget.tooltip = kStringIdNone;
                widgetList.push_back(widget);
                _info.WidgetIndexMap.push_back(std::numeric_limits<size_t>::max());
            }

            // Add custom widgets
            auto totalWidgets = _info.Desc.Widgets.size();
            auto tabWidgetsOffset = totalWidgets;
            if (_info.Desc.Tabs.size() != 0)
            {
                totalWidgets += _info.Desc.Tabs[page].Widgets.size();
            }
            for (size_t widgetDescIndex = 0; widgetDescIndex < totalWidgets; widgetDescIndex++)
            {
                const auto& widgetDesc = widgetDescIndex < _info.Desc.Widgets.size()
                    ? _info.Desc.Widgets[widgetDescIndex]
                    : _info.Desc.Tabs[page].Widgets[widgetDescIndex - tabWidgetsOffset];
                auto preWidgetSize = widgetList.size();
                CreateWidget(widgetList, widgetDesc);
                auto numWidgetsAdded = widgetList.size() - preWidgetSize;
                for (size_t i = 0; i < numWidgetsAdded; i++)
                {
                    _info.WidgetIndexMap.push_back(widgetDescIndex);
                }

                if (widgetDesc.Type == "listview")
                {
                    CustomListView listView(this, _info.ListViews.size());
                    listView.Owner = _info.Owner;
                    listView.SetScrollbars(widgetDesc.Scrollbars, true);
                    listView.SetColumns(widgetDesc.ListViewColumns, true);
                    listView.SetItems(widgetDesc.ListViewItems, true);
                    listView.SelectedCell = widgetDesc.SelectedCell;
                    listView.ShowColumnHeaders = widgetDesc.ShowColumnHeaders;
                    listView.IsStriped = widgetDesc.IsStriped;
                    listView.OnClick = widgetDesc.OnClick;
                    listView.OnHighlight = widgetDesc.OnHighlight;
                    listView.CanSelect = widgetDesc.CanSelect;
                    _info.ListViews.push_back(std::move(listView));
                }
            }

            SetWidgets(widgetList);

            WindowInitScrollWidgets(*this);
            UpdateViewport();
        }

        static void CreateWidget(std::vector<Widget>& widgetList, const CustomWidgetDesc& desc)
        {
            Widget widget{};
            widget.colour = 1;
            widget.left = desc.X;
            widget.top = desc.Y;
            widget.right = desc.X + desc.Width - 1;
            widget.bottom = desc.Y + desc.Height - 1;
            widget.content = std::numeric_limits<uint32_t>::max();
            widget.tooltip = kStringIdNone;
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
                    widget.image = desc.Image;
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
                widget.image = GetColourButtonImage(desc.Colour);
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
                widget.tooltip = kStringIdNone;
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

                // Add the increment button
                widget = {};
                widget.type = WindowWidgetType::Button;
                widget.colour = 1;
                widget.left = desc.X + desc.Width - 13;
                widget.right = widget.left + 11;
                widget.top = desc.Y + 1;
                widget.bottom = desc.Y + desc.Height - 2;
                widget.text = STR_NUMERIC_UP;
                widget.tooltip = kStringIdNone;
                if (desc.IsDisabled)
                    widget.flags |= WIDGET_FLAGS::IS_DISABLED;
                widget.flags |= WIDGET_FLAGS::IS_HOLDABLE;
                widgetList.push_back(widget);

                // Add the decrement button
                widget.left = desc.X + desc.Width - 26;
                widget.right = widget.left + 12;
                widget.text = STR_NUMERIC_DOWN;
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
                widget.text = kStringIdNone;
                widgetList.push_back(widget);
            }
        }

        static rct_windownumber GetNewWindowNumber()
        {
            auto result = _nextWindowNumber++;
            auto* windowMgr = GetWindowManager();
            while (windowMgr->FindByNumber(WindowClass::Custom, result) != nullptr)
            {
                result++;
            }
            return result;
        }
    };

    rct_windownumber CustomWindow::_nextWindowNumber;

    WindowBase* WindowCustomOpen(std::shared_ptr<Plugin> owner, DukValue dukDesc)
    {
        auto desc = CustomWindowDesc::FromDukValue(dukDesc);
        uint16_t windowFlags = WF_RESIZABLE | WF_TRANSPARENT;
        auto* windowMgr = GetWindowManager();

        CustomWindow* window{};
        if (desc.X && desc.Y)
        {
            window = windowMgr->Create<CustomWindow>(
                WindowClass::Custom, { *desc.X, *desc.Y }, desc.Width, desc.Height, windowFlags, owner, desc);
        }
        else
        {
            window = windowMgr->Create<CustomWindow>(WindowClass::Custom, desc.Width, desc.Height, windowFlags, owner, desc);
        }
        return window;
    }

    static CustomWindowInfo& GetInfo(CustomWindow* w)
    {
        return w->GetInfo();
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

    std::string GetWindowTitle(WindowBase* w)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            return customInfo.Desc.Title;
        }
        return {};
    }

    void UpdateWindowTitle(WindowBase* w, std::string_view value)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            customInfo.Desc.Title = value;
        }
    }

    void UpdateWindowTab(WindowBase* w, int32_t tabIndex)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            if (tabIndex >= 0 && tabIndex < static_cast<int32_t>(customInfo.Desc.Tabs.size()))
            {
                static_cast<CustomWindow*>(w)->ChangeTab(tabIndex);
            }
        }
    }

    void UpdateWidgetText(WindowBase* w, WidgetIndex widgetIndex, std::string_view value)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                customWidgetInfo->Text = value;
                w->widgets[widgetIndex].string = customWidgetInfo->Text.data();

                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->InvalidateWidget(*w, widgetIndex);
            }
        }
    }

    void UpdateWidgetItems(WindowBase* w, WidgetIndex widgetIndex, const std::vector<std::string>& items)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                customWidgetInfo->Items = items;
                UpdateWidgetSelectedIndex(w, widgetIndex, customWidgetInfo->SelectedIndex);
            }
        }
    }

    void UpdateWidgetColour(WindowBase* w, WidgetIndex widgetIndex, colour_t colour)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                auto& widget = w->widgets[widgetIndex];

                auto lastColour = customWidgetInfo->Colour;
                if (lastColour != colour && colour < COLOUR_COUNT)
                {
                    customWidgetInfo->Colour = colour;
                    widget.image = GetColourButtonImage(colour);

                    auto* windowMgr = Ui::GetWindowManager();
                    windowMgr->InvalidateWidget(*w, widgetIndex);

                    std::vector<DukValue> args;
                    auto ctx = customWidgetInfo->OnChange.context();
                    duk_push_int(ctx, colour);
                    args.push_back(DukValue::take_from_stack(ctx));
                    InvokeEventHandler(customInfo.Owner, customWidgetInfo->OnChange, args);
                }
            }
        }
    }

    void UpdateWidgetSelectedIndex(WindowBase* w, WidgetIndex widgetIndex, int32_t selectedIndex)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
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

                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->InvalidateWidget(*w, widgetIndex);

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

    std::vector<std::string> GetWidgetItems(WindowBase* w, WidgetIndex widgetIndex)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                return customWidgetInfo->Items;
            }
        }
        return {};
    }

    colour_t GetWidgetColour(WindowBase* w, WidgetIndex widgetIndex)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                return customWidgetInfo->Colour;
            }
        }
        return COLOUR_BLACK;
    }

    int32_t GetWidgetSelectedIndex(WindowBase* w, WidgetIndex widgetIndex)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                return customWidgetInfo->SelectedIndex;
            }
        }
        return -1;
    }

    WindowBase* FindCustomWindowByClassification(std::string_view classification)
    {
        for (const auto& w : g_window_list)
        {
            if (w->classification == WindowClass::Custom)
            {
                const auto& customInfo = GetInfo(static_cast<CustomWindow*>(w.get()));
                if (customInfo.Desc.Classification == classification)
                {
                    return w.get();
                }
            }
        }
        return nullptr;
    }

    std::optional<WidgetIndex> FindWidgetIndexByName(WindowBase* w, std::string_view name)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
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

    std::string GetWidgetName(WindowBase* w, WidgetIndex widgetIndex)
    {
        if (w->classification == WindowClass::Custom)
        {
            const auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                return customWidgetInfo->Name;
            }
        }
        return {};
    }

    void SetWidgetName(WindowBase* w, WidgetIndex widgetIndex, std::string_view name)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                customWidgetInfo->Name = std::string(name);
            }
        }
    }

    std::string GetWidgetTooltip(WindowBase* w, WidgetIndex widgetIndex)
    {
        if (w->classification == WindowClass::Custom)
        {
            const auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                return customWidgetInfo->Tooltip;
            }
        }
        return {};
    }

    void SetWidgetTooltip(WindowBase* w, WidgetIndex widgetIndex, std::string_view tooltip)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                customWidgetInfo->Tooltip = std::string(tooltip);
            }
        }
    }

    CustomListView* GetCustomListView(WindowBase* w, WidgetIndex widgetIndex)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& info = GetInfo(static_cast<CustomWindow*>(w));
            auto scrollIndex = WindowGetScrollDataIndex(*w, widgetIndex);
            if (scrollIndex < static_cast<int32_t>(info.ListViews.size()))
            {
                return &info.ListViews[scrollIndex];
            }
        }
        return nullptr;
    }

    int32_t GetWidgetMaxLength(WindowBase* w, WidgetIndex widgetIndex)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
            auto customWidgetInfo = customInfo.GetCustomWidgetDesc(w, widgetIndex);
            if (customWidgetInfo != nullptr)
            {
                return customWidgetInfo->MaxLength;
            }
        }
        return 0;
    }

    void SetWidgetMaxLength(WindowBase* w, WidgetIndex widgetIndex, int32_t value)
    {
        if (w->classification == WindowClass::Custom)
        {
            auto& customInfo = GetInfo(static_cast<CustomWindow*>(w));
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
        std::vector<std::shared_ptr<WindowBase>> customWindows;
        for (const auto& window : g_window_list)
        {
            if (window->classification == WindowClass::Custom)
            {
                auto customWindow = reinterpret_cast<CustomWindow*>(window.get());
                auto& customInfo = GetInfo(customWindow);
                if (customInfo.Owner == plugin)
                {
                    customWindows.push_back(window);
                }
            }
        }

        for (auto& window : customWindows)
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->Close(*window.get());
        }
    }

} // namespace OpenRCT2::Ui::Windows

#endif
