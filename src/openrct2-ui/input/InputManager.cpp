/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "InputManager.h"

#include "ShortcutIds.h"

#include <SDL.h>
#include <SDL_gamecontroller.h>
#include <cmath>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/MouseInput.h>
#include <openrct2-ui/input/ShortcutManager.h>
#include <openrct2-ui/interface/InGameConsole.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/config/Config.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/interface/Window.h>
#include <openrct2/paint/VirtualFloor.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Ui;

InputManager::InputManager()
{
    _modifierKeyState = EnumValue(ModifierKey::none);
}

void InputManager::QueueInputEvent(const SDL_Event& e)
{
    switch (e.type)
    {
        case SDL_CONTROLLERAXISMOTION:
        {
            // Process only the stick axes for scrolling (ignore triggers)
            if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX || e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY
                || e.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX || e.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
            {
                InputEvent ie;
                ie.DeviceKind = InputDeviceKind::JoyAxis;
                ie.Modifiers = SDL_GetModState();
                ie.Button = e.caxis.axis;
                ie.State = InputEventState::Down;
                ie.AxisValue = e.caxis.value;
                QueueInputEvent(std::move(ie));
            }
            break;
        }
        case SDL_JOYHATMOTION:
        {
            if (e.jhat.value != SDL_HAT_CENTERED)
            {
                InputEvent ie;
                ie.DeviceKind = InputDeviceKind::JoyHat;
                ie.Modifiers = SDL_GetModState();
                ie.Button = e.jhat.value;
                ie.State = InputEventState::Down;
                ie.AxisValue = 0;
                QueueInputEvent(std::move(ie));
            }
            break;
        }
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_JOYBUTTONDOWN:
        {
            InputEvent ie;
            ie.DeviceKind = InputDeviceKind::JoyButton;
            ie.Modifiers = SDL_GetModState();
            ie.Button = e.cbutton.button;
            ie.State = InputEventState::Down;
            ie.AxisValue = 0;
            QueueInputEvent(std::move(ie));
            break;
        }
        case SDL_CONTROLLERBUTTONUP:
        case SDL_JOYBUTTONUP:
        {
            InputEvent ie;
            ie.DeviceKind = InputDeviceKind::JoyButton;
            ie.Modifiers = SDL_GetModState();
            ie.Button = e.cbutton.button;
            ie.State = InputEventState::Release;
            ie.AxisValue = 0;
            QueueInputEvent(std::move(ie));
            break;
        }
        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
        case SDL_JOYDEVICEADDED:
        case SDL_JOYDEVICEREMOVED:
        {
            // Force joystick refresh on next check
            _lastJoystickCheck = 0;
            break;
        }
    }
}

void InputManager::QueueInputEvent(InputEvent&& e)
{
    _events.push(e);
}

void InputManager::CheckJoysticks()
{
    constexpr uint32_t kCheckInternalMs = 5000;

    auto tick = SDL_GetTicks();
    if (tick > _lastJoystickCheck + kCheckInternalMs)
    {
        _lastJoystickCheck = tick;

        _gameControllers.clear();
        auto numJoysticks = SDL_NumJoysticks();
        for (auto i = 0; i < numJoysticks; i++)
        {
            if (SDL_IsGameController(i))
            {
                auto gameController = SDL_GameControllerOpen(i);
                if (gameController != nullptr)
                {
                    _gameControllers.push_back(gameController);
                }
            }
        }
    }
}

