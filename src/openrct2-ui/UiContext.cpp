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

#include <algorithm>
#include <memory>
#include <vector>
#include <SDL.h>
#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/interface/window.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/Version.h>
#include "CursorRepository.h"
#include "drawing/engines/DrawingEngines.h"
#include "UiContext.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

class UiContext : public IUiContext
{
private:
    IPlatformUiContext * const _platformUiContext;

    CursorRepository _cursorRepository;

    SDL_Window *    _window = nullptr;
    sint32          _width  = 0;
    sint32          _height = 0;

    bool _resolutionsAllowAnyAspectRatio = false;
    std::vector<Resolution> _fsResolutions;

    bool _steamOverlayActive = false;

public:
    UiContext(IPlatformUiContext * platformUiContext)
        : _platformUiContext(platformUiContext)
    {
    }

    ~UiContext() override
    {
        CloseWindow();
    }

    // Window
    CURSOR_ID GetCursor() override
    {
        return _cursorRepository.GetCurrentCursor();
    }

    void SetCursor(CURSOR_ID cursor) override
    {
        _cursorRepository.SetCurrentCursor(cursor);
    }

    void * GetWindow() override
    {
        return _window;
    }

    sint32 GetWidth() override
    {
        return _width;
    }

    sint32 GetHeight() override
    {
        return _height;
    }

    void SetFullscreenMode(FULLSCREEN_MODE mode) override
    {
        static const sint32 SDLFSFlags[] = { 0, SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP };
        uint32 windowFlags = SDLFSFlags[(sint32)mode];

        // HACK Changing window size when in fullscreen usually has no effect
        if (mode == FULLSCREEN_MODE::FULLSCREEN)
        {
            SDL_SetWindowFullscreen(_window, 0);
        }

        // Set window size
        if (mode == FULLSCREEN_MODE::FULLSCREEN)
        {
            UpdateFullscreenResolutions();
            Resolution resolution = GetClosestResolution(gConfigGeneral.fullscreen_width, gConfigGeneral.fullscreen_height);
            SDL_SetWindowSize(_window, resolution.Width, resolution.Height);
        }
        else if (mode == FULLSCREEN_MODE::WINDOWED)
        {
            SDL_SetWindowSize(_window, gConfigGeneral.window_width, gConfigGeneral.window_height);
        }

        if (SDL_SetWindowFullscreen(_window, windowFlags))
        {
            log_fatal("SDL_SetWindowFullscreen %s", SDL_GetError());
            exit(1);

            // TODO try another display mode rather than just exiting the game
        }
    }

    std::vector<Resolution> GetFullscreenResolutions() override
    {
        return _fsResolutions;
    }

    // Drawing
    IDrawingEngine * CreateDrawingEngine(DRAWING_ENGINE_TYPE type) override
    {
        switch ((sint32)type) {
        case DRAWING_ENGINE_SOFTWARE:
            return CreateSoftwareDrawingEngine();
        case DRAWING_ENGINE_SOFTWARE_WITH_HARDWARE_DISPLAY:
            return CreateHardwareDisplayDrawingEngine();
#ifndef DISABLE_OPENGL
        case DRAWING_ENGINE_OPENGL:
            return CreateOpenGLDrawingEngine();
#endif
        default:
            return nullptr;
        }
    }

