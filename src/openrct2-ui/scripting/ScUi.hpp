#pragma once

#include <dukglue/dukglue.h>
#include <memory>
#include <string>
#include <openrct2/scripting/ScriptEngine.h>
#include "../common.h"
#include "../Context.h"
#include "ScWindow.hpp"

namespace OpenRCT2::Scripting
{
    class Plugin;
}

namespace OpenRCT2::Ui::Windows
{
    rct_window * window_custom_open(std::shared_ptr<OpenRCT2::Scripting::Plugin> owner, DukValue dukDesc);
}

namespace OpenRCT2::Scripting
{
    class ScUi
    {
    private:
        ScriptEngine& _scriptEngine;
    public:
        ScUi(ScriptEngine& scriptEngine)
            : _scriptEngine(scriptEngine)
        {
        }

        sint32 width_get() { return context_get_width(); }
        sint32 height_get() { return context_get_height(); }
        sint32 windows_get()
        {
            return gWindowNextSlot - g_window_list;
        }

        std::shared_ptr<ScWindow> openWindow(DukValue desc)
        {
            using namespace OpenRCT2::Ui::Windows;

            auto& execInfo = _scriptEngine.GetExecInfo();
            auto owner = execInfo.GetCurrentPlugin();

            std::shared_ptr<ScWindow> scWindow = nullptr;
            auto w = window_custom_open(owner, desc);
            if (w != nullptr)
            {
                scWindow = std::make_shared<ScWindow>(w);
            }
            return scWindow;
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
                    return std::make_shared<ScWindow>(w);
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