void InputManager::ProcessAnalogInput()
{
    _analogScroll.x = 0;
    _analogScroll.y = 0;

    const GamepadStick gamepadStick = Config::Get().general.SelectedGamepadStick;
    if (gamepadStick == GamepadStick::Disabled)
        return;

    const int32_t deadzone = Config::Get().general.GamepadDeadzone;
    const float sensitivity = Config::Get().general.GamepadSensitivity;

    for (auto* gameController : _gameControllers)
    {
        if (gameController != nullptr)
        {
            // Get stick X and Y values based on selected stick
            // Note: these have to be **32-bit** integers, otherwise axis inversion would overflow
            int32_t stickX, stickY;
            if (gamepadStick == GamepadStick::Right)
            {
                stickX = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_RIGHTX);
                stickY = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_RIGHTY);
            }
            else
            {
                stickX = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_LEFTX);
                stickY = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_LEFTY);
            }

            // Calculate the magnitude of the stick input vector
            float magnitude = std::sqrt(static_cast<float>(stickX * stickX + stickY * stickY));

            if (magnitude > deadzone)
            {
                // Apply deadzone to the magnitude, creating a more linear response
                float adjustedMagnitude = (magnitude - deadzone) / (32767.0f - deadzone);
                adjustedMagnitude = std::min(adjustedMagnitude, 1.0f);

                float rawX = (stickX / 32767.0f) * adjustedMagnitude;
                float rawY = (stickY / 32767.0f) * adjustedMagnitude;

                // Use a quadratic curve for better fine control at low sensitivities
                float sensitivityCurve = sensitivity * sensitivity;
                float moveX = rawX * sensitivityCurve * 8.0f; // Reasonable base scale
                float moveY = rawY * sensitivityCurve * 8.0f;

                // Accumulate the movement with fractional precision
                _analogScrollAccumX += moveX;
                _analogScrollAccumY += moveY;

                // Extract integer movement for this frame
                float intPartX, intPartY;
                float fracX = std::modf(_analogScrollAccumX, &intPartX);
                float fracY = std::modf(_analogScrollAccumY, &intPartY);

                int pixelsX = static_cast<int>(intPartX);
                int pixelsY = static_cast<int>(intPartY);

                _analogScrollAccumX = fracX;
                _analogScrollAccumY = fracY;

                _analogScroll.x += pixelsX;
                _analogScroll.y += pixelsY;
            }
        }
    }
}

void InputManager::UpdateAnalogScroll()
{
    _viewScroll.x += _analogScroll.x;
    _viewScroll.y += _analogScroll.y;
}

void InputManager::Process()
{
    CheckJoysticks();
    ProcessAnalogInput();
    HandleModifiers();
    ProcessEvents();
    ProcessHoldEvents();
    HandleViewScrolling();
}

void InputManager::HandleViewScrolling()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    auto& console = GetInGameConsole();
    if (console.IsOpen())
        return;

    auto mainWindow = WindowGetMain();

    // Handle gamepad analog scrolling with cursor-based viewport targeting
    if (_analogScroll.x != 0 || _analogScroll.y != 0)
    {
        // Get cursor position to find target viewport
        const CursorState* cursorState = ContextGetCursorState();
        Viewport* targetViewport = ViewportFindFromPoint(cursorState->position);

        WindowBase* targetWindow = nullptr;
        if (targetViewport != nullptr)
        {
            // Find the window that owns this viewport
            auto* windowMgr = GetWindowManager();
            targetWindow = windowMgr->GetOwner(targetViewport);
        }

        // Fallback to main window if no viewport found under cursor
        if (targetWindow == nullptr)
        {
            targetWindow = mainWindow;
        }

        if (targetWindow != nullptr)
        {
            // Only unfollow sprites for the main window or viewport windows
            // Don't unfollow for ride windows that might be following vehicles
            if (targetWindow == mainWindow || targetWindow->classification == WindowClass::Viewport)
            {
                WindowUnfollowSprite(*targetWindow);
            }
            InputScrollViewportSmooth(_analogScroll, targetWindow);
        }
    }

    // Handle keyboard shortcut scrolling with edge-based scrolling (but ignore gamepad input)
    ScreenCoordsXY keyboardScroll = { _viewScroll.x - _analogScroll.x, _viewScroll.y - _analogScroll.y };
    if (keyboardScroll.x != 0 || keyboardScroll.y != 0)
    {
        if (mainWindow != nullptr)
        {
            WindowUnfollowSprite(*mainWindow);
        }
        InputScrollViewport(keyboardScroll);
    }

    // Mouse edge scrolling
    if (Config::Get().general.EdgeScrolling)
    {
        if (InputGetState() != InputState::Normal)
            return;

        if (IsModifierKeyPressed(ModifierKey::shift) || IsModifierKeyPressed(ModifierKey::ctrl))
            return;

        GameHandleEdgeScroll();
    }
}

