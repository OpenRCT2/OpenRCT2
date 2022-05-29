/*****************************************************************************
 * Copyright (c) 2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScRideStation.hpp"

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../ride/Ride.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"
#    include "../object/ScObject.hpp"

namespace OpenRCT2::Scripting
{
    ScRideStation::ScRideStation(RideId rideId, StationIndex stationIndex)
        : _rideId(rideId)
        , _stationIndex(stationIndex)
    {
    }

    void ScRideStation::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScRideStation::start_get, &ScRideStation::start_set, "start");
        dukglue_register_property(ctx, &ScRideStation::length_get, &ScRideStation::length_set, "length");
        dukglue_register_property(ctx, &ScRideStation::entrance_get, &ScRideStation::entrance_set, "entrance");
        dukglue_register_property(ctx, &ScRideStation::exit_get, &ScRideStation::exit_set, "exit");
    }

    DukValue ScRideStation::start_get() const
    {
        auto ctx = GetContext()->GetScriptEngine()->GetContext();
        auto station = GetRideStation();
        if (station != nullptr)
        {
            auto start = CoordsXYZ(station->Start, station->GetBaseZ());
            return ToDuk(ctx, start);
        }
        return ToDuk(ctx, nullptr);
    }

    void ScRideStation::start_set(const DukValue& value)
    {
        auto station = GetRideStation();
        if (station != nullptr)
        {
            auto start = FromDuk<CoordsXYZ>(value);
            station->Start = { start.x, start.y };
            station->SetBaseZ(start.z);
        }
    }

    int32_t ScRideStation::length_get() const
    {
        auto station = GetRideStation();
        if (station != nullptr)
        {
            return station->Length;
        }
        return 0;
    }

    void ScRideStation::length_set(int32_t value)
    {
        auto station = GetRideStation();
        if (station != nullptr)
        {
            station->Length = value;
        }
    }

    DukValue ScRideStation::entrance_get() const
    {
        auto ctx = GetContext()->GetScriptEngine()->GetContext();
        auto station = GetRideStation();
        if (station != nullptr)
        {
            return ToDuk(ctx, station->Entrance.ToCoordsXYZD());
        }
        return ToDuk(ctx, nullptr);
    }

    void ScRideStation::entrance_set(const DukValue& value)
    {
        auto station = GetRideStation();
        if (station != nullptr)
        {
            station->Entrance = FromDuk<CoordsXYZD>(value);
        }
    }

    DukValue ScRideStation::exit_get() const
    {
        auto ctx = GetContext()->GetScriptEngine()->GetContext();
        auto station = GetRideStation();
        if (station != nullptr)
        {
            return ToDuk(ctx, station->Exit.ToCoordsXYZD());
        }
        return ToDuk(ctx, nullptr);
    }

    void ScRideStation::exit_set(const DukValue& value)
    {
        auto station = GetRideStation();
        if (station != nullptr)
        {
            station->Exit = FromDuk<CoordsXYZD>(value);
        }
    }

    RideStation* ScRideStation::GetRideStation() const
    {
        auto ride = get_ride(_rideId);
        if (ride != nullptr)
        {
            if (_stationIndex.ToUnderlying() < std::size(ride->GetStations()))
            {
                return &ride->GetStation(_stationIndex);
            }
        }
        return nullptr;
    }

} // namespace OpenRCT2::Scripting

#endif
