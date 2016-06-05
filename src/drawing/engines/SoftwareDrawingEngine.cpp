#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../../core/Math.hpp"
#include "../../core/Memory.hpp"
#include "../IDrawingContext.h"
#include "../IDrawingEngine.h"
#include "../Rain.h"

extern "C"
{
    #include "../../config.h"
    #include "../../interface/window.h"
    #include "../../intro.h"
    #include "../drawing.h"
}

class SoftwareDrawingEngine;

struct DirtyGrid
{
    uint32  BlockShiftX;
    uint32  BlockShiftY;
    uint32  BlockWidth;
    uint32  BlockHeight;
    uint32  BlockColumns;
    uint32  BlockRows;
    uint8 * Blocks;
};

class RainDrawer : public IRainDrawer
{
private:
    struct RainPixel
    {
        uint32 Position;
        uint8  Colour;
    };

    static constexpr uint32 MaxRainPixels = 0xFFFE;

    size_t              _rainPixelsCapacity;
    uint32              _rainPixelsCount;
    RainPixel *         _rainPixels;
    rct_drawpixelinfo * _screenDPI;

public:
    RainDrawer()
    {
        _rainPixelsCapacity = MaxRainPixels;
        _rainPixelsCount = 0;
        _rainPixels = new RainPixel[_rainPixelsCapacity];
    }

    ~RainDrawer()
    {
        delete _rainPixels;
    }

    void SetDPI(rct_drawpixelinfo * dpi)
    {
        _screenDPI = dpi;
    }

    void Draw(sint32 x, sint32 y, sint32 width, sint32 height, sint32 xStart, sint32 yStart)
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

    void Restore()
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
};

class SoftwareDrawingContext : public IDrawingContext
{
private:
    SoftwareDrawingEngine * _engine;
    rct_drawpixelinfo *     _dpi;

public:
    SoftwareDrawingContext(SoftwareDrawingEngine * engine);
    ~SoftwareDrawingContext() override;

    IDrawingEngine * GetEngine() override;

    void Clear(uint32 colour) override;
    void FillRect(uint32 colour, sint32 x, sint32 y, sint32 w, sint32 h) override;
    void DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour) override;
    void DrawSpritePaletteSet(uint32 image, sint32 x, sint32 y, uint8 * palette, uint8 * unknown) override;
    void DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage) override;

    void SetDPI(rct_drawpixelinfo * dpi);
};

class SoftwareDrawingEngine : public IDrawingEngine
{
private:
    SDL_Window *    _window         = nullptr;
    SDL_Surface *   _surface        = nullptr;
    SDL_Surface *   _RGBASurface    = nullptr;
    SDL_Palette *   _palette        = nullptr;

    uint32  _width      = 0;
    uint32  _height     = 0;
    uint32  _pitch      = 0;
    size_t  _bitsSize   = 0;
    uint8 * _bits       = nullptr;

    DirtyGrid   _dirtyGrid  = { 0 };

    rct_drawpixelinfo _bitsDPI  = { 0 };

    RainDrawer                  _rainDrawer;
    SoftwareDrawingContext *    _drawingContext;

public:
    SoftwareDrawingEngine()
    {
        _drawingContext = new SoftwareDrawingContext(this);
    }

    ~SoftwareDrawingEngine() override
    {
        delete _drawingContext;
        delete _dirtyGrid.Blocks;
        delete _bits;
        SDL_FreeSurface(_surface);
        SDL_FreeSurface(_RGBASurface);
        SDL_FreePalette(_palette);
    }

    void Initialise(SDL_Window * window) override
    {
        _window = window;
    }