void InputManager::HandleModifiers()
{
    _modifierKeyState = EnumValue(ModifierKey::none);

    auto modifiers = SDL_GetModState();
    if (modifiers & KMOD_SHIFT)
    {
        _modifierKeyState |= EnumValue(ModifierKey::shift);
    }
    if (modifiers & KMOD_CTRL)
    {
        _modifierKeyState |= EnumValue(ModifierKey::ctrl);
    }
    if (modifiers & KMOD_ALT)
    {
        _modifierKeyState |= EnumValue(ModifierKey::alt);
    }
#ifdef __MACOSX__
    if (modifiers & KMOD_GUI)
    {
        _modifierKeyState |= EnumValue(ModifierKey::cmd);
    }
#endif

    if (Config::Get().general.VirtualFloorStyle != VirtualFloorStyles::Off)
    {
        if (IsModifierKeyPressed(ModifierKey::ctrl) || IsModifierKeyPressed(ModifierKey::shift))
            VirtualFloorEnable();
        else
            VirtualFloorDisable();
    }
}

bool InputManager::IsModifierKeyPressed(ModifierKey modifier) const
{
    return _modifierKeyState & EnumValue(modifier);
}

void InputManager::ProcessEvents()
{
    while (!_events.empty())
    {
        const auto& e = _events.front();
        Process(e);
        _events.pop();
    }
}

void InputManager::Process(const InputEvent& e)
{
    auto& shortcutManager = GetShortcutManager();
    if (e.DeviceKind == InputDeviceKind::Keyboard)
    {
        auto& console = GetInGameConsole();
        if (console.IsOpen())
        {
            if (!shortcutManager.ProcessEventForSpecificShortcut(e, ShortcutId::kDebugToggleConsole))
            {
                ProcessInGameConsole(e);
            }
            return;
        }

        if (gChatOpen)
        {
            ProcessChat(e);
            return;
        }

        if (e.DeviceKind == InputDeviceKind::Keyboard)
        {
            auto* windowMgr = GetWindowManager();

            // TODO: replace with event
            auto w = windowMgr->FindByClass(WindowClass::Textinput);
            if (w != nullptr)
            {
                if (e.State == InputEventState::Release)
                {
                    OpenRCT2::Ui::Windows::WindowTextInputKey(w, e.Button);
                }
                return;
            }

            // TODO: replace with event
            w = windowMgr->FindByClass(WindowClass::LoadsaveOverwritePrompt);
            if (w != nullptr)
            {
                if (e.State == InputEventState::Release)
                {
                    OpenRCT2::Ui::Windows::WindowLoadSaveOverwritePromptInputKey(w, e.Button);
                }
                return;
            }

            // TODO: replace with event
            w = windowMgr->FindByClass(WindowClass::Loadsave);
            if (w != nullptr)
            {
                if (e.State == InputEventState::Release)
                {
                    OpenRCT2::Ui::Windows::WindowLoadSaveInputKey(w, e.Button);
                }
                return;
            }

            if (OpenRCT2::Ui::Windows::IsUsingWidgetTextBox())
            {
                return;
            }
        }
    }
    shortcutManager.ProcessEvent(e);
}

void InputManager::ProcessInGameConsole(const InputEvent& e)
{
    if (e.DeviceKind == InputDeviceKind::Keyboard && e.State == InputEventState::Release)
    {
        auto input = ConsoleInput::None;
        switch (e.Button)
        {
            case SDLK_ESCAPE:
                input = ConsoleInput::LineClear;
                break;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                input = ConsoleInput::LineExecute;
                break;
            case SDLK_UP:
                input = ConsoleInput::HistoryPrevious;
                break;
            case SDLK_DOWN:
                input = ConsoleInput::HistoryNext;
                break;
            case SDLK_PAGEUP:
                input = ConsoleInput::ScrollPrevious;
                break;
            case SDLK_PAGEDOWN:
                input = ConsoleInput::ScrollNext;
                break;
        }
        if (input != ConsoleInput::None)
        {
            auto& console = GetInGameConsole();
            console.Input(input);
        }
    }
}

void InputManager::ProcessChat(const InputEvent& e)
{
    if (e.DeviceKind == InputDeviceKind::Keyboard && e.State == InputEventState::Down)
    {
        auto input = ChatInput::None;
        switch (e.Button)
        {
            case SDLK_ESCAPE:
                input = ChatInput::Close;
                break;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                input = ChatInput::Send;
                break;
        }
        if (input != ChatInput::None)
        {
            ChatInput(input);
        }
    }
}

