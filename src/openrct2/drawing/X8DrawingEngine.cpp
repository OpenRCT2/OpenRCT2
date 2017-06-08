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

#include "../config/Config.h"
#include "../Context.h"
#include "../ui/UiContext.h"
#include "../core/Guard.hpp"
#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "IDrawingContext.h"
#include "IDrawingEngine.h"
#include "Rain.h"
#include "X8DrawingEngine.h"

extern "C"
{
    #include "../game.h"
    #include "../interface/screenshot.h"
    #include "../interface/viewport.h"
    #include "../interface/window.h"
    #include "../intro.h"
    #include "../platform/platform.h"
    #include "../rct2.h"
    #include "drawing.h"
}

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

X8RainDrawer::X8RainDrawer()
{
    _rainPixels = new RainPixel[_rainPixelsCapacity];
}

X8RainDrawer::~X8RainDrawer()
{
    delete [] _rainPixels;
}

void X8RainDrawer::SetDPI(rct_drawpixelinfo * dpi)
{
    _screenDPI = dpi;
}

void X8RainDrawer::Draw(sint32 x, sint32 y, sint32 width, sint32 height, sint32 xStart, sint32 yStart)
{
    static const uint8 RainPattern[] =
    {
        32, 32, 0, 12, 0, 14, 0, 16, 255, 0, 255, 0, 255, 0, 255, 0, 255,
        0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
        255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255,
        0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 0, 0
    };

    const uint8 * pattern = RainPattern;
    uint8 patternXSpace = *pattern++;
    uint8 patternYSpace = *pattern++;

    uint8 patternStartXOffset = xStart % patternXSpace;
    uint8 patternStartYOffset = yStart % patternYSpace;

    uint32 pixelOffset = (_screenDPI->pitch + _screenDPI->width) * y + x;
    uint8 patternYPos = patternStartYOffset % patternYSpace;

    uint8 * screenBits = _screenDPI->bits;

    //Stores the colours of changed pixels
    RainPixel * newPixels = &_rainPixels[_rainPixelsCount];
    for (; height != 0; height--)
    {
        uint8 patternX = pattern[patternYPos * 2];
        if (patternX != 0xFF)
        {
            if (_rainPixelsCount < (_rainPixelsCapacity - (uint32)width))
            {
                uint32 finalPixelOffset = width + pixelOffset;

                uint32 xPixelOffset = pixelOffset;
                xPixelOffset += ((uint8)(patternX - patternStartXOffset)) % patternXSpace;

                uint8 patternPixel = pattern[patternYPos * 2 + 1];
                for (; xPixelOffset < finalPixelOffset; xPixelOffset += patternXSpace)
                {
                    uint8 current_pixel = screenBits[xPixelOffset];
                    screenBits[xPixelOffset] = patternPixel;
                    _rainPixelsCount++;

                    // Store colour and position
                    *newPixels++ = { xPixelOffset, current_pixel };
                }
            }
        }

        pixelOffset += _screenDPI->pitch + _screenDPI->width;
        patternYPos++;
        patternYPos %= patternYSpace;
    }
}

void X8RainDrawer::Restore()
{
    if (_rainPixelsCount > 0)
    {
        uint32  numPixels = (_screenDPI->width + _screenDPI->pitch) * _screenDPI->height;
        uint8 * bits = _screenDPI->bits;
        for (uint32 i = 0; i < _rainPixelsCount; i++)
        {
            RainPixel rainPixel = _rainPixels[i];
            if (rainPixel.Position >= numPixels)
            {
                // Pixel out of bounds, bail
                break;
            }

            bits[rainPixel.Position] = rainPixel.Colour;
        }
        _rainPixelsCount = 0;
    }
}

X8DrawingEngine::X8DrawingEngine()
{
    _drawingContext = new X8DrawingContext(this);
}

X8DrawingEngine::~X8DrawingEngine()
{
    delete _drawingContext;
    delete [] _dirtyGrid.Blocks;
    delete [] _bits;
}

void X8DrawingEngine::Initialise(SDL_Window * window)
{
}

void X8DrawingEngine::Resize(uint32 width, uint32 height)
{
    uint32 pitch = width;
    ConfigureBits(width, height, pitch);
}

void X8DrawingEngine::SetPalette(const rct_palette_entry * palette)
{
}

