/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
#include "../TrackImporter.h"
#include "../actions/FootpathPlaceFromTrackAction.hpp"
#include "../actions/FootpathRemoveAction.hpp"
#include "../actions/LargeSceneryPlaceAction.hpp"
#include "../actions/LargeSceneryRemoveAction.hpp"
#include "../actions/MazePlaceTrackAction.hpp"
#include "../actions/RideEntranceExitPlaceAction.hpp"
#include "../actions/RideSetSetting.hpp"
#include "../actions/RideSetVehiclesAction.hpp"
#include "../actions/SmallSceneryPlaceAction.hpp"
#include "../actions/SmallSceneryRemoveAction.hpp"
#include "../actions/TrackPlaceAction.hpp"
#include "../actions/TrackRemoveAction.hpp"
#include "../actions/WallPlaceAction.hpp"
#include "../actions/WallRemoveAction.hpp"
#include "../audio/audio.h"
#include "../core/File.h"
#include "../core/String.hpp"
#include "../drawing/X8DrawingEngine.h"
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

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

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

TrackDesign* gActiveTrackDesign;
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

static map_backup* track_design_preview_backup_map();

static void track_design_preview_restore_map(map_backup* backup);

static void track_design_preview_clear_map();

rct_string_id TrackDesign::CreateTrackDesign(const Ride& ride)
{
    type = ride.type;
    auto object = object_entry_get_entry(OBJECT_TYPE_RIDE, ride.subtype);

    // Note we are only copying rct_object_entry in size and
    // not the extended as we don't need the chunk size.
    std::memcpy(&vehicle_object, object, sizeof(rct_object_entry));

    ride_mode = ride.mode;
    colour_scheme = ride.colour_scheme_type & 3;

    for (int32_t i = 0; i < RCT12_MAX_VEHICLES_PER_RIDE; i++)
    {
        vehicle_colours[i].body_colour = ride.vehicle_colours[i].Body;
        vehicle_colours[i].trim_colour = ride.vehicle_colours[i].Trim;
        vehicle_additional_colour[i] = ride.vehicle_colours[i].Ternary;
    }

    for (int32_t i = 0; i < RCT12_NUM_COLOUR_SCHEMES; i++)
    {
        track_spine_colour[i] = ride.track_colour[i].main;
        track_rail_colour[i] = ride.track_colour[i].additional;
        track_support_colour[i] = ride.track_colour[i].supports;
    }

    depart_flags = ride.depart_flags;
    number_of_trains = ride.num_vehicles;
    number_of_cars_per_train = ride.num_cars_per_train;
    min_waiting_time = ride.min_waiting_time;
    max_waiting_time = ride.max_waiting_time;
    operation_setting = ride.operation_option;
    lift_hill_speed = ride.lift_hill_speed;
    num_circuits = ride.num_circuits;

    entrance_style = ride.entrance_style;
    max_speed = (int8_t)(ride.max_speed / 65536);
    average_speed = (int8_t)(ride.average_speed / 65536);
    ride_length = ride_get_total_length(&ride) / 65536;
    max_positive_vertical_g = ride.max_positive_vertical_g / 32;
    max_negative_vertical_g = ride.max_negative_vertical_g / 32;
    max_lateral_g = ride.max_lateral_g / 32;
    inversions = ride.holes & 0x1F;
    inversions = ride.inversions & 0x1F;
    inversions |= (ride.sheltered_eighths << 5);
    drops = ride.drops;
    highest_drop_height = ride.highest_drop_height;

    uint16_t totalAirTime = (ride.total_air_time * 123) / 1024;
    if (totalAirTime > 255)
    {
        totalAirTime = 0;
    }
    total_air_time = (uint8_t)totalAirTime;

    excitement = ride.ratings.excitement / 10;
    intensity = ride.ratings.intensity / 10;
    nausea = ride.ratings.nausea / 10;

    upkeep_cost = ride.upkeep_cost;
    flags = 0;
    flags2 = 0;

    if (type == RIDE_TYPE_MAZE)
    {
        return CreateTrackDesignMaze(ride);
    }
    else
    {
        return CreateTrackDesignTrack(ride);
    }
}

