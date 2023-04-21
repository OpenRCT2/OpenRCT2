/*****************************************************************************
 * Copyright (c) 2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../ride/Ride.h"
#    include "../../Duktape.hpp"

namespace OpenRCT2::Scripting
{
    class ScRideStation
    {
    private:
        RideId _rideId = RideId::GetNull();
        StationIndex _stationIndex{};

    public:
        ScRideStation(RideId rideId, StationIndex stationIndex);

        static void Register(duk_context* ctx);

    private:
        DukValue start_get() const;
        void start_set(const DukValue& value);

        int32_t length_get() const;
        void length_set(int32_t value);

        DukValue entrance_get() const;
        void entrance_set(const DukValue& value);

        DukValue exit_get() const;
        void exit_set(const DukValue& value);

        int32_t segmentLength_get() const;
        void segmentLength_set(int32_t value);

        int32_t segmentTime_get() const;
        void segmentTime_set(int32_t value);

        int32_t queueLength_get() const;
        void queueLength_set(int32_t value);

        int32_t queueTime_get() const;
        void queueTime_set(int32_t value);

        RideStation* GetRideStation() const;
    };

} // namespace OpenRCT2::Scripting

#endif
