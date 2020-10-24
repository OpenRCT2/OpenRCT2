/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#    include "NetworkUser.h"

#    include "../core/Console.hpp"
#    include "../core/Guard.hpp"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "../platform/Platform2.h"

#    include <unordered_set>

constexpr const utf8* USER_STORE_FILENAME = "users.json";

NetworkUser* NetworkUser::FromJson(json_t& jsonData)
{
    Guard::Assert(jsonData.is_object(), "NetworkUser::FromJson expects parameter jsonData to be object");

    const std::string hash = Json::GetString(jsonData["hash"]);
    const std::string name = Json::GetString(jsonData["name"]);
    json_t jsonGroupId = jsonData["groupId"];

    NetworkUser* user = nullptr;
    if (!hash.empty() && !name.empty())
    {
        user = new NetworkUser();
        user->Hash = hash;
        user->Name = name;
        if (jsonGroupId.is_number_integer())
        {
            user->GroupId = Json::GetNumber<uint8_t>(jsonGroupId);
        }
        user->Remove = false;
    }
    return user;
}

json_t NetworkUser::ToJson() const
{
    json_t jsonData;
    jsonData["hash"] = Hash;
    jsonData["name"] = Name;

    json_t jsonGroupId;
    if (GroupId.HasValue())
    {
        jsonGroupId = GroupId.GetValue();
    }
    jsonData["groupId"] = jsonGroupId;

    return jsonData;
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

void NetworkUserManager::Load()
{
    utf8 path[MAX_PATH];
    GetStorePath(path, sizeof(path));

    if (Platform::FileExists(path))
    {
        DisposeUsers();

        try
        {
            json_t jsonUsers = Json::ReadFromFile(path);
            for (auto& jsonUser : jsonUsers)
            {
                if (jsonUser.is_object())
                {
                    auto networkUser = NetworkUser::FromJson(jsonUser);
                    if (networkUser != nullptr)
                    {
                        _usersByHash[networkUser->Hash] = networkUser;
                    }
                }
            }
        }
        catch (const std::exception& ex)
        {
            Console::Error::WriteLine("Failed to read %s as JSON. %s", path, ex.what());
        }
    }
}

void NetworkUserManager::Save()
{
    utf8 path[MAX_PATH];
    GetStorePath(path, sizeof(path));

    json_t jsonUsers;
    try
    {
        if (Platform::FileExists(path))
        {
            jsonUsers = Json::ReadFromFile(path);
        }
    }
    catch (const std::exception&)
    {
    }

    // Update existing users
    std::unordered_set<std::string> savedHashes;
    for (auto it = jsonUsers.begin(); it != jsonUsers.end();)
    {
        json_t jsonUser = *it;
        if (!jsonUser.is_object())
        {
            continue;
        }
        std::string hashString = Json::GetString(jsonUser["hash"]);

        const auto networkUser = GetUserByHash(hashString);
        if (networkUser != nullptr)
        {
            if (networkUser->Remove)
            {
                it = jsonUsers.erase(it);
                // erase advances the iterator so make sure we don't do it again
                continue;
            }
            else
            {
                // replace the existing element in jsonUsers
                *it = networkUser->ToJson();
                savedHashes.insert(hashString);
            }
        }

        it++;
    }

    // Add new users
    for (const auto& kvp : _usersByHash)
    {
        const NetworkUser* networkUser = kvp.second;
        if (!networkUser->Remove && savedHashes.find(networkUser->Hash) == savedHashes.end())
        {
            jsonUsers.push_back(networkUser->ToJson());
        }
    }

    Json::WriteToFile(path, jsonUsers);
}

void NetworkUserManager::UnsetUsersOfGroup(uint8_t groupId)
{
    for (const auto& kvp : _usersByHash)
    {
        NetworkUser* networkUser = kvp.second;
        if (networkUser->GroupId.HasValue() && networkUser->GroupId.GetValue() == groupId)
        {
            networkUser->GroupId = nullptr;
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
