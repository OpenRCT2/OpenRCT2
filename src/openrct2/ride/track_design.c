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

#include "../audio/audio.h"
#include "../cheats.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../localisation/string_ids.h"
#include "../management/finance.h"
#include "../network/network.h"
#include "../object/ObjectManager.h"
#include "../rct1.h"
#include "../rct1/Tables.h"
#include "../util/sawyercoding.h"
#include "../util/util.h"
#include "../windows/error.h"
#include "../world/footpath.h"
#include "../world/scenery.h"
#include "ride.h"
#include "ride_data.h"
#include "track.h"
#include "track_data.h"
#include "track_design.h"
#include "TrackDesignRepository.h"

typedef struct map_backup {
    rct_map_element map_elements[MAX_MAP_ELEMENTS];
    rct_map_element *tile_pointers[MAX_TILE_MAP_ELEMENT_POINTERS];
    rct_map_element *next_free_map_element;
    uint16 map_size_units;
    uint16 map_size_units_minus_2;
    uint16 map_size;
    uint8 current_rotation;
} map_backup;

rct_track_td6 *gActiveTrackDesign;
bool gTrackDesignSceneryToggle;
rct_xyz16 gTrackPreviewMin;
rct_xyz16 gTrackPreviewMax;
rct_xyz16 gTrackPreviewOrigin;

uint8 byte_F4414E;
bool byte_9D8150;
static uint8 _trackDesignPlaceOperation;
static bool _trackDesignDontPlaceScenery;
static money32 _trackDesignPlaceCost;
static sint16 _trackDesignPlaceZ;
static sint16 word_F44129;

static rct_track_td6 *track_design_open_from_buffer(uint8 *src, size_t srcLength);
static map_backup *track_design_preview_backup_map();
static void track_design_preview_restore_map(map_backup *backup);
static void track_design_preview_clear_map();

static void td6_reset_trailing_elements(rct_track_td6 * td6);
static void td6_set_element_helper_pointers(rct_track_td6 * td6);

rct_track_td6 *track_design_open(const utf8 *path)
{
    log_verbose("track_design_open(\"%s\")", path);

    uint8 *buffer;
    size_t bufferLength;
    if (readentirefile(path, (void * *)&buffer, &bufferLength)) {
        if (!sawyercoding_validate_track_checksum(buffer, bufferLength)) {
            log_error("Track checksum failed. %s", path);
            free(buffer);
            return NULL;
        }

        // Decode the track data
        uint8 *decoded = (uint8*)malloc(0x10000);
        size_t decodedLength = sawyercoding_decode_td6(buffer, decoded, bufferLength);
        free(buffer);
        decoded = realloc(decoded, decodedLength);
        if (decoded == NULL) {
            log_error("failed to realloc");
        } else {
            rct_track_td6 *td6 = track_design_open_from_buffer(decoded, decodedLength);
            free(decoded);

            if (td6 != NULL) {
                td6->name = track_repository_get_name_from_path(path);
                return td6;
            }
        }
    }
    return NULL;
}

static rct_track_td6 * track_design_open_from_td4(uint8 *src, size_t srcLength)
{
    rct_track_td4 * td4 = calloc(1, sizeof(rct_track_td4));
    if (td4 == NULL) {
        log_error("Unable to allocate memory for TD4 data.");
        SafeFree(td4);
        return NULL;
    }

    uint8 version = (src[7] >> 2) & 3;
    if (version == 0) {
        memcpy(td4, src, 0x38);
        td4->elementsSize = srcLength - 0x38;
        td4->elements = malloc(td4->elementsSize);
        if (td4->elements == NULL) {
            log_error("Unable to allocate memory for TD4 element data.");
            SafeFree(td4);
            return NULL;
        }
        memcpy(td4->elements, src + 0x38, td4->elementsSize);
    } else if (version == 1) {
        memcpy(td4, src, 0xC4);
        td4->elementsSize = srcLength - 0xC4;
        td4->elements = malloc(td4->elementsSize);
        if (td4->elements == NULL) {
            log_error("Unable to allocate memory for TD4 element data.");
            SafeFree(td4);
            return NULL;
        }
        memcpy(td4->elements, src + 0xC4, td4->elementsSize);
    } else {
        log_error("Unsupported track design.");
        SafeFree(td4);
        return NULL;
    }

    rct_track_td6 * td6 = calloc(1, sizeof(rct_track_td6));
    if (td6 == NULL) {
        log_error("Unable to allocate memory for TD6 data.");
        SafeFree(td4);
        return NULL;
    }

    td6->type = rct1_get_ride_type(td4->type);

    // All TD4s that use powered launch use the type that doesn't pass the station.
    td6->ride_mode = td4->mode;
    if (td4->mode == RCT1_RIDE_MODE_POWERED_LAUNCH) {
        td6->ride_mode = RIDE_MODE_POWERED_LAUNCH;
    }

    // Convert RCT1 vehicle type to RCT2 vehicle type
#pragma warning(push)
#pragma warning(disable : 4295) // 'identifier': array is too small to include a terminating null character
    rct_object_entry vehicleObject = { 0x80, { "        " }, 0 };
#pragma warning(pop)
    if (td4->type == RIDE_TYPE_MAZE) {
        const char * name = rct1_get_ride_type_object(td4->type);
        assert(name != NULL);
        memcpy(vehicleObject.name, name, min(strlen(name), 8));
    } else {
        const char * name = rct1_get_vehicle_object(td4->vehicle_type);
        assert(name != NULL);
        memcpy(vehicleObject.name, name, min(strlen(name), 8));
    }
    memcpy(&td6->vehicle_object, &vehicleObject, sizeof(rct_object_entry));
    td6->vehicle_type = td4->vehicle_type;

    td6->flags = td4->flags;
    td6->version_and_colour_scheme = td4->version_and_colour_scheme;

    // Vehicle colours
    for (sint32 i = 0; i < RCT1_MAX_TRAINS_PER_RIDE; i++) {
        td6->vehicle_colours[i].body_colour = rct1_get_colour(td4->vehicle_colours[i].body_colour);
        td6->vehicle_colours[i].trim_colour = rct1_get_colour(td4->vehicle_colours[i].trim_colour);

        // RCT1 river rapids always had black seats
        if (td4->type == RCT1_RIDE_TYPE_RIVER_RAPIDS) {
            td6->vehicle_colours[i].trim_colour = COLOUR_BLACK;
        }
    }
    // Set remaining vehicles to same colour as first vehicle
    for (sint32 i = RCT1_MAX_TRAINS_PER_RIDE; i < 32; i++) {
        td6->vehicle_colours[i] = td6->vehicle_colours[0];
    }
    // Set additional colour to trim colour for all vehicles
    for (sint32 i = 0; i < 32; i++) {
        td6->vehicle_additional_colour[i] = td6->vehicle_colours[i].trim_colour;
    }

    // Track colours
    if (version == 0) {
        for (sint32 i = 0; i < 4; i++) {
            td6->track_spine_colour[i] = rct1_get_colour(td4->track_spine_colour_v0);
            td6->track_rail_colour[i] = rct1_get_colour(td4->track_rail_colour_v0);
            td6->track_support_colour[i] = rct1_get_colour(td4->track_support_colour_v0);

            // Mazes were only hedges
            switch (td4->type) {
            case RCT1_RIDE_TYPE_HEDGE_MAZE:
                td6->track_support_colour[i] = MAZE_WALL_TYPE_HEDGE;
                break;
            case RCT1_RIDE_TYPE_RIVER_RAPIDS:
                td6->track_spine_colour[i] = COLOUR_WHITE;
                td6->track_rail_colour[i] = COLOUR_WHITE;
                break;
            }
        }
    } else {
        for (sint32 i = 0; i < 4; i++) {
            td6->track_spine_colour[i] = rct1_get_colour(td4->track_spine_colour[i]);
            td6->track_rail_colour[i] = rct1_get_colour(td4->track_rail_colour[i]);
            td6->track_support_colour[i] = rct1_get_colour(td4->track_support_colour[i]);
        }
    }

    td6->depart_flags = td4->depart_flags;
    td6->number_of_trains = td4->number_of_trains;
    td6->number_of_cars_per_train = td4->number_of_cars_per_train;
    td6->min_waiting_time = td4->min_waiting_time;
    td6->max_waiting_time = td4->max_waiting_time;
    td6->operation_setting = min(td4->operation_setting, RideProperties[td6->type].max_value);
    td6->max_speed = td4->max_speed;
    td6->average_speed = td4->average_speed;
    td6->ride_length = td4->ride_length;
    td6->max_positive_vertical_g = td4->max_positive_vertical_g;
    td6->max_negative_vertical_g = td4->max_negative_vertical_g;
    td6->max_lateral_g = td4->max_lateral_g;
    td6->inversions = td4->num_inversions;
    td6->drops = td4->num_drops;
    td6->highest_drop_height = td4->highest_drop_height / 2;
    td6->excitement = td4->excitement;
    td6->intensity = td4->intensity;
    td6->nausea = td4->nausea;
    td6->upkeep_cost = td4->upkeep_cost;
    if (version == 1) {
        td6->flags2 = td4->flags2;
    }

    td6->space_required_x = 255;
    td6->space_required_y = 255;
    td6->lift_hill_speed_num_circuits = 5;

    // Move elements across
    td6->elements = td4->elements;
    td6->elementsSize = td4->elementsSize;

    td6_reset_trailing_elements(td6);
    td6_set_element_helper_pointers(td6);

    SafeFree(td4);
    return td6;
}

