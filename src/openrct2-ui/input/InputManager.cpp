/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "InputManager.h"

#include <openrct2-ui/input/ShortcutManager.h>

using namespace OpenRCT2::Ui;

void InputManager::QueueInputEvent(InputEvent&& e)
{
    _events.push(e);
}

void InputManager::Process()
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
    shortcutManager.ProcessEvent(e);
}

static InputManager _inputManager;

InputManager& OpenRCT2::Ui::GetInputManager()
{
    return _inputManager;
}
