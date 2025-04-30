/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"
    #include "../game/ScContext.hpp"

namespace OpenRCT2::Scripting
{
    class ScPlugin
    {
    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScPlugin::plugins_get, nullptr, "plugins");
        }

    private:
        std::vector<DukValue> plugins_get()
        {
            auto ctx = getContext();
            auto& allPlugins = getallPlugins();
            return formatMetadata(ctx, allPlugins);
        }

        duk_context* getContext()
        {
            // Get the context from the script engine
            OpenRCT2::Scripting::ScriptEngine& scriptEngine = GetContext()->GetScriptEngine();
            return scriptEngine.GetContext();
        }

        const std::vector<std::shared_ptr<OpenRCT2::Scripting::Plugin>> getallPlugins()
        {
            // Get all of the plugins from the script engine
            OpenRCT2::Scripting::ScriptEngine& scriptEngine = GetContext()->GetScriptEngine();
            return scriptEngine.GetPlugins();
        }

        const std::vector<DukValue> formatMetadata(
            duk_context* ctx, const std::vector<std::shared_ptr<OpenRCT2::Scripting::Plugin>>& allPlugins)
        {
            std::vector<DukValue> formattedMetadata;
            duk_idx_t dukIdx = DUK_INVALID_INDEX;
            // Iterate through all plugins and and cast their data to Duk objects
            for (const auto& pluginPtr : allPlugins)
            {
                // Pull out metadata
                OpenRCT2::Scripting::Plugin& plugin = *pluginPtr;
                OpenRCT2::Scripting::PluginMetadata metadata = plugin.GetMetadata();
                // Create object using Duk stack
                dukIdx = duk_push_object(ctx);
                // Name and Version
                duk_push_string(ctx, metadata.Name.c_str());
                duk_put_prop_string(ctx, dukIdx, "name");
                duk_push_string(ctx, metadata.Version.c_str());
                duk_put_prop_string(ctx, dukIdx, "version");
                // Authors
                duk_idx_t arrIdx = duk_push_array(ctx);
                for (auto [s, idx] = std::tuple{ metadata.Authors.begin(), 0 }; s != metadata.Authors.end(); s++, idx++)
                {
                    auto& str = *s;
                    duk_push_string(ctx, str.c_str());
                    duk_put_prop_index(ctx, arrIdx, idx);
                }
                duk_put_prop_string(ctx, dukIdx, "authors");
                // Take from Duk stack
                formattedMetadata.push_back(DukValue::take_from_stack(ctx, dukIdx));
                dukIdx = DUK_INVALID_INDEX;
            }
            return formattedMetadata;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
