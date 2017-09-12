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

#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "../core/Util.hpp"
#include "../core/String.hpp"
#include "../network/network.h"

extern "C"
{
    #include "banner.h"
    #include "map.h"
    #include "park.h"
    #include "scenery.h"
    #include "../game.h"
    #include "../interface/window.h"
    #include "../localisation/localisation.h"
    #include "../ride/ride.h"
}

rct_banner gBanners[MAX_BANNERS];

/**
 *
 *  rct2: 0x006B7EAB
 */
static sint32 banner_get_ride_index_at(sint32 x, sint32 y, sint32 z)
{
    rct_map_element *mapElement;
    Ride *ride;
    sint32 rideIndex, resultRideIndex;

    resultRideIndex = -1;
    mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
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

static money32 BannerRemove(sint16 x, sint16 y, uint8 baseHeight, uint8 direction, uint8 flags)
{
    sint32 z = baseHeight * 8;
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = z;

    if (!(flags & GAME_COMMAND_FLAG_GHOST) && game_is_paused() && !gCheatsBuildInPauseMode)
    {
        gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        return MONEY32_UNDEFINED;
    }

    if (!map_can_build_at(x, y, z - 16))
    {
        return MONEY32_UNDEFINED;
    }

    // Slight modification to the code so that it now checks height as well
    // This was causing a bug with banners on two paths stacked.
    rct_map_element* mapElement = map_get_banner_element_at(x / 32, y / 32, baseHeight, direction);
    if (mapElement == nullptr)
    {
        return MONEY32_UNDEFINED;
    }

    rct_banner *banner = &gBanners[mapElement->properties.banner.index];
    rct_scenery_entry *bannerEntry = get_banner_entry(banner->type);
    money32 refund = 0;
    if (bannerEntry != nullptr && bannerEntry != (rct_scenery_entry *)-1)
    {
        refund = -((bannerEntry->banner.price * 3) / 4);
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST))
        {
            rct_xyz16 coord;
            coord.x = x + 16;
            coord.y = y + 16;
            coord.z = map_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        map_element_remove_banner_entry(mapElement);
        map_invalidate_tile_zoom1(x, y, z, z + 32);
        map_element_remove(mapElement);
    }

    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        refund = 0;
    }
    return refund;
}

static money32 BannerSetColour(sint16 x, sint16 y, uint8 baseHeight, uint8 direction, uint8 colour, uint8 flags)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    sint32 z = (baseHeight * 8);
    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = z;

    if (!map_can_build_at(x, y, z - 16))
    {
        return MONEY32_UNDEFINED;
    }


    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);

        bool found = false;
        do
        {
            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_BANNER)
                continue;

            if (mapElement->properties.banner.position != direction)
                continue;

            found = true;
            break;
        } while (!map_element_is_last_for_tile(mapElement++));

        if (found == false)
        {
            return MONEY32_UNDEFINED;
        }

        rct_window* window = window_find_by_number(WC_BANNER, mapElement->properties.banner.index);
        if (window != nullptr)
        {
            window_invalidate(window);
        }
        gBanners[mapElement->properties.banner.index].colour = colour;
        map_invalidate_tile_zoom1(x, y, z, z + 32);
    }

    return 0;
}

