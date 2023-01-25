/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "UiContext.h"

#include "CursorRepository.h"
#include "SDLException.h"
#include "TextComposition.h"
#include "WindowManager.h"
#include "drawing/engines/DrawingEngineFactory.hpp"
#include "input/ShortcutManager.h"
#include "interface/InGameConsole.h"
#include "interface/Theme.h"
#include "scripting/UiExtensions.h"
#include "title/TitleSequencePlayer.h"

#include <SDL.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/Version.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/audio/AudioMixer.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/InteractiveConsole.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/scripting/ScriptEngine.h>
#include <openrct2/title/TitleSequencePlayer.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Location.hpp>
#include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Scripting;
using namespace OpenRCT2::Ui;

#ifdef __MACOSX__
// macOS uses COMMAND rather than CTRL for many keyboard shortcuts
#    define KEYBOARD_PRIMARY_MODIFIER KMOD_GUI
#else
#    define KEYBOARD_PRIMARY_MODIFIER KMOD_CTRL
#endif

class UiContext final : public IUiContext
{
private:
    constexpr static uint32_t TOUCH_DOUBLE_TIMEOUT = 300;

    const std::unique_ptr<IPlatformUiContext> _platformUiContext;
    const std::unique_ptr<IWindowManager> _windowManager;

    CursorRepository _cursorRepository;

    SDL_Window* _window = nullptr;
    int32_t _width = 0;
    int32_t _height = 0;
    ScaleQuality _scaleQuality = ScaleQuality::NearestNeighbour;

    std::vector<Resolution> _fsResolutions;

    bool _steamOverlayActive = false;

    // Input
    InputManager _inputManager;
    ShortcutManager _shortcutManager;
    TextComposition _textComposition;
    CursorState _cursorState = {};
    uint32_t _lastKeyPressed = 0;
    const uint8_t* _keysState = nullptr;
    uint8_t _keysPressed[256] = {};
    uint32_t _lastGestureTimestamp = 0;
    float _gestureRadius = 0;

    InGameConsole _inGameConsole;
    std::unique_ptr<ITitleSequencePlayer> _titleSequencePlayer;

public:
    InGameConsole& GetInGameConsole()
    {
        return _inGameConsole;
    }

    InputManager& GetInputManager()
    {
        return _inputManager;
    }

    ShortcutManager& GetShortcutManager()
    {
        return _shortcutManager;
    }

    explicit UiContext(const std::shared_ptr<IPlatformEnvironment>& env)
        : _platformUiContext(CreatePlatformUiContext())
        , _windowManager(CreateWindowManager())
        , _shortcutManager(env)
    {
        LogSDLVersion();
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
        {
            SDLException::Throw("SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)");
        }
        _cursorRepository.LoadCursors();
        _shortcutManager.LoadUserBindings();
    }

    ~UiContext() override
    {
        UiContext::CloseWindow();
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    void Initialise() override
    {
#ifdef ENABLE_SCRIPTING
        auto& scriptEngine = GetContext()->GetScriptEngine();
        UiScriptExtensions::Extend(scriptEngine);
#endif
    }

    void Tick() override
    {
        _inGameConsole.Update();
    }

    void Draw(DrawPixelInfo* dpi) override
    {
        auto bgColour = ThemeGetColour(WindowClass::Chat, 0);
        ChatDraw(dpi, bgColour);
        _inGameConsole.Draw(dpi);
    }

    // Window
    void* GetWindow() override
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

    ScaleQuality GetScaleQuality() override
    {
        return _scaleQuality;
    }

    void SetFullscreenMode(FULLSCREEN_MODE mode) override
    {
        static constexpr const int32_t _sdlFullscreenFlags[] = {
            0,
            SDL_WINDOW_FULLSCREEN,
            SDL_WINDOW_FULLSCREEN_DESKTOP,
        };
        uint32_t windowFlags = _sdlFullscreenFlags[static_cast<int32_t>(mode)];

        // HACK Changing window size when in fullscreen usually has no effect
        if (mode == FULLSCREEN_MODE::FULLSCREEN)
        {
            SDL_SetWindowFullscreen(_window, 0);

            // Set window size
            UpdateFullscreenResolutions();
            Resolution resolution = GetClosestResolution(gConfigGeneral.FullscreenWidth, gConfigGeneral.FullscreenHeight);
            SDL_SetWindowSize(_window, resolution.Width, resolution.Height);
        }
        else if (mode == FULLSCREEN_MODE::WINDOWED)
        {
            SDL_SetWindowSize(_window, gConfigGeneral.WindowWidth, gConfigGeneral.WindowHeight);
        }

        if (SDL_SetWindowFullscreen(_window, windowFlags))
        {
            LOG_FATAL("SDL_SetWindowFullscreen %s", SDL_GetError());
            exit(1);

            // TODO try another display mode rather than just exiting the game
        }
    }

    const std::vector<Resolution>& GetFullscreenResolutions() override
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
        return (windowFlags & SDL_WINDOW_MINIMIZED) || (windowFlags & SDL_WINDOW_HIDDEN);
    }

