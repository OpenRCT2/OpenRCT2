/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.h"
#include "Text.h"

#include "../localisation/Localisation.h"

static TextPaint _legacyPaint;

static void DrawText(rct_drawpixelinfo * dpi, int32_t x, int32_t y, TextPaint * paint, const_utf8string text);
static void DrawText(rct_drawpixelinfo * dpi, int32_t x, int32_t y, TextPaint * paint, rct_string_id format, void * args);

StaticLayout::StaticLayout(utf8string source, TextPaint paint, int32_t width)
{
    _buffer = source;
    _paint = paint;

    int32_t fontSpriteBase;

    gCurrentFontSpriteBase = paint.SpriteBase;
    _maxWidth = gfx_wrap_string(_buffer, width, &_lineCount, &fontSpriteBase);
    _lineCount += 1;
    _lineHeight = font_get_line_height(fontSpriteBase);
}

void StaticLayout::Draw(rct_drawpixelinfo * dpi, int32_t x, int32_t y)
{
    gCurrentFontFlags = 0;
    gCurrentFontSpriteBase = _paint.SpriteBase;

    TextPaint tempPaint = _paint;

    gCurrentFontFlags = 0;
    int32_t lineY = y;
    int32_t lineX = x;
    switch (_paint.Alignment) {
    case TextAlignment::LEFT:
        lineX = x;
        break;
    case TextAlignment::CENTRE:
        lineX = x + _maxWidth / 2;
        break;
    case TextAlignment::RIGHT:
        lineX = x + _maxWidth;
        break;
    }
    utf8 * buffer = _buffer;
    for (int32_t line = 0; line < _lineCount; ++line)
    {
        DrawText(dpi, lineX, lineY, &tempPaint, buffer);
        tempPaint.Colour = TEXT_COLOUR_254;
        buffer = get_string_end(buffer) + 1;
        lineY += _lineHeight;
    }
}

int32_t StaticLayout::GetHeight()
{
    return _lineHeight * _lineCount;
}

int32_t StaticLayout::GetWidth()
{
    return _maxWidth;
}

int32_t StaticLayout::GetLineCount()
{
    return _lineCount;
}

static void DrawText(rct_drawpixelinfo * dpi, int32_t x, int32_t y, TextPaint * paint, const_utf8string text)
{
    int32_t width = gfx_get_string_width(text);

    switch (paint->Alignment) {
    case TextAlignment::LEFT:
        break;
    case TextAlignment::CENTRE:
        x -= (width - 1) / 2;
        break;
    case TextAlignment::RIGHT:
        x -= width;
        break;
    }

    ttf_draw_string(dpi, text, paint->Colour, x, y);

    if (paint->UnderlineText)
    {
        gfx_fill_rect(dpi, x, y + 11, x + width, y + 11, text_palette[1]);
        if (text_palette[2] != 0)
        {
            gfx_fill_rect(dpi, x + 1, y + 12, x + width + 1, y + 12, text_palette[2]);
        }
    }
}

static void DrawText(rct_drawpixelinfo * dpi, int32_t x, int32_t y, TextPaint * paint, rct_string_id format, void * args)
{
    utf8 buffer[256];
    format_string(buffer, sizeof(buffer), format, args);
    DrawText(dpi, x, y, paint, buffer);
}

static void DrawTextCompat(rct_drawpixelinfo * dpi, int32_t x, int32_t y, rct_string_id format, void * args, uint8_t colour,
                           TextAlignment alignment, bool underline = false)
{
    _legacyPaint.UnderlineText = underline;
    _legacyPaint.Colour = colour;
    _legacyPaint.Alignment = alignment;
    _legacyPaint.SpriteBase = FONT_SPRITE_BASE_MEDIUM;
    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    DrawText(dpi, x, y, &_legacyPaint, format, args);
}

static void DrawTextEllipsisedCompat(rct_drawpixelinfo * dpi, int32_t x, int32_t y, int32_t width, rct_string_id format, void * args,
                                     uint8_t colour,
                                     TextAlignment alignment, bool underline = false)
{
    _legacyPaint.UnderlineText = underline;
    _legacyPaint.Colour = colour;
    _legacyPaint.Alignment = alignment;
    _legacyPaint.SpriteBase = FONT_SPRITE_BASE_MEDIUM;
    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

    utf8 buffer[256];
    format_string(buffer, sizeof(buffer), format, args);
    gfx_clip_string(buffer, width);

    DrawText(dpi, x, y, &_legacyPaint, buffer);
}

