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

#include "../cheats.h"
#include "../editor.h"
#include "../game.h"
#include "../input.h"
#include "../localisation/localisation.h"
#include "../OpenRCT2.h"
#include "../ride/ride.h"
#include "../ride/ride_data.h"
#include "../ride/track.h"
#include "../scenario/scenario.h"
#include "../world/banner.h"
#include "../world/footpath.h"
#include "../world/map.h"
#include "../world/scenery.h"
#include "../world/sprite.h"
#include "viewport.h"

static void viewport_interaction_remove_scenery(rct_map_element *mapElement, sint32 x, sint32 y);
static void viewport_interaction_remove_footpath(rct_map_element *mapElement, sint32 x, sint32 y);
static void viewport_interaction_remove_footpath_item(rct_map_element *mapElement, sint32 x, sint32 y);
static void viewport_interaction_remove_park_wall(rct_map_element *mapElement, sint32 x, sint32 y);
static void viewport_interaction_remove_large_scenery(rct_map_element *mapElement, sint32 x, sint32 y);
static rct_peep *viewport_interaction_get_closest_peep(sint32 x, sint32 y, sint32 maxDistance);

/**
 *
 *  rct2: 0x006ED9D0
 */
sint32 viewport_interaction_get_item_left(sint32 x, sint32 y, viewport_interaction_info *info)
{
    rct_map_element *mapElement;
    rct_sprite *sprite;
    rct_vehicle *vehicle;

    // No click input for title screen or scenario editor or track manager
    if (gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER))
        return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

    //
    if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && gS6Info.editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER)
        return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

    rct_xy16 mapCoord = { 0 };
    get_map_coordinates_from_pos(x, y, VIEWPORT_INTERACTION_MASK_SPRITE & VIEWPORT_INTERACTION_MASK_RIDE & VIEWPORT_INTERACTION_MASK_PARK, &mapCoord.x, &mapCoord.y, &info->type, &info->mapElement, NULL);
    info->x = mapCoord.x;
    info->y = mapCoord.y;
    mapElement = info->mapElement;
    sprite = (rct_sprite*)mapElement;

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
        ride_set_map_tooltip(mapElement);
        break;
    case VIEWPORT_INTERACTION_ITEM_PARK:
        set_map_tooltip_format_arg(0, rct_string_id, gParkName);
        set_map_tooltip_format_arg(2, uint32, gParkNameArgs);
        break;
    default:
        info->type = VIEWPORT_INTERACTION_ITEM_NONE;
        break;
    }

    // If nothing is under cursor, find a close by peep
    if (info->type == VIEWPORT_INTERACTION_ITEM_NONE) {
        info->peep = viewport_interaction_get_closest_peep(x, y, 32);
        if (info->peep == NULL)
            return VIEWPORT_INTERACTION_ITEM_NONE;

        info->type = VIEWPORT_INTERACTION_ITEM_SPRITE;
        info->x = info->peep->x;
        info->y = info->peep->y;
        peep_set_map_tooltip(info->peep);
    }

    return info->type;
}

sint32 viewport_interaction_left_over(sint32 x, sint32 y)
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

