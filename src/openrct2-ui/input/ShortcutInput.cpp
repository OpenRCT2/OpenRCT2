/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiStringIds.h"
#include "ShortcutManager.h"

#include <SDL.h>
#include <cstring>
#include <openrct2/core/String.hpp>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/StringIds.h>
#include <unordered_map>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

constexpr uint32_t kUsefulModifiers = KMOD_SHIFT | KMOD_CTRL | KMOD_ALT | KMOD_GUI;

static uint32_t ParseModifier(std::string_view text)
{
    if (String::iequals(text, "CTRL"))
    {
        return KMOD_CTRL;
    }
    if (String::iequals(text, "LCTRL"))
    {
        return KMOD_LCTRL;
    }
    if (String::iequals(text, "RCTRL"))
    {
        return KMOD_RCTRL;
    }
    if (String::iequals(text, "SHIFT"))
    {
        return KMOD_SHIFT;
    }
    if (String::iequals(text, "LSHIFT"))
    {
        return KMOD_LSHIFT;
    }
    if (String::iequals(text, "RSHIFT"))
    {
        return KMOD_RSHIFT;
    }
    if (String::iequals(text, "ALT"))
    {
        return KMOD_ALT;
    }
    if (String::iequals(text, "LALT"))
    {
        return KMOD_LALT;
    }
    if (String::iequals(text, "RALT"))
    {
        return KMOD_RALT;
    }
    if (String::iequals(text, "GUI"))
    {
        return KMOD_GUI;
    }
    if (String::iequals(text, "LCTRL"))
    {
        return KMOD_LGUI;
    }
    if (String::iequals(text, "RGUI"))
    {
        return KMOD_RGUI;
    }

    return 0;
}

static uint32_t ParseKey(std::string_view text)
{
    char buffer[128]{};
    std::strncpy(buffer, text.data(), sizeof(buffer) - 1);
    auto keyCode = SDL_GetKeyFromName(buffer);
    if (keyCode != SDLK_UNKNOWN)
    {
        return keyCode;
    }
    return 0;
}

static size_t FindPlus(std::string_view s, size_t index)
{
    while (true)
    {
        index = s.find('+', index);
        if (index != std::string::npos && index != 0 && s[index - 1] == ' ')
        {
            index++;
            continue;
        }

        break;
    }
    return index;
}

ShortcutInput::ShortcutInput(std::string_view value)
{
    uint32_t newModifiers = 0;
    size_t index = 0;
    auto sepIndex = FindPlus(value, index);
    while (sepIndex != std::string::npos)
    {
        auto text = value.substr(index, sepIndex - index);
        auto mod = ParseModifier(text);
        newModifiers |= mod;
        index = sepIndex + 1;
        sepIndex = FindPlus(value, index);
    }
    auto rem = value.substr(index);

    if (String::startsWith(rem, "JOY ", true))
    {
        rem = rem.substr(4);
        if (String::equals(rem, "LEFT"))
        {
            kind = InputDeviceKind::joyHat;
            modifiers = newModifiers;
            button = SDL_HAT_LEFT;
        }
        else if (String::equals(rem, "RIGHT"))
        {
            kind = InputDeviceKind::joyHat;
            modifiers = newModifiers;
            button = SDL_HAT_RIGHT;
        }
        else if (String::equals(rem, "UP"))
        {
            kind = InputDeviceKind::joyHat;
            modifiers = newModifiers;
            button = SDL_HAT_UP;
        }
        else if (String::equals(rem, "DOWN"))
        {
            kind = InputDeviceKind::joyHat;
            modifiers = newModifiers;
            button = SDL_HAT_DOWN;
        }
        else
        {
            auto number = String::tryParse<int32_t>(rem);
            if (number.has_value())
            {
                kind = InputDeviceKind::joyButton;
                modifiers = newModifiers;
                button = number.value() - 1;
            }
        }
    }
    else if (String::startsWith(rem, "MOUSE ", true))
    {
        rem = rem.substr(6);
        auto number = String::tryParse<int32_t>(rem);
        if (number)
        {
            kind = InputDeviceKind::mouse;
            modifiers = newModifiers;
            button = *number - 1;
        }
    }
    else if (String::iequals(rem, "LMB"))
    {
        kind = InputDeviceKind::mouse;
        modifiers = newModifiers;
        button = 0;
    }
    else if (String::iequals(rem, "RMB"))
    {
        kind = InputDeviceKind::mouse;
        modifiers = newModifiers;
        button = 1;
    }
    else
    {
        kind = InputDeviceKind::keyboard;
        modifiers = newModifiers;
        button = ParseKey(rem);
    }
}

