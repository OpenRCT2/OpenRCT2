/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"
#include "../config/Config.h"
#include "../drawing/Drawing.h"
#include "../interface/Viewport.h"
#include "../localisation/Formatting.h"
#include "../localisation/Localisation.h"
#include "../localisation/LocalisationService.h"
#include "../platform/platform.h"
#include "../sprites.h"
#include "../util/Util.h"
#include "TTF.h"

#include <algorithm>

using namespace OpenRCT2;

class CodepointView
{
private:
    std::string_view _str;

public:
    class iterator
    {
    private:
        std::string_view _str;
        size_t _index;

    public:
        iterator(std::string_view str, size_t index)
            : _str(str)
            , _index(index)
        {
        }

        bool operator==(const iterator& rhs) const
        {
            return _index == rhs._index;
        }
        bool operator!=(const iterator& rhs) const
        {
            return _index != rhs._index;
        }
        const char32_t operator*() const
        {
            return utf8_get_next(&_str[_index], nullptr);
        }
        iterator& operator++()
        {
            if (_index < _str.size())
            {
                const utf8* nextch;
                utf8_get_next(&_str[_index], &nextch);
                _index = nextch - _str.data();
            }
            return *this;
        }
        iterator operator++(int)
        {
            auto result = *this;
            if (_index < _str.size())
            {
                const utf8* nextch;
                utf8_get_next(&_str[_index], &nextch);
                _index = nextch - _str.data();
            }
            return result;
        }

        size_t GetIndex() const
        {
            return _index;
        }
    };

    CodepointView(std::string_view str)
        : _str(str)
    {
    }

    iterator begin() const
    {
        return iterator(_str, 0);
    }

    iterator end() const
    {
        return iterator(_str, _str.size());
    }
};

enum : uint32_t
{
    TEXT_DRAW_FLAG_INSET = 1 << 0,
    TEXT_DRAW_FLAG_OUTLINE = 1 << 1,
    TEXT_DRAW_FLAG_DARK = 1 << 2,
    TEXT_DRAW_FLAG_EXTRA_DARK = 1 << 3,
    TEXT_DRAW_FLAG_Y_OFFSET_EFFECT = 1 << 29,
    TEXT_DRAW_FLAG_TTF = 1 << 30,
    TEXT_DRAW_FLAG_NO_DRAW = 1u << 31
};

static int32_t ttf_get_string_width(std::string_view text);

/**
 *
 *  rct2: 0x006C23B1
 */
int32_t gfx_get_string_width_new_lined(utf8* text)
{
    utf8* ch = text;
    utf8* firstCh = text;
    utf8* nextCh;
    utf8 backup;
    int32_t codepoint;

    int32_t maxWidth = 0;
    while ((codepoint = utf8_get_next(ch, const_cast<const utf8**>(&nextCh))) != 0)
    {
        if (codepoint == FORMAT_NEWLINE || codepoint == FORMAT_NEWLINE_SMALLER)
        {
            backup = *nextCh;
            *nextCh = 0;
            maxWidth = std::max(maxWidth, gfx_get_string_width(firstCh));
            *nextCh = backup;
            firstCh = nextCh;
        }
        ch = nextCh;
    }
    maxWidth = std::max(maxWidth, gfx_get_string_width(firstCh));

    return maxWidth;
}

/**
 * Return the width of the string in buffer
 *
 *  rct2: 0x006C2321
 * buffer (esi)
 */
int32_t gfx_get_string_width(std::string_view text)
{
    return ttf_get_string_width(text);
}

/**
 * Clip the text in buffer to width, add ellipsis and return the new width of the clipped string
 *
 *  rct2: 0x006C2460
 * buffer (esi)
 * width (edi)
 */
