/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Ride.h"

void ride_update_station(Ride * ride, int32_t stationIndex);
int8_t ride_get_first_valid_station_exit(Ride * ride);
int8_t ride_get_first_valid_station_start(const Ride * ride);
int8_t ride_get_first_empty_station_start(const Ride * ride);

TileCoordsXYZD ride_get_entrance_location(const int32_t rideIndex, const int32_t stationIndex);
TileCoordsXYZD ride_get_exit_location(const int32_t rideIndex, const int32_t stationIndex);
TileCoordsXYZD ride_get_entrance_location(const Ride * ride, const int32_t stationIndex);
TileCoordsXYZD ride_get_exit_location(const Ride * ride, const int32_t stationIndex);

void ride_clear_entrance_location(Ride * ride, const int32_t stationIndex);
void ride_clear_exit_location(Ride * ride, const int32_t stationIndex);

void ride_set_entrance_location(Ride * ride, const int32_t stationIndex, const TileCoordsXYZD location);
void ride_set_exit_location(Ride * ride, const int32_t stationIndex, const TileCoordsXYZD location);
