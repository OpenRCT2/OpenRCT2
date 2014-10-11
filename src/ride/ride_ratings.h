/*****************************************************************************
 * Copyright (c) 2014 Kevin Burke
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef _RIDE_RATINGS_H_
#define _RIDE_RATINGS_H_

#include "../common.h"
#include "ride.h"

void ride_ratings_update_all();
void crooked_house_excitement(rct_ride *ride);
void sub_655FD6(rct_ride *ride);
ride_rating apply_intensity_penalty(ride_rating excitement, ride_rating intensity);
rating_tuple per_ride_rating_adjustments(rct_ride *ride, ride_rating excitement,
	ride_rating intensity, ride_rating nausea);
uint16 ride_compute_upkeep(rct_ride *ride);

#endif