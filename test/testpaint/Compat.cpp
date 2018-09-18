/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Addresses.h"

#include <openrct2/config/Config.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/object/Object.h>
#include <openrct2/paint/tile_element/Paint.TileElement.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/Track.h>
#include <openrct2/world/Location.hpp>
#include <openrct2/world/Sprite.h>
#include <openrct2/world/Surface.h>

#define gRideEntries RCT2_ADDRESS(0x009ACFA4, rct_ride_entry*)
#define gTileElementTilePointers RCT2_ADDRESS(0x013CE9A4, rct_tile_element*)
rct_sprite* sprite_list = RCT2_ADDRESS(0x010E63BC, rct_sprite);

Ride gRideList[MAX_RIDES];
int16_t gMapSizeUnits;
int16_t gMapBaseZ;
bool gTrackDesignSaveMode = false;
uint8_t gTrackDesignSaveRideIndex = 255;
uint8_t gClipHeight = 255;
LocationXY8 gClipSelectionA = { 0, 0 };
LocationXY8 gClipSelectionB = { MAXIMUM_MAP_SIZE_TECHNICAL - 1, MAXIMUM_MAP_SIZE_TECHNICAL - 1 };
uint32_t gCurrentViewportFlags;
uint32_t gScenarioTicks;
uint8_t gCurrentRotation;

// clang-format off
const CoordsXY CoordsDirectionDelta[] = {
    { -32, 0 },
    { 0, +32 },
    { +32, 0 },
    { 0, -32 },
    { -32, +32 },
    { +32, +32 },
    { +32, -32 },
    { -32, -32 },
};

const TileCoordsXY TileDirectionDelta[] = {
    { -1, 0 },
    { 0, +1 },
    { +1, 0 },
    { 0, -1 },
    { -1, +1 },
    { +1, +1 },
    { +1, -1 },
    { -1, -1 },
};
// clang-format on

TileCoordsXYZD ride_get_entrance_location(const Ride* ride, const int32_t stationIndex);
TileCoordsXYZD ride_get_exit_location(const Ride* ride, const int32_t stationIndex);

uint8_t get_current_rotation()
{
    return gCurrentRotation & 3;
}

const uint32_t construction_markers[] = {
    COLOUR_DARK_GREEN << 19 | COLOUR_GREY << 24 | IMAGE_TYPE_REMAP, // White
    2 << 19 | 0b110000 << 19 | IMAGE_TYPE_TRANSPARENT,              // Translucent
};

int object_entry_group_counts[] = {
    128, // rides
    252, // small scenery
    128, // large scenery
    128, // walls
    32,  // banners
    16,  // paths
    15,  // path bits
    19,  // scenery sets
    1,   // park entrance
    1,   // water
    1    // scenario text
};

GeneralConfiguration gConfigGeneral;
uint16_t gMapSelectFlags;
uint16_t gMapSelectType;
LocationXY16 gMapSelectPositionA;
LocationXY16 gMapSelectPositionB;
LocationXYZ16 gMapSelectArrowPosition;
uint8_t gMapSelectArrowDirection;

void entrance_paint(paint_session* session, uint8_t direction, int height, const rct_tile_element* tile_element)
{
}
void banner_paint(paint_session* session, uint8_t direction, int height, const rct_tile_element* tile_element)
{
}
void surface_paint(paint_session* session, uint8_t direction, uint16_t height, const rct_tile_element* tileElement)
{
}
void path_paint(paint_session* session, uint16_t height, const rct_tile_element* tileElement)
{
}
void scenery_paint(paint_session* session, uint8_t direction, int height, const rct_tile_element* tileElement)
{
}
void fence_paint(paint_session* session, uint8_t direction, int height, const rct_tile_element* tileElement)
{
}
void large_scenery_paint(paint_session* session, uint8_t direction, uint16_t height, const rct_tile_element* tileElement)
{
}

Ride* get_ride(int index)
{
    if (index < 0 || index >= MAX_RIDES)
    {
        log_error("invalid index %d for ride", index);
        return nullptr;
    }
    return &gRideList[index];
}

rct_ride_entry* get_ride_entry(int index)
{
    if (index < 0 || index >= object_entry_group_counts[OBJECT_TYPE_RIDE])
    {
        log_error("invalid index %d for ride type", index);
        return nullptr;
    }

    return gRideEntries[index];
}

rct_ride_entry* get_ride_entry_by_ride(Ride* ride)
{
    rct_ride_entry* type = get_ride_entry(ride->subtype);
    if (type == nullptr)
    {
        log_error("Invalid ride subtype for ride");
    }
    return type;
}

rct_sprite* get_sprite(size_t sprite_idx)
{
    assert(sprite_idx < MAX_SPRITES);
    return &sprite_list[sprite_idx];
}

bool TileElementBase::IsLastForTile() const
{
    return (this->flags & TILE_ELEMENT_FLAG_LAST_TILE) != 0;
}

uint8_t TileElementBase::GetType() const
{
    return this->type & TILE_ELEMENT_TYPE_MASK;
}

rct_tile_element* map_get_first_element_at(int x, int y)
{
    if (x < 0 || y < 0 || x > 255 || y > 255)
    {
        log_error("Trying to access element outside of range");
        return nullptr;
    }
    return gTileElementTilePointers[x + y * 256];
}

bool ride_type_has_flag(int rideType, int flag)
{
    return (RideProperties[rideType].flags & flag) != 0;
}

int16_t get_height_marker_offset()
{
    return 0;
}

bool track_element_is_lift_hill(const rct_tile_element* trackElement)
{
    return trackElement->type & 0x80;
}

bool is_csg_loaded()
{
    return false;
}

uint8_t TrackElement::GetTrackType() const
{
    return trackType;
}

void TrackElement::SetTrackType(uint8_t newType)
{
    trackType = newType;
}

uint8_t TrackElement::GetSequenceIndex() const
{
    return sequence & MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
}

void TrackElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    sequence &= ~MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
    sequence |= (newSequenceIndex & MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK);
}

TileCoordsXYZD ride_get_entrance_location(const Ride* ride, const int32_t stationIndex)
{
    return ride->entrances[stationIndex];
}

TileCoordsXYZD ride_get_exit_location(const Ride* ride, const int32_t stationIndex)
{
    return ride->exits[stationIndex];
}

uint8_t TileElementBase::GetDirection() const
{
    return this->type & TILE_ELEMENT_DIRECTION_MASK;
}

uint8_t TileElementBase::GetDirectionWithOffset(uint8_t offset) const
{
    return ((this->type & TILE_ELEMENT_DIRECTION_MASK) + offset) & TILE_ELEMENT_DIRECTION_MASK;
}

uint8_t SurfaceElement::GetSlope() const
{
    return (slope & TILE_ELEMENT_SURFACE_SLOPE_MASK);
}

uint32_t SurfaceElement::GetWaterHeight() const
{
    return terrain & TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK;
}
