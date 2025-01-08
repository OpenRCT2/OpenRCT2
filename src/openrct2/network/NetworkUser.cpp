/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "NetworkUser.h"

    #include "../Context.h"
    #include "../PlatformEnvironment.h"
    #include "../core/Console.hpp"
    #include "../core/File.h"
    #include "../core/Guard.hpp"
    #include "../core/Json.hpp"
    #include "../core/Path.hpp"
    #include "../core/String.hpp"

    #include <unordered_set>

using namespace OpenRCT2;

constexpr const utf8* kUserStoreFilename = "users.json";

std::unique_ptr<NetworkUser> NetworkUser::FromJson(const json_t& jsonData)
{
    Guard::Assert(jsonData.is_object(), "NetworkUser::FromJson expects parameter jsonData to be object");

    const std::string hash = Json::GetString(jsonData["hash"]);
    const std::string name = Json::GetString(jsonData["name"]);
    json_t jsonGroupId = jsonData["groupId"];

    std::unique_ptr<NetworkUser> user = nullptr;
    if (!hash.empty() && !name.empty())
    {
        user = std::make_unique<NetworkUser>();
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
    if (GroupId.has_value())
    {
        jsonGroupId = *GroupId;
    }
    jsonData["groupId"] = jsonGroupId;

    return jsonData;
}

void NetworkUserManager::Load()
{
    const auto path = GetStorePath();

    if (File::Exists(path))
    {
        _usersByHash.clear();

        try
        {
            json_t jsonUsers = Json::ReadFromFile(path);
            for (const auto& jsonUser : jsonUsers)
            {
                if (jsonUser.is_object())
                {
                    auto networkUser = NetworkUser::FromJson(jsonUser);
                    if (networkUser != nullptr)
                    {
                        _usersByHash[networkUser->Hash] = std::move(networkUser);
                    }
                }
            }
        }
        catch (const std::exception& ex)
        {
            Console::Error::WriteLine("Failed to read %s as JSON. %s", path.c_str(), ex.what());
        }
    }
}

void NetworkUserManager::Save()
{
    const auto path = GetStorePath();

    json_t jsonUsers;
    try
    {
        if (File::Exists(path))
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

            // replace the existing element in jsonUsers
            *it = networkUser->ToJson();
            savedHashes.insert(hashString);
        }

        it++;
    }

    // Add new users
    for (const auto& kvp : _usersByHash)
    {
        const auto& networkUser = kvp.second;
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
        auto& networkUser = kvp.second;
        if (networkUser->GroupId.has_value() && *networkUser->GroupId == groupId)
        {
            networkUser->GroupId = std::nullopt;
        }
    }
}

void NetworkUserManager::RemoveUser(const std::string& hash)
{
    NetworkUser* networkUser = const_cast<NetworkUser*>(GetUserByHash(hash));
    if (networkUser != nullptr)
    {
        networkUser->Remove = true;
    }
}

const NetworkUser* NetworkUserManager::GetUserByHash(const std::string& hash) const
{
    auto it = _usersByHash.find(hash);
    if (it != _usersByHash.end())
    {
        return it->second.get();
    }
    return nullptr;
}

const NetworkUser* NetworkUserManager::GetUserByName(const std::string& name) const
{
    for (const auto& kvp : _usersByHash)
    {
        const auto& networkUser = kvp.second;
        if (String::iequals(name, networkUser->Name))
        {
            return networkUser.get();
        }
    }
    return nullptr;
}

NetworkUser* NetworkUserManager::GetOrAddUser(const std::string& hash)
{
    NetworkUser* networkUser = const_cast<NetworkUser*>(GetUserByHash(hash));
    if (networkUser == nullptr)
    {
        auto newNetworkUser = std::make_unique<NetworkUser>();
        newNetworkUser->Hash = hash;
        networkUser = newNetworkUser.get();
        _usersByHash[hash] = std::move(newNetworkUser);
    }
    return networkUser;
}

u8string NetworkUserManager::GetStorePath()
{
    auto env = OpenRCT2::GetContext()->GetPlatformEnvironment();
    return Path::Combine(env->GetDirectoryPath(OpenRCT2::DIRBASE::USER), kUserStoreFilename);
}

#endif
