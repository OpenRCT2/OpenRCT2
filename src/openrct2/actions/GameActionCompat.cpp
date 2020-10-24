/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameAction.h"
#include "GuestSetNameAction.hpp"
#include "MazeSetTrackAction.hpp"
#include "PlaceParkEntranceAction.hpp"
#include "PlacePeepSpawnAction.hpp"
#include "RideCreateAction.hpp"
#include "RideDemolishAction.hpp"
#include "RideSetName.hpp"
#include "RideSetStatus.hpp"
#include "SetParkEntranceFeeAction.hpp"
#include "StaffSetNameAction.hpp"
#include "WallRemoveAction.hpp"

#pragma region PlaceParkEntranceAction
/**
 *
 *  rct2: 0x00666F4E
 */
money32 park_entrance_place_ghost(const CoordsXYZD& entranceLoc)
{
    park_entrance_remove_ghost();

    auto gameAction = PlaceParkEntranceAction(entranceLoc);
    gameAction.SetFlags(GAME_COMMAND_FLAG_GHOST);

    auto result = GameActions::Execute(&gameAction);
    if (result->Error == GA_ERROR::OK)
    {
        gParkEntranceGhostPosition = entranceLoc;
        gParkEntranceGhostExists = true;
    }
    return result->Cost;
}
#pragma endregion

#pragma region SetParkEntranceFeeAction
void park_set_entrance_fee(money32 fee)
{
    auto gameAction = SetParkEntranceFeeAction(static_cast<money16>(fee));
    GameActions::Execute(&gameAction);
}
#pragma endregion

#pragma region RideCreateAction
/**
 *
 *  rct2: 0x006B4800
 */
void ride_construct_new(RideSelection listItem)
{
    int32_t rideEntryIndex = ride_get_entry_index(listItem.Type, listItem.EntryIndex);
    int32_t colour1 = ride_get_random_colour_preset_index(listItem.Type);
    int32_t colour2 = ride_get_unused_preset_vehicle_colour(rideEntryIndex);

    auto gameAction = RideCreateAction(listItem.Type, listItem.EntryIndex, colour1, colour2);

    gameAction.SetCallback([](const GameAction* ga, const RideCreateGameActionResult* result) {
        if (result->Error != GA_ERROR::OK)
            return;

        auto ride = get_ride(result->rideIndex);
        ride_construct(ride);
    });

    GameActions::Execute(&gameAction);
}

#pragma endregion

#pragma region RideSetStatusAction

void ride_set_status(Ride* ride, int32_t status)
{
    auto gameAction = RideSetStatusAction(ride->id, status);
    GameActions::Execute(&gameAction);
}

#pragma endregion

#pragma region RideSetNameAction
void ride_set_name(Ride* ride, const char* name, uint32_t flags)
{
    auto gameAction = RideSetNameAction(ride->id, name);
    gameAction.SetFlags(flags);
    GameActions::Execute(&gameAction);
}
#pragma endregion

#pragma region RideModifyAction
void ride_action_modify(Ride* ride, int32_t modifyType, int32_t flags)
{
    auto gameAction = RideDemolishAction(ride->id, modifyType);
    gameAction.SetFlags(flags);

    GameActions::Execute(&gameAction);
}
#pragma endregion

#pragma region GuestSetName

void guest_set_name(uint16_t spriteIndex, const char* name)
{
    auto gameAction = GuestSetNameAction(spriteIndex, name);
    GameActions::Execute(&gameAction);
}
#pragma endregion

#pragma region StaffSetName

void staff_set_name(uint16_t spriteIndex, const char* name)
{
    auto gameAction = StaffSetNameAction(spriteIndex, name);
    GameActions::Execute(&gameAction);
}
#pragma endregion

#pragma region MazeSetTrack
money32 maze_set_track(
    uint16_t x, uint16_t y, uint16_t z, uint8_t flags, bool initialPlacement, uint8_t direction, ride_id_t rideIndex,
    uint8_t mode)
{
    auto gameAction = MazeSetTrackAction({ x, y, z, direction }, initialPlacement, rideIndex, mode);
    gameAction.SetFlags(flags);

    GameActionResult::Ptr res;

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
        res = GameActions::Query(&gameAction);
    else
        res = GameActions::Execute(&gameAction);

    // NOTE: ride_construction_tooldown_construct requires them to be set.
    // Refactor result type once theres no C code referencing this function.
    if (auto title = res->ErrorTitle.AsStringId())
        gGameCommandErrorTitle = *title;
    else
        gGameCommandErrorTitle = STR_NONE;
    if (auto message = res->ErrorMessage.AsStringId())
        gGameCommandErrorText = *message;
    else
        gGameCommandErrorText = STR_NONE;

    if (res->Error != GA_ERROR::OK)
    {
        return MONEY32_UNDEFINED;
    }

    return res->Cost;
}
#pragma endregion
