/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackDesign.h"

#include "../Cheats.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../actions/LargeSceneryRemoveAction.hpp"
#include "../actions/RideEntranceExitPlaceAction.hpp"
#include "../actions/RideSetSetting.hpp"
#include "../actions/RideSetVehiclesAction.hpp"
#include "../actions/SmallSceneryPlaceAction.hpp"
#include "../actions/SmallSceneryRemoveAction.hpp"
#include "../actions/TrackPlaceAction.hpp"
#include "../actions/TrackRemoveAction.hpp"
#include "../actions/WallRemoveAction.hpp"
#include "../audio/audio.h"
#include "../core/File.h"
#include "../core/String.hpp"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../rct1/RCT1.h"
#include "../rct1/Tables.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../world/Footpath.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"
#include "../world/Wall.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"
#include "TrackDesignRepository.h"

#include <algorithm>
#include <iterator>

struct map_backup
{
    TileElement tile_elements[MAX_TILE_ELEMENTS];
    TileElement* tile_pointers[MAX_TILE_TILE_ELEMENT_POINTERS];
    TileElement* next_free_tile_element;
    uint16_t map_size_units;
    uint16_t map_size_units_minus_2;
    uint16_t map_size;
    uint8_t current_rotation;
};

rct_track_td6* gActiveTrackDesign;
bool gTrackDesignSceneryToggle;
LocationXYZ16 gTrackPreviewMin;
LocationXYZ16 gTrackPreviewMax;
LocationXYZ16 gTrackPreviewOrigin;

bool byte_9D8150;
static uint8_t _trackDesignPlaceOperation;
static bool _trackDesignDontPlaceScenery;
static money32 _trackDesignPlaceCost;
static int16_t _trackDesignPlaceZ;
static int16_t _trackDesignPlaceSceneryZ;

// Previously all flags in byte_F4414E
static bool _trackDesignPlaceStateEntranceExitPlaced = false;
static bool _trackDesignPlaceStateSceneryUnavailable = false;
static bool _trackDesignPlaceStateHasScenery = false;
static bool _trackDesignPlaceStatePlaceScenery = true;

static rct_track_td6* track_design_open_from_buffer(uint8_t* src, size_t srcLength);

static map_backup* track_design_preview_backup_map();

static void track_design_preview_restore_map(map_backup* backup);

static void track_design_preview_clear_map();

static void td6_reset_trailing_elements(rct_track_td6* td6);

static void td6_set_element_helper_pointers(rct_track_td6* td6, bool clearScenery);

rct_track_td6* track_design_open(const utf8* path)
{
    log_verbose("track_design_open(\"%s\")", path);

    try
    {
        auto buffer = File::ReadAllBytes(path);
        if (!sawyercoding_validate_track_checksum(buffer.data(), buffer.size()))
        {
            log_error("Track checksum failed. %s", path);
            return nullptr;
        }

        // Decode the track data
        uint8_t* decoded = (uint8_t*)malloc(0x10000);
        size_t decodedLength = sawyercoding_decode_td6(buffer.data(), decoded, buffer.size());
        decoded = (uint8_t*)realloc(decoded, decodedLength);
        if (decoded == nullptr)
        {
            log_error("failed to realloc");
        }
        else
        {
            rct_track_td6* td6 = track_design_open_from_buffer(decoded, decodedLength);
            free(decoded);

            if (td6 != nullptr)
            {
                td6->name = String::Duplicate(GetNameFromTrackPath(path).c_str());
                return td6;
            }
        }
    }
    catch (const std::exception& e)
    {
        log_error("Unable to load track design: %s", e.what());
    }
    return nullptr;
}

static rct_track_td6* track_design_open_from_td4(uint8_t* src, size_t srcLength)
{
    rct_track_td4* td4 = (rct_track_td4*)calloc(1, sizeof(rct_track_td4));
    if (td4 == nullptr)
    {
        log_error("Unable to allocate memory for TD4 data.");
        SafeFree(td4);
        return nullptr;
    }

    uint8_t version = (src[7] >> 2) & 3;
    if (version == 0)
    {
        std::memcpy(td4, src, 0x38);
        td4->elementsSize = srcLength - 0x38;
        td4->elements = malloc(td4->elementsSize);
        if (td4->elements == nullptr)
        {
            log_error("Unable to allocate memory for TD4 element data.");
            SafeFree(td4);
            return nullptr;
        }
        std::memcpy(td4->elements, src + 0x38, td4->elementsSize);
    }
    else if (version == 1)
    {
        std::memcpy(td4, src, 0xC4);
        td4->elementsSize = srcLength - 0xC4;
        td4->elements = malloc(td4->elementsSize);
        if (td4->elements == nullptr)
        {
            log_error("Unable to allocate memory for TD4 element data.");
            SafeFree(td4);
            return nullptr;
        }
        std::memcpy(td4->elements, src + 0xC4, td4->elementsSize);
    }
    else
    {
        log_error("Unsupported track design.");
        SafeFree(td4);
        return nullptr;
    }

    rct_track_td6* td6 = (rct_track_td6*)calloc(1, sizeof(rct_track_td6));
    if (td6 == nullptr)
    {
        log_error("Unable to allocate memory for TD6 data.");
        SafeFree(td4);
        return nullptr;
    }

    td6->type = RCT1::GetRideType(td4->type);

    // All TD4s that use powered launch use the type that doesn't pass the station.
    td6->ride_mode = td4->mode;
    if (td4->mode == RCT1_RIDE_MODE_POWERED_LAUNCH)
    {
        td6->ride_mode = RIDE_MODE_POWERED_LAUNCH;
    }

    // Convert RCT1 vehicle type to RCT2 vehicle type. Intialise with an string consisting of 8 spaces.
    rct_object_entry vehicleObject = { 0x80, "        " };
    if (td4->type == RIDE_TYPE_MAZE)
    {
        const char* name = RCT1::GetRideTypeObject(td4->type);
        assert(name != nullptr);
        std::memcpy(vehicleObject.name, name, std::min(String::SizeOf(name), (size_t)8));
    }
    else
    {
        const char* name = RCT1::GetVehicleObject(td4->vehicle_type);
        assert(name != nullptr);
        std::memcpy(vehicleObject.name, name, std::min(String::SizeOf(name), (size_t)8));
    }
    std::memcpy(&td6->vehicle_object, &vehicleObject, sizeof(rct_object_entry));
    td6->vehicle_type = td4->vehicle_type;

    td6->flags = td4->flags;
    td6->version_and_colour_scheme = td4->version_and_colour_scheme;

    // Vehicle colours
    for (int32_t i = 0; i < RCT1_MAX_TRAINS_PER_RIDE; i++)
    {
        // RCT1 had no third colour
        RCT1::RCT1VehicleColourSchemeCopyDescriptor colourSchemeCopyDescriptor = RCT1::GetColourSchemeCopyDescriptor(
            td4->vehicle_type);
        if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_1)
        {
            td6->vehicle_colours[i].body_colour = RCT1::GetColour(td4->vehicle_colours[i].body_colour);
        }
        else if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_2)
        {
            td6->vehicle_colours[i].body_colour = RCT1::GetColour(td4->vehicle_colours[i].trim_colour);
        }
        else
        {
            td6->vehicle_colours[i].body_colour = colourSchemeCopyDescriptor.colour1;
        }

        if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_1)
        {
            td6->vehicle_colours[i].trim_colour = RCT1::GetColour(td4->vehicle_colours[i].body_colour);
        }
        else if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_2)
        {
            td6->vehicle_colours[i].trim_colour = RCT1::GetColour(td4->vehicle_colours[i].trim_colour);
        }
        else
        {
            td6->vehicle_colours[i].trim_colour = colourSchemeCopyDescriptor.colour2;
        }

        if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_1)
        {
            td6->vehicle_additional_colour[i] = RCT1::GetColour(td4->vehicle_colours[i].body_colour);
        }
        else if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_2)
        {
            td6->vehicle_additional_colour[i] = RCT1::GetColour(td4->vehicle_colours[i].trim_colour);
        }
        else
        {
            td6->vehicle_additional_colour[i] = colourSchemeCopyDescriptor.colour3;
        }
    }
    // Set remaining vehicles to same colour as first vehicle
    for (int32_t i = RCT1_MAX_TRAINS_PER_RIDE; i < MAX_VEHICLES_PER_RIDE; i++)
    {
        td6->vehicle_colours[i] = td6->vehicle_colours[0];
        td6->vehicle_additional_colour[i] = td6->vehicle_additional_colour[0];
    }

    // Track colours
    if (version == 0)
    {
        for (int32_t i = 0; i < NUM_COLOUR_SCHEMES; i++)
        {
            td6->track_spine_colour[i] = RCT1::GetColour(td4->track_spine_colour_v0);
            td6->track_rail_colour[i] = RCT1::GetColour(td4->track_rail_colour_v0);
            td6->track_support_colour[i] = RCT1::GetColour(td4->track_support_colour_v0);

            // Mazes were only hedges
            switch (td4->type)
            {
                case RCT1_RIDE_TYPE_HEDGE_MAZE:
                    td6->track_support_colour[i] = MAZE_WALL_TYPE_HEDGE;
                    break;
                case RCT1_RIDE_TYPE_RIVER_RAPIDS:
                    td6->track_spine_colour[i] = COLOUR_WHITE;
                    td6->track_rail_colour[i] = COLOUR_WHITE;
                    break;
            }
        }
    }
    else
    {
        for (int32_t i = 0; i < RCT12_NUM_COLOUR_SCHEMES; i++)
        {
            td6->track_spine_colour[i] = RCT1::GetColour(td4->track_spine_colour[i]);
            td6->track_rail_colour[i] = RCT1::GetColour(td4->track_rail_colour[i]);
            td6->track_support_colour[i] = RCT1::GetColour(td4->track_support_colour[i]);
        }
    }

    td6->depart_flags = td4->depart_flags;
    td6->number_of_trains = td4->number_of_trains;
    td6->number_of_cars_per_train = td4->number_of_cars_per_train;
    td6->min_waiting_time = td4->min_waiting_time;
    td6->max_waiting_time = td4->max_waiting_time;
    td6->operation_setting = std::min(td4->operation_setting, RideProperties[td6->type].max_value);
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
    if (version == 1)
    {
        td6->flags2 = td4->flags2;
    }

    td6->space_required_x = 255;
    td6->space_required_y = 255;
    td6->lift_hill_speed_num_circuits = 5;

    // Move elements across
    td6->elements = td4->elements;
    td6->elementsSize = td4->elementsSize;

    td6_reset_trailing_elements(td6);
    td6_set_element_helper_pointers(td6, true);

    SafeFree(td4);
    return td6;
}

