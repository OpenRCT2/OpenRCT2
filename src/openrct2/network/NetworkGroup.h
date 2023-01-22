/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Game.h"
#include "../common.h"
#include "NetworkPacket.h"

#include <array>
#include <string>
#include <string_view>
namespace OpenRCT2
{
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
        static NetworkGroup FromJson(const json_t& json);

        const std::string& GetName() const noexcept;
        void SetName(std::string_view name);

        void Read(NetworkPacket& packet);
        void Write(NetworkPacket& packet) const;
        void ToggleActionPermission(NetworkPermission index);
        bool CanPerformAction(NetworkPermission index) const noexcept;
        bool CanPerformCommand(GameCommand command) const;

        /**
         * Serialise a NetworkGroup object into a JSON object
         *
         * @return JSON representation of the NetworkGroup object
         */
        json_t ToJson() const;

    private:
        std::string _name;
    };
} // namespace OpenRCT2
