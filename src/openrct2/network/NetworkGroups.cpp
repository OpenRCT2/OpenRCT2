#include "NetworkGroups.h"

#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../platform/platform.h"
#include "../util/Util.h"
#include "NetworkAction.h"

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
    {
        return true;
    }

    json_t* json_groups = json_object_get(json, "groups");
    size_t groupCount = (size_t)json_array_size(json_groups);
    for (size_t i = 0; i < groupCount; i++)
    {
        // json_t* jsonGroup = json_array_get(json_groups, i);

        // auto newgroup = std::make_unique<NetworkGroup>(NetworkGroup::FromJson(jsonGroup));
        // group_list.push_back(std::move(newgroup));
    }

    /*
    json_t* jsonDefaultGroup = json_object_get(json, "default_group");
    default_group = (uint8_t)json_integer_value(jsonDefaultGroup);
    if (GetGroupByID(default_group) == nullptr)
    {
        default_group = 0;
    }
    */
    json_decref(json);

    // Host group should always contain all permissions.
    // group_list.at(0)->ActionsAllowed.fill(0xFF);

    return true;
}

bool NetworkGroups::Save()
{
    utf8 path[MAX_PATH];

    platform_get_user_directory(path, nullptr, sizeof(path));
    safe_strcat_path(path, "groups.json", sizeof(path));
    /*
    json_t* jsonGroupsCfg = json_object();
    json_t* jsonGroups = json_array();
    for (auto& group : group_list)
    {
        json_array_append_new(jsonGroups, group->ToJson());
    }
    json_object_set_new(jsonGroupsCfg, "default_group", json_integer(default_group));
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
    */
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
            res.push_back(group.get());
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
    // Host group
    {
        NetworkGroup* group = Create("Host");
        group->ActionsAllowed.fill(0xFF);
    }

    // Admin group
    {
        NetworkGroup* group = Create("Admin");
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

NetworkGroup* NetworkGroups::Create(const std::string& name)
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

    auto group = std::make_unique<NetworkGroup>();
    group->Id = static_cast<NetworkGroupId_t>(nextFreeId);
    group->SetName(name);

    _groups[nextFreeId] = std::move(group);
    return _groups[nextFreeId].get();
}
