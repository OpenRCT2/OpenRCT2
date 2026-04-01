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

enum class TextPaintFlag
{
    underline,
    noFormatting,
};
using TextPaintFlags = FlagHolder<uint8_t, TextPaintFlag>;

enum class TextDarkness : uint8_t
{
    regular = 0,
    dark = 1,
    extraDark = 2,
};

struct TextPaint
{
    ColourWithFlags colour = { OpenRCT2::Drawing::Colour::black };
    ::FontStyle fontStyle = FontStyle::medium;
    TextPaintFlags flags{};
    TextAlignment alignment = TextAlignment::left;
    TextDarkness darkness = TextDarkness::regular;

    TextPaint() = default;
    TextPaint(ColourWithFlags _colour)
        : colour(_colour)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour _colour)
        : colour(ColourWithFlags{ _colour })
    {
    }
    TextPaint(::FontStyle _fontStyle)
        : fontStyle(_fontStyle)
    {
    }
    TextPaint(TextPaintFlags _flags)
        : flags(_flags)
    {
    }
    TextPaint(TextAlignment _alignment)
        : alignment(_alignment)
    {
    }

    TextPaint(ColourWithFlags _colour, ::FontStyle _fontStyle)
        : colour(_colour)
        , fontStyle(_fontStyle)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour _colour, ::FontStyle _fontStyle)
        : colour(ColourWithFlags{ _colour })
        , fontStyle(_fontStyle)
    {
    }
    TextPaint(ColourWithFlags _colour, TextPaintFlags _flags)
        : colour(_colour)
        , flags(_flags)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour _colour, TextPaintFlags _flags)
        : colour(ColourWithFlags{ _colour })
        , flags(_flags)
    {
    }
    TextPaint(ColourWithFlags _colour, TextAlignment _alignment)
        : colour(_colour)
        , alignment(_alignment)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour _colour, TextAlignment _alignment)
        : colour(ColourWithFlags{ _colour })
        , alignment(_alignment)
    {
    }

    TextPaint(::FontStyle _fontStyle, TextPaintFlags _flags)
        : fontStyle(_fontStyle)
        , flags(_flags)
    {
    }
    TextPaint(::FontStyle _fontStyle, TextAlignment _alignment)
        : fontStyle(_fontStyle)
        , alignment(_alignment)
    {
    }
    TextPaint(TextPaintFlags _flags, TextAlignment _alignment)
        : flags(_flags)
        , alignment(_alignment)
    {
    }

    TextPaint(ColourWithFlags _colour, ::FontStyle _fontStyle, TextPaintFlags _flags)
        : colour(_colour)
        , fontStyle(_fontStyle)
        , flags(_flags)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour _colour, ::FontStyle _fontStyle, TextPaintFlags _flags)
        : colour(ColourWithFlags{ _colour })
        , fontStyle(_fontStyle)
        , flags(_flags)
    {
    }
    TextPaint(ColourWithFlags _colour, ::FontStyle _fontStyle, TextAlignment _alignment)
        : colour(_colour)
        , fontStyle(_fontStyle)
        , alignment(_alignment)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour _colour, ::FontStyle _fontStyle, TextAlignment _alignment)
        : colour(ColourWithFlags{ _colour })
        , fontStyle(_fontStyle)
        , alignment(_alignment)
    {
    }
    TextPaint(ColourWithFlags _colour, ::FontStyle _fontStyle, TextDarkness _darkness)
        : colour(_colour)
        , fontStyle(_fontStyle)
        , darkness(_darkness)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour _colour, ::FontStyle _fontStyle, TextDarkness _darkness)
        : colour(ColourWithFlags{ _colour })
        , fontStyle(_fontStyle)
        , darkness(_darkness)
    {
    }
    TextPaint(ColourWithFlags _colour, TextPaintFlags _flags, TextAlignment _alignment)
        : colour(_colour)
        , flags(_flags)
        , alignment(_alignment)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour _colour, TextPaintFlags _flags, TextAlignment _alignment)
        : colour(ColourWithFlags{ _colour })
        , flags(_flags)
        , alignment(_alignment)
    {
    }
    TextPaint(::FontStyle _fontStyle, TextPaintFlags _flags, TextAlignment _alignment)
        : fontStyle(_fontStyle)
        , flags(_flags)
        , alignment(_alignment)
    {
    }

    TextPaint(ColourWithFlags _colour, ::FontStyle _fontStyle, TextPaintFlags _flags, TextAlignment _alignment)
        : colour(_colour)
        , fontStyle(_fontStyle)
        , flags(_flags)
        , alignment(_alignment)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour _colour, ::FontStyle _fontStyle, TextPaintFlags _flags, TextAlignment _alignment)
        : colour(ColourWithFlags{ _colour })
        , fontStyle(_fontStyle)
        , flags(_flags)
        , alignment(_alignment)
    {
    }
    TextPaint(ColourWithFlags _colour, ::FontStyle _fontStyle, TextAlignment _alignment, TextDarkness _darkness)
        : colour(_colour)
        , fontStyle(_fontStyle)
        , alignment(_alignment)
        , darkness(_darkness)
    {
    }
    TextPaint(OpenRCT2::Drawing::Colour _colour, ::FontStyle _fontStyle, TextAlignment _alignment, TextDarkness _darkness)
        : colour(ColourWithFlags{ _colour })
        , fontStyle(_fontStyle)
        , alignment(_alignment)
        , darkness(_darkness)
    {
    }
};

void drawText(OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, StringId format, TextPaint textPaint = {});
void drawText(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, StringId format, const OpenRCT2::Formatter& ft,
    TextPaint textPaint = {});
void drawText(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, u8string_view string, TextPaint textPaint = {});

void drawTextEllipsised(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format,
    TextPaint textPaint = {});
void drawTextEllipsised(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format,
    const OpenRCT2::Formatter& ft, TextPaint textPaint = {});
void drawTextEllipsised(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, u8string_view string,
    TextPaint textPaint = {});

int32_t drawTextWrapped(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format,
    TextPaint textPaint = {});
int32_t drawTextWrapped(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, StringId format,
    const OpenRCT2::Formatter& ft, TextPaint textPaint = {});
int32_t drawTextWrapped(
    OpenRCT2::Drawing::RenderTarget& rt, const ScreenCoordsXY& coords, int32_t width, u8string_view string,
    TextPaint textPaint = {});
