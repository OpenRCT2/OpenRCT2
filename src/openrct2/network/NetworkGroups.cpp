#ifndef DISABLE_NETWORK

#    include "NetworkGroups.h"

#    include "../core/Guard.hpp"
#    include "../core/Json.hpp"
#    include "../platform/platform.h"
#    include "../util/Util.h"
#    include "NetworkAction.h"

NetworkGroups::NetworkGroups()
{
    CreateDefaultGroups();
}

void NetworkGroups::Reset()
{
    Clear();
    CreateDefaultGroups();
}

void NetworkGroups::Clear()
{
    for (auto& group : _groups)
    {
        if (group == nullptr)
            continue;
        group.reset();
    }
}

bool NetworkGroups::Load()
{
    Reset();

    utf8 path[MAX_PATH];

    platform_get_user_directory(path, nullptr, sizeof(path));
    safe_strcat_path(path, "groups.json", sizeof(path));

    json_t* json = nullptr;
    if (platform_file_exists(path))
    {
        try
        {
            json = Json::ReadFromFile(path);
        }
        catch (const std::exception& e)
        {
            log_error("Failed to read %s as JSON. Setting default groups. %s", path, e.what());
        }
    }

    if (json == nullptr)
        return true;

    auto addOrUpdateGroup = [&](json_t* jsonGroup) -> void {
        auto tmpGroup = NetworkGroup::FromJson(jsonGroup);

        // Name always has priority over id.
        auto* group = GetByName(tmpGroup.GetName());
        if (group != nullptr && group->IsImmutable())
        {
            // Don't modify immutable groups, old group.json files still contain immutable groups.
            return;
        }

        // If it doesn't exist, create one with the given name.
        if (group == nullptr)
            group = Create(tmpGroup.GetName(), false, true);

        // Update.
        group->ActionsAllowed = tmpGroup.ActionsAllowed;
    };

    json_t* json_groups = json_object_get(json, "groups");
    size_t groupCount = (size_t)json_array_size(json_groups);
    for (size_t i = 0; i < groupCount; i++)
    {
        json_t* jsonGroup = json_array_get(json_groups, i);
        addOrUpdateGroup(jsonGroup);
    }

    json_t* jsonDefaultGroup = json_object_get(json, "default_group");
    NetworkGroupId_t defaultId = static_cast<NetworkGroupId_t>(json_integer_value(jsonDefaultGroup));
    if (auto* group = GetById(defaultId))
    {
        SetDefault(group);
    }

    json_decref(json);

    return true;
}

bool NetworkGroups::Save()
{
    utf8 path[MAX_PATH];

    platform_get_user_directory(path, nullptr, sizeof(path));
    safe_strcat_path(path, "groups.json", sizeof(path));

    json_t* jsonGroupsCfg = json_object();
    json_t* jsonGroups = json_array();

    auto groups = GetUserGroups();
    for (auto* group : groups)
    {
        json_array_append_new(jsonGroups, group->ToJson());
    }
    json_object_set_new(jsonGroupsCfg, "default_group", json_integer(_defaultId));
    json_object_set_new(jsonGroupsCfg, "groups", jsonGroups);
    try
    {
        Json::WriteToFile(path, jsonGroupsCfg, JSON_INDENT(4) | JSON_PRESERVE_ORDER);
    }
    catch (const std::exception& ex)
    {
        log_error("Unable to save %s: %s", path, ex.what());
    }

    json_decref(jsonGroupsCfg);

    return true;
}

NetworkGroup* NetworkGroups::GetById(NetworkGroupId_t id) const
{
    if (static_cast<size_t>(id) >= _groups.size())
        return nullptr;

    return _groups[id].get();
}

NetworkGroup* NetworkGroups::GetByName(const std::string& name) const
{
    for (auto& group : _groups)
    {
        if (group == nullptr)
            continue;
        if (group->GetName() == name)
            return group.get();
    }
    return nullptr;
}

std::vector<NetworkGroup*> NetworkGroups::GetAll() const
{
    std::vector<NetworkGroup*> res;
    res.reserve(_groups.size());
    for (auto& group : _groups)
    {
        if (group != nullptr)
        {
            res.push_back(group.get());
        }
    }
    return res;
}

