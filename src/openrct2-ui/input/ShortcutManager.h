/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <memory>
#include <openrct2/core/FileSystem.hpp>
#include <openrct2/localisation/StringIdType.h>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
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
        InputDeviceKind kind{};
        uint32_t modifiers{};
        uint32_t button{};

        ShortcutInput() = default;
        ShortcutInput(std::string_view value);
        std::string toString() const;
        std::string toLocalisedString() const;

        bool matches(const InputEvent& e) const;

        static std::optional<ShortcutInput> fromInputEvent(const InputEvent& e);

    private:
        bool appendModifier(std::string& s, uint32_t left, uint32_t right, bool localised) const;
        static std::string_view getModifierName(uint32_t key, bool localised);
        static std::string_view getLocalisedKeyName(uint32_t key);
        std::string toString(bool localised) const;
    };

    class RegisteredShortcut
    {
    public:
        std::string id;
        StringId localisedName = kStringIdNone;
        std::string customName;
        std::vector<ShortcutInput> standard;
        std::vector<ShortcutInput> current;
        std::function<void()> action;
        size_t orderIndex = static_cast<size_t>(-1);

        RegisteredShortcut() = default;
        RegisteredShortcut(std::string_view _id, std::string_view _name, const std::function<void()>& _action)
            : id(_id)
            , customName(_name)
            , action(_action)
        {
        }

        RegisteredShortcut(std::string_view _id, StringId _localisedName, const std::function<void()>& _action)
            : id(_id)
            , localisedName(_localisedName)
            , action(_action)
        {
        }

        RegisteredShortcut(
            std::string_view _id, StringId _localisedName, std::string_view _defaultChord, const std::function<void()>& _action)
            : id(_id)
            , localisedName(_localisedName)
            , standard({ _defaultChord })
            , current(standard)
            , action(_action)
        {
        }

        RegisteredShortcut(
            std::string_view _id, StringId _localisedName, std::string_view _defaultChordA, std::string_view _defaultChordB,
            const std::function<void()>& _action)
            : id(_id)
            , localisedName(_localisedName)
            , standard({ _defaultChordA, _defaultChordB })
            , current(standard)
            , action(_action)
        {
        }

        std::string_view getTopLevelGroup() const;
        std::string_view getGroup() const;
        bool matches(const InputEvent& e) const;
        bool isSuitableInputEvent(const InputEvent& e) const;
        std::string getDisplayString() const;

    private:
    };

    class ShortcutManager
    {
    private:
        IPlatformEnvironment& _env;
        std::string _pendingShortcutChange;

        static std::optional<ShortcutInput> convertLegacyBinding(uint16_t binding);
        void loadLegacyBindings(const fs::path& path);
        void loadUserBindings(const fs::path& path);
        void saveUserBindings(const fs::path& path);

        // We store the IDs separately so that we can safely use them for string_view in the map
        std::vector<std::unique_ptr<std::string>> _ids;

    public:
        std::unordered_map<std::string_view, RegisteredShortcut> shortcuts;

        ShortcutManager(IPlatformEnvironment& env);
        ShortcutManager(const ShortcutManager&) = delete;

        void loadUserBindings();
        void saveUserBindings();

        void registerShortcut(RegisteredShortcut&& shortcut);
        template<typename... Args>
        void registerShortcut(Args&&... args)
        {
            registerShortcut(RegisteredShortcut(std::forward<Args>(args)...));
        }
        void registerDefaultShortcuts();
        RegisteredShortcut* getShortcut(std::string_view id);
        void removeShortcut(std::string_view id);
        bool isPendingShortcutChange() const;
        void setPendingShortcutChange(std::string_view id);
        void processEvent(const InputEvent& e);
        bool processEventForSpecificShortcut(const InputEvent& e, std::string_view id);

        static std::string_view getLegacyShortcutId(size_t index);
    };
} // namespace OpenRCT2::Ui
