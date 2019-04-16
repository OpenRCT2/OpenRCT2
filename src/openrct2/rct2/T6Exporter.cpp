/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "T6Exporter.h"

#include "../Context.h"
#include "../core/FileStream.hpp"
#include "../core/MemoryStream.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../object/ObjectList.h"
#include "../ride/Ride.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ride/TrackDesign.h"
#include "../ride/TrackDesignRepository.h"
#include "../rct12/SawyerChunkWriter.h"
#include "../windows/Intent.h"
#include <functional>

T6Exporter::T6Exporter()
{
}

bool T6Exporter::SaveTrack(const utf8* path)
{
    auto fs = FileStream(path, FILE_MODE_WRITE);
    return SaveTrack(&fs);
}

bool T6Exporter::SaveTrack(IStream* stream)
{
    MemoryStream tempStream;
    tempStream.WriteValue<uint8_t>(_trackDesign.type);
    tempStream.WriteValue<uint8_t>(_trackDesign.vehicle_type);
    tempStream.WriteValue<uint32_t>(_trackDesign.flags);
    tempStream.WriteValue<uint8_t>(_trackDesign.ride_mode);
    tempStream.WriteValue<uint8_t>((_trackDesign.colour_scheme & 0x3) | (2 << 2));
    tempStream.WriteArray(_trackDesign.vehicle_colours, RCT12_MAX_VEHICLE_COLOURS);
    tempStream.WriteValue<uint8_t>(0);
    tempStream.WriteValue<uint8_t>(_trackDesign.entrance_style);
    tempStream.WriteValue<uint8_t>(_trackDesign.total_air_time);
    tempStream.WriteValue<uint8_t>(_trackDesign.depart_flags);
    tempStream.WriteValue<uint8_t>(_trackDesign.number_of_trains);
    tempStream.WriteValue<uint8_t>(_trackDesign.number_of_cars_per_train);
    tempStream.WriteValue<uint8_t>(_trackDesign.min_waiting_time);
    tempStream.WriteValue<uint8_t>(_trackDesign.max_waiting_time);
    tempStream.WriteValue<uint8_t>(_trackDesign.operation_setting);
    tempStream.WriteValue<int8_t>(_trackDesign.max_speed);
    tempStream.WriteValue<int8_t>(_trackDesign.average_speed);
    tempStream.WriteValue<uint16_t>(_trackDesign.ride_length);
    tempStream.WriteValue<uint8_t>(_trackDesign.max_positive_vertical_g);
    tempStream.WriteValue<int8_t>(_trackDesign.max_negative_vertical_g);
    tempStream.WriteValue<uint8_t>(_trackDesign.max_lateral_g);
    tempStream.WriteValue<uint8_t>(_trackDesign.type == RIDE_TYPE_MINI_GOLF ? _trackDesign.holes : _trackDesign.inversions);
    tempStream.WriteValue<uint8_t>(_trackDesign.drops);
    tempStream.WriteValue<uint8_t>(_trackDesign.highest_drop_height);
    tempStream.WriteValue<uint8_t>(_trackDesign.excitement);
    tempStream.WriteValue<uint8_t>(_trackDesign.intensity);
    tempStream.WriteValue<uint8_t>(_trackDesign.nausea);
    tempStream.WriteValue<money16>(_trackDesign.upkeep_cost);
    tempStream.WriteArray(_trackDesign.track_spine_colour, RCT12_NUM_COLOUR_SCHEMES);
    tempStream.WriteArray(_trackDesign.track_rail_colour, RCT12_NUM_COLOUR_SCHEMES);
    tempStream.WriteArray(_trackDesign.track_support_colour, RCT12_NUM_COLOUR_SCHEMES);
    tempStream.WriteValue<uint32_t>(_trackDesign.flags2);
    tempStream.Write(&_trackDesign.vehicle_object, sizeof(rct_object_entry));
    tempStream.WriteValue<uint8_t>(_trackDesign.space_required_x);
    tempStream.WriteValue<uint8_t>(_trackDesign.space_required_y);
    tempStream.WriteArray(_trackDesign.vehicle_additional_colour, RCT2_MAX_CARS_PER_TRAIN);
    tempStream.WriteValue<uint8_t>(_trackDesign.lift_hill_speed | (_trackDesign.num_circuits << 5));

    if (_trackDesign.type == RIDE_TYPE_MAZE)
    {
        for (const auto &mazeElement : _trackDesign.maze_elements)
        {
            tempStream.WriteValue<uint32_t>(mazeElement.all);
        }

        tempStream.WriteValue<uint32_t>(0);
    }
    else
    {
        for (const auto &trackElement : _trackDesign.track_elements)
        {
            tempStream.WriteValue<uint8_t>(trackElement.type);
            tempStream.WriteValue<uint8_t>(trackElement.flags);
        }

        tempStream.WriteValue<uint8_t>(0xFF);

        for (const auto &entranceElement : _trackDesign.entrance_elements)
        {
            tempStream.WriteValue<uint8_t>(entranceElement.z);
            tempStream.WriteValue<uint8_t>(entranceElement.direction);
            tempStream.WriteValue<int16_t>(entranceElement.x);
            tempStream.WriteValue<int16_t>(entranceElement.y);
        }

        tempStream.WriteValue<uint8_t>(0xFF);
    }

    for (const auto &sceneryElement : _trackDesign.scenery_elements)
    {
        tempStream.Write(&sceneryElement.scenery_object, sizeof(rct_object_entry));
        tempStream.WriteValue<int8_t>(sceneryElement.x);
        tempStream.WriteValue<int8_t>(sceneryElement.y);
        tempStream.WriteValue<int8_t>(sceneryElement.z);
        tempStream.WriteValue<uint8_t>(sceneryElement.flags);
        tempStream.WriteValue<uint8_t>(sceneryElement.primary_colour);
        tempStream.WriteValue<uint8_t>(sceneryElement.secondary_colour);
    }

    tempStream.WriteValue<uint8_t>(0xFF);

    SawyerChunkWriter sawyerCoding(stream);
    sawyerCoding.WriteChunkTrack(tempStream.GetData(), tempStream.GetLength());
    return true;
}

