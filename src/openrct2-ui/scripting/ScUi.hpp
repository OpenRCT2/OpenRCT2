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

#    include "CustomMenu.h"
#    include "ScTileSelection.hpp"
#    include "ScViewport.hpp"
#    include "ScWindow.hpp"

#    include <algorithm>
#    include <memory>
#    include <openrct2/Context.h>
#    include <openrct2/Input.h>
#    include <openrct2/common.h>
#    include <openrct2/scripting/Duktape.hpp>
#    include <openrct2/scripting/ScriptEngine.h>
#    include <string>

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
    class ScTool
    {
    private:
        duk_context* _ctx{};

    public:
        ScTool(duk_context* ctx)
            : _ctx(ctx)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScTool::id_get, nullptr, "id");
            dukglue_register_property(ctx, &ScTool::cursor_get, nullptr, "cursor");
            dukglue_register_method(ctx, &ScTool::cancel, "cancel");
        }

    private:
        std::string id_get() const
        {
            return ActiveCustomTool ? ActiveCustomTool->Id : "";
        }

        DukValue cursor_get() const
        {
            return ToDuk(_ctx, static_cast<CURSOR_ID>(gCurrentToolId));
        }

        void cancel()
        {
            tool_cancel();
        }
    };

    class ScUi
    {
    private:
        ScriptEngine& _scriptEngine;

    public:
        ScUi(ScriptEngine& scriptEngine)
            : _scriptEngine(scriptEngine)
        {
        }

    private:
        int32_t width_get() const
        {
            return context_get_width();
        }
        int32_t height_get() const
        {
            return context_get_height();
        }
        int32_t windows_get() const
        {
            return static_cast<int32_t>(g_window_list.size());
        }

        std::shared_ptr<ScViewport> mainViewport_get() const
        {
            return std::make_shared<ScViewport>(WC_MAIN_WINDOW);
        }

        std::shared_ptr<ScTileSelection> tileSelection_get() const
        {
            return std::make_shared<ScTileSelection>(_scriptEngine.GetContext());
        }

        std::shared_ptr<ScTool> tool_get() const
        {
            if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
            {
                return std::make_shared<ScTool>(_scriptEngine.GetContext());
            }
            return {};
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

        std::shared_ptr<ScWindow> getWindow(DukValue a) const
        {
            if (a.type() == DukValue::Type::NUMBER)
            {
                auto index = a.as_int();
                auto i = 0;
                for (auto w : g_window_list)
                {
                    if (i == index)
                    {
                        return std::make_shared<ScWindow>(w.get());
                    }
                    i++;
                }
            }
            else if (a.type() == DukValue::Type::STRING)
            {
                auto classification = a.as_string();
                auto w = FindCustomWindowByClassification(classification);
                if (w != nullptr)
                {
                    return std::make_shared<ScWindow>(w);
                }
            }
            return {};
        }

        void activateTool(const DukValue& desc)
        {
            InitialiseCustomTool(_scriptEngine, desc);
        }

        void registerMenuItem(std::string text, DukValue callback)
        {
            auto& execInfo = _scriptEngine.GetExecInfo();
            auto owner = execInfo.GetCurrentPlugin();
            CustomMenuItems.emplace_back(owner, text, callback);
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScUi::height_get, nullptr, "height");
            dukglue_register_property(ctx, &ScUi::width_get, nullptr, "width");
            dukglue_register_property(ctx, &ScUi::windows_get, nullptr, "windows");
            dukglue_register_property(ctx, &ScUi::mainViewport_get, nullptr, "mainViewport");
            dukglue_register_property(ctx, &ScUi::tileSelection_get, nullptr, "tileSelection");
            dukglue_register_property(ctx, &ScUi::tool_get, nullptr, "tool");
            dukglue_register_method(ctx, &ScUi::openWindow, "openWindow");
            dukglue_register_method(ctx, &ScUi::closeWindows, "closeWindows");
            dukglue_register_method(ctx, &ScUi::closeAllWindows, "closeAllWindows");
            dukglue_register_method(ctx, &ScUi::getWindow, "getWindow");
            dukglue_register_method(ctx, &ScUi::activateTool, "activateTool");
            dukglue_register_method(ctx, &ScUi::registerMenuItem, "registerMenuItem");
        }

    private:
        rct_windowclass GetClassification(const std::string& key) const
        {
            return WC_NULL;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
