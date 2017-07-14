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

#include "../core/MemoryStream.h"
#include "../localisation/string_ids.h"
#include "GameAction.h"

extern "C"
{
#include "../cheats.h"
#include "../world/entrance.h"
#include "../world/park.h"
#include "../world/footpath.h"
}

struct PlaceParkEntranceGameActionResult : public GameActionResult {
	PlaceParkEntranceGameActionResult(GA_ERROR error, rct_string_id message) :GameActionResult(error, message)
	{
		ErrorTitle = STR_CANT_BUILD_PARK_ENTRANCE_HERE;
	}
};

struct PlaceParkEntranceAction : public GameAction<GAME_COMMAND_PLACE_PARK_ENTRANCE, GA_FLAGS::EDITOR_ONLY>
{
public:
	sint16 x;
	sint16 y;
	sint16 z;
	uint8 direction;

	void Deserialise(IStream * stream) override
	{
		x = stream->ReadValue<sint16>(); 
		y = stream->ReadValue<sint16>();
		z = stream->ReadValue<sint16>();
		direction = stream->ReadValue<uint8>();
	}

	void Serialise(IStream * stream) const override
	{
		stream->WriteValue(x);
		stream->WriteValue(y);
		stream->WriteValue(z);
		stream->WriteValue(direction);
	}

	GameActionResult Query(uint32 flags = 0) const override
	{
		if (!(gScreenFlags & SCREEN_FLAGS_EDITOR) && !gCheatsSandboxMode)
		{
			return PlaceParkEntranceGameActionResult(GA_ERROR::NOT_IN_EDITOR_MODE, STR_NONE);
		}

		gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;

		gCommandPosition.x = x;
		gCommandPosition.y = y;
		gCommandPosition.z = z * 16;

		if (!map_check_free_elements_and_reorganise(3))
		{
			return PlaceParkEntranceGameActionResult(GA_ERROR::NO_FREE_ELEMENTS, STR_NONE);
		}

		if (x <= 32 || y <= 32 || x >= (gMapSizeUnits - 32) || y >= (gMapSizeUnits - 32))
		{
			return PlaceParkEntranceGameActionResult(GA_ERROR::INVALID_PARAMETERS, STR_TOO_CLOSE_TO_EDGE_OF_MAP);
		}

		sint8 entranceNum = -1;
		for (uint8 i = 0; i < MAX_PARK_ENTRANCES; ++i)
		{
			if (gParkEntrances[i].x == MAP_LOCATION_NULL)
			{
				entranceNum = i;
				break;
			}
		}

		if (entranceNum == -1)
		{
			return PlaceParkEntranceGameActionResult(GA_ERROR::INVALID_PARAMETERS, STR_ERR_TOO_MANY_PARK_ENTRANCES);
		}

		sint8 zLow = z * 2;
		sint8 zHigh = zLow + 12;
		rct_xy16 entranceLoc = { x, y };
		for (uint8 index = 0; index < 3; index++)
		{
			if (index == 1)
			{
				entranceLoc.x += TileDirectionDelta[(direction - 1) & 0x3].x;
				entranceLoc.y += TileDirectionDelta[(direction - 1) & 0x3].y;
			}
			else if (index == 2)
			{
				entranceLoc.x += TileDirectionDelta[(direction + 1) & 0x3].x * 2;
				entranceLoc.y += TileDirectionDelta[(direction + 1) & 0x3].y * 2;
			}

			if (!gCheatsDisableClearanceChecks)
			{
				if (!map_can_construct_at(entranceLoc.x, entranceLoc.y, zLow, zHigh, 0xF))
				{
					return PlaceParkEntranceGameActionResult(GA_ERROR::NO_CLEARANCE, STR_NONE);
				}
			}

			// Check that entrance element does not already exist at this location
			rct_map_element* entranceElement = map_get_park_entrance_element_at(entranceLoc.x, entranceLoc.y, zLow, false);
			if (entranceElement != NULL)
			{
				return PlaceParkEntranceGameActionResult(GA_ERROR::ITEM_ALREADY_PLACED, STR_NONE);
			}
		}
		return GameActionResult();
	}