static rct_track_td6* track_design_open_from_buffer(uint8_t* src, size_t srcLength)
{
    uint8_t version = (src[7] >> 2) & 3;
    if (version == 0 || version == 1)
    {
        return track_design_open_from_td4(src, srcLength);
    }
    else if (version != 2)
    {
        log_error("Unsupported track design.");
        return nullptr;
    }

    rct_track_td6* td6 = (rct_track_td6*)calloc(1, sizeof(rct_track_td6));
    if (td6 == nullptr)
    {
        log_error("Unable to allocate memory for TD6 data.");
        return nullptr;
    }
    std::memcpy(td6, src, 0xA3);
    td6->elementsSize = srcLength - 0xA3;
    td6->elements = malloc(td6->elementsSize);
    if (td6->elements == nullptr)
    {
        free(td6);
        log_error("Unable to allocate memory for TD6 element data.");
        return nullptr;
    }
    std::memcpy(td6->elements, src + 0xA3, td6->elementsSize);

    // Cap operation setting
    td6->operation_setting = std::min(td6->operation_setting, RideProperties[td6->type].max_value);

    td6_set_element_helper_pointers(td6, false);
    return td6;
}

static void td6_reset_trailing_elements(rct_track_td6* td6)
{
    void* lastElement;
    if (td6->type == RIDE_TYPE_MAZE)
    {
        rct_td6_maze_element* mazeElement = (rct_td6_maze_element*)td6->elements;
        while (mazeElement->all != 0)
        {
            mazeElement++;
        }
        lastElement = (void*)((uintptr_t)mazeElement + 1);

        size_t trailingSize = td6->elementsSize - (size_t)((uintptr_t)lastElement - (uintptr_t)td6->elements);
        std::memset(lastElement, 0, trailingSize);
    }
    else
    {
        rct_td6_track_element* trackElement = (rct_td6_track_element*)td6->elements;
        while (trackElement->type != 0xFF)
        {
            trackElement++;
        }
        lastElement = (void*)((uintptr_t)trackElement + 1);

        size_t trailingSize = td6->elementsSize - (size_t)((uintptr_t)lastElement - (uintptr_t)td6->elements);
        std::memset(lastElement, 0xFF, trailingSize);
    }
}

/**
 *
 * @param clearScenery Set when importing TD4 designs, to avoid corrupted data being interpreted as scenery.
 */
static void td6_set_element_helper_pointers(rct_track_td6* td6, bool clearScenery)
{
    uintptr_t sceneryElementsStart;

    if (td6->type == RIDE_TYPE_MAZE)
    {
        td6->track_elements = nullptr;
        td6->maze_elements = (rct_td6_maze_element*)td6->elements;

        rct_td6_maze_element* maze = td6->maze_elements;
        for (; maze->all != 0; maze++)
        {
        }
        sceneryElementsStart = (uintptr_t)(++maze);
    }
    else
    {
        td6->maze_elements = nullptr;
        td6->track_elements = (rct_td6_track_element*)td6->elements;

        rct_td6_track_element* track = td6->track_elements;
        for (; track->type != 0xFF; track++)
        {
        }
        uintptr_t entranceElementsStart = (uintptr_t)track + 1;

        rct_td6_entrance_element* entranceElement = (rct_td6_entrance_element*)entranceElementsStart;
        td6->entrance_elements = entranceElement;
        for (; entranceElement->z != -1; entranceElement++)
        {
        }
        sceneryElementsStart = (uintptr_t)entranceElement + 1;
    }

    if (clearScenery)
    {
        td6->scenery_elements = nullptr;
    }
    else
    {
        rct_td6_scenery_element* sceneryElement = (rct_td6_scenery_element*)sceneryElementsStart;
        td6->scenery_elements = sceneryElement;
    }
}

void track_design_dispose(rct_track_td6* td6)
{
    if (td6 != nullptr)
    {
        free(td6->elements);
        free(td6->name);
        free(td6);
    }
}

/**
 *
 *  rct2: 0x006ABDB0
 */
static void track_design_load_scenery_objects(rct_track_td6* td6)
{
    object_manager_unload_all_objects();

    // Load ride object
    rct_object_entry* rideEntry = &td6->vehicle_object;
    object_manager_load_object(rideEntry);

    // Load scenery objects
    rct_td6_scenery_element* scenery = td6->scenery_elements;
    for (; scenery != nullptr && scenery->scenery_object.end_flag != 0xFF; scenery++)
    {
        rct_object_entry* sceneryEntry = &scenery->scenery_object;
        object_manager_load_object(sceneryEntry);
    }
}

/**
 *
 *  rct2: 0x006D247A
 */
