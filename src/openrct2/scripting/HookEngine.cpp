/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "HookEngine.h"

    #include "../core/EnumMap.hpp"
    #include "ScriptEngine.h"

using namespace OpenRCT2::Scripting;

static const EnumMap<HookType> HooksLookupTable(
    {
        { "action.query", HookType::actionQuery },
        { "action.execute", HookType::actionExecute },
        { "interval.tick", HookType::intervalTick },
        { "interval.day", HookType::intervalDay },
        { "network.chat", HookType::networkChat },
        { "network.authenticate", HookType::networkAuthenticate },
        { "network.join", HookType::networkJoin },
        { "network.leave", HookType::networkLeave },
        { "ride.ratings.calculate", HookType::rideRatingsCalculate },
        { "action.location", HookType::actionLocation },
        { "guest.generation", HookType::guestGeneration },
        { "vehicle.crash", HookType::vehicleCrash },
        { "map.change", HookType::mapChange },
        { "map.changed", HookType::mapChanged },
        { "map.save", HookType::mapSave },
        { "park.guest.softcap.calculate", HookType::parkCalculateGuestCap },
    });

HookType OpenRCT2::Scripting::GetHookType(const std::string& name)
{
    auto result = HooksLookupTable.find(name);
    return (result != HooksLookupTable.end()) ? result->second : HookType::notDefined;
}

HookEngine::HookEngine(ScriptEngine& scriptEngine)
    : _scriptEngine(scriptEngine)
{
    _hookMap.resize(NUM_HookTypeS);
    for (size_t i = 0; i < NUM_HookTypeS; i++)
    {
        _hookMap[i].Type = static_cast<HookType>(i);
    }
}

uint32_t HookEngine::Subscribe(HookType type, const std::shared_ptr<Plugin>& owner, const JSCallback& function)
{
    auto& hookList = GetHookList(type);
    auto cookie = _nextCookie++;
    hookList.Hooks.emplace_back(cookie, owner, function);
    return cookie;
}

void HookEngine::Unsubscribe(HookType type, uint32_t cookie)
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

void HookEngine::UnsubscribeAll(const std::shared_ptr<const Plugin>& owner)
{
    for (auto& hookList : _hookMap)
    {
        auto& hooks = hookList.Hooks;
        auto isOwner = [&](auto& obj) { return obj.Owner == owner; };
        hooks.erase(std::remove_if(hooks.begin(), hooks.end(), isOwner), hooks.end());
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

bool HookEngine::HasSubscriptions(HookType type) const
{
    auto& hookList = GetHookList(type);
    return !hookList.Hooks.empty();
}

bool HookEngine::IsValidHookForPlugin(HookType type, Plugin& plugin) const
{
    if (type == HookType::mapChanged && plugin.GetMetadata().Type != PluginType::Intransient)
    {
        return false;
    }
    return true;
}

void HookEngine::Call(HookType type, bool isGameStateMutable)
{
    auto& hookList = GetHookList(type);
    for (auto& hook : hookList.Hooks)
    {
        _scriptEngine.ExecutePluginCall(hook.Owner, hook.Function.callback, {}, isGameStateMutable);
    }
}

void HookEngine::Call(HookType type, const JSValue arg, bool isGameStateMutable, bool keepArgsAlive)
{
    auto& hookList = GetHookList(type);
    for (auto& hook : hookList.Hooks)
    {
        _scriptEngine.ExecutePluginCall(hook.Owner, hook.Function.callback, { arg }, isGameStateMutable, keepArgsAlive);
    }
}

void HookEngine::Call(
    HookType type, const std::initializer_list<std::pair<std::string, HookValue>>& args, bool isGameStateMutable)
{
    auto& hookList = GetHookList(type);
    for (auto& hook : hookList.Hooks)
    {
        JSContext* ctx = hook.Owner.get()->GetContext();

        // Convert key/value pairs into an object
        JSValue obj = JS_NewObject(ctx);
        for (const auto& arg : args)
        {
            JSValue member = std::visit(
                HookValuesToJS{
                    [ctx](int64_t v) { return JS_NewInt64(ctx, v); },
                    [ctx](uint32_t v) { return JS_NewInt64(ctx, v); },
                    [ctx](int32_t v) { return JS_NewInt32(ctx, v); },
                    [ctx](uint16_t v) { return JS_NewInt32(ctx, v); },
                    [ctx](int16_t v) { return JS_NewInt32(ctx, v); },
                    [ctx](const std::string& v) { return JSFromStdString(ctx, v); },
                },
                arg.second);
            JS_SetPropertyStr(ctx, obj, arg.first.c_str(), member);
        }

        _scriptEngine.ExecutePluginCall(hook.Owner, hook.Function.callback, { obj }, isGameStateMutable);
    }
}

HookList& HookEngine::GetHookList(HookType type)
{
    auto index = static_cast<size_t>(type);
    return _hookMap[index];
}

const HookList& HookEngine::GetHookList(HookType type) const
{
    auto index = static_cast<size_t>(type);
    return _hookMap[index];
}

#endif
