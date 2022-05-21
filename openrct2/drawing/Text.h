/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../interface/Colour.h"
#include "Font.h"

struct ScreenCoordsXY;
struct rct_drawpixelinfo;
class Formatter;

enum class TextAlignment
{
    LEFT,
    CENTRE,
    RIGHT
};

enum class TextUnderline
{
    Off,
    On,
};

struct TextPaint
{
    colour_t Colour = COLOUR_BLACK;
    FontSpriteBase SpriteBase = FontSpriteBase::MEDIUM;
    TextUnderline UnderlineText = TextUnderline::Off;
    TextAlignment Alignment = TextAlignment::LEFT;

    TextPaint() = default;
    TextPaint(colour_t colour)
        : Colour(colour)
    {
    }
    TextPaint(FontSpriteBase spriteBase)
        : SpriteBase(spriteBase)
    {
    }
    TextPaint(TextUnderline underlineText)
        : UnderlineText(underlineText)
    {
    }
    TextPaint(TextAlignment alignment)
        : Alignment(alignment)
    {
    }

    TextPaint(colour_t colour, FontSpriteBase spriteBase)
        : Colour(colour)
        , SpriteBase(spriteBase)
    {
    }
    TextPaint(colour_t colour, TextUnderline underlineText)
        : Colour(colour)
        , UnderlineText(underlineText)
    {
    }
    TextPaint(colour_t colour, TextAlignment alignment)
        : Colour(colour)
        , Alignment(alignment)
    {
    }

    TextPaint(FontSpriteBase spriteBase, TextUnderline underlineText)
        : SpriteBase(spriteBase)
        , UnderlineText(underlineText)
    {
    }
    TextPaint(FontSpriteBase spriteBase, TextAlignment alignment)
        : SpriteBase(spriteBase)
        , Alignment(alignment)
    {
    }
    TextPaint(TextUnderline underlineText, TextAlignment alignment)
        : UnderlineText(underlineText)
        , Alignment(alignment)
    {
    }

    TextPaint(colour_t colour, FontSpriteBase spriteBase, TextUnderline underlineText)
        : Colour(colour)
        , SpriteBase(spriteBase)
        , UnderlineText(underlineText)
    {
    }
    TextPaint(colour_t colour, FontSpriteBase spriteBase, TextAlignment alignment)
        : Colour(colour)
        , SpriteBase(spriteBase)
        , Alignment(alignment)
    {
    }
    TextPaint(colour_t colour, TextUnderline underlineText, TextAlignment alignment)
        : Colour(colour)
        , UnderlineText(underlineText)
        , Alignment(alignment)
    {
    }
    TextPaint(FontSpriteBase spriteBase, TextUnderline underlineText, TextAlignment alignment)
        : SpriteBase(spriteBase)
        , UnderlineText(underlineText)
        , Alignment(alignment)
    {
    }

    TextPaint(colour_t colour, FontSpriteBase spriteBase, TextUnderline underlineText, TextAlignment alignment)
        : Colour(colour)
        , SpriteBase(spriteBase)
        , UnderlineText(underlineText)
        , Alignment(alignment)
    {
    }
};

void DrawTextBasic(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, rct_string_id format);
void DrawTextEllipsised(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, rct_string_id format);
int32_t DrawTextWrapped(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, rct_string_id format);

void DrawTextBasic(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, rct_string_id format, const Formatter& ft, TextPaint textPaint = {});
void DrawTextEllipsised(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, rct_string_id format, const Formatter& ft,
    TextPaint textPaint = {});
int32_t DrawTextWrapped(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, rct_string_id format, const Formatter& ft,
    TextPaint textPaint = {});
