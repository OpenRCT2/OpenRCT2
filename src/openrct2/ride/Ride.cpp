/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Ride.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Editor.h"
#include "../Game.h"
#include "../Input.h"
#include "../OpenRCT2.h"
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../common.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../network/network.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/StationObject.h"
#include "../paint/VirtualFloor.h"
#include "../peep/Peep.h"
#include "../peep/Staff.h"
#include "../rct1/RCT1.h"
#include "../scenario/Scenario.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "../world/Climate.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "CableLift.h"
#include "MusicList.h"
#include "RideData.h"
#include "RideGroupManager.h"
#include "ShopItem.h"
#include "Station.h"
#include "Track.h"
#include "TrackData.h"

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdlib>
#include <iterator>
#include <limits>

using namespace OpenRCT2;

#pragma region Ride classification table

/** rct2: 0x0097C3AF */
// clang-format off
const uint8_t gRideClassifications[255] = {
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
// clang-format on

uint8_t gTypeToRideEntryIndexMap[TYPE_TO_RIDE_ENTRY_SLOTS];

#pragma endregion

static constexpr const int32_t RideInspectionInterval[] = {
    10, 20, 30, 45, 60, 120, 0, 0,
};

Ride gRideList[MAX_RIDES];

rct_ride_measurement gRideMeasurements[MAX_RIDE_MEASUREMENTS];

uint16_t gRideCount;
bool gGotoStartPlacementMode = false;
int32_t gRideRemoveTrackPieceCallbackX;
int32_t gRideRemoveTrackPieceCallbackY;
int32_t gRideRemoveTrackPieceCallbackZ;
int32_t gRideRemoveTrackPieceCallbackDirection;
int32_t gRideRemoveTrackPieceCallbackType;

money16 gTotalRideValueForMoney;

money32 _currentTrackPrice;

uint16_t _numCurrentPossibleRideConfigurations;
uint16_t _numCurrentPossibleSpecialTrackPieces;

uint16_t _currentTrackCurve;
uint16_t _currentTrackEndX;
uint16_t _currentTrackEndY;
uint8_t _rideConstructionState;
ride_id_t _currentRideIndex;
uint16_t _currentTrackBeginX;
uint16_t _currentTrackBeginY;
uint16_t _currentTrackBeginZ;
uint8_t _currentTrackPieceDirection;
uint8_t _currentTrackPieceType;
uint8_t _currentTrackSelectionFlags;
int8_t _rideConstructionArrowPulseTime;
uint8_t _currentTrackSlopeEnd;
uint8_t _currentTrackBankEnd;
uint8_t _currentTrackLiftHill;
uint8_t _currentTrackAlternative;
uint8_t _selectedTrackType;

uint8_t _previousTrackBankEnd;
uint8_t _previousTrackSlopeEnd;

uint16_t _previousTrackPieceX;
uint16_t _previousTrackPieceY;
uint16_t _previousTrackPieceZ;

uint8_t _currentBrakeSpeed2;
uint8_t _currentSeatRotationAngle;

LocationXYZ16 _unkF44188;

CoordsXYZD _unkF440C5;

uint8_t gRideEntranceExitPlaceType;
ride_id_t gRideEntranceExitPlaceRideIndex;
uint8_t gRideEntranceExitPlaceStationIndex;
uint8_t gRideEntranceExitPlacePreviousRideConstructionState;
uint8_t gRideEntranceExitPlaceDirection;

uint8_t gLastEntranceStyle;

// Static function declarations
rct_peep* find_closest_mechanic(int32_t x, int32_t y, int32_t forInspection);
static void ride_breakdown_status_update(Ride* ride);
static void ride_breakdown_update(Ride* ride);
static void ride_call_closest_mechanic(Ride* ride);
static void ride_call_mechanic(Ride* ride, rct_peep* mechanic, int32_t forInspection);
static void ride_chairlift_update(Ride* ride);
static void ride_entrance_exit_connected(Ride* ride);
static void ride_set_name_to_vehicle_default(Ride* ride, rct_ride_entry* rideEntry);
static int32_t ride_get_new_breakdown_problem(Ride* ride);
static void ride_inspection_update(Ride* ride);
static void ride_mechanic_status_update(Ride* ride, int32_t mechanicStatus);
static void ride_music_update(Ride* ride);
static void ride_shop_connected(Ride* ride);
static void ride_spiral_slide_update(Ride* ride);
static void ride_update(Ride* ride);
void loc_6DDF9C(Ride* ride, TileElement* tileElement);

Ride* get_ride(int32_t index)
{
    if (index < 0 || index >= MAX_RIDES)
    {
        log_error("invalid index %d for ride", index);
        return nullptr;
    }
    auto ride = &gRideList[index];
#ifdef DEBUG
    assert(ride->id == index);
#endif
    return ride;
}

rct_ride_entry* get_ride_entry(int32_t index)
{
    rct_ride_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();

    auto obj = objMgr.GetLoadedObject(OBJECT_TYPE_RIDE, index);
    if (obj != nullptr)
    {
        result = (rct_ride_entry*)obj->GetLegacyData();
    }

    return result;
}

void get_ride_entry_name(char* name, int32_t index)
{
    if (index < 0 || index >= object_entry_group_counts[OBJECT_TYPE_RIDE])
    {
        log_error("invalid index %d for ride type", index);
        return;
    }

    const auto entryName = object_entry_get_entry(OBJECT_TYPE_RIDE, index)->name;
    std::memcpy(name, entryName, 8);
    name[8] = '\0';
}

rct_ride_measurement* get_ride_measurement(int32_t index)
{
    return &gRideMeasurements[index];
}

rct_ride_entry* get_ride_entry_by_ride(const Ride* ride)
{
    rct_ride_entry* type = get_ride_entry(ride->subtype);
    if (type == nullptr)
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
void reset_type_to_ride_entry_index_map(IObjectManager& objectManager)
{
    size_t stride = MAX_RIDE_OBJECTS + 1;
    uint8_t* entryTypeTable = (uint8_t*)malloc(RIDE_TYPE_COUNT * stride);
    std::fill_n(entryTypeTable, RIDE_TYPE_COUNT * stride, 0xFF);

    for (uint8_t i = 0; i < MAX_RIDE_OBJECTS; i++)
    {
        auto obj = objectManager.GetLoadedObject(OBJECT_TYPE_RIDE, i);
        if (obj != nullptr)
        {
            for (uint8_t j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++)
            {
                auto rideEntry = (rct_ride_entry*)obj->GetLegacyData();
                uint8_t rideType = rideEntry->ride_type[j];
                if (rideType < RIDE_TYPE_COUNT)
                {
                    uint8_t* entryArray = &entryTypeTable[rideType * stride];
                    uint8_t* nextEntry = (uint8_t*)memchr(entryArray, 0xFF, stride);
                    *nextEntry = i;
                }
            }
        }
    }

    uint8_t* dst = gTypeToRideEntryIndexMap;
    for (uint8_t i = 0; i < RIDE_TYPE_COUNT; i++)
    {
        uint8_t* entryArray = &entryTypeTable[i * stride];
        uint8_t* entry = entryArray;
        while (*entry != 0xFF)
        {
            *dst++ = *entry++;
        }
        *dst++ = 0xFF;
    }

    free(entryTypeTable);
}

uint8_t* get_ride_entry_indices_for_ride_type(uint8_t rideType)
{
    uint8_t* entryIndexList = gTypeToRideEntryIndexMap;
    while (rideType > 0)
    {
        do
        {
            entryIndexList++;
        } while (*(entryIndexList - 1) != RIDE_ENTRY_INDEX_NULL);
        rideType--;
    }
    return entryIndexList;
}

int32_t ride_get_count()
{
    Ride* ride;
    int32_t i, count = 0;

    FOR_ALL_RIDES (i, ride)
        count++;

    return count;
}

int32_t ride_get_total_queue_length(Ride* ride)
{
    int32_t i, queueLength = 0;
    for (i = 0; i < MAX_STATIONS; i++)
        if (!ride_get_entrance_location(ride, i).isNull())
            queueLength += ride->stations[i].QueueLength;
    return queueLength;
}

int32_t ride_get_max_queue_time(Ride* ride)
{
    uint8_t i, queueTime = 0;
    for (i = 0; i < MAX_STATIONS; i++)
        if (!ride_get_entrance_location(ride, i).isNull())
            queueTime = std::max(queueTime, ride->stations[i].QueueTime);
    return (int32_t)queueTime;
}

rct_peep* ride_get_queue_head_guest(Ride* ride, int32_t stationIndex)
{
    rct_peep* peep;
    rct_peep* result = nullptr;
    uint16_t spriteIndex = ride->stations[stationIndex].LastPeepInQueue;
    while ((peep = try_get_guest(spriteIndex)) != nullptr)
    {
        spriteIndex = peep->next_in_queue;
        result = peep;
    }
    return result;
}

static void ride_update_queue_length(Ride* ride, int32_t stationIndex)
{
    uint16_t count = 0;
    rct_peep* peep;
    uint16_t spriteIndex = ride->stations[stationIndex].LastPeepInQueue;
    while ((peep = try_get_guest(spriteIndex)) != nullptr)
    {
        spriteIndex = peep->next_in_queue;
        count++;
    }
    ride->stations[stationIndex].QueueLength = count;
}

void ride_queue_insert_guest_at_front(Ride* ride, int32_t stationIndex, rct_peep* peep)
{
    assert(ride != nullptr);
    assert(stationIndex < MAX_STATIONS);
    assert(peep != nullptr);

    peep->next_in_queue = SPRITE_INDEX_NULL;
    rct_peep* queueHeadGuest = ride_get_queue_head_guest(ride, peep->current_ride_station);
    if (queueHeadGuest == nullptr)
    {
        ride->stations[peep->current_ride_station].LastPeepInQueue = peep->sprite_index;
    }
    else
    {
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
    int32_t i;
    Ride* ride;
    uint16_t spriteIndex;
    rct_peep* peep;

    FOR_ALL_RIDES (i, ride)
        ride->guests_favourite = 0;

    FOR_ALL_PEEPS (spriteIndex, peep)
    {
        if (peep->linked_list_type_offset != SPRITE_LIST_PEEP * 2)
            return;
        if (peep->favourite_ride != RIDE_ID_NULL)
        {
            ride = get_ride(peep->favourite_ride);
            ride->guests_favourite++;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
        }
    }

    window_invalidate_by_class(WC_RIDE_LIST);
}

/**
 *
 *  rct2: 0x006AC3AB
 */
static money32 ride_calculate_income_per_hour(Ride* ride)
{
    // Get entry by ride to provide better reporting
    rct_ride_entry* entry = get_ride_entry_by_ride(ride);
    if (entry == nullptr)
    {
        return 0;
    }
    money32 customersPerHour = ride_customers_per_hour(ride);
    money32 priceMinusCost = ride_get_price(ride);

    int32_t currentShopItem = entry->shop_item;
    if (currentShopItem != SHOP_ITEM_NONE)
    {
        priceMinusCost -= get_shop_item_cost(currentShopItem);
    }

    currentShopItem = (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) ? RidePhotoItems[ride->type]
                                                                             : entry->shop_item_secondary;

    if (currentShopItem != SHOP_ITEM_NONE)
    {
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
bool ride_try_get_origin_element(ride_id_t rideIndex, CoordsXYE* output)
{
    TileElement* resultTileElement = nullptr;

    tile_element_iterator it;
    tile_element_iterator_begin(&it);
    do
    {
        if (it.element->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (rideIndex != it.element->AsTrack()->GetRideIndex())
            continue;

        // Found a track piece for target ride

        // Check if it's not the station or ??? (but allow end piece of station)
        bool specialTrackPiece
            = (it.element->AsTrack()->GetTrackType() != TRACK_ELEM_BEGIN_STATION
               && it.element->AsTrack()->GetTrackType() != TRACK_ELEM_MIDDLE_STATION
               && (TrackSequenceProperties[it.element->AsTrack()->GetTrackType()][0] & TRACK_SEQUENCE_FLAG_ORIGIN));

        // Set result tile to this track piece if first found track or a ???
        if (resultTileElement == nullptr || specialTrackPiece)
        {
            resultTileElement = it.element;

            if (output != nullptr)
            {
                output->element = resultTileElement;
                output->x = it.x * 32;
                output->y = it.y * 32;
            }
        }

        if (specialTrackPiece)
        {
            return true;
        }
    } while (tile_element_iterator_next(&it));

    return resultTileElement != nullptr;
}

/**
 *
 * rct2: 0x006C6096
 * Gets the next track block coordinates from the
 * coordinates of the first of element of a track block.
 * Use track_block_get_next if you are unsure if you are
 * on the first element of a track block
 */
bool track_block_get_next_from_zero(
    int16_t x, int16_t y, int16_t z_start, ride_id_t rideIndex, uint8_t direction_start, CoordsXYE* output, int32_t* z,
    int32_t* direction, bool isGhost)
{
    Ride* ride = get_ride(rideIndex);

    if (!(direction_start & (1 << 2)))
    {
        x += CoordsDirectionDelta[direction_start].x;
        y += CoordsDirectionDelta[direction_start].y;
    }

    TileElement* tileElement = map_get_first_element_at(x / 32, y / 32);
    if (tileElement == nullptr)
    {
        output->element = nullptr;
        output->x = LOCATION_NULL;
        return false;
    }

    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;

        if (tileElement->AsTrack()->GetSequenceIndex() != 0)
            continue;

        if (tileElement->IsGhost() != isGhost)
            continue;

        const rct_preview_track* nextTrackBlock = get_track_def_from_ride(ride, tileElement->AsTrack()->GetTrackType());
        const rct_track_coordinates* nextTrackCoordinate = get_track_coord_from_ride(
            ride, tileElement->AsTrack()->GetTrackType());

        uint8_t nextRotation = tileElement->GetDirectionWithOffset(nextTrackCoordinate->rotation_begin)
            | (nextTrackCoordinate->rotation_begin & (1 << 2));

        if (nextRotation != direction_start)
            continue;

        int16_t nextZ = nextTrackCoordinate->z_begin - nextTrackBlock->z + tileElement->base_height * 8;
        if (nextZ != z_start)
            continue;

        if (z != nullptr)
            *z = tileElement->base_height * 8;
        if (direction != nullptr)
            *direction = nextRotation;
        output->x = x;
        output->y = y;
        output->element = tileElement;
        return true;
    } while (!(tileElement++)->IsLastForTile());

    if (direction != nullptr)
        *direction = direction_start;
    if (z != nullptr)
        *z = z_start;
    output->x = x;
    output->y = y;
    output->element = --tileElement;
    return false;
}

/**
 *
 *  rct2: 0x006C60C2
 */
bool track_block_get_next(CoordsXYE* input, CoordsXYE* output, int32_t* z, int32_t* direction)
{
    ride_id_t rideIndex = input->element->AsTrack()->GetRideIndex();
    Ride* ride = get_ride(rideIndex);

    const rct_preview_track* trackBlock = get_track_def_from_ride(ride, input->element->AsTrack()->GetTrackType());
    uint8_t sequence = input->element->AsTrack()->GetSequenceIndex();
    trackBlock += sequence;

    const rct_track_coordinates* trackCoordinate = get_track_coord_from_ride(ride, input->element->AsTrack()->GetTrackType());

    int32_t x = input->x;
    int32_t y = input->y;
    int32_t OriginZ = input->element->base_height * 8;

    uint8_t rotation = input->element->GetDirection();
    switch (rotation)
    {
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

    uint8_t directionStart = ((trackCoordinate->rotation_end + rotation) & TILE_ELEMENT_DIRECTION_MASK)
        | (trackCoordinate->rotation_end & (1 << 2));

    return track_block_get_next_from_zero(x, y, OriginZ, rideIndex, directionStart, output, z, direction, false);
}

/**
 * Returns the begin position / direction and end position / direction of the
 * track piece that proceeds the given location. Gets the previous track block
 * coordinates from the coordinates of the first of element of a track block.
 * Use track_block_get_previous if you are unsure if you are on the first
 * element of a track block
 *  rct2: 0x006C63D6
 */
bool track_block_get_previous_from_zero(
    int16_t x, int16_t y, int16_t z, ride_id_t rideIndex, uint8_t direction, track_begin_end* outTrackBeginEnd)
{
    Ride* ride = get_ride(rideIndex);

    uint8_t directionStart = direction;
    direction = direction_reverse(direction);

    if (!(direction & (1 << 2)))
    {
        x += CoordsDirectionDelta[direction].x;
        y += CoordsDirectionDelta[direction].y;
    }

    TileElement* tileElement = map_get_first_element_at(x / 32, y / 32);
    if (tileElement == nullptr)
    {
        outTrackBeginEnd->end_x = x;
        outTrackBeginEnd->end_y = y;
        outTrackBeginEnd->begin_element = nullptr;
        outTrackBeginEnd->begin_direction = direction_reverse(directionStart);
        return 0;
    }

    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;

        const rct_preview_track* nextTrackBlock = get_track_def_from_ride(ride, tileElement->AsTrack()->GetTrackType());
        const rct_track_coordinates* nextTrackCoordinate = get_track_coord_from_ride(
            ride, tileElement->AsTrack()->GetTrackType());

        nextTrackBlock += tileElement->AsTrack()->GetSequenceIndex();
        if ((nextTrackBlock + 1)->index != 255)
            continue;

        uint8_t nextRotation = tileElement->GetDirectionWithOffset(nextTrackCoordinate->rotation_end)
            | (nextTrackCoordinate->rotation_end & (1 << 2));

        if (nextRotation != directionStart)
            continue;

        int16_t nextZ = nextTrackCoordinate->z_end - nextTrackBlock->z + tileElement->base_height * 8;
        if (nextZ != z)
            continue;

        nextRotation = tileElement->GetDirectionWithOffset(nextTrackCoordinate->rotation_begin)
            | (nextTrackCoordinate->rotation_begin & (1 << 2));
        outTrackBeginEnd->begin_element = tileElement;
        outTrackBeginEnd->begin_x = x;
        outTrackBeginEnd->begin_y = y;
        outTrackBeginEnd->end_x = x;
        outTrackBeginEnd->end_y = y;
        switch (nextRotation & 3)
        {
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

        outTrackBeginEnd->begin_z = tileElement->base_height * 8;
        outTrackBeginEnd->begin_z += get_track_def_from_ride(ride, tileElement->AsTrack()->GetTrackType())->z
            - nextTrackBlock->z;

        outTrackBeginEnd->begin_direction = nextRotation;
        outTrackBeginEnd->end_direction = direction_reverse(directionStart);
        return 1;
    } while (!(tileElement++)->IsLastForTile());

    outTrackBeginEnd->end_x = x;
    outTrackBeginEnd->end_y = y;
    outTrackBeginEnd->begin_z = z;
    outTrackBeginEnd->begin_element = nullptr;
    outTrackBeginEnd->end_direction = direction_reverse(directionStart);
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
bool track_block_get_previous(int32_t x, int32_t y, TileElement* tileElement, track_begin_end* outTrackBeginEnd)
{
    ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
    Ride* ride = get_ride(rideIndex);

    const rct_preview_track* trackBlock = get_track_def_from_ride(ride, tileElement->AsTrack()->GetTrackType());
    uint8_t sequence = tileElement->AsTrack()->GetSequenceIndex();
    trackBlock += sequence;

    const rct_track_coordinates* trackCoordinate = get_track_coord_from_ride(ride, tileElement->AsTrack()->GetTrackType());

    int32_t z = tileElement->base_height * 8;

    uint8_t rotation = tileElement->GetDirection();
    switch (rotation)
    {
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

    rotation = ((trackCoordinate->rotation_begin + rotation) & TILE_ELEMENT_DIRECTION_MASK)
        | (trackCoordinate->rotation_begin & (1 << 2));

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
int32_t ride_find_track_gap(ride_id_t rideIndex, CoordsXYE* input, CoordsXYE* output)
{
    assert(input->element->GetType() == TILE_ELEMENT_TYPE_TRACK);

    Ride* ride = get_ride(rideIndex);
    if (ride == nullptr)
    {
        log_error("Trying to access invalid ride %d", rideIndex);
        return 0;
    }

    if (ride->type == RIDE_TYPE_MAZE)
    {
        return 0;
    }

    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != nullptr && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && _currentRideIndex == rideIndex)
    {
        ride_construction_invalidate_current_track();
    }

    bool moveSlowIt = true;
    track_circuit_iterator it = {};
    track_circuit_iterator_begin(&it, *input);
    track_circuit_iterator slowIt = it;
    while (track_circuit_iterator_next(&it))
    {
        if (!track_is_connected_by_shape(it.last.element, it.current.element))
        {
            *output = it.current;
            return 1;
        }
        //#2081: prevent an infinite loop
        moveSlowIt = !moveSlowIt;
        if (moveSlowIt)
        {
            track_circuit_iterator_next(&slowIt);
            if (track_circuit_iterators_match(&it, &slowIt))
            {
                *output = it.current;
                return 1;
            }
        }
    }
    if (!it.looped)
    {
        *output = it.last;
        return 1;
    }

    return 0;
}

/**
 *
 *  rct2: 0x006AF561
 */
void ride_get_status(ride_id_t rideIndex, rct_string_id* formatSecondary, int32_t* argument)
{
    Ride* ride = get_ride(rideIndex);

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED)
    {
        *formatSecondary = STR_CRASHED;
        return;
    }
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        *formatSecondary = STR_BROKEN_DOWN;
        return;
    }
    if (ride->status == RIDE_STATUS_CLOSED)
    {
        *formatSecondary = STR_CLOSED;

        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
        {
            *argument = ride->num_riders;

            if (*argument == 1)
            {
                *formatSecondary = STR_CLOSED_WITH_PERSON;
            }
            else if (*argument > 1)
            {
                *formatSecondary = STR_CLOSED_WITH_PEOPLE;
            }
        }

        return;
    }
    if (ride->status == RIDE_STATUS_TESTING)
    {
        *formatSecondary = STR_TEST_RUN;
        return;
    }
    if (ride->mode == RIDE_MODE_RACE && !(ride->lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING)
        && ride->race_winner != SPRITE_INDEX_NULL && (GET_PEEP(ride->race_winner))->sprite_identifier == SPRITE_IDENTIFIER_PEEP)
    {
        rct_peep* peep = GET_PEEP(ride->race_winner);
        if (peep->name_string_idx == STR_GUEST_X)
        {
            *argument = peep->id;
            *formatSecondary = STR_RACE_WON_BY_GUEST;
        }
        else
        {
            *argument = peep->name_string_idx;
            *formatSecondary = STR_RACE_WON_BY;
        }
    }
    else
    {
        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
        {
            *argument = ride->num_riders;
            *formatSecondary = STR_PERSON_ON_RIDE;
            if (*argument != 1)
                *formatSecondary = STR_PEOPLE_ON_RIDE;
        }
        else
        {
            *formatSecondary = STR_OPEN;
        }
    }
}

int32_t ride_get_total_length(Ride* ride)
{
    int32_t i, totalLength = 0;
    for (i = 0; i < ride->num_stations; i++)
        totalLength += ride->stations[i].SegmentLength;
    return totalLength;
}

int32_t ride_get_total_time(Ride* ride)
{
    int32_t i, totalTime = 0;
    for (i = 0; i < ride->num_stations; i++)
        totalTime += ride->stations[i].SegmentTime;
    return totalTime;
}

int32_t ride_can_have_multiple_circuits(Ride* ride)
{
    if (!(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_MULTIPLE_CIRCUITS))
        return 0;

    // Only allow circuit or launch modes
    if (ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT && ride->mode != RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE
        && ride->mode != RIDE_MODE_POWERED_LAUNCH_PASSTROUGH)
    {
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
    for (int32_t i = 0; i < MAX_RIDES; i++)
    {
        auto ride = &gRideList[i];
        *ride = {};
        ride->id = i;
        ride->type = RIDE_TYPE_NULL;
    }

    for (int32_t i = 0; i < MAX_RIDE_MEASUREMENTS; i++)
    {
        rct_ride_measurement* ride_measurement = get_ride_measurement(i);
        ride_measurement->ride_index = RIDE_ID_NULL;
    }
}

/**
 *
 *  rct2: 0x006B7A38
 */
void reset_all_ride_build_dates()
{
    int32_t i;
    Ride* ride;

    FOR_ALL_RIDES (i, ride)
        ride->build_date -= gDateMonthsElapsed;
}

#pragma endregion

#pragma region Construction

static int32_t ride_check_if_construction_allowed(Ride* ride)
{
    rct_ride_entry* rideEntry = get_ride_entry_by_ride(ride);
    if (rideEntry == nullptr)
    {
        context_show_error(STR_INVALID_RIDE_TYPE, STR_CANT_EDIT_INVALID_RIDE_TYPE);
        return 0;
    }
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32_t, ride->name_arguments);
        context_show_error(STR_CANT_START_CONSTRUCTION_ON, STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING);
        return 0;
    }

    if (ride->status != RIDE_STATUS_CLOSED)
    {
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32_t, ride->name_arguments);
        context_show_error(STR_CANT_START_CONSTRUCTION_ON, STR_MUST_BE_CLOSED_FIRST);
        return 0;
    }

    return 1;
}

static rct_window* ride_create_or_find_construction_window(ride_id_t rideIndex)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    auto intent = Intent(INTENT_ACTION_RIDE_CONSTRUCTION_FOCUS);
    intent.putExtra(INTENT_EXTRA_RIDE_ID, rideIndex);
    windowManager->BroadcastIntent(intent);
    return window_find_by_class(WC_RIDE_CONSTRUCTION);
}

/**
 *
 *  rct2: 0x006B4857
 */
void ride_construct(ride_id_t rideIndex)
{
    CoordsXYE trackElement;

    if (ride_try_get_origin_element(rideIndex, &trackElement))
    {
        ride_find_track_gap(rideIndex, &trackElement, &trackElement);

        rct_window* w = window_get_main();
        if (w != nullptr && ride_modify(&trackElement))
            window_scroll_to_location(w, trackElement.x, trackElement.y, trackElement.element->base_height * 8);
    }
    else
    {
        ride_initialise_construction_window(rideIndex);
    }
}

/**
 *
 *  rct2: 0x006DD4D5
 */
static void ride_remove_cable_lift(Ride* ride)
{
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT)
    {
        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CABLE_LIFT;
        uint16_t spriteIndex = ride->cable_lift;
        do
        {
            rct_vehicle* vehicle = GET_VEHICLE(spriteIndex);
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
static void ride_remove_vehicles(Ride* ride)
{
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_ON_TRACK;
        ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_TEST_IN_PROGRESS | RIDE_LIFECYCLE_HAS_STALLED_VEHICLE);

        for (size_t i = 0; i < MAX_VEHICLES_PER_RIDE; i++)
        {
            uint16_t spriteIndex = ride->vehicles[i];
            while (spriteIndex != SPRITE_INDEX_NULL)
            {
                rct_vehicle* vehicle = GET_VEHICLE(spriteIndex);
                invalidate_sprite_2((rct_sprite*)vehicle);
                sprite_remove((rct_sprite*)vehicle);
                spriteIndex = vehicle->next_vehicle_on_train;
            }

            ride->vehicles[i] = SPRITE_INDEX_NULL;
        }

        for (size_t i = 0; i < MAX_STATIONS; i++)
            ride->stations[i].TrainAtStation = RideStation::NO_TRAIN;
    }
}

/**
 *
 *  rct2: 0x006DD4AC
 */
void ride_clear_for_construction(Ride* ride)
{
    ride_measurement_clear(ride);

    ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;

    // Open circuit rides will go directly into building mode (creating ghosts) where it would normally clear the stats,
    // however this causes desyncs since it's directly run from the window and other clients would not get it.
    // To prevent these problems, unconditionally invalidate the test results on all clients in multiplayer games.
    if (network_get_mode() != NETWORK_MODE_NONE)
    {
        invalidate_test_results(ride);
    }

    ride_remove_cable_lift(ride);
    ride_remove_vehicles(ride);
    ride_clear_blocked_tiles(ride);

    auto w = window_find_by_number(WC_RIDE, ride->id);
    if (w != nullptr)
        window_event_resize_call(w);
}

/**
 *
 *  rct2: 0x006664DF
 */
void ride_remove_peeps(Ride* ride)
{
    // Find first station
    int8_t stationIndex = ride_get_first_valid_station_start(ride);

    // Get exit position and direction
    int32_t exitX = 0;
    int32_t exitY = 0;
    int32_t exitZ = 0;
    int32_t exitDirection = 255;
    if (stationIndex != -1)
    {
        TileCoordsXYZD location = ride_get_exit_location(ride, stationIndex);
        if (!location.isNull())
        {
            exitX = location.x;
            exitY = location.y;
            exitZ = location.z;
            exitDirection = location.direction;

            exitX = (exitX * 32) - (word_981D6C[exitDirection].x * 20) + 16;
            exitY = (exitY * 32) - (word_981D6C[exitDirection].y * 20) + 16;
            exitZ = (exitZ * 8) + 2;

            // Reverse direction
            exitDirection = direction_reverse(exitDirection);

            exitDirection *= 8;
        }
    }

    // Place all the peeps at exit
    uint16_t spriteIndex;
    rct_peep* peep;
    FOR_ALL_PEEPS (spriteIndex, peep)
    {
        if (peep->state == PEEP_STATE_QUEUING_FRONT || peep->state == PEEP_STATE_ENTERING_RIDE
            || peep->state == PEEP_STATE_LEAVING_RIDE || peep->state == PEEP_STATE_ON_RIDE)
        {
            if (peep->current_ride != ride->id)
                continue;

            peep_decrement_num_riders(peep);
            if (peep->state == PEEP_STATE_QUEUING_FRONT && peep->sub_state == PEEP_RIDE_AT_ENTRANCE)
                peep->RemoveFromQueue();

            peep->Invalidate();

            if (exitDirection == 255)
            {
                int32_t x = peep->next_x + 16;
                int32_t y = peep->next_y + 16;
                int32_t z = peep->next_z * 8;
                if (peep->GetNextIsSloped())
                    z += 8;
                z++;
                sprite_move(x, y, z, (rct_sprite*)peep);
            }
            else
            {
                sprite_move(exitX, exitY, exitZ, (rct_sprite*)peep);
                peep->sprite_direction = exitDirection;
            }

            peep->Invalidate();
            peep->state = PEEP_STATE_FALLING;
            peep->SwitchToSpecialSprite(0);

            peep->happiness = std::min(peep->happiness, peep->happiness_target) / 2;
            peep->happiness_target = peep->happiness;
            peep->window_invalidate_flags |= PEEP_INVALIDATE_PEEP_STATS;
        }
    }

    ride->num_riders = 0;
    ride->slide_in_use = 0;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN;
}

void ride_clear_blocked_tiles(Ride* ride)
{
    for (int32_t y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int32_t x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            auto element = map_get_first_element_at(x, y);
            if (element != nullptr)
            {
                do
                {
                    if (element->GetType() == TILE_ELEMENT_TYPE_TRACK && element->AsTrack()->GetRideIndex() == ride->id)
                    {
                        // Unblock footpath element that is at same position
                        auto footpathElement = map_get_footpath_element(x, y, element->base_height);
                        if (footpathElement != nullptr)
                        {
                            footpathElement->flags &= ~TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE;
                        }
                    }
                } while (!(element++)->IsLastForTile());
            }
        }
    }
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
int32_t sub_6C683D(
    int32_t* x, int32_t* y, int32_t* z, int32_t direction, int32_t type, uint16_t extra_params, TileElement** output_element,
    uint16_t flags)
{
    TileElement* tileElement = map_get_first_element_at(*x / 32, *y / 32);
    TileElement* successTileElement = nullptr;

    do
    {
        if (tileElement->base_height != *z / 8)
            continue;

        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if ((tileElement->GetDirection()) != direction)
            continue;

        if (type != tileElement->AsTrack()->GetTrackType())
            continue;

        successTileElement = tileElement;
        if (tileElement->AsTrack()->GetSequenceIndex() == 0)
            break;
    } while (!(tileElement++)->IsLastForTile());

    tileElement = successTileElement;
    if (tileElement == nullptr)
        return 1;

    // Possibly z should be & 0xF8
    const rct_preview_track* trackBlock = get_track_def_from_ride_index(tileElement->AsTrack()->GetRideIndex(), type);

    int32_t sequence = tileElement->AsTrack()->GetSequenceIndex();
    uint8_t mapDirection = tileElement->GetDirection();

    switch (mapDirection)
    {
        case TILE_ELEMENT_DIRECTION_WEST:
            *x -= trackBlock[sequence].x;
            *y -= trackBlock[sequence].y;
            break;
        case TILE_ELEMENT_DIRECTION_NORTH:
            *x -= trackBlock[sequence].y;
            *y += trackBlock[sequence].x;
            break;
        case TILE_ELEMENT_DIRECTION_EAST:
            *x += trackBlock[sequence].x;
            *y += trackBlock[sequence].y;
            break;
        case TILE_ELEMENT_DIRECTION_SOUTH:
            *x += trackBlock[sequence].y;
            *y -= trackBlock[sequence].x;
            break;
    }
    *z -= trackBlock[sequence].z;

    int32_t start_x = *x, start_y = *y, start_z = *z;
    *z += trackBlock[0].z;
    for (int32_t i = 0; trackBlock[i].index != 0xFF; ++i)
    {
        int32_t cur_x = start_x, cur_y = start_y, cur_z = start_z;
        switch (mapDirection)
        {
            case TILE_ELEMENT_DIRECTION_WEST:
                cur_x += trackBlock[i].x;
                cur_y += trackBlock[i].y;
                break;
            case TILE_ELEMENT_DIRECTION_NORTH:
                cur_x += trackBlock[i].y;
                cur_y -= trackBlock[i].x;
                break;
            case TILE_ELEMENT_DIRECTION_EAST:
                cur_x -= trackBlock[i].x;
                cur_y -= trackBlock[i].y;
                break;
            case TILE_ELEMENT_DIRECTION_SOUTH:
                cur_x -= trackBlock[i].y;
                cur_y += trackBlock[i].x;
                break;
        }
        cur_z += trackBlock[i].z;

        map_invalidate_tile_full(cur_x, cur_y);

        tileElement = map_get_first_element_at(cur_x / 32, cur_y / 32);
        successTileElement = nullptr;
        do
        {
            if (tileElement->base_height != cur_z / 8)
                continue;

            if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                continue;

            if ((tileElement->GetDirection()) != direction)
                continue;

            if (tileElement->AsTrack()->GetSequenceIndex() != trackBlock[i].index)
                continue;

            if (type == tileElement->AsTrack()->GetTrackType())
            {
                successTileElement = tileElement;
                break;
            }
        } while (!(tileElement++)->IsLastForTile());

        if (successTileElement == nullptr)
        {
            return 1;
        }
        if (i == 0 && output_element != nullptr)
        {
            *output_element = tileElement;
        }
        if (flags & (1 << 0))
        {
            tileElement->AsTrack()->SetHighlight(false);
        }
        if (flags & (1 << 1))
        {
            tileElement->AsTrack()->SetHighlight(true);
        }
        if (flags & (1 << 2))
        {
            tileElement->AsTrack()->SetColourScheme((uint8_t)(extra_params & 0xFF));
        }
        if (flags & (1 << 5))
        {
            tileElement->AsTrack()->SetSeatRotation((uint8_t)(extra_params & 0xFF));
        }

        if (flags & (1 << 3))
        {
            tileElement->AsTrack()->SetHasCableLift(true);
        }
        if (flags & (1 << 4))
        {
            tileElement->AsTrack()->SetHasCableLift(false);
        }
    }

    return 0;
}

void ride_restore_provisional_track_piece()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK)
    {
        ride_id_t rideIndex;
        int32_t x, y, z, direction, type, liftHillAndAlternativeState;
        if (window_ride_construction_update_state(
                &type, &direction, &rideIndex, &liftHillAndAlternativeState, &x, &y, &z, nullptr))
        {
            ride_construction_remove_ghosts();
        }
        else
        {
            _currentTrackPrice = place_provisional_track_piece(
                rideIndex, type, direction, liftHillAndAlternativeState, x, y, z);
            window_ride_construction_update_active_elements();
        }
    }
}

void ride_remove_provisional_track_piece()
{
    if (!(_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK))
    {
        return;
    }
    Ride* ride;
    int32_t x, y, z, direction;

    ride_id_t rideIndex = _currentRideIndex;

    x = _unkF440C5.x;
    y = _unkF440C5.y;
    z = _unkF440C5.z;

    ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_MAZE)
    {
        int32_t flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5
            | GAME_COMMAND_FLAG_GHOST;
        maze_set_track(x, y, z, flags, false, 0, rideIndex, GC_SET_MAZE_TRACK_FILL);
        maze_set_track(x, y + 16, z, flags, false, 1, rideIndex, GC_SET_MAZE_TRACK_FILL);
        maze_set_track(x + 16, y + 16, z, flags, false, 2, rideIndex, GC_SET_MAZE_TRACK_FILL);
        maze_set_track(x + 16, y, z, flags, false, 3, rideIndex, GC_SET_MAZE_TRACK_FILL);
    }
    else
    {
        direction = _unkF440C5.direction;
        if (!(direction & 4))
        {
            x -= CoordsDirectionDelta[direction].x;
            y -= CoordsDirectionDelta[direction].y;
        }
        CoordsXYE next_track;
        if (track_block_get_next_from_zero(x, y, z, rideIndex, direction, &next_track, &z, &direction, true))
        {
            int32_t flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5
                | GAME_COMMAND_FLAG_GHOST;
            uint8_t trackType = next_track.element->AsTrack()->GetTrackType();
            int32_t trackSequence = next_track.element->AsTrack()->GetSequenceIndex();
            game_do_command(
                next_track.x, flags | ((direction & 3) << 8), next_track.y, trackType | (trackSequence << 8),
                GAME_COMMAND_REMOVE_TRACK, z, 0);
        }
    }
}

