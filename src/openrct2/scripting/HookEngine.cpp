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

    #include <unordered_map>

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

uint32_t HookEngine::Subscribe(HookType type, std::shared_ptr<Plugin> owner, const DukValue& function)
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

void HookEngine::UnsubscribeAll(std::shared_ptr<const Plugin> owner)
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
        _scriptEngine.ExecutePluginCall(hook.Owner, hook.Function, {}, isGameStateMutable);
    }
}

void HookEngine::Call(HookType type, const DukValue& arg, bool isGameStateMutable)
{
    auto& hookList = GetHookList(type);
    for (auto& hook : hookList.Hooks)
    {
        _scriptEngine.ExecutePluginCall(hook.Owner, hook.Function, { arg }, isGameStateMutable);
    }
}

void HookEngine::Call(
    HookType type, const std::initializer_list<std::pair<std::string_view, std::any>>& args, bool isGameStateMutable)
{
    auto& hookList = GetHookList(type);
    for (auto& hook : hookList.Hooks)
    {
        auto ctx = _scriptEngine.GetContext();

        // Convert key/value pairs into an object
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

        std::vector<DukValue> dukArgs;
        dukArgs.push_back(DukValue::take_from_stack(ctx));
        _scriptEngine.ExecutePluginCall(hook.Owner, hook.Function, dukArgs, isGameStateMutable);
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
