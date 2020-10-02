/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/Nullable.hpp"

#include <map>
#include <nlohmann/json_fwd.hpp>
#include <string>

using json_t = nlohmann::json;

class NetworkUser final
{
public:
    std::string Hash;
    std::string Name;
    Nullable<uint8_t> GroupId;
    bool Remove;

    /**
     * Creates a NetworkUser object from a JSON object
     * @param jsonData Must be a JSON node of type object
     * @return Pointer to a new NetworkUser object
     * @note jsonData is deliberately left non-const: json_t behaviour changes when const
     */
    static NetworkUser* FromJson(json_t& jsonData);

    /**
     * Serialise a NetworkUser object into a JSON object
     *
     * @return JSON representation of the NetworkUser object
     */
    json_t ToJson() const;
};

class NetworkUserManager final
{
public:
    ~NetworkUserManager();

    void Load();

    /**
     * @brief NetworkUserManager::Save
     * Reads mappings from JSON, updates them in-place and saves JSON.
     *
     * Useful for retaining custom entries in JSON file.
     */
    void Save();

    void UnsetUsersOfGroup(uint8_t groupId);
    void RemoveUser(const std::string& hash);

    NetworkUser* GetUserByHash(const std::string& hash);
    const NetworkUser* GetUserByHash(const std::string& hash) const;
    const NetworkUser* GetUserByName(const std::string& name) const;
    NetworkUser* GetOrAddUser(const std::string& hash);

private:
    std::map<std::string, NetworkUser*> _usersByHash;

    void DisposeUsers();
    static void GetStorePath(utf8* buffer, size_t bufferSize);
};
