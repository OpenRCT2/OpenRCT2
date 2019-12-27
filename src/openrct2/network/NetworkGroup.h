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
#include "NetworkPacket.h"
#include "NetworkTypes.h"

#include <array>
#include <jansson.h>
#include <string>

class NetworkGroup final
{
public:
    std::array<uint8_t, 8> ActionsAllowed{};
    NetworkGroupId_t Id = 0;

    NetworkGroup(bool immutable = false, bool canDefaultTo = true);
    static NetworkGroup FromJson(const json_t* json);

    const std::string& GetName() const;
    void SetName(std::string name);
    void ToggleActionPermission(size_t index);
    bool CanPerformAction(size_t index) const;
    bool CanPerformCommand(int32_t command) const;
    bool IsImmutable() const;
    bool CanBeDefault() const;

    void Serialise(DataSerialiser& ds);

    json_t* ToJson() const;

private:
    std::string _name;
    bool _immutable = false;
    bool _canDefaultTo = true;
    bool _hidden = false;
};
