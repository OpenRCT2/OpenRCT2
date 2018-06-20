/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <array>
#include <jansson.h>
#include <string>
#include "NetworkPacket.h"
#include "../common.h"

class NetworkGroup final
{
public:
    std::array<uint8_t, 8>    ActionsAllowed{};
    uint8_t                   Id = 0;

    static NetworkGroup FromJson(const json_t * json);

    const std::string & GetName() const;
    void SetName(std::string name);

    void Read(NetworkPacket &packet);
    void Write(NetworkPacket &packet);
    void ToggleActionPermission(size_t index);
    bool CanPerformAction(size_t index) const;
    bool CanPerformCommand(int32_t command) const;

    json_t * ToJson() const;

private:
    std::string _name;
};
