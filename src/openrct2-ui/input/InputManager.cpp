/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

void InputManager::queueInputEvent(const SDL_Event& e)
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
                ie.deviceKind = InputDeviceKind::joyAxis;
                ie.modifiers = SDL_GetModState();
                ie.button = e.caxis.axis;
                ie.state = InputEventState::down;
                ie.axisValue = e.caxis.value;
                queueInputEvent(std::move(ie));
            }
            break;
        }
        case SDL_JOYHATMOTION:
        {
            if (e.jhat.value != SDL_HAT_CENTERED)
            {
                InputEvent ie;
                ie.deviceKind = InputDeviceKind::joyHat;
                ie.modifiers = SDL_GetModState();
                ie.button = e.jhat.value;
                ie.state = InputEventState::down;
                ie.axisValue = 0;
                queueInputEvent(std::move(ie));
            }
            break;
        }
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_JOYBUTTONDOWN:
        {
            InputEvent ie;
            ie.deviceKind = InputDeviceKind::joyButton;
            ie.modifiers = SDL_GetModState();
            ie.button = e.cbutton.button;
            ie.state = InputEventState::down;
            ie.axisValue = 0;
            queueInputEvent(std::move(ie));
            break;
        }
        case SDL_CONTROLLERBUTTONUP:
        case SDL_JOYBUTTONUP:
        {
            InputEvent ie;
            ie.deviceKind = InputDeviceKind::joyButton;
            ie.modifiers = SDL_GetModState();
            ie.button = e.cbutton.button;
            ie.state = InputEventState::release;
            ie.axisValue = 0;
            queueInputEvent(std::move(ie));
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

void InputManager::queueInputEvent(InputEvent&& e)
{
    _events.push(e);
}

void InputManager::checkJoysticks()
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

void InputManager::processAnalogueInput()
{
    _analogueScroll.x = 0;
    _analogueScroll.y = 0;

    const int32_t deadzone = Config::Get().general.gamepadDeadzone;
    const float sensitivity = Config::Get().general.gamepadSensitivity;

    for (auto* gameController : _gameControllers)
    {
        if (gameController != nullptr)
        {
            int32_t stickX = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_LEFTX);
            int32_t stickY = SDL_GameControllerGetAxis(gameController, SDL_CONTROLLER_AXIS_LEFTY);

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
                _analogueScrollAccumX += moveX;
                _analogueScrollAccumY += moveY;

                // Extract integer movement for this frame
                float intPartX, intPartY;
                float fracX = std::modf(_analogueScrollAccumX, &intPartX);
                float fracY = std::modf(_analogueScrollAccumY, &intPartY);

                int pixelsX = static_cast<int>(intPartX);
                int pixelsY = static_cast<int>(intPartY);

                _analogueScrollAccumX = fracX;
                _analogueScrollAccumY = fracY;

                _analogueScroll.x += pixelsX;
                _analogueScroll.y += pixelsY;
            }
        }
    }
}

void InputManager::updateAnalogueScroll()
{
    _viewScroll.x += _analogueScroll.x;
    _viewScroll.y += _analogueScroll.y;
}

void InputManager::process()
{
    checkJoysticks();
    processAnalogueInput();
    handleModifiers();
    processEvents();
    processHoldEvents();
    handleViewScrolling();
}

void InputManager::handleViewScrolling()
{
    if (gLegacyScene == LegacyScene::titleSequence)
        return;

    auto& console = GetInGameConsole();
    if (console.IsOpen())
        return;

    auto mainWindow = WindowGetMain();

    // Handle gamepad analogue scrolling with cursor-based viewport targeting
    if (_analogueScroll.x != 0 || _analogueScroll.y != 0)
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
            if (targetWindow == mainWindow || targetWindow->classification == WindowClass::viewport)
            {
                WindowUnfollowSprite(*targetWindow);
            }
            InputScrollViewportSmooth(_analogueScroll, targetWindow);
        }
    }

    // Handle keyboard shortcut scrolling with edge-based scrolling (but ignore gamepad input)
    ScreenCoordsXY keyboardScroll = { _viewScroll.x - _analogueScroll.x, _viewScroll.y - _analogueScroll.y };
    if (keyboardScroll.x != 0 || keyboardScroll.y != 0)
    {
        if (mainWindow != nullptr)
        {
            WindowUnfollowSprite(*mainWindow);
        }
        InputScrollViewport(keyboardScroll);
    }

    // Mouse edge scrolling
    if (Config::Get().general.edgeScrolling)
    {
        if (InputGetState() != InputState::Normal)
            return;

        if (isModifierKeyPressed(ModifierKey::shift) || isModifierKeyPressed(ModifierKey::ctrl))
            return;

        GameHandleEdgeScroll();
    }
}

void InputManager::handleModifiers()
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

    if (Config::Get().general.virtualFloorStyle != VirtualFloorStyles::Off)
    {
        if (isModifierKeyPressed(ModifierKey::ctrl) || isModifierKeyPressed(ModifierKey::shift))
            VirtualFloorEnable();
        else
            VirtualFloorDisable();
    }
}

bool InputManager::isModifierKeyPressed(ModifierKey modifier) const
{
    return _modifierKeyState & EnumValue(modifier);
}

void InputManager::processEvents()
{
    while (!_events.empty())
    {
        const auto& e = _events.front();
        process(e);
        _events.pop();
    }
}

