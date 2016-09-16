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

#ifndef _TEST_PAINT_INTERCEPT_H_
#define _TEST_PAINT_INTERCEPT_H_

#include "../../src/common.h"

#define gRideEntries                RCT2_ADDRESS(RCT2_ADDRESS_RIDE_ENTRIES,                rct_ride_entry*)
#define gCurrentRotation        RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)

bool testRide(int rideType);
void initHooks();
int getTrackSequenceCount(uint8 rideType, uint8 trackType);
bool rideIsImplemented(int rideType);
bool rideSupportsTrackType(int rideType, int trackType);
bool testTrackPainting(int rideType, int trackType);
bool testSupportSegments(uint8 rideType, uint8 trackType);
bool testTunnels(uint8 rideType, uint8 trackType);
bool testVerticalTunnels(uint8 rideType, uint8 trackType);

#endif // #endif _TEST_PAINT_INTERCEPT_H_