void X8DrawingEngine::SetUncappedFrameRate(bool uncapped)
{
    // Not applicable for this engine
}

void X8DrawingEngine::Invalidate(sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    left = Math::Max(left, 0);
    top = Math::Max(top, 0);
    right = Math::Min(right, (sint32)_width);
    bottom = Math::Min(bottom, (sint32)_height);

    if (left >= right) return;
    if (top >= bottom) return;

    right--;
    bottom--;

    left >>= _dirtyGrid.BlockShiftX;
    right >>= _dirtyGrid.BlockShiftX;
    top >>= _dirtyGrid.BlockShiftY;
    bottom >>= _dirtyGrid.BlockShiftY;

    uint32 dirtyBlockColumns = _dirtyGrid.BlockColumns;
    uint8 * screenDirtyBlocks = _dirtyGrid.Blocks;
    for (sint16 y = top; y <= bottom; y++)
    {
        uint32 yOffset = y * dirtyBlockColumns;
        for (sint16 x = left; x <= right; x++)
        {
            screenDirtyBlocks[yOffset + x] = 0xFF;
        }
    }
}

void X8DrawingEngine::Draw()
{
    if (gIntroState != INTRO_STATE_NONE)
    {
        intro_draw(&_bitsDPI);
    }
    else
    {
        _rainDrawer.SetDPI(&_bitsDPI);
        _rainDrawer.Restore();

        ResetWindowVisbilities();

        // Redraw dirty regions before updating the viewports, otherwise
        // when viewports get panned, they copy dirty pixels
        DrawAllDirtyBlocks();

        window_update_all_viewports();
        DrawAllDirtyBlocks();
        window_update_all();

        gfx_draw_pickedup_peep(&_bitsDPI);
        gfx_invalidate_pickedup_peep();

        DrawRain(&_bitsDPI, &_rainDrawer);

        rct2_draw(&_bitsDPI);
    }
}

void X8DrawingEngine::CopyRect(sint32 x, sint32 y, sint32 width, sint32 height, sint32 dx, sint32 dy)
{
    if (dx == 0 && dy == 0) return;

    // Originally 0x00683359
    // Adjust for move off screen
    // NOTE: when zooming, there can be x, y, dx, dy combinations that go off the
    // screen; hence the checks. This code should ultimately not be called when
    // zooming because this function is specific to updating the screen on move
    sint32 lmargin = Math::Min(x - dx, 0);
    sint32 rmargin = Math::Min((sint32)_width - (x - dx + width), 0);
    sint32 tmargin = Math::Min(y - dy, 0);
    sint32 bmargin = Math::Min((sint32)_height - (y - dy + height), 0);
    x -= lmargin;
    y -= tmargin;
    width += lmargin + rmargin;
    height += tmargin + bmargin;

    sint32  stride = _bitsDPI.width + _bitsDPI.pitch;
    uint8 * to = _bitsDPI.bits + y * stride + x;
    uint8 * from = _bitsDPI.bits + (y - dy) * stride + x - dx;

    if (dy > 0)
    {
        // If positive dy, reverse directions
        to += (height - 1) * stride;
        from += (height - 1) * stride;
        stride = -stride;
    }

    // Move bytes
    for (sint32 i = 0; i < height; i++)
    {
        memmove(to, from, width);
        to += stride;
        from += stride;
    }
}

sint32 X8DrawingEngine::Screenshot()
{
    return screenshot_dump_png(&_bitsDPI);
}

IDrawingContext * X8DrawingEngine::GetDrawingContext(rct_drawpixelinfo * dpi)
{
    _drawingContext->SetDPI(dpi);
    return _drawingContext;
}

rct_drawpixelinfo * X8DrawingEngine::GetDrawingPixelInfo()
{
    return &_bitsDPI;
}

DRAWING_ENGINE_FLAGS X8DrawingEngine::GetFlags()
{
    return DEF_DIRTY_OPTIMISATIONS;
}

void X8DrawingEngine::InvalidateImage(uint32 image)
{
    // Not applicable for this engine
}

rct_drawpixelinfo * X8DrawingEngine::GetDPI()
{
    return &_bitsDPI;
}

