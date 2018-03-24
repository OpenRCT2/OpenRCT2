#pragma once

#include <algorithm>
#include <dukglue/dukglue.h>
#include <string>
#include "../common.h"
#include "../Context.h"
#include "ScWindow.hpp"

namespace OpenRCT2::Scripting
{
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
