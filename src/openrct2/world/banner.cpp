#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../core/util.hpp"
#include "../network/network.h"

extern "C" 
{
    #include "../game.h"
    #include "../localisation/localisation.h"
    #include "../ride/ride.h"
    #include "../interface/window.h"
    #include "../util/util.h"
    #include "scenery.h"
    #include "banner.h"
    #include "map.h"
    #include "park.h"
}

rct_banner gBanners[MAX_BANNERS];

extern "C" 
{
    /**
     *
     *  rct2: 0x006B9CB0
     */
    void banner_init()
    {
        for (sint32 i = 0; i < MAX_BANNERS; i++) {
            gBanners[i].type = BANNER_NULL;
        }
    }

    /**
     * Creates a new banner and returns the index of the banner
     * If the flag GAME_COMMAND_FLAG_APPLY is NOT set then returns
     * the first unused index but does NOT mark the banner as created.
     * returns 0xFF on failure.
     *
     *  rct2: 0x006BA278
     */
    sint32 create_new_banner(uint8 flags)
    {
        sint32 banner_index = 0;
        for (; banner_index < MAX_BANNERS; banner_index++) {
            if (gBanners[banner_index].type == BANNER_NULL) {
                break;
            }
        }

        if (banner_index == MAX_BANNERS) {
            gGameCommandErrorText = STR_TOO_MANY_BANNERS_IN_GAME;
            return BANNER_NULL;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            rct_banner* banner = &gBanners[banner_index];

            banner->flags = 0;
            banner->type = 0;
            banner->string_idx = STR_DEFAULT_SIGN;
            banner->colour = 2;
            banner->text_colour = 2;
        }
        return banner_index;
    }

    rct_map_element *banner_get_map_element(sint32 bannerIndex)
    {
        rct_banner *banner = &gBanners[bannerIndex];
        rct_map_element *mapElement = map_get_first_element_at(banner->x, banner->y);
        do {
            if (map_element_get_banner_index(mapElement) == bannerIndex) {
                return mapElement;
            }
        } while (!map_element_is_last_for_tile(mapElement++));
        return NULL;
    }

    /**
     *
     *  rct2: 0x006B7EAB
     */
    static sint32 banner_get_ride_index_at(sint32 x, sint32 y, sint32 z)
    {
        rct_map_element *mapElement;
        rct_ride *ride;
        sint32 rideIndex, resultRideIndex;

        resultRideIndex = -1;
        mapElement = map_get_first_element_at(x >> 5, y >> 5);
        do {
            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
                continue;

            rideIndex = mapElement->properties.track.ride_index;
            ride = get_ride(rideIndex);
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
                continue;

            if ((mapElement->clearance_height * 8) + 32 <= z)
                continue;

            resultRideIndex = rideIndex;
        } while (!map_element_is_last_for_tile(mapElement++));

        return resultRideIndex;
    }

    /**
     *
     *  rct2: 0x006B7D86
     */
    sint32 banner_get_closest_ride_index(sint32 x, sint32 y, sint32 z)
    {
        uint32 i, rideIndex;
        rct_ride *ride;

        static const rct_xy16 NeighbourCheckOrder[] = {
            {  32,   0 },
            { -32,   0 },
            {   0,  32 },
            {   0, -32 },
            { -32, +32 },
            { +32, -32 },
            { +32, +32 },
            { -32, +32 },
            {   0,   0 }
        };

        for (i = 0; i < Util::CountOf(NeighbourCheckOrder); i++) {
            rideIndex = banner_get_ride_index_at(x + NeighbourCheckOrder[i].x, y + NeighbourCheckOrder[i].y, z);
            if (rideIndex != -1) {
                return rideIndex;
            }
        }

        rideIndex = -1;
        sint32 resultDistance = INT_MAX;
        FOR_ALL_RIDES(i, ride) {
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
                continue;

            uint16 xy = ride->overall_view;
            if (xy == 0xFFFF)
                continue;

            sint32 rideX = (xy & 0xFF) * 32;
            sint32 rideY = (xy >> 8) * 32;
            sint32 distance = abs(x - rideX) + abs(y - rideY);
            if (distance < resultDistance) {
                resultDistance = distance;
                rideIndex = i;
            }
        }

        return rideIndex;
    }

    void fix_banner_count()
    {
        for (sint32 banner_index = 0; banner_index < MAX_BANNERS; banner_index++) {
            rct_map_element *map_element = banner_get_map_element(banner_index);
            if (map_element == NULL)
                gBanners[banner_index].type = BANNER_NULL;
        }
    }


    /**
    *
    *  rct2: 0x006BA058
    */
    void game_command_remove_banner(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
    {
        sint32 x = *eax;
        sint32 y = *ecx;
        uint8 base_height = *edx;
        uint8 banner_position = *edx >> 8;
        uint8 flags = *ebx & 0xFF;
        sint32 z = base_height * 8;
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        gCommandPosition.x = x + 16;
        gCommandPosition.y = y + 16;
        gCommandPosition.z = z;

        if (!(flags & GAME_COMMAND_FLAG_GHOST) && game_is_paused() && !gCheatsBuildInPauseMode) {
            gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode && !map_is_location_owned(x, y, z - 16)) {
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        // Slight modification to the code so that it now checks height as well
        // This was causing a bug with banners on two paths stacked.
        rct_map_element* map_element = map_get_banner_element_at(x / 32, y / 32, base_height, banner_position);
        if (map_element == NULL) {
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        rct_banner *banner = &gBanners[map_element->properties.banner.index];
        rct_scenery_entry *scenery_entry = get_banner_entry(banner->type);
        money32 refund = 0;
        if (scenery_entry != NULL && scenery_entry != (rct_scenery_entry *)-1) {
            refund = -((scenery_entry->banner.price * 3) / 4);
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            if (gGameCommandNestLevel == 1 && !(*ebx & GAME_COMMAND_FLAG_GHOST)) {
                rct_xyz16 coord;
                coord.x = x + 16;
                coord.y = y + 16;
                coord.z = map_element_height(coord.x, coord.y);
                network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
            }

            map_element_remove_banner_entry(map_element);
            map_invalidate_tile_zoom1(x, y, z, z + 32);
            map_element_remove(map_element);
        }

        *ebx = refund;
        if (gParkFlags & PARK_FLAGS_NO_MONEY) {
            *ebx = 0;
        }
    }


    /**
    *
    *  rct2: 0x006BA16A
    */
    void game_command_set_banner_colour(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
    {
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        sint32 x = *eax;
        sint32 y = *ecx;
        uint8 base_height = *edx;
        uint8 banner_position = *edx >> 8;
        uint8 colour = *ebp;
        sint32 z = (base_height * 8);
        gCommandPosition.x = x + 16;
        gCommandPosition.y = y + 16;
        gCommandPosition.z = z;

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode) {
            if (!map_is_location_owned(x, y, z - 16)) {
                *ebx = MONEY32_UNDEFINED;
                return;
            }
        }

        if (*ebx & GAME_COMMAND_FLAG_APPLY) {
            rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

            bool found = false;
            do {
                if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_BANNER)
                    continue;

                if (map_element->properties.banner.position != banner_position)
                    continue;

                found = true;
                break;
            } while (!map_element_is_last_for_tile(map_element++));

            if (found == false) {
                *ebx = MONEY32_UNDEFINED;
                return;
            }

            rct_window* window = window_find_by_number(WC_BANNER, map_element->properties.banner.index);
            if (window) {
                window_invalidate(window);
            }
            gBanners[map_element->properties.banner.index].colour = colour;
            map_invalidate_tile_zoom1(x, y, z, z + 32);
        }

        *ebx = 0;
    }

    /**
    *
    *  rct2: 0x006B9E6D
    */
    void game_command_place_banner(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
    {
        sint32 x = (uint16)*eax;
        sint32 y = (uint16)*ecx;
        uint8 base_height = *edx;
        uint8 edge = *edx >> 8;
        uint8 colour = *edi;
        uint8 type = *ebx >> 8;
        gCommandPosition.x = x + 16;
        gCommandPosition.y = y + 16;
        gCommandPosition.z = base_height * 16;
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
        if (game_is_paused() && !gCheatsBuildInPauseMode) {
            gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        if (!map_check_free_elements_and_reorganise(1)) {
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        if (x >= 8192 || y >= 8192) {
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);
        sint32 dl = base_height * 2;
        sint32 ch = (base_height - 1) * 2;

        bool pathFound = false;
        do {
            if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_PATH)
                continue;

            if (map_element->base_height != dl && map_element->base_height != ch)
                continue;

            if (!(map_element->properties.path.edges & (1 << edge)))
                continue;

            pathFound = true;
            break;
        } while (!map_element_is_last_for_tile(map_element++));

        if (pathFound == false) {
            gGameCommandErrorText = STR_CAN_ONLY_BE_BUILT_ACROSS_PATHS;
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode && !map_is_location_owned(x, y, base_height * 16)) {
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        map_element = map_get_first_element_at(x / 32, y / 32);
        dl = (base_height + 1) * 2;

        // Check to see if there is a banner in the way
        do {
            if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_BANNER)
                continue;

            if (map_element->base_height != dl)
                continue;

            if ((map_element->properties.banner.position & 0x3) != edge)
                continue;

            gGameCommandErrorText = STR_BANNER_SIGN_IN_THE_WAY;
            *ebx = MONEY32_UNDEFINED;
            return;
        } while (!map_element_is_last_for_tile(map_element++));

        sint32 banner_index = create_new_banner(*ebx);
        if (banner_index == BANNER_NULL) {
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        *edi = banner_index;
        if (*ebx & GAME_COMMAND_FLAG_APPLY) {
            if (gGameCommandNestLevel == 1 && !(*ebx & GAME_COMMAND_FLAG_GHOST)) {
                rct_xyz16 coord;
                coord.x = x + 16;
                coord.y = y + 16;
                coord.z = map_element_height(coord.x, coord.y);
                network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
            }

            rct_map_element* new_map_element = map_element_insert(x / 32, y / 32, (base_height + 1) * 2, 0);
            assert(new_map_element != NULL);
            gBanners[banner_index].type = type;
            gBanners[banner_index].colour = colour;
            gBanners[banner_index].x = x / 32;
            gBanners[banner_index].y = y / 32;
            new_map_element->type = MAP_ELEMENT_TYPE_BANNER;
            new_map_element->clearance_height = new_map_element->base_height + 2;
            new_map_element->properties.banner.position = edge;
            new_map_element->properties.banner.flags = 0xFF;
            new_map_element->properties.banner.unused = 0;
            new_map_element->properties.banner.index = banner_index;
            if (*ebx & GAME_COMMAND_FLAG_GHOST) {
                new_map_element->flags |= MAP_ELEMENT_FLAG_GHOST;
            }
            map_invalidate_tile_full(x, y);
            map_animation_create(0x0A, x, y, new_map_element->base_height);
        }
        rct_scenery_entry *scenery_entry = (rct_scenery_entry*)object_entry_groups[OBJECT_TYPE_BANNERS].chunks[type];
        *ebx = scenery_entry->banner.price;
        if (gParkFlags & PARK_FLAGS_NO_MONEY) {
            *ebx = 0;
        }
    }

    void game_command_set_banner_name(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp) {
        static char newName[128];

        if ((*ecx >= MAX_BANNERS) || (*ecx < 0))
        {
            log_warning("Invalid game command for setting banner name, banner id = %d", *ecx);
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        rct_banner* banner = &gBanners[*ecx];

        sint32 nameChunkIndex = *eax & 0xFFFF;

        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS;
        sint32 nameChunkOffset = nameChunkIndex - 1;
        if (nameChunkOffset < 0)
            nameChunkOffset = 2;
        nameChunkOffset *= 12;
        nameChunkOffset = min(nameChunkOffset, (sint32)Util::CountOf(newName) - 12);
        memcpy((void*)((uintptr_t)newName + (uintptr_t)nameChunkOffset + 0), edx, sizeof(uint32));
        memcpy((void*)((uintptr_t)newName + (uintptr_t)nameChunkOffset + 4), ebp, sizeof(uint32));
        memcpy((void*)((uintptr_t)newName + (uintptr_t)nameChunkOffset + 8), edi, sizeof(uint32));

        if (nameChunkIndex != 0) {
            *ebx = 0;
            return;
        }

        if (!(*ebx & GAME_COMMAND_FLAG_APPLY)) {
            *ebx = 0;
            return;
        }

        utf8 *buffer = gCommonStringFormatBuffer;
        utf8 *dst = buffer;
        dst = utf8_write_codepoint(dst, FORMAT_COLOUR_CODE_START + banner->text_colour);
        safe_strcpy(dst, newName, 32);

        rct_string_id stringId = user_string_allocate(128, buffer);
        if (stringId) {
            rct_string_id prev_string_id = banner->string_idx;
            banner->string_idx = stringId;
            user_string_free(prev_string_id);
            rct_window* w = window_bring_to_front_by_number(WC_BANNER, *ecx);
            if (w) {
                window_invalidate(w);
            }
        }
        else {
            gGameCommandErrorText = STR_ERR_CANT_SET_BANNER_TEXT;
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        *ebx = 0;
    }

    void game_command_set_banner_style(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp) {
        if ((*ecx >= MAX_BANNERS) || (*ecx < 0))
        {
            gGameCommandErrorText = STR_INVALID_SELECTION_OF_OBJECTS;
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        if (!(*ebx & GAME_COMMAND_FLAG_APPLY)) {
            *ebx = 0;
            return;
        }

        rct_banner* banner = &gBanners[*ecx];

        banner->colour = (uint8)*edx;
        banner->text_colour = (uint8)*edi;
        banner->flags = (uint8)*ebp;

        uint8 bannerIndex = *ecx & 0xFF;

        sint32 x = banner->x << 5;
        sint32 y = banner->y << 5;

        rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);
        bool bannerFound = false;
        do {
            if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_BANNER)
                continue;

            if (map_element->properties.banner.index != bannerIndex)
                continue;

            bannerFound = true;
            break;
        } while (!map_element_is_last_for_tile(map_element++));

        if (bannerFound == false) {
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        map_element->properties.banner.flags = 0xFF;
        if (banner->flags & BANNER_FLAG_NO_ENTRY) {
            map_element->properties.banner.flags &= ~(1 << map_element->properties.banner.position);
        }

        sint32 colourCodepoint = FORMAT_COLOUR_CODE_START + banner->text_colour;

        utf8 buffer[256];
        format_string(buffer, 256, banner->string_idx, 0);
        sint32 firstCodepoint = utf8_get_next(buffer, NULL);
        if (firstCodepoint >= FORMAT_COLOUR_CODE_START && firstCodepoint <= FORMAT_COLOUR_CODE_END) {
            utf8_write_codepoint(buffer, colourCodepoint);
        }
        else {
            utf8_insert_codepoint(buffer, colourCodepoint);
        }

        rct_string_id stringId = user_string_allocate(128, buffer);
        if (stringId != 0) {
            rct_string_id prev_string_id = banner->string_idx;
            banner->string_idx = stringId;
            user_string_free(prev_string_id);
            rct_window* w = window_bring_to_front_by_number(WC_BANNER, *ecx);
            if (w) {
                window_invalidate(w);
            }
        }
        else {
            gGameCommandErrorText = STR_ERR_CANT_SET_BANNER_TEXT;
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        *ebx = 0;
    }

}