static rct_track_td6 *track_design_open_from_buffer(uint8 * src, size_t srcLength)
{
    uint8 version = (src[7] >> 2) & 3;
    if (version == 0 || version == 1) {
        return track_design_open_from_td4(src, srcLength);
    } else if (version != 2) {
        log_error("Unsupported track design.");
        return NULL;
    }

    rct_track_td6 * td6 = calloc(1, sizeof(rct_track_td6));
    if (td6 == NULL) {
        log_error("Unable to allocate memory for TD6 data.");
        return NULL;
    }
    memcpy(td6, src, 0xA3);
    td6->elementsSize = srcLength - 0xA3;
    td6->elements = malloc(td6->elementsSize);
    if (td6->elements == NULL) {
        free(td6);
        log_error("Unable to allocate memory for TD6 element data.");
        return NULL;
    }
    memcpy(td6->elements, src + 0xA3, td6->elementsSize);

    // Cap operation setting
    td6->operation_setting = min(td6->operation_setting, RideProperties[td6->type].max_value);

    td6_set_element_helper_pointers(td6);
    return td6;
}

static void td6_reset_trailing_elements(rct_track_td6 * td6)
{
    void * lastElement;
    if (td6->type == RIDE_TYPE_MAZE) {
        rct_td6_maze_element * mazeElement = (rct_td6_maze_element *)td6->elements;
        while (mazeElement->all != 0) {
            mazeElement++;
        }
        lastElement = (void *)((uintptr_t)mazeElement + 1);
    } else {
        rct_td6_track_element * trackElement = (rct_td6_track_element *)td6->elements;
        while (trackElement->type != 0xFF) {
            trackElement++;
        }
        lastElement = (void *)((uintptr_t)trackElement + 1);
    }
    size_t trailingSize = td6->elementsSize - (size_t)((uintptr_t)lastElement - (uintptr_t)td6->elements);
    memset(lastElement, 0xFF, trailingSize);
}

static void td6_set_element_helper_pointers(rct_track_td6 * td6)
{
    uintptr_t sceneryElementsStart;
    if (td6->type == RIDE_TYPE_MAZE) {
        td6->track_elements = NULL;
        td6->maze_elements = (rct_td6_maze_element*)td6->elements;

        rct_td6_maze_element *maze = td6->maze_elements;
        for (; maze->all != 0; maze++) {}
        sceneryElementsStart = (uintptr_t)(++maze);
    } else {
        td6->maze_elements = NULL;
        td6->track_elements = (rct_td6_track_element*)td6->elements;

        rct_td6_track_element *track = td6->track_elements;
        for (; track->type != 0xFF; track++) {}
        uintptr_t entranceElementsStart = (uintptr_t)track + 1;

        rct_td6_entrance_element *entranceElement = (rct_td6_entrance_element*)entranceElementsStart;
        td6->entrance_elements = entranceElement;
        for (; entranceElement->z != -1; entranceElement++) {}
        sceneryElementsStart = (uintptr_t)entranceElement + 1;
    }

    rct_td6_scenery_element *sceneryElement = (rct_td6_scenery_element*)sceneryElementsStart;
    td6->scenery_elements = sceneryElement;
}

void track_design_dispose(rct_track_td6 *td6)
{
    if (td6 != NULL) {
        free(td6->elements);
        free(td6->name);
        free(td6);
    }
}

/**
 *
 *  rct2: 0x006ABDB0
 */
static void track_design_load_scenery_objects(rct_track_td6 *td6)
{
    object_manager_unload_all_objects();

    // Load ride object
    rct_object_entry * rideEntry = &td6->vehicle_object;
    object_manager_load_object(rideEntry);

    // Load scenery objects
    rct_td6_scenery_element *scenery = td6->scenery_elements;
    for (; (scenery->scenery_object.flags & 0xFF) != 0xFF; scenery++) {
        rct_object_entry * sceneryEntry = &scenery->scenery_object;
        object_manager_load_object(sceneryEntry);
    }
}

/**
 *
 *  rct2: 0x006D247A
 */
static void track_design_mirror_scenery(rct_track_td6 *td6)
{
    rct_td6_scenery_element *scenery = td6->scenery_elements;
    for (; (scenery->scenery_object.flags & 0xFF) != 0xFF; scenery++) {
        uint8 entry_type, entry_index;
        if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index)) {
            entry_type = scenery->scenery_object.flags & 0xF;
            if (entry_type != OBJECT_TYPE_PATHS) {
                continue;
            }
        }

        rct_scenery_entry *scenery_entry = (rct_scenery_entry*)object_entry_groups[entry_type].chunks[entry_index];
        switch (entry_type) {
        case OBJECT_TYPE_LARGE_SCENERY:
        {
            sint16 x1 = 0, x2 = 0, y1 = 0, y2 = 0;
            for (rct_large_scenery_tile* tile = scenery_entry->large_scenery.tiles; tile->x_offset != -1; tile++) {
                if (x1 > tile->x_offset) x1 = tile->x_offset;
                if (x2 < tile->x_offset) x2 = tile->x_offset;
                if (y1 > tile->y_offset) y1 = tile->y_offset;
                if (y2 > tile->y_offset) y2 = tile->y_offset;
            }

            switch (scenery->flags & 3) {
            case 0:
                scenery->y = (-(scenery->y * 32 + y1) - y2) / 32;
                break;
            case 1:
                scenery->x = (scenery->x * 32 + y2 + y1) / 32;
                scenery->y = (-(scenery->y * 32)) / 32;
                scenery->flags ^= (1 << 1);
                break;
            case 2:
                scenery->y = (-(scenery->y * 32 - y2) + y1) / 32;
                break;
            case 3:
                scenery->x = (scenery->x * 32 - y2 - y1) / 32;
                scenery->y = (-(scenery->y * 32)) / 32;
                scenery->flags ^= (1 << 1);
                break;
            }
            break;
        }
        case OBJECT_TYPE_SMALL_SCENERY:
            scenery->y = -scenery->y;

            if (scenery_entry->small_scenery.flags & SMALL_SCENERY_FLAG9) {
                scenery->flags ^= (1 << 0);
                if (!(scenery_entry->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE)) {
                    scenery->flags ^= (1 << 2);
                }
                break;
            }
            if (scenery->flags & (1 << 0)) {
                scenery->flags ^= (1 << 1);
            }

            scenery->flags ^= (1 << 2);
            break;

        case OBJECT_TYPE_WALLS:
            scenery->y = -scenery->y;
            if (scenery->flags & (1 << 0)) {
                scenery->flags ^= (1 << 1);
            }
            break;

        case OBJECT_TYPE_PATHS:
            scenery->y = -scenery->y;

            if (scenery->flags & (1 << 5)) {
                scenery->flags ^= (1 << 6);
            }

            uint8 flags = scenery->flags;
            flags = ((flags & (1 << 3)) >> 2) | ((flags & (1 << 1)) << 2);
            scenery->flags &= 0xF5;
            scenery->flags |= flags;
        }
    }
}

/**
 *
 *  rct2: 0x006D2443
 */
