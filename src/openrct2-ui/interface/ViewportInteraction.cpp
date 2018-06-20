/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/core/Math.hpp>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/LargeScenery.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Sprite.h>
#include <openrct2/world/Surface.h>
#include <openrct2/world/Wall.h>
#include <openrct2/Context.h>
#include <openrct2/actions/WallRemoveAction.hpp>
#include "Viewport.h"
#include "Window.h"

static void viewport_interaction_remove_scenery(rct_tile_element *tileElement, int32_t x, int32_t y);
static void viewport_interaction_remove_footpath(rct_tile_element *tileElement, int32_t x, int32_t y);
static void viewport_interaction_remove_footpath_item(rct_tile_element *tileElement, int32_t x, int32_t y);
static void viewport_interaction_remove_park_wall(rct_tile_element *tileElement, int32_t x, int32_t y);
static void viewport_interaction_remove_large_scenery(rct_tile_element *tileElement, int32_t x, int32_t y);
static void viewport_interaction_remove_park_entrance(rct_tile_element *tileElement, int32_t x, int32_t y);
static rct_peep *viewport_interaction_get_closest_peep(int32_t x, int32_t y, int32_t maxDistance);

/**
 *
 *  rct2: 0x006ED9D0
 */
int32_t viewport_interaction_get_item_left(int32_t x, int32_t y, viewport_interaction_info *info)
{
    rct_tile_element *tileElement;
    rct_sprite *sprite;
    rct_vehicle *vehicle;

    // No click input for title screen or scenario editor or track manager
    if (gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER))
        return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

    //
    if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && gS6Info.editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER)
        return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

    LocationXY16 mapCoord = {};
    get_map_coordinates_from_pos(x, y, VIEWPORT_INTERACTION_MASK_SPRITE & VIEWPORT_INTERACTION_MASK_RIDE & VIEWPORT_INTERACTION_MASK_PARK, &mapCoord.x, &mapCoord.y, &info->type, &info->tileElement, nullptr);
    info->x = mapCoord.x;
    info->y = mapCoord.y;
    tileElement = info->tileElement;
    sprite = (rct_sprite*)tileElement;

    switch (info->type) {
    case VIEWPORT_INTERACTION_ITEM_SPRITE:
        switch (sprite->unknown.sprite_identifier) {
        case SPRITE_IDENTIFIER_VEHICLE:
            vehicle = &(sprite->vehicle);
            if (vehicle->ride_subtype != 255)
                vehicle_set_map_toolbar(vehicle);
            else
                info->type = VIEWPORT_INTERACTION_ITEM_NONE;
            break;
        case SPRITE_IDENTIFIER_PEEP:
            peep_set_map_tooltip(&sprite->peep);
            break;
        }
        break;
    case VIEWPORT_INTERACTION_ITEM_RIDE:
        ride_set_map_tooltip(tileElement);
        break;
    case VIEWPORT_INTERACTION_ITEM_PARK:
        set_map_tooltip_format_arg(0, rct_string_id, gParkName);
        set_map_tooltip_format_arg(2, uint32_t, gParkNameArgs);
        break;
    default:
        info->type = VIEWPORT_INTERACTION_ITEM_NONE;
        break;
    }

    // If nothing is under cursor, find a close by peep
    if (info->type == VIEWPORT_INTERACTION_ITEM_NONE) {
        info->peep = viewport_interaction_get_closest_peep(x, y, 32);
        if (info->peep == nullptr)
            return VIEWPORT_INTERACTION_ITEM_NONE;

        info->type = VIEWPORT_INTERACTION_ITEM_SPRITE;
        info->x = info->peep->x;
        info->y = info->peep->y;
        peep_set_map_tooltip(info->peep);
    }

    return info->type;
}

int32_t viewport_interaction_left_over(int32_t x, int32_t y)
{
    viewport_interaction_info info;

    switch (viewport_interaction_get_item_left(x, y, &info)) {
    case VIEWPORT_INTERACTION_ITEM_SPRITE:
    case VIEWPORT_INTERACTION_ITEM_RIDE:
    case VIEWPORT_INTERACTION_ITEM_PARK:
        return 1;
    default:
        return 0;
    }
}