std::string_view ShortcutInput::getModifierName(uint32_t key, bool localised)
{
    static std::unordered_map<uint32_t, std::pair<const char*, StringId>> _keys{
        { KMOD_SHIFT, { "SHIFT", STR_SHORTCUT_MOD_SHIFT } },    { KMOD_LSHIFT, { "LSHIFT", STR_SHORTCUT_MOD_LSHIFT } },
        { KMOD_RSHIFT, { "RSHIFT", STR_SHORTCUT_MOD_RSHIFT } }, { KMOD_CTRL, { "CTRL", STR_SHORTCUT_MOD_CTRL } },
        { KMOD_LCTRL, { "LCTRL", STR_SHORTCUT_MOD_LCTRL } },    { KMOD_RCTRL, { "RCTRL", STR_SHORTCUT_MOD_RCTRL } },
        { KMOD_ALT, { "ALT", STR_SHORTCUT_MOD_ALT } },          { KMOD_LALT, { "LALT", STR_SHORTCUT_MOD_LALT } },
        { KMOD_RALT, { "RALT", STR_SHORTCUT_MOD_RALT } },       { KMOD_GUI, { "GUI", STR_SHORTCUT_MOD_GUI } },
        { KMOD_LGUI, { "LGUI", STR_SHORTCUT_MOD_LGUI } },       { KMOD_RGUI, { "RGUI", STR_SHORTCUT_MOD_RGUI } },
    };

    auto r = _keys.find(key);
    if (r != _keys.end())
    {
        if (localised && r->second.second != kStringIdNone)
        {
            return LanguageGetString(r->second.second);
        }

        return r->second.first;
    }

    return {};
}

std::string_view ShortcutInput::getLocalisedKeyName(uint32_t key)
{
    static std::unordered_map<uint32_t, StringId> _keys{
        { SDLK_LEFT, STR_SHORTCUT_LEFT },
        { SDLK_RIGHT, STR_SHORTCUT_RIGHT },
        { SDLK_UP, STR_SHORTCUT_UP },
        { SDLK_DOWN, STR_SHORTCUT_DOWN },
        { SDLK_BACKSPACE, STR_SHORTCUT_BACKSPACE },
        { SDLK_ESCAPE, STR_SHORTCUT_ESCAPE },
        { SDLK_SPACE, STR_SHORTCUT_SPACEBAR },
        { SDLK_TAB, STR_SHORTCUT_TAB },
        { SDLK_RETURN, STR_SHORTCUT_RETURN },
        { SDLK_PAGEUP, STR_SHORTCUT_PGUP },
        { SDLK_PAGEDOWN, STR_SHORTCUT_PGDN },
        { SDLK_INSERT, STR_SHORTCUT_INSERT },
        { SDLK_DELETE, STR_SHORTCUT_DELETE },
        { SDLK_KP_DIVIDE, STR_SHORTCUT_NUMPAD_DIVIDE },
        { SDLK_KP_MULTIPLY, STR_SHORTCUT_NUMPAD_MULTIPLY },
        { SDLK_KP_MINUS, STR_SHORTCUT_NUMPAD_MINUS },
        { SDLK_KP_PLUS, STR_SHORTCUT_NUMPAD_PLUS },
        { SDLK_KP_ENTER, STR_SHORTCUT_NUMPAD_RETURN },
        { SDLK_KP_1, STR_SHORTCUT_NUMPAD_1 },
        { SDLK_KP_2, STR_SHORTCUT_NUMPAD_2 },
        { SDLK_KP_3, STR_SHORTCUT_NUMPAD_3 },
        { SDLK_KP_4, STR_SHORTCUT_NUMPAD_4 },
        { SDLK_KP_5, STR_SHORTCUT_NUMPAD_5 },
        { SDLK_KP_6, STR_SHORTCUT_NUMPAD_6 },
        { SDLK_KP_7, STR_SHORTCUT_NUMPAD_7 },
        { SDLK_KP_8, STR_SHORTCUT_NUMPAD_8 },
        { SDLK_KP_9, STR_SHORTCUT_NUMPAD_9 },
        { SDLK_KP_0, STR_SHORTCUT_NUMPAD_0 },
        { SDLK_KP_PERIOD, STR_SHORTCUT_NUMPAD_PERIOD },
        { SDLK_CAPSLOCK, STR_SHORTCUT_CAPSLOCK },
        { SDLK_CLEAR, STR_SHORTCUT_CLEAR },
        { SDLK_PAUSE, STR_SHORTCUT_PAUSE },
        { SDLK_END, STR_SHORTCUT_END },
        { SDLK_HOME, STR_SHORTCUT_HOME },
        { SDLK_SELECT, STR_SHORTCUT_SELECT },
        { SDLK_PRINTSCREEN, STR_SHORTCUT_PRINT },
        { SDLK_EXECUTE, STR_SHORTCUT_EXECUTE },
        { SDLK_SYSREQ, STR_SHORTCUT_SNAPSHOT },
        { SDLK_HELP, STR_SHORTCUT_HELP },
        { SDLK_MENU, STR_SHORTCUT_MENU },
        { SDLK_NUMLOCKCLEAR, STR_SHORTCUT_NUMLOCK },
        { SDLK_SCROLLLOCK, STR_SHORTCUT_SCROLL },
    };

    auto r = _keys.find(key);
    if (r != _keys.end())
    {
        return LanguageGetString(r->second);
    }

    return {};
}

