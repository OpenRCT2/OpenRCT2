/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/JsonFwd.hpp"
#include "../core/StringTypes.h"

#include <memory>
#include <optional>
#include <unordered_map>

class NetworkUser final
{
public:
    std::string Hash;
    std::string Name;
    std::optional<uint8_t> GroupId;
    bool Remove;

    /**
     * Creates a NetworkUser object from a JSON object
     * @param jsonData Must be a JSON node of type object
     * @return Pointer to a new NetworkUser object
     * @note jsonData is deliberately left non-const: json_t behaviour changes when const
     */
    static std::unique_ptr<NetworkUser> FromJson(const json_t& jsonData);

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

    const NetworkUser* GetUserByHash(const std::string& hash) const;
    const NetworkUser* GetUserByName(const std::string& name) const;
    NetworkUser* GetOrAddUser(const std::string& hash);

private:
    std::unordered_map<std::string, std::unique_ptr<NetworkUser>> _usersByHash;

    static u8string GetStorePath();
};
