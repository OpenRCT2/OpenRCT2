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

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <memory>
#include <vector>
#include <SDL.h>
#include <openrct2/audio/AudioMixer.h>
#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2/core/Math.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/localisation/string_ids.h>
#include <openrct2/platform/Platform2.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/Version.h>
#include "CursorRepository.h"
#include "drawing/engines/DrawingEngines.h"
#include "input/KeyboardShortcuts.h"
#include "SDLException.h"
#include "TextComposition.h"
#include "UiContext.h"
#include "WindowManager.h"

extern "C"
{
    #include <openrct2/input.h>
    #include <openrct2/interface/console.h>
    #include <openrct2/interface/window.h>
    #include <openrct2/windows/error.h>
}

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Input;
using namespace OpenRCT2::Ui;

#ifdef __MACOSX__
    // macOS uses COMMAND rather than CTRL for many keyboard shortcuts
    #define KEYBOARD_PRIMARY_MODIFIER KMOD_GUI
#else
    #define KEYBOARD_PRIMARY_MODIFIER KMOD_CTRL
#endif

class UiContext final : public IUiContext
{
private:
    constexpr static uint32 TOUCH_DOUBLE_TIMEOUT = 300;

    IPlatformUiContext * const      _platformUiContext;
    IWindowManager * const          _windowManager;

    CursorRepository _cursorRepository;

    SDL_Window *    _window = nullptr;
    sint32          _width  = 0;
    sint32          _height = 0;

    bool _resolutionsAllowAnyAspectRatio = false;
    std::vector<Resolution> _fsResolutions;

    bool _steamOverlayActive = false;

