/*****************************************************************************
 * Copyright (c) 2014 Ted John
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

#include "../addresses.h"
#include "localisation.h"

char *gUserStrings = (char*)0x0135A8F4;

static bool user_string_exists(const char *text);

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
rct_string_id user_string_allocate(int base, const char *text)
{
	int highBits = (base & 0x7F) << 9;
	bool allowDuplicates = base & 0x80;

	if (!allowDuplicates && user_string_exists(text)) {
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_CHOSEN_NAME_IN_USE_ALREADY;
		return 0;
	}

	char *userString = gUserStrings;
	for (int i = 0; i < MAX_USER_STRINGS; i++, userString += USER_STRING_MAX_LENGTH) {
		if (userString[0] != 0)
			continue;

		strncpy(userString, text, USER_STRING_MAX_LENGTH - 1);
		return 0x8000 + (i | highBits);
	}
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id) = STR_TOO_MANY_NAMES_DEFINED;
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

static bool user_string_exists(const char *text)
{
	char *userString = gUserStrings;
	for (int i = 0; i < MAX_USER_STRINGS; i++, userString += USER_STRING_MAX_LENGTH) {
		if (userString[0] == 0)
			continue;

		if (_stricmp(userString, text) == 0)
			return true;
	}
	return false;
}

bool is_user_string_id(rct_string_id stringId)
{
	return stringId >= 0x8000 && stringId < 0x9000;
}