int32_t gfx_clip_string(utf8* text, int32_t width)
{
    int32_t clippedWidth;

    if (width < 6)
    {
        *text = 0;
        return 0;
    }

    clippedWidth = gfx_get_string_width(text);
    if (clippedWidth <= width)
    {
        return clippedWidth;
    }

    utf8 backup[4];
    utf8* ch = text;
    utf8* nextCh = text;
    utf8* clipCh = text;
    int32_t codepoint;
    while ((codepoint = utf8_get_next(ch, const_cast<const utf8**>(&nextCh))) != 0)
    {
        if (utf8_is_format_code(codepoint))
        {
            ch = nextCh;
            ch += utf8_get_format_code_arg_length(codepoint);
            continue;
        }

        for (int32_t i = 0; i < 4; i++)
        {
            backup[i] = nextCh[i];
        };
        for (int32_t i = 0; i < 3; i++)
        {
            nextCh[i] = '.';
        }
        nextCh[3] = 0;

        int32_t queryWidth = gfx_get_string_width(text);
        if (queryWidth < width)
        {
            clipCh = nextCh;
            clippedWidth = queryWidth;
        }
        else
        {
            for (int32_t i = 0; i < 3; i++)
            {
                clipCh[i] = '.';
            }
            clipCh[3] = 0;
            return clippedWidth;
        }

        for (int32_t i = 0; i < 4; i++)
        {
            nextCh[i] = backup[i];
        };
        ch = nextCh;
    }
    return gfx_get_string_width(text);
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
int32_t gfx_wrap_string(utf8* text, int32_t width, int32_t* outNumLines, int32_t* outFontHeight)
{
    constexpr size_t NULL_INDEX = std::numeric_limits<size_t>::max();
    thread_local std::string buffer;
    buffer.resize(0);

    size_t currentLineIndex = 0;
    size_t splitIndex = NULL_INDEX;
    size_t numLines = 0;
    int32_t maxWidth = 0;

    FmtString fmt = text;
    for (const auto& token : fmt)
    {
        if (token.IsLiteral())
        {
            CodepointView codepoints(token.text);
            for (auto codepoint : codepoints)
            {
                buffer.push_back(codepoint);

                auto lineWidth = gfx_get_string_width(&buffer[currentLineIndex]);
                if (lineWidth <= width || splitIndex == NULL_INDEX)
                {
                    if (codepoint == ' ')
                    {
                        // Mark line split here
                        splitIndex = buffer.size() - 1;
                    }
                    else if (splitIndex == NULL_INDEX)
                    {
                        // Mark line split here (this is after first character of line)
                        splitIndex = buffer.size();
                    }
                }
                else
                {
                    // Insert new line before current word
                    buffer.insert(buffer.begin() + splitIndex, '\0');

                    // Recalculate the line length after splitting
                    lineWidth = gfx_get_string_width(&buffer[currentLineIndex]);
                    maxWidth = std::max(maxWidth, lineWidth);
                    numLines++;

                    currentLineIndex = splitIndex + 1;
                    splitIndex = NULL_INDEX;

                    // Trim the beginning of the new line
                    while (buffer[currentLineIndex] == ' ')
                    {
                        buffer.erase(buffer.begin() + currentLineIndex);
                    }
                }
            }
        }
        else if (token.kind == FORMAT_NEWLINE)
        {
            buffer.push_back('\0');

            auto lineWidth = gfx_get_string_width(&buffer[currentLineIndex]);
            maxWidth = std::max(maxWidth, lineWidth);
            numLines++;
            splitIndex = buffer.size();
        }
        else
        {
            buffer.append(token.text);
        }
    }
    {
        // Final line width calculation
        auto lineWidth = gfx_get_string_width(&buffer[currentLineIndex]);
        maxWidth = std::max(maxWidth, lineWidth);
    }

    std::memcpy(text, buffer.data(), buffer.size() + 1);
    *outNumLines = static_cast<int32_t>(numLines);
    *outFontHeight = gCurrentFontSpriteBase;
    return maxWidth;
}

/**
 * Draws text that is left aligned and vertically centred.
 */
void gfx_draw_string_left_centred(
    rct_drawpixelinfo* dpi, rct_string_id format, void* args, int32_t colour, const ScreenCoordsXY& coords)
{
    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    char* buffer = gCommonStringFormatBuffer;
    format_string(buffer, 256, format, args);
    int32_t height = string_get_height_raw(buffer);
    gfx_draw_string(dpi, buffer, colour, coords - ScreenCoordsXY{ 0, (height / 2) });
}

/**
 * Changes the palette so that the next character changes colour
 */
static void colour_char(uint8_t colour, const uint16_t* current_font_flags, uint8_t* palette_pointer)
{
    int32_t colour32 = 0;
    const rct_g1_element* g1 = gfx_get_g1_element(SPR_TEXT_PALETTE);
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
static void colour_char_window(uint8_t colour, const uint16_t* current_font_flags, uint8_t* palette_pointer)
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
void draw_string_centred_raw(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t numLines, char* text)
{
    ScreenCoordsXY screenCoords(dpi->x, dpi->y);
    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    gfx_draw_string(dpi, "", COLOUR_BLACK, screenCoords);
    screenCoords = coords;
    gCurrentFontFlags = 0;

    for (int32_t i = 0; i <= numLines; i++)
    {
        int32_t width = gfx_get_string_width(text);
        gfx_draw_string(dpi, text, TEXT_COLOUR_254, screenCoords - ScreenCoordsXY{ width / 2, 0 });

        const utf8* ch = text;
        const utf8* nextCh = nullptr;

        while ((utf8_get_next(ch, &nextCh)) != 0)
        {
            ch = nextCh;
        }
        text = const_cast<char*>(ch + 1);

        screenCoords.y += font_get_line_height(gCurrentFontSpriteBase);
    }
}

int32_t string_get_height_raw(char* buffer)
{
    uint16_t fontBase = gCurrentFontSpriteBase;

    int32_t height = 0;
    if (fontBase <= FONT_SPRITE_BASE_MEDIUM)
        height += 10;
    else if (fontBase == FONT_SPRITE_BASE_TINY)
        height += 6;

    char* ch = buffer;
    while (*ch != 0)
    {
        char c = *ch++;
        switch (c)
        {
            case FORMAT_NEWLINE:
                if (fontBase == FONT_SPRITE_BASE_SMALL || fontBase == FONT_SPRITE_BASE_MEDIUM)
                {
                    height += 10;
                    break;
                }
                else if (fontBase == FONT_SPRITE_BASE_TINY)
                {
                    height += 6;
                    break;
                }
                height += 18;
                break;
            case FORMAT_NEWLINE_SMALLER:
                if (fontBase == FONT_SPRITE_BASE_SMALL || fontBase == FONT_SPRITE_BASE_MEDIUM)
                {
                    height += 5;
                    break;
                }
                else if (fontBase == FONT_SPRITE_BASE_TINY)
                {
                    height += 3;
                    break;
                }
                height += 9;
                break;
            case FORMAT_TINYFONT:
                fontBase = FONT_SPRITE_BASE_TINY;
                break;
            case FORMAT_MEDIUMFONT:
                fontBase = FONT_SPRITE_BASE_MEDIUM;
                break;
            case FORMAT_SMALLFONT:
                fontBase = FONT_SPRITE_BASE_SMALL;
                break;
            default:
                if (c >= 32)
                    continue;
                if (c <= 4)
                {
                    ch++;
                    continue;
                }
                if (c <= 16)
                    continue;
                ch += 2;
                if (c <= 22)
                    continue;
                ch += 2;
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
void gfx_draw_string_centred_wrapped_partial(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, int32_t colour, rct_string_id format, void* args,
    int32_t ticks)
{
    int32_t numLines, fontSpriteBase, lineHeight, lineY;
    utf8* buffer = gCommonStringFormatBuffer;
    ScreenCoordsXY screenCoords(dpi->x, dpi->y);

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    gfx_draw_string(dpi, "", colour, screenCoords);
    format_string(buffer, 256, format, args);

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    gfx_wrap_string(buffer, width, &numLines, &fontSpriteBase);
    lineHeight = font_get_line_height(fontSpriteBase);

    int32_t numCharactersDrawn = 0;
    int32_t numCharactersToDraw = ticks;

    gCurrentFontFlags = 0;
    lineY = coords.y - ((numLines * lineHeight) / 2);
    for (int32_t line = 0; line <= numLines; line++)
    {
        int32_t halfWidth = gfx_get_string_width(buffer) / 2;

        utf8* ch = buffer;
        utf8* nextCh;
        int32_t codepoint;
        while ((codepoint = utf8_get_next(ch, const_cast<const utf8**>(&nextCh))) != 0)
        {
            if (!utf8_is_format_code(codepoint))
            {
                numCharactersDrawn++;
                if (numCharactersDrawn > numCharactersToDraw)
                {
                    *ch = 0;
                    break;
                }
            }
            ch = nextCh;
        }

        screenCoords = { coords.x - halfWidth, lineY };
        gfx_draw_string(dpi, buffer, TEXT_COLOUR_254, screenCoords);

        if (numCharactersDrawn > numCharactersToDraw)
        {
            break;
        }

        buffer = get_string_end(buffer) + 1;
        lineY += lineHeight;
    }
}

struct text_draw_info
{
    int32_t startX;
    int32_t startY;
    int32_t x;
    int32_t y;
    int32_t maxX;
    int32_t maxY;
    int32_t flags;
    uint8_t palette[8];
    uint16_t font_sprite_base;
    const int8_t* y_offset;
};

static void ttf_draw_character_sprite(rct_drawpixelinfo* dpi, int32_t codepoint, text_draw_info* info)
{
    int32_t characterWidth = font_sprite_get_codepoint_width(info->font_sprite_base, codepoint);
    int32_t sprite = font_sprite_get_codepoint_sprite(info->font_sprite_base, codepoint);

    if (!(info->flags & TEXT_DRAW_FLAG_NO_DRAW))
    {
        auto screenCoords = ScreenCoordsXY{ info->x, info->y };
        if (info->flags & TEXT_DRAW_FLAG_Y_OFFSET_EFFECT)
        {
            screenCoords.y += *info->y_offset++;
        }

        PaletteMap paletteMap(info->palette);
        gfx_draw_glyph(dpi, sprite, screenCoords, paletteMap);
    }

    info->x += characterWidth;
}

static void ttf_draw_string_raw_sprite(rct_drawpixelinfo* dpi, const std::string_view text, text_draw_info* info)
{
    CodepointView codepoints(text);
    for (auto codepoint : codepoints)
    {
        ttf_draw_character_sprite(dpi, codepoint, info);
    }
}

#ifndef NO_TTF

static void ttf_draw_string_raw_ttf(rct_drawpixelinfo* dpi, std::string_view text, text_draw_info* info)
{
    if (!ttf_initialise())
        return;

    TTFFontDescriptor* fontDesc = ttf_get_font_from_sprite_base(info->font_sprite_base);
    if (fontDesc->font == nullptr)
    {
        ttf_draw_string_raw_sprite(dpi, text, info);
        return;
    }

    if (info->flags & TEXT_DRAW_FLAG_NO_DRAW)
    {
        info->x += ttf_getwidth_cache_get_or_add(fontDesc->font, text);
        return;
    }
    else
    {
        uint8_t colour = info->palette[1];
        TTFSurface* surface = ttf_surface_cache_get_or_add(fontDesc->font, text);
        if (surface == nullptr)
            return;

        int32_t drawX = info->x + fontDesc->offset_x;
        int32_t drawY = info->y + fontDesc->offset_y;
        int32_t width = surface->w;
        int32_t height = surface->h;

        int32_t overflowX = (dpi->x + dpi->width) - (drawX + width);
        int32_t overflowY = (dpi->y + dpi->height) - (drawY + height);
        if (overflowX < 0)
            width += overflowX;
        if (overflowY < 0)
            height += overflowY;
        int32_t skipX = drawX - dpi->x;
        int32_t skipY = drawY - dpi->y;
        info->x += width;

        auto src = static_cast<const uint8_t*>(surface->pixels);
        uint8_t* dst = dpi->bits;

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
        dst += skipY * (dpi->width + dpi->pitch);

        int32_t srcScanSkip = surface->pitch - width;
        int32_t dstScanSkip = dpi->width + dpi->pitch - width;
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
                        if (xx + skipX < dpi->width + dpi->pitch - 1)
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
                        if (yy + skipY < dpi->height - 1)
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
        {
            dst = dst_orig;
            src = src_orig;
            bool use_hinting = gConfigFonts.enable_hinting && fontDesc->hinting_threshold > 0;
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
                                *dst = blendColours(colour, PALETTE_INDEX_0);
                            }
                            else
                            {
                                *dst = blendColours(colour, *dst);
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
    }
}

#endif // NO_TTF

static void ttf_process_format_code(rct_drawpixelinfo* dpi, const FmtString::token& token, text_draw_info* info)
{
    switch (token.kind)
    {
        case FORMAT_MOVE_X:
            // info->x = info->startX + static_cast<uint8_t>(*nextCh++);
            break;
        case FORMAT_ADJUST_PALETTE:
        {
            // auto paletteMapId = static_cast<colour_t>(*nextCh++);
            // auto paletteMap = GetPaletteMapForColour(paletteMapId);
            // if (paletteMap)
            // {
            //     uint32_t c = (*paletteMap)[249] + 256;
            //     if (!(info->flags & TEXT_DRAW_FLAG_OUTLINE))
            //     {
            //         c &= 0xFF;
            //     }
            //     info->palette[1] = c & 0xFF;
            //     info->palette[2] = (c >> 8) & 0xFF;
            //
            //     // Adjust the text palette
            //     info->palette[3] = (*paletteMap)[247];
            //     info->palette[4] = (*paletteMap)[248];
            //     info->palette[5] = (*paletteMap)[250];
            //     info->palette[6] = (*paletteMap)[251];
            // }
            break;
        }
        case FORMAT_3:
        case FORMAT_4:
            // nextCh++;
            break;
        case FORMAT_NEWLINE:
            info->x = info->startX;
            info->y += font_get_line_height(info->font_sprite_base);
            break;
        case FORMAT_NEWLINE_SMALLER:
            info->x = info->startX;
            info->y += font_get_line_height_small(info->font_sprite_base);
            break;
        case FORMAT_TINYFONT:
            info->font_sprite_base = FONT_SPRITE_BASE_TINY;
            break;
        case FORMAT_SMALLFONT:
            info->font_sprite_base = FONT_SPRITE_BASE_SMALL;
            break;
        case FORMAT_MEDIUMFONT:
            info->font_sprite_base = FONT_SPRITE_BASE_MEDIUM;
            break;
        case FORMAT_OUTLINE:
            info->flags |= TEXT_DRAW_FLAG_OUTLINE;
            break;
        case FORMAT_OUTLINE_OFF:
            info->flags &= ~TEXT_DRAW_FLAG_OUTLINE;
            break;
        case FORMAT_WINDOW_COLOUR_1:
        {
            uint16_t flags = info->flags;
            colour_char_window(gCurrentWindowColours[0], &flags, info->palette);
            break;
        }
        case FORMAT_WINDOW_COLOUR_2:
        {
            uint16_t flags = info->flags;
            colour_char_window(gCurrentWindowColours[1], &flags, info->palette);
            break;
        }
        case FORMAT_WINDOW_COLOUR_3:
        {
            uint16_t flags = info->flags;
            colour_char_window(gCurrentWindowColours[2], &flags, info->palette);
            break;
        }
        case FORMAT_16:
            break;
        // case FORMAT_INLINE_SPRITE:
        // {
        //     uint32_t imageId;
        //     std::memcpy(&imageId, nextCh, sizeof(uint32_t));
        //     const rct_g1_element* g1 = gfx_get_g1_element(imageId & 0x7FFFF);
        //     if (g1 != nullptr)
        //     {
        //         if (!(info->flags & TEXT_DRAW_FLAG_NO_DRAW))
        //         {
        //             gfx_draw_sprite(dpi, imageId, { info->x, info->y }, 0);
        //         }
        //         info->x += g1->width;
        //     }
        //     nextCh += 4;
        //     break;
        // }
        default:
            if (token.kind >= FORMAT_COLOUR_CODE_START && token.kind <= FORMAT_COLOUR_CODE_END)
            {
                uint16_t flags = info->flags;
                colour_char(token.kind - FORMAT_COLOUR_CODE_START, &flags, info->palette);
            }
            else if (token.kind <= 0x16)
            { // case 0x11? FORMAT_NEW_LINE_X_Y
              // nextCh += 2;
            }
            else
            {
                // nextCh += 4; // never happens?
            }
            break;
    }
}

static void ttf_process_string_literal(rct_drawpixelinfo* dpi, const std::string_view text, text_draw_info* info)
{
#ifndef NO_TTF
    bool isTTF = info->flags & TEXT_DRAW_FLAG_TTF;
#else
    bool isTTF = false;
#endif // NO_TTF

    if (!isTTF)
    {
        ttf_draw_string_raw_sprite(dpi, text, info);
    }
    else
    {
        CodepointView codepoints(text);
        size_t ttfRunIndex = 0;
        for (auto it = codepoints.begin(); it != codepoints.end(); it++)
        {
            auto codepoint = *it;
            if (utf8_should_use_sprite_for_codepoint(codepoint))
            {
                auto index = it.GetIndex();
                auto ttfLen = index - ttfRunIndex;
                if (ttfLen > 0)
                {
                    // Draw the TTF run
                    ttf_draw_string_raw_ttf(dpi, text.substr(ttfRunIndex, ttfLen), info);
                }
                ttfRunIndex = index;

                // Draw the sprite font glyph
                ttf_draw_character_sprite(dpi, codepoint, info);
            }
        }
    }
}

static void ttf_process_string(rct_drawpixelinfo* dpi, std::string_view text, text_draw_info* info)
{
    FmtString fmt(text);
    for (const auto& token : fmt)
    {
        if (token.IsLiteral())
        {
            ttf_process_string_literal(dpi, token.text, info);
        }
        else
        {
            ttf_process_format_code(dpi, token, info);
        }
        info->maxX = std::max(info->maxX, info->x);
        info->maxY = std::max(info->maxY, info->y);
    }
}

static void ttf_process_initial_colour(int32_t colour, text_draw_info* info)
{
    if (colour != TEXT_COLOUR_254 && colour != TEXT_COLOUR_255)
    {
        info->flags &= ~(TEXT_DRAW_FLAG_INSET | TEXT_DRAW_FLAG_OUTLINE | TEXT_DRAW_FLAG_DARK | TEXT_DRAW_FLAG_EXTRA_DARK);
        if (static_cast<int16_t>(info->font_sprite_base) < 0)
        {
            info->flags |= TEXT_DRAW_FLAG_DARK;
            if (static_cast<int16_t>(info->font_sprite_base) == FONT_SPRITE_BASE_MEDIUM_EXTRA_DARK)
            {
                info->flags |= TEXT_DRAW_FLAG_EXTRA_DARK;
            }
            info->font_sprite_base = FONT_SPRITE_BASE_MEDIUM;
        }
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
                colour_char_window(colour, &flags, reinterpret_cast<uint8_t*>(&info->palette));
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
            eax = 0;
        }
    }
}

void ttf_draw_string(rct_drawpixelinfo* dpi, const_utf8string text, int32_t colour, const ScreenCoordsXY& coords)
{
    if (text == nullptr)
        return;

    text_draw_info info;
    info.font_sprite_base = gCurrentFontSpriteBase;
    info.flags = gCurrentFontFlags;
    info.startX = coords.x;
    info.startY = coords.y;
    info.x = coords.x;
    info.y = coords.y;

    if (LocalisationService_UseTrueTypeFont())
    {
        info.flags |= TEXT_DRAW_FLAG_TTF;
    }

    std::memcpy(info.palette, text_palette, sizeof(info.palette));
    ttf_process_initial_colour(colour, &info);
    ttf_process_string(dpi, text, &info);
    std::memcpy(text_palette, info.palette, sizeof(info.palette));

    gCurrentFontSpriteBase = info.font_sprite_base;
    gCurrentFontFlags = info.flags;

    gLastDrawStringX = info.x;
    gLastDrawStringY = info.y;
}

static int32_t ttf_get_string_width(std::string_view text)
{
    text_draw_info info;
    info.font_sprite_base = gCurrentFontSpriteBase;
    info.flags = gCurrentFontFlags;
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

    ttf_process_string(nullptr, text, &info);

    return info.maxX;
}

/**
 *
 *  rct2: 0x00682F28
 */
void gfx_draw_string_with_y_offsets(
    rct_drawpixelinfo* dpi, const utf8* text, int32_t colour, const ScreenCoordsXY& coords, const int8_t* yOffsets,
    bool forceSpriteFont)
{
    text_draw_info info;
    info.font_sprite_base = gCurrentFontSpriteBase;
    info.flags = gCurrentFontFlags;
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

    std::memcpy(info.palette, text_palette, sizeof(info.palette));
    ttf_process_initial_colour(colour, &info);
    ttf_process_string(dpi, text, &info);
    std::memcpy(text_palette, info.palette, sizeof(info.palette));

    gCurrentFontSpriteBase = info.font_sprite_base;
    gCurrentFontFlags = info.flags;

    gLastDrawStringX = info.x;
    gLastDrawStringY = info.y;
}

void shorten_path(utf8* buffer, size_t bufferSize, const utf8* path, int32_t availableWidth)
{
    size_t length = strlen(path);

    // Return full string if it fits
    if (gfx_get_string_width(const_cast<char*>(path)) <= availableWidth)
    {
        safe_strcpy(buffer, path, bufferSize);
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
    safe_strcpy(buffer, "...", bufferSize);

    // Abbreviate beginning with xth separator
    int32_t begin = -1;
    for (int32_t x = 0; x < path_separators; x++)
    {
        do
        {
            begin++;
        } while (path[begin] != *PATH_SEPARATOR && path[begin] != '/');

        safe_strcpy(buffer + 3, path + begin, bufferSize - 3);
        if (gfx_get_string_width(buffer) <= availableWidth)
        {
            return;
        }
    }

    safe_strcpy(buffer, path, bufferSize);
}
