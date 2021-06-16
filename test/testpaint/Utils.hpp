/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>

using track_type_t = uint16_t;

namespace Utils
{
    int getTrackSequenceCount(uint8_t rideType, track_type_t trackType);
    bool rideSupportsTrackType(uint8_t rideType, track_type_t trackType);
} // namespace Utils
