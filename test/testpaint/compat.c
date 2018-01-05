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

#include <openrct2/config/Config.h>
#include <openrct2/object/Object.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/Track.h>
#include <openrct2/world/sprite.h>
#include <openrct2/paint/tile_element/TileElement.h>
#include <openrct2/ride/Ride.h>
#include "addresses.h"

#define gRideEntries                RCT2_ADDRESS(0x009ACFA4, rct_ride_entry *)
#define gTileElementTilePointers    RCT2_ADDRESS(0x013CE9A4, rct_tile_element *)
rct_sprite *sprite_list =           RCT2_ADDRESS(0x010E63BC, rct_sprite);

Ride gRideList[MAX_RIDES];
sint16 gMapSizeUnits;
sint16 gMapBaseZ;
bool gTrackDesignSaveMode = false;
uint8 gTrackDesignSaveRideIndex = 255;
uint8 gClipHeight = 255;
uint32 gCurrentViewportFlags;
uint32 gScenarioTicks;
uint8 gCurrentRotation;

const LocationXY16 TileDirectionDelta[] = {
    {-32, 0},
    {0,   +32},
    {+32, 0},
    {0,   -32},
    {-32, +32},
    {+32, +32},
    {+32, -32},
    {-32, -32}
};

uint8 get_current_rotation() {
    return gCurrentRotation & 3;
}

const uint32 construction_markers[] = {
    COLOUR_DARK_GREEN << 19 | COLOUR_GREY << 24 | IMAGE_TYPE_REMAP, // White
    2 << 19 | 0b110000 << 19 | IMAGE_TYPE_TRANSPARENT, // Translucent
};

int object_entry_group_counts[] = {
    128,    // rides
    252,    // small scenery
    128,    // large scenery
    128,    // walls
    32,        // banners
    16,        // paths
    15,        // path bits
    19,        // scenery sets
    1,        // park entrance
    1,        // water
    1        // scenario text
};

GeneralConfiguration gConfigGeneral;
uint16 gMapSelectFlags;
uint16 gMapSelectType;
LocationXY16 gMapSelectPositionA;
LocationXY16 gMapSelectPositionB;
LocationXYZ16 gMapSelectArrowPosition;
uint8 gMapSelectArrowDirection;

void entrance_paint(paint_session * session, uint8 direction, int height, rct_tile_element *tile_element) { }
void banner_paint(paint_session * session, uint8 direction, int height, rct_tile_element *tile_element) { }
void surface_paint(paint_session * session, uint8 direction, uint16 height, rct_tile_element *tileElement) { }
void path_paint(paint_session * session, uint8 direction, uint16 height, rct_tile_element *tileElement) { }
void scenery_paint(paint_session * session, uint8 direction, int height, rct_tile_element *tileElement) { }
void fence_paint(paint_session * session, uint8 direction, int height, rct_tile_element *tileElement) { }
void large_scenery_paint(paint_session * session, uint8 direction, uint16 height, rct_tile_element *tileElement) { }

Ride *get_ride(int index) {
    if (index < 0 || index >= MAX_RIDES) {
        log_error("invalid index %d for ride", index);
        return NULL;
    }
    return &gRideList[index];
}

rct_ride_entry *get_ride_entry(int index) {
    if (index < 0 || index >= object_entry_group_counts[OBJECT_TYPE_RIDE]) {
        log_error("invalid index %d for ride type", index);
        return NULL;
    }

    return gRideEntries[index];
}

rct_ride_entry *get_ride_entry_by_ride(Ride *ride) {
    rct_ride_entry * type = get_ride_entry(ride->subtype);
    if (type == NULL) {
        log_error("Invalid ride subtype for ride");
    }
    return type;
}

rct_sprite *get_sprite(size_t sprite_idx) {
    assert(sprite_idx < MAX_SPRITES);
    return &sprite_list[sprite_idx];
}

bool tile_element_is_last_for_tile(const rct_tile_element *element) {
    return (element->flags & TILE_ELEMENT_FLAG_LAST_TILE) != 0;
}

int tile_element_get_type(const rct_tile_element *element) {
    return element->type & TILE_ELEMENT_TYPE_MASK;
}

int tile_element_get_direction(const rct_tile_element *element) {
    return element->type & TILE_ELEMENT_DIRECTION_MASK;
}

