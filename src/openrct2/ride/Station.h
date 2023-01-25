/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../common.h"

struct Ride;
struct TileCoordsXYZD;

void RideUpdateStation(Ride& ride, StationIndex stationIndex);
StationIndex RideGetFirstValidStationExit(const Ride& ride);
StationIndex RideGetFirstValidStationStart(const Ride& ride);
StationIndex RideGetFirstEmptyStationStart(const Ride& ride);