void InputManager::ProcessHoldEvents()
{
    // Get mouse state
    _mouseState = SDL_GetMouseState(nullptr, nullptr);

    // Get keyboard state
    int numkeys;
    auto keys = SDL_GetKeyboardState(&numkeys);
    _keyboardState.resize(numkeys);
    std::memcpy(_keyboardState.data(), keys, numkeys);

    // Check view scroll shortcuts
    _viewScroll.x = 0;
    _viewScroll.y = 0;

    if (!HasTextInputFocus())
    {
        auto& shortcutManager = GetShortcutManager();
        if (!shortcutManager.IsPendingShortcutChange())
        {
            ProcessViewScrollEvent(ShortcutId::kViewScrollUp, { 0, -1 });
            ProcessViewScrollEvent(ShortcutId::kViewScrollDown, { 0, 1 });
            ProcessViewScrollEvent(ShortcutId::kViewScrollLeft, { -1, 0 });
            ProcessViewScrollEvent(ShortcutId::kViewScrollRight, { 1, 0 });
        }

        UpdateAnalogScroll();
    }
}

void InputManager::ProcessViewScrollEvent(std::string_view shortcutId, const ScreenCoordsXY& delta)
{
    auto& shortcutManager = GetShortcutManager();
    auto shortcut = shortcutManager.GetShortcut(shortcutId);
    if (shortcut != nullptr && GetState(*shortcut))
    {
        _viewScroll.x += delta.x;
        _viewScroll.y += delta.y;
    }
}

bool InputManager::GetState(const RegisteredShortcut& shortcut) const
{
    for (const auto& i : shortcut.Current)
    {
        if (GetState(i))
        {
            return true;
        }
    }
    return false;
}

bool InputManager::GetState(const ShortcutInput& shortcut) const
{
    constexpr uint32_t kUsefulModifiers = KMOD_SHIFT | KMOD_CTRL | KMOD_ALT | KMOD_GUI;
    auto modifiers = SDL_GetModState() & kUsefulModifiers;
    if ((shortcut.Modifiers & kUsefulModifiers) == modifiers)
    {
        switch (shortcut.Kind)
        {
            case InputDeviceKind::Mouse:
            {
                if (_mouseState & (1 << shortcut.Button))
                {
                    return true;
                }
                break;
            }
            case InputDeviceKind::Keyboard:
            {
                auto scanCode = static_cast<size_t>(SDL_GetScancodeFromKey(shortcut.Button));
                if (scanCode < _keyboardState.size() && _keyboardState[scanCode])
                {
                    return true;
                }
                break;
            }
            case InputDeviceKind::JoyButton:
            {
                for (auto* gameController : _gameControllers)
                {
                    // Get the underlying joystick to maintain compatibility with raw button numbers
                    auto* joystick = SDL_GameControllerGetJoystick(gameController);
                    if (joystick && SDL_JoystickGetButton(joystick, shortcut.Button))
                    {
                        return true;
                    }
                }
                break;
            }
            case InputDeviceKind::JoyHat:
            {
                for (auto* gameController : _gameControllers)
                {
                    // Get the underlying joystick to maintain compatibility with hat functionality
                    auto* joystick = SDL_GameControllerGetJoystick(gameController);
                    if (joystick)
                    {
                        auto numHats = SDL_JoystickNumHats(joystick);
                        for (int i = 0; i < numHats; i++)
                        {
                            auto hat = SDL_JoystickGetHat(joystick, i);
                            if (hat & shortcut.Button)
                            {
                                return true;
                            }
                        }
                    }
                }
                break;
            }
            case InputDeviceKind::JoyAxis:
            {
                // Analog axes don't have a simple "pressed" state like buttons
                // Return false for shortcuts on analog axes as they're handled differently
                return false;
            }
        }
    }
    return false;
}

bool InputManager::HasTextInputFocus() const
{
    if (OpenRCT2::Ui::Windows::IsUsingWidgetTextBox() || gChatOpen)
        return true;

    auto* windowMgr = GetWindowManager();
    auto w = windowMgr->FindByClass(WindowClass::Textinput);
    if (w != nullptr)
        return true;

    auto& console = GetInGameConsole();
    if (console.IsOpen())
        return true;

    return false;
}
