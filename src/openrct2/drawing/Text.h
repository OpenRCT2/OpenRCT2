/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../interface/Colour.h"
#include "../localisation/StringIdType.h"
#include "Font.h"

struct ScreenCoordsXY;
struct RenderTarget;

namespace OpenRCT2
{
    class Formatter;
}

enum class TextAlignment
{
    LEFT,
    CENTRE,
    RIGHT
};

enum class TextUnderline
{
    off,
    on,
};

enum class TextDarkness
{
    regular = 0,
    dark = 1,
    extraDark = 2,
};

struct TextPaint
{
    ColourWithFlags Colour = { COLOUR_BLACK };
    ::FontStyle FontStyle = FontStyle::Medium;
    TextUnderline UnderlineText = TextUnderline::off;
    TextAlignment Alignment = TextAlignment::LEFT;
    TextDarkness Darkness = TextDarkness::regular;

    TextPaint() = default;
    TextPaint(ColourWithFlags colour)
        : Colour(colour)
    {
    }
    TextPaint(colour_t colour)
        : Colour(ColourWithFlags{ colour })
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

    TextPaint(ColourWithFlags colour, ::FontStyle fontStyle)
        : Colour(colour)
        , FontStyle(fontStyle)
    {
    }
    TextPaint(colour_t colour, ::FontStyle fontStyle)
        : Colour(ColourWithFlags{ colour })
        , FontStyle(fontStyle)
    {
    }
    TextPaint(ColourWithFlags colour, TextUnderline underlineText)
        : Colour(colour)
        , UnderlineText(underlineText)
    {
    }
    TextPaint(colour_t colour, TextUnderline underlineText)
        : Colour(ColourWithFlags{ colour })
        , UnderlineText(underlineText)
    {
    }
    TextPaint(ColourWithFlags colour, TextAlignment alignment)
        : Colour(colour)
        , Alignment(alignment)
    {
    }
    TextPaint(colour_t colour, TextAlignment alignment)
        : Colour(ColourWithFlags{ colour })
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

    TextPaint(ColourWithFlags colour, ::FontStyle fontStyle, TextUnderline underlineText)
        : Colour(colour)
        , FontStyle(fontStyle)
        , UnderlineText(underlineText)
    {
    }
    TextPaint(colour_t colour, ::FontStyle fontStyle, TextUnderline underlineText)
        : Colour(ColourWithFlags{ colour })
        , FontStyle(fontStyle)
        , UnderlineText(underlineText)
    {
    }
    TextPaint(ColourWithFlags colour, ::FontStyle fontStyle, TextAlignment alignment)
        : Colour(colour)
        , FontStyle(fontStyle)
        , Alignment(alignment)
    {
    }
    TextPaint(colour_t colour, ::FontStyle fontStyle, TextAlignment alignment)
        : Colour(ColourWithFlags{ colour })
        , FontStyle(fontStyle)
        , Alignment(alignment)
    {
    }
    TextPaint(ColourWithFlags colour, ::FontStyle fontStyle, TextDarkness darkness)
        : Colour(colour)
        , FontStyle(fontStyle)
        , Darkness(darkness)
    {
    }
    TextPaint(colour_t colour, ::FontStyle fontStyle, TextDarkness darkness)
        : Colour(ColourWithFlags{ colour })
        , FontStyle(fontStyle)
        , Darkness(darkness)
    {
    }
    TextPaint(ColourWithFlags colour, TextUnderline underlineText, TextAlignment alignment)
        : Colour(colour)
        , UnderlineText(underlineText)
        , Alignment(alignment)
    {
    }
    TextPaint(colour_t colour, TextUnderline underlineText, TextAlignment alignment)
        : Colour(ColourWithFlags{ colour })
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

    TextPaint(ColourWithFlags colour, ::FontStyle fontStyle, TextUnderline underlineText, TextAlignment alignment)
        : Colour(colour)
        , FontStyle(fontStyle)
        , UnderlineText(underlineText)
        , Alignment(alignment)
    {
    }
    TextPaint(colour_t colour, ::FontStyle fontStyle, TextUnderline underlineText, TextAlignment alignment)
        : Colour(ColourWithFlags{ colour })
        , FontStyle(fontStyle)
        , UnderlineText(underlineText)
        , Alignment(alignment)
    {
    }
    TextPaint(ColourWithFlags colour, ::FontStyle fontStyle, TextAlignment alignment, TextDarkness darkness)
        : Colour(colour)
        , FontStyle(fontStyle)
        , Alignment(alignment)
        , Darkness(darkness)
    {
    }
    TextPaint(colour_t colour, ::FontStyle fontStyle, TextAlignment alignment, TextDarkness darkness)
        : Colour(ColourWithFlags{ colour })
        , FontStyle(fontStyle)
        , Alignment(alignment)
        , Darkness(darkness)
    {
    }
};

void DrawTextBasic(RenderTarget& rt, const ScreenCoordsXY& coords, StringId format);
void DrawTextEllipsised(RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format);
int32_t DrawTextWrapped(RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format);

void DrawText(
    RenderTarget& rt, const ScreenCoordsXY& coords, const TextPaint& paint, const_utf8string text, bool noFormatting = false);
void DrawTextBasic(
    RenderTarget& rt, const ScreenCoordsXY& coords, StringId format, const OpenRCT2::Formatter& ft, TextPaint textPaint = {});
void DrawTextEllipsised(
    RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format, const OpenRCT2::Formatter& ft,
    TextPaint textPaint = {});
int32_t DrawTextWrapped(
    RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format, const OpenRCT2::Formatter& ft,
    TextPaint textPaint = {});