    // Input
    KeyboardShortcuts   _keyboardShortcuts;
    TextComposition     _textComposition;
    CursorState         _cursorState            = { 0 };
    uint32              _lastKeyPressed         = 0;
    const uint8 *       _keysState              = nullptr;
    uint8               _keysPressed[256]       = { 0 };
    uint32              _lastGestureTimestamp   = 0;
    float               _gestureRadius          = 0;

public:
    UiContext(IPlatformEnvironment * env)
        : _platformUiContext(CreatePlatformUiContext()),
          _windowManager(CreateWindowManager()),
          _keyboardShortcuts(env)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            SDLException::Throw("SDL_Init(SDL_INIT_VIDEO)");
        }
        _cursorRepository.LoadCursors();
        _keyboardShortcuts.Reset();
        _keyboardShortcuts.Load();
    }

    ~UiContext() override
    {
        CloseWindow();
        delete _windowManager;
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        delete _platformUiContext;
    }

    // Window
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
        UpdateFullscreenResolutions();
        return _fsResolutions;
    }

    bool HasFocus() override
    {
        uint32 windowFlags = GetWindowFlags();
        return (windowFlags & SDL_WINDOW_INPUT_FOCUS) != 0;
    }

    bool IsMinimised() override
    {
        uint32 windowFlags = GetWindowFlags();
        return (windowFlags & SDL_WINDOW_MINIMIZED) ||
               (windowFlags & SDL_WINDOW_HIDDEN);
    }

    bool IsSteamOverlayActive() override
    {
        return _steamOverlayActive;
    }

    // Input
    const CursorState * GetCursorState() override
    {
        return &_cursorState;
    }

    const uint8 * GetKeysState() override
    {
        return _keysState;
    }

    const uint8 * GetKeysPressed() override
    {
        return _keysPressed;
    }

    CURSOR_ID GetCursor() override
    {
        return _cursorRepository.GetCurrentCursor();
    }

    void SetCursor(CURSOR_ID cursor) override
    {
        _cursorRepository.SetCurrentCursor(cursor);
    }

    void SetCursorVisible(bool value) override
    {
        SDL_ShowCursor(value ? SDL_ENABLE : SDL_DISABLE);
    }

    void GetCursorPosition(sint32 * x, sint32 * y) override
    {
        SDL_GetMouseState(x, y);
    }

    void SetCursorPosition(sint32 x, sint32 y) override
    {
        SDL_WarpMouseInWindow(nullptr, x, y);
    }

    void SetCursorTrap(bool value) override
    {
        SDL_SetWindowGrab(_window, value ? SDL_TRUE : SDL_FALSE);
    }

    void SetKeysPressed(uint32 keysym, uint8 scancode) override
    {
        _lastKeyPressed = keysym;
        _keysPressed[scancode] = 1;
    }

    // Drawing
    IDrawingEngine * CreateDrawingEngine(DRAWING_ENGINE_TYPE type) override
    {
        switch ((sint32)type) {
        case DRAWING_ENGINE_SOFTWARE:
            return CreateSoftwareDrawingEngine(this);
        case DRAWING_ENGINE_SOFTWARE_WITH_HARDWARE_DISPLAY:
            return CreateHardwareDisplayDrawingEngine(this);
#ifndef DISABLE_OPENGL
        case DRAWING_ENGINE_OPENGL:
            return CreateOpenGLDrawingEngine(this);
#endif
        default:
            return nullptr;
        }
    }

    // Text input
    bool IsTextInputActive() override
    {
        return _textComposition.IsActive();
    }

    TextInputSession * StartTextInput(utf8 * buffer, size_t bufferSize) override
    {
        return _textComposition.Start(buffer, bufferSize);
    }

    void StopTextInput() override
    {
        _textComposition.Stop();
    }

    void ProcessMessages() override
    {
        _lastKeyPressed = 0;
        _cursorState.left &= ~CURSOR_CHANGED;
        _cursorState.middle &= ~CURSOR_CHANGED;
        _cursorState.right &= ~CURSOR_CHANGED;
        _cursorState.old = 0;
        _cursorState.touch = false;

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type) {
            case SDL_QUIT:
                context_quit();
                break;
            case SDL_WINDOWEVENT:
                // HACK: Fix #2158, OpenRCT2 does not draw if it does not think that the window is
                //                  visible - due a bug in SDL 2.0.3 this hack is required if the
                //                  window is maximised, minimised and then restored again.
                if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                {
                    if (SDL_GetWindowFlags(_window) & SDL_WINDOW_MAXIMIZED)
                    {
                        SDL_RestoreWindow(_window);
                        SDL_MaximizeWindow(_window);
                    }
                    if ((SDL_GetWindowFlags(_window) & SDL_WINDOW_FULLSCREEN_DESKTOP) == SDL_WINDOW_FULLSCREEN_DESKTOP)
                    {
                        SDL_RestoreWindow(_window);
                        SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    }
                }

                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    OnResize(e.window.data1, e.window.data2);
                }

                switch (e.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                case SDL_WINDOWEVENT_MOVED:
                case SDL_WINDOWEVENT_MAXIMIZED:
                case SDL_WINDOWEVENT_RESTORED:
                {
                    // Update default display index
                    sint32 displayIndex = SDL_GetWindowDisplayIndex(_window);
                    if (displayIndex != gConfigGeneral.default_display)
                    {
                        gConfigGeneral.default_display = displayIndex;
                        config_save_default();
                    }
                    break;
                }
                }

                if (gConfigSound.audio_focus && gConfigSound.sound_enabled)
                {
                    if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                    {
                        Mixer_SetVolume(1);
                    }
                    if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                    {
                        Mixer_SetVolume(0);
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                _cursorState.x = (sint32)(e.motion.x / gConfigGeneral.window_scale);
                _cursorState.y = (sint32)(e.motion.y / gConfigGeneral.window_scale);
                break;
            case SDL_MOUSEWHEEL:
                if (gConsoleOpen)
                {
                    console_scroll(e.wheel.y * 3); // Scroll 3 lines at a time
                    break;
                }
                _cursorState.wheel += e.wheel.y * 128;
                break;
            case SDL_MOUSEBUTTONDOWN:
            {
                sint32 x = (sint32)(e.button.x / gConfigGeneral.window_scale);
                sint32 y = (sint32)(e.button.y / gConfigGeneral.window_scale);
                switch (e.button.button) {
                case SDL_BUTTON_LEFT:
                    store_mouse_input(MOUSE_STATE_LEFT_PRESS, x, y);
                    _cursorState.left = CURSOR_PRESSED;
                    _cursorState.old = 1;
                    break;
                case SDL_BUTTON_MIDDLE:
                    _cursorState.middle = CURSOR_PRESSED;
                    break;
                case SDL_BUTTON_RIGHT:
                    store_mouse_input(MOUSE_STATE_RIGHT_PRESS, x, y);
                    _cursorState.right = CURSOR_PRESSED;
                    _cursorState.old = 2;
                    break;
                }
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                sint32 x = (sint32)(e.button.x / gConfigGeneral.window_scale);
                sint32 y = (sint32)(e.button.y / gConfigGeneral.window_scale);
                switch (e.button.button) {
                case SDL_BUTTON_LEFT:
                    store_mouse_input(MOUSE_STATE_LEFT_RELEASE, x, y);
                    _cursorState.left = CURSOR_RELEASED;
                    _cursorState.old = 3;
                    break;
                case SDL_BUTTON_MIDDLE:
                    _cursorState.middle = CURSOR_RELEASED;
                    break;
                case SDL_BUTTON_RIGHT:
                    store_mouse_input(MOUSE_STATE_RIGHT_RELEASE, x, y);
                    _cursorState.right = CURSOR_RELEASED;
                    _cursorState.old = 4;
                    break;
                }
                break;
            }
            // Apple sends touchscreen events for trackpads, so ignore these events on macOS
#ifndef __MACOSX__
            case SDL_FINGERMOTION:
                _cursorState.x = (sint32)(e.tfinger.x * _width);
                _cursorState.y = (sint32)(e.tfinger.y * _height);
                break;
            case SDL_FINGERDOWN:
            {
                sint32 x = (sint32)(e.tfinger.x * _width);
                sint32 y = (sint32)(e.tfinger.y * _height);

                _cursorState.touchIsDouble = (!_cursorState.touchIsDouble &&
                    e.tfinger.timestamp - _cursorState.touchDownTimestamp < TOUCH_DOUBLE_TIMEOUT);

                if (_cursorState.touchIsDouble)
                {
                    store_mouse_input(MOUSE_STATE_RIGHT_PRESS, x, y);
                    _cursorState.right = CURSOR_PRESSED;
                    _cursorState.old = 2;
                }
                else
                {
                    store_mouse_input(MOUSE_STATE_LEFT_PRESS, x, y);
                    _cursorState.left = CURSOR_PRESSED;
                    _cursorState.old = 1;
                }
                _cursorState.touch = true;
                _cursorState.touchDownTimestamp = e.tfinger.timestamp;
                break;
            }
            case SDL_FINGERUP:
            {
                sint32 x = (sint32)(e.tfinger.x * _width);
                sint32 y = (sint32)(e.tfinger.y * _height);

                if (_cursorState.touchIsDouble)
                {
                    store_mouse_input(MOUSE_STATE_RIGHT_RELEASE, x, y);
                    _cursorState.right = CURSOR_RELEASED;
                    _cursorState.old = 4;
                }
                else {
                    store_mouse_input(MOUSE_STATE_LEFT_RELEASE, x, y);
                    _cursorState.left = CURSOR_RELEASED;
                    _cursorState.old = 3;
                }
                _cursorState.touch = true;
                break;
            }
#endif
            case SDL_KEYDOWN:
                _textComposition.HandleMessage(&e);
                break;
            case SDL_MULTIGESTURE:
                if (e.mgesture.numFingers == 2)
                {
                    if (e.mgesture.timestamp > _lastGestureTimestamp + 1000)
                    {
                        _gestureRadius = 0;
                    }
                    _lastGestureTimestamp = e.mgesture.timestamp;
                    _gestureRadius += e.mgesture.dDist;

                    // Zoom gesture
                    constexpr sint32 tolerance = 128;
                    sint32 gesturePixels = (sint32)(_gestureRadius * _width);
                    if (abs(gesturePixels) > tolerance)
                    {
                        _gestureRadius = 0;
                        main_window_zoom(gesturePixels > 0, true);
                    }
                }
                break;
            case SDL_TEXTEDITING:
                _textComposition.HandleMessage(&e);
                break;
            case SDL_TEXTINPUT:
                _textComposition.HandleMessage(&e);
                break;
            default:
                break;
            }
        }

        _cursorState.any = _cursorState.left | _cursorState.middle | _cursorState.right;

        // Updates the state of the keys
        sint32 numKeys = 256;
        _keysState = SDL_GetKeyboardState(&numKeys);
    }

    /**
     * Helper function to set various render target features.
     * Does not get triggered on resize, but rather manually on config changes.
     */
    void TriggerResize() override
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

    void CreateWindow() override
    {
        SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, gConfigGeneral.minimize_fullscreen_focus_loss ? "1" : "0");

        // TODO This should probably be called somewhere else. It has nothing to do with window creation and can be done as soon as
        // g1.dat is loaded.
        // sub_68371D();

        // Set window position to default display
        sint32 defaultDisplay = Math::Clamp(0, gConfigGeneral.default_display, 0xFFFF);
        sint32 x = SDL_WINDOWPOS_UNDEFINED_DISPLAY(defaultDisplay);
        sint32 y = SDL_WINDOWPOS_UNDEFINED_DISPLAY(defaultDisplay);

        CreateWindow(x, y);

        // Check if steam overlay renderer is loaded into the process
        _steamOverlayActive = _platformUiContext->IsSteamOverlayAttached();
    }

    void CloseWindow() override
    {
        drawing_engine_dispose();
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }

    void RecreateWindow() override
    {
        // Use the position of the current window for the new window
        sint32 x, y;
        SDL_SetWindowFullscreen(_window, 0);
        SDL_GetWindowPosition(_window, &x, &y);

        CloseWindow();
        CreateWindow(x, y);
    }

    void ShowMessageBox(const std::string &message) override
    {
        _platformUiContext->ShowMessageBox(_window, message);
    }

    std::string ShowFileDialog(const FileDialogDesc &desc) override
    {
        return _platformUiContext->ShowFileDialog(_window, desc);
    }

    std::string ShowDirectoryDialog(const std::string &title) override
    {
        return _platformUiContext->ShowDirectoryDialog(_window, title);
    }

    IWindowManager * GetWindowManager() override
    {
        return _windowManager;
    }

    bool ReadBMP(void * * outPixels, uint32 * outWidth, uint32 * outHeight, const std::string &path) override
    {
        auto bitmap = SDL_LoadBMP(path.c_str());
        if (bitmap != nullptr)
        {
            sint32 numChannels = bitmap->format->BytesPerPixel;
            if (numChannels < 3 || bitmap->format->BitsPerPixel < 24)
            {
                window_error_open(STR_HEIGHT_MAP_ERROR, STR_ERROR_24_BIT_BITMAP);
                SDL_FreeSurface(bitmap);
                return false;
            }

            // Copy pixels over, then discard the surface
            *outPixels = nullptr;
            *outWidth = bitmap->w;
            *outHeight = bitmap->h;
            if (SDL_LockSurface(bitmap) == 0)
            {
                *outPixels = malloc(bitmap->w * bitmap->h * 4);
                memset(*outPixels, 0xFF, bitmap->w * bitmap->h);

                auto src = (const uint8 *)bitmap->pixels;
                auto dst = (uint8 *)*outPixels;
                if (numChannels == 4)
                {
                    for (sint32 y = 0; y < bitmap->h; y++)
                    {
                        memcpy(dst, src, bitmap->w);
                        src += bitmap->pitch;
                        dst += bitmap->w;
                    }
                }
                else
                {
                    for (sint32 y = 0; y < bitmap->h; y++)
                    {
                        for (sint32 x = 0; x < bitmap->w; x++)
                        {
                            memcpy(dst, src, 3);
                            src += 3;
                            dst += 4;
                        }
                        src += bitmap->pitch - (bitmap->w * 3);
                    }
                }
                SDL_UnlockSurface(bitmap);
            }
            else
            {
                return false;
            }
            SDL_FreeSurface(bitmap);
            return true;
        }
        else
        {
            log_warning("Failed to load bitmap: %s", SDL_GetError());
            window_error_open(STR_HEIGHT_MAP_ERROR, STR_ERROR_READING_BITMAP);
            return false;
        }
    }

    bool SetClipboardText(const utf8* target) override
    {
        return (SDL_SetClipboardText(target) == 0);
    }