/**
 *
 *  rct2: 0x006C96C0
 */
void ride_construction_remove_ghosts()
{
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT)
    {
        ride_entrance_exit_remove_ghost();
        _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT;
    }
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK)
    {
        ride_remove_provisional_track_piece();
        _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_TRACK;
    }
}

/*
 *  rct2: 0x006C9627
 */
void ride_construction_invalidate_current_track()
{
    int32_t x, y, z;

    switch (_rideConstructionState)
    {
        case RIDE_CONSTRUCTION_STATE_SELECTED:
            x = _currentTrackBeginX;
            y = _currentTrackBeginY;
            z = _currentTrackBeginZ;
            sub_6C683D(&x, &y, &z, _currentTrackPieceDirection & 3, _currentTrackPieceType, 0, nullptr, 1);
            break;
        case RIDE_CONSTRUCTION_STATE_MAZE_BUILD:
        case RIDE_CONSTRUCTION_STATE_MAZE_MOVE:
        case RIDE_CONSTRUCTION_STATE_MAZE_FILL:
            if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW)
            {
                map_invalidate_tile_full(_currentTrackBeginX & 0xFFE0, _currentTrackBeginY & 0xFFE0);
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
            }
            break;
        default:
            if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW)
            {
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
    Ride* ride;

    ride = get_ride(_currentRideIndex);
    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_NO_TRACK) || ride->num_stations == 0)
    {
        _currentTrackCurve = RideConstructionDefaultTrackType[ride->type] | 0x100;
        _currentTrackSlopeEnd = 0;
        _currentTrackBankEnd = 0;
        _currentTrackLiftHill = 0;
        _currentTrackAlternative = RIDE_TYPE_NO_ALTERNATIVES;
        if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_START_CONSTRUCTION_INVERTED)
        {
            _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
        }
        _previousTrackSlopeEnd = 0;
        _previousTrackBankEnd = 0;
    }
    else
    {
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
    ride_id_t rideIndex;
    int32_t x, y, z, direction, trackType, curve, bank, slope;
    Ride* ride;
    track_begin_end trackBeginEnd;
    CoordsXYE xyElement;
    TileElement* tileElement;

    _currentTrackPrice = MONEY32_UNDEFINED;
    switch (_rideConstructionState)
    {
        case RIDE_CONSTRUCTION_STATE_FRONT:
            rideIndex = _currentRideIndex;
            ride = get_ride(rideIndex);

            x = _currentTrackBeginX;
            y = _currentTrackBeginY;
            z = _currentTrackBeginZ;
            direction = _currentTrackPieceDirection;
            if (!track_block_get_previous_from_zero(x, y, z, rideIndex, direction, &trackBeginEnd))
            {
                ride_construction_reset_current_piece();
                return;
            }
            tileElement = trackBeginEnd.begin_element;
            trackType = tileElement->AsTrack()->GetTrackType();

            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_NO_TRACK))
            {
                ride_construction_reset_current_piece();
                return;
            }

            // Set whether track is covered
            _currentTrackAlternative &= ~RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
            if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE)
            {
                if (tileElement->AsTrack()->IsInverted())
                {
                    _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
                }
            }

            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                curve = gFlatRideTrackCurveChain[trackType].next;
                bank = FlatRideTrackDefinitions[trackType].bank_end;
                slope = FlatRideTrackDefinitions[trackType].vangle_end;
            }
            else
            {
                if (track_element_is_booster(ride->type, trackType))
                {
                    curve = 0x100 | TRACK_ELEM_BOOSTER;
                }
                else
                {
                    curve = gTrackCurveChain[trackType].next;
                }
                bank = TrackDefinitions[trackType].bank_end;
                slope = TrackDefinitions[trackType].vangle_end;
            }

            // Set track curve
            _currentTrackCurve = curve;

            // Set track banking
            if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE)
            {
                if (bank == TRACK_BANK_UPSIDE_DOWN)
                {
                    bank = TRACK_BANK_NONE;
                    _currentTrackAlternative ^= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
                }
            }
            _currentTrackBankEnd = bank;
            _previousTrackBankEnd = bank;

            // Set track slope and lift hill
            _currentTrackSlopeEnd = slope;
            _previousTrackSlopeEnd = slope;
            _currentTrackLiftHill = tileElement->AsTrack()->HasChain() && slope != TRACK_SLOPE_DOWN_25
                && slope != TRACK_SLOPE_DOWN_60;
            break;
        case RIDE_CONSTRUCTION_STATE_BACK:
            rideIndex = _currentRideIndex;
            ride = get_ride(rideIndex);

            x = _currentTrackBeginX;
            y = _currentTrackBeginY;
            z = _currentTrackBeginZ;
            direction = direction_reverse(_currentTrackPieceDirection);
            if (!track_block_get_next_from_zero(x, y, z, rideIndex, direction, &xyElement, &z, &direction, false))
            {
                ride_construction_reset_current_piece();
                return;
            }
            tileElement = xyElement.element;
            trackType = tileElement->AsTrack()->GetTrackType();

            // Set whether track is covered
            _currentTrackAlternative &= ~RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
            if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE)
            {
                if (tileElement->AsTrack()->IsInverted())
                {
                    _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
                }
            }

            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                curve = gFlatRideTrackCurveChain[trackType].previous;
                bank = FlatRideTrackDefinitions[trackType].bank_start;
                slope = FlatRideTrackDefinitions[trackType].vangle_start;
            }
            else
            {
                if (track_element_is_booster(ride->type, trackType))
                {
                    curve = 0x100 | TRACK_ELEM_BOOSTER;
                }
                else
                {
                    curve = gTrackCurveChain[trackType].previous;
                }
                bank = TrackDefinitions[trackType].bank_start;
                slope = TrackDefinitions[trackType].vangle_start;
            }

            // Set track curve
            _currentTrackCurve = curve;

            // Set track banking
            if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE)
            {
                if (bank == TRACK_BANK_UPSIDE_DOWN)
                {
                    bank = TRACK_BANK_NONE;
                    _currentTrackAlternative ^= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
                }
            }
            _currentTrackBankEnd = bank;
            _previousTrackBankEnd = bank;

            // Set track slope and lift hill
            _currentTrackSlopeEnd = slope;
            _previousTrackSlopeEnd = slope;
            if (!gCheatsEnableChainLiftOnAllTrack)
            {
                _currentTrackLiftHill = tileElement->AsTrack()->HasChain();
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
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED)
    {
        ride_construction_invalidate_current_track();
        int32_t x = _currentTrackBeginX;
        int32_t y = _currentTrackBeginY;
        int32_t z = _currentTrackBeginZ;
        int32_t direction = _currentTrackPieceDirection;
        int32_t type = _currentTrackPieceType;
        TileElement* tileElement;
        if (sub_6C683D(&x, &y, &z, direction & 3, type, 0, &tileElement, 0))
        {
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
            window_ride_construction_update_active_elements();
            return;
        }

        // Invalidate previous track piece (we may not be changing height!)
        virtual_floor_invalidate();

        CoordsXYE inputElement, outputElement;
        inputElement.x = x;
        inputElement.y = y;
        inputElement.element = tileElement;
        if (track_block_get_next(&inputElement, &outputElement, &z, &direction))
        {
            x = outputElement.x;
            y = outputElement.y;
            tileElement = outputElement.element;
            if (!scenery_tool_is_active())
            {
                // Set next element's height.
                virtual_floor_set_height(tileElement->base_height << 3);
            }
        }
        else
        {
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_FRONT;
            _currentTrackBeginX = outputElement.x;
            _currentTrackBeginY = outputElement.y;
            _currentTrackBeginZ = z;
            _currentTrackPieceDirection = direction;
            _currentTrackPieceType = tileElement->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            _rideConstructionArrowPulseTime = 0;
            ride_construction_set_default_next_piece();
            window_ride_construction_update_active_elements();
            return;
        }

        _currentTrackBeginX = x;
        _currentTrackBeginY = y;
        _currentTrackBeginZ = z;
        _currentTrackPieceDirection = tileElement->GetDirection();
        _currentTrackPieceType = tileElement->AsTrack()->GetTrackType();
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        window_ride_construction_update_active_elements();
    }
    else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
    {
        if (ride_select_forwards_from_back())
        {
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
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED)
    {
        ride_construction_invalidate_current_track();
        int32_t x = _currentTrackBeginX;
        int32_t y = _currentTrackBeginY;
        int32_t z = _currentTrackBeginZ;
        int32_t direction = _currentTrackPieceDirection;
        int32_t type = _currentTrackPieceType;
        TileElement* tileElement;
        if (sub_6C683D(&x, &y, &z, direction & 3, type, 0, &tileElement, 0))
        {
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
            window_ride_construction_update_active_elements();
            return;
        }

        // Invalidate previous track piece (we may not be changing height!)
        virtual_floor_invalidate();

        track_begin_end trackBeginEnd;
        if (track_block_get_previous(x, y, tileElement, &trackBeginEnd))
        {
            _currentTrackBeginX = trackBeginEnd.begin_x;
            _currentTrackBeginY = trackBeginEnd.begin_y;
            _currentTrackBeginZ = trackBeginEnd.begin_z;
            _currentTrackPieceDirection = trackBeginEnd.begin_direction;
            _currentTrackPieceType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            _rideConstructionArrowPulseTime = 0;
            if (!scenery_tool_is_active())
            {
                // Set previous element's height.
                virtual_floor_set_height(trackBeginEnd.begin_element->base_height << 3);
            }
            window_ride_construction_update_active_elements();
        }
        else
        {
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_BACK;
            _currentTrackBeginX = trackBeginEnd.end_x;
            _currentTrackBeginY = trackBeginEnd.end_y;
            _currentTrackBeginZ = trackBeginEnd.begin_z;
            _currentTrackPieceDirection = trackBeginEnd.end_direction;
            _currentTrackPieceType = tileElement->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            _rideConstructionArrowPulseTime = 0;
            ride_construction_set_default_next_piece();
            window_ride_construction_update_active_elements();
        }
    }
    else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT)
    {
        if (ride_select_backwards_from_front())
        {
            window_ride_construction_update_active_elements();
        }
    }
}

/**
 *
 *  rct2: 0x006CC2CA
 */
static int32_t ride_modify_entrance_or_exit(TileElement* tileElement, int32_t x, int32_t y)
{
    int32_t entranceType;
    rct_window* constructionWindow;

    ride_id_t rideIndex = tileElement->AsEntrance()->GetRideIndex();

    entranceType = tileElement->AsEntrance()->GetEntranceType();
    if (entranceType != ENTRANCE_TYPE_RIDE_ENTRANCE && entranceType != ENTRANCE_TYPE_RIDE_EXIT)
        return 0;

    int32_t stationIndex = tileElement->AsEntrance()->GetStationIndex();

    // Get or create construction window for ride
    constructionWindow = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (constructionWindow == nullptr)
    {
        if (!ride_initialise_construction_window(rideIndex))
            return 0;

        constructionWindow = window_find_by_class(WC_RIDE_CONSTRUCTION);
        if (constructionWindow == nullptr)
            return 0;
    }

    ride_construction_invalidate_current_track();
    if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT || !(input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        || gCurrentToolWidget.window_classification != WC_RIDE_CONSTRUCTION)
    {
        // Replace entrance / exit
        tool_set(
            constructionWindow,
            entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE ? WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE : WC_RIDE_CONSTRUCTION__WIDX_EXIT,
            TOOL_CROSSHAIR);
        gRideEntranceExitPlaceType = entranceType;
        gRideEntranceExitPlaceRideIndex = rideIndex;
        gRideEntranceExitPlaceStationIndex = stationIndex;
        input_set_flag(INPUT_FLAG_6, true);
        if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT)
        {
            gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT;
        }

        window_ride_construction_update_active_elements();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    }
    else
    {
        // Remove entrance / exit
        game_do_command(
            x, (GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_APPLY), y, rideIndex,
            GAME_COMMAND_REMOVE_RIDE_ENTRANCE_OR_EXIT, stationIndex, entranceType == ENTRANCE_TYPE_RIDE_EXIT);
        gCurrentToolWidget.widget_index = entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE ? WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE
                                                                                      : WC_RIDE_CONSTRUCTION__WIDX_EXIT;
        gRideEntranceExitPlaceType = entranceType;
    }

    window_invalidate_by_class(WC_RIDE_CONSTRUCTION);
    return 1;
}

/**
 *
 *  rct2: 0x006CC287
 */
static int32_t ride_modify_maze(TileElement* tileElement, int32_t x, int32_t y)
{
    _currentRideIndex = tileElement->AsTrack()->GetRideIndex();
    _rideConstructionState = RIDE_CONSTRUCTION_STATE_MAZE_BUILD;
    _currentTrackBeginX = x;
    _currentTrackBeginY = y;
    _currentTrackBeginZ = tileElement->base_height * 8;
    _currentTrackSelectionFlags = 0;
    _rideConstructionArrowPulseTime = 0;

    auto intent = Intent(INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION);
    context_broadcast_intent(&intent);

    return 1;
}

/**
 *
 *  rct2: 0x006CC056
 */
int32_t ride_modify(CoordsXYE* input)
{
    int32_t x, y, z, direction, type;
    CoordsXYE tileElement, endOfTrackElement;
    Ride* ride;
    rct_ride_entry* rideEntry;

    tileElement = *input;
    ride_id_t rideIndex = tile_element_get_ride_index(tileElement.element);
    ride = get_ride(rideIndex);
    if (ride == nullptr)
    {
        return 0;
    }
    rideEntry = get_ride_entry_by_ride(ride);

    if ((rideEntry == nullptr) || !ride_check_if_construction_allowed(ride))
        return 0;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE)
    {
        set_format_arg(6, rct_string_id, ride->name);
        set_format_arg(8, uint32_t, ride->name_arguments);
        context_show_error(
            STR_CANT_START_CONSTRUCTION_ON, STR_LOCAL_AUTHORITY_FORBIDS_DEMOLITION_OR_MODIFICATIONS_TO_THIS_RIDE);
        return 0;
    }

    // Stop the ride again to clear all vehicles and peeps (compatible with network games)
    ride_set_status(rideIndex, RIDE_STATUS_CLOSED);

    // Check if element is a station entrance or exit
    if (tileElement.element->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
        return ride_modify_entrance_or_exit(tileElement.element, tileElement.x, tileElement.y);

    ride_create_or_find_construction_window(rideIndex);

    if (ride->type == RIDE_TYPE_MAZE)
        return ride_modify_maze(tileElement.element, tileElement.x, tileElement.y);

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS))
    {
        if (ride_find_track_gap(rideIndex, &tileElement, &endOfTrackElement))
            tileElement = endOfTrackElement;
    }

    x = tileElement.x;
    y = tileElement.y;
    z = tileElement.element->base_height * 8;
    direction = tileElement.element->GetDirection();
    type = tileElement.element->AsTrack()->GetTrackType();

    if (sub_6C683D(&x, &y, &z, direction, type, 0, nullptr, 0))
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

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_NO_TRACK))
    {
        window_ride_construction_update_active_elements();
        return 1;
    }

    ride_select_next_section();
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT)
    {
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

    if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_BACK)
    {
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
int32_t ride_initialise_construction_window(ride_id_t rideIndex)
{
    Ride* ride;
    rct_window* w;

    tool_cancel();
    ride = get_ride(rideIndex);

    if (!ride_check_if_construction_allowed(ride))
        return 0;

    ride_clear_for_construction(ride);
    ride_remove_peeps(ride);

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
    Ride* ride;
    int32_t i;

    // Remove all rides if scenario editor
    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
    {
        if (gS6Info.editor_step <= EDITOR_STEP_INVENTIONS_LIST_SET_UP)
            FOR_ALL_RIDES (i, ride)
                ride->type = RIDE_TYPE_NULL;
        return;
    }

    window_update_viewport_ride_music();

    // Update rides
    FOR_ALL_RIDES (i, ride)
        ride_update(ride);

    ride_music_update_final();
}

/**
 *
 *  rct2: 0x006ABE73
 */
static void ride_update(Ride* ride)
{
    if (ride->vehicle_change_timeout != 0)
        ride->vehicle_change_timeout--;

    ride_music_update(ride);

    // Update stations
    if (ride->type != RIDE_TYPE_MAZE)
        for (int32_t i = 0; i < MAX_STATIONS; i++)
            ride_update_station(ride, i);

    // Update financial statistics
    ride->num_customers_timeout++;

    if (ride->num_customers_timeout >= 960)
    {
        // This is meant to update about every 30 seconds
        ride->num_customers_timeout = 0;

        // Shift number of customers history, start of the array is the most recent one
        for (int32_t i = CUSTOMER_HISTORY_SIZE - 1; i > 0; i--)
        {
            ride->num_customers[i] = ride->num_customers[i - 1];
        }
        ride->num_customers[0] = ride->cur_num_customers;

        ride->cur_num_customers = 0;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

        ride->income_per_hour = ride_calculate_income_per_hour(ride);
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

        if (ride->upkeep_cost != MONEY16_UNDEFINED)
            ride->profit = (ride->income_per_hour - ((money32)ride->upkeep_cost * 16));
    }

    // Ride specific updates
    if (ride->type == RIDE_TYPE_CHAIRLIFT)
        ride_chairlift_update(ride);
    else if (ride->type == RIDE_TYPE_SPIRAL_SLIDE)
        ride_spiral_slide_update(ride);

    ride_breakdown_update(ride);

    // Various things include news messages
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION))
        if (((gCurrentTicks >> 1) & 255) == (uint32_t)ride->id)
            ride_breakdown_status_update(ride);

    ride_inspection_update(ride);

    if (ride->status == RIDE_STATUS_TESTING && gConfigGeneral.no_test_crashes)
    {
        for (int32_t i = 0; i < ride->num_vehicles; i++)
        {
            uint16_t spriteIndex = ride->vehicles[i];
            if (spriteIndex == SPRITE_INDEX_NULL)
                continue;

            rct_vehicle* vehicle = GET_VEHICLE(spriteIndex);

            if (vehicle->status == VEHICLE_STATUS_CRASHED || vehicle->status == VEHICLE_STATUS_CRASHING)
            {
                ride_set_status(ride->id, RIDE_STATUS_CLOSED);
                ride_set_status(ride->id, RIDE_STATUS_CLOSED);
                ride_set_status(ride->id, RIDE_STATUS_TESTING);
                break;
            }
        }
    }
}

/**
 *
 *  rct2: 0x006AC489
 */
