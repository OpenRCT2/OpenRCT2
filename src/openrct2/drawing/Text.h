/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

struct rct_drawpixelinfo;

enum class TextAlignment
{
    LEFT,
    CENTRE,
    RIGHT
};

struct TextPaint
{
    uint8_t           Colour = 0;
    int16_t          SpriteBase = 0;
    bool            UnderlineText = false;
    TextAlignment   Alignment = TextAlignment::LEFT;
};

class StaticLayout
{
private:
    utf8string  _buffer;
    TextPaint   _paint;
    int32_t      _lineCount = 0;
    int32_t      _lineHeight;
    int32_t      _maxWidth;

    StaticLayout();
    StaticLayout(const StaticLayout &);

public:
    StaticLayout(utf8string source, TextPaint paint, int32_t width);
    void Draw(rct_drawpixelinfo * dpi, int32_t x, int32_t y);
    int32_t GetHeight();
    int32_t GetWidth();
    int32_t GetLineCount();
};
