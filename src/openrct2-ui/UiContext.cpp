/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include <openrct2/audio/AudioMixer.h>
#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2/core/Math.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/platform/Platform2.h>
#include <openrct2/title/TitleSequencePlayer.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/Version.h>
#include "CursorRepository.h"
#include "drawing/engines/DrawingEngineFactory.hpp"
#include "input/KeyboardShortcuts.h"
#include "interface/Theme.h"
#include "SDLException.h"
#include "TextComposition.h"
#include "UiContext.h"
#include "WindowManager.h"

#include <openrct2/Input.h>
#include <openrct2/interface/InteractiveConsole.h>
#include <openrct2-ui/interface/Window.h>

#include "interface/InGameConsole.h"
#include "title/TitleSequencePlayer.h"

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
    constexpr static uint32_t TOUCH_DOUBLE_TIMEOUT = 300;

    IPlatformUiContext * const      _platformUiContext;
    IWindowManager * const          _windowManager;

    CursorRepository _cursorRepository;

    SDL_Window *    _window = nullptr;
    int32_t          _width  = 0;
    int32_t          _height = 0;
    int32_t          _scaleQuality = 0;

    bool _resolutionsAllowAnyAspectRatio = false;
    std::vector<Resolution> _fsResolutions;

    bool _steamOverlayActive = false;

    // Input
    KeyboardShortcuts   _keyboardShortcuts;
    TextComposition     _textComposition;
    CursorState         _cursorState            = {};
    uint32_t              _lastKeyPressed         = 0;
    const uint8_t *       _keysState              = nullptr;
    uint8_t               _keysPressed[256]       = {};
    uint32_t              _lastGestureTimestamp   = 0;
    float               _gestureRadius          = 0;

    InGameConsole       _inGameConsole;
    std::unique_ptr<ITitleSequencePlayer> _titleSequencePlayer;

