#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include "GameAction.h"
#include "PlaceParkEntranceAction.hpp"
#include "SetParkEntranceFeeAction.hpp"
#include "RideCreateAction.hpp"
#include "RideSetStatus.hpp"
#include "RideSetName.hpp"

extern "C"
{
#pragma region PlaceParkEntranceAction
    money32 place_park_entrance(sint16 x, sint16 y, sint16 z, uint8 direction)
    {
        auto gameAction = PlaceParkEntranceAction(x, y, z, direction);
        auto result = GameActions::Execute(&gameAction);
        if (result->Error == GA_ERROR::OK)
        {
            return 0;
        }
        else
        {
            return MONEY32_UNDEFINED;
        }
    }

    /**
    *
    *  rct2: 0x006666E7
    */
    void game_command_place_park_entrance(sint32* eax,
                                          sint32* ebx,
                                          sint32* ecx,
                                          sint32* edx,
                                          sint32* esi,
                                          sint32* edi,
                                          sint32* ebp)
    {
        Guard::Assert(false, "GAME_COMMAND_PLACE_PARK_ENTRANCE DEPRECATED");
    }

    /**
    *
    *  rct2: 0x00666F4E
    */
    money32 park_entrance_place_ghost(sint32 x, sint32 y, sint32 z, sint32 direction)
    {
        park_entrance_remove_ghost();

        auto gameAction = PlaceParkEntranceAction(x, y, z, direction);
        gameAction.SetFlags(GAME_COMMAND_FLAG_GHOST);

        auto result = GameActions::Execute(&gameAction);
        if (result->Error == GA_ERROR::OK)
        {
            gParkEntranceGhostPosition.x = x;
            gParkEntranceGhostPosition.y = y;
            gParkEntranceGhostPosition.z = z;
            gParkEntranceGhostDirection = direction;
            gParkEntranceGhostExists = true;
        }
        return result->Cost;
    }
#pragma endregion

#pragma region SetParkEntranceFeeAction
    void park_set_entrance_fee(money32 fee)
    {
        auto gameAction = SetParkEntranceFeeAction((money16)fee);
        GameActions::Execute(&gameAction);
    }

    void game_command_set_park_entrance_fee(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
    {
        money16 fee = (money16)(*edi & 0xFFFF);
        auto gameAction = SetParkEntranceFeeAction(fee);
        GameActions::Execute(&gameAction);
    }
#pragma endregion

#pragma region RideCreateAction
    /**
    *
    *  rct2: 0x006B4800
    */
    void ride_construct_new(ride_list_item listItem)
    {
        sint32 rideEntryIndex = ride_get_entry_index(listItem.type, listItem.entry_index);
        sint32 colour1 = ride_get_random_colour_preset_index(listItem.type);
        sint32 colour2 = ride_get_unused_preset_vehicle_colour(listItem.type, rideEntryIndex);

        auto gameAction = RideCreateAction(listItem.type, listItem.entry_index, colour1, colour2);

        gameAction.SetCallback([](const GameAction *ga, const RideCreateGameActionResult * result)
        {
            if (result->Error != GA_ERROR::OK)
                return;

            ride_construct(result->rideIndex);
        });

        GameActions::Execute(&gameAction);
    }

    money32 ride_create_command(sint32 type, sint32 subType, sint32 flags, uint8 *outRideIndex, uint8 *outRideColour)
    {
        sint32 rideEntryIndex = ride_get_entry_index(type, subType);
        sint32 colour1 = ride_get_random_colour_preset_index(type);
        sint32 colour2 = ride_get_unused_preset_vehicle_colour(type, rideEntryIndex);

        auto gameAction = RideCreateAction(type, subType, colour1, colour2);
        gameAction.SetFlags(flags);

        auto r = GameActions::Execute(&gameAction);
        const RideCreateGameActionResult *res = static_cast<RideCreateGameActionResult*>(r.get());

        *outRideIndex = res->rideIndex;
        *outRideColour = colour1;

        return res->Cost;
    }

    /**
    *
    *  rct2: 0x006B3F0F
    */
    void game_command_create_ride(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
    {
        Guard::Assert(false, "GAME_COMMAND_CREATE_RIDE DEPRECATED");
    }

#pragma endregion

#pragma region RideSetStatusAction

    void ride_set_status(sint32 rideIndex, sint32 status)
    {
        auto gameAction = RideSetStatusAction(rideIndex, status);
        GameActions::Execute(&gameAction);
    }

    /**
    *
    *  rct2: 0x006B4EA6
    */
    void game_command_set_ride_status(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
    {
        Guard::Assert(false, "GAME_COMMAND_SET_RIDE_STATUS DEPRECATED");
    }

#pragma endregion

#pragma region RideSetName
    void ride_set_name(sint32 rideIndex, const char *name)
    {
        auto gameAction = RideSetNameAction(rideIndex, name);
        GameActions::Execute(&gameAction);
    }

    /**
    *
    *  rct2: 0x006B578B
    */
    void game_command_set_ride_name(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
    {
        Guard::Assert(false, "GAME_COMMAND_SET_RIDE_NAME DEPRECATED");
    }
#pragma endregion 

}