static money32 BannerPlace(sint16 x, sint16 y, uint8 pathBaseHeight, uint8 direction, uint8 colour, uint8 type, uint8 *bannerIndex, uint8 flags)
{
    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = pathBaseHeight * 16;
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    if (game_is_paused() && !gCheatsBuildInPauseMode)
    {
        gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        return MONEY32_UNDEFINED;
    }

    if (!map_check_free_elements_and_reorganise(1))
    {
        return MONEY32_UNDEFINED;
    }

    if (!map_is_location_valid(x, y))
    {
        return MONEY32_UNDEFINED;
    }

    rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);

    bool pathFound = false;
    do
    {
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_PATH)
            continue;

        if (mapElement->base_height != pathBaseHeight * 2 && mapElement->base_height != (pathBaseHeight - 1) * 2)
            continue;

        if (!(mapElement->properties.path.edges & (1 << direction)))
            continue;

        pathFound = true;
        break;
    } while (!map_element_is_last_for_tile(mapElement++));

    if (pathFound == false)
    {
        gGameCommandErrorText = STR_CAN_ONLY_BE_BUILT_ACROSS_PATHS;
        return MONEY32_UNDEFINED;
    }

    if (!map_can_build_at(x, y, pathBaseHeight * 16))
    {
        return MONEY32_UNDEFINED;
    }

    uint8 baseHeight = (pathBaseHeight + 1) * 2;
    mapElement = map_get_banner_element_at(x / 32, y / 32, baseHeight, direction);
    if (mapElement != nullptr)
    {
        gGameCommandErrorText = STR_BANNER_SIGN_IN_THE_WAY;
        return MONEY32_UNDEFINED;
    }

    *bannerIndex = create_new_banner(flags);
    if (*bannerIndex == BANNER_NULL)
    {
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST))
        {
            rct_xyz16 coord;
            coord.x = x + 16;
            coord.y = y + 16;
            coord.z = map_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        rct_map_element* newMapElement = map_element_insert(x / 32, y / 32, baseHeight, 0);
        assert(newMapElement != nullptr);
        gBanners[*bannerIndex].type = type;
        gBanners[*bannerIndex].colour = colour;
        gBanners[*bannerIndex].x = x / 32;
        gBanners[*bannerIndex].y = y / 32;
        newMapElement->type = MAP_ELEMENT_TYPE_BANNER;
        newMapElement->clearance_height = newMapElement->base_height + 2;
        newMapElement->properties.banner.position = direction;
        newMapElement->properties.banner.flags = 0xFF;
        newMapElement->properties.banner.unused = 0;
        newMapElement->properties.banner.index = *bannerIndex;
        if (flags & GAME_COMMAND_FLAG_GHOST)
        {
            newMapElement->flags |= MAP_ELEMENT_FLAG_GHOST;
        }
        map_invalidate_tile_full(x, y);
        map_animation_create(MAP_ANIMATION_TYPE_BANNER, x, y, newMapElement->base_height);
    }

    rct_scenery_entry *bannerEntry = get_banner_entry(type);
    if ((bannerEntry == nullptr) || (bannerEntry == (rct_scenery_entry *)-1))
    {
        return MONEY32_UNDEFINED;
    }

    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return 0;
    }
    return bannerEntry->banner.price;
}

static money32 BannerSetName(uint8 bannerIndex,
    uint16 nameChunkIndex,
    uint32 nameChunk1,
    uint32 nameChunk2,
    uint32 nameChunk3,
    uint8 flags)
{
    static char newName[128];

    if (bannerIndex >= MAX_BANNERS)
    {
        log_warning("Invalid game command for setting banner name, banner id = %d", bannerIndex);
        return MONEY32_UNDEFINED;
    }
    rct_banner* banner = &gBanners[bannerIndex];

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS;
    size_t indexToOffset[3] = { 24, 0, 12 };

    if (nameChunkIndex > Util::CountOf(indexToOffset))
    {
        log_warning("Invalid chunk index for setting banner name, banner id = %d, index = %d", bannerIndex, nameChunkIndex);
        return MONEY32_UNDEFINED;
    }

    size_t nameChunkOffset = Math::Min<size_t>(indexToOffset[nameChunkIndex], Util::CountOf(newName) - 12);
    Memory::Copy<uint32>((uint32*)(&newName[0 + nameChunkOffset]), &nameChunk1, sizeof(uint32));
    Memory::Copy<uint32>((uint32*)(&newName[4 + nameChunkOffset]), &nameChunk2, sizeof(uint32));
    Memory::Copy<uint32>((uint32*)(&newName[8 + nameChunkOffset]), &nameChunk3, sizeof(uint32));

    if (nameChunkIndex != 0)
    {
        return 0;
    }

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
    {
        return 0;
    }

    utf8 *buffer = gCommonStringFormatBuffer;
    utf8 *dst = buffer;
    dst = utf8_write_codepoint(dst, FORMAT_COLOUR_CODE_START + banner->text_colour);
    String::Set(dst, 256, newName, 32);

    rct_string_id stringId = user_string_allocate(USER_STRING_DUPLICATION_PERMITTED, buffer);
    if (stringId != 0)
    {
        rct_string_id prevStringId = banner->string_idx;
        banner->string_idx = stringId;
        user_string_free(prevStringId);
        rct_window* w = window_bring_to_front_by_number(WC_BANNER, bannerIndex);
        if (w != nullptr)
        {
            window_invalidate(w);
        }
    }
    else
    {
        gGameCommandErrorText = STR_ERR_CANT_SET_BANNER_TEXT;
        return MONEY32_UNDEFINED;
    }

    return 0;
}

