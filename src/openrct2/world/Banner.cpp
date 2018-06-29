/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
static uint8_t banner_get_ride_index_at(int32_t x, int32_t y, int32_t z)
{
    rct_tile_element* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    uint8_t resultRideIndex = RIDE_ID_NULL;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        uint8_t rideIndex = track_element_get_ride_index(tileElement);
        Ride* ride = get_ride(rideIndex);
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
            continue;

        if ((tileElement->clearance_height * 8) + 32 <= z)
            continue;

        resultRideIndex = rideIndex;
    } while (!(tileElement++)->IsLastForTile());

    return resultRideIndex;
}

static money32 BannerRemove(int16_t x, int16_t y, uint8_t baseHeight, uint8_t direction, uint8_t flags)
{
    int32_t z = baseHeight * 8;
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

static money32 BannerSetColour(int16_t x, int16_t y, uint8_t baseHeight, uint8_t direction, uint8_t colour, uint8_t flags)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
    int32_t z = (baseHeight * 8);
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
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_BANNER)
                continue;

            if (tileElement->properties.banner.position != direction)
                continue;

            found = true;
            break;
        } while (!(tileElement++)->IsLastForTile());

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

static money32 BannerPlace(
    int16_t x, int16_t y, uint8_t pathBaseHeight, uint8_t direction, uint8_t colour, uint8_t type, BannerIndex* bannerIndex, uint8_t flags)
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

    if (!map_is_location_valid({x, y}))
    {
        return MONEY32_UNDEFINED;
    }

    rct_tile_element* tileElement = map_get_first_element_at(x / 32, y / 32);

    bool pathFound = false;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_PATH)
            continue;

        if (tileElement->base_height != pathBaseHeight * 2 && tileElement->base_height != (pathBaseHeight - 1) * 2)
            continue;

        if (!(tileElement->properties.path.edges & (1 << direction)))
            continue;

        pathFound = true;
        break;
    } while (!(tileElement++)->IsLastForTile());

    if (!pathFound)
    {
        gGameCommandErrorText = STR_CAN_ONLY_BE_BUILT_ACROSS_PATHS;
        return MONEY32_UNDEFINED;
    }

    if (!map_can_build_at(x, y, pathBaseHeight * 16))
    {
        return MONEY32_UNDEFINED;
    }

    uint8_t baseHeight = (pathBaseHeight + 1) * 2;
    tileElement = map_get_banner_element_at(x / 32, y / 32, baseHeight, direction);
    if (tileElement != nullptr)
    {
        gGameCommandErrorText = STR_BANNER_SIGN_IN_THE_WAY;
        return MONEY32_UNDEFINED;
    }

    *bannerIndex = create_new_banner(flags);
    if (*bannerIndex == BANNER_INDEX_NULL)
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
        newTileElement->SetType(TILE_ELEMENT_TYPE_BANNER);
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

static money32 BannerSetStyle(BannerIndex bannerIndex, uint8_t colour, uint8_t textColour, uint8_t bannerFlags, uint8_t flags)
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

    int32_t colourCodepoint = FORMAT_COLOUR_CODE_START + banner->text_colour;

    utf8 buffer[256];
    format_string(buffer, 256, banner->string_idx, nullptr);
    int32_t firstCodepoint = utf8_get_next(buffer, nullptr);
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

