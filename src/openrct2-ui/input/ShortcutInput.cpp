/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ShortcutManager.h"

#include <SDL.h>
#include <cstring>
#include <openrct2/core/String.hpp>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/Localisation.h>
#include <unordered_map>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

constexpr uint32_t UsefulModifiers = KMOD_SHIFT | KMOD_CTRL | KMOD_ALT | KMOD_GUI;

static uint32_t ParseModifier(std::string_view text)
{
    if (String::Equals(text, "CTRL", true))
    {
        return KMOD_CTRL;
    }
    if (String::Equals(text, "LCTRL", true))
    {
        return KMOD_LCTRL;
    }
    if (String::Equals(text, "RCTRL", true))
    {
        return KMOD_RCTRL;
    }
    if (String::Equals(text, "SHIFT", true))
    {
        return KMOD_SHIFT;
    }
    if (String::Equals(text, "LSHIFT", true))
    {
        return KMOD_LSHIFT;
    }
    if (String::Equals(text, "RSHIFT", true))
    {
        return KMOD_RSHIFT;
    }
    if (String::Equals(text, "ALT", true))
    {
        return KMOD_ALT;
    }
    if (String::Equals(text, "LALT", true))
    {
        return KMOD_LALT;
    }
    if (String::Equals(text, "RALT", true))
    {
        return KMOD_RALT;
    }
    if (String::Equals(text, "GUI", true))
    {
        return KMOD_GUI;
    }
    if (String::Equals(text, "LCTRL", true))
    {
        return KMOD_LGUI;
    }
    if (String::Equals(text, "RGUI", true))
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
    uint32_t modifiers = 0;
    size_t index = 0;
    auto sepIndex = FindPlus(value, index);
    while (sepIndex != std::string::npos)
    {
        auto text = value.substr(index, sepIndex - index);
        auto mod = ParseModifier(text);
        modifiers |= mod;
        index = sepIndex + 1;
        sepIndex = FindPlus(value, index);
    }
    auto rem = value.substr(index);

    if (String::StartsWith(rem, "JOY ", true))
    {
        rem = rem.substr(4);
        if (String::Equals(rem, "LEFT"))
        {
            Kind = InputDeviceKind::JoyHat;
            Modifiers = modifiers;
            Button = SDL_HAT_LEFT;
        }
        else if (String::Equals(rem, "RIGHT"))
        {
            Kind = InputDeviceKind::JoyHat;
            Modifiers = modifiers;
            Button = SDL_HAT_RIGHT;
        }
        else if (String::Equals(rem, "UP"))
        {
            Kind = InputDeviceKind::JoyHat;
            Modifiers = modifiers;
            Button = SDL_HAT_UP;
        }
        else if (String::Equals(rem, "DOWN"))
        {
            Kind = InputDeviceKind::JoyHat;
            Modifiers = modifiers;
            Button = SDL_HAT_DOWN;
        }
        else
        {
            auto number = String::Parse<int32_t>(rem);
            if (number.has_value())
            {
                Kind = InputDeviceKind::JoyButton;
                Modifiers = modifiers;
                Button = number.value() - 1;
            }
        }
    }
    else if (String::StartsWith(rem, "MOUSE ", true))
    {
        rem = rem.substr(6);
        auto number = String::Parse<int32_t>(rem);
        if (number)
        {
            Kind = InputDeviceKind::Mouse;
            Modifiers = modifiers;
            Button = *number - 1;
        }
    }
    else if (String::Equals(rem, "LMB", true))
    {
        Kind = InputDeviceKind::Mouse;
        Modifiers = modifiers;
        Button = 0;
    }
    else if (String::Equals(rem, "RMB", true))
    {
        Kind = InputDeviceKind::Mouse;
        Modifiers = modifiers;
        Button = 1;
    }
    else
    {
        Kind = InputDeviceKind::Keyboard;
        Modifiers = modifiers;
        Button = ParseKey(rem);
    }
}

std::string_view ShortcutInput::GetModifierName(uint32_t key, bool localised)
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
        if (localised && r->second.second != STR_NONE)
        {
            return language_get_string(r->second.second);
        }

        return r->second.first;
    }

    return {};
}

std::string_view ShortcutInput::GetLocalisedKeyName(uint32_t key)
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
        { SDLK_CAPSLOCK, STR_SHORTCUT_NUMPAD_PERIOD },
    };

    auto r = _keys.find(key);
    if (r != _keys.end())
    {
        return language_get_string(r->second);
    }

    return {};
}

