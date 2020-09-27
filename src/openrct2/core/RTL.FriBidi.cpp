/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(_WIN32) && defined(USE_FRIBIDI)
#    include "RTL.h"

#    include <string>

extern "C" {
#    include <fribidi/fribidi-bidi-types.h>
#    include <fribidi/fribidi-char-sets.h>
#    include <fribidi/fribidi-flags.h>
#    include <fribidi/fribidi.h>
}

static constexpr const uint16_t BufferLength = 1024;

std::string FixRTL(std::string& input)
{
    FriBidiChar utf32String[BufferLength] = { 0 };
    auto len = static_cast<FriBidiStrIndex>(input.length() + 1);
    fribidi_charset_to_unicode(FRIBIDI_CHAR_SET_UTF8, input.c_str(), len, utf32String);

    FriBidiStrIndex utf32len = 0;
    for (; utf32len < static_cast<FriBidiStrIndex>(std::size(utf32String)); utf32len++)
    {
        if (utf32String[utf32len] == 0)
        {
            break;
        }
    }

    FriBidiChar reorderedStr[BufferLength] = { 0 };
    // All our strings start in LTR direction due to the "STR_0001: prefix", even fully Arabic ones.
    FriBidiCharType pbase_dir = FRIBIDI_TYPE_LTR;

    fribidi_log2vis(utf32String, utf32len, &pbase_dir, reorderedStr, nullptr, nullptr, nullptr);

    char outputString[BufferLength];
    fribidi_unicode_to_charset(FRIBIDI_CHAR_SET_UTF8, reorderedStr, len, outputString);

    return std::string(outputString);
}
#elif defined(_WIN32)
#    include "../Diagnostic.h"
#    include "RTL.h"

#    include <string>

static bool hasWarned = false;

std::string FixRTL(std::string& input)
{
    if (!hasWarned)
    {
        log_warning("This build of OpenRCT2 cannot properly handle right-to-left text!");
        hasWarned = true;
    }

    return input;
}
#endif