void X8DrawingEngine::ConfigureBits(uint32 width, uint32 height, uint32 pitch)
{
    size_t  newBitsSize = pitch * height;
    uint8 * newBits = new uint8[newBitsSize];
    if (_bits == nullptr)
    {
        Memory::Set(newBits, 0, newBitsSize);
    }
    else
    {
        if (_pitch == pitch)
        {
            Memory::Copy(newBits, _bits, Math::Min(_bitsSize, newBitsSize));
        }
        else
        {
            uint8 * src = _bits;
            uint8 * dst = newBits;

            uint32 minWidth = Math::Min(_width, width);
            uint32 minHeight = Math::Min(_height, height);
            for (uint32 y = 0; y < minHeight; y++)
            {
                Memory::Copy(dst, src, minWidth);
                if (pitch - minWidth > 0)
                {
                    Memory::Set(dst + minWidth, 0, pitch - minWidth);
                }
                src += _pitch;
                dst += pitch;
            }
        }
        delete [] _bits;
    }

    _bits = newBits;
    _bitsSize = newBitsSize;
    _width = width;
    _height = height;
    _pitch = pitch;

    rct_drawpixelinfo * dpi = &_bitsDPI;
    dpi->bits = _bits;
    dpi->x = 0;
    dpi->y = 0;
    dpi->width = width;
    dpi->height = height;
    dpi->pitch = _pitch - width;

    ConfigureDirtyGrid();
}

void X8DrawingEngine::ConfigureDirtyGrid()
{
    _dirtyGrid.BlockShiftX = 7;
    _dirtyGrid.BlockShiftY = 6;
    _dirtyGrid.BlockWidth = 1 << _dirtyGrid.BlockShiftX;
    _dirtyGrid.BlockHeight = 1 << _dirtyGrid.BlockShiftY;
    _dirtyGrid.BlockColumns = (_width >> _dirtyGrid.BlockShiftX) + 1;
    _dirtyGrid.BlockRows = (_height >> _dirtyGrid.BlockShiftY) + 1;

    delete [] _dirtyGrid.Blocks;
    _dirtyGrid.Blocks = new uint8[_dirtyGrid.BlockColumns * _dirtyGrid.BlockRows];
}

void X8DrawingEngine::ResetWindowVisbilities()
{
    // reset window visibility status to unknown
    for (rct_window *w = g_window_list; w < gWindowNextSlot; w++)
    {
        w->visibility = VC_UNKNOWN;
        if (w->viewport != NULL) w->viewport->visibility = VC_UNKNOWN;
    }
}

void X8DrawingEngine::DrawAllDirtyBlocks()
{
    uint32  dirtyBlockColumns = _dirtyGrid.BlockColumns;
    uint32  dirtyBlockRows = _dirtyGrid.BlockRows;
    uint8 * dirtyBlocks = _dirtyGrid.Blocks;

    for (uint32 x = 0; x < dirtyBlockColumns; x++)
    {
        for (uint32 y = 0; y < dirtyBlockRows; y++)
        {
            uint32 yOffset = y * dirtyBlockColumns;
            if (dirtyBlocks[yOffset + x] == 0)
            {
                continue;
            }

            // Determine columns
            uint32 xx;
            for (xx = x; xx < dirtyBlockColumns; xx++)
            {
                if (dirtyBlocks[yOffset + xx] == 0)
                {
                    break;
                }
            }
            uint32 columns = xx - x;

            // Check rows
            uint32 yy;
            for (yy = y; yy < dirtyBlockRows; yy++)
            {
                uint32 yyOffset = yy * dirtyBlockColumns;
                for (xx = x; xx < x + columns; xx++)
                {
                    if (dirtyBlocks[yyOffset + xx] == 0)
                    {
                        goto endRowCheck;
                    }
                }
            }

        endRowCheck:
            uint32 rows = yy - y;
            DrawDirtyBlocks(x, y, columns, rows);
        }
    }
}

void X8DrawingEngine::DrawDirtyBlocks(uint32 x, uint32 y, uint32 columns, uint32 rows)
{
    uint32  dirtyBlockColumns = _dirtyGrid.BlockColumns;
    uint8 * screenDirtyBlocks = _dirtyGrid.Blocks;

    // Unset dirty blocks
    for (uint32 top = y; top < y + (uint32)rows; top++)
    {
        uint32 topOffset = top * dirtyBlockColumns;
        for (uint32 left = x; left < x + (uint32)columns; left++)
        {
            screenDirtyBlocks[topOffset + left] = 0;
        }
    }

    // Determine region in pixels
    uint32 left = Math::Max<uint32>(0, x * _dirtyGrid.BlockWidth);
    uint32 top = Math::Max<uint32>(0, y * _dirtyGrid.BlockHeight);
    uint32 right = Math::Min(_width, left + (columns * _dirtyGrid.BlockWidth));
    uint32 bottom = Math::Min(_height, top + (rows * _dirtyGrid.BlockHeight));
    if (right <= left || bottom <= top)
    {
        return;
    }

    // Draw region
    window_draw_all(&_bitsDPI, left, top, right, bottom);
}

