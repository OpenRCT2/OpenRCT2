/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../drawing/Drawing.String.h"

#include "../Context.h"
#include "../SpriteIds.h"
#include "../config/Config.h"
#include "../core/CodepointView.hpp"
#include "../core/String.hpp"
#include "../core/UTF8.h"
#include "../core/UnicodeChar.h"
#include "../drawing/ColourMap.h"
#include "../drawing/Drawing.h"
#include "../drawing/IDrawingContext.h"
#include "../drawing/IDrawingEngine.h"
#include "../drawing/Text.h"
#include "../interface/ColourWithFlags.h"
#include "../interface/Viewport.h"
#include "../localisation/Formatting.h"
#include "../localisation/LocalisationService.h"
#include "../platform/Platform.h"
#include "TTF.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

static int32_t TTFGetStringWidth(std::string_view text, FontStyle fontStyle, bool noFormatting);

/**
 *
 *  rct2: 0x006C23B1
 */
int32_t GfxGetStringWidthNewLined(std::string_view text, FontStyle fontStyle)
{
    u8string buffer;

    std::optional<int32_t> maxWidth;
    FmtString fmt(text);
    for (const auto& token : fmt)
    {
        if (token.kind == FormatToken::newline || token.kind == FormatToken::newlineSmall)
        {
            auto width = GfxGetStringWidth(buffer, fontStyle);
            if (!maxWidth.has_value() || maxWidth.value() > width)
            {
                maxWidth = width;
            }
            buffer.clear();
        }
        else
        {
            buffer.append(token.text);
        }
    }
    if (!maxWidth.has_value())
    {
        maxWidth = GfxGetStringWidth(buffer, fontStyle);
    }
    return maxWidth.value();
}

/**
 * Return the width of the string in buffer
 *
 *  rct2: 0x006C2321
 * buffer (esi)
 */
int32_t GfxGetStringWidth(std::string_view text, FontStyle fontStyle)
{
    return TTFGetStringWidth(text, fontStyle, false);
}

int32_t GfxGetStringWidthNoFormatting(std::string_view text, FontStyle fontStyle)
{
    return TTFGetStringWidth(text, fontStyle, true);
}

/**
 * Clip the text in buffer to width, add ellipsis and return the new width of the clipped string
 *
 *  rct2: 0x006C2460
 * buffer (esi)
 * width (edi)
 */
int32_t GfxClipString(utf8* text, int32_t width, FontStyle fontStyle)
{
    if (width < 6)
    {
        *text = 0;
        return 0;
    }

    // If width of the full string is less than allowed width then we don't need to clip
    auto clippedWidth = GfxGetStringWidth(text, fontStyle);
    if (clippedWidth <= width)
    {
        return clippedWidth;
    }

    // Append each character 1 by 1 with an ellipsis on the end until width is exceeded
    thread_local std::string buffer;
    buffer.clear();

    size_t bestLength = 0;
    int32_t bestWidth = 0;

    FmtString fmt(text);
    for (const auto& token : fmt)
    {
        CodepointView codepoints(token.text);
        for (auto codepoint : codepoints)
        {
            // Add the ellipsis before checking the width
            buffer.append("...");

            auto currentWidth = GfxGetStringWidth(buffer, fontStyle);
            if (currentWidth < width)
            {
                bestLength = buffer.size();
                bestWidth = currentWidth;

                // Trim the ellipsis
                buffer.resize(bestLength - 3);
            }
            else
            {
                // Width exceeded, rollback to best length and put ellipsis back
                buffer.resize(bestLength);
                for (auto i = static_cast<int32_t>(bestLength) - 1; i >= 0 && i >= static_cast<int32_t>(bestLength) - 3; i--)
                {
                    buffer[i] = '.';
                }

                // Copy buffer back to input text buffer
                std::strcpy(text, buffer.c_str());
                return bestWidth;
            }

            char cb[8]{};
            UTF8WriteCodepoint(cb, codepoint);
            buffer.append(cb);
        }
    }
    return GfxGetStringWidth(text, fontStyle);
}

