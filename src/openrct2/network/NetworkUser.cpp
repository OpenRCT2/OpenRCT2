#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

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
            user->GroupId = (uint8)json_integer_value(jsonGroupId);
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
        catch (const Exception &ex)
        {
            Console::Error::WriteLine("Failed to read %s as JSON. %s", path, ex.GetMessage());
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
    catch (const Exception &)
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

void NetworkUserManager::UnsetUsersOfGroup(uint8 groupId)
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
