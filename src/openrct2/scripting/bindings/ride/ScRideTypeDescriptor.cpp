/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScRideTypeDescriptor.hpp"

    #include "../../../Context.h"
    #include "../../../ride/RideData.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"
    #include "../object/ScObject.hpp"

namespace OpenRCT2::Scripting
{
    ScRideTypeDescriptor::ScRideTypeDescriptor(RideTypeDescriptor rideTypeDescriptor)
        : _rideTypeDescriptor(rideTypeDescriptor)
    {
    }

    uint8_t ScRideTypeDescriptor::category_get() const
    {
        return _rideTypeDescriptor.Category;
    }

    uint16_t ScRideTypeDescriptor::startTrackPiece_get() const
    {
        return EnumValue(_rideTypeDescriptor.StartTrackPiece);
    }

    uint8_t ScRideTypeDescriptor::defaultMode_get() const
    {
        return EnumValue(_rideTypeDescriptor.DefaultMode);
    }

    uint8_t ScRideTypeDescriptor::availableBreakdowns_get() const
    {
        return _rideTypeDescriptor.AvailableBreakdowns;
    }

    uint8_t ScRideTypeDescriptor::maxMass_get() const
    {
        return _rideTypeDescriptor.MaxMass;
    }

    uint8_t ScRideTypeDescriptor::bonusValue_get() const
    {
        return _rideTypeDescriptor.BonusValue;
    }

    std::string ScRideTypeDescriptor::name_get() const
    {
        return std::string(_rideTypeDescriptor.Name);
    }

    std::shared_ptr<ScRideRatingsDescriptor> ScRideTypeDescriptor::ratingsData_get()
    {
        return std::make_shared<ScRideRatingsDescriptor>(_rideTypeDescriptor.RatingsData);
    }

    void ScRideTypeDescriptor::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScRideTypeDescriptor::category_get, nullptr, "category");
        dukglue_register_property(ctx, &ScRideTypeDescriptor::startTrackPiece_get, nullptr, "startTrackPiece");
        dukglue_register_property(ctx, &ScRideTypeDescriptor::defaultMode_get, nullptr, "defaultMode");
        dukglue_register_property(ctx, &ScRideTypeDescriptor::availableBreakdowns_get, nullptr, "availableBreakdowns");
        dukglue_register_property(ctx, &ScRideTypeDescriptor::maxMass_get, nullptr, "maxMass");
        dukglue_register_property(ctx, &ScRideTypeDescriptor::bonusValue_get, nullptr, "bonusValue");
        dukglue_register_property(ctx, &ScRideTypeDescriptor::name_get, nullptr, "name");
        dukglue_register_property(ctx, &ScRideTypeDescriptor::ratingsData_get, nullptr, "ratingsData");
    }
} // namespace OpenRCT2::Scripting

#endif // ENABLE_SCRIPTING