/**
 * Wrap the text in buffer to width, returns width of longest line.
 *
 * Inserts NULL where line should break (as \n is used for something else),
 * so the number of lines is returned in num_lines. font_height seems to be
 * a control character for line height.
 *
 *  rct2: 0x006C21E2
 * buffer (esi)
 * width (edi) - in
 * num_lines (edi) - out
 * font_height (ebx) - out
 */
int32_t GfxWrapString(u8string_view text, int32_t width, FontStyle fontStyle, u8string* outWrappedText, int32_t* outNumLines)
{
    constexpr size_t kNullIndex = std::numeric_limits<size_t>::max();
    u8string buffer;

    size_t currentLineIndex = 0;
    size_t splitIndex = kNullIndex;
    size_t bestSplitIndex = kNullIndex;
    size_t numLines = 0;
    int32_t maxWidth = 0;

    FmtString fmt(text);
    for (const auto& token : fmt)
    {
        if (token.IsLiteral())
        {
            CodepointView codepoints(token.text);
            for (auto codepoint : codepoints)
            {
                char cb[8]{};
                UTF8WriteCodepoint(cb, codepoint);
                buffer.append(cb);

                auto lineWidth = GfxGetStringWidth(&buffer[currentLineIndex], fontStyle);
                if (lineWidth <= width || (splitIndex == kNullIndex && bestSplitIndex == kNullIndex))
                {
                    if (codepoint == ' ')
                    {
                        // Mark line split here
                        splitIndex = buffer.size() - 1;
                    }
                    else if (splitIndex == kNullIndex)
                    {
                        // Mark line split here (this is after first character of line)
                        bestSplitIndex = buffer.size();
                    }
                }
                else
                {
                    // Insert new line before current word
                    if (splitIndex == kNullIndex)
                    {
                        splitIndex = bestSplitIndex;
                    }
                    buffer.insert(buffer.begin() + splitIndex, '\0');

                    // Recalculate the line length after splitting
                    lineWidth = GfxGetStringWidth(&buffer[currentLineIndex], fontStyle);
                    maxWidth = std::max(maxWidth, lineWidth);
                    numLines++;

                    currentLineIndex = splitIndex + 1;
                    splitIndex = kNullIndex;
                    bestSplitIndex = kNullIndex;

                    // Trim the beginning of the new line
                    while (buffer[currentLineIndex] == ' ')
                    {
                        buffer.erase(buffer.begin() + currentLineIndex);
                    }
                }
            }
        }
        else if (token.kind == FormatToken::newline)
        {
            buffer.push_back('\0');

            auto lineWidth = GfxGetStringWidth(&buffer[currentLineIndex], fontStyle);
            maxWidth = std::max(maxWidth, lineWidth);
            numLines++;

            currentLineIndex = buffer.size();
            splitIndex = kNullIndex;
            bestSplitIndex = kNullIndex;
        }
        else
        {
            buffer.append(token.text);
        }
    }
    {
        // Final line width calculation
        auto lineWidth = GfxGetStringWidth(&buffer[currentLineIndex], fontStyle);
        maxWidth = std::max(maxWidth, lineWidth);
    }

    if (outWrappedText != nullptr)
    {
        *outWrappedText = std::move(buffer);
    }
    if (outNumLines != nullptr)
    {
        *outNumLines = static_cast<int32_t>(numLines);
    }
    return maxWidth;
}

/**
 * Draws text that is left aligned and vertically centred.
 */
void GfxDrawStringLeftCentred(
    RenderTarget& rt, StringId format, void* args, ColourWithFlags colour, const ScreenCoordsXY& coords)
{
    char buffer[512];
    auto bufferPtr = buffer;
    FormatStringLegacy(bufferPtr, sizeof(buffer), format, args);
    int32_t height = StringGetHeightRaw(bufferPtr, FontStyle::medium);
    DrawText(rt, coords - ScreenCoordsXY{ 0, (height / 2) }, { colour }, bufferPtr);
}

