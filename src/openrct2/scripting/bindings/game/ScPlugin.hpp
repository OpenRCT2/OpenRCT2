/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../../ScriptEngine.h"
    #include "../game/ScContext.hpp"

namespace OpenRCT2::Scripting
{
    class ScPlugin;
    extern ScPlugin gScPlugin;
    class ScPlugin final : public ScBase
    {
    private:
        static const std::vector<std::shared_ptr<Plugin>> getallPlugins()
        {
            // Get all of the plugins from the script engine
            ScriptEngine& scriptEngine = GetContext()->GetScriptEngine();
            return scriptEngine.GetPlugins();
        }

        static JSValue plugins_get(JSContext* ctx, JSValue)
        {
            auto& allPlugins = getallPlugins();
            return formatMetadata(ctx, allPlugins);
        }

        static JSValue formatMetadata(
            JSContext* ctx, const std::vector<std::shared_ptr<OpenRCT2::Scripting::Plugin>>& allPlugins)
        {
            JSValue formattedMetadata = JS_NewArray(ctx);
            // Iterate through all plugins and and cast their data to JSValue objects
            int64_t index = 0;
            for (const auto& pluginPtr : allPlugins)
            {
                // Pull out metadata
                Plugin& plugin = *pluginPtr;
                PluginMetadata metadata = plugin.GetMetadata();
                // Create object using context
                JSValue val = JS_NewObject(ctx);
                // Name and Version
                JS_SetPropertyStr(ctx, val, "name", JSFromStdString(ctx, metadata.Name));
                JS_SetPropertyStr(ctx, val, "version", JSFromStdString(ctx, metadata.Version));
                // Authors
                JSValue authorsArray = JS_NewArray(ctx);

                int64_t idx = 0;
                for (auto& str : metadata.Authors)
                {
                    JSValue authorStr = JSFromStdString(ctx, str);
                    JS_SetPropertyInt64(ctx, authorsArray, idx++, authorStr);
                }
                JS_SetPropertyStr(ctx, val, "authors", authorsArray);
                JS_SetPropertyInt64(ctx, formattedMetadata, index++, val);
            }
            return formattedMetadata;
        }

    public:
        JSValue New(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("plugins", ScPlugin::plugins_get, nullptr),
            };

            return MakeWithOpaque(ctx, funcs, nullptr);
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "PluginManager");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