bool T6Exporter::Save(ride_id_t rideId)
{
    Ride* ride = get_ride(rideId);

    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
    {
        context_show_error(STR_CANT_SAVE_TRACK_DESIGN, STR_NONE);
        return false;
    }

    if (!ride_has_ratings(ride))
    {
        context_show_error(STR_CANT_SAVE_TRACK_DESIGN, STR_NONE);
        return false;
    }

    auto errMessage = CreateTrackDesign(*ride);
    if (errMessage != STR_NONE)
    {
        context_show_error(STR_CANT_SAVE_TRACK_DESIGN, errMessage);
        return false;
    }

    if (gTrackDesignSaveMode)
    {
        if (!CreateTrackDesignScenery(*ride))
        {
            return false;
        }
    }

    utf8 track_name[256];
    format_string(track_name, sizeof(track_name), ride->name, &ride->name_arguments);

    auto intent = Intent(WC_LOADSAVE);
    intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_TRACK);
    intent.putExtra(INTENT_EXTRA_PATH, std::string{ track_name });
    intent.putExtra(INTENT_EXTRA_CALLBACK, []() {
        track_repository_scan();
        gfx_invalidate_screen();
    });

    context_open_intent(&intent);

    return true;
}

rct_string_id T6Exporter::CreateTrackDesign(Ride& ride)
{
    _trackDesign.type = ride.type;
    auto object = object_entry_get_entry(OBJECT_TYPE_RIDE, ride.subtype);

    // Note we are only copying rct_object_entry in size and
    // not the extended as we don't need the chunk size.
    std::memcpy(&_trackDesign.vehicle_object, object, sizeof(rct_object_entry));

    _trackDesign.ride_mode = ride.mode;
    _trackDesign.colour_scheme = ride.colour_scheme_type & 3;

    for (int32_t i = 0; i < RCT12_MAX_VEHICLES_PER_RIDE; i++)
    {
        _trackDesign.vehicle_colours[i].body_colour = ride.vehicle_colours[i].Body;
        _trackDesign.vehicle_colours[i].trim_colour = ride.vehicle_colours[i].Trim;
        _trackDesign.vehicle_additional_colour[i] = ride.vehicle_colours[i].Ternary;
    }

    for (int32_t i = 0; i < RCT12_NUM_COLOUR_SCHEMES; i++)
    {
        _trackDesign.track_spine_colour[i] = ride.track_colour[i].main;
        _trackDesign.track_rail_colour[i] = ride.track_colour[i].additional;
        _trackDesign.track_support_colour[i] = ride.track_colour[i].supports;
    }

    _trackDesign.depart_flags = ride.depart_flags;
    _trackDesign.number_of_trains = ride.num_vehicles;
    _trackDesign.number_of_cars_per_train = ride.num_cars_per_train;
    _trackDesign.min_waiting_time = ride.min_waiting_time;
    _trackDesign.max_waiting_time = ride.max_waiting_time;
    _trackDesign.operation_setting = ride.operation_option;
    _trackDesign.lift_hill_speed = ride.lift_hill_speed;
    _trackDesign.num_circuits = ride.num_circuits;

    _trackDesign.entrance_style = ride.entrance_style;
    _trackDesign.max_speed = (int8_t)(ride.max_speed / 65536);
    _trackDesign.average_speed = (int8_t)(ride.average_speed / 65536);
    _trackDesign.ride_length = ride_get_total_length(&ride) / 65536;
    _trackDesign.max_positive_vertical_g = ride.max_positive_vertical_g / 32;
    _trackDesign.max_negative_vertical_g = ride.max_negative_vertical_g / 32;
    _trackDesign.max_lateral_g = ride.max_lateral_g / 32;
    _trackDesign.inversions = ride.holes & 0x1F;
    _trackDesign.inversions = ride.inversions & 0x1F;
    _trackDesign.inversions |= (ride.sheltered_eighths << 5);
    _trackDesign.drops = ride.drops;
    _trackDesign.highest_drop_height = ride.highest_drop_height;

    uint16_t total_air_time = (ride.total_air_time * 123) / 1024;
    if (total_air_time > 255)
    {
        total_air_time = 0;
    }
    _trackDesign.total_air_time = (uint8_t)total_air_time;

    _trackDesign.excitement = ride.ratings.excitement / 10;
    _trackDesign.intensity = ride.ratings.intensity / 10;
    _trackDesign.nausea = ride.ratings.nausea / 10;

    _trackDesign.upkeep_cost = ride.upkeep_cost;
    _trackDesign.flags = 0;
    _trackDesign.flags2 = 0;

    if (_trackDesign.type == RIDE_TYPE_MAZE)
    {
        return CreateTrackDesignMaze(ride);
    }
    else
    {
        return CreateTrackDesignTrack(ride);
    }
}

