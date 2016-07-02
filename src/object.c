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
#include "drawing/drawing.h"
#include "localisation/localisation.h"
#include "object.h"
#include "object_list.h"
#include "platform/platform.h"
#include "rct1.h"
#include "ride/ride.h"
#include "scenario.h"
#include "util/sawyercoding.h"
#include "world/entrance.h"
#include "world/footpath.h"
#include "world/scenery.h"
#include "world/water.h"

char gTempObjectLoadName[9] = { 0 };
uint32 gTotalNoImages = 0;

int object_load_entry(const utf8 *path, rct_object_entry *outEntry)
{
	SDL_RWops *file;

	file = SDL_RWFromFile(path, "rb");
	if (file == NULL)
		return 0;

	if (SDL_RWread(file, outEntry, sizeof(rct_object_entry), 1) != 1) {
		SDL_RWclose(file);
		return 0;
	}

	SDL_RWclose(file);
	return 1;
}

int object_entry_compare(const rct_object_entry *a, const rct_object_entry *b)
{
	// If an official object don't bother checking checksum
	if ((a->flags & 0xF0) || (b->flags & 0xF0)) {
		if ((a->flags & 0x0F) != (b->flags & 0x0F))
			return 0;
		int match = memcmp(a->name, b->name, 8);
		if (match)
			return 0;
	}
	else {
		if (a->flags != b->flags)
			return 0;
		int match = memcmp(a->name, b->name, 8);
		if (match)
			return 0;
		if (a->checksum != b->checksum)
			return 0;
	}

	return 1;
}

int object_calculate_checksum(const rct_object_entry *entry, const uint8 *data, int dataLength)
{
	const uint8 *entryBytePtr = (uint8*)entry;

	uint32 checksum = 0xF369A75B;
	checksum ^= entryBytePtr[0];
	checksum = rol32(checksum, 11);
	for (int i = 4; i < 12; i++) {
		checksum ^= entryBytePtr[i];
		checksum = rol32(checksum, 11);
	}
	for (int i = 0; i < dataLength; i++) {
		checksum ^= data[i];
		checksum = rol32(checksum, 11);
	}
	return (int)checksum;
}
