/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../drawing/Drawing.h"

#include "../Context.h"
#include "../common.h"
#include "../config/Config.h"
#include "../core/String.hpp"
#include "../drawing/IDrawingContext.h"
#include "../drawing/IDrawingEngine.h"
#include "../interface/Viewport.h"
#include "../localisation/Formatting.h"
#include "../localisation/Localisation.h"
#include "../localisation/LocalisationService.h"
#include "../platform/Platform.h"
#include "../sprites.h"
#include "../util/Util.h"
#include "TTF.h"

#include <algorithm>

using namespace OpenRCT2;

enum : uint32_t
{
    TEXT_DRAW_FLAG_INSET = 1 << 0,
    TEXT_DRAW_FLAG_OUTLINE = 1 << 1,
    TEXT_DRAW_FLAG_DARK = 1 << 2,
    TEXT_DRAW_FLAG_EXTRA_DARK = 1 << 3,
    TEXT_DRAW_FLAG_NO_FORMATTING = 1 << 28,
    TEXT_DRAW_FLAG_Y_OFFSET_EFFECT = 1 << 29,
    TEXT_DRAW_FLAG_TTF = 1 << 30,
    TEXT_DRAW_FLAG_NO_DRAW = 1u << 31
};

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
        if (token.kind == FormatToken::Newline || token.kind == FormatToken::NewlineSmall)
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
    constexpr size_t NULL_INDEX = std::numeric_limits<size_t>::max();
    u8string buffer;

    size_t currentLineIndex = 0;
    size_t splitIndex = NULL_INDEX;
    size_t bestSplitIndex = NULL_INDEX;
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
                if (lineWidth <= width || (splitIndex == NULL_INDEX && bestSplitIndex == NULL_INDEX))
                {
                    if (codepoint == ' ')
                    {
                        // Mark line split here
                        splitIndex = buffer.size() - 1;
                    }
                    else if (splitIndex == NULL_INDEX)
                    {
                        // Mark line split here (this is after first character of line)
                        bestSplitIndex = buffer.size();
                    }
                }
                else
                {
                    // Insert new line before current word
                    if (splitIndex == NULL_INDEX)
                    {
                        splitIndex = bestSplitIndex;
                    }
                    buffer.insert(buffer.begin() + splitIndex, '\0');

                    // Recalculate the line length after splitting
                    lineWidth = GfxGetStringWidth(&buffer[currentLineIndex], fontStyle);
                    maxWidth = std::max(maxWidth, lineWidth);
                    numLines++;

                    currentLineIndex = splitIndex + 1;
                    splitIndex = NULL_INDEX;
                    bestSplitIndex = NULL_INDEX;

                    // Trim the beginning of the new line
                    while (buffer[currentLineIndex] == ' ')
                    {
                        buffer.erase(buffer.begin() + currentLineIndex);
                    }
                }
            }
        }
        else if (token.kind == FormatToken::Newline)
        {
            buffer.push_back('\0');

            auto lineWidth = GfxGetStringWidth(&buffer[currentLineIndex], fontStyle);
            maxWidth = std::max(maxWidth, lineWidth);
            numLines++;

            currentLineIndex = buffer.size();
            splitIndex = NULL_INDEX;
            bestSplitIndex = NULL_INDEX;
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
void GfxDrawStringLeftCentred(DrawPixelInfo& dpi, StringId format, void* args, colour_t colour, const ScreenCoordsXY& coords)
{
    char buffer[CommonTextBufferSize];
    auto bufferPtr = buffer;
    FormatStringLegacy(bufferPtr, sizeof(buffer), format, args);
    int32_t height = StringGetHeightRaw(bufferPtr, FontStyle::Medium);
    GfxDrawString(dpi, coords - ScreenCoordsXY{ 0, (height / 2) }, bufferPtr, { colour });
}

/**
 * Changes the palette so that the next character changes colour
 */
static void ColourCharacter(uint8_t colour, const uint16_t* current_font_flags, uint8_t* palette_pointer)
{
    int32_t colour32 = 0;
    const G1Element* g1 = GfxGetG1Element(SPR_TEXT_PALETTE);
    if (g1 != nullptr)
    {
        uint32_t idx = (colour & 0xFF) * 4;
        std::memcpy(&colour32, &g1->offset[idx], sizeof(colour32));
    }

    if (!(*current_font_flags & TEXT_DRAW_FLAG_OUTLINE))
    {
        colour32 = colour32 & 0x0FF0000FF;
    }
    // Adjust text palette. Store current colour?
    palette_pointer[1] = colour32 & 0xFF;
    palette_pointer[2] = (colour32 >> 8) & 0xFF;
    palette_pointer[3] = (colour32 >> 16) & 0xFF;
    palette_pointer[4] = (colour32 >> 24) & 0xFF;
}

/**
 * Changes the palette so that the next character changes colour
 * This is specific to changing to a predefined window related colour
 */
static void ColourCharacterWindow(uint8_t colour, const uint16_t* current_font_flags, uint8_t* palette_pointer)
{
    int32_t eax;

    colour = NOT_TRANSLUCENT(colour);
    eax = ColourMapA[colour].colour_11;
    if (*current_font_flags & TEXT_DRAW_FLAG_OUTLINE)
    {
        eax |= 0x0A0A00;
    }
    // Adjust text palette. Store current colour?
    palette_pointer[1] = eax & 0xFF;
    palette_pointer[2] = (eax >> 8) & 0xFF;
    palette_pointer[3] = (eax >> 16) & 0xFF;
    palette_pointer[4] = (eax >> 24) & 0xFF;
}

/**
 *
 *  rct2: 0x006C1DB7
 *
 * left     : cx
 * top      : dx
 * numLines : bp
 * text     : esi
 * dpi      : edi
 */
void DrawStringCentredRaw(
    DrawPixelInfo& dpi, const ScreenCoordsXY& coords, int32_t numLines, const utf8* text, FontStyle fontStyle)
{
    ScreenCoordsXY screenCoords(dpi.x, dpi.y);
    GfxDrawString(dpi, screenCoords, "", { COLOUR_BLACK, fontStyle });
    screenCoords = coords;

    for (int32_t i = 0; i <= numLines; i++)
    {
        int32_t width = GfxGetStringWidth(text, fontStyle);
        GfxDrawString(dpi, screenCoords - ScreenCoordsXY{ width / 2, 0 }, text, { TEXT_COLOUR_254, fontStyle });

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
    if (fontStyle <= FontStyle::Medium)
        height += 10;
    else if (fontStyle == FontStyle::Tiny)
        height += 6;

    FmtString fmt(text);
    for (const auto& token : fmt)
    {
        switch (token.kind)
        {
            case FormatToken::Newline:
                if (fontStyle == FontStyle::Small || fontStyle == FontStyle::Medium)
                {
                    height += 10;
                    break;
                }

                if (fontStyle == FontStyle::Tiny)
                {
                    height += 6;
                    break;
                }
                height += 18;
                break;
            case FormatToken::NewlineSmall:
                if (fontStyle == FontStyle::Small || fontStyle == FontStyle::Medium)
                {
                    height += 5;
                    break;
                }

                if (fontStyle == FontStyle::Tiny)
                {
                    height += 3;
                    break;
                }
                height += 9;
                break;
            case FormatToken::FontTiny:
                fontStyle = FontStyle::Tiny;
                break;
            case FormatToken::FontMedium:
                fontStyle = FontStyle::Medium;
                break;
            case FormatToken::FontSmall:
                fontStyle = FontStyle::Small;
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
 * dpi      : edi
 * width    : bp
 * ticks    : ebp >> 16
 */
void DrawNewsTicker(
    DrawPixelInfo& dpi, const ScreenCoordsXY& coords, int32_t width, colour_t colour, StringId format, u8string_view args,
    int32_t ticks)
{
    int32_t numLines, lineHeight, lineY;
    ScreenCoordsXY screenCoords(dpi.x, dpi.y);

    GfxDrawString(dpi, screenCoords, "", { colour });

    u8string wrappedString;
    GfxWrapString(FormatStringID(format, args), width, FontStyle::Small, &wrappedString, &numLines);
    lineHeight = FontGetLineHeight(FontStyle::Small);

    int32_t numCharactersDrawn = 0;
    int32_t numCharactersToDraw = ticks;

    const utf8* buffer = wrappedString.data();
    lineY = coords.y - ((numLines * lineHeight) / 2);
    for (int32_t line = 0; line <= numLines; line++)
    {
        int32_t halfWidth = GfxGetStringWidth(buffer, FontStyle::Small) / 2;

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
        GfxDrawString(dpi, screenCoords, buffer, { TEXT_COLOUR_254, FontStyle::Small });

        if (numCharactersDrawn > numCharactersToDraw)
        {
            break;
        }

        buffer = GetStringEnd(buffer) + 1;
        lineY += lineHeight;
    }
}

struct TextDrawInfo
{
    int32_t startX;
    int32_t startY;
    int32_t x;
    int32_t y;
    int32_t maxX;
    int32_t maxY;
    int32_t flags;
    uint8_t palette[8];
    ::FontStyle FontStyle;
    const int8_t* y_offset;
};

static void TTFDrawCharacterSprite(DrawPixelInfo& dpi, int32_t codepoint, TextDrawInfo* info)
{
    int32_t characterWidth = FontSpriteGetCodepointWidth(info->FontStyle, codepoint);
    auto sprite = FontSpriteGetCodepointSprite(info->FontStyle, codepoint);

    if (!(info->flags & TEXT_DRAW_FLAG_NO_DRAW))
    {
        auto screenCoords = ScreenCoordsXY{ info->x, info->y };
        if (info->flags & TEXT_DRAW_FLAG_Y_OFFSET_EFFECT)
        {
            screenCoords.y += *info->y_offset++;
        }

        PaletteMap paletteMap(info->palette);
        GfxDrawGlyph(&dpi, sprite, screenCoords, paletteMap);
    }

    info->x += characterWidth;
}

static void TTFDrawStringRawSprite(DrawPixelInfo& dpi, std::string_view text, TextDrawInfo* info)
{
    CodepointView codepoints(text);
    for (auto codepoint : codepoints)
    {
        TTFDrawCharacterSprite(dpi, codepoint, info);
    }
}

#ifndef NO_TTF

static int _ttfGlId = 0;
static void TTFDrawStringRawTTF(DrawPixelInfo& dpi, std::string_view text, TextDrawInfo* info)
{
    if (!TTFInitialise())
        return;

    TTFFontDescriptor* fontDesc = TTFGetFontFromSpriteBase(info->FontStyle);
    if (fontDesc->font == nullptr)
    {
        TTFDrawStringRawSprite(dpi, text, info);
        return;
    }

    if (info->flags & TEXT_DRAW_FLAG_NO_DRAW)
    {
        info->x += TTFGetWidthCacheGetOrAdd(fontDesc->font, text);
        return;
    }

    uint8_t colour = info->palette[1];
    TTFSurface* surface = TTFSurfaceCacheGetOrAdd(fontDesc->font, text);
    if (surface == nullptr)
        return;

    int32_t drawX = info->x + fontDesc->offset_x;
    int32_t drawY = info->y + fontDesc->offset_y;
    int32_t width = surface->w;
    int32_t height = surface->h;

    if (OpenRCT2::GetContext()->GetDrawingEngineType() == DrawingEngine::OpenGL)
    {
        auto pixels = reinterpret_cast<uint8_t*>(const_cast<void*>(surface->pixels));
        auto pixelsLen = static_cast<size_t>(surface->pitch) * surface->h;
        for (size_t pp = 0; pp < pixelsLen; pp++)
        {
            if (pixels[pp] != 0)
            {
                pixels[pp] = colour;
            }
            else
            {
                pixels[pp] = PALETTE_INDEX_0;
            }
        }

        auto baseId = uint32_t(0x7FFFF) - 1024;
        auto imageId = baseId + _ttfGlId;
        auto drawingEngine = dpi.DrawingEngine;
        auto drawingContext = drawingEngine->GetDrawingContext();
        drawingEngine->InvalidateImage(imageId);
        drawingContext->DrawBitmap(&dpi, imageId, surface->pixels, surface->pitch, surface->h, drawX, drawY);

        _ttfGlId++;
        if (_ttfGlId >= 1023)
        {
            _ttfGlId = 0;
        }
        return;
    }

    int32_t overflowX = (dpi.x + dpi.width) - (drawX + width);
    int32_t overflowY = (dpi.y + dpi.height) - (drawY + height);
    if (overflowX < 0)
        width += overflowX;
    if (overflowY < 0)
        height += overflowY;
    int32_t skipX = drawX - dpi.x;
    int32_t skipY = drawY - dpi.y;
    info->x += width;

    auto src = static_cast<const uint8_t*>(surface->pixels);
    uint8_t* dst = dpi.bits;

    if (skipX < 0)
    {
        width += skipX;
        src += -skipX;
        skipX = 0;
    }
    if (skipY < 0)
    {
        height += skipY;
        src += (-skipY * surface->pitch);
        skipY = 0;
    }

    dst += skipX;
    dst += skipY * (dpi.width + dpi.pitch);

    int32_t srcScanSkip = surface->pitch - width;
    int32_t dstScanSkip = dpi.width + dpi.pitch - width;
    uint8_t* dst_orig = dst;
    const uint8_t* src_orig = src;

    // Draw shadow/outline
    if (info->flags & TEXT_DRAW_FLAG_OUTLINE)
    {
        for (int32_t yy = 0; yy < height - 0; yy++)
        {
            for (int32_t xx = 0; xx < width - 0; xx++)
            {
                if (*src != 0)
                {
                    // right
                    if (xx + skipX < dpi.width + dpi.pitch - 1)
                    {
                        *(dst + 1) = info->palette[3];
                    }
                    // left
                    if (xx + skipX > 1)
                    {
                        *(dst - 1) = info->palette[3];
                    }
                    // top
                    if (yy + skipY > 1)
                    {
                        *(dst - width - dstScanSkip) = info->palette[3];
                    }
                    // bottom
                    if (yy + skipY < dpi.height - 1)
                    {
                        *(dst + width + dstScanSkip) = info->palette[3];
                    }
                }
                src++;
                dst++;
            }
            // Skip any remaining bits
            src += srcScanSkip;
            dst += dstScanSkip;
        }
    }

    dst = dst_orig;
    src = src_orig;
    bool use_hinting = gConfigFonts.EnableHinting && fontDesc->hinting_threshold > 0;
    for (int32_t yy = 0; yy < height; yy++)
    {
        for (int32_t xx = 0; xx < width; xx++)
        {
            if (*src != 0)
            {
                if (info->flags & TEXT_DRAW_FLAG_INSET)
                {
                    *(dst + width + dstScanSkip + 1) = info->palette[3];
                }

                if (*src > 180 || !use_hinting)
                {
                    // Centre of the glyph: use full colour.
                    *dst = colour;
                }
                else if (use_hinting && *src > fontDesc->hinting_threshold)
                {
                    // Simulate font hinting by shading the background colour instead.
                    if (info->flags & TEXT_DRAW_FLAG_OUTLINE)
                    {
                        // As outlines are black, these texts should always use a darker shade
                        // of the foreground colour for font hinting.
                        *dst = BlendColours(colour, PALETTE_INDEX_0);
                    }
                    else
                    {
                        *dst = BlendColours(colour, *dst);
                    }
                }
            }
            src++;
            dst++;
        }
        src += srcScanSkip;
        dst += dstScanSkip;
    }
}

#endif // NO_TTF

static void TTFProcessFormatCode(DrawPixelInfo& dpi, const FmtString::Token& token, TextDrawInfo* info)
{
    switch (token.kind)
    {
        case FormatToken::Move:
            info->x = info->startX + token.parameter;
            break;
        case FormatToken::Newline:
            info->x = info->startX;
            info->y += FontGetLineHeight(info->FontStyle);
            break;
        case FormatToken::NewlineSmall:
            info->x = info->startX;
            info->y += FontGetLineHeightSmall(info->FontStyle);
            break;
        case FormatToken::FontTiny:
            info->FontStyle = FontStyle::Tiny;
            break;
        case FormatToken::FontSmall:
            info->FontStyle = FontStyle::Small;
            break;
        case FormatToken::FontMedium:
            info->FontStyle = FontStyle::Medium;
            break;
        case FormatToken::OutlineEnable:
            info->flags |= TEXT_DRAW_FLAG_OUTLINE;
            break;
        case FormatToken::OutlineDisable:
            info->flags &= ~TEXT_DRAW_FLAG_OUTLINE;
            break;
        case FormatToken::ColourWindow1:
        {
            uint16_t flags = info->flags;
            ColourCharacterWindow(gCurrentWindowColours[0], &flags, info->palette);
            break;
        }
        case FormatToken::ColourWindow2:
        {
            uint16_t flags = info->flags;
            ColourCharacterWindow(gCurrentWindowColours[1], &flags, info->palette);
            break;
        }
        case FormatToken::ColourWindow3:
        {
            uint16_t flags = info->flags;
            ColourCharacterWindow(gCurrentWindowColours[2], &flags, info->palette);
            break;
        }
        case FormatToken::InlineSprite:
        {
            auto imageId = ImageId::FromUInt32(token.parameter);
            auto g1 = GfxGetG1Element(imageId.GetIndex());
            if (g1 != nullptr && g1->width <= 32 && g1->height <= 32)
            {
                if (!(info->flags & TEXT_DRAW_FLAG_NO_DRAW))
                {
                    GfxDrawSprite(&dpi, imageId, { info->x, info->y });
                }
                info->x += g1->width;
            }
            break;
        }
        default:
            if (FormatTokenIsColour(token.kind))
            {
                uint16_t flags = info->flags;
                auto colourIndex = FormatTokenGetTextColourIndex(token.kind);
                ColourCharacter(static_cast<uint8_t>(colourIndex), &flags, info->palette);
            }
            break;
    }
}

#ifndef NO_TTF
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
#endif // NO_TTF

static void TTFProcessStringLiteral(DrawPixelInfo& dpi, std::string_view text, TextDrawInfo* info)
{
#ifndef NO_TTF
    bool isTTF = info->flags & TEXT_DRAW_FLAG_TTF;
#else
    bool isTTF = false;
#endif // NO_TTF

    if (!isTTF)
    {
        TTFDrawStringRawSprite(dpi, text, info);
    }
#ifndef NO_TTF
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
                    // TODO: Re-evaluate somewhere in 2023.
#    ifdef __MINGW32__
#        pragma GCC diagnostic push
#        pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#    endif
                    auto len = it.GetIndex() - ttfRunIndex.value();
                    TTFDrawStringRawTTF(dpi, text.substr(ttfRunIndex.value(), len), info);
#    ifdef __MINGW32__
#        pragma GCC diagnostic pop
#    endif
                    ttfRunIndex = std::nullopt;
                }

                // Draw the sprite font glyph
                TTFDrawCharacterSprite(dpi, codepoint, info);
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
            TTFDrawStringRawTTF(dpi, text.substr(ttfRunIndex.value(), len), info);
        }
    }
#endif // NO_TTF
}

static void TTFProcessStringCodepoint(DrawPixelInfo& dpi, codepoint_t codepoint, TextDrawInfo* info)
{
    char buffer[8]{};
    UTF8WriteCodepoint(buffer, codepoint);
    TTFProcessStringLiteral(dpi, buffer, info);
}

static void TTFProcessString(DrawPixelInfo& dpi, std::string_view text, TextDrawInfo* info)
{
    if (info->flags & TEXT_DRAW_FLAG_NO_FORMATTING)
    {
        TTFProcessStringLiteral(dpi, text, info);
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
                TTFProcessStringLiteral(dpi, token.text, info);
            }
            else if (token.IsCodepoint())
            {
                auto codepoint = token.GetCodepoint();
                TTFProcessStringCodepoint(dpi, codepoint, info);
            }
            else
            {
                TTFProcessFormatCode(dpi, token, info);
            }
            info->maxX = std::max(info->maxX, info->x);
            info->maxY = std::max(info->maxY, info->y);
        }
    }
}

