/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#    include <openrct2/scripting/ScriptEngine.h>

namespace OpenRCT2::Scripting
{
    class ScWidget
    {
    protected:
        rct_windowclass _class{};
        rct_windownumber _number{};
        rct_widgetindex _widgetIndex{};

    public:
        ScWidget(rct_windowclass c, rct_windownumber n, rct_widgetindex widgetIndex)
            : _class(c)
            , _number(n)
            , _widgetIndex(widgetIndex)
        {
        }

        static DukValue ToDukValue(duk_context* ctx, rct_window* w, rct_widgetindex widgetIndex);

    private:
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
                    case WWT_FRAME:
                        return "frame";
                    case WWT_RESIZE:
                        return "resize";
                    case WWT_IMGBTN:
                    case WWT_COLOURBTN:
                    case WWT_TRNBTN:
                    case WWT_FLATBTN:
                    case WWT_BUTTON:
                    case WWT_CLOSEBOX:
                        return "button";
                    case WWT_TAB:
                        return "tab";
                    case WWT_LABEL_CENTRED:
                    case WWT_LABEL:
                        return "label";
                    case WWT_TABLE_HEADER:
                        return "table_header";
                    case WWT_SPINNER:
                        return "spinner";
                    case WWT_DROPDOWN:
                        return "dropdown";
                    case WWT_VIEWPORT:
                        return "viewport";
                    case WWT_GROUPBOX:
                        return "groupbox";
                    case WWT_CAPTION:
                        return "caption";
                    case WWT_SCROLL:
                        return "scroll_view";
                    case WWT_CHECKBOX:
                        return "checkbox";
                    case WWT_TEXT_BOX:
                        return "textbox";
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

