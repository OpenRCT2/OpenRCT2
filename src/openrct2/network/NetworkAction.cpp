/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "NetworkAction.h"

    #include "../localisation/StringIds.h"

    #include <algorithm>

namespace OpenRCT2::Network
{
    Permission NetworkActions::findCommand(GameCommand command)
    {
        auto it = std::find_if(kActions.begin(), kActions.end(), [&command](NetworkAction const& action) {
            for (GameCommand currentCommand : action.commands)
            {
                if (currentCommand == command)
                {
                    return true;
                }
            }
            return false;
        });
        if (it != kActions.end())
        {
            return static_cast<Permission>(it - kActions.begin());
        }
        return Permission::count;
    }

    Permission NetworkActions::findCommandByPermissionName(const std::string& permission_name)
    {
        auto it = std::find_if(kActions.begin(), kActions.end(), [&permission_name](NetworkAction const& action) {
            return action.permissionName == permission_name;
        });
        if (it != kActions.end())
        {
            return static_cast<Permission>(it - kActions.begin());
        }
        return Permission::count;
    }

    const std::array<NetworkAction, static_cast<size_t>(Permission::count)> NetworkActions::kActions = {
        NetworkAction{
            STR_ACTION_CHAT,
            "PERMISSION_CHAT",
            {},
        },
        NetworkAction{
            STR_ACTION_TERRAFORM,
            "PERMISSION_TERRAFORM",
            {
                GameCommand::setLandHeight,
                GameCommand::raiseLand,
                GameCommand::lowerLand,
                GameCommand::editLandSmooth,
                GameCommand::changeSurfaceStyle,
            },
        },
        NetworkAction{
            STR_ACTION_SET_WATER_LEVEL,
            "PERMISSION_SET_WATER_LEVEL",
            {
                GameCommand::setWaterHeight,
                GameCommand::raiseWater,
                GameCommand::lowerWater,
            },
        },
        NetworkAction{
            STR_ACTION_TOGGLE_PAUSE,
            "PERMISSION_TOGGLE_PAUSE",
            {
                GameCommand::togglePause,
            },
        },
        NetworkAction{
            STR_ACTION_CREATE_RIDE,
            "PERMISSION_CREATE_RIDE",
            {
                GameCommand::createRide,
            },
        },
        NetworkAction{
            STR_ACTION_REMOVE_RIDE,
            "PERMISSION_REMOVE_RIDE",
            {
                GameCommand::demolishRide,
            },
        },
        NetworkAction{
            STR_ACTION_BUILD_RIDE,
            "PERMISSION_BUILD_RIDE",
            {
                GameCommand::placeTrack,
                GameCommand::removeTrack,
                GameCommand::setMazeTrack,
                GameCommand::placeTrackDesign,
                GameCommand::placeMazeDesign,
                GameCommand::placeRideEntranceOrExit,
                GameCommand::removeRideEntranceOrExit,
            },
        },
        NetworkAction{
            STR_ACTION_RIDE_PROPERTIES,
            "PERMISSION_RIDE_PROPERTIES",
            {
                GameCommand::setRideName,
                GameCommand::setRideAppearance,
                GameCommand::setRideStatus,
                GameCommand::setRideVehicles,
                GameCommand::setRideSetting,
                GameCommand::setRidePrice,
                GameCommand::setBrakesSpeed,
                GameCommand::setColourScheme,
            },
        },
        NetworkAction{
            STR_ACTION_SCENERY,
            "PERMISSION_SCENERY",
            {
                GameCommand::removeScenery,
                GameCommand::placeScenery,
                GameCommand::setBrakesSpeed,
                GameCommand::removeWall,
                GameCommand::placeWall,
                GameCommand::removeLargeScenery,
                GameCommand::placeLargeScenery,
                GameCommand::placeBanner,
                GameCommand::removeBanner,
                GameCommand::setSceneryColour,
                GameCommand::setWallColour,
                GameCommand::setLargeSceneryColour,
                GameCommand::setBannerColour,
                GameCommand::setBannerName,
                GameCommand::setSignName,
                GameCommand::setBannerStyle,
                GameCommand::setSignStyle,
            },
        },
        NetworkAction{
            STR_ACTION_PATH,
            "PERMISSION_PATH",
            {
                GameCommand::placePath,
                GameCommand::placePathLayout,
                GameCommand::removePath,
                GameCommand::placeFootpathAddition,
                GameCommand::removeFootpathAddition,
            },
        },
        NetworkAction{
            STR_ACTION_CLEAR_LANDSCAPE,
            "PERMISSION_CLEAR_LANDSCAPE",
            {
                GameCommand::clearScenery,
            },
        },
        NetworkAction{
            STR_ACTION_GUEST,
            "PERMISSION_GUEST",
            {
                GameCommand::setGuestName,
                GameCommand::pickupGuest,
                GameCommand::balloonPress,
                GameCommand::guestSetFlags,
            },
        },
        NetworkAction{
            STR_ACTION_STAFF,
            "PERMISSION_STAFF",
            {
                GameCommand::hireNewStaffMember,
                GameCommand::setStaffPatrol,
                GameCommand::fireStaffMember,
                GameCommand::setStaffOrders,
                GameCommand::setStaffCostume,
                GameCommand::setStaffColour,
                GameCommand::setStaffName,
                GameCommand::pickupStaff,
            },
        },
        NetworkAction{
            STR_ACTION_PARK_PROPERTIES,
            "PERMISSION_PARK_PROPERTIES",
            {
                GameCommand::setParkName,
                GameCommand::setParkOpen,
                GameCommand::setParkEntranceFee,
                GameCommand::setLandOwnership,
                GameCommand::buyLandRights,
                GameCommand::placeParkEntrance,
                GameCommand::removeParkEntrance,
                GameCommand::placePeepSpawn,
                GameCommand::changeMapSize,
            },
        },
        NetworkAction{
            STR_ACTION_PARK_FUNDING,
            "PERMISSION_PARK_FUNDING",
            {
                GameCommand::setCurrentLoan,
                GameCommand::setResearchFunding,
                GameCommand::startMarketingCampaign,
            },
        },
        NetworkAction{
            STR_ACTION_KICK_PLAYER,
            "PERMISSION_KICK_PLAYER",
            {
                GameCommand::kickPlayer,
            },
        },
        NetworkAction{
            STR_ACTION_MODIFY_GROUPS,
            "PERMISSION_MODIFY_GROUPS",
            {
                GameCommand::modifyGroups,
            },
        },
        NetworkAction{
            STR_ACTION_SET_PLAYER_GROUP,
            "PERMISSION_SET_PLAYER_GROUP",
            {
                GameCommand::setPlayerGroup,
            },
        },
        NetworkAction{
            STR_ACTION_CHEAT,
            "PERMISSION_CHEAT",
            {
                GameCommand::cheat,
                GameCommand::setDate,
                GameCommand::freezeRideRating,
                GameCommand::setRideVisibility,
                GameCommand::setRideBreakdown,
            },
        },
        NetworkAction{
            STR_ACTION_TOGGLE_SCENERY_CLUSTER,
            "PERMISSION_TOGGLE_SCENERY_CLUSTER",
            {},
        },
        NetworkAction{
            STR_ACTION_PASSWORDLESS_LOGIN,
            "PERMISSION_PASSWORDLESS_LOGIN",
            {},
        },
        NetworkAction{
            STR_ACTION_MODIFY_TILE,
            "PERMISSION_MODIFY_TILE",
            {
                GameCommand::modifyTile,
            },
        },
        NetworkAction{
            STR_ACTION_EDIT_SCENARIO_OPTIONS,
            "PERMISSION_EDIT_SCENARIO_OPTIONS",
            {
                GameCommand::editScenarioOptions,
            },
        },
        NetworkAction{
            STR_ACTION_PATH_DRAG_AREA,
            "PERMISSION_DRAG_PATH_AREA",
            {},
        },
    };
} // namespace OpenRCT2::Network

#endif
