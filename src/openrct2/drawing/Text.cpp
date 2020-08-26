/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Text.h"

#include "../localisation/Localisation.h"
#include "Drawing.h"

static void DrawText(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const TextPaint& paint, const_utf8string text);
static void DrawText(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const TextPaint& paint, rct_string_id format, const void* args);

StaticLayout::StaticLayout(utf8string source, const TextPaint& paint, int32_t width)
{
    Buffer = source;
    Paint = paint;

    int32_t fontSpriteBase;

    gCurrentFontSpriteBase = paint.SpriteBase;
    MaxWidth = gfx_wrap_string(Buffer, width, &LineCount, &fontSpriteBase);
    LineCount += 1;
    LineHeight = font_get_line_height(fontSpriteBase);
}

void StaticLayout::Draw(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords)
{
    gCurrentFontFlags = 0;
    gCurrentFontSpriteBase = Paint.SpriteBase;

    TextPaint tempPaint = Paint;

    gCurrentFontFlags = 0;
    auto lineCoords = coords;
    switch (Paint.Alignment)
    {
        case TextAlignment::LEFT:
            break;
        case TextAlignment::CENTRE:
            lineCoords.x += MaxWidth / 2;
            break;
        case TextAlignment::RIGHT:
            lineCoords.x += MaxWidth;
            break;
    }
    utf8* buffer = Buffer;
    for (int32_t line = 0; line < LineCount; ++line)
    {
        DrawText(dpi, lineCoords, tempPaint, buffer);
        tempPaint.Colour = TEXT_COLOUR_254;
        buffer = get_string_end(buffer) + 1;
        lineCoords.y += LineHeight;
    }
}

int32_t StaticLayout::GetHeight()
{
    return LineHeight * LineCount;
}

int32_t StaticLayout::GetWidth()
{
    return MaxWidth;
}

int32_t StaticLayout::GetLineCount()
{
    return LineCount;
}

static void DrawText(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const TextPaint& paint, const_utf8string text)
{
    int32_t width = gfx_get_string_width(text);

    auto alignedCoords = coords;
    switch (paint.Alignment)
    {
        case TextAlignment::LEFT:
            break;
        case TextAlignment::CENTRE:
            alignedCoords.x -= (width - 1) / 2;
            break;
        case TextAlignment::RIGHT:
            alignedCoords.x -= width;
            break;
    }

    ttf_draw_string(dpi, text, paint.Colour, alignedCoords);

    if (paint.UnderlineText)
    {
        gfx_fill_rect(
            dpi, { { alignedCoords + ScreenCoordsXY{ 0, 11 } }, { alignedCoords + ScreenCoordsXY{ width, 11 } } },
            text_palette[1]);
        if (text_palette[2] != 0)
        {
            gfx_fill_rect(
                dpi, { { alignedCoords + ScreenCoordsXY{ 1, 12 } }, { alignedCoords + ScreenCoordsXY{ width + 1, 12 } } },
                text_palette[2]);
        }
    }
}

static void DrawText(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, const TextPaint& paint, rct_string_id format, const void* args)
{
    utf8 buffer[512];
    format_string(buffer, sizeof(buffer), format, args);
    DrawText(dpi, coords, paint, buffer);
}

void DrawTextBasic(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, rct_string_id format, const void* args, colour_t colour,
    TextAlignment alignment, bool underline)
{
    TextPaint textPaint = { colour, FONT_SPRITE_BASE_MEDIUM, underline, alignment };
    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    DrawText(dpi, coords, textPaint, format, args);
}

void DrawTextBasic(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, rct_string_id format, const Formatter& ft, colour_t colour,
    TextAlignment alignment, bool underline)
{
    return DrawTextBasic(dpi, coords, format, ft.GetStartBuf(), colour, alignment, underline);
}

void DrawTextEllipsised(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, rct_string_id format, const void* args,
    colour_t colour, TextAlignment alignment, bool underline)
{
    TextPaint textPaint = { colour, FONT_SPRITE_BASE_MEDIUM, underline, alignment };
    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

    utf8 buffer[512];
    format_string(buffer, sizeof(buffer), format, args);
    gfx_clip_string(buffer, width);

    DrawText(dpi, coords, textPaint, buffer);
}

