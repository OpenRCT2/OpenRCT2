/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#define FAMILY_OPENRCT2_SPRITE nullptr

#ifndef NO_TTF
#    include <vector>
namespace OpenRCT2
{

    struct TTFFontSetDescriptor;

    using TTFontFamily = std::vector<TTFFontSetDescriptor*>;

    extern TTFontFamily const TTFFamilyChineseSimplified;
    extern TTFontFamily const TTFFamilyChineseTraditional;
    extern TTFontFamily const TTFFamilyJapanese;
    extern TTFontFamily const TTFFamilyKorean;
    extern TTFontFamily const TTFFamilySansSerif;

} // namespace OpenRCT2
#    define FAMILY(x) x

#else // NO_TTF

#    define FAMILY(x) FAMILY_OPENRCT2_SPRITE

#endif // NO_TTF