/**
 * Changes the palette so that the next character changes colour
 */
static void ColourCharacter(TextColour colour, bool withOutline, TextColours& textPalette)
{
    auto mapping = getTextColourMapping(colour);

    if (!withOutline)
    {
        mapping.sunnyOutline = PaletteIndex::transparent;
        mapping.shadowOutline = PaletteIndex::transparent;
    }

    textPalette = mapping;
}

/**
 * Changes the palette so that the next character changes colour
 * This is specific to changing to a predefined window related colour
 */
static void ColourCharacterWindow(OpenRCT2::Drawing::Colour colour, bool withOutline, TextColours& textPalette)
{
    TextColours mapping = {
        getColourMap(colour).colour11,
        PaletteIndex::transparent,
        PaletteIndex::transparent,
    };

    if (withOutline)
    {
        mapping.sunnyOutline = PaletteIndex::pi10;
        mapping.shadowOutline = PaletteIndex::pi10;
    }

    textPalette = mapping;
}

/**
 *
 *  rct2: 0x006C1DB7
 *
 * left     : cx
 * top      : dx
 * numLines : bp
 * text     : esi
 * rt      : edi
 */
void DrawStringCentredRaw(
    RenderTarget& rt, const ScreenCoordsXY& coords, int32_t numLines, const utf8* text, FontStyle fontStyle)
{
    ScreenCoordsXY screenCoords(rt.x, rt.y);
    DrawText(rt, screenCoords, { OpenRCT2::Drawing::Colour::black, fontStyle }, "");
    screenCoords = coords;

    for (int32_t i = 0; i <= numLines; i++)
    {
        int32_t width = GfxGetStringWidth(text, fontStyle);
        DrawText(rt, screenCoords - ScreenCoordsXY{ width / 2, 0 }, { OpenRCT2::Drawing::kColourNull, fontStyle }, text);

        const utf8* ch = text;
        const utf8* nextCh = nullptr;

        while ((UTF8GetNext(ch, &nextCh)) != 0)
        {
            ch = nextCh;
        }
        text = const_cast<char*>(ch + 1);

        screenCoords.y += FontGetLineHeight(fontStyle);
    }
}

int32_t StringGetHeightRaw(std::string_view text, FontStyle fontStyle)
{
    int32_t height = 0;
    if (fontStyle <= FontStyle::medium)
        height += 10;
    else if (fontStyle == FontStyle::tiny)
        height += 6;

    FmtString fmt(text);
    for (const auto& token : fmt)
    {
        switch (token.kind)
        {
            case FormatToken::newline:
                if (fontStyle == FontStyle::small || fontStyle == FontStyle::medium)
                {
                    height += 10;
                    break;
                }

                if (fontStyle == FontStyle::tiny)
                {
                    height += 6;
                    break;
                }
                height += 18;
                break;
            case FormatToken::newlineSmall:
                if (fontStyle == FontStyle::small || fontStyle == FontStyle::medium)
                {
                    height += 5;
                    break;
                }

                if (fontStyle == FontStyle::tiny)
                {
                    height += 3;
                    break;
                }
                height += 9;
                break;
            case FormatToken::fontTiny:
                fontStyle = FontStyle::tiny;
                break;
            case FormatToken::fontMedium:
                fontStyle = FontStyle::medium;
                break;
            case FormatToken::fontSmall:
                fontStyle = FontStyle::small;
                break;
            default:
                break;
        }
    }
    return height;
}

/**
 *
 *  rct2: 0x006C1F57
 *
 * colour   : al
 * format   : bx
 * x        : cx
 * y        : dx
 * text     : esi
 * rt      : edi
 * width    : bp
 * ticks    : ebp >> 16
 */
