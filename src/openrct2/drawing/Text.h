#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

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
