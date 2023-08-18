/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"
#    include "../game/ScContext.hpp"

namespace OpenRCT2::Scripting
{
    class ScPlugin
    {
    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_method(ctx, &ScPlugin::getPlugins, "getPlugins");
        }

    private:
        std::vector<DukValue> getPlugins()
        {
            // Get all of the plugins from the script engine
            OpenRCT2::Scripting::ScriptEngine& scriptEngine = GetContext()->GetScriptEngine();
            const auto& allPlugins = scriptEngine.GetPlugins();
            auto ctx = scriptEngine.GetContext();
            std::vector<DukValue> result;
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
                for (auto [s, i] = std::tuple{ metadata.Authors.begin(), 0 }; s != metadata.Authors.end(); s++, i++)
                {
                    auto& str = *s;
                    duk_push_string(ctx, str.c_str());
                    duk_put_prop_index(ctx, arrIdx, i);
                }
                duk_put_prop_string(ctx, dukIdx, "authors");
                // Take from Duk stack
                result.push_back(DukValue::take_from_stack(ctx, dukIdx));
                dukIdx = DUK_INVALID_INDEX;
            }
            return result;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