void DrawNewsTicker(
    RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, OpenRCT2::Drawing::Colour colour, StringId format,
    u8string_view args, int32_t ticks)
{
    int32_t numLines, lineHeight, lineY;
    ScreenCoordsXY screenCoords(rt.x, rt.y);

    DrawText(rt, screenCoords, { colour }, "");

    u8string wrappedString;
    GfxWrapString(FormatStringID(format, args), width, FontStyle::small, &wrappedString, &numLines);
    lineHeight = FontGetLineHeight(FontStyle::small);

    int32_t numCharactersDrawn = 0;
    int32_t numCharactersToDraw = ticks;

    const utf8* buffer = wrappedString.data();
    lineY = coords.y - ((numLines * lineHeight) / 2);
    for (int32_t line = 0; line <= numLines; line++)
    {
        int32_t halfWidth = GfxGetStringWidth(buffer, FontStyle::small) / 2;

        FmtString fmt(buffer);
        for (const auto& token : fmt)
        {
            bool doubleBreak = false;
            if (token.IsLiteral())
            {
                CodepointView codepoints(token.text);
                for (auto it = codepoints.begin(); it != codepoints.end(); it++)
                {
                    numCharactersDrawn++;
                    if (numCharactersDrawn > numCharactersToDraw)
                    {
                        auto ch = const_cast<char*>(&token.text[it.GetIndex()]);
                        *ch = '\0';
                        doubleBreak = true;
                        break;
                    }
                }
            }
            if (doubleBreak)
                break;
        }

        screenCoords = { coords.x - halfWidth, lineY };
        DrawText(rt, screenCoords, { OpenRCT2::Drawing::kColourNull, FontStyle::small }, buffer);

        if (numCharactersDrawn > numCharactersToDraw)
        {
            break;
        }

        buffer = GetStringEnd(buffer) + 1;
        lineY += lineHeight;
    }
}

static void TTFDrawCharacterSprite(RenderTarget& rt, int32_t codepoint, TextDrawInfo* info)
{
    int32_t characterWidth = FontSpriteGetCodepointWidth(info->fontStyle, codepoint);
    auto sprite = FontSpriteGetCodepointSprite(info->fontStyle, codepoint);

    if (!info->textDrawFlags.has(TextDrawFlag::noDraw))
    {
        auto screenCoords = ScreenCoordsXY{ info->x, info->y };
        if (info->textDrawFlags.has(TextDrawFlag::yOffsetEffect))
        {
            screenCoords.y += *info->yOffset++;
        }

        PaletteIndex palette[8]{};
        palette[1] = info->palette.fill;
        palette[2] = info->palette.sunnyOutline;
        palette[3] = info->palette.shadowOutline;

        PaletteMap paletteMap(palette);
        GfxDrawGlyph(rt, sprite, screenCoords, paletteMap);
    }

    info->x += characterWidth;
}

static void TTFDrawStringRawSprite(RenderTarget& rt, std::string_view text, TextDrawInfo* info)
{
    CodepointView codepoints(text);
    for (auto codepoint : codepoints)
    {
        TTFDrawCharacterSprite(rt, codepoint, info);
    }
}

#ifndef DISABLE_TTF

static void TTFDrawStringRawTTF(RenderTarget& rt, std::string_view text, TextDrawInfo* info)
{
    if (!TTFInitialise())
        return;

    TTFFontDescriptor* fontDesc = TTFGetFontFromSpriteBase(info->fontStyle);
    if (fontDesc->font == nullptr)
    {
        TTFDrawStringRawSprite(rt, text, info);
        return;
    }

    if (info->textDrawFlags.has(TextDrawFlag::noDraw))
    {
        info->x += TTFGetWidthCacheGetOrAdd(fontDesc->font, text);
        return;
    }

    TTFSurface* surface = TTFSurfaceCacheGetOrAdd(fontDesc->font, text);
    if (surface == nullptr)
        return;

    auto drawingEngine = rt.DrawingEngine;
    if (drawingEngine != nullptr)
    {
        int32_t drawX = info->x + fontDesc->offset_x;
        int32_t drawY = info->y + fontDesc->offset_y;
        uint8_t hintThresh = Config::Get().fonts.enableHinting ? fontDesc->hinting_threshold : 0;
        IDrawingContext* dc = drawingEngine->GetDrawingContext();
        dc->DrawTTFBitmap(rt, info, surface, drawX, drawY, hintThresh);
    }
    info->x += surface->w;
}

