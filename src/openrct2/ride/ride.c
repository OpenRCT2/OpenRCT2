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
#include "../audio/AudioMixer.h"
#include "../cheats.h"
#include "../common.h"
#include "../config/Config.h"
#include "../Context.h"
#include "../Editor.h"
#include "../game.h"
#include "../input.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../management/finance.h"
#include "../management/marketing.h"
#include "../management/news_item.h"
#include "../network/network.h"
#include "../object_list.h"
#include "../OpenRCT2.h"
#include "../peep/peep.h"
#include "../peep/staff.h"
#include "../rct1.h"
#include "../rct2/addresses.h"
#include "../scenario/scenario.h"
#include "../util/util.h"
#include "../windows/error.h"
#include "../world/banner.h"
#include "../world/Climate.h"
#include "../world/footpath.h"
#include "../world/map.h"
#include "../world/map_animation.h"
#include "../world/scenery.h"
#include "../world/sprite.h"
#include "cable_lift.h"
#include "ride.h"
#include "ride_data.h"
#include "RideGroupManager.h"
#include "station.h"
#include "track.h"
#include "track_data.h"

#pragma region Ride classification table

/** rct2: 0x0097C3AF */
const uint8 gRideClassifications[255] = {
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_SHOP_OR_STALL,
    RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_RIDE, RIDE_CLASS_SHOP_OR_STALL,
    RIDE_CLASS_KIOSK_OR_FACILITY, RIDE_CLASS_KIOSK_OR_FACILITY, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_KIOSK_OR_FACILITY, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_KIOSK_OR_FACILITY, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_RIDE, RIDE_CLASS_SHOP_OR_STALL,
    RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_SHOP_OR_STALL, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE,
    RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE, RIDE_CLASS_RIDE
};

uint8 gTypeToRideEntryIndexMap[TYPE_TO_RIDE_ENTRY_SLOTS];

#pragma endregion

static const sint32 RideInspectionInterval[] = {
    10, 20, 30, 45, 60, 120, 0, 0
};

#ifdef NO_RCT2
Ride gRideList[MAX_RIDES];
#else
Ride *gRideList = RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, Ride);
#endif

rct_ride_measurement gRideMeasurements[MAX_RIDE_MEASUREMENTS];

uint16 gRideCount;
bool gGotoStartPlacementMode = false;
sint32 gRideRemoveTrackPieceCallbackX;
sint32 gRideRemoveTrackPieceCallbackY;
sint32 gRideRemoveTrackPieceCallbackZ;
sint32 gRideRemoveTrackPieceCallbackDirection;
sint32 gRideRemoveTrackPieceCallbackType;

money16 gTotalRideValue;
uint32 gSamePriceThroughoutParkA;
uint32 gSamePriceThroughoutParkB;

money32 _currentTrackPrice;

uint16 _numCurrentPossibleRideConfigurations;
uint16 _numCurrentPossibleSpecialTrackPieces;

uint16 _currentTrackCurve;
uint16 _currentTrackEndX;
uint16 _currentTrackEndY;
uint8 _rideConstructionState;
uint8 _currentRideIndex;
uint16 _currentTrackBeginX;
uint16 _currentTrackBeginY;
uint16 _currentTrackBeginZ;
uint8 _currentTrackPieceDirection;
uint8 _currentTrackPieceType;
uint8 _currentTrackSelectionFlags;
sint8 _rideConstructionArrowPulseTime;
uint8 _currentTrackSlopeEnd;
uint8 _currentTrackBankEnd;
uint8 _currentTrackLiftHill;
uint8 _currentTrackAlternative;
uint8 _selectedTrackType;

uint8 _previousTrackBankEnd;
uint8 _previousTrackSlopeEnd;

uint16 _previousTrackPieceX;
uint16 _previousTrackPieceY;
uint16 _previousTrackPieceZ;

uint8 _currentBrakeSpeed2;
uint8 _currentSeatRotationAngle;

rct_xyz16 _unkF44188;


rct_xyzd16 _unkF440C5;

uint8 gRideEntranceExitPlaceType;
uint8 gRideEntranceExitPlaceRideIndex;
uint8 gRideEntranceExitPlaceStationIndex;
uint8 gRideEntranceExitPlacePreviousRideConstructionState;
uint8 gRideEntranceExitPlaceDirection;

uint8 gLastEntranceStyle;

// Static function declarations
rct_peep *find_closest_mechanic(sint32 x, sint32 y, sint32 forInspection);
static void ride_breakdown_status_update(sint32 rideIndex);
static void ride_breakdown_update(sint32 rideIndex);
static void ride_call_closest_mechanic(sint32 rideIndex);
static void ride_call_mechanic(sint32 rideIndex, rct_peep *mechanic, sint32 forInspection);
static void ride_chairlift_update(Ride *ride);
static void ride_entrance_exit_connected(Ride* ride, sint32 ride_idx);
static sint32 ride_get_new_breakdown_problem(Ride *ride);
static void ride_inspection_update(Ride *ride);
static void ride_mechanic_status_update(sint32 rideIndex, sint32 mechanicStatus);
static void ride_music_update(sint32 rideIndex);
static void ride_shop_connected(Ride* ride, sint32 ride_idx);
static void ride_spiral_slide_update(Ride *ride);
static void ride_update(sint32 rideIndex);
static void ride_update_vehicle_colours(sint32 rideIndex);
static void ride_set_vehicle_colours_to_random_preset(Ride *ride, uint8 preset_index);
void loc_6DDF9C(Ride *ride, rct_map_element *mapElement);
static void ride_set_name_to_track_default(Ride * ride, rct_ride_entry * rideEntry);
static void ride_set_name_to_vehicle_default(Ride * ride, rct_ride_entry * rideEntry);


Ride *get_ride(sint32 index)
{
    if (index < 0 || index >= MAX_RIDES)
    {
        log_error("invalid index %d for ride", index);
        return NULL;
    }
    return &gRideList[index];
}

rct_ride_entry *get_ride_entry(sint32 index)
{
    if (index < 0 || index >= object_entry_group_counts[OBJECT_TYPE_RIDE])
    {
        log_error("invalid index %d for ride type", index);
        return NULL;
    }
    return gRideEntries[index];
}

void get_ride_entry_name(char *name, sint32 index)
{
    if (index < 0 || index >= object_entry_group_counts[OBJECT_TYPE_RIDE])
    {
        log_error("invalid index %d for ride type", index);
        return;
    }

    const char *entryName = object_entry_groups[OBJECT_TYPE_RIDE].entries[index].name;
    memcpy(name, entryName, 8);
    name[8] = '\0';
}

rct_ride_measurement *get_ride_measurement(sint32 index)
{
    return &gRideMeasurements[index];
}

rct_ride_entry *get_ride_entry_by_ride(Ride *ride)
{
    rct_ride_entry *type = get_ride_entry(ride->subtype);
    if (type == NULL)
    {
        char oldname[128];
        format_string(oldname, 128, ride->name, &ride->name_arguments);
        log_error("Invalid ride subtype for ride %s", oldname);
    }
    return type;
}

/**
*
*  rct2: 0x006DED68
*/
void reset_type_to_ride_entry_index_map()
{
    uint8 maxRideEntries = 128;
    size_t stride = maxRideEntries + 1;
    uint8 * entryTypeTable = malloc(RIDE_TYPE_COUNT * stride);
    memset(entryTypeTable, 0xFF, RIDE_TYPE_COUNT * stride);

    for (uint8 i = 0; i < maxRideEntries; i++) {
        rct_ride_entry * rideEntry = get_ride_entry(i);
        if (rideEntry == (rct_ride_entry *)-1) {
            continue;
        }
        for (uint8 j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++) {
            uint8 rideType = rideEntry->ride_type[j];
            if (rideType < RIDE_TYPE_COUNT) {
                uint8 * entryArray = &entryTypeTable[rideType * stride];
                uint8 * nextEntry = memchr(entryArray, 0xFF, stride);
                *nextEntry = i;
            }
        }
    }

    uint8 * dst = gTypeToRideEntryIndexMap;
    for (uint8 i = 0; i < RIDE_TYPE_COUNT; i++) {
        uint8 * entryArray = &entryTypeTable[i * stride];
        uint8 * entry = entryArray;
        while (*entry != 0xFF) {
            *dst++ = *entry++;
        }
        *dst++ = 0xFF;
    }

    free(entryTypeTable);
}

uint8 *get_ride_entry_indices_for_ride_type(uint8 rideType)
{
    uint8 *entryIndexList = gTypeToRideEntryIndexMap;
    while (rideType > 0) {
        do {
            entryIndexList++;
        } while (*(entryIndexList - 1) != 255);
        rideType--;
    }
    return entryIndexList;
}

sint32 ride_get_count()
{
    Ride *ride;
    sint32 i, count = 0;

    FOR_ALL_RIDES(i, ride)
        count++;

    return count;
}

sint32 ride_get_total_queue_length(Ride *ride)
{
    sint32 i, queueLength = 0;
    for (i = 0; i < MAX_STATIONS; i++)
        if (ride->entrances[i].xy != RCT_XY8_UNDEFINED)
            queueLength += ride->queue_length[i];
    return queueLength;
}

sint32 ride_get_max_queue_time(Ride *ride)
{
    sint32 i, queueTime = 0;
    for (i = 0; i < MAX_STATIONS; i++)
        if (ride->entrances[i].xy != RCT_XY8_UNDEFINED)
            queueTime = max(queueTime, ride->queue_time[i]);
    return queueTime;
}

rct_peep * ride_get_queue_head_guest(Ride * ride, sint32 stationIndex)
{
    rct_peep * peep;
    rct_peep * result = NULL;
    uint16 spriteIndex = ride->last_peep_in_queue[stationIndex];
    while ((peep = try_get_guest(spriteIndex)) != NULL) {
        spriteIndex = peep->next_in_queue;
        result = peep;
    }
    return result;
}

static void ride_update_queue_length(Ride * ride, sint32 stationIndex)
{
    uint16 count = 0;
    rct_peep * peep;
    uint16 spriteIndex = ride->last_peep_in_queue[stationIndex];
    while ((peep = try_get_guest(spriteIndex)) != NULL) {
        spriteIndex = peep->next_in_queue;
        count++;
    }
    ride->queue_length[stationIndex] = count;
}

void ride_queue_insert_guest_at_front(Ride * ride, sint32 stationIndex, rct_peep * peep)
{
    assert(ride != NULL);
    assert(stationIndex < MAX_STATIONS);
    assert(peep != NULL);

    peep->next_in_queue = SPRITE_INDEX_NULL;
    rct_peep * queueHeadGuest = ride_get_queue_head_guest(ride, peep->current_ride_station);
    if (queueHeadGuest == NULL) {
        ride->last_peep_in_queue[peep->current_ride_station] = peep->sprite_index;
    } else {
        queueHeadGuest->next_in_queue = peep->sprite_index;
    }
    ride_update_queue_length(ride, peep->current_ride_station);
}

/**
 *
 *  rct2: 0x006AC916
 */
void ride_update_favourited_stat()
{
    sint32 i;
    Ride *ride;
    uint16 spriteIndex;
    rct_peep* peep;

    FOR_ALL_RIDES(i, ride)
        ride->guests_favourite = 0;

    FOR_ALL_PEEPS(spriteIndex, peep) {
        if (peep->linked_list_type_offset != SPRITE_LIST_PEEP * 2)
            return;
        if (peep->favourite_ride != 0xff) {
            ride = &gRideList[peep->favourite_ride];
            ride->guests_favourite++;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

        }

    }

    window_invalidate_by_class(WC_RIDE_LIST);
}

money32 get_shop_item_cost(sint32 shopItem)
{
    return ShopItemStats[shopItem].cost;
}

money16 get_shop_base_value(sint32 shopItem)
{
    return ShopItemStats[shopItem].base_value;
}

money16 get_shop_cold_value(sint32 shopItem)
{
    return ShopItemStats[shopItem].cold_value;
}

money16 get_shop_hot_value(sint32 shopItem)
{
    return ShopItemStats[shopItem].hot_value;
}

/**
 *
 *  rct2: 0x006AC3AB
 */
static money32 ride_calculate_income_per_hour(Ride *ride)
{
    // Get entry by ride to provide better reporting
    rct_ride_entry *entry = get_ride_entry_by_ride(ride);
    if (entry == NULL || entry == (rct_ride_entry*)-1) {
        return 0;
    }
    money32 customersPerHour = ride_customers_per_hour(ride);
    money32 priceMinusCost = ride_get_price(ride);

    sint32 currentShopItem = entry->shop_item;
    if (currentShopItem != SHOP_ITEM_NONE) {
        priceMinusCost -= get_shop_item_cost(currentShopItem);
    }

    currentShopItem = (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) ?
        RidePhotoItems[ride->type] :
        entry->shop_item_secondary;

    if (currentShopItem != SHOP_ITEM_NONE) {
        priceMinusCost += ride->price_secondary;
        priceMinusCost -= get_shop_item_cost(currentShopItem);

        if (entry->shop_item != SHOP_ITEM_NONE)
            priceMinusCost /= 2;
    }

    return customersPerHour * priceMinusCost;
}

/**
 *
 *  rct2: 0x006CAF80
 * ax result x
 * bx result y
 * dl ride index
 * esi result map element
 */
bool ride_try_get_origin_element(sint32 rideIndex, rct_xy_element *output)
{
    rct_map_element *resultMapElement = NULL;

    map_element_iterator it;
    map_element_iterator_begin(&it);
    do {
        if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_TRACK)
            continue;
        if (rideIndex != it.element->properties.track.ride_index)
            continue;

        // Found a track piece for target ride

        // Check if it's not the station or ??? (but allow end piece of station)
        bool specialTrackPiece = (
            it.element->properties.track.type != TRACK_ELEM_BEGIN_STATION &&
            it.element->properties.track.type != TRACK_ELEM_MIDDLE_STATION &&
            (TrackSequenceProperties[it.element->properties.track.type][0] & TRACK_SEQUENCE_FLAG_ORIGIN)
        );

        // Set result tile to this track piece if first found track or a ???
        if (resultMapElement == NULL || specialTrackPiece) {
            resultMapElement = it.element;

            if (output != NULL) {
                output->element = resultMapElement;
                output->x = it.x * 32;
                output->y = it.y * 32;
            }
        }

        if (specialTrackPiece) {
            return true;
        }
    } while (map_element_iterator_next(&it));

    return resultMapElement != NULL;
}

/**
*
* rct2: 0x006C6096
* Gets the next track block coordinates from the
* coordinates of the first of element of a track block.
* Use track_block_get_next if you are unsure if you are
* on the first element of a track block
*/
bool track_block_get_next_from_zero(sint16 x, sint16 y, sint16 z_start, uint8 rideIndex, uint8 direction_start, rct_xy_element *output, sint32 *z, sint32 *direction)
{
    Ride* ride = get_ride(rideIndex);

    if (!(direction_start & (1 << 2))){
        x += TileDirectionDelta[direction_start].x;
        y += TileDirectionDelta[direction_start].y;
    }

    rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);
    if (mapElement == NULL){
        output->element = NULL;
        output->x = MAP_LOCATION_NULL;
        return 0;
    }

    do{
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
            continue;

        if (mapElement->properties.track.ride_index != rideIndex)
            continue;

        if (map_element_get_track_sequence(mapElement) != 0)
            continue;

        const rct_preview_track* nextTrackBlock = get_track_def_from_ride(ride, mapElement->properties.track.type);
        const rct_track_coordinates* nextTrackCoordinate = get_track_coord_from_ride(ride, mapElement->properties.track.type);

        uint8 nextRotation =
            map_element_get_direction_with_offset(mapElement, nextTrackCoordinate->rotation_begin) |
            (nextTrackCoordinate->rotation_begin & (1 << 2));

        if (nextRotation != direction_start)
            continue;

        sint16 nextZ = nextTrackCoordinate->z_begin - nextTrackBlock->z + mapElement->base_height * 8;
        if (nextZ != z_start)
            continue;

        if (z != NULL) *z = mapElement->base_height * 8;
        if (direction != NULL) *direction = nextRotation;
        output->x = x;
        output->y = y;
        output->element = mapElement;
        return 1;
    } while (!map_element_is_last_for_tile(mapElement++));

    if (direction != NULL) *direction = direction_start;
    if (z != NULL) *z = z_start;
    output->x = x;
    output->y = y;
    output->element = --mapElement;
    return 0;
}

/**
 *
 *  rct2: 0x006C60C2
 */
bool track_block_get_next(rct_xy_element *input, rct_xy_element *output, sint32 *z, sint32 *direction)
{
    uint8 rideIndex = input->element->properties.track.ride_index;
    Ride* ride = get_ride(rideIndex);

    const rct_preview_track* trackBlock = get_track_def_from_ride(ride, input->element->properties.track.type);
    uint8 sequence = map_element_get_track_sequence(input->element);
    trackBlock += sequence;

    const rct_track_coordinates* trackCoordinate = get_track_coord_from_ride(ride, input->element->properties.track.type);

    sint32 x = input->x;
    sint32 y = input->y;
    sint32 OriginZ = input->element->base_height * 8;

    uint8 rotation = map_element_get_direction(input->element);
    switch (rotation){
    case 0:
        x += trackCoordinate->x;
        x -= trackBlock->x;
        y += trackCoordinate->y;
        y -= trackBlock->y;
        break;
    case 1:
        x += trackCoordinate->y;
        x -= trackBlock->y;
        y -= trackCoordinate->x;
        y += trackBlock->x;
        break;
    case 2:
        x -= trackCoordinate->x;
        x += trackBlock->x;
        y -= trackCoordinate->y;
        y += trackBlock->y;
        break;
    case 3:
        x -= trackCoordinate->y;
        x += trackBlock->y;
        y += trackCoordinate->x;
        y -= trackBlock->x;
        break;
    }

    OriginZ -= trackBlock->z;
    OriginZ += trackCoordinate->z_end;

    uint8 directionStart = ((trackCoordinate->rotation_end + rotation) & MAP_ELEMENT_DIRECTION_MASK) |
        (trackCoordinate->rotation_end & (1 << 2));

    return track_block_get_next_from_zero(x, y, OriginZ, rideIndex, directionStart, output, z, direction);
}

/**
 * Returns the begin position / direction and end position / direction of the
 * track piece that proceeds the given location. Gets the previous track block
 * coordinates from the coordinates of the first of element of a track block.
 * Use track_block_get_previous if you are unsure if you are on the first
 * element of a track block
 *  rct2: 0x006C63D6
 */
bool track_block_get_previous_from_zero(sint16 x, sint16 y, sint16 z, uint8 rideIndex, uint8 direction, track_begin_end *outTrackBeginEnd){
    Ride* ride = get_ride(rideIndex);

    uint8 directionStart = direction;
    direction ^= (1 << 1);

    if (!(direction & (1 << 2))){
        x += TileDirectionDelta[direction].x;
        y += TileDirectionDelta[direction].y;
    }

    rct_map_element* mapElement = map_get_first_element_at(x / 32, y / 32);
    if (mapElement == NULL){
        outTrackBeginEnd->end_x = x;
        outTrackBeginEnd->end_y = y;
        outTrackBeginEnd->begin_element = NULL;
        outTrackBeginEnd->begin_direction = directionStart ^ (1 << 1);
        return 0;
    }

    do{
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
            continue;

        if (mapElement->properties.track.ride_index != rideIndex)
            continue;

        const rct_preview_track* nextTrackBlock = get_track_def_from_ride(ride, mapElement->properties.track.type);
        const rct_track_coordinates* nextTrackCoordinate = get_track_coord_from_ride(ride, mapElement->properties.track.type);

        nextTrackBlock += map_element_get_track_sequence(mapElement);
        if ((nextTrackBlock + 1)->index != 255)
            continue;

        uint8 nextRotation =
            map_element_get_direction_with_offset(mapElement, nextTrackCoordinate->rotation_end) |
            (nextTrackCoordinate->rotation_end & (1 << 2));

        if (nextRotation != directionStart)
            continue;

        sint16 nextZ = nextTrackCoordinate->z_end - nextTrackBlock->z + mapElement->base_height * 8;
        if (nextZ != z)
            continue;

        nextRotation =
            map_element_get_direction_with_offset(mapElement, nextTrackCoordinate->rotation_begin) |
            (nextTrackCoordinate->rotation_begin & (1 << 2));
        outTrackBeginEnd->begin_element = mapElement;
        outTrackBeginEnd->begin_x = x;
        outTrackBeginEnd->begin_y = y;
        outTrackBeginEnd->end_x = x;
        outTrackBeginEnd->end_y = y;
        switch (nextRotation & 3){
        case 0:
            outTrackBeginEnd->begin_x -= nextTrackCoordinate->x;
            outTrackBeginEnd->begin_y -= nextTrackCoordinate->y;
            break;
        case 1:
            outTrackBeginEnd->begin_x -= nextTrackCoordinate->y;
            outTrackBeginEnd->begin_y += nextTrackCoordinate->x;
            break;
        case 2:
            outTrackBeginEnd->begin_x += nextTrackCoordinate->x;
            outTrackBeginEnd->begin_y += nextTrackCoordinate->y;
            break;
        case 3:
            outTrackBeginEnd->begin_x += nextTrackCoordinate->y;
            outTrackBeginEnd->begin_y -= nextTrackCoordinate->x;
            break;
        }

        outTrackBeginEnd->begin_z = mapElement->base_height * 8;
        outTrackBeginEnd->begin_z +=
            get_track_def_from_ride(ride, mapElement->properties.track.type)->z -
            nextTrackBlock->z;

        outTrackBeginEnd->begin_direction = nextRotation;
        outTrackBeginEnd->end_direction = directionStart ^ (1 << 1);
        return 1;
    } while (!map_element_is_last_for_tile(mapElement++));

    outTrackBeginEnd->end_x = x;
    outTrackBeginEnd->end_y = y;
    outTrackBeginEnd->begin_z = z;
    outTrackBeginEnd->begin_element = NULL;
    outTrackBeginEnd->end_direction = directionStart ^ (1 << 1);
    return 0;
}

/**
 *
 *  rct2: 0x006C6402
 *
 * @remarks outTrackBeginEnd.begin_x and outTrackBeginEnd.begin_y will be in the
 * higher two bytes of ecx and edx where as outTrackBeginEnd.end_x and
 * outTrackBeginEnd.end_y will be in the lower two bytes (cx and dx).
 */
bool track_block_get_previous(sint32 x, sint32 y, rct_map_element *mapElement, track_begin_end *outTrackBeginEnd)
{
    uint8 rideIndex = mapElement->properties.track.ride_index;
    Ride* ride = get_ride(rideIndex);

    const rct_preview_track* trackBlock = get_track_def_from_ride(ride, mapElement->properties.track.type);
    uint8 sequence = map_element_get_track_sequence(mapElement);
    trackBlock += sequence;

    const rct_track_coordinates* trackCoordinate = get_track_coord_from_ride(ride, mapElement->properties.track.type);

    sint32 z = mapElement->base_height * 8;

    uint8 rotation = map_element_get_direction(mapElement);
    switch (rotation){
    case 0:
        x -= trackBlock->x;
        y -= trackBlock->y;
        break;
    case 1:
        x -= trackBlock->y;
        y += trackBlock->x;
        break;
    case 2:
        x += trackBlock->x;
        y += trackBlock->y;
        break;
    case 3:
        x += trackBlock->y;
        y -= trackBlock->x;
        break;
    }

    z -= trackBlock->z;
    z += trackCoordinate->z_begin;

    rotation =
        ((trackCoordinate->rotation_begin + rotation) & MAP_ELEMENT_DIRECTION_MASK) |
        (trackCoordinate->rotation_begin & (1 << 2));

    return track_block_get_previous_from_zero(x, y, z, rideIndex, rotation, outTrackBeginEnd);
}

/**
 *
 * Make sure to pass in the x and y of the start track element too.
 *  rct2: 0x006CB02F
 * ax result x
 * bx result y
 * esi input / output map element
 */
sint32 ride_find_track_gap(rct_xy_element *input, rct_xy_element *output)
{
    rct_window *w;
    Ride *ride;
    track_circuit_iterator it = { 0 }, slowIt;
    sint32 rideIndex;

    assert(map_element_get_type(input->element) == MAP_ELEMENT_TYPE_TRACK);
    rideIndex = input->element->properties.track.ride_index;
    ride = get_ride(rideIndex);

    if (ride->type == RIDE_TYPE_MAZE)
        return 0;

    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != NULL && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && _currentRideIndex == rideIndex)
        ride_construction_invalidate_current_track();

    bool counter = true;
    track_circuit_iterator_begin(&it, *input);
    slowIt = it;
    while (track_circuit_iterator_next(&it)) {
        if (!track_is_connected_by_shape(it.last.element, it.current.element)) {
            *output = it.current;
            return 1;
        }
        //#2081: prevent an infinite loop
        counter = !counter;
        if (counter) {
            track_circuit_iterator_next(&slowIt);
            if (slowIt.currentZ == it.currentZ &&
                slowIt.currentDirection == it.currentDirection &&
                slowIt.current.x == it.current.x &&
                slowIt.current.y == it.current.y) {
                *output = it.current;
                return 1;
            }
        }

    }
    if (!it.looped) {
        *output = it.last;
        return 1;
    }

    return 0;
}

/**
 *
 *  rct2: 0x006AF561
 */
void ride_get_status(sint32 rideIndex, rct_string_id *formatSecondary, sint32 *argument)
{
    Ride *ride = get_ride(rideIndex);

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED) {
        *formatSecondary = STR_CRASHED;
        return;
    }
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
        *formatSecondary = STR_BROKEN_DOWN;
        return;
    }
    if (ride->status == RIDE_STATUS_CLOSED) {
        *formatSecondary = STR_CLOSED;

        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP)) {
            *argument = ride->num_riders;

            if (*argument == 1) {
                *formatSecondary = STR_CLOSED_WITH_PERSON;
            } else if (*argument > 1) {
                *formatSecondary = STR_CLOSED_WITH_PEOPLE;
            }
        }

        return;
    }
    if (ride->status == RIDE_STATUS_TESTING) {
        *formatSecondary = STR_TEST_RUN;
        return;
    }
    if (ride->mode == RIDE_MODE_RACE && !(ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING) && ride->race_winner != SPRITE_INDEX_NULL && (GET_PEEP(ride->race_winner))->sprite_identifier == SPRITE_IDENTIFIER_PEEP) {
        rct_peep *peep = GET_PEEP(ride->race_winner);
        if (peep->name_string_idx == STR_GUEST_X) {
            *argument = peep->id;
            *formatSecondary = STR_RACE_WON_BY_GUEST;
        } else {
            *argument = peep->name_string_idx;
            *formatSecondary = STR_RACE_WON_BY;
        }
    } else {
        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP)) {
            *argument = ride->num_riders;
            *formatSecondary = STR_PERSON_ON_RIDE;
            if (*argument != 1)
                *formatSecondary = STR_PEOPLE_ON_RIDE;

        } else {
            *formatSecondary = STR_OPEN;
        }
    }
}

sint32 ride_get_total_length(Ride *ride)
{
    sint32 i, totalLength = 0;
    for (i = 0; i < ride->num_stations; i++)
        totalLength += ride->length[i];
    return totalLength;
}

sint32 ride_get_total_time(Ride *ride)
{
    sint32 i, totalTime = 0;
    for (i = 0; i < ride->num_stations; i++)
        totalTime += ride->time[i];
    return totalTime;
}

sint32 ride_can_have_multiple_circuits(Ride *ride)
{
    if (!(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_MULTIPLE_CIRCUITS))
        return 0;

    // Only allow circuit or launch modes
    if (
        ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT &&
        ride->mode != RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE &&
        ride->mode != RIDE_MODE_POWERED_LAUNCH_PASSTROUGH
    ) {
        return 0;
    }

    // Must have no more than one vehicle and one station
    if (ride->num_vehicles > 1 || ride->num_stations > 1)
        return 0;

    return 1;
}

#pragma region Initialisation functions

/**
 *
 *  rct2: 0x006ACA89
 */
void ride_init_all()
{
    for (sint32 i = 0; i < MAX_RIDES; i++) {
        Ride *ride = get_ride(i);
        ride->type = RIDE_TYPE_NULL;
    }

    for (sint32 i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
        rct_ride_measurement *ride_measurement = get_ride_measurement(i);
        ride_measurement->ride_index = 255;
    }
}

/**
*
*  rct2: 0x006B7A38
*/
void reset_all_ride_build_dates()
{
    sint32 i;
    Ride *ride;

    FOR_ALL_RIDES(i, ride)
        ride->build_date -= gDateMonthsElapsed;
}

#pragma endregion

#pragma region Construction

static sint32 ride_check_if_construction_allowed(Ride *ride)
{
    rct_ride_entry *rideType = get_ride_entry_by_ride(ride);
    if (rideType == NULL) {
        window_error_open(STR_INVALID_RIDE_TYPE, STR_CANT_EDIT_INVALID_RIDE_TYPE);
        return 0;
    }
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);
        window_error_open(STR_CANT_START_CONSTRUCTION_ON, STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING);
        return 0;
    }

    if (ride->status != RIDE_STATUS_CLOSED) {
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);
        window_error_open(STR_CANT_START_CONSTRUCTION_ON, STR_MUST_BE_CLOSED_FIRST);
        return 0;
    }

    return 1;
}

static rct_window *ride_create_or_find_construction_window(sint32 rideIndex)
{
    rct_window *w;

    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == NULL || w->number != rideIndex) {
        window_close_construction_windows();
        _currentRideIndex = rideIndex;
        w = window_ride_construction_open();
    } else {
        ride_construction_invalidate_current_track();
        _currentRideIndex = rideIndex;
    }

    return w;
}

static sint32 ride_create_ride(ride_list_item listItem)
{
    sint32 eax, ebx, ecx, edx, esi, edi, ebp;
    edx = listItem.ride_type_and_entry;
    eax = 0;
    ecx = 0;
    ebx = GAME_COMMAND_FLAG_APPLY;
    edi = 0;
    esi = GAME_COMMAND_CREATE_RIDE;
    ebp = 0;

    gGameCommandErrorTitle = STR_CANT_CREATE_NEW_RIDE_ATTRACTION;

    game_do_command_p(esi, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
    return ebx == MONEY32_UNDEFINED ? -1 : edi;
}

/**
 *
 *  rct2: 0x006B4800
 */
void ride_construct_new(ride_list_item listItem)
{
    game_command_callback = game_command_callback_ride_construct_new;
    ride_create_ride(listItem);
}

/**
 *
 *  rct2: 0x006B4857
 */
void ride_construct(sint32 rideIndex)
{
    rct_xy_element trackElement;

    if (ride_try_get_origin_element(rideIndex, &trackElement)) {
        ride_find_track_gap(&trackElement, &trackElement);

        rct_window *w = window_get_main();
        if (w != NULL && ride_modify(&trackElement))
            window_scroll_to_location(w, trackElement.x, trackElement.y, trackElement.element->base_height * 8);
    } else {
        ride_initialise_construction_window(rideIndex);
    }
}

/**
 *
 *  rct2: 0x006DD4D5
 */
static void ride_remove_cable_lift(Ride *ride)
{
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT) {
        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CABLE_LIFT;
        uint16 spriteIndex = ride->cable_lift;
        do {
            rct_vehicle *vehicle = GET_VEHICLE(spriteIndex);
            invalidate_sprite_2((rct_sprite*)vehicle);
            sprite_remove((rct_sprite*)vehicle);
            spriteIndex = vehicle->next_vehicle_on_train;
        } while (spriteIndex != SPRITE_INDEX_NULL);
    }
}

/**
 *
 *  rct2: 0x006DD506
 */
static void ride_remove_vehicles(Ride *ride)
{
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) {
        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_ON_TRACK;
        ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_TEST_IN_PROGRESS | RIDE_LIFECYCLE_HAS_STALLED_VEHICLE);

        for (size_t i = 0; i < MAX_VEHICLES_PER_RIDE; i++) {
            uint16 spriteIndex = ride->vehicles[i];
            while (spriteIndex != SPRITE_INDEX_NULL) {
                rct_vehicle *vehicle = GET_VEHICLE(spriteIndex);
                invalidate_sprite_2((rct_sprite*)vehicle);
                sprite_remove((rct_sprite*)vehicle);
                spriteIndex = vehicle->next_vehicle_on_train;
            }

            ride->vehicles[i] = SPRITE_INDEX_NULL;
        }

        for (size_t i = 0; i < MAX_STATIONS; i++)
            ride->train_at_station[i] = 255;
    }
}

/**
 *
 *  rct2: 0x006DD4AC
 */
void ride_clear_for_construction(sint32 rideIndex)
{
    Ride *ride;
    rct_window *w;

    ride = get_ride(rideIndex);

    ride_measurement_clear(ride);

    ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;

    // Open circuit rides will go directly into building mode (creating ghosts) where it would normally clear the stats,
    // however this causes desyncs since it's directly run from the window and other clients would not get it.
    // To prevent these problems, unconditionally invalidate the test results on all clients in multiplayer games.
    if (network_get_mode() != NETWORK_MODE_NONE) {
        invalidate_test_results(rideIndex);
    }

    ride_remove_cable_lift(ride);
    ride_remove_vehicles(ride);

    w = window_find_by_number(WC_RIDE, rideIndex);
    if (w != NULL)
        window_event_resize_call(w);
}

/**
 *
 *  rct2: 0x006664DF
 */
void ride_remove_peeps(sint32 rideIndex)
{
    Ride *ride = get_ride(rideIndex);

    // Find first station
    sint8 stationIndex = ride_get_first_valid_station_start(ride);

    // Get exit position and direction
    sint32 exitX = 0;
    sint32 exitY = 0;
    sint32 exitZ = 0;
    sint32 exitDirection = 255;
    if (stationIndex != -1) {
        rct_xy8 location = ride->exits[stationIndex];
        if (location.xy != RCT_XY8_UNDEFINED) {
            exitX = location.x;
            exitY = location.y;
            exitZ = ride->station_heights[stationIndex];
            rct_map_element *mapElement = ride_get_station_exit_element(ride, exitX, exitY, exitZ);

            exitDirection = (mapElement == NULL ? 0 : map_element_get_direction(mapElement));
            exitX = (exitX * 32) - (word_981D6C[exitDirection].x * 20) + 16;
            exitY = (exitY * 32) - (word_981D6C[exitDirection].y * 20) + 16;
            exitZ = (exitZ * 8) + 2;

            // Reverse direction
            exitDirection ^= 2;

            exitDirection *= 8;
        }
    }

    // Place all the peeps at exit
    uint16 spriteIndex;
    rct_peep *peep;
    FOR_ALL_PEEPS(spriteIndex, peep) {
        if (
            peep->state == PEEP_STATE_QUEUING_FRONT ||
            peep->state == PEEP_STATE_ENTERING_RIDE ||
            peep->state == PEEP_STATE_LEAVING_RIDE ||
            peep->state == PEEP_STATE_ON_RIDE
        ) {
            if (peep->current_ride != rideIndex)
                continue;

            peep_decrement_num_riders(peep);
            if (peep->state == PEEP_STATE_QUEUING_FRONT && peep->sub_state == 0)
                remove_peep_from_queue(peep);

            invalidate_sprite_2((rct_sprite*)peep);

            if (exitDirection == 255) {
                sint32 x = peep->next_x + 16;
                sint32 y = peep->next_y + 16;
                sint32 z = peep->next_z * 8;
                if (peep->next_var_29 & 4)
                    z += 8;
                z++;
                sprite_move(x, y, z, (rct_sprite*)peep);
            } else {
                sprite_move(exitX, exitY, exitZ, (rct_sprite*)peep);
                peep->sprite_direction = exitDirection;
            }

            invalidate_sprite_2((rct_sprite*)peep);
            peep->state = PEEP_STATE_FALLING;
            peep_switch_to_special_sprite(peep, 0);

            peep->happiness = min(peep->happiness, peep->happiness_target) / 2;
            peep->happiness_target = peep->happiness;
            peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_STATS;
        }
    }

    ride->num_riders = 0;
    ride->slide_in_use = 0;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN;
}

