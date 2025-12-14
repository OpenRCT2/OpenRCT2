/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "ScriptUtil.hpp"

    #include <any>
    #include <memory>
    #include <string>
    #include <variant>
    #include <vector>

namespace OpenRCT2::Scripting
{
    class ScriptEngine;
    class ScriptExecutionInfo;
    class Plugin;

    enum class HookType
    {
        actionQuery,
        actionExecute,
        intervalTick,
        intervalDay,
        networkChat,
        networkAuthenticate,
        networkJoin,
        networkLeave,
        rideRatingsCalculate,
        actionLocation,
        guestGeneration,
        vehicleCrash,
        mapChange,
        mapChanged,
        mapSave,
        parkCalculateGuestCap,
        count,
        notDefined = -1,
    };
    constexpr size_t NUM_HookTypeS = static_cast<size_t>(HookType::count);
    HookType GetHookType(const std::string& name);

    using HookValue = std::variant<int32_t, int16_t, uint16_t, std::string>;

    template<class... Ts>
    struct HookValuesToJS : Ts...
    {
        using Ts::operator()...;
    };

    struct Hook
    {
        uint32_t Cookie;
        std::shared_ptr<Plugin> Owner;
        JSCallback Function;

        Hook() = default;
        Hook(uint32_t cookie, const std::shared_ptr<Plugin>& owner, const JSCallback& function)
            : Cookie(cookie)
            , Owner(owner)
            , Function(function)
        {
        }
    };

    struct HookList
    {
        HookType Type{};
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
        uint32_t Subscribe(HookType type, const std::shared_ptr<Plugin>& owner, const JSCallback& function);
        void Unsubscribe(HookType type, uint32_t cookie);
        void UnsubscribeAll(const std::shared_ptr<const Plugin>& owner);
        void UnsubscribeAll();
        bool HasSubscriptions(HookType type) const;
        bool IsValidHookForPlugin(HookType type, Plugin& plugin) const;
        void Call(HookType type, bool isGameStateMutable);
        void Call(HookType type, JSValue arg, bool isGameStateMutable, bool keepArgsAlive = false);
        void Call(HookType type, const std::initializer_list<std::pair<std::string, HookValue>>& args, bool isGameStateMutable);

    private:
        HookList& GetHookList(HookType type);
        const HookList& GetHookList(HookType type) const;
    };
} // namespace OpenRCT2::Scripting

#endif
