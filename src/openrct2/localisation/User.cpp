/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Game.h"
#include "../ride/Ride.h"
#include "../util/Util.h"
#include "Localisation.h"
#include "User.h"

utf8 gUserStrings[MAX_USER_STRINGS][USER_STRING_MAX_LENGTH];

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
rct_string_id user_string_allocate(int32_t base, const utf8 *text)
{
    int32_t highBits = (base & 0x7F) << 9;
    bool allowDuplicates = base & USER_STRING_DUPLICATION_PERMITTED;

    if (!allowDuplicates && user_string_exists(text)) {
        gGameCommandErrorText = STR_CHOSEN_NAME_IN_USE_ALREADY;
        return 0;
    }

    for (int32_t i = 0; i < MAX_USER_STRINGS; i++)
    {
        char * userString = gUserStrings[i];

        if (userString[0] != 0)
            continue;

        safe_strcpy(userString, text, USER_STRING_MAX_LENGTH);
        return USER_STRING_START + (i | highBits);
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
    gUserStrings[id][0] = 0;
}

static bool user_string_exists(const utf8 *text)
{
    char * userString;
    for (int32_t i = 0; i < MAX_USER_STRINGS; i++)
    {
        userString = gUserStrings[i];
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
    for (int32_t i = 0; i < MAX_USER_STRINGS; i++)
    {
        gUserStrings[i][0] = 0;
    }

    ride_reset_all_names();
}