public:
    InGameConsole& GetInGameConsole() { return _inGameConsole; }

    explicit UiContext(const std::shared_ptr<IPlatformEnvironment>& env)
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

    void Update() override
    {
        _inGameConsole.Update();
    }

    void Draw(rct_drawpixelinfo * dpi) override
    {
        auto bgColour = theme_get_colour(WC_CHAT, 0);
        chat_draw(dpi, bgColour);
        _inGameConsole.Draw(dpi);
    }

    // Window
    void * GetWindow() override
    {
        return _window;
    }

    int32_t GetWidth() override
    {
        return _width;
    }

    int32_t GetHeight() override
    {
        return _height;
    }

    int32_t GetScaleQuality() override
    {
        return _scaleQuality;
    }

    void SetFullscreenMode(FULLSCREEN_MODE mode) override
    {
        static constexpr const int32_t SDLFSFlags[] = { 0, SDL_WINDOW_FULLSCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP };
        uint32_t windowFlags = SDLFSFlags[(int32_t)mode];

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
        uint32_t windowFlags = GetWindowFlags();
        return (windowFlags & SDL_WINDOW_INPUT_FOCUS) != 0;
    }

    bool IsMinimised() override
    {
        uint32_t windowFlags = GetWindowFlags();
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

    const uint8_t * GetKeysState() override
    {
        return _keysState;
    }

    const uint8_t * GetKeysPressed() override
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

    void SetCursorScale(uint8_t scale) override
    {
        _cursorRepository.SetCursorScale(scale);
    }

    void SetCursorVisible(bool value) override
    {
        SDL_ShowCursor(value ? SDL_ENABLE : SDL_DISABLE);
    }

    void GetCursorPosition(int32_t * x, int32_t * y) override
    {
        SDL_GetMouseState(x, y);
    }

    void SetCursorPosition(int32_t x, int32_t y) override
    {
        SDL_WarpMouseInWindow(nullptr, x, y);
    }

    void SetCursorTrap(bool value) override
    {
        SDL_SetWindowGrab(_window, value ? SDL_TRUE : SDL_FALSE);
    }

    void SetKeysPressed(uint32_t keysym, uint8_t scancode) override
    {
        _lastKeyPressed = keysym;
        _keysPressed[scancode] = 1;
    }

    // Drawing
    std::shared_ptr<Drawing::IDrawingEngineFactory> GetDrawingEngineFactory() override
    {
        return std::make_shared<DrawingEngineFactory>();
    }

    void DrawRainAnimation(IRainDrawer* rainDrawer, rct_drawpixelinfo* dpi, DrawRainFunc drawFunc) override
    {
        int32_t left = dpi->x;
        int32_t right = left + dpi->width;
        int32_t top = dpi->y;
        int32_t bottom = top + dpi->height;

        for (auto& w : g_window_list)
        {
            DrawRainWindow(rainDrawer, w.get(), left, right, top, bottom, drawFunc);
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
                    int32_t displayIndex = SDL_GetWindowDisplayIndex(_window);
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
                _cursorState.x = (int32_t)(e.motion.x / gConfigGeneral.window_scale);
                _cursorState.y = (int32_t)(e.motion.y / gConfigGeneral.window_scale);
                break;
            case SDL_MOUSEWHEEL:
                if (_inGameConsole.IsOpen())
                {
                    _inGameConsole.Scroll(e.wheel.y * 3); // Scroll 3 lines at a time
                    break;
                }
                _cursorState.wheel -= e.wheel.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
            {
                int32_t x = (int32_t)(e.button.x / gConfigGeneral.window_scale);
                int32_t y = (int32_t)(e.button.y / gConfigGeneral.window_scale);
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
                int32_t x = (int32_t)(e.button.x / gConfigGeneral.window_scale);
                int32_t y = (int32_t)(e.button.y / gConfigGeneral.window_scale);
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
                _cursorState.x = (int32_t)(e.tfinger.x * _width);
                _cursorState.y = (int32_t)(e.tfinger.y * _height);
                break;
            case SDL_FINGERDOWN:
            {
                int32_t x = (int32_t)(e.tfinger.x * _width);
                int32_t y = (int32_t)(e.tfinger.y * _height);

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
                int32_t x = (int32_t)(e.tfinger.x * _width);
                int32_t y = (int32_t)(e.tfinger.y * _height);

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
                    constexpr int32_t tolerance = 128;
                    int32_t gesturePixels = (int32_t)(_gestureRadius * _width);
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
        int32_t numKeys = 256;
        _keysState = SDL_GetKeyboardState(&numKeys);
    }

    /**
     * Helper function to set various render target features.
     * Does not get triggered on resize, but rather manually on config changes.
     */
    void TriggerResize() override
    {
        char scaleQualityBuffer[4];
        _scaleQuality = gConfigGeneral.scale_quality;
        if (gConfigGeneral.window_scale == std::floor(gConfigGeneral.window_scale))
        {
            _scaleQuality = SCALE_QUALITY_NN;
        }

        int32_t scaleQuality = _scaleQuality;
        if (_scaleQuality == SCALE_QUALITY_SMOOTH_NN)
        {
            scaleQuality = SCALE_QUALITY_LINEAR;
        }
        snprintf(scaleQualityBuffer, sizeof(scaleQualityBuffer), "%u", scaleQuality);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scaleQualityBuffer);

        int32_t width, height;
        SDL_GetWindowSize(_window, &width, &height);
        OnResize(width, height);
    }

    void CreateWindow() override
    {
        SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, gConfigGeneral.minimize_fullscreen_focus_loss ? "1" : "0");

        // Set window position to default display
        int32_t defaultDisplay = Math::Clamp(0, gConfigGeneral.default_display, 0xFFFF);
        int32_t x = SDL_WINDOWPOS_UNDEFINED_DISPLAY(defaultDisplay);
        int32_t y = SDL_WINDOWPOS_UNDEFINED_DISPLAY(defaultDisplay);

        CreateWindow(x, y);

        // Check if steam overlay renderer is loaded into the process
        _steamOverlayActive = _platformUiContext->IsSteamOverlayAttached();
    }

    void CloseWindow() override
    {
        drawing_engine_dispose();
        if (_window != nullptr)
        {
            SDL_DestroyWindow(_window);
            _window = nullptr;
        }
    }

    void RecreateWindow() override
    {
        // Use the position of the current window for the new window
        int32_t x, y;
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

    bool SetClipboardText(const utf8* target) override
    {
        return (SDL_SetClipboardText(target) == 0);
    }

    ITitleSequencePlayer * GetTitleSequencePlayer() override
    {
        if (_titleSequencePlayer == nullptr)
        {
            auto context = GetContext();
            auto scenarioRepository = context->GetScenarioRepository();
            auto gameState = context->GetGameState();
            _titleSequencePlayer = CreateTitleSequencePlayer(*scenarioRepository, *gameState);
        }
        return _titleSequencePlayer.get();
    }

private:
    void CreateWindow(int32_t x, int32_t y)
    {
        // Get saved window size
        int32_t width = gConfigGeneral.window_width;
        int32_t height = gConfigGeneral.window_height;
        if (width <= 0) width = 640;
        if (height <= 0) height = 480;

        // Create window in window first rather than fullscreen so we have the display the window is on first
        uint32_t flags = SDL_WINDOW_RESIZABLE;
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

    void OnResize(int32_t width, int32_t height)
    {
        // Scale the native window size to the game's canvas size
        _width = (int32_t)(width / gConfigGeneral.window_scale);
        _height = (int32_t)(height / gConfigGeneral.window_scale);

        drawing_engine_resize();

        uint32_t flags = SDL_GetWindowFlags(_window);
        if ((flags & SDL_WINDOW_MINIMIZED) == 0)
        {
            window_resize_gui(_width, _height);
            window_relocate_windows(_width, _height);
        }

        gfx_invalidate_screen();

        // Check if the window has been resized in windowed mode and update the config file accordingly
        int32_t nonWindowFlags =
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
        int32_t displayIndex = SDL_GetWindowDisplayIndex(_window);
        int32_t numDisplayModes = SDL_GetNumDisplayModes(displayIndex);

        // Get desktop aspect ratio
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(displayIndex, &mode);

        // Get resolutions
        auto resolutions = std::vector<Resolution>();
        float desktopAspectRatio = (float)mode.w / mode.h;
        for (int32_t i = 0; i < numDisplayModes; i++)
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
                int32_t areaA = a.Width * a.Height;
                int32_t areaB = b.Width * b.Height;
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

    Resolution GetClosestResolution(int32_t inWidth, int32_t inHeight)
    {
        Resolution result = { 640, 480 };
        int32_t closestAreaDiff = -1;
        int32_t destinationArea = inWidth * inHeight;
        for (const Resolution &resolution : _fsResolutions)
        {
            // Check if exact match
            if (resolution.Width == inWidth && resolution.Height == inHeight)
            {
                result = resolution;
                break;
            }

            // Check if area is closer to best match
            int32_t areaDiff = std::abs((resolution.Width * resolution.Height) - destinationArea);
            if (closestAreaDiff == -1 || areaDiff < closestAreaDiff)
            {
                closestAreaDiff = areaDiff;
                result = resolution;
            }
        }
        return result;
    }

    uint32_t GetWindowFlags()
    {
        return SDL_GetWindowFlags(_window);
    }

    static void DrawRainWindow(
        IRainDrawer * rainDrawer,
        rct_window * original_w,
        int16_t left,
        int16_t right,
        int16_t top,
        int16_t bottom,
        DrawRainFunc drawFunc)
    {
        rct_window * w{};
        for (auto i = window_get_index(original_w) + 1; ; i++)
        {
            if (i >= g_window_list.size())
            {
                // Loop ended, draw rain for original_w
                auto vp = original_w->viewport;
                if (vp != nullptr)
                {
                    left = std::max<int16_t>(left, vp->x);
                    right = std::min<int16_t>(right, vp->x + vp->width);
                    top = std::max<int16_t>(top, vp->y);
                    bottom = std::min<int16_t>(bottom, vp->y + vp->height);
                    if (left < right && top < bottom)
                    {
                        auto width = right - left;
                        auto height = bottom - top;
                        drawFunc(rainDrawer, left, top, width, height);
                    }
                }
                return;
            }

            w = g_window_list[i].get();
            if (right <= w->x || bottom <= w->y)
            {
                continue;
            }

            if (RCT_WINDOW_RIGHT(w) <= left || RCT_WINDOW_BOTTOM(w) <= top)
            {
                continue;
            }

            if (left >= w->x)
            {
                break;
            }

            DrawRainWindow(rainDrawer, original_w, left, w->x, top, bottom, drawFunc);

            left = w->x;
            DrawRainWindow(rainDrawer, original_w, left, right, top, bottom, drawFunc);
            return;
        }

        int16_t w_right = RCT_WINDOW_RIGHT(w);
        if (right > w_right) {
            DrawRainWindow(rainDrawer, original_w, left, w_right, top, bottom, drawFunc);

            left = w_right;
            DrawRainWindow(rainDrawer, original_w, left, right, top, bottom, drawFunc);
            return;
        }

        if (top < w->y) {
            DrawRainWindow(rainDrawer, original_w, left, right, top, w->y, drawFunc);

            top = w->y;
            DrawRainWindow(rainDrawer, original_w, left, right, top, bottom, drawFunc);
            return;
        }

        int16_t w_bottom = RCT_WINDOW_BOTTOM(w);
        if (bottom > w_bottom)
        {
            DrawRainWindow(rainDrawer, original_w, left, right, top, w_bottom, drawFunc);

            top = w_bottom;
            DrawRainWindow(rainDrawer, original_w, left, right, top, bottom, drawFunc);
            return;
        }
    }
};

std::unique_ptr<IUiContext> OpenRCT2::Ui::CreateUiContext(const std::shared_ptr<IPlatformEnvironment>& env)
{
    return std::make_unique<UiContext>(env);
}

InGameConsole& OpenRCT2::Ui::GetInGameConsole()
{
    auto uiContext = std::static_pointer_cast<UiContext>(GetContext()->GetUiContext());
    return uiContext->GetInGameConsole();
}