static money32 BannerSetStyle(uint8 bannerIndex, uint8 colour, uint8 textColour, uint8 bannerFlags, uint8 flags)
{
    if (bannerIndex >= MAX_BANNERS)
    {
        gGameCommandErrorText = STR_INVALID_SELECTION_OF_OBJECTS;
        return MONEY32_UNDEFINED;
    }

    rct_banner* banner = &gBanners[bannerIndex];

    rct_map_element* mapElement = banner_get_map_element(bannerIndex);

    if (mapElement == nullptr)
    {
        return MONEY32_UNDEFINED;
    }

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
    {
        return 0;
    }

    banner->colour = colour;
    banner->text_colour = textColour;
    banner->flags = bannerFlags;

    mapElement->properties.banner.flags = 0xFF;
    if (banner->flags & BANNER_FLAG_NO_ENTRY)
    {
        mapElement->properties.banner.flags &= ~(1 << mapElement->properties.banner.position);
    }

    sint32 colourCodepoint = FORMAT_COLOUR_CODE_START + banner->text_colour;

    utf8 buffer[256];
    format_string(buffer, 256, banner->string_idx, 0);
    sint32 firstCodepoint = utf8_get_next(buffer, nullptr);
    if (firstCodepoint >= FORMAT_COLOUR_CODE_START && firstCodepoint <= FORMAT_COLOUR_CODE_END)
    {
        utf8_write_codepoint(buffer, colourCodepoint);
    }
    else
    {
        utf8_insert_codepoint(buffer, colourCodepoint);
    }

    rct_string_id stringId = user_string_allocate(USER_STRING_DUPLICATION_PERMITTED, buffer);
    if (stringId != 0)
    {
        rct_string_id prevStringId = banner->string_idx;
        banner->string_idx = stringId;
        user_string_free(prevStringId);
        rct_window* w = window_bring_to_front_by_number(WC_BANNER, bannerIndex);
        if (w != nullptr)
        {
            window_invalidate(w);
        }
    }
    else
    {
        gGameCommandErrorText = STR_ERR_CANT_SET_BANNER_TEXT;
        return MONEY32_UNDEFINED;
    }

    return 0;
}

static uint8 BannerGetNewIndex() {
    uint8 bannerIndex = 0;
    for (; bannerIndex < MAX_BANNERS; bannerIndex++)
    {
        if (gBanners[bannerIndex].type == BANNER_NULL)
        {
            return bannerIndex;
        }
    }
    return BANNER_NULL;
}

