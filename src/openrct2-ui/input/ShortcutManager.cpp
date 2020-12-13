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
#include <openrct2/core/String.hpp>
#include <openrct2/interface/Window.h>
#include <openrct2/localisation/Language.h>

using namespace OpenRCT2::Ui;

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

ShortcutInput::ShortcutInput(const std::string_view& value)
{
    uint32_t modifiers = 0;
    size_t index = 0;
    auto sepIndex = value.find('+', index);
    while (sepIndex != std::string::npos)
    {
        auto text = value.substr(index, sepIndex);
        auto mod = ParseModifier(text);
        modifiers |= mod;
        index = sepIndex + 1;
        sepIndex = value.find('+', index);
    }

    auto kind = ShortcutInputKind::Keyboard;
    auto key = 0u;
    auto colonIndex = value.find(':', index);
    if (colonIndex != std::string::npos)
    {
        auto device = value.substr(index, colonIndex - index);
        if (device == "MOUSE")
        {
            auto rem = std::string(value.substr(colonIndex + 1));
            kind = ShortcutInputKind::Mouse;
            key = atoi(rem.c_str());
        }
    }
    else
    {
        key = ParseKey(value.substr(index));
    }

    Kind = kind;
    Modifiers = modifiers;
    Key = key;
}

std::string ShortcutInput::ToString() const
{
    std::string result;
    AppendModifier(result, "SHIFT", KMOD_LSHIFT, KMOD_RSHIFT);
    AppendModifier(result, "CTRL", KMOD_LCTRL, KMOD_RCTRL);
    AppendModifier(result, "ALT", KMOD_LALT, KMOD_RALT);
    AppendModifier(result, "GUI", KMOD_LGUI, KMOD_RGUI);

    if (Kind == ShortcutInputKind::Keyboard)
    {
        switch (Key)
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
            default:
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
                break;
        }
    }
    else if (Kind == ShortcutInputKind::Mouse)
    {
        switch (Key)
        {
            case 0:
                result += "LMB";
                break;
            case 1:
                result += "RMB";
                break;
            default:
                result += "MOUSE ";
                result += std::to_string(Key);
                break;
        }
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

bool ShortcutInput::Matches(const InputEvent& e) const
{
    if (e.DeviceKind == InputDeviceKind::Mouse)
    {
        if (Kind == ShortcutInputKind::Mouse && Key == e.Button)
        {
            return true;
        }
    }
    return false;
}

std::optional<ShortcutInput> ShortcutInput::FromInputEvent(const InputEvent& e)
{
    if (e.DeviceKind == InputDeviceKind::Mouse)
    {
        ShortcutInput result;
        result.Kind = ShortcutInputKind::Mouse;
        result.Key = e.Button;
        return result;
    }
    return {};
}

std::string_view RegisteredShortcut::GetGroup() const
{
    auto fullstopIndex = Id.find('.');
    if (fullstopIndex != std::string::npos)
    {
        return std::string_view(Id.c_str(), fullstopIndex);
    }
    return {};
}

ShortcutManager::ShortcutManager()
{
    RegisterDefaultShortcuts();
}

void ShortcutManager::RegisterShortcut(RegisteredShortcut&& shortcut)
{
    Shortcuts.push_back(shortcut);
}

RegisteredShortcut* ShortcutManager::GetShortcut(std::string_view id)
{
    auto result = std::find_if(Shortcuts.begin(), Shortcuts.end(), [id](const RegisteredShortcut& s) { return s.Id == id; });
    return result == Shortcuts.end() ? nullptr : &(*result);
}

void ShortcutManager::SetPendingShortcutChange(std::string_view id)
{
    _pendingShortcutChange = id;
}

bool ShortcutManager::IsSuitableInputEvent(const InputEvent& e)
{
    if (e.DeviceKind == InputDeviceKind::Mouse)
    {
        // Do not allow LMB or RMB to be shortcut
        if (e.Button == 0 || e.Button == 1)
        {
            return false;
        }
    }
    return true;
}

void ShortcutManager::ProcessEvent(const InputEvent& e)
{
    if (_pendingShortcutChange.empty())
    {
        for (const auto& shortcut : Shortcuts)
        {
            for (const auto& action : shortcut.Current)
            {
                if (action.Matches(e))
                {
                    shortcut.Action();
                    break;
                }
            }
        }
    }
    else if (IsSuitableInputEvent(e))
    {
        auto shortcut = GetShortcut(_pendingShortcutChange);
        if (shortcut != nullptr)
        {
            auto shortcutInput = ShortcutInput::FromInputEvent(e);
            if (shortcutInput)
            {
                shortcut->Current.clear();
                shortcut->Current.push_back(std::move(*shortcutInput));
            }
        }
        _pendingShortcutChange.clear();
        window_close_by_class(WC_CHANGE_KEYBOARD_SHORTCUT);
    }
}

static ShortcutManager _shortcutManager;

ShortcutManager& OpenRCT2::Ui::GetShortcutManager()
{
    return _shortcutManager;
}
