/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../config/Config.h"
#include "../core/CodepointView.hpp"
#include "../core/String.hpp"
#include "../core/UTF8.h"
#include "../core/UnicodeChar.h"
#include "../drawing/IDrawingContext.h"
#include "../drawing/IDrawingEngine.h"
#include "../drawing/Text.h"
#include "../interface/Viewport.h"
#include "../localisation/Formatting.h"
#include "../localisation/LocalisationService.h"
#include "../platform/Platform.h"
#include "../sprites.h"
#include "TTF.h"

using namespace OpenRCT2;

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
        else if (token.kind == FormatToken::Newline)
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
    DrawPixelInfo& dpi, StringId format, void* args, ColourWithFlags colour, const ScreenCoordsXY& coords)
{
    char buffer[512];
    auto bufferPtr = buffer;
    FormatStringLegacy(bufferPtr, sizeof(buffer), format, args);
    int32_t height = StringGetHeightRaw(bufferPtr, FontStyle::Medium);
    DrawText(dpi, coords - ScreenCoordsXY{ 0, (height / 2) }, { colour }, bufferPtr);
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
static void ColourCharacterWindow(colour_t colour, const uint16_t* current_font_flags, uint8_t* palette_pointer)
{
    int32_t eax;

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
    DrawText(dpi, screenCoords, { COLOUR_BLACK, fontStyle }, "");
    screenCoords = coords;

    for (int32_t i = 0; i <= numLines; i++)
    {
        int32_t width = GfxGetStringWidth(text, fontStyle);
        DrawText(dpi, screenCoords - ScreenCoordsXY{ width / 2, 0 }, { kTextColour254, fontStyle }, text);

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

    DrawText(dpi, screenCoords, { colour }, "");

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
        DrawText(dpi, screenCoords, { kTextColour254, FontStyle::Small }, buffer);

        if (numCharactersDrawn > numCharactersToDraw)
        {
            break;
        }

        buffer = GetStringEnd(buffer) + 1;
        lineY += lineHeight;
    }
}

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
        GfxDrawGlyph(dpi, sprite, screenCoords, paletteMap);
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

    TTFSurface* surface = TTFSurfaceCacheGetOrAdd(fontDesc->font, text);
    if (surface == nullptr)
        return;

    auto drawingEngine = dpi.DrawingEngine;
    if (drawingEngine != nullptr)
    {
        int32_t drawX = info->x + fontDesc->offset_x;
        int32_t drawY = info->y + fontDesc->offset_y;
        uint8_t hintThresh = Config::Get().fonts.EnableHinting ? fontDesc->hinting_threshold : 0;
        OpenRCT2::Drawing::IDrawingContext* dc = drawingEngine->GetDrawingContext();
        dc->DrawTTFBitmap(dpi, info, surface, drawX, drawY, hintThresh);
    }
    info->x += surface->w;
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
            auto imageId = ImageId(token.parameter);
            auto g1 = GfxGetG1Element(imageId);
            if (g1 != nullptr && g1->width <= 32 && g1->height <= 32)
            {
                if (!(info->flags & TEXT_DRAW_FLAG_NO_DRAW))
                {
                    GfxDrawSprite(dpi, imageId, { info->x, info->y });
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
                    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105937 is fixed in GCC13
    #if defined(__GNUC__) && !defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
    #endif
                    auto len = it.GetIndex() - ttfRunIndex.value();
                    TTFDrawStringRawTTF(dpi, text.substr(ttfRunIndex.value(), len), info);
    #if defined(__GNUC__) && !defined(__clang__)
        #pragma GCC diagnostic pop
    #endif
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

static void TTFProcessInitialColour(ColourWithFlags colour, TextDrawInfo* info)
{
    if (colour.colour != kTextColour254 && colour.colour != kTextColour255)
    {
        info->flags &= ~(TEXT_DRAW_FLAG_INSET | TEXT_DRAW_FLAG_OUTLINE);
        if (colour.hasFlag(ColourFlag::withOutline))
        {
            info->flags |= TEXT_DRAW_FLAG_OUTLINE;
        }
        if (!colour.hasFlag(ColourFlag::inset))
        {
            if (!(info->flags & TEXT_DRAW_FLAG_INSET))
            {
                uint16_t flags = info->flags;
                ColourCharacterWindow(colour.colour, &flags, reinterpret_cast<uint8_t*>(&info->palette));
            }
        }
        else
        {
            info->flags |= TEXT_DRAW_FLAG_INSET;

            uint32_t eax;
            if (info->flags & TEXT_DRAW_FLAG_DARK)
            {
                if (info->flags & TEXT_DRAW_FLAG_EXTRA_DARK)
                {
                    eax = ColourMapA[colour.colour].mid_light;
                    eax = eax << 16;
                    eax = eax | ColourMapA[colour.colour].dark;
                }
                else
                {
                    eax = ColourMapA[colour.colour].light;
                    eax = eax << 16;
                    eax = eax | ColourMapA[colour.colour].mid_dark;
                }
            }
            else
            {
                eax = ColourMapA[colour.colour].lighter;
                eax = eax << 16;
                eax = eax | ColourMapA[colour.colour].mid_light;
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
    DrawPixelInfo& dpi, const_utf8string text, ColourWithFlags colour, const ScreenCoordsXY& coords, bool noFormatting,
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
    DrawPixelInfo& dpi, const utf8* text, ColourWithFlags colour, const ScreenCoordsXY& coords, const int8_t* yOffsets,
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