extern "C"
{
    /**
     *
     *  rct2: 0x006B9CB0
     */
    void banner_init()
    {
        for (sint32 i = 0; i < MAX_BANNERS; i++)
        {
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
        uint8 bannerIndex = BannerGetNewIndex();

        if (bannerIndex == BANNER_NULL)
        {
            gGameCommandErrorText = STR_TOO_MANY_BANNERS_IN_GAME;
            return BANNER_NULL;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            rct_banner* banner = &gBanners[bannerIndex];

            banner->flags = 0;
            banner->type = 0;
            banner->string_idx = STR_DEFAULT_SIGN;
            banner->colour = 2;
            banner->text_colour = 2;
        }
        return bannerIndex;
    }

    rct_map_element *banner_get_map_element(sint32 bannerIndex)
    {
        rct_banner *banner = &gBanners[bannerIndex];
        rct_map_element *mapElement = map_get_first_element_at(banner->x, banner->y);
        do
        {
            if (map_element_get_banner_index(mapElement) == bannerIndex)
            {
                return mapElement;
            }
        } while (!map_element_is_last_for_tile(mapElement++));
        return nullptr;
    }

    /**
     *
     *  rct2: 0x006B7D86
     */
    sint32 banner_get_closest_ride_index(sint32 x, sint32 y, sint32 z)
    {
        sint32 i, rideIndex;
        Ride *ride;

        static const rct_xy16 NeighbourCheckOrder[] =
        {
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

        for (i = 0; i < (sint32)Util::CountOf(NeighbourCheckOrder); i++)
        {
            rideIndex = banner_get_ride_index_at(x + NeighbourCheckOrder[i].x, y + NeighbourCheckOrder[i].y, z);
            if (rideIndex != -1)
            {
                return rideIndex;
            }
        }

        rideIndex = -1;
        sint32 resultDistance = INT_MAX;
        FOR_ALL_RIDES(i, ride)
        {
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
                continue;

            rct_xy8 location = ride->overall_view;
            if (location.xy == RCT_XY8_UNDEFINED)
                continue;

            sint32 rideX = location.x * 32;
            sint32 rideY = location.y * 32;
            sint32 distance = abs(x - rideX) + abs(y - rideY);
            if (distance < resultDistance)
            {
                resultDistance = distance;
                rideIndex = i;
            }
        }

        return rideIndex;
    }

    void banner_reset_broken_index()
    {
        for (sint32 bannerIndex = 0; bannerIndex < MAX_BANNERS; bannerIndex++)
        {
            rct_map_element *mapElement = banner_get_map_element(bannerIndex);
            if (mapElement == nullptr)
                gBanners[bannerIndex].type = BANNER_NULL;
        }
    }

    /**
     *
     *  rct2: 0x006BA058
     */
    void game_command_remove_banner(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
    {
        *ebx = BannerRemove(
            *eax & 0xFFFF,
            *ecx & 0xFFFF,
            *edx & 0xFF,
            (*edx >> 8) & 0xFF,
            *ebx & 0xFF
        );
    }

    /**
     *
     *  rct2: 0x006BA16A
     */
    void game_command_set_banner_colour(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
    {
        *ebx = BannerSetColour(
            *eax & 0xFFFF,
            *ecx & 0xFFFF,
            *edx & 0xFF,
            (*edx >> 8) & 0xFF,
            *ebp & 0xFF,
            *ebx & 0xFF
        );
    }

    /**
     *
     *  rct2: 0x006B9E6D
     */
    void game_command_place_banner(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
    {
        *ebx = BannerPlace(
            *eax & 0xFFFF,
            *ecx & 0xFFFF,
            *edx & 0xFF,
            (*edx >> 8) & 0xFF,
            *ebp & 0xFF,
            (*ebx >> 8) & 0xFF,
            (uint8 *)edi,
            *ebx & 0xFF
        );
    }

    void game_command_set_banner_name(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
    {
        *ebx = BannerSetName(
            *ecx & 0xFF,
            *eax & 0xFFFF,
            *edx,
            *ebp,
            *edi,
            *ebx & 0xFF
        );
    }

    void game_command_set_banner_style(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
    {
        *ebx = BannerSetStyle(
            *ecx & 0xFF,
            *edx & 0xFF,
            *edi & 0xFF,
            *ebp & 0xFF,
            *ebx & 0xFF
        );
    }
}