#endif // DISABLE_TTF

static void TTFProcessFormatCode(RenderTarget& rt, const FmtString::Token& token, TextDrawInfo* info)
{
    switch (token.kind)
    {
        case FormatToken::move:
            info->x = info->startX + token.parameter;
            break;
        case FormatToken::newline:
            info->x = info->startX;
            info->y += FontGetLineHeight(info->fontStyle);
            break;
        case FormatToken::newlineSmall:
            info->x = info->startX;
            info->y += FontGetLineHeightSmall(info->fontStyle);
            break;
        case FormatToken::fontTiny:
            info->fontStyle = FontStyle::tiny;
            break;
        case FormatToken::fontSmall:
            info->fontStyle = FontStyle::small;
            break;
        case FormatToken::fontMedium:
            info->fontStyle = FontStyle::medium;
            break;
        case FormatToken::outlineEnable:
            info->colourFlags.set(ColourFlag::withOutline);
            break;
        case FormatToken::outlineDisable:
            info->colourFlags.unset(ColourFlag::withOutline);
            break;
        case FormatToken::colourWindow1:
        {
            ColourCharacterWindow(gCurrentWindowColours[0], info->colourFlags.has(ColourFlag::withOutline), info->palette);
            break;
        }
        case FormatToken::colourWindow2:
        {
            ColourCharacterWindow(gCurrentWindowColours[1], info->colourFlags.has(ColourFlag::withOutline), info->palette);
            break;
        }
        case FormatToken::colourWindow3:
        {
            ColourCharacterWindow(gCurrentWindowColours[2], info->colourFlags.has(ColourFlag::withOutline), info->palette);
            break;
        }
        case FormatToken::inlineSprite:
        {
            auto imageId = ImageId(token.parameter);
            auto g1 = GfxGetG1Element(imageId);
            if (g1 != nullptr && g1->width <= 32 && g1->height <= 32)
            {
                if (!info->textDrawFlags.has(TextDrawFlag::noDraw))
                {
                    GfxDrawSprite(rt, imageId, { info->x, info->y });
                }
                info->x += g1->width;
            }
            break;
        }
        default:
            if (FormatTokenIsColour(token.kind))
            {
                auto colourIndex = FormatTokenToTextColour(token.kind);
                ColourCharacter(colourIndex, info->colourFlags.has(ColourFlag::withOutline), info->palette);
            }
            break;
    }
}

#ifndef DISABLE_TTF
static bool ShouldUseSpriteForCodepoint(char32_t codepoint)
{
    switch (codepoint)
    {
        case UnicodeChar::up:
        case UnicodeChar::down:
        case UnicodeChar::leftguillemet:
        case UnicodeChar::tick:
        case UnicodeChar::cross:
        case UnicodeChar::right:
        case UnicodeChar::rightguillemet:
        case UnicodeChar::small_up:
        case UnicodeChar::small_down:
        case UnicodeChar::left:
        case UnicodeChar::quote_open:
        case UnicodeChar::quote_close:
        case UnicodeChar::german_quote_open:
        case UnicodeChar::plus:
        case UnicodeChar::minus:
        case UnicodeChar::variation_selector:
        case UnicodeChar::eye:
        case UnicodeChar::road:
        case UnicodeChar::railway:
            return true;
        default:
            return false;
    }
}
#endif // DISABLE_TTF