sint32 viewport_interaction_left_click(sint32 x, sint32 y)
{
    viewport_interaction_info info;

    switch (viewport_interaction_get_item_left(x, y, &info)) {
    case VIEWPORT_INTERACTION_ITEM_SPRITE:
        switch (info.sprite->unknown.sprite_identifier) {
        case SPRITE_IDENTIFIER_VEHICLE:
            window_ride_open_vehicle(info.vehicle);
            break;
        case SPRITE_IDENTIFIER_PEEP:
            window_guest_open(info.peep);
            break;
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
        window_ride_open_track(info.mapElement);
        return 1;
    case VIEWPORT_INTERACTION_ITEM_PARK:
        window_park_entrance_open();
        return 1;
    default:
        return 0;
    }
}

/**
 *
 *  rct2: 0x006EDE88
 */
sint32 viewport_interaction_get_item_right(sint32 x, sint32 y, viewport_interaction_info *info)
{
    rct_map_element *mapElement;
    rct_scenery_entry *sceneryEntry;
    rct_banner *banner;
    Ride *ride;
    sint32 i, stationIndex;

    // No click input for title screen or track manager
    if (gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_TRACK_MANAGER))
        return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

    //
    if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && gS6Info.editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER)
        return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

    rct_xy16 mapCoord = { 0 };
    get_map_coordinates_from_pos(x, y, ~(VIEWPORT_INTERACTION_MASK_TERRAIN & VIEWPORT_INTERACTION_MASK_WATER), &mapCoord.x, &mapCoord.y, &info->type, &info->mapElement, NULL);
    info->x = mapCoord.x;
    info->y = mapCoord.y;
    mapElement = info->mapElement;

    switch (info->type) {
    case VIEWPORT_INTERACTION_ITEM_SPRITE:
        if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || mapElement->type != 0)
            return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

        mapElement += 6;
        ride = get_ride(mapElement->type);
        if (ride->status == RIDE_STATUS_CLOSED) {
            set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
            set_map_tooltip_format_arg(2, rct_string_id, ride->name);
            set_map_tooltip_format_arg(4, uint32, ride->name_arguments);
        }
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_RIDE:
        if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
            return info->type = VIEWPORT_INTERACTION_ITEM_NONE;
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH)
            return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

        ride = get_ride(mapElement->properties.track.ride_index);
        if (ride->status != RIDE_STATUS_CLOSED)
            return info->type;

        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);

        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE) {
            rct_string_id stringId;
            if (mapElement->properties.track.type == ENTRANCE_TYPE_RIDE_ENTRANCE) {
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
        } else if (track_element_is_station(mapElement)) {
            rct_string_id stringId;
            if (ride->num_stations > 1) {
                stringId = STR_RIDE_STATION_X;
            } else {
                stringId = STR_RIDE_STATION;
            }
            set_map_tooltip_format_arg(2, rct_string_id, stringId);
        } else {
            if (!gCheatsSandboxMode && !map_is_location_owned(info->x, info->y, mapElement->base_height << 4)) {
                return info->type = VIEWPORT_INTERACTION_ITEM_NONE;
            }

            set_map_tooltip_format_arg(2, rct_string_id, ride->name);
            set_map_tooltip_format_arg(4, uint32, ride->name_arguments);
            return info->type;
        }

        set_map_tooltip_format_arg(4, rct_string_id, ride->name);
        set_map_tooltip_format_arg(6, uint32, ride->name_arguments);
        set_map_tooltip_format_arg(10, rct_string_id, RideComponentNames[RideNameConvention[ride->type].station].capitalised);

        stationIndex = map_element_get_station(mapElement);
        for (i = stationIndex; i >= 0; i--)
            if (ride->station_starts[i].xy == RCT_XY8_UNDEFINED)
                stationIndex--;
        stationIndex++;
        set_map_tooltip_format_arg(12, uint16, stationIndex);
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_WALL:
        sceneryEntry = get_wall_entry(mapElement->properties.scenery.type);
        if (sceneryEntry->wall.scrolling_mode != 255) {
            set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
            set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
            return info->type;
        }
        break;

    case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        sceneryEntry = get_large_scenery_entry(mapElement->properties.scenerymultiple.type & 0x3FF);
        if (sceneryEntry->large_scenery.scrolling_mode != 255) {
            set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
            set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
            return info->type;
        }
        break;

    case VIEWPORT_INTERACTION_ITEM_BANNER:
        banner = &gBanners[mapElement->properties.banner.index];
        sceneryEntry = get_banner_entry(banner->type);

        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
        set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
        return info->type;
    }

    if (!(input_test_flag(INPUT_FLAG_6)) || !(input_test_flag(INPUT_FLAG_TOOL_ACTIVE))) {
        if (window_find_by_class(WC_RIDE_CONSTRUCTION) == NULL && window_find_by_class(WC_FOOTPATH) == NULL) {
            return info->type = VIEWPORT_INTERACTION_ITEM_NONE;
        }
    }

    switch (info->type) {
    case VIEWPORT_INTERACTION_ITEM_SCENERY:
        sceneryEntry = get_small_scenery_entry(mapElement->properties.scenery.type);
        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
        set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
        set_map_tooltip_format_arg(2, rct_string_id, STR_FOOTPATH_MAP_TIP);
        if (mapElement->type & 1)
            set_map_tooltip_format_arg(2, rct_string_id, STR_QUEUE_LINE_MAP_TIP);
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM:
        sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(mapElement));
        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
        if (mapElement->flags & MAP_ELEMENT_FLAG_BROKEN) {
            set_map_tooltip_format_arg(2, rct_string_id, STR_BROKEN);
            set_map_tooltip_format_arg(4, rct_string_id, sceneryEntry->name);
        } else {
            set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
        }
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_PARK:
        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
            break;

        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE)
            break;

        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
        set_map_tooltip_format_arg(2, rct_string_id, STR_OBJECT_SELECTION_PARK_ENTRANCE);
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_WALL:
        sceneryEntry = get_wall_entry(mapElement->properties.scenery.type);
        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
        set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
        return info->type;

    case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        sceneryEntry = get_large_scenery_entry(mapElement->properties.scenery.type & 0x3FF);
        set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
        set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
        return info->type;
    }

    return info->type = VIEWPORT_INTERACTION_ITEM_NONE;
}

