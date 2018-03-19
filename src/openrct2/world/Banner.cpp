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

#include <algorithm>
#include <cstring>
#include <limits>

#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "../core/Util.hpp"
#include "../core/String.hpp"
#include "../network/network.h"

#include "Banner.h"
#include "Map.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Scenery.h"
#include "../Game.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/Track.h"
#include "../windows/Intent.h"
#include "../Context.h"

rct_banner gBanners[MAX_BANNERS];

/**
 *
 *  rct2: 0x006B7EAB
 */
static sint32 banner_get_ride_index_at(sint32 x, sint32 y, sint32 z)
{
    rct_tile_element *tileElement;
    Ride *ride;
    sint32 rideIndex, resultRideIndex;

    resultRideIndex = -1;
    tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_TRACK)
            continue;

        rideIndex = track_element_get_ride_index(tileElement);
        ride = get_ride(rideIndex);
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
            continue;

        if ((tileElement->clearance_height * 8) + 32 <= z)
            continue;

        resultRideIndex = rideIndex;
    } while (!tile_element_is_last_for_tile(tileElement++));

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
    rct_tile_element* tileElement = map_get_banner_element_at(x / 32, y / 32, baseHeight, direction);
    if (tileElement == nullptr)
    {
        return MONEY32_UNDEFINED;
    }

    rct_banner *banner = &gBanners[tileElement->properties.banner.index];
    rct_scenery_entry *bannerEntry = get_banner_entry(banner->type);
    money32 refund = 0;
    if (bannerEntry != nullptr)
    {
        refund = -((bannerEntry->banner.price * 3) / 4);
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST))
        {
            LocationXYZ16 coord;
            coord.x = x + 16;
            coord.y = y + 16;
            coord.z = tile_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        tile_element_remove_banner_entry(tileElement);
        map_invalidate_tile_zoom1(x, y, z, z + 32);
        tile_element_remove(tileElement);
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
        rct_tile_element* tileElement = map_get_first_element_at(x / 32, y / 32);

        bool found = false;
        do
        {
            if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_BANNER)
                continue;

            if (tileElement->properties.banner.position != direction)
                continue;

            found = true;
            break;
        } while (!tile_element_is_last_for_tile(tileElement++));

        if (!found)
        {
            return MONEY32_UNDEFINED;
        }

        auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
        intent.putExtra(INTENT_EXTRA_BANNER_INDEX, tileElement->properties.banner.index);
        context_broadcast_intent(&intent);

        gBanners[tileElement->properties.banner.index].colour = colour;
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

    rct_tile_element* tileElement = map_get_first_element_at(x / 32, y / 32);

    bool pathFound = false;
    do
    {
        if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_PATH)
            continue;

        if (tileElement->base_height != pathBaseHeight * 2 && tileElement->base_height != (pathBaseHeight - 1) * 2)
            continue;

        if (!(tileElement->properties.path.edges & (1 << direction)))
            continue;

        pathFound = true;
        break;
    } while (!tile_element_is_last_for_tile(tileElement++));

    if (!pathFound)
    {
        gGameCommandErrorText = STR_CAN_ONLY_BE_BUILT_ACROSS_PATHS;
        return MONEY32_UNDEFINED;
    }

    if (!map_can_build_at(x, y, pathBaseHeight * 16))
    {
        return MONEY32_UNDEFINED;
    }

    uint8 baseHeight = (pathBaseHeight + 1) * 2;
    tileElement = map_get_banner_element_at(x / 32, y / 32, baseHeight, direction);
    if (tileElement != nullptr)
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
            LocationXYZ16 coord;
            coord.x = x + 16;
            coord.y = y + 16;
            coord.z = tile_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        rct_tile_element* newTileElement = tile_element_insert(x / 32, y / 32, baseHeight, 0);
        assert(newTileElement != nullptr);
        gBanners[*bannerIndex].type = type;
        gBanners[*bannerIndex].colour = colour;
        gBanners[*bannerIndex].x = x / 32;
        gBanners[*bannerIndex].y = y / 32;
        newTileElement->type = TILE_ELEMENT_TYPE_BANNER;
        newTileElement->clearance_height = newTileElement->base_height + 2;
        newTileElement->properties.banner.position = direction;
        newTileElement->properties.banner.flags = 0xFF;
        newTileElement->properties.banner.unused = 0;
        newTileElement->properties.banner.index = *bannerIndex;
        if (flags & GAME_COMMAND_FLAG_GHOST)
        {
            newTileElement->flags |= TILE_ELEMENT_FLAG_GHOST;
        }
        map_invalidate_tile_full(x, y);
        map_animation_create(MAP_ANIMATION_TYPE_BANNER, x, y, newTileElement->base_height);
    }

    rct_scenery_entry *bannerEntry = get_banner_entry(type);
    if (bannerEntry == nullptr)
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

    size_t nameChunkOffset = std::min<size_t>(indexToOffset[nameChunkIndex], Util::CountOf(newName) - 12);
    std::memcpy(&newName[0 + nameChunkOffset], &nameChunk1, sizeof(uint32));
    std::memcpy(&newName[4 + nameChunkOffset], &nameChunk2, sizeof(uint32));
    std::memcpy(&newName[8 + nameChunkOffset], &nameChunk3, sizeof(uint32));

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

        auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
        intent.putExtra(INTENT_EXTRA_BANNER_INDEX, bannerIndex);
        context_broadcast_intent(&intent);
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

    rct_tile_element* tileElement = banner_get_tile_element(bannerIndex);

    if (tileElement == nullptr)
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

    tileElement->properties.banner.flags = 0xFF;
    if (banner->flags & BANNER_FLAG_NO_ENTRY)
    {
        tileElement->properties.banner.flags &= ~(1 << tileElement->properties.banner.position);
    }

    sint32 colourCodepoint = FORMAT_COLOUR_CODE_START + banner->text_colour;

    utf8 buffer[256];
    format_string(buffer, 256, banner->string_idx, nullptr);
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

        auto intent = Intent(INTENT_ACTION_UPDATE_BANNER);
        intent.putExtra(INTENT_EXTRA_BANNER_INDEX, bannerIndex);
        context_broadcast_intent(&intent);
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

