/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Game.h"
#include "NetworkPacket.h"

#include <array>
#include <string>
#include <string_view>

namespace OpenRCT2::Network
{
    enum class Permission : uint32_t;

    class NetworkGroup final
    {
    public:
        std::array<uint8_t, 8> actionsAllowed{};
        uint8_t id = 0;

        /**
         * Creates a NetworkGroup object from a JSON object
         *
         * @param json JSON data source
         * @return A NetworkGroup object
         * @note json is deliberately left non-const: json_t behaviour changes when const
         */
        static NetworkGroup fromJson(const json_t& json);

        const std::string& getName() const noexcept;
        void setName(std::string_view name);

        void read(Packet& packet);
        void write(Packet& packet) const;
        void toggleActionPermission(Permission index);
        bool canPerformAction(Permission index) const noexcept;
        bool canPerformCommand(GameCommand command) const;

        /**
         * Serialise a NetworkGroup object into a JSON object
         *
         * @return JSON representation of the NetworkGroup object
         */
        json_t toJson() const;

    private:
        std::string _name;
    };
} // namespace OpenRCT2::Network
