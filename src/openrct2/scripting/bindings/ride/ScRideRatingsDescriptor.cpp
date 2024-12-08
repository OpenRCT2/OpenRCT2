/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScRideRatingsDescriptor.hpp"

    #include "../../../Context.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"
    #include "../object/ScObject.hpp"

namespace OpenRCT2::Scripting
{
    ScRideRatingsDescriptor::ScRideRatingsDescriptor(RideRatingsDescriptor rideRatingsDescriptor)
        : _rideRatingsDescriptor(rideRatingsDescriptor)
    {
    }

    uint8_t ScRideRatingsDescriptor::type_get() const
    {
        return EnumValue(_rideRatingsDescriptor.Type);
    }

    uint8_t ScRideRatingsDescriptor::unreliability_get() const
    {
        return _rideRatingsDescriptor.Unreliability;
    }

    int8_t ScRideRatingsDescriptor::rideShelter_get() const
    {
        return _rideRatingsDescriptor.RideShelter;
    }

    bool ScRideRatingsDescriptor::relaxRequirementsIfInversions_get() const
    {
        return _rideRatingsDescriptor.RelaxRequirementsIfInversions;
    }

    std::vector<std::shared_ptr<ScRatingsModifier>> ScRideRatingsDescriptor::modifiers_get() const
    {
        std::vector<std::shared_ptr<ScRatingsModifier>> modifiers;
        for (const auto& modifier : _rideRatingsDescriptor.Modifiers)
        {
            if (modifier.type == RatingsModifierType::NoModifier)
            {
                break;
            }

            modifiers.push_back(std::make_shared<ScRatingsModifier>(modifier));
        }
        return modifiers;
    }

    void ScRideRatingsDescriptor::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScRideRatingsDescriptor::type_get, nullptr, "type");
        dukglue_register_property(ctx, &ScRideRatingsDescriptor::unreliability_get, nullptr, "unreliability");
        dukglue_register_property(ctx, &ScRideRatingsDescriptor::rideShelter_get, nullptr, "rideShelter");
        dukglue_register_property(
            ctx, &ScRideRatingsDescriptor::relaxRequirementsIfInversions_get, nullptr, "relaxRequirementsIfInversions");
        dukglue_register_property(ctx, &ScRideRatingsDescriptor::modifiers_get, nullptr, "modifiers");
    }
} // namespace OpenRCT2::Scripting

#endif // ENABLE_SCRIPTING
