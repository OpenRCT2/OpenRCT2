/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Font.h"

#include "../Diagnostic.h"
#include "../SpriteIds.h"
#include "../core/EnumUtils.hpp"
#include "../core/UTF8.h"
#include "../core/UnicodeChar.h"
#include "../localisation/LocalisationService.h"
#include "../rct12/CSChar.h"
#include "Colour.h"
#include "Drawing.Sprite.h"
#include "ScrollingText.h"
#include "TTF.h"

#include <iterator>
#include <limits>
#include <unordered_map>

using namespace OpenRCT2;

static constexpr int32_t kSpriteFontLineHeight[kFontStyleCount] = {
    10,
    10,
    6,
};

static uint8_t _spriteFontCharacterWidths[kFontStyleCount][SPR_FONTS_GLYPH_COUNT] = {};

#ifndef DISABLE_TTF
TTFFontSetDescriptor* gCurrentTTFFontSet;
#endif // DISABLE_TTF

constexpr uint8_t kCSSpriteFontOffset = 32;

static const std::unordered_map<UnicodeChar, int32_t> kCodepointOffsetMap = {
    { UnicodeChar::aeUc, SPR_FONTS_AE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::oStrokeUc, SPR_FONTS_O_STROKE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::yAcuteUc, SPR_FONTS_Y_ACUTE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::ae, SPR_FONTS_AE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::oStroke, SPR_FONTS_O_STROKE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::yAcute, SPR_FONTS_Y_ACUTE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::aBreveUc, SPR_FONTS_A_BREVE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::aBreve, 226 - kCSSpriteFontOffset }, // Render as â, no visual difference in the RCT font
    { UnicodeChar::aOgonekUc, EnumValue(CSChar::aOgonekUc) - kCSSpriteFontOffset },
    { UnicodeChar::aOgonek, EnumValue(CSChar::aOgonek) - kCSSpriteFontOffset },
    { UnicodeChar::cAcuteUc, EnumValue(CSChar::cAcuteUc) - kCSSpriteFontOffset },
    { UnicodeChar::cAcute, EnumValue(CSChar::cAcute) - kCSSpriteFontOffset },
    { UnicodeChar::cCaronUc, SPR_FONTS_C_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cCaron, SPR_FONTS_C_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::dCaronUc, SPR_FONTS_D_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::dCaron, SPR_FONTS_D_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::eOgonekUc, EnumValue(CSChar::eOgonekUc) - kCSSpriteFontOffset },
    { UnicodeChar::eOgonek, EnumValue(CSChar::eOgonek) - kCSSpriteFontOffset },
    { UnicodeChar::eCaronUc, SPR_FONTS_E_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::eCaron, SPR_FONTS_E_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::gBreveUc, SPR_FONTS_G_BREVE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::gBreve, SPR_FONTS_G_BREVE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::iWithDotUc, SPR_FONTS_I_WITH_DOT_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::iWithoutDot, SPR_FONTS_I_WITHOUT_DOT_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::lStrokeUc, EnumValue(CSChar::lStrokeUc) - kCSSpriteFontOffset },
    { UnicodeChar::lStroke, EnumValue(CSChar::lStroke) - kCSSpriteFontOffset },
    { UnicodeChar::nAcuteUc, EnumValue(CSChar::nAcuteUc) - kCSSpriteFontOffset },
    { UnicodeChar::nAcute, EnumValue(CSChar::nAcute) - kCSSpriteFontOffset },
    { UnicodeChar::nCaronUc, SPR_FONTS_N_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::nCaron, SPR_FONTS_N_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::oMacron, EnumValue(CSChar::oCircumflex) - kCSSpriteFontOffset }, // No visual difference
    { UnicodeChar::oDoubleAcuteUc, SPR_FONTS_O_DOUBLE_ACUTE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::oDoubleAcute, SPR_FONTS_O_DOUBLE_ACUTE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::oeUc, SPR_FONTS_OE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::oe, SPR_FONTS_OE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::rCaronUc, SPR_FONTS_R_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::rCaron, SPR_FONTS_R_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::sAcuteUc, EnumValue(CSChar::sAcuteUc) - kCSSpriteFontOffset },
    { UnicodeChar::sAcute, EnumValue(CSChar::sAcute) - kCSSpriteFontOffset },
    { UnicodeChar::sCedillaUc, SPR_FONTS_S_CEDILLA_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::sCedilla, SPR_FONTS_S_CEDILLA_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::sCaronUc, SPR_FONTS_S_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::sCaron, SPR_FONTS_S_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::tCaronUc, SPR_FONTS_T_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::tCaron, SPR_FONTS_T_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::uRingUc, SPR_FONTS_U_RING_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::uRing, SPR_FONTS_U_RING_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::uDoubleAcuteUc, SPR_FONTS_U_DOUBLE_ACUTE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::uDoubleAcute, SPR_FONTS_U_DOUBLE_ACUTE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::wCircumflexUc, SPR_FONTS_W_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::wCircumflex, SPR_FONTS_W_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::yCircumflexUc, SPR_FONTS_Y_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::yCircumflex, SPR_FONTS_Y_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::zAcuteUc, EnumValue(CSChar::zAcuteUc) - kCSSpriteFontOffset },
    { UnicodeChar::zAcute, EnumValue(CSChar::zAcute) - kCSSpriteFontOffset },
    { UnicodeChar::zDotUc, EnumValue(CSChar::zDotUc) - kCSSpriteFontOffset },
    { UnicodeChar::zDot, EnumValue(CSChar::zDot) - kCSSpriteFontOffset },
    { UnicodeChar::zCaronUc, SPR_FONTS_Z_CARON_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::zCaron, SPR_FONTS_Z_CARON_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::fWithHookUc, 'F' - kCSSpriteFontOffset },
    { UnicodeChar::sCommaUc, SPR_FONTS_S_CEDILLA_UPPER - SPR_FONTS_BEGIN }, // No visual difference
    { UnicodeChar::sComma, SPR_FONTS_S_CEDILLA_LOWER - SPR_FONTS_BEGIN },   // Ditto
    { UnicodeChar::tCommaUc, SPR_FONTS_T_COMMA_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::tComma, SPR_FONTS_T_COMMA_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::sharpSUc, 223 - kCSSpriteFontOffset },
    { UnicodeChar::cCircumflexUc, SPR_FONTS_C_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cCircumflex, SPR_FONTS_C_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::gCircumflexUc, SPR_FONTS_G_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::gCircumflex, SPR_FONTS_G_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::hCircumflexUc, SPR_FONTS_H_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::hCircumflex, SPR_FONTS_H_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::jCircumflexUc, SPR_FONTS_J_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::jCircumflex, SPR_FONTS_J_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::sCircumflexUc, SPR_FONTS_S_CIRCUMFLEX_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::sCircumflex, SPR_FONTS_S_CIRCUMFLEX_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::uBreveUc, SPR_FONTS_U_BREVE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::uBreve, SPR_FONTS_U_BREVE_LOWER - SPR_FONTS_BEGIN },

    // Cyrillic alphabet
    { UnicodeChar::cyrillicIoUc, 203 - kCSSpriteFontOffset }, // Looks just like Ë
    { UnicodeChar::cyrillicUkrainianIeUc, SPR_FONTS_CYRILLIC_UKRAINIAN_IE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicDzeUc, 'S' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicDottedIUc, 'I' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicYiUc, 207 - kCSSpriteFontOffset }, // Looks just like Ï
    { UnicodeChar::cyrillicJeUc, 'J' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicAUc, 'A' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicBeUc, SPR_FONTS_CYRILLIC_BE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicVeUc, 'B' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicGheUc, SPR_FONTS_CYRILLIC_GHE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicDeUc, SPR_FONTS_CYRILLIC_DE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicIeUc, 'E' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicZheUc, SPR_FONTS_CYRILLIC_ZHE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicZeUc, SPR_FONTS_CYRILLIC_ZE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicIUc, SPR_FONTS_CYRILLIC_I_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicShortIUc, SPR_FONTS_CYRILLIC_SHORT_I_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicKaUc, 'K' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicElUc, SPR_FONTS_CYRILLIC_EL_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicEmUc, 'M' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicEnUc, 'H' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicOUc, 'O' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicPeUc, SPR_FONTS_CYRILLIC_PE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicErUc, 'P' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicEsUc, 'C' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicTeUc, 'T' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicUUc, SPR_FONTS_CYRILLIC_U_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicEfUc, SPR_FONTS_CYRILLIC_EF_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicHaUc, 'X' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicTseUc, SPR_FONTS_CYRILLIC_TSE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicCheUc, SPR_FONTS_CYRILLIC_CHE_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicShaUc, SPR_FONTS_CYRILLIC_SHA_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicShchaUc, SPR_FONTS_CYRILLIC_SHCHA_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicHardSignUc, SPR_FONTS_CYRILLIC_HARD_SIGN_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicYeruUc, SPR_FONTS_CYRILLIC_YERU_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicSoftSignUc, SPR_FONTS_CYRILLIC_SOFT_SIGN_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicEUc, SPR_FONTS_CYRILLIC_E_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicYuUc, SPR_FONTS_CYRILLIC_YU_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicYaUc, SPR_FONTS_CYRILLIC_YA_UPPER - SPR_FONTS_BEGIN },

    { UnicodeChar::cyrillicA, 'a' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicBe, SPR_FONTS_CYRILLIC_BE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicVe, SPR_FONTS_CYRILLIC_VE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicGhe, SPR_FONTS_CYRILLIC_GHE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicDe, SPR_FONTS_CYRILLIC_DE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicIe, 'e' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicZhe, SPR_FONTS_CYRILLIC_ZHE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicZe, SPR_FONTS_CYRILLIC_ZE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicI, SPR_FONTS_CYRILLIC_I_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicShortI, SPR_FONTS_CYRILLIC_SHORT_I_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicKa, SPR_FONTS_CYRILLIC_KA_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicEl, SPR_FONTS_CYRILLIC_EL_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicEm, SPR_FONTS_CYRILLIC_EM_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicEn, SPR_FONTS_CYRILLIC_EN_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicO, 'o' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicPe, SPR_FONTS_CYRILLIC_PE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicEr, 'p' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicEs, 'c' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicTe, SPR_FONTS_CYRILLIC_TE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicU, 'y' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicEf, SPR_FONTS_CYRILLIC_EF_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicHa, 'x' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicTse, SPR_FONTS_CYRILLIC_TSE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicChe, SPR_FONTS_CYRILLIC_CHE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicSha, SPR_FONTS_CYRILLIC_SHA_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicShcha, SPR_FONTS_CYRILLIC_SHCHA_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicHardSign, SPR_FONTS_CYRILLIC_HARD_SIGN_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicYeru, SPR_FONTS_CYRILLIC_YERU_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicSoftSign, SPR_FONTS_CYRILLIC_SOFT_SIGN_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicE, SPR_FONTS_CYRILLIC_E_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicYu, SPR_FONTS_CYRILLIC_YU_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicYa, SPR_FONTS_CYRILLIC_YA_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicIo, 235 - kCSSpriteFontOffset }, // Looks just like ë
    { UnicodeChar::cyrillicUkrainianIe, SPR_FONTS_CYRILLIC_UKRAINIAN_IE_LOWER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicDze, 's' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicDottedI, 'i' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicYi, 239 - kCSSpriteFontOffset }, // Looks just like ï
    { UnicodeChar::cyrillicJe, 'J' - kCSSpriteFontOffset },
    { UnicodeChar::cyrillicGheUpturnUc, SPR_FONTS_CYRILLIC_GHE_UPTURN_UPPER - SPR_FONTS_BEGIN },
    { UnicodeChar::cyrillicGheUpturn, SPR_FONTS_CYRILLIC_GHE_UPTURN_LOWER - SPR_FONTS_BEGIN },

    // Punctuation
    { UnicodeChar::leftBrace, SPR_FONTS_LEFT_BRACE - SPR_FONTS_BEGIN },
    { UnicodeChar::verticalBar, SPR_FONTS_VERTICAL_BAR - SPR_FONTS_BEGIN },
    { UnicodeChar::rightBrace, SPR_FONTS_RIGHT_BRACE - SPR_FONTS_BEGIN },
    { UnicodeChar::tilde, SPR_FONTS_TILDE - SPR_FONTS_BEGIN },
    { UnicodeChar::nonBreakingSpace, ' ' - kCSSpriteFontOffset },
    { UnicodeChar::interpunct, SPR_FONTS_INTERPUNCT - SPR_FONTS_BEGIN },
    { UnicodeChar::multiplicationSign, EnumValue(CSChar::cross) - kCSSpriteFontOffset },
    { UnicodeChar::enDash, '-' - kCSSpriteFontOffset },
    { UnicodeChar::emDash, '-' - kCSSpriteFontOffset },
    { UnicodeChar::singleQuoteOpen, '`' - kCSSpriteFontOffset },
    { UnicodeChar::singleQuoteEnd, '\'' - kCSSpriteFontOffset },
    { UnicodeChar::singleGermanQuoteOpen, ',' - kCSSpriteFontOffset },
    { UnicodeChar::germanQuoteOpen, SPR_FONTS_GERMAN_OPENQUOTES - SPR_FONTS_BEGIN },
    { UnicodeChar::bullet, EnumValue(CSChar::bullet) - kCSSpriteFontOffset },
    { UnicodeChar::ellipsis, SPR_FONTS_ELLIPSIS - SPR_FONTS_BEGIN },
    { UnicodeChar::narrowNonBreakingSpace, ' ' - kCSSpriteFontOffset },
    { UnicodeChar::quoteOpen, EnumValue(CSChar::quoteOpen) - kCSSpriteFontOffset },
    { UnicodeChar::quoteClose, EnumValue(CSChar::quoteClose) - kCSSpriteFontOffset },

    // Currency
    { UnicodeChar::guilder, SPR_FONTS_GUILDER_SIGN - SPR_FONTS_BEGIN },
    { UnicodeChar::euro, EnumValue(CSChar::euro) - kCSSpriteFontOffset },
    { UnicodeChar::won, SPR_FONTS_WON_SIGN - SPR_FONTS_BEGIN },
    { UnicodeChar::hryvnia, SPR_FONTS_HRYVNIA_SIGN - SPR_FONTS_BEGIN },
    { UnicodeChar::rouble, SPR_FONTS_ROUBLE_SIGN - SPR_FONTS_BEGIN },

    // Dingbats
    { UnicodeChar::up, EnumValue(CSChar::up) - kCSSpriteFontOffset },
    { UnicodeChar::smallUp, EnumValue(CSChar::smallUp) - kCSSpriteFontOffset },
    { UnicodeChar::right, EnumValue(CSChar::right) - kCSSpriteFontOffset },
    { UnicodeChar::down, EnumValue(CSChar::down) - kCSSpriteFontOffset },
    { UnicodeChar::smallDown, EnumValue(CSChar::smallDown) - kCSSpriteFontOffset },
    { UnicodeChar::left, EnumValue(CSChar::left) - kCSSpriteFontOffset },
    { UnicodeChar::air, EnumValue(CSChar::air) - kCSSpriteFontOffset },
    { UnicodeChar::tick, EnumValue(CSChar::tick) - kCSSpriteFontOffset },
    { UnicodeChar::plus, '+' - kCSSpriteFontOffset },
    { UnicodeChar::minus, '-' - kCSSpriteFontOffset },

    // Emoji
    { UnicodeChar::cross, EnumValue(CSChar::cross) - kCSSpriteFontOffset },
    { UnicodeChar::water, EnumValue(CSChar::water) - kCSSpriteFontOffset },
    { UnicodeChar::eye, SPR_FONTS_EYE - SPR_FONTS_BEGIN },
    { UnicodeChar::road, EnumValue(CSChar::road) - kCSSpriteFontOffset },
    { UnicodeChar::railway, EnumValue(CSChar::railway) - kCSSpriteFontOffset },

    // Misc
    { UnicodeChar::superscriptMinusOne, EnumValue(CSChar::superscriptMinusOne) - kCSSpriteFontOffset },
};

