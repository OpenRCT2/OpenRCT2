/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

        void checkJoysticks();
        void processAnalogueInput();
        void updateAnalogueScroll();

        void handleViewScrolling();
        void handleModifiers();
        void processEvents();
        void process(const InputEvent& e);
        void processInGameConsole(const InputEvent& e);
        void processChat(const InputEvent& e);
        void processHoldEvents();
        void processViewScrollEvent(std::string_view shortcutId, const ScreenCoordsXY& delta);

        bool getState(const RegisteredShortcut& shortcut) const;
        bool getState(const ShortcutInput& shortcut) const;

        bool hasTextInputFocus() const;

    public:
        InputManager();

        bool isModifierKeyPressed(ModifierKey modifier) const;
        void queueInputEvent(const SDL_Event& e);
        void queueInputEvent(InputEvent&& e);
        void process();
    };
} // namespace OpenRCT2::Ui
