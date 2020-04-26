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
    private:
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

    private:
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
                Invalidate();
                widget->left = value;
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
                Invalidate();
                widget->top = value;
                Invalidate();
            }
        }

        int32_t width_get() const
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                return widget->right - widget->left;
            }
            return 0;
        }
        void width_set(int32_t value)
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                Invalidate();
                widget->right = widget->left + value;
                Invalidate();
            }
        }

        int32_t height_get() const
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                return widget->bottom - widget->top;
            }
            return 0;
        }
        void height_set(int32_t value)
        {
            auto widget = GetWidget();
            if (widget != nullptr)
            {
                Invalidate();
                widget->bottom = widget->top + value;
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
            }
        }

        uint32_t image_get() const
        {
            if (IsCustomWindow())
            {
                auto widget = GetWidget();
                if (widget != nullptr && widget->type == WWT_FLATBTN)
                {
                    return widget->image;
                }
            }
            return 0;
        }
        void image_set(uint32_t value)
        {
            auto widget = GetWidget();
            if (widget != nullptr && widget->type == WWT_FLATBTN)
            {
                widget->image = value;
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
            dukglue_register_property(ctx, &ScWidget::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScWidget::x_get, &ScWidget::x_set, "x");
            dukglue_register_property(ctx, &ScWidget::y_get, &ScWidget::y_set, "y");
            dukglue_register_property(ctx, &ScWidget::width_get, &ScWidget::width_set, "width");
            dukglue_register_property(ctx, &ScWidget::height_get, &ScWidget::height_set, "height");
            dukglue_register_property(ctx, &ScWidget::isDisabled_get, &ScWidget::isDisabled_set, "isDisabled");

            // No so common
            dukglue_register_property(ctx, &ScWidget::image_get, &ScWidget::image_set, "image");
            dukglue_register_property(ctx, &ScWidget::text_get, &ScWidget::text_set, "text");
            dukglue_register_property(ctx, &ScWidget::isChecked_get, &ScWidget::isChecked_set, "isChecked");
            dukglue_register_property(ctx, &ScWidget::viewport_get, nullptr, "viewport");
        }

    private:
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
} // namespace OpenRCT2::Scripting

#endif
