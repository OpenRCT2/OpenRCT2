/*****************************************************************************
 * Copyright (c) 2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScRideStation.hpp"

    #include "../../../Context.h"
    #include "../../../ride/Ride.h"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    void ScRideStation::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "RideStation", Finalize);
    }

    JSValue ScRideStation::New(JSContext* ctx, RideId rideId, StationIndex stationIndex)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("start", ScRideStation::start_get, ScRideStation::start_set),
            JS_CGETSET_DEF("length", ScRideStation::length_get, ScRideStation::length_set),
            JS_CGETSET_DEF("entrance", ScRideStation::entrance_get, ScRideStation::entrance_set),
            JS_CGETSET_DEF("exit", ScRideStation::exit_get, ScRideStation::exit_set),
        };
        return MakeWithOpaque(ctx, funcs, new RideStationData{ rideId, stationIndex });
    }

    void ScRideStation::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        RideStationData* data = GetRideStationData(thisVal);
        if (data)
            delete data;
    }

    ScRideStation::RideStationData* ScRideStation::GetRideStationData(JSValue thisVal)
    {
        return gScRideStation.GetOpaque<RideStationData*>(thisVal);
    }

    RideStation* ScRideStation::GetRideStation(JSValue thisVal)
    {
        RideStationData* data = GetRideStationData(thisVal);
        auto ride = ::GetRide(data->_rideId);
        if (ride != nullptr)
        {
            if (data->_stationIndex.ToUnderlying() < std::size(ride->getStations()))
            {
                return &ride->getStation(data->_stationIndex);
            }
        }
        return nullptr;
    }

    JSValue ScRideStation::start_get(JSContext* ctx, JSValue thisVal)
    {
        auto station = GetRideStation(thisVal);
        if (station != nullptr)
        {
            auto start = CoordsXYZ(station->Start, station->GetBaseZ());
            return ToJSValue(ctx, start);
        }
        return JS_NULL;
    }

    JSValue ScRideStation::start_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto station = GetRideStation(thisVal);
        if (station != nullptr)
        {
            auto start = JSToCoordsXYZ(ctx, value);
            station->Start = { start.x, start.y };
            station->SetBaseZ(start.z);
        }
        return JS_UNDEFINED;
    }

    JSValue ScRideStation::length_get(JSContext* ctx, JSValue thisVal)
    {
        auto station = GetRideStation(thisVal);
        return JS_NewInt32(ctx, station != nullptr ? station->Length : 0);
    }

    JSValue ScRideStation::length_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_UNPACK_INT32(valueInt, ctx, value);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto station = GetRideStation(thisVal);
        if (station != nullptr)
        {
            station->Length = valueInt;
        }
        return JS_UNDEFINED;
    }

    JSValue ScRideStation::entrance_get(JSContext* ctx, JSValue thisVal)
    {
        auto station = GetRideStation(thisVal);
        if (station != nullptr)
        {
            return ToJSValue(ctx, station->Entrance.ToCoordsXYZD());
        }
        return JS_NULL;
    }

    JSValue ScRideStation::entrance_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto station = GetRideStation(thisVal);
        if (station != nullptr)
        {
            station->Entrance = JSToCoordsXYZD(ctx, value);
        }
        return JS_UNDEFINED;
    }

    JSValue ScRideStation::exit_get(JSContext* ctx, JSValue thisVal)
    {
        auto station = GetRideStation(thisVal);
        if (station != nullptr)
        {
            return ToJSValue(ctx, station->Exit.ToCoordsXYZD());
        }
        return JS_NULL;
    }

    JSValue ScRideStation::exit_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto station = GetRideStation(thisVal);
        if (station != nullptr)
        {
            station->Exit = JSToCoordsXYZD(ctx, value);
        }
        return JS_UNDEFINED;
    }

} // namespace OpenRCT2::Scripting

#endif