    bool IsSteamOverlayActive() override
    {
        return _steamOverlayActive;
    }

    // Input
    const CursorState* GetCursorState() override
    {
        return &_cursorState;
    }

    const uint8_t* GetKeysState() override
    {
        return _keysState;
    }

    const uint8_t* GetKeysPressed() override
    {
        return _keysPressed;
    }

    CursorID GetCursor() override
    {
        return _cursorRepository.GetCurrentCursor();
    }

    void SetCursor(CursorID cursor) override
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

    ScreenCoordsXY GetCursorPosition() override
    {
        ScreenCoordsXY cursorPosition;
        SDL_GetMouseState(&cursorPosition.x, &cursorPosition.y);
        return cursorPosition;
    }

    void SetCursorPosition(const ScreenCoordsXY& cursorPosition) override
    {
        SDL_WarpMouseInWindow(nullptr, cursorPosition.x, cursorPosition.y);
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

    void DrawWeatherAnimation(IWeatherDrawer* weatherDrawer, DrawPixelInfo* dpi, DrawWeatherFunc drawFunc) override
    {
        int32_t left = dpi->x;
        int32_t right = left + dpi->width;
        int32_t top = dpi->y;
        int32_t bottom = top + dpi->height;

        for (auto& w : g_window_list)
        {
            DrawWeatherWindow(dpi, weatherDrawer, w.get(), left, right, top, bottom, drawFunc);
        }
    }

    // Text input
    bool IsTextInputActive() override
    {
        return _textComposition.IsActive();
    }

    TextInputSession* StartTextInput(utf8* buffer, size_t bufferSize) override
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

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    ContextQuit();
                    break;
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        OnResize(e.window.data1, e.window.data2);
                    }

                    switch (e.window.event)
                    {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                        case SDL_WINDOWEVENT_MOVED:
                        case SDL_WINDOWEVENT_MAXIMIZED:
                        case SDL_WINDOWEVENT_RESTORED:
                        {
                            // Update default display index
                            int32_t displayIndex = SDL_GetWindowDisplayIndex(_window);
                            if (displayIndex != gConfigGeneral.DefaultDisplay)
                            {
                                gConfigGeneral.DefaultDisplay = displayIndex;
                                ConfigSaveDefault();
                            }
                            break;
                        }
                    }

                    if (gConfigSound.audio_focus)
                    {
                        if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                        {
                            SetAudioVolume(1);
                        }
                        if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                        {
                            SetAudioVolume(0);
                        }
                    }
                    break;
                case SDL_MOUSEMOTION:
                    _cursorState.position = { static_cast<int32_t>(e.motion.x / gConfigGeneral.WindowScale),
                                              static_cast<int32_t>(e.motion.y / gConfigGeneral.WindowScale) };
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
                    if (e.button.which == SDL_TOUCH_MOUSEID)
                    {
                        break;
                    }
                    ScreenCoordsXY mousePos = { static_cast<int32_t>(e.button.x / gConfigGeneral.WindowScale),
                                                static_cast<int32_t>(e.button.y / gConfigGeneral.WindowScale) };
                    switch (e.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            StoreMouseInput(MouseState::LeftPress, mousePos);
                            _cursorState.left = CURSOR_PRESSED;
                            _cursorState.old = 1;
                            break;
                        case SDL_BUTTON_MIDDLE:
                            _cursorState.middle = CURSOR_PRESSED;
                            break;
                        case SDL_BUTTON_RIGHT:
                            StoreMouseInput(MouseState::RightPress, mousePos);
                            _cursorState.right = CURSOR_PRESSED;
                            _cursorState.old = 2;
                            break;
                    }
                    _cursorState.touch = false;