static void track_design_mirror_scenery(rct_track_td6* td6)
{
    rct_td6_scenery_element* scenery = td6->scenery_elements;
    for (; scenery != nullptr && scenery->scenery_object.end_flag != 0xFF; scenery++)
    {
        uint8_t entry_type, entry_index;
        if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index))
        {
            entry_type = object_entry_get_type(&scenery->scenery_object);
            if (entry_type != OBJECT_TYPE_PATHS)
            {
                continue;
            }
        }

        rct_scenery_entry* scenery_entry = (rct_scenery_entry*)object_entry_get_chunk(entry_type, entry_index);
        switch (entry_type)
        {
            case OBJECT_TYPE_LARGE_SCENERY:
            {
                int16_t x1 = 0, x2 = 0, y1 = 0, y2 = 0;
                for (rct_large_scenery_tile* tile = scenery_entry->large_scenery.tiles; tile->x_offset != -1; tile++)
                {
                    if (x1 > tile->x_offset)
                    {
                        x1 = tile->x_offset;
                    }
                    if (x2 < tile->x_offset)
                    {
                        x2 = tile->x_offset;
                    }
                    if (y1 > tile->y_offset)
                    {
                        y1 = tile->y_offset;
                    }
                    if (y2 > tile->y_offset)
                    {
                        y2 = tile->y_offset;
                    }
                }

                switch (scenery->flags & 3)
                {
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

                if (scenery_small_entry_has_flag(scenery_entry, SMALL_SCENERY_FLAG_DIAGONAL))
                {
                    scenery->flags ^= (1 << 0);
                    if (!scenery_small_entry_has_flag(scenery_entry, SMALL_SCENERY_FLAG_FULL_TILE))
                    {
                        scenery->flags ^= (1 << 2);
                    }
                    break;
                }
                if (scenery->flags & (1 << 0))
                {
                    scenery->flags ^= (1 << 1);
                }

                scenery->flags ^= (1 << 2);
                break;

            case OBJECT_TYPE_WALLS:
                scenery->y = -scenery->y;
                if (scenery->flags & (1 << 0))
                {
                    scenery->flags ^= (1 << 1);
                }
                break;

            case OBJECT_TYPE_PATHS:
                scenery->y = -scenery->y;

                if (scenery->flags & (1 << 5))
                {
                    scenery->flags ^= (1 << 6);
                }

                uint8_t flags = scenery->flags;
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
static void track_design_mirror_ride(rct_track_td6* td6)
{
    rct_td6_track_element* track = td6->track_elements;
    for (; track->type != 0xFF; track++)
    {
        track->type = TrackElementMirrorMap[track->type];
    }

    rct_td6_entrance_element* entrance = td6->entrance_elements;
    for (; entrance->z != -1; entrance++)
    {
        entrance->y = -entrance->y;
        if (entrance->direction & 1)
        {
            entrance->direction = direction_reverse(entrance->direction);
        }
    }
}

/** rct2: 0x00993EDC */
static constexpr const uint8_t maze_segment_mirror_map[] = {
    5, 4, 2, 7, 1, 0, 14, 3, 13, 12, 10, 15, 9, 8, 6, 11,
};

/**
 *
 *  rct2: 0x006D25FA
 */
static void track_design_mirror_maze(rct_track_td6* td6)
{
    rct_td6_maze_element* maze = td6->maze_elements;
    for (; maze->all != 0; maze++)
    {
        maze->y = -maze->y;

        if (maze->type == 0x8 || maze->type == 0x80)
        {
            if (maze->direction & 1)
            {
                maze->direction = direction_reverse(maze->direction);
            }
            continue;
        }

        uint16_t maze_entry = maze->maze_entry;
        uint16_t new_entry = 0;
        for (uint8_t position = bitscanforward(maze_entry); position != 0xFF; position = bitscanforward(maze_entry))
        {
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
void track_design_mirror(rct_track_td6* td6)
{
    if (td6->type == RIDE_TYPE_MAZE)
    {
        track_design_mirror_maze(td6);
    }
    else
    {
        track_design_mirror_ride(td6);
    }
    track_design_mirror_scenery(td6);
}

static void track_design_add_selection_tile(int16_t x, int16_t y)
{
    LocationXY16* selectionTile = gMapSelectionTiles;
    // Subtract 2 because the tile gets incremented later on
    for (; (selectionTile < gMapSelectionTiles + std::size(gMapSelectionTiles) - 2) && (selectionTile->x != -1);
         selectionTile++)
    {
        if (selectionTile->x == x && selectionTile->y == y)
        {
            return;
        }
        if (selectionTile + 1 >= &gMapSelectionTiles[300])
        {
            return;
        }
    }
    selectionTile->x = x;
    selectionTile->y = y;
    selectionTile++;
    selectionTile->x = -1;
}

static void track_design_update_max_min_coordinates(int16_t x, int16_t y, int16_t z)
{
    gTrackPreviewMin.x = std::min(gTrackPreviewMin.x, x);
    gTrackPreviewMax.x = std::max(gTrackPreviewMax.x, x);
    gTrackPreviewMin.y = std::min(gTrackPreviewMin.y, y);
    gTrackPreviewMax.y = std::max(gTrackPreviewMax.y, y);
    gTrackPreviewMin.z = std::min(gTrackPreviewMin.z, z);
    gTrackPreviewMax.z = std::max(gTrackPreviewMax.z, z);
}

/**
 *
 *  rct2: 0x006D0964
 */
static int32_t track_design_place_scenery(
    rct_td6_scenery_element* scenery_start, int32_t originX, int32_t originY, int32_t originZ)
{
    for (uint8_t mode = 0; mode <= 1; mode++)
    {
        if (scenery_start->scenery_object.end_flag != 0xFF)
        {
            _trackDesignPlaceStateHasScenery = true;
        }

        if (!_trackDesignPlaceStatePlaceScenery)
        {
            continue;
        }

        for (rct_td6_scenery_element* scenery = scenery_start; scenery->scenery_object.end_flag != 0xFF; scenery++)
        {
            uint8_t rotation = _currentTrackPieceDirection;
            LocationXY8 tile = { (uint8_t)(originX / 32), (uint8_t)(originY / 32) };
            switch (rotation & 3)
            {
                case TILE_ELEMENT_DIRECTION_WEST:
                    tile.x += scenery->x;
                    tile.y += scenery->y;
                    break;
                case TILE_ELEMENT_DIRECTION_NORTH:
                    tile.x += scenery->y;
                    tile.y -= scenery->x;
                    break;
                case TILE_ELEMENT_DIRECTION_EAST:
                    tile.x -= scenery->x;
                    tile.y -= scenery->y;
                    break;
                case TILE_ELEMENT_DIRECTION_SOUTH:
                    tile.x -= scenery->y;
                    tile.y += scenery->x;
                    break;
            }

            LocationXY16 mapCoord = { (int16_t)(tile.x * 32), (int16_t)(tile.y * 32) };
            track_design_update_max_min_coordinates(mapCoord.x, mapCoord.y, originZ);

            if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES && mode == 0)
            {
                uint8_t new_tile = 1;
                LocationXY16* selectionTile = gMapSelectionTiles;
                for (; selectionTile->x != -1; selectionTile++)
                {
                    if (selectionTile->x == tile.x && selectionTile->y == tile.y)
                    {
                        new_tile = 0;
                        break;
                    }
                    // Need to subtract one because selectionTile in following block is incremented
                    if (selectionTile + 1 >= &gMapSelectionTiles[std::size(gMapSelectionTiles) - 1])
                    {
                        new_tile = 0;
                        break;
                    }
                }
                if (new_tile)
                {
                    selectionTile->x = tile.x;
                    selectionTile->y = tile.y;
                    selectionTile++;
                    selectionTile->x = -1;
                }
            }

            if (_trackDesignPlaceOperation == PTD_OPERATION_CLEAR_OUTLINES && mode == 0)
            {
                uint8_t entry_type, entry_index;
                if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index))
                {
                    entry_type = object_entry_get_type(&scenery->scenery_object);
                    if (entry_type != OBJECT_TYPE_PATHS)
                    {
                        entry_type = 0xFF;
                    }
                    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
                    {
                        entry_type = 0xFF;
                    }

                    entry_index = 0;
                    for (PathSurfaceEntry* path = get_path_surface_entry(0);
                         entry_index < object_entry_group_counts[OBJECT_TYPE_PATHS];
                         path = get_path_surface_entry(entry_index), entry_index++)
                    {
                        if (path == nullptr)
                        {
                            continue;
                        }
                        if (path->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR)
                        {
                            continue;
                        }
                    }

                    if (entry_index == object_entry_group_counts[OBJECT_TYPE_PATHS])
                    {
                        entry_type = 0xFF;
                    }
                }
                int32_t z;
                const uint32_t flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5
                    | GAME_COMMAND_FLAG_GHOST;

                switch (entry_type)
                {
                    case OBJECT_TYPE_SMALL_SCENERY:
                    {
                        // bl
                        rotation += scenery->flags;
                        rotation &= 3;

                        // bh
                        uint8_t quadrant = (scenery->flags >> 2) + _currentTrackPieceDirection;
                        quadrant &= 3;

                        rct_scenery_entry* small_scenery = get_small_scenery_entry(entry_index);
                        if (!(!scenery_small_entry_has_flag(small_scenery, SMALL_SCENERY_FLAG_FULL_TILE)
                              && scenery_small_entry_has_flag(small_scenery, SMALL_SCENERY_FLAG_DIAGONAL))
                            && scenery_small_entry_has_flag(
                                   small_scenery,
                                   SMALL_SCENERY_FLAG_DIAGONAL | SMALL_SCENERY_FLAG_HALF_SPACE
                                       | SMALL_SCENERY_FLAG_THREE_QUARTERS))
                        {
                            quadrant = 0;
                        }

                        z = (scenery->z * 8 + originZ) / 8;

                        auto removeSceneryAction = SmallSceneryRemoveAction(mapCoord.x, mapCoord.y, z, quadrant, entry_index);
                        removeSceneryAction.SetFlags(flags);
                        removeSceneryAction.Execute();

                        break;
                    }
                    case OBJECT_TYPE_LARGE_SCENERY:
                    {
                        z = (scenery->z * 8 + originZ) / 8;

                        auto removeSceneryAction = LargeSceneryRemoveAction(
                            mapCoord.x, mapCoord.y, z, (rotation + scenery->flags) & 0x3, 0);
                        removeSceneryAction.SetFlags(flags);
                        removeSceneryAction.Execute();

                        break;
                    }
                    case OBJECT_TYPE_WALLS:
                    {
                        z = (scenery->z * 8 + originZ) / 8;

                        uint8_t direction = (rotation + scenery->flags) & TILE_ELEMENT_DIRECTION_MASK;

                        TileCoordsXYZD wallLocation = { tile.x, tile.y, z, direction };
                        auto wallRemoveAction = WallRemoveAction(wallLocation);
                        wallRemoveAction.SetFlags(flags);

                        GameActions::Execute(&wallRemoveAction);

                        break;
                    }
                    case OBJECT_TYPE_PATHS:
                        z = (scenery->z * 8 + originZ) / 8;
                        footpath_remove(mapCoord.x, mapCoord.y, z, flags);
                        break;
                }
            }

            if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z)
            {
                int32_t z = scenery->z * 8 + _trackDesignPlaceZ;
                if (z < _trackDesignPlaceSceneryZ)
                {
                    _trackDesignPlaceSceneryZ = z;
                }
            }

            if (_trackDesignPlaceOperation == PTD_OPERATION_1 || _trackDesignPlaceOperation == PTD_OPERATION_2
                || _trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z || _trackDesignPlaceOperation == PTD_OPERATION_4
                || _trackDesignPlaceOperation == PTD_OPERATION_GET_COST)
            {
                uint8_t entry_type, entry_index;
                if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index))
                {
                    entry_type = object_entry_get_type(&scenery->scenery_object);
                    if (entry_type != OBJECT_TYPE_PATHS)
                    {
                        _trackDesignPlaceStateSceneryUnavailable = true;
                        continue;
                    }

                    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
                    {
                        _trackDesignPlaceStateSceneryUnavailable = true;
                        continue;
                    }

                    entry_index = 0;
                    for (PathSurfaceEntry* path = get_path_surface_entry(0);
                         entry_index < object_entry_group_counts[OBJECT_TYPE_PATHS];
                         path = get_path_surface_entry(entry_index), entry_index++)
                    {
                        if (path == nullptr)
                        {
                            continue;
                        }
                        if (path->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR)
                        {
                            continue;
                        }
                    }

                    if (entry_index == object_entry_group_counts[OBJECT_TYPE_PATHS])
                    {
                        _trackDesignPlaceStateSceneryUnavailable = true;
                        continue;
                    }
                }

                money32 cost;
                int16_t z;
                uint8_t flags;
                uint8_t quadrant;

                switch (entry_type)
                {
                    case OBJECT_TYPE_SMALL_SCENERY:
                    {
                        if (mode != 0)
                        {
                            continue;
                        }
                        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z)
                        {
                            continue;
                        }

                        rotation += scenery->flags;
                        rotation &= 3;
                        z = scenery->z * 8 + originZ;
                        quadrant = ((scenery->flags >> 2) + _currentTrackPieceDirection) & 3;

                        flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY;
                        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY
                                | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
                        }
                        else if (_trackDesignPlaceOperation == PTD_OPERATION_4)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY
                                | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_5;
                        }
                        else if (_trackDesignPlaceOperation == PTD_OPERATION_1)
                        {
                            flags = GAME_COMMAND_FLAG_PATH_SCENERY;
                        }

                        gGameCommandErrorTitle = STR_CANT_POSITION_THIS_HERE;

                        auto smallSceneryPlace = SmallSceneryPlaceAction(
                            { mapCoord.x, mapCoord.y, z, rotation }, quadrant, entry_index, scenery->primary_colour,
                            scenery->secondary_colour);

                        smallSceneryPlace.SetFlags(flags);
                        auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&smallSceneryPlace)
                                                                   : GameActions::QueryNested(&smallSceneryPlace);

                        cost = res->Error == GA_ERROR::OK ? res->Cost : 0;
                        break;
                    }
                    case OBJECT_TYPE_LARGE_SCENERY:
                        if (mode != 0)
                        {
                            continue;
                        }
                        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z)
                        {
                            continue;
                        }

                        rotation += scenery->flags;
                        rotation &= 3;

                        z = scenery->z * 8 + originZ;

                        flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY;
                        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY
                                | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
                        }
                        else if (_trackDesignPlaceOperation == PTD_OPERATION_4)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY
                                | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_5;
                        }
                        else if (_trackDesignPlaceOperation == PTD_OPERATION_1)
                        {
                            flags = GAME_COMMAND_FLAG_PATH_SCENERY;
                        }

                        cost = game_do_command(
                            mapCoord.x, flags | (rotation << 8), mapCoord.y,
                            scenery->primary_colour | (scenery->secondary_colour << 8), GAME_COMMAND_PLACE_LARGE_SCENERY,
                            entry_index, z);

                        if (cost == MONEY32_UNDEFINED)
                        {
                            cost = 0;
                        }
                        break;
                    case OBJECT_TYPE_WALLS:
                        if (mode != 0)
                        {
                            continue;
                        }
                        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z)
                        {
                            continue;
                        }

                        z = scenery->z * 8 + originZ;
                        rotation += scenery->flags;
                        rotation &= 3;

                        flags = GAME_COMMAND_FLAG_APPLY;
                        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY
                                | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
                        }
                        else if (_trackDesignPlaceOperation == PTD_OPERATION_4)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5
                                | GAME_COMMAND_FLAG_GHOST;
                        }
                        else if (_trackDesignPlaceOperation == PTD_OPERATION_1)
                        {
                            flags = 0;
                        }

                        gGameCommandErrorTitle = STR_CANT_BUILD_PARK_ENTRANCE_HERE;

                        cost = game_do_command(
                            mapCoord.x, flags | (entry_index << 8), mapCoord.y, rotation | (scenery->primary_colour << 8),
                            GAME_COMMAND_PLACE_WALL, z, scenery->secondary_colour | ((scenery->flags & 0xFC) << 6));

                        if (cost == MONEY32_UNDEFINED)
                        {
                            cost = 0;
                        }
                        break;
                    case OBJECT_TYPE_PATHS:
                        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z)
                        {
                            continue;
                        }

                        z = (scenery->z * 8 + originZ) / 8;
                        if (mode == 0)
                        {
                            if (scenery->flags & (1 << 7))
                            {
                                // dh
                                entry_index |= (1 << 7);
                            }

                            uint8_t bh = ((scenery->flags & 0xF) << rotation);
                            flags = bh >> 4;
                            bh = (bh | flags) & 0xF;
                            flags = (((scenery->flags >> 5) + rotation) & 3) << 5;
                            bh |= flags;

                            bh |= scenery->flags & 0x90;

                            flags = GAME_COMMAND_FLAG_APPLY;
                            if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST)
                            {
                                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
                            }
                            if (_trackDesignPlaceOperation == PTD_OPERATION_4)
                            {
                                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5
                                    | GAME_COMMAND_FLAG_GHOST;
                            }
                            if (_trackDesignPlaceOperation == PTD_OPERATION_1)
                            {
                                flags = 0;
                            }

                            gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                            cost = game_do_command(
                                mapCoord.x, flags | (bh << 8), mapCoord.y, z | (entry_index << 8),
                                GAME_COMMAND_PLACE_PATH_FROM_TRACK, 0, 0);

                            if (cost == MONEY32_UNDEFINED)
                            {
                                cost = 0;
                            }
                        }
                        else
                        {
                            if (_trackDesignPlaceOperation == PTD_OPERATION_1)
                            {
                                continue;
                            }

                            TileElement* tile_element = map_get_path_element_at(mapCoord.x / 32, mapCoord.y / 32, z);

                            if (tile_element == nullptr)
                            {
                                continue;
                            }

                            footpath_queue_chain_reset();
                            footpath_remove_edges_at(mapCoord.x, mapCoord.y, tile_element);

                            flags = GAME_COMMAND_FLAG_APPLY;
                            if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST)
                            {
                                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
                            }
                            if (_trackDesignPlaceOperation == PTD_OPERATION_4)
                            {
                                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5
                                    | GAME_COMMAND_FLAG_GHOST;
                            }

                            footpath_connect_edges(mapCoord.x, mapCoord.y, tile_element, flags);
                            footpath_update_queue_chains();
                            continue;
                        }
                        break;
                    default:
                        _trackDesignPlaceStateSceneryUnavailable = true;
                        continue;
                }
                _trackDesignPlaceCost = add_clamp_money32(_trackDesignPlaceCost, cost);
                if (_trackDesignPlaceOperation != PTD_OPERATION_2)
                {
                    if (cost == MONEY32_UNDEFINED)
                    {
                        _trackDesignPlaceCost = MONEY32_UNDEFINED;
                    }
                }
                if (_trackDesignPlaceCost != MONEY32_UNDEFINED)
                {
                    continue;
                }
                if (_trackDesignPlaceOperation == PTD_OPERATION_2)
                {
                    continue;
                }
                return 0;
            }
        }
    }
    return 1;
}