sint32 viewport_interaction_right_over(sint32 x, sint32 y)
{
    viewport_interaction_info info;

    return viewport_interaction_get_item_right(x, y, &info) != 0;
}

/**
 *
 *  rct2: 0x006E8A62
 */
sint32 viewport_interaction_right_click(sint32 x, sint32 y)
{
    rct_xy_element mapElement;
    viewport_interaction_info info;

    switch (viewport_interaction_get_item_right(x, y, &info)) {
    case VIEWPORT_INTERACTION_ITEM_NONE:
        return 0;

    case VIEWPORT_INTERACTION_ITEM_SPRITE:
        if (info.sprite->unknown.sprite_identifier == SPRITE_IDENTIFIER_VEHICLE)
            ride_construct(info.sprite->vehicle.ride);
        break;
    case VIEWPORT_INTERACTION_ITEM_RIDE:
        mapElement.x = info.x;
        mapElement.y = info.y;
        mapElement.element = info.mapElement;
        ride_modify(&mapElement);
        break;
    case VIEWPORT_INTERACTION_ITEM_SCENERY:
        viewport_interaction_remove_scenery(info.mapElement, info.x, info.y);
        break;
    case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
        viewport_interaction_remove_footpath(info.mapElement, info.x, info.y);
        break;
    case VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM:
        viewport_interaction_remove_footpath_item(info.mapElement, info.x, info.y);
        break;
    case VIEWPORT_INTERACTION_ITEM_PARK:
        viewport_interaction_remove_park_entrance(info.mapElement, info.x, info.y);
        break;
    case VIEWPORT_INTERACTION_ITEM_WALL:
        viewport_interaction_remove_park_wall(info.mapElement, info.x, info.y);
        break;
    case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        viewport_interaction_remove_large_scenery(info.mapElement, info.x, info.y);
        break;
    case VIEWPORT_INTERACTION_ITEM_BANNER:
        window_banner_open(info.mapElement->properties.banner.index);
        break;
    }

    return 1;
}

/**
 *
 *  rct2: 0x006E08D2
 */
static void viewport_interaction_remove_scenery(rct_map_element *mapElement, sint32 x, sint32 y)
{
    gGameCommandErrorTitle = STR_CANT_REMOVE_THIS;
    game_do_command(
        x,
        (mapElement->type << 8) | 1,
        y,
        (mapElement->properties.scenery.type << 8) | mapElement->base_height,
        GAME_COMMAND_REMOVE_SCENERY,
        0,
        0
    );
}

/**
 *
 *  rct2: 0x006A614A
 */
static void viewport_interaction_remove_footpath(rct_map_element *mapElement, sint32 x, sint32 y)
{
    sint32 z;
    rct_window *w;
    rct_map_element *mapElement2;

    z = mapElement->base_height;

    w = window_find_by_class(WC_FOOTPATH);
    if (w != NULL)
        footpath_provisional_update();

    mapElement2 = map_get_first_element_at(x / 32, y / 32);
    do {
        if (map_element_get_type(mapElement2) == MAP_ELEMENT_TYPE_PATH && mapElement2->base_height == z) {
            gGameCommandErrorTitle = STR_CANT_REMOVE_FOOTPATH_FROM_HERE;
            footpath_remove(x, y, z, 1);
            break;
        }
    } while (!map_element_is_last_for_tile(mapElement2++));
}

/**
 *
 *  rct2: 0x006A61AB
 */
static void viewport_interaction_remove_footpath_item(rct_map_element *mapElement, sint32 x, sint32 y)
{
    sint32 type;

    type = mapElement->properties.path.type >> 4;
    if (mapElement->type & 1)
        type |= 0x80;

    gGameCommandErrorTitle = STR_CANT_REMOVE_THIS;
    game_do_command(
        x,
        ((mapElement->properties.path.type & 7) << 8) | 1,
        y,
        (type << 8) | mapElement->base_height,
        GAME_COMMAND_PLACE_PATH,
        0,
        0
    );
}

/**
 *
 *  rct2: 0x00666C0E
 */
void viewport_interaction_remove_park_entrance(rct_map_element *mapElement, sint32 x, sint32 y)
{
    sint32 rotation = map_element_get_direction_with_offset(mapElement, 1);
    switch (mapElement->properties.entrance.index & 0x0F) {
    case 1:
        x += TileDirectionDelta[rotation].x;
        y += TileDirectionDelta[rotation].y;
        break;
    case 2:
        x -= TileDirectionDelta[rotation].x;
        y -= TileDirectionDelta[rotation].y;
        break;
    }
    gGameCommandErrorTitle = STR_CANT_REMOVE_THIS;
    game_do_command(x, GAME_COMMAND_FLAG_APPLY, y, mapElement->base_height / 2, GAME_COMMAND_REMOVE_PARK_ENTRANCE, 0, 0);
}