private:
    void CreateWindow(sint32 x, sint32 y)
    {
        // Get saved window size
        sint32 width = gConfigGeneral.window_width;
        sint32 height = gConfigGeneral.window_height;
        if (width <= 0) width = 640;
        if (height <= 0) height = 480;

        // Create window in window first rather than fullscreen so we have the display the window is on first
        uint32 flags = SDL_WINDOW_RESIZABLE;
        if (gConfigGeneral.drawing_engine == DRAWING_ENGINE_OPENGL)
        {
            flags |= SDL_WINDOW_OPENGL;
        }

        _window = SDL_CreateWindow(OPENRCT2_NAME, x, y, width, height, flags);
        if (_window == nullptr)
        {
            SDLException::Throw("SDL_CreateWindow(...)");
        }

        SDL_SetWindowMinimumSize(_window, 720, 480);
        SetCursorTrap(gConfigGeneral.trap_cursor);
        _platformUiContext->SetWindowIcon(_window);

        // Initialise the surface, palette and draw buffer
        drawing_engine_init();
        OnResize(width, height);

        UpdateFullscreenResolutions();
        SetFullscreenMode((FULLSCREEN_MODE)gConfigGeneral.fullscreen_mode);

        TriggerResize();
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
        sint32 nonWindowFlags =
#ifndef __MACOSX__
            SDL_WINDOW_MAXIMIZED |
#endif
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

    void UpdateFullscreenResolutions()
    {
        // Query number of display modes
        sint32 displayIndex = SDL_GetWindowDisplayIndex(_window);
        sint32 numDisplayModes = SDL_GetNumDisplayModes(displayIndex);

        // Get desktop aspect ratio
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(displayIndex, &mode);

        // Get resolutions
        auto resolutions = std::vector<Resolution>();
        float desktopAspectRatio = (float)mode.w / mode.h;
        for (sint32 i = 0; i < numDisplayModes; i++)
        {
            SDL_GetDisplayMode(displayIndex, i, &mode);
            if (mode.w > 0 && mode.h > 0)
            {
                float aspectRatio = (float)mode.w / mode.h;
                if (_resolutionsAllowAnyAspectRatio || std::fabs(desktopAspectRatio - aspectRatio) < 0.0001f)
                {
                    resolutions.push_back({ mode.w, mode.h });
                }
            }
        }

        // Sort by area
        std::sort(resolutions.begin(), resolutions.end(),
            [](const Resolution &a, const Resolution &b) -> bool
            {
                sint32 areaA = a.Width * a.Height;
                sint32 areaB = b.Width * b.Height;
                return areaA < areaB;
            });

        // Remove duplicates
        auto last = std::unique(resolutions.begin(), resolutions.end(),
            [](const Resolution &a, const Resolution &b) -> bool
            {
                return (a.Width == b.Width && a.Height == b.Height);
            });
        resolutions.erase(last, resolutions.end());

        // Update config fullscreen resolution if not set
        if (gConfigGeneral.fullscreen_width == -1 || gConfigGeneral.fullscreen_height == -1)
        {
            gConfigGeneral.fullscreen_width = resolutions.back().Width;
            gConfigGeneral.fullscreen_height = resolutions.back().Height;
        }

        _fsResolutions = resolutions;
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

    uint32 GetWindowFlags()
    {
        return SDL_GetWindowFlags(_window);
    }
};

IUiContext * OpenRCT2::Ui::CreateUiContext(IPlatformEnvironment * env)
{
    return new UiContext(env);
}
