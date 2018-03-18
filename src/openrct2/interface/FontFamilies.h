#pragma region Copyright(c) 2014 - 2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef OPENRCT2_FONT_FAMILIES_H
#define OPENRCT2_FONT_FAMILIES_H

#define FAMILY_OPENRCT2_SPRITE nullptr

#ifndef NO_TTF

#include <vector>

struct TTFFontSetDescriptor;

using TTFontFamily = std::vector<TTFFontSetDescriptor *>;

extern TTFontFamily const TTFFamilyChineseSimplified;
extern TTFontFamily const TTFFamilyChineseTraditional;
extern TTFontFamily const TTFFamilyJapanese;
extern TTFontFamily const TTFFamilyKorean;
extern TTFontFamily const TTFFamilySansSerif;

#define FAMILY(x) x

#else  // NO_TTF

#define FAMILY(x) FAMILY_OPENRCT2_SPRITE

#endif // NO_TTF

#endif // OPENRCT2_FONT_FAMILIES_H