static char32_t _smallestCodepointValue = 0;
static char32_t _biggestCodepointValue = 0;

/**
 *
 *  rct2: 0x006C19AC
 */
void FontSpriteInitialiseCharacters()
{
    // Compute min and max that helps avoiding lookups for no reason.
    _smallestCodepointValue = std::numeric_limits<char32_t>::max();
    for (const auto& entry : kCodepointOffsetMap)
    {
        _smallestCodepointValue = std::min(_smallestCodepointValue, EnumValue(entry.first));
        _biggestCodepointValue = std::max(_biggestCodepointValue, EnumValue(entry.first));
    }

    for (const auto& fontStyle : kFontStyles)
    {
        int32_t glyphOffset = EnumValue(fontStyle) * SPR_FONTS_GLYPH_COUNT;
        for (auto glyphIndex = 0u; glyphIndex < SPR_FONTS_GLYPH_COUNT; glyphIndex++)
        {
            const G1Element* g1 = GfxGetG1Element(glyphIndex + SPR_FONTS_BEGIN + glyphOffset);
            int32_t width = 0;
            if (g1 != nullptr)
            {
                width = g1->width + (2 * g1->xOffset) - 1;
            }
            _spriteFontCharacterWidths[EnumValue(fontStyle)][glyphIndex] = static_cast<uint8_t>(width);
        }
    }

    Drawing::ScrollingText::initialiseBitmaps();
}

