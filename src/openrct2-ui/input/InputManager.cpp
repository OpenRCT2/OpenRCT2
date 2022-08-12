/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "InputManager.h"

#include "ShortcutIds.h"

#include <SDL.h>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/ShortcutManager.h>
#include <openrct2-ui/interface/InGameConsole.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/config/Config.h>
#include <openrct2/interface/Chat.h>
#include <openrct2/interface/Window.h>
#include <openrct2/paint/VirtualFloor.h>
#include <openrct2/ui/UiContext.h>

using namespace OpenRCT2::Ui;

void InputManager::QueueInputEvent(const SDL_Event& e)
{
    switch (e.type)
    {
        case SDL_JOYHATMOTION:
        {
            if (e.jhat.value != SDL_HAT_CENTERED)
            {
                InputEvent ie;
                ie.DeviceKind = InputDeviceKind::JoyHat;
                ie.Modifiers = SDL_GetModState();
                ie.Button = e.jhat.value;
                ie.State = InputEventState::Down;
                QueueInputEvent(std::move(ie));
            }
            break;
        }
        case SDL_JOYBUTTONDOWN:
        {
            InputEvent ie;
            ie.DeviceKind = InputDeviceKind::JoyButton;
            ie.Modifiers = SDL_GetModState();
            ie.Button = e.jbutton.button;
            ie.State = InputEventState::Down;
            QueueInputEvent(std::move(ie));
            break;
        }
        case SDL_JOYBUTTONUP:
        {
            InputEvent ie;
            ie.DeviceKind = InputDeviceKind::JoyButton;
            ie.Modifiers = SDL_GetModState();
            ie.Button = e.jbutton.button;
            ie.State = InputEventState::Release;
            QueueInputEvent(std::move(ie));
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
    constexpr uint32_t CHECK_INTERVAL_MS = 5000;

    auto tick = SDL_GetTicks();
    if (tick > _lastJoystickCheck + CHECK_INTERVAL_MS)
    {
        _lastJoystickCheck = tick;

        _joysticks.clear();
        auto numJoysticks = SDL_NumJoysticks();
        for (auto i = 0; i < numJoysticks; i++)
        {
            auto joystick = SDL_JoystickOpen(i);
            if (joystick != nullptr)
            {
                _joysticks.push_back(joystick);
            }
        }
    }
}

void InputManager::Process()
{
    CheckJoysticks();
    HandleModifiers();
    ProcessEvents();
    ProcessHoldEvents();
    HandleViewScrolling();
}

void InputManager::HandleViewScrolling()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return;

    auto& console = GetInGameConsole();
    if (console.IsOpen())
        return;

    // Shortcut scrolling
    auto mainWindow = window_get_main();
    if (mainWindow != nullptr && (_viewScroll.x != 0 || _viewScroll.y != 0))
    {
        window_unfollow_sprite(*mainWindow);
    }
    InputScrollViewport(_viewScroll);

    // Mouse edge scrolling
    if (gConfigGeneral.edge_scrolling)
    {
        if (input_get_state() != InputState::Normal)
            return;

        if (gInputPlaceObjectModifier & (PLACE_OBJECT_MODIFIER_SHIFT_Z | PLACE_OBJECT_MODIFIER_COPY_Z))
            return;

        GameHandleEdgeScroll();
    }
}

void InputManager::HandleModifiers()
{
    auto modifiers = SDL_GetModState();
    gInputPlaceObjectModifier = PLACE_OBJECT_MODIFIER_NONE;
    if (modifiers & KMOD_SHIFT)
    {
        gInputPlaceObjectModifier |= PLACE_OBJECT_MODIFIER_SHIFT_Z;
    }
    if (modifiers & KMOD_CTRL)
    {
        gInputPlaceObjectModifier |= PLACE_OBJECT_MODIFIER_COPY_Z;
    }
    if (modifiers & KMOD_ALT)
    {
        gInputPlaceObjectModifier |= 4;
    }
#ifdef __MACOSX__
    if (modifiers & KMOD_GUI)
    {
        gInputPlaceObjectModifier |= 8;
    }
#endif

    if (gConfigGeneral.virtual_floor_style != VirtualFloorStyles::Off)
    {
        if (gInputPlaceObjectModifier & (PLACE_OBJECT_MODIFIER_COPY_Z | PLACE_OBJECT_MODIFIER_SHIFT_Z))
            virtual_floor_enable();
        else
            virtual_floor_disable();
    }
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
            if (!shortcutManager.ProcessEventForSpecificShortcut(e, ShortcutId::DebugToggleConsole))
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
            auto w = window_find_by_class(WC_TEXTINPUT);
            if (w != nullptr)
            {
                if (e.State == InputEventState::Release)
                {
                    WindowTextInputKey(w, e.Button);
                }
                return;
            }

            if (gUsingWidgetTextBox)
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
            chat_input(input);
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
            ProcessViewScrollEvent(ShortcutId::ViewScrollUp, { 0, -1 });
            ProcessViewScrollEvent(ShortcutId::ViewScrollDown, { 0, 1 });
            ProcessViewScrollEvent(ShortcutId::ViewScrollLeft, { -1, 0 });
            ProcessViewScrollEvent(ShortcutId::ViewScrollRight, { 1, 0 });
        }
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
    constexpr uint32_t UsefulModifiers = KMOD_SHIFT | KMOD_CTRL | KMOD_ALT | KMOD_GUI;
    auto modifiers = SDL_GetModState() & UsefulModifiers;
    if ((shortcut.Modifiers & UsefulModifiers) == modifiers)
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
                for (auto* joystick : _joysticks)
                {
                    if (SDL_JoystickGetButton(joystick, shortcut.Button))
                    {
                        return true;
                    }
                }
                break;
            }
            case InputDeviceKind::JoyHat:
            {
                for (auto* joystick : _joysticks)
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
                break;
            }
        }
    }
    return false;
}

bool InputManager::HasTextInputFocus() const
{
    if (gUsingWidgetTextBox || gChatOpen)
        return true;

    auto w = window_find_by_class(WC_TEXTINPUT);
    if (w != nullptr)
        return true;

    auto& console = GetInGameConsole();
    if (console.IsOpen())
        return true;

    return false;
}
