/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Viewport.h"
#include "Window.h"

#include <algorithm>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/LargeSceneryRemoveAction.hpp>
#include <openrct2/actions/SmallSceneryRemoveAction.hpp>
#include <openrct2/actions/WallRemoveAction.hpp>
#include <openrct2/actions/FootpathSceneryRemoveAction.hpp>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/LargeScenery.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/Sprite.h>
#include <openrct2/world/Surface.h>
#include <openrct2/world/Wall.h>

static void viewport_interaction_remove_scenery(TileElement* tileElement, int32_t x, int32_t y);
static void viewport_interaction_remove_footpath(TileElement* tileElement, int32_t x, int32_t y);
static void viewport_interaction_remove_footpath_item(TileElement* tileElement, int32_t x, int32_t y);
static void viewport_interaction_remove_park_wall(TileElement* tileElement, int32_t x, int32_t y);
static void viewport_interaction_remove_large_scenery(TileElement* tileElement, int32_t x, int32_t y);
static void viewport_interaction_remove_park_entrance(TileElement* tileElement, int32_t x, int32_t y);
static Peep* viewport_interaction_get_closest_peep(int32_t x, int32_t y, int32_t maxDistance);

/**
 *
 *  rct2: 0x006ED9D0
 */
int32_t viewport_interaction_get_item_left(int32_t x, int32_t y, viewport_interaction_info* info)
{
    TileElement* tileElement;
    rct_sprite* sprite;
    rct_vehicle* vehicle;

    // No click input for title screen or scenario editor or track manager
    if (gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_MANAGER))
        return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

    //
    if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && gS6Info.editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER)
        return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

    LocationXY16 mapCoord = {};
    get_map_coordinates_from_pos(
        x, y, VIEWPORT_INTERACTION_MASK_SPRITE & VIEWPORT_INTERACTION_MASK_RIDE & VIEWPORT_INTERACTION_MASK_PARK, &mapCoord.x,
        &mapCoord.y, &info->type, &info->tileElement, nullptr);
    info->x = mapCoord.x;
    info->y = mapCoord.y;
    tileElement = info->tileElement;
    sprite = (rct_sprite*)tileElement;

    switch (info->type)
    {
        case VIEWPORT_INTERACTION_ITEM_SPRITE:
            switch (sprite->generic.sprite_identifier)
            {
                case SPRITE_IDENTIFIER_VEHICLE:
                    vehicle = &(sprite->vehicle);
                    if (vehicle->ride_subtype != RIDE_ENTRY_INDEX_NULL)
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
    if (info->type == VIEWPORT_INTERACTION_ITEM_NONE)
    {
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

    switch (viewport_interaction_get_item_left(x, y, &info))
    {
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

    switch (viewport_interaction_get_item_left(x, y, &info))
    {
        case VIEWPORT_INTERACTION_ITEM_SPRITE:
            switch (info.sprite->generic.sprite_identifier)
            {
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
                    if (game_is_not_paused())
                    {
                        switch (info.sprite->generic.type)
                        {
                            case SPRITE_MISC_BALLOON:
                                game_do_command(
                                    info.sprite->balloon.sprite_index, GAME_COMMAND_FLAG_APPLY, 0, 0,
                                    GAME_COMMAND_BALLOON_PRESS, 0, 0);
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
int32_t viewport_interaction_get_item_right(int32_t x, int32_t y, viewport_interaction_info* info)
{
    TileElement* tileElement;
    rct_scenery_entry* sceneryEntry;
    rct_banner* banner;
    Ride* ride;
    int32_t i, stationIndex;

    // No click input for title screen or track manager
    if (gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_TRACK_MANAGER))
        return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

    //
    if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) && gS6Info.editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER)
        return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

    LocationXY16 mapCoord = {};
    get_map_coordinates_from_pos(
        x, y, ~(VIEWPORT_INTERACTION_MASK_TERRAIN & VIEWPORT_INTERACTION_MASK_WATER), &mapCoord.x, &mapCoord.y, &info->type,
        &info->tileElement, nullptr);
    info->x = mapCoord.x;
    info->y = mapCoord.y;
    tileElement = info->tileElement;
    rct_sprite* sprite = info->sprite;

    switch (info->type)
    {
        case VIEWPORT_INTERACTION_ITEM_SPRITE:
            if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || sprite->generic.sprite_identifier != SPRITE_IDENTIFIER_VEHICLE)
                return info->type = VIEWPORT_INTERACTION_ITEM_NONE;

            ride = get_ride(sprite->vehicle.ride);
            if (ride->status == RIDE_STATUS_CLOSED)
            {
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

            if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
            {
                rct_string_id stringId;
                if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
                {
                    if (ride->num_stations > 1)
                    {
                        stringId = STR_RIDE_STATION_X_ENTRANCE;
                    }
                    else
                    {
                        stringId = STR_RIDE_ENTRANCE;
                    }
                }
                else
                {
                    if (ride->num_stations > 1)
                    {
                        stringId = STR_RIDE_STATION_X_EXIT;
                    }
                    else
                    {
                        stringId = STR_RIDE_EXIT;
                    }
                }
                set_map_tooltip_format_arg(2, rct_string_id, stringId);
            }
            else if (track_element_is_station(tileElement))
            {
                rct_string_id stringId;
                if (ride->num_stations > 1)
                {
                    stringId = STR_RIDE_STATION_X;
                }
                else
                {
                    stringId = STR_RIDE_STATION;
                }
                set_map_tooltip_format_arg(2, rct_string_id, stringId);
            }
            else
            {
                if (!gCheatsSandboxMode && !map_is_location_owned(info->x, info->y, tileElement->base_height << 4))
                {
                    return info->type = VIEWPORT_INTERACTION_ITEM_NONE;
                }

                set_map_tooltip_format_arg(2, rct_string_id, ride->name);
                set_map_tooltip_format_arg(4, uint32_t, ride->name_arguments);
                return info->type;
            }

            set_map_tooltip_format_arg(4, rct_string_id, ride->name);
            set_map_tooltip_format_arg(6, uint32_t, ride->name_arguments);
            set_map_tooltip_format_arg(
                10, rct_string_id, RideComponentNames[RideNameConvention[ride->type].station].capitalised);

            if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
                stationIndex = tileElement->AsEntrance()->GetStationIndex();
            else
                stationIndex = tileElement->AsTrack()->GetStationIndex();

            for (i = stationIndex; i >= 0; i--)
                if (ride->stations[i].Start.xy == RCT_XY8_UNDEFINED)
                    stationIndex--;
            stationIndex++;
            set_map_tooltip_format_arg(12, uint16_t, stationIndex);
            return info->type;

        case VIEWPORT_INTERACTION_ITEM_WALL:
            sceneryEntry = tileElement->AsWall()->GetEntry();
            if (sceneryEntry->wall.scrolling_mode != SCROLLING_MODE_NONE)
            {
                set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
                set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
                return info->type;
            }
            break;

        case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
            sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            if (sceneryEntry->large_scenery.scrolling_mode != SCROLLING_MODE_NONE)
            {
                set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
                set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
                return info->type;
            }
            break;

        case VIEWPORT_INTERACTION_ITEM_BANNER:
            banner = &gBanners[tileElement->AsBanner()->GetIndex()];
            sceneryEntry = get_banner_entry(banner->type);

            set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_MODIFY);
            set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
            return info->type;
    }

    if (!(input_test_flag(INPUT_FLAG_6)) || !(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
    {
        if (window_find_by_class(WC_RIDE_CONSTRUCTION) == nullptr && window_find_by_class(WC_FOOTPATH) == nullptr)
        {
            return info->type = VIEWPORT_INTERACTION_ITEM_NONE;
        }
    }

    switch (info->type)
    {
        case VIEWPORT_INTERACTION_ITEM_SCENERY:
            sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
            set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
            set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
            return info->type;

        case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
            set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
            set_map_tooltip_format_arg(2, rct_string_id, STR_FOOTPATH_MAP_TIP);
            if (tileElement->AsPath()->IsQueue())
                set_map_tooltip_format_arg(2, rct_string_id, STR_QUEUE_LINE_MAP_TIP);
            return info->type;

        case VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM:
            sceneryEntry = tileElement->AsPath()->GetAdditionEntry();
            set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
            if (tileElement->AsPath()->IsBroken())
            {
                set_map_tooltip_format_arg(2, rct_string_id, STR_BROKEN);
                set_map_tooltip_format_arg(4, rct_string_id, sceneryEntry->name);
            }
            else
            {
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
            sceneryEntry = tileElement->AsWall()->GetEntry();
            set_map_tooltip_format_arg(0, rct_string_id, STR_MAP_TOOLTIP_STRINGID_CLICK_TO_REMOVE);
            set_map_tooltip_format_arg(2, rct_string_id, sceneryEntry->name);
            return info->type;

        case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
            sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
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

    switch (viewport_interaction_get_item_right(x, y, &info))
    {
        case VIEWPORT_INTERACTION_ITEM_NONE:
            return 0;

        case VIEWPORT_INTERACTION_ITEM_SPRITE:
            if (info.sprite->generic.sprite_identifier == SPRITE_IDENTIFIER_VEHICLE)
                ride_construct(get_ride(info.sprite->vehicle.ride));
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
            context_open_detail_window(WD_BANNER, info.tileElement->AsBanner()->GetIndex());
            break;
    }

    return 1;
}

/**
 *
 *  rct2: 0x006E08D2
 */
static void viewport_interaction_remove_scenery(TileElement* tileElement, int32_t x, int32_t y)
{
    auto removeSceneryAction = SmallSceneryRemoveAction(
        x, y, tileElement->base_height, tileElement->AsSmallScenery()->GetSceneryQuadrant(),
        tileElement->AsSmallScenery()->GetEntryIndex());

    GameActions::Execute(&removeSceneryAction);
}

/**
 *
 *  rct2: 0x006A614A
 */
static void viewport_interaction_remove_footpath(TileElement* tileElement, int32_t x, int32_t y)
{
    int32_t z;
    rct_window* w;
    TileElement* tileElement2;

    z = tileElement->base_height;

    w = window_find_by_class(WC_FOOTPATH);
    if (w != nullptr)
        footpath_provisional_update();

    tileElement2 = map_get_first_element_at(x / 32, y / 32);
    do
    {
        if (tileElement2->GetType() == TILE_ELEMENT_TYPE_PATH && tileElement2->base_height == z)
        {
            footpath_remove(x, y, z, GAME_COMMAND_FLAG_APPLY);
            break;
        }
    } while (!(tileElement2++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006A61AB
 */
static void viewport_interaction_remove_footpath_item(TileElement* tileElement, int32_t x, int32_t y)
{
    auto footpathSceneryRemoveAction = FootpathSceneryRemoveAction({x, y, tileElement->base_height * 8});
    GameActions::Execute(&footpathSceneryRemoveAction);
}

/**
 *
 *  rct2: 0x00666C0E
 */
void viewport_interaction_remove_park_entrance(TileElement* tileElement, int32_t x, int32_t y)
{
    int32_t rotation = tileElement->GetDirectionWithOffset(1);
    switch (tileElement->AsEntrance()->GetSequenceIndex())
    {
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
static void viewport_interaction_remove_park_wall(TileElement* tileElement, int32_t x, int32_t y)
{
    rct_scenery_entry* sceneryEntry = tileElement->AsWall()->GetEntry();
    if (sceneryEntry->wall.scrolling_mode != SCROLLING_MODE_NONE)
    {
        context_open_detail_window(WD_SIGN_SMALL, tileElement->AsWall()->GetBannerIndex());
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
static void viewport_interaction_remove_large_scenery(TileElement* tileElement, int32_t x, int32_t y)
{
    rct_scenery_entry* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();

    if (sceneryEntry->large_scenery.scrolling_mode != SCROLLING_MODE_NONE)
    {
        BannerIndex bannerIndex = tileElement->AsLargeScenery()->GetBannerIndex();
        context_open_detail_window(WD_SIGN, bannerIndex);
    }
    else
    {
        auto removeSceneryAction = LargeSceneryRemoveAction(
            x, y, tileElement->base_height, tileElement->GetDirection(), tileElement->AsLargeScenery()->GetSequenceIndex());
        GameActions::Execute(&removeSceneryAction);
    }
}

static Peep* viewport_interaction_get_closest_peep(int32_t x, int32_t y, int32_t maxDistance)
{
    int32_t distance, closestDistance;
    uint16_t spriteIndex;
    rct_window* w;
    rct_viewport* viewport;
    Peep *peep, *closestPeep;

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
    FOR_ALL_PEEPS (spriteIndex, peep)
    {
        if (peep->sprite_left == LOCATION_NULL)
            continue;

        distance = abs(((peep->sprite_left + peep->sprite_right) / 2) - x)
            + abs(((peep->sprite_top + peep->sprite_bottom) / 2) - y);
        if (distance > maxDistance)
            continue;

        if (distance < closestDistance)
        {
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
void sub_68A15E(int32_t screenX, int32_t screenY, int16_t* x, int16_t* y, int32_t* direction, TileElement** tileElement)
{
    int16_t my_x, my_y;
    int32_t interactionType;
    TileElement* myTileElement;
    rct_viewport* viewport;
    get_map_coordinates_from_pos(
        screenX, screenY, VIEWPORT_INTERACTION_MASK_TERRAIN & VIEWPORT_INTERACTION_MASK_WATER, &my_x, &my_y, &interactionType,
        &myTileElement, &viewport);

    if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE)
    {
        *x = LOCATION_NULL;
        return;
    }

    int16_t originalZ = 0;
    if (interactionType == VIEWPORT_INTERACTION_ITEM_WATER)
    {
        originalZ = myTileElement->AsSurface()->GetWaterHeight() << 4;
    }

    LocationXY16 start_vp_pos = screen_coord_to_viewport_coord(viewport, screenX, screenY);
    LocationXY16 map_pos = { (int16_t)(my_x + 16), (int16_t)(my_y + 16) };

    for (int32_t i = 0; i < 5; i++)
    {
        int16_t z = originalZ;
        if (interactionType != VIEWPORT_INTERACTION_ITEM_WATER)
        {
            z = tile_element_height(map_pos.x, map_pos.y);
        }
        map_pos = viewport_coord_to_map_coord(start_vp_pos.x, start_vp_pos.y, z);
        map_pos.x = std::clamp<int16_t>(map_pos.x, my_x, my_x + 31);
        map_pos.y = std::clamp<int16_t>(map_pos.y, my_y, my_y + 31);
    }

    // Determine to which edge the cursor is closest
    int32_t myDirection;
    int32_t mod_x = map_pos.x & 0x1F;
    int32_t mod_y = map_pos.y & 0x1F;
    if (mod_x < mod_y)
    {
        if (mod_x + mod_y < 32)
        {
            myDirection = 0;
        }
        else
        {
            myDirection = 1;
        }
    }
    else
    {
        if (mod_x + mod_y < 32)
        {
            myDirection = 3;
        }
        else
        {
            myDirection = 2;
        }
    }

    *x = map_pos.x & ~0x1F;
    *y = map_pos.y & ~0x1F;
    if (direction != nullptr)
        *direction = myDirection;
    if (tileElement != nullptr)
        *tileElement = myTileElement;
}