int tile_element_get_direction_with_offset(const rct_tile_element *element, uint8 offset) {
    return ((element->type & TILE_ELEMENT_DIRECTION_MASK) + offset) & TILE_ELEMENT_DIRECTION_MASK;
}

rct_tile_element *map_get_first_element_at(int x, int y) {
    if (x < 0 || y < 0 || x > 255 || y > 255) {
        log_error("Trying to access element outside of range");
        return NULL;
    }
    return gTileElementTilePointers[x + y * 256];
}

int tile_element_get_station(const rct_tile_element * tileElement) {
    return (tileElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK) >> 4;
}

void tile_element_set_station(rct_tile_element * tileElement, uint32 stationIndex)
{
    tileElement->properties.track.sequence &= ~MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK;
    tileElement->properties.track.sequence |= (stationIndex << 4);
}

sint32 tile_element_get_track_sequence(const rct_tile_element * tileElement)
{
    return tileElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
}

void tile_element_set_track_sequence(rct_tile_element * tileElement, int trackSequence)
{
    tileElement->properties.track.sequence &= ~MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
    tileElement->properties.track.sequence |= (trackSequence & MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK);
}

bool tile_element_get_green_light(const rct_tile_element * tileElement)
{
    return (tileElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT) != 0;
}

void tile_element_set_green_light(rct_tile_element * tileElement, bool greenLight)
{
    tileElement->properties.track.sequence &= ~MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
    if (greenLight)
    {
        tileElement->properties.track.sequence |= MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
    }
}

int tile_element_get_brake_booster_speed(const rct_tile_element *tileElement)
{
    return (tileElement->properties.track.sequence >> 4) << 1;
}

void tile_element_set_brake_booster_speed(rct_tile_element *tileElement, int speed)
{
    tileElement->properties.track.sequence = tile_element_get_track_sequence(tileElement) | ((speed >> 1) << 4);
}

bool tile_element_is_taking_photo(const rct_tile_element * tileElement)
{
    return (tileElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_TAKING_PHOTO_MASK) != 0;
}

void tile_element_set_onride_photo_timeout(rct_tile_element * tileElement)
{
    tileElement->properties.track.sequence &= MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
    tileElement->properties.track.sequence |= (3 << 4);
}

void tile_element_decrement_onride_photo_timout(rct_tile_element * tileElement)
{
    // We should only touch the upper 4 bits, avoid underflow into the lower 4.
    if (tileElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_TAKING_PHOTO_MASK)
    {
        tileElement->properties.track.sequence -= (1 << 4);
    }
}

sint32 map_get_water_height(const rct_tile_element * tileElement)
{
    return tileElement->properties.surface.terrain & TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK;
}

bool ride_type_has_flag(int rideType, int flag)
{
    return (RideProperties[rideType].flags & flag) != 0;
}

sint16 get_height_marker_offset()
{
    return 0;
}

bool track_element_is_lift_hill(rct_tile_element *trackElement)
{
    return trackElement->type & 0x80;
}

bool track_element_is_cable_lift(rct_tile_element *trackElement)
{
    return trackElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

bool track_element_is_inverted(rct_tile_element *trackElement)
{
    return trackElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
}

void track_element_set_inverted(rct_tile_element * tileElement, bool inverted)
{
    if (inverted)
    {
        tileElement->properties.track.colour |= TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
    else
    {
        tileElement->properties.track.colour &= ~TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
}

bool is_csg_loaded()
{
    return false;
}

uint8 track_element_get_colour_scheme(const rct_tile_element * tileElement)
{
    return tileElement->properties.track.colour & 0x3;
}

uint16 track_element_get_maze_entry(const rct_tile_element * tileElement)
{
    return tileElement->properties.track.maze_entry;
}

uint8 track_element_get_ride_index(const rct_tile_element * tileElement)
{
    return tileElement->properties.track.ride_index;
}

void track_element_set_ride_index(rct_tile_element * tileElement, uint8 rideIndex)
{
    tileElement->properties.track.ride_index = rideIndex;
}

uint8 track_element_get_type(const rct_tile_element * tileElement)
{
    return tileElement->properties.track.type;
}

void track_element_set_type(rct_tile_element * tileElement, uint8 type)
{
    tileElement->properties.track.type = type;
}

void track_element_set_cable_lift(rct_tile_element * trackElement)
{
    trackElement->properties.track.colour |= TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

void track_element_clear_cable_lift(rct_tile_element * trackElement)
{
    trackElement->properties.track.colour &= ~TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}
