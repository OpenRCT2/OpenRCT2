/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>

namespace Utils {
    int getTrackSequenceCount(uint8 rideType, uint8 trackType);
    bool rideSupportsTrackType(uint8 rideType, uint8 trackType);
    bool rideIsImplemented(uint8 rideType);
}