static int32_t track_design_place_maze(rct_track_td6* td6, int16_t x, int16_t y, int16_t z, Ride* ride)
{
    if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES)
    {
        gMapSelectionTiles->x = -1;
        gMapSelectArrowPosition.x = x;
        gMapSelectArrowPosition.y = y;
        gMapSelectArrowPosition.z = tile_element_height(x, y) & 0xFFFF;
        gMapSelectArrowDirection = _currentTrackPieceDirection;
    }

    _trackDesignPlaceZ = 0;
    _trackDesignPlaceCost = 0;

    rct_td6_maze_element* maze_element = td6->maze_elements;
    for (; maze_element->all != 0; maze_element++)
    {
        uint8_t rotation = _currentTrackPieceDirection & 3;
        int16_t tmpX = maze_element->x * 32;
        int16_t tmpY = maze_element->y * 32;
        rotate_map_coordinates(&tmpX, &tmpY, rotation);
        CoordsXY mapCoord = { tmpX, tmpY };
        mapCoord.x += x;
        mapCoord.y += y;

        track_design_update_max_min_coordinates(mapCoord.x, mapCoord.y, z);

        if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES)
        {
            track_design_add_selection_tile(mapCoord.x, mapCoord.y);
        }

        if (_trackDesignPlaceOperation == PTD_OPERATION_1 || _trackDesignPlaceOperation == PTD_OPERATION_2
            || _trackDesignPlaceOperation == PTD_OPERATION_4 || _trackDesignPlaceOperation == PTD_OPERATION_GET_COST)
        {
            uint8_t flags;
            money32 cost = 0;
            uint16_t maze_entry;
            switch (maze_element->type)
            {
                case MAZE_ELEMENT_TYPE_ENTRANCE:
                    // entrance
                    rotation += maze_element->direction;
                    rotation &= 3;

                    flags = GAME_COMMAND_FLAG_APPLY;
                    gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

                    if (_trackDesignPlaceOperation == PTD_OPERATION_1)
                    {
                        auto res = RideEntranceExitPlaceAction::TrackPlaceQuery({ mapCoord.x, mapCoord.y, z }, false);
                        cost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
                    }
                    else
                    {
                        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
                        }
                        else if (_trackDesignPlaceOperation == PTD_OPERATION_4)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5
                                | GAME_COMMAND_FLAG_GHOST;
                        }
                        auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(mapCoord, rotation, ride->id, 0, false);
                        rideEntranceExitPlaceAction.SetFlags(flags);
                        auto res = GameActions::ExecuteNested(&rideEntranceExitPlaceAction);
                        cost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
                    }
                    if (cost != MONEY32_UNDEFINED)
                    {
                        _trackDesignPlaceStateEntranceExitPlaced = true;
                    }
                    break;
                case MAZE_ELEMENT_TYPE_EXIT:
                    // exit
                    rotation += maze_element->direction;
                    rotation &= 3;

                    flags = GAME_COMMAND_FLAG_APPLY;
                    gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

                    if (_trackDesignPlaceOperation == PTD_OPERATION_1)
                    {
                        auto res = RideEntranceExitPlaceAction::TrackPlaceQuery({ mapCoord.x, mapCoord.y, z }, true);
                        cost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
                    }
                    else
                    {
                        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
                        }
                        else if (_trackDesignPlaceOperation == PTD_OPERATION_4)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5
                                | GAME_COMMAND_FLAG_GHOST;
                        }
                        auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(mapCoord, rotation, ride->id, 0, true);
                        rideEntranceExitPlaceAction.SetFlags(flags);
                        auto res = GameActions::ExecuteNested(&rideEntranceExitPlaceAction);
                        cost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
                    }
                    if (cost != MONEY32_UNDEFINED)
                    {
                        _trackDesignPlaceStateEntranceExitPlaced = true;
                    }
                    break;
                default:
                    maze_entry = rol16(maze_element->maze_entry, rotation * 4);

                    if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST)
                    {
                        flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
                    }
                    else if (_trackDesignPlaceOperation == PTD_OPERATION_4)
                    {
                        flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5
                            | GAME_COMMAND_FLAG_GHOST;
                    }
                    else if (_trackDesignPlaceOperation == PTD_OPERATION_1)
                    {
                        flags = 0;
                    }
                    else
                    {
                        flags = GAME_COMMAND_FLAG_APPLY;
                    }

                    gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

                    cost = game_do_command(
                        mapCoord.x, flags | (maze_entry & 0xFF) << 8, mapCoord.y, ride->id | (maze_entry & 0xFF00),
                        GAME_COMMAND_PLACE_MAZE_DESIGN, z, 0);
                    break;
            }

            _trackDesignPlaceCost += cost;

            if (cost == MONEY32_UNDEFINED)
            {
                _trackDesignPlaceCost = cost;
                return 0;
            }
        }

        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z)
        {
            if (mapCoord.x < 0)
            {
                continue;
            }
            if (mapCoord.y < 0)
            {
                continue;
            }
            if (mapCoord.x >= 256 * 32)
            {
                continue;
            }
            if (mapCoord.y >= 256 * 32)
            {
                continue;
            }

            TileElement* tile_element = map_get_surface_element_at(mapCoord);
            int16_t map_height = tile_element->base_height * 8;
            if (tile_element->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
            {
                map_height += 16;
                if (tile_element->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
                {
                    map_height += 16;
                }
            }

            if (tile_element->AsSurface()->GetWaterHeight() > 0)
            {
                int16_t water_height = tile_element->AsSurface()->GetWaterHeight();
                water_height *= 16;
                if (water_height > map_height)
                {
                    map_height = water_height;
                }
            }

            int16_t temp_z = z + _trackDesignPlaceZ - map_height;
            if (temp_z < 0)
            {
                _trackDesignPlaceZ -= temp_z;
            }
        }
    }

    if (_trackDesignPlaceOperation == PTD_OPERATION_CLEAR_OUTLINES)
    {
        ride_action_modify(
            ride, RIDE_MODIFY_DEMOLISH,
            GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_GHOST);
    }

    gTrackPreviewOrigin.x = x;
    gTrackPreviewOrigin.y = y;
    gTrackPreviewOrigin.z = z;
    return 1;
}

