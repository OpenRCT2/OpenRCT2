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

    #include "../../../entity/EntityTweener.h"
    #include "../../../ride/Ride.h"
    #include "ScEntity.hpp"

    #include <optional>

namespace OpenRCT2::Scripting
{
    class ScVehicle final : public ScEntity
    {
    public:
        static JSValue New(JSContext* ctx, EntityId entityId);

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj);
        static Vehicle* GetVehicle(JSValue thisVal);

        static JSValue rideObject_get(JSContext* ctx, JSValue thisVal);
        static JSValue rideObject_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue vehicleObject_get(JSContext* ctx, JSValue thisVal);
        static JSValue vehicleObject_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue spriteType_get(JSContext* ctx, JSValue thisVal);
        static JSValue spriteType_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue ride_get(JSContext* ctx, JSValue thisVal);
        static JSValue ride_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue numSeats_get(JSContext* ctx, JSValue thisVal);
        static JSValue numSeats_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue nextCarOnTrain_get(JSContext* ctx, JSValue thisVal);
        static JSValue nextCarOnTrain_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue previousCarOnRide_get(JSContext* ctx, JSValue thisVal);
        static JSValue previousCarOnRide_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue nextCarOnRide_get(JSContext* ctx, JSValue thisVal);
        static JSValue nextCarOnRide_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue currentStation_get(JSContext* ctx, JSValue thisVal);
        static JSValue currentStation_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue mass_get(JSContext* ctx, JSValue thisVal);
        static JSValue mass_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue acceleration_get(JSContext* ctx, JSValue thisVal);
        static JSValue acceleration_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue velocity_get(JSContext* ctx, JSValue thisVal);
        static JSValue velocity_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue bankRotation_get(JSContext* ctx, JSValue thisVal);
        static JSValue bankRotation_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        template<uint32_t flag>
        static JSValue flag_get(JSContext* ctx, JSValue thisVal);
        template<uint32_t flag>
        static JSValue flag_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue colours_get(JSContext* ctx, JSValue thisVal);
        static JSValue colours_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue trackLocation_get(JSContext* ctx, JSValue thisVal);

        static JSValue trackProgress_get(JSContext* ctx, JSValue thisVal);

        static JSValue remainingDistance_get(JSContext* ctx, JSValue thisVal);

        static JSValue subposition_get(JSContext* ctx, JSValue thisVal);

        static JSValue poweredAcceleration_get(JSContext* ctx, JSValue thisVal);
        static JSValue poweredAcceleration_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue poweredMaxSpeed_get(JSContext* ctx, JSValue thisVal);
        static JSValue poweredMaxSpeed_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue status_get(JSContext* ctx, JSValue thisVal);
        static JSValue status_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue spin_get(JSContext* ctx, JSValue thisVal);
        static JSValue spin_set(JSContext* ctx, JSValue thisVal, JSValue jsValue);

        static JSValue guests_get(JSContext* ctx, JSValue thisVal);

        static JSValue gForces_get(JSContext* ctx, JSValue thisVal);

        static JSValue travelBy(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);

        static JSValue moveToTrack(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
    };

} // namespace OpenRCT2::Scripting

#endif