                    {
                        InputEvent ie;
                        ie.DeviceKind = InputDeviceKind::Mouse;
                        ie.Modifiers = SDL_GetModState();
                        ie.Button = e.button.button;
                        ie.State = InputEventState::Down;
                        _inputManager.QueueInputEvent(std::move(ie));
                    }
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    if (e.button.which == SDL_TOUCH_MOUSEID)
                    {
                        break;
                    }
                    ScreenCoordsXY mousePos = { static_cast<int32_t>(e.button.x / gConfigGeneral.WindowScale),
                                                static_cast<int32_t>(e.button.y / gConfigGeneral.WindowScale) };
                    switch (e.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            StoreMouseInput(MouseState::LeftRelease, mousePos);
                            _cursorState.left = CURSOR_RELEASED;
                            _cursorState.old = 3;
                            break;
                        case SDL_BUTTON_MIDDLE:
                            _cursorState.middle = CURSOR_RELEASED;
                            break;
                        case SDL_BUTTON_RIGHT:
                            StoreMouseInput(MouseState::RightRelease, mousePos);
                            _cursorState.right = CURSOR_RELEASED;
                            _cursorState.old = 4;
                            break;
                    }
                    _cursorState.touch = false;

                    {
                        InputEvent ie;
                        ie.DeviceKind = InputDeviceKind::Mouse;
                        ie.Modifiers = SDL_GetModState();
                        ie.Button = e.button.button;
                        ie.State = InputEventState::Release;
                        _inputManager.QueueInputEvent(std::move(ie));
                    }
                    break;
                }
                // Apple sends touchscreen events for trackpads, so ignore these events on macOS
#ifndef __MACOSX__
                case SDL_FINGERMOTION:
                    _cursorState.position = { static_cast<int32_t>(e.tfinger.x * _width),
                                              static_cast<int32_t>(e.tfinger.y * _height) };
                    break;
                case SDL_FINGERDOWN:
                {
                    ScreenCoordsXY fingerPos = { static_cast<int32_t>(e.tfinger.x * _width),
                                                 static_cast<int32_t>(e.tfinger.y * _height) };

                    _cursorState.touchIsDouble
                        = (!_cursorState.touchIsDouble
                           && e.tfinger.timestamp - _cursorState.touchDownTimestamp < TOUCH_DOUBLE_TIMEOUT);

                    if (_cursorState.touchIsDouble)
                    {
                        StoreMouseInput(MouseState::RightPress, fingerPos);
                        _cursorState.right = CURSOR_PRESSED;
                        _cursorState.old = 2;
                    }
                    else
                    {
                        StoreMouseInput(MouseState::LeftPress, fingerPos);
                        _cursorState.left = CURSOR_PRESSED;
                        _cursorState.old = 1;
                    }
                    _cursorState.touch = true;
                    _cursorState.touchDownTimestamp = e.tfinger.timestamp;
                    break;
                }
                case SDL_FINGERUP:
                {
                    ScreenCoordsXY fingerPos = { static_cast<int32_t>(e.tfinger.x * _width),
                                                 static_cast<int32_t>(e.tfinger.y * _height) };

                    if (_cursorState.touchIsDouble)
                    {
                        StoreMouseInput(MouseState::RightRelease, fingerPos);
                        _cursorState.right = CURSOR_RELEASED;
                        _cursorState.old = 4;
                    }
                    else
                    {
                        StoreMouseInput(MouseState::LeftRelease, fingerPos);
                        _cursorState.left = CURSOR_RELEASED;
                        _cursorState.old = 3;
                    }
                    _cursorState.touch = true;
                    break;
                }