rct_string_id T6Exporter::CreateTrackDesignTrack(Ride& ride)
{
    CoordsXYE trackElement;
    if (!ride_try_get_origin_element(&ride, &trackElement))
    {
        return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
    }

    ride_get_start_of_track(&trackElement);

    int32_t z = trackElement.element->base_height * 8;
    uint8_t track_type = trackElement.element->AsTrack()->GetTrackType();
    uint8_t direction = trackElement.element->GetDirection();
    _saveDirection = direction;

    if (sub_6C683D(&trackElement.x, &trackElement.y, &z, direction, track_type, 0, &trackElement.element, 0))
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
        rct_td6_track_element track{};
        track.type = trackElement.element->AsTrack()->GetTrackType();
        // TODO move to RCT2 limit
        if (track.type == TRACK_ELEM_255)
        {
            track.type = TRACK_ELEM_255_ALIAS;
        }

        uint8_t flags;
        if (track_element_has_speed_setting(track.type))
        {
            flags = trackElement.element->AsTrack()->GetBrakeBoosterSpeed() >> 1;
        }
        else
        {
            flags = trackElement.element->AsTrack()->GetSeatRotation();
        }

        if (trackElement.element->AsTrack()->HasChain())
            flags |= (1 << 7);
        flags |= trackElement.element->AsTrack()->GetColourScheme() << 4;
        if (RideData4[ride.type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE
            && trackElement.element->AsTrack()->IsInverted())
        {
            flags |= TRACK_ELEMENT_FLAG_INVERTED;
        }

        track.flags = flags;
        _trackDesign.track_elements.push_back(track);

        if (!track_block_get_next(&trackElement, &trackElement, nullptr, nullptr))
        {
            break;
        }

        z = trackElement.element->base_height * 8;
        direction = trackElement.element->GetDirection();
        track_type = trackElement.element->AsTrack()->GetTrackType();

        if (sub_6C683D(&trackElement.x, &trackElement.y, &z, direction, track_type, 0, &trackElement.element, 0))
        {
            break;
        }

        // TODO move to RCT2 limit
        constexpr auto TD6MaxTrackElements = 8192;

        if (_trackDesign.track_elements.size() > TD6MaxTrackElements)
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
            do
            {
                if (tileElement == nullptr)
                    break;
                if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                    continue;
                if (tileElement->base_height == z)
                    break;
            } while (!(tileElement++)->IsLastForTile());

            if (tileElement == nullptr)
            {
                continue;
            }
            // Add something that stops this from walking off the end

            Direction entranceDirection = tileElement->GetDirection();
            entranceDirection -= _saveDirection;
            entranceDirection &= TILE_ELEMENT_DIRECTION_MASK;

            rct_td6_entrance_element entrance{};
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

            if (z == 0xFF)
            {
                z = 0x80;
            }

            entrance.z = z;

            // If this is the exit version
            if (i == 1)
            {
                entrance.direction |= (1 << 7);
            }
            _trackDesign.entrance_elements.push_back(entrance);
        }
    }

    place_virtual_track(&_trackDesign, PTD_OPERATION_DRAW_OUTLINES, true, get_ride(0), 4096, 4096, 0);

    // Resave global vars for scenery reasons.
    gTrackPreviewOrigin = { start_x, start_y, start_z };

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;

    _trackDesign.space_required_x = ((gTrackPreviewMax.x - gTrackPreviewMin.x) / 32) + 1;
    _trackDesign.space_required_y = ((gTrackPreviewMax.y - gTrackPreviewMin.y) / 32) + 1;
    return 0;
}