rct_string_id TrackDesign::CreateTrackDesignTrack(const Ride& ride)
{
    CoordsXYE trackElement;
    if (!ride_try_get_origin_element(&ride, &trackElement))
    {
        return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
    }

    ride_get_start_of_track(&trackElement);

    int32_t z = trackElement.element->base_height * 8;
    uint8_t trackType = trackElement.element->AsTrack()->GetTrackType();
    uint8_t direction = trackElement.element->GetDirection();
    _saveDirection = direction;

    if (sub_6C683D(&trackElement.x, &trackElement.y, &z, direction, trackType, 0, &trackElement.element, 0))
    {
        return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
    }

    const rct_track_coordinates* trackCoordinates = &TrackCoordinates[trackElement.element->AsTrack()->GetTrackType()];
    // Used in the following loop to know when we have
    // completed all of the elements and are back at the
    // start.
    TileElement* initialMap = trackElement.element;

    int16_t start_x = trackElement.x;
    int16_t start_y = trackElement.y;
    int16_t start_z = z + trackCoordinates->z_begin;
    gTrackPreviewOrigin = { start_x, start_y, start_z };

    do
    {
        TrackDesignTrackElement track{};
        track.type = trackElement.element->AsTrack()->GetTrackType();
        // TODO move to RCT2 limit
        if (track.type == TRACK_ELEM_MULTIDIM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP)
        {
            track.type = TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP_ALIAS;
        }

        uint8_t trackFlags;
        if (track_element_has_speed_setting(track.type))
        {
            trackFlags = trackElement.element->AsTrack()->GetBrakeBoosterSpeed() >> 1;
        }
        else
        {
            trackFlags = trackElement.element->AsTrack()->GetSeatRotation();
        }

        if (trackElement.element->AsTrack()->HasChain())
            trackFlags |= RCT12_TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
        trackFlags |= trackElement.element->AsTrack()->GetColourScheme() << 4;
        if (RideData4[ride.type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE
            && trackElement.element->AsTrack()->IsInverted())
        {
            trackFlags |= TD6_TRACK_ELEMENT_FLAG_INVERTED;
        }

        track.flags = trackFlags;
        track_elements.push_back(track);

        if (!track_block_get_next(&trackElement, &trackElement, nullptr, nullptr))
        {
            break;
        }

        z = trackElement.element->base_height * 8;
        direction = trackElement.element->GetDirection();
        trackType = trackElement.element->AsTrack()->GetTrackType();

        if (sub_6C683D(&trackElement.x, &trackElement.y, &z, direction, trackType, 0, &trackElement.element, 0))
        {
            break;
        }

        // TODO move to RCT2 limit
        constexpr auto TD6MaxTrackElements = 8192;

        if (track_elements.size() > TD6MaxTrackElements)
        {
            return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
        }
    } while (trackElement.element != initialMap);

    // First entrances, second exits
    for (int32_t i = 0; i < 2; i++)
    {
        for (int32_t station_index = 0; station_index < RCT12_MAX_STATIONS_PER_RIDE; station_index++)
        {
            z = ride.stations[station_index].Height;

            TileCoordsXYZD location;
            if (i == 0)
            {
                location = ride_get_entrance_location(&ride, station_index);
            }
            else
            {
                location = ride_get_exit_location(&ride, station_index);
            }

            if (location.isNull())
            {
                continue;
            }

            int16_t x = location.x * 32;
            int16_t y = location.y * 32;

            TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
            if (tileElement == nullptr)
                continue;

            do
            {
                if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                    continue;
                if (tileElement->base_height == z)
                    break;
            } while (!(tileElement++)->IsLastForTile());

            // Add something that stops this from walking off the end

            Direction entranceDirection = tileElement->GetDirection();
            entranceDirection -= _saveDirection;
            entranceDirection &= TILE_ELEMENT_DIRECTION_MASK;

            TrackDesignEntranceElement entrance{};
            entrance.direction = entranceDirection;

            x -= gTrackPreviewOrigin.x;
            y -= gTrackPreviewOrigin.y;

            // Rotate entrance coordinates backwards to the correct direction
            rotate_map_coordinates(&x, &y, (0 - _saveDirection) & 3);
            entrance.x = x;
            entrance.y = y;

            z *= 8;
            z -= gTrackPreviewOrigin.z;
            z /= 8;

            if (z > 127 || z < -126)
            {
                return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
            }

            entrance.z = z;

            // If this is the exit version
            if (i == 1)
            {
                entrance.isExit = true;
            }
            entrance_elements.push_back(entrance);
        }
    }

    place_virtual_track(this, PTD_OPERATION_DRAW_OUTLINES, true, GetOrAllocateRide(0), 4096, 4096, 0);

    // Resave global vars for scenery reasons.
    gTrackPreviewOrigin = { start_x, start_y, start_z };

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;

    space_required_x = ((gTrackPreviewMax.x - gTrackPreviewMin.x) / 32) + 1;
    space_required_y = ((gTrackPreviewMax.y - gTrackPreviewMin.y) / 32) + 1;
    return STR_NONE;
}

rct_string_id TrackDesign::CreateTrackDesignMaze(const Ride& ride)
{
    auto startLoc = MazeGetFirstElement(ride);

    if (startLoc.element == nullptr)
    {
        return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
    }

    gTrackPreviewOrigin = { static_cast<int16_t>(startLoc.x), static_cast<int16_t>(startLoc.y),
                            (int16_t)(startLoc.element->base_height * 8) };

    // x is defined here as we can start the search
    // on tile start_x, start_y but then the next row
    // must restart on 0
    for (int16_t y = startLoc.y, x = startLoc.y; y < 8192; y += 32)
    {
        for (; x < 8192; x += 32)
        {
            auto tileElement = map_get_first_element_at(x / 32, y / 32);
            do
            {
                if (tileElement == nullptr)
                    break;
                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;
                if (tileElement->AsTrack()->GetRideIndex() != ride.id)
                    continue;

                TrackDesignMazeElement maze{};

                maze.maze_entry = tileElement->AsTrack()->GetMazeEntry();
                maze.x = (x - startLoc.x) / 32;
                maze.y = (y - startLoc.y) / 32;
                _saveDirection = tileElement->GetDirection();
                maze_elements.push_back(maze);

                if (maze_elements.size() >= 2000)
                {
                    return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
                }
            } while (!(tileElement++)->IsLastForTile());
        }
        x = 0;
    }

    auto location = ride_get_entrance_location(&ride, 0);
    if (location.isNull())
    {
        return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
    }

    CoordsXY entranceLoc = { location.x * 32, location.y * 32 };
    auto tileElement = map_get_first_element_at(location.x, location.y);
    do
    {
        if (tileElement == nullptr)
            return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
            continue;
        if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE)
            continue;
        if (tileElement->AsEntrance()->GetRideIndex() == ride.id)
            break;
    } while (!(tileElement++)->IsLastForTile());
    // Add something that stops this from walking off the end

    uint8_t entranceDirection = tileElement->GetDirection();
    TrackDesignMazeElement mazeEntrance{};
    mazeEntrance.direction = entranceDirection;
    mazeEntrance.type = 8;
    mazeEntrance.x = (int8_t)((entranceLoc.x - startLoc.x) / 32);
    mazeEntrance.y = (int8_t)((entranceLoc.y - startLoc.y) / 32);
    maze_elements.push_back(mazeEntrance);

    location = ride_get_exit_location(&ride, 0);
    if (location.isNull())
    {
        return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
    }

    CoordsXY exitLoc = { location.x * 32, location.y * 32 };
    tileElement = map_get_first_element_at(location.x, location.y);
    if (tileElement == nullptr)
        return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
            continue;
        if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_EXIT)
            continue;
        if (tileElement->AsEntrance()->GetRideIndex() == ride.id)
            break;
    } while (!(tileElement++)->IsLastForTile());
    // Add something that stops this from walking off the end

    uint8_t exit_direction = tileElement->GetDirection();
    TrackDesignMazeElement mazeExit{};
    mazeExit.direction = exit_direction;
    mazeExit.type = 0x80;
    mazeExit.x = (int8_t)((exitLoc.x - startLoc.x) / 32);
    mazeExit.y = (int8_t)((exitLoc.y - startLoc.y) / 32);
    maze_elements.push_back(mazeExit);

    // Save global vars as they are still used by scenery????
    int16_t startZ = gTrackPreviewOrigin.z;
    place_virtual_track(this, PTD_OPERATION_DRAW_OUTLINES, true, GetOrAllocateRide(0), 4096, 4096, 0);
    gTrackPreviewOrigin = { static_cast<int16_t>(startLoc.x), static_cast<int16_t>(startLoc.y), startZ };

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;

    space_required_x = ((gTrackPreviewMax.x - gTrackPreviewMin.x) / 32) + 1;
    space_required_y = ((gTrackPreviewMax.y - gTrackPreviewMin.y) / 32) + 1;
    return STR_NONE;
}

