/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ShortcutManager.h"

#include <SDL.h>
#include <openrct2/localisation/Language.h>

std::string RegisteredShortcut::GetKeyText()
{
    std::string result;
    AppendModifier(result, "SHIFT", KMOD_LSHIFT, KMOD_RSHIFT);
    AppendModifier(result, "CTRL", KMOD_LCTRL, KMOD_RCTRL);
    AppendModifier(result, "ALT", KMOD_LALT, KMOD_RALT);
    AppendModifier(result, "GUI", KMOD_LGUI, KMOD_RGUI);

    if (Key & SDLK_SCANCODE_MASK)
    {
        auto name = SDL_GetScancodeName(static_cast<SDL_Scancode>(Key & ~SDLK_SCANCODE_MASK));
        result += name;
    }
    else
    {
        char buffer[8]{};
        utf8_write_codepoint(buffer, Key);
        result += buffer;
    }
    return result;
}

bool RegisteredShortcut::AppendModifier(std::string& s, const std::string_view& text, uint32_t left, uint32_t right)
{
    if ((Modifiers & (left | right)) == (left | right))
    {
        s += text;
        s += "+";
        return true;
    }
    else if (Modifiers & left)
    {
        s += "L";
        s += text;
        s += "+";
        return true;
    }
    else if (Modifiers & right)
    {
        s += "R";
        s += text;
        s += "+";
        return true;
    }
    return false;
}

void ShortcutManager::RegisterShortcut(RegisteredShortcut&& shortcut)
{
    Shortcuts.push_back(shortcut);
}
