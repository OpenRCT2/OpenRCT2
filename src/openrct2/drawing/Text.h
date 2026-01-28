/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../drawing/Colour.h"
#include "../interface/ColourWithFlags.h"
#include "../localisation/StringIdType.h"
#include "Font.h"

struct ScreenCoordsXY;

using OpenRCT2::ColourWithFlags;

namespace OpenRCT2
{
    class Formatter;
}

namespace OpenRCT2::Drawing
{
    struct RenderTarget;
}

enum class TextAlignment
{
    left,
    centre,
    right,
};

enum class TextUnderline
{
    off,
    on,
};

enum class TextDarkness : uint8_t
{
    regular = 0,
    dark = 1,
    extraDark = 2,
};

struct TextPaint
{
    ColourWithFlags Colour = { OpenRCT2::Drawing::Colour::black };
    ::FontStyle FontStyle = FontStyle::medium;
    TextUnderline UnderlineText = TextUnderline::off;
    TextAlignment Alignment = TextAlignment::left;
    TextDarkness Darkness = TextDarkness::regular;

    TextPaint() = default;
    TextPaint(ColourWithFlags colour)
        : Colour(colour)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour colour)
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
    TextPaint(OpenRCT2::Drawing::Colour colour, ::FontStyle fontStyle)
        : Colour(ColourWithFlags{ colour })
        , FontStyle(fontStyle)
    {
    }
    TextPaint(ColourWithFlags colour, TextUnderline underlineText)
        : Colour(colour)
        , UnderlineText(underlineText)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour colour, TextUnderline underlineText)
        : Colour(ColourWithFlags{ colour })
        , UnderlineText(underlineText)
    {
    }
    TextPaint(ColourWithFlags colour, TextAlignment alignment)
        : Colour(colour)
        , Alignment(alignment)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour colour, TextAlignment alignment)
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
    TextPaint(OpenRCT2::Drawing::Colour colour, ::FontStyle fontStyle, TextUnderline underlineText)
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
    TextPaint(OpenRCT2::Drawing::Colour colour, ::FontStyle fontStyle, TextAlignment alignment)
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
    TextPaint(OpenRCT2::Drawing::Colour colour, ::FontStyle fontStyle, TextDarkness darkness)
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
    TextPaint(OpenRCT2::Drawing::Colour colour, TextUnderline underlineText, TextAlignment alignment)
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
    TextPaint(OpenRCT2::Drawing::Colour colour, ::FontStyle fontStyle, TextUnderline underlineText, TextAlignment alignment)
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
    TextPaint(OpenRCT2::Drawing::Colour colour, ::FontStyle fontStyle, TextAlignment alignment, TextDarkness darkness)
        : Colour(ColourWithFlags{ colour })
        , FontStyle(fontStyle)
        , Alignment(alignment)
        , Darkness(darkness)
    {
    }
};

void DrawTextBasic(OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, StringId format);
void DrawTextEllipsised(OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format);
int32_t DrawTextWrapped(OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format);

void DrawText(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, const TextPaint& paint, const_utf8string text,
    bool noFormatting = false);
void DrawTextBasic(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, StringId format, const OpenRCT2::Formatter& ft,
    TextPaint textPaint = {});
void DrawTextEllipsised(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format,
    const OpenRCT2::Formatter& ft, TextPaint textPaint = {});
int32_t DrawTextWrapped(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format,
    const OpenRCT2::Formatter& ft, TextPaint textPaint = {});