CoordsXYE TrackDesign::MazeGetFirstElement(const Ride& ride)
{
    CoordsXYE tile{};
    for (tile.y = 0; tile.y < 8192; tile.y += 32)
    {
        for (tile.x = 0; tile.x < 8192; tile.x += 32)
        {
            tile.element = map_get_first_element_at(tile.x / 32, tile.y / 32);
            do
            {
                if (tile.element == nullptr)
                    break;

                if (tile.element->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;
                if (tile.element->AsTrack()->GetRideIndex() == ride.id)
                {
                    return tile;
                }
            } while (!(tile.element++)->IsLastForTile());
        }
    }
    tile.element = nullptr;
    return tile;
}

rct_string_id TrackDesign::CreateTrackDesignScenery()
{
    scenery_elements = _trackSavedTileElementsDesc;
    // Run an element loop
    for (auto& scenery : scenery_elements)
    {
        switch (object_entry_get_type(&scenery.scenery_object))
        {
            case OBJECT_TYPE_PATHS:
            {
                uint8_t slope = (scenery.flags & 0x60) >> 5;
                slope -= _saveDirection;

                scenery.flags &= 0x9F;
                scenery.flags |= ((slope & 3) << 5);

                // Direction of connection on path
                uint8_t direction = scenery.flags & 0xF;
                // Rotate the direction by the track direction
                direction = ((direction << 4) >> _saveDirection);

                scenery.flags &= 0xF0;
                scenery.flags |= (direction & 0xF) | (direction >> 4);
                break;
            }
            case OBJECT_TYPE_WALLS:
            {
                uint8_t direction = scenery.flags & 3;
                direction -= _saveDirection;

                scenery.flags &= 0xFC;
                scenery.flags |= (direction & 3);
                break;
            }
            default:
            {
                uint8_t direction = scenery.flags & 3;
                uint8_t quadrant = (scenery.flags & 0x0C) >> 2;

                direction -= _saveDirection;
                quadrant -= _saveDirection;

                scenery.flags &= 0xF0;
                scenery.flags |= (direction & 3) | ((quadrant & 3) << 2);
                break;
            }
        }

        int16_t x = ((uint8_t)scenery.x) * 32 - gTrackPreviewOrigin.x;
        int16_t y = ((uint8_t)scenery.y) * 32 - gTrackPreviewOrigin.y;
        rotate_map_coordinates(&x, &y, (0 - _saveDirection) & 3);
        x /= 32;
        y /= 32;

        if (x > 127 || y > 127 || x < -126 || y < -126)
        {
            return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
        }

        scenery.x = (int8_t)x;
        scenery.y = (int8_t)y;

        int32_t z = scenery.z * 8 - gTrackPreviewOrigin.z;
        z /= 8;
        if (z > 127 || z < -126)
        {
            return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
        }
        scenery.z = z;
    }

    return STR_NONE;
}

std::unique_ptr<TrackDesign> track_design_open(const utf8* path)
{
    try
    {
        auto trackImporter = TrackImporter::Create(path);
        trackImporter->Load(path);
        return trackImporter->Import();
    }
    catch (const std::exception& e)
    {
        log_error("Unable to load track design: %s", e.what());
    }
    log_verbose("track_design_open(\"%s\")", path);
    return nullptr;
}

/**
 *
 *  rct2: 0x006ABDB0
 */
static void track_design_load_scenery_objects(TrackDesign* td6)
{
    object_manager_unload_all_objects();

    // Load ride object
    rct_object_entry* rideEntry = &td6->vehicle_object;
    object_manager_load_object(rideEntry);

    // Load scenery objects
    for (const auto& scenery : td6->scenery_elements)
    {
        const rct_object_entry* sceneryEntry = &scenery.scenery_object;
        object_manager_load_object(sceneryEntry);
    }
}

/**
 *
 *  rct2: 0x006D247A
 */
static void track_design_mirror_scenery(TrackDesign* td6)
{
    for (auto& scenery : td6->scenery_elements)
    {
        uint8_t entry_type{ 0 }, entry_index{ 0 };
        if (!find_object_in_entry_group(&scenery.scenery_object, &entry_type, &entry_index))
        {
            entry_type = object_entry_get_type(&scenery.scenery_object);
            if (entry_type != OBJECT_TYPE_PATHS)
            {
                continue;
            }

            entry_index = 0;
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
                    if (y2 < tile->y_offset)
                    {
                        y2 = tile->y_offset;
                    }
                }

                switch (scenery.flags & 3)
                {
                    case 0:
                        scenery.y = (-(scenery.y * 32 + y1) - y2) / 32;
                        break;
                    case 1:
                        scenery.x = (scenery.x * 32 + y2 + y1) / 32;
                        scenery.y = (-(scenery.y * 32)) / 32;
                        scenery.flags ^= (1 << 1);
                        break;
                    case 2:
                        scenery.y = (-(scenery.y * 32 - y2) + y1) / 32;
                        break;
                    case 3:
                        scenery.x = (scenery.x * 32 - y2 - y1) / 32;
                        scenery.y = (-(scenery.y * 32)) / 32;
                        scenery.flags ^= (1 << 1);
                        break;
                }
                break;
            }
            case OBJECT_TYPE_SMALL_SCENERY:
                scenery.y = -scenery.y;

                if (scenery_small_entry_has_flag(scenery_entry, SMALL_SCENERY_FLAG_DIAGONAL))
                {
                    scenery.flags ^= (1 << 0);
                    if (!scenery_small_entry_has_flag(scenery_entry, SMALL_SCENERY_FLAG_FULL_TILE))
                    {
                        scenery.flags ^= (1 << 2);
                    }
                    break;
                }
                if (scenery.flags & (1 << 0))
                {
                    scenery.flags ^= (1 << 1);
                }

                scenery.flags ^= (1 << 2);
                break;

            case OBJECT_TYPE_WALLS:
                scenery.y = -scenery.y;
                if (scenery.flags & (1 << 0))
                {
                    scenery.flags ^= (1 << 1);
                }
                break;

            case OBJECT_TYPE_PATHS:
                scenery.y = -scenery.y;

                if (scenery.flags & (1 << 5))
                {
                    scenery.flags ^= (1 << 6);
                }

                uint8_t flags = scenery.flags;
                flags = ((flags & (1 << 3)) >> 2) | ((flags & (1 << 1)) << 2);
                scenery.flags &= 0xF5;
                scenery.flags |= flags;
        }
    }
}