int32_t FontSpriteGetCodepointOffset(int32_t codepoint)
{
    // Only search the table when its in range of the map.
    if (static_cast<char32_t>(codepoint) >= _smallestCodepointValue
        && static_cast<char32_t>(codepoint) <= _biggestCodepointValue)
    {
        auto result = kCodepointOffsetMap.find(static_cast<UnicodeChar>(codepoint));
        if (result != kCodepointOffsetMap.end())
            return result->second;
    }

    if (codepoint < 32 || codepoint >= 256)
        codepoint = '?';

    return codepoint - 32;
}

int32_t FontSpriteGetCodepointWidth(FontStyle fontStyle, int32_t codepoint)
{
    int32_t glyphIndex = FontSpriteGetCodepointOffset(codepoint);
    auto baseFontIndex = EnumValue(fontStyle);

    if (glyphIndex >= static_cast<int32_t>(std::size(_spriteFontCharacterWidths[baseFontIndex])))
    {
        LOG_WARNING("Invalid glyph index %u", glyphIndex);
        glyphIndex = 0;
    }
    return _spriteFontCharacterWidths[baseFontIndex][glyphIndex];
}

ImageId FontSpriteGetCodepointSprite(FontStyle fontStyle, int32_t codepoint)
{
    auto codePointOffset = FontSpriteGetCodepointOffset(codepoint);
    int32_t offset = EnumValue(fontStyle) * SPR_FONTS_GLYPH_COUNT;

    return ImageId(SPR_FONTS_BEGIN + offset + codePointOffset, OpenRCT2::Drawing::Colour::black);
}