/**
 * Gets the origin track element (sequence 0). Seems to do more than that though and even invalidates track.
 *  rct2: 0x006C683D
 * ax : x
 * bx : direction << 8, type
 * cx : y
 * dx : z
 * si : extra_params
 * di : output_element
 * bp : flags
 */
sint32 sub_6C683D(sint32* x, sint32* y, sint32* z, sint32 direction, sint32 type, uint16 extra_params, rct_map_element** output_element, uint16 flags)
{
    rct_map_element *mapElement = map_get_first_element_at(*x / 32, *y / 32);
    rct_map_element *successMapElement = NULL;

    do {
        if (mapElement->base_height != *z / 8)
            continue;

        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
            continue;

        if ((map_element_get_direction(mapElement)) != direction)
            continue;

        if (type != mapElement->properties.track.type)
            continue;

        successMapElement = mapElement;
        if (map_element_get_track_sequence(mapElement) == 0)
            break;
    } while (!map_element_is_last_for_tile(mapElement++));

    mapElement = successMapElement;
    if (mapElement == NULL)
        return 1;

    // Possibly z should be & 0xF8
    const rct_preview_track *trackBlock = get_track_def_from_ride_index(mapElement->properties.track.ride_index, type);

    sint32 sequence = map_element_get_track_sequence(mapElement);
    uint8 mapDirection = map_element_get_direction(mapElement);

    switch (mapDirection){
    case MAP_ELEMENT_DIRECTION_WEST:
        *x -= trackBlock[sequence].x;
        *y -= trackBlock[sequence].y;
        break;
    case MAP_ELEMENT_DIRECTION_NORTH:
        *x -= trackBlock[sequence].y;
        *y += trackBlock[sequence].x;
        break;
    case MAP_ELEMENT_DIRECTION_EAST:
        *x += trackBlock[sequence].x;
        *y += trackBlock[sequence].y;
        break;
    case MAP_ELEMENT_DIRECTION_SOUTH:
        *x += trackBlock[sequence].y;
        *y -= trackBlock[sequence].x;
        break;
    }
    *z -= trackBlock[sequence].z;

    sint32 start_x = *x, start_y = *y, start_z = *z;
    *z += trackBlock[0].z;
    for (sint32 i = 0; trackBlock[i].index != 0xFF; ++i){
        sint32 cur_x = start_x, cur_y = start_y, cur_z = start_z;
        switch (mapDirection){
        case MAP_ELEMENT_DIRECTION_WEST:
            cur_x += trackBlock[i].x;
            cur_y += trackBlock[i].y;
            break;
        case MAP_ELEMENT_DIRECTION_NORTH:
            cur_x += trackBlock[i].y;
            cur_y -= trackBlock[i].x;
            break;
        case MAP_ELEMENT_DIRECTION_EAST:
            cur_x -= trackBlock[i].x;
            cur_y -= trackBlock[i].y;
            break;
        case MAP_ELEMENT_DIRECTION_SOUTH:
            cur_x -= trackBlock[i].y;
            cur_y += trackBlock[i].x;
            break;
        }
        cur_z += trackBlock[i].z;

        map_invalidate_tile_full(cur_x, cur_y);

        mapElement = map_get_first_element_at(cur_x / 32, cur_y / 32);
        successMapElement = NULL;
        do {
            if (mapElement->base_height != cur_z / 8)
                continue;

            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
                continue;

            if ((map_element_get_direction(mapElement)) != direction)
                continue;

            if (map_element_get_track_sequence(mapElement) != trackBlock[i].index)
                continue;

            if (type == mapElement->properties.track.type) {
                successMapElement = mapElement;
                break;
            }
        } while (!map_element_is_last_for_tile(mapElement++));

        if (successMapElement == NULL) {
            return 1;
        }
        if (i == 0 && output_element != NULL) {
            *output_element = mapElement;
        }
        if (flags & (1 << 0)) {
            // Switch highlight off
            mapElement->type &= ~MAP_ELEMENT_TYPE_FLAG_HIGHLIGHT;
        }
        if (flags & (1 << 1)) {
            // Switch highlight on
            mapElement->type |= MAP_ELEMENT_TYPE_FLAG_HIGHLIGHT;
        }
        if (flags & (1 << 2)) {
            mapElement->properties.track.colour &= 0xFC;
            mapElement->properties.track.colour |= extra_params & 0xFF;
        }
        if (flags & (1 << 5)) {
            // Seat rotation
            mapElement->properties.track.colour &= 0x0F;
            mapElement->properties.track.colour |= (extra_params & 0xFF) << 4;
        }

        if (flags & (1 << 3)) {
            track_element_set_cable_lift(mapElement);
        }
        if (flags & (1 << 4)) {
            track_element_clear_cable_lift(mapElement);
        }
    }

    return 0;
}

void ride_restore_provisional_track_piece()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK) {
        sint32 x, y, z, direction, type, rideIndex, liftHillAndAlternativeState;
        if (sub_6CA2DF(&type, &direction, &rideIndex, &liftHillAndAlternativeState, &x, &y, &z, NULL)) {
            ride_construction_remove_ghosts();
        } else {
            _currentTrackPrice = place_provisional_track_piece(rideIndex, type, direction, liftHillAndAlternativeState, x, y, z);
            window_ride_construction_update_active_elements();
        }
    }
}

void ride_remove_provisional_track_piece()
{
    if (!(_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK)) {
        return;
    }
    Ride *ride;
    sint32 rideIndex, x, y, z, direction;

    rideIndex = _currentRideIndex;

    x = _unkF440C5.x;
    y = _unkF440C5.y;
    z = _unkF440C5.z;

    ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_MAZE) {
        sint32 flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5;
        game_do_command(x     , flags | (0 << 8), y     , rideIndex | (2 << 8), GAME_COMMAND_SET_MAZE_TRACK, z, 0);
        game_do_command(x     , flags | (1 << 8), y + 16, rideIndex | (2 << 8), GAME_COMMAND_SET_MAZE_TRACK, z, 0);
        game_do_command(x + 16, flags | (2 << 8), y + 16, rideIndex | (2 << 8), GAME_COMMAND_SET_MAZE_TRACK, z, 0);
        game_do_command(x + 16, flags | (3 << 8), y     , rideIndex | (2 << 8), GAME_COMMAND_SET_MAZE_TRACK, z, 0);
    } else {
        direction = _unkF440C5.direction;
        if (!(direction & 4)) {
            x -= TileDirectionDelta[direction].x;
            y -= TileDirectionDelta[direction].y;
        }
        rct_xy_element next_track;
        if (track_block_get_next_from_zero(x, y, z, rideIndex, direction, &next_track, &z, &direction)) {
            sint32 flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_GHOST;
            game_do_command(
                next_track.x,
                flags | ((direction & 3) << 8),
                next_track.y,
                next_track.element->properties.track.type | (map_element_get_track_sequence(next_track.element) << 8),
                GAME_COMMAND_REMOVE_TRACK,
                z,
                0
            );
        }
    }
}

/**
 *
 *  rct2: 0x006C96C0
 */
void ride_construction_remove_ghosts()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT) {
        ride_entrance_exit_remove_ghost();
        _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT;
    }
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK) {
        ride_remove_provisional_track_piece();
        _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_TRACK;
    }
}

/*
*  rct2: 0x006C9627
*/
void ride_construction_invalidate_current_track()
{
    sint32 x, y, z;

    switch (_rideConstructionState) {
    case RIDE_CONSTRUCTION_STATE_SELECTED:
        x = _currentTrackBeginX;
        y = _currentTrackBeginY;
        z = _currentTrackBeginZ;
        sub_6C683D(
            &x,
            &y,
            &z,
            _currentTrackPieceDirection & 3,
            _currentTrackPieceType,
            0,
            NULL,
            1
        );
        break;
    case RIDE_CONSTRUCTION_STATE_MAZE_BUILD:
    case RIDE_CONSTRUCTION_STATE_MAZE_MOVE:
    case RIDE_CONSTRUCTION_STATE_MAZE_FILL:
        if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW) {
            map_invalidate_tile_full(
                _currentTrackBeginX & 0xFFE0,
                _currentTrackBeginY & 0xFFE0
            );
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
        }
        break;
    default:
        if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW) {
            _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_ARROW;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
            map_invalidate_tile_full(_currentTrackBeginX, _currentTrackBeginY);
        }
        ride_construction_remove_ghosts();
        break;
    }
}

/**
 *
 *  rct2: 0x006C9B19
 */
static void ride_construction_reset_current_piece()
{
    Ride *ride;

    ride = get_ride(_currentRideIndex);
    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_NO_TRACK) || ride->num_stations == 0) {
        _currentTrackCurve = RideConstructionDefaultTrackType[ride->type] | 0x100;
        _currentTrackSlopeEnd = 0;
        _currentTrackBankEnd = 0;
        _currentTrackLiftHill = 0;
        _currentTrackAlternative = RIDE_TYPE_NO_ALTERNATIVES;
        if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_START_CONSTRUCTION_INVERTED) {
            _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
        }
        _previousTrackSlopeEnd = 0;
        _previousTrackBankEnd = 0;
    } else {
        _currentTrackCurve = 0xFFFF;
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
    }
}

/**
 *
 *  rct2: 0x006C9800
 */
void ride_construction_set_default_next_piece()
{
    sint32 x, y, z, direction, rideIndex, trackType, curve, bank, slope;
    Ride *ride;
    track_begin_end trackBeginEnd;
    rct_xy_element xyElement;
    rct_map_element *mapElement;

    _currentTrackPrice = MONEY32_UNDEFINED;
    switch (_rideConstructionState) {
    case RIDE_CONSTRUCTION_STATE_FRONT:
        rideIndex = _currentRideIndex;
        ride = get_ride(rideIndex);

        x = _currentTrackBeginX;
        y = _currentTrackBeginY;
        z = _currentTrackBeginZ;
        direction = _currentTrackPieceDirection;
        if (!track_block_get_previous_from_zero(x, y, z, rideIndex, direction, &trackBeginEnd)) {
            ride_construction_reset_current_piece();
            return;
        }
        mapElement = trackBeginEnd.begin_element;
        trackType = mapElement->properties.track.type;

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_NO_TRACK)) {
            ride_construction_reset_current_piece();
            return;
        }

        // Set whether track is covered
        _currentTrackAlternative &= ~RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
        if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE) {
            if (mapElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED) {
                _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
            }
        }

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
            curve = gFlatRideTrackCurveChain[trackType].next;
            bank = FlatRideTrackDefinitions[trackType].bank_end;
            slope = FlatRideTrackDefinitions[trackType].vangle_end;
        } else {
            if (track_element_is_booster(ride->type, trackType)) {
                curve = 0x100 | TRACK_ELEM_BOOSTER;
            } else {
                curve = gTrackCurveChain[trackType].next;
            }
            bank = TrackDefinitions[trackType].bank_end;
            slope = TrackDefinitions[trackType].vangle_end;
        }

        // Set track curve
        _currentTrackCurve = curve;

        // Set track banking
        if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE) {
            if (bank == TRACK_BANK_UPSIDE_DOWN) {
                bank = TRACK_BANK_NONE;
                _currentTrackAlternative ^= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
            }
        }
        _currentTrackBankEnd = bank;
        _previousTrackBankEnd = bank;

        // Set track slope and lift hill
        _currentTrackSlopeEnd = slope;
        _previousTrackSlopeEnd = slope;
        _currentTrackLiftHill = ((mapElement->type & 0x80) && slope != TRACK_SLOPE_DOWN_25 && slope != TRACK_SLOPE_DOWN_60) != 0;
        break;
    case RIDE_CONSTRUCTION_STATE_BACK:
        rideIndex = _currentRideIndex;
        ride = get_ride(rideIndex);

        x = _currentTrackBeginX;
        y = _currentTrackBeginY;
        z = _currentTrackBeginZ;
        direction = _currentTrackPieceDirection ^ 2;
        if (!track_block_get_next_from_zero(x, y, z, rideIndex, direction, &xyElement, &z, &direction)) {
            ride_construction_reset_current_piece();
            return;
        }
        mapElement = xyElement.element;
        trackType = mapElement->properties.track.type;

        // Set whether track is covered
        _currentTrackAlternative &= ~RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
        if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE) {
            if (mapElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED) {
                _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
            }
        }

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
            curve = gFlatRideTrackCurveChain[trackType].previous;
            bank = FlatRideTrackDefinitions[trackType].bank_start;
            slope = FlatRideTrackDefinitions[trackType].vangle_start;
        } else {
            if (track_element_is_booster(ride->type, trackType)) {
                curve = 0x100 | TRACK_ELEM_BOOSTER;
            } else {
                curve = gTrackCurveChain[trackType].previous;
            }
            bank = TrackDefinitions[trackType].bank_start;
            slope = TrackDefinitions[trackType].vangle_start;
        }

        // Set track curve
        _currentTrackCurve = curve;

        // Set track banking
        if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE) {
            if (bank == TRACK_BANK_UPSIDE_DOWN) {
                bank = TRACK_BANK_NONE;
                _currentTrackAlternative ^= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
            }
        }
        _currentTrackBankEnd = bank;
        _previousTrackBankEnd = bank;

        // Set track slope and lift hill
        _currentTrackSlopeEnd = slope;
        _previousTrackSlopeEnd = slope;
        if (!gCheatsEnableChainLiftOnAllTrack) {
            _currentTrackLiftHill = track_element_is_lift_hill(mapElement);
        }
        break;
    }
}

/**
 *
 *  rct2: 0x006C9296
 */
void ride_select_next_section()
{
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED) {
        ride_construction_invalidate_current_track();
        sint32 x = _currentTrackBeginX;
        sint32 y = _currentTrackBeginY;
        sint32 z = _currentTrackBeginZ;
        sint32 direction = _currentTrackPieceDirection;
        sint32 type = _currentTrackPieceType;
        rct_map_element *mapElement;
        if (sub_6C683D(&x, &y, &z, direction & 3, type, 0, &mapElement, 0)) {
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
            window_ride_construction_update_active_elements();
            return;
        }
        rct_xy_element inputElement, outputElement;
        inputElement.x = x;
        inputElement.y = y;
        inputElement.element = mapElement;
        if (track_block_get_next(&inputElement, &outputElement, &z, &direction)) {
            x = outputElement.x;
            y = outputElement.y;
            mapElement = outputElement.element;
        } else {
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_FRONT;
            _currentTrackBeginX = outputElement.x;
            _currentTrackBeginY = outputElement.y;
            _currentTrackBeginZ = z;
            _currentTrackPieceDirection = direction;
            _currentTrackPieceType = mapElement->properties.track.type;
            _currentTrackSelectionFlags = 0;
            _rideConstructionArrowPulseTime = 0;
            ride_construction_set_default_next_piece();
            window_ride_construction_update_active_elements();
            return;
        }

        _currentTrackBeginX = x;
        _currentTrackBeginY = y;
        _currentTrackBeginZ = z;
        _currentTrackPieceDirection = map_element_get_direction(mapElement);
        _currentTrackPieceType = mapElement->properties.track.type;
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        window_ride_construction_update_active_elements();
    } else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
        if (ride_select_forwards_from_back()) {
            window_ride_construction_update_active_elements();
        }
    }
}

/**
 *
 *  rct2: 0x006C93B8
 */
void ride_select_previous_section()
{
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED) {
        ride_construction_invalidate_current_track();
        sint32 x = _currentTrackBeginX;
        sint32 y = _currentTrackBeginY;
        sint32 z = _currentTrackBeginZ;
        sint32 direction = _currentTrackPieceDirection;
        sint32 type = _currentTrackPieceType;
        rct_map_element *mapElement;
        if (sub_6C683D(&x, &y, &z, direction & 3, type, 0, &mapElement, 0)) {
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
            window_ride_construction_update_active_elements();
            return;
        }
        track_begin_end trackBeginEnd;
        if (track_block_get_previous(x, y, mapElement, &trackBeginEnd)) {
            _currentTrackBeginX = trackBeginEnd.begin_x;
            _currentTrackBeginY = trackBeginEnd.begin_y;
            _currentTrackBeginZ = trackBeginEnd.begin_z;
            _currentTrackPieceDirection = trackBeginEnd.begin_direction;
            _currentTrackPieceType = trackBeginEnd.begin_element->properties.track.type;
            _currentTrackSelectionFlags = 0;
            _rideConstructionArrowPulseTime = 0;
            window_ride_construction_update_active_elements();
        } else {
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_BACK;
            _currentTrackBeginX = trackBeginEnd.end_x;
            _currentTrackBeginY = trackBeginEnd.end_y;
            _currentTrackBeginZ = trackBeginEnd.begin_z;
            _currentTrackPieceDirection = trackBeginEnd.end_direction;
            _currentTrackPieceType = mapElement->properties.track.type;
            _currentTrackSelectionFlags = 0;
            _rideConstructionArrowPulseTime = 0;
            ride_construction_set_default_next_piece();
            window_ride_construction_update_active_elements();
        }
    } else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT) {
        if (ride_select_backwards_from_front()) {
            window_ride_construction_update_active_elements();
        }
    }
}

/**
 *
 *  rct2: 0x006CC2CA
 */
static sint32 ride_modify_entrance_or_exit(rct_map_element *mapElement, sint32 x, sint32 y)
{
    sint32 rideIndex, entranceType;
    rct_window *constructionWindow;

    rideIndex = mapElement->properties.entrance.ride_index;

    entranceType = mapElement->properties.entrance.type;
    if (entranceType != ENTRANCE_TYPE_RIDE_ENTRANCE && entranceType != ENTRANCE_TYPE_RIDE_EXIT)
        return 0;

    sint32 bl = (mapElement->properties.entrance.index & 0x70) >> 4;

    // Get or create construction window for ride
    constructionWindow = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (constructionWindow == NULL) {
        if (!ride_initialise_construction_window(rideIndex))
            return 0;

        constructionWindow = window_find_by_class(WC_RIDE_CONSTRUCTION);
        if (constructionWindow == NULL)
            return 0;
    }

    ride_construction_invalidate_current_track();
    if (
        _rideConstructionState != RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT ||
        !(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)) ||
        gCurrentToolWidget.window_classification != WC_RIDE_CONSTRUCTION
    ) {
        // Replace entrance / exit
        tool_set(constructionWindow, entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE ? WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE : WC_RIDE_CONSTRUCTION__WIDX_EXIT, TOOL_CROSSHAIR);
        gRideEntranceExitPlaceType = entranceType;
        gRideEntranceExitPlaceRideIndex = rideIndex;
        gRideEntranceExitPlaceStationIndex = bl;
        input_set_flag(INPUT_FLAG_6, true);
        if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT) {
            gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT;
        }

        window_ride_construction_update_active_elements();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    } else {
        // Remove entrance / exit
        game_do_command(x, (GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_APPLY), y, rideIndex, GAME_COMMAND_REMOVE_RIDE_ENTRANCE_OR_EXIT, bl, 0);
        gCurrentToolWidget.widget_index = entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE ? WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE : WC_RIDE_CONSTRUCTION__WIDX_EXIT;
        gRideEntranceExitPlaceType = entranceType;
    }

    window_invalidate_by_class(WC_RIDE_CONSTRUCTION);
    return 1;
}

/**
 *
 *  rct2: 0x006CC287
 */
static sint32 ride_modify_maze(rct_map_element *mapElement, sint32 x, sint32 y)
{
    _currentRideIndex = mapElement->properties.track.ride_index;
    _rideConstructionState = RIDE_CONSTRUCTION_STATE_MAZE_BUILD;
    _currentTrackBeginX = x;
    _currentTrackBeginY = y;
    _currentTrackBeginZ = mapElement->base_height * 8;
    _currentTrackSelectionFlags = 0;
    _rideConstructionArrowPulseTime = 0;
    window_maze_construction_update_pressed_widgets();
    return 1;
}

/**
 *
 *  rct2: 0x006CC056
 */
sint32 ride_modify(rct_xy_element *input)
{
    sint32 rideIndex, x, y, z, direction, type;
    rct_xy_element mapElement, endOfTrackElement;
    Ride *ride;
    rct_ride_entry *rideType;

    mapElement = *input;
    rideIndex = mapElement.element->properties.track.ride_index;
    ride = get_ride(rideIndex);
    if (ride == NULL) {
        return 0;
    }
    rideType = get_ride_entry_by_ride(ride);

    if ((rideType == NULL) || !ride_check_if_construction_allowed(ride))
        return 0;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE) {
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32, ride->name_arguments);
        window_error_open(STR_CANT_START_CONSTRUCTION_ON, STR_LOCAL_AUTHORITY_FORBIDS_DEMOLITION_OR_MODIFICATIONS_TO_THIS_RIDE);
        return 0;
    }

    // Stop the ride again to clear all vehicles and peeps (compatible with network games)
    ride_set_status(rideIndex, RIDE_STATUS_CLOSED);

    // Check if element is a station entrance or exit
    if (map_element_get_type(mapElement.element) == MAP_ELEMENT_TYPE_ENTRANCE)
        return ride_modify_entrance_or_exit(mapElement.element, mapElement.x, mapElement.y);

    ride_create_or_find_construction_window(rideIndex);

    if (ride->type == RIDE_TYPE_MAZE)
        return ride_modify_maze(mapElement.element, mapElement.x, mapElement.y);

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS)) {
        if (ride_find_track_gap(&mapElement, &endOfTrackElement))
            mapElement = endOfTrackElement;
    }

    x = mapElement.x;
    y = mapElement.y;
    z = mapElement.element->base_height * 8;
    direction = map_element_get_direction(mapElement.element);
    type = mapElement.element->properties.track.type;

    if (sub_6C683D(&x, &y, &z, direction, type, 0, NULL, 0))
        return 0;

    _currentRideIndex = rideIndex;
    _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
    _currentTrackBeginX = x;
    _currentTrackBeginY = y;
    _currentTrackBeginZ = z;
    _currentTrackPieceDirection = direction;
    _currentTrackPieceType = type;
    _currentTrackSelectionFlags = 0;
    _rideConstructionArrowPulseTime = 0;

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_NO_TRACK)) {
        window_ride_construction_update_active_elements();
        return 1;
    }

    ride_select_next_section();
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT) {
        window_ride_construction_update_active_elements();
        return 1;
    }

    _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
    _currentTrackBeginX = x;
    _currentTrackBeginY = y;
    _currentTrackBeginZ = z;
    _currentTrackPieceDirection = direction;
    _currentTrackPieceType = type;
    _currentTrackSelectionFlags = 0;
    _rideConstructionArrowPulseTime = 0;

    ride_select_previous_section();

    if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_BACK) {
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
        _currentTrackBeginX = x;
        _currentTrackBeginY = y;
        _currentTrackBeginZ = z;
        _currentTrackPieceDirection = direction;
        _currentTrackPieceType = type;
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
    }

    window_ride_construction_update_active_elements();
    return 1;
}

/**
 *
 *  rct2: 0x006CC3FB
 */
sint32 ride_initialise_construction_window(sint32 rideIndex)
{
    Ride *ride;
    rct_window *w;

    tool_cancel();
    ride = get_ride(rideIndex);

    if (!ride_check_if_construction_allowed(ride))
        return 0;

    ride_clear_for_construction(rideIndex);
    ride_remove_peeps(rideIndex);

    w = ride_create_or_find_construction_window(rideIndex);

    tool_set(w, WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT, TOOL_CROSSHAIR);
    input_set_flag(INPUT_FLAG_6, true);

    ride = get_ride(_currentRideIndex);

    _currentTrackCurve = RideConstructionDefaultTrackType[ride->type] | 0x100;
    _currentTrackSlopeEnd = 0;
    _currentTrackBankEnd = 0;
    _currentTrackLiftHill = 0;
    _currentTrackAlternative = RIDE_TYPE_NO_ALTERNATIVES;

    if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_START_CONSTRUCTION_INVERTED)
        _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;

    _previousTrackBankEnd = 0;
    _previousTrackSlopeEnd = 0;

    _currentTrackPieceDirection = 0;
    _rideConstructionState = RIDE_CONSTRUCTION_STATE_PLACE;
    _currentTrackSelectionFlags = 0;
    _rideConstructionArrowPulseTime = 0;

    window_ride_construction_update_active_elements();
    return 1;
}

#pragma endregion

#pragma region Update functions

/**
 *
 *  rct2: 0x006ABE4C
 */
void ride_update_all()
{
    Ride *ride;
    sint32 i;

    // Remove all rides if scenario editor
    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) {
        if (gS6Info.editor_step <= EDITOR_STEP_INVENTIONS_LIST_SET_UP)
            FOR_ALL_RIDES(i, ride)
                ride->type = RIDE_TYPE_NULL;
        return;
    }

    window_update_viewport_ride_music();

    // Update rides
    FOR_ALL_RIDES(i, ride)
        ride_update(i);

    ride_music_update_final();
}

/**
 *
 *  rct2: 0x006ABE73
 */
static void ride_update(sint32 rideIndex)
{
    Ride *ride = get_ride(rideIndex);

    if (ride->vehicle_change_timeout != 0)
        ride->vehicle_change_timeout--;

    ride_music_update(rideIndex);

    // Update stations
    if (ride->type != RIDE_TYPE_MAZE)
        for (sint32 i = 0; i < MAX_STATIONS; i++)
            ride_update_station(ride, i);

    // Update financial statistics
    ride->num_customers_timeout++;

    if (ride->num_customers_timeout >= 960) {
        // This is meant to update about every 30 seconds
        ride->num_customers_timeout = 0;

        // Shift number of customers history, start of the array is the most recent one
        for (sint32 i = CUSTOMER_HISTORY_SIZE - 1; i > 0; i--)
        {
            ride->num_customers[i] = ride->num_customers[i - 1];
        }
        ride->num_customers[0] = ride->cur_num_customers;

        ride->cur_num_customers = 0;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

        ride->income_per_hour = ride_calculate_income_per_hour(ride);
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

        if (ride->upkeep_cost != (money16)(uint16)0xFFFF)
            ride->profit = (ride->income_per_hour - ((money32)ride->upkeep_cost * 16));
    }

    // Ride specific updates
    if (ride->type == RIDE_TYPE_CHAIRLIFT)
        ride_chairlift_update(ride);
    else if (ride->type == RIDE_TYPE_SPIRAL_SLIDE)
        ride_spiral_slide_update(ride);

    ride_breakdown_update(rideIndex);

    // Various things include news messages
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION))
        if (((gCurrentTicks >> 1) & 255) == (uint32)rideIndex)
            ride_breakdown_status_update(rideIndex);

    ride_inspection_update(ride);

    if (ride->status == RIDE_STATUS_TESTING && gConfigGeneral.no_test_crashes) {
        for (sint32 i = 0; i < ride->num_vehicles; i++) {
            rct_vehicle *vehicle = GET_VEHICLE(ride->vehicles[i]);

            if (vehicle->status == VEHICLE_STATUS_CRASHED || vehicle->status == VEHICLE_STATUS_CRASHING) {
                ride_set_status(rideIndex, RIDE_STATUS_CLOSED);
                ride_set_status(rideIndex, RIDE_STATUS_CLOSED);
                ride_set_status(rideIndex, RIDE_STATUS_TESTING);
                break;
            }
        }
    }
}

/**
 *
 *  rct2: 0x006AC489
 */
static void ride_chairlift_update(Ride *ride)
{
    sint32 x, y, z;

    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
        return;
    if ((ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)) &&
        ride->breakdown_reason_pending == 0)
        return;

    uint16 old_chairlift_bullwheel_rotation = ride->chairlift_bullwheel_rotation >> 14;
    ride->chairlift_bullwheel_rotation += ride->speed * 2048;
    if (old_chairlift_bullwheel_rotation == ride->speed / 8)
        return;

    x = ride->chairlift_bullwheel_location[0].x * 32;
    y = ride->chairlift_bullwheel_location[0].y * 32;
    z = ride->chairlift_bullwheel_z[0] * 8;
    map_invalidate_tile_zoom1(x, y, z, z + (4 * 8));

    x = ride->chairlift_bullwheel_location[1].x * 32;
    y = ride->chairlift_bullwheel_location[1].y * 32;
    z = ride->chairlift_bullwheel_z[1] * 8;
    map_invalidate_tile_zoom1(x, y, z, z + (4 * 8));
}

/**
 *
 *  rct2: 0x0069A3A2
 * edi: ride (in code as bytes offset from start of rides list)
 * bl: happiness
 */
void ride_update_satisfaction(Ride* ride, uint8 happiness) {
    ride->satisfaction_next += happiness;
    ride->satisfaction_time_out++;
    if (ride->satisfaction_time_out >= 20) {
        ride->satisfaction = ride->satisfaction_next >> 2;
        ride->satisfaction_next = 0;
        ride->satisfaction_time_out = 0;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

    }
}

/**
 *
 *  rct2: 0x0069A3D7
 * Updates the ride popularity
 * edi : ride
 * bl  : pop_amount
 * pop_amount can be zero if peep visited but did not purchase.
 */
void ride_update_popularity(Ride* ride, uint8 pop_amount){
    ride->popularity_next += pop_amount;
    ride->popularity_time_out++;
    if (ride->popularity_time_out < 25)return;

    ride->popularity = ride->popularity_next;
    ride->popularity_next = 0;
    ride->popularity_time_out = 0;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
}

/** rct2: 0x0098DDB8, 0x0098DDBA */
static const rct_xy16 ride_spiral_slide_main_tile_offset[][4] = {
    {
        {  32,  32 },
        {   0,  32 },
        {   0,   0 },
        {  32,   0 },
    }, {
        {  32,   0 },
        {   0,   0 },
        {   0, -32 },
        {  32, -32 },
    }, {
        {   0,   0 },
        { -32,   0 },
        { -32, -32 },
        {   0, -32 },
    }, {
        {   0,   0 },
        {   0,  32 },
        { -32,  32 },
        { -32,   0 },
    },
};

/**
 *
 *  rct2: 0x006AC545
 */
static void ride_spiral_slide_update(Ride *ride)
{
    if (gCurrentTicks & 3)
        return;
    if (ride->slide_in_use == 0)
        return;

    ride->spiral_slide_progress++;
    if (ride->spiral_slide_progress >= 48) {
        ride->slide_in_use--;

        rct_peep *peep = GET_PEEP(ride->slide_peep);
        peep->destination_x++;
    }

    const uint8 current_rotation = get_current_rotation();
    // Invalidate something related to station start
    for (sint32 i = 0; i < MAX_STATIONS; i++) {
        if (ride->station_starts[i].xy == RCT_XY8_UNDEFINED)
            continue;

        sint32 x = ride->station_starts[i].x;
        sint32 y = ride->station_starts[i].y;

        rct_map_element *mapElement = ride_get_station_start_track_element(ride, i);
        if (mapElement == NULL)
            continue;

        sint32 rotation = map_element_get_direction(mapElement);
        x *= 32;
        y *= 32;
        x += ride_spiral_slide_main_tile_offset[rotation][current_rotation].x;
        y += ride_spiral_slide_main_tile_offset[rotation][current_rotation].y;

        map_invalidate_tile_zoom0(x, y, mapElement->base_height * 8, mapElement->clearance_height * 8);
    }
}

#pragma endregion

#pragma region Breakdown and inspection functions

static uint8 _breakdownProblemProbabilities[] = {
    25,     // BREAKDOWN_SAFETY_CUT_OUT
    12,     // BREAKDOWN_RESTRAINTS_STUCK_CLOSED
    10,     // BREAKDOWN_RESTRAINTS_STUCK_OPEN
    13,     // BREAKDOWN_DOORS_STUCK_CLOSED
    10,     // BREAKDOWN_DOORS_STUCK_OPEN
    6,      // BREAKDOWN_VEHICLE_MALFUNCTION
    0,      // BREAKDOWN_BRAKES_FAILURE
    3       // BREAKDOWN_CONTROL_FAILURE
};

/**
 *
 *  rct2: 0x006AC7C2
 */
static void ride_inspection_update(Ride *ride)
{
    if (gCurrentTicks & 2047)
        return;
    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
        return;

    ride->last_inspection++;
    if (ride->last_inspection == 0)
        ride->last_inspection--;

    sint32 inspectionIntervalMinutes = RideInspectionInterval[ride->inspection_interval];
    if (inspectionIntervalMinutes == 0)
        return;

    if (RideAvailableBreakdowns[ride->type] == 0)
        return;

    if (inspectionIntervalMinutes > ride->last_inspection)
        return;

    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION | RIDE_LIFECYCLE_CRASHED))
        return;

    // Inspect the first station that has an exit
    ride->lifecycle_flags |= RIDE_LIFECYCLE_DUE_INSPECTION;
    ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;

    sint8 stationIndex = ride_get_first_valid_station_exit(ride);
    ride->inspection_station = (stationIndex != -1) ? stationIndex : 0;
}

static sint32 get_age_penalty(Ride *ride) {
    sint32 years;
    years = date_get_year(gDateMonthsElapsed - ride->build_date);
    switch (years) {
    case 0:
        return 0;
    case 1:
        return ride->unreliability_factor / 8;
    case 2:
        return ride->unreliability_factor / 4;
    case 3:
    case 4:
        return ride->unreliability_factor / 2;
    case 5:
    case 6:
    case 7:
        return ride->unreliability_factor;
    default:
        return ride->unreliability_factor * 2;
    }
}

/**
 *
 *  rct2: 0x006AC622
 */
static void ride_breakdown_update(sint32 rideIndex)
{
    if (gCurrentTicks & 255)
        return;
    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
        return;

    Ride *ride = get_ride(rideIndex);
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        ride->downtime_history[0]++;

    if (!(gCurrentTicks & 8191))
    {
        sint32 totalDowntime = 0;

        for (sint32 i = 0; i < DOWNTIME_HISTORY_SIZE; i++)
        {
            totalDowntime += ride->downtime_history[i];
        }

        ride->downtime = min(totalDowntime / 2, 100);

        for (sint32 i = DOWNTIME_HISTORY_SIZE - 1; i > 0; i--)
        {
            ride->downtime_history[i] = ride->downtime_history[i - 1];
        }
        ride->downtime_history[0] = 0;

        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
    }

    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        return;
    if (ride->status == RIDE_STATUS_CLOSED)
        return;

    // Calculate breakdown probability?
    sint32 unreliabilityAccumulator = ride->unreliability_factor + get_age_penalty(ride);
    ride->reliability = max(0, ride->reliability - unreliabilityAccumulator);
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;

    // Random probability of a breakdown. Roughly this is 1 in
    //
    // (25000 - reliability) / 3 000 000
    //
    // a 0.8% chance, less the breakdown factor which accumulates as the game
    // continues.
    if ((ride->reliability == 0 || (sint32)(scenario_rand() & 0x2FFFFF) <= 1 + RIDE_INITIAL_RELIABILITY - ride->reliability)
            && !gCheatsDisableAllBreakdowns) {
        sint32 breakdownReason = ride_get_new_breakdown_problem(ride);
        if (breakdownReason != -1)
            ride_prepare_breakdown(rideIndex, breakdownReason);
    }
}

/**
 *
 *  rct2: 0x006B7294
 */