#endif
                case SDL_KEYDOWN:
                {
#ifndef __MACOSX__
                    // Ignore winkey keydowns. Handles edge case where tiling
                    // window managers don't eat the keypresses when changing
                    // workspaces.
                    if (SDL_GetModState() & KMOD_GUI)
                    {
                        break;
                    }
#endif
                    _textComposition.HandleMessage(&e);
                    auto ie = GetInputEventFromSDLEvent(e);
                    ie.State = InputEventState::Down;
                    _inputManager.QueueInputEvent(std::move(ie));
                    break;
                }
                case SDL_KEYUP:
                {
                    auto ie = GetInputEventFromSDLEvent(e);
                    ie.State = InputEventState::Release;
                    _inputManager.QueueInputEvent(std::move(ie));
                    break;
                }
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
                        int32_t gesturePixels = static_cast<int32_t>(_gestureRadius * _width);
                        if (abs(gesturePixels) > tolerance)
                        {
                            _gestureRadius = 0;
                            MainWindowZoom(gesturePixels > 0, true);
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
                {
                    _inputManager.QueueInputEvent(e);
                    break;
                }
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
        _scaleQuality = ScaleQuality::SmoothNearestNeighbour;
        if (gConfigGeneral.WindowScale == std::floor(gConfigGeneral.WindowScale))
        {
            _scaleQuality = ScaleQuality::NearestNeighbour;
        }

        ScaleQuality scaleQuality = _scaleQuality;
        if (_scaleQuality == ScaleQuality::SmoothNearestNeighbour)
        {
            scaleQuality = ScaleQuality::Linear;
        }
        snprintf(scaleQualityBuffer, sizeof(scaleQualityBuffer), "%d", static_cast<int32_t>(scaleQuality));
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scaleQualityBuffer);

        int32_t width, height;
        SDL_GetWindowSize(_window, &width, &height);
        OnResize(width, height);
    }

    void CreateWindow() override
    {
        SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, gConfigGeneral.MinimizeFullscreenFocusLoss ? "1" : "0");

        // Set window position to default display
        int32_t defaultDisplay = std::clamp(gConfigGeneral.DefaultDisplay, 0, 0xFFFF);
        auto windowPos = ScreenCoordsXY{ static_cast<int32_t>(SDL_WINDOWPOS_UNDEFINED_DISPLAY(defaultDisplay)),
                                         static_cast<int32_t>(SDL_WINDOWPOS_UNDEFINED_DISPLAY(defaultDisplay)) };

        CreateWindow(windowPos);

        // Check if steam overlay renderer is loaded into the process
        _steamOverlayActive = _platformUiContext->IsSteamOverlayAttached();
    }

    void CloseWindow() override
    {
        DrawingEngineDispose();
        if (_window != nullptr)
        {
            SDL_DestroyWindow(_window);
            _window = nullptr;
        }
    }

    void RecreateWindow() override
    {
        // Use the position of the current window for the new window
        ScreenCoordsXY windowPos;
        SDL_SetWindowFullscreen(_window, 0);
        SDL_GetWindowPosition(_window, &windowPos.x, &windowPos.y);

        CloseWindow();
        CreateWindow(windowPos);
    }

    void ShowMessageBox(const std::string& message) override
    {
        _platformUiContext->ShowMessageBox(_window, message);
    }

    int32_t ShowMessageBox(
        const std::string& title, const std::string& message, const std::vector<std::string>& options) override
    {
        auto message_box_button_data = std::make_unique<SDL_MessageBoxButtonData[]>(options.size());
        for (size_t i = 0; i < options.size(); i++)
        {
            message_box_button_data[i].buttonid = static_cast<int>(i);
            message_box_button_data[i].text = options[i].c_str();
        }

        SDL_MessageBoxData message_box_data{};
        message_box_data.window = _window;
        message_box_data.title = title.c_str();
        message_box_data.message = message.c_str();
        message_box_data.numbuttons = static_cast<int>(options.size());
        message_box_data.buttons = message_box_button_data.get();

        int buttonid{};

        SDL_ShowMessageBox(&message_box_data, &buttonid);

        return buttonid;
    }

    bool HasMenuSupport() override
    {
        return _platformUiContext->HasMenuSupport();
    }

    int32_t ShowMenuDialog(const std::vector<std::string>& options, const std::string& title, const std::string& text) override
    {
        return _platformUiContext->ShowMenuDialog(options, title, text);
    }

    void OpenFolder(const std::string& path) override
    {
        _platformUiContext->OpenFolder(path);
    }

    void OpenURL(const std::string& url) override
    {
        _platformUiContext->OpenURL(url);
    }

    std::string ShowFileDialog(const FileDialogDesc& desc) override
    {
        return _platformUiContext->ShowFileDialog(_window, desc);
    }

    std::string ShowDirectoryDialog(const std::string& title) override
    {
        return _platformUiContext->ShowDirectoryDialog(_window, title);
    }

    bool HasFilePicker() const override
    {
        return _platformUiContext->HasFilePicker();
    }

    IWindowManager* GetWindowManager() override
    {
        return _windowManager.get();
    }

    bool SetClipboardText(const utf8* target) override
    {
        return (SDL_SetClipboardText(target) == 0);
    }

    ITitleSequencePlayer* GetTitleSequencePlayer() override
    {
        if (_titleSequencePlayer == nullptr)
        {
            auto context = GetContext();
            auto gameState = context->GetGameState();
            _titleSequencePlayer = OpenRCT2::Title::CreateTitleSequencePlayer(*gameState);
        }
        return _titleSequencePlayer.get();
    }

