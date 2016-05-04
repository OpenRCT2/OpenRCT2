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

#include "addresses.h"
#include "config.h"
#include "game.h"
#include "localisation/string_ids.h"
#include "rct1.h"
#include "util/sawyercoding.h"
#include "util/util.h"

bool rct1_read_sc4(const char *path, rct1_s4 *s4)
{
	uint8 *buffer, *decodedBuffer;
	long length, decodedLength;
	bool success;

	if (!readentirefile(path, (void**)&buffer, (int*)&length)) {
		gErrorType = ERROR_TYPE_FILE_LOAD;
		gErrorStringId = STR_FILE_CONTAINS_INVALID_DATA;
		return 0;
	}

	int fileType = sawyercoding_detect_file_type(buffer, length);

	decodedBuffer = malloc(sizeof(rct1_s4));
	decodedLength = (fileType & FILE_VERSION_MASK) == FILE_VERSION_RCT1 ?
		sawyercoding_decode_sv4(buffer, decodedBuffer, length) :
		sawyercoding_decode_sc4(buffer, decodedBuffer, length);
	if (decodedLength == sizeof(rct1_s4)) {
		memcpy(s4, decodedBuffer, sizeof(rct1_s4));
		success = true;
	} else {
		success = false;
	}

	free(buffer);
	free(decodedBuffer);
	return success;
}

bool rct1_read_sv4(const char *path, rct1_s4 *s4)
{
	uint8 *buffer, *decodedBuffer;
	long length, decodedLength;
	bool success;

	if (!readentirefile(path, (void**)&buffer, (int*)&length)) {
		gErrorType = ERROR_TYPE_FILE_LOAD;
		gErrorStringId = STR_FILE_CONTAINS_INVALID_DATA;
		return 0;
	}

	decodedBuffer = malloc(sizeof(rct1_s4));
	decodedLength = sawyercoding_decode_sv4(buffer, decodedBuffer, length);
	if (decodedLength == sizeof(rct1_s4)) {
		memcpy(s4, decodedBuffer, sizeof(rct1_s4));
		success = true;
	} else {
		success = false;
	}

	free(buffer);
	free(decodedBuffer);
	return success;
}

bool rideTypeShouldLoseSeparateFlag(rct_ride_entry *rideEntry)
{
	if (!gConfigInterface.select_by_track_type) {
		return false;
	}

	bool remove_flag = true;
	for (int j = 0; j < 3; j++) {
		if (ride_type_has_flag(rideEntry->ride_type[j], RIDE_TYPE_FLAG_FLAT_RIDE)) {
			remove_flag = false;
		}
		if (rideEntry->ride_type[j] == RIDE_TYPE_MAZE || rideEntry->ride_type[j] == RIDE_TYPE_MINI_GOLF) {
			remove_flag = false;
		}
	}
	return remove_flag;
}

const uint8 gRideCategories[] = {
	2,		 // Spiral Roller coaster
	2,		 // Stand Up Coaster
	2,		 // Suspended Swinging
	2,		 // Inverted
	2,		 // Steel Mini Coaster
	0,		 // Mini Railroad
	0,		 // Monorail
	2,		 // Mini Suspended Coaster
	4,		 // Boat ride
	2,		 // Wooden Wild Mine/Mouse
	2,		 // Steeplechase/Motorbike/Soap Box Derby
	1,		 // Car Ride
	3,		 // Launched Freefall
	2,		 // Bobsleigh Coaster
	1,		 // Observation Tower
	2,		 // Looping Roller Coaster
	4,		 // Dinghy Slide
	2,		 // Mine Train Coaster
	0,		 // Chairlift
	2,		 // Corkscrew Roller Coaster
	1,		 // Maze
	1,		 // Spiral Slide
	3,		 // Go Karts
	4,		 // Log Flume
	4,		 // River Rapids
	1,		 // Dodgems
	3,		 // Pirate Ship
	3,		 // Swinging Inverter Ship
	5,		 // Food Stall
	255,	 // (none)
	5,		 // Drink Stall
	255,	 // (none)
	5,		 // Shop (all types)
	1,		 // Merry Go Round
	5,		 // Balloon Stall (maybe)
	5,		 // Information Kiosk
	5,		 // Bathroom
	1,		 // Ferris Wheel
	3,		 // Motion Simulator
	3,		 // 3D Cinema
	3,		 // Top Spin
	1,		 // Space Rings
	2,		 // Reverse Freefall Coaster
	0,		 // Elevator
	2,		 // Vertical Drop Roller Coaster
	5,		 // ATM
	3,		 // Twist
	1,		 // Haunted House
	5,		 // First Aid
	1,		 // Circus Show
	1,		 // Ghost Train
	2,		 // Twister Roller Coaster
	2,		 // Wooden Roller Coaster
	2,		 // Side-Friction Roller Coaster
	2,		 // Wild Mouse
	2,		 // Multi Dimension Coaster
	255,	 // (none)
	2,		 // Flying Roller Coaster
	255,	 // (none)
	2,		 // Virginia Reel
	4,		 // Splash Boats
	1,		 // Mini Helicopters
	2,		 // Lay-down Roller Coaster
	0,		 // Suspended Monorail
	255,	 // (none)
	2,		 // Reverser Roller Coaster
	2,		 // Heartline Twister Roller Coaster
	1,		 // Mini Golf
	2,		 // Giga Coaster
	3,		 // Roto-Drop
	1,		 // Flying Saucers
	1,		 // Crooked House
	1,		 // Monorail Cycles
	2,		 // Compact Inverted Coaster
	2,		 // Water Coaster
	2,		 // Air Powered Vertical Coaster
	2,		 // Inverted Hairpin Coaster
	3,		 // Magic Carpet
	4,		 // Submarine Ride
	4,		 // River Rafts
	255,	 // (none)
	3,		 // Enterprise
	255,	 // (none)
	255,	 // (none)
	255,	 // (none)
	255,	 // (none)
	2,		 // Inverted Impulse Coaster
	2,		 // Mini Roller Coaster
	2,		 // Mine Ride
	255,	 //59 Unknown Ride
	2		 // LIM Launched Roller Coaster
};
