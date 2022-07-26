/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../entity/Staff.h"
#include "../ride/Track.h"
#include "../world/Entrance.h"
#include "../world/Park.h"
#include "GameAction.h"
#include "GuestSetNameAction.h"
#include "MazeSetTrackAction.h"
#include "PlaceParkEntranceAction.h"
#include "PlacePeepSpawnAction.h"
#include "RideCreateAction.h"
#include "RideDemolishAction.h"
#include "RideSetNameAction.h"
#include "RideSetStatusAction.h"
#include "SetParkEntranceFeeAction.h"
#include "WallRemoveAction.h"

#pragma region PlaceParkEntranceAction
/**
 *
 *  rct2: 0x00666F4E
 */
money32 park_entrance_place_ghost(const CoordsXYZD& entranceLoc)
{
    park_entrance_remove_ghost();

    auto gameAction = PlaceParkEntranceAction(entranceLoc, gFootpathSelectedId);
    gameAction.SetFlags(GAME_COMMAND_FLAG_GHOST);

    auto result = GameActions::Execute(&gameAction);
    if (result.Error == GameActions::Status::Ok)
    {
        gParkEntranceGhostPosition = entranceLoc;
        gParkEntranceGhostExists = true;
    }
    return result.Cost;
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

    auto gameAction = RideCreateAction(listItem.Type, listItem.EntryIndex, colour1, colour2, gLastEntranceStyle);

    gameAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error != GameActions::Status::Ok)
            return;
        const auto rideIndex = result->GetData<RideId>();
        auto ride = get_ride(rideIndex);
        ride_construct(ride);
    });

    GameActions::Execute(&gameAction);
}

#pragma endregion

#pragma region RideSetStatusAction

void ride_set_status(Ride* ride, RideStatus status)
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

void guest_set_name(EntityId spriteIndex, const char* name)
{
    auto gameAction = GuestSetNameAction(spriteIndex, name);
    GameActions::Execute(&gameAction);
}
#pragma endregion
