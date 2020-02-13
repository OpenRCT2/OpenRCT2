/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#    include "NetworkUser.h"

#    include "../core/Console.hpp"
#    include "../core/Json.hpp"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "../platform/platform.h"
#    include "NetworkGroups.h"

#    include <unordered_set>

constexpr const utf8* USER_STORE_FILENAME = "users.json";

static NetworkUser* NetworkUserFromJson(json_t* json, NetworkGroups& groups)
{
    const char* hash = json_string_value(json_object_get(json, "hash"));
    const char* name = json_string_value(json_object_get(json, "name"));
    const char* groupName = json_string_value(json_object_get(json, "group"));
    const json_t* jsonGroupId = json_object_get(json, "groupId");

    NetworkUser* user = nullptr;
    if (hash != nullptr && name != nullptr)
    {
        user = new NetworkUser();
        user->Hash = std::string(hash);
        user->Name = std::string(name);
        user->GroupId = groups.GetDefaultId();

        // NOTE: This part only exists for importing old configurations.
        if (!json_is_null(jsonGroupId))
        {
            NetworkGroupId_t oldId = static_cast<NetworkGroupId_t>(json_integer_value(jsonGroupId));
            if (groups.GetById(oldId) != nullptr)
            {
                user->GroupId = (uint8_t)json_integer_value(jsonGroupId);
            }
        }
        else if (groupName != nullptr)
        {
            auto* group = groups.GetByName(groupName);
            if (group != nullptr)
            {
                user->GroupId = group->Id;
            }
        }

        user->Remove = false;
        return user;
    }
    return user;
}

static json_t* NetworkUserToJson(const NetworkUser* user, json_t* json, NetworkGroups& groups)
{
    json_object_set_new(json, "hash", json_string(user->Hash.c_str()));
    json_object_set_new(json, "name", json_string(user->Name.c_str()));

    auto* group = groups.GetById(user->GroupId);
    if (group != nullptr)
    {
        const char* groupName = group->GetName().c_str();
        json_object_set_new(json, "group", json_string(groupName));
    }

    json_object_del(json, "groupId");

    return json;
}

static json_t* NetworkUserToJson(const NetworkUser* user, NetworkGroups& groups)
{
    return NetworkUserToJson(user, json_object(), groups);
}

NetworkUserManager::~NetworkUserManager()
{
    DisposeUsers();
}

void NetworkUserManager::DisposeUsers()
{
    for (const auto& kvp : _usersByHash)
    {
        delete kvp.second;
    }
    _usersByHash.clear();
}

void NetworkUserManager::Load(NetworkGroups& groups)
{
    utf8 path[MAX_PATH];
    GetStorePath(path, sizeof(path));

    if (platform_file_exists(path))
    {
        DisposeUsers();

        try
        {
            json_t* jsonUsers = Json::ReadFromFile(path);
            size_t numUsers = json_array_size(jsonUsers);
            for (size_t i = 0; i < numUsers; i++)
            {
                json_t* jsonUser = json_array_get(jsonUsers, i);
                NetworkUser* networkUser = NetworkUserFromJson(jsonUser, groups);
                if (networkUser != nullptr)
                {
                    _usersByHash[networkUser->Hash] = networkUser;
                }
            }
            json_decref(jsonUsers);
        }
        catch (const std::exception& ex)
        {
            Console::Error::WriteLine("Failed to read %s as JSON. %s", path, ex.what());
        }
    }
}

void NetworkUserManager::Save(NetworkGroups& groups)
{
    utf8 path[MAX_PATH];
    GetStorePath(path, sizeof(path));

    json_t* jsonUsers = nullptr;
    try
    {
        if (platform_file_exists(path))
        {
            jsonUsers = Json::ReadFromFile(path);
        }
    }
    catch (const std::exception&)
    {
    }

    if (jsonUsers == nullptr)
    {
        jsonUsers = json_array();
    }

    // Update existing users
    std::unordered_set<std::string> savedHashes;
    size_t numUsers = json_array_size(jsonUsers);
    for (size_t i = 0; i < numUsers; i++)
    {
        json_t* jsonUser = json_array_get(jsonUsers, i);
        const char* hash = json_string_value(json_object_get(jsonUser, "hash"));
        if (hash != nullptr)
        {
            auto hashString = std::string(hash);
            const NetworkUser* networkUser = GetUserByHash(hashString);
            if (networkUser != nullptr)
            {
                if (networkUser->Remove)
                {
                    json_array_remove(jsonUsers, i);
                    i--;
                }
                else
                {
                    NetworkUserToJson(networkUser, jsonUser, groups);
                    savedHashes.insert(hashString);
                }
            }
        }
    }

    // Add new users
    for (const auto& kvp : _usersByHash)
    {
        const NetworkUser* networkUser = kvp.second;
        if (!networkUser->Remove && savedHashes.find(networkUser->Hash) == savedHashes.end())
        {
            json_t* jsonUser = NetworkUserToJson(networkUser, groups);
            json_array_append_new(jsonUsers, jsonUser);
        }
    }

    Json::WriteToFile(path, jsonUsers, JSON_INDENT(4) | JSON_PRESERVE_ORDER);
    json_decref(jsonUsers);
}

void NetworkUserManager::SwitchGroupUsers(NetworkGroupId_t oldGroupId, NetworkGroupId_t newGroupId)
{
    for (const auto& kvp : _usersByHash)
    {
        NetworkUser* networkUser = kvp.second;
        if (networkUser->GroupId == oldGroupId)
        {
            networkUser->GroupId = newGroupId;
        }
    }
}

void NetworkUserManager::RemoveUser(const std::string& hash)
{
    NetworkUser* networkUser = GetUserByHash(hash);
    if (networkUser != nullptr)
    {
        networkUser->Remove = true;
    }
}

NetworkUser* NetworkUserManager::GetUserByHash(const std::string& hash)
{
    auto it = _usersByHash.find(hash);
    if (it != _usersByHash.end())
    {
        return it->second;
    }
    return nullptr;
}

const NetworkUser* NetworkUserManager::GetUserByHash(const std::string& hash) const
{
    auto it = _usersByHash.find(hash);
    if (it != _usersByHash.end())
    {
        return it->second;
    }
    return nullptr;
}

const NetworkUser* NetworkUserManager::GetUserByName(const std::string& name) const
{
    for (auto kvp : _usersByHash)
    {
        const NetworkUser* networkUser = kvp.second;
        if (String::Equals(name.c_str(), networkUser->Name.c_str(), true))
        {
            return networkUser;
        }
    }
    return nullptr;
}

NetworkUser* NetworkUserManager::GetOrAddUser(const std::string& hash)
{
    NetworkUser* networkUser = GetUserByHash(hash);
    if (networkUser == nullptr)
    {
        networkUser = new NetworkUser();
        networkUser->Hash = hash;
        _usersByHash[hash] = networkUser;
    }
    return networkUser;
}

void NetworkUserManager::GetStorePath(utf8* buffer, size_t bufferSize)
{
    platform_get_user_directory(buffer, nullptr, bufferSize);
    Path::Append(buffer, bufferSize, USER_STORE_FILENAME);
}

#endif