static void track_design_mirror_ride(rct_track_td6 *td6)
{
    rct_td6_track_element *track = td6->track_elements;
    for (; track->type != 0xFF; track++) {
        track->type = TrackElementMirrorMap[track->type];
    }

    rct_td6_entrance_element *entrance = td6->entrance_elements;
    for (; entrance->z != -1; entrance++) {
        entrance->y = -entrance->y;
        if (entrance->direction & 1) {
            entrance->direction ^= (1 << 1);
        }
    }
}

/** rct2: 0x00993EDC */
static const uint8 maze_segment_mirror_map[] = {
    5, 4, 2, 7, 1, 0, 14, 3, 13, 12, 10, 15, 9, 8, 6, 11
};

/**
 *
 *  rct2: 0x006D25FA
 */
static void track_design_mirror_maze(rct_track_td6 *td6)
{
    rct_td6_maze_element *maze = td6->maze_elements;
    for (; maze->all != 0; maze++) {
        maze->y = -maze->y;

        if (maze->type == 0x8 || maze->type == 0x80) {
            if (maze->unk_2 & 1) {
                maze->unk_2 ^= (1 << 1);
            }
            continue;
        }

        uint16 maze_entry = maze->maze_entry;
        uint16 new_entry = 0;
        for (uint8 position = bitscanforward(maze_entry); position != 0xFF; position = bitscanforward(maze_entry)) {
            maze_entry &= ~(1 << position);
            new_entry |= (1 << maze_segment_mirror_map[position]);
        }
        maze->maze_entry = new_entry;
    }
}

/**
 *
 *  rct2: 0x006D2436
 */
void track_design_mirror(rct_track_td6 *td6)
{
    if (td6->type == RIDE_TYPE_MAZE) {
        track_design_mirror_maze(td6);
    } else {
        track_design_mirror_ride(td6);
    }
    track_design_mirror_scenery(td6);
}

static void track_design_add_selection_tile(sint16 x, sint16 y)
{
    rct_xy16 *selectionTile = gMapSelectionTiles;
    for (; selectionTile->x != -1; selectionTile++) {
        if (selectionTile->x == x && selectionTile->y == y) {
            return;
        }
        if (selectionTile + 1 >= &gMapSelectionTiles[300]) {
            return;
        }
    }
    selectionTile->x = x;
    selectionTile->y = y;
    selectionTile++;
    selectionTile->x = -1;
}

static void track_design_update_max_min_coordinates(sint16 x, sint16 y, sint16 z)
{
    gTrackPreviewMin.x = min(gTrackPreviewMin.x, x);
    gTrackPreviewMax.x = max(gTrackPreviewMax.x, x);
    gTrackPreviewMin.y = min(gTrackPreviewMin.y, y);
    gTrackPreviewMax.y = max(gTrackPreviewMax.y, y);
    gTrackPreviewMin.z = min(gTrackPreviewMin.z, z);
    gTrackPreviewMax.z = max(gTrackPreviewMax.z, z);
}

/**
 *
 *  rct2: 0x006D0964
 */
