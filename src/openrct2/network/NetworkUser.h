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

#pragma once

#ifdef __cplusplus

#include "../common.h"
#include "../core/Nullable.hpp"

#include <jansson.h>
#include <map>
#include <string>

class NetworkUser final
{
public:
    std::string         Hash;
    std::string         Name;
    Nullable<uint8>     GroupId;
    bool                Remove;

    static NetworkUser * FromJson(json_t * json);

    json_t * ToJson() const;
    json_t * ToJson(json_t * json) const;
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

    void UnsetUsersOfGroup(uint8 groupId);
    void RemoveUser(const std::string &hash);

    NetworkUser * GetUserByHash(const std::string &hash);
    const NetworkUser * GetUserByHash(const std::string &hash) const;
    const NetworkUser * GetUserByName(const std::string &name) const;
    NetworkUser * GetOrAddUser(const std::string &hash);

private:
    std::map<std::string, NetworkUser*> _usersByHash;

    void DisposeUsers();
    static void GetStorePath(utf8 * buffer, size_t bufferSize);
};

#endif
