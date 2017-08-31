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

#include <vector>
#include <openrct2/common.h>
#include <SDL.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/drawing/X8DrawingEngine.h>
#include <openrct2/ui/UiContext.h>
#include "DrawingEngines.h"

extern "C"
{
    #include <openrct2/drawing/lightfx.h>
    #include <openrct2/game.h>
    #include <openrct2/paint/paint.h>
}

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

class HardwareDisplayDrawingEngine final : public X8DrawingEngine
{
private:
    constexpr static uint32 DIRTY_VISUAL_TIME = 32;

    IUiContext * const  _uiContext;
    SDL_Window *        _window                     = nullptr;
    SDL_Renderer *      _sdlRenderer                = nullptr;
    SDL_Texture *       _screenTexture              = nullptr;
    SDL_PixelFormat *   _screenTextureFormat        = nullptr;
    uint32              _paletteHWMapped[256]       = { 0 };
#ifdef __ENABLE_LIGHTFX__
    uint32              _lightPaletteHWMapped[256]  = { 0 };
#endif

    // Steam overlay checking
    uint32  _pixelBeforeOverlay     = 0;
    uint32  _pixelAfterOverlay      = 0;
    bool    _overlayActive          = false;
    bool    _pausedBeforeOverlay    = false;

    std::vector<uint32> _dirtyVisualsTime;

public:
    explicit HardwareDisplayDrawingEngine(IUiContext * uiContext)
        : X8DrawingEngine(uiContext),
          _uiContext(uiContext)
    {
        _window = (SDL_Window *)_uiContext->GetWindow();
    }

    ~HardwareDisplayDrawingEngine() override
    {
        SDL_DestroyTexture(_screenTexture);
        SDL_FreeFormat(_screenTextureFormat);
        SDL_DestroyRenderer(_sdlRenderer);
    }

    void Initialise() override
    {
        _sdlRenderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    }

    void Resize(uint32 width, uint32 height) override
    {
        SDL_DestroyTexture(_screenTexture);
        SDL_FreeFormat(_screenTextureFormat);

        SDL_RendererInfo rendererInfo = {};
        sint32 result = SDL_GetRendererInfo(_sdlRenderer, &rendererInfo);
        if (result < 0)
        {
            log_warning("HWDisplayDrawingEngine::Resize error: %s", SDL_GetError());
            return;
        }
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

    void SetPalette(const rct_palette_entry * palette) override
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
                auto lightPalette = lightfx_get_palette();
                for (sint32 i = 0; i < 256; i++)
                {
                    auto src = &lightPalette->entries[i];
                    _lightPaletteHWMapped[i] = SDL_MapRGBA(_screenTextureFormat, src->red, src->green, src->blue, src->alpha);
                }
            }
#endif
        }
    }

    void EndDraw() override
    {
        Display();
        if (gShowDirtyVisuals)
        {
            UpdateDirtyVisuals();
        }
    }

protected:
    void OnDrawDirtyBlock(uint32 left, uint32 top, uint32 columns, uint32 rows) override
    {
        if (gShowDirtyVisuals)
        {
            uint32 right = left + columns;
            uint32 bottom = top + rows;
            for (uint32 x = left; x < right; x++)
            {
                for (uint32 y = top; y < bottom; y++)
                {
                    SetDirtyVisualTime(x, y, DIRTY_VISUAL_TIME);
                }
            }
        }
    }

private:
    void Display()
    {
#ifdef __ENABLE_LIGHTFX__
        if (gConfigGeneral.enable_light_fx)
        {
            void * pixels;
            sint32 pitch;
            if (SDL_LockTexture(_screenTexture, nullptr, &pixels, &pitch) == 0)
            {
                lightfx_render_to_texture(pixels, pitch, _bits, _width, _height, _paletteHWMapped, _lightPaletteHWMapped);
                SDL_UnlockTexture(_screenTexture);
            }
        }
        else
#endif
        {
            CopyBitsToTexture(_screenTexture, _bits, (sint32)_width, (sint32)_height, _paletteHWMapped);
        }
        SDL_RenderCopy(_sdlRenderer, _screenTexture, nullptr, nullptr);

        if (gShowDirtyVisuals)
        {
            RenderDirtyVisuals();
        }

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

    uint32 GetDirtyVisualTime(uint32 x, uint32 y)
    {
        uint32 result = 0;
        uint32 i = y * _dirtyGrid.BlockColumns + x;
        if (_dirtyVisualsTime.size() > i)
        {
            result = _dirtyVisualsTime[i];
        }
        return result;
    }

    void SetDirtyVisualTime(uint32 x, uint32 y, uint32 value)
    {
        uint32 i = y * _dirtyGrid.BlockColumns + x;
        if (_dirtyVisualsTime.size() > i)
        {
            _dirtyVisualsTime[i] = value;
        }
    }

    void UpdateDirtyVisuals()
    {
        _dirtyVisualsTime.resize(_dirtyGrid.BlockRows * _dirtyGrid.BlockColumns);
        for (uint32 y = 0; y < _dirtyGrid.BlockRows; y++)
        {
            for (uint32 x = 0; x < _dirtyGrid.BlockColumns; x++)
            {
                auto timeLeft = GetDirtyVisualTime(x, y);
                if (timeLeft > 0)
                {
                    SetDirtyVisualTime(x, y, timeLeft - 1);
                }
            }
        }
    }

    void RenderDirtyVisuals()
    {
        float scaleX = gConfigGeneral.window_scale;
        float scaleY = gConfigGeneral.window_scale;

        SDL_SetRenderDrawBlendMode(_sdlRenderer, SDL_BLENDMODE_BLEND);
        for (uint32 y = 0; y < _dirtyGrid.BlockRows; y++)
        {
            for (uint32 x = 0; x < _dirtyGrid.BlockColumns; x++)
            {
                auto timeLeft = GetDirtyVisualTime(x, y);
                if (timeLeft > 0)
                {
                    uint8 alpha = (uint8)(timeLeft * 5 / 2);

                    SDL_Rect ddRect;
                    ddRect.x = (sint32)(x * _dirtyGrid.BlockWidth * scaleX);
                    ddRect.y = (sint32)(y * _dirtyGrid.BlockHeight * scaleY);
                    ddRect.w = (sint32)(_dirtyGrid.BlockWidth * scaleX);
                    ddRect.h = (sint32)(_dirtyGrid.BlockHeight * scaleY);

                    SDL_SetRenderDrawColor(_sdlRenderer, 255, 255, 255, alpha);
                    SDL_RenderFillRect(_sdlRenderer, &ddRect);
                }
            }
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

        // Detect an active Steam overlay by checking if the centre pixel is changed by the gray fade.
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

IDrawingEngine * OpenRCT2::Ui::CreateHardwareDisplayDrawingEngine(IUiContext * uiContext)
{
    return new HardwareDisplayDrawingEngine(uiContext);
}