X8DrawingContext::X8DrawingContext(X8DrawingEngine * engine)
{
    _engine = engine;
}

X8DrawingContext::~X8DrawingContext()
{

}

IDrawingEngine * X8DrawingContext::GetEngine()
{
    return _engine;
}

void X8DrawingContext::Clear(uint8 paletteIndex)
{
    rct_drawpixelinfo * dpi = _dpi;

    sint32 w = dpi->width >> dpi->zoom_level;
    sint32 h = dpi->height >> dpi->zoom_level;
    uint8 * ptr = dpi->bits;

    for (sint32 y = 0; y < h; y++)
    {
        Memory::Set(ptr, paletteIndex, w);
        ptr += w + dpi->pitch;
    }
}

/** rct2: 0x0097FF04 */
static const uint16 Pattern[] = {
    0b0111111110000000,
    0b0011111111000000,
    0b0001111111100000,
    0b0000111111110000,
    0b0000011111111000,
    0b0000001111111100,
    0b0000000111111110,
    0b0000000011111111,
    0b1000000001111111,
    0b1100000000111111,
    0b1110000000011111,
    0b1111000000001111,
    0b1111100000000111,
    0b1111110000000011,
    0b1111111000000001,
    0b1111111100000000,
};

/** rct2: 0x0097FF14 */
static const uint16 PatternInverse[] = {
    0b1000000001111111,
    0b1100000000111111,
    0b1110000000011111,
    0b1111000000001111,
    0b1111100000000111,
    0b1111110000000011,
    0b1111111000000001,
    0b1111111100000000,
    0b0111111110000000,
    0b0011111111000000,
    0b0001111111100000,
    0b0000111111110000,
    0b0000011111111000,
    0b0000001111111100,
    0b0000000111111110,
    0b0000000011111111
};

/** rct2: 0x0097FEFC */
static const uint16 * Patterns[] = {
    Pattern,
    PatternInverse
};

void X8DrawingContext::FillRect(uint32 colour, sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    rct_drawpixelinfo * dpi = _dpi;

    if (left > right) return;
    if (top > bottom) return;
    if (dpi->x > right) return;
    if (left >= dpi->x + dpi->width) return;
    if (bottom < dpi->y) return;
    if (top >= dpi->y + dpi->height) return;

    uint16 crossPattern = 0;

    sint32 startX = left - dpi->x;
    if (startX < 0)
    {
        crossPattern ^= startX;
        startX = 0;
    }

    sint32 endX = right - dpi->x + 1;
    if (endX > dpi->width)
    {
        endX = dpi->width;
    }

    sint32 startY = top - dpi->y;
    if (startY < 0)
    {
        crossPattern ^= startY;
        startY = 0;
    }

    sint32 endY = bottom - dpi->y + 1;
    if (endY > dpi->height)
    {
        endY = dpi->height;
    }

    sint32 width = endX - startX;
    sint32 height = endY - startY;

    if (colour & 0x1000000)
    {
        // Cross hatching
        uint8 * dst = (startY * (dpi->width + dpi->pitch)) + startX + dpi->bits;
        for (sint32 i = 0; i < height; i++)
        {
            uint8 * nextdst = dst + dpi->width + dpi->pitch;
            uint32  p = ror32(crossPattern, 1);
            p = (p & 0xFFFF0000) | width;

            // Fill every other pixel with the colour
            for (; (p & 0xFFFF) != 0; p--)
            {
                p = p ^ 0x80000000;
                if (p & 0x80000000)
                {
                    *dst = colour & 0xFF;
                }
                dst++;
            }
            crossPattern ^= 1;
            dst = nextdst;
        }
    }
    else if (colour & 0x2000000)
    {
        assert(false);
    }
    else if (colour & 0x4000000)
    {
        uint8 * dst = startY * (dpi->width + dpi->pitch) + startX + dpi->bits;

        // The pattern loops every 15 lines this is which
        // part the pattern is on.
        sint32 patternY = (startY + dpi->y) % 16;

        // The pattern loops every 15 pixels this is which
        // part the pattern is on.
        sint32 startPatternX = (startX + dpi->x) % 16;
        sint32 patternX = startPatternX;

        const uint16 * patternsrc = Patterns[colour >> 28]; // or possibly uint8)[esi*4] ?

        for (sint32 numLines = height; numLines > 0; numLines--)
        {
            uint8 * nextdst = dst + dpi->width + dpi->pitch;
            uint16  pattern = patternsrc[patternY];

            for (sint32 numPixels = width; numPixels > 0; numPixels--)
            {
                if (pattern & (1 << patternX))
                {
                    *dst = colour & 0xFF;
                }
                patternX = (patternX + 1) % 16;
                dst++;
            }
            patternX = startPatternX;
            patternY = (patternY + 1) % 16;
            dst = nextdst;
        }
    }
    else
    {
        uint8 * dst = startY * (dpi->width + dpi->pitch) + startX + dpi->bits;
        for (sint32 i = 0; i < height; i++)
        {
            Memory::Set(dst, colour & 0xFF, width);
            dst += dpi->width + dpi->pitch;
        }
    }
}

