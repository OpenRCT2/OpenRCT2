/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../common.h"
#include "ScWindow.hpp"

#include <dukglue/dukglue.h>
#include <memory>
#include <openrct2/scripting/ScriptEngine.h>
#include <string>

namespace OpenRCT2::Scripting
{
    class Plugin;
}

namespace OpenRCT2::Ui::Windows
{
    rct_window* window_custom_open(std::shared_ptr<OpenRCT2::Scripting::Plugin> owner, DukValue dukDesc);
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

        int32_t width_get()
        {
            return context_get_width();
        }
        int32_t height_get()
        {
            return context_get_height();
        }
        int32_t windows_get()
        {
            return static_cast<int32_t>(g_window_list.size());
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

        std::shared_ptr<ScWindow> getWindow(int32_t index)
        {
            for (int32_t i = 0; i < g_window_list.size(); i++)
            {
                if (i == index)
                {
                    auto w = g_window_list[i].get();
                    return std::make_shared<ScWindow>(w);
                }
                i++;
            }
            return nullptr;
        }

        static void Register(duk_context* ctx)
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
} // namespace OpenRCT2::Scripting
