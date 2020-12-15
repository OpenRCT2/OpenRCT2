/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/world/Location.hpp>
#include <queue>

namespace OpenRCT2::Ui
{
    enum class InputDeviceKind
    {
        Mouse,
        Keyboard,
        Gamepad,
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
        std::queue<InputEvent> _events;
        ScreenCoordsXY _viewScroll;

        void HandleViewScrolling();
        void HandleModifiers();
        void ProcessEvents();
        void Process(const InputEvent& e);
        void ProcessInGameConsole(const InputEvent& e);
        void ProcessChat(const InputEvent& e);

    public:
        void QueueInputEvent(InputEvent&& e);
        void Process();
    };

    InputManager& GetInputManager();

} // namespace OpenRCT2::Ui
