/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

constexpr auto kFamilyOpenRCT2Sprite = nullptr;

#ifndef NO_TTF

#    include <vector>

struct TTFFontSetDescriptor;

using TTFontFamily = std::vector<TTFFontSetDescriptor*>;

extern TTFontFamily const TTFFamilyChineseSimplified;
extern TTFontFamily const TTFFamilyChineseTraditional;
extern TTFontFamily const TTFFamilyJapanese;
extern TTFontFamily const TTFFamilyKorean;
extern TTFontFamily const TTFFamilySansSerif;

constexpr TTFontFamily const* Family(TTFontFamily const* x)
{
    return x;
}

#else // NO_TTF

constexpr auto Famil(auto x)
{
    return kFamilyOpenRCT2Sprite;
}

#endif // NO_TTF
