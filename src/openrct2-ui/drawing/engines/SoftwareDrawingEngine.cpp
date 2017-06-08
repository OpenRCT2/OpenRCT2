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

#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/core/Math.hpp>
#include <openrct2/core/Memory.hpp>
#include <openrct2/drawing/IDrawingContext.h>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/drawing/Rain.h>
#include <openrct2/drawing/X8DrawingEngine.h>
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

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

class SoftwareDrawingEngine final : public X8DrawingEngine
{
private:
    IUiContext * const  _uiContext;
    bool const          _hardwareDisplay;

    SDL_Window *    _window         = nullptr;
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
#endif

    // Steam overlay checking
    uint32  _pixelBeforeOverlay     = 0;
    uint32  _pixelAfterOverlay      = 0;
    bool    _overlayActive          = false;
    bool    _pausedBeforeOverlay    = false;

public:
    explicit SoftwareDrawingEngine(IUiContext * uiContext, bool hardwareDisplay)
        : _uiContext(uiContext),
          _hardwareDisplay(hardwareDisplay)
    {
        UNUSED(_uiContext); // Will be used in due course to retrieve window information
    }

    ~SoftwareDrawingEngine() override
    {
        SDL_FreeSurface(_surface);
        SDL_FreeSurface(_RGBASurface);
        SDL_FreePalette(_palette);
        SDL_DestroyTexture(_screenTexture);
        SDL_FreeFormat(_screenTextureFormat);
        SDL_DestroyRenderer(_sdlRenderer);
    }

    void Initialise(SDL_Window * window) override
    {
        _window = window;
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

    void SetPalette(const rct_palette_entry * palette) override
    {
        if (_sdlRenderer != nullptr)
        {
            if (_screenTextureFormat != nullptr)
            {
                for (sint32 i = 0; i < 256; i++)
                {
                    _paletteHWMapped[i] = SDL_MapRGB(_screenTextureFormat, palette[i].red, palette[i].green, palette[i].blue);
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
            SDL_Surface * windowSurface = SDL_GetWindowSurface(_window);
            if (windowSurface != nullptr && _palette != nullptr)
            {
                SDL_Colour colours[256];
                for (sint32 i = 0; i < 256; i++) {
                    colours[i].r = palette[i].red;
                    colours[i].g = palette[i].green;
                    colours[i].b = palette[i].blue;
                    colours[i].a = palette[i].alpha;
                }
                SDL_SetPaletteColors(_palette, colours, 0, 256);
            }
        }
    }

    void Draw() override
    {
        X8DrawingEngine::Draw();
        if (_sdlRenderer != nullptr)
        {
            DisplayViaTexture();
        }
        else
        {
            Display();
        }
    }

private:
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
            SDL_Surface * windowSurface = SDL_GetWindowSurface(_window);
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
            if (SDL_BlitScaled(_RGBASurface, nullptr, SDL_GetWindowSurface(_window), nullptr))
            {
                log_fatal("SDL_BlitScaled %s", SDL_GetError());
                exit(1);
            }
        }
        if (SDL_UpdateWindowSurface(_window))
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

        bool isSteamOverlayActive = GetContext()->GetUiContext()->IsSteamOverlayActive();
        if (isSteamOverlayActive && gConfigGeneral.steam_overlay_pause)
        {
            OverlayPreRenderCheck();
        }

        SDL_RenderPresent(_sdlRenderer);

        if (isSteamOverlayActive && gConfigGeneral.steam_overlay_pause)
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

IDrawingEngine * OpenRCT2::Ui::CreateSoftwareDrawingEngine(IUiContext * uiContext)
{
    return new SoftwareDrawingEngine(uiContext, false);
}

IDrawingEngine * OpenRCT2::Ui::CreateHardwareDisplayDrawingEngine(IUiContext * uiContext)
{
    return new SoftwareDrawingEngine(uiContext, true);
}