/**
 *
 *  rct2: 0x006D2443
 */
static void track_design_mirror_ride(TrackDesign* td6)
{
    for (auto& track : td6->track_elements)
    {
        track.type = TrackElementMirrorMap[track.type];
    }

    for (auto& entrance : td6->entrance_elements)
    {
        entrance.y = -entrance.y;
        if (entrance.direction & 1)
        {
            entrance.direction = direction_reverse(entrance.direction);
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
static void track_design_mirror_maze(TrackDesign* td6)
{
    for (auto& maze : td6->maze_elements)
    {
        maze.y = -maze.y;

        if (maze.type == 0x8 || maze.type == 0x80)
        {
            if (maze.direction & 1)
            {
                maze.direction = direction_reverse(maze.direction);
            }
            continue;
        }

        uint16_t maze_entry = maze.maze_entry;
        uint16_t new_entry = 0;
        for (uint8_t position = bitscanforward(maze_entry); position != 0xFF; position = bitscanforward(maze_entry))
        {
            maze_entry &= ~(1 << position);
            new_entry |= (1 << maze_segment_mirror_map[position]);
        }
        maze.maze_entry = new_entry;
    }
}

/**
 *
 *  rct2: 0x006D2436
 */
void track_design_mirror(TrackDesign* td6)
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
    for (const auto& tile : gMapSelectionTiles)
    {
        if (tile.x == x && tile.y == y)
        {
            return;
        }
    }
    gMapSelectionTiles.push_back(CoordsXY(x, y));
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

static bool TrackDesignPlaceSceneryElementGetEntry(
    uint8_t& entry_type, uint8_t& entry_index, const TrackDesignSceneryElement& scenery)
{
    if (!find_object_in_entry_group(&scenery.scenery_object, &entry_type, &entry_index))
    {
        entry_type = object_entry_get_type(&scenery.scenery_object);
        if (entry_type != OBJECT_TYPE_PATHS)
        {
            _trackDesignPlaceStateSceneryUnavailable = true;
            return true;
        }

        if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
        {
            _trackDesignPlaceStateSceneryUnavailable = true;
            return true;
        }

        entry_index = 0;
        for (PathSurfaceEntry* path = get_path_surface_entry(0); entry_index < object_entry_group_counts[OBJECT_TYPE_PATHS];
             path = get_path_surface_entry(entry_index), entry_index++)
        {
            if (path == nullptr)
            {
                return true;
            }
            if (path->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR)
            {
                return true;
            }
        }

        if (entry_index == object_entry_group_counts[OBJECT_TYPE_PATHS])
        {
            _trackDesignPlaceStateSceneryUnavailable = true;
            return true;
        }
    }
    return false;
}

static bool TrackDesignPlaceSceneryElementRemoveGhost(
    CoordsXY mapCoord, const TrackDesignSceneryElement& scenery, uint8_t rotation, int32_t originZ)
{
    uint8_t entry_type, entry_index;
    if (TrackDesignPlaceSceneryElementGetEntry(entry_type, entry_index, scenery))
    {
        return true;
    }

    if (_trackDesignPlaceStateSceneryUnavailable)
    {
        return true;
    }

    int32_t z = (scenery.z * 8 + originZ) / 8;
    uint8_t sceneryRotation = (rotation + scenery.flags) & TILE_ELEMENT_DIRECTION_MASK;
    const uint32_t flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
        | GAME_COMMAND_FLAG_GHOST;
    std::unique_ptr<GameAction> ga;
    switch (entry_type)
    {
        case OBJECT_TYPE_SMALL_SCENERY:
        {
            uint8_t quadrant = (scenery.flags >> 2) + _currentTrackPieceDirection;
            quadrant &= 3;

            rct_scenery_entry* small_scenery = get_small_scenery_entry(entry_index);
            if (!(!scenery_small_entry_has_flag(small_scenery, SMALL_SCENERY_FLAG_FULL_TILE)
                  && scenery_small_entry_has_flag(small_scenery, SMALL_SCENERY_FLAG_DIAGONAL))
                && scenery_small_entry_has_flag(
                    small_scenery,
                    SMALL_SCENERY_FLAG_DIAGONAL | SMALL_SCENERY_FLAG_HALF_SPACE | SMALL_SCENERY_FLAG_THREE_QUARTERS))
            {
                quadrant = 0;
            }

            ga = std::make_unique<SmallSceneryRemoveAction>(CoordsXYZ{ mapCoord.x, mapCoord.y, z * 8 }, quadrant, entry_index);
            break;
        }
        case OBJECT_TYPE_LARGE_SCENERY:
            ga = std::make_unique<LargeSceneryRemoveAction>(CoordsXYZD{ mapCoord.x, mapCoord.y, z * 8, sceneryRotation }, 0);
            break;
        case OBJECT_TYPE_WALLS:
            ga = std::make_unique<WallRemoveAction>(CoordsXYZD{ mapCoord.x, mapCoord.y, z * 8, sceneryRotation });
            break;
        case OBJECT_TYPE_PATHS:
            ga = std::make_unique<FootpathRemoveAction>(CoordsXYZ{ mapCoord.x, mapCoord.y, z * 8 });
            break;
        default:
            return true;
    }
    ga->SetFlags(flags);
    GameActions::ExecuteNested(ga.get());
    return true;
}

static bool TrackDesignPlaceSceneryElementGetPlaceZ(const TrackDesignSceneryElement& scenery)
{
    int32_t z = scenery.z * 8 + _trackDesignPlaceZ;
    if (z < _trackDesignPlaceSceneryZ)
    {
        _trackDesignPlaceSceneryZ = z;
    }

    uint8_t entry_type, entry_index;
    TrackDesignPlaceSceneryElementGetEntry(entry_type, entry_index, scenery);

    return true;
}

static bool TrackDesignPlaceSceneryElement(
    CoordsXY mapCoord, uint8_t mode, const TrackDesignSceneryElement& scenery, uint8_t rotation, int32_t originZ)
{
    if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES && mode == 0)
    {
        track_design_add_selection_tile(mapCoord.x, mapCoord.y);
        return true;
    }

    if (_trackDesignPlaceOperation == PTD_OPERATION_REMOVE_GHOST && mode == 0)
    {
        return TrackDesignPlaceSceneryElementRemoveGhost(mapCoord, scenery, rotation, originZ);
    }

    if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z)
    {
        return TrackDesignPlaceSceneryElementGetPlaceZ(scenery);
    }

    if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_QUERY || _trackDesignPlaceOperation == PTD_OPERATION_PLACE
        || _trackDesignPlaceOperation == PTD_OPERATION_PLACE_GHOST
        || _trackDesignPlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
    {
        uint8_t entry_type, entry_index;
        if (TrackDesignPlaceSceneryElementGetEntry(entry_type, entry_index, scenery))
        {
            return true;
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
                    return true;
                }
                if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z)
                {
                    return true;
                }

                rotation += scenery.flags;
                rotation &= 3;
                z = scenery.z * 8 + originZ;
                quadrant = ((scenery.flags >> 2) + _currentTrackPieceDirection) & 3;

                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY;
                if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                {
                    flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                        | GAME_COMMAND_FLAG_NO_SPEND;
                }
                else if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_GHOST)
                {
                    flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                        | GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_NO_SPEND;
                }
                else if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_QUERY)
                {
                    flags = GAME_COMMAND_FLAG_PATH_SCENERY;
                }

                gGameCommandErrorTitle = STR_CANT_POSITION_THIS_HERE;

                auto smallSceneryPlace = SmallSceneryPlaceAction(
                    { mapCoord.x, mapCoord.y, z, rotation }, quadrant, entry_index, scenery.primary_colour,
                    scenery.secondary_colour);

                smallSceneryPlace.SetFlags(flags);
                auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&smallSceneryPlace)
                                                           : GameActions::QueryNested(&smallSceneryPlace);

                cost = res->Error == GA_ERROR::OK ? res->Cost : 0;
                break;
            }
            case OBJECT_TYPE_LARGE_SCENERY:
            {
                if (mode != 0)
                {
                    return true;
                }
                if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z)
                {
                    return true;
                }

                rotation += scenery.flags;
                rotation &= 3;

                z = scenery.z * 8 + originZ;

                flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY;
                if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                {
                    flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                        | GAME_COMMAND_FLAG_NO_SPEND;
                }
                else if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_GHOST)
                {
                    flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                        | GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_NO_SPEND;
                }
                else if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_QUERY)
                {
                    flags = GAME_COMMAND_FLAG_PATH_SCENERY;
                }

                auto sceneryPlaceAction = LargeSceneryPlaceAction(
                    { mapCoord.x, mapCoord.y, z, rotation }, entry_index, scenery.primary_colour, scenery.secondary_colour);
                sceneryPlaceAction.SetFlags(flags);
                auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::Execute(&sceneryPlaceAction)
                                                           : GameActions::Query(&sceneryPlaceAction);

                cost = res->Cost;
                break;
            }
            case OBJECT_TYPE_WALLS:
            {
                if (mode != 0)
                {
                    return true;
                }
                if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z)
                {
                    return true;
                }

                z = scenery.z * 8 + originZ;
                rotation += scenery.flags;
                rotation &= 3;

                flags = GAME_COMMAND_FLAG_APPLY;
                if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                {
                    flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_PATH_SCENERY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                        | GAME_COMMAND_FLAG_NO_SPEND;
                }
                else if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_GHOST)
                {
                    flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
                        | GAME_COMMAND_FLAG_GHOST;
                }
                else if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_QUERY)
                {
                    flags = 0;
                }

                auto wallPlaceAction = WallPlaceAction(
                    entry_index, { mapCoord.x, mapCoord.y, z }, rotation, scenery.primary_colour, scenery.secondary_colour,
                    (scenery.flags & 0xFC) >> 2);
                wallPlaceAction.SetFlags(flags);
                auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&wallPlaceAction)
                                                           : GameActions::QueryNested(&wallPlaceAction);

                cost = res->Cost;
                break;
            }
            case OBJECT_TYPE_PATHS:
                if (_trackDesignPlaceOperation == PTD_OPERATION_GET_PLACE_Z)
                {
                    return true;
                }

                z = (scenery.z * 8 + originZ) / 8;
                if (mode == 0)
                {
                    if (scenery.flags & (1 << 7))
                    {
                        // dh
                        entry_index |= (1 << 7);
                    }

                    uint8_t bh = ((scenery.flags & 0xF) << rotation);
                    flags = bh >> 4;
                    bh = (bh | flags) & 0xF;
                    flags = (((scenery.flags >> 5) + rotation) & 3) << 5;
                    bh |= flags;

                    bh |= scenery.flags & 0x90;

                    flags = GAME_COMMAND_FLAG_APPLY;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                    {
                        flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND;
                    }
                    if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_GHOST)
                    {
                        flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
                            | GAME_COMMAND_FLAG_GHOST;
                    }
                    if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_QUERY)
                    {
                        flags = 0;
                    }

                    uint8_t slope = ((bh >> 5) & 0x3) | ((bh >> 2) & 0x4);
                    uint8_t edges = bh & 0xF;
                    auto footpathPlaceAction = FootpathPlaceFromTrackAction(
                        { mapCoord.x, mapCoord.y, z * 8 }, slope, entry_index, edges);
                    footpathPlaceAction.SetFlags(flags);
                    auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&footpathPlaceAction)
                                                               : GameActions::QueryNested(&footpathPlaceAction);
                    // Ignore failures
                    cost = res->Error == GA_ERROR::OK ? res->Cost : 0;
                }
                else
                {
                    if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_QUERY)
                    {
                        return true;
                    }

                    auto* pathElement = map_get_path_element_at({ mapCoord.x / 32, mapCoord.y / 32, z });

                    if (pathElement == nullptr)
                    {
                        return true;
                    }

                    footpath_queue_chain_reset();
                    footpath_remove_edges_at(mapCoord.x, mapCoord.y, reinterpret_cast<TileElement*>(pathElement));

                    flags = GAME_COMMAND_FLAG_APPLY;
                    if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                    {
                        flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND;
                    }
                    if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_GHOST)
                    {
                        flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
                            | GAME_COMMAND_FLAG_GHOST;
                    }

                    footpath_connect_edges(mapCoord.x, mapCoord.y, reinterpret_cast<TileElement*>(pathElement), flags);
                    footpath_update_queue_chains();
                    return true;
                }
                break;
            default:
                _trackDesignPlaceStateSceneryUnavailable = true;
                return true;
        }
        _trackDesignPlaceCost = add_clamp_money32(_trackDesignPlaceCost, cost);
        if (_trackDesignPlaceOperation != PTD_OPERATION_PLACE)
        {
            if (cost == MONEY32_UNDEFINED)
            {
                _trackDesignPlaceCost = MONEY32_UNDEFINED;
            }
        }
        if (_trackDesignPlaceCost != MONEY32_UNDEFINED)
        {
            return true;
        }
        if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE)
        {
            return true;
        }
        return false;
    }
    return true;
}

