/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Fonts.h"

#include "../config/Config.h"
#include "../core/String.hpp"
#include "../drawing/TTF.h"
#include "../localisation/Language.h"
#include "../localisation/LocalisationService.h"
#include "FontFamilies.h"

using namespace OpenRCT2::Localisation;

#ifndef NO_TTF
uint8_t const HINTING_DISABLED = 0;
uint8_t const HINTING_THRESHOLD_LOW = 40;
uint8_t const HINTING_THRESHOLD_MEDIUM = 60;

// clang-format off
TTFFontSetDescriptor TTFFontMSGothic = { {
    { u8"msgothic.ttc", u8"MS PGothic",  9, 1,  1,  9, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"msgothic.ttc", u8"MS PGothic", 12, 1,  0, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"msgothic.ttc", u8"MS PGothic", 12, 1,  0, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontNotoSansMono = { {
    { u8"NotoSansCJK-Regular.ttc", u8"Noto Sans Mono CJK JP",  9, 1, -2,  9, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"NotoSansCJK-Regular.ttc", u8"Noto Sans Mono CJK JP", 12, 1, -3, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"NotoSansCJK-Regular.ttc", u8"Noto Sans Mono CJK JP", 12, 1, -3, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontHiragano = { {
    { u8"ヒラギノ丸ゴ ProN W4.ttc", u8"Hiragino Maru Gothic ProN",  9, 1,  1,  9, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"ヒラギノ丸ゴ ProN W4.ttc", u8"Hiragino Maru Gothic ProN", 11, 1,  0, 13, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"ヒラギノ丸ゴ ProN W4.ttc", u8"Hiragino Maru Gothic ProN", 11, 1,  0, 13, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontMingLiu = { {
    {    u8"msjh.ttc", u8"JhengHei",  9, -1, -1,  9, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"mingliu.ttc", u8"MingLiU", 11,  1,  1, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"mingliu.ttc", u8"MingLiU", 12,  1,  0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontHeiti = { {
    { u8"华文黑体.ttf", u8"STHeiti",  9, -1, -1,  9, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"华文黑体.ttf", u8"STHeiti", 11,  1,  1, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"华文黑体.ttf", u8"STHeiti", 12,  1,  0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontSimSun = { {
    {   u8"msyh.ttc", u8"Microsoft YaHei",  9, -1, -1,  9, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"simsun.ttc", u8"SimSun", 11,  1, -1, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"simsun.ttc", u8"SimSun", 12,  1, -2, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontLiHeiPro = { {
    { u8"儷黑 Pro.ttf", u8"LiHei Pro",  9, 1, -1,  9, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"儷黑 Pro.ttf", u8"LiHei Pro", 11, 1,  0, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"儷黑 Pro.ttf", u8"LiHei Pro", 12, 1,  0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontGulim = { {
    { u8"gulim.ttc", u8"Gulim", 10, 1, 0, 10, HINTING_DISABLED,         nullptr },
    { u8"gulim.ttc", u8"Gulim", 12, 1, 0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"gulim.ttc", u8"Gulim", 12, 1, 0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },
} };

TTFFontSetDescriptor TTFFontMalgun = { {
    { u8"malgun.ttf", u8"Malgun Gothic", 10, 1, -3, 10, HINTING_THRESHOLD_LOW, nullptr },
    { u8"malgun.ttf", u8"Malgun Gothic", 12, 1, -3, 15, HINTING_THRESHOLD_LOW, nullptr },
    { u8"malgun.ttf", u8"Malgun Gothic", 12, 1, -3, 15, HINTING_THRESHOLD_LOW, nullptr },
} };

TTFFontSetDescriptor TTFFontNanum = { {
    { u8"NanumGothic.ttf", u8"Nanum Gothic", 10, 1, -2, 10, HINTING_DISABLED, nullptr },
    { u8"NanumGothic.ttf", u8"Nanum Gothic", 12, 1, -2, 15, HINTING_THRESHOLD_LOW, nullptr },
    { u8"NanumGothic.ttf", u8"Nanum Gothic", 12, 1, -2, 15, HINTING_THRESHOLD_LOW, nullptr },
} };

TTFFontSetDescriptor TTFFontArial = { {
    { u8"arial.ttf", u8"Arial", 10, 0, -1,  9, HINTING_THRESHOLD_LOW, nullptr },
    { u8"arial.ttf", u8"Arial", 12, 0, -1, 12, HINTING_THRESHOLD_LOW, nullptr },
    { u8"arial.ttf", u8"Arial", 12, 0, -1, 12, HINTING_THRESHOLD_LOW, nullptr },
} };

TTFFontSetDescriptor TTFFontArialUnicode = { {
    { u8"arialuni.ttf", u8"Arial Unicode MS", 10, 0, -1,  9, HINTING_THRESHOLD_LOW, nullptr },
    { u8"arialuni.ttf", u8"Arial Unicode MS", 12, 0, -1, 12, HINTING_THRESHOLD_LOW, nullptr },
    { u8"arialuni.ttf", u8"Arial Unicode MS", 12, 0, -1, 12, HINTING_THRESHOLD_LOW, nullptr },
} };

TTFFontSetDescriptor TTFFontMicroHei = { {
    { u8"wqy-microhei.ttc", u8"WenQuanYi Micro Hei", 10, -1, -1,  10, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"wqy-microhei.ttc", u8"WenQuanYi Micro Hei", 11,  1, -1, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
    { u8"wqy-microhei.ttc", u8"WenQuanYi Micro Hei", 11,  1, -2, 14, HINTING_THRESHOLD_MEDIUM, nullptr },
} };
// clang-format on
#endif // NO_TTF

static void LoadSpriteFont(LocalisationService& localisationService)
{
    ttf_dispose();
    localisationService.UseTrueTypeFont(false);
#ifndef NO_TTF
    gCurrentTTFFontSet = nullptr;
#endif // NO_TTF
}

#ifndef NO_TTF
static bool LoadFont(LocalisationService& localisationService, TTFFontSetDescriptor* font)
{
    localisationService.UseTrueTypeFont(true);
    gCurrentTTFFontSet = font;

    ttf_dispose();
    bool fontInitialised = ttf_initialise();
    return fontInitialised;
}

static bool LoadCustomConfigFont(LocalisationService& localisationService)
{
    static TTFFontSetDescriptor TTFFontCustom = { {
        { gConfigFonts.file_name, gConfigFonts.font_name, gConfigFonts.size_tiny, gConfigFonts.x_offset, gConfigFonts.y_offset,
          gConfigFonts.height_tiny, gConfigFonts.hinting_threshold, nullptr },
        { gConfigFonts.file_name, gConfigFonts.font_name, gConfigFonts.size_small, gConfigFonts.x_offset, gConfigFonts.y_offset,
          gConfigFonts.height_small, gConfigFonts.hinting_threshold, nullptr },
        { gConfigFonts.file_name, gConfigFonts.font_name, gConfigFonts.size_medium, gConfigFonts.x_offset,
          gConfigFonts.y_offset, gConfigFonts.height_medium, gConfigFonts.hinting_threshold, nullptr },
    } };

    ttf_dispose();
    localisationService.UseTrueTypeFont(true);
    gCurrentTTFFontSet = &TTFFontCustom;

    bool fontInitialised = ttf_initialise();
    return fontInitialised;
}
#endif // NO_TTF

void TryLoadFonts(LocalisationService& localisationService)
{
#ifndef NO_TTF
    auto currentLanguage = localisationService.GetCurrentLanguage();
    TTFontFamily const* fontFamily = LanguagesDescriptors[currentLanguage].font_family;

    if (fontFamily != FAMILY_OPENRCT2_SPRITE)
    {
        if (!String::IsNullOrEmpty(gConfigFonts.file_name))
        {
            if (LoadCustomConfigFont(localisationService))
            {
                return;
            }
            log_verbose("Unable to initialise configured TrueType font -- falling back to the language's default.");
        }

        for (auto& font : *fontFamily)
        {
            if (LoadFont(localisationService, font))
            {
                return;
            }

            TTFFontDescriptor smallFont = font->size[FONT_SIZE_SMALL];
            log_verbose("Unable to load TrueType font '%s' -- trying the next font in the family.", smallFont.font_name);
        }

        if (fontFamily != &TTFFamilySansSerif)
        {
            log_verbose("Unable to initialise any of the preferred TrueType fonts -- falling back to sans serif fonts.");

            for (auto& font : TTFFamilySansSerif)
            {
                if (LoadFont(localisationService, font))
                {
                    return;
                }

                TTFFontDescriptor smallFont = font->size[FONT_SIZE_SMALL];
                log_verbose("Unable to load TrueType font '%s' -- trying the next font in the family.", smallFont.font_name);
            }

            log_verbose("Unable to initialise any of the preferred TrueType fonts -- falling back to sprite font.");
        }
    }
#endif // NO_TTF
    LoadSpriteFont(localisationService);
}