static BannerIndex BannerGetNewIndex()
{
    for (BannerIndex bannerIndex = 0; bannerIndex < MAX_BANNERS; bannerIndex++)
    {
        if (gBanners[bannerIndex].type == BANNER_NULL)
        {
            return bannerIndex;
        }
    }
    return BANNER_INDEX_NULL;
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
BannerIndex create_new_banner(uint8_t flags)
{
    BannerIndex bannerIndex = BannerGetNewIndex();

    if (bannerIndex == BANNER_INDEX_NULL)
    {
        gGameCommandErrorText = STR_TOO_MANY_BANNERS_IN_GAME;
        return bannerIndex;
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

rct_tile_element* banner_get_tile_element(BannerIndex bannerIndex)
{
    rct_banner *banner = &gBanners[bannerIndex];
    rct_tile_element *tileElement = map_get_first_element_at(banner->x, banner->y);
    do
    {
        if (tile_element_get_banner_index(tileElement) == bannerIndex)
        {
            return tileElement;
        }
    } while (!(tileElement++)->IsLastForTile());
    return nullptr;
}

/**
 *
 *  rct2: 0x006B7D86
 */
uint8_t banner_get_closest_ride_index(int32_t x, int32_t y, int32_t z)
{
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

    for (size_t i = 0; i < (int32_t)Util::CountOf(NeighbourCheckOrder); i++)
    {
        uint8_t rideIndex = banner_get_ride_index_at(x + NeighbourCheckOrder[i].x, y + NeighbourCheckOrder[i].y, z);
        if (rideIndex != RIDE_ID_NULL)
        {
            return rideIndex;
        }
    }

    uint8_t index;
    uint8_t rideIndex = RIDE_ID_NULL;
    int32_t resultDistance = std::numeric_limits<int32_t>::max();
    FOR_ALL_RIDES(index, ride)
    {
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
            continue;

        LocationXY8 location = ride->overall_view;
        if (location.xy == RCT_XY8_UNDEFINED)
            continue;

        int32_t rideX = location.x * 32;
        int32_t rideY = location.y * 32;
        int32_t distance = abs(x - rideX) + abs(y - rideY);
        if (distance < resultDistance)
        {
            resultDistance = distance;
            rideIndex = index;
        }
    }

    return rideIndex;
}

void banner_reset_broken_index()
{
    for (BannerIndex bannerIndex = 0; bannerIndex < MAX_BANNERS; bannerIndex++)
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
                if (tileElement->GetType() == TILE_ELEMENT_TYPE_BANNER)
                {
                    uint8_t bannerIndex = tileElement->properties.banner.index;
                    if (activeBanners[bannerIndex])
                    {
                        log_info(
                            "Duplicated banner with index %d found at x = %d, y = %d and z = %d.", bannerIndex, x, y,
                            tileElement->base_height);

                        // Banner index is already in use by another banner, so duplicate it
                        BannerIndex newBannerIndex = create_new_banner(GAME_COMMAND_FLAG_APPLY);
                        if (newBannerIndex == BANNER_INDEX_NULL)
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
            } while (!(tileElement++)->IsLastForTile());
        }
    }
}

/**
 *
 *  rct2: 0x006BA058
 */
void game_command_remove_banner(
    int32_t *                  eax,
    int32_t *                  ebx,
    int32_t *                  ecx,
    int32_t *                  edx,
    [[maybe_unused]] int32_t * esi,
    [[maybe_unused]] int32_t * edi,
    [[maybe_unused]] int32_t * ebp)
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
void game_command_set_banner_colour(
    int32_t *                  eax,
    int32_t *                  ebx,
    int32_t *                  ecx,
    int32_t *                  edx,
    [[maybe_unused]] int32_t * esi,
    [[maybe_unused]] int32_t * edi,
    int32_t *                  ebp)
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
void game_command_place_banner(
    int32_t * eax, int32_t * ebx, int32_t * ecx, int32_t * edx, [[maybe_unused]] int32_t * esi, int32_t * edi, int32_t * ebp)
{
    *ebx = BannerPlace(
        *eax & 0xFFFF,
        *ecx & 0xFFFF,
        *edx & 0xFF,
        (*edx >> 8) & 0xFF,
        *ebp & 0xFF,
        (*ebx >> 8) & 0xFF,
        (BannerIndex*)edi,
        *ebx & 0xFF
    );
}

void game_command_set_banner_style(
    [[maybe_unused]] int32_t * eax,
    int32_t *                  ebx,
    int32_t *                  ecx,
    int32_t *                  edx,
    [[maybe_unused]] int32_t * esi,
    int32_t *                  edi,
    int32_t *                  ebp)
{
    *ebx = BannerSetStyle(
        *ecx & 0xFF,
        *edx & 0xFF,
        *edi & 0xFF,
        *ebp & 0xFF,
        *ebx & 0xFF
    );
}
