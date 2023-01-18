/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../interface/Widget.h"
#    include "../interface/Window.h"
#    include "CustomListView.h"
#    include "CustomWindow.h"
#    include "ScViewport.hpp"

#    include <memory>
#    include <openrct2/Context.h>
#    include <openrct2/common.h>
#    include <openrct2/scripting/Duktape.hpp>
#    include <openrct2/scripting/IconNames.hpp>
#    include <openrct2/scripting/ScriptEngine.h>

namespace OpenRCT2::Scripting
{
    class ScWindow;
    class ScWidget
    {
    protected:
        WindowClass _class{};
        rct_windownumber _number{};
        WidgetIndex _widgetIndex{};

    public:
        ScWidget(WindowClass c, rct_windownumber n, WidgetIndex widgetIndex)
            : _class(c)
            , _number(n)
            , _widgetIndex(widgetIndex)
        {
        }

        static DukValue ToDukValue(duk_context* ctx, rct_window* w, WidgetIndex widgetIndex);

    private:
        std::shared_ptr<ScWindow> window_get() const;

        std::string name_get() const
        {
            auto w = GetWindow();
            if (w != nullptr && IsCustomWindow())
            {
                return OpenRCT2::Ui::Windows::GetWidgetName(w, _widgetIndex);
            }
            return {};
        }

        void name_set(const std::string& value)
        {
            auto w = GetWindow();
            if (w != nullptr && IsCustomWindow())
            {
                OpenRCT2::Ui::Windows::SetWidgetName(w, _widgetIndex, value);
            }
        }