int32_t viewport_interaction_left_click(int32_t x, int32_t y)
{
    viewport_interaction_info info;

    switch (viewport_interaction_get_item_left(x, y, &info)) {
    case VIEWPORT_INTERACTION_ITEM_SPRITE:
        switch (info.sprite->unknown.sprite_identifier) {
        case SPRITE_IDENTIFIER_VEHICLE:
        {
            auto intent = Intent(WD_VEHICLE);
            intent.putExtra(INTENT_EXTRA_VEHICLE, info.vehicle);
            context_open_intent(&intent);
            break;
        }
        case SPRITE_IDENTIFIER_PEEP:
        {
            auto intent = Intent(WC_PEEP);
            intent.putExtra(INTENT_EXTRA_PEEP, info.peep);
            context_open_intent(&intent);
            break;
        }
        case SPRITE_IDENTIFIER_MISC:
            if (game_is_not_paused()) {
                switch (info.sprite->unknown.misc_identifier) {
                case SPRITE_MISC_BALLOON:
                    game_do_command(info.sprite->balloon.sprite_index, GAME_COMMAND_FLAG_APPLY, 0, 0, GAME_COMMAND_BALLOON_PRESS, 0, 0);
                    break;
                case SPRITE_MISC_DUCK:
                    duck_press(&info.sprite->duck);
                    break;
                }
            }
            break;
        }
        return 1;
    case VIEWPORT_INTERACTION_ITEM_RIDE:
    {
        auto intent = Intent(WD_TRACK);
        intent.putExtra(INTENT_EXTRA_TILE_ELEMENT, info.tileElement);
        context_open_intent(&intent);
        return true;
    }
    case VIEWPORT_INTERACTION_ITEM_PARK:
        context_open_window(WC_PARK_INFORMATION);
        return 1;
    default:
        return 0;
    }
}

/**
 *
 *  rct2: 0x006EDE88
 */