private:
    void LogSDLVersion()
    {
        SDL_version version{};
        SDL_GetVersion(&version);
        LOG_VERBOSE("SDL2 version: %d.%d.%d", version.major, version.minor, version.patch);
    }

    void CreateWindow(const ScreenCoordsXY& windowPos)
    {
        // Get saved window size
        int32_t width = gConfigGeneral.WindowWidth;
        int32_t height = gConfigGeneral.WindowHeight;
        if (width <= 0)
            width = 640;
        if (height <= 0)
            height = 480;

        // Create window in window first rather than fullscreen so we have the display the window is on first
        uint32_t flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
        if (gConfigGeneral.DrawingEngine == DrawingEngine::OpenGL)
        {
            flags |= SDL_WINDOW_OPENGL;
        }

        _window = SDL_CreateWindow(OPENRCT2_NAME, windowPos.x, windowPos.y, width, height, flags);
        if (_window == nullptr)
        {
            SDLException::Throw("SDL_CreateWindow(...)");
        }

        ApplyScreenSaverLockSetting();

        SDL_SetWindowMinimumSize(_window, 720, 480);
        SetCursorTrap(gConfigGeneral.TrapCursor);
        _platformUiContext->SetWindowIcon(_window);

        // Initialise the surface, palette and draw buffer
        DrawingEngineInit();
        OnResize(width, height);

        UpdateFullscreenResolutions();

        // Fix #4022: Force Mac to windowed to avoid cursor offset on launch issue
#ifdef __MACOSX__
        gConfigGeneral.FullscreenMode = static_cast<int32_t>(OpenRCT2::Ui::FULLSCREEN_MODE::WINDOWED);
#else
        SetFullscreenMode(static_cast<FULLSCREEN_MODE>(gConfigGeneral.FullscreenMode));
#endif
        TriggerResize();
    }

    void OnResize(int32_t width, int32_t height)
    {
        // Scale the native window size to the game's canvas size
        _width = static_cast<int32_t>(width / gConfigGeneral.WindowScale);
        _height = static_cast<int32_t>(height / gConfigGeneral.WindowScale);

        DrawingEngineResize();

        uint32_t flags = SDL_GetWindowFlags(_window);
        if ((flags & SDL_WINDOW_MINIMIZED) == 0)
        {
            WindowResizeGui(_width, _height);
            WindowRelocateWindows(_width, _height);
        }

        GfxInvalidateScreen();

        // Check if the window has been resized in windowed mode and update the config file accordingly
        int32_t nonWindowFlags =
#ifndef __MACOSX__
            SDL_WINDOW_MAXIMIZED |
#endif
            SDL_WINDOW_MINIMIZED | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP;

        if (!(flags & nonWindowFlags))
        {
            if (width != gConfigGeneral.WindowWidth || height != gConfigGeneral.WindowHeight)
            {
                gConfigGeneral.WindowWidth = width;
                gConfigGeneral.WindowHeight = height;
                ConfigSaveDefault();
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
        float desktopAspectRatio = static_cast<float>(mode.w) / mode.h;
        for (int32_t i = 0; i < numDisplayModes; i++)
        {
            SDL_GetDisplayMode(displayIndex, i, &mode);
            if (mode.w > 0 && mode.h > 0)
            {
                float aspectRatio = static_cast<float>(mode.w) / mode.h;
                if (std::fabs(desktopAspectRatio - aspectRatio) < 0.1f)
                {
                    resolutions.push_back({ mode.w, mode.h });
                }
            }
        }

        // Sort by area
        std::sort(resolutions.begin(), resolutions.end(), [](const Resolution& a, const Resolution& b) -> bool {
            int32_t areaA = a.Width * a.Height;
            int32_t areaB = b.Width * b.Height;
            return areaA < areaB;
        });

        // Remove duplicates
        auto last = std::unique(resolutions.begin(), resolutions.end(), [](const Resolution& a, const Resolution& b) -> bool {
            return (a.Width == b.Width && a.Height == b.Height);
        });
        resolutions.erase(last, resolutions.end());

        // Update config fullscreen resolution if not set
        if (!resolutions.empty() && (gConfigGeneral.FullscreenWidth == -1 || gConfigGeneral.FullscreenHeight == -1))
        {
            gConfigGeneral.FullscreenWidth = resolutions.back().Width;
            gConfigGeneral.FullscreenHeight = resolutions.back().Height;
        }

        _fsResolutions = resolutions;
    }

    Resolution GetClosestResolution(int32_t inWidth, int32_t inHeight)
    {
        Resolution result = { 640, 480 };
        int32_t closestAreaDiff = -1;
        int32_t destinationArea = inWidth * inHeight;
        for (const Resolution& resolution : _fsResolutions)
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

    static void DrawWeatherWindow(
        DrawPixelInfo* dpi, IWeatherDrawer* weatherDrawer, WindowBase* original_w, int16_t left, int16_t right, int16_t top,
        int16_t bottom, DrawWeatherFunc drawFunc)
    {
        WindowBase* w{};
        auto itStart = WindowGetIterator(original_w);
        for (auto it = std::next(itStart);; it++)
        {
            if (it == g_window_list.end())
            {
                // Loop ended, draw weather for original_w
                auto vp = original_w->viewport;
                if (vp != nullptr)
                {
                    left = std::max<int16_t>(left, vp->pos.x);
                    right = std::min<int16_t>(right, vp->pos.x + vp->width);
                    top = std::max<int16_t>(top, vp->pos.y);
                    bottom = std::min<int16_t>(bottom, vp->pos.y + vp->height);
                    if (left < right && top < bottom)
                    {
                        auto width = right - left;
                        auto height = bottom - top;
                        drawFunc(dpi, weatherDrawer, left, top, width, height);
                    }
                }
                return;
            }

            w = it->get();
            if (right <= w->windowPos.x || bottom <= w->windowPos.y)
            {
                continue;
            }

            if (RCT_WINDOW_RIGHT(w) <= left || RCT_WINDOW_BOTTOM(w) <= top)
            {
                continue;
            }

            if (left >= w->windowPos.x)
            {
                break;
            }

            DrawWeatherWindow(dpi, weatherDrawer, original_w, left, w->windowPos.x, top, bottom, drawFunc);

            left = w->windowPos.x;
            DrawWeatherWindow(dpi, weatherDrawer, original_w, left, right, top, bottom, drawFunc);
            return;
        }

        int16_t w_right = RCT_WINDOW_RIGHT(w);
        if (right > w_right)
        {
            DrawWeatherWindow(dpi, weatherDrawer, original_w, left, w_right, top, bottom, drawFunc);

            left = w_right;
            DrawWeatherWindow(dpi, weatherDrawer, original_w, left, right, top, bottom, drawFunc);
            return;
        }

        if (top < w->windowPos.y)
        {
            DrawWeatherWindow(dpi, weatherDrawer, original_w, left, right, top, w->windowPos.y, drawFunc);

            top = w->windowPos.y;
            DrawWeatherWindow(dpi, weatherDrawer, original_w, left, right, top, bottom, drawFunc);
            return;
        }

        int16_t w_bottom = RCT_WINDOW_BOTTOM(w);
        if (bottom > w_bottom)
        {
            DrawWeatherWindow(dpi, weatherDrawer, original_w, left, right, top, w_bottom, drawFunc);

            top = w_bottom;
            DrawWeatherWindow(dpi, weatherDrawer, original_w, left, right, top, bottom, drawFunc);
            return;
        }
    }

    InputEvent GetInputEventFromSDLEvent(const SDL_Event& e)
    {
        InputEvent ie;
        ie.DeviceKind = InputDeviceKind::Keyboard;
        ie.Modifiers = e.key.keysym.mod;
        ie.Button = e.key.keysym.sym;

        // Handle dead keys
        if (ie.Button == (SDLK_SCANCODE_MASK | 0))
        {
            switch (e.key.keysym.scancode)
            {
                case SDL_SCANCODE_APOSTROPHE:
                    ie.Button = '\'';
                    break;
                case SDL_SCANCODE_GRAVE:
                    ie.Button = '`';
                    break;
                default:
                    break;
            }
        }

        return ie;
    }

    void SetAudioVolume(float value)
    {
        auto audioContext = GetContext()->GetAudioContext();
        auto mixer = audioContext->GetMixer();
        if (mixer != nullptr)
        {
            mixer->SetVolume(value);
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

InputManager& OpenRCT2::Ui::GetInputManager()
{
    auto uiContext = std::static_pointer_cast<UiContext>(GetContext()->GetUiContext());
    return uiContext->GetInputManager();
}

ShortcutManager& OpenRCT2::Ui::GetShortcutManager()
{
    auto uiContext = std::static_pointer_cast<UiContext>(GetContext()->GetUiContext());
    return uiContext->GetShortcutManager();
}
