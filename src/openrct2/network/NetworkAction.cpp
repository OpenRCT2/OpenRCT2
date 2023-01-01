/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#    include "NetworkAction.h"

#    include "../Game.h"
#    include "../localisation/StringIds.h"

#    include <algorithm>

NetworkPermission NetworkActions::FindCommand(GameCommand command)
{
    auto it = std::find_if(Actions.begin(), Actions.end(), [&command](NetworkAction const& action) {
        for (GameCommand currentCommand : action.Commands)
        {
            if (currentCommand == command)
            {
                return true;
            }
        }
        return false;
    });
    if (it != Actions.end())
    {
        return static_cast<NetworkPermission>(it - Actions.begin());
    }
    return NetworkPermission::Count;
}

NetworkPermission NetworkActions::FindCommandByPermissionName(const std::string& permission_name)
{
    auto it = std::find_if(Actions.begin(), Actions.end(), [&permission_name](NetworkAction const& action) {
        return action.PermissionName == permission_name;
    });
    if (it != Actions.end())
    {
        return static_cast<NetworkPermission>(it - Actions.begin());
    }
    return NetworkPermission::Count;
}

const std::array<NetworkAction, static_cast<size_t>(NetworkPermission::Count)> NetworkActions::Actions = {
    NetworkAction{
        STR_ACTION_CHAT,
        "PERMISSION_CHAT",
        {},
    },
    NetworkAction{
        STR_ACTION_TERRAFORM,
        "PERMISSION_TERRAFORM",
        {
            GameCommand::SetLandHeight,
            GameCommand::RaiseLand,
            GameCommand::LowerLand,
            GameCommand::EditLandSmooth,
            GameCommand::ChangeSurfaceStyle,
        },
    },
    NetworkAction{
        STR_ACTION_SET_WATER_LEVEL,
        "PERMISSION_SET_WATER_LEVEL",
        {
            GameCommand::SetWaterHeight,
            GameCommand::RaiseWater,
            GameCommand::LowerWater,
        },
    },
    NetworkAction{
        STR_ACTION_TOGGLE_PAUSE,
        "PERMISSION_TOGGLE_PAUSE",
        {
            GameCommand::TogglePause,
        },
    },
    NetworkAction{
        STR_ACTION_CREATE_RIDE,
        "PERMISSION_CREATE_RIDE",
        {
            GameCommand::CreateRide,
        },
    },
    NetworkAction{
        STR_ACTION_REMOVE_RIDE,
        "PERMISSION_REMOVE_RIDE",
        {
            GameCommand::DemolishRide,
        },
    },
    NetworkAction{
        STR_ACTION_BUILD_RIDE,
        "PERMISSION_BUILD_RIDE",
        {
            GameCommand::PlaceTrack,
            GameCommand::RemoveTrack,
            GameCommand::SetMazeTrack,
            GameCommand::PlaceTrackDesign,
            GameCommand::PlaceMazeDesign,
            GameCommand::PlaceRideEntranceOrExit,
            GameCommand::RemoveRideEntranceOrExit,
        },
    },
    NetworkAction{
        STR_ACTION_RIDE_PROPERTIES,
        "PERMISSION_RIDE_PROPERTIES",
        {
            GameCommand::SetRideName,
            GameCommand::SetRideAppearance,
            GameCommand::SetRideStatus,
            GameCommand::SetRideVehicles,
            GameCommand::SetRideSetting,
            GameCommand::SetRidePrice,
            GameCommand::SetBrakesSpeed,
            GameCommand::SetColourScheme,
        },
    },
    NetworkAction{
        STR_ACTION_SCENERY,
        "PERMISSION_SCENERY",
        {
            GameCommand::RemoveScenery,
            GameCommand::PlaceScenery,
            GameCommand::SetBrakesSpeed,
            GameCommand::RemoveWall,
            GameCommand::PlaceWall,
            GameCommand::RemoveLargeScenery,
            GameCommand::PlaceLargeScenery,
            GameCommand::PlaceBanner,
            GameCommand::RemoveBanner,
            GameCommand::SetSceneryColour,
            GameCommand::SetWallColour,
            GameCommand::SetLargeSceneryColour,
            GameCommand::SetBannerColour,
            GameCommand::SetBannerName,
            GameCommand::SetSignName,
            GameCommand::SetBannerStyle,
            GameCommand::SetSignStyle,
        },
    },
    NetworkAction{
        STR_ACTION_PATH,
        "PERMISSION_PATH",
        {
            GameCommand::PlacePath,
            GameCommand::PlacePathFromTrack,
            GameCommand::RemovePath,
            GameCommand::PlaceFootpathAddition,
            GameCommand::RemoveFootpathAddition,
        },
    },
    NetworkAction{
        STR_ACTION_CLEAR_LANDSCAPE,
        "PERMISSION_CLEAR_LANDSCAPE",
        {
            GameCommand::ClearScenery,
        },
    },
    NetworkAction{
        STR_ACTION_GUEST,
        "PERMISSION_GUEST",
        {
            GameCommand::SetGuestName,
            GameCommand::PickupGuest,
            GameCommand::BalloonPress,
            GameCommand::GuestSetFlags,
        },
    },
    NetworkAction{
        STR_ACTION_STAFF,
        "PERMISSION_STAFF",
        {
            GameCommand::HireNewStaffMember,
            GameCommand::SetStaffPatrol,
            GameCommand::FireStaffMember,
            GameCommand::SetStaffOrders,
            GameCommand::SetStaffCostume,
            GameCommand::SetStaffColour,
            GameCommand::SetStaffName,
            GameCommand::PickupStaff,
        },
    },
    NetworkAction{
        STR_ACTION_PARK_PROPERTIES,
        "PERMISSION_PARK_PROPERTIES",
        {
            GameCommand::SetParkName,
            GameCommand::SetParkOpen,
            GameCommand::SetParkEntranceFee,
            GameCommand::SetLandOwnership,
            GameCommand::BuyLandRights,
            GameCommand::PlaceParkEntrance,
            GameCommand::RemoveParkEntrance,
            GameCommand::PlacePeepSpawn,
            GameCommand::ChangeMapSize,
        },
    },
    NetworkAction{
        STR_ACTION_PARK_FUNDING,
        "PERMISSION_PARK_FUNDING",
        {
            GameCommand::SetCurrentLoan,
            GameCommand::SetResearchFunding,
            GameCommand::StartMarketingCampaign,
        },
    },
    NetworkAction{
        STR_ACTION_KICK_PLAYER,
        "PERMISSION_KICK_PLAYER",
        {
            GameCommand::KickPlayer,
        },
    },
    NetworkAction{
        STR_ACTION_MODIFY_GROUPS,
        "PERMISSION_MODIFY_GROUPS",
        {
            GameCommand::ModifyGroups,
        },
    },
    NetworkAction{
        STR_ACTION_SET_PLAYER_GROUP,
        "PERMISSION_SET_PLAYER_GROUP",
        {
            GameCommand::SetPlayerGroup,
        },
    },
    NetworkAction{
        STR_ACTION_CHEAT,
        "PERMISSION_CHEAT",
        {
            GameCommand::Cheat,
            GameCommand::SetDate,
            GameCommand::FreezeRideRating,
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
            GameCommand::ModifyTile,
        },
    },
    NetworkAction{
        STR_ACTION_EDIT_SCENARIO_OPTIONS,
        "PERMISSION_EDIT_SCENARIO_OPTIONS",
        {
            GameCommand::EditScenarioOptions,
            GameCommand::SetClimate,
        },
    },
};

#endif
