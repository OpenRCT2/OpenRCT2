/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#include <unordered_set>

#include "../core/Console.hpp"
#include "../core/Json.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "NetworkUser.h"

#include "../platform/platform.h"

constexpr const utf8 * USER_STORE_FILENAME = "users.json";

NetworkUser * NetworkUser::FromJson(json_t * json)
{
    const char * hash = json_string_value(json_object_get(json, "hash"));
    const char * name = json_string_value(json_object_get(json, "name"));
    const json_t * jsonGroupId = json_object_get(json, "groupId");

    NetworkUser * user = nullptr;
    if (hash != nullptr && name != nullptr) {
        user = new NetworkUser();
        user->Hash = std::string(hash);
        user->Name = std::string(name);
        if (!json_is_null(jsonGroupId)) {
            user->GroupId = (uint8_t)json_integer_value(jsonGroupId);
        }
        user->Remove = false;
        return user;
    }
    return user;
}

json_t * NetworkUser::ToJson() const
{
    return ToJson(json_object());
}

json_t * NetworkUser::ToJson(json_t * json) const
{
    json_object_set_new(json, "hash", json_string(Hash.c_str()));
    json_object_set_new(json, "name", json_string(Name.c_str()));

    json_t * jsonGroupId;
    if (GroupId.HasValue()) {
        jsonGroupId = json_integer(GroupId.GetValue());
    } else {
        jsonGroupId = json_null();
    }
    json_object_set_new(json, "groupId", jsonGroupId);

    return json;
}

NetworkUserManager::~NetworkUserManager()
{
    DisposeUsers();
}

void NetworkUserManager::DisposeUsers()
{
    for (const auto &kvp : _usersByHash)
    {
        delete kvp.second;
    }
    _usersByHash.clear();
}

void NetworkUserManager::Load()
{
    utf8 path[MAX_PATH];
    GetStorePath(path, sizeof(path));

    if (platform_file_exists(path))
    {
        DisposeUsers();

        try
        {
            json_t * jsonUsers = Json::ReadFromFile(path);
            size_t numUsers = json_array_size(jsonUsers);
            for (size_t i = 0; i < numUsers; i++)
            {
                json_t * jsonUser = json_array_get(jsonUsers, i);
                NetworkUser * networkUser = NetworkUser::FromJson(jsonUser);
                if (networkUser != nullptr)
                {
                    _usersByHash[networkUser->Hash] = networkUser;
                }
            }
            json_decref(jsonUsers);
        }
        catch (const std::exception &ex)
        {
            Console::Error::WriteLine("Failed to read %s as JSON. %s", path, ex.what());
        }
    }
}

void NetworkUserManager::Save()
{
    utf8 path[MAX_PATH];
    GetStorePath(path, sizeof(path));

    json_t * jsonUsers = nullptr;
    try
    {
        if (platform_file_exists(path))
        {
            jsonUsers = Json::ReadFromFile(path);
        }
    }
    catch (const std::exception &)
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
        json_t * jsonUser = json_array_get(jsonUsers, i);
        const char * hash = json_string_value(json_object_get(jsonUser, "hash"));
        if (hash != nullptr)
        {
            auto hashString = std::string(hash);
            const NetworkUser * networkUser = GetUserByHash(hashString);
            if (networkUser != nullptr)
            {
                if (networkUser->Remove)
                {
                    json_array_remove(jsonUsers, i);
                    i--;
                }
                else
                {
                    networkUser->ToJson(jsonUser);
                    savedHashes.insert(hashString);
                }
            }
        }
    }

    // Add new users
    for (const auto &kvp : _usersByHash)
    {
        const NetworkUser * networkUser = kvp.second;
        if (!networkUser->Remove && savedHashes.find(networkUser->Hash) == savedHashes.end())
        {
            json_t * jsonUser = networkUser->ToJson();
            json_array_append_new(jsonUsers, jsonUser);
        }
    }

    Json::WriteToFile(path, jsonUsers, JSON_INDENT(4) | JSON_PRESERVE_ORDER);
    json_decref(jsonUsers);
}

void NetworkUserManager::UnsetUsersOfGroup(uint8_t groupId)
{
    for (const auto &kvp : _usersByHash)
    {
        NetworkUser * networkUser = kvp.second;
        if (networkUser->GroupId.HasValue() &&
            networkUser->GroupId.GetValue() == groupId)
        {
            networkUser->GroupId = nullptr;
        }
    }
}

void NetworkUserManager::RemoveUser(const std::string &hash)
{
    NetworkUser * networkUser = GetUserByHash(hash);
    if (networkUser != nullptr)
    {
        networkUser->Remove = true;
    }
}

NetworkUser * NetworkUserManager::GetUserByHash(const std::string &hash)
{
    auto it = _usersByHash.find(hash);
    if (it != _usersByHash.end())
    {
        return it->second;
    }
    return nullptr;
}

const NetworkUser * NetworkUserManager::GetUserByHash(const std::string &hash) const
{
    auto it = _usersByHash.find(hash);
    if (it != _usersByHash.end())
    {
        return it->second;
    }
    return nullptr;
}

const NetworkUser * NetworkUserManager::GetUserByName(const std::string &name) const
{
    for (auto kvp : _usersByHash)
    {
        const NetworkUser * networkUser = kvp.second;
        if (String::Equals(name.c_str(), networkUser->Name.c_str(), true))
        {
            return networkUser;
        }
    }
    return nullptr;
}

NetworkUser * NetworkUserManager::GetOrAddUser(const std::string &hash)
{
    NetworkUser * networkUser = GetUserByHash(hash);
    if (networkUser == nullptr)
    {
        networkUser = new NetworkUser();
        networkUser->Hash = hash;
        _usersByHash[hash] = networkUser;
    }
    return networkUser;
}

void NetworkUserManager::GetStorePath(utf8 * buffer, size_t bufferSize)
{
    platform_get_user_directory(buffer, nullptr, bufferSize);
    Path::Append(buffer, bufferSize, USER_STORE_FILENAME);
}

#endif
