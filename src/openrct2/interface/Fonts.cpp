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

#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../core/String.hpp"
#include "../localisation/LanguagePack.h"
#include "Fonts.h"

#include "../drawing/drawing.h"
#include "../drawing/ttf.h"
#include "../localisation/language.h"

#ifndef NO_TTF
TTFFontSetDescriptor TTFFontMSGothic = { {
    { "msgothic.ttc", "MS PGothic", 9, 1, 0, 15, nullptr },
    { "msgothic.ttc", "MS PGothic", 12, 1, 0, 17, nullptr },
    { "msgothic.ttc", "MS PGothic", 12, 1, 0, 17, nullptr },
    { "msgothic.ttc", "MS PGothic", 13, 1, 0, 20, nullptr },
} };

TTFFontSetDescriptor TTFFontMingLiu = { {
    { "msjh.ttc", "JhengHei", 9, -1, -3, 6, nullptr },
    { "mingliu.ttc", "MingLiU", 11, 1, 1, 12, nullptr },
    { "mingliu.ttc", "MingLiU", 12, 1, 0, 12, nullptr },
    { "mingliu.ttc", "MingLiU", 13, 1, 0, 20, nullptr },
} };

TTFFontSetDescriptor TTFFontSimSun = { {
    { "msyh.ttc", "YaHei", 9, -1, -3, 6, nullptr },
    { "simsun.ttc", "SimSun", 11, 1, -1, 14, nullptr },
    { "simsun.ttc", "SimSun", 12, 1, -2, 14, nullptr },
    { "simsun.ttc", "SimSun", 13, 1, 0, 20, nullptr },
} };

TTFFontSetDescriptor TTFFontGulim = { {
    { "gulim.ttc", "Gulim", 11, 1, 0, 15, nullptr },
    { "gulim.ttc", "Gulim", 12, 1, 0, 17, nullptr },
    { "gulim.ttc", "Gulim", 12, 1, 0, 17, nullptr },
    { "gulim.ttc", "Gulim", 13, 1, 0, 20, nullptr },
} };

TTFFontSetDescriptor TTFFontArial = { {
    { "arial.ttf", "Arial", 8, 0, -1, 6, nullptr },
    { "arial.ttf", "Arial", 10, 0, -1, 12, nullptr },
    { "arial.ttf", "Arial", 11, 0, -1, 12, nullptr },
    { "arial.ttf", "Arial", 12, 0, -1, 20, nullptr },
} };
#endif // NO_TTF

static void LoadSpriteFont()
{
    ttf_dispose();
    gUseTrueTypeFont   = false;
#ifndef NO_TTF
    gCurrentTTFFontSet = nullptr;
#endif // NO_TTF
}

#ifndef NO_TTF
static bool LoadFont(TTFFontSetDescriptor * font)
{
    gUseTrueTypeFont   = true;
    gCurrentTTFFontSet = font;

    ttf_dispose();
    bool fontInitialised = ttf_initialise();
    return fontInitialised;
}

static bool LoadCustomConfigFont()
{
    static TTFFontSetDescriptor TTFFontCustom = { {
        { gConfigFonts.file_name, gConfigFonts.font_name, gConfigFonts.size_tiny, gConfigFonts.x_offset, gConfigFonts.y_offset,
          gConfigFonts.height_tiny, nullptr },
        { gConfigFonts.file_name, gConfigFonts.font_name, gConfigFonts.size_small, gConfigFonts.x_offset, gConfigFonts.y_offset,
          gConfigFonts.height_small, nullptr },
        { gConfigFonts.file_name, gConfigFonts.font_name, gConfigFonts.size_medium, gConfigFonts.x_offset,
          gConfigFonts.y_offset, gConfigFonts.height_medium, nullptr },
        { gConfigFonts.file_name, gConfigFonts.font_name, gConfigFonts.size_big, gConfigFonts.x_offset, gConfigFonts.y_offset,
          gConfigFonts.height_big, nullptr },
    } };

    ttf_dispose();
    gUseTrueTypeFont   = true;
    gCurrentTTFFontSet = &TTFFontCustom;

    bool fontInitialised = ttf_initialise();
    return fontInitialised;
}
#endif // NO_TTF

void TryLoadFonts()
{
#ifndef NO_TTF
    TTFFontSetDescriptor * font = LanguagesDescriptors[gCurrentLanguage].font;
    if (font != FONT_OPENRCT2_SPRITE)
    {
        if (!String::IsNullOrEmpty(gConfigFonts.file_name))
        {
            if (LoadCustomConfigFont())
            {
                return;
            }
            Console::Error::WriteLine("Unable to initialise configured TrueType font -- falling back to Language default.");
        }

        if (LoadFont(font))
        {
            return;
        }
        Console::Error::WriteLine("Unable to initialise preferred TrueType font -- falling back to Arial.");

        if (LoadFont(&TTFFontArial))
        {
            return;
        }
        Console::Error::WriteLine("Unable to initialise preferred TrueType font -- Falling back to sprite font.");
    }
#endif // NO_TTF
    LoadSpriteFont();
}
