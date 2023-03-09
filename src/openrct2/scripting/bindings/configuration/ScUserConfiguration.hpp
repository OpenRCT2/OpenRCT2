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

#    include "../../../Context.h"
#    include "../../../config/Config.h"
#    include "../../../localisation/LocalisationService.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"
#    include "../game/ScConfiguration.hpp"
#    include "ScLocalisation.hpp"

namespace OpenRCT2::Scripting
{
    class ScUserConfiguration : public ScConfiguration
    {
    public:
        ScUserConfiguration()
            : ScConfiguration()
        {
        }
        ScUserConfiguration(ScConfigurationKind kind, const DukValue& backingObject)
            : ScConfiguration(kind,  backingObject)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScUserConfiguration::localisation_get, nullptr, "localisation");
            dukglue_register_method(ctx, &ScUserConfiguration::getAll, "getAll");
            dukglue_register_method(ctx, &ScUserConfiguration::get, "get");
            dukglue_register_method(ctx, &ScUserConfiguration::set, "set");
            dukglue_register_method(ctx, &ScUserConfiguration::has, "has");
        }

    protected:
        DukValue getAll(const DukValue& dukNamespace) const override
        {
            return ScConfiguration::getAll(dukNamespace);
        }
        
        DukValue get(const std::string& key, const DukValue& defaultValue) const override
        {
            return ScConfiguration::get(key, defaultValue);
        }

        void set(const std::string& key, const DukValue& value) const override
        {
            ScConfiguration::set(key, value);
        }

        bool has(const std::string& key) const override
        {
            return ScConfiguration::has(key);
        }

    private:
        std::shared_ptr<ScLocalisation> localisation_get() const
        {
            return std::make_shared<ScLocalisation>();
        }
    };
} // namespace OpenRCT2::Scripting

#endif
