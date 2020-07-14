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

namespace OpenRCT2::Ui
{
    enum class ShortcutInputKind
    {
        Keyboard,
        Mouse,
    };

    struct ShortcutInput
    {
    public:
        ShortcutInputKind Kind{};
        uint32_t Modifiers{};
        uint32_t Key{};

        ShortcutInput() = default;
        ShortcutInput(const std::string_view& value);
        std::string ToString() const;

    private:
        bool AppendModifier(std::string& s, const std::string_view& text, uint32_t left, uint32_t right) const;
    };

    class RegisteredShortcut
    {
    public:
        std::string Id;
        rct_string_id LocalisedName = STR_NONE;
        std::vector<ShortcutInput> Default;
        std::vector<ShortcutInput> Current;
        std::function<void()> Action;

        RegisteredShortcut() = default;
        RegisteredShortcut(
            const std::string_view& id, rct_string_id localisedName, const std::string_view& defaultChord,
            const std::function<void()>& action)
            : Id(id)
            , LocalisedName(localisedName)
            , Default({ defaultChord })
            , Current(Default)
            , Action(action)
        {
        }

        RegisteredShortcut(
            const std::string_view& id, rct_string_id localisedName, const std::string_view& defaultChordA,
            const std::string_view& defaultChordB, const std::function<void()>& action)
            : Id(id)
            , LocalisedName(localisedName)
            , Default({ defaultChordA, defaultChordB })
            , Current(Default)
            , Action(action)
        {
        }

        std::string_view GetGroup() const;

    private:
    };

    class ShortcutManager
    {
    public:
        std::vector<RegisteredShortcut> Shortcuts;

        ShortcutManager();
        ShortcutManager(const ShortcutManager&) = delete;

        void RegisterShortcut(RegisteredShortcut&& shortcut);
        void RegisterDefaultShortcuts();
    };

    ShortcutManager& GetShortcutManager();

} // namespace OpenRCT2::Ui