/**
 *
 *  rct2: 0x006B9CB0
 */
void banner_init()
{
    for (auto &banner : gBanners) {
        banner.type = BANNER_NULL;
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

rct_tile_element *banner_get_tile_element(sint32 bannerIndex)
{
    rct_banner *banner = &gBanners[bannerIndex];
    rct_tile_element *tileElement = map_get_first_element_at(banner->x, banner->y);
    do
    {
        if (tile_element_get_banner_index(tileElement) == bannerIndex)
        {
            return tileElement;
        }
    } while (!tile_element_is_last_for_tile(tileElement++));
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

    static constexpr const LocationXY16 NeighbourCheckOrder[] =
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
    sint32 resultDistance = std::numeric_limits<sint32>::max();
    FOR_ALL_RIDES(i, ride)
    {
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
            continue;

        LocationXY8 location = ride->overall_view;
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
        rct_tile_element *tileElement = banner_get_tile_element(bannerIndex);
        if (tileElement == nullptr)
            gBanners[bannerIndex].type = BANNER_NULL;
    }
}

void fix_duplicated_banners()
{
    // For each banner in the map, check if the banner index is in use already, and if so, create a new entry for it
    bool               activeBanners[Util::CountOf(gBanners)]{};
    rct_tile_element * tileElement;
    for (int y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            tileElement = map_get_first_element_at(x, y);
            do
            {
                // TODO: Handle walls and large-scenery that use banner indices too. Large scenery can be tricky, as they occupy
                // multiple tiles that should both refer to the same banner index.
                if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_BANNER)
                {
                    uint8 bannerIndex = tileElement->properties.banner.index;
                    if (activeBanners[bannerIndex])
                    {
                        log_info(
                            "Duplicated banner with index %d found at x = %d, y = %d and z = %d.", bannerIndex, x, y,
                            tileElement->base_height);

                        // Banner index is already in use by another banner, so duplicate it
                        uint8 newBannerIndex = create_new_banner(GAME_COMMAND_FLAG_APPLY);
                        if (newBannerIndex == BANNER_NULL)
                        {
                            log_error("Failed to create new banner.");
                            continue;
                        }
                        Guard::Assert(activeBanners[newBannerIndex] == false);

                        // Copy over the original banner, but update the location
                        rct_banner & newBanner = gBanners[newBannerIndex];
                        newBanner              = gBanners[bannerIndex];
                        newBanner.x            = x;
                        newBanner.y            = y;

                        // Duplicate user string too
                        rct_string_id stringIdx = newBanner.string_idx;
                        if (is_user_string_id(stringIdx))
                        {
                            utf8 buffer[USER_STRING_MAX_LENGTH];
                            format_string(buffer, USER_STRING_MAX_LENGTH, stringIdx, nullptr);
                            rct_string_id newStringIdx = user_string_allocate(USER_STRING_DUPLICATION_PERMITTED, buffer);
                            if (newStringIdx == 0)
                            {
                                log_error("Failed to allocate user string for banner");
                                continue;
                            }
                            newBanner.string_idx = newStringIdx;
                        }

                        tileElement->properties.banner.index = newBannerIndex;
                    }

                    // Mark banner index as in-use
                    activeBanners[bannerIndex] = true;
                }
            } while (!tile_element_is_last_for_tile(tileElement++));
        }
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