int32_t viewport_interaction_get_item_right(int32_t x, int32_t y, viewport_interaction_info *info)
{
    rct_tile_element *tileElement;
    rct_scenery_entry *sceneryEntry;
    rct_banner *banner;
    Ride *ride;
    int32_t i, stationIndex;

    // No click input for title screen or track manager
    if (gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_TRACK_MANAGER))
        return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

    //
    if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && gS6Info.editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER)
        return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

    LocationXY16 mapCoord = {};
    get_map_coordinates_from_pos(x, y, ~(VIEWPORT_INTERACTION_MASK_TERRAIN & VIEWPORT_INTERACTION_MASK_WATER), &mapCoord.x, &mapCoord.y, &info->type, &info->tileElement, nullptr);
    info->x = mapCoord.x;
    info->y = mapCoord.y;
    tileElement = info->tileElement;

    switch (info->type) {
    case VIEWPORT_INTERACTION_ITEM_SPRITE:
        if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || tileElement->type != 0)
            return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

        tileElement += 6;
        ride = get_ride(tileElement->type);
        if (ride->status == RIDE_STATUS_CLOSED) {
            set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
            set_map_tooltip_format_arg(2, rct_string_id, ride->name);
            set_map_tooltip_format_arg(4, uint32_t, ride->name_arguments);
        }
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_RIDE:
        if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
            return info->type = VIEWPORT_INTERACTION_ITEM_NONE;
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
            return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

        ride = get_ride(tile_element_get_ride_index(tileElement));
        if (ride->status != RIDE_STATUS_CLOSED)
            return info->type;

        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE) {
            rct_string_id stringId;
            if (tileElement->properties.entrance.type == ENTRANCE_TYPE_RIDE_ENTRANCE) {
                if (ride->num_stations > 1) {
                    stringId = STR_RIDE_STATION_X_ENTRANCE;
                } else {
                    stringId = STR_RIDE_ENTRANCE;
                }
            } else {
                if (ride->num_stations > 1) {
                    stringId = STR_RIDE_STATION_X_EXIT;
                } else {
                    stringId = STR_RIDE_EXIT;
                }
            }
            set_map_tooltip_format_arg(2, rct_string_id, stringId);
        } else if (track_element_is_station(tileElement)) {
            rct_string_id stringId;
            if (ride->num_stations > 1) {
                stringId = STR_RIDE_STATION_X;
            } else {
                stringId = STR_RIDE_STATION;
            }
            set_map_tooltip_format_arg(2, rct_string_id, stringId);
        } else {
            if (!gCheatsSandboxMode && !map_is_location_owned(info->x, info->y, tileElement->base_height << 4)) {
                return info->type = VIEWPORT_INTERACTION_ITEM_NONE;
            }

            set_map_tooltip_format_arg(2, rct_string_id, ride->name);
            set_map_tooltip_format_arg(4, uint32_t, ride->name_arguments);
            return info->type;
        }

        set_map_tooltip_format_arg(4, rct_string_id, ride->name);
        set_map_tooltip_format_arg(6, uint32_t, ride->name_arguments);
        set_map_tooltip_format_arg(10, rct_string_id, RideComponentNames[RideNameConvention[ride->type].station].capitalised);

        stationIndex = tile_element_get_station(tileElement);
        for (i = stationIndex; i >= 0; i--)
            if (ride->station_starts[i].xy == RCT_XY8_UNDEFINED)
                stationIndex--;
        stationIndex++;
        set_map_tooltip_format_arg(12, uint16_t, stationIndex);
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_WALL:
        sceneryEntry = get_wall_entry(tileElement->properties.scenery.type);
        if (sceneryEntry->wall.scrolling_mode != 255) {
            set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
            set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
            return info->type;
        }
        break;

    case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        sceneryEntry = get_large_scenery_entry(scenery_large_get_type(tileElement));
        if (sceneryEntry->large_scenery.scrolling_mode != 255) {
            set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
            set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
            return info->type;
        }
        break;

    case VIEWPORT_INTERACTION_ITEM_BANNER:
        banner = &gBanners[tileElement->properties.banner.index];
        sceneryEntry = get_banner_entry(banner->type);

        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
        set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
        return info->type;
    }

    if (!(input_test_flag(INPUT_FLAG_6)) || !(input_test_flag(INPUT_FLAG_TOOL_ACTIVE))) {
        if (window_find_by_class(WC_RIDE_CONSTRUCTION) == nullptr && window_find_by_class(WC_FOOTPATH) == nullptr) {
            return info->type = VIEWPORT_INTERACTION_ITEM_NONE;
        }
    }

    switch (info->type) {
    case VIEWPORT_INTERACTION_ITEM_SCENERY:
        sceneryEntry = get_small_scenery_entry(tileElement->properties.scenery.type);
        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
        set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
        set_map_tooltip_format_arg(2, rct_string_id, STR_FOOTPATH_MAP_TIP);
        if (footpath_element_is_queue(tileElement))
            set_map_tooltip_format_arg(2, rct_string_id, STR_QUEUE_LINE_MAP_TIP);
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM:
        sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tileElement));
        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
        if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN) {
            set_map_tooltip_format_arg(2, rct_string_id, STR_BROKEN);
            set_map_tooltip_format_arg(4, rct_string_id, sceneryEntry->name);
        } else {
            set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
        }
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_PARK:
        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
            break;

        if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
            break;

        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
        set_map_tooltip_format_arg(2, rct_string_id, STR_OBJECT_SELECTION_PARK_ENTRANCE);
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_WALL:
        sceneryEntry = get_wall_entry(tileElement->properties.scenery.type);
        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
        set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        sceneryEntry = get_large_scenery_entry(tileElement->properties.scenery.type & 0x3FF);
        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
        set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
        return info->type;
    }

    return info->type = VIEWPORT_INTERACTION_ITEM_NONE;
}

int32_t viewport_interaction_right_over(int32_t x, int32_t y)
{
    viewport_interaction_info info;

    return viewport_interaction_get_item_right(x, y, &info) != 0;
}

/**
 *
 *  rct2: 0x006E8A62
 */
int32_t viewport_interaction_right_click(int32_t x, int32_t y)
{
    CoordsXYE tileElement;
    viewport_interaction_info info;

    switch (viewport_interaction_get_item_right(x, y, &info)) {
    case VIEWPORT_INTERACTION_ITEM_NONE:
        return 0;

    case VIEWPORT_INTERACTION_ITEM_SPRITE:
        if (info.sprite->unknown.sprite_identifier == SPRITE_IDENTIFIER_VEHICLE)
            ride_construct(info.sprite->vehicle.ride);
        break;
    case VIEWPORT_INTERACTION_ITEM_RIDE:
        tileElement.x = info.x;
        tileElement.y = info.y;
        tileElement.element = info.tileElement;
        ride_modify(&tileElement);
        break;
    case VIEWPORT_INTERACTION_ITEM_SCENERY:
        viewport_interaction_remove_scenery(info.tileElement, info.x, info.y);
        break;
    case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
        viewport_interaction_remove_footpath(info.tileElement, info.x, info.y);
        break;
    case VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM:
        viewport_interaction_remove_footpath_item(info.tileElement, info.x, info.y);
        break;
    case VIEWPORT_INTERACTION_ITEM_PARK:
        viewport_interaction_remove_park_entrance(info.tileElement, info.x, info.y);
        break;
    case VIEWPORT_INTERACTION_ITEM_WALL:
        viewport_interaction_remove_park_wall(info.tileElement, info.x, info.y);
        break;
    case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        viewport_interaction_remove_large_scenery(info.tileElement, info.x, info.y);
        break;
    case VIEWPORT_INTERACTION_ITEM_BANNER:
        context_open_detail_window(WD_BANNER, info.tileElement->properties.banner.index);
        break;
    }

    return 1;
}

