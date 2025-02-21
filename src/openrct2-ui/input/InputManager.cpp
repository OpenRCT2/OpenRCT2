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
    constexpr uint32_t kCheckInternalMs = 5000;

    auto tick = SDL_GetTicks();
    if (tick > _lastJoystickCheck + kCheckInternalMs)
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
    auto mainWindow = WindowGetMain();
    if (mainWindow != nullptr && (_viewScroll.x != 0 || _viewScroll.y != 0))
    {
        WindowUnfollowSprite(*mainWindow);
    }
    InputScrollViewport(_viewScroll);

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