static sint32 ride_get_new_breakdown_problem(Ride *ride)
{
    sint32 availableBreakdownProblems, monthsOld, totalProbability, randomProbability, problemBits, breakdownProblem;
    rct_ride_entry *entry;

    // Brake failure is more likely when it's raining
    _breakdownProblemProbabilities[BREAKDOWN_BRAKES_FAILURE] = gClimateCurrentRainLevel == 0 ? 3 : 20;

    entry = get_ride_entry_by_ride(ride);
    if (entry->flags & RIDE_ENTRY_FLAG_CANNOT_BREAK_DOWN)
        return -1;

    availableBreakdownProblems = RideAvailableBreakdowns[ride->type];

    // Calculate the total probability range for all possible breakdown problems
    totalProbability = 0;
    problemBits = availableBreakdownProblems;
    while (problemBits != 0) {
        breakdownProblem = bitscanforward(problemBits);
        problemBits &= ~(1 << breakdownProblem);
        totalProbability += _breakdownProblemProbabilities[breakdownProblem];
    }
    if (totalProbability == 0)
        return -1;

    // Choose a random number within this range
    randomProbability = scenario_rand() % totalProbability;

    // Find which problem range the random number lies
    problemBits = availableBreakdownProblems;
    do {
        breakdownProblem = bitscanforward(problemBits);
        problemBits &= ~(1 << breakdownProblem);
        randomProbability -= _breakdownProblemProbabilities[breakdownProblem];
    } while (randomProbability >= 0);

    if (breakdownProblem != BREAKDOWN_BRAKES_FAILURE)
        return breakdownProblem;

    // Brakes failure can not happen if block brakes are used (so long as there is more than one vehicle)
    // However if this is the case, brake failure should be taken out the equation, otherwise block brake
    // rides have a lower probability to break down due to a random implementation reason.
    if (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
        if (ride->num_vehicles != 1)
            return -1;

    // If brakes failure is disabled, also take it out of the equation (see above comment why)
    if (gCheatsDisableBrakesFailure)
        return -1;

    monthsOld = gDateMonthsElapsed - ride->build_date;
    if (monthsOld < 16 || ride->reliability_percentage > 50)
        return -1;

    return BREAKDOWN_BRAKES_FAILURE;
}

static void choose_random_train_to_breakdown_safe(Ride * ride)
{
    ride->broken_vehicle = scenario_rand() % ride->num_vehicles;

    // Prevent crash caused by accessing SPRITE_INDEX_NULL on hacked rides.
    // This should probably be cleaned up on import instead.
    while (ride->vehicles[ride->broken_vehicle] == SPRITE_INDEX_NULL && ride->broken_vehicle != 0)
    {
        --ride->broken_vehicle;
    }
}

/**
 *
 *  rct2: 0x006B7348
 */
void ride_prepare_breakdown(sint32 rideIndex, sint32 breakdownReason)
{
    sint32 i;
    Ride *ride;
    rct_vehicle *vehicle;

    ride = get_ride(rideIndex);
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        return;

    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
    ride->lifecycle_flags |= RIDE_LIFECYCLE_BREAKDOWN_PENDING;

    ride->breakdown_reason_pending = breakdownReason;
    ride->mechanic_status = RIDE_MECHANIC_STATUS_UNDEFINED;
    ride->breakdown_sound_modifier = 0;
    ride->not_fixed_timeout = 0;

    switch (breakdownReason) {
    case BREAKDOWN_SAFETY_CUT_OUT:
    case BREAKDOWN_CONTROL_FAILURE:
        // Inspect first station with an exit
        i = ride_get_first_valid_station_exit(ride);
        if (i != -1)
        {
            ride->inspection_station = i;
        }

        break;
    case BREAKDOWN_RESTRAINTS_STUCK_CLOSED:
    case BREAKDOWN_RESTRAINTS_STUCK_OPEN:
    case BREAKDOWN_DOORS_STUCK_CLOSED:
    case BREAKDOWN_DOORS_STUCK_OPEN:
        // Choose a random train and car
        choose_random_train_to_breakdown_safe(ride);
        ride->broken_car = scenario_rand() % ride->num_cars_per_train;

        // Set flag on broken car
        vehicle = GET_VEHICLE(ride->vehicles[ride->broken_vehicle]);
        for (i = ride->broken_car; i > 0; i--) {
            if (vehicle->next_vehicle_on_train == SPRITE_INDEX_NULL) {
                vehicle = NULL;
                break;
            }
            else {
                vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
            }
        }
        if (vehicle != NULL)
            vehicle->update_flags |= VEHICLE_UPDATE_FLAG_BROKEN_CAR;
        break;
    case BREAKDOWN_VEHICLE_MALFUNCTION:
        // Choose a random train
        choose_random_train_to_breakdown_safe(ride);
        ride->broken_car = 0;

        // Set flag on broken train, first car
        vehicle = GET_VEHICLE(ride->vehicles[ride->broken_vehicle]);
        vehicle->update_flags |= VEHICLE_UPDATE_FLAG_BROKEN_TRAIN;
        break;
    case BREAKDOWN_BRAKES_FAILURE:
        // Original code generates a random number but does not use it
        // Unsure if this was supposed to choose a random station (or random station with an exit)
        i = ride_get_first_valid_station_exit(ride);
        if (i != -1)
        {
            ride->inspection_station = i;
        }
        break;
    }
}

/**
 *
 *  rct2: 0x006B74FA
 */
void ride_breakdown_add_news_item(sint32 rideIndex)
{
    Ride *ride = get_ride(rideIndex);

    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32, ride->name_arguments);
    if (gConfigNotifications.ride_broken_down) {
        news_item_add_to_queue(NEWS_ITEM_RIDE, STR_RIDE_IS_BROKEN_DOWN, rideIndex);
    }
}

/**
 *
 *  rct2: 0x006B75C8
 */
static void ride_breakdown_status_update(sint32 rideIndex)
{
    Ride *ride = get_ride(rideIndex);

    // Warn player if ride hasn't been fixed for ages
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
        ride->not_fixed_timeout++;
        // When there has been a full 255 timeout ticks this
        // will force timeout ticks to keep issuing news every
        // 16 ticks. Note there is no reason to do this.
        if (ride->not_fixed_timeout == 0)
            ride->not_fixed_timeout -= 16;

        if (
            !(ride->not_fixed_timeout & 15) &&
            ride->mechanic_status != RIDE_MECHANIC_STATUS_FIXING &&
            ride->mechanic_status != RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES
        ) {
            set_format_arg(0, rct_string_id, ride->name);
            set_format_arg(2, uint32, ride->name_arguments);
            if (gConfigNotifications.ride_warnings) {
                news_item_add_to_queue(NEWS_ITEM_RIDE, STR_RIDE_IS_STILL_NOT_FIXED, rideIndex);
            }
        }
    }

    ride_mechanic_status_update(rideIndex, ride->mechanic_status);
}

/**
 *
 *  rct2: 0x006B762F
 */
static void ride_mechanic_status_update(sint32 rideIndex, sint32 mechanicStatus)
{
    sint32 breakdownReason;
    Ride *ride;
    rct_peep *mechanic;

    ride = get_ride(rideIndex);
    switch (mechanicStatus) {
    case RIDE_MECHANIC_STATUS_UNDEFINED:
        breakdownReason = ride->breakdown_reason_pending;
        if (
            breakdownReason == BREAKDOWN_SAFETY_CUT_OUT ||
            breakdownReason == BREAKDOWN_BRAKES_FAILURE ||
            breakdownReason == BREAKDOWN_CONTROL_FAILURE
        ) {
            ride->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE | RIDE_INVALIDATE_RIDE_LIST | RIDE_INVALIDATE_RIDE_MAIN;
            ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            ride->breakdown_reason = breakdownReason;
            ride_breakdown_add_news_item(rideIndex);
        }
        break;
    case RIDE_MECHANIC_STATUS_CALLING:
        if (RideAvailableBreakdowns[ride->type] == 0) {
            ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION);
            break;
        }

        ride_call_closest_mechanic(rideIndex);
        break;
    case RIDE_MECHANIC_STATUS_HEADING:
        mechanic = NULL;
        if (ride->mechanic != SPRITE_INDEX_NULL) {
            mechanic = &(get_sprite(ride->mechanic)->peep);
        }
        if (mechanic == NULL ||
            !peep_is_mechanic(mechanic) ||
            (mechanic->state != PEEP_STATE_HEADING_TO_INSPECTION && mechanic->state != PEEP_STATE_ANSWERING) ||
            mechanic->current_ride != rideIndex
        ) {
            ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
            ride_mechanic_status_update(rideIndex, RIDE_MECHANIC_STATUS_CALLING);
        }
        break;
    case RIDE_MECHANIC_STATUS_FIXING:
        mechanic = NULL;
        if (ride->mechanic != SPRITE_INDEX_NULL) {
            mechanic = &(get_sprite(ride->mechanic)->peep);
        }
        if (mechanic == NULL ||
            !peep_is_mechanic(mechanic) ||
            (
                mechanic->state != PEEP_STATE_HEADING_TO_INSPECTION &&
                mechanic->state != PEEP_STATE_FIXING &&
                mechanic->state != PEEP_STATE_INSPECTING &&
                mechanic->state != PEEP_STATE_ANSWERING
            )
        ) {
            ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
            ride_mechanic_status_update(rideIndex, RIDE_MECHANIC_STATUS_CALLING);
        }
        break;
    }
}

/**
 *
 *  rct2: 0x006B796C
 */
static void ride_call_mechanic(sint32 rideIndex, rct_peep *mechanic, sint32 forInspection)
{
    Ride *ride;

    ride = get_ride(rideIndex);
    peep_decrement_num_riders(mechanic);
    mechanic->state = forInspection ? PEEP_STATE_HEADING_TO_INSPECTION : PEEP_STATE_ANSWERING;
    peep_window_state_update(mechanic);
    mechanic->sub_state = 0;
    ride->mechanic_status = RIDE_MECHANIC_STATUS_HEADING;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
    ride->mechanic = mechanic->sprite_index;
    mechanic->current_ride = rideIndex;
    mechanic->current_ride_station = ride->inspection_station;
}

/**
 *
 *  rct2: 0x006B76AB
 */
static void ride_call_closest_mechanic(sint32 rideIndex)
{
    Ride *ride;
    rct_peep *mechanic;
    sint32 forInspection;

    ride = get_ride(rideIndex);
    forInspection = (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)) == 0;

    mechanic = ride_find_closest_mechanic(ride, forInspection);
    if (mechanic != NULL)
        ride_call_mechanic(rideIndex, mechanic, forInspection);
}

rct_peep *ride_find_closest_mechanic(Ride *ride, sint32 forInspection)
{
    sint32 x, y, z, stationIndex;
    rct_xy8 location;
    rct_map_element *mapElement;

    // Get either exit position or entrance position if there is no exit
    stationIndex = ride->inspection_station;
    location = ride->exits[stationIndex];
    if (location.xy == RCT_XY8_UNDEFINED) {
        location = ride->entrances[stationIndex];
        if (location.xy == RCT_XY8_UNDEFINED)
            return NULL;
    }

    // Get station start track element and position
    x = location.x;
    y = location.y;
    z = ride->station_heights[stationIndex];
    mapElement = ride_get_station_exit_element(ride, x, y, z);
    if (mapElement == NULL)
        return NULL;

    x *= 32;
    y *= 32;

    // Set x,y to centre of the station exit for the mechanic search.
    x += 16;
    y += 16;

    return find_closest_mechanic(x, y, forInspection);
}

/**
 *
 *  rct2: 0x006B774B (forInspection = 0)
 *  rct2: 0x006B78C3 (forInspection = 1)
 */
rct_peep *find_closest_mechanic(sint32 x, sint32 y, sint32 forInspection)
{
    uint32 closestDistance, distance;
    uint16 spriteIndex;
    rct_peep *peep, *closestMechanic = NULL;

    closestDistance = UINT_MAX;
    FOR_ALL_STAFF(spriteIndex, peep) {
        if (peep->staff_type != STAFF_TYPE_MECHANIC)
            continue;

        if (!forInspection) {
            if (peep->state == PEEP_STATE_HEADING_TO_INSPECTION){
                if (peep->sub_state >= 4)
                    continue;
            }
            else if (peep->state != PEEP_STATE_PATROLLING)
                continue;

            if (!(peep->staff_orders & STAFF_ORDERS_FIX_RIDES))
                continue;
        } else {
            if (peep->state != PEEP_STATE_PATROLLING || !(peep->staff_orders & STAFF_ORDERS_INSPECT_RIDES))
                continue;
        }

        if (map_is_location_in_park(x, y))
            if (!staff_is_location_in_patrol(peep, x & 0xFFE0, y & 0xFFE0))
                continue;

        if (peep->x == MAP_LOCATION_NULL)
            continue;

        // Manhattan distance
        distance = abs(peep->x - x) + abs(peep->y - y);
        if (distance < closestDistance) {
            closestDistance = distance;
            closestMechanic = peep;
        }
    }

    return closestMechanic;
}

rct_peep *ride_get_assigned_mechanic(Ride *ride)
{
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
        if (
            ride->mechanic_status == RIDE_MECHANIC_STATUS_HEADING ||
            ride->mechanic_status == RIDE_MECHANIC_STATUS_FIXING ||
            ride->mechanic_status == RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES
        ) {
            rct_peep *peep = &(get_sprite(ride->mechanic)->peep);
            if (peep_is_mechanic(peep))
                return peep;
        }
    }

    return NULL;
}

#pragma endregion

#pragma region Music functions

#define MAKE_TUNEID_LIST(...) (uint8[]){(countof(((uint8[]){__VA_ARGS__}))), __VA_ARGS__}

// 0x009AEF28
uint8 *ride_music_style_tuneids[] = {
    MAKE_TUNEID_LIST(13), // MUSIC_STYLE_DODGEMS_BEAT
    MAKE_TUNEID_LIST(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12), // MUSIC_STYLE_FAIRGROUND_ORGAN
    MAKE_TUNEID_LIST(15), // MUSIC_STYLE_ROMAN_FANFARE
    MAKE_TUNEID_LIST(16), // MUSIC_STYLE_ORIENTAL
    MAKE_TUNEID_LIST(17), // MUSIC_STYLE_MARTIAN
    MAKE_TUNEID_LIST(18), // MUSIC_STYLE_JUNGLE_DRUMS
    MAKE_TUNEID_LIST(19), // MUSIC_STYLE_EGYPTIAN
    MAKE_TUNEID_LIST(20), // MUSIC_STYLE_TOYLAND
    MAKE_TUNEID_LIST(21), // MUSIC_STYLE_CIRCUS_SHOW
    MAKE_TUNEID_LIST(22), // MUSIC_STYLE_SPACE
    MAKE_TUNEID_LIST(23), // MUSIC_STYLE_HORROR
    MAKE_TUNEID_LIST(24), // MUSIC_STYLE_TECHNO
    MAKE_TUNEID_LIST(25), // MUSIC_STYLE_GENTLE
    MAKE_TUNEID_LIST(26), // MUSIC_STYLE_SUMMER
    MAKE_TUNEID_LIST(27), // MUSIC_STYLE_WATER
    MAKE_TUNEID_LIST(28), // MUSIC_STYLE_WILD_WEST
    MAKE_TUNEID_LIST(29), // MUSIC_STYLE_JURASSIC
    MAKE_TUNEID_LIST(30), // MUSIC_STYLE_ROCK
    MAKE_TUNEID_LIST(31), // MUSIC_STYLE_RAGTIME
    MAKE_TUNEID_LIST(32), // MUSIC_STYLE_FANTASY
    MAKE_TUNEID_LIST(33), // MUSIC_STYLE_ROCK_STYLE_2
    MAKE_TUNEID_LIST(34), // MUSIC_STYLE_ICE
    MAKE_TUNEID_LIST(35), // MUSIC_STYLE_SNOW
    MAKE_TUNEID_LIST(36), // MUSIC_STYLE_CUSTOM_MUSIC_1
    MAKE_TUNEID_LIST(37), // MUSIC_STYLE_CUSTOM_MUSIC_2
    MAKE_TUNEID_LIST(38), // MUSIC_STYLE_MEDIEVAL
    MAKE_TUNEID_LIST(39), // MUSIC_STYLE_URBAN
    MAKE_TUNEID_LIST(40), // MUSIC_STYLE_ORGAN
    MAKE_TUNEID_LIST(41), // MUSIC_STYLE_MECHANICAL
    MAKE_TUNEID_LIST(42), // MUSIC_STYLE_MODERN
    MAKE_TUNEID_LIST(43), // MUSIC_STYLE_PIRATES
    MAKE_TUNEID_LIST(44), // MUSIC_STYLE_ROCK_STYLE_3
    MAKE_TUNEID_LIST(45), // MUSIC_STYLE_CANDY_STYLE
};

/**
 *
 *  rct2: 0x006ABE85
 */
static void ride_music_update(sint32 rideIndex)
{
    Ride *ride = get_ride(rideIndex);

    if (
        !(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_MUSIC_ON_DEFAULT) &&
        !(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_MUSIC)
    ) {
        return;
    }

    if (ride->status != RIDE_STATUS_OPEN || !(ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC)) {
        ride->music_tune_id = 255;
        return;
    }

    if (ride->type == RIDE_TYPE_CIRCUS_SHOW) {
        rct_vehicle *vehicle = GET_VEHICLE(ride->vehicles[0]);
        if (vehicle->status != VEHICLE_STATUS_DOING_CIRCUS_SHOW) {
            ride->music_tune_id = 255;
            return;
        }
    }

    // Oscillate parameters for a power cut effect when breaking down
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)) {
        if (ride->breakdown_reason_pending == BREAKDOWN_CONTROL_FAILURE) {
            if (!(gCurrentTicks & 7))
                if (ride->breakdown_sound_modifier != 255)
                    ride->breakdown_sound_modifier++;
        } else {
            if (
                (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) ||
                ride->breakdown_reason_pending == BREAKDOWN_BRAKES_FAILURE ||
                ride->breakdown_reason_pending == BREAKDOWN_CONTROL_FAILURE
            ) {
                if (ride->breakdown_sound_modifier != 255)
                    ride->breakdown_sound_modifier++;
            }

            if (ride->breakdown_sound_modifier == 255) {
                ride->music_tune_id = 255;
                return;
            }
        }
    }

    // Select random tune from available tunes for a music style (of course only merry-go-rounds have more than one tune)
    if (ride->music_tune_id == 255) {
        uint8 *musicStyleTunes = ride_music_style_tuneids[ride->music];
        uint8 numTunes = *musicStyleTunes++;
        ride->music_tune_id = musicStyleTunes[util_rand() % numTunes];
        ride->music_position = 0;
        return;
    }

    sint32 x = ride->station_starts[0].x * 32 + 16;
    sint32 y = ride->station_starts[0].y * 32 + 16;
    sint32 z = ride->station_heights[0] * 8;

    sint32 sampleRate = 22050;

    // Alter sample rate for a power cut effect
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)) {
        sampleRate = ride->breakdown_sound_modifier * 70;
        if (ride->breakdown_reason_pending != BREAKDOWN_CONTROL_FAILURE)
            sampleRate *= -1;
        sampleRate += 22050;
    }

    ride->music_position = ride_music_params_update(x, y, z, rideIndex, sampleRate, ride->music_position, &ride->music_tune_id);
}

#pragma endregion

#pragma region Measurement functions

/**
 *
 *  rct2: 0x006B642B
 */
void ride_measurement_clear(Ride *ride)
{
    rct_ride_measurement *measurement;

    if (ride->measurement_index == 255)
        return;

    measurement = get_ride_measurement(ride->measurement_index);
    measurement->ride_index = 255;
    ride->measurement_index = 255;
}

/**
 *
 *  rct2: 0x006B64F2
 */
static void ride_measurement_update(rct_ride_measurement *measurement)
{
    uint16 spriteIndex;
    Ride *ride;
    rct_vehicle *vehicle;
    sint32 velocity, altitude, verticalG, lateralG;

    ride = get_ride(measurement->ride_index);
    spriteIndex = ride->vehicles[measurement->vehicle_index];
    if (spriteIndex == SPRITE_INDEX_NULL)
        return;

    vehicle = GET_VEHICLE(spriteIndex);

    if (measurement->flags & RIDE_MEASUREMENT_FLAG_UNLOADING) {
        if (vehicle->status != VEHICLE_STATUS_DEPARTING && vehicle->status != VEHICLE_STATUS_TRAVELLING_CABLE_LIFT)
            return;

        measurement->flags &= ~RIDE_MEASUREMENT_FLAG_UNLOADING;
        if (measurement->current_station == vehicle->current_station)
            measurement->current_item = 0;
    }

    if (vehicle->status == VEHICLE_STATUS_UNLOADING_PASSENGERS) {
        measurement->flags |= RIDE_MEASUREMENT_FLAG_UNLOADING;
        return;
    }

    uint8 trackType = (vehicle->track_type >> 2) & 0xFF;
    if (trackType == TRACK_ELEM_BLOCK_BRAKES ||
        trackType == TRACK_ELEM_CABLE_LIFT_HILL ||
        trackType == TRACK_ELEM_25_DEG_UP_TO_FLAT ||
        trackType == TRACK_ELEM_60_DEG_UP_TO_FLAT ||
        trackType == TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT ||
        trackType == TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT)
        if (vehicle->velocity == 0)
            return;

    if (measurement->current_item >= RIDE_MEASUREMENT_MAX_ITEMS)
        return;

    if (measurement->flags & RIDE_MEASUREMENT_FLAG_G_FORCES) {
        vehicle_get_g_forces(vehicle, &verticalG, &lateralG);
        verticalG = clamp(-127, verticalG / 8, 127);
        lateralG = clamp(-127, lateralG / 8, 127);

        if (gScenarioTicks & 1) {
            verticalG = (verticalG + measurement->vertical[measurement->current_item]) / 2;
            lateralG = (lateralG + measurement->lateral[measurement->current_item]) / 2;
        }

        measurement->vertical[measurement->current_item] = verticalG & 0xFF;
        measurement->lateral[measurement->current_item] = lateralG & 0xFF;
    }

    velocity = min(abs((vehicle->velocity * 5) >> 16), 255);
    altitude = min(vehicle->z / 8, 255);

    if (gScenarioTicks & 1) {
        velocity = (velocity + measurement->velocity[measurement->current_item]) / 2;
        altitude = (altitude + measurement->altitude[measurement->current_item]) / 2;
    }

    measurement->velocity[measurement->current_item] = velocity & 0xFF;
    measurement->altitude[measurement->current_item] = altitude & 0xFF;

    if (gScenarioTicks & 1) {
        measurement->current_item++;
        measurement->num_items = max(measurement->num_items, measurement->current_item);
    }
}

/**
 *
 *  rct2: 0x006B6456
 */
void ride_measurements_update()
{
    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return;

    // For each ride measurement
    for (sint32 i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
        rct_ride_measurement *measurement = get_ride_measurement(i);
        if (measurement->ride_index == 255)
            continue;

        Ride *ride = get_ride(measurement->ride_index);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
            continue;

        if (measurement->flags & RIDE_MEASUREMENT_FLAG_RUNNING) {
            ride_measurement_update(measurement);
        } else {
            // For each vehicle
            for (sint32 j = 0; j < ride->num_vehicles; j++) {
                uint16 spriteIndex = ride->vehicles[j];
                if (spriteIndex == SPRITE_INDEX_NULL)
                    continue;

                rct_vehicle *vehicle = GET_VEHICLE(spriteIndex);
                if (vehicle->status == VEHICLE_STATUS_DEPARTING || vehicle->status == VEHICLE_STATUS_TRAVELLING_CABLE_LIFT) {
                    measurement->vehicle_index = j;
                    measurement->current_station = vehicle->current_station;
                    measurement->flags |= RIDE_MEASUREMENT_FLAG_RUNNING;
                    measurement->flags &= ~RIDE_MEASUREMENT_FLAG_UNLOADING;
                    ride_measurement_update(measurement);
                    break;
                }
            }

        }
    }
}

static rct_ride_measurement *ride_get_existing_measurement(sint32 rideIndex)
{
    for (sint32 i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
        rct_ride_measurement *measurement = get_ride_measurement(i);
        if (measurement->ride_index == rideIndex)
            return measurement;
    }

    return NULL;
}

static sint32 ride_get_free_measurement()
{
    for (sint32 i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
        rct_ride_measurement *measurement = get_ride_measurement(i);
        if (measurement->ride_index == 255)
            return i;
    }

    return -1;
}

/**
 *
 *  rct2: 0x006B66D9
 */
rct_ride_measurement *ride_get_measurement(sint32 rideIndex, rct_string_id *message)
{
    Ride *ride = get_ride(rideIndex);

    // Check if ride type supports data logging
    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_DATA_LOGGING)) {
        if (message != NULL) *message = STR_DATA_LOGGING_NOT_AVAILABLE_FOR_THIS_TYPE_OF_RIDE;
        return NULL;
    }

    // Check if a measurement already exists for this ride
    rct_ride_measurement *measurement = ride_get_existing_measurement(rideIndex);
    if (measurement == NULL) {
        // Find a free measurement
        sint32 i = ride_get_free_measurement();
        if (i == -1) {
            // Use last recently used measurement for some other ride
            sint32 lruIndex = 0;
            uint32 lruTicks = 0xFFFFFFFF;
            for (i = 0; i < MAX_RIDE_MEASUREMENTS; i++) {
                measurement = get_ride_measurement(i);

                if (measurement->last_use_tick <= lruTicks) {
                    lruTicks = measurement->last_use_tick;
                    lruIndex = i;
                }
            }

            i = lruIndex;
            measurement = get_ride_measurement(i);
            get_ride(measurement->ride_index)->measurement_index = 255;
        } else {
            measurement = get_ride_measurement(i);
        }

        measurement->ride_index = rideIndex;
        ride->measurement_index = i;
        measurement->flags = 0;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_G_FORCES))
            measurement->flags |= RIDE_MEASUREMENT_FLAG_G_FORCES;
        measurement->num_items = 0;
        measurement->current_item = 0;
    }

    measurement->last_use_tick = gScenarioTicks;
    if (measurement->flags & 1) {
        if (message != NULL) *message = 0;
        return measurement;
    } else {
        set_format_arg(0, rct_string_id, RideComponentNames[RideNameConvention[ride->type].vehicle].singular);
        set_format_arg(2, rct_string_id, RideComponentNames[RideNameConvention[ride->type].station].singular);
        if (message != NULL) *message = STR_DATA_LOGGING_WILL_START_WHEN_NEXT_LEAVES;
        return NULL;
    }
}

#pragma endregion

#pragma region Colour functions

track_colour ride_get_track_colour(Ride *ride, sint32 colourScheme)
{
    track_colour result;
    result.main = ride->track_colour_main[colourScheme];
    result.additional = ride->track_colour_additional[colourScheme];
    result.supports = ride->track_colour_supports[colourScheme];
    return result;
}

vehicle_colour ride_get_vehicle_colour(Ride *ride, sint32 vehicleIndex)
{
    vehicle_colour result;

    //Prevent indexing array out of bounds
    if (vehicleIndex > 31) {
        vehicleIndex = 31;
    }

    result.main = ride->vehicle_colours[vehicleIndex].body_colour;
    result.additional_1 = ride->vehicle_colours[vehicleIndex].trim_colour;
    result.additional_2 = ride->vehicle_colours_extended[vehicleIndex];
    return result;
}

static bool ride_does_vehicle_colour_exist(uint8 ride_sub_type, vehicle_colour *vehicleColour)
{
    sint32 i;
    Ride *ride2;
    FOR_ALL_RIDES(i, ride2) {
        if (ride2->subtype != ride_sub_type) continue;
        if (ride2->vehicle_colours[0].body_colour != vehicleColour->main) continue;
        return false;
    }
    return true;
}

static sint32 ride_get_unused_preset_vehicle_colour(uint8 ride_type, uint8 ride_sub_type)
{
    if (ride_sub_type >= 128)
    {
        return 0;
    }
    rct_ride_entry *rideEntry = get_ride_entry(ride_sub_type);
    if (rideEntry == (rct_ride_entry *)-1)
    {
        return 0;
    }
    vehicle_colour_preset_list *presetList = rideEntry->vehicle_preset_list;
    if (presetList->count == 0)
        return 0;
    if (presetList->count == 255)
        return 255;

    for (sint32 attempt = 0; attempt < 200; attempt++) {
        uint8 numColourConfigurations = presetList->count;
        sint32 randomConfigIndex = util_rand() % numColourConfigurations;
        vehicle_colour *preset = &presetList->list[randomConfigIndex];

        if (ride_does_vehicle_colour_exist(ride_sub_type, preset)) {
            return randomConfigIndex;
        }
    }
    return 0;
}

/**
 *
 *  rct2: 0x006DE52C
 */
static void ride_set_vehicle_colours_to_random_preset(Ride *ride, uint8 preset_index)
{
    rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
    vehicle_colour_preset_list *presetList = rideEntry->vehicle_preset_list;

    if (presetList->count != 0 && presetList->count != 255) {
        assert(preset_index < presetList->count);

        ride->colour_scheme_type = RIDE_COLOUR_SCHEME_ALL_SAME;
        vehicle_colour *preset = &presetList->list[preset_index];
        ride->vehicle_colours[0].body_colour = preset->main;
        ride->vehicle_colours[0].trim_colour = preset->additional_1;
        ride->vehicle_colours_extended[0] = preset->additional_2;
    } else {
        ride->colour_scheme_type = RIDE_COLOUR_SCHEME_DIFFERENT_PER_TRAIN;
        sint32 count = min(presetList->count, 32);
        for (sint32 i = 0; i < count; i++) {
            vehicle_colour *preset = &presetList->list[i];
            ride->vehicle_colours[i].body_colour = preset->main;
            ride->vehicle_colours[i].trim_colour = preset->additional_1;
            ride->vehicle_colours_extended[i] = preset->additional_2;
        }
    }
}

#pragma endregion

#pragma region Reachability

/**
 *
 *  rct2: 0x006B7A5E
 */
void ride_check_all_reachable()
{
    Ride *ride;
    sint32 i;

    FOR_ALL_RIDES(i, ride) {
        if (ride->connected_message_throttle != 0)
            ride->connected_message_throttle--;
        if (ride->status != RIDE_STATUS_OPEN || ride->connected_message_throttle != 0)
            continue;

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
            ride_shop_connected(ride, i);
        else
            ride_entrance_exit_connected(ride, i);
    }
}

/**
 *
 *  rct2: 0x006B7C59
 * @return 1 if the coordinate is reachable or has no entrance, 0 otherwise
 */
static sint32 ride_entrance_exit_is_reachable(rct_xy8 coordinates, Ride* ride, sint32 index)
{
    sint32 x, y, z;
    rct_map_element *mapElement;

    x = coordinates.x;
    y = coordinates.y;
    z = ride->station_heights[index];
    mapElement = map_get_first_element_at(x, y);

    for (;;) {
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE && z == mapElement->base_height) {
            break;
        } else if (map_element_is_last_for_tile(mapElement)) {
            return 1;
        }
        mapElement++;
    }

    uint8 face_direction = map_element_get_direction(mapElement);

    x *= 32;
    y *= 32;
    x -= TileDirectionDelta[face_direction].x;
    y -= TileDirectionDelta[face_direction].y;
    x /= 32;
    y /= 32;

    return map_coord_is_connected(x, y, z, face_direction);
}

static void ride_entrance_exit_connected(Ride* ride, sint32 ride_idx)
{
    for (sint32 i = 0; i < MAX_STATIONS; ++i) {
        rct_xy8 station_start = ride->station_starts[i],
            entrance = ride->entrances[i],
            exit = ride->exits[i];

        if (station_start.xy == RCT_XY8_UNDEFINED )
            continue;
        if (entrance.xy != RCT_XY8_UNDEFINED && !ride_entrance_exit_is_reachable(entrance, ride, i)) {
            // name of ride is parameter of the format string
            set_format_arg(0, rct_string_id, ride->name);
            set_format_arg(2, uint32, ride->name_arguments);
            if (gConfigNotifications.ride_warnings) {
            news_item_add_to_queue(1, STR_ENTRANCE_NOT_CONNECTED, ride_idx);
            }
            ride->connected_message_throttle = 3;
        }

        if (exit.xy != RCT_XY8_UNDEFINED && !ride_entrance_exit_is_reachable(exit, ride, i)) {
            // name of ride is parameter of the format string
            set_format_arg(0, rct_string_id, ride->name);
            set_format_arg(2, uint32, ride->name_arguments);
            if (gConfigNotifications.ride_warnings) {
            news_item_add_to_queue(1, STR_EXIT_NOT_CONNECTED, ride_idx);
            }
            ride->connected_message_throttle = 3;
        }

    }
}

static void ride_shop_connected(Ride* ride, sint32 ride_idx)
{
    sint32 x, y, count;
    rct_map_element *mapElement;

    rct_xy8 coordinates = ride->station_starts[0];
    if (coordinates.xy == RCT_XY8_UNDEFINED)
        return;

    x = coordinates.x;
    y = coordinates.y;

    mapElement = map_get_first_element_at(x, y);
    do {
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK && mapElement->properties.track.ride_index == ride_idx)
            break;
    } while (!map_element_is_last_for_tile(mapElement++));

    uint16 entrance_directions = 0;
    uint8 track_type = mapElement->properties.track.type;
    ride = get_ride(mapElement->properties.track.ride_index);
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
        entrance_directions = FlatRideTrackSequenceProperties[track_type][0];
    } else {
        entrance_directions = TrackSequenceProperties[track_type][0];
    }

    uint8 tile_direction = map_element_get_direction(mapElement);
    entrance_directions <<= tile_direction;
    entrance_directions = ((entrance_directions >> 12) | entrance_directions) & 0xF;

    // Now each bit in entrance_directions stands for an entrance direction to check
    if (entrance_directions == 0)
        return;

    // Turn x, y from tiles into units
    x *= 32;
    y *= 32;

    for (count = 0; entrance_directions != 0; count++) {
        if (!(entrance_directions & 1)) {
            entrance_directions >>= 1;
            continue;
        }
        entrance_directions >>= 1;

        // Flip direction north<->south, east<->west
        uint8 face_direction = count ^ 2;

        sint32 y2 = y - TileDirectionDelta[face_direction].y;
        sint32 x2 = x - TileDirectionDelta[face_direction].x;

        if (map_coord_is_connected(x2 / 32, y2 / 32, mapElement->base_height, face_direction))
            return;
    }

    // Name of ride is parameter of the format string
    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32, ride->name_arguments);
    if (gConfigNotifications.ride_warnings) {
    news_item_add_to_queue(1, STR_ENTRANCE_NOT_CONNECTED, ride_idx);
    }

    ride->connected_message_throttle = 3;
}

#pragma endregion

#pragma region Interface

static void ride_track_set_map_tooltip(rct_map_element *mapElement)
{
    sint32 rideIndex;
    Ride *ride;

    rideIndex = mapElement->properties.track.ride_index;
    ride = get_ride(rideIndex);

    set_map_tooltip_format_arg(0, rct_string_id, STR_RIDE_MAP_TIP);
    set_map_tooltip_format_arg(2, rct_string_id, ride->name);
    set_map_tooltip_format_arg(4, uint32, ride->name_arguments);

    rct_string_id formatSecondary;
    sint32 arg1 = 0;
    ride_get_status(rideIndex, &formatSecondary, &arg1);
    set_map_tooltip_format_arg(8, rct_string_id, formatSecondary);
    set_map_tooltip_format_arg(10, uint32, arg1);
}

static void ride_station_set_map_tooltip(rct_map_element *mapElement)
{
    sint32 i, rideIndex, stationIndex;
    Ride *ride;

    rideIndex = mapElement->properties.track.ride_index;
    ride = get_ride(rideIndex);

    stationIndex = map_element_get_station(mapElement);
    for (i = stationIndex; i >= 0; i--)
        if (ride->station_starts[i].xy == RCT_XY8_UNDEFINED)
            stationIndex--;

    set_map_tooltip_format_arg(0, rct_string_id, STR_RIDE_MAP_TIP);
    set_map_tooltip_format_arg(2, rct_string_id, ride->num_stations <= 1 ? STR_RIDE_STATION : STR_RIDE_STATION_X);
    set_map_tooltip_format_arg(4, rct_string_id, ride->name);
    set_map_tooltip_format_arg(6, uint32, ride->name_arguments);
    set_map_tooltip_format_arg(10, rct_string_id, RideComponentNames[RideNameConvention[ride->type].station].capitalised);
    set_map_tooltip_format_arg(12, uint16, stationIndex + 1);

    rct_string_id formatSecondary;
    sint32 arg1;
    ride_get_status(rideIndex, &formatSecondary, &arg1);
    set_map_tooltip_format_arg(14, rct_string_id, formatSecondary);
    set_map_tooltip_format_arg(16, uint32, arg1);
}

