/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "HookEngine.h"

#include "ScriptEngine.h"

#include <unordered_map>

using namespace OpenRCT2::Scripting;

HOOK_TYPE OpenRCT2::Scripting::GetHookType(const std::string& name)
{
    static const std::unordered_map<std::string, HOOK_TYPE> LookupTable({
        { "interval.tick", HOOK_TYPE::INTERVAL_TICK },
        { "interval.day", HOOK_TYPE::INTERVAL_DAY },
        { "network.chat", HOOK_TYPE::NETWORK_CHAT },
    });
    auto result = LookupTable.find(name);
    return (result != LookupTable.end()) ? result->second : HOOK_TYPE::UNDEFINED;
}

HookEngine::HookEngine(ScriptExecutionInfo& execInfo)
    : _execInfo(execInfo)
{
    _hookMap.resize(NUM_HOOK_TYPES);
    for (size_t i = 0; i < NUM_HOOK_TYPES; i++)
    {
        _hookMap[i].Type = static_cast<HOOK_TYPE>(i);
    }
}

uint32_t HookEngine::Subscribe(HOOK_TYPE type, std::shared_ptr<Plugin> owner, const DukValue& function)
{
    auto& hookList = GetHookList(type);
    auto cookie = _nextCookie++;
    Hook hook(cookie, owner, function);
    hookList.Hooks.push_back(hook);
    return cookie;
}

void HookEngine::Unsubscribe(HOOK_TYPE type, uint32_t cookie)
{
    auto& hookList = GetHookList(type);
    auto& hooks = hookList.Hooks;
    for (auto it = hooks.begin(); it != hooks.end(); it++)
    {
        if (it->Cookie == cookie)
        {
            hooks.erase(it);
            break;
        }
    }
}

void HookEngine::UnsubscribeAll(std::shared_ptr<const Plugin> owner)
{
    for (auto& hookList : _hookMap)
    {
        auto& hooks = hookList.Hooks;
        for (auto it = hooks.begin(); it != hooks.end();)
        {
            if (it->Owner == owner)
            {
                it = hooks.erase(it);
            }
            else
            {
                it++;
            }
        }
    }
}

void HookEngine::UnsubscribeAll()
{
    for (auto& hookList : _hookMap)
    {
        auto& hooks = hookList.Hooks;
        hooks.clear();
    }
}

void HookEngine::Call(HOOK_TYPE type)
{
    auto& hookList = GetHookList(type);
    for (auto& hook : hookList.Hooks)
    {
        ScriptExecutionInfo::PluginScope scope(_execInfo, hook.Owner);

        const auto& function = hook.Function;
        function.push();
        duk_pcall(function.context(), 0);
        duk_pop(function.context());
    }
}

void HookEngine::Call(HOOK_TYPE type, const std::initializer_list<std::pair<std::string_view, std::any>>& args)
{
    auto& hookList = GetHookList(type);
    for (auto& hook : hookList.Hooks)
    {
        ScriptExecutionInfo::PluginScope scope(_execInfo, hook.Owner);

        const auto& function = hook.Function;
        auto ctx = function.context();
        function.push();

        auto objIdx = duk_push_object(ctx);
        for (const auto& arg : args)
        {
            if (arg.second.type() == typeid(int32_t))
            {
                auto val = std::any_cast<int32_t>(arg.second);
                duk_push_int(ctx, val);
            }
            else if (arg.second.type() == typeid(std::string))
            {
                const auto& val = std::any_cast<std::string>(arg.second);
                duk_push_string(ctx, val.c_str());
            }
            else
            {
                throw std::runtime_error("Not implemented");
            }
            duk_put_prop_string(ctx, objIdx, arg.first.data());
        }
        duk_pcall(ctx, 1);
        duk_pop(ctx);
    }
}

HookList& HookEngine::GetHookList(HOOK_TYPE type)
{
    auto index = static_cast<size_t>(type);
    return _hookMap[index];
}
