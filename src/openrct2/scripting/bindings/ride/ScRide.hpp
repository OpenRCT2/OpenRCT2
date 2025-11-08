/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../Context.h"
#include "../../../ride/Ride.h"
#include "../../ScriptEngine.h"
#include "../object/ScObject.hpp"
#include "ScRideStation.hpp"

#ifdef ENABLE_SCRIPTING_REFACTOR

namespace OpenRCT2::Scripting
{

    inline JSValue ToJSValue(JSContext* ctx, const VehicleColour& value)
    {
        JSValue obj = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, obj, "body", JS_NewInt32(ctx, value.Body));
        JS_SetPropertyStr(ctx, obj, "trim", JS_NewInt32(ctx, value.Trim));
        JS_SetPropertyStr(ctx, obj, "ternary", JS_NewInt32(ctx, value.Tertiary)); // backwards compatible typo
        JS_SetPropertyStr(ctx, obj, "tertiary", JS_NewInt32(ctx, value.Tertiary));
        return obj;
    }

    inline VehicleColour JSToVehicleColours(JSContext* ctx, JSValue val)
    {
        VehicleColour result{};
        result.Body = AsOrDefault(ctx, val, "body", 0);
        result.Trim = AsOrDefault(ctx, val, "trim", 0);
        result.Tertiary = AsOrDefault(ctx, val, "ternary", 0); // backwards compatible typo
        result.Tertiary = AsOrDefault(ctx, val, "tertiary", static_cast<uint32_t>(result.Tertiary));
        return result;
    }

    inline JSValue ToJSValue(JSContext* ctx, const TrackColour& value)
    {
        JSValue obj = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, obj, "main", JS_NewInt32(ctx, value.main));
        JS_SetPropertyStr(ctx, obj, "additional", JS_NewInt32(ctx, value.additional));
        JS_SetPropertyStr(ctx, obj, "supports", JS_NewInt32(ctx, value.supports));
        return obj;
    }

    inline TrackColour JSToTrackColour(JSContext* ctx, JSValue val)
    {
        TrackColour result{};
        result.main = AsOrDefault(ctx, val, "main", 0);
        result.additional = AsOrDefault(ctx, val, "additional", 0);
        result.supports = AsOrDefault(ctx, val, "supports", 0);
        return result;
    }

    class ScRide;
    extern ScRide gScRide;

    class ScRide final : public ScBase
    {
    private:
        struct RideData
        {
            RideId _rideId = RideId::GetNull();
        };

    public:
        void Register(JSContext* ctx);
        JSValue New(JSContext* ctx, RideId rideId);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
        static RideData* GetRideData(JSValue thisVal);
        static Ride* GetRide(JSValue thisVal);

        static JSValue id_get(JSContext* ctx, JSValue thisVal);
        static JSValue object_get(JSContext* ctx, JSValue thisVal);
        static JSValue type_get(JSContext* ctx, JSValue thisVal);
        static JSValue classification_get(JSContext* ctx, JSValue thisVal);
        static JSValue name_get(JSContext* ctx, JSValue thisVal);
        static JSValue name_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue status_get(JSContext* ctx, JSValue thisVal);
        static JSValue lifecycleFlags_get(JSContext* ctx, JSValue thisVal);
        static JSValue lifecycleFlags_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue mode_get(JSContext* ctx, JSValue thisVal);
        static JSValue mode_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue departFlags_get(JSContext* ctx, JSValue thisVal);
        static JSValue departFlags_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue minimumWaitingTime_get(JSContext* ctx, JSValue thisVal);
        static JSValue minimumWaitingTime_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue maximumWaitingTime_get(JSContext* ctx, JSValue thisVal);
        static JSValue maximumWaitingTime_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue vehicles_get(JSContext* ctx, JSValue thisVal);
        static JSValue vehicleColours_get(JSContext* ctx, JSValue thisVal);
        static JSValue vehicleColours_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue colourSchemes_get(JSContext* ctx, JSValue thisVal);
        static JSValue colourSchemes_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue stationStyle_get(JSContext* ctx, JSValue thisVal);
        static JSValue stationStyle_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue music_get(JSContext* ctx, JSValue thisVal);
        static JSValue music_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue stations_get(JSContext* ctx, JSValue thisVal);
        static JSValue price_get(JSContext* ctx, JSValue thisVal);
        static JSValue price_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue excitement_get(JSContext* ctx, JSValue thisVal);
        static JSValue excitement_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue intensity_get(JSContext* ctx, JSValue thisVal);
        static JSValue intensity_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue nausea_get(JSContext* ctx, JSValue thisVal);
        static JSValue nausea_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue totalCustomers_get(JSContext* ctx, JSValue thisVal);
        static JSValue totalCustomers_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue buildDate_get(JSContext* ctx, JSValue thisVal);
        static JSValue buildDate_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue age_get(JSContext* ctx, JSValue thisVal);
        static JSValue runningCost_get(JSContext* ctx, JSValue thisVal);
        static JSValue runningCost_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue totalProfit_get(JSContext* ctx, JSValue thisVal);
        static JSValue totalProfit_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue inspectionInterval_get(JSContext* ctx, JSValue thisVal);
        static JSValue inspectionInterval_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue value_get(JSContext* ctx, JSValue thisVal);
        static JSValue value_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue downtime_get(JSContext* ctx, JSValue thisVal);
        static JSValue liftHillSpeed_get(JSContext* ctx, JSValue thisVal);
        static JSValue liftHillSpeed_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue maxLiftHillSpeed_get(JSContext* ctx, JSValue thisVal);
        static JSValue minLiftHillSpeed_get(JSContext* ctx, JSValue thisVal);
        static JSValue satisfaction_get(JSContext* ctx, JSValue thisVal);
        static JSValue maxSpeed_get(JSContext* ctx, JSValue thisVal);
        static JSValue averageSpeed_get(JSContext* ctx, JSValue thisVal);
        static JSValue rideTime_get(JSContext* ctx, JSValue thisVal);
        static JSValue rideLength_get(JSContext* ctx, JSValue thisVal);
        static JSValue maxPositiveVerticalGs_get(JSContext* ctx, JSValue thisVal);
        static JSValue maxNegativeVerticalGs_get(JSContext* ctx, JSValue thisVal);
        static JSValue maxLateralGs_get(JSContext* ctx, JSValue thisVal);
        static JSValue totalAirTime_get(JSContext* ctx, JSValue thisVal);
        static JSValue numDrops_get(JSContext* ctx, JSValue thisVal);
        static JSValue numLiftHills_get(JSContext* ctx, JSValue thisVal);
        static JSValue highestDropHeight_get(JSContext* ctx, JSValue thisVal);
        static JSValue breakdown_get(JSContext* ctx, JSValue thisVal);
        static JSValue setBreakdown(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
        static JSValue fixBreakdown(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv);
    };
} // namespace OpenRCT2::Scripting

#endif