static void ride_entrance_set_map_tooltip(rct_map_element *mapElement)
{
    sint32 i, rideIndex, stationIndex;
    Ride *ride;

    rideIndex = mapElement->properties.track.ride_index;
    ride = get_ride(rideIndex);

    // Get the station
    stationIndex = map_element_get_station(mapElement);
    for (i = stationIndex; i >= 0; i--)
        if (ride->station_starts[i].xy == RCT_XY8_UNDEFINED)
            stationIndex--;

    if (mapElement->properties.entrance.type == ENTRANCE_TYPE_RIDE_ENTRANCE) {
        // Get the queue length
        sint32 queueLength = 0;
        if (ride->entrances[stationIndex].xy != RCT_XY8_UNDEFINED)
            queueLength = ride->queue_length[stationIndex];

        set_map_tooltip_format_arg(0, rct_string_id, STR_RIDE_MAP_TIP);
        set_map_tooltip_format_arg(2, rct_string_id, ride->num_stations <= 1 ? STR_RIDE_ENTRANCE : STR_RIDE_STATION_X_ENTRANCE);
        set_map_tooltip_format_arg(4, rct_string_id, ride->name);
        set_map_tooltip_format_arg(6, uint32, ride->name_arguments);
        set_map_tooltip_format_arg(12, uint16, stationIndex + 1);
        if (queueLength == 0) {
            set_map_tooltip_format_arg(14, rct_string_id, STR_QUEUE_EMPTY);
        } else if (queueLength == 1) {
            set_map_tooltip_format_arg(14, rct_string_id, STR_QUEUE_ONE_PERSON);
        } else {
            set_map_tooltip_format_arg(14, rct_string_id, STR_QUEUE_PEOPLE);
        }
        set_map_tooltip_format_arg(16, uint16, queueLength);
    } else {
        // Get the station
        stationIndex = map_element_get_station(mapElement);
        for (i = stationIndex; i >= 0; i--)
            if (ride->station_starts[i].xy == RCT_XY8_UNDEFINED)
                stationIndex--;

        set_map_tooltip_format_arg(0, rct_string_id, ride->num_stations <= 1 ? STR_RIDE_EXIT : STR_RIDE_STATION_X_EXIT);
        set_map_tooltip_format_arg(2, rct_string_id, ride->name);
        set_map_tooltip_format_arg(4, uint32, ride->name_arguments);
        set_map_tooltip_format_arg(10, uint16, stationIndex + 1);
    }
}

void ride_set_map_tooltip(rct_map_element *mapElement)
{
    if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE) {
        ride_entrance_set_map_tooltip(mapElement);
    } else {
        if (
            mapElement->properties.track.type == 2 ||
            mapElement->properties.track.type == 3 ||
            mapElement->properties.track.type == 1
        ) {
            ride_station_set_map_tooltip(mapElement);
        } else {
            ride_track_set_map_tooltip(mapElement);
        }
    }
}

/**
 *
 *  rct2: 0x006BC3AC
 * Update ride music parameters
 * @param x (ax)
 * @param y (cx)
 * @param z (dx)
 * @param sampleRate (di)
 * @param rideIndex (bl)
 * @param position (ebp)
 * @param tuneId (bh)
 * @returns new position (ebp)
 */
sint32 ride_music_params_update(sint16 x, sint16 y, sint16 z, uint8 rideIndex, uint16 sampleRate, uint32 position, uint8 *tuneId)
{
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gGameSoundsOff && g_music_tracking_viewport != (rct_viewport*)-1) {
        rct_xy16 rotatedCoords = { 0, 0 };

        switch (get_current_rotation()) {
            case 0:
                rotatedCoords.x = y - x;
                rotatedCoords.y = ((y + x) / 2) - z;
                break;
            case 1:
                rotatedCoords.x = -x - y;
                rotatedCoords.y = ((y - x) / 2) - z;
                break;
            case 2:
                rotatedCoords.x = x - y;
                rotatedCoords.y = ((-y - x) / 2) - z;
                break;
            case 3:
                rotatedCoords.x = y + x;
                rotatedCoords.y = ((x - y) / 2) - z;
                break;
        }
        rct_viewport* viewport = g_music_tracking_viewport;
        sint16 view_width = viewport->view_width;
        sint16 view_width2 = view_width * 2;
        sint16 view_x = viewport->view_x - view_width2;
        sint16 view_y = viewport->view_y - view_width;
        sint16 view_x2 = view_width2 + view_width2 + viewport->view_width + view_x;
        sint16 view_y2 = view_width + view_width + viewport->view_height + view_y;
        if (view_x >= rotatedCoords.x ||
            view_y >= rotatedCoords.y ||
            view_x2 < rotatedCoords.x ||
            view_y2 < rotatedCoords.y) {
                goto label58;
        }
        sint32 x2 = viewport->x + ((rotatedCoords.x - viewport->view_x) >> viewport->zoom);
        x2 *= 0x10000;
        uint16 screenwidth = context_get_width();
        if (screenwidth < 64) {
            screenwidth = 64;
        }
        sint32 pan_x = ((x2 / screenwidth) - 0x8000) >> 4;

        sint32 y2 = viewport->y + ((rotatedCoords.y - viewport->view_y) >> viewport->zoom);
        y2 *= 0x10000;
        uint16 screenheight = context_get_height();
        if (screenheight < 64) {
            screenheight = 64;
        }
        sint32 pan_y = ((y2 / screenheight) - 0x8000) >> 4;

        uint8 vol1 = -1;
        uint8 vol2 = -1;
        sint32 panx2 = pan_x;
        sint32 pany2 = pan_y;
        if (pany2 < 0) {
            pany2 = -pany2;
        }
        if (pany2 > 6143) {
            pany2 = 6143;
        }
        pany2 -= 2048;
        if (pany2 > 0) {
            pany2 = -((pany2 / 4) - 1024) / 4;
            vol1 = (uint8)pany2;
            if (pany2 >= 256) {
                vol1 = -1;
            }
        }

        if (panx2 < 0) {
            panx2 = -panx2;
        }
        if (panx2 > 6143) {
            panx2 = 6143;
        }
        panx2 -= 2048;
        if (panx2 > 0) {
            panx2 = -((panx2 / 4) - 1024) / 4;
            vol2 = (uint8)panx2;
            if (panx2 >= 256) {
                vol2 = -1;
            }
        }
        if (vol1 >= vol2) {
            vol1 = vol2;
        }
        if (vol1 < gVolumeAdjustZoom * 3) {
            vol1 = 0;
        } else {
            vol1 = vol1 - (gVolumeAdjustZoom * 3);
        }
        sint32 v32 = -(((uint8)(-vol1 - 1) * (uint8)(-vol1 - 1)) / 16) - 700;
        if (vol1 && v32 >= -4000) {
            if (pan_x > 10000) {
                pan_x = 10000;
            }
            if (pan_x < -10000) {
                pan_x = -10000;
            }
            rct_ride_music* ride_music = &gRideMusicList[0];
            sint32 channel = 0;
            uint32 a1;
            while (ride_music->ride_id != rideIndex || ride_music->tune_id != *tuneId) {
                ride_music++;
                channel++;
                if (channel >= AUDIO_MAX_RIDE_MUSIC) {
                    rct_ride_music_info* ride_music_info = gRideMusicInfoList[*tuneId];
                    a1 = position + ride_music_info->offset;
                    goto label51;
                }
            }
            sint32 playing = Mixer_Channel_IsPlaying(gRideMusicList[channel].sound_channel);
            if (!playing) {
                *tuneId = 0xFF;
                return 0;
            }
            a1 = (uint32)Mixer_Channel_GetOffset(gRideMusicList[channel].sound_channel);
        label51:
            if (a1 < gRideMusicInfoList[*tuneId]->length) {
                position = a1;
                rct_ride_music_params* ride_music_params = gRideMusicParamsListEnd;
                if (ride_music_params < &gRideMusicParamsList[countof(gRideMusicParamsList)]) {
                    ride_music_params->ride_id = rideIndex;
                    ride_music_params->tune_id = *tuneId;
                    ride_music_params->offset = a1;
                    ride_music_params->volume = v32;
                    ride_music_params->pan = pan_x;
                    ride_music_params->frequency = sampleRate;
                    gRideMusicParamsListEnd++;
                }
            } else {
                *tuneId = 0xFF;
                return 0;
            }
        } else {
        label58:
            ;
            rct_ride_music_info* ride_music_info = gRideMusicInfoList[*tuneId];
            position += ride_music_info->offset;
            if (position < ride_music_info->length) {
                return position;
            } else {
                *tuneId = 0xFF;
                return 0;
            }
        }
    }
    return position;
}

#define INIT_MUSIC_INFO(path_id, offset, length, unknown) (rct_ride_music_info[]){length, offset, path_id, unknown}

//0x009AF1C8
rct_ride_music_info* gRideMusicInfoList[NUM_DEFAULT_MUSIC_TRACKS] = {
    INIT_MUSIC_INFO(PATH_ID_CSS4, 1378, 8139054, 0),
    INIT_MUSIC_INFO(PATH_ID_CSS5, 1378, 7796656, 0),
    INIT_MUSIC_INFO(PATH_ID_CSS6, 1378, 15787850, 0),
    INIT_MUSIC_INFO(PATH_ID_CSS7, 1378, 15331658, 0),
    INIT_MUSIC_INFO(PATH_ID_CSS8, 1378, 17503414, 0),
    INIT_MUSIC_INFO(PATH_ID_CSS9, 1378, 7005802, 0),
    INIT_MUSIC_INFO(0, 1378, 0, 0),                     // Referred to the nearly empty CSS10.DAT file
    INIT_MUSIC_INFO(PATH_ID_CSS11, 1378, 7023288, 0),
    INIT_MUSIC_INFO(PATH_ID_CSS12, 1378, 2767948, 0),
    INIT_MUSIC_INFO(PATH_ID_CSS13, 1378, 3373390, 0),
    INIT_MUSIC_INFO(PATH_ID_CSS14, 1378, 20783042, 0),
    INIT_MUSIC_INFO(PATH_ID_CSS15, 1378, 10009312, 0),
    INIT_MUSIC_INFO(0, 1378, 0, 0),                     // Referred to the nearly empty CSS16.DAT file
    INIT_MUSIC_INFO(PATH_ID_CSS3, 689, 1244886, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS17, 2756, -1, 0),
    INIT_MUSIC_INFO(PATH_ID_CSS18, 2756, 8429568, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS19, 2756, 10143784, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS20, 2756, 12271656, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS21, 2756, 9680968, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS22, 2756, 10062056, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS23, 2756, 11067432, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS24, 2756, 12427456, 0),
    INIT_MUSIC_INFO(PATH_ID_CSS25, 2756, 15181512, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS26, 2756, 10694816, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS27, 2756, 10421232, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS28, 2756, 13118376, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS29, 2756, 15310892, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS30, 2756, 10215464, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS31, 2756, 11510316, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS32, 2756, 11771944, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS33, 2756, 10759724, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS34, 2756, 14030716, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS35, 2756, 11642576, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS36, 2756, 8953764, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS37, 2756, 13303852, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS38, 2756, 10093888, 1),
    INIT_MUSIC_INFO(PATH_ID_CUSTOM1, 2756, 0, 1),
    INIT_MUSIC_INFO(PATH_ID_CUSTOM2, 2756, 0, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS39, 2756, 7531564, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS40, 1378, 5291306, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS41, 2756, 27860700, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS42, 2756, 6774090, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS43, 2756, 15630412, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS44, 2756, 8209704, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS45, 2756, 10006740, 1),
    INIT_MUSIC_INFO(PATH_ID_CSS46, 2756, 6772776, 1),
};

/**
*  Play/update ride music based on structs updated in 0x006BC3AC
*  rct2: 0x006BC6D8
*/
void ride_music_update_final()
{
    rct_ride_music_params* edi = NULL;
    sint32 ebx = 0;
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)) {
        // TODO Allow circus music (CSS24) to play if ride music is disabled (that should be sound)
        if (!gGameSoundsOff && gConfigSound.ride_music_enabled && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)) {
            while (1) {
                sint32 v8 = 0;
                sint32 v9 = 1;
                rct_ride_music_params* ride_music_params = &gRideMusicParamsList[0];
                while (ride_music_params < gRideMusicParamsListEnd) {
                    if (ride_music_params->ride_id != (uint8)-1) {
                        v8++;
                        if (v9 >= ride_music_params->volume) {
                            v9 = ride_music_params->volume;
                            edi = ride_music_params;
                        }
                    }
                    ride_music_params++;
                }
                if (v8 <= 2) {
                    break;
                }
                edi->ride_id = -1;
            }

            // stop currently playing music that is not in music params list or not playing?
            rct_ride_music* ride_music = &gRideMusicList[0];
            sint32 channel = 0;
            do {
                if (ride_music->ride_id != (uint8)-1) {
                    rct_ride_music_params* ride_music_params = &gRideMusicParamsList[0];
                    while (ride_music_params < gRideMusicParamsListEnd) {
                        if (ride_music_params->ride_id == ride_music->ride_id && ride_music_params->tune_id == ride_music->tune_id) {
                            sint32 isplaying = Mixer_Channel_IsPlaying(gRideMusicList[channel].sound_channel);
                            if (isplaying) {
                                goto label32;
                            }
                            break;
                        }
                        ride_music_params++;
                    }
                    Mixer_Stop_Channel(gRideMusicList[channel].sound_channel);
                    ride_music->ride_id = -1;
                }
            label32:
                ride_music++;
                channel++;
            } while(channel < AUDIO_MAX_RIDE_MUSIC);

            for (rct_ride_music_params* ride_music_params = &gRideMusicParamsList[0]; ride_music_params < gRideMusicParamsListEnd; ride_music_params++) {
                if (ride_music_params->ride_id != (uint8)-1) {
                    rct_ride_music* ride_music_2 = &gRideMusicList[0];
                    sint32 channel2 = 0;
                    while (ride_music_params->ride_id != ride_music_2->ride_id || ride_music_params->tune_id != ride_music_2->tune_id) {
                        if (ride_music_2->ride_id == (uint8)-1) {
                            ebx = channel2;
                        }
                        ride_music_2++;
                        channel2++;
                        if (channel2 >= AUDIO_MAX_RIDE_MUSIC) {
                            rct_ride_music_info* ride_music_info = gRideMusicInfoList[ride_music_params->tune_id];
                            rct_ride_music* ride_music_3 = &gRideMusicList[ebx];
                            ride_music_3->sound_channel = Mixer_Play_Music(ride_music_info->path_id, MIXER_LOOP_NONE, true);
                            if (ride_music_3->sound_channel) {
                                ride_music_3->volume = ride_music_params->volume;
                                ride_music_3->pan = ride_music_params->pan;
                                ride_music_3->frequency = ride_music_params->frequency;
                                ride_music_3->ride_id = ride_music_params->ride_id;
                                ride_music_3->tune_id = ride_music_params->tune_id;
                                Mixer_Channel_Volume(ride_music_3->sound_channel, DStoMixerVolume(ride_music_3->volume));
                                Mixer_Channel_Pan(ride_music_3->sound_channel, DStoMixerPan(ride_music_3->pan));
                                Mixer_Channel_Rate(ride_music_3->sound_channel, DStoMixerRate(ride_music_3->frequency));
                                sint32 offset = ride_music_params->offset - 10000;
                                if (offset < 0) {
                                    offset = 0;
                                }
                                Mixer_Channel_SetOffset(ride_music_3->sound_channel, offset);

                                // Move circus music to the sound mixer group
                                if (ride_music_info->path_id == PATH_ID_CSS24) {
                                    Mixer_Channel_SetGroup(ride_music_3->sound_channel, MIXER_GROUP_SOUND);
                                }
                            }
                            return;
                        }
                    }

                    if (ride_music_params->volume != ride_music_2->volume) {
                        ride_music_2->volume = ride_music_params->volume;
                        Mixer_Channel_Volume(ride_music_2->sound_channel, DStoMixerVolume(ride_music_2->volume));
                    }
                    if (ride_music_params->pan != ride_music_2->pan) {
                        ride_music_2->pan = ride_music_params->pan;
                        Mixer_Channel_Pan(ride_music_2->sound_channel, DStoMixerPan(ride_music_2->pan));
                    }
                    if (ride_music_params->frequency != ride_music_2->frequency) {
                        ride_music_2->frequency = ride_music_params->frequency;
                        Mixer_Channel_Rate(ride_music_2->sound_channel, DStoMixerRate(ride_music_2->frequency));
                    }

                }
            }
        }
    }
}

#pragma endregion

static bool ride_is_mode_valid(Ride *ride, uint8 mode)
{
    rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
    const uint8 *availableModes = ride_seek_available_modes(ride);

    if ((rideEntry->flags & RIDE_ENTRY_DISABLE_FIRST_TWO_OPERATING_MODES) && !gConfigInterface.select_by_track_type && !gCheatsShowAllOperatingModes)
    {
        availableModes += 2;
    }

    for (; *availableModes != 0xFF; availableModes++) {
        if (*availableModes == mode) {
            return true;
        }
    }

    return false;
}

static bool ride_is_valid_lift_hill_speed(Ride *ride, sint32 speed)
{
    sint32 minSpeed = gCheatsFastLiftHill ? 0   : RideLiftData[ride->type].minimum_speed;
    sint32 maxSpeed = gCheatsFastLiftHill ? 255 : RideLiftData[ride->type].maximum_speed;
    return speed >= minSpeed && speed <= maxSpeed;
}

static bool ride_is_valid_num_circuits(Ride *ride, sint32 numCircuits)
{
    sint32 minNumCircuits = 1;
    sint32 maxNumCircuits = gCheatsFastLiftHill ? 255 : 20;
    return numCircuits >= minNumCircuits && numCircuits <= maxNumCircuits;
}

static bool ride_is_valid_operation_option(Ride *ride, uint8 value)
{
    uint8 minValue = RideProperties[ride->type].min_value;
    uint8 maxValue = RideProperties[ride->type].max_value;
    if (gCheatsFastLiftHill) {
        minValue = 0;
        maxValue = 255;
    }

    return value >= minValue && value <= maxValue;
}

static money32 ride_set_setting(uint8 rideIndex, uint8 setting, uint8 value, uint8 flags)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS;

    Ride *ride = get_ride(rideIndex);
    if (ride == NULL || ride->type == RIDE_TYPE_NULL) {
        log_warning("Invalid ride: #%u.", rideIndex);
        return MONEY32_UNDEFINED;
    }

    switch (setting) {
    case RIDE_SETTING_MODE:
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
            gGameCommandErrorText = STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING;
            return MONEY32_UNDEFINED;
        }

        if (ride->status != RIDE_STATUS_CLOSED) {
            gGameCommandErrorText = STR_MUST_BE_CLOSED_FIRST;
            return MONEY32_UNDEFINED;
        }

        if (!ride_is_mode_valid(ride, value)) {
            log_warning("Invalid ride mode.");
            return MONEY32_UNDEFINED;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            invalidate_test_results(rideIndex);
            ride_clear_for_construction(rideIndex);
            ride_remove_peeps(rideIndex);

            ride->mode = value;
            ride_update_max_vehicles(rideIndex);
        }
        break;
    case RIDE_SETTING_DEPARTURE:
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            ride->depart_flags = value;
        }
        break;
    case RIDE_SETTING_MIN_WAITING_TIME:
        if (value > 250) {
            log_warning("Invalid minimum waiting time.");
            return MONEY32_UNDEFINED;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            ride->min_waiting_time = value;
            ride->max_waiting_time = max(value, ride->max_waiting_time);
        }
        break;
    case RIDE_SETTING_MAX_WAITING_TIME:
        if (value > 250) {
            log_warning("Invalid maximum waiting time.");
            return MONEY32_UNDEFINED;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            ride->max_waiting_time = value;
            ride->min_waiting_time = min(value, ride->min_waiting_time);
        }
        break;
    case RIDE_SETTING_OPERATION_OPTION:
        if (!ride_is_valid_operation_option(ride, value)) {
            log_warning("Invalid operation option value.");
            return MONEY32_UNDEFINED;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            invalidate_test_results(rideIndex);
            ride->operation_option = value;
        }
        break;
    case RIDE_SETTING_INSPECTION_INTERVAL:
        if (value > RIDE_INSPECTION_NEVER) {
            log_warning("Invalid inspection interval.");
            return MONEY32_UNDEFINED;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            ride->inspection_interval = value;
        }
        break;
    case RIDE_SETTING_MUSIC:
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_MUSIC;
            if (value) {
                ride->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
            }
        }
        break;
    case RIDE_SETTING_MUSIC_TYPE:
        if (value >= MUSIC_STYLE_COUNT) {
            log_warning("Invalid music style.");
            return MONEY32_UNDEFINED;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            if (value != ride->music) {
                ride->music = value;
                ride->music_tune_id = 0xFF;
            }
        }
        break;
    case RIDE_SETTING_LIFT_HILL_SPEED:
        if (!ride_is_valid_lift_hill_speed(ride, value)) {
            log_warning("Invalid lift hill speed.");
            return MONEY32_UNDEFINED;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            if (value != ride->lift_hill_speed) {
                ride->lift_hill_speed = value;
                invalidate_test_results(rideIndex);
            }
        }
        break;
    case RIDE_SETTING_NUM_CIRCUITS:
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT && value > 1) {
            gGameCommandErrorText = STR_MULTICIRCUIT_NOT_POSSIBLE_WITH_CABLE_LIFT_HILL;
            return MONEY32_UNDEFINED;
        }

        if (!ride_is_valid_num_circuits(ride, value)) {
            log_warning("Invalid number of circuits.");
            return MONEY32_UNDEFINED;
        }

        if (flags & GAME_COMMAND_FLAG_APPLY) {
            if (value != ride->num_circuits) {
                ride->num_circuits = value;
                invalidate_test_results(rideIndex);
            }
        }
        break;
    case RIDE_SETTING_RIDE_TYPE:
        if (!gCheatsAllowArbitraryRideTypeChanges) {
            return MONEY32_UNDEFINED;
        }
        if (flags & GAME_COMMAND_FLAG_APPLY) {
            ride->type = value;
        }
        break;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY) {
        if (ride->overall_view.xy != RCT_XY8_UNDEFINED) {
            rct_xyz16 coord;
            coord.x = ride->overall_view.x * 32 + 16;
            coord.y = ride->overall_view.y * 32 + 16;
            coord.z = map_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        window_invalidate_by_number(WC_RIDE, rideIndex);
    }

    return 0;
}

/**
 *
 *  rct2: 0x006B5559
 */
void game_command_set_ride_setting(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    uint8 rideIndex = *edx & 0xFF;
    uint8 setting = (*edx >> 8) & 0xFF;
    uint8 newValue = (*ebx >> 8) & 0xFF;
    uint8 flags = *ebx & 0xFF;
    *ebx = ride_set_setting(rideIndex, setting, newValue, flags);
}

/**
 *
 *  rct2: 0x006B4CC1
 */
static sint32 ride_mode_check_valid_station_numbers(Ride *ride)
{
    uint8 no_stations = 0;
    for (uint8 station_index = 0; station_index < MAX_STATIONS; ++station_index){
        if (ride->station_starts[station_index].xy != RCT_XY8_UNDEFINED)
        {
            no_stations++;
        }
    }

    switch (ride->mode){
    case RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE:
    case RIDE_MODE_POWERED_LAUNCH_PASSTROUGH:
    case RIDE_MODE_POWERED_LAUNCH:
    case RIDE_MODE_LIM_POWERED_LAUNCH:
        if (no_stations <= 1) return 1;
        gGameCommandErrorText = STR_UNABLE_TO_OPERATE_WITH_MORE_THAN_ONE_STATION_IN_THIS_MODE;
        return 0;
    case RIDE_MODE_SHUTTLE:
        if (no_stations >= 2) return 1;
        gGameCommandErrorText = STR_UNABLE_TO_OPERATE_WITH_LESS_THAN_TWO_STATIONS_IN_THIS_MODE;
        return 0;
    }

    if (ride->type == RIDE_TYPE_GO_KARTS || ride->type == RIDE_TYPE_MINI_GOLF){
        if (no_stations <= 1) return 1;
        gGameCommandErrorText = STR_UNABLE_TO_OPERATE_WITH_MORE_THAN_ONE_STATION_IN_THIS_MODE;
        return 0;
    }

    return 1;
}

/**
 * returns stationIndex of first station on success
 * -1 on failure.
 */
static sint32 ride_mode_check_station_present(Ride* ride){
    sint32 stationIndex = ride_get_first_valid_station_start(ride);

    if (stationIndex == -1) {
        gGameCommandErrorText = STR_NOT_YET_CONSTRUCTED;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_NO_TRACK))
            return -1;

        if (ride->type == RIDE_TYPE_MAZE)
            return -1;

        gGameCommandErrorText = STR_REQUIRES_A_STATION_PLATFORM;
        return -1;
    }

    return stationIndex;
}

/**
 *
 *  rct2: 0x006B5872
 */
static sint32 ride_check_for_entrance_exit(sint32 rideIndex)
{
    Ride* ride = get_ride(rideIndex);

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
        return 1;

    sint32 i;
    uint8 entrance = 0;
    uint8 exit = 0;
    for (i = 0; i < MAX_STATIONS; i++) {
        if (ride->station_starts[i].xy == RCT_XY8_UNDEFINED)
            continue;

        if (ride->entrances[i].xy != RCT_XY8_UNDEFINED) {
            entrance = 1;
        }

        if (ride->exits[i].xy != RCT_XY8_UNDEFINED) {
            exit = 1;
        }

        // If station start and no entrance/exit
        // Sets same error message as no entrance
        if (ride->exits[i].xy == RCT_XY8_UNDEFINED && ride->entrances[i].xy == RCT_XY8_UNDEFINED){
            entrance = 0;
            break;
        }
    }

    if (entrance == 0){
        gGameCommandErrorText = STR_ENTRANCE_NOT_YET_BUILT;
        return 0;
    }

    if (exit == 0){
        gGameCommandErrorText = STR_EXIT_NOT_YET_BUILT;
        return 0;
    }

    return 1;
}

/**
 *
 *  rct2: 0x006B5952
 */
static void sub_6B5952(sint32 rideIndex)
{
    Ride *ride = get_ride(rideIndex);

    for (sint32 i = 0; i < MAX_STATIONS; i++) {
        rct_xy8 location = ride->entrances[i];
        if (location.xy == RCT_XY8_UNDEFINED)
            continue;

        sint32 x = location.x * 32;
        sint32 y = location.y * 32;
        sint32 z = ride->station_heights[i];

        rct_map_element *mapElement = map_get_first_element_at(x / 32, y / 32);
        do {
            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE)
                continue;
            if (mapElement->base_height != z)
                continue;

            sint32 direction = map_element_get_direction(mapElement);
            footpath_chain_ride_queue(rideIndex, i, x, y, mapElement, direction ^ 2);
        } while (!map_element_is_last_for_tile(mapElement++));
    }
}

/**
 *
 *  rct2: 0x006D3319
 */
static sint32 ride_check_block_brakes(rct_xy_element *input, rct_xy_element *output)
{
    rct_window *w;
    track_circuit_iterator it;
    sint32 rideIndex, type;

    rideIndex = input->element->properties.track.ride_index;
    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != NULL && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && _currentRideIndex == rideIndex)
        ride_construction_invalidate_current_track();

    track_circuit_iterator_begin(&it, *input);
    while (track_circuit_iterator_next(&it)) {
        if (it.current.element->properties.track.type == TRACK_ELEM_BLOCK_BRAKES) {
            type = it.last.element->properties.track.type;
            if (type == TRACK_ELEM_END_STATION) {
                gGameCommandErrorText = STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_STATION;
                *output = it.current;
                return 0;
            }
            if (type == TRACK_ELEM_BLOCK_BRAKES) {
                gGameCommandErrorText = STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_EACH_OTHER;
                *output = it.current;
                return 0;
            }
            if ((it.last.element->type & 0x80) && type != TRACK_ELEM_LEFT_CURVED_LIFT_HILL && type != TRACK_ELEM_RIGHT_CURVED_LIFT_HILL) {
                gGameCommandErrorText = STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_THE_TOP_OF_THIS_LIFT_HILL;
                *output = it.current;
                return 0;
            }
        }
    }
    if (!it.looped) {
        // Not sure why this is the case...
        gGameCommandErrorText = STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_STATION;
        *output = it.last;
        return 0;
    }

    return 1;
}

/**
 * Iterates along the track until an inversion (loop, corkscrew, barrel roll etc.) track piece is reached.
 * @param input The start track element and position.
 * @param output The first track element and position which is classified as an inversion.
 * @returns true if an inversion track piece is found, otherwise false.
 *  rct2: 0x006CB149
 */
static bool ride_check_track_contains_inversions(rct_xy_element *input, rct_xy_element *output)
{
    sint32 rideIndex = input->element->properties.track.ride_index;
    Ride *ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_MAZE)
        return true;

    rct_window *w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != NULL && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && rideIndex == _currentRideIndex) {
        ride_construction_invalidate_current_track();
    }

    track_circuit_iterator it;
    track_circuit_iterator_begin(&it, *input);
    while (track_circuit_iterator_next(&it)) {
        sint32 trackType = it.current.element->properties.track.type;
        if (TrackFlags[trackType] & TRACK_ELEM_FLAG_INVERSION_TO_NORMAL) {
            *output = it.current;
            return true;
        }
    }
    return false;
}

/**
 * Iterates along the track until a banked track piece is reached.
 * @param input The start track element and position.
 * @param output The first track element and position which is banked.
 * @returns true if a banked track piece is found, otherwise false.
 *  rct2: 0x006CB1D3
 */
static bool ride_check_track_contains_banked(rct_xy_element *input, rct_xy_element *output)
{
    sint32 rideIndex = input->element->properties.track.ride_index;
    Ride *ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_MAZE)
        return true;

    rct_window *w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != NULL && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && rideIndex == _currentRideIndex) {
        ride_construction_invalidate_current_track();
    }

    track_circuit_iterator it;
    track_circuit_iterator_begin(&it, *input);
    while (track_circuit_iterator_next(&it)) {
        sint32 trackType = output->element->properties.track.type;
        if (TrackFlags[trackType] & TRACK_ELEM_FLAG_BANKED) {
            *output = it.current;
            return true;
        }
    }
    return false;
}

/**
 *
 *  rct2: 0x006CB25D
 */
static sint32 ride_check_station_length(rct_xy_element *input, rct_xy_element *output)
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != 0 &&
        _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 &&
        _currentRideIndex == input->element->properties.track.ride_index){
        ride_construction_invalidate_current_track();
    }

    output->x = input->x;
    output->y = input->y;
    output->element = input->element;
    track_begin_end trackBeginEnd;
    while (track_block_get_previous(output->x, output->y, output->element, &trackBeginEnd)){
        output->x = trackBeginEnd.begin_x;
        output->y = trackBeginEnd.begin_y;
        output->element = trackBeginEnd.begin_element;
    }

    sint32 num_station_elements = 0;
    rct_xy_element last_good_station = *output;

    do{
        if (TrackSequenceProperties[output->element->properties.track.type][0] & TRACK_SEQUENCE_FLAG_ORIGIN){
            num_station_elements++;
            last_good_station = *output;
        }
        else{
            if (num_station_elements == 0)
                continue;
            if (num_station_elements == 1){
                return 0;
            }
            num_station_elements = 0;
        }
    } while (track_block_get_next(output, output, NULL, NULL));

    // Prevent returning a pointer to a map element with no track.
    *output = last_good_station;
    if (num_station_elements == 1)
        return 0;

    return 1;
}

/**
 *
 *  rct2: 0x006CB2DA
 */
static bool ride_check_start_and_end_is_station(rct_xy_element *input, rct_xy_element *output)
{
    rct_window *w;
    Ride *ride;
    sint32 rideIndex, trackType;
    rct_xy_element trackBack, trackFront;

    rideIndex = input->element->properties.track.ride_index;
    ride = get_ride(rideIndex);

    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != NULL && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && rideIndex == _currentRideIndex) {
        ride_construction_invalidate_current_track();
    }

    // Check back of the track
    track_get_back(input, &trackBack);
    trackType = trackBack.element->properties.track.type;
    if (!(TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_ORIGIN)) {
        return false;
    }
    ride->chairlift_bullwheel_location[0].x = trackBack.x >> 5;
    ride->chairlift_bullwheel_location[0].y = trackBack.y >> 5;
    ride->chairlift_bullwheel_z[0] = trackBack.element->base_height;

    // Check front of the track
    track_get_front(input, &trackFront);
    trackType = trackFront.element->properties.track.type;
    if (!(TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_ORIGIN)) {
        return false;
    }
    ride->chairlift_bullwheel_location[1].x = trackFront.x >> 5;
    ride->chairlift_bullwheel_location[1].y = trackFront.y >> 5;
    ride->chairlift_bullwheel_z[1] = trackFront.element->base_height;
    return true;
}

/**
 * Sets the position and direction of the returning point on the track of a boat hire ride. This will either be the end of the
 * station or the last track piece from the end of the direction.
 *  rct2: 0x006B4D39
 */
static void ride_set_boat_hire_return_point(Ride *ride, rct_xy_element *startElement)
{
    sint32 trackType = -1;
    sint32 returnX = startElement->x;
    sint32 returnY = startElement->y;
    sint32 startX = returnX;
    sint32 startY = returnY;
    rct_map_element *returnTrackElement = startElement->element;
    track_begin_end trackBeginEnd;
    while (track_block_get_previous(returnX, returnY, returnTrackElement, &trackBeginEnd)) {
        // If previous track is back to the starting x, y, then break loop (otherwise possible infinite loop)
        if (trackType != -1 && startX == trackBeginEnd.begin_x && startY == trackBeginEnd.begin_y) break;

        sint32 x = trackBeginEnd.begin_x;
        sint32 y = trackBeginEnd.begin_y;
        sint32 z = trackBeginEnd.begin_z;
        sint32 direction = trackBeginEnd.begin_direction;
        trackType = trackBeginEnd.begin_element->properties.track.type;
        sub_6C683D(&x, &y, &z, direction, trackType, 0, &returnTrackElement, 0);
        returnX = x;
        returnY = y;
    };

    trackType = returnTrackElement->properties.track.type;
    sint32 elementReturnDirection = TrackCoordinates[trackType].rotation_begin;
    ride->boat_hire_return_direction = (returnTrackElement->type + elementReturnDirection) & 3;
    ride->boat_hire_return_position.x = returnX >> 5;
    ride->boat_hire_return_position.y = returnY >> 5;
}

/**
 *
 *  rct2: 0x006B4D39
 */
static void ride_set_maze_entrance_exit_points(Ride *ride)
{
    // Needs room for an entrance and an exit per station, plus one position for the list terminator.
    uint16 positions[(MAX_STATIONS * 2) + 1];

    // Create a list of all the entrance and exit positions
    uint16 *position = positions;
    for (sint32 i = 0; i < MAX_STATIONS; i++) {
        if (ride->entrances[i].xy != RCT_XY8_UNDEFINED) {
            *position++ = ride->entrances[i].xy;
        }
        if (ride->exits[i].xy != RCT_XY8_UNDEFINED) {
            *position++ = ride->exits[i].xy;
        }
    }
    *position++ = RCT_XY8_UNDEFINED;

    // Enumerate entrance and exit positions
    for (position = positions; *position != RCT_XY8_UNDEFINED; position++) {
        sint32 x = (*position & 0xFF) << 5;
        sint32 y = (*position >> 8) << 5;
        sint32 z = ride->station_heights[0];

        rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
        do {
            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE) continue;
            if (
                mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE &&
                mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_EXIT
            ) {
                continue;
            }
            if (mapElement->base_height != z) continue;

            maze_entrance_hedge_removal(x, y, mapElement);
        } while (!map_element_is_last_for_tile(mapElement++));
    }
}

