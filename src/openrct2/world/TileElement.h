/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#pragma pack(push, 1)
struct rct_tile_element_surface_properties {
    uint8_t slope; //4 0xE0 Edge Style, 0x1F Slope
    uint8_t terrain; //5 0xE0 Terrain Style, 0x1F Water height
    uint8_t grass_length; //6
    uint8_t ownership; //7
};
assert_struct_size(rct_tile_element_surface_properties, 4);

struct rct_tile_element_path_properties {
    uint8_t type; //4 0xF0 Path type, 0x08 Ride sign, 0x04 Set when path is diagonal, 0x03 Rotation
    uint8_t additions; //5
    uint8_t edges; //6
    union {
        uint8_t addition_status; //7
        uint8_t ride_index;
    };
};
assert_struct_size(rct_tile_element_path_properties, 4);

struct rct_tile_element_track_properties {
    uint8_t type; //4
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
            uint8_t sequence; //5.
            uint8_t colour; //6
        };
        uint16_t maze_entry; // 5
    };
    uint8_t ride_index; //7
};
assert_struct_size(rct_tile_element_track_properties, 4);

struct rct_tile_element_scenery_properties {
    uint8_t type; //4
    uint8_t age; //5
    uint8_t colour_1; //6
    uint8_t colour_2; //7
};
assert_struct_size(rct_tile_element_scenery_properties, 4);

struct rct_tile_element_entrance_properties {
    uint8_t type; //4
    uint8_t index; //5
    uint8_t path_type; //6
    uint8_t ride_index; //7
};
assert_struct_size(rct_tile_element_entrance_properties, 4);

struct rct_tile_element_wall_properties {
    uint8_t type; //4
    union {
        uint8_t colour_3; //5
        BannerIndex banner_index; // 5
    };
    uint8_t colour_1; //6 0b_2221_1111 2 = colour_2 (uses flags for rest of colour2), 1 = colour_1
    uint8_t animation; //7 0b_dfff_ft00 d = direction, f = frame num, t = across track flag (not used)
};
assert_struct_size(rct_tile_element_wall_properties, 4);

struct rct_tile_element_scenerymultiple_properties {
    uint16_t type; //4
    uint8_t colour[2]; //6
};
assert_struct_size(rct_tile_element_scenerymultiple_properties, 4);

struct rct_tile_element_banner_properties {
    BannerIndex index; // 4
    uint8_t position; //5
    uint8_t flags; //6
    uint8_t unused; //7
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
    uint8_t type; //0
    uint8_t flags; //1
    uint8_t base_height; //2
    uint8_t clearance_height; //3
    rct_tile_element_properties properties;

    uint8_t GetType() const;
    void  SetType(uint8_t newType);
    uint8_t GetDirection() const;
    void  SetDirection(uint8_t direction);
    uint8_t GetDirectionWithOffset(uint8_t offset) const;
    bool  IsLastForTile() const;
    bool  IsGhost() const;
    uint8_t GetSceneryQuadrant() const;
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
    TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE = (1 << 6),
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

int32_t tile_element_get_direction(const rct_tile_element * element);
int32_t tile_element_get_direction_with_offset(const rct_tile_element * element, uint8_t offset);
BannerIndex tile_element_get_banner_index(rct_tile_element* tileElement);
bool tile_element_is_ghost(const rct_tile_element * element);
bool tile_element_is_underground(rct_tile_element * tileElement);
bool tile_element_is_last_for_tile(const rct_tile_element *element);

// ~Oli414: The banner functions should probably be part of banner.
void tile_element_set_banner_index(rct_tile_element* tileElement, BannerIndex bannerIndex);
void tile_element_remove_banner_entry(rct_tile_element *tileElement);

uint8_t tile_element_get_ride_index(const rct_tile_element * tileElement);