static bool track_design_place_ride(rct_track_td6* td6, int16_t x, int16_t y, int16_t z, Ride* ride)
{
    const rct_preview_track** trackBlockArray = (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_TRACK)) ? TrackBlocks
                                                                                                          : FlatRideTrackBlocks;

    gTrackPreviewOrigin.x = x;
    gTrackPreviewOrigin.y = y;
    gTrackPreviewOrigin.z = z;
    if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES)
    {
        gMapSelectionTiles->x = -1;
        gMapSelectArrowPosition.x = x;
        gMapSelectArrowPosition.y = y;
        gMapSelectArrowPosition.z = tile_element_height(x, y) & 0xFFFF;
        gMapSelectArrowDirection = _currentTrackPieceDirection;
    }

    _trackDesignPlaceZ = 0;
    _trackDesignPlaceCost = 0;
    uint8_t rotation = _currentTrackPieceDirection;

    // Track elements
    rct_td6_track_element* track = td6->track_elements;
    for (; track->type != 0xFF; track++)
    {
        uint8_t trackType = track->type;
        if (trackType == TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP)
        {
            trackType = 0xFF;
        }

        track_design_update_max_min_coordinates(x, y, z);

        switch (_trackDesignPlaceOperation)
        {
            case PTD_OPERATION_DRAW_OUTLINES:
                for (const rct_preview_track* trackBlock = trackBlockArray[trackType]; trackBlock->index != 0xFF; trackBlock++)
                {
                    LocationXY16 tile = { x, y };
                    map_offset_with_rotation(&tile.x, &tile.y, trackBlock->x, trackBlock->y, rotation);
                    track_design_update_max_min_coordinates(tile.x, tile.y, z);
                    track_design_add_selection_tile(tile.x, tile.y);
                }
                break;
            case PTD_OPERATION_CLEAR_OUTLINES:
            {
                const rct_track_coordinates* trackCoordinates = &TrackCoordinates[trackType];
                const rct_preview_track* trackBlock = trackBlockArray[trackType];
                int32_t tempZ = z - trackCoordinates->z_begin + trackBlock->z;
                auto trackRemoveAction = TrackRemoveAction(trackType, 0, { x, y, tempZ, static_cast<Direction>(rotation & 3) });
                trackRemoveAction.SetFlags(
                    GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_GHOST);
                GameActions::ExecuteNested(&trackRemoveAction);
                break;
            }
            case PTD_OPERATION_1:
            case PTD_OPERATION_2:
            case PTD_OPERATION_4:
            case PTD_OPERATION_GET_COST:
            {
                const rct_track_coordinates* trackCoordinates = &TrackCoordinates[trackType];

                // di
                int16_t tempZ = z - trackCoordinates->z_begin;
                uint32_t trackColour = (track->flags >> 4) & 0x3;
                uint32_t brakeSpeed = (track->flags & 0x0F) * 2;
                uint32_t seatRotation = track->flags & 0x0F;

                int32_t liftHillAndAlternativeState = 0;
                if (track->flags & TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT)
                {
                    liftHillAndAlternativeState |= 1;
                }
                if (track->flags & TRACK_ELEMENT_FLAG_INVERTED)
                {
                    liftHillAndAlternativeState |= 2;
                }

                uint8_t flags = GAME_COMMAND_FLAG_APPLY;
                if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST)
                {
                    flags |= GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED;
                    flags |= GAME_COMMAND_FLAG_5;
                }
                else if (_trackDesignPlaceOperation == PTD_OPERATION_4)
                {
                    flags |= GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED;
                    flags |= GAME_COMMAND_FLAG_5;
                    flags |= GAME_COMMAND_FLAG_GHOST;
                }
                else if (_trackDesignPlaceOperation == PTD_OPERATION_1)
                {
                    flags = 0;
                }

                gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                auto trackPlaceAction = TrackPlaceAction(
                    _currentRideIndex, trackType, { x, y, tempZ, static_cast<uint8_t>(rotation) }, brakeSpeed, trackColour,
                    seatRotation, liftHillAndAlternativeState);
                trackPlaceAction.SetFlags(flags);

                auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&trackPlaceAction)
                                                           : GameActions::QueryNested(&trackPlaceAction);
                money32 cost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;

                _trackDesignPlaceCost += cost;
                if (cost == MONEY32_UNDEFINED)
                {
                    _trackDesignPlaceCost = cost;
                    return false;
                }
                break;
            }
            case PTD_OPERATION_GET_PLACE_Z:
            {
                int32_t tempZ = z - TrackCoordinates[trackType].z_begin;
                for (const rct_preview_track* trackBlock = trackBlockArray[trackType]; trackBlock->index != 0xFF; trackBlock++)
                {
                    int16_t tmpX = x;
                    int16_t tmpY = y;
                    map_offset_with_rotation(&tmpX, &tmpY, trackBlock->x, trackBlock->y, rotation);
                    CoordsXY tile = { tmpX, tmpY };
                    if (tile.x < 0 || tile.y < 0 || tile.x >= (256 * 32) || tile.y >= (256 * 32))
                    {
                        continue;
                    }

                    TileElement* tileElement = map_get_surface_element_at(tile);
                    if (tileElement == nullptr)
                    {
                        return false;
                    }

                    int32_t height = tileElement->base_height * 8;
                    if (tileElement->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
                    {
                        height += 16;
                        if (tileElement->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
                        {
                            height += 16;
                        }
                    }

                    uint8_t water_height = tileElement->AsSurface()->GetWaterHeight() * 16;
                    if (water_height > 0 && water_height > height)
                    {
                        height = water_height;
                    }
                    int32_t heightDifference = tempZ + _trackDesignPlaceZ + trackBlock->z - height;
                    if (heightDifference < 0)
                    {
                        _trackDesignPlaceZ -= heightDifference;
                    }
                }
                break;
            }
        }

        const rct_track_coordinates* track_coordinates = &TrackCoordinates[trackType];
        map_offset_with_rotation(&x, &y, track_coordinates->x, track_coordinates->y, rotation);
        z -= track_coordinates->z_begin;
        z += track_coordinates->z_end;

        rotation = (rotation + track_coordinates->rotation_end - track_coordinates->rotation_begin) & 3;
        if (track_coordinates->rotation_end & (1 << 2))
        {
            rotation |= (1 << 2);
        }
        else
        {
            x += CoordsDirectionDelta[rotation].x;
            y += CoordsDirectionDelta[rotation].y;
        }
    }

    // Entrance elements
    rct_td6_entrance_element* entrance = td6->entrance_elements;
    for (; entrance->z != -1; entrance++)
    {
        rotation = _currentTrackPieceDirection & 3;
        x = entrance->x;
        y = entrance->y;
        rotate_map_coordinates(&x, &y, rotation);
        x += gTrackPreviewOrigin.x;
        y += gTrackPreviewOrigin.y;

        track_design_update_max_min_coordinates(x, y, z);

        switch (_trackDesignPlaceOperation)
        {
            case PTD_OPERATION_DRAW_OUTLINES:
                track_design_add_selection_tile(x, y);
                break;
            case PTD_OPERATION_1:
            case PTD_OPERATION_2:
            case PTD_OPERATION_4:
            case PTD_OPERATION_GET_COST:
            {
                rotation = (rotation + entrance->direction) & 3;
                bool isExit = false;
                if (entrance->direction & (1 << 7))
                {
                    isExit = true;
                }

                if (_trackDesignPlaceOperation != PTD_OPERATION_1)
                {
                    LocationXY16 tile = {
                        (int16_t)(x + CoordsDirectionDelta[rotation].x),
                        (int16_t)(y + CoordsDirectionDelta[rotation].y),
                    };
                    TileElement* tile_element = map_get_first_element_at(tile.x >> 5, tile.y >> 5);
                    z = gTrackPreviewOrigin.z / 8;
                    z += (entrance->z == (int8_t)(uint8_t)0x80) ? -1 : entrance->z;

                    do
                    {
                        if (tile_element->GetType() != TILE_ELEMENT_TYPE_TRACK)
                        {
                            continue;
                        }
                        if (tile_element->base_height != z)
                        {
                            continue;
                        }

                        int32_t stationIndex = tile_element->AsTrack()->GetStationIndex();
                        uint8_t flags = GAME_COMMAND_FLAG_APPLY;
                        if (_trackDesignPlaceOperation == PTD_OPERATION_GET_COST)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
                        }
                        if (_trackDesignPlaceOperation == PTD_OPERATION_4)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5
                                | GAME_COMMAND_FLAG_GHOST;
                        }
                        if (_trackDesignPlaceOperation == PTD_OPERATION_1)
                        {
                            flags = 0;
                        }

                        gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                        auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(
                            { x, y }, rotation, ride->id, stationIndex, isExit);
                        rideEntranceExitPlaceAction.SetFlags(flags);
                        auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&rideEntranceExitPlaceAction)
                                                                   : GameActions::QueryNested(&rideEntranceExitPlaceAction);

                        _trackDesignPlaceCost += res->Cost;

                        if (res->Error != GA_ERROR::OK)
                        {
                            _trackDesignPlaceCost = MONEY32_UNDEFINED;
                            return false;
                        }
                        _trackDesignPlaceStateEntranceExitPlaced = true;
                        break;
                    } while (!(tile_element++)->IsLastForTile());
                }
                else
                {
                    z = (entrance->z == (int8_t)(uint8_t)0x80) ? -1 : entrance->z;
                    z *= 8;
                    z += gTrackPreviewOrigin.z;

                    auto res = RideEntranceExitPlaceAction::TrackPlaceQuery({ x, y, z }, false);
                    if (res->Error != GA_ERROR::OK)
                    {
                        _trackDesignPlaceCost = MONEY32_UNDEFINED;
                        return false;
                    }
                    else
                    {
                        _trackDesignPlaceCost += res->Cost;
                        _trackDesignPlaceStateEntranceExitPlaced = true;
                    }
                }
                break;
            }
        }
    }

    if (_trackDesignPlaceOperation == PTD_OPERATION_CLEAR_OUTLINES)
    {
        sub_6CB945(ride);
        ride_delete(ride);
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
int32_t place_virtual_track(
    rct_track_td6* td6, uint8_t ptdOperation, bool placeScenery, Ride* ride, int16_t x, int16_t y, int16_t z)
{
    // Previously byte_F4414E was cleared here
    _trackDesignPlaceStatePlaceScenery = placeScenery;
    _trackDesignPlaceStateEntranceExitPlaced = false;
    _trackDesignPlaceStateSceneryUnavailable = false;
    _trackDesignPlaceStateHasScenery = false;

    _trackDesignPlaceOperation = ptdOperation;
    if (gTrackDesignSceneryToggle)
    {
        _trackDesignPlaceStatePlaceScenery = false;
    }
    _currentRideIndex = ride->id;

    gTrackPreviewMin.x = x;
    gTrackPreviewMin.y = y;
    gTrackPreviewMin.z = z;
    gTrackPreviewMax.x = x;
    gTrackPreviewMax.y = y;
    gTrackPreviewMax.z = z;

    _trackDesignPlaceSceneryZ = 0;
    uint8_t track_place_success = 0;
    if (td6->type == RIDE_TYPE_MAZE)
    {
        track_place_success = track_design_place_maze(td6, x, y, z, ride);
    }
    else
    {
        track_place_success = track_design_place_ride(td6, x, y, z, ride);
    }

    // Scenery elements
    rct_td6_scenery_element* scenery = td6->scenery_elements;
    if (track_place_success && scenery != nullptr)
    {
        if (!track_design_place_scenery(scenery, gTrackPreviewOrigin.x, gTrackPreviewOrigin.y, gTrackPreviewOrigin.z))
        {
            return _trackDesignPlaceCost;
        }
    }

    // 0x6D0FE6
    if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES)
    {
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;
        map_invalidate_map_selection_tiles();
    }

    if (ptdOperation == PTD_OPERATION_GET_PLACE_Z)
    {
        // Change from vanilla: originally, _trackDesignPlaceSceneryZ was not subtracted
        // from _trackDesignPlaceZ, causing bug #259.
        return _trackDesignPlaceZ - _trackDesignPlaceSceneryZ;
    }
    return _trackDesignPlaceCost;
}