static sint32 track_design_place_scenery(rct_td6_scenery_element *scenery_start, uint8 rideIndex, sint32 originX, sint32 originY, sint32 originZ)
{
    for (uint8 mode = 0; mode <= 1; mode++) {
        if ((scenery_start->scenery_object.flags & 0xFF) != 0xFF) {
            byte_F4414E |= BYTE_F4414E_HAS_SCENERY;
        }

        if (byte_F4414E & BYTE_F4414E_DONT_PLACE_SCENERY) {
            continue;
        }

        for (rct_td6_scenery_element *scenery = scenery_start; (scenery->scenery_object.flags & 0xFF) != 0xFF; scenery++) {
            uint8 rotation = _currentTrackPieceDirection;
            rct_xy8 tile = { .x = originX / 32, .y = originY / 32 };
            switch (rotation & 3){
            case MAP_ELEMENT_DIRECTION_WEST:
                tile.x += scenery->x;
                tile.y += scenery->y;
                break;
            case MAP_ELEMENT_DIRECTION_NORTH:
                tile.x += scenery->y;
                tile.y -= scenery->x;
                break;
            case MAP_ELEMENT_DIRECTION_EAST:
                tile.x -= scenery->x;
                tile.y -= scenery->y;
                break;
            case MAP_ELEMENT_DIRECTION_SOUTH:
                tile.x -= scenery->y;
                tile.y += scenery->x;
                break;
            }

            rct_xy16 mapCoord = { .x = tile.x * 32, .y = tile.y * 32 };
            track_design_update_max_min_coordinates(mapCoord.x, mapCoord.y, originZ);

            if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES && mode == 0) {
                uint8 new_tile = 1;
                rct_xy16* selectionTile = gMapSelectionTiles;
                for (; selectionTile->x != -1; selectionTile++){
                    if (selectionTile->x == tile.x && selectionTile->y == tile.y){
                        new_tile = 0;
                        break;
                    }
                    if (selectionTile + 1 >= &gMapSelectionTiles[300]){
                        new_tile = 0;
                        break;
                    }
                }
                if (new_tile){
                    selectionTile->x = tile.x;
                    selectionTile->y = tile.y;
                    selectionTile++;
                    selectionTile->x = -1;
                }
            }

            if (_trackDesignPlaceOperation == PTD_OPERATION_CLEAR_OUTLINES && mode == 0) {
                uint8 entry_type, entry_index;
                if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index)) {
                    entry_type = scenery->scenery_object.flags & 0xF;
                    if (entry_type != OBJECT_TYPE_PATHS) {
                        entry_type = 0xFF;
                    }
                    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) {
                        entry_type = 0xFF;
                    }

                    entry_index = 0;
                    for (rct_footpath_entry *path = get_footpath_entry(0);
                        entry_index < object_entry_group_counts[OBJECT_TYPE_PATHS];
                        path = get_footpath_entry(entry_index), entry_index++
                    ) {
                        if (path == (rct_footpath_entry*)-1) {
                            continue;
                        }
                        if (path->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) {
                            continue;
                        }
                    }

                    if (entry_index == object_entry_group_counts[OBJECT_TYPE_PATHS]) {
                        entry_type = 0xFF;
                    }
                }
                sint32 z;
                switch (entry_type) {
                case OBJECT_TYPE_SMALL_SCENERY:
                    //bl
                    rotation += scenery->flags;
                    rotation &= 3;

                    //bh
                    uint8 quadrant = (scenery->flags >> 2) + _currentTrackPieceDirection;
                    quadrant &= 3;

                    uint8 bh = rotation | (quadrant << 6) | MAP_ELEMENT_TYPE_SCENERY;

                    rct_scenery_entry* small_scenery = get_small_scenery_entry(entry_index);
                    if (!(!(small_scenery->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE) &&
                        (small_scenery->small_scenery.flags & SMALL_SCENERY_FLAG9)) &&
                        (small_scenery->small_scenery.flags & (SMALL_SCENERY_FLAG9 | SMALL_SCENERY_FLAG24 | SMALL_SCENERY_FLAG25)))
                    {
                        bh &= 0x3F;
                    }

                    z = (scenery->z * 8 + originZ) / 8;
                    game_do_command(
                        mapCoord.x,
                        0x69 | bh << 8,
                        mapCoord.y,
                        (entry_index << 8) | z,
                        GAME_COMMAND_REMOVE_SCENERY,
                        0,
                        0);
                    break;
                case OBJECT_TYPE_LARGE_SCENERY:
                    z = (scenery->z * 8 + originZ) / 8;
                    game_do_command(
                        mapCoord.x,
                        0x69 | (((rotation + scenery->flags) & 0x3) << 8),
                        mapCoord.y,
                        z,
                        GAME_COMMAND_REMOVE_LARGE_SCENERY,
                        0,
                        0);
                    break;
                case OBJECT_TYPE_WALLS:
                    z = (scenery->z * 8 + originZ) / 8;
                    game_do_command(
                        mapCoord.x,
                        0x69,
                        mapCoord.y,
                        (z << 8) | ((rotation + scenery->flags) & 0x3),
                        GAME_COMMAND_REMOVE_WALL,
                        0,
                        0);
                    break;
                case OBJECT_TYPE_PATHS:
                    z = (scenery->z * 8 + originZ) / 8;
                    footpath_remove(mapCoord.x, mapCoord.y, z, 0x69);
                    break;
                }
            }

            if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z) {
                sint32 z = scenery->z * 8 + _trackDesignPlaceZ;
                if (z < word_F44129) {
                    word_F44129 = z;
                }
            }

            if (_trackDesignPlaceOperation == PTD_OPERATION_1 ||
                _trackDesignPlaceOperation == PTD_OPERATION_2 ||
                _trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z ||
                _trackDesignPlaceOperation == PTD_OPERATION_4 ||
                _trackDesignPlaceOperation == PTD_OPERATION_GET_COST
            ) {

                uint8 entry_type, entry_index;
                if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index)){
                    entry_type = scenery->scenery_object.flags & 0xF;
                    if (entry_type != OBJECT_TYPE_PATHS){
                        byte_F4414E |= BYTE_F4414E_SCENERY_UNAVAILABLE;
                        continue;
                    }

                    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER){
                        byte_F4414E |= BYTE_F4414E_SCENERY_UNAVAILABLE;
                        continue;
                    }

                    entry_index = 0;
                    for (rct_footpath_entry* path = get_footpath_entry(0);
                        entry_index < object_entry_group_counts[OBJECT_TYPE_PATHS];
                        path = get_footpath_entry(entry_index), entry_index++){

                        if (path == (rct_footpath_entry*)-1)
                            continue;
                        if (path->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR)
                            continue;
                    }

                    if (entry_index == object_entry_group_counts[OBJECT_TYPE_PATHS]){
                        byte_F4414E |= BYTE_F4414E_SCENERY_UNAVAILABLE;
                        continue;
                    }
                }

                money32 cost;
                sint16 z;
                uint8 bl;

                switch (entry_type){
                case OBJECT_TYPE_SMALL_SCENERY:
                    if (mode != 0) continue;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z) continue;

                    rotation += scenery->flags;
                    rotation &= 3;
                    z = scenery->z * 8 + originZ;
                    uint8 quadrant = ((scenery->flags >> 2) + _currentTrackPieceDirection) & 3;

                    bl = 0x81;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST) bl = 0xA9;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_4) bl = 0xE9;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_1) bl = 0x80;

                    gGameCommandErrorTitle = STR_CANT_POSITION_THIS_HERE;

                    cost = game_do_command(
                        mapCoord.x,
                        bl | (entry_index << 8),
                        mapCoord.y,
                        quadrant | (scenery->primary_colour << 8),
                        GAME_COMMAND_PLACE_SCENERY,
                        rotation | (scenery->secondary_colour << 16),
                        z
                    );

                    if (cost == MONEY32_UNDEFINED) {
                        cost = 0;
                    }
                    break;
                case OBJECT_TYPE_LARGE_SCENERY:
                    if (mode != 0) continue;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z) continue;

                    rotation += scenery->flags;
                    rotation &= 3;

                    z = scenery->z * 8 + originZ;

                    bl = 0x81;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST) bl = 0xA9;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_4) bl = 0xE9;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_1) bl = 0x80;

                    cost = game_do_command(
                        mapCoord.x,
                        bl | (rotation << 8),
                        mapCoord.y,
                        scenery->primary_colour | (scenery->secondary_colour << 8),
                        GAME_COMMAND_PLACE_LARGE_SCENERY,
                        entry_index,
                        z
                        );

                    if (cost == MONEY32_UNDEFINED)
                        cost = 0;
                    break;
                case OBJECT_TYPE_WALLS:
                    if (mode != 0) continue;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z) continue;

                    z = scenery->z * 8 + originZ;
                    rotation += scenery->flags;
                    rotation &= 3;

                    bl = 1;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST) bl = 0xA9;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_4) bl = 105;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_1) bl = 0;

                    gGameCommandErrorTitle = STR_CANT_BUILD_PARK_ENTRANCE_HERE;

                    cost = game_do_command(
                        mapCoord.x,
                        bl | (entry_index << 8),
                        mapCoord.y,
                        rotation | (scenery->primary_colour << 8),
                        GAME_COMMAND_PLACE_WALL,
                        z,
                        scenery->secondary_colour | ((scenery->flags & 0xFC) << 6)
                        );

                    if (cost == MONEY32_UNDEFINED)
                        cost = 0;
                    break;
                case OBJECT_TYPE_PATHS:
                    if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z) continue;

                    z = (scenery->z * 8 + originZ) / 8;
                    if (mode == 0) {
                        if (scenery->flags & (1 << 7)){
                            //dh
                            entry_index |= (1 << 7);
                        }

                        uint8 bh = ((scenery->flags & 0xF) << rotation);
                        bl = bh >> 4;
                        bh = (bh | bl) & 0xF;
                        bl = (((scenery->flags >> 5) + rotation) & 3) << 5;
                        bh |= bl;

                        bh |= scenery->flags & 0x90;

                        bl = 1;
                        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST) bl = 41;
                        if (_trackDesignPlaceOperation == PTD_OPERATION_4) bl = 105;
                        if (_trackDesignPlaceOperation == PTD_OPERATION_1) bl = 0;

                        gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                        cost = game_do_command(mapCoord.x, bl | (bh << 8), mapCoord.y, z | (entry_index << 8), GAME_COMMAND_PLACE_PATH_FROM_TRACK, 0, 0);
                    }
                    else{
                        if (_trackDesignPlaceOperation == PTD_OPERATION_1)
                            continue;

                        rct_map_element* map_element = map_get_path_element_at(mapCoord.x / 32, mapCoord.y / 32, z);

                        if (map_element == NULL)
                            continue;

                        footpath_queue_chain_reset();
                        footpath_remove_edges_at(mapCoord.x, mapCoord.y, map_element);

                        bl = 1;
                        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST) bl = 41;
                        if (_trackDesignPlaceOperation == PTD_OPERATION_4) bl = 105;

                        footpath_connect_edges(mapCoord.x, mapCoord.y, map_element, bl);
                        footpath_update_queue_chains();
                        continue;
                    }
                    break;
                default:
                    byte_F4414E |= BYTE_F4414E_SCENERY_UNAVAILABLE;
                    continue;
                    break;
                }
                _trackDesignPlaceCost += cost;
                if (_trackDesignPlaceOperation != PTD_OPERATION_2) {
                    if (cost == MONEY32_UNDEFINED){
                        _trackDesignPlaceCost = MONEY32_UNDEFINED;
                    }
                }
                if (_trackDesignPlaceCost != MONEY32_UNDEFINED) {
                    continue;
                }
                if (_trackDesignPlaceOperation == PTD_OPERATION_2) {
                    continue;
                }
                return 0;
            }
        }
    }
    return 1;
}

