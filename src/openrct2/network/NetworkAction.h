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
#include "../localisation/StringIdType.h"

#include <array>
#include <string>
#include <vector>

namespace OpenRCT2::Network
{
    enum class Permission : uint32_t
    {
        chat,
        terraform,
        setWaterLevel,
        togglePause,
        createRide,
        removeRide,
        buildRide,
        rideProperties,
        scenery,
        path,
        clearLandscape,
        guest,
        staff,
        parkProperties,
        parkFunding,
        kickPlayer,
        modifyGroups,
        setPlayerGroup,
        cheat,
        toggleSceneryCluster,
        passwordlessLogin,
        modifyTile,
        editScenarioOptions,
        dragPathArea,

        count,
    };

    class NetworkAction final
    {
    public:
        StringId Name;
        std::string PermissionName;
        std::vector<GameCommand> Commands;
    };

    class NetworkActions final
    {
    public:
        static const std::array<NetworkAction, static_cast<size_t>(Permission::count)> Actions;

        static Permission FindCommand(GameCommand command);
        static Permission FindCommandByPermissionName(const std::string& permission_name);
    };
} // namespace OpenRCT2::Network
