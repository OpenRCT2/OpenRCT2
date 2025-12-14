/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScPlayerGroup.hpp"

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../actions/NetworkModifyGroupAction.h"
    #include "../../../actions/PlayerSetGroupAction.h"
    #include "../../../core/String.hpp"
    #include "../../../network/Network.h"
    #include "../../../network/NetworkAction.h"

namespace OpenRCT2::Scripting
{
    using OpaquePlayerGroupData = struct
    {
        int32_t id;
    };

    int32_t ScPlayerGroup::GetGroupId(JSValue thisVal)
    {
        OpaquePlayerGroupData* data = gScPlayerGroup.GetOpaque<OpaquePlayerGroupData*>(thisVal);
        if (data)
            return data->id;
        return -1;
    }

    JSValue ScPlayerGroup::id_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt32(ctx, GetGroupId(thisVal));
    }

    JSValue ScPlayerGroup::name_get(JSContext* ctx, JSValue thisVal)
    {
    #ifndef DISABLE_NETWORK
        auto index = Network::GetGroupIndex(GetGroupId(thisVal));
        if (index == -1)
            return JSFromStdString(ctx, {});
        return JSFromStdString(ctx, Network::GetGroupName(index));
    #else
        return JSFromStdString(ctx, {});
    #endif
    }

    JSValue ScPlayerGroup::name_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
    #ifndef DISABLE_NETWORK
        JS_UNPACK_STR(valueStr, ctx, value);
        auto action = GameActions::NetworkModifyGroupAction(
            GameActions::ModifyGroupType::SetName, GetGroupId(thisVal), valueStr);
        GameActions::Execute(&action, getGameState());
    #endif
        return JS_UNDEFINED;
    }

    #ifndef DISABLE_NETWORK
    static JSValue TransformPermissionKeyToJS(JSContext* ctx, const std::string& s)
    {
        auto result = s.substr(sizeof("PERMISSION_") - 1);
        for (auto& c : result)
        {
            c = std::tolower(static_cast<unsigned char>(c));
        }
        return JSFromStdString(ctx, result);
    }

    static std::string TransformPermissionKeyToInternal(JSContext* ctx, JSValue s)
    {
        if (JS_IsString(s))
            return "PERMISSION_" + String::toUpper(JSToStdString(ctx, s));
        return std::string();
    }
    #endif

    JSValue ScPlayerGroup::permissions_get(JSContext* ctx, JSValue thisVal)
    {
        // Create array of permissions
        JSValue result = JS_NewArray(ctx);

    #ifndef DISABLE_NETWORK
        auto index = Network::GetGroupIndex(GetGroupId(thisVal));
        if (index == -1)
            return result;

        auto permissionIndex = 0;
        int64_t resultIdx = 0;
        for (const auto& action : Network::NetworkActions::Actions)
        {
            if (Network::CanPerformAction(index, static_cast<Network::Permission>(permissionIndex)))
            {
                JS_SetPropertyInt64(ctx, result, resultIdx++, TransformPermissionKeyToJS(ctx, action.PermissionName));
            }
            permissionIndex++;
        }
    #endif
        return result;
    }

    JSValue ScPlayerGroup::permissions_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
    #ifndef DISABLE_NETWORK
        JS_UNPACK_ARRAY(array, ctx, value);

        int32_t id = GetGroupId(thisVal);
        auto groupIndex = Network::GetGroupIndex(id);
        if (groupIndex == -1)
            return JS_UNDEFINED;

        // First clear all permissions
        auto networkAction = GameActions::NetworkModifyGroupAction(
            GameActions::ModifyGroupType::SetPermissions, id, "", 0, GameActions::PermissionState::ClearAll);
        GameActions::Execute(&networkAction, getGameState());

        // Don't use vector<bool> since the weird bitpacking specialisation does not work with the lambda (on some compilers)
        std::vector<uint8_t> enabledPermissions(Network::NetworkActions::Actions.size());
        JSIterateArray(ctx, array, [&enabledPermissions](JSContext* ctx2, JSValue x) {
            auto permissionName = TransformPermissionKeyToInternal(ctx2, x);

            auto permissionIndex = 0;
            for (const auto& action : Network::NetworkActions::Actions)
            {
                if (action.PermissionName == permissionName)
                {
                    enabledPermissions[permissionIndex] = true;
                }
                permissionIndex++;
            }
        });

        for (size_t i = 0; i < enabledPermissions.size(); i++)
        {
            auto toggle
                = (enabledPermissions[i] != (Network::CanPerformAction(groupIndex, static_cast<Network::Permission>(i)) != 0));
            if (toggle)
            {
                auto networkAction2 = GameActions::NetworkModifyGroupAction(
                    GameActions::ModifyGroupType::SetPermissions, id, "", static_cast<uint32_t>(i),
                    GameActions::PermissionState::Toggle);
                GameActions::Execute(&networkAction2, getGameState());
            }
        }
    #endif
        return JS_UNDEFINED;
    }

    JSValue ScPlayerGroup::New(JSContext* ctx, int32_t id)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("id", ScPlayerGroup::id_get, nullptr),
            JS_CGETSET_DEF("name", ScPlayerGroup::name_get, ScPlayerGroup::name_set),
            JS_CGETSET_DEF("permissions", ScPlayerGroup::permissions_get, ScPlayerGroup::permissions_set),
        };
        return MakeWithOpaque(ctx, funcs, new OpaquePlayerGroupData{ id });
    }

    void ScPlayerGroup::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "PlayerGroup", Finalize);
    }

    void ScPlayerGroup::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        OpaquePlayerGroupData* data = gScPlayerGroup.GetOpaque<OpaquePlayerGroupData*>(thisVal);
        if (data)
            delete data;
    }

} // namespace OpenRCT2::Scripting

#endif
