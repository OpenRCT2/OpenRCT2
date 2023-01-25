/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
struct DrawPixelInfo;
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

enum class TextDarkness
{
    Regular = 0,
    Dark = 1,
    ExtraDark = 2,
};

struct TextPaint
{
    colour_t Colour = COLOUR_BLACK;
    ::FontStyle FontStyle = FontStyle::Medium;
    TextUnderline UnderlineText = TextUnderline::Off;
    TextAlignment Alignment = TextAlignment::LEFT;
    TextDarkness Darkness = TextDarkness::Regular;

    TextPaint() = default;
    TextPaint(colour_t colour)
        : Colour(colour)
    {
    }
    TextPaint(::FontStyle fontStyle)
        : FontStyle(fontStyle)
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

    TextPaint(colour_t colour, ::FontStyle fontStyle)
        : Colour(colour)
        , FontStyle(fontStyle)
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

    TextPaint(::FontStyle fontStyle, TextUnderline underlineText)
        : FontStyle(fontStyle)
        , UnderlineText(underlineText)
    {
    }
    TextPaint(::FontStyle fontStyle, TextAlignment alignment)
        : FontStyle(fontStyle)
        , Alignment(alignment)
    {
    }
    TextPaint(TextUnderline underlineText, TextAlignment alignment)
        : UnderlineText(underlineText)
        , Alignment(alignment)
    {
    }

    TextPaint(colour_t colour, ::FontStyle fontStyle, TextUnderline underlineText)
        : Colour(colour)
        , FontStyle(fontStyle)
        , UnderlineText(underlineText)
    {
    }
    TextPaint(colour_t colour, ::FontStyle fontStyle, TextAlignment alignment)
        : Colour(colour)
        , FontStyle(fontStyle)
        , Alignment(alignment)
    {
    }
    TextPaint(colour_t colour, ::FontStyle fontStyle, TextDarkness darkness)
        : Colour(colour)
        , FontStyle(fontStyle)
        , Darkness(darkness)
    {
    }
    TextPaint(colour_t colour, TextUnderline underlineText, TextAlignment alignment)
        : Colour(colour)
        , UnderlineText(underlineText)
        , Alignment(alignment)
    {
    }
    TextPaint(::FontStyle fontStyle, TextUnderline underlineText, TextAlignment alignment)
        : FontStyle(fontStyle)
        , UnderlineText(underlineText)
        , Alignment(alignment)
    {
    }

    TextPaint(colour_t colour, ::FontStyle fontStyle, TextUnderline underlineText, TextAlignment alignment)
        : Colour(colour)
        , FontStyle(fontStyle)
        , UnderlineText(underlineText)
        , Alignment(alignment)
    {
    }
    TextPaint(colour_t colour, ::FontStyle fontStyle, TextAlignment alignment, TextDarkness darkness)
        : Colour(colour)
        , FontStyle(fontStyle)
        , Alignment(alignment)
        , Darkness(darkness)
    {
    }
};

void DrawTextBasic(DrawPixelInfo* dpi, const ScreenCoordsXY& coords, StringId format);
void DrawTextEllipsised(DrawPixelInfo* dpi, const ScreenCoordsXY& coords, int32_t width, StringId format);
int32_t DrawTextWrapped(DrawPixelInfo* dpi, const ScreenCoordsXY& coords, int32_t width, StringId format);

void DrawTextBasic(
    DrawPixelInfo* dpi, const ScreenCoordsXY& coords, StringId format, const Formatter& ft, TextPaint textPaint = {});
void DrawTextEllipsised(
    DrawPixelInfo* dpi, const ScreenCoordsXY& coords, int32_t width, StringId format, const Formatter& ft,
    TextPaint textPaint = {});
int32_t DrawTextWrapped(
    DrawPixelInfo* dpi, const ScreenCoordsXY& coords, int32_t width, StringId format, const Formatter& ft,
    TextPaint textPaint = {});
