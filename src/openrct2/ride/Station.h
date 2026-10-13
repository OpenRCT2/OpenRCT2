/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"

namespace OpenRCT2
{
    struct Ride;

    void RideUpdateStation(OpenRCT2::Ride& ride, StationIndex stationIndex);
    StationIndex RideGetFirstValidStationExit(const OpenRCT2::Ride& ride);
    StationIndex RideGetFirstValidStationStart(const OpenRCT2::Ride& ride);
    StationIndex RideGetFirstEmptyStationStart(const OpenRCT2::Ride& ride);
} // namespace OpenRCT2
