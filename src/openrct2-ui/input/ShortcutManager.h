/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "InputManager.h"

#include <cstdint>
#include <functional>
#include <openrct2/localisation/StringIds.h>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace OpenRCT2::Ui
{
    struct ShortcutInput
    {
    public:
        InputDeviceKind Kind{};
        uint32_t Modifiers{};
        uint32_t Button{};

        ShortcutInput() = default;
        ShortcutInput(const std::string_view& value);
        std::string ToString() const;

        bool Matches(const InputEvent& e) const;

        static std::optional<ShortcutInput> FromInputEvent(const InputEvent& e);

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
        RegisteredShortcut(const std::string_view& id, rct_string_id localisedName, const std::function<void()>& action)
            : Id(id)
            , LocalisedName(localisedName)
            , Action(action)
        {
        }

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
        bool Matches(const InputEvent& e) const;
        bool IsSuitableInputEvent(const InputEvent& e) const;

    private:
    };

    class ShortcutManager
    {
    private:
        std::string _pendingShortcutChange;

    public:
        std::vector<RegisteredShortcut> Shortcuts;

        ShortcutManager();
        ShortcutManager(const ShortcutManager&) = delete;

        void RegisterShortcut(RegisteredShortcut&& shortcut);
        template<typename... Args> void RegisterShortcut(Args&&... args)
        {
            RegisterShortcut(RegisteredShortcut(std::forward<Args>(args)...));
        }
        void RegisterDefaultShortcuts();
        RegisteredShortcut* GetShortcut(std::string_view id);
        void SetPendingShortcutChange(std::string_view id);
        void ProcessEvent(const InputEvent& e);
        bool ProcessEventForSpecificShortcut(const InputEvent& e, std::string_view id);
    };

    ShortcutManager& GetShortcutManager();

    constexpr const char* SHORTCUT_ID_DEBUG_CONSOLE = "debug.console";

} // namespace OpenRCT2::Ui
