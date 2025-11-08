/*****************************************************************************
 * Copyright (c) 2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "../../../Context.h"
    #include "../../../ride/Ride.h"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScRideStation;
    extern ScRideStation gScRideStation;

    class ScRideStation final : public ScBase
    {
    private:
        struct RideStationData
        {
            RideId _rideId = RideId::GetNull();
            StationIndex _stationIndex{};
        };

    public:
        void Register(JSContext* ctx);
        JSValue New(JSContext* ctx, RideId rideId, StationIndex stationIndex);

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal);
        static RideStationData* GetRideStationData(JSValue thisVal);
        static RideStation* GetRideStation(JSValue thisVal);

        static JSValue start_get(JSContext* ctx, JSValue thisVal);
        static JSValue start_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue length_get(JSContext* ctx, JSValue thisVal);
        static JSValue length_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue entrance_get(JSContext* ctx, JSValue thisVal);
        static JSValue entrance_set(JSContext* ctx, JSValue thisVal, JSValue value);
        static JSValue exit_get(JSContext* ctx, JSValue thisVal);
        static JSValue exit_set(JSContext* ctx, JSValue thisVal, JSValue value);
    };

} // namespace OpenRCT2::Scripting

#endif
