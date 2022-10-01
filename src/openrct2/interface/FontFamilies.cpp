/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FontFamilies.h"

#include "Fonts.h"

#ifndef NO_TTF

TTFontFamily const TTFFamilyChineseSimplified = {
    &TTFFontSimSun,
    &TTFFontHeiti,
    &TTFFontMicroHei,
};

TTFontFamily const TTFFamilyChineseTraditional = {
    &TTFFontMingLiu,
    &TTFFontLiHeiPro,
    &TTFFontMicroHei,
};

TTFontFamily const TTFFamilyJapanese = {
    &TTFFontHiragano,
    &TTFFontNotoSansMono,
    &TTFFontMSGothic,
};

TTFontFamily const TTFFamilyKorean = {
    &TTFFontGulim,
    &TTFFontNanum,
    &TTFFontMalgun,
};

TTFontFamily const TTFFamilySansSerif = {
    &TTFFontArialUnicode,
    &TTFFontArial,
};

#endif // NO_TTF