static sint32 track_design_place_maze(rct_track_td6 *td6, sint16 x, sint16 y, sint16 z, uint8 rideIndex)
{
    if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES) {
        gMapSelectionTiles->x = -1;
        gMapSelectArrowPosition.x = x;
        gMapSelectArrowPosition.y = y;
        gMapSelectArrowPosition.z = map_element_height(x, y) & 0xFFFF;
        gMapSelectArrowDirection = _currentTrackPieceDirection;
    }

    _trackDesignPlaceZ = 0;
    _trackDesignPlaceCost = 0;

    rct_td6_maze_element *maze = td6->maze_elements;
    for (; maze->all != 0; maze++){
        uint8 rotation = _currentTrackPieceDirection & 3;
        rct_xy16 mapCoord = { .x = maze->x * 32, .y = maze->y * 32 };
        rotate_map_coordinates(&mapCoord.x, &mapCoord.y, rotation);
        mapCoord.x += x;
        mapCoord.y += y;

        track_design_update_max_min_coordinates(mapCoord.x, mapCoord.y, z);

        if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES) {
            track_design_add_selection_tile(mapCoord.x, mapCoord.y);
        }

        if (_trackDesignPlaceOperation == PTD_OPERATION_1 ||
            _trackDesignPlaceOperation == PTD_OPERATION_2 ||
            _trackDesignPlaceOperation == PTD_OPERATION_4 ||
            _trackDesignPlaceOperation == PTD_OPERATION_GET_COST
        ) {
            uint8 bl;
            money32 cost = 0;
            uint16 maze_entry;
            switch (maze->type) {
            case 0x08:
                // entrance
                rotation += maze->unk_2;
                rotation &= 3;

                gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

                bl = 1;
                if (_trackDesignPlaceOperation == PTD_OPERATION_4) bl = 0x69;
                if (_trackDesignPlaceOperation == PTD_OPERATION_1) {
                    cost = game_do_command(mapCoord.x, 0 | rotation << 8, mapCoord.y, (z / 16) & 0xFF, GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, -1, 0);
                } else {
                    cost = game_do_command(mapCoord.x, bl | rotation << 8, mapCoord.y, rideIndex, GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, 0, 0);
                }
                if (cost != MONEY32_UNDEFINED){
                    byte_F4414E |= BYTE_F4414E_ENTRANCE_EXIT_PLACED;
                }
                break;
            case 0x80:
                // exit
                rotation += maze->unk_2;
                rotation &= 3;

                gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

                bl = 1;
                if (_trackDesignPlaceOperation == PTD_OPERATION_4) bl = 0x69;
                if (_trackDesignPlaceOperation == PTD_OPERATION_1) {
                    cost = game_do_command(mapCoord.x, 0 | rotation << 8, mapCoord.y, ((z / 16) & 0xFF) | (1 << 8), GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, -1, 0);
                }
                else{
                    cost = game_do_command(mapCoord.x, bl | rotation << 8, mapCoord.y, rideIndex | (1 << 8), GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, 0, 0);
                }
                if (cost != MONEY32_UNDEFINED){
                    byte_F4414E |= BYTE_F4414E_ENTRANCE_EXIT_PLACED;
                }
                break;
            default:
                maze_entry = rol16(maze->maze_entry, rotation * 4);

                bl = 1;
                if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST) bl = 0x29;
                if (_trackDesignPlaceOperation == PTD_OPERATION_4) bl = 0x69;
                if (_trackDesignPlaceOperation == PTD_OPERATION_1) bl = 0;

                gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

                cost = game_do_command(mapCoord.x, bl | (maze_entry & 0xFF) << 8, mapCoord.y, rideIndex | (maze_entry & 0xFF00), GAME_COMMAND_PLACE_MAZE_DESIGN, z, 0);
                break;
            }

            _trackDesignPlaceCost += cost;

            if (cost == MONEY32_UNDEFINED){
                _trackDesignPlaceCost = cost;
                return 0;
            }
        }

        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z) {
            if (mapCoord.x < 0) continue;
            if (mapCoord.y < 0) continue;
            if (mapCoord.x >= 256 * 32) continue;
            if (mapCoord.y >= 256 * 32) continue;

            rct_map_element *map_element = map_get_surface_element_at(mapCoord.x / 32, mapCoord.y / 32);
            sint16 map_height = map_element->base_height * 8;
            if (map_element->properties.surface.slope & 0xF) {
                map_height += 16;
                if (map_element->properties.surface.slope & 0x10) {
                    map_height += 16;
                }
            }

            if (map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK) {
                sint16 water_height = map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
                water_height *= 16;
                if (water_height > map_height) {
                    map_height = water_height;
                }
            }

            sint16 temp_z = z + _trackDesignPlaceZ - map_height;
            if (temp_z < 0) {
                _trackDesignPlaceZ -= temp_z;
            }
        }
    }

    if (_trackDesignPlaceOperation == PTD_OPERATION_CLEAR_OUTLINES) {
        game_do_command(0, 0x69, 0, rideIndex, GAME_COMMAND_DEMOLISH_RIDE, 0, 0);
    }

    gTrackPreviewOrigin = (rct_xyz16) { x, y, z };
    return 1;
}

static bool track_design_place_ride(rct_track_td6 *td6, sint16 x, sint16 y, sint16 z, uint8 rideIndex)
{
    gTrackPreviewOrigin = (rct_xyz16) { x, y, z };
    if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES) {
        gMapSelectionTiles->x = -1;
        gMapSelectArrowPosition.x = x;
        gMapSelectArrowPosition.y = y;
        gMapSelectArrowPosition.z = map_element_height(x, y) & 0xFFFF;
        gMapSelectArrowDirection = _currentTrackPieceDirection;
    }

    _trackDesignPlaceZ = 0;
    _trackDesignPlaceCost = 0;
    uint8 rotation = _currentTrackPieceDirection;

    // Track elements
    rct_td6_track_element *track = td6->track_elements;
    for (; track->type != 0xFF; track++) {
        uint8 trackType = track->type;
        if (trackType == TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP) {
            trackType = 0xFF;
        }

        track_design_update_max_min_coordinates(x, y, z);

        switch (_trackDesignPlaceOperation) {
        case PTD_OPERATION_DRAW_OUTLINES:
            for (const rct_preview_track* trackBlock = TrackBlocks[trackType]; trackBlock->index != 0xFF; trackBlock++) {
                rct_xy16 tile = { x, y };
                map_offset_with_rotation(&tile.x, &tile.y, trackBlock->x, trackBlock->y, rotation);
                track_design_update_max_min_coordinates(tile.x, tile.y, z);
                track_design_add_selection_tile(tile.x, tile.y);
            }
            break;
        case PTD_OPERATION_CLEAR_OUTLINES:
        {
            const rct_track_coordinates *trackCoordinates = &TrackCoordinates[trackType];
            const rct_preview_track *trackBlock = TrackBlocks[trackType];
            sint32 tempZ = z - trackCoordinates->z_begin + trackBlock->z;
            uint8 flags =
                GAME_COMMAND_FLAG_APPLY |
                GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED |
                GAME_COMMAND_FLAG_5 |
                GAME_COMMAND_FLAG_GHOST;
            ride_remove_track_piece(x, y, tempZ, rotation & 3, trackType, flags);
            break;
        }
        case PTD_OPERATION_1:
        case PTD_OPERATION_2:
        case PTD_OPERATION_4:
        case PTD_OPERATION_GET_COST:
        {
            const rct_track_coordinates *trackCoordinates = &TrackCoordinates[trackType];

            //di
            sint16 tempZ = z - trackCoordinates->z_begin;
            uint32 edi =
                ((track->flags & 0x0F) << 17) |
                ((track->flags & 0x0F) << 28) |
                (((track->flags >> 4) & 0x03) << 24) |
                (tempZ & 0xFFFF);

            sint32 edx = _currentRideIndex | (trackType << 8);
            if (track->flags & 0x80) edx |= 0x10000;
            if (track->flags & 0x40) edx |= 0x20000;

            uint8 flags = GAME_COMMAND_FLAG_APPLY;
            if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST) {
                flags |= GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED;
                flags |= GAME_COMMAND_FLAG_5;
            }
            else if (_trackDesignPlaceOperation == PTD_OPERATION_4) {
                flags |= GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED;
                flags |= GAME_COMMAND_FLAG_5;
                flags |= GAME_COMMAND_FLAG_GHOST;
            }
            else if (_trackDesignPlaceOperation == PTD_OPERATION_1) {
                flags = 0;
            }

            gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
            money32 cost = game_do_command(x, flags | (rotation << 8), y, edx, GAME_COMMAND_PLACE_TRACK, edi, 0);
            _trackDesignPlaceCost += cost;
            if (cost == MONEY32_UNDEFINED) {
                _trackDesignPlaceCost = cost;
                return false;
            }
            break;
        }
        case PTD_OPERATION_GET_PLACE_Z:
        {
            sint32 tempZ = z - TrackCoordinates[trackType].z_begin;
            for (const rct_preview_track *trackBlock = TrackBlocks[trackType]; trackBlock->index != 0xFF; trackBlock++) {
                rct_xy16 tile = { x, y };
                map_offset_with_rotation(&tile.x, &tile.y, trackBlock->x, trackBlock->y, rotation);
                if (tile.x < 0 || tile.y < 0 || tile.x >= (256 * 32) || tile.y >= (256 * 32)) {
                    continue;
                }

                rct_map_element *mapElement = map_get_surface_element_at(tile.x >> 5, tile.y >> 5);
                if (mapElement == NULL) {
                    return false;
                }

                sint32 height = mapElement->base_height * 8;
                if (mapElement->properties.surface.slope & 0x0F) {
                    height += 16;
                    if (mapElement->properties.surface.slope & 0x10) {
                        height += 16;
                    }
                }

                uint8 water_height = 16 * mapElement->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
                if (water_height > 0 && water_height > height) {
                    height = water_height;
                }
                sint32 heightDifference = tempZ + _trackDesignPlaceZ + trackBlock->z - height;
                if (heightDifference < 0) {
                    _trackDesignPlaceZ -= heightDifference;
                }
            }
            break;
        }
        }

        const rct_track_coordinates *track_coordinates = &TrackCoordinates[trackType];
        map_offset_with_rotation(&x, &y, track_coordinates->x, track_coordinates->y, rotation);
        z -= track_coordinates->z_begin;
        z += track_coordinates->z_end;

        rotation = (rotation + track_coordinates->rotation_end - track_coordinates->rotation_begin) & 3;
        if (track_coordinates->rotation_end & (1 << 2)) {
            rotation |= (1 << 2);
        } else {
            x += TileDirectionDelta[rotation].x;
            y += TileDirectionDelta[rotation].y;
        }
    }

    // Entrance elements
    rct_td6_entrance_element *entrance = td6->entrance_elements;
    for (; entrance->z != -1; entrance++) {
        rotation = _currentTrackPieceDirection & 3;
        x = entrance->x;
        y = entrance->y;
        rotate_map_coordinates(&x, &y, rotation);
        x += gTrackPreviewOrigin.x;
        y += gTrackPreviewOrigin.y;

        track_design_update_max_min_coordinates(x, y, z);

        switch (_trackDesignPlaceOperation) {
        case PTD_OPERATION_DRAW_OUTLINES:
            track_design_add_selection_tile(x, y);
            break;
        case PTD_OPERATION_1:
        case PTD_OPERATION_2:
        case PTD_OPERATION_4:
        case PTD_OPERATION_GET_COST:
        {
            rotation = (rotation + entrance->direction) & 3;
            uint8 isExit = 0;
            if (entrance->direction & (1 << 7)) {
                isExit = 1;
            }

            if (_trackDesignPlaceOperation != PTD_OPERATION_1) {
                rct_xy16 tile = {
                    x + TileDirectionDelta[rotation].x,
                    y + TileDirectionDelta[rotation].y
                };
                rct_map_element* map_element = map_get_first_element_at(tile.x >> 5, tile.y >> 5);
                z = gTrackPreviewOrigin.z / 8;
                z += (entrance->z == (sint8)(uint8)0x80) ? -1 : entrance->z;

                do {
                    if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_TRACK) continue;
                    if (map_element->base_height != z) continue;

                    sint32 di = (map_element->properties.track.sequence >> 4) & 0x07;
                    uint8 bl = 1;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST) bl = 41;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_4) bl = 105;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_1) bl = 0;

                    gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                    money32 cost = game_do_command(x, bl | (rotation << 8), y, rideIndex | (isExit << 8), GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, di, 0);
                    _trackDesignPlaceCost += cost;

                    if (cost == MONEY32_UNDEFINED) {
                        _trackDesignPlaceCost = cost;
                        return 0;
                    }
                    byte_F4414E |= BYTE_F4414E_ENTRANCE_EXIT_PLACED;
                    break;
                } while (!map_element_is_last_for_tile(map_element++));
            } else {
                z = (entrance->z == (sint8)(uint8)0x80) ? -1 : entrance->z;
                z *= 8;
                z += gTrackPreviewOrigin.z;
                z >>= 4;

                gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                money32 cost = game_do_command(x, 0 | (rotation << 8), y, z | (isExit << 8), GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, -1, 0);
                if (cost == MONEY32_UNDEFINED) {
                    _trackDesignPlaceCost = cost;
                    return 0;
                } else {
                    _trackDesignPlaceCost += cost;
                    byte_F4414E |= BYTE_F4414E_ENTRANCE_EXIT_PLACED;
                }
            }
            break;
        }
        }
    }

    if (_trackDesignPlaceOperation == PTD_OPERATION_CLEAR_OUTLINES) {
        sub_6CB945(_currentRideIndex);
        ride_delete(_currentRideIndex);
    }
    return true;
}