        std::string type_get() const
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                switch (widget->type)
                {
                    case WindowWidgetType::Frame:
                        return "frame";
                    case WindowWidgetType::Resize:
                        return "resize";
                    case WindowWidgetType::ImgBtn:
                    case WindowWidgetType::TrnBtn:
                    case WindowWidgetType::FlatBtn:
                    case WindowWidgetType::Button:
                    case WindowWidgetType::CloseBox:
                        return "button";
                    case WindowWidgetType::ColourBtn:
                        return "colourpicker";
                    case WindowWidgetType::Tab:
                        return "tab";
                    case WindowWidgetType::LabelCentred:
                    case WindowWidgetType::Label:
                        return "label";
                    case WindowWidgetType::TableHeader:
                        return "table_header";
                    case WindowWidgetType::Spinner:
                        return "spinner";
                    case WindowWidgetType::DropdownMenu:
                        return "dropdown";
                    case WindowWidgetType::Viewport:
                        return "viewport";
                    case WindowWidgetType::Groupbox:
                        return "groupbox";
                    case WindowWidgetType::Caption:
                        return "caption";
                    case WindowWidgetType::Scroll:
                        return "listview";
                    case WindowWidgetType::Checkbox:
                        return "checkbox";
                    case WindowWidgetType::TextBox:
                        return "textbox";
                    case WindowWidgetType::Empty:
                        return "empty";
                    case WindowWidgetType::Placeholder:
                        return "placeholder";
                    case WindowWidgetType::Custom:
                        return "custom";
                    case WindowWidgetType::Last:
                        return "last";
                }
            }
            return "unknown";
        }

        int32_t x_get() const
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                return widget->left;
            }
            return 0;
        }
        void x_set(int32_t value)
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                auto delta = value - widget->left;

                Invalidate();
                widget->left += delta;
                widget->right += delta;

                if (widget->type == WindowWidgetType::DropdownMenu)
                {
                    auto buttonWidget = widget + 1;
                    buttonWidget->left += delta;
                    buttonWidget->right += delta;
                    WidgetInvalidateByNumber(_class, _number, _widgetIndex + 1);
                }
                else if (widget->type == WindowWidgetType::Spinner)
                {
                    auto upWidget = widget + 1;
                    upWidget->left += delta;
                    upWidget->right += delta;
                    WidgetInvalidateByNumber(_class, _number, _widgetIndex + 1);

                    auto downWidget = widget + 2;
                    downWidget->left += delta;
                    downWidget->right += delta;
                    WidgetInvalidateByNumber(_class, _number, _widgetIndex + 2);
                }

                Invalidate();
            }
        }

        int32_t y_get() const
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                return widget->top;
            }
            return 0;
        }
        void y_set(int32_t value)
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                auto delta = value - widget->top;

                Invalidate();
                widget->top += delta;
                widget->bottom += delta;

                if (widget->type == WindowWidgetType::DropdownMenu)
                {
                    auto buttonWidget = widget + 1;
                    buttonWidget->top += delta;
                    buttonWidget->bottom += delta;
                    WidgetInvalidateByNumber(_class, _number, _widgetIndex + 1);
                }
                else if (widget->type == WindowWidgetType::Spinner)
                {
                    auto upWidget = widget + 1;
                    upWidget->top += delta;
                    upWidget->bottom += delta;
                    WidgetInvalidateByNumber(_class, _number, _widgetIndex + 1);

                    auto downWidget = widget + 2;
                    downWidget->top += delta;
                    downWidget->bottom += delta;
                    WidgetInvalidateByNumber(_class, _number, _widgetIndex + 2);
                }

                Invalidate();
            }
        }

        int32_t width_get() const
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                return widget->width();
            }
            return 0;
        }
        void width_set(int32_t value)
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                auto delta = widget->left + value - widget->right;

                Invalidate();
                widget->right += delta;

                if (widget->type == WindowWidgetType::DropdownMenu)
                {
                    auto buttonWidget = widget + 1;
                    buttonWidget->left += delta;
                    buttonWidget->right += delta;
                    WidgetInvalidateByNumber(_class, _number, _widgetIndex + 1);
                }
                else if (widget->type == WindowWidgetType::Spinner)
                {
                    auto upWidget = widget + 1;
                    upWidget->left += delta;
                    upWidget->right += delta;
                    WidgetInvalidateByNumber(_class, _number, _widgetIndex + 1);

                    auto downWidget = widget + 2;
                    downWidget->left += delta;
                    downWidget->right += delta;
                    WidgetInvalidateByNumber(_class, _number, _widgetIndex + 2);
                }

                Invalidate();
            }
        }

        int32_t height_get() const
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                return widget->height();
            }
            return 0;
        }
        void height_set(int32_t value)
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                auto delta = widget->top + value - widget->bottom;

                Invalidate();
                widget->bottom += delta;

                if (widget->type == WindowWidgetType::DropdownMenu)
                {
                    auto buttonWidget = widget + 1;
                    buttonWidget->bottom += delta;
                    WidgetInvalidateByNumber(_class, _number, _widgetIndex + 1);
                }
                else if (widget->type == WindowWidgetType::Spinner)
                {
                    auto upWidget = widget + 1;
                    upWidget->bottom += delta;
                    WidgetInvalidateByNumber(_class, _number, _widgetIndex + 1);

                    auto downWidget = widget + 2;
                    downWidget->bottom += delta;
                    WidgetInvalidateByNumber(_class, _number, _widgetIndex + 2);
                }

                Invalidate();
            }
        }

        std::string tooltip_get() const
        {
            auto w = GetWindow();
            if (w != nullptr && IsCustomWindow())
            {
                return OpenRCT2::Ui::Windows::GetWidgetTooltip(w, _widgetIndex);
            }
            return {};
        }
        void tooltip_set(const std::string& value)
        {
            auto w = GetWindow();
            if (w != nullptr && IsCustomWindow())
            {
                OpenRCT2::Ui::Windows::SetWidgetTooltip(w, _widgetIndex, value);
            }
        }

        bool isDisabled_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return WidgetIsDisabled(*w, _widgetIndex);
            }
            return false;
        }
        void isDisabled_set(bool value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                WidgetSetDisabled(*w, _widgetIndex, value);

                auto widget = GetWidget();
                if (widget != nullptr)
                {
                    if (widget->type == WindowWidgetType::DropdownMenu)
                    {
                        WidgetSetDisabled(*w, _widgetIndex + 1, value);
                    }
                    else if (widget->type == WindowWidgetType::Spinner)
                    {
                        WidgetSetDisabled(*w, _widgetIndex + 1, value);
                        WidgetSetDisabled(*w, _widgetIndex + 2, value);
                    }
                }
            }
        }

        bool isVisible_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return WidgetIsVisible(*w, _widgetIndex);
            }
            return false;
        }
        void isVisible_set(bool value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                WidgetSetVisible(*w, _widgetIndex, value);

                auto widget = GetWidget();
                if (widget != nullptr)
                {
                    if (widget->type == WindowWidgetType::DropdownMenu)
                    {
                        WidgetSetVisible(*w, _widgetIndex + 1, value);
                    }
                    else if (widget->type == WindowWidgetType::Spinner)
                    {
                        WidgetSetVisible(*w, _widgetIndex + 1, value);
                        WidgetSetVisible(*w, _widgetIndex + 2, value);
                    }
                }
            }
        }

    protected:
        std::string text_get() const
        {
            if (IsCustomWindow())
            {
                auto widget = GetWidget();
                if (widget != nullptr && (widget->flags & WIDGET_FLAGS::TEXT_IS_STRING) && widget->string != nullptr)
                {
                    return widget->string;
                }
            }
            return "";
        }

        void text_set(std::string value)
        {
            auto w = GetWindow();
            if (w != nullptr && IsCustomWindow())
            {
                OpenRCT2::Ui::Windows::UpdateWidgetText(w, _widgetIndex, value);
            }
        }

    public:
        static void Register(duk_context* ctx);

    protected:
        rct_window* GetWindow() const
        {
            if (_class == WindowClass::MainWindow)
                return WindowGetMain();

            return WindowFindByNumber(_class, _number);
        }

        Widget* GetWidget() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return &w->widgets[_widgetIndex];
            }
            return nullptr;
        }

        bool IsCustomWindow() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->classification == WindowClass::Custom;
            }
            return false;
        }

        void Invalidate()
        {
            WidgetInvalidateByNumber(_class, _number, _widgetIndex);
        }
    };

    class ScButtonWidget : public ScWidget
    {
    public:
        ScButtonWidget(WindowClass c, rct_windownumber n, WidgetIndex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScButtonWidget>(ctx);
            dukglue_register_property(ctx, &ScButtonWidget::border_get, &ScButtonWidget::border_set, "border");
            dukglue_register_property(ctx, &ScButtonWidget::isPressed_get, &ScButtonWidget::isPressed_set, "isPressed");
            dukglue_register_property(ctx, &ScButtonWidget::image_get, &ScButtonWidget::image_set, "image");
            // Explicit template due to text being a base method
            dukglue_register_property<ScButtonWidget, std::string, std::string>(
                ctx, &ScButtonWidget::text_get, &ScButtonWidget::text_set, "text");
        }

    private:
        bool border_get() const
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                return widget->type == WindowWidgetType::ImgBtn;
            }
            return false;
        }
        void border_set(bool value)
        {
            auto widget = GetWidget();
            if (widget != nullptr && (widget->type == WindowWidgetType::FlatBtn || widget->type == WindowWidgetType::ImgBtn))
            {
                if (value)
                    widget->type = WindowWidgetType::ImgBtn;
                else
                    widget->type = WindowWidgetType::FlatBtn;
                Invalidate();
            }
        }

        bool isPressed_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return WidgetIsPressed(*w, _widgetIndex);
            }
            return false;
        }
        void isPressed_set(bool value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                WidgetSetCheckboxValue(*w, _widgetIndex, value ? 1 : 0);
                Invalidate();
            }
        }

        uint32_t image_get() const
        {
            auto widget = GetWidget();
            if (widget != nullptr && widget->type == WindowWidgetType::FlatBtn)
            {
                if (GetTargetAPIVersion() <= API_VERSION_63_G2_REORDER)
                {
                    return LegacyIconIndex(widget->image.GetIndex());
                }
                return widget->image.GetIndex();
            }
            return 0;
        }

        void image_set(DukValue value)
        {
            auto widget = GetWidget();
            if (widget != nullptr && widget->type == WindowWidgetType::FlatBtn)
            {
                widget->image = ImageId(ImageFromDuk(value));
                Invalidate();
            }
        }
    };

    class ScCheckBoxWidget : public ScWidget
    {
    public:
        ScCheckBoxWidget(WindowClass c, rct_windownumber n, WidgetIndex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScCheckBoxWidget>(ctx);
            dukglue_register_property(ctx, &ScCheckBoxWidget::isChecked_get, &ScCheckBoxWidget::isChecked_set, "isChecked");
            // Explicit template due to text being a base method
            dukglue_register_property<ScCheckBoxWidget, std::string, std::string>(
                ctx, &ScCheckBoxWidget::text_get, &ScCheckBoxWidget::text_set, "text");
        }

    private:
        bool isChecked_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return WidgetIsPressed(*w, _widgetIndex);
            }
            return false;
        }
        void isChecked_set(bool value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                WidgetSetCheckboxValue(*w, _widgetIndex, value ? 1 : 0);
                Invalidate();
            }
        }
    };

    class ScColourPickerWidget : public ScWidget
    {
    public:
        ScColourPickerWidget(WindowClass c, rct_windownumber n, WidgetIndex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScColourPickerWidget>(ctx);
            dukglue_register_property(ctx, &ScColourPickerWidget::colour_get, &ScColourPickerWidget::colour_set, "colour");
        }

    private:
        colour_t colour_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return GetWidgetColour(w, _widgetIndex);
            }
            return COLOUR_BLACK;
        }
        void colour_set(colour_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                UpdateWidgetColour(w, _widgetIndex, value);
                Invalidate();
            }
        }
    };

    class ScDropdownWidget : public ScWidget
    {
    public:
        ScDropdownWidget(WindowClass c, rct_windownumber n, WidgetIndex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScDropdownWidget>(ctx);
            dukglue_register_property(ctx, &ScDropdownWidget::items_get, &ScDropdownWidget::items_set, "items");
            dukglue_register_property(
                ctx, &ScDropdownWidget::selectedIndex_get, &ScDropdownWidget::selectedIndex_set, "selectedIndex");
            // Explicit template due to text being a base method
            dukglue_register_property<ScDropdownWidget, std::string, std::string>(
                ctx, &ScDropdownWidget::text_get, &ScDropdownWidget::text_set, "text");
        }

    private:
        int32_t selectedIndex_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return GetWidgetSelectedIndex(w, _widgetIndex);
            }
            return -1;
        }
        void selectedIndex_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                UpdateWidgetSelectedIndex(w, _widgetIndex, value);
            }
        }

        std::vector<std::string> items_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return GetWidgetItems(w, _widgetIndex);
            }
            return {};
        }

        void items_set(const std::vector<std::string>& value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                UpdateWidgetItems(w, _widgetIndex, value);
            }
        }
    };

    class ScGroupBoxWidget : public ScWidget
    {
    public:
        ScGroupBoxWidget(WindowClass c, rct_windownumber n, WidgetIndex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScGroupBoxWidget>(ctx);
            // Explicit template due to text being a base method
            dukglue_register_property<ScGroupBoxWidget, std::string, std::string>(
                ctx, &ScGroupBoxWidget::text_get, &ScGroupBoxWidget::text_set, "text");
        }
    };

    class ScLabelWidget : public ScWidget
    {
    public:
        ScLabelWidget(WindowClass c, rct_windownumber n, WidgetIndex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScLabelWidget>(ctx);
            // Explicit template due to text being a base method
            dukglue_register_property<ScLabelWidget, std::string, std::string>(
                ctx, &ScLabelWidget::text_get, &ScLabelWidget::text_set, "text");
            dukglue_register_property(ctx, &ScLabelWidget::textAlign_get, &ScLabelWidget::textAlign_set, "textAlign");
        }

    private:
        std::string textAlign_get() const
        {
            auto* widget = GetWidget();
            if (widget != nullptr)
            {
                if (widget->type == WindowWidgetType::LabelCentred)
                {
                    return "centred";
                }
            }
            return "left";
        }

        void textAlign_set(const std::string& value)
        {
            auto* widget = GetWidget();
            if (widget != nullptr)
            {
                if (value == "centred")
                    widget->type = WindowWidgetType::LabelCentred;
                else
                    widget->type = WindowWidgetType::Label;
            }
        }
    };

    class ScListViewWidget : public ScWidget
    {
    public:
        ScListViewWidget(WindowClass c, rct_windownumber n, WidgetIndex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScListViewWidget>(ctx);
            dukglue_register_property(ctx, &ScListViewWidget::canSelect_get, &ScListViewWidget::canSelect_set, "canSelect");
            dukglue_register_property(ctx, &ScListViewWidget::isStriped_get, &ScListViewWidget::isStriped_set, "isStriped");
            dukglue_register_property(ctx, &ScListViewWidget::scrollbars_get, &ScListViewWidget::scrollbars_set, "scrollbars");
            dukglue_register_property(
                ctx, &ScListViewWidget::showColumnHeaders_get, &ScListViewWidget::showColumnHeaders_set, "showColumnHeaders");
            dukglue_register_property(ctx, &ScListViewWidget::highlightedCell_get, nullptr, "highlightedCell");
            dukglue_register_property(
                ctx, &ScListViewWidget::selectedCell_get, &ScListViewWidget::selectedCell_set, "selectedCell");
            dukglue_register_property(ctx, &ScListViewWidget::columns_get, &ScListViewWidget::columns_set, "columns");
            dukglue_register_property(ctx, &ScListViewWidget::items_get, &ScListViewWidget::items_set, "items");
        }

    private:
        bool canSelect_get() const
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                return listView->CanSelect;
            }
            return false;
        }

        void canSelect_set(bool value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->CanSelect = value;
            }
        }

        bool isStriped_get() const
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                return listView->IsStriped;
            }
            return false;
        }

        void isStriped_set(bool value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->IsStriped = value;
            }
        }

        DukValue scrollbars_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto scrollType = ScrollbarType::None;
            auto listView = GetListView();
            if (listView != nullptr)
            {
                scrollType = listView->GetScrollbars();
            }
            return ToDuk(ctx, scrollType);
        }

        void scrollbars_set(const DukValue& value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->SetScrollbars(FromDuk<ScrollbarType>(value));
            }
        }

        bool showColumnHeaders_get() const
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                return listView->ShowColumnHeaders;
            }
            return false;
        }

        void showColumnHeaders_set(bool value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->ShowColumnHeaders = value;
            }
        }

        DukValue highlightedCell_get()
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto listView = GetListView();
            if (listView != nullptr)
            {
                return ToDuk(ctx, listView->LastHighlightedCell);
            }
            return ToDuk(ctx, nullptr);
        }

        DukValue selectedCell_get()
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto listView = GetListView();
            if (listView != nullptr)
            {
                return ToDuk(ctx, listView->SelectedCell);
            }
            return ToDuk(ctx, nullptr);
        }

        void selectedCell_set(const DukValue& value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->SelectedCell = FromDuk<std::optional<RowColumn>>(value);
            }
        }

        std::vector<std::vector<std::string>> items_get()
        {
            std::vector<std::vector<std::string>> result;
            auto listView = GetListView();
            if (listView != nullptr)
            {
                for (const auto& item : listView->GetItems())
                {
                    result.push_back(item.Cells);
                }
            }
            return result;
        }

        void items_set(const DukValue& value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->SetItems(FromDuk<std::vector<ListViewItem>>(value));
            }
        }

        std::vector<DukValue> columns_get()
        {
            std::vector<DukValue> result;
            auto listView = GetListView();
            if (listView != nullptr)
            {
                auto ctx = GetContext()->GetScriptEngine().GetContext();
                for (const auto& column : listView->GetColumns())
                {
                    result.push_back(ToDuk(ctx, column));
                }
            }
            return result;
        }

        void columns_set(const DukValue& value)
        {
            auto listView = GetListView();
            if (listView != nullptr)
            {
                listView->SetColumns(FromDuk<std::vector<ListViewColumn>>(value));
            }
        }

        CustomListView* GetListView() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return GetCustomListView(w, _widgetIndex);
            }
            return nullptr;
        }
    };

    class ScSpinnerWidget : public ScWidget
    {
    public:
        ScSpinnerWidget(WindowClass c, rct_windownumber n, WidgetIndex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScSpinnerWidget>(ctx);
            // Explicit template due to text being a base method
            dukglue_register_property<ScSpinnerWidget, std::string, std::string>(
                ctx, &ScSpinnerWidget::text_get, &ScSpinnerWidget::text_set, "text");
        }
    };

    class ScTextBoxWidget : public ScWidget
    {
    public:
        ScTextBoxWidget(WindowClass c, rct_windownumber n, WidgetIndex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScTextBoxWidget>(ctx);
            dukglue_register_property(ctx, &ScTextBoxWidget::maxLength_get, &ScTextBoxWidget::maxLength_set, "maxLength");
            // Explicit template due to text being a base method
            dukglue_register_property<ScTextBoxWidget, std::string, std::string>(
                ctx, &ScTextBoxWidget::text_get, &ScTextBoxWidget::text_set, "text");
        }

    private:
        int32_t maxLength_get() const
        {
            auto w = GetWindow();
            if (w != nullptr && IsCustomWindow())
            {
                return OpenRCT2::Ui::Windows::GetWidgetMaxLength(w, _widgetIndex);
            }
            return 0;
        }

        void maxLength_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr && IsCustomWindow())
            {
                OpenRCT2::Ui::Windows::SetWidgetMaxLength(w, _widgetIndex, value);
            }
        }
    };

    class ScViewportWidget : public ScWidget
    {
    public:
        ScViewportWidget(WindowClass c, rct_windownumber n, WidgetIndex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScViewportWidget>(ctx);
            dukglue_register_property(ctx, &ScViewportWidget::viewport_get, nullptr, "viewport");
        }

    private:
        std::shared_ptr<ScViewport> viewport_get() const
        {
            auto w = GetWindow();
            if (w != nullptr && IsCustomWindow())
            {
                auto widget = GetWidget();
                if (widget != nullptr && widget->type == WindowWidgetType::Viewport)
                {
                    return std::make_shared<ScViewport>(w->classification, w->number);
                }
            }
            return {};
        }
    };

    inline DukValue ScWidget::ToDukValue(duk_context* ctx, rct_window* w, WidgetIndex widgetIndex)
    {
        const auto& widget = w->widgets[widgetIndex];
        auto c = w->classification;
        auto n = w->number;
        switch (widget.type)
        {
            case WindowWidgetType::Button:
            case WindowWidgetType::FlatBtn:
            case WindowWidgetType::ImgBtn:
                return GetObjectAsDukValue(ctx, std::make_shared<ScButtonWidget>(c, n, widgetIndex));
            case WindowWidgetType::Checkbox:
                return GetObjectAsDukValue(ctx, std::make_shared<ScCheckBoxWidget>(c, n, widgetIndex));
            case WindowWidgetType::ColourBtn:
                return GetObjectAsDukValue(ctx, std::make_shared<ScColourPickerWidget>(c, n, widgetIndex));
            case WindowWidgetType::DropdownMenu:
                return GetObjectAsDukValue(ctx, std::make_shared<ScDropdownWidget>(c, n, widgetIndex));
            case WindowWidgetType::Groupbox:
                return GetObjectAsDukValue(ctx, std::make_shared<ScGroupBoxWidget>(c, n, widgetIndex));
            case WindowWidgetType::Label:
            case WindowWidgetType::LabelCentred:
                return GetObjectAsDukValue(ctx, std::make_shared<ScLabelWidget>(c, n, widgetIndex));
            case WindowWidgetType::Scroll:
                return GetObjectAsDukValue(ctx, std::make_shared<ScListViewWidget>(c, n, widgetIndex));
            case WindowWidgetType::Spinner:
                return GetObjectAsDukValue(ctx, std::make_shared<ScSpinnerWidget>(c, n, widgetIndex));
            case WindowWidgetType::TextBox:
                return GetObjectAsDukValue(ctx, std::make_shared<ScTextBoxWidget>(c, n, widgetIndex));
            case WindowWidgetType::Viewport:
                return GetObjectAsDukValue(ctx, std::make_shared<ScViewportWidget>(c, n, widgetIndex));
            default:
                return GetObjectAsDukValue(ctx, std::make_shared<ScWidget>(c, n, widgetIndex));
        }
    }

} // namespace OpenRCT2::Scripting

#endif