    // Text input
    bool                        IsTextInputActive() override { return false; }
    const TextInputSession *    StartTextInput(utf8 * buffer, sint32 bufferSize) override { return nullptr; }
    void                        StopTextInput() override { }

private:
    void CreateWindow()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            log_fatal("SDL_Init %s", SDL_GetError());
            exit(-1);
        }

        SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, gConfigGeneral.minimize_fullscreen_focus_loss ? "1" : "0");

        // TODO This should probably be called somewhere else. It has nothing to do with window creation and can be done as soon as
        // g1.dat is loaded.
        sub_68371D();

        // Get saved window size
        sint32 width = gConfigGeneral.window_width;
        sint32 height = gConfigGeneral.window_height;
        if (width == -1) width = 640;
        if (height == -1) height = 480;

        // Create window in window first rather than fullscreen so we have the display the window is on first
        uint32 flags = SDL_WINDOW_RESIZABLE;
        if (gConfigGeneral.drawing_engine == DRAWING_ENGINE_OPENGL)
        {
            flags |= SDL_WINDOW_OPENGL;
        }

        _window = SDL_CreateWindow(OPENRCT2_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
        if (_window == nullptr)
        {
            log_fatal("SDL_CreateWindow failed %s", SDL_GetError());
            exit(-1);
        }

        SDL_SetWindowGrab(_window, gConfigGeneral.trap_cursor ? SDL_TRUE : SDL_FALSE);
        SDL_SetWindowMinimumSize(_window, 720, 480);
        _platformUiContext->SetWindowIcon(_window);
#ifdef __MACOSX__
        macos_disallow_automatic_window_tabbing();
#endif

        // Initialise the surface, palette and draw buffer
        OnResize(width, height);

        UpdateFullscreenResolutions();
        SetFullscreenMode((FULLSCREEN_MODE)gConfigGeneral.fullscreen_mode);

        // Check if steam overlay renderer is loaded into the process
        _steamOverlayActive = _platformUiContext->IsSteamOverlayAttached();
        TriggerResize();
    }

    void CloseWindow()
    {
        drawing_engine_dispose();
        SDL_DestroyWindow(_window);
        SDL_Quit();
    }

    void OnResize(sint32 width, sint32 height)
    {
        // Scale the native window size to the game's canvas size
        _width = (sint32)(width / gConfigGeneral.window_scale);
        _height = (sint32)(height / gConfigGeneral.window_scale);

        drawing_engine_resize();

        uint32 flags = SDL_GetWindowFlags(_window);
        if ((flags & SDL_WINDOW_MINIMIZED) == 0)
        {
            window_resize_gui(_width, _height);
            window_relocate_windows(_width, _height);
        }

        gfx_invalidate_screen();

        // Check if the window has been resized in windowed mode and update the config file accordingly
        // This is called in rct2_update and is only called after resizing a window has finished
        sint32 nonWindowFlags = SDL_WINDOW_MAXIMIZED |
                                SDL_WINDOW_MINIMIZED |
                                SDL_WINDOW_FULLSCREEN |
                                SDL_WINDOW_FULLSCREEN_DESKTOP;
        if (!(flags & nonWindowFlags))
        {
            if (width != gConfigGeneral.window_width || height != gConfigGeneral.window_height)
            {
                gConfigGeneral.window_width = width;
                gConfigGeneral.window_height = height;
                config_save_default();
            }
        }
    }

    /**
     * Helper function to set various render target features.
     * Does not get triggered on resize, but rather manually on config changes.
     */
    void TriggerResize()
    {
        char scaleQualityBuffer[4];
        uint8 scaleQuality = gConfigGeneral.scale_quality;
        if (gConfigGeneral.use_nn_at_integer_scales &&
            gConfigGeneral.window_scale == std::floor(gConfigGeneral.window_scale))
        {
            scaleQuality = 0;
        }
        snprintf(scaleQualityBuffer, sizeof(scaleQualityBuffer), "%u", scaleQuality);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scaleQualityBuffer);

        sint32 width, height;
        SDL_GetWindowSize(_window, &width, &height);
        OnResize(width, height);
    }

    void UpdateFullscreenResolutions()
    {
        // Query number of display modes
        sint32 displayIndex = SDL_GetWindowDisplayIndex(_window);
        sint32 numDisplayModes = SDL_GetNumDisplayModes(displayIndex);

        // Get desktop aspect ratio
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(displayIndex, &mode);

        // Get resolutions
        auto resolutions = std::vector<Resolution>(numDisplayModes);
        float desktopAspectRatio = (float)mode.w / mode.h;
        for (sint32 i = 0; i < numDisplayModes; i++)
        {
            SDL_GetDisplayMode(displayIndex, i, &mode);

            float aspectRatio = (float)mode.w / mode.h;
            if (_resolutionsAllowAnyAspectRatio || std::fabs(desktopAspectRatio - aspectRatio) < 0.0001f)
            {
                resolutions.push_back({ mode.w, mode.h });
            }
        }

        // Sort by area
        std::sort(resolutions.begin(), resolutions.end());
        //    [](const Resolution &a, const Resolution &b) -> bool
        //    {
        //        sint32 areaA = a.Width * a.Height;
        //        sint32 areaB = b.Width * b.Height;
        //        return areaA < areaB;
        //    });

        // Remove duplicates
        auto last = std::unique(resolutions.begin(), resolutions.end());
        //    [](const Resolution &a, const Resolution &b) -> bool
        //    {
        //        return (a.Width == b.Width && a.Height == b.Height);
        //    });
        resolutions.erase(last, resolutions.end());

        // Update config fullscreen resolution if not set
        if (gConfigGeneral.fullscreen_width == -1 || gConfigGeneral.fullscreen_height == -1)
        {
            gConfigGeneral.fullscreen_width = resolutions.back().Width;
            gConfigGeneral.fullscreen_height = resolutions.back().Height;
        }
    }

    Resolution GetClosestResolution(sint32 inWidth, sint32 inHeight)
    {
        Resolution result = { 640, 480 };
        sint32 closestAreaDiff = -1;
        sint32 destinationArea = inWidth * inHeight;
        for (const Resolution &resolution : _fsResolutions)
        {
            // Check if exact match
            if (resolution.Width == inWidth && resolution.Height == inHeight)
            {
                result = resolution;
                break;
            }

            // Check if area is closer to best match
            sint32 areaDiff = std::abs((resolution.Width * resolution.Height) - destinationArea);
            if (closestAreaDiff == -1 || areaDiff < closestAreaDiff)
            {
                closestAreaDiff = areaDiff;
                result = resolution;
            }
        }
        return result;
    }
};

IUiContext * OpenRCT2::Ui::CreateContext()
{
    auto platformUiContext = std::unique_ptr<IPlatformUiContext>(CreatePlatformUiContext());
    return new UiContext(platformUiContext.get());
}