/**
* Places a virtual track. This can involve highlighting the surface tiles and showing the track layout. It is also used by
* the track preview window to place the whole track.
* Depending on the value of bl it modifies the function.
* bl == 0, Draw outlines on the ground
* bl == 1,
* bl == 2,
* bl == 3, Returns the z value of a successful placement. Only lower 16 bits are the value, the rest may be garbage?
* bl == 4,
* bl == 5, Returns cost to create the track. All 32 bits are used. Places the track. (used by the preview)
* bl == 6, Clear white outlined track.
*  rct2: 0x006D01B3
*/
sint32 place_virtual_track(rct_track_td6 *td6, uint8 bl, uint8 rideIndex, sint32 x, sint32 y, sint32 z)
{
    byte_F4414E = (bl & PTD_OPERATION_DONT_PLACE_SCENERY) ? BYTE_F4414E_DONT_PLACE_SCENERY : 0;
    _trackDesignPlaceOperation = bl & 0x7F;
    if (gTrackDesignSceneryToggle) {
        byte_F4414E |= BYTE_F4414E_DONT_PLACE_SCENERY;
    }
    _currentRideIndex = rideIndex;

    gTrackPreviewMin = (rct_xyz16){ x, y, z };
    gTrackPreviewMax = (rct_xyz16){ x, y, z };

    word_F44129 = 0;
    uint8 track_place_success = 0;
    if (td6->type == RIDE_TYPE_MAZE) {
        track_place_success = track_design_place_maze(td6, x, y, z, rideIndex);
    } else {
        track_place_success = track_design_place_ride(td6, x, y, z, rideIndex);
    }

    // Scenery elements
    rct_td6_scenery_element *scenery = td6->scenery_elements;
    if (track_place_success && scenery != NULL) {
        if (!track_design_place_scenery(
            scenery,
            rideIndex,
            gTrackPreviewOrigin.x,
            gTrackPreviewOrigin.y,
            gTrackPreviewOrigin.z
        )) {
            return _trackDesignPlaceCost;
        }
    }

    // 0x6D0FE6
    if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES) {
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;
        map_invalidate_map_selection_tiles();
    }

    if (bl == 3) {
        return _trackDesignPlaceZ;
    }
    return _trackDesignPlaceCost;
}

/**
 *
 *  rct2: 0x006D2189
 * ebx = ride_id
 * cost = edi
 */
static bool track_design_place_preview(rct_track_td6 *td6, money32 *cost, uint8 *rideId, uint8 *flags)
{
    *flags = 0;

    uint8 entry_type, entry_index;
    if (!find_object_in_entry_group(&td6->vehicle_object, &entry_type, &entry_index)) {
        entry_index = 0xFF;
    }

    uint8 rideIndex;
    uint8 colour;
    uint8 rideCreateFlags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
    if (ride_create_command(td6->type, entry_index, rideCreateFlags, &rideIndex, &colour) == MONEY32_UNDEFINED) {
        return false;
    }

    rct_ride *ride = get_ride(rideIndex);
    rct_string_id new_ride_name = user_string_allocate(USER_STRING_HIGH_ID_NUMBER | USER_STRING_DUPLICATION_PERMITTED, "");
    if (new_ride_name != 0) {
        rct_string_id old_name = ride->name;
        ride->name = new_ride_name;
        user_string_free(old_name);
    }

    ride->entrance_style = td6->entrance_style;

    for (sint32 i = 0; i < 4; i++) {
        ride->track_colour_main[i] = td6->track_spine_colour[i];
        ride->track_colour_additional[i] = td6->track_rail_colour[i];
        ride->track_colour_supports[i] = td6->track_support_colour[i];
    }

    byte_9D8150 = true;
    uint8 backup_rotation = _currentTrackPieceDirection;
    uint32 backup_park_flags = gParkFlags;
    gParkFlags &= ~PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
    sint32 mapSize = gMapSize << 4;

    _currentTrackPieceDirection = 0;
    sint32 z = place_virtual_track(td6, PTD_OPERATION_GET_PLACE_Z, 0, mapSize, mapSize, 16);

    if (byte_F4414E & BYTE_F4414E_HAS_SCENERY) {
        *flags |= TRACK_DESIGN_FLAG_HAS_SCENERY;
    }

    z += 16 - word_F44129;

    sint32 operation = PTD_OPERATION_GET_COST;
    if (byte_F4414E & BYTE_F4414E_SCENERY_UNAVAILABLE) {
        operation |= PTD_OPERATION_DONT_PLACE_SCENERY;
        *flags |= TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE;
    }

    money32 resultCost = place_virtual_track(td6, operation, rideIndex, mapSize, mapSize, z);
    gParkFlags = backup_park_flags;

    if (resultCost != MONEY32_UNDEFINED) {
        if (!find_object_in_entry_group(&td6->vehicle_object, &entry_type, &entry_index)){
            *flags |= TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE;
        }

        _currentTrackPieceDirection = backup_rotation;
        byte_9D8150 = false;
        *cost = resultCost;
        *rideId = rideIndex;
        return true;
    } else {
        _currentTrackPieceDirection = backup_rotation;
        user_string_free(ride->name);
        ride->type = RIDE_TYPE_NULL;
        byte_9D8150 = false;
        return false;
    }
}

