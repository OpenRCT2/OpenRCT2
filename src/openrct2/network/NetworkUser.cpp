/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2::Network
{
    constexpr const utf8* kUserStoreFilename = "users.json";

    std::unique_ptr<User> User::fromJson(const json_t& jsonData)
    {
        Guard::Assert(jsonData.is_object(), "Network::User::fromJson expects parameter jsonData to be object");

        const std::string hash = Json::GetString(jsonData["hash"]);
        const std::string name = Json::GetString(jsonData["name"]);
        json_t jsonGroupId = jsonData["groupId"];

        std::unique_ptr<User> user = nullptr;
        if (!hash.empty() && !name.empty())
        {
            user = std::make_unique<User>();
            user->hash = hash;
            user->name = name;
            if (jsonGroupId.is_number_integer())
            {
                user->groupId = Json::GetNumber<uint8_t>(jsonGroupId);
            }
            user->remove = false;
        }
        return user;
    }

    json_t User::toJson() const
    {
        json_t jsonData;
        jsonData["hash"] = hash;
        jsonData["name"] = name;

        json_t jsonGroupId;
        if (groupId.has_value())
        {
            jsonGroupId = *groupId;
        }
        jsonData["groupId"] = jsonGroupId;

        return jsonData;
    }

    void UserManager::load()
    {
        const auto path = getStorePath();

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
                        auto networkUser = User::fromJson(jsonUser);
                        if (networkUser != nullptr)
                        {
                            _usersByHash[networkUser->hash] = std::move(networkUser);
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

    void UserManager::save()
    {
        const auto path = getStorePath();

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

            const auto networkUser = getUserByHash(hashString);
            if (networkUser != nullptr)
            {
                if (networkUser->remove)
                {
                    it = jsonUsers.erase(it);
                    // erase advances the iterator so make sure we don't do it again
                    continue;
                }

                // replace the existing element in jsonUsers
                *it = networkUser->toJson();
                savedHashes.insert(hashString);
            }

            it++;
        }

        // Add new users
        for (const auto& kvp : _usersByHash)
        {
            const auto& networkUser = kvp.second;
            if (!networkUser->remove && savedHashes.find(networkUser->hash) == savedHashes.end())
            {
                jsonUsers.push_back(networkUser->toJson());
            }
        }

        Json::WriteToFile(path, jsonUsers);
    }

    void UserManager::unsetUsersOfGroup(uint8_t groupId)
    {
        for (const auto& kvp : _usersByHash)
        {
            auto& networkUser = kvp.second;
            if (networkUser->groupId.has_value() && *networkUser->groupId == groupId)
            {
                networkUser->groupId = std::nullopt;
            }
        }
    }

    void UserManager::removeUser(const std::string& hash)
    {
        User* networkUser = const_cast<User*>(getUserByHash(hash));
        if (networkUser != nullptr)
        {
            networkUser->remove = true;
        }
    }

    const User* UserManager::getUserByHash(const std::string& hash) const
    {
        auto it = _usersByHash.find(hash);
        if (it != _usersByHash.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    const User* UserManager::getUserByName(const std::string& name) const
    {
        for (const auto& kvp : _usersByHash)
        {
            const auto& networkUser = kvp.second;
            if (String::iequals(name, networkUser->name))
            {
                return networkUser.get();
            }
        }
        return nullptr;
    }

    User* UserManager::getOrAddUser(const std::string& hash)
    {
        User* networkUser = const_cast<User*>(getUserByHash(hash));
        if (networkUser == nullptr)
        {
            auto newUser = std::make_unique<User>();
            newUser->hash = hash;
            networkUser = newUser.get();
            _usersByHash[hash] = std::move(newUser);
        }
        return networkUser;
    }

    u8string UserManager::getStorePath()
    {
        auto& env = GetContext()->GetPlatformEnvironment();
        return Path::Combine(env.GetDirectoryPath(DirBase::user), kUserStoreFilename);
    }
} // namespace OpenRCT2::Network

#endif
