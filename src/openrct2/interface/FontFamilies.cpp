/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FontFamilies.h"

#include "Fonts.h"

#ifndef DISABLE_TTF

TTFontFamily const TTFFamilyChineseSimplified = {
    &TTFFontSimSun,
    &TTFFontHeiti,
    &TTFFontMicroHei,
    &TTFFontNotoSansCJK,
};

TTFontFamily const TTFFamilyChineseTraditional = {
    &TTFFontMingLiu,
    &TTFFontLiHeiPro,
    &TTFFontMicroHei,
    &TTFFontNotoSansCJK,
};

TTFontFamily const TTFFamilyJapanese = {
    &TTFFontHiragano,
    &TTFFontNotoSansMono,
    &TTFFontMSGothic,
    &TTFFontNotoSansCJK,
};

TTFontFamily const TTFFamilyKorean = {
    &TTFFontGulim,
    &TTFFontNanum,
    &TTFFontMalgun,
    &TTFFontNotoSansCJK,
};

TTFontFamily const TTFFamilyArabic = {
    &TTFFontArialUnicode,
    &TTFFontArial,
    &TTFFontNotoNaskhArabic,
};

TTFontFamily const TTFFamilySansSerif = {
    &TTFFontArialUnicode,
    &TTFFontArial,
    &TTFFontRoboto,
};

#endif // DISABLE_TTF
