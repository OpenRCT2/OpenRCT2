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
#include <openrct2/core/FileSystem.hpp>
#include <openrct2/localisation/StringIds.h>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace OpenRCT2
{
    struct IPlatformEnvironment;
}

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
        std::string CustomName;
        std::vector<ShortcutInput> Default;
        std::vector<ShortcutInput> Current;
        std::function<void()> Action;

        RegisteredShortcut() = default;
        RegisteredShortcut(const std::string_view& id, std::string_view name, const std::function<void()>& action)
            : Id(id)
            , CustomName(name)
            , Action(action)
        {
        }

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

        std::string_view GetTopLevelGroup() const;
        std::string_view GetGroup() const;
        bool Matches(const InputEvent& e) const;
        bool IsSuitableInputEvent(const InputEvent& e) const;
        std::string GetDisplayString() const;

    private:
    };

    class ShortcutManager
    {
    private:
        std::shared_ptr<IPlatformEnvironment> _env;
        std::string _pendingShortcutChange;

        static std::optional<ShortcutInput> ConvertLegacyBinding(uint16_t binding);
        void LoadLegacyBindings(const fs::path& path);
        void LoadUserBindings(const fs::path& path);
        void SaveUserBindings(const fs::path& path);

    public:
        std::vector<RegisteredShortcut> Shortcuts;

        ShortcutManager(const std::shared_ptr<IPlatformEnvironment>& env);
        ShortcutManager(const ShortcutManager&) = delete;

        void LoadUserBindings();
        void SaveUserBindings();

        void RegisterShortcut(RegisteredShortcut&& shortcut);
        template<typename... Args> void RegisterShortcut(Args&&... args)
        {
            RegisterShortcut(RegisteredShortcut(std::forward<Args>(args)...));
        }
        void RegisterDefaultShortcuts();
        RegisteredShortcut* GetShortcut(std::string_view id);
        void RemoveShortcut(std::string_view id);
        bool IsPendingShortcutChange() const;
        void SetPendingShortcutChange(std::string_view id);
        void ProcessEvent(const InputEvent& e);
        bool ProcessEventForSpecificShortcut(const InputEvent& e, std::string_view id);

        static std::string_view GetLegacyShortcutId(size_t index);
    };

    ShortcutManager& GetShortcutManager();
} // namespace OpenRCT2::Ui
