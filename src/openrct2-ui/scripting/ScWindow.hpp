/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef __ENABLE_SCRIPTING__

#    include "ScWidget.hpp"

#    include <openrct2/common.h>
#    include <openrct2/interface/Window.h>
#    include <openrct2/interface/Window_internal.h>
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

        int32_t x_get()
        {
            return GetWindow()->x;
        }
        void x_set(int32_t value)
        {
            auto w = GetWindow();
            window_set_position(w, { value, w->y });
        }
        int32_t y_get()
        {
            return GetWindow()->y;
        }
        void y_set(int32_t value)
        {
            auto w = GetWindow();
            window_set_position(w, { w->x, value });
        }
        int32_t width_get()
        {
            return GetWindow()->width;
        }
        int32_t height_get()
        {
            return GetWindow()->height;
        }
        bool isSticky_get()
        {
            auto flags = GetWindow()->flags;
            return (flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)) != 0;
        }

        std::vector<std::shared_ptr<ScWidget>> widgets_get()
        {
            std::vector<std::shared_ptr<ScWidget>> result;
            auto w = GetWindow();
            if (w != nullptr)
            {
                rct_widgetindex widgetIndex = 0;
                for (auto widget = w->widgets; widget->type != WWT_LAST; widget++)
                {
                    result.push_back(std::make_shared<ScWidget>(_class, _number, widgetIndex));
                    widgetIndex++;
                }
            }
            return result;
        }

        void close()
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_close(w);
            }
        }

        std::shared_ptr<ScWidget> findWidget(std::string name)
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                auto widgetIndex = FindWidgetIndexByName(w, name);
                if (widgetIndex)
                {
                    return std::make_shared<ScWidget>(_class, _number, *widgetIndex);
                }
            }
            return {};
        }

        void bringToFront()
        {
            auto w = GetWindow();
            if (w != nullptr)
            {
                window_bring_to_front(w);
                w->flags |= WF_WHITE_BORDER_MASK;
            }
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScWindow::x_get, &ScWindow::x_set, "x");
            dukglue_register_property(ctx, &ScWindow::y_get, &ScWindow::y_set, "y");
            dukglue_register_property(ctx, &ScWindow::width_get, nullptr, "width");
            dukglue_register_property(ctx, &ScWindow::height_get, nullptr, "height");
            dukglue_register_property(ctx, &ScWindow::isSticky_get, nullptr, "isSticky");
            dukglue_register_property(ctx, &ScWindow::widgets_get, nullptr, "widgets");

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
