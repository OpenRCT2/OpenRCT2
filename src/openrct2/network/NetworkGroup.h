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
#include "NetworkPacket.h"

#include <array>
#include <string>

enum class NetworkPermission : uint32_t;

class NetworkGroup final
{
public:
    std::array<uint8_t, 8> ActionsAllowed{};
    uint8_t Id = 0;

    /**
     * Creates a NetworkGroup object from a JSON object
     *
     * @param json JSON data source
     * @return A NetworkGroup object
     * @note json is deliberately left non-const: json_t behaviour changes when const
     */
    static NetworkGroup FromJson(json_t& json);

    const std::string& GetName() const;
    void SetName(std::string name);

    void Read(NetworkPacket& packet);
    void Write(NetworkPacket& packet);
    void ToggleActionPermission(NetworkPermission index);
    bool CanPerformAction(NetworkPermission index) const;
    bool CanPerformCommand(int32_t command) const;

    /**
     * Serialise a NetworkGroup object into a JSON object
     *
     * @return JSON representation of the NetworkGroup object
     */
    json_t ToJson() const;

private:
    std::string _name;
};