rct_string_id T6Exporter::CreateTrackDesignMaze(Ride& ride)
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

                rct_td6_maze_element maze{};

                maze.maze_entry = tileElement->AsTrack()->GetMazeEntry();
                maze.x = (x - startLoc.x) / 32;
                maze.y = (y - startLoc.y) / 32;

                _trackDesign.maze_elements.push_back(maze);

                if (_trackDesign.maze_elements.size() >= 2000)
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
            break;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
            continue;
        if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE)
            continue;
        if (tileElement->AsEntrance()->GetRideIndex() == ride.id)
            break;
    } while (!(tileElement++)->IsLastForTile());
    // Add something that stops this from walking off the end

    uint8_t entranceDirection = tileElement->GetDirection();
    rct_td6_maze_element mazeEntrance{};
    mazeEntrance.direction = entranceDirection;
    mazeEntrance.type = 8;
    mazeEntrance.x = (int8_t)((entranceLoc.x - startLoc.x) / 32);
    mazeEntrance.y = (int8_t)((entranceLoc.y - startLoc.y) / 32);
    _trackDesign.maze_elements.push_back(mazeEntrance);

    location = ride_get_exit_location(&ride, 0);
    if (location.isNull())
    {
        return STR_TRACK_TOO_LARGE_OR_TOO_MUCH_SCENERY;
    }

    CoordsXY exitLoc = { location.x * 32, location.y * 32 };
    tileElement = map_get_first_element_at(location.x, location.y);
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
    rct_td6_maze_element mazeExit{};
    mazeExit.direction = exit_direction;
    mazeExit.type = 0x80;
    mazeExit.x = (int8_t)((exitLoc.x - startLoc.x) / 32);
    mazeExit.y = (int8_t)((exitLoc.y - startLoc.y) / 32);
    _trackDesign.maze_elements.push_back(mazeExit);

    // Save global vars as they are still used by scenery????
    int16_t startZ = gTrackPreviewOrigin.z;
    place_virtual_track(&_trackDesign, PTD_OPERATION_DRAW_OUTLINES, true, get_ride(0), 4096, 4096, 0);
    gTrackPreviewOrigin = { static_cast<int16_t>(startLoc.x), static_cast<int16_t>(startLoc.y), startZ };

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;

    _trackDesign.space_required_x = ((gTrackPreviewMax.x - gTrackPreviewMin.x) / 32) + 1;
    _trackDesign.space_required_y = ((gTrackPreviewMax.y - gTrackPreviewMin.y) / 32) + 1;
    return 0;
}

CoordsXYE T6Exporter::MazeGetFirstElement(Ride& ride)
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

rct_string_id T6Exporter::CreateTrackDesignScenery([[maybe_unused]] Ride& ride)
{
    _trackDesign.scenery_elements = _trackSavedTileElementsDesc;
    // Run an element loop
    for (auto& scenery : _trackDesign.scenery_elements)
    {
        switch (object_entry_get_type(&scenery.scenery_object))
        {
            case OBJECT_TYPE_PATHS: {
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
            case OBJECT_TYPE_WALLS: {
                uint8_t direction = scenery.flags & 3;
                direction -= _saveDirection;

                scenery.flags &= 0xFC;
                scenery.flags |= (direction & 3);
                break;
            }
            default: {
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

    return true;
}