/**
 * Sets a flag on all the track elements that can be the start of a circuit block. i.e. where a train can start.
 *  rct2: 0x006B4E6B
 */
static void ride_set_block_points(rct_xy_element *startElement)
{
    rct_xy_element currentElement = *startElement;
    do {
        sint32 trackType = currentElement.element->properties.track.type;
        switch (trackType) {
        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_CABLE_LIFT_HILL:
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        case TRACK_ELEM_60_DEG_UP_TO_FLAT:
        case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
        case TRACK_ELEM_BLOCK_BRAKES:
            currentElement.element->flags &= ~(1 << 5);
            break;
        }
    } while (track_block_get_next(&currentElement, &currentElement, NULL, NULL) && currentElement.element != startElement->element);
}

/**
 *
 *  rct2: 0x006B4D26
 */
static void ride_set_start_finish_points(sint32 rideIndex, rct_xy_element *startElement)
{
    Ride *ride = get_ride(rideIndex);

    switch (ride->type) {
    case RIDE_TYPE_BOAT_RIDE:
        ride_set_boat_hire_return_point(ride, startElement);
        break;
    case RIDE_TYPE_MAZE:
        ride_set_maze_entrance_exit_points(ride);
        break;
    }

    if (ride_is_block_sectioned(ride) && !(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)) {
        ride_set_block_points(startElement);
    }
}

/**
 *
 *  rct2: 0x0069ED9E
 */
static sint32 count_free_misc_sprite_slots()
{
    sint32 miscSpriteCount = gSpriteListCount[SPRITE_LIST_MISC];
    sint32 remainingSpriteCount = gSpriteListCount[SPRITE_LIST_NULL];
    return max(0, miscSpriteCount + remainingSpriteCount - 300);
}

const rct_xy16 word_9A3AB4[4] = {
    {   0,   0 },
    {   0, -96 },
    { -96, -96 },
    { -96,   0 },
};

const rct_xy16 word_9A2A60[] = {
    {   0,  16 },
    {  16,  31 },
    {  31,  16 },
    {  16,   0 },
    {  16,  16 },
    {  64,  64 },
    {  64, -32 },
    { -32, -32 },
    { -32,  64 },
};

/**
 *
 *  rct2: 0x006DD90D
 */
static rct_vehicle *vehicle_create_car(
    sint32 rideIndex,
    sint32 vehicleEntryIndex,
    sint32 carIndex,
    sint32 vehicleIndex,
    sint32 x,
    sint32 y,
    sint32 z,
    sint32 *remainingDistance,
    rct_map_element *mapElement
) {
    registers regs = { 0 };

    Ride *ride = get_ride(rideIndex);
    rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
    rct_ride_entry_vehicle *vehicleEntry = &rideEntry->vehicles[vehicleEntryIndex];

    rct_vehicle *vehicle = (rct_vehicle*)create_sprite(1);
    vehicle->sprite_identifier = SPRITE_IDENTIFIER_VEHICLE;
    vehicle->ride = rideIndex;
    vehicle->ride_subtype = ride->subtype;

    vehicle->vehicle_type = vehicleEntryIndex;
    vehicle->is_child = carIndex == 0 ? 0 : 1;
    vehicle->var_44 = ror32(vehicleEntry->spacing, 10) & 0xFFFF;
    regs.edx = vehicleEntry->spacing >> 1;
    *remainingDistance -= regs.edx;
    vehicle->remaining_distance = *remainingDistance;
    if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_30)) {
        *remainingDistance -= regs.edx;
    }

    // loc_6DD9A5:
    vehicle->sprite_width = vehicleEntry->sprite_width;
    vehicle->sprite_height_negative = vehicleEntry->sprite_height_negative;
    vehicle->sprite_height_positive = vehicleEntry->sprite_height_positive;
    vehicle->friction = vehicleEntry->car_friction;
    vehicle->num_seats = vehicleEntry->num_seats;
    vehicle->speed = vehicleEntry->powered_max_speed;
    vehicle->powered_acceleration = vehicleEntry->powered_acceleration;
    vehicle->velocity = 0;
    vehicle->acceleration = 0;
    vehicle->var_4A = 0;
    vehicle->swinging_car_var_0 = 0;
    vehicle->var_4E = 0;
    vehicle->restraints_position = 0;
    vehicle->var_BA = 0;
    vehicle->var_B6 = 0;
    vehicle->var_B8 = 0;
    vehicle->sound1_id = RCT12_SOUND_ID_NULL;
    vehicle->sound2_id = RCT12_SOUND_ID_NULL;
    vehicle->next_vehicle_on_train = SPRITE_INDEX_NULL;
    vehicle->var_C4 = 0;
    vehicle->var_C5 = 0;
    vehicle->var_C8 = 0;
    vehicle->scream_sound_id = 255;
    vehicle->vehicle_sprite_type = 0;
    vehicle->bank_rotation = 0;
    vehicle->target_seat_rotation = 4;
    vehicle->seat_rotation = 4;
    for (sint32 i = 0; i < 32; i++) {
        vehicle->peep[i] = SPRITE_INDEX_NULL;
    }

    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_31) {
        // loc_6DDCA4:
        vehicle->var_CD = 0;
        sint32 direction = map_element_get_direction(mapElement);
        x += word_9A3AB4[direction].x;
        y += word_9A3AB4[direction].y;
        z = mapElement->base_height * 8;
        vehicle->track_x = x;
        vehicle->track_y = y;
        vehicle->track_z = z;
        vehicle->current_station = map_element_get_station(mapElement);

        z += RideData5[ride->type].z_offset;

        vehicle->track_type = mapElement->properties.track.type << 2;
        vehicle->track_progress = 0;
        vehicle->status = 0;
        vehicle->sub_state = 0;
        vehicle->update_flags = 0;

        rct_xy16 chosenLoc;
        // loc_6DDD26:
        do {
            vehicle->sprite_direction = scenario_rand() & 0x1E;
            chosenLoc.y = y + (scenario_rand() & 0xFF);
            chosenLoc.x = x + (scenario_rand() & 0xFF);
        } while (vehicle_update_dodgems_collision(vehicle, chosenLoc.x, chosenLoc.y, NULL));

        sprite_move(chosenLoc.x, chosenLoc.y, z, (rct_sprite*)vehicle);
    } else {
        regs.dl = 0;
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_28) {
            regs.dl = 1;
        }

        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_30) {
            // Choose which lane Go Kart should start in
            regs.dl = 5;
            if (vehicleIndex & 1) {
                regs.dl = 6;
            }
        }
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_MINI_GOLF) {
            regs.dl = 9;
            vehicle->var_D3 = 0;
            vehicle->mini_golf_current_animation = 0;
            vehicle->mini_golf_flags = 0;
        }
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_4) {
            if (!vehicle->is_child) {
                regs.dl = 15;
            }
        }
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_5) {
            regs.dl = 16;
        }
        vehicle->var_CD = regs.dl;

        vehicle->track_x = x;
        vehicle->track_y = y;

        sint32 direction = map_element_get_direction(mapElement);
        vehicle->sprite_direction = direction << 3;

        if (ride->type == RIDE_TYPE_SPACE_RINGS) {
            direction = 4;
        } else {
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_16)) {
                if (RideConstructionDefaultTrackType[ride->type] != FLAT_TRACK_ELEM_1_X_4_B) {
                    if (RideConstructionDefaultTrackType[ride->type] != FLAT_TRACK_ELEM_1_X_4_A) {
                        if (ride->type == RIDE_TYPE_ENTERPRISE) {
                            direction += 5;
                        } else {
                            direction = 4;
                        }
                    }
                }
            }
        }

        x += word_9A2A60[direction].x;
        y += word_9A2A60[direction].y;
        vehicle->track_z = mapElement->base_height * 8;

        vehicle->current_station = map_element_get_station(mapElement);
        z = mapElement->base_height * 8;
        z += RideData5[ride->type].z_offset;

        sprite_move(x, y, z, (rct_sprite*)vehicle);
        vehicle->track_type = (mapElement->properties.track.type << 2) | (vehicle->sprite_direction >> 3);
        vehicle->track_progress = 31;
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_MINI_GOLF) {
            vehicle->track_progress = 15;
        }
        vehicle->update_flags = VEHICLE_UPDATE_FLAG_1;
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET) {
            if (mapElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED) {
                vehicle->update_flags |= VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES;
            }
        }
        vehicle->status = VEHICLE_STATUS_MOVING_TO_END_OF_STATION;
        vehicle->sub_state = 0;
    }

    // loc_6DDD5E:
    vehicle->num_peeps = 0;
    vehicle->next_free_seat = 0;
    return vehicle;
}

/**
 *
 *  rct2: 0x006DD84C
 */
static train_ref vehicle_create_train(sint32 rideIndex, sint32 x, sint32 y, sint32 z, sint32 vehicleIndex, sint32 *remainingDistance, rct_map_element *mapElement)
{
    Ride *ride = get_ride(rideIndex);

    train_ref train = { NULL, NULL };
    for (sint32 carIndex = 0; carIndex < ride->num_cars_per_train; carIndex++) {
        const uint8 vehicle = ride_entry_get_vehicle_at_position(ride->subtype, ride->num_cars_per_train, carIndex);
        rct_vehicle *car = vehicle_create_car(rideIndex, vehicle, carIndex, vehicleIndex, x, y, z, remainingDistance, mapElement);
        if (carIndex == 0) {
            train.head = car;
        } else {
            // Link the previous car with this car
            train.tail->next_vehicle_on_train = car->sprite_index;
            train.tail->next_vehicle_on_ride = car->sprite_index;
            car->prev_vehicle_on_ride = train.tail->sprite_index;
        }
        train.tail = car;
    }
    return train;
}

static void vehicle_create_trains(sint32 rideIndex, sint32 x, sint32 y, sint32 z, rct_map_element *mapElement)
{
    Ride *ride = get_ride(rideIndex);
    train_ref firstTrain = { 0 }, lastTrain = { 0 };
    sint32 remainingDistance = 0;

    for (sint32 vehicleIndex = 0; vehicleIndex < ride->num_vehicles; vehicleIndex++) {
        if (ride_is_block_sectioned(ride)) {
            remainingDistance = 0;
        }
        train_ref train = vehicle_create_train(rideIndex, x, y, z, vehicleIndex, &remainingDistance, mapElement);
        if (vehicleIndex == 0) {
            firstTrain = train;
        } else {
            // Link the end of the previous train with the front of this train
            lastTrain.tail->next_vehicle_on_ride = train.head->sprite_index;
            train.head->prev_vehicle_on_ride = lastTrain.tail->sprite_index;
        }
        lastTrain = train;

        // Add train to ride vehicle list
        move_sprite_to_list((rct_sprite*)train.head, SPRITE_LIST_TRAIN * 2);
        for (sint32 i = 0; i < MAX_VEHICLES_PER_RIDE; i++) {
            if (ride->vehicles[i] == SPRITE_INDEX_NULL) {
                ride->vehicles[i] = train.head->sprite_index;
                break;
            }
        }
    }

    // Link the first train and last train together
    firstTrain.head->prev_vehicle_on_ride = lastTrain.tail->sprite_index;
    lastTrain.tail->next_vehicle_on_ride = firstTrain.head->sprite_index;
}

static void vehicle_unset_var_48_b1(rct_vehicle *head)
{
    rct_vehicle *vehicle = head;
    while (true) {
        vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_1;
        uint16 spriteIndex = vehicle->next_vehicle_on_train;
        if (spriteIndex == SPRITE_INDEX_NULL) {
            break;
        }
        vehicle = GET_VEHICLE(spriteIndex);
    }
}

/**
 *
 *  rct2: 0x006DDE9E
 */
static void ride_create_vehicles_find_first_block(Ride *ride, rct_xy_element *outXYElement)
{
    rct_vehicle *vehicle = GET_VEHICLE(ride->vehicles[0]);
    sint32 firstX = vehicle->track_x;
    sint32 firstY = vehicle->track_y;
    sint32 firstZ = vehicle->track_z;
    rct_map_element *firstElement = map_get_track_element_at(firstX, firstY, firstZ / 8);

    assert(firstElement != NULL);

    sint32 x = firstX;
    sint32 y = firstY;
    rct_map_element *trackElement = firstElement;
    track_begin_end trackBeginEnd;
    while (track_block_get_previous(x, y, trackElement, &trackBeginEnd)) {
        x = trackBeginEnd.end_x;
        y = trackBeginEnd.end_y;
        trackElement = trackBeginEnd.begin_element;
        if (x == firstX && y == firstY && trackElement == firstElement) {
            break;
        }

        sint32 trackType = trackElement->properties.track.type;
        switch (trackType) {
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        case TRACK_ELEM_60_DEG_UP_TO_FLAT:
            if (track_element_is_lift_hill(trackElement)) {
                outXYElement->x = x;
                outXYElement->y = y;
                outXYElement->element = trackElement;
                return;
            }
            break;
        case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
            if (track_element_is_lift_hill(trackElement)) {
                rct_map_element *mapElement = map_get_first_element_at(trackBeginEnd.begin_x >> 5, trackBeginEnd.begin_y >> 5);
                do {
                    if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;
                    if (mapElement->properties.track.type != trackType) continue;
                    if (map_element_get_track_sequence(mapElement) != 0) continue;
                    if (mapElement->base_height != trackBeginEnd.begin_z / 8) continue;
                    outXYElement->x = trackBeginEnd.begin_x;
                    outXYElement->y = trackBeginEnd.begin_y;
                    outXYElement->element = mapElement;
                    return;
                } while (!map_element_is_last_for_tile(mapElement++));
            }
            break;
        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_CABLE_LIFT_HILL:
        case TRACK_ELEM_BLOCK_BRAKES:
            outXYElement->x = x;
            outXYElement->y = y;
            outXYElement->element = trackElement;
            return;
        }
    }

    outXYElement->x = firstX;
    outXYElement->y = firstY;
    outXYElement->element = firstElement;
}

/**
 *
 *  rct2: 0x006DD84C
 */
static bool ride_create_vehicles(Ride *ride, sint32 rideIndex, rct_xy_element *element, sint32 isApplying)
{
    ride_update_max_vehicles(rideIndex);
    if (ride->subtype == RIDE_ENTRY_INDEX_NULL) {
        return true;
    }

    // Check if there are enough free sprite slots for all the vehicles
    sint32 totalCars = ride->num_vehicles * ride->num_cars_per_train;
    if (totalCars > count_free_misc_sprite_slots()) {
        gGameCommandErrorText = STR_UNABLE_TO_CREATE_ENOUGH_VEHICLES;
        return false;
    }

    if (!isApplying) {
        return true;
    }

    rct_map_element *mapElement = element->element;
    sint32 x = element->x;
    sint32 y = element->y;
    sint32 z = element->element->base_height;
    sint32 direction = map_element_get_direction(mapElement);

    //
    if (ride->mode == RIDE_MODE_STATION_TO_STATION) {
        x = element->x - TileDirectionDelta[direction].x;
        y = element->y - TileDirectionDelta[direction].y;

        mapElement = map_get_first_element_at(x >> 5, y >> 5);
        do {
            if (mapElement->base_height != z) continue;
            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;
            break;
        } while (!map_element_is_last_for_tile(mapElement++));

        z = mapElement->base_height;
        direction = map_element_get_direction(mapElement);
    }

    vehicle_create_trains(rideIndex, x, y, z, mapElement);
    // return true;

    // Initialise station departs
// 006DDDD0:
    ride->lifecycle_flags |= RIDE_LIFECYCLE_ON_TRACK;
    for (sint32 i = 0; i < MAX_STATIONS; i++) {
        ride->station_depart[i] = (ride->station_depart[i] & 0x80) | 1;
    }

    //
    if (ride->type != RIDE_TYPE_SPACE_RINGS && !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_16)) {
        if (ride_is_block_sectioned(ride)) {
            rct_xy_element firstBlock;
            ride_create_vehicles_find_first_block(ride, &firstBlock);
            loc_6DDF9C(ride, firstBlock.element);
        } else {
            for (sint32 i = 0; i < ride->num_vehicles; i++) {
                rct_vehicle *vehicle = GET_VEHICLE(ride->vehicles[i]);

                rct_ride_entry_vehicle *vehicleEntry = vehicle_get_vehicle_entry(vehicle);

                if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_31)) {
                    vehicle_update_track_motion(vehicle, NULL);
                }

                vehicle_unset_var_48_b1(vehicle);
            }
        }
    }
    ride_update_vehicle_colours(rideIndex);
    return true;
}

/**
 *
 *  rct2: 0x006DDF9C
 */
void loc_6DDF9C(Ride *ride, rct_map_element *mapElement)
{
    rct_vehicle *train, *car;

    for (sint32 i = 0; i < ride->num_vehicles; i++) {
        train = GET_VEHICLE(ride->vehicles[i]);
        if (i == 0) {
            vehicle_update_track_motion(train, NULL);
            vehicle_unset_var_48_b1(train);
            continue;
        }

        vehicle_update_track_motion(train, NULL);

        do {
            mapElement->flags |= MAP_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED;
            car = train;
            while (true) {
                car->velocity = 0;
                car->acceleration = 0;
                car->var_4A = 0;
                car->remaining_distance += 13962;

                uint16 spriteIndex = car->next_vehicle_on_train;
                if (spriteIndex == SPRITE_INDEX_NULL) {
                    break;
                }
                car = GET_VEHICLE(spriteIndex);
            }
        } while (!(vehicle_update_track_motion(train, NULL) & VEHICLE_UPDATE_MOTION_TRACK_FLAG_10));

        mapElement->flags |= MAP_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED;
        car = train;
        while (true) {
            car->update_flags &= ~VEHICLE_UPDATE_FLAG_1;
            car->status = VEHICLE_STATUS_TRAVELLING;
            if ((car->track_type >> 2) == TRACK_ELEM_END_STATION) {
                car->status = VEHICLE_STATUS_MOVING_TO_END_OF_STATION;
            }

            uint16 spriteIndex = car->next_vehicle_on_train;
            if (spriteIndex == SPRITE_INDEX_NULL) {
                break;
            }
            car = GET_VEHICLE(spriteIndex);
        }
    }
}

/**
 * Checks and initialises the cable lift track returns false if unable to find
 * appropriate track.
 *  rct2: 0x006D31A6
 */
static bool ride_initialise_cable_lift_track(Ride *ride, bool isApplying)
{
    rct_xy8 location;
    sint32 stationIndex;
    for (stationIndex = 0; stationIndex < MAX_STATIONS; stationIndex++) {
        location = ride->station_starts[stationIndex];
        if (location.xy != RCT_XY8_UNDEFINED) break;
        if (stationIndex == 3) {
            gGameCommandErrorText = STR_CABLE_LIFT_HILL_MUST_START_IMMEDIATELY_AFTER_STATION;
            return false;
        }
    }

    sint32 x = location.x * 32;
    sint32 y = location.y * 32;
    sint32 z = ride->station_heights[stationIndex];

    bool success = false;
    rct_map_element *mapElement = map_get_first_element_at(location.x, location.y);
    do {
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;
        if (mapElement->base_height != z) continue;

        if (!(TrackSequenceProperties[mapElement->properties.track.type][0] & TRACK_SEQUENCE_FLAG_ORIGIN)) {
            continue;
        }
        success = true;
        break;
    } while (!map_element_is_last_for_tile(mapElement++));

    if (!success)
        return false;

    enum {
        STATE_FIND_CABLE_LIFT,
        STATE_FIND_STATION,
        STATE_REST_OF_TRACK
    };
    sint32 state = STATE_FIND_CABLE_LIFT;

    track_circuit_iterator it;
    track_circuit_iterator_begin(&it, (rct_xy_element){
        .x = x,
        .y = y,
        .element = mapElement
    });
    while (track_circuit_iterator_previous(&it)) {
        mapElement = it.current.element;
        sint32 trackType = mapElement->properties.track.type;

        uint16 flags = 16;
        switch (state) {
        case STATE_FIND_CABLE_LIFT:
            // Search for a cable lift hill track element
            if (trackType == TRACK_ELEM_CABLE_LIFT_HILL) {
                flags = 8;
                state = STATE_FIND_STATION;
            }
            break;
        case STATE_FIND_STATION:
            // Search for the start of the hill
            switch (trackType) {
            case TRACK_ELEM_FLAT:
            case TRACK_ELEM_25_DEG_UP:
            case TRACK_ELEM_60_DEG_UP:
            case TRACK_ELEM_FLAT_TO_25_DEG_UP:
            case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
            case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
            case TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE:
                flags = 8;
                break;
            case TRACK_ELEM_END_STATION:
                state = STATE_REST_OF_TRACK;
                break;
            default:
                gGameCommandErrorText = STR_CABLE_LIFT_HILL_MUST_START_IMMEDIATELY_AFTER_STATION;
                return false;
            }
            break;
        }
        if (isApplying) {
            z = mapElement->base_height * 8;
            sint32 direction = map_element_get_direction(mapElement);
            trackType = mapElement->properties.track.type;
            x = it.current.x;
            y = it.current.y;
            sub_6C683D(&x, &y, &z, direction, trackType, 0, &mapElement, flags);
        }
    }
    return true;
}

/**
 *
 *  rct2: 0x006DF4D4
 */
static bool ride_create_cable_lift(sint32 rideIndex, bool isApplying)
{
    Ride *ride = get_ride(rideIndex);

    if (ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED &&
        ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT
    ) {
        gGameCommandErrorText = STR_CABLE_LIFT_UNABLE_TO_WORK_IN_THIS_OPERATING_MODE;
        return false;
    }

    if (ride->num_circuits > 1) {
        gGameCommandErrorText = STR_MULTICIRCUIT_NOT_POSSIBLE_WITH_CABLE_LIFT_HILL;
        return false;
    }

    if (count_free_misc_sprite_slots() <= 5) {
        gGameCommandErrorText = STR_UNABLE_TO_CREATE_ENOUGH_VEHICLES;
        return false;
    }

    if (!ride_initialise_cable_lift_track(ride, isApplying)) {
        return false;
    }

    if (!isApplying) {
        return true;
    }

    sint32 x = ride->cable_lift_x;
    sint32 y = ride->cable_lift_y;
    sint32 z = ride->cable_lift_z;
    rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;
        if (mapElement->base_height != z) continue;
        break;
    } while (!map_element_is_last_for_tile(mapElement++));
    sint32 direction = map_element_get_direction(mapElement);

    rct_vehicle *head = NULL;
    rct_vehicle *tail = NULL;
    uint32 ebx = 0;
    for (sint32 i = 0; i < 5; i++) {
        uint32 edx = ror32(0x15478, 10);
        uint16 var_44 = edx & 0xFFFF;
        edx = rol32(edx, 10) >> 1;
        ebx -= edx;
        sint32 remaining_distance = ebx;
        ebx -= edx;

        rct_vehicle *current = cable_lift_segment_create(rideIndex, x, y, z, direction, var_44, remaining_distance, i == 0);
        current->next_vehicle_on_train = SPRITE_INDEX_NULL;
        if (i == 0) {
            head = current;
        } else {
            tail->next_vehicle_on_train = current->sprite_index;
            tail->next_vehicle_on_ride = current->sprite_index;
            current->prev_vehicle_on_ride = tail->sprite_index;
        }
        tail = current;
    }
    head->prev_vehicle_on_ride = tail->sprite_index;
    tail->next_vehicle_on_ride = head->sprite_index;

    ride->lifecycle_flags |= RIDE_LIFECYCLE_CABLE_LIFT;
    cable_lift_update_track_motion(head);
    return true;
}

/**
 *
 *  rct2: 0x006B51C0
 */
static void loc_6B51C0(sint32 rideIndex)
{
    Ride *ride = get_ride(rideIndex);

    if (gUnk141F568 != gUnk13CA740)
        return;

    rct_window *w = window_get_main();
    if (w == NULL)
        return;

    sint8 entranceOrExit = -1;
    sint32 i;
    for (i = 0; i < MAX_STATIONS; i++) {
        if (ride->station_starts[i].xy == RCT_XY8_UNDEFINED)
            continue;

        if (ride->entrances[i].xy == RCT_XY8_UNDEFINED) {
            entranceOrExit = 0;
            break;
        }

        if (ride->exits[i].xy == RCT_XY8_UNDEFINED) {
            entranceOrExit = 1;
            break;
        }
    }

    if (entranceOrExit == -1)
        return;

    if (ride->type != RIDE_TYPE_MAZE) {
        sint32 x = ride->station_starts[i].x * 32;
        sint32 y = ride->station_starts[i].y * 32;
        sint32 z = ride->station_heights[i] * 8;
        window_scroll_to_location(w, x, y, z);

        rct_xy_element trackElement;
        ride_try_get_origin_element(rideIndex, &trackElement);
        ride_find_track_gap(&trackElement, &trackElement);
        sint32 ok = ride_modify(&trackElement);
        if (ok == 0) {
            return;
        }

        w = window_find_by_class(WC_RIDE_CONSTRUCTION);
        if (w != NULL)
            window_event_mouse_up_call(w, WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE + entranceOrExit);
    }
}

/**
 *
 *  rct2: 0x006B528A
 */
static void ride_scroll_to_track_error(rct_xy_element *trackElement)
{
    if (!gGameCommandIsNetworked && gUnk141F568 == gUnk13CA740) {
        rct_window *w = window_get_main();
        if (w != NULL) {
            window_scroll_to_location(w, trackElement->x, trackElement->y, trackElement->element->base_height * 8);
            ride_modify(trackElement);
        }
    }
}

/**
 *
 *  rct2: 0x006B4F6B
 */
