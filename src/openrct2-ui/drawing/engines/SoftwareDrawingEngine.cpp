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

#include <openrct2/config/Config.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/core/Math.hpp>
#include <openrct2/core/Memory.hpp>
#include <openrct2/drawing/IDrawingContext.h>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/drawing/Rain.h>
#include "DrawingEngines.h"

extern "C"
{
    #include <openrct2/config/Config.h>
    #include <openrct2/game.h>
    #include <openrct2/interface/screenshot.h>
    #include <openrct2/interface/viewport.h>
    #include <openrct2/interface/window.h>
    #include <openrct2/intro.h>
    #include <openrct2/platform/platform.h>
    #include <openrct2/rct2.h>
    #include <openrct2/drawing/drawing.h>
    #include <openrct2/drawing/lightfx.h>
}

using namespace OpenRCT2::Drawing;

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

class RainDrawer final : public IRainDrawer
{
private:
    struct RainPixel
    {
        uint32 Position;
        uint8  Colour;
    };

    static constexpr uint32 MaxRainPixels = 0xFFFE;

    size_t              _rainPixelsCapacity = MaxRainPixels;
    uint32              _rainPixelsCount    = 0;
    RainPixel *         _rainPixels         = nullptr;
    rct_drawpixelinfo * _screenDPI          = nullptr;

public:
    RainDrawer()
    {
        _rainPixels = new RainPixel[_rainPixelsCapacity];
    }

    ~RainDrawer()
    {
        delete [] _rainPixels;
    }

    void SetDPI(rct_drawpixelinfo * dpi)
    {
        _screenDPI = dpi;
    }

    void Draw(sint32 x, sint32 y, sint32 width, sint32 height, sint32 xStart, sint32 yStart) override
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

class SoftwareDrawingContext final : public IDrawingContext
{
private:
    SoftwareDrawingEngine * _engine = nullptr;
    rct_drawpixelinfo *     _dpi    = nullptr;

public:
    explicit SoftwareDrawingContext(SoftwareDrawingEngine * engine);
    ~SoftwareDrawingContext() override;

    IDrawingEngine * GetEngine() override;

    void Clear(uint8 paletteIndex) override;
    void FillRect(uint32 colour, sint32 x, sint32 y, sint32 w, sint32 h) override;
    void FilterRect(FILTER_PALETTE_ID palette, sint32 left, sint32 top, sint32 right, sint32 bottom) override;
    void DrawLine(uint32 colour, sint32 x1, sint32 y1, sint32 x2, sint32 y2) override;
    void DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour) override;
    void DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage) override;
    void DrawSpriteSolid(uint32 image, sint32 x, sint32 y, uint8 colour) override;
    void DrawGlyph(uint32 image, sint32 x, sint32 y, uint8 * palette) override;

    void SetDPI(rct_drawpixelinfo * dpi);
};

class SoftwareDrawingEngine final : public IDrawingEngine
{
private:
    bool _hardwareDisplay;

    SDL_Surface *   _surface        = nullptr;
    SDL_Surface *   _RGBASurface    = nullptr;
    SDL_Palette *   _palette        = nullptr;

    // For hardware display only
    SDL_Renderer *      _sdlRenderer            = nullptr;
    SDL_Texture *       _screenTexture          = nullptr;
    SDL_PixelFormat *   _screenTextureFormat    = nullptr;
    uint32              _paletteHWMapped[256] = { 0 };
#ifdef __ENABLE_LIGHTFX__
    uint32              _lightPaletteHWMapped[256] = { 0 };
    bool                _lastLightFXenabled = false;
#endif

    // Steam overlay checking
    uint32  _pixelBeforeOverlay     = 0;
    uint32  _pixelAfterOverlay      = 0;
    bool    _overlayActive          = false;
    bool    _pausedBeforeOverlay    = false;

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
    explicit SoftwareDrawingEngine(bool hardwareDisplay)
    {
        _hardwareDisplay = hardwareDisplay;
        _drawingContext = new SoftwareDrawingContext(this);
#ifdef __ENABLE_LIGHTFX__
        _lastLightFXenabled = (gConfigGeneral.enable_light_fx != 0);
#endif
    }

    ~SoftwareDrawingEngine() override
    {
        delete _drawingContext;
        delete [] _dirtyGrid.Blocks;
        delete [] _bits;
        SDL_FreeSurface(_surface);
        SDL_FreeSurface(_RGBASurface);
        SDL_FreePalette(_palette);
        SDL_DestroyTexture(_screenTexture);
        SDL_FreeFormat(_screenTextureFormat);
        SDL_DestroyRenderer(_sdlRenderer);
    }

    void Initialise(SDL_Window * window) override
    {
        if (_hardwareDisplay)
        {
            // Try to create the accelerated renderer.
            _sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        }
    }

