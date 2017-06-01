#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "Utils.hpp"

extern "C" {
    #include <openrct2/ride/ride.h>
    #include <openrct2/ride/track.h>
    #include <openrct2/ride/track_data.h>
}

namespace Utils {
    int getTrackSequenceCount(uint8 rideType, uint8 trackType) {
        int sequenceCount = 0;
        const rct_preview_track **trackBlocks;

        if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_FLAT_RIDE)) {
            trackBlocks = FlatRideTrackBlocks;
        } else {
            trackBlocks = TrackBlocks;
        }

        for (int i = 0; i < 256; i++) {
            if (trackBlocks[trackType][i].index == 0xFF) {
                break;
            }

            sequenceCount++;
        }

        return sequenceCount;
    }

    bool rideSupportsTrackType(uint8 rideType, uint8 trackType) {
        TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];

        if (newPaintGetter == NULL) {
            return false;
        }

        if (newPaintGetter(trackType, 0) == NULL) {
            return false;
        }

        if (RideTypeTrackPaintFunctionsOld[rideType][trackType] == 0) {
            return false;
        }

        return true;
    }

    bool rideIsImplemented(uint8 rideType) {
        TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
        return (newPaintGetter != 0);
    }
}