void gfx_draw_string(rct_drawpixelinfo *dpi, const_utf8string buffer, uint8_t colour, int32_t x, int32_t y)
{
    _legacyPaint.UnderlineText = false;
    _legacyPaint.Colour = colour;
    _legacyPaint.Alignment = TextAlignment::LEFT;
    _legacyPaint.SpriteBase = gCurrentFontSpriteBase;
    DrawText(dpi, x, y, &_legacyPaint, buffer);
}

// Basic
void gfx_draw_string_left(rct_drawpixelinfo * dpi, rct_string_id format, void * args, uint8_t colour, int32_t x, int32_t y)
{
    DrawTextCompat(dpi, x, y, format, args, colour, TextAlignment::LEFT);
}

void gfx_draw_string_centred(rct_drawpixelinfo * dpi, rct_string_id format, int32_t x, int32_t y, uint8_t colour, void * args)
{
    DrawTextCompat(dpi, x, y, format, args, colour, TextAlignment::CENTRE);
}

void gfx_draw_string_right(rct_drawpixelinfo * dpi, rct_string_id format, void * args, uint8_t colour, int32_t x, int32_t y)
{
    DrawTextCompat(dpi, x, y, format, args, colour, TextAlignment::RIGHT);
}

// Underline
void draw_string_left_underline(rct_drawpixelinfo * dpi, rct_string_id format, void * args, uint8_t colour, int32_t x, int32_t y)
{
    DrawTextCompat(dpi, x, y, format, args, colour, TextAlignment::LEFT, true);
}

void draw_string_centred_underline(rct_drawpixelinfo * dpi, rct_string_id format, void * args, uint8_t colour, int32_t x, int32_t y)
{
    DrawTextCompat(dpi, x, y, format, args, colour, TextAlignment::CENTRE, true);
}

void draw_string_right_underline(rct_drawpixelinfo * dpi, rct_string_id format, void * args, uint8_t colour, int32_t x, int32_t y)
{
    DrawTextCompat(dpi, x, y, format, args, colour, TextAlignment::RIGHT, true);
}

// Ellipsised
void gfx_draw_string_left_clipped(rct_drawpixelinfo * dpi, rct_string_id format, void * args, uint8_t colour, int32_t x, int32_t y, int32_t width)
{
    DrawTextEllipsisedCompat(dpi, x, y, width, format, args, colour, TextAlignment::LEFT);
}

void gfx_draw_string_centred_clipped(rct_drawpixelinfo * dpi, rct_string_id format, void * args, uint8_t colour, int32_t x, int32_t y, int32_t width)
{
    DrawTextEllipsisedCompat(dpi, x, y, width, format, args, colour, TextAlignment::CENTRE);
}

void gfx_draw_string_right_clipped(rct_drawpixelinfo * dpi, rct_string_id format, void * args, uint8_t colour, int32_t x, int32_t y, int32_t width)
{
    DrawTextEllipsisedCompat(dpi, x, y, width, format, args, colour, TextAlignment::RIGHT);
}

// Wrapping
int32_t gfx_draw_string_left_wrapped(rct_drawpixelinfo * dpi, void * args, int32_t x, int32_t y, int32_t width, rct_string_id format, uint8_t colour)
{
    utf8 buffer[256];
    format_string(buffer, sizeof(buffer), format, args);

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

    _legacyPaint.UnderlineText = false;
    _legacyPaint.Colour = colour;
    _legacyPaint.Alignment = TextAlignment::LEFT;
    _legacyPaint.SpriteBase = gCurrentFontSpriteBase;

    StaticLayout layout(buffer, _legacyPaint, width);
    layout.Draw(dpi, x, y);

    return layout.GetHeight();
}

int32_t gfx_draw_string_centred_wrapped(rct_drawpixelinfo * dpi, void * args, int32_t x, int32_t y, int32_t width, rct_string_id format, uint8_t colour)
{
    utf8 buffer[256];
    format_string(buffer, sizeof(buffer), format, args);

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

    _legacyPaint.UnderlineText = false;
    _legacyPaint.Colour = colour;
    _legacyPaint.Alignment = TextAlignment::CENTRE;
    _legacyPaint.SpriteBase = gCurrentFontSpriteBase;

    StaticLayout layout(buffer, _legacyPaint, width);

    // The original tried to vertically centre the text, but used line count - 1
    int32_t lineCount = layout.GetLineCount();
    int32_t lineHeight = layout.GetHeight() / lineCount;
    int32_t yOffset = (lineCount - 1) * lineHeight / 2;

    layout.Draw(dpi, x - layout.GetWidth() / 2, y - yOffset);

    return layout.GetHeight();
}

