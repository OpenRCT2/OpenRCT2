#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#ifndef _RIDE_RATINGS_H_
#define _RIDE_RATINGS_H_

#include "../common.h"
#include "ride.h"

enum {
	RIDE_RATING_STATION_FLAG_NO_ENTRANCE = 1 << 0
};

#define _rideRatingsProximityX				RCT2_GLOBAL(0x0138B584, uint16)
#define _rideRatingsProximityY				RCT2_GLOBAL(0x0138B586, uint16)
#define _rideRatingsProximityZ				RCT2_GLOBAL(0x0138B588, uint16)
#define _rideRatingsCurrentRide				RCT2_GLOBAL(0x0138B590, uint8)
#define _rideRatingsState					RCT2_GLOBAL(0x0138B591, uint8)
#define _rideRatingsProximityTrackType		RCT2_GLOBAL(0x0138B592, uint8)
#define _rideRatingsProximityBaseHeight		RCT2_GLOBAL(0x0138B593, uint8)
#define _rideRatingsProximityTotal			RCT2_GLOBAL(0x0138B594, uint16)
#define _rideRatingsProximityStartX			RCT2_GLOBAL(0x0138B58A, uint16)
#define _rideRatingsProximityStartY			RCT2_GLOBAL(0x0138B58C, uint16)
#define _rideRatingsProximityStartZ			RCT2_GLOBAL(0x0138B58E, uint16)

#define _rideRatingsNumBrakes				RCT2_GLOBAL(0x0138B5CA, uint16)
#define _rideRatingsNumReversers			RCT2_GLOBAL(0x0138B5CC, uint16)
#define _rideRatingsStationFlags			RCT2_GLOBAL(0x0138B5CE, uint16)

extern uint16 *_proximityScores;

void ride_ratings_update_all();

#endif