/**
 *
 *  rct2: 0x006D2189
 * ebx = ride_id
 * cost = edi
 */
static bool track_design_place_preview(rct_track_td6* td6, money32* cost, Ride** outRide, uint8_t* flags)
{
    *outRide = nullptr;
    *flags = 0;

    uint8_t entry_type, entry_index;
    if (!find_object_in_entry_group(&td6->vehicle_object, &entry_type, &entry_index))
    {
        entry_index = RIDE_ENTRY_INDEX_NULL;
    }

    ride_id_t rideIndex;
    uint8_t colour;
    uint8_t rideCreateFlags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
    if (ride_create_command(td6->type, entry_index, rideCreateFlags, &rideIndex, &colour) == MONEY32_UNDEFINED)
    {
        return false;
    }

    auto ride = get_ride(rideIndex);
    rct_string_id new_ride_name = user_string_allocate(USER_STRING_HIGH_ID_NUMBER | USER_STRING_DUPLICATION_PERMITTED, "");
    if (new_ride_name != 0)
    {
        rct_string_id old_name = ride->name;
        ride->name = new_ride_name;
        user_string_free(old_name);
    }

    ride->entrance_style = td6->entrance_style;

    for (int32_t i = 0; i < RCT12_NUM_COLOUR_SCHEMES; i++)
    {
        ride->track_colour[i].main = td6->track_spine_colour[i];
        ride->track_colour[i].additional = td6->track_rail_colour[i];
        ride->track_colour[i].supports = td6->track_support_colour[i];
    }

    // Flat rides need their vehicle colours loaded for display
    // in the preview window
    if (!ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_TRACK))
    {
        for (int32_t i = 0; i < RCT12_MAX_VEHICLE_COLOURS; i++)
        {
            ride->vehicle_colours[i].Body = td6->vehicle_colours[i].body_colour;
            ride->vehicle_colours[i].Trim = td6->vehicle_colours[i].trim_colour;
            ride->vehicle_colours[i].Ternary = td6->vehicle_additional_colour[i];
        }
    }

    byte_9D8150 = true;
    uint8_t backup_rotation = _currentTrackPieceDirection;
    uint32_t backup_park_flags = gParkFlags;
    gParkFlags &= ~PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
    int32_t mapSize = gMapSize << 4;

    _currentTrackPieceDirection = 0;
    int32_t z = place_virtual_track(td6, PTD_OPERATION_GET_PLACE_Z, true, get_ride(0), mapSize, mapSize, 16);

    if (_trackDesignPlaceStateHasScenery)
    {
        *flags |= TRACK_DESIGN_FLAG_HAS_SCENERY;
    }

    z += 16 - _trackDesignPlaceSceneryZ;

    bool placeScenery = true;
    if (_trackDesignPlaceStateSceneryUnavailable)
    {
        placeScenery = false;
        *flags |= TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE;
    }

    money32 resultCost = place_virtual_track(td6, PTD_OPERATION_GET_COST, placeScenery, ride, mapSize, mapSize, z);
    gParkFlags = backup_park_flags;

    if (resultCost != MONEY32_UNDEFINED)
    {
        if (!find_object_in_entry_group(&td6->vehicle_object, &entry_type, &entry_index))
        {
            *flags |= TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE;
        }
        else if (!ride_entry_is_invented(entry_index) && !gCheatsIgnoreResearchStatus)
        {
            *flags |= TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE;
        }

        _currentTrackPieceDirection = backup_rotation;
        byte_9D8150 = false;
        *cost = resultCost;
        *outRide = ride;
        return true;
    }
    else
    {
        _currentTrackPieceDirection = backup_rotation;
        user_string_free(ride->name);
        ride->type = RIDE_TYPE_NULL;
        byte_9D8150 = false;
        return false;
    }
}