static void TTFProcessInitialColour(int32_t colour, TextDrawInfo* info)
{
    if (colour != TEXT_COLOUR_254 && colour != TEXT_COLOUR_255)
    {
        info->flags &= ~(TEXT_DRAW_FLAG_INSET | TEXT_DRAW_FLAG_OUTLINE);
        if (colour & COLOUR_FLAG_OUTLINE)
        {
            info->flags |= TEXT_DRAW_FLAG_OUTLINE;
        }
        colour &= ~COLOUR_FLAG_OUTLINE;
        if (!(colour & COLOUR_FLAG_INSET))
        {
            if (!(info->flags & TEXT_DRAW_FLAG_INSET))
            {
                uint16_t flags = info->flags;
                ColourCharacterWindow(colour, &flags, reinterpret_cast<uint8_t*>(&info->palette));
            }
        }
        else
        {
            info->flags |= TEXT_DRAW_FLAG_INSET;
            colour &= ~COLOUR_FLAG_INSET;

            uint32_t eax;
            if (info->flags & TEXT_DRAW_FLAG_DARK)
            {
                if (info->flags & TEXT_DRAW_FLAG_EXTRA_DARK)
                {
                    eax = ColourMapA[colour].mid_light;
                    eax = eax << 16;
                    eax = eax | ColourMapA[colour].dark;
                }
                else
                {
                    eax = ColourMapA[colour].light;
                    eax = eax << 16;
                    eax = eax | ColourMapA[colour].mid_dark;
                }
            }
            else
            {
                eax = ColourMapA[colour].lighter;
                eax = eax << 16;
                eax = eax | ColourMapA[colour].mid_light;
            }

            // Adjust text palette. Store current colour? ;
            info->palette[1] = eax & 0xFF;
            info->palette[2] = (eax >> 8) & 0xFF;
            info->palette[3] = (eax >> 16) & 0xFF;
            info->palette[4] = (eax >> 24) & 0xFF;
        }
    }
}

