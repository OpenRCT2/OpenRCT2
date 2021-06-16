/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Utils.hpp"

#include "Data.h"

#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>

namespace Utils
{
    int getTrackSequenceCount(uint8_t rideType, track_type_t trackType)
    {
        int sequenceCount = 0;

        for (int i = 0; i < TrackElemType::Count; i++)
        {
            if (TrackBlocks[trackType][i].index == 0xFF)
            {
                break;
            }

            sequenceCount++;
        }

        return sequenceCount;
    }

    bool rideSupportsTrackType(uint8_t rideType, track_type_t trackType)
    {
        TRACK_PAINT_FUNCTION_GETTER newPaintGetter = GetRideTypeDescriptor(rideType).TrackPaintFunction;

        if (newPaintGetter == nullptr)
        {
            return false;
        }

        if (newPaintGetter(trackType) == nullptr)
        {
            return false;
        }

        return true;
    }
} // namespace Utils