/**
 *
 *  rct2: 0x006E57A9
 */
static void viewport_interaction_remove_park_wall(rct_map_element *mapElement, sint32 x, sint32 y)
{
    rct_scenery_entry *sceneryEntry = get_wall_entry(mapElement->properties.wall.type);
    if (sceneryEntry->wall.scrolling_mode != 0xFF){
        window_sign_small_open(mapElement->properties.wall.banner_index);
    } else {
        gGameCommandErrorTitle = STR_CANT_REMOVE_THIS;
        game_do_command(
            x,
            GAME_COMMAND_FLAG_APPLY,
            y,
            map_element_get_direction(mapElement) | (mapElement->base_height << 8),
            GAME_COMMAND_REMOVE_WALL,
            0,
            0
        );
    }
}

/**
 *
 *  rct2: 0x006B88DC
 */
static void viewport_interaction_remove_large_scenery(rct_map_element *mapElement, sint32 x, sint32 y)
{
    rct_scenery_entry *sceneryEntry = get_large_scenery_entry(mapElement->properties.scenerymultiple.type & MAP_ELEMENT_LARGE_TYPE_MASK);

    if (sceneryEntry->large_scenery.scrolling_mode != 0xFF){
        sint32 id = (mapElement->type & 0xC0) |
            ((mapElement->properties.scenerymultiple.colour[0] & 0xE0) >> 2) |
            ((mapElement->properties.scenerymultiple.colour[1] & 0xE0) >> 5);
        window_sign_open(id);
    } else {
        gGameCommandErrorTitle = STR_CANT_REMOVE_THIS;
        game_do_command(
            x,
            1 | (map_element_get_direction(mapElement) << 8),
            y,
            mapElement->base_height | ((mapElement->properties.scenerymultiple.type >> 10) << 8),
            GAME_COMMAND_REMOVE_LARGE_SCENERY,
            0,
            0
        );
    }
}

static rct_peep *viewport_interaction_get_closest_peep(sint32 x, sint32 y, sint32 maxDistance)
{
    sint32 distance, closestDistance;
    uint16 spriteIndex;
    rct_window *w;
    rct_viewport *viewport;
    rct_peep *peep, *closestPeep;

    w = window_find_from_point(x, y);
    if (w == NULL)
        return 0;

    viewport = w->viewport;
    if (viewport == NULL || viewport->zoom >= 2)
        return 0;

    x = ((x - viewport->x) << viewport->zoom) + viewport->view_x;
    y = ((y - viewport->y) << viewport->zoom) + viewport->view_y;

    closestPeep = NULL;
    closestDistance = 0xFFFF;
    FOR_ALL_PEEPS(spriteIndex, peep) {
        if (peep->sprite_left == (sint16)(uint16)0x8000)
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
void sub_68A15E(sint32 screenX, sint32 screenY, sint16 *x, sint16 *y, sint32 *direction, rct_map_element **mapElement)
{
    sint16 my_x, my_y;
    sint32 interactionType;
    rct_map_element *myMapElement;
    rct_viewport *viewport;
    get_map_coordinates_from_pos(screenX, screenY, VIEWPORT_INTERACTION_MASK_TERRAIN & VIEWPORT_INTERACTION_MASK_WATER, &my_x, &my_y, &interactionType, &myMapElement, &viewport);

    if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE) {
        *x = 0x8000;
        return;
    }

    sint16 originalZ = 0;
    if (interactionType == VIEWPORT_INTERACTION_ITEM_WATER) {
        originalZ = map_get_water_height(myMapElement) << 4;
    }

    rct_xy16 start_vp_pos = screen_coord_to_viewport_coord(viewport, screenX, screenY);
    rct_xy16 map_pos = { my_x + 16, my_y + 16 };

    for (sint32 i = 0; i < 5; i++) {
        sint16 z = originalZ;
        if (interactionType != VIEWPORT_INTERACTION_ITEM_WATER) {
            z = map_element_height(map_pos.x, map_pos.y);
        }
        map_pos = viewport_coord_to_map_coord(start_vp_pos.x, start_vp_pos.y, z);
        map_pos.x = clamp(my_x, map_pos.x, my_x + 31);
        map_pos.y = clamp(my_y, map_pos.y, my_y + 31);
    }

    // Determine to which edge the cursor is closest
    sint32 myDirection;
    sint32 mod_x = map_pos.x & 0x1F;
    sint32 mod_y = map_pos.y & 0x1F;
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
    if (direction != NULL) *direction = myDirection;
    if (mapElement != NULL) *mapElement = myMapElement;
}