void TTFDrawString(
    DrawPixelInfo& dpi, const_utf8string text, int32_t colour, const ScreenCoordsXY& coords, bool noFormatting,
    FontStyle fontStyle, TextDarkness darkness)
{
    if (text == nullptr)
        return;

    TextDrawInfo info;
    info.FontStyle = fontStyle;
    info.flags = 0;
    info.startX = coords.x;
    info.startY = coords.y;
    info.x = coords.x;
    info.y = coords.y;

    if (LocalisationService_UseTrueTypeFont())
    {
        info.flags |= TEXT_DRAW_FLAG_TTF;
    }

    if (noFormatting)
    {
        info.flags |= TEXT_DRAW_FLAG_NO_FORMATTING;
    }

    if (darkness == TextDarkness::Dark)
    {
        info.flags |= TEXT_DRAW_FLAG_DARK;
    }
    else if (darkness == TextDarkness::ExtraDark)
    {
        info.flags |= (TEXT_DRAW_FLAG_DARK | TEXT_DRAW_FLAG_EXTRA_DARK);
    }

    std::memcpy(info.palette, gTextPalette, sizeof(info.palette));
    TTFProcessInitialColour(colour, &info);
    TTFProcessString(dpi, text, &info);
    std::memcpy(gTextPalette, info.palette, sizeof(info.palette));

    dpi.lastStringPos = { info.x, info.y };
}

