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

#pragma once

#include "../common.h"

#pragma pack(push, 1)
struct rct_tile_element_surface_properties {
    uint8 slope; //4 0xE0 Edge Style, 0x1F Slope
    uint8 terrain; //5 0xE0 Terrain Style, 0x1F Water height
    uint8 grass_length; //6
    uint8 ownership; //7
};
assert_struct_size(rct_tile_element_surface_properties, 4);

struct rct_tile_element_path_properties {
    uint8 type; //4 0xF0 Path type, 0x08 Ride sign, 0x04 Set when path is diagonal, 0x03 Rotation
    uint8 additions; //5
    uint8 edges; //6
    union {
        uint8 addition_status; //7
        uint8 ride_index;
    };
};
assert_struct_size(rct_tile_element_path_properties, 4);

struct rct_tile_element_track_properties {
    uint8 type; //4
    union {
        struct {
            // The lower 4 bits are the track sequence.
            // The upper 4 bits are either station bits or on-ride photo bits.
            //
            // Station bits:
            // - Bit 8 marks green light
            // - Bit 5-7 are station index.
            //
            // On-ride photo bits:
            // - Bits 7 and 8 are never set
            // - Bits 5 and 6 are set when a vehicle triggers the on-ride photo and act like a countdown from 3.
            // - If any of the bits 5-8 are set, the game counts it as a photo being taken.
            uint8 sequence; //5.
            uint8 colour; //6
        };
        uint16 maze_entry; // 5
    };
    uint8 ride_index; //7
};
assert_struct_size(rct_tile_element_track_properties, 4);

struct rct_tile_element_scenery_properties {
    uint8 type; //4
    uint8 age; //5
    uint8 colour_1; //6
    uint8 colour_2; //7
};
assert_struct_size(rct_tile_element_scenery_properties, 4);

struct rct_tile_element_entrance_properties {
    uint8 type; //4
    uint8 index; //5
    uint8 path_type; //6
    uint8 ride_index; //7
};
assert_struct_size(rct_tile_element_entrance_properties, 4);

struct rct_tile_element_wall_properties {
    uint8 type; //4
    union {
        uint8 colour_3; //5
        uint8 banner_index; //5
    };
    uint8 colour_1; //6 0b_2221_1111 2 = colour_2 (uses flags for rest of colour2), 1 = colour_1
    uint8 animation; //7 0b_dfff_ft00 d = direction, f = frame num, t = across track flag (not used)
};
assert_struct_size(rct_tile_element_wall_properties, 4);

struct rct_tile_element_scenerymultiple_properties {
    uint16 type; //4
    uint8 colour[2]; //6
};
assert_struct_size(rct_tile_element_scenerymultiple_properties, 4);

struct rct_tile_element_banner_properties {
    uint8 index; //4
    uint8 position; //5
    uint8 flags; //6
    uint8 unused; //7
};
assert_struct_size(rct_tile_element_banner_properties, 4);

union rct_tile_element_properties {
    rct_tile_element_surface_properties surface;
    rct_tile_element_path_properties path;
    rct_tile_element_track_properties track;
    rct_tile_element_scenery_properties scenery;
    rct_tile_element_entrance_properties entrance;
    rct_tile_element_wall_properties wall;
    rct_tile_element_scenerymultiple_properties scenerymultiple;
    rct_tile_element_banner_properties banner;
};
assert_struct_size(rct_tile_element_properties, 4);

/**
* Map element structure
* size: 0x08
*/
struct rct_tile_element {
    uint8 type; //0
    uint8 flags; //1
    uint8 base_height; //2
    uint8 clearance_height; //3
    rct_tile_element_properties properties;

    uint8 GetType() const;
    void  SetType(uint8 newType);
    uint8 GetDirection() const;
    void  SetDirection(uint8 direction);
    uint8 GetDirectionWithOffset(uint8 offset) const;
    bool  IsLastForTile() const;
    bool  IsGhost() const;
    uint8 GetSceneryQuadrant() const;
};
assert_struct_size(rct_tile_element, 8);
#pragma pack(pop)

enum {
    TILE_ELEMENT_QUADRANT_SW,
    TILE_ELEMENT_QUADRANT_NW,
    TILE_ELEMENT_QUADRANT_NE,
    TILE_ELEMENT_QUADRANT_SE
};

enum {
    TILE_ELEMENT_TYPE_SURFACE = (0 << 2),
    TILE_ELEMENT_TYPE_PATH = (1 << 2),
    TILE_ELEMENT_TYPE_TRACK = (2 << 2),
    TILE_ELEMENT_TYPE_SMALL_SCENERY = (3 << 2),
    TILE_ELEMENT_TYPE_ENTRANCE = (4 << 2),
    TILE_ELEMENT_TYPE_WALL = (5 << 2),
    TILE_ELEMENT_TYPE_LARGE_SCENERY = (6 << 2),
    TILE_ELEMENT_TYPE_BANNER = (7 << 2),
    // The corrupt element type is used for skipping drawing other following
    // elements on a given tile.
    TILE_ELEMENT_TYPE_CORRUPT = (8 << 2),
};

enum {
    TILE_ELEMENT_TYPE_FLAG_HIGHLIGHT = (1 << 6)
};

enum {
    TILE_ELEMENT_DIRECTION_WEST,
    TILE_ELEMENT_DIRECTION_NORTH,
    TILE_ELEMENT_DIRECTION_EAST,
    TILE_ELEMENT_DIRECTION_SOUTH
};

enum {
    TILE_ELEMENT_FLAG_GHOST = (1 << 4),
    TILE_ELEMENT_FLAG_BROKEN = (1 << 5),
    TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED = (1 << 5),
    TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE = (1 << 6),
    TILE_ELEMENT_FLAG_LAST_TILE = (1 << 7)
};

enum {
    ENTRANCE_TYPE_RIDE_ENTRANCE,
    ENTRANCE_TYPE_RIDE_EXIT,
    ENTRANCE_TYPE_PARK_ENTRANCE
};

enum {
    ELEMENT_IS_ABOVE_GROUND = 1 << 0,
    ELEMENT_IS_UNDERGROUND = 1 << 1,
    ELEMENT_IS_UNDERWATER = 1 << 2,
};

enum
{
    MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT = (1 << 7),
};



#define TILE_ELEMENT_QUADRANT_MASK  0b11000000
#define TILE_ELEMENT_TYPE_MASK      0b00111100
#define TILE_ELEMENT_DIRECTION_MASK 0b00000011

#define TILE_ELEMENT_COLOUR_MASK    0b00011111

#define MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK 0b01110000
#define MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK      0b00001111
#define MAP_ELEM_TRACK_SEQUENCE_TAKING_PHOTO_MASK  0b11110000

sint32 tile_element_get_direction(const rct_tile_element * element);
sint32 tile_element_get_direction_with_offset(const rct_tile_element * element, uint8 offset);
sint32 tile_element_get_banner_index(rct_tile_element * tileElement);
bool tile_element_is_ghost(const rct_tile_element * element);
bool tile_element_is_underground(rct_tile_element * tileElement);
bool tile_element_is_last_for_tile(const rct_tile_element *element);

// ~Oli414: The banner functions should probably be part of banner.
void tile_element_set_banner_index(rct_tile_element * tileElement, sint32 bannerIndex);
void tile_element_remove_banner_entry(rct_tile_element *tileElement);

uint8 tile_element_get_ride_index(const rct_tile_element * tileElement);