static money32 place_track_design(int16_t x, int16_t y, int16_t z, uint8_t flags, ride_id_t* outRideIndex)
{
    *outRideIndex = RIDE_ID_NULL;

    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = z;

    if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED))
    {
        if (game_is_paused() && !gCheatsBuildInPauseMode)
        {
            gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
            return MONEY32_UNDEFINED;
        }
    }

    rct_track_td6* td6 = gActiveTrackDesign;
    if (td6 == nullptr)
    {
        return MONEY32_UNDEFINED;
    }
    rct_object_entry* rideEntryObject = &td6->vehicle_object;

    uint8_t entryType, entryIndex;
    if (!find_object_in_entry_group(rideEntryObject, &entryType, &entryIndex))
    {
        entryIndex = 0xFF;
    }
    // Force a fallback if the entry is not invented yet a td6 of it is selected, which can happen in select-by-track-type mode.
    else if (!ride_entry_is_invented(entryIndex) && !gCheatsIgnoreResearchStatus)
    {
        entryIndex = 0xFF;
    }

    // The rest of the cases are handled by the code in ride_create()
    if (RideGroupManager::RideTypeHasRideGroups(td6->type) && entryIndex == 0xFF)
    {
        const ObjectRepositoryItem* ori = object_repository_find_object_by_name(rideEntryObject->name);
        if (ori != nullptr)
        {
            uint8_t rideGroupIndex = ori->RideInfo.RideGroupIndex;
            const RideGroup* td6RideGroup = RideGroupManager::RideGroupFind(td6->type, rideGroupIndex);

            uint8_t* availableRideEntries = get_ride_entry_indices_for_ride_type(td6->type);
            for (uint8_t* rei = availableRideEntries; *rei != RIDE_ENTRY_INDEX_NULL; rei++)
            {
                rct_ride_entry* ire = get_ride_entry(*rei);

                if (!ride_entry_is_invented(*rei) && !gCheatsIgnoreResearchStatus)
                {
                    continue;
                }

                const RideGroup* irg = RideGroupManager::GetRideGroup(td6->type, ire);
                if (td6RideGroup->Equals(irg))
                {
                    entryIndex = *rei;
                    break;
                }
            }
        }
    }

    ride_id_t rideIndex;
    uint8_t rideColour;
    money32 createRideResult = ride_create_command(td6->type, entryIndex, flags, &rideIndex, &rideColour);
    if (createRideResult == MONEY32_UNDEFINED)
    {
        gGameCommandErrorTitle = STR_CANT_CREATE_NEW_RIDE_ATTRACTION;
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        return MONEY32_UNDEFINED;
    }

    auto ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_NULL)
    {
        log_warning("Invalid game command for track placement, ride id = %d", ride->id);
        return MONEY32_UNDEFINED;
    }

    money32 cost = 0;
    if (!(flags & GAME_COMMAND_FLAG_APPLY))
    {
        _trackDesignDontPlaceScenery = false;
        cost = place_virtual_track(td6, PTD_OPERATION_1, true, ride, x, y, z);
        if (_trackDesignPlaceStateSceneryUnavailable)
        {
            _trackDesignDontPlaceScenery = true;
            cost = place_virtual_track(td6, PTD_OPERATION_1, false, ride, x, y, z);
        }
    }
    else
    {
        uint8_t operation;
        if (flags & GAME_COMMAND_FLAG_GHOST)
        {
            operation = PTD_OPERATION_4;
        }
        else
        {
            operation = PTD_OPERATION_2;
        }

        cost = place_virtual_track(td6, operation, !_trackDesignDontPlaceScenery, ride, x, y, z);
    }

    if (cost == MONEY32_UNDEFINED || !(flags & GAME_COMMAND_FLAG_APPLY))
    {
        rct_string_id error_reason = gGameCommandErrorText;
        ride_action_modify(ride, RIDE_MODIFY_DEMOLISH, flags);
        gGameCommandErrorText = error_reason;
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        *outRideIndex = ride->id;
        return cost;
    }

    if (entryIndex != 0xFF)
    {
        auto colour = ride_get_unused_preset_vehicle_colour(entryIndex);
        auto rideSetVehicleAction = RideSetVehicleAction(ride->id, RideSetVehicleType::RideEntry, entryIndex, colour);
        flags& GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&rideSetVehicleAction)
                                       : GameActions::QueryNested(&rideSetVehicleAction);
    }

    set_operating_setting_nested(ride->id, RideSetSetting::Mode, td6->ride_mode, flags);
    auto rideSetVehicleAction2 = RideSetVehicleAction(ride->id, RideSetVehicleType::NumTrains, td6->number_of_trains);
    flags& GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&rideSetVehicleAction2)
                                   : GameActions::QueryNested(&rideSetVehicleAction2);
    auto rideSetVehicleAction3 = RideSetVehicleAction(
        ride->id, RideSetVehicleType::NumCarsPerTrain, td6->number_of_cars_per_train);
    flags& GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&rideSetVehicleAction3)
                                   : GameActions::QueryNested(&rideSetVehicleAction3);
    set_operating_setting_nested(ride->id, RideSetSetting::Departure, td6->depart_flags, flags);
    set_operating_setting_nested(ride->id, RideSetSetting::MinWaitingTime, td6->min_waiting_time, flags);
    set_operating_setting_nested(ride->id, RideSetSetting::MaxWaitingTime, td6->max_waiting_time, flags);
    set_operating_setting_nested(ride->id, RideSetSetting::Operation, td6->operation_setting, flags);
    set_operating_setting_nested(ride->id, RideSetSetting::LiftHillSpeed, td6->lift_hill_speed_num_circuits & 0x1F, flags);

    uint8_t num_circuits = td6->lift_hill_speed_num_circuits >> 5;
    if (num_circuits == 0)
    {
        num_circuits = 1;
    }
    set_operating_setting_nested(ride->id, RideSetSetting::NumCircuits, num_circuits, flags);
    ride_set_to_default_inspection_interval(ride);
    ride->lifecycle_flags |= RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN;
    ride->colour_scheme_type = td6->version_and_colour_scheme & 3;

    ride->entrance_style = td6->entrance_style;

    for (int32_t i = 0; i < RCT12_NUM_COLOUR_SCHEMES; i++)
    {
        ride->track_colour[i].main = td6->track_spine_colour[i];
        ride->track_colour[i].additional = td6->track_rail_colour[i];
        ride->track_colour[i].supports = td6->track_support_colour[i];
    }

    for (int32_t i = 0; i < MAX_VEHICLES_PER_RIDE; i++)
    {
        ride->vehicle_colours[i].Body = td6->vehicle_colours[i].body_colour;
        ride->vehicle_colours[i].Trim = td6->vehicle_colours[i].trim_colour;
        ride->vehicle_colours[i].Ternary = td6->vehicle_additional_colour[i];
    }

    ride_set_name(ride, td6->name, flags);

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
    *outRideIndex = ride->id;
    return cost;
}

