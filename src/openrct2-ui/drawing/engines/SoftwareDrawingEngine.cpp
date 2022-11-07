/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "DrawingEngineFactory.hpp"

#include <SDL.h>
#include <algorithm>
#include <openrct2/Game.h>
#include <openrct2/common.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/drawing/X8DrawingEngine.h>
#include <openrct2/ui/UiContext.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

class SoftwareDrawingEngine final : public X8DrawingEngine
{
private:
    std::shared_ptr<IUiContext> const _uiContext;
    SDL_Window* _window = nullptr;
    SDL_Surface* _surface = nullptr;
    SDL_Surface* _RGBASurface = nullptr;
    SDL_Palette* _palette = nullptr;

public:
    explicit SoftwareDrawingEngine(const std::shared_ptr<IUiContext>& uiContext)
        : X8DrawingEngine(uiContext)
        , _uiContext(uiContext)
    {
        _window = static_cast<SDL_Window*>(_uiContext->GetWindow());
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

    void Resize(uint32_t width, uint32_t height) override
    {
        SDL_FreeSurface(_surface);
        SDL_FreeSurface(_RGBASurface);
        SDL_FreePalette(_palette);

        _surface = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
        _RGBASurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
        SDL_SetSurfaceBlendMode(_RGBASurface, SDL_BLENDMODE_NONE);
        _palette = SDL_AllocPalette(256);

        if (_surface == nullptr || _palette == nullptr || _RGBASurface == nullptr)
        {
            log_fatal("%p || %p || %p == nullptr %s", _surface, _palette, _RGBASurface, SDL_GetError());
            exit(-1);
        }

        if (SDL_SetSurfacePalette(_surface, _palette))
        {
            log_fatal("SDL_SetSurfacePalette failed %s", SDL_GetError());
            exit(-1);
        }

        ConfigureBits(width, height, _surface->pitch);
    }

    void SetPalette(const GamePalette& palette) override
    {
        SDL_Surface* windowSurface = SDL_GetWindowSurface(_window);
        if (windowSurface != nullptr && _palette != nullptr)
        {
            SDL_Colour colours[256];
            for (int32_t i = 0; i < 256; i++)
            {
                colours[i].r = palette[i].Red;
                colours[i].g = palette[i].Green;
                colours[i].b = palette[i].Blue;
                colours[i].a = palette[i].Alpha;
            }
            SDL_SetPaletteColors(_palette, colours, 0, 256);
        }
    }

    void EndDraw() override
    {
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
        std::copy_n(_bits, _surface->pitch * _surface->h, static_cast<uint8_t*>(_surface->pixels));

        // Unlock the surface
        if (SDL_MUSTLOCK(_surface))
        {
            SDL_UnlockSurface(_surface);
        }

        // Copy the surface to the window
        if (gConfigGeneral.WindowScale == 1 || gConfigGeneral.WindowScale <= 0)
        {
            SDL_Surface* windowSurface = SDL_GetWindowSurface(_window);
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

std::unique_ptr<IDrawingEngine> OpenRCT2::Ui::CreateSoftwareDrawingEngine(const std::shared_ptr<IUiContext>& uiContext)
{
    return std::make_unique<SoftwareDrawingEngine>(uiContext);
}
