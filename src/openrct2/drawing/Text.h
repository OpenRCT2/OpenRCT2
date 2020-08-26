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

struct ScreenCoordsXY;
struct rct_drawpixelinfo;
class Formatter;

enum class TextAlignment
{
    LEFT,
    CENTRE,
    RIGHT
};

struct TextPaint
{
    uint8_t Colour = 0;
    int16_t SpriteBase = 0;
    bool UnderlineText = false;
    TextAlignment Alignment = TextAlignment::LEFT;
};

class StaticLayout
{
private:
    utf8string Buffer;
    TextPaint Paint;
    int32_t LineCount = 0;
    int32_t LineHeight;
    int32_t MaxWidth;

    StaticLayout();
    StaticLayout(const StaticLayout&);

public:
    StaticLayout(utf8string source, const TextPaint& paint, int32_t width);
    void Draw(rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords);
    int32_t GetHeight();
    int32_t GetWidth();
    int32_t GetLineCount();
};

void DrawTextBasic(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, rct_string_id format, const Formatter& ft, colour_t colour,
    TextAlignment alignment, bool underline = false);
void DrawTextBasic(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, rct_string_id format, const void* args, colour_t colour,
    TextAlignment alignment, bool underline = false);
void DrawTextEllipsised(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, rct_string_id format, const Formatter& ft,
    colour_t colour, TextAlignment alignment, bool underline = false);
void DrawTextEllipsised(
    rct_drawpixelinfo* dpi, const ScreenCoordsXY& coords, int32_t width, rct_string_id format, const void* args,
    colour_t colour, TextAlignment alignment, bool underline = false);
