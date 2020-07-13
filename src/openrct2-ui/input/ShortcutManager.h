/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <functional>
#include <openrct2/localisation/StringIds.h>
#include <string>
#include <string_view>
#include <vector>

class RegisteredShortcut
{
public:
    std::string Id;
    rct_string_id LocalisedName = STR_NONE;
    uint32_t Modifiers{};
    uint32_t Key{};
    std::function<void()> Action;

    RegisteredShortcut() = default;
    RegisteredShortcut(
        const std::string_view& id, rct_string_id localisedName, const std::string_view& shortcut,
        const std::function<void()>& action)
        : Id(id)
        , LocalisedName(localisedName)
        , Action(action)
    {
    }

    std::string GetKeyText();

private:
    bool AppendModifier(std::string& s, const std::string_view& text, uint32_t left, uint32_t right);
};

class ShortcutManager
{
    std::vector<RegisteredShortcut> Shortcuts;

    void RegisterShortcut(RegisteredShortcut&& shortcut);
    void RegisterDefaultShortcuts();
};