int32_t FontGetLineHeight(FontStyle fontStyle)
{
    auto fontSize = EnumValue(fontStyle);
#ifndef DISABLE_TTF
    if (LocalisationService_UseTrueTypeFont())
    {
        return gCurrentTTFFontSet->size[fontSize].line_height;
    }
#endif // DISABLE_TTF
    return kSpriteFontLineHeight[fontSize];
}

int32_t FontGetLineHeightSmall(FontStyle fontStyle)
{
    return FontGetLineHeight(fontStyle) / 2;
}

bool FontSupportsStringSprite(const utf8* text)
{
    const utf8* src = text;

    uint32_t codepoint;
    while ((codepoint = UTF8GetNext(src, &src)) != 0)
    {
        bool supported = false;

        if ((codepoint >= 32 && codepoint < 256)
            || (codepoint >= EnumValue(UnicodeChar::cyrillicAUc) && codepoint <= EnumValue(UnicodeChar::cyrillicYa)))
        {
            supported = true;
        }

        auto result = kCodepointOffsetMap.find(static_cast<UnicodeChar>(codepoint));
        if (result != kCodepointOffsetMap.end())
            supported = true;

        if (!supported)
        {
            return false;
        }
    }
    return true;
}

bool FontSupportsStringTTF(const utf8* text, FontStyle fontStyle)
{
#ifndef DISABLE_TTF
    const utf8* src = text;
    const TTF_Font* font = gCurrentTTFFontSet->size[EnumValue(fontStyle)].font;
    if (font == nullptr)
    {
        return false;
    }

    uint32_t codepoint;
    while ((codepoint = UTF8GetNext(src, &src)) != 0)
    {
        bool supported = TTFProvidesGlyph(font, codepoint);
        if (!supported)
        {
            return false;
        }
    }
    return true;
#else
    return false;
#endif // DISABLE_TTF
}

bool FontSupportsString(const utf8* text, FontStyle fontStyle)
{
    if (LocalisationService_UseTrueTypeFont())
    {
        return FontSupportsStringTTF(text, fontStyle);
    }

    return FontSupportsStringSprite(text);
}