std::string ShortcutInput::toString() const
{
    return toString(false);
}

std::string ShortcutInput::toLocalisedString() const
{
    return toString(true);
}

std::string ShortcutInput::toString(bool localised) const
{
    std::string result;
    appendModifier(result, KMOD_LSHIFT, KMOD_RSHIFT, localised);
    appendModifier(result, KMOD_LCTRL, KMOD_RCTRL, localised);
    appendModifier(result, KMOD_LALT, KMOD_RALT, localised);
    appendModifier(result, KMOD_LGUI, KMOD_RGUI, localised);

    if (kind == InputDeviceKind::keyboard)
    {
        if (button != 0)
        {
            if (localised)
            {
                auto name = getLocalisedKeyName(button);
                if (!name.empty())
                {
                    result += name;
                }
                else
                {
                    result += SDL_GetKeyName(button);
                }
            }
            else
            {
                result += SDL_GetKeyName(button);
            }
        }
    }
    else if (kind == InputDeviceKind::mouse)
    {
        switch (button)
        {
            case 0:
                result += localised ? FormatStringID(STR_SHORTCUT_MOUSE_LEFT, button + 1) : "LMB";
                break;
            case 1:
                result += localised ? FormatStringID(STR_SHORTCUT_MOUSE_RIGHT, button + 1) : "RMB";
                break;
            default:
                result += localised ? FormatStringID(STR_SHORTCUT_MOUSE_NUMBER, button + 1)
                                    : "MOUSE " + std::to_string(button + 1);
                break;
        }
    }
    else if (kind == InputDeviceKind::joyButton)
    {
        result += localised ? FormatStringID(STR_SHORTCUT_JOY_NUMBER, button + 1) : "JOY " + std::to_string(button + 1);
    }
    else if (kind == InputDeviceKind::joyHat)
    {
        if (button & SDL_HAT_LEFT)
            result += localised ? LanguageGetString(STR_SHORTCUT_JOY_LEFT) : "JOY LEFT";
        else if (button & SDL_HAT_RIGHT)
            result += localised ? LanguageGetString(STR_SHORTCUT_JOY_RIGHT) : "JOY RIGHT";
        else if (button & SDL_HAT_UP)
            result += localised ? LanguageGetString(STR_SHORTCUT_JOY_UP) : "JOY UP";
        else if (button & SDL_HAT_DOWN)
            result += localised ? LanguageGetString(STR_SHORTCUT_JOY_DOWN) : "JOY DOWN";
        else
            result += "JOY ?";
    }
    return result;
}

bool ShortcutInput::appendModifier(std::string& s, uint32_t left, uint32_t right, bool localised) const
{
    if ((modifiers & (left | right)) == (left | right))
    {
        s += getModifierName(left | right, localised);
        s += "+";
        return true;
    }
    if (modifiers & left)
    {
        s += getModifierName(left, localised);
        s += "+";
        return true;
    }
    if (modifiers & right)
    {
        s += getModifierName(right, localised);
        s += "+";
        return true;
    }
    return false;
}

static bool HasModifier(uint32_t shortcut, uint32_t actual, uint32_t left, uint32_t right)
{
    if (shortcut & (left | right))
    {
        if ((shortcut & left) && (actual & left))
        {
            return true;
        }
        if ((shortcut & right) && (actual & right))
        {
            return true;
        }
        return false;
    }
    if (actual & (left | right))
    {
        return false;
    }
    return true;
}

static bool CompareModifiers(uint32_t shortcut, uint32_t actual)
{
    shortcut &= kUsefulModifiers;
    return HasModifier(shortcut, actual, KMOD_LCTRL, KMOD_RCTRL) && HasModifier(shortcut, actual, KMOD_LSHIFT, KMOD_RSHIFT)
        && HasModifier(shortcut, actual, KMOD_LALT, KMOD_RALT) && HasModifier(shortcut, actual, KMOD_LGUI, KMOD_RGUI);
}

bool ShortcutInput::matches(const InputEvent& e) const
{
    if (CompareModifiers(modifiers, e.modifiers))
    {
        if (e.deviceKind == kind && button == e.button)
        {
            return true;
        }
    }
    return false;
}

std::optional<ShortcutInput> ShortcutInput::fromInputEvent(const InputEvent& e)
{
    // Assume any side modifier (more specific configurations can be done by manually editing config file)
    auto modifiers = e.modifiers & kUsefulModifiers;
    for (auto mod : { KMOD_CTRL, KMOD_SHIFT, KMOD_ALT, KMOD_GUI })
    {
        if (modifiers & mod)
        {
            modifiers |= mod;
        }
    }

    ShortcutInput result;
    result.kind = e.deviceKind;
    result.modifiers = modifiers;
    result.button = e.button;
    return result;
}