    void Resize(uint32 width, uint32 height) override
    {
        SDL_FreeSurface(_surface);
        SDL_FreeSurface(_RGBASurface);
        SDL_FreePalette(_palette);

        _surface = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
        _RGBASurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
        SDL_SetSurfaceBlendMode(_RGBASurface, SDL_BLENDMODE_NONE);
        _palette = SDL_AllocPalette(256);

        if (_surface == nullptr ||
            _palette == nullptr ||
            _RGBASurface == nullptr)
        {
            log_fatal("%p || %p || %p == NULL %s", _surface, _palette, _RGBASurface, SDL_GetError());
            exit(-1);
        }

        if (SDL_SetSurfacePalette(_surface, _palette))
        {
            log_fatal("SDL_SetSurfacePalette failed %s", SDL_GetError());
            exit(-1);
        }

        ConfigureBits(width, height, _surface->pitch);
    }

    void SetPalette(SDL_Color * palette) override
    {
        SDL_Surface * windowSurface = SDL_GetWindowSurface(gWindow);
        if (windowSurface == nullptr)
        {
            log_fatal("SDL_GetWindowSurface failed %s", SDL_GetError());
            exit(1);
        }

        if (_palette != nullptr && SDL_SetPaletteColors(_palette, gPalette, 0, 256))
        {
            log_fatal("SDL_SetPaletteColors failed %s", SDL_GetError());
            exit(1);
        }
    }

    void Invalidate(sint32 left, sint32 top, sint32 right, sint32 bottom) override
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

    void Draw() override
    {
        if (gIntroState != INTRO_STATE_NONE) {
            intro_draw(&_bitsDPI);
        } else {
            _rainDrawer.SetDPI(&_bitsDPI);
            _rainDrawer.Restore();

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
        Display();
    }

    IDrawingContext * GetDrawingContext(rct_drawpixelinfo * dpi) override
    {
        _drawingContext->SetDPI(dpi);
        return _drawingContext;
    }

    rct_drawpixelinfo * GetDPI()
    {
        return &_bitsDPI;
    }

private:

    void ConfigureBits(uint32 width, uint32 height, uint32 pitch)
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
            delete _bits;
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

        gScreenDPI = *dpi;

        ConfigureDirtyGrid();
    }

    void ConfigureDirtyGrid()
    {
        _dirtyGrid.BlockShiftX = 7;
        _dirtyGrid.BlockShiftY = 6;
        _dirtyGrid.BlockWidth = 1 << _dirtyGrid.BlockShiftX;
        _dirtyGrid.BlockHeight = 1 << _dirtyGrid.BlockShiftY;
        _dirtyGrid.BlockColumns = (_width >> _dirtyGrid.BlockShiftX) + 1;
        _dirtyGrid.BlockRows = (_height >> _dirtyGrid.BlockShiftY) + 1;

        delete _dirtyGrid.Blocks;
        _dirtyGrid.Blocks = new uint8[_dirtyGrid.BlockColumns * _dirtyGrid.BlockRows];
    }

    void DrawAllDirtyBlocks()
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

    void DrawDirtyBlocks(uint32 x, uint32 y, uint32 columns, uint32 rows)
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
        uint32 right = Math::Min((uint32)gScreenWidth, left + (columns * _dirtyGrid.BlockWidth));
        uint32 bottom = Math::Min((uint32)gScreenHeight, top + (rows * _dirtyGrid.BlockHeight));
        if (right <= left || bottom <= top)
        {
            return;
        }