    void Resize(uint32 width, uint32 height) override
    {
        SDL_FreeSurface(_surface);
        SDL_FreeSurface(_RGBASurface);
        SDL_FreePalette(_palette);
        SDL_DestroyTexture(_screenTexture);
        SDL_FreeFormat(_screenTextureFormat);

        if (_sdlRenderer != nullptr)
        {
            SDL_RendererInfo rendererInfo;
            SDL_GetRendererInfo(_sdlRenderer, &rendererInfo);
            uint32 pixelFormat = SDL_PIXELFORMAT_UNKNOWN;
            for (uint32 i = 0; i < rendererInfo.num_texture_formats; i++)
            {
                uint32 format = rendererInfo.texture_formats[i];
                if (!SDL_ISPIXELFORMAT_FOURCC(format) &&
                    !SDL_ISPIXELFORMAT_INDEXED(format) &&
                    (pixelFormat == SDL_PIXELFORMAT_UNKNOWN || SDL_BYTESPERPIXEL(format) < SDL_BYTESPERPIXEL(pixelFormat)))
                {
                    pixelFormat = format;
                }
            }

            _screenTexture = SDL_CreateTexture(_sdlRenderer, pixelFormat, SDL_TEXTUREACCESS_STREAMING, width, height);

            uint32 format;
            SDL_QueryTexture(_screenTexture, &format, 0, 0, 0);
            _screenTextureFormat = SDL_AllocFormat(format);

            ConfigureBits(width, height, width);
        }
        else
        {
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
    }

    void SetPalette(SDL_Color * palette) override
    {
        if (_sdlRenderer != nullptr)
        {
            if (_screenTextureFormat != nullptr)
            {
                for (sint32 i = 0; i < 256; i++)
                {
                    _paletteHWMapped[i] = SDL_MapRGB(_screenTextureFormat, palette[i].r, palette[i].g, palette[i].b);
                }

#ifdef __ENABLE_LIGHTFX__
                if (gConfigGeneral.enable_light_fx)
                {
                    const SDL_Color * lightPalette = lightfx_get_palette();
                    for (sint32 i = 0; i < 256; i++)
                    {
                        _lightPaletteHWMapped[i] = SDL_MapRGBA(_screenTextureFormat, lightPalette[i].r, lightPalette[i].g, lightPalette[i].b, lightPalette[i].a);
                    }
                }
#endif
            }
        }
        else
        {
            SDL_Surface * windowSurface = SDL_GetWindowSurface(gWindow);
            if (windowSurface == nullptr)
            {
                log_fatal("SDL_GetWindowSurface failed %s", SDL_GetError());
                exit(1);
            }

            if (_palette != nullptr && SDL_SetPaletteColors(_palette, palette, 0, 256))
            {
                log_fatal("SDL_SetPaletteColors failed %s", SDL_GetError());
                exit(1);
            }
        }
    }

    void SetUncappedFrameRate(bool uncapped) override
    {
        // Not applicable for this engine
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
#ifdef __ENABLE_LIGHTFX__
            // HACK we need to re-configure the bits if light fx has been enabled / disabled
            if (_lastLightFXenabled != (gConfigGeneral.enable_light_fx != 0)) {
                Resize(_width, _height);
            }
#endif

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

        if (_sdlRenderer != nullptr)
        {
            DisplayViaTexture();
        }
        else
        {
            Display();
        }
    }

    void CopyRect(sint32 x, sint32 y, sint32 width, sint32 height, sint32 dx, sint32 dy) override
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

    sint32 Screenshot() override
    {
        return screenshot_dump_png(&_bitsDPI);
    }

    IDrawingContext * GetDrawingContext(rct_drawpixelinfo * dpi) override
    {
        _drawingContext->SetDPI(dpi);
        return _drawingContext;
    }

    rct_drawpixelinfo * GetDrawingPixelInfo() override
    {
        return &_bitsDPI;
    }

    DRAWING_ENGINE_FLAGS GetFlags() override
    {
        return DEF_DIRTY_OPTIMISATIONS;
    }

    void InvalidateImage(uint32 image) override
    {
        // Not applicable for this engine
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

#ifdef __ENABLE_LIGHTFX__
        if (gConfigGeneral.enable_light_fx)
        {
            lightfx_update_buffers(dpi);
        }
#endif
    }

    void ConfigureDirtyGrid()
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

    static void ResetWindowVisbilities()
    {
        // reset window visibility status to unknown
        for (rct_window *w = g_window_list; w < gWindowNextSlot; w++)
        {
            w->visibility = VC_UNKNOWN;
            if (w->viewport != NULL) w->viewport->visibility = VC_UNKNOWN;
        }
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
        window_draw_all(&_bitsDPI, left, top, right, bottom);
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

    void DisplayViaTexture()
    {
#ifdef __ENABLE_LIGHTFX__
        if (gConfigGeneral.enable_light_fx)
        {
            lightfx_render_to_texture(_screenTexture, _bits, _width, _height, _paletteHWMapped, _lightPaletteHWMapped);
        }
        else
#endif
        {
            CopyBitsToTexture(_screenTexture, _bits, (sint32)_width, (sint32)_height, _paletteHWMapped);
        }
        SDL_RenderCopy(_sdlRenderer, _screenTexture, nullptr, nullptr);

        if (gSteamOverlayActive && gConfigGeneral.steam_overlay_pause)
        {
            OverlayPreRenderCheck();
        }

        SDL_RenderPresent(_sdlRenderer);

        if (gSteamOverlayActive && gConfigGeneral.steam_overlay_pause)
        {
            OverlayPostRenderCheck();
        }
    }

    void CopyBitsToTexture(SDL_Texture * texture, uint8 * src, sint32 width, sint32 height, uint32 * palette)
    {
        void *  pixels;
        sint32     pitch;
        if (SDL_LockTexture(texture, nullptr, &pixels, &pitch) == 0)
        {
            sint32 padding = pitch - (width * 4);
            if (pitch == width * 4)
            {
                uint32 * dst = (uint32 *)pixels;
                for (sint32 i = width * height; i > 0; i--)
                {
                    *dst++ = palette[*src++];
                }
            }
            else
            {
                if (pitch == (width * 2) + padding)
                {
                    uint16 * dst = (uint16 *)pixels;
                    for (sint32 y = height; y > 0; y--)
                    {
                        for (sint32 x = width; x > 0; x--)
                        {
                            const uint8 lower = *(uint8 *)(&palette[*src++]);
                            const uint8 upper = *(uint8 *)(&palette[*src++]);
                            *dst++ = (lower << 8) | upper;
                        }
                        dst = (uint16*)(((uint8 *)dst) + padding);
                    }
                }
                else if (pitch == width + padding)
                {
                    uint8 * dst = (uint8 *)pixels;
                    for (sint32 y = height; y > 0; y--)
                    {
                        for (sint32 x = width; x > 0; x--)
                        {
                            *dst++ = *(uint8 *)(&palette[*src++]);
                        }
                        dst += padding;
                    }
                }
            }
            SDL_UnlockTexture(texture);
        }
    }

    void ReadCentrePixel(uint32 * pixel)
    {
        SDL_Rect centrePixelRegion = { (sint32)(_width / 2), (sint32)(_height / 2), 1, 1 };
        SDL_RenderReadPixels(_sdlRenderer, &centrePixelRegion, SDL_PIXELFORMAT_RGBA8888, pixel, sizeof(uint32));
    }

    // Should be called before SDL_RenderPresent to capture frame buffer before Steam overlay is drawn.
    void OverlayPreRenderCheck()
    {
        ReadCentrePixel(&_pixelBeforeOverlay);
    }

    // Should be called after SDL_RenderPresent, when Steam overlay has had the chance to be drawn.
    void OverlayPostRenderCheck()
    {
        ReadCentrePixel(&_pixelAfterOverlay);

        // Detect an active Steam overlay by checking if the center pixel is changed by the gray fade.
        // Will not be triggered by applications rendering to corners, like FRAPS, MSI Afterburner and Friends popups.
        bool newOverlayActive = _pixelBeforeOverlay != _pixelAfterOverlay;

        // Toggle game pause state consistently with base pause state
        if (!_overlayActive && newOverlayActive)
        {
            _pausedBeforeOverlay = gGamePaused & GAME_PAUSED_NORMAL;
            if (!_pausedBeforeOverlay)
            {
                pause_toggle();
            }
        }
        else if (_overlayActive && !newOverlayActive && !_pausedBeforeOverlay)
        {
            pause_toggle();
        }

        _overlayActive = newOverlayActive;
    }
};

IDrawingEngine * OpenRCT2::Ui::CreateSoftwareDrawingEngine()
{
    return new SoftwareDrawingEngine(false);
}

IDrawingEngine * OpenRCT2::Ui::CreateHardwareDisplayDrawingEngine()
{
    return new SoftwareDrawingEngine(true);
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

void SoftwareDrawingContext::Clear(uint8 paletteIndex)
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

void SoftwareDrawingContext::FillRect(uint32 colour, sint32 left, sint32 top, sint32 right, sint32 bottom)
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

void SoftwareDrawingContext::FilterRect(FILTER_PALETTE_ID palette, sint32 left, sint32 top, sint32 right, sint32 bottom)
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

void SoftwareDrawingContext::DrawLine(uint32 colour, sint32 x1, sint32 y1, sint32 x2, sint32 y2)
{
    gfx_draw_line_software(_dpi, x1, y1, x2, y2, colour);
}

void SoftwareDrawingContext::DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour)
{
    gfx_draw_sprite_software(_dpi, image, x, y, tertiaryColour);
}

void SoftwareDrawingContext::DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage)
{
    gfx_draw_sprite_raw_masked_software(_dpi, x, y, maskImage, colourImage);
}

void SoftwareDrawingContext::DrawSpriteSolid(uint32 image, sint32 x, sint32 y, uint8 colour)
{
    uint8 palette[256];
    memset(palette, colour, 256);
    palette[0] = 0;

    image &= 0x7FFFF;
    gfx_draw_sprite_palette_set_software(_dpi, image | 0x20000000, x, y, palette, nullptr);
}

void SoftwareDrawingContext::DrawGlyph(uint32 image, sint32 x, sint32 y, uint8 * palette)
{
    gfx_draw_sprite_palette_set_software(_dpi, image, x, y, palette, nullptr);
}

void SoftwareDrawingContext::SetDPI(rct_drawpixelinfo * dpi)
{
    _dpi = dpi;
}