static void TTFProcessStringLiteral(RenderTarget& rt, std::string_view text, TextDrawInfo* info)
{
#ifndef DISABLE_TTF
    bool isTTF = info->textDrawFlags.has(TextDrawFlag::ttf);
#else
    bool isTTF = false;
#endif // DISABLE_TTF

    if (!isTTF)
    {
        TTFDrawStringRawSprite(rt, text, info);
    }
#ifndef DISABLE_TTF
    else
    {
        CodepointView codepoints(text);
        std::optional<size_t> ttfRunIndex{};
        for (auto it = codepoints.begin(); it != codepoints.end(); it++)
        {
            auto codepoint = *it;
            if (ShouldUseSpriteForCodepoint(codepoint))
            {
                if (ttfRunIndex.has_value())
                {
                    // Draw the TTF run
                    // This error suppression abomination is here to suppress https://github.com/OpenRCT2/OpenRCT2/issues/17371.
                    // Additionally, we have to suppress the error for the error suppression... :'-(
                    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105937 is fixed in GCC13
    #if defined(__GNUC__) && !defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
    #endif
                    auto len = it.GetIndex() - ttfRunIndex.value();
                    TTFDrawStringRawTTF(rt, text.substr(ttfRunIndex.value(), len), info);
    #if defined(__GNUC__) && !defined(__clang__)
        #pragma GCC diagnostic pop
    #endif
                    ttfRunIndex = std::nullopt;
                }

                // Draw the sprite font glyph
                TTFDrawCharacterSprite(rt, codepoint, info);
            }
            else
            {
                if (!ttfRunIndex.has_value())
                {
                    ttfRunIndex = it.GetIndex();
                }
            }
        }

        if (ttfRunIndex.has_value())
        {
            // Final TTF run
            auto len = text.size() - *ttfRunIndex;
            TTFDrawStringRawTTF(rt, text.substr(ttfRunIndex.value(), len), info);
        }
    }
#endif // DISABLE_TTF
}

static void TTFProcessStringCodepoint(RenderTarget& rt, codepoint_t codepoint, TextDrawInfo* info)
{
    char buffer[8]{};
    UTF8WriteCodepoint(buffer, codepoint);
    TTFProcessStringLiteral(rt, buffer, info);
}

static void TTFProcessString(RenderTarget& rt, std::string_view text, TextDrawInfo* info)
{
    if (info->textDrawFlags.has(TextDrawFlag::noFormatting))
    {
        TTFProcessStringLiteral(rt, text, info);
        info->maxX = std::max(info->maxX, info->x);
        info->maxY = std::max(info->maxY, info->y);
    }
    else
    {
        FmtString fmt(text);
        for (const auto& token : fmt)
        {
            if (token.IsLiteral())
            {
                TTFProcessStringLiteral(rt, token.text, info);
            }
            else if (token.IsCodepoint())
            {
                auto codepoint = token.GetCodepoint();
                TTFProcessStringCodepoint(rt, codepoint, info);
            }
            else
            {
                TTFProcessFormatCode(rt, token, info);
            }
            info->maxX = std::max(info->maxX, info->x);
            info->maxY = std::max(info->maxY, info->y);
        }
    }
}

static void TTFProcessInitialColour(ColourWithFlags colour, TextDrawInfo* info)
{
    if (colour.colour != OpenRCT2::Drawing::kColourNull)
    {
        info->colourFlags = colour.flags;
        if (!colour.flags.has(ColourFlag::inset))
        {
            ColourCharacterWindow(colour.colour, info->colourFlags.has(ColourFlag::withOutline), info->palette);
        }
        else
        {
            TextColours newPalette = {};
            switch (info->darkness)
            {
                case TextDarkness::extraDark:
                    newPalette.fill = getColourMap(colour.colour).dark;
                    newPalette.shadowOutline = getColourMap(colour.colour).midLight;
                    break;

                case TextDarkness::dark:
                    newPalette.fill = getColourMap(colour.colour).midDark;
                    newPalette.shadowOutline = getColourMap(colour.colour).light;
                    break;

                case TextDarkness::regular:
                    newPalette.fill = getColourMap(colour.colour).midLight;
                    newPalette.shadowOutline = getColourMap(colour.colour).lighter;
                    break;
            }

            info->palette = newPalette;
        }
    }
}