        // Draw region
        gfx_redraw_screen_rect(left, top, right, bottom);
    }

    void Display()
    {
        // Lock the surface before setting its pixels
        if (SDL_MUSTLOCK(_surface))
        {
            if (SDL_LockSurface(_surface) < 0)
            {
                log_error("locking failed %s", SDL_GetError());
                return;
            }
        }

        // Copy pixels from the virtual screen buffer to the surface
        Memory::Copy<void>(_surface->pixels, _bits, _surface->pitch * _surface->h);

        // Unlock the surface
        if (SDL_MUSTLOCK(_surface))
        {
            SDL_UnlockSurface(_surface);
        }

        // Copy the surface to the window
        if (gConfigGeneral.window_scale == 1 || gConfigGeneral.window_scale <= 0)
        {
            SDL_Surface * windowSurface = SDL_GetWindowSurface(gWindow);
            if (SDL_BlitSurface(_surface, nullptr, windowSurface, nullptr))
            {
                log_fatal("SDL_BlitSurface %s", SDL_GetError());
                exit(1);
            }
        }
        else
        {
            // first blit to rgba surface to change the pixel format
            if (SDL_BlitSurface(_surface, nullptr, _RGBASurface, nullptr))
            {
                log_fatal("SDL_BlitSurface %s", SDL_GetError());
                exit(1);
            }

            // then scale to window size. Without changing to RGBA first, SDL complains
            // about blit configurations being incompatible.
            if (SDL_BlitScaled(_RGBASurface, nullptr, SDL_GetWindowSurface(gWindow), nullptr))
            {
                log_fatal("SDL_BlitScaled %s", SDL_GetError());
                exit(1);
            }
        }
        if (SDL_UpdateWindowSurface(gWindow))
        {
            log_fatal("SDL_UpdateWindowSurface %s", SDL_GetError());
            exit(1);
        }
    }
};

IDrawingEngine * DrawingEngineFactory::CreateSoftware()
{
    return new SoftwareDrawingEngine();
}

SoftwareDrawingContext::SoftwareDrawingContext(SoftwareDrawingEngine * engine)
{
    _engine = engine;
}

SoftwareDrawingContext::~SoftwareDrawingContext()
{

}

IDrawingEngine * SoftwareDrawingContext::GetEngine()
{
    return _engine;
}

void SoftwareDrawingContext::Clear(uint32 colour)
{
    rct_drawpixelinfo * dpi = _dpi;

    int w = dpi->width >> dpi->zoom_level;
    int h = dpi->height >> dpi->zoom_level;
    uint8 * ptr = dpi->bits;

    for (int y = 0; y < h; y++)
    {
        Memory::Set(ptr, colour, w);
        ptr += w + dpi->pitch;
    }
}

