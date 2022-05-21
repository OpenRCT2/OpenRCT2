/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../common.h"
#    include "Duktape.hpp"

#    include <any>
#    include <memory>
#    include <string>
#    include <tuple>
#    include <vector>

namespace OpenRCT2::Scripting
{
    class ScriptEngine;
    class ScriptExecutionInfo;
    class Plugin;

    enum class HOOK_TYPE
    {
        ACTION_QUERY,
        ACTION_EXECUTE,
        INTERVAL_TICK,
        INTERVAL_DAY,
        NETWORK_CHAT,
        NETWORK_AUTHENTICATE,
        NETWORK_JOIN,
        NETWORK_LEAVE,
        RIDE_RATINGS_CALCULATE,
        ACTION_LOCATION,
        GUEST_GENERATION,
        VEHICLE_CRASH,
        MAP_CHANGE,
        MAP_CHANGED,
        MAP_SAVE,
        COUNT,
        UNDEFINED = -1,
    };
    constexpr size_t NUM_HOOK_TYPES = static_cast<size_t>(HOOK_TYPE::COUNT);
    HOOK_TYPE GetHookType(const std::string& name);

    struct Hook
    {
        uint32_t Cookie;
        std::shared_ptr<Plugin> Owner;
        DukValue Function;

        Hook() = default;
        Hook(uint32_t cookie, std::shared_ptr<Plugin> owner, const DukValue& function)
            : Cookie(cookie)
            , Owner(owner)
            , Function(function)
        {
        }
    };

    struct HookList
    {
        HOOK_TYPE Type{};
        std::vector<Hook> Hooks;

        HookList() = default;
        HookList(const HookList&) = delete;
        HookList(HookList&& src) = default;
    };

    class HookEngine
    {
    private:
        ScriptEngine& _scriptEngine;
        std::vector<HookList> _hookMap;
        uint32_t _nextCookie = 1;

    public:
        HookEngine(ScriptEngine& scriptEngine);
        HookEngine(const HookEngine&) = delete;
        uint32_t Subscribe(HOOK_TYPE type, std::shared_ptr<Plugin> owner, const DukValue& function);
        void Unsubscribe(HOOK_TYPE type, uint32_t cookie);
        void UnsubscribeAll(std::shared_ptr<const Plugin> owner);
        void UnsubscribeAll();
        bool HasSubscriptions(HOOK_TYPE type) const;
        bool IsValidHookForPlugin(HOOK_TYPE type, Plugin& plugin) const;
        void Call(HOOK_TYPE type, bool isGameStateMutable);
        void Call(HOOK_TYPE type, const DukValue& arg, bool isGameStateMutable);
        void Call(
            HOOK_TYPE type, const std::initializer_list<std::pair<std::string_view, std::any>>& args, bool isGameStateMutable);

    private:
        HookList& GetHookList(HOOK_TYPE type);
        const HookList& GetHookList(HOOK_TYPE type) const;
    };
} // namespace OpenRCT2::Scripting

#endif
