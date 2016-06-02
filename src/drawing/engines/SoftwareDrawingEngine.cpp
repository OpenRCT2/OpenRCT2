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

#include "../../config.h"
#include "../../core/Math.hpp"
#include "../../core/Memory.hpp"
#include "../IDrawingEngine.h"

extern "C"
{
    #include "../drawing.h"
}

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

public:
    SoftwareDrawingEngine()
    {

    }

    ~SoftwareDrawingEngine() override
    {
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
        _width = width;
        _height = height;

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
        DrawAllDirtyBlocks();
        Display();
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
