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

#include <openrct2/common.h>
#include <SDL.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/core/Memory.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/drawing/X8DrawingEngine.h>
#include <openrct2/ui/UiContext.h>
#include "DrawingEngines.h"

extern "C"
{
    #include <openrct2/game.h>
}

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

class SoftwareDrawingEngine final : public X8DrawingEngine
{
private:
    IUiContext * const  _uiContext;
    SDL_Window *        _window         = nullptr;
    SDL_Surface *       _surface        = nullptr;
    SDL_Surface *       _RGBASurface    = nullptr;
    SDL_Palette *       _palette        = nullptr;

public:
    explicit SoftwareDrawingEngine(IUiContext * uiContext)
        : _uiContext(uiContext)
    {
        _window = (SDL_Window *)_uiContext->GetWindow();
    }

    ~SoftwareDrawingEngine() override
    {
        SDL_FreeSurface(_surface);
        SDL_FreeSurface(_RGBASurface);
        SDL_FreePalette(_palette);
    }

    void Initialise() override
    {
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

    void SetPalette(const rct_palette_entry * palette) override
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

    void Draw() override
    {
        X8DrawingEngine::Draw();
        Display();
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
};

IDrawingEngine * OpenRCT2::Ui::CreateSoftwareDrawingEngine(IUiContext * uiContext)
{
    return new SoftwareDrawingEngine(uiContext);
}