void SoftwareDrawingContext::FillRect(uint32 colour, sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    rct_drawpixelinfo * dpi = _dpi;

    if (left > right) return;
    if (top > bottom) return;
    if (dpi->x > right) return;
    if (left >= dpi->x + dpi->width) return;
    if (bottom < dpi->y) return;
    if (top >= dpi->y + dpi->height) return;

    colour |= RCT2_GLOBAL(0x009ABD9C, uint32);

    uint16 crossPattern = 0;

    int startX = left - dpi->x;
    if (startX < 0)
    {
        crossPattern ^= startX;
        startX = 0;
    }

    int endX = right - dpi->x + 1;
    if (endX > dpi->width)
    {
        endX = dpi->width;
    }

    int startY = top - dpi->y;
    if (startY < 0)
    {
        crossPattern ^= startY;
        startY = 0;
    }

    int endY = bottom - dpi->y + 1;
    if (endY > dpi->height)
    {
        endY = dpi->height;
    }

    int width = endX - startX;
    int height = endY - startY;

    if (colour & 0x1000000)
    {
        // Cross hatching
        uint8 * dst = (startY * (dpi->width + dpi->pitch)) + startX + dpi->bits;
        for (int i = 0; i < height; i++)
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
        //0x2000000
        // 00678B7E   00678C83
        // Location in screen buffer?
        uint8 * dst = dpi->bits + (uint32)((startY >> (dpi->zoom_level)) * ((dpi->width >> dpi->zoom_level) + dpi->pitch) + (startX >> dpi->zoom_level));
    
        // Find colour in colour table?
        uint16           g1Index = palette_to_g1_offset[colour & 0xFF];
        rct_g1_element * g1Element = &g1Elements[g1Index];
        uint8 *          g1Bits = g1Element->offset;
    
        // Fill the rectangle with the colours from the colour table
        for (int i = 0; i < height >> dpi->zoom_level; i++)
        {
            uint8 * nextdst = dst + (dpi->width >> dpi->zoom_level) + dpi->pitch;
            for (int j = 0; j < (width >> dpi->zoom_level); j++)
            {
                *dst = g1Bits[*dst];
                dst++;
            }
            dst = nextdst;
        }
    }
    else if (colour & 0x4000000)
    {
        uint8 * dst = startY * (dpi->width + dpi->pitch) + startX + dpi->bits;
    
        // The pattern loops every 15 lines this is which
        // part the pattern is on.
        int patternY = (startY + dpi->y) % 16;
    
        // The pattern loops every 15 pixels this is which
        // part the pattern is on.
        int startPatternX = (startX + dpi->x) % 16;
        int patternX = startPatternX;
    
        uint16 * patternsrc = RCT2_ADDRESS(0x0097FEFC, uint16*)[colour >> 28]; // or possibly uint8)[esi*4] ?
    
        for (int numLines = height; numLines > 0; numLines--)
        {
            uint8 * nextdst = dst + dpi->width + dpi->pitch;
            uint16  pattern = patternsrc[patternY];
    
            for (int numPixels = width; numPixels > 0; numPixels--)
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
    else if (colour & 0x8000000)
    {
        int esi = left - RCT2_GLOBAL(0x1420070, sint16);
        RCT2_GLOBAL(0xEDF824, uint32) = esi;
        esi = top - RCT2_GLOBAL(0x1420072, sint16);
        RCT2_GLOBAL(0xEDF828, uint32) = esi;
        left -= dpi->x;
        if (left < 0)
        {
            RCT2_GLOBAL(0xEDF824, sint32) -= left;
            left = 0;
        }
        right -= dpi->x;
        right++;
        if (right > dpi->width)
        {
            right = dpi->width;
        }
        right -= left;
        top -= dpi->y;
        if (top < 0)
        {
            RCT2_GLOBAL(0xEDF828, sint32) -= top;
            top = 0;
        }
        bottom -= dpi->y;
        bottom++;
        if (bottom > dpi->height)
        {
            bottom = dpi->height;
        }
        bottom -= top;
        RCT2_GLOBAL(0xEDF824, sint32) &= 0x3F;
        RCT2_GLOBAL(0xEDF828, sint32) &= 0x3F;
        esi = dpi->width;
        esi += dpi->pitch;
        esi *= top;
        esi += left;
        esi += (uint32)dpi->bits;
        RCT2_GLOBAL(0xEDF82C, sint32) = right;
        RCT2_GLOBAL(0xEDF830, sint32) = bottom;
        left = dpi->width;
        left += dpi->pitch;
        left -= right;
        RCT2_GLOBAL(0xEDF834, sint32) = left;
        colour &= 0xFF;
        colour--;
        right = colour;
        colour <<= 8;
        right |= colour;
        RCT2_GLOBAL(0xEDF838, sint32) = right;
        //right <<= 4;
        esi = RCT2_GLOBAL(0xEDF828, sint32);
        esi *= 0x40;
        left = 0;
        esi += (uint32)g1Elements[right].offset;//???
        //Not finished
        //Start of loop
    }
    else
    {
        uint8 * dst = startY * (dpi->width + dpi->pitch) + startX + dpi->bits;
        for (int i = 0; i < height; i++)
        {
            Memory::Set(dst, colour & 0xFF, width);
            dst += dpi->width + dpi->pitch;
        }
    }
}

void SoftwareDrawingContext::DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour)
{
    gfx_draw_sprite_software(_dpi, image, x, y, tertiaryColour);
}

void SoftwareDrawingContext::DrawSpritePaletteSet(uint32 image, sint32 x, sint32 y, uint8 * palette, uint8 * unknown)
{
    gfx_draw_sprite_palette_set_software(_dpi, image, x, y, palette, unknown);
}

void SoftwareDrawingContext::DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage)
{
    gfx_draw_sprite_raw_masked_software(_dpi, x, y, maskImage, colourImage);
}

void SoftwareDrawingContext::SetDPI(rct_drawpixelinfo * dpi)
{
    _dpi = dpi;
}
