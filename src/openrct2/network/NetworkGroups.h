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

static constexpr size_t kMaxGroups = std::numeric_limits<NetworkGroupId_t>::max();

static constexpr NetworkGroupId_t kGroupIdHost = static_cast<NetworkGroupId_t>(kMaxGroups - 1);
static constexpr NetworkGroupId_t kGroupIdAdmin = 0;

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
    NetworkGroupId_t GetDefaultId() const;

    NetworkGroup* Create(const std::string& name, bool immutable = false, bool canBeDefault = true);
    NetworkGroup* GetById(NetworkGroupId_t id) const;
    NetworkGroup* GetByName(const std::string& name) const;

    std::vector<NetworkGroup*> GetAll(bool excludeHost) const;
    size_t GetCount() const;

    void Serialise(DataSerialiser& ds);

private:
    void CreateDefaultGroups();

private:
    std::array<std::unique_ptr<NetworkGroup>, kMaxGroups> _groups;
    NetworkGroupId_t _defaultId = 0;
};