static void ride_chairlift_update(Ride* ride)
{
    int32_t x, y, z;

    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
        return;
    if ((ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        && ride->breakdown_reason_pending == 0)
        return;

    uint16_t old_chairlift_bullwheel_rotation = ride->chairlift_bullwheel_rotation >> 14;
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
void ride_update_satisfaction(Ride* ride, uint8_t happiness)
{
    ride->satisfaction_next += happiness;
    ride->satisfaction_time_out++;
    if (ride->satisfaction_time_out >= 20)
    {
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
void ride_update_popularity(Ride* ride, uint8_t pop_amount)
{
    ride->popularity_next += pop_amount;
    ride->popularity_time_out++;
    if (ride->popularity_time_out < 25)
        return;

    ride->popularity = ride->popularity_next;
    ride->popularity_next = 0;
    ride->popularity_time_out = 0;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
}

/** rct2: 0x0098DDB8, 0x0098DDBA */
static constexpr const LocationXY16 ride_spiral_slide_main_tile_offset[][4] = {
    {
        { 32, 32 },
        { 0, 32 },
        { 0, 0 },
        { 32, 0 },
    },
    {
        { 32, 0 },
        { 0, 0 },
        { 0, -32 },
        { 32, -32 },
    },
    {
        { 0, 0 },
        { -32, 0 },
        { -32, -32 },
        { 0, -32 },
    },
    {
        { 0, 0 },
        { 0, 32 },
        { -32, 32 },
        { -32, 0 },
    },
};

/**
 *
 *  rct2: 0x006AC545
 */
static void ride_spiral_slide_update(Ride* ride)
{
    if (gCurrentTicks & 3)
        return;
    if (ride->slide_in_use == 0)
        return;

    ride->spiral_slide_progress++;
    if (ride->spiral_slide_progress >= 48)
    {
        ride->slide_in_use--;

        rct_peep* peep = GET_PEEP(ride->slide_peep);
        peep->destination_x++;
    }

    const uint8_t current_rotation = get_current_rotation();
    // Invalidate something related to station start
    for (int32_t i = 0; i < MAX_STATIONS; i++)
    {
        if (ride->stations[i].Start.xy == RCT_XY8_UNDEFINED)
            continue;

        int32_t x = ride->stations[i].Start.x;
        int32_t y = ride->stations[i].Start.y;

        TileElement* tileElement = ride_get_station_start_track_element(ride, i);
        if (tileElement == nullptr)
            continue;

        int32_t rotation = tileElement->GetDirection();
        x *= 32;
        y *= 32;
        x += ride_spiral_slide_main_tile_offset[rotation][current_rotation].x;
        y += ride_spiral_slide_main_tile_offset[rotation][current_rotation].y;

        map_invalidate_tile_zoom0(x, y, tileElement->base_height * 8, tileElement->clearance_height * 8);
    }
}

#pragma endregion

#pragma region Breakdown and inspection functions

static uint8_t _breakdownProblemProbabilities[] = {
    25, // BREAKDOWN_SAFETY_CUT_OUT
    12, // BREAKDOWN_RESTRAINTS_STUCK_CLOSED
    10, // BREAKDOWN_RESTRAINTS_STUCK_OPEN
    13, // BREAKDOWN_DOORS_STUCK_CLOSED
    10, // BREAKDOWN_DOORS_STUCK_OPEN
    6,  // BREAKDOWN_VEHICLE_MALFUNCTION
    0,  // BREAKDOWN_BRAKES_FAILURE
    3   // BREAKDOWN_CONTROL_FAILURE
};

/**
 *
 *  rct2: 0x006AC7C2
 */
static void ride_inspection_update(Ride* ride)
{
    if (gCurrentTicks & 2047)
        return;
    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
        return;

    ride->last_inspection++;
    if (ride->last_inspection == 0)
        ride->last_inspection--;

    int32_t inspectionIntervalMinutes = RideInspectionInterval[ride->inspection_interval];
    // An inspection interval of 0 minutes means the ride is set to never be inspected.
    if (inspectionIntervalMinutes == 0)
    {
        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
        return;
    }

    if (RideAvailableBreakdowns[ride->type] == 0)
        return;

    if (inspectionIntervalMinutes > ride->last_inspection)
        return;

    if (ride->lifecycle_flags
        & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION
           | RIDE_LIFECYCLE_CRASHED))
        return;

    // Inspect the first station that has an exit
    ride->lifecycle_flags |= RIDE_LIFECYCLE_DUE_INSPECTION;
    ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;

    int8_t stationIndex = ride_get_first_valid_station_exit(ride);
    ride->inspection_station = (stationIndex != -1) ? stationIndex : 0;
}

static int32_t get_age_penalty(Ride* ride)
{
    int32_t years;
    years = date_get_year(gDateMonthsElapsed - ride->build_date);
    switch (years)
    {
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
static void ride_breakdown_update(Ride* ride)
{
    if (gCurrentTicks & 255)
        return;
    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
        return;

    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        ride->downtime_history[0]++;

    if (!(gCurrentTicks & 8191))
    {
        int32_t totalDowntime = 0;

        for (int32_t i = 0; i < DOWNTIME_HISTORY_SIZE; i++)
        {
            totalDowntime += ride->downtime_history[i];
        }

        ride->downtime = std::min(totalDowntime / 2, 100);

        for (int32_t i = DOWNTIME_HISTORY_SIZE - 1; i > 0; i--)
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

    if (!ride->CanBreakDown())
    {
        ride->reliability = RIDE_INITIAL_RELIABILITY;
        return;
    }

    // Calculate breakdown probability?
    int32_t unreliabilityAccumulator = ride->unreliability_factor + get_age_penalty(ride);
    ride->reliability = (uint16_t)std::max(0, (ride->reliability - unreliabilityAccumulator));
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;

    // Random probability of a breakdown. Roughly this is 1 in
    //
    // (25000 - reliability) / 3 000 000
    //
    // a 0.8% chance, less the breakdown factor which accumulates as the game
    // continues.
    if ((ride->reliability == 0 || (int32_t)(scenario_rand() & 0x2FFFFF) <= 1 + RIDE_INITIAL_RELIABILITY - ride->reliability)
        && !gCheatsDisableAllBreakdowns)
    {
        int32_t breakdownReason = ride_get_new_breakdown_problem(ride);
        if (breakdownReason != -1)
            ride_prepare_breakdown(ride->id, breakdownReason);
    }
}

/**
 *
 *  rct2: 0x006B7294
 */
static int32_t ride_get_new_breakdown_problem(Ride* ride)
{
    int32_t availableBreakdownProblems, monthsOld, totalProbability, randomProbability, problemBits, breakdownProblem;

    // Brake failure is more likely when it's raining
    _breakdownProblemProbabilities[BREAKDOWN_BRAKES_FAILURE] = climate_is_raining() ? 20 : 3;

    if (!ride->CanBreakDown())
        return -1;

    availableBreakdownProblems = RideAvailableBreakdowns[ride->type];

    // Calculate the total probability range for all possible breakdown problems
    totalProbability = 0;
    problemBits = availableBreakdownProblems;
    while (problemBits != 0)
    {
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
    do
    {
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

bool Ride::CanBreakDown() const
{
    if (RideAvailableBreakdowns[this->type] == 0)
    {
        return false;
    }

    rct_ride_entry* entry = get_ride_entry_by_ride(this);
    if (entry == nullptr || entry->flags & RIDE_ENTRY_FLAG_CANNOT_BREAK_DOWN)
    {
        return false;
    }

    return true;
}

static void choose_random_train_to_breakdown_safe(Ride* ride)
{
    // Prevent integer division by zero in case of hacked ride.
    if (ride->num_vehicles == 0)
        return;

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
void ride_prepare_breakdown(ride_id_t rideIndex, int32_t breakdownReason)
{
    int32_t i;
    uint16_t vehicleSpriteIdx;
    Ride* ride;
    rct_vehicle* vehicle;

    ride = get_ride(rideIndex);
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        return;

    ride->lifecycle_flags |= RIDE_LIFECYCLE_BREAKDOWN_PENDING;

    ride->breakdown_reason_pending = breakdownReason;
    ride->breakdown_sound_modifier = 0;
    ride->not_fixed_timeout = 0;

    switch (breakdownReason)
    {
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
            vehicleSpriteIdx = ride->vehicles[ride->broken_vehicle];
            if (vehicleSpriteIdx != SPRITE_INDEX_NULL)
            {
                vehicle = GET_VEHICLE(vehicleSpriteIdx);
                for (i = ride->broken_car; i > 0; i--)
                {
                    if (vehicle->next_vehicle_on_train == SPRITE_INDEX_NULL)
                    {
                        vehicle = nullptr;
                        break;
                    }
                    else
                    {
                        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
                    }
                }
                if (vehicle != nullptr)
                    vehicle->update_flags |= VEHICLE_UPDATE_FLAG_BROKEN_CAR;
            }
            break;
        case BREAKDOWN_VEHICLE_MALFUNCTION:
            // Choose a random train
            choose_random_train_to_breakdown_safe(ride);
            ride->broken_car = 0;

            // Set flag on broken train, first car
            vehicleSpriteIdx = ride->vehicles[ride->broken_vehicle];
            if (vehicleSpriteIdx != SPRITE_INDEX_NULL)
            {
                vehicle = GET_VEHICLE(vehicleSpriteIdx);
                vehicle->update_flags |= VEHICLE_UPDATE_FLAG_BROKEN_TRAIN;
            }
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
void ride_breakdown_add_news_item(ride_id_t rideIndex)
{
    Ride* ride = get_ride(rideIndex);

    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);
    if (gConfigNotifications.ride_broken_down)
    {
        news_item_add_to_queue(NEWS_ITEM_RIDE, STR_RIDE_IS_BROKEN_DOWN, rideIndex);
    }
}

/**
 *
 *  rct2: 0x006B75C8
 */
static void ride_breakdown_status_update(Ride* ride)
{
    // Warn player if ride hasn't been fixed for ages
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        ride->not_fixed_timeout++;
        // When there has been a full 255 timeout ticks this
        // will force timeout ticks to keep issuing news every
        // 16 ticks. Note there is no reason to do this.
        if (ride->not_fixed_timeout == 0)
            ride->not_fixed_timeout -= 16;

        if (!(ride->not_fixed_timeout & 15) && ride->mechanic_status != RIDE_MECHANIC_STATUS_FIXING
            && ride->mechanic_status != RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES)
        {
            set_format_arg(0, rct_string_id, ride->name);
            set_format_arg(2, uint32_t, ride->name_arguments);
            if (gConfigNotifications.ride_warnings)
            {
                news_item_add_to_queue(NEWS_ITEM_RIDE, STR_RIDE_IS_STILL_NOT_FIXED, ride->id);
            }
        }
    }

    ride_mechanic_status_update(ride, ride->mechanic_status);
}

/**
 *
 *  rct2: 0x006B762F
 */
static void ride_mechanic_status_update(Ride* ride, int32_t mechanicStatus)
{
    rct_peep* mechanic;

    // Turn a pending breakdown into a breakdown.
    if ((mechanicStatus == RIDE_MECHANIC_STATUS_UNDEFINED || mechanicStatus == RIDE_MECHANIC_STATUS_CALLING
         || mechanicStatus == RIDE_MECHANIC_STATUS_HEADING)
        && (ride->lifecycle_flags & RIDE_LIFECYCLE_BREAKDOWN_PENDING) && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        auto breakdownReason = ride->breakdown_reason_pending;
        if (breakdownReason == BREAKDOWN_SAFETY_CUT_OUT || breakdownReason == BREAKDOWN_BRAKES_FAILURE
            || breakdownReason == BREAKDOWN_CONTROL_FAILURE)
        {
            ride->lifecycle_flags |= RIDE_LIFECYCLE_BROKEN_DOWN;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE | RIDE_INVALIDATE_RIDE_LIST
                | RIDE_INVALIDATE_RIDE_MAIN;
            ride->breakdown_reason = breakdownReason;
            ride_breakdown_add_news_item(ride->id);
        }
    }
    switch (mechanicStatus)
    {
        case RIDE_MECHANIC_STATUS_UNDEFINED:
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
            {
                ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            }
            break;
        case RIDE_MECHANIC_STATUS_CALLING:
            if (RideAvailableBreakdowns[ride->type] == 0)
            {
                ride->lifecycle_flags &= ~(
                    RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION);
                break;
            }

            ride_call_closest_mechanic(ride);
            break;
        case RIDE_MECHANIC_STATUS_HEADING:
            mechanic = nullptr;
            if (ride->mechanic != SPRITE_INDEX_NULL)
            {
                mechanic = &(get_sprite(ride->mechanic)->peep);
            }
            if (mechanic == nullptr || !mechanic->IsMechanic()
                || (mechanic->state != PEEP_STATE_HEADING_TO_INSPECTION && mechanic->state != PEEP_STATE_ANSWERING)
                || mechanic->current_ride != ride->id)
            {
                ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
                ride_mechanic_status_update(ride, RIDE_MECHANIC_STATUS_CALLING);
            }
            break;
        case RIDE_MECHANIC_STATUS_FIXING:
            mechanic = nullptr;
            if (ride->mechanic != SPRITE_INDEX_NULL)
            {
                mechanic = &(get_sprite(ride->mechanic)->peep);
            }
            if (mechanic == nullptr || !mechanic->IsMechanic()
                || (mechanic->state != PEEP_STATE_HEADING_TO_INSPECTION && mechanic->state != PEEP_STATE_FIXING
                    && mechanic->state != PEEP_STATE_INSPECTING && mechanic->state != PEEP_STATE_ANSWERING))
            {
                ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
                ride_mechanic_status_update(ride, RIDE_MECHANIC_STATUS_CALLING);
            }
            break;
    }
}

/**
 *
 *  rct2: 0x006B796C
 */
static void ride_call_mechanic(Ride* ride, rct_peep* mechanic, int32_t forInspection)
{
    mechanic->SetState(forInspection ? PEEP_STATE_HEADING_TO_INSPECTION : PEEP_STATE_ANSWERING);
    mechanic->sub_state = 0;
    ride->mechanic_status = RIDE_MECHANIC_STATUS_HEADING;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
    ride->mechanic = mechanic->sprite_index;
    mechanic->current_ride = ride->id;
    mechanic->current_ride_station = ride->inspection_station;
}

/**
 *
 *  rct2: 0x006B76AB
 */
static void ride_call_closest_mechanic(Ride* ride)
{
    auto forInspection = (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)) == 0;
    auto mechanic = ride_find_closest_mechanic(ride, forInspection);
    if (mechanic != nullptr)
        ride_call_mechanic(ride, mechanic, forInspection);
}

rct_peep* ride_find_closest_mechanic(Ride* ride, int32_t forInspection)
{
    int32_t x, y, z, stationIndex;
    TileCoordsXYZD location;
    TileElement* tileElement;

    // Get either exit position or entrance position if there is no exit
    stationIndex = ride->inspection_station;
    location = ride_get_exit_location(ride, stationIndex);
    if (location.isNull())
    {
        location = ride_get_entrance_location(ride, stationIndex);
        if (location.isNull())
            return nullptr;
    }

    // Get station start track element and position
    x = location.x;
    y = location.y;
    z = location.z;
    tileElement = ride_get_station_exit_element(x, y, z);
    if (tileElement == nullptr)
        return nullptr;

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
rct_peep* find_closest_mechanic(int32_t x, int32_t y, int32_t forInspection)
{
    uint32_t closestDistance, distance;
    uint16_t spriteIndex;
    rct_peep *peep, *closestMechanic = nullptr;

    closestDistance = UINT_MAX;
    FOR_ALL_STAFF (spriteIndex, peep)
    {
        if (peep->staff_type != STAFF_TYPE_MECHANIC)
            continue;

        if (!forInspection)
        {
            if (peep->state == PEEP_STATE_HEADING_TO_INSPECTION)
            {
                if (peep->sub_state >= 4)
                    continue;
            }
            else if (peep->state != PEEP_STATE_PATROLLING)
                continue;

            if (!(peep->staff_orders & STAFF_ORDERS_FIX_RIDES))
                continue;
        }
        else
        {
            if (peep->state != PEEP_STATE_PATROLLING || !(peep->staff_orders & STAFF_ORDERS_INSPECT_RIDES))
                continue;
        }

        if (map_is_location_in_park({ x, y }))
            if (!staff_is_location_in_patrol(peep, x & 0xFFE0, y & 0xFFE0))
                continue;

        if (peep->x == LOCATION_NULL)
            continue;

        // Manhattan distance
        distance = std::abs(peep->x - x) + std::abs(peep->y - y);
        if (distance < closestDistance)
        {
            closestDistance = distance;
            closestMechanic = peep;
        }
    }

    return closestMechanic;
}

rct_peep* ride_get_assigned_mechanic(Ride* ride)
{
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        if (ride->mechanic_status == RIDE_MECHANIC_STATUS_HEADING || ride->mechanic_status == RIDE_MECHANIC_STATUS_FIXING
            || ride->mechanic_status == RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES)
        {
            rct_peep* peep = &(get_sprite(ride->mechanic)->peep);
            if (peep->IsMechanic())
                return peep;
        }
    }

    return nullptr;
}

#pragma endregion

#pragma region Music functions

/**
 *
 *  rct2: 0x006ABE85
 */
static void ride_music_update(Ride* ride)
{
    if (!(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_MUSIC_ON_DEFAULT)
        && !(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_MUSIC))
    {
        return;
    }

    if (ride->status != RIDE_STATUS_OPEN || !(ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC))
    {
        ride->music_tune_id = 255;
        return;
    }

    if (ride->type == RIDE_TYPE_CIRCUS)
    {
        uint16_t vehicleSpriteIdx = ride->vehicles[0];
        if (vehicleSpriteIdx != SPRITE_INDEX_NULL)
        {
            rct_vehicle* vehicle = GET_VEHICLE(vehicleSpriteIdx);
            if (vehicle->status != VEHICLE_STATUS_DOING_CIRCUS_SHOW)
            {
                ride->music_tune_id = 255;
                return;
            }
        }
    }

    // Oscillate parameters for a power cut effect when breaking down
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        if (ride->breakdown_reason_pending == BREAKDOWN_CONTROL_FAILURE)
        {
            if (!(gCurrentTicks & 7))
                if (ride->breakdown_sound_modifier != 255)
                    ride->breakdown_sound_modifier++;
        }
        else
        {
            if ((ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
                || ride->breakdown_reason_pending == BREAKDOWN_BRAKES_FAILURE
                || ride->breakdown_reason_pending == BREAKDOWN_CONTROL_FAILURE)
            {
                if (ride->breakdown_sound_modifier != 255)
                    ride->breakdown_sound_modifier++;
            }

            if (ride->breakdown_sound_modifier == 255)
            {
                ride->music_tune_id = 255;
                return;
            }
        }
    }

    // Select random tune from available tunes for a music style (of course only merry-go-rounds have more than one tune)
    if (ride->music_tune_id == 255)
    {
        const auto& musicStyleTunes = gRideMusicStyleTuneIds[ride->music];
        auto numTunes = musicStyleTunes.size();
        ride->music_tune_id = musicStyleTunes[util_rand() % numTunes];
        ride->music_position = 0;
        return;
    }

    int32_t x = ride->stations[0].Start.x * 32 + 16;
    int32_t y = ride->stations[0].Start.y * 32 + 16;
    int32_t z = ride->stations[0].Height * 8;

    int32_t sampleRate = 22050;

    // Alter sample rate for a power cut effect
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        sampleRate = ride->breakdown_sound_modifier * 70;
        if (ride->breakdown_reason_pending != BREAKDOWN_CONTROL_FAILURE)
            sampleRate *= -1;
        sampleRate += 22050;
    }

    ride->music_position = ride_music_params_update(x, y, z, ride->id, sampleRate, ride->music_position, &ride->music_tune_id);
}

#pragma endregion

#pragma region Measurement functions

/**
 *
 *  rct2: 0x006B642B
 */
void ride_measurement_clear(Ride* ride)
{
    rct_ride_measurement* measurement;

    if (ride->measurement_index == 255)
        return;

    measurement = get_ride_measurement(ride->measurement_index);
    measurement->ride_index = RIDE_ID_NULL;
    ride->measurement_index = 255;
}

/**
 *
 *  rct2: 0x006B64F2
 */
static void ride_measurement_update(rct_ride_measurement* measurement)
{
    uint16_t spriteIndex;
    Ride* ride;
    rct_vehicle* vehicle;
    int32_t velocity, altitude, verticalG, lateralG;

    ride = get_ride(measurement->ride_index);
    spriteIndex = ride->vehicles[measurement->vehicle_index];
    if (spriteIndex == SPRITE_INDEX_NULL)
        return;

    vehicle = GET_VEHICLE(spriteIndex);

    if (measurement->flags & RIDE_MEASUREMENT_FLAG_UNLOADING)
    {
        if (vehicle->status != VEHICLE_STATUS_DEPARTING && vehicle->status != VEHICLE_STATUS_TRAVELLING_CABLE_LIFT)
            return;

        measurement->flags &= ~RIDE_MEASUREMENT_FLAG_UNLOADING;
        if (measurement->current_station == vehicle->current_station)
            measurement->current_item = 0;
    }

    if (vehicle->status == VEHICLE_STATUS_UNLOADING_PASSENGERS)
    {
        measurement->flags |= RIDE_MEASUREMENT_FLAG_UNLOADING;
        return;
    }

    uint8_t trackType = (vehicle->track_type >> 2) & 0xFF;
    if (trackType == TRACK_ELEM_BLOCK_BRAKES || trackType == TRACK_ELEM_CABLE_LIFT_HILL
        || trackType == TRACK_ELEM_25_DEG_UP_TO_FLAT || trackType == TRACK_ELEM_60_DEG_UP_TO_FLAT
        || trackType == TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT || trackType == TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT)
        if (vehicle->velocity == 0)
            return;

    if (measurement->current_item >= RIDE_MEASUREMENT_MAX_ITEMS)
        return;

    if (measurement->flags & RIDE_MEASUREMENT_FLAG_G_FORCES)
    {
        vehicle_get_g_forces(vehicle, &verticalG, &lateralG);
        verticalG = std::clamp(verticalG / 8, -127, 127);
        lateralG = std::clamp(lateralG / 8, -127, 127);

        if (gScenarioTicks & 1)
        {
            verticalG = (verticalG + measurement->vertical[measurement->current_item]) / 2;
            lateralG = (lateralG + measurement->lateral[measurement->current_item]) / 2;
        }

        measurement->vertical[measurement->current_item] = verticalG & 0xFF;
        measurement->lateral[measurement->current_item] = lateralG & 0xFF;
    }

    velocity = std::min(std::abs((vehicle->velocity * 5) >> 16), 255);
    altitude = std::min(vehicle->z / 8, 255);

    if (gScenarioTicks & 1)
    {
        velocity = (velocity + measurement->velocity[measurement->current_item]) / 2;
        altitude = (altitude + measurement->altitude[measurement->current_item]) / 2;
    }

    measurement->velocity[measurement->current_item] = velocity & 0xFF;
    measurement->altitude[measurement->current_item] = altitude & 0xFF;

    if (gScenarioTicks & 1)
    {
        measurement->current_item++;
        measurement->num_items = std::max(measurement->num_items, measurement->current_item);
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
    for (int32_t i = 0; i < MAX_RIDE_MEASUREMENTS; i++)
    {
        rct_ride_measurement* measurement = get_ride_measurement(i);
        if (measurement->ride_index == RIDE_ID_NULL)
            continue;

        Ride* ride = get_ride(measurement->ride_index);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
            continue;

        if (measurement->flags & RIDE_MEASUREMENT_FLAG_RUNNING)
        {
            ride_measurement_update(measurement);
        }
        else
        {
            // For each vehicle
            for (int32_t j = 0; j < ride->num_vehicles; j++)
            {
                uint16_t vehicleSpriteIdx = ride->vehicles[j];
                if (vehicleSpriteIdx == SPRITE_INDEX_NULL)
                    continue;

                rct_vehicle* vehicle = GET_VEHICLE(vehicleSpriteIdx);
                if (vehicle->status == VEHICLE_STATUS_DEPARTING || vehicle->status == VEHICLE_STATUS_TRAVELLING_CABLE_LIFT)
                {
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

static rct_ride_measurement* ride_get_existing_measurement(ride_id_t rideIndex)
{
    for (int32_t i = 0; i < MAX_RIDE_MEASUREMENTS; i++)
    {
        rct_ride_measurement* measurement = get_ride_measurement(i);
        if (measurement->ride_index == rideIndex)
            return measurement;
    }

    return nullptr;
}

static int32_t ride_get_free_measurement()
{
    for (int32_t i = 0; i < MAX_RIDE_MEASUREMENTS; i++)
    {
        rct_ride_measurement* measurement = get_ride_measurement(i);
        if (measurement->ride_index == RIDE_ID_NULL)
            return i;
    }

    return -1;
}

/**
 *
 *  rct2: 0x006B66D9
 */
rct_ride_measurement* ride_get_measurement(ride_id_t rideIndex, rct_string_id* message)
{
    Ride* ride = get_ride(rideIndex);

    // Check if ride type supports data logging
    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_DATA_LOGGING))
    {
        if (message != nullptr)
            *message = STR_DATA_LOGGING_NOT_AVAILABLE_FOR_THIS_TYPE_OF_RIDE;
        return nullptr;
    }

    // Check if a measurement already exists for this ride
    rct_ride_measurement* measurement = ride_get_existing_measurement(rideIndex);
    if (measurement == nullptr)
    {
        // Find a free measurement
        int32_t i = ride_get_free_measurement();
        if (i == -1)
        {
            // Use last recently used measurement for some other ride
            int32_t lruIndex = 0;
            uint32_t lruTicks = 0xFFFFFFFF;
            for (i = 0; i < MAX_RIDE_MEASUREMENTS; i++)
            {
                measurement = get_ride_measurement(i);

                if (measurement->last_use_tick <= lruTicks)
                {
                    lruTicks = measurement->last_use_tick;
                    lruIndex = i;
                }
            }

            i = lruIndex;
            measurement = get_ride_measurement(i);
            get_ride(measurement->ride_index)->measurement_index = 255;
        }
        else
        {
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
    if (measurement->flags & 1)
    {
        if (message != nullptr)
            *message = STR_EMPTY;
        return measurement;
    }
    else
    {
        set_format_arg(0, rct_string_id, RideComponentNames[RideNameConvention[ride->type].vehicle].singular);
        set_format_arg(2, rct_string_id, RideComponentNames[RideNameConvention[ride->type].station].singular);
        if (message != nullptr)
            *message = STR_DATA_LOGGING_WILL_START_WHEN_NEXT_LEAVES;
        return nullptr;
    }
}

#pragma endregion

#pragma region Colour functions

TrackColour ride_get_track_colour(Ride* ride, int32_t colourScheme)
{
    TrackColour result;
    result.main = ride->track_colour[colourScheme].main;
    result.additional = ride->track_colour[colourScheme].additional;
    result.supports = ride->track_colour[colourScheme].supports;
    return result;
}

vehicle_colour ride_get_vehicle_colour(Ride* ride, int32_t vehicleIndex)
{
    vehicle_colour result;

    // Prevent indexing array out of bounds
    if (vehicleIndex > 31)
    {
        vehicleIndex = 31;
    }

    result.main = ride->vehicle_colours[vehicleIndex].Body;
    result.additional_1 = ride->vehicle_colours[vehicleIndex].Trim;
    result.additional_2 = ride->vehicle_colours[vehicleIndex].Ternary;
    return result;
}

static bool ride_does_vehicle_colour_exist(uint8_t ride_sub_type, vehicle_colour* vehicleColour)
{
    int32_t i;
    Ride* ride2;
    FOR_ALL_RIDES (i, ride2)
    {
        if (ride2->subtype != ride_sub_type)
            continue;
        if (ride2->vehicle_colours[0].Body != vehicleColour->main)
            continue;
        return false;
    }
    return true;
}

int32_t ride_get_unused_preset_vehicle_colour(uint8_t ride_sub_type)
{
    if (ride_sub_type >= 128)
    {
        return 0;
    }
    rct_ride_entry* rideEntry = get_ride_entry(ride_sub_type);
    if (rideEntry == nullptr)
    {
        return 0;
    }
    vehicle_colour_preset_list* presetList = rideEntry->vehicle_preset_list;
    if (presetList->count == 0)
        return 0;
    if (presetList->count == 255)
        return 255;

    for (int32_t attempt = 0; attempt < 200; attempt++)
    {
        uint8_t numColourConfigurations = presetList->count;
        int32_t randomConfigIndex = util_rand() % numColourConfigurations;
        vehicle_colour* preset = &presetList->list[randomConfigIndex];

        if (ride_does_vehicle_colour_exist(ride_sub_type, preset))
        {
            return randomConfigIndex;
        }
    }
    return 0;
}

/**
 *
 *  rct2: 0x006DE52C
 */
void ride_set_vehicle_colours_to_random_preset(Ride* ride, uint8_t preset_index)
{
    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
    vehicle_colour_preset_list* presetList = rideEntry->vehicle_preset_list;

    if (presetList->count != 0 && presetList->count != 255)
    {
        assert(preset_index < presetList->count);

        ride->colour_scheme_type = RIDE_COLOUR_SCHEME_ALL_SAME;
        vehicle_colour* preset = &presetList->list[preset_index];
        ride->vehicle_colours[0].Body = preset->main;
        ride->vehicle_colours[0].Trim = preset->additional_1;
        ride->vehicle_colours[0].Ternary = preset->additional_2;
    }
    else
    {
        ride->colour_scheme_type = RIDE_COLOUR_SCHEME_DIFFERENT_PER_TRAIN;
        uint32_t count = std::min(presetList->count, (uint8_t)32);
        for (uint32_t i = 0; i < count; i++)
        {
            vehicle_colour* preset = &presetList->list[i];
            ride->vehicle_colours[i].Body = preset->main;
            ride->vehicle_colours[i].Trim = preset->additional_1;
            ride->vehicle_colours[i].Ternary = preset->additional_2;
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
    Ride* ride;
    int32_t i;

    FOR_ALL_RIDES (i, ride)
    {
        if (ride->connected_message_throttle != 0)
            ride->connected_message_throttle--;
        if (ride->status != RIDE_STATUS_OPEN || ride->connected_message_throttle != 0)
            continue;

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
            ride_shop_connected(ride);
        else
            ride_entrance_exit_connected(ride);
    }
}

/**
 *
 *  rct2: 0x006B7C59
 * @return 1 if the coordinate is reachable or has no entrance, 0 otherwise
 */
static int32_t ride_entrance_exit_is_reachable(TileCoordsXYZD coordinates)
{
    int32_t x, y, z;

    if (coordinates.isNull())
        return 1;

    x = coordinates.x;
    y = coordinates.y;
    z = coordinates.z;
    uint8_t face_direction = coordinates.direction;

    x *= 32;
    y *= 32;
    x -= CoordsDirectionDelta[face_direction].x;
    y -= CoordsDirectionDelta[face_direction].y;
    x /= 32;
    y /= 32;

    return map_coord_is_connected(x, y, z, face_direction);
}

static void ride_entrance_exit_connected(Ride* ride)
{
    for (int32_t i = 0; i < MAX_STATIONS; ++i)
    {
        LocationXY8 station_start = ride->stations[i].Start;
        TileCoordsXYZD entrance = ride_get_entrance_location(ride->id, i);
        TileCoordsXYZD exit = ride_get_exit_location(ride->id, i);

        if (station_start.xy == RCT_XY8_UNDEFINED)
            continue;
        if (!entrance.isNull() && !ride_entrance_exit_is_reachable(entrance))
        {
            // name of ride is parameter of the format string
            set_format_arg(0, rct_string_id, ride->name);
            set_format_arg(2, uint32_t, ride->name_arguments);
            if (gConfigNotifications.ride_warnings)
            {
                news_item_add_to_queue(1, STR_ENTRANCE_NOT_CONNECTED, ride->id);
            }
            ride->connected_message_throttle = 3;
        }

        if (!exit.isNull() && !ride_entrance_exit_is_reachable(exit))
        {
            // name of ride is parameter of the format string
            set_format_arg(0, rct_string_id, ride->name);
            set_format_arg(2, uint32_t, ride->name_arguments);
            if (gConfigNotifications.ride_warnings)
            {
                news_item_add_to_queue(1, STR_EXIT_NOT_CONNECTED, ride->id);
            }
            ride->connected_message_throttle = 3;
        }
    }
}

static void ride_shop_connected(Ride* ride)
{
    int32_t x, y, count;
    TileElement* tileElement;

    LocationXY8 coordinates = ride->stations[0].Start;
    if (coordinates.xy == RCT_XY8_UNDEFINED)
        return;

    x = coordinates.x;
    y = coordinates.y;

    tileElement = map_get_first_element_at(x, y);
    do
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK && tileElement->AsTrack()->GetRideIndex() == ride->id)
            break;
    } while (!(tileElement++)->IsLastForTile());

    uint16_t entrance_directions = 0;
    uint8_t track_type = tileElement->AsTrack()->GetTrackType();
    ride = get_ride(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr)
    {
        return;
    }
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
    {
        entrance_directions = FlatRideTrackSequenceProperties[track_type][0];
    }
    else
    {
        entrance_directions = TrackSequenceProperties[track_type][0];
    }

    uint8_t tile_direction = tileElement->GetDirection();
    entrance_directions <<= tile_direction;
    entrance_directions = ((entrance_directions >> 12) | entrance_directions) & 0xF;

    // Now each bit in entrance_directions stands for an entrance direction to check
    if (entrance_directions == 0)
        return;

    // Turn x, y from tiles into units
    x *= 32;
    y *= 32;

    for (count = 0; entrance_directions != 0; count++)
    {
        if (!(entrance_directions & 1))
        {
            entrance_directions >>= 1;
            continue;
        }
        entrance_directions >>= 1;

        // Flip direction north<->south, east<->west
        uint8_t face_direction = direction_reverse(count);

        int32_t y2 = y - CoordsDirectionDelta[face_direction].y;
        int32_t x2 = x - CoordsDirectionDelta[face_direction].x;

        if (map_coord_is_connected(x2 / 32, y2 / 32, tileElement->base_height, face_direction))
            return;
    }

    // Name of ride is parameter of the format string
    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);
    if (gConfigNotifications.ride_warnings)
    {
        news_item_add_to_queue(1, STR_ENTRANCE_NOT_CONNECTED, ride->id);
    }

    ride->connected_message_throttle = 3;
}

#pragma endregion

#pragma region Interface

static void ride_track_set_map_tooltip(TileElement* tileElement)
{
    ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
    auto ride = get_ride(rideIndex);

    set_map_tooltip_format_arg(0, rct_string_id, STR_RIDE_MAP_TIP);
    set_map_tooltip_format_arg(2, rct_string_id, ride->name);
    set_map_tooltip_format_arg(4, uint32_t, ride->name_arguments);

    rct_string_id formatSecondary;
    int32_t arg1 = 0;
    ride_get_status(rideIndex, &formatSecondary, &arg1);
    set_map_tooltip_format_arg(8, rct_string_id, formatSecondary);
    set_map_tooltip_format_arg(10, uint32_t, arg1);
}

static void ride_queue_banner_set_map_tooltip(TileElement* tileElement)
{
    ride_id_t rideIndex = tileElement->AsPath()->GetRideIndex();
    auto ride = get_ride(rideIndex);

    set_map_tooltip_format_arg(0, rct_string_id, STR_RIDE_MAP_TIP);
    set_map_tooltip_format_arg(2, rct_string_id, ride->name);
    set_map_tooltip_format_arg(4, uint32_t, ride->name_arguments);

    rct_string_id formatSecondary;
    int32_t arg1 = 0;
    ride_get_status(rideIndex, &formatSecondary, &arg1);
    set_map_tooltip_format_arg(8, rct_string_id, formatSecondary);
    set_map_tooltip_format_arg(10, uint32_t, arg1);
}

static void ride_station_set_map_tooltip(TileElement* tileElement)
{
    ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
    auto ride = get_ride(rideIndex);
    auto stationIndex = tileElement->AsTrack()->GetStationIndex();
    for (int32_t i = stationIndex; i >= 0; i--)
        if (ride->stations[i].Start.xy == RCT_XY8_UNDEFINED)
            stationIndex--;

    set_map_tooltip_format_arg(0, rct_string_id, STR_RIDE_MAP_TIP);
    set_map_tooltip_format_arg(2, rct_string_id, ride->num_stations <= 1 ? STR_RIDE_STATION : STR_RIDE_STATION_X);
    set_map_tooltip_format_arg(4, rct_string_id, ride->name);
    set_map_tooltip_format_arg(6, uint32_t, ride->name_arguments);
    set_map_tooltip_format_arg(10, rct_string_id, RideComponentNames[RideNameConvention[ride->type].station].capitalised);
    set_map_tooltip_format_arg(12, uint16_t, stationIndex + 1);

    rct_string_id formatSecondary;
    int32_t arg1;
    ride_get_status(rideIndex, &formatSecondary, &arg1);
    set_map_tooltip_format_arg(14, rct_string_id, formatSecondary);
    set_map_tooltip_format_arg(16, uint32_t, arg1);
}

static void ride_entrance_set_map_tooltip(TileElement* tileElement)
{
    ride_id_t rideIndex = tileElement->AsEntrance()->GetRideIndex();
    auto ride = get_ride(rideIndex);

    // Get the station
    auto stationIndex = tileElement->AsEntrance()->GetStationIndex();
    for (int32_t i = stationIndex; i >= 0; i--)
        if (ride->stations[i].Start.xy == RCT_XY8_UNDEFINED)
            stationIndex--;

    if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
    {
        // Get the queue length
        int32_t queueLength = 0;
        if (!ride_get_entrance_location(ride, stationIndex).isNull())
            queueLength = ride->stations[stationIndex].QueueLength;

        set_map_tooltip_format_arg(0, rct_string_id, STR_RIDE_MAP_TIP);
        set_map_tooltip_format_arg(2, rct_string_id, ride->num_stations <= 1 ? STR_RIDE_ENTRANCE : STR_RIDE_STATION_X_ENTRANCE);
        set_map_tooltip_format_arg(4, rct_string_id, ride->name);
        set_map_tooltip_format_arg(6, uint32_t, ride->name_arguments);
        set_map_tooltip_format_arg(12, uint16_t, stationIndex + 1);
        if (queueLength == 0)
        {
            set_map_tooltip_format_arg(14, rct_string_id, STR_QUEUE_EMPTY);
        }
        else if (queueLength == 1)
        {
            set_map_tooltip_format_arg(14, rct_string_id, STR_QUEUE_ONE_PERSON);
        }
        else
        {
            set_map_tooltip_format_arg(14, rct_string_id, STR_QUEUE_PEOPLE);
        }
        set_map_tooltip_format_arg(16, uint16_t, queueLength);
    }
    else
    {
        // Get the station
        stationIndex = tileElement->AsEntrance()->GetStationIndex();
        for (int32_t i = stationIndex; i >= 0; i--)
            if (ride->stations[i].Start.xy == RCT_XY8_UNDEFINED)
                stationIndex--;

        set_map_tooltip_format_arg(0, rct_string_id, ride->num_stations <= 1 ? STR_RIDE_EXIT : STR_RIDE_STATION_X_EXIT);
        set_map_tooltip_format_arg(2, rct_string_id, ride->name);
        set_map_tooltip_format_arg(4, uint32_t, ride->name_arguments);
        set_map_tooltip_format_arg(10, uint16_t, stationIndex + 1);
    }
}

void ride_set_map_tooltip(TileElement* tileElement)
{
    if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
    {
        ride_entrance_set_map_tooltip(tileElement);
    }
    else if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
    {
        if (track_element_is_station(tileElement))
        {
            ride_station_set_map_tooltip(tileElement);
        }
        else
        {
            ride_track_set_map_tooltip(tileElement);
        }
    }
    else if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
    {
        ride_queue_banner_set_map_tooltip(tileElement);
    }
}

static int32_t ride_music_params_update_label_51(
    uint32_t a1, uint8_t* tuneId, ride_id_t rideIndex, int32_t v32, int32_t pan_x, uint16_t sampleRate)
{
    if (a1 < gRideMusicInfoList[*tuneId].length)
    {
        rct_ride_music_params* ride_music_params = gRideMusicParamsListEnd;
        if (ride_music_params < &gRideMusicParamsList[std::size(gRideMusicParamsList)])
        {
            ride_music_params->ride_id = rideIndex;
            ride_music_params->tune_id = *tuneId;
            ride_music_params->offset = a1;
            ride_music_params->volume = v32;
            ride_music_params->pan = pan_x;
            ride_music_params->frequency = sampleRate;
            gRideMusicParamsListEnd++;
        }

        return a1;
    }
    else
    {
        *tuneId = 0xFF;
        return 0;
    }
}

static int32_t ride_music_params_update_label_58(uint32_t position, uint8_t* tuneId)
{
    rct_ride_music_info* ride_music_info = &gRideMusicInfoList[*tuneId];
    position += ride_music_info->offset;
    if (position < ride_music_info->length)
    {
        return position;
    }
    else
    {
        *tuneId = 0xFF;
        return 0;
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
int32_t ride_music_params_update(
    int16_t x, int16_t y, int16_t z, ride_id_t rideIndex, uint16_t sampleRate, uint32_t position, uint8_t* tuneId)
{
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gGameSoundsOff && g_music_tracking_viewport != nullptr)
    {
        const LocationXY16 rotatedCoords = ride_get_rotated_coords(x, y, z);
        rct_viewport* viewport = g_music_tracking_viewport;
        int16_t view_width = viewport->view_width;
        int16_t view_width2 = view_width * 2;
        int16_t view_x = viewport->view_x - view_width2;
        int16_t view_y = viewport->view_y - view_width;
        int16_t view_x2 = view_width2 + view_width2 + viewport->view_width + view_x;
        int16_t view_y2 = view_width + view_width + viewport->view_height + view_y;

        if (view_x >= rotatedCoords.x || view_y >= rotatedCoords.y || view_x2 < rotatedCoords.x || view_y2 < rotatedCoords.y)
        {
            return ride_music_params_update_label_58(position, tuneId);
        }

        int32_t x2 = viewport->x + ((rotatedCoords.x - viewport->view_x) >> viewport->zoom);
        x2 *= 0x10000;
        uint16_t screenwidth = context_get_width();
        if (screenwidth < 64)
        {
            screenwidth = 64;
        }
        int32_t pan_x = ((x2 / screenwidth) - 0x8000) >> 4;

        int32_t y2 = viewport->y + ((rotatedCoords.y - viewport->view_y) >> viewport->zoom);
        y2 *= 0x10000;
        uint16_t screenheight = context_get_height();
        if (screenheight < 64)
        {
            screenheight = 64;
        }
        int32_t pan_y = ((y2 / screenheight) - 0x8000) >> 4;

        uint8_t vol1 = 255;
        uint8_t vol2 = 255;
        int32_t panx2 = pan_x;
        int32_t pany2 = pan_y;
        if (pany2 < 0)
        {
            pany2 = -pany2;
        }
        if (pany2 > 6143)
        {
            pany2 = 6143;
        }
        pany2 -= 2048;
        if (pany2 > 0)
        {
            pany2 = -((pany2 / 4) - 1024) / 4;
            vol1 = (uint8_t)pany2;
            if (pany2 >= 256)
            {
                vol1 = 255;
            }
        }

        if (panx2 < 0)
        {
            panx2 = -panx2;
        }
        if (panx2 > 6143)
        {
            panx2 = 6143;
        }
        panx2 -= 2048;
        if (panx2 > 0)
        {
            panx2 = -((panx2 / 4) - 1024) / 4;
            vol2 = (uint8_t)panx2;
            if (panx2 >= 256)
            {
                vol2 = 255;
            }
        }
        if (vol1 >= vol2)
        {
            vol1 = vol2;
        }
        if (vol1 < gVolumeAdjustZoom * 3)
        {
            vol1 = 0;
        }
        else
        {
            vol1 = vol1 - (gVolumeAdjustZoom * 3);
        }
        int32_t v32 = -(((uint8_t)(-vol1 - 1) * (uint8_t)(-vol1 - 1)) / 16) - 700;
        if (vol1 && v32 >= -4000)
        {
            if (pan_x > 10000)
            {
                pan_x = 10000;
            }
            if (pan_x < -10000)
            {
                pan_x = -10000;
            }
            rct_ride_music* ride_music = &gRideMusicList[0];
            int32_t channel = 0;
            uint32_t a1;
            while (ride_music->ride_id != rideIndex || ride_music->tune_id != *tuneId)
            {
                ride_music++;
                channel++;
                if (channel >= AUDIO_MAX_RIDE_MUSIC)
                {
                    rct_ride_music_info* ride_music_info = &gRideMusicInfoList[*tuneId];
                    a1 = position + ride_music_info->offset;

                    return ride_music_params_update_label_51(a1, tuneId, rideIndex, v32, pan_x, sampleRate);
                }
            }
            int32_t playing = Mixer_Channel_IsPlaying(gRideMusicList[channel].sound_channel);
            if (!playing)
            {
                *tuneId = 0xFF;
                return 0;
            }
            a1 = (uint32_t)Mixer_Channel_GetOffset(gRideMusicList[channel].sound_channel);

            return ride_music_params_update_label_51(a1, tuneId, rideIndex, v32, pan_x, sampleRate);
        }
        else
        {
            return ride_music_params_update_label_58(position, tuneId);
        }
    }
    return position;
}

/**
 *  Play/update ride music based on structs updated in 0x006BC3AC
 *  rct2: 0x006BC6D8
 */
void ride_music_update_final()
{
    rct_ride_music_params* edi = nullptr;
    int32_t ebx = 0;
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR))
    {
        // TODO Allow circus music (CSS24) to play if ride music is disabled (that should be sound)
        if (!gGameSoundsOff && gConfigSound.ride_music_enabled && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            while (1)
            {
                int32_t v8 = 0;
                int32_t v9 = 1;
                rct_ride_music_params* ride_music_params = &gRideMusicParamsList[0];
                while (ride_music_params < gRideMusicParamsListEnd)
                {
                    if (ride_music_params->ride_id != (uint8_t)-1)
                    {
                        v8++;
                        if (v9 >= ride_music_params->volume)
                        {
                            v9 = ride_music_params->volume;
                            edi = ride_music_params;
                        }
                    }
                    ride_music_params++;
                }
                if (v8 <= 2)
                {
                    break;
                }
                edi->ride_id = RIDE_ID_NULL;
            }

            // stop currently playing music that is not in music params list or not playing?
            rct_ride_music* ride_music = &gRideMusicList[0];
            int32_t channel = 0;
            do
            {
                if (ride_music->ride_id != RIDE_ID_NULL)
                {
                    rct_ride_music_params* ride_music_params = &gRideMusicParamsList[0];
                    int32_t isplaying = 0;
                    while (ride_music_params < gRideMusicParamsListEnd && !isplaying)
                    {
                        if (ride_music_params->ride_id == ride_music->ride_id
                            && ride_music_params->tune_id == ride_music->tune_id)
                        {
                            isplaying = Mixer_Channel_IsPlaying(gRideMusicList[channel].sound_channel);
                            break;
                        }
                        ride_music_params++;
                    }
                    if (!isplaying)
                    {
                        Mixer_Stop_Channel(gRideMusicList[channel].sound_channel);
                        ride_music->ride_id = RIDE_ID_NULL;
                    }
                }
                ride_music++;
                channel++;
            } while (channel < AUDIO_MAX_RIDE_MUSIC);

            for (rct_ride_music_params* ride_music_params = &gRideMusicParamsList[0];
                 ride_music_params < gRideMusicParamsListEnd; ride_music_params++)
            {
                if (ride_music_params->ride_id != RIDE_ID_NULL)
                {
                    rct_ride_music* ride_music_2 = &gRideMusicList[0];
                    int32_t channel2 = 0;
                    while (ride_music_params->ride_id != ride_music_2->ride_id
                           || ride_music_params->tune_id != ride_music_2->tune_id)
                    {
                        if (ride_music_2->ride_id == RIDE_ID_NULL)
                        {
                            ebx = channel2;
                        }
                        ride_music_2++;
                        channel2++;
                        if (channel2 >= AUDIO_MAX_RIDE_MUSIC)
                        {
                            rct_ride_music_info* ride_music_info = &gRideMusicInfoList[ride_music_params->tune_id];
                            rct_ride_music* ride_music_3 = &gRideMusicList[ebx];
                            ride_music_3->sound_channel = Mixer_Play_Music(ride_music_info->path_id, MIXER_LOOP_NONE, true);
                            if (ride_music_3->sound_channel)
                            {
                                ride_music_3->volume = ride_music_params->volume;
                                ride_music_3->pan = ride_music_params->pan;
                                ride_music_3->frequency = ride_music_params->frequency;
                                ride_music_3->ride_id = ride_music_params->ride_id;
                                ride_music_3->tune_id = ride_music_params->tune_id;
                                Mixer_Channel_Volume(ride_music_3->sound_channel, DStoMixerVolume(ride_music_3->volume));
                                Mixer_Channel_Pan(ride_music_3->sound_channel, DStoMixerPan(ride_music_3->pan));
                                Mixer_Channel_Rate(ride_music_3->sound_channel, DStoMixerRate(ride_music_3->frequency));
                                int32_t offset = ride_music_params->offset - 10000;
                                if (offset < 0)
                                {
                                    offset = 0;
                                }
                                Mixer_Channel_SetOffset(ride_music_3->sound_channel, offset);

                                // Move circus music to the sound mixer group
                                if (ride_music_info->path_id == PATH_ID_CSS24)
                                {
                                    Mixer_Channel_SetGroup(ride_music_3->sound_channel, MIXER_GROUP_SOUND);
                                }
                            }
                            return;
                        }
                    }

                    if (ride_music_params->volume != ride_music_2->volume)
                    {
                        ride_music_2->volume = ride_music_params->volume;
                        Mixer_Channel_Volume(ride_music_2->sound_channel, DStoMixerVolume(ride_music_2->volume));
                    }
                    if (ride_music_params->pan != ride_music_2->pan)
                    {
                        ride_music_2->pan = ride_music_params->pan;
                        Mixer_Channel_Pan(ride_music_2->sound_channel, DStoMixerPan(ride_music_2->pan));
                    }
                    if (ride_music_params->frequency != ride_music_2->frequency)
                    {
                        ride_music_2->frequency = ride_music_params->frequency;
                        Mixer_Channel_Rate(ride_music_2->sound_channel, DStoMixerRate(ride_music_2->frequency));
                    }
                }
            }
        }
    }
}

#pragma endregion

static bool ride_is_mode_valid(Ride* ride, uint8_t mode)
{
    const uint8_t* availableModes = ride_seek_available_modes(ride);

    for (; *availableModes != 0xFF; availableModes++)
    {
        if (*availableModes == mode)
        {
            return true;
        }
    }

    return false;
}

static bool ride_is_valid_lift_hill_speed(Ride* ride, int32_t speed)
{
    int32_t minSpeed = gCheatsFastLiftHill ? 0 : RideLiftData[ride->type].minimum_speed;
    int32_t maxSpeed = gCheatsFastLiftHill ? 255 : RideLiftData[ride->type].maximum_speed;
    return speed >= minSpeed && speed <= maxSpeed;
}

static bool ride_is_valid_num_circuits(int32_t numCircuits)
{
    int32_t minNumCircuits = 1;
    int32_t maxNumCircuits = gCheatsFastLiftHill ? 255 : 20;
    return numCircuits >= minNumCircuits && numCircuits <= maxNumCircuits;
}

static bool ride_is_valid_operation_option(Ride* ride, uint8_t value)
{
    uint8_t minValue = RideProperties[ride->type].min_value;
    uint8_t maxValue = RideProperties[ride->type].max_value;
    if (gCheatsFastLiftHill)
    {
        minValue = 0;
        maxValue = 255;
    }

    return value >= minValue && value <= maxValue;
}

static money32 ride_set_setting(ride_id_t rideIndex, uint8_t setting, uint8_t value, uint8_t flags)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS;

    Ride* ride = get_ride(rideIndex);
    if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
    {
        log_warning("Invalid ride: #%u.", rideIndex);
        return MONEY32_UNDEFINED;
    }

    switch (setting)
    {
        case RIDE_SETTING_MODE:
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
            {
                gGameCommandErrorText = STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING;
                return MONEY32_UNDEFINED;
            }

            if (ride->status != RIDE_STATUS_CLOSED)
            {
                gGameCommandErrorText = STR_MUST_BE_CLOSED_FIRST;
                return MONEY32_UNDEFINED;
            }

            if (!ride_is_mode_valid(ride, value))
            {
                log_warning("Invalid ride mode.");
                return MONEY32_UNDEFINED;
            }

            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                invalidate_test_results(ride);
                ride_clear_for_construction(ride);
                ride_remove_peeps(ride);

                ride->mode = value;
                ride_update_max_vehicles(ride);
            }
            break;
        case RIDE_SETTING_DEPARTURE:
            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride->depart_flags = value;
            }
            break;
        case RIDE_SETTING_MIN_WAITING_TIME:
            if (value > 250)
            {
                log_warning("Invalid minimum waiting time.");
                return MONEY32_UNDEFINED;
            }

            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride->min_waiting_time = value;
                ride->max_waiting_time = std::max(value, ride->max_waiting_time);
            }
            break;
        case RIDE_SETTING_MAX_WAITING_TIME:
            if (value > 250)
            {
                log_warning("Invalid maximum waiting time.");
                return MONEY32_UNDEFINED;
            }

            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride->max_waiting_time = value;
                ride->min_waiting_time = std::min(value, ride->min_waiting_time);
            }
            break;
        case RIDE_SETTING_OPERATION_OPTION:
            if (!ride_is_valid_operation_option(ride, value))
            {
                log_warning("Invalid operation option value.");
                return MONEY32_UNDEFINED;
            }

            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                invalidate_test_results(ride);
                ride->operation_option = value;
            }
            break;
        case RIDE_SETTING_INSPECTION_INTERVAL:
            if (value > RIDE_INSPECTION_NEVER)
            {
                log_warning("Invalid inspection interval.");
                return MONEY32_UNDEFINED;
            }

            if (value == RIDE_INSPECTION_NEVER)
            {
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
            }

            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride->inspection_interval = value;
            }
            break;
        case RIDE_SETTING_MUSIC:
            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_MUSIC;
                if (value)
                {
                    ride->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
                }
            }
            break;
        case RIDE_SETTING_MUSIC_TYPE:
            if (value >= MUSIC_STYLE_COUNT)
            {
                log_warning("Invalid music style.");
                return MONEY32_UNDEFINED;
            }

            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                if (value != ride->music)
                {
                    ride->music = value;
                    ride->music_tune_id = 0xFF;
                }
            }
            break;
        case RIDE_SETTING_LIFT_HILL_SPEED:
            if (!ride_is_valid_lift_hill_speed(ride, value))
            {
                log_warning("Invalid lift hill speed.");
                return MONEY32_UNDEFINED;
            }

            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                if (value != ride->lift_hill_speed)
                {
                    ride->lift_hill_speed = value;
                    invalidate_test_results(ride);
                }
            }
            break;
        case RIDE_SETTING_NUM_CIRCUITS:
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT && value > 1)
            {
                gGameCommandErrorText = STR_MULTICIRCUIT_NOT_POSSIBLE_WITH_CABLE_LIFT_HILL;
                return MONEY32_UNDEFINED;
            }

            if (!ride_is_valid_num_circuits(value))
            {
                log_warning("Invalid number of circuits.");
                return MONEY32_UNDEFINED;
            }

            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                if (value != ride->num_circuits)
                {
                    ride->num_circuits = value;
                    invalidate_test_results(ride);
                }
            }
            break;
        case RIDE_SETTING_RIDE_TYPE:
            if (!gCheatsAllowArbitraryRideTypeChanges)
            {
                return MONEY32_UNDEFINED;
            }
            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride->type = value;
            }
            break;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
        {
            LocationXYZ16 coord;
            coord.x = ride->overall_view.x * 32 + 16;
            coord.y = ride->overall_view.y * 32 + 16;
            coord.z = tile_element_height(coord.x, coord.y);
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
void game_command_set_ride_setting(
    [[maybe_unused]] int32_t* eax, int32_t* ebx, [[maybe_unused]] int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi,
    [[maybe_unused]] int32_t* edi, [[maybe_unused]] int32_t* ebp)
{
    ride_id_t rideIndex = *edx & 0xFF;
    uint8_t setting = (*edx >> 8) & 0xFF;
    uint8_t newValue = (*ebx >> 8) & 0xFF;
    uint8_t flags = *ebx & 0xFF;
    *ebx = ride_set_setting(rideIndex, setting, newValue, flags);
}

/**
 *
 *  rct2: 0x006B4CC1
 */
static int32_t ride_mode_check_valid_station_numbers(Ride* ride)
{
    uint8_t no_stations = 0;
    for (uint8_t station_index = 0; station_index < MAX_STATIONS; ++station_index)
    {
        if (ride->stations[station_index].Start.xy != RCT_XY8_UNDEFINED)
        {
            no_stations++;
        }
    }

    switch (ride->mode)
    {
        case RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE:
        case RIDE_MODE_POWERED_LAUNCH_PASSTROUGH:
        case RIDE_MODE_POWERED_LAUNCH:
        case RIDE_MODE_LIM_POWERED_LAUNCH:
            if (no_stations <= 1)
                return 1;
            gGameCommandErrorText = STR_UNABLE_TO_OPERATE_WITH_MORE_THAN_ONE_STATION_IN_THIS_MODE;
            return 0;
        case RIDE_MODE_SHUTTLE:
            if (no_stations >= 2)
                return 1;
            gGameCommandErrorText = STR_UNABLE_TO_OPERATE_WITH_LESS_THAN_TWO_STATIONS_IN_THIS_MODE;
            return 0;
    }

    if (ride->type == RIDE_TYPE_GO_KARTS || ride->type == RIDE_TYPE_MINI_GOLF)
    {
        if (no_stations <= 1)
            return 1;
        gGameCommandErrorText = STR_UNABLE_TO_OPERATE_WITH_MORE_THAN_ONE_STATION_IN_THIS_MODE;
        return 0;
    }

    return 1;
}

/**
 * returns stationIndex of first station on success
 * -1 on failure.
 */
static int32_t ride_mode_check_station_present(Ride* ride)
{
    int32_t stationIndex = ride_get_first_valid_station_start(ride);

    if (stationIndex == -1)
    {
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
static int32_t ride_check_for_entrance_exit(ride_id_t rideIndex)
{
    Ride* ride = get_ride(rideIndex);

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
        return 1;

    int32_t i;
    uint8_t entrance = 0;
    uint8_t exit = 0;
    for (i = 0; i < MAX_STATIONS; i++)
    {
        if (ride->stations[i].Start.xy == RCT_XY8_UNDEFINED)
            continue;

        if (!ride_get_entrance_location(ride, i).isNull())
        {
            entrance = 1;
        }

        if (!ride_get_exit_location(ride, i).isNull())
        {
            exit = 1;
        }

        // If station start and no entrance/exit
        // Sets same error message as no entrance
        if (ride_get_exit_location(ride, i).isNull() && ride_get_entrance_location(ride, i).isNull())
        {
            entrance = 0;
            break;
        }
    }

    if (entrance == 0)
    {
        gGameCommandErrorText = STR_ENTRANCE_NOT_YET_BUILT;
        return 0;
    }

    if (exit == 0)
    {
        gGameCommandErrorText = STR_EXIT_NOT_YET_BUILT;
        return 0;
    }

    return 1;
}

/**
 *
 *  rct2: 0x006B5952
 */
static void sub_6B5952(ride_id_t rideIndex)
{
    for (int32_t i = 0; i < MAX_STATIONS; i++)
    {
        TileCoordsXYZD location = ride_get_entrance_location(rideIndex, i);
        if (location.isNull())
            continue;

        int32_t x = location.x * 32;
        int32_t y = location.y * 32;
        int32_t z = location.z;

        // This will fire for every entrance on this x, y and z, regardless whether that actually belongs to
        // the ride or not.
        TileElement* tileElement = map_get_first_element_at(location.x, location.y);
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;
            if (tileElement->base_height != z)
                continue;

            int32_t direction = tileElement->GetDirection();
            footpath_chain_ride_queue(rideIndex, i, x, y, tileElement, direction_reverse(direction));
        } while (!(tileElement++)->IsLastForTile());
    }
}

/**
 *
 *  rct2: 0x006D3319
 */
static int32_t ride_check_block_brakes(CoordsXYE* input, CoordsXYE* output)
{
    rct_window* w;
    track_circuit_iterator it;
    int32_t type;

    ride_id_t rideIndex = input->element->AsTrack()->GetRideIndex();
    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != nullptr && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && _currentRideIndex == rideIndex)
        ride_construction_invalidate_current_track();

    track_circuit_iterator_begin(&it, *input);
    while (track_circuit_iterator_next(&it))
    {
        if (it.current.element->AsTrack()->GetTrackType() == TRACK_ELEM_BLOCK_BRAKES)
        {
            type = it.last.element->AsTrack()->GetTrackType();
            if (type == TRACK_ELEM_END_STATION)
            {
                gGameCommandErrorText = STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_STATION;
                *output = it.current;
                return 0;
            }
            if (type == TRACK_ELEM_BLOCK_BRAKES)
            {
                gGameCommandErrorText = STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_EACH_OTHER;
                *output = it.current;
                return 0;
            }
            if (it.last.element->AsTrack()->HasChain() && type != TRACK_ELEM_LEFT_CURVED_LIFT_HILL
                && type != TRACK_ELEM_RIGHT_CURVED_LIFT_HILL)
            {
                gGameCommandErrorText = STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_THE_TOP_OF_THIS_LIFT_HILL;
                *output = it.current;
                return 0;
            }
        }
    }
    if (!it.looped)
    {
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
static bool ride_check_track_contains_inversions(CoordsXYE* input, CoordsXYE* output)
{
    ride_id_t rideIndex = input->element->AsTrack()->GetRideIndex();
    Ride* ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_MAZE)
        return true;

    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != nullptr && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && rideIndex == _currentRideIndex)
    {
        ride_construction_invalidate_current_track();
    }

    bool moveSlowIt = true;
    track_circuit_iterator it, slowIt;
    track_circuit_iterator_begin(&it, *input);
    slowIt = it;

    while (track_circuit_iterator_next(&it))
    {
        int32_t trackType = it.current.element->AsTrack()->GetTrackType();
        if (TrackFlags[trackType] & TRACK_ELEM_FLAG_INVERSION_TO_NORMAL)
        {
            *output = it.current;
            return true;
        }

        // Prevents infinite loops
        moveSlowIt = !moveSlowIt;
        if (moveSlowIt)
        {
            track_circuit_iterator_next(&slowIt);
            if (track_circuit_iterators_match(&it, &slowIt))
            {
                return false;
            }
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
static bool ride_check_track_contains_banked(CoordsXYE* input, CoordsXYE* output)
{
    ride_id_t rideIndex = input->element->AsTrack()->GetRideIndex();
    Ride* ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_MAZE)
        return true;

    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != nullptr && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && rideIndex == _currentRideIndex)
    {
        ride_construction_invalidate_current_track();
    }

    bool moveSlowIt = true;
    track_circuit_iterator it, slowIt;
    track_circuit_iterator_begin(&it, *input);
    slowIt = it;

    while (track_circuit_iterator_next(&it))
    {
        int32_t trackType = output->element->AsTrack()->GetTrackType();
        if (TrackFlags[trackType] & TRACK_ELEM_FLAG_BANKED)
        {
            *output = it.current;
            return true;
        }

        // Prevents infinite loops
        moveSlowIt = !moveSlowIt;
        if (moveSlowIt)
        {
            track_circuit_iterator_next(&slowIt);
            if (track_circuit_iterators_match(&it, &slowIt))
            {
                return false;
            }
        }
    }
    return false;
}

/**
 *
 *  rct2: 0x006CB25D
 */
static int32_t ride_check_station_length(CoordsXYE* input, CoordsXYE* output)
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != nullptr && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0
        && _currentRideIndex == input->element->AsTrack()->GetRideIndex())
    {
        ride_construction_invalidate_current_track();
    }

    output->x = input->x;
    output->y = input->y;
    output->element = input->element;
    track_begin_end trackBeginEnd;
    while (track_block_get_previous(output->x, output->y, output->element, &trackBeginEnd))
    {
        output->x = trackBeginEnd.begin_x;
        output->y = trackBeginEnd.begin_y;
        output->element = trackBeginEnd.begin_element;
    }

    int32_t num_station_elements = 0;
    CoordsXYE last_good_station = *output;

    do
    {
        if (TrackSequenceProperties[output->element->AsTrack()->GetTrackType()][0] & TRACK_SEQUENCE_FLAG_ORIGIN)
        {
            num_station_elements++;
            last_good_station = *output;
        }
        else
        {
            if (num_station_elements == 0)
                continue;
            if (num_station_elements == 1)
            {
                return 0;
            }
            num_station_elements = 0;
        }
    } while (track_block_get_next(output, output, nullptr, nullptr));

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
static bool ride_check_start_and_end_is_station(CoordsXYE* input)
{
    int32_t trackType;
    CoordsXYE trackBack, trackFront;

    ride_id_t rideIndex = input->element->AsTrack()->GetRideIndex();
    auto ride = get_ride(rideIndex);

    auto w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w != nullptr && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && rideIndex == _currentRideIndex)
    {
        ride_construction_invalidate_current_track();
    }

    // Check back of the track
    track_get_back(input, &trackBack);
    trackType = trackBack.element->AsTrack()->GetTrackType();
    if (!(TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
    {
        return false;
    }
    ride->chairlift_bullwheel_location[0].x = trackBack.x >> 5;
    ride->chairlift_bullwheel_location[0].y = trackBack.y >> 5;
    ride->chairlift_bullwheel_z[0] = trackBack.element->base_height;

    // Check front of the track
    track_get_front(input, &trackFront);
    trackType = trackFront.element->AsTrack()->GetTrackType();
    if (!(TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
    {
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
static void ride_set_boat_hire_return_point(Ride* ride, CoordsXYE* startElement)
{
    int32_t trackType = -1;
    int32_t returnX = startElement->x;
    int32_t returnY = startElement->y;
    int32_t startX = returnX;
    int32_t startY = returnY;
    TileElement* returnTrackElement = startElement->element;
    track_begin_end trackBeginEnd;
    while (track_block_get_previous(returnX, returnY, returnTrackElement, &trackBeginEnd))
    {
        // If previous track is back to the starting x, y, then break loop (otherwise possible infinite loop)
        if (trackType != -1 && startX == trackBeginEnd.begin_x && startY == trackBeginEnd.begin_y)
            break;

        int32_t x = trackBeginEnd.begin_x;
        int32_t y = trackBeginEnd.begin_y;
        int32_t z = trackBeginEnd.begin_z;
        int32_t direction = trackBeginEnd.begin_direction;
        trackType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
        sub_6C683D(&x, &y, &z, direction, trackType, 0, &returnTrackElement, 0);
        returnX = x;
        returnY = y;
    };

    trackType = returnTrackElement->AsTrack()->GetTrackType();
    int32_t elementReturnDirection = TrackCoordinates[trackType].rotation_begin;
    ride->boat_hire_return_direction = returnTrackElement->GetDirectionWithOffset(elementReturnDirection);
    ride->boat_hire_return_position.x = returnX >> 5;
    ride->boat_hire_return_position.y = returnY >> 5;
}

/**
 *
 *  rct2: 0x006B4D39
 */
static void ride_set_maze_entrance_exit_points(Ride* ride)
{
    // Needs room for an entrance and an exit per station, plus one position for the list terminator.
    TileCoordsXYZD positions[(MAX_STATIONS * 2) + 1];

    // Create a list of all the entrance and exit positions
    TileCoordsXYZD* position = positions;
    for (int32_t i = 0; i < MAX_STATIONS; i++)
    {
        const auto entrance = ride_get_entrance_location(ride, i);
        const auto exit = ride_get_exit_location(ride, i);

        if (!entrance.isNull())
        {
            *position++ = entrance;
        }
        if (!exit.isNull())
        {
            *position++ = exit;
        }
    }
    (*position++).x = COORDS_NULL;

    // Enumerate entrance and exit positions
    for (position = positions; !(*position).isNull(); position++)
    {
        int32_t x = (*position).x << 5;
        int32_t y = (*position).y << 5;
        int32_t z = (*position).z;

        TileElement* tileElement = map_get_first_element_at((*position).x, (*position).y);
        do
        {
            if (tileElement == nullptr)
                break;
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;
            if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE
                && tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_EXIT)
            {
                continue;
            }
            if (tileElement->base_height != z)
                continue;

            maze_entrance_hedge_removal(x, y, tileElement);
        } while (!(tileElement++)->IsLastForTile());
    }
}

/**
 * Sets a flag on all the track elements that can be the start of a circuit block. i.e. where a train can start.
 *  rct2: 0x006B4E6B
 */
static void ride_set_block_points(CoordsXYE* startElement)
{
    CoordsXYE currentElement = *startElement;
    do
    {
        int32_t trackType = currentElement.element->AsTrack()->GetTrackType();
        switch (trackType)
        {
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
    } while (track_block_get_next(&currentElement, &currentElement, nullptr, nullptr)
             && currentElement.element != startElement->element);
}

/**
 *
 *  rct2: 0x006B4D26
 */
static void ride_set_start_finish_points(ride_id_t rideIndex, CoordsXYE* startElement)
{
    Ride* ride = get_ride(rideIndex);

    switch (ride->type)
    {
        case RIDE_TYPE_BOAT_HIRE:
            ride_set_boat_hire_return_point(ride, startElement);
            break;
        case RIDE_TYPE_MAZE:
            ride_set_maze_entrance_exit_points(ride);
            break;
    }

    if (ride_is_block_sectioned(ride) && !(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
    {
        ride_set_block_points(startElement);
    }
}

/**
 *
 *  rct2: 0x0069ED9E
 */
static int32_t count_free_misc_sprite_slots()
{
    int32_t miscSpriteCount = gSpriteListCount[SPRITE_LIST_MISC];
    int32_t remainingSpriteCount = gSpriteListCount[SPRITE_LIST_NULL];
    return std::max(0, miscSpriteCount + remainingSpriteCount - 300);
}

static constexpr const LocationXY16 word_9A3AB4[4] = {
    { 0, 0 },
    { 0, -96 },
    { -96, -96 },
    { -96, 0 },
};

// clang-format off
static constexpr const LocationXY16 word_9A2A60[] = {
    { 0, 16 },
    { 16, 31 },
    { 31, 16 },
    { 16, 0 },
    { 16, 16 },
    { 64, 64 },
    { 64, -32 },
    { -32, -32 },
    { -32, 64 },
};
// clang-format on

/**
 *
 *  rct2: 0x006DD90D
 */
static rct_vehicle* vehicle_create_car(
    ride_id_t rideIndex, int32_t vehicleEntryIndex, int32_t carIndex, int32_t vehicleIndex, int32_t x, int32_t y, int32_t z,
    int32_t* remainingDistance, TileElement* tileElement)
{
    Ride* ride = get_ride(rideIndex);
    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
    rct_ride_entry_vehicle* vehicleEntry = &rideEntry->vehicles[vehicleEntryIndex];
    int32_t edx;

    rct_vehicle* vehicle = (rct_vehicle*)create_sprite(1);
    vehicle->sprite_identifier = SPRITE_IDENTIFIER_VEHICLE;
    vehicle->ride = rideIndex;
    vehicle->ride_subtype = ride->subtype;

    vehicle->vehicle_type = vehicleEntryIndex;
    vehicle->is_child = carIndex == 0 ? 0 : 1;
    vehicle->var_44 = ror32(vehicleEntry->spacing, 10) & 0xFFFF;
    edx = vehicleEntry->spacing >> 1;
    *remainingDistance -= edx;
    vehicle->remaining_distance = *remainingDistance;
    if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_GO_KART))
    {
        *remainingDistance -= edx;
    }

    // loc_6DD9A5:
    vehicle->sprite_width = vehicleEntry->sprite_width;
    vehicle->sprite_height_negative = vehicleEntry->sprite_height_negative;
    vehicle->sprite_height_positive = vehicleEntry->sprite_height_positive;
    vehicle->mass = vehicleEntry->car_mass;
    vehicle->num_seats = vehicleEntry->num_seats;
    vehicle->speed = vehicleEntry->powered_max_speed;
    vehicle->powered_acceleration = vehicleEntry->powered_acceleration;
    vehicle->velocity = 0;
    vehicle->acceleration = 0;
    vehicle->swing_sprite = 0;
    vehicle->swinging_car_var_0 = 0;
    vehicle->var_4E = 0;
    vehicle->restraints_position = 0;
    vehicle->spin_sprite = 0;
    vehicle->spin_speed = 0;
    vehicle->sound2_flags = 0;
    vehicle->sound1_id = RCT12_SOUND_ID_NULL;
    vehicle->sound2_id = RCT12_SOUND_ID_NULL;
    vehicle->next_vehicle_on_train = SPRITE_INDEX_NULL;
    vehicle->var_C4 = 0;
    vehicle->animation_frame = 0;
    vehicle->var_C8 = 0;
    vehicle->scream_sound_id = 255;
    vehicle->vehicle_sprite_type = 0;
    vehicle->bank_rotation = 0;
    vehicle->target_seat_rotation = 4;
    vehicle->seat_rotation = 4;
    for (int32_t i = 0; i < 32; i++)
    {
        vehicle->peep[i] = SPRITE_INDEX_NULL;
    }

    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT)
    {
        // loc_6DDCA4:
        vehicle->var_CD = 0;
        int32_t direction = tileElement->GetDirection();
        x += word_9A3AB4[direction].x;
        y += word_9A3AB4[direction].y;
        z = tileElement->base_height * 8;
        vehicle->track_x = x;
        vehicle->track_y = y;
        vehicle->track_z = z;
        vehicle->current_station = tileElement->AsTrack()->GetStationIndex();

        z += RideData5[ride->type].z_offset;

        vehicle->track_type = tileElement->AsTrack()->GetTrackType() << 2;
        vehicle->track_progress = 0;
        vehicle->status = 0;
        vehicle->sub_state = 0;
        vehicle->update_flags = 0;

        LocationXY16 chosenLoc;
        // loc_6DDD26:
        do
        {
            vehicle->sprite_direction = scenario_rand() & 0x1E;
            chosenLoc.y = y + (scenario_rand() & 0xFF);
            chosenLoc.x = x + (scenario_rand() & 0xFF);
        } while (vehicle_update_dodgems_collision(vehicle, chosenLoc.x, chosenLoc.y, nullptr));

        sprite_move(chosenLoc.x, chosenLoc.y, z, (rct_sprite*)vehicle);
    }
    else
    {
        int16_t dl = 0;
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_CHAIRLIFT)
        {
            dl = 1;
        }

        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_GO_KART)
        {
            // Choose which lane Go Kart should start in
            dl = 5;
            if (vehicleIndex & 1)
            {
                dl = 6;
            }
        }
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_MINI_GOLF)
        {
            dl = 9;
            vehicle->var_D3 = 0;
            vehicle->mini_golf_current_animation = 0;
            vehicle->mini_golf_flags = 0;
        }
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_4)
        {
            if (!vehicle->is_child)
            {
                dl = 15;
            }
        }
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_5)
        {
            dl = 16;
        }
        vehicle->var_CD = dl;

        vehicle->track_x = x;
        vehicle->track_y = y;

        int32_t direction = tileElement->GetDirection();
        vehicle->sprite_direction = direction << 3;

        if (ride->type == RIDE_TYPE_SPACE_RINGS)
        {
            direction = 4;
        }
        else
        {
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_16))
            {
                if (RideConstructionDefaultTrackType[ride->type] != FLAT_TRACK_ELEM_1_X_4_B)
                {
                    if (RideConstructionDefaultTrackType[ride->type] != FLAT_TRACK_ELEM_1_X_4_A)
                    {
                        if (ride->type == RIDE_TYPE_ENTERPRISE)
                        {
                            direction += 5;
                        }
                        else
                        {
                            direction = 4;
                        }
                    }
                }
            }
        }

        x += word_9A2A60[direction].x;
        y += word_9A2A60[direction].y;
        vehicle->track_z = tileElement->base_height * 8;

        vehicle->current_station = tileElement->AsTrack()->GetStationIndex();
        z = tileElement->base_height * 8;
        z += RideData5[ride->type].z_offset;

        sprite_move(x, y, z, (rct_sprite*)vehicle);
        vehicle->track_type = (tileElement->AsTrack()->GetTrackType() << 2) | (vehicle->sprite_direction >> 3);
        vehicle->track_progress = 31;
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_MINI_GOLF)
        {
            vehicle->track_progress = 15;
        }
        vehicle->update_flags = VEHICLE_UPDATE_FLAG_1;
        if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET)
        {
            if (tileElement->AsTrack()->IsInverted())
            {
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
static train_ref vehicle_create_train(
    ride_id_t rideIndex, int32_t x, int32_t y, int32_t z, int32_t vehicleIndex, int32_t* remainingDistance,
    TileElement* tileElement)
{
    Ride* ride = get_ride(rideIndex);

    train_ref train = { nullptr, nullptr };
    for (int32_t carIndex = 0; carIndex < ride->num_cars_per_train; carIndex++)
    {
        const uint8_t vehicle = ride_entry_get_vehicle_at_position(ride->subtype, ride->num_cars_per_train, carIndex);
        rct_vehicle* car = vehicle_create_car(
            rideIndex, vehicle, carIndex, vehicleIndex, x, y, z, remainingDistance, tileElement);
        if (carIndex == 0)
        {
            train.head = car;
        }
        else
        {
            // Link the previous car with this car
            train.tail->next_vehicle_on_train = car->sprite_index;
            train.tail->next_vehicle_on_ride = car->sprite_index;
            car->prev_vehicle_on_ride = train.tail->sprite_index;
        }
        train.tail = car;
    }
    return train;
}

static void vehicle_create_trains(ride_id_t rideIndex, int32_t x, int32_t y, int32_t z, TileElement* tileElement)
{
    Ride* ride = get_ride(rideIndex);
    train_ref firstTrain = {};
    train_ref lastTrain = {};
    int32_t remainingDistance = 0;

    for (int32_t vehicleIndex = 0; vehicleIndex < ride->num_vehicles; vehicleIndex++)
    {
        if (ride_is_block_sectioned(ride))
        {
            remainingDistance = 0;
        }
        train_ref train = vehicle_create_train(rideIndex, x, y, z, vehicleIndex, &remainingDistance, tileElement);
        if (vehicleIndex == 0)
        {
            firstTrain = train;
        }
        else
        {
            // Link the end of the previous train with the front of this train
            lastTrain.tail->next_vehicle_on_ride = train.head->sprite_index;
            train.head->prev_vehicle_on_ride = lastTrain.tail->sprite_index;
        }
        lastTrain = train;

        // Add train to ride vehicle list
        move_sprite_to_list((rct_sprite*)train.head, SPRITE_LIST_TRAIN * 2);
        for (int32_t i = 0; i < MAX_VEHICLES_PER_RIDE; i++)
        {
            if (ride->vehicles[i] == SPRITE_INDEX_NULL)
            {
                ride->vehicles[i] = train.head->sprite_index;
                break;
            }
        }
    }

    // Link the first train and last train together. Nullptr checks are there to keep Clang happy.
    if (lastTrain.tail != nullptr)
        firstTrain.head->prev_vehicle_on_ride = lastTrain.tail->sprite_index;
    if (firstTrain.head != nullptr)
        lastTrain.tail->next_vehicle_on_ride = firstTrain.head->sprite_index;
}

static void vehicle_unset_update_flag_b1(rct_vehicle* head)
{
    rct_vehicle* vehicle = head;
    while (true)
    {
        vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_1;
        uint16_t spriteIndex = vehicle->next_vehicle_on_train;
        if (spriteIndex == SPRITE_INDEX_NULL)
        {
            break;
        }
        vehicle = GET_VEHICLE(spriteIndex);
    }
}

/**
 *
 *  rct2: 0x006DDE9E
 */
static void ride_create_vehicles_find_first_block(Ride* ride, CoordsXYE* outXYElement)
{
    rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[0]);
    int32_t firstX = vehicle->track_x;
    int32_t firstY = vehicle->track_y;
    int32_t firstZ = vehicle->track_z;
    TileElement* firstElement = map_get_track_element_at(firstX, firstY, firstZ / 8);

    assert(firstElement != nullptr);

    int32_t x = firstX;
    int32_t y = firstY;
    TileElement* trackElement = firstElement;
    track_begin_end trackBeginEnd;
    while (track_block_get_previous(x, y, trackElement, &trackBeginEnd))
    {
        x = trackBeginEnd.end_x;
        y = trackBeginEnd.end_y;
        trackElement = trackBeginEnd.begin_element;
        if (x == firstX && y == firstY && trackElement == firstElement)
        {
            break;
        }

        int32_t trackType = trackElement->AsTrack()->GetTrackType();
        switch (trackType)
        {
            case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            case TRACK_ELEM_60_DEG_UP_TO_FLAT:
                if (trackElement->AsTrack()->HasChain())
                {
                    outXYElement->x = x;
                    outXYElement->y = y;
                    outXYElement->element = trackElement;
                    return;
                }
                break;
            case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
            case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
                if (trackElement->AsTrack()->HasChain())
                {
                    TileElement* tileElement = map_get_first_element_at(trackBeginEnd.begin_x >> 5, trackBeginEnd.begin_y >> 5);
                    do
                    {
                        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                            continue;
                        if (tileElement->AsTrack()->GetTrackType() != trackType)
                            continue;
                        if (tileElement->AsTrack()->GetSequenceIndex() != 0)
                            continue;
                        if (tileElement->base_height != trackBeginEnd.begin_z / 8)
                            continue;
                        outXYElement->x = trackBeginEnd.begin_x;
                        outXYElement->y = trackBeginEnd.begin_y;
                        outXYElement->element = tileElement;
                        return;
                    } while (!(tileElement++)->IsLastForTile());
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
static bool ride_create_vehicles(Ride* ride, CoordsXYE* element, int32_t isApplying)
{
    ride_update_max_vehicles(ride);
    if (ride->subtype == RIDE_ENTRY_INDEX_NULL)
    {
        return true;
    }

    // Check if there are enough free sprite slots for all the vehicles
    int32_t totalCars = ride->num_vehicles * ride->num_cars_per_train;
    if (totalCars > count_free_misc_sprite_slots())
    {
        gGameCommandErrorText = STR_UNABLE_TO_CREATE_ENOUGH_VEHICLES;
        return false;
    }

    if (!isApplying)
    {
        return true;
    }

    TileElement* tileElement = element->element;
    int32_t x = element->x;
    int32_t y = element->y;
    int32_t z = element->element->base_height;
    int32_t direction = tileElement->GetDirection();

    //
    if (ride->mode == RIDE_MODE_STATION_TO_STATION)
    {
        x = element->x - CoordsDirectionDelta[direction].x;
        y = element->y - CoordsDirectionDelta[direction].y;

        tileElement = map_get_first_element_at(x >> 5, y >> 5);
        do
        {
            if (tileElement->base_height != z)
                continue;
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                continue;
            break;
        } while (!(tileElement++)->IsLastForTile());

        z = tileElement->base_height;
        direction = tileElement->GetDirection();
    }

    vehicle_create_trains(ride->id, x, y, z, tileElement);
    // return true;

    // Initialise station departs
    // 006DDDD0:
    ride->lifecycle_flags |= RIDE_LIFECYCLE_ON_TRACK;
    for (int32_t i = 0; i < MAX_STATIONS; i++)
    {
        ride->stations[i].Depart = (ride->stations[i].Depart & STATION_DEPART_FLAG) | 1;
    }

    //
    if (ride->type != RIDE_TYPE_SPACE_RINGS && !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_16))
    {
        if (ride_is_block_sectioned(ride))
        {
            CoordsXYE firstBlock;
            ride_create_vehicles_find_first_block(ride, &firstBlock);
            loc_6DDF9C(ride, firstBlock.element);
        }
        else
        {
            for (int32_t i = 0; i < ride->num_vehicles; i++)
            {
                rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[i]);

                rct_ride_entry_vehicle* vehicleEntry = vehicle_get_vehicle_entry(vehicle);

                if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT))
                {
                    vehicle_update_track_motion(vehicle, nullptr);
                }

                vehicle_unset_update_flag_b1(vehicle);
            }
        }
    }
    ride_update_vehicle_colours(ride);
    return true;
}

/**
 *
 *  rct2: 0x006DDF9C
 */
void loc_6DDF9C(Ride* ride, TileElement* tileElement)
{
    rct_vehicle *train, *car;

    for (int32_t i = 0; i < ride->num_vehicles; i++)
    {
        uint16_t vehicleSpriteIdx = ride->vehicles[i];
        if (vehicleSpriteIdx == SPRITE_INDEX_NULL)
            continue;

        train = GET_VEHICLE(vehicleSpriteIdx);
        if (i == 0)
        {
            vehicle_update_track_motion(train, nullptr);
            vehicle_unset_update_flag_b1(train);
            continue;
        }

        vehicle_update_track_motion(train, nullptr);

        do
        {
            tileElement->flags |= TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED;
            car = train;
            while (true)
            {
                car->velocity = 0;
                car->acceleration = 0;
                car->swing_sprite = 0;
                car->remaining_distance += 13962;

                uint16_t spriteIndex = car->next_vehicle_on_train;
                if (spriteIndex == SPRITE_INDEX_NULL)
                {
                    break;
                }
                car = GET_VEHICLE(spriteIndex);
            }
        } while (!(vehicle_update_track_motion(train, nullptr) & VEHICLE_UPDATE_MOTION_TRACK_FLAG_10));

        tileElement->flags |= TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED;
        car = train;
        while (true)
        {
            car->update_flags &= ~VEHICLE_UPDATE_FLAG_1;
            car->status = VEHICLE_STATUS_TRAVELLING;
            if ((car->track_type >> 2) == TRACK_ELEM_END_STATION)
            {
                car->status = VEHICLE_STATUS_MOVING_TO_END_OF_STATION;
            }

            uint16_t spriteIndex = car->next_vehicle_on_train;
            if (spriteIndex == SPRITE_INDEX_NULL)
            {
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
static bool ride_initialise_cable_lift_track(Ride* ride, bool isApplying)
{
    LocationXY8 location;
    int32_t stationIndex;
    for (stationIndex = 0; stationIndex < MAX_STATIONS; stationIndex++)
    {
        location = ride->stations[stationIndex].Start;
        if (location.xy != RCT_XY8_UNDEFINED)
            break;
        if (stationIndex == 3)
        {
            gGameCommandErrorText = STR_CABLE_LIFT_HILL_MUST_START_IMMEDIATELY_AFTER_STATION;
            return false;
        }
    }

    int32_t x = location.x * 32;
    int32_t y = location.y * 32;
    int32_t z = ride->stations[stationIndex].Height;

    bool success = false;
    TileElement* tileElement = map_get_first_element_at(location.x, location.y);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != z)
            continue;

        if (!(TrackSequenceProperties[tileElement->AsTrack()->GetTrackType()][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
        {
            continue;
        }
        success = true;
        break;
    } while (!(tileElement++)->IsLastForTile());

    if (!success)
        return false;

    enum
    {
        STATE_FIND_CABLE_LIFT,
        STATE_FIND_STATION,
        STATE_REST_OF_TRACK
    };
    int32_t state = STATE_FIND_CABLE_LIFT;

    track_circuit_iterator it;
    track_circuit_iterator_begin(&it, { x, y, tileElement });
    while (track_circuit_iterator_previous(&it))
    {
        tileElement = it.current.element;
        int32_t trackType = tileElement->AsTrack()->GetTrackType();

        uint16_t flags = 16;
        switch (state)
        {
            case STATE_FIND_CABLE_LIFT:
                // Search for a cable lift hill track element
                if (trackType == TRACK_ELEM_CABLE_LIFT_HILL)
                {
                    flags = 8;
                    state = STATE_FIND_STATION;
                }
                break;
            case STATE_FIND_STATION:
                // Search for the start of the hill
                switch (trackType)
                {
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
        if (isApplying)
        {
            z = tileElement->base_height * 8;
            int32_t direction = tileElement->GetDirection();
            trackType = tileElement->AsTrack()->GetTrackType();
            x = it.current.x;
            y = it.current.y;
            sub_6C683D(&x, &y, &z, direction, trackType, 0, &tileElement, flags);
        }
    }
    return true;
}

/**
 *
 *  rct2: 0x006DF4D4
 */
static bool ride_create_cable_lift(ride_id_t rideIndex, bool isApplying)
{
    Ride* ride = get_ride(rideIndex);

    if (ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED && ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT)
    {
        gGameCommandErrorText = STR_CABLE_LIFT_UNABLE_TO_WORK_IN_THIS_OPERATING_MODE;
        return false;
    }

    if (ride->num_circuits > 1)
    {
        gGameCommandErrorText = STR_MULTICIRCUIT_NOT_POSSIBLE_WITH_CABLE_LIFT_HILL;
        return false;
    }

    if (count_free_misc_sprite_slots() <= 5)
    {
        gGameCommandErrorText = STR_UNABLE_TO_CREATE_ENOUGH_VEHICLES;
        return false;
    }

    if (!ride_initialise_cable_lift_track(ride, isApplying))
    {
        return false;
    }

    if (!isApplying)
    {
        return true;
    }

    int32_t x = ride->cable_lift_x;
    int32_t y = ride->cable_lift_y;
    int32_t z = ride->cable_lift_z;
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->base_height != z)
            continue;
        break;
    } while (!(tileElement++)->IsLastForTile());
    int32_t direction = tileElement->GetDirection();

    rct_vehicle* head = nullptr;
    rct_vehicle* tail = nullptr;
    uint32_t ebx = 0;
    for (int32_t i = 0; i < 5; i++)
    {
        uint32_t edx = ror32(0x15478, 10);
        uint16_t var_44 = edx & 0xFFFF;
        edx = rol32(edx, 10) >> 1;
        ebx -= edx;
        int32_t remaining_distance = ebx;
        ebx -= edx;

        rct_vehicle* current = cable_lift_segment_create(rideIndex, x, y, z, direction, var_44, remaining_distance, i == 0);
        current->next_vehicle_on_train = SPRITE_INDEX_NULL;
        if (i == 0)
        {
            head = current;
        }
        else
        {
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
static void loc_6B51C0(ride_id_t rideIndex)
{
    Ride* ride = get_ride(rideIndex);

    if (gUnk141F568 != gUnk13CA740)
        return;

    rct_window* w = window_get_main();
    if (w == nullptr)
        return;

    int8_t entranceOrExit = -1;
    int32_t i;
    for (i = 0; i < MAX_STATIONS; i++)
    {
        if (ride->stations[i].Start.xy == RCT_XY8_UNDEFINED)
            continue;

        if (ride_get_entrance_location(rideIndex, i).isNull())
        {
            entranceOrExit = 0;
            break;
        }

        if (ride_get_exit_location(rideIndex, i).isNull())
        {
            entranceOrExit = 1;
            break;
        }
    }

    if (entranceOrExit == -1)
        return;

    if (ride->type != RIDE_TYPE_MAZE)
    {
        int32_t x = ride->stations[i].Start.x * 32;
        int32_t y = ride->stations[i].Start.y * 32;
        int32_t z = ride->stations[i].Height * 8;
        window_scroll_to_location(w, x, y, z);

        CoordsXYE trackElement;
        ride_try_get_origin_element(rideIndex, &trackElement);
        ride_find_track_gap(rideIndex, &trackElement, &trackElement);
        int32_t ok = ride_modify(&trackElement);
        if (ok == 0)
        {
            return;
        }

        w = window_find_by_class(WC_RIDE_CONSTRUCTION);
        if (w != nullptr)
            window_event_mouse_up_call(w, WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE + entranceOrExit);
    }
}

/**
 *
 *  rct2: 0x006B528A
 */
static void ride_scroll_to_track_error(CoordsXYE* trackElement)
{
    if (!gGameCommandIsNetworked && gUnk141F568 == gUnk13CA740)
    {
        rct_window* w = window_get_main();
        if (w != nullptr)
        {
            window_scroll_to_location(w, trackElement->x, trackElement->y, trackElement->element->base_height * 8);
            ride_modify(trackElement);
        }
    }
}

/**
 *
 *  rct2: 0x006B4F6B
 */
static TileElement* loc_6B4F6B(ride_id_t rideIndex, int32_t x, int32_t y)
{
    Ride* ride = get_ride(rideIndex);
    TileElement* tileElement = map_get_first_element_at(x / 32, y / 32);
    do
    {
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (RideProperties[ride->type].flags & RIDE_TYPE_FLAG_FLAT_RIDE)
        {
            if (!(FlatRideTrackSequenceProperties[tileElement->AsTrack()->GetTrackType()][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
                continue;
        }
        else
        {
            if (!(TrackSequenceProperties[tileElement->AsTrack()->GetTrackType()][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
                continue;
        }

        if (tileElement->AsTrack()->GetRideIndex() == rideIndex)
            return tileElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

int32_t ride_is_valid_for_test(ride_id_t rideIndex, int32_t goingToBeOpen, int32_t isApplying)
{
    int32_t stationIndex;
    Ride* ride;
    CoordsXYE trackElement, problematicTrackElement = {};

    ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_NULL)
    {
        log_warning("Invalid ride type for ride %u", rideIndex);
        return 0;
    }

    window_close_by_number(WC_RIDE_CONSTRUCTION, rideIndex);

    stationIndex = ride_mode_check_station_present(ride);
    if (stationIndex == -1)
        return 0;

    if (!ride_mode_check_valid_station_numbers(ride))
        return 0;

    if (!ride_check_for_entrance_exit(rideIndex))
    {
        loc_6B51C0(rideIndex);
        return 0;
    }

    if (goingToBeOpen && isApplying)
    {
        sub_6B5952(rideIndex);
        ride->lifecycle_flags |= RIDE_LIFECYCLE_EVER_BEEN_OPENED;
    }

    // z = ride->stations[i].Height * 8;
    trackElement.x = ride->stations[stationIndex].Start.x * 32;
    trackElement.y = ride->stations[stationIndex].Start.y * 32;
    trackElement.element = loc_6B4F6B(rideIndex, trackElement.x, trackElement.y);
    if (trackElement.element == nullptr)
    {
        // Maze is strange, station start is 0... investigation required
        if (ride->type != RIDE_TYPE_MAZE)
            return 0;
    }

    if (ride->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER || ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT
        || ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
    {
        if (ride_find_track_gap(rideIndex, &trackElement, &problematicTrackElement)
            && (!gConfigGeneral.test_unfinished_tracks || ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED
                || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED))
        {
            gGameCommandErrorText = STR_TRACK_IS_NOT_A_COMPLETE_CIRCUIT;
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }
    }

    if (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
    {
        if (!ride_check_block_brakes(&trackElement, &problematicTrackElement))
        {
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }
    }

    if (ride->subtype != RIDE_ENTRY_INDEX_NULL)
    {
        rct_ride_entry* rideType = get_ride_entry(ride->subtype);
        if (rideType->flags & RIDE_ENTRY_FLAG_NO_INVERSIONS)
        {
            gGameCommandErrorText = STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN;
            if (ride_check_track_contains_inversions(&trackElement, &problematicTrackElement))
            {
                ride_scroll_to_track_error(&problematicTrackElement);
                return 0;
            }
        }
        if (rideType->flags & RIDE_ENTRY_FLAG_NO_BANKED_TRACK)
        {
            gGameCommandErrorText = STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN;
            if (ride_check_track_contains_banked(&trackElement, &problematicTrackElement))
            {
                ride_scroll_to_track_error(&problematicTrackElement);
                return 0;
            }
        }
    }

    if (ride->mode == RIDE_MODE_STATION_TO_STATION)
    {
        if (!ride_find_track_gap(rideIndex, &trackElement, &problematicTrackElement))
        {
            gGameCommandErrorText = STR_RIDE_MUST_START_AND_END_WITH_STATIONS;
            return 0;
        }

        gGameCommandErrorText = STR_STATION_NOT_LONG_ENOUGH;
        if (!ride_check_station_length(&trackElement, &problematicTrackElement))
        {
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }

        gGameCommandErrorText = STR_RIDE_MUST_START_AND_END_WITH_STATIONS;
        if (!ride_check_start_and_end_is_station(&trackElement))
        {
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }
    }

    if (isApplying)
        ride_set_start_finish_points(rideIndex, &trackElement);

    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES) && !(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
    {
        if (!ride_create_vehicles(ride, &trackElement, isApplying))
        {
            return 0;
        }
    }

    if ((RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_CABLE_LIFT_HILL)
        && (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED)
        && !(ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT))
    {
        if (!ride_create_cable_lift(rideIndex, isApplying))
            return 0;
    }

    return 1;
}
/**
 *
 *  rct2: 0x006B4EEA
 */
int32_t ride_is_valid_for_open(ride_id_t rideIndex, int32_t goingToBeOpen, int32_t isApplying)
{
    int32_t stationIndex;
    Ride* ride;
    CoordsXYE trackElement, problematicTrackElement = {};

    ride = get_ride(rideIndex);

    // Check to see if construction tool is in use. If it is close the construction window
    // to set the track to its final state and clean up ghosts.
    // We can't just call close as it would cause a stack overflow during shop creation
    // with auto open on.
    if (WC_RIDE_CONSTRUCTION == gCurrentToolWidget.window_classification && rideIndex == gCurrentToolWidget.window_number
        && (input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
        window_close_by_number(WC_RIDE_CONSTRUCTION, rideIndex);

    stationIndex = ride_mode_check_station_present(ride);
    if (stationIndex == -1)
        return 0;

    if (!ride_mode_check_valid_station_numbers(ride))
        return 0;

    if (!ride_check_for_entrance_exit(rideIndex))
    {
        loc_6B51C0(rideIndex);
        return 0;
    }

    if (goingToBeOpen && isApplying)
    {
        sub_6B5952(rideIndex);
        ride->lifecycle_flags |= RIDE_LIFECYCLE_EVER_BEEN_OPENED;
    }

    // z = ride->stations[i].Height * 8;
    trackElement.x = ride->stations[stationIndex].Start.x * 32;
    trackElement.y = ride->stations[stationIndex].Start.y * 32;
    trackElement.element = loc_6B4F6B(rideIndex, trackElement.x, trackElement.y);
    if (trackElement.element == nullptr)
    {
        // Maze is strange, station start is 0... investigation required
        if (ride->type != RIDE_TYPE_MAZE)
            return 0;
    }

    if (ride->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER || ride->mode == RIDE_MODE_RACE
        || ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT || ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED
        || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
    {
        if (ride_find_track_gap(rideIndex, &trackElement, &problematicTrackElement))
        {
            gGameCommandErrorText = STR_TRACK_IS_NOT_A_COMPLETE_CIRCUIT;
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }
    }

    if (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
    {
        if (!ride_check_block_brakes(&trackElement, &problematicTrackElement))
        {
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }
    }

    if (ride->subtype != RIDE_ENTRY_INDEX_NULL)
    {
        rct_ride_entry* rideType = get_ride_entry(ride->subtype);
        if (rideType->flags & RIDE_ENTRY_FLAG_NO_INVERSIONS)
        {
            gGameCommandErrorText = STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN;
            if (ride_check_track_contains_inversions(&trackElement, &problematicTrackElement))
            {
                ride_scroll_to_track_error(&problematicTrackElement);
                return 0;
            }
        }
        if (rideType->flags & RIDE_ENTRY_FLAG_NO_BANKED_TRACK)
        {
            gGameCommandErrorText = STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN;
            if (ride_check_track_contains_banked(&trackElement, &problematicTrackElement))
            {
                ride_scroll_to_track_error(&problematicTrackElement);
                return 0;
            }
        }
    }

    if (ride->mode == RIDE_MODE_STATION_TO_STATION)
    {
        if (!ride_find_track_gap(rideIndex, &trackElement, &problematicTrackElement))
        {
            gGameCommandErrorText = STR_RIDE_MUST_START_AND_END_WITH_STATIONS;
            return 0;
        }

        gGameCommandErrorText = STR_STATION_NOT_LONG_ENOUGH;
        if (!ride_check_station_length(&trackElement, &problematicTrackElement))
        {
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }

        gGameCommandErrorText = STR_RIDE_MUST_START_AND_END_WITH_STATIONS;
        if (!ride_check_start_and_end_is_station(&trackElement))
        {
            ride_scroll_to_track_error(&problematicTrackElement);
            return 0;
        }
    }

    if (isApplying)
        ride_set_start_finish_points(rideIndex, &trackElement);

    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES) && !(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
    {
        if (!ride_create_vehicles(ride, &trackElement, isApplying))
        {
            return 0;
        }
    }

    if ((RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_CABLE_LIFT_HILL)
        && (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED)
        && !(ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT))
    {
        if (!ride_create_cable_lift(rideIndex, isApplying))
            return 0;
    }

    return 1;
}

/**
 * Given a track element of the ride, find the start of the track.
 * It has to do this as a backwards loop in case this is an incomplete track.
 */
void ride_get_start_of_track(CoordsXYE* output)
{
    track_begin_end trackBeginEnd;
    CoordsXYE trackElement = *output;
    if (track_block_get_previous(trackElement.x, trackElement.y, trackElement.element, &trackBeginEnd))
    {
        TileElement* initial_map = trackElement.element;
        track_begin_end slowIt = trackBeginEnd;
        bool moveSlowIt = true;
        do
        {
            CoordsXYE lastGood = {
                /* .x = */ trackBeginEnd.begin_x,
                /* .y = */ trackBeginEnd.begin_y,
                /* .element = */ trackBeginEnd.begin_element,
            };

            if (!track_block_get_previous(
                    trackBeginEnd.end_x, trackBeginEnd.end_y, trackBeginEnd.begin_element, &trackBeginEnd))
            {
                trackElement = lastGood;
                break;
            }

            moveSlowIt = !moveSlowIt;
            if (moveSlowIt)
            {
                if (!track_block_get_previous(slowIt.end_x, slowIt.end_y, slowIt.begin_element, &slowIt)
                    || slowIt.begin_element == trackBeginEnd.begin_element)
                {
                    break;
                }
            }
        } while (initial_map != trackBeginEnd.begin_element);
    }
    *output = trackElement;
}

/**
 *
 *  rct2: 0x006CB7FB
 */
int32_t ride_get_refund_price(int32_t ride_id)
{
    CoordsXYE trackElement;
    money32 addedcost, cost = 0;

    if (!ride_try_get_origin_element(ride_id, &trackElement))
    {
        return 0; // Ride has no track to refund
    }

    // Find the start in case it is not a complete circuit
    ride_get_start_of_track(&trackElement);

    uint8_t direction = trackElement.element->GetDirection();

    // Used in the following loop to know when we have
    // completed all of the elements and are back at the
    // start.
    TileElement* initial_map = trackElement.element;
    CoordsXYE slowIt = trackElement;
    bool moveSlowIt = true;

    do
    {
        addedcost = game_do_command(
            trackElement.x, GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | (direction << 8), trackElement.y,
            trackElement.element->AsTrack()->GetTrackType() | ((trackElement.element->AsTrack()->GetSequenceIndex()) << 8),
            GAME_COMMAND_REMOVE_TRACK, trackElement.element->base_height * 8, 0);

        cost += (addedcost == MONEY32_UNDEFINED) ? 0 : addedcost;

        if (!track_block_get_next(&trackElement, &trackElement, nullptr, nullptr))
        {
            break;
        }

        moveSlowIt = !moveSlowIt;
        if (moveSlowIt)
        {
            if (!track_block_get_next(&slowIt, &slowIt, nullptr, nullptr) || slowIt.element == trackElement.element)
            {
                break;
            }
        }

        direction = trackElement.element->GetDirection();

    } while (trackElement.element != initial_map);

    return cost;
}

/**
 *
 *  rct2: 0x00696707
 */
void ride_stop_peeps_queuing(ride_id_t rideIndex)
{
    uint16_t spriteIndex;
    rct_peep* peep;

    FOR_ALL_PEEPS (spriteIndex, peep)
    {
        if (peep->state != PEEP_STATE_QUEUING)
            continue;
        if (peep->current_ride != rideIndex)
            continue;

        peep->RemoveFromQueue();
        peep->SetState(PEEP_STATE_FALLING);
    }
}

ride_id_t ride_get_empty_slot()
{
    for (ride_id_t i = 0; i < MAX_RIDES; i++)
    {
        Ride* ride = get_ride(i);
        if (ride->type == RIDE_TYPE_NULL)
        {
            return i;
        }
    }
    return RIDE_ID_NULL;
}

int32_t ride_get_default_mode(Ride* ride)
{
    const rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
    const uint8_t* availableModes = RideAvailableModes;

    for (int32_t i = 0; i < ride->type; i++)
    {
        while (*(availableModes++) != 255)
        {
        }
    }
    // Since this only selects a default mode and does not prevent other modes from being used, there is no need
    // to check if select-by-track-type or the all-ride-modes cheat have been enabled.
    if (rideEntry->flags & RIDE_ENTRY_DISABLE_FIRST_TWO_OPERATING_MODES)
    {
        availableModes += 2;
    }
    return availableModes[0];
}

static bool ride_with_colour_config_exists(uint8_t ride_type, const TrackColour* colours)
{
    Ride* ride;
    int32_t i;

    FOR_ALL_RIDES (i, ride)
    {
        if (ride->type != ride_type)
            continue;
        if (ride->track_colour[0].main != colours->main)
            continue;
        if (ride->track_colour[0].additional != colours->additional)
            continue;
        if (ride->track_colour[0].supports != colours->supports)
            continue;

        return true;
    }
    return false;
}

static bool ride_name_exists(char* name)
{
    char buffer[256];
    Ride* ride;
    int32_t i;
    FOR_ALL_RIDES (i, ride)
    {
        format_string(buffer, 256, ride->name, &ride->name_arguments);
        if ((strcmp(buffer, name) == 0) && ride_has_any_track_elements(i))
        {
            return true;
        }
    }
    return false;
}

/**
 *
 *  Based on rct2: 0x006B4776
 */
int32_t ride_get_random_colour_preset_index(uint8_t ride_type)
{
    if (ride_type >= 128)
    {
        return 0;
    }

    const track_colour_preset_list* colourPresets = &RideColourPresets[ride_type];

    // 200 attempts to find a colour preset that hasn't already been used in the park for this ride type
    for (int32_t i = 0; i < 200; i++)
    {
        int32_t listIndex = util_rand() % colourPresets->count;
        const TrackColour* colours = &colourPresets->list[listIndex];

        if (!ride_with_colour_config_exists(ride_type, colours))
        {
            return listIndex;
        }
    }
    return 0;
}

/**
 *
 *  Based on rct2: 0x006B4776
 */
void ride_set_colour_preset(Ride* ride, uint8_t index)
{
    const track_colour_preset_list* colourPresets = &RideColourPresets[ride->type];
    TrackColour colours = { COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK };
    if (index < colourPresets->count)
    {
        colours = colourPresets->list[index];
    }
    for (int32_t i = 0; i < NUM_COLOUR_SCHEMES; i++)
    {
        ride->track_colour[i].main = colours.main;
        ride->track_colour[i].additional = colours.additional;
        ride->track_colour[i].supports = colours.supports;
    }
    ride->colour_scheme_type = 0;
}

money32 ride_get_common_price(Ride* forRide)
{
    Ride* ride;
    int32_t i;

    FOR_ALL_RIDES (i, ride)
    {
        if (ride->type == forRide->type && ride != forRide)
        {
            return ride->price;
        }
    }

    return MONEY32_UNDEFINED;
}

void ride_set_name_to_default(Ride* ride, rct_ride_entry* rideEntry)
{
    if (RideGroupManager::RideTypeIsIndependent(ride->type))
    {
        ride_set_name_to_vehicle_default(ride, rideEntry);
    }
    else
    {
        ride_set_name_to_track_default(ride, rideEntry);
    }
}

void ride_set_name_to_track_default(Ride* ride, rct_ride_entry* rideEntry)
{
    char rideNameBuffer[256];
    ride_name_args name_args;

    ride->name = STR_NONE;

    if (RideGroupManager::RideTypeHasRideGroups(ride->type))
    {
        const RideGroup* rideGroup = RideGroupManager::GetRideGroup(ride->type, rideEntry);
        name_args.type_name = rideGroup->Naming.name;
    }
    else
    {
        name_args.type_name = RideNaming[ride->type].name;
    }

    name_args.number = 0;
    do
    {
        name_args.number++;
        format_string(rideNameBuffer, 256, 1, &name_args);
    } while (ride_name_exists(rideNameBuffer));

    ride->name = 1;
    ride->name_arguments_type_name = name_args.type_name;
    ride->name_arguments_number = name_args.number;
}

static void ride_set_name_to_vehicle_default(Ride* ride, rct_ride_entry* rideEntry)
{
    char rideNameBuffer[256];
    ride_name_args name_args;

    ride->name = 1;
    ride->name_arguments_type_name = rideEntry->naming.name;
    rct_string_id rideNameStringId = 0;
    name_args.type_name = rideEntry->naming.name;
    name_args.number = 0;

    do
    {
        name_args.number++;
        format_string(rideNameBuffer, 256, ride->name, &name_args);
    } while (ride_name_exists(rideNameBuffer));

    ride->name_arguments_type_name = name_args.type_name;
    ride->name_arguments_number = name_args.number;

    rideNameStringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER | USER_STRING_DUPLICATION_PERMITTED, rideNameBuffer);
    if (rideNameStringId != 0)
    {
        ride->name = rideNameStringId;
    }
    else
    {
        ride_set_name_to_track_default(ride, rideEntry);
    }
}

/**
 * This will return the name of the ride, as seen in the New Ride window.
 */
rct_ride_name get_ride_naming(const uint8_t rideType, rct_ride_entry* rideEntry)
{
    if (RideGroupManager::RideTypeHasRideGroups(rideType))
    {
        const RideGroup* rideGroup = RideGroupManager::GetRideGroup(rideType, rideEntry);
        return rideGroup->Naming;
    }
    else if (!RideGroupManager::RideTypeIsIndependent(rideType))
    {
        return RideNaming[rideType];
    }
    else
    {
        return rideEntry->naming;
    }
}

/**
 *
 * Network client callback when placing ride pieces
 *   Client does execute placing the piece on the same tick as mouse_up - waits for server command
 * Re-executes function from ride_construction - window_ride_construction_construct()
 * Only uses part that deals with construction state
 */

void game_command_callback_ride_construct_placed_back(
    [[maybe_unused]] int32_t eax, [[maybe_unused]] int32_t ebx, [[maybe_unused]] int32_t ecx, [[maybe_unused]] int32_t edx,
    [[maybe_unused]] int32_t esi, [[maybe_unused]] int32_t edi, [[maybe_unused]] int32_t ebp)
{
    int32_t trackDirection, x, y, z;
    track_begin_end trackBeginEnd;

    trackDirection = direction_reverse(_currentTrackPieceDirection);
    x = _currentTrackBeginX;
    y = _currentTrackBeginY;
    z = _currentTrackBeginZ;
    if (!(trackDirection & 4))
    {
        x += CoordsDirectionDelta[trackDirection].x;
        y += CoordsDirectionDelta[trackDirection].y;
    }

    if (track_block_get_previous_from_zero(x, y, z, _currentRideIndex, trackDirection, &trackBeginEnd))
    {
        _currentTrackBeginX = trackBeginEnd.begin_x;
        _currentTrackBeginY = trackBeginEnd.begin_y;
        _currentTrackBeginZ = trackBeginEnd.begin_z;
        _currentTrackPieceDirection = trackBeginEnd.begin_direction;
        _currentTrackPieceType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
        ride_select_previous_section();
    }
    else
    {
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
    }

    window_ride_construction_do_station_check();
    window_ride_construction_update_active_elements();
}

void game_command_callback_ride_construct_placed_front(
    [[maybe_unused]] int32_t eax, [[maybe_unused]] int32_t ebx, [[maybe_unused]] int32_t ecx, [[maybe_unused]] int32_t edx,
    [[maybe_unused]] int32_t esi, [[maybe_unused]] int32_t edi, [[maybe_unused]] int32_t ebp)
{
    int32_t trackDirection, x, y, z;

    trackDirection = _currentTrackPieceDirection;
    x = _currentTrackBeginX;
    y = _currentTrackBeginY;
    z = _currentTrackBeginZ;
    if (!(trackDirection & 4))
    {
        x -= CoordsDirectionDelta[trackDirection].x;
        y -= CoordsDirectionDelta[trackDirection].y;
    }

    CoordsXYE next_track;
    if (track_block_get_next_from_zero(x, y, z, _currentRideIndex, trackDirection, &next_track, &z, &trackDirection, false))
    {
        _currentTrackBeginX = next_track.x;
        _currentTrackBeginY = next_track.y;
        _currentTrackBeginZ = z;
        _currentTrackPieceDirection = next_track.element->GetDirection();
        _currentTrackPieceType = next_track.element->AsTrack()->GetTrackType();
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
        ride_select_next_section();
    }
    else
    {
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

void game_command_callback_ride_remove_track_piece(
    [[maybe_unused]] int32_t eax, [[maybe_unused]] int32_t ebx, [[maybe_unused]] int32_t ecx, [[maybe_unused]] int32_t edx,
    [[maybe_unused]] int32_t esi, [[maybe_unused]] int32_t edi, [[maybe_unused]] int32_t ebp)
{
    int32_t x, y, z, direction, type;

    x = gRideRemoveTrackPieceCallbackX;
    y = gRideRemoveTrackPieceCallbackY;
    z = gRideRemoveTrackPieceCallbackZ;
    direction = gRideRemoveTrackPieceCallbackDirection;
    type = gRideRemoveTrackPieceCallbackType;

    window_ride_construction_mouseup_demolish_next_piece(x, y, z, direction, type);
}

bool ride_type_has_flag(int32_t rideType, int32_t flag)
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

void increment_turn_count_1_element(Ride* ride, uint8_t type)
{
    uint16_t* turn_count;
    switch (type)
    {
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
    uint16_t value = (*turn_count & TURN_MASK_1_ELEMENT) + 1;
    *turn_count &= ~TURN_MASK_1_ELEMENT;

    if (value > TURN_MASK_1_ELEMENT)
        value = TURN_MASK_1_ELEMENT;
    *turn_count |= value;
}

void increment_turn_count_2_elements(Ride* ride, uint8_t type)
{
    uint16_t* turn_count;
    switch (type)
    {
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
    uint16_t value = (*turn_count & TURN_MASK_2_ELEMENTS) + 0x20;
    *turn_count &= ~TURN_MASK_2_ELEMENTS;

    if (value > TURN_MASK_2_ELEMENTS)
        value = TURN_MASK_2_ELEMENTS;
    *turn_count |= value;
}

void increment_turn_count_3_elements(Ride* ride, uint8_t type)
{
    uint16_t* turn_count;
    switch (type)
    {
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
    uint16_t value = (*turn_count & TURN_MASK_3_ELEMENTS) + 0x100;
    *turn_count &= ~TURN_MASK_3_ELEMENTS;

    if (value > TURN_MASK_3_ELEMENTS)
        value = TURN_MASK_3_ELEMENTS;
    *turn_count |= value;
}

void increment_turn_count_4_plus_elements(Ride* ride, uint8_t type)
{
    uint16_t* turn_count;
    switch (type)
    {
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
    uint16_t value = (*turn_count & TURN_MASK_4_PLUS_ELEMENTS) + 0x800;
    *turn_count &= ~TURN_MASK_4_PLUS_ELEMENTS;

    if (value > TURN_MASK_4_PLUS_ELEMENTS)
        value = TURN_MASK_4_PLUS_ELEMENTS;
    *turn_count |= value;
}

int32_t get_turn_count_1_element(Ride* ride, uint8_t type)
{
    uint16_t* turn_count;
    switch (type)
    {
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

int32_t get_turn_count_2_elements(Ride* ride, uint8_t type)
{
    uint16_t* turn_count;
    switch (type)
    {
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

int32_t get_turn_count_3_elements(Ride* ride, uint8_t type)
{
    uint16_t* turn_count;
    switch (type)
    {
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

int32_t get_turn_count_4_plus_elements(Ride* ride, uint8_t type)
{
    uint16_t* turn_count;
    switch (type)
    {
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

bool ride_has_spinning_tunnel(Ride* ride)
{
    return ride->special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
}

bool ride_has_water_splash(Ride* ride)
{
    return ride->special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
}

bool ride_has_rapids(Ride* ride)
{
    return ride->special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
}

bool ride_has_log_reverser(Ride* ride)
{
    return ride->special_track_elements & RIDE_ELEMENT_REVERSER_OR_WATERFALL;
}

bool ride_has_waterfall(Ride* ride)
{
    return ride->special_track_elements & RIDE_ELEMENT_REVERSER_OR_WATERFALL;
}

bool ride_has_whirlpool(Ride* ride)
{
    return ride->special_track_elements & RIDE_ELEMENT_WHIRLPOOL;
}

uint8_t ride_get_helix_sections(Ride* ride)
{
    // Helix sections stored in the low 5 bits.
    return ride->special_track_elements & 0x1F;
}

bool ride_is_powered_launched(Ride* ride)
{
    return ride->mode == RIDE_MODE_POWERED_LAUNCH_PASSTROUGH || ride->mode == RIDE_MODE_POWERED_LAUNCH
        || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED;
}

bool ride_is_block_sectioned(Ride* ride)
{
    return ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED;
}

bool ride_has_any_track_elements(ride_id_t rideIndex)
{
    tile_element_iterator it;

    tile_element_iterator_begin(&it);
    while (tile_element_iterator_next(&it))
    {
        if (it.element->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (it.element->AsTrack()->GetRideIndex() != rideIndex)
            continue;
        if (it.element->flags & TILE_ELEMENT_FLAG_GHOST)
            continue;

        return true;
    }

    return false;
}

void ride_all_has_any_track_elements(bool* rideIndexArray)
{
    tile_element_iterator it;

    std::fill_n(rideIndexArray, MAX_RIDES, false);

    tile_element_iterator_begin(&it);
    while (tile_element_iterator_next(&it))
    {
        if (it.element->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (it.element->flags & TILE_ELEMENT_FLAG_GHOST)
            continue;

        rideIndexArray[it.element->AsTrack()->GetRideIndex()] = true;
    }
}

/**
 *
 *  rct2: 0x006847BA
 */
void set_vehicle_type_image_max_sizes(rct_ride_entry_vehicle* vehicle_type, int32_t num_images)
{
    uint8_t bitmap[200][200] = { 0 };

    rct_drawpixelinfo dpi = {
        /*.bits = */ (uint8_t*)bitmap,
        /*.x = */ -100,
        /*.y = */ -100,
        /*.width = */ 200,
        /*.height = */ 200,
        /*.pitch = */ 0,
        /*.zoom_level = */ 0,
    };

    for (int32_t i = 0; i < num_images; ++i)
    {
        gfx_draw_sprite_software(&dpi, vehicle_type->base_image_id + i, 0, 0, 0);
    }
    int32_t al = -1;
    for (int32_t i = 99; i != 0; --i)
    {
        for (int32_t j = 0; j < 200; j++)
        {
            if (bitmap[j][100 - i] != 0)
            {
                al = i;
                break;
            }
        }

        if (al != -1)
            break;

        for (int32_t j = 0; j < 200; j++)
        {
            if (bitmap[j][100 + i] != 0)
            {
                al = i;
                break;
            }
        }

        if (al != -1)
            break;
    }

    al++;
    int32_t bl = -1;

    for (int32_t i = 99; i != 0; --i)
    {
        for (int32_t j = 0; j < 200; j++)
        {
            if (bitmap[100 - i][j] != 0)
            {
                bl = i;
                break;
            }
        }

        if (bl != -1)
            break;
    }
    bl++;

    int32_t bh = -1;

    for (int32_t i = 99; i != 0; --i)
    {
        for (int32_t j = 0; j < 200; j++)
        {
            if (bitmap[100 + i][j] != 0)
            {
                bh = i;
                break;
            }
        }

        if (bh != -1)
            break;
    }
    bh++;

    // Moved from object paint

    if (vehicle_type->flags & VEHICLE_ENTRY_FLAG_13)
    {
        bl += 16;
    }

    vehicle_type->sprite_width = al;
    vehicle_type->sprite_height_negative = bl;
    vehicle_type->sprite_height_positive = bh;
}

static int32_t loc_6CD18E(
    int16_t mapX, int16_t mapY, int16_t entranceMinX, int16_t entranceMinY, int16_t entranceMaxX, int16_t entranceMaxY)
{
    int32_t direction = 0;
    if (mapX == entranceMinX)
    {
        if (mapY > entranceMinY && mapY < entranceMaxY)
        {
            return direction;
        }
    }
    direction = 1;
    if (mapY == entranceMaxY)
    {
        if (mapX > entranceMinX && mapX < entranceMaxX)
        {
            return direction;
        }
    }
    direction = 2;
    if (mapX == entranceMaxX)
    {
        if (mapY > entranceMinY && mapY < entranceMaxY)
        {
            return direction;
        }
    }
    direction = 3;
    if (mapY == entranceMinY)
    {
        if (mapX > entranceMinX && mapX < entranceMaxX)
        {
            return direction;
        }
    }
    return -1;
}

/**
 *
 *  rct2: 0x006CCF70
 */
void ride_get_entrance_or_exit_position_from_screen_position(
    int32_t screenX, int32_t screenY, int32_t* outX, int32_t* outY, int32_t* outDirection)
{
    int16_t mapX, mapY;
    int16_t entranceMinX, entranceMinY, entranceMaxX, entranceMaxY, word_F4418C, word_F4418E;
    int32_t interactionType, direction, stationHeight, stationDirection;
    TileElement* tileElement;
    rct_viewport* viewport;
    Ride* ride;

    gRideEntranceExitPlaceDirection = 255;
    get_map_coordinates_from_pos(screenX, screenY, 0xFFFB, &mapX, &mapY, &interactionType, &tileElement, &viewport);
    if (interactionType != 0)
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
        {
            if (tileElement->AsTrack()->GetRideIndex() == gRideEntranceExitPlaceRideIndex)
            {
                if (TrackSequenceProperties[tileElement->AsTrack()->GetTrackType()][0] & TRACK_SEQUENCE_FLAG_ORIGIN)
                {
                    if (tileElement->AsTrack()->GetTrackType() == TRACK_ELEM_MAZE)
                    {
                        gRideEntranceExitPlaceStationIndex = 0;
                    }
                    else
                    {
                        gRideEntranceExitPlaceStationIndex = tileElement->AsTrack()->GetStationIndex();
                    }
                }
            }
        }
    }

    ride = get_ride(gRideEntranceExitPlaceRideIndex);
    stationHeight = ride->stations[gRideEntranceExitPlaceStationIndex].Height;

    screen_get_map_xy_with_z(screenX, screenY, stationHeight * 8, &mapX, &mapY);
    if (mapX == LOCATION_NULL)
    {
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

    LocationXY8 stationStart = ride->stations[gRideEntranceExitPlaceStationIndex].Start;
    if (stationStart.xy == RCT_XY8_UNDEFINED)
        return;

    _unkF44188.z = stationHeight;

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3))
    {
        mapX = (word_F4418C & 0x1F) - 16;
        mapY = (word_F4418E & 0x1F) - 16;
        if (std::abs(mapX) < std::abs(mapY))
        {
            direction = mapY < 0 ? 3 : 1;
        }
        else
        {
            direction = mapX < 0 ? 0 : 2;
        }

        for (int32_t i = 0; i < MAX_STATIONS; i++)
        {
            mapX = _unkF44188.x + CoordsDirectionDelta[direction].x;
            mapY = _unkF44188.y + CoordsDirectionDelta[direction].y;
            if (mapX >= 0 && mapY >= 0 && mapX < (256 * 32) && mapY < (256 * 32))
            {
                tileElement = map_get_first_element_at(mapX >> 5, mapY >> 5);
                do
                {
                    if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                        continue;
                    if (tileElement->base_height != stationHeight)
                        continue;
                    if (tileElement->AsTrack()->GetRideIndex() != gRideEntranceExitPlaceRideIndex)
                        continue;
                    if (tileElement->AsTrack()->GetTrackType() == TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP)
                    {
                        gRideEntranceExitPlaceDirection = direction_reverse(direction);
                        *outDirection = direction_reverse(direction);
                        return;
                    }
                    if (tileElement->AsTrack()->GetStationIndex() != gRideEntranceExitPlaceStationIndex)
                        continue;

                    int32_t eax = (direction + 2 - tileElement->GetDirection()) & TILE_ELEMENT_DIRECTION_MASK;
                    if (FlatRideTrackSequenceProperties[tileElement->AsTrack()->GetTrackType()]
                                                       [tileElement->AsTrack()->GetSequenceIndex()]
                        & (1 << eax))
                    {
                        gRideEntranceExitPlaceDirection = direction_reverse(direction);
                        *outDirection = direction_reverse(direction);
                        return;
                    }
                } while (!(tileElement++)->IsLastForTile());
            }
            direction = (direction + 1) & 3;
        }
        gRideEntranceExitPlaceDirection = 0xFF;
    }
    else
    {
        mapX = stationStart.x * 32;
        mapY = stationStart.y * 32;
        entranceMinX = mapX;
        entranceMinY = mapY;

        tileElement = ride_get_station_start_track_element(ride, gRideEntranceExitPlaceStationIndex);
        if (tileElement == nullptr)
        {
            *outX = 0x8000;
            return;
        }
        direction = tileElement->GetDirection();
        stationDirection = direction;

        while (true)
        {
            entranceMaxX = mapX;
            entranceMaxY = mapY;
            mapX -= CoordsDirectionDelta[direction].x;
            mapY -= CoordsDirectionDelta[direction].y;
            tileElement = map_get_first_element_at(mapX >> 5, mapY >> 5);
            bool goToNextTile = false;

            do
            {
                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;
                if (tileElement->AsTrack()->GetRideIndex() != gRideEntranceExitPlaceRideIndex)
                    continue;
                if (tileElement->AsTrack()->GetStationIndex() != gRideEntranceExitPlaceStationIndex)
                    continue;

                switch (tileElement->AsTrack()->GetTrackType())
                {
                    case TRACK_ELEM_END_STATION:
                    case TRACK_ELEM_BEGIN_STATION:
                    case TRACK_ELEM_MIDDLE_STATION:
                        goToNextTile = true;
                }
            } while (!goToNextTile && !(tileElement++)->IsLastForTile());

            if (!goToNextTile)
                break;
        }

        mapX = entranceMinX;
        if (mapX > entranceMaxX)
        {
            entranceMinX = entranceMaxX;
            entranceMaxX = mapX;
        }

        mapY = entranceMinY;
        if (mapY > entranceMaxY)
        {
            entranceMinY = entranceMaxY;
            entranceMaxY = mapY;
        }

        direction = loc_6CD18E(*outX, *outY, entranceMinX - 32, entranceMinY - 32, entranceMaxX + 32, entranceMaxY + 32);
        if (direction != -1 && direction != stationDirection && direction != direction_reverse(stationDirection))
        {
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
    if (track_block_get_previous_from_zero(
            _currentTrackBeginX, _currentTrackBeginY, _currentTrackBeginZ, _currentRideIndex, _currentTrackPieceDirection,
            &trackBeginEnd))
    {
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
        _currentTrackBeginX = trackBeginEnd.begin_x;
        _currentTrackBeginY = trackBeginEnd.begin_y;
        _currentTrackBeginZ = trackBeginEnd.begin_z;
        _currentTrackPieceDirection = trackBeginEnd.begin_direction;
        _currentTrackPieceType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        return true;
    }
    else
    {
        return false;
    }
}

bool ride_select_forwards_from_back()
{
    int32_t x, y, z, direction;

    ride_construction_invalidate_current_track();

    x = _currentTrackBeginX;
    y = _currentTrackBeginY;
    z = _currentTrackBeginZ;
    direction = direction_reverse(_currentTrackPieceDirection);
    CoordsXYE next_track;

    if (track_block_get_next_from_zero(x, y, z, _currentRideIndex, direction, &next_track, &z, &direction, false))
    {
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
        _currentTrackBeginX = next_track.x;
        _currentTrackBeginY = next_track.y;
        _currentTrackBeginZ = z;
        _currentTrackPieceDirection = next_track.element->GetDirection();
        _currentTrackPieceType = next_track.element->AsTrack()->GetTrackType();
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        return true;
    }
    else
    {
        return false;
    }
}

money32 ride_remove_track_piece(int32_t x, int32_t y, int32_t z, int32_t direction, int32_t type, uint8_t flags)
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
bool ride_are_all_possible_entrances_and_exits_built(Ride* ride)
{
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
        return true;

    for (int32_t i = 0; i < MAX_STATIONS; i++)
    {
        if (ride->stations[i].Start.xy == RCT_XY8_UNDEFINED)
        {
            continue;
        }
        if (ride_get_entrance_location(ride, i).isNull())
        {
            gGameCommandErrorText = STR_ENTRANCE_NOT_YET_BUILT;
            return false;
        }
        if (ride_get_exit_location(ride, i).isNull())
        {
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
void invalidate_test_results(Ride* ride)
{
    ride_measurement_clear(ride);
    ride->excitement = RIDE_RATING_UNDEFINED;
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_TESTED;
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_TEST_IN_PROGRESS;
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        for (int32_t i = 0; i < ride->num_vehicles; i++)
        {
            uint16_t spriteIndex = ride->vehicles[i];
            if (spriteIndex != SPRITE_INDEX_NULL)
            {
                rct_vehicle* vehicle = GET_VEHICLE(spriteIndex);
                vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_TESTING;
            }
        }
    }
    window_invalidate_by_number(WC_RIDE, ride->id);
}

/**
 *
 *  rct2: 0x006B7481
 *
 * @param rideIndex (dl)
 * @param reliabilityIncreaseFactor (ax)
 */
void ride_fix_breakdown(Ride* ride, int32_t reliabilityIncreaseFactor)
{
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_BREAKDOWN_PENDING;
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_BROKEN_DOWN;
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST | RIDE_INVALIDATE_RIDE_MAINTENANCE;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        for (int32_t i = 0; i < ride->num_vehicles; i++)
        {
            uint16_t spriteIndex = ride->vehicles[i];
            while (spriteIndex != SPRITE_INDEX_NULL)
            {
                rct_vehicle* vehicle = GET_VEHICLE(spriteIndex);
                vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_ZERO_VELOCITY;
                vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_BROKEN_CAR;
                vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_BROKEN_TRAIN;
                spriteIndex = vehicle->next_vehicle_on_train;
            }
        }
    }

    uint8_t unreliability = 100 - ride->reliability_percentage;
    ride->reliability += reliabilityIncreaseFactor * (unreliability / 2);
}

/**
 *
 *  rct2: 0x006DE102
 */
void ride_update_vehicle_colours(Ride* ride)
{
    if (ride->type == RIDE_TYPE_SPACE_RINGS || ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_16))
    {
        gfx_invalidate_screen();
    }

    for (int32_t i = 0; i < MAX_VEHICLES_PER_RIDE; i++)
    {
        int32_t carIndex = 0;
        uint16_t spriteIndex = ride->vehicles[i];
        VehicleColour colours = {};

        while (spriteIndex != SPRITE_INDEX_NULL)
        {
            rct_vehicle* vehicle = GET_VEHICLE(spriteIndex);
            switch (ride->colour_scheme_type & 3)
            {
                case RIDE_COLOUR_SCHEME_ALL_SAME:
                    colours = ride->vehicle_colours[0];
                    colours.Ternary = ride->vehicle_colours[0].Ternary;
                    break;
                case RIDE_COLOUR_SCHEME_DIFFERENT_PER_TRAIN:
                    colours = ride->vehicle_colours[i];
                    colours.Ternary = ride->vehicle_colours[i].Ternary;
                    break;
                case RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR:
                    colours = ride->vehicle_colours[std::min(carIndex, MAX_CARS_PER_TRAIN - 1)];
                    colours.Ternary = ride->vehicle_colours[std::min(carIndex, MAX_CARS_PER_TRAIN - 1)].Ternary;
                    break;
            }

            vehicle->colours.body_colour = colours.Body;
            vehicle->colours.trim_colour = colours.Trim;
            vehicle->colours_extended = colours.Ternary;
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
void ride_entry_get_train_layout(int32_t rideEntryIndex, int32_t numCarsPerTrain, uint8_t* trainLayout)
{
    for (int32_t i = 0; i < numCarsPerTrain; i++)
    {
        trainLayout[i] = ride_entry_get_vehicle_at_position(rideEntryIndex, numCarsPerTrain, i);
    }
}

uint8_t ride_entry_get_vehicle_at_position(int32_t rideEntryIndex, int32_t numCarsPerTrain, int32_t position)
{
    rct_ride_entry* rideEntry = get_ride_entry(rideEntryIndex);
    if (position == 0 && rideEntry->front_vehicle != 255)
    {
        return rideEntry->front_vehicle;
    }
    else if (position == 1 && rideEntry->second_vehicle != 255)
    {
        return rideEntry->second_vehicle;
    }
    else if (position == 2 && rideEntry->third_vehicle != 255)
    {
        return rideEntry->third_vehicle;
    }
    else if (position == numCarsPerTrain - 1 && rideEntry->rear_vehicle != 255)
    {
        return rideEntry->rear_vehicle;
    }
    else
    {
        return rideEntry->default_vehicle;
    }
}

// Finds track pieces that a given ride entry has sprites for
uint64_t ride_entry_get_supported_track_pieces(const rct_ride_entry* rideEntry)
{
    uint64_t supportedPieces = 0xFFFFFFFFFFFFFFFFULL;
    uint16_t trackPieceRequiredSprites[55] = {
        0x0001u, 0x0001u, 0x0001u, 0x0000u, 0x0006u, 0x0002u, 0x0020u, 0x000E,  0x0003u, 0x0006u, 0x0007u,
        0x0002u, 0x0004u, 0x0001u, 0x0001u, 0x0001u, 0x0001u, 0x0061u, 0x000E,  0x1081u, 0x0001u, 0x0020u,
        0x0020u, 0x0001u, 0x0001u, 0x0000u, 0x0001u, 0x0001u, 0x000C,  0x0061u, 0x0002u, 0x000E,  0x0480u,
        0x0001u, 0x0061u, 0x0001u, 0x0001u, 0x000Fu, 0x0001u, 0x0200u, 0x0007u, 0x0008u, 0x0000u, 0x0000u,
        0x4000u, 0x0008u, 0x0001u, 0x0001u, 0x0061u, 0x0061u, 0x0008u, 0x0008u, 0x0001u, 0x000Eu, 0x000Eu,
    };

    // Only check default vehicle; it's assumed the others will have correct sprites if this one does (I've yet to find an
    // exception, at least)
    for (int32_t j = 0; j < 55; j++)
    {
        if ((rideEntry->vehicles[rideEntry->default_vehicle].sprite_flags & trackPieceRequiredSprites[j])
            != trackPieceRequiredSprites[j])
            supportedPieces &= ~(1ULL << j);
    }

    return supportedPieces;
}

static int32_t ride_get_smallest_station_length(Ride* ride)
{
    auto result = std::numeric_limits<int32_t>::max();
    for (int32_t i = 0; i < MAX_STATIONS; i++)
    {
        if (ride->stations[i].Start.xy != RCT_XY8_UNDEFINED)
        {
            result = std::min<int32_t>(result, ride->stations[i].Length);
        }
    }
    return result;
}

/**
 *
 *  rct2: 0x006CB3AA
 */
static int32_t ride_get_track_length(Ride* ride)
{
    rct_window* w;
    TileElement* tileElement = nullptr;
    track_circuit_iterator it, slowIt;
    ride_id_t rideIndex;
    int32_t x = 0, y = 0, z, trackType, result;
    bool foundTrack = false;

    for (int32_t i = 0; i < MAX_STATIONS && !foundTrack; i++)
    {
        LocationXY8 location = ride->stations[i].Start;
        if (location.xy == RCT_XY8_UNDEFINED)
            continue;

        x = location.x * 32;
        y = location.y * 32;
        z = ride->stations[i].Height;

        tileElement = map_get_first_element_at(x >> 5, y >> 5);
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                continue;

            trackType = tileElement->AsTrack()->GetTrackType();
            if (!(TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
                continue;

            if (tileElement->base_height != z)
                continue;

            foundTrack = true;
        } while (!foundTrack && !(tileElement++)->IsLastForTile());
    }

    if (foundTrack)
    {
        rideIndex = tileElement->AsTrack()->GetRideIndex();

        w = window_find_by_class(WC_RIDE_CONSTRUCTION);
        if (w != nullptr && _rideConstructionState != RIDE_CONSTRUCTION_STATE_0 && _currentRideIndex == rideIndex)
        {
            ride_construction_invalidate_current_track();
        }

        bool moveSlowIt = true;
        result = 0;
        track_circuit_iterator_begin(&it, { x, y, tileElement });
        slowIt = it;
        while (track_circuit_iterator_next(&it))
        {
            trackType = it.current.element->AsTrack()->GetTrackType();
            result += TrackPieceLengths[trackType];

            moveSlowIt = !moveSlowIt;
            if (moveSlowIt)
            {
                track_circuit_iterator_next(&slowIt);
                if (track_circuit_iterators_match(&it, &slowIt))
                {
                    return 0;
                }
            }
        }
        return result;
    }
    else
    {
        return 0;
    }
}

/**
 *
 *  rct2: 0x006DD57D
 */
void ride_update_max_vehicles(Ride* ride)
{
    if (ride->subtype == RIDE_ENTRY_INDEX_NULL)
        return;

    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry == nullptr)
    {
        return;
    }
    rct_ride_entry_vehicle* vehicleEntry;
    uint8_t numCarsPerTrain, numVehicles;
    int32_t maxNumTrains;

    if (rideEntry->cars_per_flat_ride == 0xFF)
    {
        int32_t trainLength;
        ride->num_cars_per_train = std::max(rideEntry->min_cars_in_train, ride->num_cars_per_train);
        ride->min_max_cars_per_train = rideEntry->max_cars_in_train | (rideEntry->min_cars_in_train << 4);

        // Calculate maximum train length based on smallest station length
        int32_t stationLength = ride_get_smallest_station_length(ride);
        if (stationLength == -1)
            return;

        stationLength = (stationLength * 0x44180) - 0x16B2A;
        int32_t maxMass = RideData5[ride->type].max_mass << 8;
        int32_t maxCarsPerTrain = 1;
        for (int32_t numCars = rideEntry->max_cars_in_train; numCars > 0; numCars--)
        {
            trainLength = 0;
            int32_t totalMass = 0;
            for (int32_t i = 0; i < numCars; i++)
            {
                vehicleEntry = &rideEntry->vehicles[ride_entry_get_vehicle_at_position(ride->subtype, numCars, i)];
                trainLength += vehicleEntry->spacing;
                totalMass += vehicleEntry->car_mass;
            }

            if (trainLength <= stationLength && totalMass <= maxMass)
            {
                maxCarsPerTrain = numCars;
                break;
            }
        }
        int32_t newCarsPerTrain = std::max(ride->proposed_num_cars_per_train, rideEntry->min_cars_in_train);
        maxCarsPerTrain = std::max(maxCarsPerTrain, (int32_t)rideEntry->min_cars_in_train);
        if (!gCheatsDisableTrainLengthLimit)
        {
            newCarsPerTrain = std::min(maxCarsPerTrain, newCarsPerTrain);
        }
        ride->min_max_cars_per_train = maxCarsPerTrain | (rideEntry->min_cars_in_train << 4);

        switch (ride->mode)
        {
            case RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED:
            case RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED:
                maxNumTrains = std::clamp(ride->num_stations + ride->num_block_brakes - 1, 1, 31);
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
                for (int32_t i = 0; i < newCarsPerTrain; i++)
                {
                    vehicleEntry = &rideEntry->vehicles[ride_entry_get_vehicle_at_position(ride->subtype, newCarsPerTrain, i)];
                    trainLength += vehicleEntry->spacing;
                }

                int32_t totalLength = trainLength / 2;
                if (newCarsPerTrain != 1)
                    totalLength /= 2;

                maxNumTrains = 0;
                do
                {
                    maxNumTrains++;
                    totalLength += trainLength;
                } while (totalLength <= stationLength);

                if ((ride->mode != RIDE_MODE_STATION_TO_STATION && ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT)
                    || !(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_MORE_VEHICLES_THAN_STATION_FITS))
                {
                    maxNumTrains = std::min(maxNumTrains, 31);
                }
                else
                {
                    vehicleEntry = &rideEntry->vehicles[ride_entry_get_vehicle_at_position(ride->subtype, newCarsPerTrain, 0)];
                    int32_t speed = vehicleEntry->powered_max_speed;

                    int32_t totalSpacing = 0;
                    for (int32_t i = 0; i < newCarsPerTrain; i++)
                    {
                        vehicleEntry = &rideEntry
                                            ->vehicles[ride_entry_get_vehicle_at_position(ride->subtype, newCarsPerTrain, i)];
                        totalSpacing += vehicleEntry->spacing;
                    }

                    totalSpacing >>= 13;
                    int32_t trackLength = ride_get_track_length(ride) / 4;
                    if (speed > 10)
                        trackLength = (trackLength * 3) / 4;
                    if (speed > 25)
                        trackLength = (trackLength * 3) / 4;
                    if (speed > 40)
                        trackLength = (trackLength * 3) / 4;

                    maxNumTrains = 0;
                    int32_t length = 0;
                    do
                    {
                        maxNumTrains++;
                        length += totalSpacing;
                    } while (maxNumTrains < 31 && length < trackLength);
                }
                break;
        }
        ride->max_trains = maxNumTrains;

        numCarsPerTrain = std::min(ride->proposed_num_cars_per_train, (uint8_t)newCarsPerTrain);
    }
    else
    {
        ride->max_trains = rideEntry->cars_per_flat_ride;
        ride->min_max_cars_per_train = rideEntry->max_cars_in_train | (rideEntry->min_cars_in_train << 4);
        numCarsPerTrain = rideEntry->max_cars_in_train;
        maxNumTrains = rideEntry->cars_per_flat_ride;
    }

    if (gCheatsDisableTrainLengthLimit)
    {
        maxNumTrains = 31;
    }
    numVehicles = std::min(ride->proposed_num_vehicles, (uint8_t)maxNumTrains);

    // Refresh new current num vehicles / num cars per vehicle
    if (numVehicles != ride->num_vehicles || numCarsPerTrain != ride->num_cars_per_train)
    {
        ride->num_cars_per_train = numCarsPerTrain;
        ride->num_vehicles = numVehicles;
        window_invalidate_by_number(WC_RIDE, ride->id);
    }
}

void ride_set_ride_entry(ride_id_t rideIndex, int32_t rideEntry)
{
    gGameCommandErrorTitle = STR_RIDE_SET_VEHICLE_TYPE_FAIL;
    game_do_command(
        0, GAME_COMMAND_FLAG_APPLY | (RIDE_SET_VEHICLES_COMMAND_TYPE_RIDE_ENTRY << 8), 0, (rideEntry << 8) | rideIndex,
        GAME_COMMAND_SET_RIDE_VEHICLES, 0, 0);
}

void ride_set_num_vehicles(ride_id_t rideIndex, int32_t numVehicles)
{
    gGameCommandErrorTitle = STR_RIDE_SET_VEHICLE_SET_NUM_TRAINS_FAIL;
    game_do_command(
        0, GAME_COMMAND_FLAG_APPLY | (RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_TRAINS << 8), 0, (numVehicles << 8) | rideIndex,
        GAME_COMMAND_SET_RIDE_VEHICLES, 0, 0);
}

void ride_set_num_cars_per_vehicle(ride_id_t rideIndex, int32_t numCarsPerVehicle)
{
    gGameCommandErrorTitle = STR_RIDE_SET_VEHICLE_SET_NUM_CARS_PER_TRAIN_FAIL;
    game_do_command(
        0, GAME_COMMAND_FLAG_APPLY | (RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_CARS_PER_TRAIN << 8), 0,
        (numCarsPerVehicle << 8) | rideIndex, GAME_COMMAND_SET_RIDE_VEHICLES, 0, 0);
}

static bool ride_is_vehicle_type_valid(Ride* ride, uint8_t inputRideEntryIndex)
{
    bool selectionShouldBeExpanded;
    int32_t rideTypeIterator, rideTypeIteratorMax;

    if (gCheatsShowVehiclesFromOtherTrackTypes
        && !(ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) || ride->type == RIDE_TYPE_MAZE
             || ride->type == RIDE_TYPE_MINI_GOLF))
    {
        selectionShouldBeExpanded = true;
        rideTypeIterator = 0;
        rideTypeIteratorMax = RIDE_TYPE_COUNT - 1;
    }
    else
    {
        selectionShouldBeExpanded = false;
        rideTypeIterator = ride->type;
        rideTypeIteratorMax = ride->type;
    }

    for (; rideTypeIterator <= rideTypeIteratorMax; rideTypeIterator++)
    {
        if (selectionShouldBeExpanded)
        {
            if (ride_type_has_flag(rideTypeIterator, RIDE_TYPE_FLAG_FLAT_RIDE))
                continue;
            if (rideTypeIterator == RIDE_TYPE_MAZE || rideTypeIterator == RIDE_TYPE_MINI_GOLF)
                continue;
        }

        uint8_t* rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(rideTypeIterator);
        for (uint8_t* currentRideEntryIndex = rideEntryIndexPtr; *currentRideEntryIndex != RIDE_ENTRY_INDEX_NULL;
             currentRideEntryIndex++)
        {
            uint8_t rideEntryIndex = *currentRideEntryIndex;
            if (rideEntryIndex == inputRideEntryIndex)
            {
                if (!ride_entry_is_invented(rideEntryIndex) && !gCheatsIgnoreResearchStatus)
                {
                    return false;
                }

                return true;
            }
        }
    }

    return false;
}

static money32 ride_set_vehicles(ride_id_t rideIndex, uint8_t setting, uint8_t value, uint32_t flags, uint8_t ex)
{
    rct_ride_entry* rideEntry;

    Ride* ride = get_ride(rideIndex);
    if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
    {
        log_warning("Invalid game command for ride #%u", rideIndex);
        return MONEY32_UNDEFINED;
    }

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS;

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        gGameCommandErrorText = STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING;
        return MONEY32_UNDEFINED;
    }

    if (ride->status != RIDE_STATUS_CLOSED)
    {
        gGameCommandErrorText = STR_MUST_BE_CLOSED_FIRST;
        return MONEY32_UNDEFINED;
    }

    switch (setting)
    {
        case RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_TRAINS:
            if (!(flags & GAME_COMMAND_FLAG_APPLY))
            {
                return 0;
            }

            ride_clear_for_construction(ride);
            ride_remove_peeps(ride);
            ride->vehicle_change_timeout = 100;

            ride->proposed_num_vehicles = value;
            break;
        case RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_CARS_PER_TRAIN:
            if (!(flags & GAME_COMMAND_FLAG_APPLY))
            {
                return 0;
            }

            ride_clear_for_construction(ride);
            ride_remove_peeps(ride);
            ride->vehicle_change_timeout = 100;

            invalidate_test_results(ride);
            rideEntry = get_ride_entry(ride->subtype);
            if (!gCheatsDisableTrainLengthLimit)
            {
                value = std::clamp(value, rideEntry->min_cars_in_train, rideEntry->max_cars_in_train);
            }
            ride->proposed_num_cars_per_train = value;
            break;
        case RIDE_SET_VEHICLES_COMMAND_TYPE_RIDE_ENTRY:
        {
            if (!ride_is_vehicle_type_valid(ride, value))
            {
                log_error("Invalid vehicle type.");
                return MONEY32_UNDEFINED;
            }

            if (!(flags & GAME_COMMAND_FLAG_APPLY))
            {
                return 0;
            }

            ride_clear_for_construction(ride);
            ride_remove_peeps(ride);
            ride->vehicle_change_timeout = 100;

            invalidate_test_results(ride);
            ride->subtype = value;
            rideEntry = get_ride_entry(ride->subtype);

            uint8_t preset = ex;
            if (!(flags & GAME_COMMAND_FLAG_NETWORKED))
            {
                preset = ride_get_unused_preset_vehicle_colour(ride->subtype);
            }

            // Validate preset
            vehicle_colour_preset_list* presetList = rideEntry->vehicle_preset_list;
            if (preset >= presetList->count)
            {
                log_error("Unknown vehicle colour preset.");
                return MONEY32_UNDEFINED;
            }

            ride_set_vehicle_colours_to_random_preset(ride, preset);
            if (!gCheatsDisableTrainLengthLimit)
            {
                ride->proposed_num_cars_per_train = std::clamp(
                    ride->proposed_num_cars_per_train, rideEntry->min_cars_in_train, rideEntry->max_cars_in_train);
            }
            break;
        }

        default:
            log_error("Unknown vehicle command.");
            return MONEY32_UNDEFINED;
    }

    ride->num_circuits = 1;
    ride_update_max_vehicles(ride);

    if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
    {
        LocationXYZ16 coord;
        coord.x = ride->overall_view.x * 32 + 16;
        coord.y = ride->overall_view.y * 32 + 16;
        coord.z = tile_element_height(coord.x, coord.y);
        network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
    }

    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    auto intent = Intent(INTENT_ACTION_RIDE_PAINT_RESET_VEHICLE);
    intent.putExtra(INTENT_EXTRA_RIDE_ID, rideIndex);
    windowManager->BroadcastIntent(intent);

    gfx_invalidate_screen();
    return 0;
}

/**
 *
 *  rct2: 0x006B52D4
 */
void game_command_set_ride_vehicles(
    int32_t* eax, int32_t* ebx, [[maybe_unused]] int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi,
    [[maybe_unused]] int32_t* edi, [[maybe_unused]] int32_t* ebp)
{
    ride_id_t rideIndex = *edx & 0xFF;
    uint8_t setting = (*ebx >> 8) & 0xFF;
    uint8_t value = (*edx >> 8) & 0xFF;
    uint32_t flags = *ebx;
    uint8_t ex = *eax & 0xFF;
    *ebx = ride_set_vehicles(rideIndex, setting, value, flags, ex);
}

/**
 *
 *  rct2: 0x006CB945
 */
void sub_6CB945(ride_id_t rideIndex)
{
    Ride* ride = get_ride(rideIndex);
    if (ride->type != RIDE_TYPE_MAZE)
    {
        for (uint8_t stationId = 0; stationId < MAX_STATIONS; ++stationId)
        {
            if (ride->stations[stationId].Start.xy == RCT_XY8_UNDEFINED)
                continue;

            LocationXYZ16 location = {
                (int16_t)(ride->stations[stationId].Start.x * 32),
                (int16_t)(ride->stations[stationId].Start.y * 32),
                (ride->stations[stationId].Height),
            };
            uint8_t direction = 0xFF;

            bool specialTrack = false;
            TileElement* tileElement = nullptr;

            while (true)
            {
                if (direction != 0xFF)
                {
                    location.x -= CoordsDirectionDelta[direction].x;
                    location.y -= CoordsDirectionDelta[direction].y;
                }
                tileElement = map_get_first_element_at(location.x >> 5, location.y >> 5);

                bool trackFound = false;
                do
                {
                    if (tileElement->base_height != location.z)
                        continue;
                    if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                        continue;
                    if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
                        continue;
                    if (tileElement->AsTrack()->GetSequenceIndex() != 0)
                        continue;
                    if (!(TrackSequenceProperties[tileElement->AsTrack()->GetTrackType()][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
                        continue;

                    trackFound = true;
                    break;
                } while (!(tileElement++)->IsLastForTile());

                if (trackFound == false)
                {
                    break;
                }

                tileElement->AsTrack()->SetStationIndex(stationId);
                direction = tileElement->GetDirection();

                if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3))
                {
                    specialTrack = true;
                    break;
                }
            }

            if (specialTrack == false)
            {
                continue;
            }

            const rct_preview_track* trackBlock = get_track_def_from_ride(ride, tileElement->AsTrack()->GetTrackType());
            while ((++trackBlock)->index != 0xFF)
            {
                LocationXYZ16 blockLocation = location;
                map_offset_with_rotation(&blockLocation.x, &blockLocation.y, trackBlock->x, trackBlock->y, direction);

                bool trackFound = false;
                tileElement = map_get_first_element_at(blockLocation.x >> 5, blockLocation.y >> 5);
                do
                {
                    if (blockLocation.z != tileElement->base_height)
                        continue;
                    if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                        continue;
                    if (!(TrackSequenceProperties[tileElement->AsTrack()->GetTrackType()][0] & TRACK_SEQUENCE_FLAG_ORIGIN))
                        continue;

                    trackFound = true;
                    break;
                } while (!(tileElement++)->IsLastForTile());

                if (!trackFound)
                {
                    break;
                }

                tileElement->AsTrack()->SetStationIndex(stationId);
            }
        }
    }

    // Needs room for an entrance and an exit per station, plus one position for the list terminator.
    TileCoordsXYZD locations[(MAX_STATIONS * 2) + 1];
    TileCoordsXYZD* locationList = locations;
    for (uint8_t stationId = 0; stationId < MAX_STATIONS; ++stationId)
    {
        TileCoordsXYZD entrance = ride_get_entrance_location(rideIndex, stationId);
        if (!entrance.isNull())
        {
            *locationList++ = entrance;
            ride_clear_entrance_location(ride, stationId);
        }

        TileCoordsXYZD exit = ride_get_exit_location(rideIndex, stationId);
        if (!exit.isNull())
        {
            *locationList++ = exit;
            ride_clear_exit_location(ride, stationId);
        }
    }
    (*locationList++).x = COORDS_NULL;

    locationList = locations;
    for (; !(*locationList).isNull(); locationList++)
    {
        TileCoordsXYZD* locationList2 = locationList;
        locationList2++;

        bool duplicateLocation = false;
        do
        {
            if ((*locationList).x == (*locationList2).x && (*locationList).y == (*locationList2).y)
            {
                duplicateLocation = true;
                break;
            }
        } while (!(*locationList2++).isNull());

        if (duplicateLocation)
        {
            continue;
        }

        CoordsXY location = { (*locationList).x * 32, (*locationList).y * 32 };

        TileElement* tileElement = map_get_first_element_at(location.x >> 5, location.y >> 5);
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                continue;
            if (tileElement->AsEntrance()->GetRideIndex() != rideIndex)
                continue;
            if (tileElement->AsEntrance()->GetEntranceType() > ENTRANCE_TYPE_RIDE_EXIT)
                continue;

            CoordsXY nextLocation = location;
            nextLocation.x += CoordsDirectionDelta[tileElement->GetDirection()].x;
            nextLocation.y += CoordsDirectionDelta[tileElement->GetDirection()].y;

            bool shouldRemove = true;
            TileElement* trackElement = map_get_first_element_at(nextLocation.x >> 5, nextLocation.y >> 5);
            do
            {
                if (trackElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;
                if (trackElement->AsTrack()->GetRideIndex() != rideIndex)
                    continue;
                if (trackElement->base_height != tileElement->base_height)
                    continue;

                uint8_t trackType = trackElement->AsTrack()->GetTrackType();
                uint8_t trackSequence = trackElement->AsTrack()->GetSequenceIndex();

                uint8_t direction = (tileElement->GetDirection() - trackElement->GetDirectionWithOffset(2)) & 3;

                if (!(TrackSequenceProperties[trackType][trackSequence] & (1 << direction)))
                {
                    continue;
                }

                uint8_t stationId = 0;
                if (trackType != TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP)
                {
                    stationId = trackElement->AsTrack()->GetStationIndex();
                }

                if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT)
                {
                    if (!ride_get_exit_location(ride, stationId).isNull())
                        break;

                    ride_set_exit_location(
                        ride, stationId,
                        { location.x / 32, location.y / 32, ride->stations[stationId].Height,
                          (uint8_t)tileElement->GetDirection() });
                }
                else
                {
                    if (!ride_get_entrance_location(ride, stationId).isNull())
                        break;

                    TileCoordsXYZD entranceLocation = {
                        location.x / 32,
                        location.y / 32,
                        ride->stations[stationId].Height,
                        (uint8_t)tileElement->GetDirection(),
                    };
                    ride_set_entrance_location(ride, stationId, entranceLocation);
                }

                tileElement->AsEntrance()->SetStationIndex(stationId);
                shouldRemove = false;
            } while (!(trackElement++)->IsLastForTile());

            if (shouldRemove == true)
            {
                footpath_queue_chain_reset();
                maze_entrance_hedge_replacement(location.x, location.y, tileElement);
                footpath_remove_edges_at(location.x, location.y, tileElement);
                footpath_update_queue_chains();
                map_invalidate_tile_full(location.x, location.y);
                tile_element_remove(tileElement);
                tileElement--;
            }
        } while (!(tileElement++)->IsLastForTile());
    }
}

void ride_set_to_default_inspection_interval(ride_id_t rideIndex)
{
    Ride* ride = get_ride(rideIndex);
    uint8_t defaultInspectionInterval = gConfigGeneral.default_inspection_interval;
    if (ride->inspection_interval != defaultInspectionInterval)
    {
        if (defaultInspectionInterval <= RIDE_INSPECTION_NEVER)
        {
            gGameCommandErrorTitle = STR_CANT_CHANGE_OPERATING_MODE;
            game_do_command(
                0, (defaultInspectionInterval << 8) | 1, 0, (5 << 8) | rideIndex, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
        }
    }
}

/**
 *
 *  rct2: 0x006B752C
 */
void ride_crash(ride_id_t rideIndex, uint8_t vehicleIndex)
{
    Ride* ride = get_ride(rideIndex);
    rct_vehicle* vehicle = GET_VEHICLE(ride->vehicles[vehicleIndex]);

    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        // Open ride window for crashed vehicle
        auto intent = Intent(WD_VEHICLE);
        intent.putExtra(INTENT_EXTRA_VEHICLE, vehicle);
        rct_window* w = context_open_intent(&intent);

        rct_viewport* viewport = window_get_viewport(w);
        if (w != nullptr && viewport != nullptr)
        {
            viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
        }
    }

    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);
    if (gConfigNotifications.ride_crashed)
    {
        news_item_add_to_queue(NEWS_ITEM_RIDE, STR_RIDE_HAS_CRASHED, rideIndex);
    }
}

void ride_reset_all_names()
{
    int32_t i;
    Ride* ride;
    char rideNameBuffer[256];
    ride_name_args name_args;

    FOR_ALL_RIDES (i, ride)
    {
        ride->name = STR_NONE;

        name_args.type_name = RideNaming[ride->type].name;
        name_args.number = 0;
        do
        {
            name_args.number++;
            format_string(rideNameBuffer, 256, 1, &name_args);
        } while (ride_name_exists(rideNameBuffer));

        ride->name = 1;
        ride->name_arguments_type_name = name_args.type_name;
        ride->name_arguments_number = name_args.number;
    }
}

const uint8_t* ride_seek_available_modes(Ride* ride)
{
    const uint8_t* availableModes;

    if (!gCheatsShowAllOperatingModes)
    {
        availableModes = RideAvailableModes;

        for (int32_t i = 0; i < ride->type; i++)
        {
            while (*(availableModes++) != 255)
            {
            }
        }
    }
    else
    {
        availableModes = AllRideModesAvailable;
    }

    return availableModes;
}

// Gets the approximate value of customers per hour for this ride. Multiplies ride_customers_in_last_5_minutes() by 12.
uint32_t ride_customers_per_hour(const Ride* ride)
{
    return ride_customers_in_last_5_minutes(ride) * 12;
}

// Calculates the number of customers for this ride in the last 5 minutes (or more correctly 9600 game ticks)
uint32_t ride_customers_in_last_5_minutes(const Ride* ride)
{
    uint32_t sum = 0;

    for (int32_t i = 0; i < CUSTOMER_HISTORY_SIZE; i++)
    {
        sum += ride->num_customers[i];
    }

    return sum;
}

rct_vehicle* ride_get_broken_vehicle(Ride* ride)
{
    uint16_t vehicleIndex = ride->vehicles[ride->broken_vehicle];

    if (vehicleIndex == SPRITE_INDEX_NULL)
    {
        return nullptr;
    }

    rct_vehicle* vehicle = GET_VEHICLE(vehicleIndex);
    for (uint8_t i = 0; i < ride->broken_car; i++)
    {
        vehicle = GET_VEHICLE(vehicle->next_vehicle_on_train);
    }

    return vehicle;
}

/**
 *
 *  rct2: 0x006D235B
 */
void ride_delete(ride_id_t rideIndex)
{
    Ride* ride = get_ride(rideIndex);
    user_string_free(ride->name);
    ride->type = RIDE_TYPE_NULL;
}

void ride_renew(Ride* ride)
{
    // Set build date to current date (so the ride is brand new)
    ride->build_date = gDateMonthsElapsed;
    ride->reliability = RIDE_INITIAL_RELIABILITY;
}

static bool ride_is_ride(Ride* ride)
{
    switch (ride->type)
    {
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

money16 ride_get_price(Ride* ride)
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
        return 0;
    if (ride_is_ride(ride))
    {
        if (!park_ride_prices_unlocked())
        {
            return 0;
        }
    }
    return ride->price;
}

/**
 * Return the tile_element of an adjacent station at x,y,z(+-2).
 * Returns nullptr if no suitable tile_element is found.
 */
TileElement* get_station_platform(int32_t x, int32_t y, int32_t z, int32_t z_tolerance)
{
    bool foundTileElement = false;
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                continue;
            /* Check if tileElement is a station platform. */
            if (!track_element_is_station(tileElement))
                continue;

            if (z - z_tolerance > tileElement->base_height || z + z_tolerance < tileElement->base_height)
            {
                /* The base height if tileElement is not within
                 * the z tolerance. */
                continue;
            }

            foundTileElement = true;
            break;
        } while (!(tileElement++)->IsLastForTile());
    }
    if (!foundTileElement)
    {
        return nullptr;
    }

    return tileElement;
}

/**
 * Check for an adjacent station to x,y,z in direction.
 */
static bool check_for_adjacent_station(int32_t x, int32_t y, int32_t z, uint8_t direction)
{
    bool found = false;
    int32_t adjX = x;
    int32_t adjY = y;
    for (uint32_t i = 0; i <= RIDE_ADJACENCY_CHECK_DISTANCE; i++)
    {
        adjX += CoordsDirectionDelta[direction].x;
        adjY += CoordsDirectionDelta[direction].y;
        TileElement* stationElement = get_station_platform(adjX, adjY, z, 2);
        if (stationElement != nullptr)
        {
            ride_id_t rideIndex = stationElement->AsTrack()->GetRideIndex();
            Ride* ride = get_ride(rideIndex);
            if (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)
            {
                found = true;
            }
        }
    }
    return found;
}

/**
 * Return whether ride has at least one adjacent station to it.
 */
bool ride_has_adjacent_station(Ride* ride)
{
    bool found = false;

    /* Loop through all of the ride stations, checking for an
     * adjacent station on either side. */
    for (int32_t stationNum = 0; stationNum < MAX_STATIONS; stationNum++)
    {
        if (ride->stations[stationNum].Start.xy != RCT_XY8_UNDEFINED)
        {
            /* Get the map element for the station start. */
            uint16_t stationX = ride->stations[stationNum].Start.x * 32;
            uint16_t stationY = ride->stations[stationNum].Start.y * 32;
            uint8_t stationZ = ride->stations[stationNum].Height;

            TileElement* stationElement = get_station_platform(stationX, stationY, stationZ, 0);
            if (stationElement == nullptr)
            {
                continue;
            }
            /* Check the first side of the station */
            int32_t direction = stationElement->GetDirectionWithOffset(1);
            found = check_for_adjacent_station(stationX, stationY, stationZ, direction);
            if (found)
                break;
            /* Check the other side of the station */
            direction = direction_reverse(direction);
            found = check_for_adjacent_station(stationX, stationY, stationZ, direction);
            if (found)
                break;
        }
    }
    return found;
}

bool ride_has_station_shelter(Ride* ride)
{
    auto stationObj = ride_get_station_object(ride);
    return stationObj != nullptr && stationObj->BaseImageId != 0;
}

bool ride_has_ratings(const Ride* ride)
{
    return ride->excitement != RIDE_RATING_UNDEFINED;
}

const char* ride_type_get_enum_name(int32_t rideType)
{
    static constexpr const char* RideTypeEnumNames[RIDE_TYPE_COUNT] = {
        nameof(RIDE_TYPE_SPIRAL_ROLLER_COASTER),
        nameof(RIDE_TYPE_STAND_UP_ROLLER_COASTER),
        nameof(RIDE_TYPE_SUSPENDED_SWINGING_COASTER),
        nameof(RIDE_TYPE_INVERTED_ROLLER_COASTER),
        nameof(RIDE_TYPE_JUNIOR_ROLLER_COASTER),
        nameof(RIDE_TYPE_MINIATURE_RAILWAY),
        nameof(RIDE_TYPE_MONORAIL),
        nameof(RIDE_TYPE_MINI_SUSPENDED_COASTER),
        nameof(RIDE_TYPE_BOAT_HIRE),
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
        nameof(RIDE_TYPE_SWINGING_SHIP),
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
        nameof(RIDE_TYPE_CIRCUS),
        nameof(RIDE_TYPE_GHOST_TRAIN),
        nameof(RIDE_TYPE_TWISTER_ROLLER_COASTER),
        nameof(RIDE_TYPE_WOODEN_ROLLER_COASTER),
        nameof(RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER),
        nameof(RIDE_TYPE_STEEL_WILD_MOUSE),
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
uint8_t ride_entry_get_first_non_null_ride_type(const rct_ride_entry* rideEntry)
{
    for (uint8_t i = 0; i < MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++)
    {
        if (rideEntry->ride_type[i] != RIDE_TYPE_NULL)
        {
            return rideEntry->ride_type[i];
        }
    }
    return RIDE_TYPE_NULL;
}

bool ride_type_supports_boosters(uint8_t rideType)
{
    if (rideType == RIDE_TYPE_LOOPING_ROLLER_COASTER || rideType == RIDE_TYPE_CORKSCREW_ROLLER_COASTER
        || rideType == RIDE_TYPE_TWISTER_ROLLER_COASTER || rideType == RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
        || rideType == RIDE_TYPE_GIGA_COASTER || rideType == RIDE_TYPE_JUNIOR_ROLLER_COASTER)
    {
        return true;
    }
    return false;
}

int32_t get_booster_speed(uint8_t rideType, int32_t rawSpeed)
{
    int8_t shiftFactor = RideProperties[rideType].booster_speed_factor;
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
        // Workaround for an issue with older compilers (GCC 6, Clang 4) which would fail the build
        int8_t shiftFactorAbs = std::abs(shiftFactor);
        return (rawSpeed >> shiftFactorAbs);
    }
}

void fix_invalid_vehicle_sprite_sizes()
{
    Ride* ride;
    uint16_t i;
    FOR_ALL_RIDES (i, ride)
    {
        for (uint16_t j = 0; j < MAX_VEHICLES_PER_RIDE; j++)
        {
            uint16_t rideSpriteIndex = ride->vehicles[j];
            while (rideSpriteIndex != SPRITE_INDEX_NULL)
            {
                rct_vehicle* vehicle = try_get_vehicle(rideSpriteIndex);
                if (vehicle == nullptr)
                {
                    break;
                }

                rct_ride_entry_vehicle* vehicleEntry = vehicle_get_vehicle_entry(vehicle);
                if (vehicleEntry == nullptr)
                {
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

bool ride_entry_has_category(const rct_ride_entry* rideEntry, uint8_t category)
{
    for (int32_t i = 0; i < MAX_CATEGORIES_PER_RIDE; i++)
    {
        if (rideEntry->category[i] == category)
        {
            return true;
        }
    }

    return false;
}

int32_t ride_get_entry_index(int32_t rideType, int32_t rideSubType)
{
    int32_t subType = rideSubType;

    if (subType == RIDE_ENTRY_INDEX_NULL)
    {
        uint8_t* availableRideEntries = get_ride_entry_indices_for_ride_type(rideType);
        for (uint8_t* rideEntryIndex = availableRideEntries; *rideEntryIndex != RIDE_ENTRY_INDEX_NULL; rideEntryIndex++)
        {
            rct_ride_entry* rideEntry = get_ride_entry(*rideEntryIndex);
            if (rideEntry == nullptr)
            {
                return RIDE_ENTRY_INDEX_NULL;
            }

            // Can happen in select-by-track-type mode
            if (!ride_entry_is_invented(*rideEntryIndex) && !gCheatsIgnoreResearchStatus)
            {
                continue;
            }

            if (!RideGroupManager::RideTypeIsIndependent(rideType))
            {
                subType = *rideEntryIndex;
                break;
            }
        }
        if (subType == RIDE_ENTRY_INDEX_NULL)
        {
            subType = availableRideEntries[0];
        }
    }

    return subType;
}

StationObject* ride_get_station_object(const Ride* ride)
{
    auto& objManager = GetContext()->GetObjectManager();
    return static_cast<StationObject*>(objManager.GetLoadedObject(OBJECT_TYPE_STATION, ride->entrance_style));
}

LocationXY16 ride_get_rotated_coords(int16_t x, int16_t y, int16_t z)
{
    LocationXY16 rotatedCoords = { 0, 0 };

    switch (get_current_rotation())
    {
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

    return rotatedCoords;
}

// Normally, a station has at most one entrance and one exit, which are at the same height
// as the station. But in hacked parks, neither can be taken for granted. This code ensures
// that the ride->entrances and ride->exits arrays will point to one of them. There is
// an ever-so-slight chance two entrances/exits for the same station reside on the same tile.
// In cases like this, the one at station height will be considered the "true" one.
// If none exists at that height, newer and higher placed ones take precedence.
void determine_ride_entrance_and_exit_locations()
{
    log_verbose("Inspecting ride entrance / exit locations");

    ride_id_t rideIndex;
    Ride* ride;
    FOR_ALL_RIDES (rideIndex, ride)
    {
        for (int32_t stationIndex = 0; stationIndex < MAX_STATIONS; stationIndex++)
        {
            TileCoordsXYZD entranceLoc = ride->stations[stationIndex].Entrance;
            TileCoordsXYZD exitLoc = ride->stations[stationIndex].Exit;
            bool fixEntrance = false;
            bool fixExit = false;

            // Skip if the station has no entrance
            if (!entranceLoc.isNull())
            {
                const EntranceElement* entranceElement = map_get_ride_entrance_element_at(
                    entranceLoc.x * 32, entranceLoc.y * 32, entranceLoc.z, false);

                if (entranceElement == nullptr || entranceElement->GetRideIndex() != rideIndex
                    || entranceElement->GetStationIndex() != stationIndex)
                {
                    fixEntrance = true;
                }
                else
                {
                    ride->stations[stationIndex].Entrance.direction = (uint8_t)entranceElement->GetDirection();
                }
            }

            if (!exitLoc.isNull())
            {
                const EntranceElement* entranceElement = map_get_ride_exit_element_at(
                    exitLoc.x * 32, exitLoc.y * 32, entranceLoc.z, false);

                if (entranceElement == nullptr || entranceElement->GetRideIndex() != rideIndex
                    || entranceElement->GetStationIndex() != stationIndex)
                {
                    fixExit = true;
                }
                else
                {
                    ride->stations[stationIndex].Exit.direction = (uint8_t)entranceElement->GetDirection();
                }
            }

            if (!fixEntrance && !fixExit)
            {
                continue;
            }

            // At this point, we know we have a disconnected entrance or exit.
            // Search the map to find it. Skip the outer ring of invisible tiles.
            bool alreadyFoundEntrance = false;
            bool alreadyFoundExit = false;
            for (uint8_t x = 1; x < MAXIMUM_MAP_SIZE_TECHNICAL - 1; x++)
            {
                for (uint8_t y = 1; y < MAXIMUM_MAP_SIZE_TECHNICAL - 1; y++)
                {
                    TileElement* tileElement = map_get_first_element_at(x, y);

                    if (tileElement != nullptr)
                    {
                        do
                        {
                            if (tileElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
                            {
                                continue;
                            }
                            const EntranceElement* entranceElement = tileElement->AsEntrance();
                            if (entranceElement->GetRideIndex() != rideIndex)
                            {
                                continue;
                            }
                            if (entranceElement->GetStationIndex() != stationIndex)
                            {
                                continue;
                            }

                            // The expected height is where entrances and exit reside in non-hacked parks.
                            const uint8_t expectedHeight = ride->stations[stationIndex].Height;

                            if (fixEntrance && entranceElement->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
                            {
                                if (alreadyFoundEntrance)
                                {
                                    if (ride->stations[stationIndex].Entrance.z == expectedHeight)
                                        continue;
                                    if (ride->stations[stationIndex].Entrance.z > entranceElement->base_height)
                                        continue;
                                }

                                // Found our entrance
                                TileCoordsXYZD newEntranceLoc = {
                                    x,
                                    y,
                                    entranceElement->base_height,
                                    (uint8_t)entranceElement->GetDirection(),
                                };
                                ride_set_entrance_location(ride, stationIndex, newEntranceLoc);
                                alreadyFoundEntrance = true;

                                log_verbose(
                                    "Fixed disconnected entrance of ride %d, station %d to x = %d, y = %d and z = %d.",
                                    rideIndex, stationIndex, x, y, entranceElement->base_height);
                            }
                            else if (fixExit && entranceElement->GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT)
                            {
                                if (alreadyFoundExit)
                                {
                                    if (ride->stations[stationIndex].Exit.z == expectedHeight)
                                        continue;
                                    if (ride->stations[stationIndex].Exit.z > entranceElement->base_height)
                                        continue;
                                }

                                // Found our exit
                                ride_set_exit_location(
                                    ride, stationIndex,
                                    { x, y, entranceElement->base_height, (uint8_t)entranceElement->GetDirection() });
                                alreadyFoundExit = true;

                                log_verbose(
                                    "Fixed disconnected exit of ride %d, station %d to x = %d, y = %d and z = %d.", rideIndex,
                                    stationIndex, x, y, entranceElement->base_height);
                            }
                        } while (!(tileElement++)->IsLastForTile());
                    }
                }
            }

            if (fixEntrance && !alreadyFoundEntrance)
            {
                ride_clear_entrance_location(ride, stationIndex);
                log_verbose("Cleared disconnected entrance of ride %d, station %d.", rideIndex, stationIndex);
            }
            if (fixExit && !alreadyFoundExit)
            {
                ride_clear_exit_location(ride, stationIndex);
                log_verbose("Cleared disconnected exit of ride %d, station %d.", rideIndex, stationIndex);
            }
        }
    }
}

void ride_clear_leftover_entrances(ride_id_t rideIndex)
{
    tile_element_iterator it;

    tile_element_iterator_begin(&it);
    while (tile_element_iterator_next(&it))
    {
        if (it.element->GetType() == TILE_ELEMENT_TYPE_ENTRANCE
            && it.element->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE
            && it.element->AsEntrance()->GetRideIndex() == rideIndex)
        {
            tile_element_remove(it.element);
            tile_element_iterator_restart_for_tile(&it);
        }
    }
}