/**
 *
 *  rct2: 0x006D0964
 */
static int32_t track_design_place_all_scenery(
    const std::vector<TrackDesignSceneryElement>& sceneryList, int32_t originX, int32_t originY, int32_t originZ)
{
    for (uint8_t mode = 0; mode <= 1; mode++)
    {
        if (!sceneryList.empty())
        {
            _trackDesignPlaceStateHasScenery = true;
        }

        if (!_trackDesignPlaceStatePlaceScenery)
        {
            continue;
        }

        for (const auto& scenery : sceneryList)
        {
            uint8_t rotation = _currentTrackPieceDirection;
            TileCoordsXY tileCoords = { originX / 32, originY / 32 };
            TileCoordsXY offsets = { scenery.x, scenery.y };
            tileCoords += offsets.Rotate(rotation);

            CoordsXY mapCoord = { tileCoords.x * 32, tileCoords.y * 32 };
            track_design_update_max_min_coordinates(mapCoord.x, mapCoord.y, originZ);

            if (!TrackDesignPlaceSceneryElement(mapCoord, mode, scenery, rotation, originZ))
            {
                return 0;
            }
        }
    }
    return 1;
}

static int32_t track_design_place_maze(TrackDesign* td6, int16_t x, int16_t y, int16_t z, Ride* ride)
{
    if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES)
    {
        gMapSelectionTiles.clear();
        gMapSelectArrowPosition.x = x;
        gMapSelectArrowPosition.y = y;
        gMapSelectArrowPosition.z = tile_element_height({ x, y });
        gMapSelectArrowDirection = _currentTrackPieceDirection;
    }

    _trackDesignPlaceZ = 0;
    _trackDesignPlaceCost = 0;

    for (const auto& maze_element : td6->maze_elements)
    {
        uint8_t rotation = _currentTrackPieceDirection & 3;
        int16_t tmpX = maze_element.x * 32;
        int16_t tmpY = maze_element.y * 32;
        rotate_map_coordinates(&tmpX, &tmpY, rotation);
        CoordsXY mapCoord = { tmpX, tmpY };
        mapCoord.x += x;
        mapCoord.y += y;

        track_design_update_max_min_coordinates(mapCoord.x, mapCoord.y, z);

        if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES)
        {
            track_design_add_selection_tile(mapCoord.x, mapCoord.y);
        }

        if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_QUERY || _trackDesignPlaceOperation == PTD_OPERATION_PLACE
            || _trackDesignPlaceOperation == PTD_OPERATION_PLACE_GHOST
            || _trackDesignPlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
        {
            uint8_t flags;
            money32 cost = 0;
            uint16_t maze_entry;
            switch (maze_element.type)
            {
                case MAZE_ELEMENT_TYPE_ENTRANCE:
                    // entrance
                    rotation += maze_element.direction;
                    rotation &= 3;

                    flags = GAME_COMMAND_FLAG_APPLY;
                    gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

                    if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_QUERY)
                    {
                        auto res = RideEntranceExitPlaceAction::TrackPlaceQuery({ mapCoord.x, mapCoord.y, z }, false);
                        cost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
                    }
                    else
                    {
                        if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                                | GAME_COMMAND_FLAG_NO_SPEND;
                        }
                        else if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_GHOST)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
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
                    rotation += maze_element.direction;
                    rotation &= 3;

                    flags = GAME_COMMAND_FLAG_APPLY;
                    gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

                    if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_QUERY)
                    {
                        auto res = RideEntranceExitPlaceAction::TrackPlaceQuery({ mapCoord.x, mapCoord.y, z }, true);
                        cost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
                    }
                    else
                    {
                        if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                                | GAME_COMMAND_FLAG_NO_SPEND;
                        }
                        else if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_GHOST)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
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
                    maze_entry = rol16(maze_element.maze_entry, rotation * 4);

                    if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                    {
                        flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND;
                    }
                    else if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_GHOST)
                    {
                        flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
                            | GAME_COMMAND_FLAG_GHOST;
                    }
                    else if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_QUERY)
                    {
                        flags = 0;
                    }
                    else
                    {
                        flags = GAME_COMMAND_FLAG_APPLY;
                    }

                    gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;

                    auto mazePlace = MazePlaceTrackAction({ mapCoord, z }, ride->id, maze_entry);
                    mazePlace.SetFlags(flags);
                    auto res = flags & GAME_COMMAND_FLAG_APPLY ? GameActions::ExecuteNested(&mazePlace)
                                                               : GameActions::QueryNested(&mazePlace);
                    cost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;
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

            auto surfaceElement = map_get_surface_element_at(mapCoord);
            int16_t map_height = surfaceElement->base_height * 8;
            if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
            {
                map_height += 16;
                if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
                {
                    map_height += 16;
                }
            }

            if (surfaceElement->GetWaterHeight() > 0)
            {
                int16_t water_height = surfaceElement->GetWaterHeight();
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

    if (_trackDesignPlaceOperation == PTD_OPERATION_REMOVE_GHOST)
    {
        ride_action_modify(
            ride, RIDE_MODIFY_DEMOLISH,
            GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
                | GAME_COMMAND_FLAG_GHOST);
    }

    gTrackPreviewOrigin.x = x;
    gTrackPreviewOrigin.y = y;
    gTrackPreviewOrigin.z = z;
    return 1;
}