void X8DrawingContext::FilterRect(FILTER_PALETTE_ID palette, sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    rct_drawpixelinfo * dpi = _dpi;

    if (left > right) return;
    if (top > bottom) return;
    if (dpi->x > right) return;
    if (left >= dpi->x + dpi->width) return;
    if (bottom < dpi->y) return;
    if (top >= dpi->y + dpi->height) return;

    sint32 startX = left - dpi->x;
    if (startX < 0)
    {
        startX = 0;
    }

    sint32 endX = right - dpi->x + 1;
    if (endX > dpi->width)
    {
        endX = dpi->width;
    }

    sint32 startY = top - dpi->y;
    if (startY < 0)
    {
        startY = 0;
    }

    sint32 endY = bottom - dpi->y + 1;
    if (endY > dpi->height)
    {
        endY = dpi->height;
    }

    sint32 width = endX - startX;
    sint32 height = endY - startY;


    //0x2000000
    // 00678B7E   00678C83
    // Location in screen buffer?
    uint8 * dst = dpi->bits + (uint32)((startY >> (dpi->zoom_level)) * ((dpi->width >> dpi->zoom_level) + dpi->pitch) + (startX >> dpi->zoom_level));

    // Find colour in colour table?
    uint16           g1Index = palette_to_g1_offset[palette];
    rct_g1_element * g1Element = &g1Elements[g1Index];
    uint8 *          g1Bits = g1Element->offset;

    // Fill the rectangle with the colours from the colour table
    for (sint32 i = 0; i < height >> dpi->zoom_level; i++)
    {
        uint8 * nextdst = dst + (dpi->width >> dpi->zoom_level) + dpi->pitch;
        for (sint32 j = 0; j < (width >> dpi->zoom_level); j++)
        {
            *dst = g1Bits[*dst];
            dst++;
        }
        dst = nextdst;
    }
}

void X8DrawingContext::DrawLine(uint32 colour, sint32 x1, sint32 y1, sint32 x2, sint32 y2)
{
    gfx_draw_line_software(_dpi, x1, y1, x2, y2, colour);
}

void X8DrawingContext::DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour)
{
    gfx_draw_sprite_software(_dpi, image, x, y, tertiaryColour);
}

void X8DrawingContext::DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage)
{
    gfx_draw_sprite_raw_masked_software(_dpi, x, y, maskImage, colourImage);
}

void X8DrawingContext::DrawSpriteSolid(uint32 image, sint32 x, sint32 y, uint8 colour)
{
    uint8 palette[256];
    memset(palette, colour, 256);
    palette[0] = 0;

    image &= 0x7FFFF;
    gfx_draw_sprite_palette_set_software(_dpi, image | 0x20000000, x, y, palette, nullptr);
}

void X8DrawingContext::DrawGlyph(uint32 image, sint32 x, sint32 y, uint8 * palette)
{
    gfx_draw_sprite_palette_set_software(_dpi, image, x, y, palette, nullptr);
}

void X8DrawingContext::SetDPI(rct_drawpixelinfo * dpi)
{
    _dpi = dpi;
}
