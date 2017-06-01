#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../game.h"
#include "../ride/ride.h"
#include "../util/util.h"
#include "localisation.h"
#include "user.h"

utf8 gUserStrings[MAX_USER_STRINGS * USER_STRING_MAX_LENGTH];

static bool user_string_exists(const utf8 *text);

/**
 *
 *  rct2: 0x006C4209
 */
void user_string_clear_all()
{
	memset(gUserStrings, 0, MAX_USER_STRINGS * USER_STRING_MAX_LENGTH);
}

/**
 *
 *  rct2: 0x006C421D
 */
rct_string_id user_string_allocate(sint32 base, const utf8 *text)
{
	sint32 highBits = (base & 0x7F) << 9;
	bool allowDuplicates = base & 0x80;

	if (!allowDuplicates && user_string_exists(text)) {
		gGameCommandErrorText = STR_CHOSEN_NAME_IN_USE_ALREADY;
		return 0;
	}

	char *userString = gUserStrings;
	for (sint32 i = 0; i < MAX_USER_STRINGS; i++, userString += USER_STRING_MAX_LENGTH) {
		if (userString[0] != 0)
			continue;

		safe_strcpy(userString, text, USER_STRING_MAX_LENGTH);
		return 0x8000 + (i | highBits);
	}
	gGameCommandErrorText = STR_TOO_MANY_NAMES_DEFINED;
	return 0;
}

/**
 *
 *  rct2: 0x006C42AC
 */
void user_string_free(rct_string_id id)
{
	if (!is_user_string_id(id))
		return;

	id %= MAX_USER_STRINGS;
	gUserStrings[id * USER_STRING_MAX_LENGTH] = 0;
}

static bool user_string_exists(const utf8 *text)
{
	char *userString = gUserStrings;
	for (sint32 i = 0; i < MAX_USER_STRINGS; i++, userString += USER_STRING_MAX_LENGTH) {
		if (userString[0] == 0)
			continue;

		if (strcmp(userString, text) == 0)
			return true;
	}
	return false;
}

bool is_user_string_id(rct_string_id stringId)
{
	return stringId >= 0x8000 && stringId < 0x9000;
}

void reset_user_strings()
{
	char *userString = gUserStrings;

	for (sint32 i = 0; i < MAX_USER_STRINGS; i++, userString += USER_STRING_MAX_LENGTH) {
		userString[0] = 0;
	}

	ride_reset_all_names();
}