static rct_map_element *loc_6B4F6B(sint32 rideIndex, sint32 x, sint32 y)
{
    Ride * ride = get_ride(rideIndex);
    rct_map_element *mapElement = map_get_first_element_at(x / 32, y / 32);
    do {
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
            continue;

        if (RideProperties[ride->type].flags & RIDE_TYPE_FLAG_FLAT_RIDE) {
            if (!(FlatRideTrackSequenceProperties[mapElement->properties.track.type][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
                continue;
        } else {
            if (!(TrackSequenceProperties[mapElement->properties.track.type][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
                continue;
        }

        if (mapElement->properties.track.ride_index == rideIndex)
            return mapElement;
    } while (!map_element_is_last_for_tile(mapElement++));

    return NULL;
}

static sint32 ride_is_valid_for_test(sint32 rideIndex, sint32 goingToBeOpen, sint32 isApplying)
{
    sint32 stationIndex;
    Ride *ride;
    rct_xy_element trackElement, problematicTrackElement = { 0 };

    ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_NULL)
    {
        log_warning("Invalid ride type for ride %u", rideIndex);
        return 0;
    }

    window_close_by_number(WC_RIDE_CONSTRUCTION, rideIndex);

    stationIndex = ride_mode_check_station_present(ride);
    if (stationIndex == -1)return 0;

    if (!ride_mode_check_valid_station_numbers(ride))
        return 0;

    if (!ride_check_for_entrance_exit(rideIndex)) {
        loc_6B51C0(rideIndex);
        return 0;
    }

    if (goingToBeOpen && isApplying) {
        sub_6B5952(rideIndex);
        ride->lifecycle_flags |= RIDE_LIFECYCLE_EVER_BEEN_OPENED;
    }

    // z = ride->station_heights[i] * 8;
    trackElement.x = ride->station_starts[stationIndex].x * 32;
    trackElement.y = ride->station_starts[stationIndex].y * 32;
    trackElement.element = loc_6B4F6B(rideIndex, trackElement.x, trackElement.y);
    if (trackElement.element == NULL) {
        // Maze is strange, station start is 0... investigation required
        if (ride->type != RIDE_TYPE_MAZE)
            return 0;
    }

    if (
        ride->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER ||
        ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT ||
        ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED ||
        ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
        ) {
        if (ride_find_track_gap(&trackElement, &problematicTrackElement) && (!gConfigGeneral.test_unfinished_tracks ||
            ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)) {
            gGameCommandErrorText = STR_TRACK_IS_NOT_A_COMPLETE_CIRCUIT;
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }
    }

    if (
        ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED ||
        ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
        ) {
        if (!ride_check_block_brakes(&trackElement, &problematicTrackElement)) {
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }
    }

    if (ride->subtype != 255) {
        rct_ride_entry *rideType = get_ride_entry(ride->subtype);
        if (rideType->flags & RIDE_ENTRY_FLAG_NO_INVERSIONS) {
            gGameCommandErrorText = STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN;
            if (ride_check_track_contains_inversions(&trackElement, &problematicTrackElement)) {
                ride_scroll_to_track_error(&problematicTrackElement);
                return 0;
            }
        }
        if (rideType->flags & RIDE_ENTRY_FLAG_NO_BANKED_TRACK) {
            gGameCommandErrorText = STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN;
            if (ride_check_track_contains_banked(&trackElement, &problematicTrackElement)) {
                ride_scroll_to_track_error(&problematicTrackElement);
                return 0;
            }
        }
    }

    if (ride->mode == RIDE_MODE_STATION_TO_STATION) {
        if (!ride_find_track_gap(&trackElement, &problematicTrackElement)) {
            gGameCommandErrorText = STR_RIDE_MUST_START_AND_END_WITH_STATIONS;
            return 0;
        }

        gGameCommandErrorText = STR_STATION_NOT_LONG_ENOUGH;
        if (!ride_check_station_length(&trackElement, &problematicTrackElement)) {
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }

        gGameCommandErrorText = STR_RIDE_MUST_START_AND_END_WITH_STATIONS;
        if (!ride_check_start_and_end_is_station(&trackElement, &problematicTrackElement)) {
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }
    }

    if (isApplying)
        ride_set_start_finish_points(rideIndex, &trackElement);

    if (
        !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES) &&
        !(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    ) {
        if (!ride_create_vehicles(ride, rideIndex, &trackElement, isApplying)) {
            return 0;
        }
    }

    if (
        (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_CABLE_LIFT_HILL) &&
        (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED) &&
        !(ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT)
        ) {
        if (!ride_create_cable_lift(rideIndex, isApplying))
            return 0;
    }

    return 1;
}
/**
 *
 *  rct2: 0x006B4EEA
 */
static sint32 ride_is_valid_for_open(sint32 rideIndex, sint32 goingToBeOpen, sint32 isApplying)
{
    sint32 stationIndex;
    Ride *ride;
    rct_xy_element trackElement, problematicTrackElement = { 0 };

    ride = get_ride(rideIndex);

    // Check to see if construction tool is in use. If it is close the construction window
    // to set the track to its final state and clean up ghosts.
    // We can't just call close as it would cause a stack overflow during shop creation
    // with auto open on.
    if (WC_RIDE_CONSTRUCTION == gCurrentToolWidget.window_classification &&
        rideIndex == gCurrentToolWidget.window_number &&
        (input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
        window_close_by_number(WC_RIDE_CONSTRUCTION, rideIndex);

    stationIndex = ride_mode_check_station_present(ride);
    if (stationIndex == -1)return 0;

    if (!ride_mode_check_valid_station_numbers(ride))
        return 0;

    if (!ride_check_for_entrance_exit(rideIndex)) {
        loc_6B51C0(rideIndex);
        return 0;
    }

    if (goingToBeOpen && isApplying) {
        sub_6B5952(rideIndex);
        ride->lifecycle_flags |= RIDE_LIFECYCLE_EVER_BEEN_OPENED;
    }

    // z = ride->station_heights[i] * 8;
    trackElement.x = ride->station_starts[stationIndex].x * 32;
    trackElement.y = ride->station_starts[stationIndex].y * 32;
    trackElement.element = loc_6B4F6B(rideIndex, trackElement.x, trackElement.y);
    if (trackElement.element == NULL) {
        // Maze is strange, station start is 0... investigation required
        if (ride->type != RIDE_TYPE_MAZE)
            return 0;
    }

    if (
        ride->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER ||
        ride->mode == RIDE_MODE_RACE ||
        ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT ||
        ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED ||
        ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
    ) {
        if (ride_find_track_gap(&trackElement, &problematicTrackElement)) {
            gGameCommandErrorText = STR_TRACK_IS_NOT_A_COMPLETE_CIRCUIT;
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }
    }

    if (
        ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED ||
        ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
    ) {
        if (!ride_check_block_brakes(&trackElement, &problematicTrackElement)) {
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }
    }

    if (ride->subtype != 255) {
        rct_ride_entry *rideType = get_ride_entry(ride->subtype);
        if (rideType->flags & RIDE_ENTRY_FLAG_NO_INVERSIONS) {
            gGameCommandErrorText = STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN;
            if (ride_check_track_contains_inversions(&trackElement, &problematicTrackElement)) {
                ride_scroll_to_track_error(&problematicTrackElement);
                return 0;
            }
        }
        if (rideType->flags & RIDE_ENTRY_FLAG_NO_BANKED_TRACK) {
            gGameCommandErrorText = STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN;
            if (ride_check_track_contains_banked(&trackElement, &problematicTrackElement)) {
                ride_scroll_to_track_error(&problematicTrackElement);
                return 0;
            }
        }
    }

    if (ride->mode == RIDE_MODE_STATION_TO_STATION) {
        if (!ride_find_track_gap(&trackElement, &problematicTrackElement)) {
            gGameCommandErrorText = STR_RIDE_MUST_START_AND_END_WITH_STATIONS;
            return 0;
        }

        gGameCommandErrorText = STR_STATION_NOT_LONG_ENOUGH;
        if (!ride_check_station_length(&trackElement, &problematicTrackElement)) {
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }

        gGameCommandErrorText = STR_RIDE_MUST_START_AND_END_WITH_STATIONS;
        if (!ride_check_start_and_end_is_station(&trackElement, &problematicTrackElement)) {
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }
    }

    if (isApplying)
        ride_set_start_finish_points(rideIndex, &trackElement);

    if (
        !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES) &&
        !(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    ) {
        if (!ride_create_vehicles(ride, rideIndex, &trackElement, isApplying)) {
            return 0;
        }
    }

    if (
        (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_CABLE_LIFT_HILL) &&
        (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED) &&
        !(ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT)
    ) {
        if (!ride_create_cable_lift(rideIndex, isApplying))
            return 0;
    }

    return 1;
}

void ride_set_status(sint32 rideIndex, sint32 status)
{
    game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, rideIndex | (status << 8), GAME_COMMAND_SET_RIDE_STATUS, 0, 0);
}

/**
 *
 *  rct2: 0x006B4EA6
 */
void game_command_set_ride_status(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    sint32 rideIndex, targetStatus;
    Ride *ride;

    rideIndex = *edx & 0xFF;
    if (rideIndex >= MAX_RIDES)
    {
        log_warning("Invalid game command for ride %u", rideIndex);
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    targetStatus = (*edx >> 8) & 0xFF;

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS;

    ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_NULL)
    {
        log_warning("Invalid game command for ride %u", rideIndex);
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    if (*ebx & GAME_COMMAND_FLAG_APPLY) {
        if (ride->overall_view.xy != RCT_XY8_UNDEFINED) {
            rct_xyz16 coord;
            coord.x = ride->overall_view.x * 32 + 16;
            coord.y = ride->overall_view.y * 32 + 16;
            coord.z = map_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }
    }

    switch (targetStatus) {
    case RIDE_STATUS_CLOSED:
        if (*ebx & GAME_COMMAND_FLAG_APPLY) {
            if (ride->status == targetStatus) {
                if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)) {
                    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CRASHED;
                    ride_clear_for_construction(rideIndex);
                    ride_remove_peeps(rideIndex);
                }
            }

            ride->status = RIDE_STATUS_CLOSED;
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING;
            ride->race_winner = SPRITE_INDEX_NULL;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
            window_invalidate_by_number(WC_RIDE, rideIndex);
        }
        *ebx = 0;
        return;
    case RIDE_STATUS_TESTING:
    case RIDE_STATUS_OPEN:
        if (ride->status == targetStatus) {
            *ebx = 0;
            return;
        }

        if (*ebx & GAME_COMMAND_FLAG_APPLY) {
            // Fix #3183: Make sure we close the construction window so the ride finishes any editing code before opening
            //            otherwise vehicles get added to the ride incorrectly (such as to a ghost station)
            rct_window *constructionWindow = window_find_by_number(WC_RIDE_CONSTRUCTION, rideIndex);
            if (constructionWindow != NULL) {
                window_close(constructionWindow);
            }
        } else {
            // #5787: We need to make sure ghost elements are removed before validating
            //        the track.
            ride_construction_remove_ghosts();
        }

        if (targetStatus == RIDE_STATUS_TESTING) {
            if (!ride_is_valid_for_test(rideIndex, targetStatus == RIDE_STATUS_OPEN, *ebx & GAME_COMMAND_FLAG_APPLY)) {
                *ebx = MONEY32_UNDEFINED;
                return;
            }
        }
        else if (!ride_is_valid_for_open(rideIndex, targetStatus == RIDE_STATUS_OPEN, *ebx & GAME_COMMAND_FLAG_APPLY)) {
            *ebx = MONEY32_UNDEFINED;
            return;
        }

        if (*ebx & GAME_COMMAND_FLAG_APPLY) {
            ride->race_winner = SPRITE_INDEX_NULL;
            ride->status = targetStatus;
            ride_get_measurement(rideIndex, NULL);
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
            window_invalidate_by_number(WC_RIDE, rideIndex);
        }
        *ebx = 0;
        return;
    }
}

void ride_set_name(sint32 rideIndex, const char *name)
{
    typedef union name_union {
        char as_char[4];
        sint32 as_int;
    } name_union;

    name_union name_buffer[9];
    safe_strcpy((char *)name_buffer, name, sizeof(name_buffer));
    gGameCommandErrorTitle = STR_CANT_RENAME_RIDE_ATTRACTION;
    game_do_command(1, (rideIndex << 8) | 1, 0, name_buffer[0].as_int, GAME_COMMAND_SET_RIDE_NAME, name_buffer[2].as_int, name_buffer[1].as_int);
    game_do_command(2, (rideIndex << 8) | 1, 0, name_buffer[3].as_int, GAME_COMMAND_SET_RIDE_NAME, name_buffer[5].as_int, name_buffer[4].as_int);
    game_do_command(0, (rideIndex << 8) | 1, 0, name_buffer[6].as_int, GAME_COMMAND_SET_RIDE_NAME, name_buffer[8].as_int, name_buffer[7].as_int);
}

/**
 *
 *  rct2: 0x006B578B
 */
void game_command_set_ride_name(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    char oldName[128];
    static char newName[128];

    sint32 rideIndex = (*ebx >> 8) & 0xFF;
    if (rideIndex >= MAX_RIDES)
    {
        log_warning("Invalid game command for ride %u", rideIndex);
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    sint32 nameChunkIndex = *eax & 0xFFFF;

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS;
    //if (*ebx & GAME_COMMAND_FLAG_APPLY) { // this check seems to be useless and causes problems in multiplayer
        sint32 nameChunkOffset = nameChunkIndex - 1;
        if (nameChunkOffset < 0)
            nameChunkOffset = 2;
        nameChunkOffset *= 12;
        nameChunkOffset = min(nameChunkOffset, countof(newName) - 12);
        memcpy((void*)((uintptr_t)newName + (uintptr_t)nameChunkOffset + 0), edx, sizeof(uint32));
        memcpy((void*)((uintptr_t)newName + (uintptr_t)nameChunkOffset + 4), ebp, sizeof(uint32));
        memcpy((void*)((uintptr_t)newName + (uintptr_t)nameChunkOffset + 8), edi, sizeof(uint32));
    //}

    if (nameChunkIndex != 0) {
        *ebx = 0;
        return;
    }

    Ride *ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_NULL)
    {
        log_warning("Invalid game command for ride %u", rideIndex);
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    format_string(oldName, 128, ride->name, &ride->name_arguments);
    if (strcmp(oldName, newName) == 0) {
        *ebx = 0;
        return;
    }

    if (newName[0] == 0) {
        gGameCommandErrorText = STR_INVALID_RIDE_ATTRACTION_NAME;
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    rct_string_id newUserStringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER | USER_STRING_DUPLICATION_PERMITTED, newName);
    if (newUserStringId == 0) {
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    if (*ebx & GAME_COMMAND_FLAG_APPLY) {
        // Log ride rename command if we are in multiplayer and logging is enabled
        if ((network_get_mode() == NETWORK_MODE_CLIENT || network_get_mode() == NETWORK_MODE_SERVER) && gConfigNetwork.log_server_actions) {
            // Get player name
            int player_index = network_get_player_index(game_command_playerid);
            const char* player_name = network_get_player_name(player_index);

            char log_msg[256];
            char* args[3] = {
                (char *) player_name,
                oldName,
                newName
            };
            format_string(log_msg, 256, STR_LOG_RIDE_NAME, args);
            network_append_server_log(log_msg);
        }

        if (ride->overall_view.xy != RCT_XY8_UNDEFINED) {
            rct_xyz16 coord;
            coord.x = ride->overall_view.x * 32 + 16;
            coord.y = ride->overall_view.y * 32 + 16;
            coord.z = map_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        // Free the old ride name
        user_string_free(ride->name);

        ride->name = newUserStringId;

        gfx_invalidate_screen();

        // Force ride list window refresh
        rct_window *w = window_find_by_class(WC_RIDE_LIST);
        if (w != NULL)
            w->no_list_items = 0;
    } else {
        user_string_free(newUserStringId);
    }

    *ebx = 0;
}

/**
 *
 *  rct2: 0x006CB7FB
 */
sint32 ride_get_refund_price(sint32 ride_id)
{
    uint8 oldpaused = gGamePaused;
    gGamePaused = 0;
    money32 refundPrice = 0;

    map_element_iterator it;

    map_element_iterator_begin(&it);
    while (map_element_iterator_next(&it)) {
        if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_TRACK)
            continue;

        if (it.element->properties.track.ride_index != ride_id)
            continue;

        sint32 x = it.x * 32, y = it.y * 32;
        sint32 z = it.element->base_height * 8;

        uint8 rotation = map_element_get_direction(it.element);
        uint8 type = it.element->properties.track.type;

        if (type != TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP){
            money32 removePrice = game_do_command(
                x,
                GAME_COMMAND_FLAG_APPLY | (rotation << 8),
                y,
                type | (map_element_get_track_sequence(it.element) << 8),
                GAME_COMMAND_REMOVE_TRACK,
                z,
                0);
            if (removePrice == MONEY32_UNDEFINED) {
                map_element_remove(it.element);
            } else {
                refundPrice += removePrice;
            }
            map_element_iterator_restart_for_tile(&it);
            continue;
        }

        // Using GAME_COMMAND_FLAG_2 for below commands as a HACK to stop fences from being removed
        refundPrice += game_do_command(
            x,
            GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_2 | (0 << 8),
            y,
            ride_id | (2 << 8),
            GAME_COMMAND_SET_MAZE_TRACK,
            z,
            0);

        refundPrice += game_do_command(
            x,
            GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_2 | (1 << 8),
            y + 16,
            ride_id | (2 << 8),
            GAME_COMMAND_SET_MAZE_TRACK,
            z,
            0);

        refundPrice += game_do_command(
            x + 16,
            GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_2 | (2 << 8),
            y + 16,
            ride_id | (2 << 8),
            GAME_COMMAND_SET_MAZE_TRACK,
            z,
            0);

        refundPrice += game_do_command(
            x + 16,
            GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_2 | (3 << 8),
            y,
            ride_id | (2 << 8),
            GAME_COMMAND_SET_MAZE_TRACK,
            z,
            0);
        map_element_iterator_restart_for_tile(&it);
    }
    gGamePaused = oldpaused;
    return refundPrice;
}

/**
 *
 *  rct2: 0x00696707
 */
static void ride_stop_peeps_queuing(sint32 rideIndex)
{
    uint16 spriteIndex;
    rct_peep *peep;

    FOR_ALL_PEEPS(spriteIndex, peep) {
        if (peep->state != PEEP_STATE_QUEUING)
            continue;
        if (peep->current_ride != rideIndex)
            continue;

        remove_peep_from_queue(peep);
        peep_decrement_num_riders(peep);

        peep->state = PEEP_STATE_FALLING;

        peep_window_state_update(peep);
    }
}

static sint32 ride_get_empty_slot()
{
    for (sint32 i = 0; i < MAX_RIDES; i++) {
        Ride *ride = get_ride(i);
        if (ride->type == RIDE_TYPE_NULL) {
            return i;
        }
    }
    return -1;
}

static sint32 ride_get_default_mode(Ride *ride)
{
    const rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
    const uint8 *availableModes = RideAvailableModes;

    for (sint32 i = 0; i < ride->type; i++) {
        while (*(availableModes++) != 255) {}
    }
    // Since this only selects a default mode and does not prevent other modes from being used, there is no need
    // to check if select-by-track-type or the all-ride-modes cheat have been enabled.
    if (rideEntry->flags & RIDE_ENTRY_DISABLE_FIRST_TWO_OPERATING_MODES)
    {
        availableModes += 2;
    }
    return availableModes[0];
}

static bool ride_with_colour_config_exists(uint8 ride_type, const track_colour *colours)
{
    Ride *ride;
    sint32 i;

    FOR_ALL_RIDES(i, ride) {
        if (ride->type != ride_type) continue;
        if (ride->track_colour_main[0] != colours->main) continue;
        if (ride->track_colour_additional[0] != colours->additional) continue;
        if (ride->track_colour_supports[0] != colours->supports) continue;

        return true;
    }
    return false;
}

static bool ride_name_exists(char *name)
{
    char buffer[256];
    Ride *ride;
    sint32 i;
    FOR_ALL_RIDES(i, ride) {
        format_string(buffer, 256, ride->name, &ride->name_arguments);
        if ((strcmp(buffer, name) == 0) && ride_has_any_track_elements(i)) {
            return true;
        }
    }
    return false;
}

/**
 *
 *  Based on rct2: 0x006B4776
 */
static sint32 ride_get_random_colour_preset_index(uint8 ride_type)
{
    if (ride_type >= 128)
    {
        return 0;
    }

    const track_colour_preset_list *colourPresets = &RideColourPresets[ride_type];

    // 200 attempts to find a colour preset that hasn't already been used in the park for this ride type
    for (sint32 i = 0; i < 200; i++) {
        sint32 listIndex = util_rand() % colourPresets->count;
        const track_colour *colours = &colourPresets->list[listIndex];

        if (!ride_with_colour_config_exists(ride_type, colours)) {
            return listIndex;
        }
    }
    return 0;
}

/**
 *
 *  Based on rct2: 0x006B4776
 */
static void ride_set_colour_preset(Ride *ride, uint8 index)
{
    const track_colour_preset_list * colourPresets = &RideColourPresets[ride->type];
    track_colour colours = { COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK };
    if (index < colourPresets->count) {
        colours = colourPresets->list[index];
    }
    for (sint32 i = 0; i < NUM_COLOUR_SCHEMES; i++) {
        ride->track_colour_main[i] = colours.main;
        ride->track_colour_additional[i] = colours.additional;
        ride->track_colour_supports[i] = colours.supports;
    }
    ride->colour_scheme_type = 0;
}

static money32 ride_get_common_price(Ride *forRide)
{
    Ride *ride;
    sint32 i;

    FOR_ALL_RIDES(i, ride) {
        if (ride->type == forRide->type && ride != forRide) {
            return ride->price;
        }
    }

    return MONEY32_UNDEFINED;
}

static money32 shop_item_get_common_price(Ride *forRide, sint32 shopItem)
{
    rct_ride_entry *rideEntry;
    Ride *ride;
    sint32 i;

    FOR_ALL_RIDES(i, ride) {
        if (ride != forRide) {
            rideEntry = get_ride_entry(ride->subtype);
            if (rideEntry == NULL) {
                continue;
            }
            if (rideEntry->shop_item == shopItem) {
                return ride->price;
            }
            if (rideEntry->shop_item_secondary == shopItem) {
                return ride->price_secondary;
            }
            if (shop_item_is_photo(shopItem) && (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)) {
                return ride->price_secondary;
            }
        }
    }

    return MONEY32_UNDEFINED;
}

bool shop_item_is_photo(sint32 shopItem)
{
    return (
        shopItem == SHOP_ITEM_PHOTO || shopItem == SHOP_ITEM_PHOTO2 ||
        shopItem == SHOP_ITEM_PHOTO3 || shopItem == SHOP_ITEM_PHOTO4
    );
}

bool shop_item_has_common_price(sint32 shopItem)
{
    if (shopItem < 32) {
        return gSamePriceThroughoutParkA & (1u << shopItem);
    } else {
        return gSamePriceThroughoutParkB & (1u << (shopItem - 32));
    }
}

money32 ride_create_command(sint32 type, sint32 subType, sint32 flags, uint8 *outRideIndex, uint8 *outRideColour)
{
    sint32 eax = 0;
    sint32 ebx = flags;
    sint32 ecx = 0;
    sint32 edx = type | (subType << 8);
    sint32 esi = 0;
    sint32 edi = 0;
    sint32 ebp = 0;
    money32 cost = game_do_command_p(GAME_COMMAND_CREATE_RIDE, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
    *outRideIndex = edi & 0xFF;
    *outRideColour = eax;
    return cost;
}

/**
 *
 *  rct2: 0x006B3F0F
 */
static money32 ride_create(sint32 type, sint32 subType, sint32 flags, sint32 *outRideIndex, sint32 *outRideColour)
{
    Ride *ride;
    rct_ride_entry *rideEntry;
    sint32 rideIndex, rideEntryIndex;

    if (type >= RIDE_TYPE_COUNT)
    {
        log_warning("Invalid request for ride type %u", type);
        return MONEY32_UNDEFINED;
    }

    if (subType == RIDE_ENTRY_INDEX_NULL) {
        uint8 *availableRideEntries = get_ride_entry_indices_for_ride_type(type);
        for (uint8 *rei = availableRideEntries; *rei != RIDE_ENTRY_INDEX_NULL; rei++) {
            rideEntry = get_ride_entry(*rei);

            // Can happen in select-by-track-type mode
            if (!ride_entry_is_invented(*rei) && !gCheatsIgnoreResearchStatus)
            {
                continue;
            }

            if (!(rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME) || rideTypeShouldLoseSeparateFlag(rideEntry)) {
                subType = *rei;
                goto foundRideEntry;
            }
        }
        subType = availableRideEntries[0];
        if (subType == RIDE_ENTRY_INDEX_NULL) {
            return MONEY32_UNDEFINED;
        }
    }

foundRideEntry:
    rideEntryIndex = subType;
    rideIndex = ride_get_empty_slot();
    if (subType >= 128)
    {
        log_warning("Invalid request for ride entry %u", subType);
        return MONEY32_UNDEFINED;
    }
    if (rideIndex == -1) {
        gGameCommandErrorText = STR_TOO_MANY_RIDES;
        return MONEY32_UNDEFINED;
    }
    *outRideIndex = rideIndex;

    // Ride/vehicle colour is calculated before applying to ensure
    // correct colour is passed over the network.
    if (!(flags & GAME_COMMAND_FLAG_APPLY) && !(flags & GAME_COMMAND_FLAG_NETWORKED)) {
        *outRideColour =
            ride_get_random_colour_preset_index(type) |
            (ride_get_unused_preset_vehicle_colour(type, subType) << 8);
    }

    if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
        gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
        gCommandPosition.x = 0x8000;
        return 0;
    }

    ride = get_ride(rideIndex);
    rideEntry = get_ride_entry(rideEntryIndex);
    if (rideEntry == (rct_ride_entry *)-1)
    {
        log_warning("Invalid request for ride %u", rideIndex);
        return MONEY32_UNDEFINED;
    }
    ride->type = type;
    ride->subtype = rideEntryIndex;
    ride_set_colour_preset(ride, *outRideColour & 0xFF);
    ride->overall_view.xy = RCT_XY8_UNDEFINED;

    // Ride name
    if (rideEntryIndex == RIDE_ENTRY_INDEX_NULL)
    {
        ride_set_name_to_track_default(ride, rideEntry);
    }
    else
    {
        ride_set_name_to_default(ride, rideEntry);
    }

    for (size_t i = 0; i < MAX_STATIONS; i++) {
        ride->station_starts[i].xy = RCT_XY8_UNDEFINED;
        ride->entrances[i].xy = RCT_XY8_UNDEFINED;
        ride->exits[i].xy = RCT_XY8_UNDEFINED;
        ride->train_at_station[i] = 255;
        ride->queue_time[i] = 0;
    }

    for (size_t i = 0; i < MAX_VEHICLES_PER_RIDE; i++) {
        ride->vehicles[i] = SPRITE_INDEX_NULL;
    }

    ride->status = RIDE_STATUS_CLOSED;
    ride->lifecycle_flags = 0;
    ride->vehicle_change_timeout = 0;
    ride->num_stations = 0;
    ride->num_vehicles = 1;
    ride->proposed_num_vehicles = 32;
    ride->max_trains = 32;
    ride->num_cars_per_train = 1;
    ride->proposed_num_cars_per_train = 12;
    ride->min_waiting_time = 10;
    ride->max_waiting_time = 60;
    ride->depart_flags = RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH | WAIT_FOR_LOAD_FULL;
    if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_MUSIC_ON_DEFAULT) {
        ride->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
    }
    ride->music = RideData4[ride->type].default_music;

    const rct_ride_properties rideProperties = RideProperties[ride->type];
    ride->operation_option = (rideProperties.min_value * 3 + rideProperties.max_value) / 4;

    ride->lift_hill_speed = RideLiftData[ride->type].minimum_speed;

    ride->measurement_index = 255;
    ride->excitement = (ride_rating)-1;
    ride->cur_num_customers = 0;
    ride->num_customers_timeout = 0;
    ride->chairlift_bullwheel_rotation = 0;

    ride->price = 0;
    ride->price_secondary = 0;
    if (!(gParkFlags & PARK_FLAGS_NO_MONEY)) {
        ride->price = RideData4[ride->type].price;
        ride->price_secondary = RideData4[ride->type].price_secondary;

        if (rideEntry->shop_item == SHOP_ITEM_NONE) {
            if (!(gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) && !gCheatsUnlockAllPrices) {
                ride->price = 0;
            }
        } else {
            ride->price = DefaultShopItemPrice[rideEntry->shop_item];
        }
        if (rideEntry->shop_item_secondary != SHOP_ITEM_NONE) {
            ride->price_secondary = DefaultShopItemPrice[rideEntry->shop_item_secondary];
        }

        if (gScenarioObjectiveType == OBJECTIVE_BUILD_THE_BEST) {
            ride->price = 0;
        }

        if (ride->type == RIDE_TYPE_TOILETS) {
            if (shop_item_has_common_price(SHOP_ITEM_ADMISSION)) {
                money32 price = ride_get_common_price(ride);
                if (price != MONEY32_UNDEFINED) {
                    ride->price = (money16)price;
                }
            }
        }

        if (rideEntry->shop_item != SHOP_ITEM_NONE) {
            if (shop_item_has_common_price(rideEntry->shop_item)) {
                money32 price = shop_item_get_common_price(ride, rideEntry->shop_item);
                if (price != MONEY32_UNDEFINED) {
                    ride->price = (money16)price;
                }
            }
        }

        if (rideEntry->shop_item_secondary != SHOP_ITEM_NONE) {
            if (shop_item_has_common_price(rideEntry->shop_item_secondary)) {
                money32 price = shop_item_get_common_price(ride, rideEntry->shop_item_secondary);
                if (price != MONEY32_UNDEFINED) {
                    ride->price_secondary = (money16)price;
                }
            }
        }

        // Set the on-ride photo price, whether the ride has one or not (except shops).
        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP) && shop_item_has_common_price(SHOP_ITEM_PHOTO)) {
            money32 price = shop_item_get_common_price(ride, SHOP_ITEM_PHOTO);
            if (price != MONEY32_UNDEFINED) {
                ride->price_secondary = (money16)price;
            }
        }
    }

    memset(ride->num_customers, 0, sizeof(ride->num_customers));
    ride->value = 0xFFFF;
    ride->satisfaction = 255;
    ride->satisfaction_time_out = 0;
    ride->satisfaction_next = 0;
    ride->popularity = 255;
    ride->popularity_time_out = 0;
    ride->popularity_next = 0;
    ride->window_invalidate_flags = 0;
    ride->total_customers = 0;
    ride->total_profit = 0;
    ride->num_riders = 0;
    ride->slide_in_use = 0;
    ride->maze_tiles = 0;
    ride->build_date = gDateMonthsElapsed;
    ride->music_tune_id = 255;

    ride->breakdown_reason = 255;
    ride->upkeep_cost = (money16)-1;
    ride->reliability = RIDE_INITIAL_RELIABILITY;
    ride->unreliability_factor = 1;
    ride->inspection_interval = RIDE_INSPECTION_EVERY_30_MINUTES;
    ride->last_inspection = 0;
    ride->downtime = 0;
    memset(ride->downtime_history, 0, sizeof(ride->downtime_history));
    ride->no_primary_items_sold = 0;
    ride->no_secondary_items_sold = 0;
    ride->last_crash_type = RIDE_CRASH_TYPE_NONE;
    ride->income_per_hour = MONEY32_UNDEFINED;
    ride->profit = MONEY32_UNDEFINED;
    ride->connected_message_throttle = 0;
    ride->entrance_style = RIDE_ENTRANCE_STYLE_PLAIN;
    ride->num_block_brakes = 0;
    ride->guests_favourite = 0;

    ride->num_circuits = 1;
    ride->mode = ride_get_default_mode(ride);
    ride->min_max_cars_per_train = (rideEntry->min_cars_in_train << 4) | rideEntry->max_cars_in_train;
    ride_set_vehicle_colours_to_random_preset(ride, 0xFF & (*outRideColour >> 8));
    window_invalidate_by_class(WC_RIDE_LIST);

    // Log ride creation
    if (network_get_mode() == NETWORK_MODE_SERVER) {
        int ebp = 1;
        game_log_multiplayer_command(GAME_COMMAND_CREATE_RIDE, 0, 0, 0, &rideIndex, 0, &ebp);
    }

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
    gCommandPosition.x = 0x8000;
    return 0;
}

void ride_set_name_to_default(Ride * ride, rct_ride_entry * rideEntry)
{
    if (!(rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME) || rideTypeShouldLoseSeparateFlag(rideEntry)) {
        ride_set_name_to_track_default(ride, rideEntry);
    } else {
        ride_set_name_to_vehicle_default(ride, rideEntry);
    }
}

static void ride_set_name_to_track_default(Ride *ride, rct_ride_entry * rideEntry)
{
    char rideNameBuffer[256];
    ride_name_args name_args;

    ride->name = STR_NONE;

    // This fixes the Hyper-Twister being displayed as the generic Steel Twister when not in select-by-track-type mode.
    if (!gConfigInterface.select_by_track_type && ride->type == RIDE_TYPE_TWISTER_ROLLER_COASTER && !(rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE)) {
        name_args.type_name = STR_HYPER_TWISTER_GROUP;
    }
    else if (ride_type_has_ride_groups(ride->type)) {
        const ride_group * rideGroup = get_ride_group(ride->type, rideEntry);
        name_args.type_name = rideGroup->naming.name;
    } else {
        name_args.type_name = RideNaming[ride->type].name;
    }

    name_args.number = 0;
    do {
        name_args.number++;
        format_string(rideNameBuffer, 256, 1, &name_args);
    } while (ride_name_exists(rideNameBuffer));
    ride->name = 1;
    ride->name_arguments_type_name = name_args.type_name;
    ride->name_arguments_number = name_args.number;
}

static void ride_set_name_to_vehicle_default(Ride * ride, rct_ride_entry * rideEntry)
{
    char rideNameBuffer[256];
    ride_name_args name_args;

    ride->name = 1;
    ride->name_arguments_type_name = rideEntry->naming.name;
    rct_string_id rideNameStringId = 0;
    name_args.type_name = rideEntry->naming.name;
    name_args.number = 0;

    do {
        name_args.number++;
        format_string(rideNameBuffer, 256, ride->name, &name_args);
    } while (ride_name_exists(rideNameBuffer));

    ride->name_arguments_type_name = name_args.type_name;
    ride->name_arguments_number = name_args.number;

    rideNameStringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER | USER_STRING_DUPLICATION_PERMITTED, rideNameBuffer);
    if (rideNameStringId != 0) {
        ride->name = rideNameStringId;
    } else {
        ride_set_name_to_track_default(ride, rideEntry);
    }
}

/**
 * This will return the name of the ride, as seen in the New Ride window.
 */
rct_ride_name get_ride_naming(uint8 rideType, rct_ride_entry * rideEntry)
{
    // This fixes the Hyper-Twister being displayed as the generic Steel Twister when not in select-by-track-type mode.
    if (!gConfigInterface.select_by_track_type && rideType == RIDE_TYPE_TWISTER_ROLLER_COASTER && !(rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE)) {
        return (rct_ride_name){ STR_HYPER_TWISTER_GROUP, STR_HYPER_TWISTER_GROUP_DESC };
    }

    if (ride_type_has_ride_groups(rideType)) {
        const ride_group * rideGroup = get_ride_group(rideType, rideEntry);
        return rideGroup->naming;
    }
    else if (!(rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME) || rideTypeShouldLoseSeparateFlag(rideEntry)) {
        return RideNaming[rideType];
    }
    else {
        return rideEntry->naming;
    }
}

/**
 *
 *  rct2: 0x006B3F0F
 */
void game_command_create_ride(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    *ebx = ride_create(*edx & 0xFF, (*edx >> 8) & 0xFF, *ebx, edi, eax);
}

void game_command_callback_ride_construct_new(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp)
{
    sint32 rideIndex = edi;
    if (rideIndex != -1)
        ride_construct(rideIndex);
}

/**
 *
 * Network client callback when placing ride pieces
 *   Client does execute placing the piece on the same tick as mouse_up - waits for server command
 * Re-executes function from ride_construction - window_ride_construction_construct()
 * Only uses part that deals with construction state
 */

void game_command_callback_ride_construct_placed_back(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp)
{
    sint32 trackDirection, x, y, z;
    track_begin_end trackBeginEnd;

    trackDirection = _currentTrackPieceDirection ^ 2;
    x = _currentTrackBeginX;
    y = _currentTrackBeginY;
    z = _currentTrackBeginZ;
    if (!(trackDirection & 4)) {
        x += TileDirectionDelta[trackDirection].x;
        y += TileDirectionDelta[trackDirection].y;
    }

    if (track_block_get_previous_from_zero(x, y, z, _currentRideIndex, trackDirection, &trackBeginEnd)) {
        _currentTrackBeginX = trackBeginEnd.begin_x;
        _currentTrackBeginY = trackBeginEnd.begin_y;
        _currentTrackBeginZ = trackBeginEnd.begin_z;
        _currentTrackPieceDirection = trackBeginEnd.begin_direction;
        _currentTrackPieceType = trackBeginEnd.begin_element->properties.track.type;
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
        ride_select_previous_section();
    }
    else {
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
    }

    window_ride_construction_do_station_check();
    window_ride_construction_update_active_elements();
}

void game_command_callback_ride_construct_placed_front(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp)
{
    sint32 trackDirection, x, y, z;

    trackDirection = _currentTrackPieceDirection;
    x = _currentTrackBeginX;
    y = _currentTrackBeginY;
    z = _currentTrackBeginZ;
    if (!(trackDirection & 4)) {
        x -= TileDirectionDelta[trackDirection].x;
        y -= TileDirectionDelta[trackDirection].y;
    }

    rct_xy_element next_track;
    if (track_block_get_next_from_zero(x, y, z, _currentRideIndex, trackDirection, &next_track, &z, &trackDirection)) {
        _currentTrackBeginX = next_track.x;
        _currentTrackBeginY = next_track.y;
        _currentTrackBeginZ = z;
        _currentTrackPieceDirection = map_element_get_direction(next_track.element);
        _currentTrackPieceType = next_track.element->properties.track.type;
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
        ride_select_next_section();
    }
    else {
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
    }

    window_ride_construction_do_station_check();
    window_ride_construction_do_entrance_exit_check();
    window_ride_construction_update_active_elements();
}

/**
*
* Network client callback when removing ride pieces
*   Client does execute placing the piece on the same tick as mouse_up - waits for server command
* Re-executes function from ride_construction - window_ride_construction_mouseup_demolish()
* Only uses part that deals with construction state
*/

void game_command_callback_ride_remove_track_piece(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp)
{
    sint32 x, y, z, direction, type;

    x = gRideRemoveTrackPieceCallbackX;
    y = gRideRemoveTrackPieceCallbackY;
    z = gRideRemoveTrackPieceCallbackZ;
    direction = gRideRemoveTrackPieceCallbackDirection;
    type = gRideRemoveTrackPieceCallbackType;

    window_ride_construction_mouseup_demolish_next_piece(x, y, z, direction, type);
}

/**
 *
 *  rct2: 0x006B49D9
 */
void game_command_demolish_ride(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    uint8 ride_id = *(uint8*)edx;
    if (ride_id >= MAX_RIDES)
    {
        log_warning("Invalid game command for ride %u", ride_id);
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    gCommandPosition.x = 0;
    gCommandPosition.y = 0;
    gCommandPosition.z = 0;
    Ride *ride = get_ride(ride_id);
    if (ride->type == RIDE_TYPE_NULL)
    {
        log_warning("Invalid game command for ride %u", ride_id);
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    sint32 x = 0, y = 0, z = 0;
    if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
    {
        x = (ride->overall_view.x * 32) + 16;
        y = (ride->overall_view.y * 32) + 16;
        z = map_element_height(x, y);
        gCommandPosition.x = x;
        gCommandPosition.y = y;
        gCommandPosition.z = z;
    }
    if (!(*ebx & 0x40) && game_is_paused() && !gCheatsBuildInPauseMode)
    {
        gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    else if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
    {
        gGameCommandErrorText = STR_LOCAL_AUTHORITY_FORBIDS_DEMOLITION_OR_MODIFICATIONS_TO_THIS_RIDE;
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    else
    {
        if (*ebx & GAME_COMMAND_FLAG_APPLY)
        {
            if (ride->overall_view.xy != RCT_XY8_UNDEFINED) {
                rct_xyz16 coord;
                coord.x = ride->overall_view.x * 32 + 16;
                coord.y = ride->overall_view.y * 32 + 16;
                coord.z = map_element_height(coord.x, coord.y);
                network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
            }

            if (!(*ebx & 8)) {
                window_close_by_number(WC_RIDE_CONSTRUCTION, ride_id);
            }
            window_close_by_number(WC_RIDE, ride_id);
            window_close_by_number(WC_DEMOLISH_RIDE_PROMPT, ride_id);
            window_close_by_class(WC_NEW_CAMPAIGN);
            ride_clear_for_construction(ride_id);
            ride_remove_peeps(ride_id);
            ride_stop_peeps_queuing(ride_id);
            *ebx = ride_get_refund_price(ride_id);

            sub_6CB945(ride_id);
            news_item_disable_news(NEWS_ITEM_RIDE, ride_id);

            for (sint32 i = 0; i < MAX_BANNERS; i++) {
                rct_banner *banner = &gBanners[i];
                if (banner->type != BANNER_NULL && banner->flags & BANNER_FLAG_LINKED_TO_RIDE && banner->colour == ride_id) {
                    banner->flags &= 0xFB;
                    banner->string_idx = STR_DEFAULT_SIGN;
                }
            }

            uint16 spriteIndex;
            rct_peep *peep;
            FOR_ALL_GUESTS(spriteIndex, peep)
            {
                uint8 ride_id_bit = ride_id % 8;
                uint8 ride_id_offset = ride_id / 8;
                peep->rides_been_on[ride_id_offset] &= ~(1 << ride_id_bit); // clear ride from potentially being in rides_been_on
                if (peep->state == PEEP_STATE_WATCHING) {
                    if (peep->current_ride == ride_id) {
                        peep->current_ride = MAX_RIDES;
                        if (peep->time_to_stand >= 50) { // make peep stop watching the ride
                            peep->time_to_stand = 50;
                        }
                    }
                }
                // remove any free voucher for this ride from peep
                if (peep->item_standard_flags & PEEP_ITEM_VOUCHER) {
                    if (peep->voucher_type == VOUCHER_TYPE_RIDE_FREE && peep->voucher_arguments == ride_id) {
                        peep->item_standard_flags &= ~(PEEP_ITEM_VOUCHER);
                    }
                }
                // remove any photos of this ride from peep
                if (peep->item_standard_flags & PEEP_ITEM_PHOTO) {
                    if (peep->photo1_ride_ref == ride_id) {
                        peep->item_standard_flags &= ~PEEP_ITEM_PHOTO;
                    }
                }
                if (peep->item_extra_flags & PEEP_ITEM_PHOTO2) {
                    if (peep->photo2_ride_ref == ride_id) {
                        peep->item_extra_flags &= ~PEEP_ITEM_PHOTO2;
                    }
                }
                if (peep->item_extra_flags & PEEP_ITEM_PHOTO3) {
                    if (peep->photo3_ride_ref == ride_id) {
                        peep->item_extra_flags &= ~PEEP_ITEM_PHOTO3;
                    }
                }
                if (peep->item_extra_flags & PEEP_ITEM_PHOTO4) {
                    if (peep->photo4_ride_ref == ride_id) {
                        peep->item_extra_flags &= ~PEEP_ITEM_PHOTO4;
                    }
                }
                if (peep->guest_heading_to_ride_id == ride_id) {
                    peep->guest_heading_to_ride_id = MAX_RIDES;
                }
                if (peep->favourite_ride == ride_id) {
                    peep->favourite_ride = MAX_RIDES;
                }
                for (sint32 i = 0; i < PEEP_MAX_THOUGHTS; i++) {
                    if (peep->thoughts[i].item == ride_id) {
                        // Clear top thought, push others up
                        memmove(&peep->thoughts[i], &peep->thoughts[i + 1], sizeof(rct_peep_thought)*(PEEP_MAX_THOUGHTS - i - 1));
                        peep->thoughts[PEEP_MAX_THOUGHTS - 1].type = PEEP_THOUGHT_TYPE_NONE;
                    }
                }
            }

            user_string_free(ride->name);
            ride->type = RIDE_TYPE_NULL;
            window_invalidate_by_class(WC_RIDE_LIST);
            gParkValue = calculate_park_value();
            gCommandPosition.x = x;
            gCommandPosition.y = y;
            gCommandPosition.z = z;
            gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
            return;
        }
        else
        {
            *ebx = 0;
            gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
            return;
        }
    }
}

/**
 *
 *  rct2: 0x006B2FC5
 */
void game_command_set_ride_appearance(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    bool apply = (*ebx & GAME_COMMAND_FLAG_APPLY);

    uint8 ride_id = *edx;
    if (ride_id >= MAX_RIDES)
    {
        log_warning("Invalid game command for ride %u", ride_id);
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    uint8 type = *ebx >> 8;
    uint8 value = *edx >> 8;
    sint32 index = *edi;

    if (index < 0) {
        log_warning("Invalid game command, index %d out of bounds", index);
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    Ride *ride = get_ride(ride_id);
    if (ride->type == RIDE_TYPE_NULL) {
        log_warning("Invalid game command, ride_id = %u", ride_id);
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    if (apply && gGameCommandNestLevel == 1) {
        if (ride->overall_view.xy != RCT_XY8_UNDEFINED) {
            rct_xyz16 coord;
            coord.x = ride->overall_view.x * 32 + 16;
            coord.y = ride->overall_view.y * 32 + 16;
            coord.z = map_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }
    }

    *ebx = 0;
    switch(type) {
    case 0:
        if (index >= countof(ride->track_colour_main)) {
            log_warning("Invalid game command, index %d out of bounds", index);
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        if (apply) {
            ride->track_colour_main[index] = value;
            gfx_invalidate_screen();
        }
        break;
    case 1:
        if (index >= countof(ride->track_colour_additional)) {
            log_warning("Invalid game command, index %d out of bounds", index);
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        if (apply) {
            ride->track_colour_additional[index] = value;
            gfx_invalidate_screen();
        }
        break;
    case 2:
        if (index >= countof(ride->vehicle_colours)) {
            log_warning("Invalid game command, index %d out of bounds", index);
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        if (apply) {
            *((uint8*)(&ride->vehicle_colours[index])) = value;
            ride_update_vehicle_colours(ride_id);
        }
        break;
    case 3:
        if (index >= countof(ride->vehicle_colours)) {
            log_warning("Invalid game command, index %d out of bounds", index);
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        if (apply) {
            *((uint8*)(&ride->vehicle_colours[index]) + 1) = value;
            ride_update_vehicle_colours(ride_id);
        }
        break;
    case 4:
        if (index >= countof(ride->track_colour_supports)) {
            log_warning("Invalid game command, index %d out of bounds", index);
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        if (apply) {
            ride->track_colour_supports[index] = value;
            gfx_invalidate_screen();
        }
        break;
    case 5:
        if (apply) {
            ride->colour_scheme_type &= ~(RIDE_COLOUR_SCHEME_DIFFERENT_PER_TRAIN | RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR);
            ride->colour_scheme_type |= value;
            for (sint32 i = 1; i < countof(ride->vehicle_colours); i++) {
                ride->vehicle_colours[i] = ride->vehicle_colours[0];
                ride->vehicle_colours_extended[i] = ride->vehicle_colours_extended[0];
            }
            ride_update_vehicle_colours(ride_id);
        }
        break;
    case 6:
        if (apply) {
            ride->entrance_style = value;
            gLastEntranceStyle = value;
            gfx_invalidate_screen();
        }
        break;
    case 7:
        if (index >= countof(ride->vehicle_colours_extended)) {
            log_warning("Invalid game command, index %d out of bounds", index);
            *ebx = MONEY32_UNDEFINED;
            return;
        } else {
            if (apply) {
                ride->vehicle_colours_extended[index] = value;
                ride_update_vehicle_colours(ride_id);
            }
        }
        break;
    }
    window_invalidate_by_number(WC_RIDE, ride_id);
}

/**
 *
 *  rct2: 0x006B53E9
 */
void game_command_set_ride_price(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    uint32 flags = *ebx;
    uint8 ride_number = (*edx & 0xFF);
    if (ride_number >= MAX_RIDES)
    {
        log_warning("Invalid game command for ride %u", ride_number);
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    Ride *ride = get_ride(ride_number);
    if (ride->type == RIDE_TYPE_NULL)
    {
        log_warning("Invalid game command for ride %u", ride_number);
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
    money16 price = *edi;
    bool secondary_price = (*edx >> 8);

    if (rideEntry == (rct_ride_entry *)-1)
    {
        log_warning("Invalid game command for ride %u", ride_number);
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    //eax
    //ebx flags
    //ecx ecx
    //edx ride_number
    //ebp rideEntry

    *ebx = 0; // for cost check - changing ride price does not cost anything

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS;
    if (flags & GAME_COMMAND_FLAG_APPLY) {
        uint32 shop_item;

        if (ride->overall_view.xy != RCT_XY8_UNDEFINED) {
            rct_xyz16 coord;
            coord.x = ride->overall_view.x * 32 + 16;
            coord.y = ride->overall_view.y * 32 + 16;
            coord.z = map_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        if (!secondary_price) {
            shop_item = SHOP_ITEM_ADMISSION;
            if (ride->type != RIDE_TYPE_TOILETS) {
                shop_item = rideEntry->shop_item;
                if (shop_item == SHOP_ITEM_NONE) {
                    ride->price = price;
                    window_invalidate_by_class(WC_RIDE);
                    return;
                }
            }
            // Check same price in park flags
            if (!shop_item_has_common_price(shop_item)) {
                ride->price = price;
                window_invalidate_by_class(WC_RIDE);
                return;
            }
        }
        else {
            shop_item = rideEntry->shop_item_secondary;
            if (shop_item == SHOP_ITEM_NONE) {
                shop_item = RidePhotoItems[ride->type];
                if ((ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) == 0) {
                    ride->price_secondary = price;
                    window_invalidate_by_class(WC_RIDE);
                    return;
                }
            }
            // Check same price in park flags
            if (!shop_item_has_common_price(shop_item)) {
                ride->price_secondary = price;
                window_invalidate_by_class(WC_RIDE);
                return;
            }
        }
        ride = get_ride(0);

        for (uint8 rideId = 0; rideId < 0xFF; rideId++, ride++) {
            // Unplaced rides have a type of NULL
            if (ride->type == RIDE_TYPE_NULL)
                continue;

            rideEntry = get_ride_entry(ride->subtype);

            if (ride->type != RIDE_TYPE_TOILETS || shop_item != SHOP_ITEM_ADMISSION) {
                if (rideEntry->shop_item == shop_item) {
                    ride->price = price;
                    window_invalidate_by_number(WC_RIDE, rideId);
                }
            }
            else {
                ride->price = price;
                window_invalidate_by_number(WC_RIDE, rideId);
            }
            // If the shop item is the same or an on-ride photo
            if (rideEntry->shop_item_secondary == shop_item ||
                (rideEntry->shop_item_secondary == SHOP_ITEM_NONE && shop_item_is_photo(shop_item))) {

                ride->price_secondary = price;
                window_invalidate_by_number(WC_RIDE, rideId);
            }
        }
    }
}

bool ride_type_has_flag(sint32 rideType, sint32 flag)
{
    return (RideProperties[rideType].flags & flag) != 0;
}

/*
 * The next eight functions are helpers to access ride data at the offset 10E &
 * 110. Known as the turn counts. There are 3 different types (default, banked, sloped)
 * and there are 4 counts as follows:
 *
 * 1 element turns: low 5 bits
 * 2 element turns: bits 6-8
 * 3 element turns: bits 9-11
 * 4 element or more turns: bits 12-15
 *
 * 4 plus elements only possible on sloped type. Falls back to 3 element
 * if by some miracle you manage 4 element none sloped.
 */

void increment_turn_count_1_element(Ride* ride, uint8 type){
    uint16* turn_count;
    switch (type){
    case 0:
        turn_count = &ride->turn_count_default;
        break;
    case 1:
        turn_count = &ride->turn_count_banked;
        break;
    case 2:
        turn_count = &ride->turn_count_sloped;
        break;
    default:
        return;
    }
    uint16 value = (*turn_count & TURN_MASK_1_ELEMENT) + 1;
    *turn_count &= ~TURN_MASK_1_ELEMENT;

    if (value > TURN_MASK_1_ELEMENT)
        value = TURN_MASK_1_ELEMENT;
    *turn_count |= value;
}

void increment_turn_count_2_elements(Ride* ride, uint8 type){
    uint16* turn_count;
    switch (type){
    case 0:
        turn_count = &ride->turn_count_default;
        break;
    case 1:
        turn_count = &ride->turn_count_banked;
        break;
    case 2:
        turn_count = &ride->turn_count_sloped;
        break;
    default:
        return;
    }
    uint16 value = (*turn_count & TURN_MASK_2_ELEMENTS) + 0x20;
    *turn_count &= ~TURN_MASK_2_ELEMENTS;

    if (value > TURN_MASK_2_ELEMENTS)
        value = TURN_MASK_2_ELEMENTS;
    *turn_count |= value;
}

void increment_turn_count_3_elements(Ride* ride, uint8 type){
    uint16* turn_count;
    switch (type){
    case 0:
        turn_count = &ride->turn_count_default;
        break;
    case 1:
        turn_count = &ride->turn_count_banked;
        break;
    case 2:
        turn_count = &ride->turn_count_sloped;
        break;
    default:
        return;
    }
    uint16 value = (*turn_count & TURN_MASK_3_ELEMENTS) + 0x100;
    *turn_count &= ~TURN_MASK_3_ELEMENTS;

    if (value > TURN_MASK_3_ELEMENTS)
        value = TURN_MASK_3_ELEMENTS;
    *turn_count |= value;
}

void increment_turn_count_4_plus_elements(Ride* ride, uint8 type){
    uint16* turn_count;
    switch (type){
    case 0:
    case 1:
        // Just in case fallback to 3 element turn
        increment_turn_count_3_elements(ride, type);
        return;
    case 2:
        turn_count = &ride->turn_count_sloped;
        break;
    default:
        return;
    }
    uint16 value = (*turn_count & TURN_MASK_4_PLUS_ELEMENTS) + 0x800;
    *turn_count &= ~TURN_MASK_4_PLUS_ELEMENTS;

    if (value > TURN_MASK_4_PLUS_ELEMENTS)
        value = TURN_MASK_4_PLUS_ELEMENTS;
    *turn_count |= value;
}

sint32 get_turn_count_1_element(Ride* ride, uint8 type) {
    uint16* turn_count;
    switch (type){
    case 0:
        turn_count = &ride->turn_count_default;
        break;
    case 1:
        turn_count = &ride->turn_count_banked;
        break;
    case 2:
        turn_count = &ride->turn_count_sloped;
        break;
    default:
        return 0;
    }

    return (*turn_count) & TURN_MASK_1_ELEMENT;
}

sint32 get_turn_count_2_elements(Ride* ride, uint8 type) {
    uint16* turn_count;
    switch (type){
    case 0:
        turn_count = &ride->turn_count_default;
        break;
    case 1:
        turn_count = &ride->turn_count_banked;
        break;
    case 2:
        turn_count = &ride->turn_count_sloped;
        break;
    default:
        return 0;
    }

    return ((*turn_count) & TURN_MASK_2_ELEMENTS) >> 5;
}

sint32 get_turn_count_3_elements(Ride* ride, uint8 type) {
    uint16* turn_count;
    switch (type){
    case 0:
        turn_count = &ride->turn_count_default;
        break;
    case 1:
        turn_count = &ride->turn_count_banked;
        break;
    case 2:
        turn_count = &ride->turn_count_sloped;
        break;
    default:
        return 0;
    }

    return ((*turn_count) & TURN_MASK_3_ELEMENTS) >> 8;
}

sint32 get_turn_count_4_plus_elements(Ride* ride, uint8 type) {
    uint16* turn_count;
    switch (type){
    case 0:
    case 1:
        return 0;
    case 2:
        turn_count = &ride->turn_count_sloped;
        break;
    default:
        return 0;
    }

    return ((*turn_count) & TURN_MASK_4_PLUS_ELEMENTS) >> 11;
}

bool ride_has_spinning_tunnel(Ride *ride) {
    return ride->special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
}

bool ride_has_water_splash(Ride *ride) {
    return ride->special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
}

bool ride_has_rapids(Ride *ride) {
    return ride->special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
}

bool ride_has_log_reverser(Ride *ride) {
    return ride->special_track_elements & RIDE_ELEMENT_REVERSER_OR_WATERFALL;
}

bool ride_has_waterfall(Ride *ride) {
    return ride->special_track_elements & RIDE_ELEMENT_REVERSER_OR_WATERFALL;
}

bool ride_has_whirlpool(Ride *ride) {
    return ride->special_track_elements & RIDE_ELEMENT_WHIRLPOOL;
}

uint8 ride_get_helix_sections(Ride *ride) {
    // Helix sections stored in the low 5 bits.
    return ride->special_track_elements & 0x1F;
}

bool ride_is_powered_launched(Ride *ride)
{
    return
        ride->mode == RIDE_MODE_POWERED_LAUNCH_PASSTROUGH ||
        ride->mode == RIDE_MODE_POWERED_LAUNCH ||
        ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED;
}

bool ride_is_block_sectioned(Ride *ride)
{
    return
        ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED ||
        ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED;
}

bool ride_has_any_track_elements(sint32 rideIndex)
{
    map_element_iterator it;

    map_element_iterator_begin(&it);
    while (map_element_iterator_next(&it)) {
        if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_TRACK)
            continue;
        if (it.element->properties.track.ride_index != rideIndex)
            continue;
        if (it.element->flags & MAP_ELEMENT_FLAG_GHOST)
            continue;

        return true;
    }

    return false;
}

void ride_all_has_any_track_elements(bool *rideIndexArray)
{
    map_element_iterator it;

    memset(rideIndexArray, 0, MAX_RIDES * sizeof(bool));

    map_element_iterator_begin(&it);
    while (map_element_iterator_next(&it)) {
        if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_TRACK)
            continue;
        if (it.element->flags & MAP_ELEMENT_FLAG_GHOST)
            continue;

        rideIndexArray[it.element->properties.track.ride_index] = true;
    }
}

/**
 *
 *  rct2: 0x006847BA
 */
void set_vehicle_type_image_max_sizes(rct_ride_entry_vehicle* vehicle_type, sint32 num_images){
    uint8 bitmap[200][200] = { 0 };

    rct_drawpixelinfo dpi = {
        .bits = (uint8*)bitmap,
        .x = -100,
        .y = -100,
        .width = 200,
        .height = 200,
        .pitch = 0,
        .zoom_level = 0
    };

    for (sint32 i = 0; i < num_images; ++i){
        gfx_draw_sprite_software(&dpi, vehicle_type->base_image_id + i, 0, 0, 0);
    }
    sint32 al = -1;
    for (sint32 i = 99; i != 0; --i){
        for (sint32 j = 0; j < 200; j++){
            if (bitmap[j][100 - i] != 0){
                al = i;
                break;
            }
        }

        if (al != -1)
            break;

        for (sint32 j = 0; j < 200; j++){
            if (bitmap[j][100 + i] != 0){
                al = i;
                break;
            }
        }

        if (al != -1)
            break;
    }

    al++;
    sint32 bl = -1;

    for (sint32 i = 99; i != 0; --i){
        for (sint32 j = 0; j < 200; j++){
            if (bitmap[100 - i][j] != 0){
                bl = i;
                break;
            }
        }

        if (bl != -1)
            break;
    }
    bl++;

    sint32 bh = -1;

    for (sint32 i = 99; i != 0; --i){
        for (sint32 j = 0; j < 200; j++){
            if (bitmap[100 + i][j] != 0){
                bh = i;
                break;
            }
        }

        if (bh != -1)
            break;
    }
    bh++;

    // Moved from object paint

    if (vehicle_type->flags & VEHICLE_ENTRY_FLAG_13){
        bl += 16;
    }

    vehicle_type->sprite_width = al;
    vehicle_type->sprite_height_negative = bl;
    vehicle_type->sprite_height_positive = bh;
}

static sint32 loc_6CD18E(sint16 mapX, sint16 mapY, sint16 entranceMinX, sint16 entranceMinY, sint16 entranceMaxX, sint16 entranceMaxY)
{
    sint32 direction = 0;
    if (mapX == entranceMinX) {
        if (mapY > entranceMinY && mapY < entranceMaxY) {
            return direction;
        }
    }
    direction = 1;
    if (mapY == entranceMaxY) {
        if (mapX > entranceMinX && mapX < entranceMaxX) {
            return direction;
        }
    }
    direction = 2;
    if (mapX == entranceMaxX) {
        if (mapY > entranceMinY && mapY < entranceMaxY) {
            return direction;
        }
    }
    direction = 3;
    if (mapY == entranceMinY) {
        if (mapX > entranceMinX && mapX < entranceMaxX) {
            return direction;
        }
    }
    return -1;
}

/**
 *
 *  rct2: 0x006CCF70
 */
void ride_get_entrance_or_exit_position_from_screen_position(sint32 screenX, sint32 screenY, sint32 *outX, sint32 *outY, sint32 *outDirection)
{
    sint16 mapX, mapY;
    sint16 entranceMinX, entranceMinY, entranceMaxX, entranceMaxY, word_F4418C, word_F4418E;
    sint32 interactionType, direction, stationHeight, stationDirection;
    rct_map_element *mapElement;
    rct_viewport *viewport;
    Ride *ride;

    gRideEntranceExitPlaceDirection = 255;
    get_map_coordinates_from_pos(screenX, screenY, 0xFFFB, &mapX, &mapY, &interactionType, &mapElement, &viewport);
    if (interactionType != 0) {
        if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK) {
            if (mapElement->properties.track.ride_index == gRideEntranceExitPlaceRideIndex) {
                if (TrackSequenceProperties[mapElement->properties.track.type][0] & TRACK_SEQUENCE_FLAG_ORIGIN) {
                    if (mapElement->properties.track.type == TRACK_ELEM_MAZE) {
                        gRideEntranceExitPlaceStationIndex = 0;
                    } else {
                        gRideEntranceExitPlaceStationIndex = map_element_get_station(mapElement);
                    }
                }
            }
        }
    }

    ride = get_ride(gRideEntranceExitPlaceRideIndex);
    stationHeight = ride->station_heights[gRideEntranceExitPlaceStationIndex];

    screen_get_map_xy_with_z(screenX, screenY, stationHeight * 8, &mapX, &mapY);
    if (mapX == MAP_LOCATION_NULL) {
        *outX = 0x8000;
        return;
    }

    word_F4418C = mapX;
    word_F4418E = mapY;
    _unkF44188.x = floor2(mapX, 32);
    _unkF44188.y = floor2(mapY, 32);
    *outX = _unkF44188.x;
    *outY = _unkF44188.y;

    if (ride->type == RIDE_TYPE_NULL)
        return;

    rct_xy8 stationStart = ride->station_starts[gRideEntranceExitPlaceStationIndex];
    if (stationStart.xy == RCT_XY8_UNDEFINED)
        return;

    _unkF44188.z = stationHeight;

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3)) {
        mapX = (word_F4418C & 0x1F) - 16;
        mapY = (word_F4418E & 0x1F) - 16;
        if (abs(mapX) < abs(mapY)) {
            direction = mapY < 0 ? 3 : 1;
        } else {
            direction = mapX < 0 ? 0 : 2;
        }

        for (sint32 i = 0; i < MAX_STATIONS; i++) {
            mapX = _unkF44188.x + TileDirectionDelta[direction].x;
            mapY = _unkF44188.y + TileDirectionDelta[direction].y;
            if (mapX >= 0 && mapY >= 0 && mapX < (256 * 32) && mapY < (256 * 32)) {
                mapElement = map_get_first_element_at(mapX >> 5, mapY >> 5);
                do {
                    if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
                        continue;
                    if (mapElement->base_height != stationHeight)
                        continue;
                    if (mapElement->properties.track.ride_index != gRideEntranceExitPlaceRideIndex)
                        continue;
                    if (mapElement->properties.track.type == TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP) {
                        gRideEntranceExitPlaceDirection = direction ^ 2;
                        *outDirection = direction ^ 2;
                        return;
                    }
                    if (map_element_get_station(mapElement) != gRideEntranceExitPlaceStationIndex)
                        continue;

                    sint32 eax = (direction + 2 - map_element_get_direction(mapElement)) & MAP_ELEMENT_DIRECTION_MASK;
                    if (FlatRideTrackSequenceProperties[mapElement->properties.track.type][map_element_get_track_sequence(mapElement)] & (1 << eax)) {
                        gRideEntranceExitPlaceDirection = direction ^ 2;
                        *outDirection = direction ^ 2;
                        return;
                    }
                } while (!map_element_is_last_for_tile(mapElement++));
            }
            direction = (direction + 1) & 3;
        }
        gRideEntranceExitPlaceDirection = 0xFF;
    } else {
        mapX = stationStart.x * 32;
        mapY = stationStart.y * 32;
        entranceMinX = mapX;
        entranceMinY = mapY;

        mapElement = ride_get_station_start_track_element(ride, gRideEntranceExitPlaceStationIndex);
        if (mapElement == NULL) {
            *outX = 0x8000;
            return;
        }
        direction = map_element_get_direction(mapElement);
        stationDirection = direction;

    nextTile:
        entranceMaxX = mapX;
        entranceMaxY = mapY;
        mapX -= TileDirectionDelta[direction].x;
        mapY -= TileDirectionDelta[direction].y;
        mapElement = map_get_first_element_at(mapX >> 5, mapY >> 5);
        do {
            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
                continue;
            if (mapElement->properties.track.ride_index != gRideEntranceExitPlaceRideIndex)
                continue;
            if (map_element_get_station(mapElement) != gRideEntranceExitPlaceStationIndex)
                continue;

            switch (mapElement->properties.track.type) {
            case TRACK_ELEM_END_STATION:
            case TRACK_ELEM_BEGIN_STATION:
            case TRACK_ELEM_MIDDLE_STATION:
                goto nextTile;
            }
        } while (!map_element_is_last_for_tile(mapElement++));

        mapX = entranceMinX;
        if (mapX > entranceMaxX) {
            entranceMinX = entranceMaxX;
            entranceMaxX = mapX;
        }

        mapY = entranceMinY;
        if (mapY > entranceMaxY) {
            entranceMinY = entranceMaxY;
            entranceMaxY = mapY;
        }

        direction = loc_6CD18E(*outX, *outY, entranceMinX - 32, entranceMinY - 32, entranceMaxX + 32, entranceMaxY + 32);
        if (direction != -1 && direction != stationDirection && direction != (stationDirection ^ 2)) {
            gRideEntranceExitPlaceDirection = direction;
            *outDirection = direction;
            return;
        }
    }
}

bool ride_select_backwards_from_front()
{
    track_begin_end trackBeginEnd;

    ride_construction_invalidate_current_track();
    if (track_block_get_previous_from_zero(_currentTrackBeginX, _currentTrackBeginY, _currentTrackBeginZ, _currentRideIndex, _currentTrackPieceDirection, &trackBeginEnd)) {
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
        _currentTrackBeginX = trackBeginEnd.begin_x;
        _currentTrackBeginY = trackBeginEnd.begin_y;
        _currentTrackBeginZ = trackBeginEnd.begin_z;
        _currentTrackPieceDirection = trackBeginEnd.begin_direction;
        _currentTrackPieceType = trackBeginEnd.begin_element->properties.track.type;
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        return true;
    } else {
        return false;
    }
}

bool ride_select_forwards_from_back()
{
    sint32 x, y, z, direction;

    ride_construction_invalidate_current_track();

    x = _currentTrackBeginX;
    y = _currentTrackBeginY;
    z = _currentTrackBeginZ;
    direction = _currentTrackPieceDirection ^ 2;
    rct_xy_element next_track;

    if (track_block_get_next_from_zero(x, y, z, _currentRideIndex, direction, &next_track, &z, &direction)) {
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
        _currentTrackBeginX = next_track.x;
        _currentTrackBeginY = next_track.y;
        _currentTrackBeginZ = z;
        _currentTrackPieceDirection = map_element_get_direction(next_track.element);
        _currentTrackPieceType = next_track.element->properties.track.type;
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        return true;
    } else {
        return false;
    }
}

money32 ride_remove_track_piece(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 type, uint8 flags)
{
    gGameCommandErrorTitle = STR_RIDE_CONSTRUCTION_CANT_REMOVE_THIS;
    if (network_get_mode() == NETWORK_MODE_CLIENT)
    {
        game_command_callback = game_command_callback_ride_remove_track_piece;
    }
    return game_do_command(x, flags | ((direction & 3) << 8), y, type, GAME_COMMAND_REMOVE_TRACK, z, 0);
}

/**
 *
 *  rct2: 0x006B58EF
 */
bool ride_are_all_possible_entrances_and_exits_built(Ride *ride)
{
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
        return true;

    for (sint32 i = 0; i < MAX_STATIONS; i++) {
        if (ride->station_starts[i].xy == RCT_XY8_UNDEFINED)
        {
            continue;
        }
        if (ride->entrances[i].xy == RCT_XY8_UNDEFINED) {
            gGameCommandErrorText = STR_ENTRANCE_NOT_YET_BUILT;
            return false;
        }
        if (ride->exits[i].xy == RCT_XY8_UNDEFINED) {
            gGameCommandErrorText = STR_EXIT_NOT_YET_BUILT;
            return false;
        }
    }
    return true;
}

/**
 *
 *  rct2: 0x006B59C6
 */
void invalidate_test_results(sint32 rideIndex)
{
    Ride *ride = get_ride(rideIndex);

    ride_measurement_clear(ride);
    ride->excitement = RIDE_RATING_UNDEFINED;
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_TESTED;
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_TEST_IN_PROGRESS;
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) {
        for (sint32 i = 0; i < ride->num_vehicles; i++) {
            uint16 spriteIndex = ride->vehicles[i];
            if (spriteIndex != SPRITE_INDEX_NULL) {
                rct_vehicle *vehicle = GET_VEHICLE(spriteIndex);
                vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_TESTING;
            }
        }
    }
    window_invalidate_by_number(WC_RIDE, rideIndex);
}

/**
 *
 *  rct2: 0x006B7481
 *
 * @param rideIndex (dl)
 * @param reliabilityIncreaseFactor (ax)
 */
void ride_fix_breakdown(sint32 rideIndex, sint32 reliabilityIncreaseFactor)
{
    Ride *ride = get_ride(rideIndex);

    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_BREAKDOWN_PENDING;
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_BROKEN_DOWN;
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST | RIDE_INVALIDATE_RIDE_MAINTENANCE;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) {
        for (sint32 i = 0; i < ride->num_vehicles; i++) {
            uint16 spriteIndex = ride->vehicles[i];
            while (spriteIndex != SPRITE_INDEX_NULL) {
                rct_vehicle *vehicle = GET_VEHICLE(spriteIndex);
                vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_7;
                vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_BROKEN_CAR;
                vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_BROKEN_TRAIN;
                spriteIndex = vehicle->next_vehicle_on_train;
            }
        }
    }

    uint8 unreliability = 100 - ride->reliability_percentage;
    ride->reliability += reliabilityIncreaseFactor * (unreliability / 2);
}

/**
 *
 *  rct2: 0x006DE102
 */
static void ride_update_vehicle_colours(sint32 rideIndex)
{
    Ride *ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_SPACE_RINGS || ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_16)) {
        gfx_invalidate_screen();
    }

    for (sint32 i = 0; i < MAX_VEHICLES_PER_RIDE; i++) {
        sint32 carIndex = 0;
        uint16 spriteIndex = ride->vehicles[i];
        rct_vehicle_colour colours = { 0 };
        uint8 coloursExtended = 0;

        while (spriteIndex != SPRITE_INDEX_NULL) {
            rct_vehicle *vehicle = GET_VEHICLE(spriteIndex);
            switch (ride->colour_scheme_type & 3) {
            case RIDE_COLOUR_SCHEME_ALL_SAME:
                colours = ride->vehicle_colours[0];
                coloursExtended = ride->vehicle_colours_extended[0];
                break;
            case RIDE_COLOUR_SCHEME_DIFFERENT_PER_TRAIN:
                colours = ride->vehicle_colours[i];
                coloursExtended = ride->vehicle_colours_extended[i];
                break;
            case RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR:
                colours = ride->vehicle_colours[min(carIndex, MAX_CARS_PER_TRAIN - 1)];
                coloursExtended = ride->vehicle_colours_extended[min(carIndex, MAX_CARS_PER_TRAIN - 1)];
                break;
            }

            vehicle->colours = colours;
            vehicle->colours_extended = coloursExtended;
            invalidate_sprite_2((rct_sprite*)vehicle);
            spriteIndex = vehicle->next_vehicle_on_train;
            carIndex++;
        }
    }
}

/**
 *
 *  rct2: 0x006DE4CD
 * trainLayout: Originally fixed to 0x00F64E38. This no longer postfixes with 255.
 */
void ride_entry_get_train_layout(sint32 rideEntryIndex, sint32 numCarsPerTrain, uint8 *trainLayout)
{
    rct_ride_entry *rideEntry = get_ride_entry(rideEntryIndex);

    for (sint32 i = 0; i < numCarsPerTrain; i++) {
        uint8 vehicleType = rideEntry->default_vehicle;
        if (i == 0 && rideEntry->front_vehicle != 255) {
            vehicleType = rideEntry->front_vehicle;
        } else if (i == 1 && rideEntry->second_vehicle != 255) {
            vehicleType = rideEntry->second_vehicle;
        } else if (i == 2 && rideEntry->third_vehicle != 255) {
            vehicleType = rideEntry->third_vehicle;
        } else if (i == numCarsPerTrain - 1 && rideEntry->rear_vehicle != 255) {
            vehicleType = rideEntry->rear_vehicle;
        }
        trainLayout[i] = vehicleType;
    }
}

uint8 ride_entry_get_vehicle_at_position(sint32 rideEntryIndex,sint32 numCarsPerTrain,sint32 position)
{
    rct_ride_entry *rideEntry = get_ride_entry(rideEntryIndex);
    if (position == 0 && rideEntry->front_vehicle != 255) {
        return rideEntry->front_vehicle;
    } else if (position == 1 && rideEntry->second_vehicle != 255) {
        return rideEntry->second_vehicle;
    } else if (position == 2 && rideEntry->third_vehicle != 255) {
        return rideEntry->third_vehicle;
    } else if (position == numCarsPerTrain - 1 && rideEntry->rear_vehicle != 255) {
        return rideEntry->rear_vehicle;
    } else {
        return rideEntry->default_vehicle;
    }
}

// Finds track pieces that a given ride entry has sprites for
uint64 ride_entry_get_supported_track_pieces(rct_ride_entry* rideEntry)
{
    uint64 supportedPieces = 0xFFFFFFFFFFFFFFFFULL;
    uint16 trackPieceRequiredSprites[55] = {
        0x0001u, 0x0001u, 0x0001u, 0x0000u, 0x0006u, 0x0002u, 0x0020u, 0x000E,
        0x0003u, 0x0006u, 0x0007u, 0x0002u, 0x0004u, 0x0001u, 0x0001u, 0x0001u,
        0x0001u, 0x0061u, 0x000E, 0x1081u, 0x0001u, 0x0020u, 0x0020u, 0x0001u,
        0x0001u, 0x0000u, 0x0001u, 0x0001u, 0x000C, 0x0061u, 0x0002u, 0x000E,
        0x0480u, 0x0001u, 0x0061u, 0x0001u, 0x0001u, 0x000Fu, 0x0001u, 0x0200u,
        0x0007u, 0x0008u, 0x0000u, 0x0000u, 0x4000u, 0x0008u, 0x0001u, 0x0001u,
        0x0061u, 0x0061u, 0x0008u, 0x0008u, 0x0001u, 0x000Eu, 0x000Eu
    };

    //Only check default vehicle; it's assumed the others will have correct sprites if this one does (I've yet to find an exception, at least)
    for (sint32 j = 0; j < 55; j++) {
        if ((rideEntry->vehicles[rideEntry->default_vehicle].sprite_flags & trackPieceRequiredSprites[j]) != trackPieceRequiredSprites[j])
            supportedPieces &= ~(1ULL << j);
    }

    return supportedPieces;
}

static sint32 ride_get_smallest_station_length(Ride *ride)
{
    uint32 result = -1;
    for (sint32 i = 0; i < MAX_STATIONS; i++) {
        if (ride->station_starts[i].xy != RCT_XY8_UNDEFINED) {
            result = min(result, (uint32)(ride->station_length[i]));
        }
    }
    return (sint32)result;
}

/**
 *
 *  rct2: 0x006CB3AA
 */
static sint32 ride_get_track_length(Ride *ride)
{
    rct_window *w;
    rct_map_element *mapElement;
    track_circuit_iterator it;
    sint32 x, y, z, trackType, rideIndex, result;

    for (sint32 i = 0; i < MAX_STATIONS; i++) {
        rct_xy8 location = ride->station_starts[i];
        if (location.xy == RCT_XY8_UNDEFINED)
            continue;

        x = location.x * 32;
        y = location.y * 32;
        z = ride->station_heights[i];

        mapElement = map_get_first_element_at(x >> 5, y >> 5);
        do {
            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
                continue;

            trackType = mapElement->properties.track.type;
            if (!(TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
                continue;

            if (mapElement->base_height != z)
                continue;

            goto foundTrack;
        } while (!map_element_is_last_for_tile(mapElement++));
    }
    return 0;

foundTrack:
    rideIndex = mapElement->properties.track.ride_index;

    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != NULL && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && _currentRideIndex == rideIndex) {
        ride_construction_invalidate_current_track();
    }

    result = 0;
    track_circuit_iterator_begin(&it, (rct_xy_element){ x, y, mapElement });
    while (track_circuit_iterator_next(&it)) {
        trackType = it.current.element->properties.track.type;
        result += TrackPieceLengths[trackType];
    }
    return result;
}

/**
 *
 *  rct2: 0x006DD57D
 */
void ride_update_max_vehicles(sint32 rideIndex)
{
    Ride *ride = get_ride(rideIndex);
    if (ride->subtype == RIDE_ENTRY_INDEX_NULL)
        return;

    rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry == (rct_ride_entry*)-1)
    {
        return;
    }
    rct_ride_entry_vehicle *vehicleEntry;
    uint8 numCarsPerTrain, numVehicles;
    sint32 maxNumTrains;

    if (rideEntry->cars_per_flat_ride == 0xFF) {
        sint32 trainLength;
        ride->num_cars_per_train = max(rideEntry->min_cars_in_train, ride->num_cars_per_train);
        ride->min_max_cars_per_train = rideEntry->max_cars_in_train | (rideEntry->min_cars_in_train << 4);

        // Calculate maximum train length based on smallest station length
        sint32 stationLength = ride_get_smallest_station_length(ride);
        if (stationLength == -1)
            return;

        stationLength = (stationLength * 0x44180) - 0x16B2A;
        sint32 maxFriction = RideData5[ride->type].max_friction << 8;
        sint32 maxCarsPerTrain = 1;
        for (sint32 numCars = rideEntry->max_cars_in_train; numCars > 0; numCars--) {
            trainLength = 0;
            sint32 totalFriction = 0;
            for (sint32 i = 0; i < numCars; i++) {
                vehicleEntry = &rideEntry->vehicles[ride_entry_get_vehicle_at_position(ride->subtype, numCars, i)];
                trainLength += vehicleEntry->spacing;
                totalFriction += vehicleEntry->car_friction;
            }

            if (trainLength <= stationLength && totalFriction <= maxFriction) {
                maxCarsPerTrain = numCars;
                break;
            }
        }
        sint32 newCarsPerTrain = max(ride->proposed_num_cars_per_train, rideEntry->min_cars_in_train);
        maxCarsPerTrain = max(maxCarsPerTrain, rideEntry->min_cars_in_train);
        if (!gCheatsDisableTrainLengthLimit) {
            newCarsPerTrain = min(maxCarsPerTrain, newCarsPerTrain);
        }
        ride->min_max_cars_per_train = maxCarsPerTrain | (rideEntry->min_cars_in_train << 4);

        switch (ride->mode) {
        case RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED:
        case RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED:
            maxNumTrains = clamp(1, ride->num_stations + ride->num_block_brakes - 1, 31);
            break;
        case RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE:
        case RIDE_MODE_POWERED_LAUNCH_PASSTROUGH:
        case RIDE_MODE_SHUTTLE:
        case RIDE_MODE_LIM_POWERED_LAUNCH:
        case RIDE_MODE_POWERED_LAUNCH:
            maxNumTrains = 1;
            break;
        default:
            // Calculate maximum number of trains
            trainLength = 0;
            for (sint32 i = 0; i < newCarsPerTrain; i++) {
                vehicleEntry = &rideEntry->vehicles[ride_entry_get_vehicle_at_position(ride->subtype, newCarsPerTrain, i)];
                trainLength += vehicleEntry->spacing;
            }

            sint32 totalLength = trainLength / 2;
            if (newCarsPerTrain != 1)
                totalLength /= 2;

            maxNumTrains = 0;
            do {
                maxNumTrains++;
                totalLength += trainLength;
            } while (totalLength <= stationLength);

            if (
                (ride->mode != RIDE_MODE_STATION_TO_STATION && ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT) ||
                !(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_MORE_VEHICLES_THAN_STATION_FITS)
            ) {
                maxNumTrains = min(maxNumTrains, 31);
            } else {
                vehicleEntry = &rideEntry->vehicles[ride_entry_get_vehicle_at_position(ride->subtype, newCarsPerTrain, 0)];
                sint32 speed = vehicleEntry->powered_max_speed;

                sint32 totalSpacing = 0;
                for (sint32 i = 0; i < newCarsPerTrain; i++) {
                    vehicleEntry = &rideEntry->vehicles[ride_entry_get_vehicle_at_position(ride->subtype, newCarsPerTrain, i)];
                    totalSpacing += vehicleEntry->spacing;
                }

                totalSpacing >>= 13;
                sint32 trackLength = ride_get_track_length(ride) / 4;
                if (speed > 10) trackLength = (trackLength * 3) / 4;
                if (speed > 25) trackLength = (trackLength * 3) / 4;
                if (speed > 40) trackLength = (trackLength * 3) / 4;

                maxNumTrains = 0;
                sint32 length = 0;
                do {
                    maxNumTrains++;
                    length += totalSpacing;
                } while (maxNumTrains < 31 && length < trackLength);
            }
            break;
        }
        ride->max_trains = maxNumTrains;

        numCarsPerTrain = min(ride->proposed_num_cars_per_train, newCarsPerTrain);
    } else {
        ride->max_trains = rideEntry->cars_per_flat_ride;
        ride->min_max_cars_per_train = rideEntry->max_cars_in_train | (rideEntry->min_cars_in_train << 4);
        numCarsPerTrain = rideEntry->max_cars_in_train;
        maxNumTrains = rideEntry->cars_per_flat_ride;
    }

    if (gCheatsDisableTrainLengthLimit) {
        maxNumTrains = 31;
    }
    numVehicles = min(ride->proposed_num_vehicles, maxNumTrains);

    // Refresh new current num vehicles / num cars per vehicle
    if (numVehicles != ride->num_vehicles || numCarsPerTrain != ride->num_cars_per_train) {
        ride->num_cars_per_train = numCarsPerTrain;
        ride->num_vehicles = numVehicles;
        window_invalidate_by_number(WC_RIDE, rideIndex);
    }
}

void ride_set_ride_entry(sint32 rideIndex, sint32 rideEntry)
{
    gGameCommandErrorTitle = STR_RIDE_SET_VEHICLE_TYPE_FAIL;
    game_do_command(
        0,
        GAME_COMMAND_FLAG_APPLY | (RIDE_SET_VEHICLES_COMMAND_TYPE_RIDE_ENTRY << 8),
        0,
        (rideEntry << 8) | rideIndex,
        GAME_COMMAND_SET_RIDE_VEHICLES,
        0,
        0
    );
}

void ride_set_num_vehicles(sint32 rideIndex, sint32 numVehicles)
{
    gGameCommandErrorTitle = STR_RIDE_SET_VEHICLE_SET_NUM_TRAINS_FAIL;
    game_do_command(
        0,
        GAME_COMMAND_FLAG_APPLY | (RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_TRAINS << 8),
        0,
        (numVehicles << 8) | rideIndex,
        GAME_COMMAND_SET_RIDE_VEHICLES,
        0,
        0
    );
}

void ride_set_num_cars_per_vehicle(sint32 rideIndex, sint32 numCarsPerVehicle)
{
    gGameCommandErrorTitle = STR_RIDE_SET_VEHICLE_SET_NUM_CARS_PER_TRAIN_FAIL;
    game_do_command(
        0,
        GAME_COMMAND_FLAG_APPLY | (RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_CARS_PER_TRAIN << 8),
        0,
        (numCarsPerVehicle << 8) | rideIndex,
        GAME_COMMAND_SET_RIDE_VEHICLES,
        0,
        0
    );
}

static bool ride_is_vehicle_type_valid(Ride *ride, uint8 inputRideEntryIndex)
{
    bool selectionShouldBeExpanded;
    sint32 rideTypeIterator, rideTypeIteratorMax;

    if (gCheatsShowVehiclesFromOtherTrackTypes &&
        !(ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) || ride->type == RIDE_TYPE_MAZE || ride->type == RIDE_TYPE_MINI_GOLF)
    ) {
        selectionShouldBeExpanded = true;
        rideTypeIterator = 0;
        rideTypeIteratorMax = RIDE_TYPE_COUNT - 1;
    } else {
        selectionShouldBeExpanded = false;
        rideTypeIterator = ride->type;
        rideTypeIteratorMax = ride->type;
    }

    for (; rideTypeIterator <= rideTypeIteratorMax; rideTypeIterator++) {
        if (selectionShouldBeExpanded) {
            if (ride_type_has_flag(rideTypeIterator, RIDE_TYPE_FLAG_FLAT_RIDE)) continue;
            if (rideTypeIterator == RIDE_TYPE_MAZE || rideTypeIterator == RIDE_TYPE_MINI_GOLF) continue;
        }

        uint8 *rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(rideTypeIterator);
        for (uint8 *currentRideEntryIndex = rideEntryIndexPtr; *currentRideEntryIndex != RIDE_ENTRY_INDEX_NULL; currentRideEntryIndex++) {
            uint8 rideEntryIndex = *currentRideEntryIndex;
            if (rideEntryIndex == inputRideEntryIndex) {
                if (!ride_entry_is_invented(rideEntryIndex) && !gCheatsIgnoreResearchStatus) {
                    return false;
                }

                return true;
            }
        }
    }

    return false;
}

static money32 ride_set_vehicles(uint8 rideIndex, uint8 setting, uint8 value, uint32 flags, uint8 ex)
{
    rct_ride_entry *rideEntry;

    Ride *ride = get_ride(rideIndex);
    if (ride == NULL || ride->type == RIDE_TYPE_NULL) {
        log_warning("Invalid game command for ride #%u", rideIndex);
        return MONEY32_UNDEFINED;
    }

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
        gGameCommandErrorText = STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING;
        return MONEY32_UNDEFINED;
    }

    if (ride->status != RIDE_STATUS_CLOSED) {
        gGameCommandErrorText = STR_MUST_BE_CLOSED_FIRST;
        return MONEY32_UNDEFINED;
    }

    switch (setting) {
    case RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_TRAINS:
        if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
            return 0;
        }

        ride_clear_for_construction(rideIndex);
        ride_remove_peeps(rideIndex);
        ride->vehicle_change_timeout = 100;

        ride->proposed_num_vehicles = value;
        break;
    case RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_CARS_PER_TRAIN:
        if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
            return 0;
        }

        ride_clear_for_construction(rideIndex);
        ride_remove_peeps(rideIndex);
        ride->vehicle_change_timeout = 100;

        invalidate_test_results(rideIndex);
        rideEntry = get_ride_entry(ride->subtype);
        if (!gCheatsDisableTrainLengthLimit) {
            value = clamp(rideEntry->min_cars_in_train, value, rideEntry->max_cars_in_train);
        }
        ride->proposed_num_cars_per_train = value;
        break;
    case RIDE_SET_VEHICLES_COMMAND_TYPE_RIDE_ENTRY:
        if (!ride_is_vehicle_type_valid(ride, value)) {
            log_error("Invalid vehicle type.");
            return MONEY32_UNDEFINED;
        }

        if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
            return 0;
        }

        ride_clear_for_construction(rideIndex);
        ride_remove_peeps(rideIndex);
        ride->vehicle_change_timeout = 100;

        invalidate_test_results(rideIndex);
        rideEntry = get_ride_entry(ride->subtype);
        ride->subtype = value;

        uint8 preset = ex;
        if (!(flags & GAME_COMMAND_FLAG_NETWORKED)) {
            preset = ride_get_unused_preset_vehicle_colour(ride->type, ride->subtype);
        }

        // Validate preset
        vehicle_colour_preset_list *presetList = rideEntry->vehicle_preset_list;
        if (preset >= presetList->count) {
            log_error("Unknown vehicle colour preset.");
            return MONEY32_UNDEFINED;
        }

        ride_set_vehicle_colours_to_random_preset(ride, preset);
        if (!gCheatsDisableTrainLengthLimit) {
            ride->proposed_num_cars_per_train = clamp(rideEntry->min_cars_in_train, ride->proposed_num_cars_per_train, rideEntry->max_cars_in_train);
        }
        break;
    default:
        log_error("Unknown vehicle command.");
        return MONEY32_UNDEFINED;
    }

    ride->num_circuits = 1;
    ride_update_max_vehicles(rideIndex);

    if (ride->overall_view.xy != RCT_XY8_UNDEFINED) {
        rct_xyz16 coord;
        coord.x = ride->overall_view.x * 32 + 16;
        coord.y = ride->overall_view.y * 32 + 16;
        coord.z = map_element_height(coord.x, coord.y);
        network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
    }

    rct_window *w = window_find_by_number(WC_RIDE, rideIndex);
    if (w != NULL) {
        if (w->page == 4) { // WINDOW_RIDE_PAGE_COLOUR
            w->vehicleIndex = 0;
        }
        window_invalidate(w);
    }

    gfx_invalidate_screen();
    return 0;
}

/**
 *
 *  rct2: 0x006B52D4
 */
void game_command_set_ride_vehicles(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    uint8 rideIndex = *edx & 0xFF;
    uint8 setting = (*ebx >> 8) & 0xFF;
    uint8 value = (*edx >> 8) & 0xFF;
    uint32 flags = *ebx;
    uint8 ex = *eax & 0xFF;
    *ebx = ride_set_vehicles(rideIndex, setting, value, flags, ex);
}

/**
 *
 *  rct2: 0x006CB945
 */
void sub_6CB945(sint32 rideIndex)
{
    Ride* ride = get_ride(rideIndex);
    if (ride->type != RIDE_TYPE_MAZE) {
        for (uint8 stationId = 0; stationId < MAX_STATIONS; ++stationId) {
            if (ride->station_starts[stationId].xy == RCT_XY8_UNDEFINED)
                continue;

            rct_xyz16 location = {
                .x = ride->station_starts[stationId].x * 32,
                .y = ride->station_starts[stationId].y * 32,
                .z = ride->station_heights[stationId]
            };
            uint8 direction = 0xFF;

            bool specialTrack = false;
            rct_map_element* mapElement = NULL;

            while (true) {
                if (direction != 0xFF) {
                    location.x -= TileDirectionDelta[direction].x;
                    location.y -= TileDirectionDelta[direction].y;
                }
                mapElement = map_get_first_element_at(location.x >> 5, location.y >> 5);

                bool trackFound = false;
                do {
                    if (mapElement->base_height != location.z) continue;
                    if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;
                    if (mapElement->properties.track.ride_index != rideIndex) continue;
                    if (map_element_get_track_sequence(mapElement) != 0) continue;
                    if (!(TrackSequenceProperties[mapElement->properties.track.type][0] & TRACK_SEQUENCE_FLAG_ORIGIN)) continue;

                    trackFound = true;
                    break;
                } while (!map_element_is_last_for_tile(mapElement++));

                if (trackFound == false) {
                    break;
                }

                map_element_set_station(mapElement, stationId);
                direction = map_element_get_direction(mapElement);

                if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3)) {
                    specialTrack = true;
                    break;
                }
            }

            if (specialTrack == false) {
                continue;
            }

            const rct_preview_track *trackBlock = get_track_def_from_ride(ride, mapElement->properties.track.type);
            while ((++trackBlock)->index != 0xFF) {
                rct_xyz16 blockLocation = location;
                map_offset_with_rotation(&blockLocation.x, &blockLocation.y, trackBlock->x, trackBlock->y, direction);

                bool trackFound = false;
                mapElement = map_get_first_element_at(blockLocation.x >> 5, blockLocation.y >> 5);
                do {
                    if (blockLocation.z != mapElement->base_height) continue;
                    if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;
                    if (!(TrackSequenceProperties[mapElement->properties.track.type][0] & TRACK_SEQUENCE_FLAG_ORIGIN)) continue;

                    trackFound = true;
                    break;
                } while (!map_element_is_last_for_tile(mapElement++));

                if (!trackFound) {
                    break;
                }

                map_element_set_station(mapElement, stationId);
            }
        }
    }

    // Needs room for an entrance and an exit per station, plus one position for the list terminator.
    uint16 locations[(MAX_STATIONS * 2) + 1];
    uint16 *locationList = locations;
    for (uint8 stationId = 0; stationId < MAX_STATIONS; ++stationId) {
        if (ride->entrances[stationId].xy != RCT_XY8_UNDEFINED) {
            *locationList++ = ride->entrances[stationId].xy;
            ride->entrances[stationId].xy = RCT_XY8_UNDEFINED;
        }

        if (ride->exits[stationId].xy != RCT_XY8_UNDEFINED) {
            *locationList++ = ride->exits[stationId].xy;
            ride->exits[stationId].xy = RCT_XY8_UNDEFINED;
        }
    }
    *locationList++ = RCT_XY8_UNDEFINED;

    locationList = locations;
    for (; *locationList != RCT_XY8_UNDEFINED; locationList++) {
        uint16* locationList2 = locationList;
        locationList2++;

        bool duplicateLocation = false;
        do {
            if (*locationList == *locationList2) {
                duplicateLocation = true;
                break;
            }
        } while (*locationList2++ != RCT_XY8_UNDEFINED);

        if (duplicateLocation == true) {
            continue;
        }

        rct_xy16 location = {
            .x = (*locationList & 0xFF) * 32,
            .y = ((*locationList >> 8) & 0xFF) * 32
        };

        rct_map_element *mapElement = map_get_first_element_at(location.x >> 5, location.y >> 5);
        do {
            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE) continue;
            if (mapElement->properties.entrance.ride_index != rideIndex) continue;
            if (mapElement->properties.entrance.type > ENTRANCE_TYPE_RIDE_EXIT) continue;

            rct_xy16 nextLocation = location;
            nextLocation.x += TileDirectionDelta[map_element_get_direction(mapElement)].x;
            nextLocation.y += TileDirectionDelta[map_element_get_direction(mapElement)].y;

            bool shouldRemove = true;
            rct_map_element *trackElement = map_get_first_element_at(nextLocation.x >> 5, nextLocation.y >> 5);
            do {
                if (map_element_get_type(trackElement) != MAP_ELEMENT_TYPE_TRACK) continue;
                if (trackElement->properties.track.ride_index != rideIndex) continue;
                if (trackElement->base_height != mapElement->base_height) continue;

                uint8 trackType = trackElement->properties.track.type;
                uint8 trackSequence = map_element_get_track_sequence(mapElement);

                uint8 direction = (map_element_get_direction(mapElement) - map_element_get_direction(trackElement) + 2) & 3;

                if (!(TrackSequenceProperties[trackType][trackSequence] & (1 << direction))) {
                    continue;
                }

                uint8 stationId = 0;
                if (trackElement->properties.track.type != TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP) {
                    stationId = map_element_get_station(trackElement);
                }

                if (mapElement->properties.entrance.type == ENTRANCE_TYPE_RIDE_EXIT) {
                    if (ride->exits[stationId].xy != RCT_XY8_UNDEFINED) {
                        break;
                    }
                    ride->exits[stationId].x = location.x / 32;
                    ride->exits[stationId].y = location.y / 32;
                } else {
                    if (ride->entrances[stationId].xy != RCT_XY8_UNDEFINED) {
                        break;
                    }
                    ride->entrances[stationId].x = location.x / 32;
                    ride->entrances[stationId].y = location.y / 32;
                }

                mapElement->properties.entrance.index &= 0x8F;
                mapElement->properties.entrance.index |= stationId << 4;
                shouldRemove = false;
            } while (!map_element_is_last_for_tile(trackElement++));

            if (shouldRemove == true) {
                footpath_queue_chain_reset();
                maze_entrance_hedge_replacement(location.x, location.y, mapElement);
                footpath_remove_edges_at(location.x, location.y, mapElement);
                footpath_update_queue_chains();
                map_invalidate_tile_full(location.x, location.y);
                map_element_remove(mapElement);
                mapElement--;
            }
        } while (!map_element_is_last_for_tile(mapElement++));
    }
}


void ride_set_to_default_inspection_interval(sint32 rideIndex)
{
    Ride *ride = get_ride(rideIndex);
    uint8 defaultInspectionInterval = gConfigGeneral.default_inspection_interval;
    if (ride->inspection_interval != defaultInspectionInterval) {
        if (defaultInspectionInterval <= RIDE_INSPECTION_NEVER) {
            gGameCommandErrorTitle = STR_CANT_CHANGE_OPERATING_MODE;
            game_do_command(0, (defaultInspectionInterval << 8) | 1, 0, (5 << 8) | rideIndex, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
        }
    }
}

/**
 *
 *  rct2: 0x006B752C
 */
void ride_crash(uint8 rideIndex, uint8 vehicleIndex)
{
    Ride *ride = get_ride(rideIndex);
    rct_vehicle *vehicle = GET_VEHICLE(ride->vehicles[vehicleIndex]);

    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)) {
        // Open ride window for crashed vehicle
        rct_window *w = window_ride_open_vehicle(vehicle);
        if (w->viewport != NULL) {
            w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
        }
    }

    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32, ride->name_arguments);
    if (gConfigNotifications.ride_crashed) {
        news_item_add_to_queue(NEWS_ITEM_RIDE, STR_RIDE_HAS_CRASHED, rideIndex);
    }
}