void InputManager::process(const InputEvent& e)
{
    auto& shortcutManager = GetShortcutManager();
    if (e.deviceKind == InputDeviceKind::keyboard)
    {
        auto& console = GetInGameConsole();
        if (console.IsOpen())
        {
            if (!shortcutManager.processEventForSpecificShortcut(e, ShortcutId::kDebugToggleConsole))
            {
                processInGameConsole(e);
            }
            return;
        }

        if (gChatOpen)
        {
            processChat(e);
            return;
        }

        if (e.deviceKind == InputDeviceKind::keyboard)
        {
            auto* windowMgr = GetWindowManager();

            // TODO: replace with event
            auto w = windowMgr->FindByClass(WindowClass::textinput);
            if (w != nullptr)
            {
                if (e.state == InputEventState::release)
                {
                    Windows::WindowTextInputKey(w, e.button);
                }
                return;
            }

            // TODO: replace with event
            w = windowMgr->FindByClass(WindowClass::loadsaveOverwritePrompt);
            if (w != nullptr)
            {
                if (e.state == InputEventState::release)
                {
                    Windows::WindowLoadSaveOverwritePromptInputKey(w, e.button);
                }
                return;
            }

            // TODO: replace with event
            w = windowMgr->FindByClass(WindowClass::loadsave);
            if (w != nullptr)
            {
                if (e.state == InputEventState::release)
                {
                    Windows::WindowLoadSaveInputKey(w, e.button);
                }
                return;
            }

            if (Windows::IsUsingWidgetTextBox())
            {
                return;
            }
        }
    }
    shortcutManager.processEvent(e);
}

void InputManager::processInGameConsole(const InputEvent& e)
{
    if (e.deviceKind == InputDeviceKind::keyboard && e.state == InputEventState::release)
    {
        auto input = ConsoleInput::None;
        switch (e.button)
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

void InputManager::processChat(const InputEvent& e)
{
    if (e.deviceKind == InputDeviceKind::keyboard && e.state == InputEventState::down)
    {
        auto input = ChatInput::None;
        switch (e.button)
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

void InputManager::processHoldEvents()
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

    if (!hasTextInputFocus())
    {
        auto& shortcutManager = GetShortcutManager();
        if (!shortcutManager.isPendingShortcutChange())
        {
            processViewScrollEvent(ShortcutId::kViewScrollUp, { 0, -1 });
            processViewScrollEvent(ShortcutId::kViewScrollDown, { 0, 1 });
            processViewScrollEvent(ShortcutId::kViewScrollLeft, { -1, 0 });
            processViewScrollEvent(ShortcutId::kViewScrollRight, { 1, 0 });
        }

        updateAnalogueScroll();
    }
}

void InputManager::processViewScrollEvent(std::string_view shortcutId, const ScreenCoordsXY& delta)
{
    auto& shortcutManager = GetShortcutManager();
    auto shortcut = shortcutManager.getShortcut(shortcutId);
    if (shortcut != nullptr && getState(*shortcut))
    {
        _viewScroll.x += delta.x;
        _viewScroll.y += delta.y;
    }
}

bool InputManager::getState(const RegisteredShortcut& shortcut) const
{
    for (const auto& i : shortcut.current)
    {
        if (getState(i))
        {
            return true;
        }
    }
    return false;
}

bool InputManager::getState(const ShortcutInput& shortcut) const
{
    constexpr uint32_t kUsefulModifiers = KMOD_SHIFT | KMOD_CTRL | KMOD_ALT | KMOD_GUI;
    auto modifiers = SDL_GetModState() & kUsefulModifiers;
    if ((shortcut.modifiers & kUsefulModifiers) == modifiers)
    {
        switch (shortcut.kind)
        {
            case InputDeviceKind::mouse:
            {
                if (_mouseState & (1 << shortcut.button))
                {
                    return true;
                }
                break;
            }
            case InputDeviceKind::keyboard:
            {
                auto scanCode = static_cast<size_t>(SDL_GetScancodeFromKey(shortcut.button));
                if (scanCode < _keyboardState.size() && _keyboardState[scanCode])
                {
                    return true;
                }
                break;
            }
            case InputDeviceKind::joyButton:
            {
                for (auto* gameController : _gameControllers)
                {
                    // Get the underlying joystick to maintain compatibility with raw button numbers
                    auto* joystick = SDL_GameControllerGetJoystick(gameController);
                    if (joystick && SDL_JoystickGetButton(joystick, shortcut.button))
                    {
                        return true;
                    }
                }
                break;
            }
            case InputDeviceKind::joyHat:
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
                            if (hat & shortcut.button)
                            {
                                return true;
                            }
                        }
                    }
                }
                break;
            }
            case InputDeviceKind::joyAxis:
            {
                // analogue axes don't have a simple "pressed" state like buttons
                // Return false for shortcuts on analogue axes as they're handled differently
                return false;
            }
        }
    }
    return false;
}

bool InputManager::hasTextInputFocus() const
{
    if (Windows::IsUsingWidgetTextBox() || gChatOpen)
        return true;

    auto* windowMgr = GetWindowManager();
    auto w = windowMgr->FindByClass(WindowClass::textinput);
    if (w != nullptr)
        return true;

    auto& console = GetInGameConsole();
    if (console.IsOpen())
        return true;

    return false;
}
