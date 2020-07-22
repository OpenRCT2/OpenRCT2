/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../object/Object.h"
#include "../ride/Ride.h"
#include "../ride/Track.h"

#include <cstdint>

ObjectEntryIndex RCT2RideTypeToOpenRCT2RideType(uint8_t rct2RideType, const rct_ride_entry* rideEntry)
{
    switch (rct2RideType)
    {
        case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
            if (rideEntry != nullptr && !(ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_VERTICAL_LOOP)))
                return RIDE_TYPE_HYPERCOASTER;
            else
                return RIDE_TYPE_CORKSCREW_ROLLER_COASTER;
        case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
            if (rideEntry != nullptr && ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP))
                return RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER;
            else
                return RIDE_TYPE_JUNIOR_ROLLER_COASTER;
        case RIDE_TYPE_CAR_RIDE:
            if (rideEntry != nullptr && ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP))
                return RIDE_TYPE_MONSTER_TRUCKS;
            else
                return RIDE_TYPE_CAR_RIDE;
        case RIDE_TYPE_TWISTER_ROLLER_COASTER:
            if (rideEntry != nullptr && rideEntry->flags & RIDE_ENTRY_FLAG_NO_INVERSIONS)
                return RIDE_TYPE_HYPER_TWISTER;
            else
                return RIDE_TYPE_TWISTER_ROLLER_COASTER;
        case RIDE_TYPE_STEEL_WILD_MOUSE:
            if (rideEntry != nullptr && !(ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP)))
                return RIDE_TYPE_SPINNING_WILD_MOUSE;
            else
                return RIDE_TYPE_STEEL_WILD_MOUSE;

        default:
            return rct2RideType;
    }
}

bool RCT2RideTypeNeedsConversion(uint8_t rct2RideType)
{
    switch (rct2RideType)
    {
        case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
        case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
        case RIDE_TYPE_CAR_RIDE:
        case RIDE_TYPE_TWISTER_ROLLER_COASTER:
        case RIDE_TYPE_STEEL_WILD_MOUSE:
            return true;

        default:
            return false;
    }
}

uint8_t OpenRCT2RideTypeToRCT2RideType(ObjectEntryIndex openrct2Type)
{
    switch (openrct2Type)
    {
        case RIDE_TYPE_HYPERCOASTER:
            return RIDE_TYPE_CORKSCREW_ROLLER_COASTER;
        case RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER:
            return RIDE_TYPE_JUNIOR_ROLLER_COASTER;
        case RIDE_TYPE_MONSTER_TRUCKS:
            return RIDE_TYPE_CAR_RIDE;
        case RIDE_TYPE_HYPER_TWISTER:
            return RIDE_TYPE_TWISTER_ROLLER_COASTER;
        case RIDE_TYPE_SPINNING_WILD_MOUSE:
            return RIDE_TYPE_STEEL_WILD_MOUSE;

        default:
            return openrct2Type;
    }
}
