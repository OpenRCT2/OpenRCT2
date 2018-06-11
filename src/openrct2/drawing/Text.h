/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
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
    uint8           Colour = 0;
    sint16          SpriteBase = 0;
    bool            UnderlineText = false;
    TextAlignment   Alignment = TextAlignment::LEFT;
};

class StaticLayout
{
private:
    utf8string  _buffer;
    TextPaint   _paint;
    sint32      _lineCount = 0;
    sint32      _lineHeight;
    sint32      _maxWidth;

    StaticLayout();
    StaticLayout(const StaticLayout &);

public:
    StaticLayout(utf8string source, TextPaint paint, sint32 width);
    void Draw(rct_drawpixelinfo * dpi, sint32 x, sint32 y);
    sint32 GetHeight();
    sint32 GetWidth();
    sint32 GetLineCount();
};