                if (widget->type == WWT_DROPDOWN)
                {
                    auto buttonWidget = widget + 1;
                    buttonWidget->left += delta;
                    buttonWidget->right += delta;
                    widget_invalidate_by_number(_class, _number, _widgetIndex + 1);
                }
                else if (widget->type == WWT_SPINNER)
                {
                    auto upWidget = widget + 1;
                    upWidget->left += delta;
                    upWidget->right += delta;
                    widget_invalidate_by_number(_class, _number, _widgetIndex + 1);

                    auto downWidget = widget + 2;
                    downWidget->left += delta;
                    downWidget->right += delta;
                    widget_invalidate_by_number(_class, _number, _widgetIndex + 2);
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

                if (widget->type == WWT_DROPDOWN)
                {
                    auto buttonWidget = widget + 1;
                    buttonWidget->top += delta;
                    buttonWidget->bottom += delta;
                    widget_invalidate_by_number(_class, _number, _widgetIndex + 1);
                }
                else if (widget->type == WWT_SPINNER)
                {
                    auto upWidget = widget + 1;
                    upWidget->top += delta;
                    upWidget->bottom += delta;
                    widget_invalidate_by_number(_class, _number, _widgetIndex + 1);

                    auto downWidget = widget + 2;
                    downWidget->top += delta;
                    downWidget->bottom += delta;
                    widget_invalidate_by_number(_class, _number, _widgetIndex + 2);
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

                if (widget->type == WWT_DROPDOWN)
                {
                    auto buttonWidget = widget + 1;
                    buttonWidget->left += delta;
                    buttonWidget->right += delta;
                    widget_invalidate_by_number(_class, _number, _widgetIndex + 1);
                }
                else if (widget->type == WWT_SPINNER)
                {
                    auto upWidget = widget + 1;
                    upWidget->left += delta;
                    upWidget->right += delta;
                    widget_invalidate_by_number(_class, _number, _widgetIndex + 1);

                    auto downWidget = widget + 2;
                    downWidget->left += delta;
                    downWidget->right += delta;
                    widget_invalidate_by_number(_class, _number, _widgetIndex + 2);
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

                if (widget->type == WWT_DROPDOWN)
                {
                    auto buttonWidget = widget + 1;
                    buttonWidget->bottom += delta;
                    widget_invalidate_by_number(_class, _number, _widgetIndex + 1);
                }
                else if (widget->type == WWT_SPINNER)
                {
                    auto upWidget = widget + 1;
                    upWidget->bottom += delta;
                    widget_invalidate_by_number(_class, _number, _widgetIndex + 1);

                    auto downWidget = widget + 2;
                    downWidget->bottom += delta;
                    widget_invalidate_by_number(_class, _number, _widgetIndex + 2);
                }

                Invalidate();
            }
        }

        bool isDisabled_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return widget_is_disabled(w, _widgetIndex);
            }
            return false;
        }
        void isDisabled_set(bool value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                auto mask = 1ULL << _widgetIndex;
                if (value)
                    w->disabled_widgets |= mask;
                else
                    w->disabled_widgets &= ~mask;
            }
        }

        std::string text_get() const
        {
            if (IsCustomWindow())
            {
                auto widget = GetWidget();
                if (widget != nullptr && (widget->flags & WIDGET_FLAGS::TEXT_IS_STRING) && widget->string != nullptr)
                {
                    return language_convert_string_to_tokens(widget->string);
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

        std::shared_ptr<ScViewport> viewport_get() const
        {
            auto w = GetWindow();
            if (w != nullptr && IsCustomWindow())
            {
                auto widget = GetWidget();
                if (widget != nullptr && widget->type == WWT_VIEWPORT)
                {
                    return std::make_shared<ScViewport>(w->classification, w->number);
                }
            }
            return {};
        }

    public:
        static void Register(duk_context* ctx)
        {
            // Common
            dukglue_register_property(ctx, &ScWidget::name_get, &ScWidget::name_set, "name");
            dukglue_register_property(ctx, &ScWidget::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScWidget::x_get, &ScWidget::x_set, "x");
            dukglue_register_property(ctx, &ScWidget::y_get, &ScWidget::y_set, "y");
            dukglue_register_property(ctx, &ScWidget::width_get, &ScWidget::width_set, "width");
            dukglue_register_property(ctx, &ScWidget::height_get, &ScWidget::height_set, "height");
            dukglue_register_property(ctx, &ScWidget::isDisabled_get, &ScWidget::isDisabled_set, "isDisabled");

            // No so common
            dukglue_register_property(ctx, &ScWidget::text_get, &ScWidget::text_set, "text");
            dukglue_register_property(ctx, &ScWidget::viewport_get, nullptr, "viewport");
        }

    protected:
        rct_window* GetWindow() const
        {
            if (_class == WC_MAIN_WINDOW)
                return window_get_main();
            else
                return window_find_by_number(_class, _number);
        }

        rct_widget* GetWidget() const
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
                return w->classification == WC_CUSTOM;
            }
            return false;
        }

        void Invalidate()
        {
            widget_invalidate_by_number(_class, _number, _widgetIndex);
        }
    };

    class ScButtonWidget : public ScWidget
    {
    public:
        ScButtonWidget(rct_windowclass c, rct_windownumber n, rct_widgetindex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScButtonWidget>(ctx);
            dukglue_register_property(ctx, &ScButtonWidget::border_get, &ScButtonWidget::border_set, "border");
            dukglue_register_property(ctx, &ScButtonWidget::isPressed_get, &ScButtonWidget::isPressed_set, "isPressed");
            dukglue_register_property(ctx, &ScButtonWidget::image_get, &ScButtonWidget::image_set, "image");
        }

    private:
        bool border_get() const
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                return widget->type == WWT_IMGBTN;
            }
            return false;
        }
        void border_set(bool value)
        {
            auto widget = GetWidget();
            if (widget != nullptr && (widget->type == WWT_FLATBTN || widget->type == WWT_IMGBTN))
            {
                if (value)
                    widget->type = WWT_IMGBTN;
                else
                    widget->type = WWT_FLATBTN;
                Invalidate();
            }
        }

        bool isPressed_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return widget_is_pressed(w, _widgetIndex);
            }
            return false;
        }
        void isPressed_set(bool value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                widget_set_checkbox_value(w, _widgetIndex, value ? 1 : 0);
                Invalidate();
            }
        }

        uint32_t image_get() const
        {
            auto widget = GetWidget();
            if (widget != nullptr && widget->type == WWT_FLATBTN)
            {
                return widget->image;
            }
            return 0;
        }
        void image_set(uint32_t value)
        {
            auto widget = GetWidget();
            if (widget != nullptr && widget->type == WWT_FLATBTN)
            {
                widget->image = value;
                Invalidate();
            }
        }
    };

    class ScCheckBoxWidget : public ScWidget
    {
    public:
        ScCheckBoxWidget(rct_windowclass c, rct_windownumber n, rct_widgetindex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScCheckBoxWidget>(ctx);
            dukglue_register_property(ctx, &ScCheckBoxWidget::isChecked_get, &ScCheckBoxWidget::isChecked_set, "isChecked");
        }

    private:
        bool isChecked_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return widget_is_pressed(w, _widgetIndex);
            }
            return false;
        }
        void isChecked_set(bool value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                widget_set_checkbox_value(w, _widgetIndex, value ? 1 : 0);
                Invalidate();
            }
        }
    };

    class ScDropdownWidget : public ScWidget
    {
    public:
        ScDropdownWidget(rct_windowclass c, rct_windownumber n, rct_widgetindex widgetIndex)
            : ScWidget(c, n, widgetIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScWidget, ScDropdownWidget>(ctx);
            dukglue_register_property(ctx, &ScDropdownWidget::items_get, &ScDropdownWidget::items_set, "items");
            dukglue_register_property(
                ctx, &ScDropdownWidget::selectedIndex_get, &ScDropdownWidget::selectedIndex_set, "selectedIndex");
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

    class ScListViewWidget : public ScWidget
    {
    public:
        ScListViewWidget(rct_windowclass c, rct_windownumber n, rct_widgetindex widgetIndex)
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

    inline DukValue ScWidget::ToDukValue(duk_context* ctx, rct_window* w, rct_widgetindex widgetIndex)
    {
        const auto& widget = w->widgets[widgetIndex];
        auto c = w->classification;
        auto n = w->number;
        switch (widget.type)
        {
            case WWT_BUTTON:
            case WWT_FLATBTN:
            case WWT_IMGBTN:
                return GetObjectAsDukValue(ctx, std::make_shared<ScButtonWidget>(c, n, widgetIndex));
            case WWT_CHECKBOX:
                return GetObjectAsDukValue(ctx, std::make_shared<ScCheckBoxWidget>(c, n, widgetIndex));
            case WWT_DROPDOWN:
                return GetObjectAsDukValue(ctx, std::make_shared<ScDropdownWidget>(c, n, widgetIndex));
            case WWT_SCROLL:
                return GetObjectAsDukValue(ctx, std::make_shared<ScListViewWidget>(c, n, widgetIndex));
            default:
                return GetObjectAsDukValue(ctx, std::make_shared<ScWidget>(c, n, widgetIndex));
        }
    }

} // namespace OpenRCT2::Scripting

#endif
