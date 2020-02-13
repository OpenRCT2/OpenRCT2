/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/Nullable.hpp"
#include "NetworkGroups.h"

#include <jansson.h>
#include <map>
#include <string>

struct NetworkUser
{
    std::string Hash;
    std::string Name;
    NetworkGroupId_t GroupId;
    bool Remove;
};

class NetworkUserManager final
{
public:
    ~NetworkUserManager();

    void Load(NetworkGroups& groups);

    /**
     * @brief NetworkUserManager::Save
     * Reads mappings from JSON, updates them in-place and saves JSON.
     *
     * Useful for retaining custom entries in JSON file.
     */
    void Save(NetworkGroups& groups);

    void SwitchGroupUsers(NetworkGroupId_t oldGroupId, NetworkGroupId_t newGroupId);
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
