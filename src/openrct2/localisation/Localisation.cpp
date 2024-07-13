/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Localisation.h"

#include "../Diagnostic.h"
#include "../Game.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../core/String.hpp"
#include "../ride/Ride.h"
#include "../util/Util.h"
#include "Currency.h"
#include "FormatCodes.h"
#include "Formatting.h"
#include "Localisation.Date.h"
#include "StringIds.h"

#include <cstring>

using namespace OpenRCT2;

thread_local char gCommonStringFormatBuffer[CommonTextBufferSize];

// clang-format off
const StringId RideInspectionIntervalNames[] = {
    STR_EVERY_10_MINUTES,
    STR_EVERY_20_MINUTES,
    STR_EVERY_30_MINUTES,
    STR_EVERY_45_MINUTES,
    STR_EVERY_HOUR,
    STR_EVERY_2_HOURS,
    STR_NEVER,
};
// clang-format on

std::string FormatStringIDLegacy(StringId format, const void* args)
{
    std::string buffer(256, 0);
    size_t len{};
    for (;;)
    {
        OpenRCT2::FormatStringLegacy(buffer.data(), buffer.size(), format, args);
        len = buffer.find('\0');
        if (len == std::string::npos)
        {
            len = buffer.size();
        }
        if (len >= buffer.size() - 1)
        {
            // Null terminator to close to end of buffer, grow buffer and try again
            buffer.resize(buffer.size() * 2);
        }
        else
        {
            buffer.resize(len);
            break;
        }
    }
    return buffer;
}

/**
 * Writes a formatted string to a buffer and converts it to upper case.
 *  rct2: 0x006C2538
 * dest (edi)
 * format (ax)
 * args (ecx)
 */
void FormatStringToUpper(utf8* dest, size_t size, StringId format, const void* args)
{
    if (size == 0)
    {
        return;
    }

    OpenRCT2::FormatStringLegacy(dest, size, format, args);

    std::string upperString = String::ToUpper(dest);

    if (upperString.size() + 1 >= size)
    {
        upperString.resize(size - 1);
        dest[size - 1] = '\0';
        LOG_WARNING("Truncating formatted string \"%s\" to %d bytes.", dest, size);
    }

    upperString.copy(dest, upperString.size());
    dest[upperString.size()] = '\0';
}