bool shop_item_is_food_or_drink(sint32 shopItem)
{
    switch (shopItem) {
    case SHOP_ITEM_DRINK:
    case SHOP_ITEM_BURGER:
    case SHOP_ITEM_CHIPS:
    case SHOP_ITEM_ICE_CREAM:
    case SHOP_ITEM_CANDYFLOSS:
    case SHOP_ITEM_PIZZA:
    case SHOP_ITEM_POPCORN:
    case SHOP_ITEM_HOT_DOG:
    case SHOP_ITEM_TENTACLE:
    case SHOP_ITEM_CANDY_APPLE:
    case SHOP_ITEM_DONUT:
    case SHOP_ITEM_COFFEE:
    case SHOP_ITEM_CHICKEN:
    case SHOP_ITEM_LEMONADE:
    case SHOP_ITEM_PRETZEL:
    case SHOP_ITEM_CHOCOLATE:
    case SHOP_ITEM_ICED_TEA:
    case SHOP_ITEM_FUNNEL_CAKE:
    case SHOP_ITEM_BEEF_NOODLES:
    case SHOP_ITEM_FRIED_RICE_NOODLES:
    case SHOP_ITEM_WONTON_SOUP:
    case SHOP_ITEM_MEATBALL_SOUP:
    case SHOP_ITEM_FRUIT_JUICE:
    case SHOP_ITEM_SOYBEAN_MILK:
    case SHOP_ITEM_SU_JONGKWA:
    case SHOP_ITEM_SUB_SANDWICH:
    case SHOP_ITEM_COOKIE:
    case SHOP_ITEM_ROAST_SAUSAGE:
        return true;
    default:
        return false;
    }
}