std::vector<NetworkGroup*> NetworkGroups::GetUserGroups() const
{
    std::vector<NetworkGroup*> res;
    res.reserve(_groups.size());
    for (auto& group : _groups)
    {
        if (group != nullptr && group->Id != kGroupIdHost)
        {
            res.push_back(group.get());
        }
    }
    return res;
}

size_t NetworkGroups::GetCount() const
{
    size_t res = 0;
    for (auto& group : _groups)
    {
        if (group == nullptr)
            continue;
        res++;
    }
    return res;
}

void NetworkGroups::Serialise(DataSerialiser& ds)
{
    uint8_t count = 0;
    if (ds.IsSaving())
    {
        auto groups = GetAll();
        count = static_cast<uint8_t>(groups.size());
        ds << count;
        for (auto& group : groups)
        {
            group->Serialise(ds);
        }
    }
    else
    {
        Clear();

        ds << count;
        for (uint8_t i = 0; i < count; i++)
        {
            auto group = std::make_unique<NetworkGroup>();
            group->Serialise(ds);

            _groups[group->Id] = std::move(group);
        }
    }
}

void NetworkGroups::CreateDefaultGroups()
{
    // Host group, this is a special one that indicates the server machine.
    {
        auto group = std::make_unique<NetworkGroup>(true, false);
        group->Id = kGroupIdHost;
        group->SetName("Host");
        group->ActionsAllowed.fill(0xFF);

        _groups[kGroupIdHost] = std::move(group);
    }

    // Admin group
    {
        NetworkGroup* group = Create("Admin", true, false);
        group->ActionsAllowed.fill(0xFF);
    }

    // Spectator group
    {
        NetworkGroup* group = Create("Spectator");
        group->ToggleActionPermission(NETWORK_PERMISSION_CHAT);

        // Default to spectators.
        SetDefault(group);
    }

    // User group
    {
        NetworkGroup* group = Create("User");
        group->ActionsAllowed.fill(0xFF);
        group->ToggleActionPermission(NETWORK_PERMISSION_KICK_PLAYER);
        group->ToggleActionPermission(NETWORK_PERMISSION_MODIFY_GROUPS);
        group->ToggleActionPermission(NETWORK_PERMISSION_SET_PLAYER_GROUP);
        group->ToggleActionPermission(NETWORK_PERMISSION_CHEAT);
        group->ToggleActionPermission(NETWORK_PERMISSION_PASSWORDLESS_LOGIN);
        group->ToggleActionPermission(NETWORK_PERMISSION_MODIFY_TILE);
        group->ToggleActionPermission(NETWORK_PERMISSION_EDIT_SCENARIO_OPTIONS);
    }
}

bool NetworkGroups::Remove(const NetworkGroup* group)
{
    return Remove(group->Id);
}

bool NetworkGroups::Remove(NetworkGroupId_t id)
{
    if (static_cast<size_t>(id) >= _groups.size())
        return false;

    if (_groups[id] == nullptr)
        return false;

    _groups[id].reset();
    return true;
}

void NetworkGroups::SetDefault(const NetworkGroup* group)
{
    _defaultId = group->Id;
}

NetworkGroup* NetworkGroups::GetDefault() const
{
    Guard::Assert(static_cast<size_t>(_defaultId) < _groups.size());
    return _groups[_defaultId].get();
}

NetworkGroupId_t NetworkGroups::GetDefaultId() const
{
    return _defaultId;
}

NetworkGroup* NetworkGroups::Create(const std::string& name, bool immutable /*= false*/, bool canBeDefault /*= true*/)
{
    size_t nextFreeId = std::numeric_limits<size_t>::max();
    for (size_t i = 0; i < _groups.size(); i++)
    {
        if (_groups[i] == nullptr)
        {
            nextFreeId = i;
            break;
        }
    }

    if (nextFreeId == std::numeric_limits<size_t>::max())
    {
        log_error("Unable to create group, no free group slots left");
        return nullptr;
    }

    auto group = std::make_unique<NetworkGroup>(immutable, canBeDefault);
    group->Id = static_cast<NetworkGroupId_t>(nextFreeId);
    group->SetName(name);

    _groups[nextFreeId] = std::move(group);
    return _groups[nextFreeId].get();
}

#endif // DISABLE_NETWORK