void DrawTextEllipsised(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, rct_string_id format, const Formatter& ft,
    colour_t colour, TextAlignment alignment, bool underline)
{
    return DrawTextEllipsised(dpi, coords, width, format, ft.GetStartBuf(), colour, alignment, underline);
}

void gfx_draw_string(rct_drawpixelinfo* dpi, const_utf8string buffer, uint8_t colour, const ScreenCoordsXY& coords)
{
    TextPaint textPaint = { colour, gCurrentFontSpriteBase, false, TextAlignment::LEFT };
    DrawText(dpi, coords, textPaint, buffer);
}

// Basic
void gfx_draw_string_left(
    rct_drawpixelinfo* dpi, rct_string_id format, void* args, uint8_t colour, const ScreenCoordsXY& coords)
{
    DrawTextBasic(dpi, coords, format, args, colour, TextAlignment::LEFT);
}

void gfx_draw_string_centred(
    rct_drawpixelinfo* dpi, rct_string_id format, const ScreenCoordsXY& coords, uint8_t colour, const void* args)
{
    DrawTextBasic(dpi, coords, format, args, colour, TextAlignment::CENTRE);
}

void gfx_draw_string_right(
    rct_drawpixelinfo* dpi, rct_string_id format, void* args, uint8_t colour, const ScreenCoordsXY& coords)
{
    DrawTextBasic(dpi, coords, format, args, colour, TextAlignment::RIGHT);
}

// Ellipsised
void gfx_draw_string_left_clipped(
    rct_drawpixelinfo* dpi, rct_string_id format, void* args, uint8_t colour, const ScreenCoordsXY& coords, int32_t width)
{
    DrawTextEllipsised(dpi, coords, width, format, args, colour, TextAlignment::LEFT);
}

void gfx_draw_string_centred_clipped(
    rct_drawpixelinfo* dpi, rct_string_id format, void* args, uint8_t colour, const ScreenCoordsXY& coords, int32_t width)
{
    DrawTextEllipsised(dpi, coords, width, format, args, colour, TextAlignment::CENTRE);
}

void gfx_draw_string_right_clipped(
    rct_drawpixelinfo* dpi, rct_string_id format, void* args, uint8_t colour, const ScreenCoordsXY& coords, int32_t width)
{
    DrawTextEllipsised(dpi, coords, width, format, args, colour, TextAlignment::RIGHT);
}

// Wrapping
int32_t gfx_draw_string_left_wrapped(
    rct_drawpixelinfo* dpi, void* args, const ScreenCoordsXY& coords, int32_t width, rct_string_id format, uint8_t colour)
{
    utf8 buffer[512];
    format_string(buffer, sizeof(buffer), format, args);

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

    TextPaint textPaint = { colour, gCurrentFontSpriteBase, false, TextAlignment::LEFT };
    StaticLayout layout(buffer, textPaint, width);
    layout.Draw(dpi, coords);

    return layout.GetHeight();
}

int32_t gfx_draw_string_centred_wrapped(
    rct_drawpixelinfo* dpi, void* args, const ScreenCoordsXY& coords, int32_t width, rct_string_id format, uint8_t colour)
{
    utf8 buffer[512];
    format_string(buffer, sizeof(buffer), format, args);

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

    TextPaint textPaint = { colour, gCurrentFontSpriteBase, false, TextAlignment::CENTRE };
    StaticLayout layout(buffer, textPaint, width);

    // The original tried to vertically centre the text, but used line count - 1
    int32_t lineCount = layout.GetLineCount();
    int32_t lineHeight = layout.GetHeight() / lineCount;
    int32_t yOffset = (lineCount - 1) * lineHeight / 2;

    layout.Draw(dpi, coords - ScreenCoordsXY{ layout.GetWidth() / 2, yOffset });

    return layout.GetHeight();
}