bool shop_item_is_food(sint32 shopItem)
{
    switch (shopItem) {
    case SHOP_ITEM_BURGER:
    case SHOP_ITEM_CHIPS:
    case SHOP_ITEM_ICE_CREAM:
    case SHOP_ITEM_CANDYFLOSS:
    case SHOP_ITEM_PIZZA:
    case SHOP_ITEM_POPCORN:
    case SHOP_ITEM_HOT_DOG:
    case SHOP_ITEM_TENTACLE:
    case SHOP_ITEM_CANDY_APPLE:
    case SHOP_ITEM_DONUT:
    case SHOP_ITEM_CHICKEN:
    case SHOP_ITEM_PRETZEL:
    case SHOP_ITEM_FUNNEL_CAKE:
    case SHOP_ITEM_BEEF_NOODLES:
    case SHOP_ITEM_FRIED_RICE_NOODLES:
    case SHOP_ITEM_WONTON_SOUP:
    case SHOP_ITEM_MEATBALL_SOUP:
    case SHOP_ITEM_SUB_SANDWICH:
    case SHOP_ITEM_COOKIE:
    case SHOP_ITEM_ROAST_SAUSAGE:
        return true;
    default:
        return false;
    }
}

bool shop_item_is_drink(sint32 shopItem)
{
    switch (shopItem) {
    case SHOP_ITEM_DRINK:
    case SHOP_ITEM_COFFEE:
    case SHOP_ITEM_LEMONADE:
    case SHOP_ITEM_CHOCOLATE:
    case SHOP_ITEM_ICED_TEA:
    case SHOP_ITEM_FRUIT_JUICE:
    case SHOP_ITEM_SOYBEAN_MILK:
    case SHOP_ITEM_SU_JONGKWA:
        return true;
    default:
        return false;
    }
}

bool shop_item_is_souvenir(sint32 shopItem)
{
    switch (shopItem) {
    case SHOP_ITEM_BALLOON:
    case SHOP_ITEM_TOY:
    case SHOP_ITEM_MAP:
    case SHOP_ITEM_PHOTO:
    case SHOP_ITEM_UMBRELLA:
    case SHOP_ITEM_HAT:
    case SHOP_ITEM_TSHIRT:
    case SHOP_ITEM_PHOTO2:
    case SHOP_ITEM_PHOTO3:
    case SHOP_ITEM_PHOTO4:
    case SHOP_ITEM_SUNGLASSES:
        return true;
    default:
        return false;
    }
}

void ride_reset_all_names()
{
    sint32 i;
    Ride *ride;
    char rideNameBuffer[256];
    ride_name_args name_args;

    FOR_ALL_RIDES(i, ride)
    {
        ride->name = STR_NONE;

        name_args.type_name = RideNaming[ride->type].name;
        name_args.number = 0;
        do {
            name_args.number++;
            format_string(rideNameBuffer, 256, 1, &name_args);
        } while (ride_name_exists(rideNameBuffer));

        ride->name = 1;
        ride->name_arguments_type_name = name_args.type_name;
        ride->name_arguments_number = name_args.number;
    }
}

const uint8* ride_seek_available_modes(Ride *ride)
{
    const uint8* availableModes;

    if (!gCheatsShowAllOperatingModes) {
        availableModes = RideAvailableModes;

        for (sint32 i = 0; i < ride->type; i++) {
            while (*(availableModes++) != 255) { }
        }
    }
    else
    {
        availableModes = AllRideModesAvailable;
    }

    return availableModes;
}

// Gets the approximate value of customers per hour for this ride. Multiplies ride_customers_in_last_5_minutes() by 12.
uint32 ride_customers_per_hour(const Ride *ride) {
    return ride_customers_in_last_5_minutes(ride) * 12;
}

// Calculates the number of customers for this ride in the last 5 minutes (or more correctly 9600 game ticks)
uint32 ride_customers_in_last_5_minutes(const Ride *ride)
{
    uint32 sum = 0;

    for (sint32 i = 0; i < CUSTOMER_HISTORY_SIZE; i++)
    {
        sum += ride->num_customers[i];
    }

    return sum;
}

rct_vehicle *ride_get_broken_vehicle(Ride *ride) {
    uint16 vehicleIndex = ride->vehicles[ride->broken_vehicle];

    if (vehicleIndex == SPRITE_INDEX_NULL) {
        return NULL;
    }

    rct_vehicle *vehicle = GET_VEHICLE(vehicleIndex);
    for (uint8 i = 0; i < ride->broken_car; i++) {
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    return vehicle;
}

/**
 *
 *  rct2: 0x006D235B
 */
void ride_delete(uint8 rideIndex)
{
    Ride *ride = get_ride(rideIndex);
    user_string_free(ride->name);
    ride->type = RIDE_TYPE_NULL;
}

static bool ride_is_ride(Ride * ride)
{
    switch (ride->type) {
    case RIDE_TYPE_FOOD_STALL:
    case RIDE_TYPE_1D:
    case RIDE_TYPE_DRINK_STALL:
    case RIDE_TYPE_1F:
    case RIDE_TYPE_SHOP:
    case RIDE_TYPE_22:
    case RIDE_TYPE_INFORMATION_KIOSK:
    case RIDE_TYPE_TOILETS:
    case RIDE_TYPE_CASH_MACHINE:
    case RIDE_TYPE_FIRST_AID:
        return false;
    default:
        return true;
    }
}

money16 ride_get_price(Ride * ride)
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY) return 0;
    if (ride_is_ride(ride)) {
        if (!gCheatsUnlockAllPrices) {
            if (!(gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY)) return 0;
        }
    }
    return ride->price;
}

/**
 * Return the map_element of an adjacent station at x,y,z(+-2).
 * Returns NULL if no suitable map_element is found.
 */
rct_map_element *get_station_platform(sint32 x, sint32 y, sint32 z, sint32 z_tolerance) {
    bool foundMapElement = false;
    rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
    if (mapElement != NULL) {
        do {
            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;
            /* Check if mapElement is a station platform. */
            if (!track_element_is_station(mapElement)) continue;

            if (z - z_tolerance > mapElement->base_height ||
                z + z_tolerance < mapElement->base_height
            ) {
                /* The base height if mapElement is not within
                 * the z tolerance. */
                continue;
            }

            foundMapElement = true;
            break;
        } while (!map_element_is_last_for_tile(mapElement++));
    }
    if (!foundMapElement) {
        return NULL;
    }

    return mapElement;
}

/**
 * Check for an adjacent station to x,y,z in direction.
 */
static bool check_for_adjacent_station(sint32 x, sint32 y, sint32 z, uint8 direction) {
    bool found = false;
    sint32 adjX = x + TileDirectionDelta[direction].x;
    sint32 adjY = y + TileDirectionDelta[direction].y;
    rct_map_element *stationElement = get_station_platform(adjX, adjY, z, 2);
    if (stationElement != NULL) {
        sint32 rideIndex = stationElement->properties.track.ride_index;
        Ride *ride = get_ride(rideIndex);
        if (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS) {
            found = true;
        }
    }
    return found;
}

/**
 * Return whether ride has at least one adjacent station to it.
 */
bool ride_has_adjacent_station(Ride *ride)
{
    bool found = false;

    /* Loop through all of the ride stations, checking for an
     * adjacent station on either side. */
    for (sint32 stationNum = 0; stationNum < MAX_STATIONS; stationNum++) {
        if (ride->station_starts[stationNum].xy != RCT_XY8_UNDEFINED) {
            /* Get the map element for the station start. */
            uint16 stationX = ride->station_starts[stationNum].x * 32;
            uint16 stationY = ride->station_starts[stationNum].y * 32;
            uint8 stationZ = ride->station_heights[stationNum];

            rct_map_element *stationElement = get_station_platform(stationX, stationY, stationZ, 0);
            if (stationElement == NULL) {
                continue;
            }
            /* Check the first side of the station */
            sint32 direction = map_element_get_direction_with_offset(stationElement, 1);
            found = check_for_adjacent_station(stationX, stationY, stationZ, direction);
            if (found) break;
            /* Check the other side of the station */
            direction ^= 2;
            found = check_for_adjacent_station(stationX, stationY, stationZ, direction);
            if (found) break;
        }
    }
    return found;
}

bool ride_has_ratings(const Ride * ride)
{
    return ride->excitement != RIDE_RATING_UNDEFINED;
}

const char * ride_type_get_enum_name(sint32 rideType)
{
    static const char * RideTypeEnumNames[RIDE_TYPE_COUNT] =
    {
        nameof(RIDE_TYPE_SPIRAL_ROLLER_COASTER),
        nameof(RIDE_TYPE_STAND_UP_ROLLER_COASTER),
        nameof(RIDE_TYPE_SUSPENDED_SWINGING_COASTER),
        nameof(RIDE_TYPE_INVERTED_ROLLER_COASTER),
        nameof(RIDE_TYPE_JUNIOR_ROLLER_COASTER),
        nameof(RIDE_TYPE_MINIATURE_RAILWAY),
        nameof(RIDE_TYPE_MONORAIL),
        nameof(RIDE_TYPE_MINI_SUSPENDED_COASTER),
        nameof(RIDE_TYPE_BOAT_RIDE),
        nameof(RIDE_TYPE_WOODEN_WILD_MOUSE),
        nameof(RIDE_TYPE_STEEPLECHASE),
        nameof(RIDE_TYPE_CAR_RIDE),
        nameof(RIDE_TYPE_LAUNCHED_FREEFALL),
        nameof(RIDE_TYPE_BOBSLEIGH_COASTER),
        nameof(RIDE_TYPE_OBSERVATION_TOWER),
        nameof(RIDE_TYPE_LOOPING_ROLLER_COASTER),
        nameof(RIDE_TYPE_DINGHY_SLIDE),
        nameof(RIDE_TYPE_MINE_TRAIN_COASTER),
        nameof(RIDE_TYPE_CHAIRLIFT),
        nameof(RIDE_TYPE_CORKSCREW_ROLLER_COASTER),
        nameof(RIDE_TYPE_MAZE),
        nameof(RIDE_TYPE_SPIRAL_SLIDE),
        nameof(RIDE_TYPE_GO_KARTS),
        nameof(RIDE_TYPE_LOG_FLUME),
        nameof(RIDE_TYPE_RIVER_RAPIDS),
        nameof(RIDE_TYPE_DODGEMS),
        nameof(RIDE_TYPE_PIRATE_SHIP),
        nameof(RIDE_TYPE_SWINGING_INVERTER_SHIP),
        nameof(RIDE_TYPE_FOOD_STALL),
        nameof(RIDE_TYPE_1D),
        nameof(RIDE_TYPE_DRINK_STALL),
        nameof(RIDE_TYPE_1F),
        nameof(RIDE_TYPE_SHOP),
        nameof(RIDE_TYPE_MERRY_GO_ROUND),
        nameof(RIDE_TYPE_22),
        nameof(RIDE_TYPE_INFORMATION_KIOSK),
        nameof(RIDE_TYPE_TOILETS),
        nameof(RIDE_TYPE_FERRIS_WHEEL),
        nameof(RIDE_TYPE_MOTION_SIMULATOR),
        nameof(RIDE_TYPE_3D_CINEMA),
        nameof(RIDE_TYPE_TOP_SPIN),
        nameof(RIDE_TYPE_SPACE_RINGS),
        nameof(RIDE_TYPE_REVERSE_FREEFALL_COASTER),
        nameof(RIDE_TYPE_LIFT),
        nameof(RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER),
        nameof(RIDE_TYPE_CASH_MACHINE),
        nameof(RIDE_TYPE_TWIST),
        nameof(RIDE_TYPE_HAUNTED_HOUSE),
        nameof(RIDE_TYPE_FIRST_AID),
        nameof(RIDE_TYPE_CIRCUS_SHOW),
        nameof(RIDE_TYPE_GHOST_TRAIN),
        nameof(RIDE_TYPE_TWISTER_ROLLER_COASTER),
        nameof(RIDE_TYPE_WOODEN_ROLLER_COASTER),
        nameof(RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER),
        nameof(RIDE_TYPE_WILD_MOUSE),
        nameof(RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER),
        nameof(RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT),
        nameof(RIDE_TYPE_FLYING_ROLLER_COASTER),
        nameof(RIDE_TYPE_FLYING_ROLLER_COASTER_ALT),
        nameof(RIDE_TYPE_VIRGINIA_REEL),
        nameof(RIDE_TYPE_SPLASH_BOATS),
        nameof(RIDE_TYPE_MINI_HELICOPTERS),
        nameof(RIDE_TYPE_LAY_DOWN_ROLLER_COASTER),
        nameof(RIDE_TYPE_SUSPENDED_MONORAIL),
        nameof(RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT),
        nameof(RIDE_TYPE_REVERSER_ROLLER_COASTER),
        nameof(RIDE_TYPE_HEARTLINE_TWISTER_COASTER),
        nameof(RIDE_TYPE_MINI_GOLF),
        nameof(RIDE_TYPE_GIGA_COASTER),
        nameof(RIDE_TYPE_ROTO_DROP),
        nameof(RIDE_TYPE_FLYING_SAUCERS),
        nameof(RIDE_TYPE_CROOKED_HOUSE),
        nameof(RIDE_TYPE_MONORAIL_CYCLES),
        nameof(RIDE_TYPE_COMPACT_INVERTED_COASTER),
        nameof(RIDE_TYPE_WATER_COASTER),
        nameof(RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER),
        nameof(RIDE_TYPE_INVERTED_HAIRPIN_COASTER),
        nameof(RIDE_TYPE_MAGIC_CARPET),
        nameof(RIDE_TYPE_SUBMARINE_RIDE),
        nameof(RIDE_TYPE_RIVER_RAFTS),
        nameof(RIDE_TYPE_50),
        nameof(RIDE_TYPE_ENTERPRISE),
        nameof(RIDE_TYPE_52),
        nameof(RIDE_TYPE_53),
        nameof(RIDE_TYPE_54),
        nameof(RIDE_TYPE_55),
        nameof(RIDE_TYPE_INVERTED_IMPULSE_COASTER),
        nameof(RIDE_TYPE_MINI_ROLLER_COASTER),
        nameof(RIDE_TYPE_MINE_RIDE),
        nameof(RIDE_TYPE_59),
        nameof(RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER),
    };

    return RideTypeEnumNames[rideType];
}

/**
 *  Searches for a non-null ride type in a ride entry.
 *  If none is found, it will still return RIDE_TYPE_NULL.
 */
uint8 ride_entry_get_first_non_null_ride_type(rct_ride_entry * rideEntry)
{
    for (uint8 i = 0; i < MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++)
    {
        if (rideEntry->ride_type[i] != RIDE_TYPE_NULL)
        {
            return rideEntry->ride_type[i];
        }
    }
   return RIDE_TYPE_NULL;
}

bool ride_type_supports_boosters(uint8 rideType)
{
    if (rideType == RIDE_TYPE_LOOPING_ROLLER_COASTER ||
        rideType == RIDE_TYPE_CORKSCREW_ROLLER_COASTER ||
        rideType == RIDE_TYPE_TWISTER_ROLLER_COASTER ||
        rideType == RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER ||
        rideType == RIDE_TYPE_GIGA_COASTER ||
        rideType == RIDE_TYPE_JUNIOR_ROLLER_COASTER)
    {
        return true;
    }
    return false;
}

sint32 get_booster_speed(uint8 rideType, sint32 rawSpeed)
{
    sint8 shiftFactor = RideProperties[rideType].booster_speed_factor;
    if (shiftFactor == 0)
    {
        return rawSpeed;
    }
    else if (shiftFactor > 0)
    {
        return (rawSpeed << shiftFactor);
    }
    else
    {
        return (rawSpeed >> abs(shiftFactor));
    }
}

void fix_invalid_vehicle_sprite_sizes()
{
    Ride *ride;
    uint16 i;
    FOR_ALL_RIDES(i, ride)
    {
        for (uint16 j = 0; j < MAX_VEHICLES_PER_RIDE; j++) {
            uint16 rideSpriteIndex = ride->vehicles[j];
            while (rideSpriteIndex != SPRITE_INDEX_NULL) {
                rct_vehicle * vehicle = try_get_vehicle(rideSpriteIndex);
                if (vehicle == NULL) {
                    break;
                }

                rct_ride_entry_vehicle * vehicleEntry = vehicle_get_vehicle_entry(vehicle);
                if (vehicleEntry == NULL || vehicleEntry == (rct_ride_entry_vehicle*)-1) {
                    break;
                }

                if (vehicle->sprite_width == 0)
                {
                    vehicle->sprite_width = vehicleEntry->sprite_width;
                }
                if (vehicle->sprite_height_negative == 0)
                {
                    vehicle->sprite_height_negative = vehicleEntry->sprite_height_negative;
                }
                if (vehicle->sprite_height_positive == 0)
                {
                    vehicle->sprite_height_positive = vehicleEntry->sprite_height_positive;
                }
                rideSpriteIndex = vehicle->next_vehicle_on_train;
            }
        }
    }
}

bool ride_entry_has_category(const rct_ride_entry * rideEntry, uint8 category)
{
    for (sint32 i = 0; i < MAX_CATEGORIES_PER_RIDE; i++)
    {
        if (rideEntry->category[i] == category)
        {
            return true;
        }
    }

    return false;
}