static money32 place_maze_design(uint8_t flags, Ride* ride, uint16_t mazeEntry, int16_t x, int16_t y, int16_t z)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
    gCommandPosition.x = x + 8;
    gCommandPosition.y = y + 8;
    gCommandPosition.z = z;
    if (!map_check_free_elements_and_reorganise(1))
    {
        return MONEY32_UNDEFINED;
    }

    if ((z & 15) != 0)
    {
        return MONEY32_UNDEFINED;
    }

    if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED))
    {
        if (game_is_paused() && !gCheatsBuildInPauseMode)
        {
            gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
            return MONEY32_UNDEFINED;
        }
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (!(flags & GAME_COMMAND_FLAG_GHOST))
        {
            footpath_remove_litter(x, y, z);
            wall_remove_at(floor2(x, 32), floor2(y, 32), z, z + 32);
        }
    }

    if (!gCheatsSandboxMode)
    {
        if (!map_is_location_owned(floor2(x, 32), floor2(y, 32), z))
        {
            return MONEY32_UNDEFINED;
        }
    }

    // Check support height
    if (!gCheatsDisableSupportLimits)
    {
        TileElement* tileElement = map_get_surface_element_at({ x, y });
        uint8_t supportZ = (z + 32) >> 3;
        if (supportZ > tileElement->base_height)
        {
            uint8_t supportHeight = (supportZ - tileElement->base_height) / 2;
            uint8_t maxSupportHeight = RideData5[RIDE_TYPE_MAZE].max_height;
            if (supportHeight > maxSupportHeight)
            {
                gGameCommandErrorText = STR_TOO_HIGH_FOR_SUPPORTS;
                return MONEY32_UNDEFINED;
            }
        }
    }

    money32 cost = 0;
    // Clearance checks
    if (!gCheatsDisableClearanceChecks)
    {
        int32_t fx = floor2(x, 32);
        int32_t fy = floor2(y, 32);
        int32_t fz0 = z >> 3;
        int32_t fz1 = fz0 + 4;

        if (!map_can_construct_with_clear_at(
                fx, fy, fz0, fz1, &map_place_non_scenery_clear_func, { 0b1111, 0 }, flags, &cost, CREATE_CROSSING_MODE_NONE))
        {
            return MONEY32_UNDEFINED;
        }

        uint8_t elctgaw = gMapGroundFlags;
        if (elctgaw & ELEMENT_IS_UNDERWATER)
        {
            gGameCommandErrorText = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
            return MONEY32_UNDEFINED;
        }
        if (elctgaw & ELEMENT_IS_UNDERGROUND)
        {
            gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
            return MONEY32_UNDEFINED;
        }
    }

    // Calculate price
    money32 price = 0;
    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        price = RideTrackCosts[ride->type].track_price * TrackPricing[TRACK_ELEM_MAZE];
        price = (price >> 17) * 10;
    }

    cost += price;

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST))
        {
            LocationXYZ16 coord;
            coord.x = x + 8;
            coord.y = y + 8;
            coord.z = z;
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        // Place track element
        int32_t fx = floor2(x, 32);
        int32_t fy = floor2(y, 32);
        int32_t fz = z >> 3;
        TileElement* tileElement = tile_element_insert(fx >> 5, fy >> 5, fz, 15);
        tileElement->clearance_height = fz + 4;
        tileElement->SetType(TILE_ELEMENT_TYPE_TRACK);
        tileElement->AsTrack()->SetTrackType(TRACK_ELEM_MAZE);
        tileElement->AsTrack()->SetRideIndex(ride->id);
        tileElement->AsTrack()->SetMazeEntry(mazeEntry);
        if (flags & GAME_COMMAND_FLAG_GHOST)
        {
            tileElement->SetGhost(true);
        }

        map_invalidate_element(fx, fy, tileElement);

        ride->maze_tiles++;
        ride->stations[0].Height = tileElement->base_height;
        ride->stations[0].Start.xy = 0;
        if (ride->maze_tiles == 1)
        {
            ride->overall_view.x = fx / 32;
            ride->overall_view.y = fy / 32;
        }
    }

    return cost;
}

/**
 *
 *  rct2: 0x006D13FE
 */
void game_command_place_track_design(
    int32_t* eax, int32_t* ebx, int32_t* ecx, [[maybe_unused]] int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi,
    [[maybe_unused]] int32_t* ebp)
{
    int16_t x = *eax & 0xFFFF;
    int16_t y = *ecx & 0xFFFF;
    int16_t z = *edi & 0xFFFF;
    uint8_t flags = *ebx;
    ride_id_t rideIndex;
    *ebx = place_track_design(x, y, z, flags, &rideIndex);
    *edi = rideIndex;
}

/**
 *
 *  rct2: 0x006CDEE4
 */
void game_command_place_maze_design(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi,
    [[maybe_unused]] int32_t* ebp)
{
    auto ride = get_ride(*edx & 0xFF);
    *ebx = place_maze_design(
        *ebx & 0xFF, ride, ((*ebx >> 8) & 0xFF) | (((*edx >> 8) & 0xFF) << 8), *eax & 0xFFFF, *ecx & 0xFFFF, *edi & 0xFFFF);
}

#pragma region Track Design Preview

/**
 *
 *  rct2: 0x006D1EF0
 */
void track_design_draw_preview(rct_track_td6* td6, uint8_t* pixels)
{
    // Make a copy of the map
    map_backup* mapBackup = track_design_preview_backup_map();
    if (mapBackup == nullptr)
    {
        return;
    }
    track_design_preview_clear_map();

    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
    {
        track_design_load_scenery_objects(td6);
    }

    money32 cost;
    Ride* ride;
    uint8_t flags;
    if (!track_design_place_preview(td6, &cost, &ride, &flags))
    {
        std::fill_n(pixels, TRACK_PREVIEW_IMAGE_SIZE * 4, 0x00);
        track_design_preview_restore_map(mapBackup);
        return;
    }
    td6->cost = cost;
    td6->track_flags = flags & 7;

    CoordsXYZ centre;
    centre.x = (gTrackPreviewMin.x + gTrackPreviewMax.x) / 2 + 16;
    centre.y = (gTrackPreviewMin.y + gTrackPreviewMax.y) / 2 + 16;
    centre.z = (gTrackPreviewMin.z + gTrackPreviewMax.z) / 2;

    int32_t size_x = gTrackPreviewMax.x - gTrackPreviewMin.x;
    int32_t size_y = gTrackPreviewMax.y - gTrackPreviewMin.y;
    int32_t size_z = gTrackPreviewMax.z - gTrackPreviewMin.z;

    // Special case for flat rides - Z-axis info is irrelevant
    // and must be zeroed out lest the preview be off-centre
    if (!ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_TRACK))
    {
        centre.z = 0;
        size_z = 0;
    }

    int32_t zoom_level = 1;

    if (size_x < size_y)
    {
        size_x = size_y;
    }

    if (size_x > 1000 || size_z > 280)
    {
        zoom_level = 2;
    }

    if (size_x > 1600 || size_z > 1000)
    {
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

    CoordsXY offset = { size_x / 2, size_y / 2 };
    for (uint8_t i = 0; i < 4; i++)
    {
        gCurrentRotation = i;

        CoordsXY pos2d = translate_3d_to_2d_with_z(i, centre);
        pos2d.x -= offset.x;
        pos2d.y -= offset.y;

        int32_t left = pos2d.x;
        int32_t top = pos2d.y;
        int32_t right = left + size_x;
        int32_t bottom = top + size_y;

        view.view_x = left;
        view.view_y = top;
        viewport_paint(&view, &dpi, left, top, right, bottom);

        dpi.bits += TRACK_PREVIEW_IMAGE_SIZE;
    }

    ride_delete(ride);
    track_design_preview_restore_map(mapBackup);
}

/**
 * Create a backup of the map as it will be cleared for drawing the track
 * design preview.
 *  rct2: 0x006D1C68
 */
static map_backup* track_design_preview_backup_map()
{
    map_backup* backup = (map_backup*)malloc(sizeof(map_backup));
    if (backup != nullptr)
    {
        std::memcpy(backup->tile_elements, gTileElements, sizeof(backup->tile_elements));
        std::memcpy(backup->tile_pointers, gTileElementTilePointers, sizeof(backup->tile_pointers));
        backup->next_free_tile_element = gNextFreeTileElement;
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
static void track_design_preview_restore_map(map_backup* backup)
{
    std::memcpy(gTileElements, backup->tile_elements, sizeof(backup->tile_elements));
    std::memcpy(gTileElementTilePointers, backup->tile_pointers, sizeof(backup->tile_pointers));
    gNextFreeTileElement = backup->next_free_tile_element;
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

    for (int32_t i = 0; i < MAX_TILE_TILE_ELEMENT_POINTERS; i++)
    {
        TileElement* tile_element = &gTileElements[i];
        tile_element->ClearAs(TILE_ELEMENT_TYPE_SURFACE);
        tile_element->flags = TILE_ELEMENT_FLAG_LAST_TILE;
        tile_element->AsSurface()->SetSlope(TILE_ELEMENT_SLOPE_FLAT);
        tile_element->AsSurface()->SetWaterHeight(0);
        tile_element->AsSurface()->SetSurfaceStyle(TERRAIN_GRASS);
        tile_element->AsSurface()->SetEdgeStyle(TERRAIN_EDGE_ROCK);
        tile_element->AsSurface()->SetGrassLength(GRASS_LENGTH_CLEAR_0);
        tile_element->AsSurface()->SetOwnership(OWNERSHIP_OWNED);
        tile_element->AsSurface()->SetParkFences(0);
    }
    map_update_tile_pointers();
}

bool track_design_are_entrance_and_exit_placed()
{
    return _trackDesignPlaceStateEntranceExitPlaced;
}

#pragma endregion
