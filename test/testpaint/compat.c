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
#include <openrct2/object.h>
#include <openrct2/interface/colour.h>
#include <openrct2/interface/viewport.h>
#include <openrct2/ride/ride.h>
#include <openrct2/ride/track.h>
#include <openrct2/world/sprite.h>
#include <openrct2/paint/map_element/map_element.h>

#define RCT2_ADDRESS_SPRITE_LIST                    0x010E63BC

#define gRideEntries                RCT2_ADDRESS(RCT2_ADDRESS_RIDE_ENTRIES,                rct_ride_entry*)
#define gCurrentRotation        RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)

rct_map_element *gMapElements = (rct_map_element *) RCT2_ADDRESS_MAP_ELEMENTS;
rct_map_element **gMapElementTilePointers = (rct_map_element **) RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS;
rct_ride *gRideList = RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride);
rct_sprite *sprite_list = RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite);
sint16 gMapSizeUnits;
sint16 gMapBaseZ;
bool gTrackDesignSaveMode = false;
uint8 gTrackDesignSaveRideIndex = 255;
uint8 gClipHeight = 255;

const rct_xy16 TileDirectionDelta[] = {
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
rct_xy16 gMapSelectPositionA;
rct_xy16 gMapSelectPositionB;
rct_xyz16 gMapSelectArrowPosition;
uint8 gMapSelectArrowDirection;

void entrance_paint(uint8 direction, int height, rct_map_element *map_element) { }
void banner_paint(paint_session * session, uint8 direction, int height, rct_map_element *map_element) { }
void surface_paint(paint_session * session, uint8 direction, uint16 height, rct_map_element *mapElement) { }
void path_paint(paint_session * session, uint8 direction, uint16 height, rct_map_element *mapElement) { }
void scenery_paint(uint8 direction, int height, rct_map_element *mapElement) { }
void fence_paint(uint8 direction, int height, rct_map_element *mapElement) { }
void scenery_multiple_paint(paint_session * session, uint8 direction, uint16 height, rct_map_element *mapElement) { }

rct_ride *get_ride(int index) {
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

rct_ride_entry *get_ride_entry_by_ride(rct_ride *ride) {
    rct_ride_entry *type = get_ride_entry(ride->subtype);
    if (type == NULL) {
        log_error("Invalid ride subtype for ride");
    }
    return type;
}

rct_sprite *get_sprite(size_t sprite_idx) {
    assert(sprite_idx < MAX_SPRITES);
    return &sprite_list[sprite_idx];
}

int map_element_is_last_for_tile(const rct_map_element *element) {
    return element->flags & MAP_ELEMENT_FLAG_LAST_TILE;
}

int map_element_get_type(const rct_map_element *element) {
    return element->type & MAP_ELEMENT_TYPE_MASK;
}

int map_element_get_direction(const rct_map_element *element) {
    return element->type & MAP_ELEMENT_DIRECTION_MASK;
}

int map_element_get_direction_with_offset(const rct_map_element *element, uint8 offset) {
    return ((element->type & MAP_ELEMENT_DIRECTION_MASK) + offset) & MAP_ELEMENT_DIRECTION_MASK;
}

rct_map_element *map_get_first_element_at(int x, int y) {
    if (x < 0 || y < 0 || x > 255 || y > 255) {
        log_error("Trying to access element outside of range");
        return NULL;
    }
    return gMapElementTilePointers[x + y * 256];
}

int map_element_get_station(const rct_map_element * mapElement) {
    return (mapElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK) >> 4;
}

void map_element_set_station(rct_map_element * mapElement, uint32 stationIndex)
{
    mapElement->properties.track.sequence &= ~MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK;
    mapElement->properties.track.sequence |= (stationIndex << 4);
}

sint32 map_element_get_track_sequence(const rct_map_element * mapElement)
{
    return mapElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
}

void map_element_set_track_sequence(rct_map_element * mapElement, int trackSequence)
{
    mapElement->properties.track.sequence &= ~MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
    mapElement->properties.track.sequence |= (trackSequence & MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK);
}

bool map_element_get_green_light(const rct_map_element * mapElement)
{
    return (mapElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT) != 0;
}

void map_element_set_green_light(rct_map_element * mapElement, bool greenLight)
{
    mapElement->properties.track.sequence &= ~MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
    if (greenLight)
    {
        mapElement->properties.track.sequence |= MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
    }
}

int map_element_get_brake_booster_speed(const rct_map_element *mapElement)
{
    return (mapElement->properties.track.sequence >> 4) << 1;
}

void map_element_set_brake_booster_speed(rct_map_element *mapElement, int speed)
{
    mapElement->properties.track.sequence = map_element_get_track_sequence(mapElement) | ((speed >> 1) << 4);
}

bool map_element_is_taking_photo(const rct_map_element * mapElement)
{
    return (mapElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_TAKING_PHOTO_MASK) != 0;
}

void map_element_set_onride_photo_timeout(rct_map_element * mapElement)
{
    mapElement->properties.track.sequence &= MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
    mapElement->properties.track.sequence |= (3 << 4);
}

void map_element_decrement_onride_photo_timout(rct_map_element * mapElement)
{
    // We should only touch the upper 4 bits, avoid underflow into the lower 4.
    if (mapElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_TAKING_PHOTO_MASK)
    {
        mapElement->properties.track.sequence -= (1 << 4);
    }
}

sint32 map_get_water_height(const rct_map_element * mapElement)
{
    return mapElement->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
}

bool ride_type_has_flag(int rideType, int flag)
{
    return (RideProperties[rideType].flags & flag) != 0;
}

sint16 get_height_marker_offset()
{
    return 0;
}

bool track_element_is_lift_hill(rct_map_element *trackElement)
{
    return trackElement->type & 0x80;
}

bool track_element_is_cable_lift(rct_map_element *trackElement)
{
    return trackElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

bool track_element_is_inverted(rct_map_element *trackElement)
{
    return trackElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
}

bool is_csg_loaded()
{
    return false;
}
