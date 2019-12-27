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
#include "NetworkGroup.h"

#include <climits>
#include <string>

class NetworkGroups
{
public:
    NetworkGroups();

    void Reset();
    void Clear();
    bool Load();
    bool Save();
    bool Remove(const NetworkGroup* group);
    bool Remove(NetworkGroupId_t id);

    void SetDefault(const NetworkGroup* group);
    NetworkGroup* GetDefault() const;

    NetworkGroup* Create(const std::string& name);
    NetworkGroup* GetById(NetworkGroupId_t id) const;
    NetworkGroup* GetByName(const std::string& name) const;

    std::vector<NetworkGroup*> GetAll() const;
    size_t GetCount() const;

    void Serialise(DataSerialiser& ds);

private:
    void CreateDefaultGroups();

private:
    std::array<std::unique_ptr<NetworkGroup>, 256> _groups;
    NetworkGroupId_t _defaultId = 0;
};