static int32_t TTFGetStringWidth(std::string_view text, FontStyle fontStyle, bool noFormatting)
{
    TextDrawInfo info;
    info.FontStyle = fontStyle;
    info.flags = 0;
    info.startX = 0;
    info.startY = 0;
    info.x = 0;
    info.y = 0;
    info.maxX = 0;
    info.maxY = 0;

    info.flags |= TEXT_DRAW_FLAG_NO_DRAW;
    if (LocalisationService_UseTrueTypeFont())
    {
        info.flags |= TEXT_DRAW_FLAG_TTF;
    }

    if (noFormatting)
    {
        info.flags |= TEXT_DRAW_FLAG_NO_FORMATTING;
    }

    DrawPixelInfo dummy{};
    TTFProcessString(dummy, text, &info);

    return info.maxX;
}

/**
 *
 *  rct2: 0x00682F28
 */
void GfxDrawStringWithYOffsets(
    DrawPixelInfo& dpi, const utf8* text, int32_t colour, const ScreenCoordsXY& coords, const int8_t* yOffsets,
    bool forceSpriteFont, FontStyle fontStyle)
{
    TextDrawInfo info;
    info.FontStyle = fontStyle;
    info.flags = 0;
    info.startX = coords.x;
    info.startY = coords.y;
    info.x = coords.x;
    info.y = coords.y;
    info.y_offset = yOffsets;

    info.flags |= TEXT_DRAW_FLAG_Y_OFFSET_EFFECT;

    if (!forceSpriteFont && LocalisationService_UseTrueTypeFont())
    {
        info.flags |= TEXT_DRAW_FLAG_TTF;
    }

    std::memcpy(info.palette, gTextPalette, sizeof(info.palette));
    TTFProcessInitialColour(colour, &info);
    TTFProcessString(dpi, text, &info);
    std::memcpy(gTextPalette, info.palette, sizeof(info.palette));

    dpi.lastStringPos = { info.x, info.y };
}

void ShortenPath(utf8* buffer, size_t bufferSize, const utf8* path, int32_t availableWidth, FontStyle fontStyle)
{
    size_t length = strlen(path);

    // Return full string if it fits
    if (GfxGetStringWidth(const_cast<char*>(path), fontStyle) <= availableWidth)
    {
        SafeStrCpy(buffer, path, bufferSize);
        return;
    }

    // Count path separators
    int32_t path_separators = 0;
    for (size_t x = 0; x < length; x++)
    {
        if (path[x] == *PATH_SEPARATOR || path[x] == '/')
        {
            path_separators++;
        }
    }

    // TODO: Replace with unicode ellipsis when supported
    SafeStrCpy(buffer, "...", bufferSize);

    // Abbreviate beginning with xth separator
    int32_t begin = -1;
    for (int32_t x = 0; x < path_separators; x++)
    {
        do
        {
            begin++;
        } while (path[begin] != *PATH_SEPARATOR && path[begin] != '/');

        SafeStrCpy(buffer + 3, path + begin, bufferSize - 3);
        if (GfxGetStringWidth(buffer, fontStyle) <= availableWidth)
        {
            return;
        }
    }

    SafeStrCpy(buffer, path, bufferSize);
}
