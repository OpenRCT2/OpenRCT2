/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Fonts.h"
#include "FontFamilies.h"

#ifndef NO_TTF

TTFontFamily const TTFFamilyChineseSimplified =
{
    &TTFFontSimSun,
    &TTFFontHeiti,
};

TTFontFamily const TTFFamilyChineseTraditional =
{
    &TTFFontMingLiu,
    &TTFFontLiHeiPro,
};

TTFontFamily const TTFFamilyJapanese =
{
    &TTFFontHiragano,
    &TTFFontMSGothic,
};

TTFontFamily const TTFFamilyKorean =
{
    &TTFFontGulim,
    &TTFFontNanum,
};

TTFontFamily const TTFFamilySansSerif =
{
    &TTFFontArialUnicode,
    &TTFFontArial,
};

#endif // NO_TTF
