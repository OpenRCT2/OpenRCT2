/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/world/Location.hpp>
#include <queue>
#include <string_view>

typedef struct _SDL_Joystick SDL_Joystick;
typedef union SDL_Event SDL_Event;

namespace OpenRCT2::Ui
{
    class RegisteredShortcut;
    struct ShortcutInput;

    enum class InputDeviceKind
    {
        Mouse,
        Keyboard,
        JoyButton,
        JoyHat,
    };

    enum class InputEventState
    {
        Down,
        Release,
    };

    struct InputEvent
    {
        InputDeviceKind DeviceKind;
        uint32_t Modifiers;
        uint32_t Button;
        InputEventState State;
    };

    class InputManager
    {
    private:
        uint32_t _lastJoystickCheck{};
        std::vector<SDL_Joystick*> _joysticks;
        std::queue<InputEvent> _events;
        ScreenCoordsXY _viewScroll;
        uint32_t _mouseState{};
        std::vector<uint8_t> _keyboardState;

        void CheckJoysticks();

        void HandleViewScrolling();
        void HandleModifiers();
        void ProcessEvents();
        void Process(const InputEvent& e);
        void ProcessInGameConsole(const InputEvent& e);
        void ProcessChat(const InputEvent& e);
        void ProcessHoldEvents();
        void ProcessViewScrollEvent(std::string_view shortcutId, const ScreenCoordsXY& delta);

        bool GetState(const RegisteredShortcut& shortcut) const;
        bool GetState(const ShortcutInput& shortcut) const;

        bool HasTextInputFocus() const;

    public:
        void QueueInputEvent(const SDL_Event& e);
        void QueueInputEvent(InputEvent&& e);
        void Process();
    };
} // namespace OpenRCT2::Ui
