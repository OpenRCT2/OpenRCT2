/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
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

using namespace OpenRCT2::Ui;

constexpr uint32_t UsefulModifiers = KMOD_SHIFT | KMOD_CTRL | KMOD_ALT | KMOD_GUI;

static uint32_t ParseModifier(const std::string_view& text)
{
    if (String::Equals(text, "CTRL", true))
    {
        return KMOD_CTRL;
    }
    else if (String::Equals(text, "LCTRL", true))
    {
        return KMOD_LCTRL;
    }
    else if (String::Equals(text, "RCTRL", true))
    {
        return KMOD_RCTRL;
    }
    else if (String::Equals(text, "SHIFT", true))
    {
        return KMOD_SHIFT;
    }
    else if (String::Equals(text, "LSHIFT", true))
    {
        return KMOD_LSHIFT;
    }
    else if (String::Equals(text, "RSHIFT", true))
    {
        return KMOD_RSHIFT;
    }
    else if (String::Equals(text, "ALT", true))
    {
        return KMOD_ALT;
    }
    else if (String::Equals(text, "LALT", true))
    {
        return KMOD_LALT;
    }
    else if (String::Equals(text, "RALT", true))
    {
        return KMOD_RALT;
    }
    else if (String::Equals(text, "GUI", true))
    {
        return KMOD_GUI;
    }
    else if (String::Equals(text, "LCTRL", true))
    {
        return KMOD_LGUI;
    }
    else if (String::Equals(text, "RGUI", true))
    {
        return KMOD_RGUI;
    }
    else
    {
        return 0;
    }
}

static uint32_t ParseKey(const std::string_view& text)
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
        else
        {
            break;
        }
    }
    return index;
}

ShortcutInput::ShortcutInput(const std::string_view& value)
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
            if (number)
            {
                Kind = InputDeviceKind::JoyButton;
                Modifiers = modifiers;
                Button = *number - 1;
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

std::string ShortcutInput::ToString() const
{
    std::string result;
    AppendModifier(result, "SHIFT", KMOD_LSHIFT, KMOD_RSHIFT);
    AppendModifier(result, "CTRL", KMOD_LCTRL, KMOD_RCTRL);
    AppendModifier(result, "ALT", KMOD_LALT, KMOD_RALT);
    AppendModifier(result, "GUI", KMOD_LGUI, KMOD_RGUI);

    if (Kind == InputDeviceKind::Keyboard)
    {
        switch (Button)
        {
            case 0:
                break;
            case SDLK_BACKSPACE:
                result += "BACKSPACE";
                break;
            case SDLK_ESCAPE:
                result += "ESCAPE";
                break;
            case SDLK_SPACE:
                result += "SPACE";
                break;
            case SDLK_TAB:
                result += "TAB";
                break;
            case SDLK_RETURN:
                result += "RETURN";
                break;
            case SDLK_PAGEUP:
                result += "PAGE UP";
                break;
            case SDLK_PAGEDOWN:
                result += "PAGE DOWN";
                break;

            case SDLK_KP_DIVIDE:
                result += "NUMPAD /";
                break;
            case SDLK_KP_MULTIPLY:
                result += "NUMPAD *";
                break;
            case SDLK_KP_MINUS:
                result += "NUMPAD -";
                break;
            case SDLK_KP_PLUS:
                result += "NUMPAD +";
                break;
            case SDLK_KP_ENTER:
                result += "NUMPAD RETURN";
                break;
            case SDLK_KP_1:
                result += "NUMPAD 1";
                break;
            case SDLK_KP_2:
                result += "NUMPAD 2";
                break;
            case SDLK_KP_3:
                result += "NUMPAD 3";
                break;
            case SDLK_KP_4:
                result += "NUMPAD 4";
                break;
            case SDLK_KP_5:
                result += "NUMPAD 5";
                break;
            case SDLK_KP_6:
                result += "NUMPAD 6";
                break;
            case SDLK_KP_7:
                result += "NUMPAD 7";
                break;
            case SDLK_KP_8:
                result += "NUMPAD 8";
                break;
            case SDLK_KP_9:
                result += "NUMPAD 9";
                break;
            case SDLK_KP_0:
                result += "NUMPAD 0";
                break;
            case SDLK_KP_PERIOD:
                result += "NUMPAD .";
                break;

            default:
                if (Button & SDLK_SCANCODE_MASK)
                {
                    auto name = SDL_GetScancodeName(static_cast<SDL_Scancode>(Button & ~SDLK_SCANCODE_MASK));
                    result += name;
                }
                else
                {
                    String::AppendCodepoint(result, std::toupper(Button));
                }
                break;
        }
    }
    else if (Kind == InputDeviceKind::Mouse)
    {
        switch (Button)
        {
            case 0:
                result += "LMB";
                break;
            case 1:
                result += "RMB";
                break;
            default:
                result += "MOUSE ";
                result += std::to_string(Button + 1);
                break;
        }
    }
    else if (Kind == InputDeviceKind::JoyButton)
    {
        result += "JOY ";
        result += std::to_string(Button + 1);
    }
    else if (Kind == InputDeviceKind::JoyHat)
    {
        if (Button & SDL_HAT_LEFT)
            result += "JOY LEFT";
        else if (Button & SDL_HAT_RIGHT)
            result += "JOY RIGHT";
        else if (Button & SDL_HAT_UP)
            result += "JOY UP";
        else if (Button & SDL_HAT_DOWN)
            result += "JOY DOWN";
        else
            result += "JOY ?";
    }
    return result;
}

bool ShortcutInput::AppendModifier(std::string& s, const std::string_view& text, uint32_t left, uint32_t right) const
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
    else if (actual & (left | right))
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
