/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

void ride_update_station(Ride* ride, StationIndex stationIndex);
StationIndex ride_get_first_valid_station_exit(Ride* ride);
StationIndex ride_get_first_valid_station_start(const Ride* ride);
StationIndex ride_get_first_empty_station_start(const Ride* ride);

TileCoordsXYZD ride_get_entrance_location(const Ride* ride, const StationIndex stationIndex);
TileCoordsXYZD ride_get_exit_location(const Ride* ride, const StationIndex stationIndex);

void ride_clear_entrance_location(Ride* ride, const StationIndex stationIndex);
void ride_clear_exit_location(Ride* ride, const StationIndex stationIndex);

void ride_set_entrance_location(Ride* ride, const StationIndex stationIndex, const TileCoordsXYZD& location);
void ride_set_exit_location(Ride* ride, const StationIndex stationIndex, const TileCoordsXYZD& location);