static bool track_design_place_ride(TrackDesign* td6, int16_t x, int16_t y, int16_t z, Ride* ride)
{
    const rct_preview_track** trackBlockArray = (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_TRACK)) ? TrackBlocks
                                                                                                          : FlatRideTrackBlocks;

    gTrackPreviewOrigin.x = x;
    gTrackPreviewOrigin.y = y;
    gTrackPreviewOrigin.z = z;
    if (_trackDesignPlaceOperation == PTD_OPERATION_DRAW_OUTLINES)
    {
        gMapSelectionTiles.clear();
        gMapSelectArrowPosition.x = x;
        gMapSelectArrowPosition.y = y;
        gMapSelectArrowPosition.z = tile_element_height({ x, y });
        gMapSelectArrowDirection = _currentTrackPieceDirection;
    }

    _trackDesignPlaceZ = 0;
    _trackDesignPlaceCost = 0;
    uint8_t rotation = _currentTrackPieceDirection;

    // Track elements
    for (const auto& track : td6->track_elements)
    {
        uint8_t trackType = track.type;
        if (trackType == TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP_ALIAS)
        {
            trackType = TRACK_ELEM_MULTIDIM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP;
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
            case PTD_OPERATION_REMOVE_GHOST:
            {
                const rct_track_coordinates* trackCoordinates = &TrackCoordinates[trackType];
                const rct_preview_track* trackBlock = trackBlockArray[trackType];
                int32_t tempZ = z - trackCoordinates->z_begin + trackBlock->z;
                auto trackRemoveAction = TrackRemoveAction(trackType, 0, { x, y, tempZ, static_cast<Direction>(rotation & 3) });
                trackRemoveAction.SetFlags(
                    GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
                GameActions::ExecuteNested(&trackRemoveAction);
                break;
            }
            case PTD_OPERATION_PLACE_QUERY:
            case PTD_OPERATION_PLACE:
            case PTD_OPERATION_PLACE_GHOST:
            case PTD_OPERATION_PLACE_TRACK_PREVIEW:
            {
                const rct_track_coordinates* trackCoordinates = &TrackCoordinates[trackType];

                // di
                int16_t tempZ = z - trackCoordinates->z_begin;
                uint32_t trackColour = (track.flags >> 4) & 0x3;
                uint32_t brakeSpeed = (track.flags & 0x0F) * 2;
                uint32_t seatRotation = track.flags & 0x0F;

                int32_t liftHillAndAlternativeState = 0;
                if (track.flags & RCT12_TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT)
                {
                    liftHillAndAlternativeState |= 1;
                }
                if (track.flags & TD6_TRACK_ELEMENT_FLAG_INVERTED)
                {
                    liftHillAndAlternativeState |= 2;
                }

                uint8_t flags = GAME_COMMAND_FLAG_APPLY;
                if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                {
                    flags |= GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED;
                    flags |= GAME_COMMAND_FLAG_NO_SPEND;
                }
                else if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_GHOST)
                {
                    flags |= GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED;
                    flags |= GAME_COMMAND_FLAG_NO_SPEND;
                    flags |= GAME_COMMAND_FLAG_GHOST;
                }
                else if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_QUERY)
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

                    auto surfaceElement = map_get_surface_element_at(tile);
                    if (surfaceElement == nullptr)
                    {
                        return false;
                    }

                    int32_t height = surfaceElement->base_height * 8;
                    if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
                    {
                        height += 16;
                        if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
                        {
                            height += 16;
                        }
                    }

                    uint8_t water_height = surfaceElement->GetWaterHeight() * 16;
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
    for (const auto& entrance : td6->entrance_elements)
    {
        rotation = _currentTrackPieceDirection & 3;
        x = entrance.x;
        y = entrance.y;
        rotate_map_coordinates(&x, &y, rotation);
        x += gTrackPreviewOrigin.x;
        y += gTrackPreviewOrigin.y;

        track_design_update_max_min_coordinates(x, y, z);

        switch (_trackDesignPlaceOperation)
        {
            case PTD_OPERATION_DRAW_OUTLINES:
                track_design_add_selection_tile(x, y);
                break;
            case PTD_OPERATION_PLACE_QUERY:
            case PTD_OPERATION_PLACE:
            case PTD_OPERATION_PLACE_GHOST:
            case PTD_OPERATION_PLACE_TRACK_PREVIEW:
            {
                rotation = (rotation + entrance.direction) & 3;
                if (_trackDesignPlaceOperation != PTD_OPERATION_PLACE_QUERY)
                {
                    LocationXY16 tile = {
                        (int16_t)(x + CoordsDirectionDelta[rotation].x),
                        (int16_t)(y + CoordsDirectionDelta[rotation].y),
                    };
                    TileElement* tile_element = map_get_first_element_at(tile.x >> 5, tile.y >> 5);
                    z = gTrackPreviewOrigin.z / 8;
                    z += entrance.z;
                    if (tile_element == nullptr)
                    {
                        _trackDesignPlaceCost = MONEY32_UNDEFINED;
                        return false;
                    }

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
                        if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_TRACK_PREVIEW)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED
                                | GAME_COMMAND_FLAG_NO_SPEND;
                        }
                        if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_GHOST)
                        {
                            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
                                | GAME_COMMAND_FLAG_GHOST;
                        }
                        if (_trackDesignPlaceOperation == PTD_OPERATION_PLACE_QUERY)
                        {
                            flags = 0;
                        }

                        gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
                        auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(
                            { x, y }, rotation, ride->id, stationIndex, entrance.isExit);
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
                    z = entrance.z * 8;
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

    if (_trackDesignPlaceOperation == PTD_OPERATION_REMOVE_GHOST)
    {
        sub_6CB945(ride);
        ride->Delete();
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
    TrackDesign* td6, uint8_t ptdOperation, bool placeScenery, Ride* ride, int16_t x, int16_t y, int16_t z)
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
    if (track_place_success)
    {
        if (!track_design_place_all_scenery(
                td6->scenery_elements, gTrackPreviewOrigin.x, gTrackPreviewOrigin.y, gTrackPreviewOrigin.z))
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
static bool track_design_place_preview(TrackDesign* td6, money32* cost, Ride** outRide, uint8_t* flags)
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
    uint8_t rideCreateFlags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND;
    if (ride_create_command(td6->type, entry_index, rideCreateFlags, &rideIndex, &colour) == MONEY32_UNDEFINED)
    {
        return false;
    }

    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return false;

    ride->custom_name = {};
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
    int32_t z = place_virtual_track(td6, PTD_OPERATION_GET_PLACE_Z, true, GetOrAllocateRide(0), mapSize, mapSize, 16);

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

    money32 resultCost = place_virtual_track(td6, PTD_OPERATION_PLACE_TRACK_PREVIEW, placeScenery, ride, mapSize, mapSize, z);
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
        ride->Delete();
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

    TrackDesign* td6 = gActiveTrackDesign;
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
    if (ride == nullptr)
    {
        log_warning("Invalid game command for track placement, ride id = %d", rideIndex);
        return MONEY32_UNDEFINED;
    }

    money32 cost = 0;
    if (!(flags & GAME_COMMAND_FLAG_APPLY))
    {
        _trackDesignDontPlaceScenery = false;
        cost = place_virtual_track(td6, PTD_OPERATION_PLACE_QUERY, true, ride, x, y, z);
        if (_trackDesignPlaceStateSceneryUnavailable)
        {
            _trackDesignDontPlaceScenery = true;
            cost = place_virtual_track(td6, PTD_OPERATION_PLACE_QUERY, false, ride, x, y, z);
        }
    }
    else
    {
        uint8_t operation;
        if (flags & GAME_COMMAND_FLAG_GHOST)
        {
            operation = PTD_OPERATION_PLACE_GHOST;
        }
        else
        {
            operation = PTD_OPERATION_PLACE;
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
    set_operating_setting_nested(ride->id, RideSetSetting::LiftHillSpeed, td6->lift_hill_speed & 0x1F, flags);

    uint8_t num_circuits = td6->num_circuits;
    if (num_circuits == 0)
    {
        num_circuits = 1;
    }
    set_operating_setting_nested(ride->id, RideSetSetting::NumCircuits, num_circuits, flags);
    ride->SetToDefaultInspectionInterval();
    ride->lifecycle_flags |= RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN;
    ride->colour_scheme_type = td6->colour_scheme;

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

    ride_set_name(ride, td6->name.c_str(), flags);

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
    *outRideIndex = ride->id;
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

#pragma region Track Design Preview

/**
 *
 *  rct2: 0x006D1EF0
 */
void track_design_draw_preview(TrackDesign* td6, uint8_t* pixels)
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

    auto drawingEngine = std::make_unique<X8DrawingEngine>(GetContext()->GetUiContext());
    dpi.DrawingEngine = drawingEngine.get();

    CoordsXY offset = { size_x / 2, size_y / 2 };
    for (uint8_t i = 0; i < 4; i++)
    {
        gCurrentRotation = i;

        auto screenCoords = translate_3d_to_2d_with_z(i, centre);
        screenCoords.x -= offset.x;
        screenCoords.y -= offset.y;

        int32_t left = screenCoords.x;
        int32_t top = screenCoords.y;
        int32_t right = left + size_x;
        int32_t bottom = top + size_y;

        view.view_x = left;
        view.view_y = top;
        viewport_paint(&view, &dpi, left, top, right, bottom);

        dpi.bits += TRACK_PREVIEW_IMAGE_SIZE;
    }

    ride->Delete();
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
        tile_element->SetLastForTile(true);
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
