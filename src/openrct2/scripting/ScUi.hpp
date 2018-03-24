#pragma once

#include <algorithm>
#include <dukglue/dukglue.h>
#include <string>
#include "../common.h"
#include "../Context.h"
#include "../interface/Window.h"
#include "../interface/Window_internal.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"

namespace OpenRCT2::Scripting
{
    class ScWindow
    {
    private:
        rct_windowclass _class;
        rct_windownumber _number;

    public:
        ScWindow(rct_windowclass c, rct_windownumber n)
            : _class(c),
              _number(n)
        {
        }

        sint32 x_get() { return GetWindow()->x; }
        void x_set(sint32 value)
        {
            auto w = GetWindow();
            window_set_position(w, value, w->y);
        }
        sint32 y_get() { return GetWindow()->y; }
        void y_set(sint32 value)
        {
            auto w = GetWindow();
            window_set_position(w, w->x, value);
        }
        sint32 width_get() { return GetWindow()->width; }
        sint32 height_get() { return GetWindow()->height; }
        bool isSticky_get()
        {
            auto flags = GetWindow()->flags;
            return (flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)) != 0;
        }

        static void Register(duk_context * ctx)
        {
            dukglue_register_property(ctx, &ScWindow::x_get, &ScWindow::x_set, "x");
            dukglue_register_property(ctx, &ScWindow::y_get, &ScWindow::y_set, "y");
            dukglue_register_property(ctx, &ScWindow::width_get, nullptr, "width");
            dukglue_register_property(ctx, &ScWindow::height_get, nullptr, "height");
            dukglue_register_property(ctx, &ScWindow::isSticky_get, nullptr, "isSticky");
        }

    private:
        rct_window * GetWindow() const
        {
            return window_find_by_number(_class, _number);
        }
    };

    class ScUi
    {
    public:
        sint32 width_get() { return context_get_width(); }
        sint32 height_get() { return context_get_height(); }
        sint32 windows_get()
        {
            return gWindowNextSlot - g_window_list;
        }

        std::shared_ptr<ScWindow> openWindow(DukValue desc)
        {
            return nullptr;
        }

        void closeWindows(std::string classification, DukValue id)
        {
            auto cls = GetClassification(classification);
            if (cls != WC_NULL)
            {
                if (id.type() == DukValue::Type::NUMBER)
                {
                    window_close_by_number(cls, id.as_int());
                }
                else
                {
                    window_close_by_class(cls);
                }
            }
        }

        void closeAllWindows()
        {
            window_close_all();
        }

        std::shared_ptr<ScWindow> getWindow(sint32 index)
        {
            auto windowEnd = gWindowNextSlot;
            auto i = 0;
            for (auto w = g_window_list; w < windowEnd; w++)
            {
                if (i == index)
                {
                    return std::make_shared<ScWindow>(w->classification, w->number);
                }
                i++;
            }
            return nullptr;
        }

        static void Register(duk_context * ctx)
        {
            dukglue_register_property(ctx, &ScUi::height_get, nullptr, "height");
            dukglue_register_property(ctx, &ScUi::width_get, nullptr, "width");
            dukglue_register_property(ctx, &ScUi::windows_get, nullptr, "windows");
            dukglue_register_method(ctx, &ScUi::openWindow, "openWindow");
            dukglue_register_method(ctx, &ScUi::closeWindows, "closeWindows");
            dukglue_register_method(ctx, &ScUi::closeAllWindows, "closeAllWindows");
            dukglue_register_method(ctx, &ScUi::getWindow, "getWindow");
        }

    private:
        rct_windowclass GetClassification(const std::string& key)
        {
            return WC_NULL;
        }
    };
}