std::string ShortcutInput::ToString() const
{
    return ToString(false);
}

std::string ShortcutInput::ToLocalisedString() const
{
    return ToString(true);
}

std::string ShortcutInput::ToString(bool localised) const
{
    std::string result;
    AppendModifier(result, KMOD_LSHIFT, KMOD_RSHIFT, localised);
    AppendModifier(result, KMOD_LCTRL, KMOD_RCTRL, localised);
    AppendModifier(result, KMOD_LALT, KMOD_RALT, localised);
    AppendModifier(result, KMOD_LGUI, KMOD_RGUI, localised);

    if (Kind == InputDeviceKind::Keyboard)
    {
        if (Button != 0)
        {
            if (localised)
            {
                auto name = GetLocalisedKeyName(Button);
                if (!name.empty())
                {
                    result += name;
                }
                else
                {
                    result += SDL_GetKeyName(Button);
                }
            }
            else
            {
                result += SDL_GetKeyName(Button);
            }
        }
    }
    else if (Kind == InputDeviceKind::Mouse)
    {
        switch (Button)
        {
            case 0:
                result += localised ? FormatStringId(STR_SHORTCUT_MOUSE_LEFT, Button + 1) : "LMB";
                break;
            case 1:
                result += localised ? FormatStringId(STR_SHORTCUT_MOUSE_RIGHT, Button + 1) : "RMB";
                break;
            default:
                result += localised ? FormatStringId(STR_SHORTCUT_MOUSE_NUMBER, Button + 1)
                                    : "MOUSE " + std::to_string(Button + 1);
                break;
        }
    }
    else if (Kind == InputDeviceKind::JoyButton)
    {
        result += localised ? FormatStringId(STR_SHORTCUT_JOY_NUMBER, Button + 1) : "JOY " + std::to_string(Button + 1);
    }
    else if (Kind == InputDeviceKind::JoyHat)
    {
        if (Button & SDL_HAT_LEFT)
            result += localised ? language_get_string(STR_SHORTCUT_JOY_LEFT) : "JOY LEFT";
        else if (Button & SDL_HAT_RIGHT)
            result += localised ? language_get_string(STR_SHORTCUT_JOY_RIGHT) : "JOY RIGHT";
        else if (Button & SDL_HAT_UP)
            result += localised ? language_get_string(STR_SHORTCUT_JOY_UP) : "JOY UP";
        else if (Button & SDL_HAT_DOWN)
            result += localised ? language_get_string(STR_SHORTCUT_JOY_DOWN) : "JOY DOWN";
        else
            result += "JOY ?";
    }
    return result;
}

bool ShortcutInput::AppendModifier(std::string& s, uint32_t left, uint32_t right, bool localised) const
{
    if ((Modifiers & (left | right)) == (left | right))
    {
        s += GetModifierName(left | right, localised);
        s += "+";
        return true;
    }
    if (Modifiers & left)
    {
        s += GetModifierName(left, localised);
        s += "+";
        return true;
    }
    if (Modifiers & right)
    {
        s += GetModifierName(right, localised);
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
    shortcut &= UsefulModifiers;
    return HasModifier(shortcut, actual, KMOD_LCTRL, KMOD_RCTRL) && HasModifier(shortcut, actual, KMOD_LSHIFT, KMOD_RSHIFT)
        && HasModifier(shortcut, actual, KMOD_LALT, KMOD_RALT) && HasModifier(shortcut, actual, KMOD_LGUI, KMOD_RGUI);
}

bool ShortcutInput::Matches(const InputEvent& e) const
{
    if (CompareModifiers(Modifiers, e.Modifiers))
    {
        if (e.DeviceKind == Kind && Button == e.Button)
        {
            return true;
        }
    }
    return false;
}

std::optional<ShortcutInput> ShortcutInput::FromInputEvent(const InputEvent& e)
{
    // Assume any side modifier (more specific configurations can be done by manually editing config file)
    auto modifiers = e.Modifiers & UsefulModifiers;
    for (auto mod : { KMOD_CTRL, KMOD_SHIFT, KMOD_ALT, KMOD_GUI })
    {
        if (modifiers & mod)
        {
            modifiers |= mod;
        }
    }

    ShortcutInput result;
    result.Kind = e.DeviceKind;
    result.Modifiers = modifiers;
    result.Button = e.Button;
    return result;
}
