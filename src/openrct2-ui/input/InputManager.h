/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

typedef struct _SDL_GameController SDL_GameController;
typedef union SDL_Event SDL_Event;

namespace OpenRCT2::Ui
{
    class RegisteredShortcut;
    struct ShortcutInput;

    enum class InputDeviceKind
    {
        mouse,
        keyboard,
        joyButton,
        joyHat,
        joyAxis,
    };

    enum class InputEventState
    {
        down,
        release,
    };

    struct InputEvent
    {
        InputDeviceKind deviceKind;
        uint32_t modifiers;
        uint32_t button;
        InputEventState state;
        int16_t axisValue{}; // For analogue stick values (-32768 to 32767)
    };

    enum class ModifierKey : uint8_t
    {
        none = 0,
        shift = 1 << 0,
        ctrl = 1 << 1,
        alt = 1 << 2,
        cmd = 1 << 3,
    };

    class InputManager
    {
    private:
        uint32_t _lastJoystickCheck{};
        std::vector<SDL_GameController*> _gameControllers;
        std::queue<InputEvent> _events;
        ScreenCoordsXY _viewScroll;
        ScreenCoordsXY _analogueScroll;     // analogue stick scroll values
        float _analogueScrollAccumX = 0.0f; // Fractional accumulator for X axis
        float _analogueScrollAccumY = 0.0f; // Fractional accumulator for Y axis
        uint32_t _mouseState{};
        std::vector<uint8_t> _keyboardState;
        uint8_t _modifierKeyState;

        void CheckJoysticks();
        void processAnalogueInput();
        void updateAnalogueScroll();

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
        InputManager();

        bool IsModifierKeyPressed(ModifierKey modifier) const;
        void QueueInputEvent(const SDL_Event& e);
        void QueueInputEvent(InputEvent&& e);
        void Process();
    };
} // namespace OpenRCT2::Ui
