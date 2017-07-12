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

extern "C"
{
    #include "../drawing/drawing.h"
    #include "../drawing/ttf.h"
    #include "../localisation/language.h"
}

#ifndef NO_TTF
static TTFFontSetDescriptor TTFFontMSGothic = { {
    { "msgothic.ttc", "MS PGothic", 9, 1, 0, 15, nullptr },
    { "msgothic.ttc", "MS PGothic", 12, 1, 0, 17, nullptr },
    { "msgothic.ttc", "MS PGothic", 12, 1, 0, 17, nullptr },
    { "msgothic.ttc", "MS PGothic", 13, 1, 0, 20, nullptr },
} };

static TTFFontSetDescriptor TTFFontMingLiu = { {
    { "msjh.ttc", "JhengHei", 9, -1, -3, 6, nullptr },
    { "mingliu.ttc", "MingLiU", 11, 1, 1, 12, nullptr },
    { "mingliu.ttc", "MingLiU", 12, 1, 0, 12, nullptr },
    { "mingliu.ttc", "MingLiU", 13, 1, 0, 20, nullptr },
} };

static TTFFontSetDescriptor TTFFontSimSun = { {
    { "msyh.ttc", "YaHei", 9, -1, -3, 6, nullptr },
    { "simsun.ttc", "SimSun", 11, 1, -1, 14, nullptr },
    { "simsun.ttc", "SimSun", 12, 1, -2, 14, nullptr },
    { "simsun.ttc", "SimSun", 13, 1, 0, 20, nullptr },
} };

static TTFFontSetDescriptor TTFFontGulim = { {
    { "gulim.ttc", "Gulim", 11, 1, 0, 15, nullptr },
    { "gulim.ttc", "Gulim", 12, 1, 0, 17, nullptr },
    { "gulim.ttc", "Gulim", 12, 1, 0, 17, nullptr },
    { "gulim.ttc", "Gulim", 13, 1, 0, 20, nullptr },
} };

static TTFFontSetDescriptor TTFFontArial = { {
    { "arial.ttf", "Arial", 8, 0, -1, 6, nullptr },
    { "arial.ttf", "Arial", 10, 0, -1, 12, nullptr },
    { "arial.ttf", "Arial", 11, 0, -1, 12, nullptr },
    { "arial.ttf", "Arial", 12, 0, -1, 20, nullptr },
} };
#define FONT(x) x
#else
#define FONT(x) FONT_OPENRCT2_SPRITE
#endif // NO_TTF

const language_descriptor LanguagesDescriptors[LANGUAGE_COUNT] = {
    { "", "", "", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_ENGLISH_UK },                              // LANGUAGE_UNDEFINED
    { "en-GB", "English (UK)", "English (UK)", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_ENGLISH_UK }, // LANGUAGE_ENGLISH_UK
    { "en-US", "English (US)", "English (US)", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_ENGLISH_US }, // LANGUAGE_ENGLISH_US
    { "de-DE", "German", "Deutsch", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_GERMAN },                // LANGUAGE_GERMAN
    { "nl-NL", "Dutch", "Nederlands", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_DUTCH },               // LANGUAGE_DUTCH
    { "fr-FR", "French", u8"Français", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_FRENCH },             // LANGUAGE_FRENCH
    { "hu-HU", "Hungarian", "Magyar", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_ENGLISH_UK },          // LANGUAGE_HUNGARIAN
    { "pl-PL", "Polish", "Polski", FONT(&TTFFontArial), RCT2_LANGUAGE_ID_ENGLISH_UK },              // LANGUAGE_POLISH
    { "es-ES", "Spanish", u8"Español", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_SPANISH },            // LANGUAGE_SPANISH
    { "sv-SE", "Swedish", "Svenska", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_SWEDISH },              // LANGUAGE_SWEDISH
    { "it-IT", "Italian", "Italiano", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_ITALIAN },             // LANGUAGE_ITALIAN
    { "pt-BR", "Portuguese (BR)", u8"Português (BR)", FONT_OPENRCT2_SPRITE,
      RCT2_LANGUAGE_ID_PORTUGUESE }, // LANGUAGE_PORTUGUESE_BR
    { "zh-TW", "Chinese (Traditional)", "Chinese (Traditional)", FONT(&TTFFontMingLiu),
      RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL }, // LANGUAGE_CHINESE_TRADITIONAL
    { "zh-CN", "Chinese (Simplified)", "Chinese (Simplified)", FONT(&TTFFontSimSun),
      RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED },                                                  // LANGUAGE_CHINESE_SIMPLIFIED
    { "fi-FI", "Finnish", "Suomi", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_ENGLISH_UK },       // LANGUAGE_FINNISH
    { "ko-KR", "Korean", "Korean", FONT(&TTFFontGulim), RCT2_LANGUAGE_ID_KOREAN },            // LANGUAGE_KOREAN
    { "ru-RU", "Russian", "Russian", FONT(&TTFFontArial), RCT2_LANGUAGE_ID_ENGLISH_UK },      // LANGUAGE_RUSSIAN
    { "cs-CZ", "Czech", "Czech", FONT(&TTFFontArial), RCT2_LANGUAGE_ID_ENGLISH_UK },          // LANGUAGE_CZECH
    { "ja-JP", "Japanese", "Japanese", FONT(&TTFFontMSGothic), RCT2_LANGUAGE_ID_ENGLISH_UK }, // LANGUAGE_JAPANESE
    { "nb-NO", "Norwegian", "Norsk", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_ENGLISH_UK },     // LANGUAGE_NORWEGIAN
    { "ca-ES", "Catalan", u8"Català", FONT_OPENRCT2_SPRITE, RCT2_LANGUAGE_ID_SPANISH },       // LANGUAGE_CATALAN
};

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
