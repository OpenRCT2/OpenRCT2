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

#    include "ScWidget.hpp"

#    include <openrct2/common.h>
#    include <openrct2/interface/Window.h>
#    include <openrct2/interface/Window_internal.h>
#    include <openrct2/localisation/Language.h>
#    include <openrct2/scripting/Duktape.hpp>

namespace OpenRCT2::Scripting
{
    using namespace OpenRCT2::Ui::Windows;

    class ScWindow
    {
    private:
        rct_windowclass _class;
        rct_windownumber _number;

    public:
        ScWindow(rct_window* w)
            : ScWindow(w->classification, w->number)
        {
        }

        ScWindow(rct_windowclass c, rct_windownumber n)
            : _class(c)
            , _number(n)
        {
        }

        int32_t classification_get() const
        {
            return static_cast<int32_t>(_class);
        }

        int32_t number_get() const
        {
            return static_cast<int32_t>(_number);
        }

        int32_t x_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->windowPos.x;
            }
            return 0;
        }
        void x_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_set_position(*w, { value, w->windowPos.y });
            }
        }
        int32_t y_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->windowPos.y;
            }
            return 0;
        }
        void y_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_set_position(*w, { w->windowPos.x, value });
            }
        }
        int32_t width_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->width;
            }
            return 0;
        }
        void width_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_resize(*w, value - w->width, 0);
            }
        }
        int32_t height_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->height;
            }
            return 0;
        }
        void height_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_resize(*w, 0, value - w->height);
            }
        }
        int32_t minWidth_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->min_width;
            }
            return 0;
        }
        void minWidth_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_set_resize(*w, value, w->min_height, w->max_width, w->max_height);
            }
        }
        int32_t maxWidth_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->max_width;
            }
            return 0;
        }
        void maxWidth_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_set_resize(*w, w->min_width, w->min_height, value, w->max_height);
            }
        }
        int32_t minHeight_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->min_height;
            }
            return 0;
        }
        void minHeight_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_set_resize(*w, w->min_width, value, w->max_width, w->max_height);
            }
        }
        int32_t maxHeight_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return w->max_height;
            }
            return 0;
        }
        void maxHeight_set(int32_t value)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_set_resize(*w, w->min_width, w->min_height, w->max_width, value);
            }
        }
        bool isSticky_get() const
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                return (w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)) != 0;
            }
            return false;
        }

        std::vector<DukValue> widgets_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();

            std::vector<DukValue> result;
            auto w = GetWindow();
            if (w != nullptr)
            {
                rct_widgetindex widgetIndex = 0;
                for (auto widget = w->widgets; widget->type != WindowWidgetType::Last; widget++)
                {
                    result.push_back(ScWidget::ToDukValue(ctx, w, widgetIndex));
                    widgetIndex++;
                }
            }
            return result;
        }

        std::vector<int32_t> colours_get() const
        {
            std::vector<int32_t> result;
            auto w = GetWindow();
            if (w != nullptr)
            {
                result.reserve(std::size(w->colours));
                for (auto c : w->colours)
                {
                    result.push_back(c);
                }
            }
            return result;
        }
        void colours_set(std::vector<int32_t> colours)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                for (size_t i = 0; i < std::size(w->colours); i++)
                {
                    int32_t c = COLOUR_BLACK;
                    if (i < colours.size())
                    {
                        c = std::clamp<int32_t>(BASE_COLOUR(colours[i]), COLOUR_BLACK, COLOUR_COUNT - 1);
                        if (colours[i] & COLOUR_FLAG_TRANSLUCENT)
                        {
                            c = TRANSLUCENT(c);
                        }
                    }
                    w->colours[i] = c;
                }
            }
        }

        std::string title_get() const
        {
            auto w = GetWindow();
            if (w != nullptr && w->classification == WC_CUSTOM)
            {
                return GetWindowTitle(w);
            }
            return {};
        }
        void title_set(std::string value)
        {
            auto w = GetWindow();
            if (w != nullptr && w->classification == WC_CUSTOM)
            {
                UpdateWindowTitle(w, value);
            }
        }

        int32_t tabIndex_get() const
        {
            auto w = GetWindow();
            if (w != nullptr && w->classification == WC_CUSTOM)
            {
                return w->page;
            }
            return 0;
        }
        void tabIndex_set(size_t tab) const
        {
            auto w = GetWindow();
            if (w != nullptr && w->classification == WC_CUSTOM)
            {
                UpdateWindowTab(static_cast<CustomWindow*>(w), tab);
            }
        }

        void close()
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_close(*w);
            }
        }

        DukValue findWidget(std::string name) const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto w = GetWindow();
            if (w != nullptr)
            {
                auto widgetIndex = FindWidgetIndexByName(w, name);
                if (widgetIndex)
                {
                    return ScWidget::ToDukValue(ctx, w, *widgetIndex);
                }
            }
            return GetObjectAsDukValue<ScWidget>(ctx, nullptr);
        }

        void bringToFront()
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_bring_to_front(*w);
                w->flags |= WF_WHITE_BORDER_MASK;
            }
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScWindow::classification_get, nullptr, "classification");
            dukglue_register_property(ctx, &ScWindow::number_get, nullptr, "number");
            dukglue_register_property(ctx, &ScWindow::x_get, &ScWindow::x_set, "x");
            dukglue_register_property(ctx, &ScWindow::y_get, &ScWindow::y_set, "y");
            dukglue_register_property(ctx, &ScWindow::width_get, &ScWindow::width_set, "width");
            dukglue_register_property(ctx, &ScWindow::height_get, &ScWindow::height_set, "height");
            dukglue_register_property(ctx, &ScWindow::minWidth_get, &ScWindow::minWidth_set, "minWidth");
            dukglue_register_property(ctx, &ScWindow::maxWidth_get, &ScWindow::maxWidth_set, "maxWidth");
            dukglue_register_property(ctx, &ScWindow::minHeight_get, &ScWindow::minHeight_set, "minHeight");
            dukglue_register_property(ctx, &ScWindow::maxHeight_get, &ScWindow::maxHeight_set, "maxHeight");
            dukglue_register_property(ctx, &ScWindow::isSticky_get, nullptr, "isSticky");
            dukglue_register_property(ctx, &ScWindow::widgets_get, nullptr, "widgets");
            dukglue_register_property(ctx, &ScWindow::colours_get, &ScWindow::colours_set, "colours");
            dukglue_register_property(ctx, &ScWindow::title_get, &ScWindow::title_set, "title");
            dukglue_register_property(ctx, &ScWindow::tabIndex_get, nullptr, "tabIndex");

            dukglue_register_method(ctx, &ScWindow::close, "close");
            dukglue_register_method(ctx, &ScWindow::findWidget, "findWidget");
            dukglue_register_method(ctx, &ScWindow::bringToFront, "bringToFront");
        }

    private:
        rct_window* GetWindow() const
        {
            return window_find_by_number(_class, _number);
        }
    };
} // namespace OpenRCT2::Scripting

#endif