/**
 *
 *  rct2: 0x006E08D2
 */
static void viewport_interaction_remove_scenery(rct_tile_element *tileElement, int32_t x, int32_t y)
{
    gGameCommandErrorTitle = STR_CANT_REMOVE_THIS;
    game_do_command(
        x,
        (tileElement->type << 8) | 1,
        y,
        (tileElement->properties.scenery.type << 8) | tileElement->base_height,
        GAME_COMMAND_REMOVE_SCENERY,
        0,
        0
    );
}

/**
 *
 *  rct2: 0x006A614A
 */
static void viewport_interaction_remove_footpath(rct_tile_element *tileElement, int32_t x, int32_t y)
{
    int32_t z;
    rct_window *w;
    rct_tile_element *tileElement2;

    z = tileElement->base_height;

    w = window_find_by_class(WC_FOOTPATH);
    if (w != nullptr)
        footpath_provisional_update();

    tileElement2 = map_get_first_element_at(x / 32, y / 32);
    do {
        if (tileElement2->GetType() == TILE_ELEMENT_TYPE_PATH && tileElement2->base_height == z) {
            gGameCommandErrorTitle = STR_CANT_REMOVE_FOOTPATH_FROM_HERE;
            footpath_remove(x, y, z, GAME_COMMAND_FLAG_APPLY);
            break;
        }
    } while (!(tileElement2++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006A61AB
 */
static void viewport_interaction_remove_footpath_item(rct_tile_element *tileElement, int32_t x, int32_t y)
{
    int32_t type;

    type = footpath_element_get_type(tileElement);
    if (footpath_element_is_queue(tileElement))
        type |= 0x80;

    gGameCommandErrorTitle = STR_CANT_REMOVE_THIS;
    game_do_command(
        x,
        ((tileElement->properties.path.type & 7) << 8) | 1,
        y,
        (type << 8) | tileElement->base_height,
        GAME_COMMAND_PLACE_PATH,
        0,
        0
    );
}

/**
 *
 *  rct2: 0x00666C0E
 */
void viewport_interaction_remove_park_entrance(rct_tile_element *tileElement, int32_t x, int32_t y)
{
    int32_t rotation = tile_element_get_direction_with_offset(tileElement, 1);
    switch (tileElement->properties.entrance.index & 0x0F) {
    case 1:
        x += CoordsDirectionDelta[rotation].x;
        y += CoordsDirectionDelta[rotation].y;
        break;
    case 2:
        x -= CoordsDirectionDelta[rotation].x;
        y -= CoordsDirectionDelta[rotation].y;
        break;
    }
    gGameCommandErrorTitle = STR_CANT_REMOVE_THIS;
    game_do_command(x, GAME_COMMAND_FLAG_APPLY, y, tileElement->base_height / 2, GAME_COMMAND_REMOVE_PARK_ENTRANCE, 0, 0);
}

/**
 *
 *  rct2: 0x006E57A9
 */
static void viewport_interaction_remove_park_wall(rct_tile_element *tileElement, int32_t x, int32_t y)
{
    rct_scenery_entry *sceneryEntry = get_wall_entry(tileElement->properties.wall.type);
    if (sceneryEntry->wall.scrolling_mode != 0xFF)
    {
        context_open_detail_window(WD_SIGN_SMALL, tileElement->properties.wall.banner_index);
    }
    else
    {
        TileCoordsXYZD wallLocation = { x >> 5, y >> 5, tileElement->base_height, tileElement->GetDirection() };
        auto wallRemoveAction = WallRemoveAction(wallLocation);
        GameActions::Execute(&wallRemoveAction);
    }
}

/**
 *
 *  rct2: 0x006B88DC
 */
static void viewport_interaction_remove_large_scenery(rct_tile_element *tileElement, int32_t x, int32_t y)
{
    rct_scenery_entry *sceneryEntry = get_large_scenery_entry(scenery_large_get_type(tileElement));

    if (sceneryEntry->large_scenery.scrolling_mode != 0xFF)
    {
        BannerIndex bannerIndex = scenery_large_get_banner_id(tileElement);
        context_open_detail_window(WD_SIGN, bannerIndex);
    } else {
        gGameCommandErrorTitle = STR_CANT_REMOVE_THIS;
        game_do_command(
            x,
            1 | (tile_element_get_direction(tileElement) << 8),
            y,
            tileElement->base_height | (scenery_large_get_sequence(tileElement) << 8),
            GAME_COMMAND_REMOVE_LARGE_SCENERY,
            0,
            0
        );
    }
}

static rct_peep *viewport_interaction_get_closest_peep(int32_t x, int32_t y, int32_t maxDistance)
{
    int32_t distance, closestDistance;
    uint16_t spriteIndex;
    rct_window *w;
    rct_viewport *viewport;
    rct_peep *peep, *closestPeep;

    w = window_find_from_point(x, y);
    if (w == nullptr)
        return nullptr;

    viewport = w->viewport;
    if (viewport == nullptr || viewport->zoom >= 2)
        return nullptr;

    x = ((x - viewport->x) << viewport->zoom) + viewport->view_x;
    y = ((y - viewport->y) << viewport->zoom) + viewport->view_y;

    closestPeep = nullptr;
    closestDistance = 0xFFFF;
    FOR_ALL_PEEPS(spriteIndex, peep) {
        if (peep->sprite_left == LOCATION_NULL)
            continue;

        distance =
            abs(((peep->sprite_left + peep->sprite_right) / 2) - x) +
            abs(((peep->sprite_top + peep->sprite_bottom) / 2) - y);
        if (distance > maxDistance)
            continue;

        if (distance < closestDistance) {
            closestPeep = peep;
            closestDistance = distance;
        }
    }

    return closestPeep;
}

/**
 *
 *  rct2: 0x0068A15E
 */
void sub_68A15E(int32_t screenX, int32_t screenY, int16_t *x, int16_t *y, int32_t *direction, rct_tile_element **tileElement)
{
    int16_t my_x, my_y;
    int32_t interactionType;
    rct_tile_element *myTileElement;
    rct_viewport *viewport;
    get_map_coordinates_from_pos(screenX, screenY, VIEWPORT_INTERACTION_MASK_TERRAIN & VIEWPORT_INTERACTION_MASK_WATER, &my_x, &my_y, &interactionType, &myTileElement, &viewport);

    if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE) {
        *x = LOCATION_NULL;
        return;
    }

    int16_t originalZ = 0;
    if (interactionType == VIEWPORT_INTERACTION_ITEM_WATER) {
        originalZ = surface_get_water_height(myTileElement) << 4;
    }

    LocationXY16 start_vp_pos = screen_coord_to_viewport_coord(viewport, screenX, screenY);
    LocationXY16 map_pos = { (int16_t)(my_x + 16), (int16_t)(my_y + 16) };

    for (int32_t i = 0; i < 5; i++) {
        int16_t z = originalZ;
        if (interactionType != VIEWPORT_INTERACTION_ITEM_WATER) {
            z = tile_element_height(map_pos.x, map_pos.y);
        }
        map_pos = viewport_coord_to_map_coord(start_vp_pos.x, start_vp_pos.y, z);
        map_pos.x = Math::Clamp<int16_t>(map_pos.x, my_x, my_x + 31);
        map_pos.y = Math::Clamp<int16_t>(map_pos.y, my_y, my_y + 31);
    }

    // Determine to which edge the cursor is closest
    int32_t myDirection;
    int32_t mod_x = map_pos.x & 0x1F;
    int32_t mod_y = map_pos.y & 0x1F;
    if (mod_x < mod_y) {
        if (mod_x + mod_y < 32) {
            myDirection = 0;
        } else {
            myDirection = 1;
        }
    } else {
        if (mod_x + mod_y < 32) {
            myDirection = 3;
        } else {
            myDirection = 2;
        }
    }

    *x = map_pos.x & ~0x1F;
    *y = map_pos.y & ~0x1F;
    if (direction != nullptr) *direction = myDirection;
    if (tileElement != nullptr) *tileElement = myTileElement;
}