	GameActionResult Execute(uint32 flags = 0) const override
	{
		gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LAND_PURCHASE;

		gCommandPosition.x = x;
		gCommandPosition.y = y;
		gCommandPosition.z = z * 16;

		sint8 entranceNum = -1;
		for (uint8 i = 0; i < MAX_PARK_ENTRANCES; ++i)
		{
			if (gParkEntrances[i].x == MAP_LOCATION_NULL)
			{
				entranceNum = i;
				break;
			}
		}

		Guard::Assert(entranceNum != -1);

		gParkEntrances[entranceNum].x = x;
		gParkEntrances[entranceNum].y = y;
		gParkEntrances[entranceNum].z = z * 16;
		gParkEntrances[entranceNum].direction = direction;

		sint8 zLow = z * 2;
		sint8 zHigh = zLow + 12;
		rct_xy16 entranceLoc = { x, y };
		for (uint8 index = 0; index < 3; index++)
		{
			if (index == 1)
			{
				entranceLoc.x += TileDirectionDelta[(direction - 1) & 0x3].x;
				entranceLoc.y += TileDirectionDelta[(direction - 1) & 0x3].y;
			}
			else if (index == 2)
			{
				entranceLoc.x += TileDirectionDelta[(direction + 1) & 0x3].x * 2;
				entranceLoc.y += TileDirectionDelta[(direction + 1) & 0x3].y * 2;
			}

			if (!(flags & GAME_COMMAND_FLAG_GHOST))
			{
				rct_map_element* surfaceElement = map_get_surface_element_at(entranceLoc.x / 32, entranceLoc.y / 32);
				surfaceElement->properties.surface.ownership = 0;
			}

			rct_map_element* newElement = map_element_insert(entranceLoc.x / 32, entranceLoc.y / 32, zLow, 0xF);
			Guard::Assert(newElement != NULL);
			newElement->clearance_height = zHigh;

			if (flags & GAME_COMMAND_FLAG_GHOST)
			{
				newElement->flags |= MAP_ELEMENT_FLAG_GHOST;
			}

			newElement->type = MAP_ELEMENT_TYPE_ENTRANCE;
			newElement->type |= direction;
			newElement->properties.entrance.index = index;
			newElement->properties.entrance.type = ENTRANCE_TYPE_PARK_ENTRANCE;
			newElement->properties.entrance.path_type = gFootpathSelectedId;

			if (!(flags & GAME_COMMAND_FLAG_GHOST))
			{
				footpath_connect_edges(entranceLoc.x, entranceLoc.y, newElement, 1);
			}

			update_park_fences(entranceLoc.x, entranceLoc.y);
			update_park_fences(entranceLoc.x - 32, entranceLoc.y);
			update_park_fences(entranceLoc.x + 32, entranceLoc.y);
			update_park_fences(entranceLoc.x, entranceLoc.y - 32);
			update_park_fences(entranceLoc.x, entranceLoc.y + 32);

			map_invalidate_tile(entranceLoc.x, entranceLoc.y, newElement->base_height * 8, newElement->clearance_height * 8);

			if (index == 0)
			{
				map_animation_create(MAP_ANIMATION_TYPE_PARK_ENTRANCE, entranceLoc.x, entranceLoc.y, zLow);
			}
		}
		return GameActionResult();
	}
};

static auto Factory UNUSED_ATTR = GameActions::Register<PlaceParkEntranceAction>();

extern "C"
{
	money32 place_park_entrance(sint16 x, sint16 y, sint16 z, uint8 direction)
	{
		auto gameAction = PlaceParkEntranceAction();
		gameAction.x = x;
		gameAction.y = y;
		gameAction.z = z;
		gameAction.direction = direction;
		auto result = GameActions::Execute(&gameAction);
		if (result.Error == GA_ERROR::OK)
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
		Guard::Assert(false, "GAME_COMMAND_PLACE_PARK_ENTRANCE DEPRECIATED");
	}

	/**
	*
	*  rct2: 0x00666F4E
	*/
	money32 park_entrance_place_ghost(sint32 x, sint32 y, sint32 z, sint32 direction)
	{
		park_entrance_remove_ghost();
		auto gameAction = PlaceParkEntranceAction();
		gameAction.x = x;
		gameAction.y = y;
		gameAction.z = z;
		gameAction.direction = direction;
		auto result = GameActions::Execute(&gameAction, GAME_COMMAND_FLAG_GHOST);
		if (result.Error == GA_ERROR::OK)
		{
			gParkEntranceGhostPosition.x = x;
			gParkEntranceGhostPosition.y = y;
			gParkEntranceGhostPosition.z = z;
			gParkEntranceGhostDirection = direction;
			gParkEntranceGhostExists = true;
		}
		return result.Cost;
	}
}
