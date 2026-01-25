/*****************************************************************************
 * Copyright (c) 2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScRide.hpp"

    #include "../../../Context.h"
    #include "../../../core/EnumMap.hpp"
    #include "../../../core/UnitConversion.h"
    #include "../../../ride/Ride.h"
    #include "../../../ride/RideData.h"
    #include "../../ScriptEngine.h"
    #include "../object/ScObject.hpp"

namespace OpenRCT2::Scripting
{
    static const EnumMap<uint8_t> BreakdownMap(
        {
            { "safety_cut_out", BREAKDOWN_SAFETY_CUT_OUT },
            { "restraints_stuck_closed", BREAKDOWN_RESTRAINTS_STUCK_CLOSED },
            { "restraints_stuck_open", BREAKDOWN_RESTRAINTS_STUCK_OPEN },
            { "doors_stuck_closed", BREAKDOWN_DOORS_STUCK_CLOSED },
            { "doors_stuck_open", BREAKDOWN_DOORS_STUCK_OPEN },
            { "vehicle_malfunction", BREAKDOWN_VEHICLE_MALFUNCTION },
            { "brakes_failure", BREAKDOWN_BRAKES_FAILURE },
            { "control_failure", BREAKDOWN_CONTROL_FAILURE },
        });

    void ScRide::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "Ride", Finalize);
    }

    JSValue ScRide::New(JSContext* ctx, RideId rideId)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("id", ScRide::id_get, nullptr),
            JS_CGETSET_DEF("object", ScRide::object_get, nullptr),
            JS_CGETSET_DEF("type", ScRide::type_get, nullptr),
            JS_CGETSET_DEF("classification", ScRide::classification_get, nullptr),
            JS_CGETSET_DEF("name", ScRide::name_get, ScRide::name_set),
            JS_CGETSET_DEF("status", ScRide::status_get, nullptr),
            JS_CGETSET_DEF("lifecycleFlags", ScRide::lifecycleFlags_get, ScRide::lifecycleFlags_set),
            JS_CGETSET_DEF("mode", ScRide::mode_get, ScRide::mode_set),
            JS_CGETSET_DEF("departFlags", ScRide::departFlags_get, ScRide::departFlags_set),
            JS_CGETSET_DEF("minimumWaitingTime", ScRide::minimumWaitingTime_get, ScRide::minimumWaitingTime_set),
            JS_CGETSET_DEF("maximumWaitingTime", ScRide::maximumWaitingTime_get, ScRide::maximumWaitingTime_set),
            JS_CGETSET_DEF("vehicles", ScRide::vehicles_get, nullptr),
            JS_CGETSET_DEF("vehicleColours", ScRide::vehicleColours_get, ScRide::vehicleColours_set),
            JS_CGETSET_DEF("colourSchemes", ScRide::colourSchemes_get, ScRide::colourSchemes_set),
            JS_CGETSET_DEF("stationStyle", ScRide::stationStyle_get, ScRide::stationStyle_set),
            JS_CGETSET_DEF("music", ScRide::music_get, ScRide::music_set),
            JS_CGETSET_DEF("stations", ScRide::stations_get, nullptr),
            JS_CGETSET_DEF("price", ScRide::price_get, ScRide::price_set),
            JS_CGETSET_DEF("excitement", ScRide::excitement_get, ScRide::excitement_set),
            JS_CGETSET_DEF("intensity", ScRide::intensity_get, ScRide::intensity_set),
            JS_CGETSET_DEF("nausea", ScRide::nausea_get, ScRide::nausea_set),
            JS_CGETSET_DEF("totalCustomers", ScRide::totalCustomers_get, ScRide::totalCustomers_set),
            JS_CGETSET_DEF("buildDate", ScRide::buildDate_get, ScRide::buildDate_set),
            JS_CGETSET_DEF("age", ScRide::age_get, nullptr),
            JS_CGETSET_DEF("runningCost", ScRide::runningCost_get, ScRide::runningCost_set),
            JS_CGETSET_DEF("totalProfit", ScRide::totalProfit_get, ScRide::totalProfit_set),
            JS_CGETSET_DEF("inspectionInterval", ScRide::inspectionInterval_get, ScRide::inspectionInterval_set),
            JS_CGETSET_DEF("value", ScRide::value_get, ScRide::value_set),
            JS_CGETSET_DEF("downtime", ScRide::downtime_get, nullptr),
            JS_CGETSET_DEF("liftHillSpeed", ScRide::liftHillSpeed_get, ScRide::liftHillSpeed_set),
            JS_CGETSET_DEF("maxLiftHillSpeed", ScRide::maxLiftHillSpeed_get, nullptr),
            JS_CGETSET_DEF("minLiftHillSpeed", ScRide::minLiftHillSpeed_get, nullptr),
            JS_CGETSET_DEF("satisfaction", ScRide::satisfaction_get, nullptr),
            JS_CGETSET_DEF("maxSpeed", ScRide::maxSpeed_get, nullptr),
            JS_CGETSET_DEF("averageSpeed", ScRide::averageSpeed_get, nullptr),
            JS_CGETSET_DEF("rideTime", ScRide::rideTime_get, nullptr),
            JS_CGETSET_DEF("rideLength", ScRide::rideLength_get, nullptr),
            JS_CGETSET_DEF("maxPositiveVerticalGs", ScRide::maxPositiveVerticalGs_get, nullptr),
            JS_CGETSET_DEF("maxNegativeVerticalGs", ScRide::maxNegativeVerticalGs_get, nullptr),
            JS_CGETSET_DEF("maxLateralGs", ScRide::maxLateralGs_get, nullptr),
            JS_CGETSET_DEF("totalAirTime", ScRide::totalAirTime_get, nullptr),
            JS_CGETSET_DEF("numDrops", ScRide::numDrops_get, nullptr),
            JS_CGETSET_DEF("numLiftHills", ScRide::numLiftHills_get, nullptr),
            JS_CGETSET_DEF("highestDropHeight", ScRide::highestDropHeight_get, nullptr),
            JS_CGETSET_DEF("breakdown", ScRide::breakdown_get, nullptr),
            JS_CFUNC_DEF("setBreakdown", 1, ScRide::setBreakdown),
            JS_CFUNC_DEF("fixBreakdown", 0, ScRide::fixBreakdown),
        };
        return MakeWithOpaque(ctx, funcs, new RideData{ rideId });
    }

    void ScRide::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        RideData* data = GetRideData(thisVal);
        if (data)
            delete data;
    }

    ScRide::RideData* ScRide::GetRideData(JSValue thisVal)
    {
        return gScRide.GetOpaque<RideData*>(thisVal);
    }

    Ride* ScRide::GetRide(JSValue thisVal)
    {
        RideData* data = GetRideData(thisVal);
        return ::GetRide(data->_rideId);
    }

    JSValue ScRide::id_get(JSContext* ctx, JSValue thisVal)
    {
        RideData* data = GetRideData(thisVal);
        return JS_NewInt32(ctx, data->_rideId.ToUnderlying());
    }

    JSValue ScRide::object_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            auto rideObject = GetContext()->GetObjectManager().GetLoadedObject<RideObject>(ride->subtype);
            if (rideObject != nullptr)
            {
                return ScRideObject::New(ctx, ObjectType::ride, ride->subtype);
            }
        }
        return JS_NULL;
    }

    JSValue ScRide::type_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewInt32(ctx, ride != nullptr ? ride->type : 0);
    }

    JSValue ScRide::classification_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        std::string str = "";
        if (ride != nullptr)
        {
            switch (ride->getClassification())
            {
                case RideClassification::ride:
                    str = "ride";
                    break;
                case RideClassification::shopOrStall:
                    str = "stall";
                    break;
                case RideClassification::kioskOrFacility:
                    str = "facility";
                    break;
            }
        }
        return JSFromStdString(ctx, str);
    }

    JSValue ScRide::name_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JSFromStdString(ctx, ride != nullptr ? ride->getName() : std::string());
    }

    JSValue ScRide::name_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_STR(valueStr, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->customName = std::move(valueStr);
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::status_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        std::string str = "";

        if (ride != nullptr)
        {
            switch (ride->status)
            {
                case RideStatus::closed:
                    str = "closed";
                    break;
                case RideStatus::open:
                    str = "open";
                    break;
                case RideStatus::testing:
                    str = "testing";
                    break;
                case RideStatus::simulating:
                    str = "simulating";
                    break;
                case RideStatus::count:
                    str = "count";
                    break;
            }
        }
        return JSFromStdString(ctx, str);
    }

    JSValue ScRide::lifecycleFlags_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? ride->lifecycleFlags : 0);
    }

    JSValue ScRide::lifecycleFlags_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_UINT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->lifecycleFlags = valueInt;
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::mode_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? static_cast<uint8_t>(ride->mode) : 0);
    }

    JSValue ScRide::mode_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_UINT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->mode = static_cast<RideMode>(valueInt);
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::departFlags_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? ride->departFlags : 0);
    }

    JSValue ScRide::departFlags_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_UINT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->departFlags = static_cast<uint8_t>(valueInt);
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::minimumWaitingTime_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? ride->minWaitingTime : 0);
    }

    JSValue ScRide::minimumWaitingTime_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_UINT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->minWaitingTime = static_cast<uint8_t>(valueInt);
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::maximumWaitingTime_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? ride->maxWaitingTime : 0);
    }

    JSValue ScRide::maximumWaitingTime_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_UINT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->maxWaitingTime = static_cast<uint8_t>(valueInt);
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::vehicles_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue result = JS_NewArray(ctx);
        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            int64_t index = 0;
            std::for_each(std::begin(ride->vehicles), std::begin(ride->vehicles) + ride->numTrains, [&](auto& veh) {
                JS_SetPropertyInt64(ctx, result, index++, JS_NewUint32(ctx, veh.ToUnderlying()));
            });
        }
        return result;
    }

    JSValue ScRide::vehicleColours_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue result = JS_NewArray(ctx);
        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            int64_t index = 0;
            for (const auto& vehicleColour : ride->vehicleColours)
            {
                JS_SetPropertyInt64(ctx, result, index++, ToJSValue(ctx, vehicleColour));
            }
        }
        return result;
    }

    JSValue ScRide::vehicleColours_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr && JS_IsArray(value))
        {
            int64_t length;
            JS_GetLength(ctx, value, &length);

            auto count = std::min(static_cast<size_t>(length), std::size(ride->vehicleColours));
            for (size_t i = 0; i < count; i++)
            {
                JSValue item = JS_GetPropertyInt64(ctx, value, static_cast<int64_t>(i));
                ride->vehicleColours[i] = JSToVehicleColours(ctx, item);
                JS_FreeValue(ctx, item);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::colourSchemes_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue result = JS_NewArray(ctx);
        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            int64_t index = 0;
            for (const auto& trackColour : ride->trackColours)
            {
                JS_SetPropertyInt64(ctx, result, index++, ToJSValue(ctx, trackColour));
            }
        }
        return result;
    }

    JSValue ScRide::colourSchemes_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr && JS_IsArray(value))
        {
            int64_t length;
            JS_GetLength(ctx, value, &length);

            auto count = std::min(static_cast<size_t>(length), std::size(ride->trackColours));
            for (size_t i = 0; i < count; i++)
            {
                JSValue item = JS_GetPropertyInt64(ctx, value, static_cast<int64_t>(i));
                ride->trackColours[i] = JSToTrackColour(ctx, item);
                JS_FreeValue(ctx, item);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::stationStyle_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? ride->entranceStyle : 0);
    }

    JSValue ScRide::stationStyle_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_UINT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->entranceStyle = static_cast<ObjectEntryIndex>(valueInt);
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::music_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? ride->music : 0);
    }

    JSValue ScRide::music_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_UINT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->music = static_cast<ObjectEntryIndex>(valueInt);
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::stations_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue result = JS_NewArray(ctx);
        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            int64_t index = 0;
            for (const auto& station : ride->getStations())
            {
                JS_SetPropertyInt64(ctx, result, index++, gScRideStation.New(ctx, ride->id, ride->getStationIndex(&station)));
            }
        }
        return result;
    }

    JSValue ScRide::price_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue result = JS_NewArray(ctx);
        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            auto numPrices = ride->getNumPrices();
            for (size_t i = 0; i < numPrices; i++)
            {
                JS_SetPropertyInt64(ctx, result, static_cast<int64_t>(i), JS_NewInt64(ctx, ride->price[i]));
            }
        }
        return result;
    }

    JSValue ScRide::price_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr && JS_IsArray(value))
        {
            int64_t length;
            JS_GetLength(ctx, value, &length);

            auto numPrices = std::min(static_cast<size_t>(length), ride->getNumPrices());
            for (size_t i = 0; i < numPrices; i++)
            {
                JSValue item = JS_GetPropertyInt64(ctx, value, static_cast<uint32_t>(i));
                JS_UNPACK_MONEY64(money, ctx, item);
                ride->price[i] = std::clamp<money64>(money, kRideMinPrice, kRideMaxPrice);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::excitement_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewInt32(ctx, ride != nullptr ? ride->ratings.excitement : 0);
    }

    JSValue ScRide::excitement_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->ratings.excitement = valueInt;
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::intensity_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewInt32(ctx, ride != nullptr ? ride->ratings.intensity : 0);
    }

    JSValue ScRide::intensity_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->ratings.intensity = valueInt;
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::nausea_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewInt32(ctx, ride != nullptr ? ride->ratings.nausea : 0);
    }

    JSValue ScRide::nausea_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->ratings.nausea = valueInt;
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::totalCustomers_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewInt32(ctx, ride != nullptr ? ride->totalCustomers : 0);
    }

    JSValue ScRide::totalCustomers_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->totalCustomers = valueInt;
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::buildDate_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewInt32(ctx, ride != nullptr ? ride->buildDate : 0);
    }

    JSValue ScRide::buildDate_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->buildDate = valueInt;
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::age_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewInt32(ctx, ride != nullptr ? ride->getAge() : 0);
    }

    JSValue ScRide::runningCost_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewInt64(ctx, ride != nullptr ? ride->upkeepCost : 0);
    }

    JSValue ScRide::runningCost_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_MONEY64(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->upkeepCost = valueInt;
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::totalProfit_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewInt32(ctx, ride != nullptr ? ride->totalProfit : 0);
    }

    JSValue ScRide::totalProfit_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_MONEY64(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->totalProfit = valueInt;
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::inspectionInterval_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? EnumValue(ride->inspectionInterval) : 0);
    }

    JSValue ScRide::inspectionInterval_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_UINT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            auto clamped = std::clamp<uint8_t>(
                static_cast<uint8_t>(valueInt), EnumValue(RideInspection::every10Minutes), EnumValue(RideInspection::never));
            ride->inspectionInterval = static_cast<RideInspection>(clamped);
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::value_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        if (ride != nullptr && ride->value != kRideValueUndefined)
        {
            return JS_NewInt32(ctx, ride->value);
        }
        return JS_NULL;
    }

    JSValue ScRide::value_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            if (JS_IsNumber(value))
            {
                JS_UNPACK_MONEY64(valueInt, ctx, value);
                ride->value = valueInt;
            }
            else
            {
                ride->value = kRideValueUndefined;
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::downtime_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? ride->downtime : 0);
    }

    JSValue ScRide::liftHillSpeed_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? ride->liftHillSpeed : 0);
    }

    JSValue ScRide::liftHillSpeed_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_UINT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            ride->liftHillSpeed = static_cast<uint8_t>(valueInt);
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::maxLiftHillSpeed_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        auto maximum = ride != nullptr ? ride->getRideTypeDescriptor().LiftData.maximum_speed : 0;
        return JS_NewUint32(ctx, maximum);
    }

    JSValue ScRide::minLiftHillSpeed_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        auto minimum = ride != nullptr ? ride->getRideTypeDescriptor().LiftData.minimum_speed : 0;
        return JS_NewUint32(ctx, minimum);
    }

    JSValue ScRide::satisfaction_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? ride->satisfaction * 5 : 0);
    }

    JSValue ScRide::maxSpeed_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewFloat64(ctx, ride != nullptr ? ToHumanReadableSpeed(ride->maxSpeed) : 0);
    }

    JSValue ScRide::averageSpeed_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewFloat64(ctx, ride != nullptr ? ToHumanReadableSpeed(ride->averageSpeed) : 0);
    }

    JSValue ScRide::rideTime_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewInt32(ctx, ride != nullptr ? ride->getTotalTime() : 0);
    }

    JSValue ScRide::rideLength_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewFloat64(ctx, ride != nullptr ? ToHumanReadableRideLength(ride->getTotalLength()) : 0);
    }

    JSValue ScRide::maxPositiveVerticalGs_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewFloat64(ctx, ride != nullptr ? ride->maxPositiveVerticalG / 100.0 : 0);
    }

    JSValue ScRide::maxNegativeVerticalGs_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewFloat64(ctx, ride != nullptr ? ride->maxNegativeVerticalG / 100.0 : 0);
    }

    JSValue ScRide::maxLateralGs_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewFloat64(ctx, ride != nullptr ? ride->maxLateralG / 100.0 : 0);
    }

    JSValue ScRide::totalAirTime_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewFloat64(ctx, ride != nullptr ? ToHumanReadableAirTime(ride->totalAirTime) / 100.0 : 0);
    }

    JSValue ScRide::numDrops_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? ride->numDrops : 0);
    }

    JSValue ScRide::numLiftHills_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewUint32(ctx, ride != nullptr ? ride->numPoweredLifts : 0);
    }

    JSValue ScRide::highestDropHeight_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        return JS_NewFloat64(ctx, ride != nullptr ? ride->highestDropHeight : 0);
    }

    JSValue ScRide::breakdown_get(JSContext* ctx, JSValue thisVal)
    {
        auto ride = GetRide(thisVal);
        if (ride != nullptr)
        {
            if (!(ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN))
            {
                return JSFromStdString(ctx, "none");
            }
            auto it = BreakdownMap.find(ride->breakdownReason);
            if (it != BreakdownMap.end())
                return JSFromStdString(ctx, std::string(it->first));
        }
        return JSFromStdString(ctx, "");
    }

    JSValue ScRide::setBreakdown(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(breakDown, ctx, argv[0]);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr && ride->canBreakDown() && ride->status == RideStatus::open)
        {
            auto it = BreakdownMap.find(breakDown);
            if (it != BreakdownMap.end())
            {
                RidePrepareBreakdown(*ride, it->second);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScRide::fixBreakdown(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto ride = GetRide(thisVal);
        if (ride != nullptr && ride->canBreakDown())
        {
            RideFixBreakdown(*ride, 0);
        }
        return JS_UNDEFINED;
    }

} // namespace OpenRCT2::Scripting

#endif