static money32 place_track_design(sint16 x, sint16 y, sint16 z, uint8 flags, uint8 *outRideIndex)
{
    *outRideIndex = 255;

    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = z;

    if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED)) {
        if (game_is_paused() && !gCheatsBuildInPauseMode) {
            gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
            return MONEY32_UNDEFINED;
        }
    }

    rct_track_td6 *td6 = gActiveTrackDesign;
    if (td6 == NULL) {
        return MONEY32_UNDEFINED;
    }
    rct_object_entry *rideEntryObject = &td6->vehicle_object;

    uint8 entryType, entryIndex;
    if (!find_object_in_entry_group(rideEntryObject, &entryType, &entryIndex)) {
        entryIndex = 0xFF;
    }

    uint8 rideIndex;
    uint8 rideColour;
    money32 createRideResult = ride_create_command(td6->type, entryIndex, GAME_COMMAND_FLAG_APPLY, &rideIndex, &rideColour);
    if (createRideResult == MONEY32_UNDEFINED) {
        gGameCommandErrorTitle = STR_CANT_CREATE_NEW_RIDE_ATTRACTION;
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        return MONEY32_UNDEFINED;
    }

    rct_ride* ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_NULL) {
        log_warning("Invalid game command for track placement, ride id = %d", rideIndex);
        return MONEY32_UNDEFINED;
    }

    money32 cost = 0;
    if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
        _trackDesignDontPlaceScenery = false;
        cost = place_virtual_track(td6, PTD_OPERATION_1, rideIndex, x, y, z);
        if (byte_F4414E & BYTE_F4414E_SCENERY_UNAVAILABLE) {
            _trackDesignDontPlaceScenery = true;
            cost = place_virtual_track(td6, PTD_OPERATION_DONT_PLACE_SCENERY | PTD_OPERATION_1, rideIndex, x, y, z);
        }
    } else {
        uint8 operation;
        if (flags & GAME_COMMAND_FLAG_GHOST) {
            operation = PTD_OPERATION_4;
        } else {
            operation = PTD_OPERATION_2;
        }

        if (_trackDesignDontPlaceScenery)
            operation |= PTD_OPERATION_DONT_PLACE_SCENERY;
        cost = place_virtual_track(td6, operation, rideIndex, x, y, z);
    }

    if (cost == MONEY32_UNDEFINED || !(flags & GAME_COMMAND_FLAG_APPLY)) {
        rct_string_id error_reason = gGameCommandErrorText;
        game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, rideIndex, GAME_COMMAND_DEMOLISH_RIDE, 0, 0);
        gGameCommandErrorText = error_reason;
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        *outRideIndex = rideIndex;
        return cost;
    }

    if (entryIndex != 0xFF) {
        game_do_command(0, GAME_COMMAND_FLAG_APPLY | (2 << 8), 0, rideIndex | (entryIndex << 8), GAME_COMMAND_SET_RIDE_VEHICLES, 0, 0);
    }

    game_do_command(0, GAME_COMMAND_FLAG_APPLY | (td6->ride_mode << 8), 0, rideIndex | (0 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
    game_do_command(0, GAME_COMMAND_FLAG_APPLY | (0 << 8), 0, rideIndex | (td6->number_of_trains << 8), GAME_COMMAND_SET_RIDE_VEHICLES, 0, 0);
    game_do_command(0, GAME_COMMAND_FLAG_APPLY | (1 << 8), 0, rideIndex | (td6->number_of_cars_per_train << 8), GAME_COMMAND_SET_RIDE_VEHICLES, 0, 0);
    game_do_command(0, GAME_COMMAND_FLAG_APPLY | (td6->depart_flags << 8), 0, rideIndex | (1 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
    game_do_command(0, GAME_COMMAND_FLAG_APPLY | (td6->min_waiting_time << 8), 0, rideIndex | (2 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
    game_do_command(0, GAME_COMMAND_FLAG_APPLY | (td6->max_waiting_time << 8), 0, rideIndex | (3 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
    game_do_command(0, GAME_COMMAND_FLAG_APPLY | (td6->operation_setting << 8), 0, rideIndex | (4 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
    game_do_command(0, GAME_COMMAND_FLAG_APPLY | ((td6->lift_hill_speed_num_circuits & 0x1F) << 8), 0, rideIndex | (8 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);

    uint8 num_circuits = td6->lift_hill_speed_num_circuits >> 5;
    if (num_circuits == 0) {
        num_circuits = 1;
    }
    game_do_command(0, GAME_COMMAND_FLAG_APPLY | (num_circuits << 8), 0, rideIndex | (9 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);

    ride_set_to_default_inspection_interval(rideIndex);
    ride->lifecycle_flags |= RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN;
    ride->colour_scheme_type = td6->version_and_colour_scheme & 3;

    ride->entrance_style = td6->entrance_style;

    for (sint32 i = 0; i < 4; i++) {
        ride->track_colour_main[i] = td6->track_spine_colour[i];
        ride->track_colour_additional[i] = td6->track_rail_colour[i];
        ride->track_colour_supports[i] = td6->track_support_colour[i];
    }

    for (sint32 i = 0; i < 32; i++) {
        ride->vehicle_colours[i].body_colour = td6->vehicle_colours[i].body_colour;
        ride->vehicle_colours[i].trim_colour = td6->vehicle_colours[i].trim_colour;
        ride->vehicle_colours_extended[i] = td6->vehicle_additional_colour[i];
    }

    ride_set_name(rideIndex, td6->name);

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
    *outRideIndex = rideIndex;
    return cost;
}

static money32 place_maze_design(uint8 flags, uint8 rideIndex, uint16 mazeEntry, sint16 x, sint16 y, sint16 z)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
    gCommandPosition.x = x + 8;
    gCommandPosition.y = y + 8;
    gCommandPosition.z = z;
    if (!map_check_free_elements_and_reorganise(1)) {
        return MONEY32_UNDEFINED;
    }

    if ((z & 15) != 0) {
        return MONEY32_UNDEFINED;
    }

    if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED)) {
        if (game_is_paused() && !gCheatsBuildInPauseMode) {
            gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
            return MONEY32_UNDEFINED;
        }
    }

    if (flags & GAME_COMMAND_FLAG_APPLY) {
        if (!(flags & GAME_COMMAND_FLAG_GHOST)) {
            footpath_remove_litter(x, y, z);
            wall_remove_at(floor2(x, 32), floor2(y, 32), z, z + 32);
        }
    }

    if (!gCheatsSandboxMode) {
        if (!map_is_location_owned(floor2(x, 32), floor2(y, 32), z)) {
            return MONEY32_UNDEFINED;
        }
    }

    // Check support height
    if (!gCheatsDisableSupportLimits) {
        rct_map_element *mapElement = map_get_surface_element_at(x >> 5, y >> 5);
        uint8 supportZ = (z + 32) >> 3;
        if (supportZ > mapElement->base_height) {
            uint8 supportHeight = (supportZ - mapElement->base_height) / 2;
            uint8 maxSupportHeight = RideData5[RIDE_TYPE_MAZE].max_height;
            if (supportHeight > maxSupportHeight) {
                gGameCommandErrorText = STR_TOO_HIGH_FOR_SUPPORTS;
                return MONEY32_UNDEFINED;
            }
        }
    }

    money32 cost = 0;
    // Clearance checks
    if (!gCheatsDisableClearanceChecks) {
        sint32 fx = floor2(x, 32);
        sint32 fy = floor2(y, 32);
        sint32 fz0 = z >> 3;
        sint32 fz1 = fz0 + 4;

        if (!map_can_construct_with_clear_at(fx, fy, fz0, fz1, &map_place_non_scenery_clear_func, 15, flags, &cost)) {
            return MONEY32_UNDEFINED;
        }

        uint8 elctgaw = gMapGroundFlags;
        if (elctgaw & ELEMENT_IS_UNDERWATER) {
            gGameCommandErrorText = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
            return MONEY32_UNDEFINED;
        }
        if (elctgaw & ELEMENT_IS_UNDERGROUND) {
            gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
            return MONEY32_UNDEFINED;
        }
    }

    rct_ride *ride = get_ride(rideIndex);

    // Calculate price
    money32 price = 0;
    if (!(gParkFlags & PARK_FLAGS_NO_MONEY)) {
        price = RideTrackCosts[ride->type].track_price * TrackPricing[TRACK_ELEM_MAZE];
        price = (price >> 17) * 10;
    }

    cost += price;

    if (flags & GAME_COMMAND_FLAG_APPLY) {
        if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST)) {
            rct_xyz16 coord;
            coord.x = x + 8;
            coord.y = y + 8;
            coord.z = z;
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        // Place track element
        sint32 fx = floor2(x, 32);
        sint32 fy = floor2(y, 32);
        sint32 fz = z >> 3;
        rct_map_element *mapElement = map_element_insert(fx >> 5, fy >> 5, fz, 15);
        mapElement->clearance_height = fz + 4;
        mapElement->type = MAP_ELEMENT_TYPE_TRACK;
        mapElement->properties.track.type = TRACK_ELEM_MAZE;
        mapElement->properties.track.ride_index = rideIndex;
        mapElement->properties.track.maze_entry = mazeEntry;
        if (flags & GAME_COMMAND_FLAG_GHOST) {
            mapElement->flags |= MAP_ELEMENT_FLAG_GHOST;
        }

        map_invalidate_element(fx, fy, mapElement);

        ride->maze_tiles++;
        ride->station_heights[0] = mapElement->base_height;
        ride->station_starts[0] = 0;
        if (ride->maze_tiles == 1) {
            ride->overall_view = (fx >> 5) | ((fy >> 5) << 8);
        }
    }

    return cost;
}

/**
 *
 *  rct2: 0x006D13FE
 */
void game_command_place_track_design(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
{
    sint16 x = *eax & 0xFFFF;
    sint16 y = *ecx & 0xFFFF;
    sint16 z = *edi & 0xFFFF;
    uint8 flags = *ebx;
    uint8 rideIndex;
    *ebx = place_track_design(x, y, z, flags, &rideIndex);
    *edi = rideIndex;
}

/**
 *
 *  rct2: 0x006CDEE4
 */
void game_command_place_maze_design(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
{
    *ebx = place_maze_design(
        *ebx & 0xFF,
        *edx & 0xFF,
        ((*ebx >> 8) & 0xFF) | (((*edx >> 8) & 0xFF) << 8),
        *eax & 0xFFFF,
        *ecx & 0xFFFF,
        *edi & 0xFFFF
    );
}

#pragma region Track Design Preview

/**
 *
 *  rct2: 0x006D1EF0
 */
void track_design_draw_preview(rct_track_td6 *td6, uint8 *pixels)
{
    // Make a copy of the map
    map_backup *mapBackup = track_design_preview_backup_map();
    if (mapBackup == NULL) {
        return;
    }
    track_design_preview_clear_map();

    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
        track_design_load_scenery_objects(td6);
    }

    money32 cost;
    uint8 rideIndex;
    uint8 flags;
    if (!track_design_place_preview(td6, &cost, &rideIndex, &flags)) {
        memset(pixels, 0, TRACK_PREVIEW_IMAGE_SIZE * 4);
        track_design_preview_restore_map(mapBackup);
        return;
    }
    td6->cost = cost;
    td6->track_flags = flags & 7;

    rct_xyz32 centre;
    centre.x = (gTrackPreviewMin.x + gTrackPreviewMax.x) / 2 + 16;
    centre.y = (gTrackPreviewMin.y + gTrackPreviewMax.y) / 2 + 16;
    centre.z = (gTrackPreviewMin.z + gTrackPreviewMax.z) / 2;

    sint32 size_x = gTrackPreviewMax.x - gTrackPreviewMin.x;
    sint32 size_y = gTrackPreviewMax.y - gTrackPreviewMin.y;
    sint32 size_z = gTrackPreviewMax.z - gTrackPreviewMin.z;

    sint32 zoom_level = 1;

    if (size_x < size_y) {
        size_x = size_y;
    }

    if (size_x > 1000 || size_z > 280) {
        zoom_level = 2;
    }

    if (size_x > 1600 || size_z > 1000) {
        zoom_level = 3;
    }

    size_x = 370 << zoom_level;
    size_y = 217 << zoom_level;

    rct_viewport view;
    view.width = 370;
    view.height = 217;
    view.view_width = size_x;
    view.view_height = size_y;
    view.x = 0;
    view.y = 0;
    view.zoom = zoom_level;
    view.flags = VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_INVISIBLE_SPRITES;

    rct_drawpixelinfo dpi;
    dpi.zoom_level = zoom_level;
    dpi.x = 0;
    dpi.y = 0;
    dpi.width = 370;
    dpi.height = 217;
    dpi.pitch = 0;
    dpi.bits = pixels;

    rct_xy32 offset = { size_x / 2, size_y / 2 };
    for (sint32 i = 0; i < 4; i++) {
        gCurrentRotation = i;

        rct_xy32 pos2d = translate_3d_to_2d_with_z(i, centre);
        pos2d.x -= offset.x;
        pos2d.y -= offset.y;

        sint32 left = pos2d.x;
        sint32 top = pos2d.y;
        sint32 right = left + size_x;
        sint32 bottom = top + size_y;

        view.view_x = left;
        view.view_y = top;
        viewport_paint(&view, &dpi, left, top, right, bottom);

        dpi.bits += TRACK_PREVIEW_IMAGE_SIZE;
    }

    ride_delete(rideIndex);
    track_design_preview_restore_map(mapBackup);
}

/**
 * Create a backup of the map as it will be cleared for drawing the track
 * design preview.
 *  rct2: 0x006D1C68
 */
static map_backup *track_design_preview_backup_map()
{
    map_backup *backup = malloc(sizeof(map_backup));
    if (backup != NULL) {
        memcpy(
            backup->map_elements,
            gMapElements,
            sizeof(backup->map_elements)
        );
        memcpy(
            backup->tile_pointers,
            gMapElementTilePointers,
            sizeof(backup->tile_pointers)
        );
        backup->next_free_map_element = gNextFreeMapElement;
        backup->map_size_units = gMapSizeUnits;
        backup->map_size_units_minus_2 = gMapSizeMinus2;
        backup->map_size = gMapSize;
        backup->current_rotation = get_current_rotation();
    }
    return backup;
}

/**
 * Restores the map from a backup.
 *  rct2: 0x006D2378
 */
static void track_design_preview_restore_map(map_backup *backup)
{
    memcpy(
        gMapElements,
        backup->map_elements,
        sizeof(backup->map_elements)
    );
    memcpy(
        gMapElementTilePointers,
        backup->tile_pointers,
        sizeof(backup->tile_pointers)
    );
    gNextFreeMapElement = backup->next_free_map_element;
    gMapSizeUnits = backup->map_size_units;
    gMapSizeMinus2 = backup->map_size_units_minus_2;
    gMapSize = backup->map_size;
    gCurrentRotation = backup->current_rotation;

    free(backup);
}

/**
 * Resets all the map elements to surface tiles for track preview.
 *  rct2: 0x006D1D9A
 */
static void track_design_preview_clear_map()
{
    // These values were previously allocated in backup map but
    // it seems more fitting to place in this function
    gMapSizeUnits = 255 * 32;
    gMapSizeMinus2 = (264 * 32) - 2;
    gMapSize = 256;

    for (sint32 i = 0; i < MAX_TILE_MAP_ELEMENT_POINTERS; i++) {
        rct_map_element* map_element = &gMapElements[i];
        map_element->type = MAP_ELEMENT_TYPE_SURFACE;
        map_element->flags = MAP_ELEMENT_FLAG_LAST_TILE;
        map_element->base_height = 2;
        map_element->clearance_height = 0;
        map_element->properties.surface.slope = 0;
        map_element->properties.surface.terrain = 0;
        map_element->properties.surface.grass_length = GRASS_LENGTH_CLEAR_0;
        map_element->properties.surface.ownership = OWNERSHIP_OWNED;
    }
    map_update_tile_pointers();
}

#pragma endregion