void TTFDrawString(
    RenderTarget& rt, const_utf8string text, ColourWithFlags colour, const ScreenCoordsXY& coords, bool noFormatting,
    FontStyle fontStyle, TextDarkness darkness)
{
    if (text == nullptr)
        return;

    TextDrawInfo info{};
    info.fontStyle = fontStyle;
    info.startX = coords.x;
    info.startY = coords.y;
    info.x = coords.x;
    info.y = coords.y;
    info.darkness = darkness;

    if (LocalisationService_UseTrueTypeFont())
    {
        info.textDrawFlags.set(TextDrawFlag::ttf);
    }

    if (noFormatting)
    {
        info.textDrawFlags.set(TextDrawFlag::noFormatting);
    }

    info.palette = gTextPalette;
    TTFProcessInitialColour(colour, &info);
    TTFProcessString(rt, text, &info);
    gTextPalette = info.palette;

    rt.lastStringPos = { info.x, info.y };
}

static int32_t TTFGetStringWidth(std::string_view text, FontStyle fontStyle, bool noFormatting)
{
    TextDrawInfo info{};
    info.fontStyle = fontStyle;
    info.startX = 0;
    info.startY = 0;
    info.x = 0;
    info.y = 0;
    info.maxX = 0;
    info.maxY = 0;

    info.textDrawFlags.set(TextDrawFlag::noDraw);
    if (LocalisationService_UseTrueTypeFont())
    {
        info.textDrawFlags.set(TextDrawFlag::ttf);
    }

    if (noFormatting)
    {
        info.textDrawFlags.set(TextDrawFlag::noFormatting);
    }

    RenderTarget dummy{};
    TTFProcessString(dummy, text, &info);

    return info.maxX;
}

/**
 *
 *  rct2: 0x00682F28
 */
void GfxDrawStringWithYOffsets(
    RenderTarget& rt, const utf8* text, ColourWithFlags colour, const ScreenCoordsXY& coords, const int8_t* yOffsets,
    bool forceSpriteFont, FontStyle fontStyle)
{
    TextDrawInfo info{};
    info.fontStyle = fontStyle;
    info.startX = coords.x;
    info.startY = coords.y;
    info.x = coords.x;
    info.y = coords.y;
    info.yOffset = yOffsets;

    info.textDrawFlags.set(TextDrawFlag::yOffsetEffect);

    if (!forceSpriteFont && LocalisationService_UseTrueTypeFont())
    {
        info.textDrawFlags.set(TextDrawFlag::ttf);
    }

    info.palette = gTextPalette;
    TTFProcessInitialColour(colour, &info);
    TTFProcessString(rt, text, &info);
    gTextPalette = info.palette;

    rt.lastStringPos = { info.x, info.y };
}

u8string ShortenPath(const u8string& path, int32_t availableWidth, FontStyle fontStyle)
{
    if (GfxGetStringWidth(path, fontStyle) <= availableWidth)
    {
        return path;
    }

    u8string shortenedPath = u8"...";

    size_t begin = 0;
    while (begin < path.size())
    {
        begin = path.find_first_of(*PATH_SEPARATOR, begin + 1);
        if (begin == path.npos)
            break;

        shortenedPath = u8"..." + path.substr(begin);
        if (GfxGetStringWidth(shortenedPath, fontStyle) <= availableWidth)
        {
            return shortenedPath;
        }
    }

    return shortenedPath;
}
