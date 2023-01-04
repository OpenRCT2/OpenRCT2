/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScPlayerGroup.hpp"

#    include "../../../Context.h"
#    include "../../../actions/NetworkModifyGroupAction.h"
#    include "../../../actions/PlayerSetGroupAction.h"
#    include "../../../core/String.hpp"
#    include "../../../network/NetworkAction.h"
#    include "../../../network/network.h"
#    include "../../Duktape.hpp"

namespace OpenRCT2::Scripting
{
    ScPlayerGroup::ScPlayerGroup(int32_t id)
        : _id(id)
    {
    }

    int32_t ScPlayerGroup::id_get()
    {
        return _id;
    }

    std::string ScPlayerGroup::name_get() const
    {
#    ifndef DISABLE_NETWORK
        auto index = network_get_group_index(_id);
        if (index == -1)
            return {};
        return network_get_group_name(index);
#    else
        return {};
#    endif
    }

    void ScPlayerGroup::name_set(std::string value)
    {
#    ifndef DISABLE_NETWORK
        auto action = NetworkModifyGroupAction(ModifyGroupType::SetName, _id, value);
        GameActions::Execute(&action);
#    endif
    }

#    ifndef DISABLE_NETWORK
    static std::string TransformPermissionKeyToJS(const std::string& s)
    {
        auto result = s.substr(sizeof("PERMISSION_") - 1);
        for (auto& c : result)
        {
            c = std::tolower(static_cast<unsigned char>(c));
        }
        return result;
    }

    static std::string TransformPermissionKeyToInternal(const std::string& s)
    {
        return "PERMISSION_" + String::ToUpper(s);
    }
#    endif

    std::vector<std::string> ScPlayerGroup::permissions_get() const
    {
#    ifndef DISABLE_NETWORK
        auto index = network_get_group_index(_id);
        if (index == -1)
            return {};

        // Create array of permissions
        std::vector<std::string> result;
        auto permissionIndex = 0;
        for (const auto& action : NetworkActions::Actions)
        {
            if (network_can_perform_action(index, static_cast<NetworkPermission>(permissionIndex)))
            {
                result.push_back(TransformPermissionKeyToJS(action.PermissionName));
            }
            permissionIndex++;
        }
        return result;
#    else
        return {};
#    endif
    }

    void ScPlayerGroup::permissions_set(std::vector<std::string> value)
    {
#    ifndef DISABLE_NETWORK
        auto groupIndex = network_get_group_index(_id);
        if (groupIndex == -1)
            return;

        // First clear all permissions
        auto networkAction = NetworkModifyGroupAction(ModifyGroupType::SetPermissions, _id, "", 0, PermissionState::ClearAll);
        GameActions::Execute(&networkAction);

        std::vector<bool> enabledPermissions;
        enabledPermissions.resize(NetworkActions::Actions.size());
        for (const auto& p : value)
        {
            auto permissionName = TransformPermissionKeyToInternal(p);

            auto permissionIndex = 0;
            for (const auto& action : NetworkActions::Actions)
            {
                if (action.PermissionName == permissionName)
                {
                    enabledPermissions[permissionIndex] = true;
                }
                permissionIndex++;
            }
        }

        for (size_t i = 0; i < enabledPermissions.size(); i++)
        {
            auto toggle
                = (enabledPermissions[i] != (network_can_perform_action(groupIndex, static_cast<NetworkPermission>(i)) != 0));
            if (toggle)
            {
                auto networkAction2 = NetworkModifyGroupAction(
                    ModifyGroupType::SetPermissions, _id, "", static_cast<uint32_t>(i), PermissionState::Toggle);
                GameActions::Execute(&networkAction2);
            }
        }
#    endif
    }

    void ScPlayerGroup::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScPlayerGroup::id_get, nullptr, "id");
        dukglue_register_property(ctx, &ScPlayerGroup::name_get, &ScPlayerGroup::name_set, "name");
        dukglue_register_property(ctx, &ScPlayerGroup::permissions_get, &ScPlayerGroup::permissions_set, "permissions");
    }

} // namespace OpenRCT2::Scripting

#endif
