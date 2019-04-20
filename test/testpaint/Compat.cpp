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

class StationObject;

#define gRideEntries RCT2_ADDRESS(0x009ACFA4, rct_ride_entry*)
#define gTileElementTilePointers RCT2_ADDRESS(0x013CE9A4, TileElement*)
rct_sprite* sprite_list = RCT2_ADDRESS(0x010E63BC, rct_sprite);

Ride gRideList[MAX_RIDES];
int16_t gMapSizeUnits;
int16_t gMapBaseZ;
bool gTrackDesignSaveMode = false;
uint8_t gTrackDesignSaveRideIndex = RIDE_ID_NULL;
uint8_t gClipHeight = 255;
LocationXY8 gClipSelectionA = { 0, 0 };
LocationXY8 gClipSelectionB = { MAXIMUM_MAP_SIZE_TECHNICAL - 1, MAXIMUM_MAP_SIZE_TECHNICAL - 1 };
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

void entrance_paint(paint_session* session, uint8_t direction, int height, const TileElement* tile_element)
{
}
void banner_paint(paint_session* session, uint8_t direction, int height, const TileElement* tile_element)
{
}
void surface_paint(paint_session* session, uint8_t direction, uint16_t height, const TileElement* tileElement)
{
}
void path_paint(paint_session* session, uint16_t height, const TileElement* tileElement)
{
}
void scenery_paint(paint_session* session, uint8_t direction, int height, const TileElement* tileElement)
{
}
void fence_paint(paint_session* session, uint8_t direction, int height, const TileElement* tileElement)
{
}
void large_scenery_paint(paint_session* session, uint8_t direction, uint16_t height, const TileElement* tileElement)
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

rct_ride_entry* Ride::GetRideEntry() const
{
    rct_ride_entry* rideEntry = get_ride_entry(subtype);
    if (rideEntry == nullptr)
    {
        log_error("Invalid ride subtype for ride");
    }
    return rideEntry;
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

bool TileElementBase::IsGhost() const
{
    return (this->flags & TILE_ELEMENT_FLAG_GHOST) != 0;
}

bool TrackElement::BlockBrakeClosed() const
{
    return (flags & TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED) != 0;
}

TileElement* map_get_first_element_at(int x, int y)
{
    if (x < 0 || y < 0 || x > 255 || y > 255)
    {
        log_error("Trying to access element outside of range");
        return nullptr;
    }
    return gTileElementTilePointers[x + y * 256];
}

bool ride_type_has_flag(int rideType, uint32_t flag)
{
    return (RideProperties[rideType].flags & flag) != 0;
}

int16_t get_height_marker_offset()
{
    return 0;
}

bool is_csg_loaded()
{
    return false;
}

uint8_t TrackElement::GetSeatRotation() const
{
    return colour >> 4;
}

void TrackElement::SetSeatRotation(uint8_t newSeatRotation)
{
    colour &= 0x0F;
    colour |= (newSeatRotation << 4);
}

bool TrackElement::IsTakingPhoto() const
{
    return (sequence & MAP_ELEM_TRACK_SEQUENCE_TAKING_PHOTO_MASK) != 0;
}

void TrackElement::SetPhotoTimeout()
{
    sequence &= MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
    sequence |= (3 << 4);
}

void TrackElement::DecrementPhotoTimeout()
{
    // We should only touch the upper 4 bits, avoid underflow into the lower 4.
    if (sequence & MAP_ELEM_TRACK_SEQUENCE_TAKING_PHOTO_MASK)
    {
        sequence -= (1 << 4);
    }
}

uint16_t TrackElement::GetMazeEntry() const
{
    return mazeEntry;
}

void TrackElement::SetMazeEntry(uint16_t newMazeEntry)
{
    mazeEntry = newMazeEntry;
}

void TrackElement::MazeEntryAdd(uint16_t addVal)
{
    mazeEntry |= addVal;
}

void TrackElement::MazeEntrySubtract(uint16_t subVal)
{
    mazeEntry &= ~subVal;
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

uint8_t TrackElement::GetStationIndex() const
{
    return (sequence & MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK) >> 4;
}

void TrackElement::SetStationIndex(uint8_t newStationIndex)
{
    sequence &= ~MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK;
    sequence |= (newStationIndex << 4);
}

uint8_t TrackElement::GetDoorAState() const
{
    return (colour & TRACK_ELEMENT_DOOR_A_MASK) >> 2;
}

uint8_t TrackElement::GetDoorBState() const
{
    return (colour & TRACK_ELEMENT_DOOR_B_MASK) >> 5;
}

uint8_t TrackElement::GetRideIndex() const
{
    return rideIndex;
}

void TrackElement::SetRideIndex(uint8_t newRideIndex)
{
    rideIndex = newRideIndex;
}

uint8_t TrackElement::GetColourScheme() const
{
    return colour & 0x3;
}

void TrackElement::SetColourScheme(uint8_t newColourScheme)
{
    colour &= ~0x3;
    colour |= (newColourScheme & 0x3);
}

bool TrackElement::HasCableLift() const
{
    return colour & TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

void TrackElement::SetHasCableLift(bool on)
{
    colour &= ~TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
    if (on)
        colour |= TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

bool TrackElement::IsInverted() const
{
    return colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
}

void TrackElement::SetInverted(bool inverted)
{
    if (inverted)
    {
        colour |= TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
    else
    {
        colour &= ~TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
}

uint8_t TrackElement::GetBrakeBoosterSpeed() const
{
    return (sequence >> 4) << 1;
}

void TrackElement::SetBrakeBoosterSpeed(uint8_t speed)
{
    sequence &= ~0b11110000;
    sequence |= ((speed >> 1) << 4);
}

uint8_t TrackElement::HasGreenLight() const
{
    return (sequence & MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT) != 0;
}

void TrackElement::SetHasGreenLight(uint8_t greenLight)
{
    sequence &= ~MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
    if (greenLight)
    {
        sequence |= MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
    }
}

bool TrackElement::HasChain() const
{
    return type & TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
}

void TrackElement::SetHasChain(bool on)
{
    if (on)
    {
        type |= TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
    }
    else
    {
        type &= ~TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
    }
}

TileCoordsXYZD ride_get_entrance_location(const Ride* ride, const int32_t stationIndex)
{
    return ride->stations[stationIndex].Entrance;
}

TileCoordsXYZD ride_get_exit_location(const Ride* ride, const int32_t stationIndex)
{
    return ride->stations[stationIndex].Exit;
}

void TileElementBase::SetType(uint8_t newType)
{
    this->type &= ~TILE_ELEMENT_TYPE_MASK;
    this->type |= (newType & TILE_ELEMENT_TYPE_MASK);
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

bool TrackElement::IsHighlighted() const
{
    return (type & TILE_ELEMENT_TYPE_FLAG_HIGHLIGHT);
}

uint8_t PathElement::GetEdges() const
{
    return edges & 0xF;
}

StationObject* ride_get_station_object(const Ride* ride)
{
    return nullptr;
}
