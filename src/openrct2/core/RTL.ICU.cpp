/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _WIN32
    #include "RTL.h"

    #include <algorithm>
    #include <string>
    #include <unicode/ubidi.h>
    #include <unicode/unistr.h>
    #include <unicode/ushape.h>
    #include <unicode/ustring.h>
    #include <unicode/utf.h>
    #include <unicode/utypes.h>

std::string FixRTL(std::string& input)
{
    UErrorCode err = static_cast<UErrorCode>(0);
    // Force a hard left-to-right at the beginning (will mess up mixed strings' word order otherwise)
    std::string text2 = std::string(u8"\xE2\x80\xAA") + input;

    icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(text2));

    int32_t length = ustr.length();
    icu::UnicodeString reordered;
    icu::UnicodeString shaped;
    UBiDi* bidi = ubidi_openSized(length, 0, &err);
    // UBIDI_DEFAULT_LTR preserves formatting codes.
    ubidi_setPara(bidi, ustr.getBuffer(), length, UBIDI_DEFAULT_LTR, nullptr, &err);
    ubidi_writeReordered(bidi, reordered.getBuffer(length), length, UBIDI_DO_MIRRORING | UBIDI_REMOVE_BIDI_CONTROLS, &err);
    ubidi_close(bidi);
    reordered.releaseBuffer(length);
    u_shapeArabic(
        reordered.getBuffer(), length, shaped.getBuffer(length), length,
        U_SHAPE_LETTERS_SHAPE | U_SHAPE_LENGTH_FIXED_SPACES_NEAR | U_SHAPE_TEXT_DIRECTION_VISUAL_LTR, &err);
    shaped.releaseBuffer(length);

    std::string cppstring;
    shaped.toUTF8String(cppstring);

    // libicu seems to leave a null terminator on the end
    cppstring.erase(std::find(cppstring.begin(), cppstring.end(), '\0'), cppstring.end());

    return cppstring;
}
#endif
