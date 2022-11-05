/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
#include "../actions/ResultWithMessage.h"
#include "../actions/RideSetSettingAction.h"
#include "../actions/RideSetStatusAction.h"
#include "../actions/RideSetVehicleAction.h"
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../common.h"
#include "../config/Config.h"
#include "../core/BitSet.hpp"
#include "../core/FixedVector.h"
#include "../core/Guard.hpp"
#include "../core/Numerics.hpp"
#include "../entity/EntityRegistry.h"
#include "../entity/Peep.h"
#include "../entity/Staff.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Formatter.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../network/network.h"
#include "../object/MusicObject.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/RideObject.h"
#include "../object/StationObject.h"
#include "../paint/VirtualFloor.h"
#include "../profiling/Profiling.h"
#include "../rct1/RCT1.h"
#include "../scenario/Scenario.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Banner.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/TileElementsView.h"
#include "CableLift.h"
#include "RideAudio.h"
#include "RideConstruction.h"
#include "RideData.h"
#include "RideEntry.h"
#include "ShopItem.h"
#include "Station.h"
#include "Track.h"
#include "TrackData.h"
#include "TrackDesign.h"
#include "TrainManager.h"
#include "Vehicle.h"

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <optional>

using namespace OpenRCT2;
using namespace OpenRCT2::TrackMetaData;

RideMode& operator++(RideMode& d, int)
{
    return d = (d == RideMode::Count) ? RideMode::Normal : static_cast<RideMode>(static_cast<uint8_t>(d) + 1);
}

static constexpr const int32_t RideInspectionInterval[] = {
    10, 20, 30, 45, 60, 120, 0, 0,
};

static std::vector<Ride> _rides;

struct StationIndexWithMessage
{
    ::StationIndex StationIndex;
    StringId Message = STR_NONE;
};

// Static function declarations
Staff* find_closest_mechanic(const CoordsXY& entrancePosition, int32_t forInspection);
static void ride_breakdown_status_update(Ride* ride);
static void ride_breakdown_update(Ride* ride);
static void ride_call_closest_mechanic(Ride* ride);
static void ride_call_mechanic(Ride* ride, Peep* mechanic, int32_t forInspection);
static void ride_entrance_exit_connected(Ride* ride);
static int32_t ride_get_new_breakdown_problem(Ride* ride);
static void ride_inspection_update(Ride* ride);
static void ride_mechanic_status_update(Ride* ride, int32_t mechanicStatus);
static void RideMusicUpdate(Ride* ride);
static void ride_shop_connected(Ride* ride);

RideManager GetRideManager()
{
    return {};
}

size_t RideManager::size() const
{
    size_t count = 0;
    for (size_t i = 0; i < _rides.size(); i++)
    {
        if (_rides[i].type != RIDE_TYPE_NULL)
        {
            count++;
        }
    }
    return count;
}

RideManager::Iterator RideManager::begin()
{
    const auto endIndex = static_cast<RideId::UnderlyingType>(_rides.size());
    return RideManager::Iterator(*this, 0u, endIndex);
}

RideManager::Iterator RideManager::end()
{
    const auto endIndex = static_cast<RideId::UnderlyingType>(_rides.size());
    return RideManager::Iterator(*this, endIndex, endIndex);
}

RideManager::Iterator RideManager::get(RideId rideId)
{
    return RideManager::Iterator(*this, rideId.ToUnderlying(), _rides.size());
}

RideId GetNextFreeRideId()
{
    auto result = static_cast<RideId::UnderlyingType>(_rides.size());
    for (RideId::UnderlyingType i = 0; i < _rides.size(); i++)
    {
        if (_rides[i].type == RIDE_TYPE_NULL)
        {
            result = i;
            break;
        }
    }
    if (result >= OpenRCT2::Limits::MaxRidesInPark)
    {
        return RideId::GetNull();
    }
    return RideId::FromUnderlying(result);
}

Ride* GetOrAllocateRide(RideId index)
{
    const auto idx = index.ToUnderlying();
    if (_rides.size() <= idx)
    {
        _rides.resize(idx + 1);
    }

    auto result = &_rides[idx];
    result->id = index;
    return result;
}

Ride* get_ride(RideId index)
{
    const auto idx = index.ToUnderlying();
    if (idx < _rides.size())
    {
        auto& ride = _rides[idx];
        if (ride.type != RIDE_TYPE_NULL)
        {
            assert(ride.id == index);
            return &ride;
        }
    }
    return nullptr;
}

rct_ride_entry* get_ride_entry(ObjectEntryIndex index)
{
    rct_ride_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();

    auto obj = objMgr.GetLoadedObject(ObjectType::Ride, index);
    if (obj != nullptr)
    {
        result = static_cast<rct_ride_entry*>(obj->GetLegacyData());
    }

    return result;
}

std::string_view get_ride_entry_name(ObjectEntryIndex index)
{
    if (index >= object_entry_group_counts[EnumValue(ObjectType::Ride)])
    {
        log_error("invalid index %d for ride type", index);
        return {};
    }

    auto objectEntry = object_entry_get_object(ObjectType::Ride, index);
    if (objectEntry != nullptr)
    {
        return objectEntry->GetLegacyIdentifier();
    }
    return {};
}

rct_ride_entry* Ride::GetRideEntry() const
{
    return get_ride_entry(subtype);
}

int32_t ride_get_count()
{
    return static_cast<int32_t>(GetRideManager().size());
}

size_t Ride::GetNumPrices() const
{
    size_t result = 0;
    const auto& rtd = GetRideTypeDescriptor();
    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_CASH_MACHINE) || rtd.HasFlag(RIDE_TYPE_FLAG_IS_FIRST_AID))
    {
        result = 0;
    }
    else if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET))
    {
        result = 1;
    }
    else
    {
        result = 1;

        auto rideEntry = GetRideEntry();
        if (rideEntry != nullptr)
        {
            if (lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
            {
                result++;
            }
            else if (rideEntry->shop_item[1] != ShopItem::None)
            {
                result++;
            }
        }
    }
    return result;
}

int32_t Ride::GetAge() const
{
    return gDateMonthsElapsed - build_date;
}

int32_t Ride::GetTotalQueueLength() const
{
    int32_t queueLength = 0;
    for (const auto& station : stations)
        if (!station.Entrance.IsNull())
            queueLength += station.QueueLength;
    return queueLength;
}

int32_t Ride::GetMaxQueueTime() const
{
    uint8_t queueTime = 0;
    for (const auto& station : stations)
        if (!station.Entrance.IsNull())
            queueTime = std::max(queueTime, station.QueueTime);
    return static_cast<int32_t>(queueTime);
}

Guest* Ride::GetQueueHeadGuest(StationIndex stationIndex) const
{
    Guest* peep;
    Guest* result = nullptr;
    auto spriteIndex = GetStation(stationIndex).LastPeepInQueue;
    while ((peep = TryGetEntity<Guest>(spriteIndex)) != nullptr)
    {
        spriteIndex = peep->GuestNextInQueue;
        result = peep;
    }
    return result;
}

void Ride::UpdateQueueLength(StationIndex stationIndex)
{
    uint16_t count = 0;
    Guest* peep;
    auto& station = GetStation(stationIndex);
    auto spriteIndex = station.LastPeepInQueue;
    while ((peep = TryGetEntity<Guest>(spriteIndex)) != nullptr)
    {
        spriteIndex = peep->GuestNextInQueue;
        count++;
    }
    station.QueueLength = count;
}

void Ride::QueueInsertGuestAtFront(StationIndex stationIndex, Guest* peep)
{
    assert(stationIndex.ToUnderlying() < OpenRCT2::Limits::MaxStationsPerRide);
    assert(peep != nullptr);

    peep->GuestNextInQueue = EntityId::GetNull();
    auto* queueHeadGuest = GetQueueHeadGuest(peep->CurrentRideStation);
    if (queueHeadGuest == nullptr)
    {
        GetStation(peep->CurrentRideStation).LastPeepInQueue = peep->sprite_index;
    }
    else
    {
        queueHeadGuest->GuestNextInQueue = peep->sprite_index;
    }
    UpdateQueueLength(peep->CurrentRideStation);
}

/**
 *
 *  rct2: 0x006AC916
 */
void ride_update_favourited_stat()
{
    for (auto& ride : GetRideManager())
        ride.guests_favourite = 0;

    for (auto peep : EntityList<Guest>())
    {
        if (!peep->FavouriteRide.IsNull())
        {
            auto ride = get_ride(peep->FavouriteRide);
            if (ride != nullptr)
            {
                ride->guests_favourite++;
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
            }
        }
    }

    window_invalidate_by_class(WindowClass::RideList);
}

/**
 *
 *  rct2: 0x006AC3AB
 */
money64 Ride::CalculateIncomePerHour() const
{
    // Get entry by ride to provide better reporting
    rct_ride_entry* entry = GetRideEntry();
    if (entry == nullptr)
    {
        return 0;
    }
    auto customersPerHour = ride_customers_per_hour(this);
    money64 priceMinusCost = ride_get_price(this);

    ShopItem currentShopItem = entry->shop_item[0];
    if (currentShopItem != ShopItem::None)
    {
        priceMinusCost -= GetShopItemDescriptor(currentShopItem).Cost;
    }

    currentShopItem = (lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) ? GetRideTypeDescriptor().PhotoItem
                                                                       : entry->shop_item[1];

    if (currentShopItem != ShopItem::None)
    {
        const money16 shopItemProfit = price[1] - GetShopItemDescriptor(currentShopItem).Cost;

        if (GetShopItemDescriptor(currentShopItem).IsPhoto())
        {
            const int32_t rideTicketsSold = total_customers - no_secondary_items_sold;

            // Use the ratio between photo sold and total admissions to approximate the photo income(as not every guest will buy
            // one).
            // TODO: use data from the last 5 minutes instead of all-time values for a more accurate calculation
            if (rideTicketsSold > 0)
            {
                priceMinusCost += ((static_cast<int32_t>(no_secondary_items_sold) * shopItemProfit) / rideTicketsSold);
            }
        }
        else
        {
            priceMinusCost += shopItemProfit;
        }

        if (entry->shop_item[0] != ShopItem::None)
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
bool ride_try_get_origin_element(const Ride* ride, CoordsXYE* output)
{
    TileElement* resultTileElement = nullptr;

    tile_element_iterator it;
    TileElementIteratorBegin(&it);
    do
    {
        if (it.element->GetType() != TileElementType::Track)
            continue;
        if (it.element->AsTrack()->GetRideIndex() != ride->id)
            continue;

        // Found a track piece for target ride

        // Check if it's not the station or ??? (but allow end piece of station)
        const auto& ted = GetTrackElementDescriptor(it.element->AsTrack()->GetTrackType());
        bool specialTrackPiece
            = (it.element->AsTrack()->GetTrackType() != TrackElemType::BeginStation
               && it.element->AsTrack()->GetTrackType() != TrackElemType::MiddleStation
               && (std::get<0>(ted.SequenceProperties) & TRACK_SEQUENCE_FLAG_ORIGIN));

        // Set result tile to this track piece if first found track or a ???
        if (resultTileElement == nullptr || specialTrackPiece)
        {
            resultTileElement = it.element;

            if (output != nullptr)
            {
                output->element = resultTileElement;
                output->x = it.x * COORDS_XY_STEP;
                output->y = it.y * COORDS_XY_STEP;
            }
        }

        if (specialTrackPiece)
        {
            return true;
        }
    } while (TileElementIteratorNext(&it));

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
    const CoordsXYZ& startPos, Ride* ride, uint8_t direction_start, CoordsXYE* output, int32_t* z, int32_t* direction,
    bool isGhost)
{
    auto trackPos = startPos;

    if (!(direction_start & TRACK_BLOCK_2))
    {
        trackPos += CoordsDirectionDelta[direction_start];
    }

    TileElement* tileElement = MapGetFirstElementAt(trackPos);
    if (tileElement == nullptr)
    {
        output->element = nullptr;
        output->x = LOCATION_NULL;
        return false;
    }

    do
    {
        auto trackElement = tileElement->AsTrack();
        if (trackElement == nullptr)
            continue;

        if (trackElement->GetRideIndex() != ride->id)
            continue;

        if (trackElement->GetSequenceIndex() != 0)
            continue;

        if (tileElement->IsGhost() != isGhost)
            continue;

        const auto& ted = GetTrackElementDescriptor(trackElement->GetTrackType());
        const auto* nextTrackBlock = ted.Block;
        if (nextTrackBlock == nullptr)
            continue;

        const auto& nextTrackCoordinate = ted.Coordinates;
        uint8_t nextRotation = tileElement->GetDirectionWithOffset(nextTrackCoordinate.rotation_begin)
            | (nextTrackCoordinate.rotation_begin & TRACK_BLOCK_2);

        if (nextRotation != direction_start)
            continue;

        int16_t nextZ = nextTrackCoordinate.z_begin - nextTrackBlock->z + tileElement->GetBaseZ();
        if (nextZ != trackPos.z)
            continue;

        if (z != nullptr)
            *z = tileElement->GetBaseZ();
        if (direction != nullptr)
            *direction = nextRotation;
        *output = { trackPos, tileElement };
        return true;
    } while (!(tileElement++)->IsLastForTile());

    if (direction != nullptr)
        *direction = direction_start;
    if (z != nullptr)
        *z = trackPos.z;
    *output = { trackPos, --tileElement };
    return false;
}

/**
 *
 *  rct2: 0x006C60C2
 */
bool track_block_get_next(CoordsXYE* input, CoordsXYE* output, int32_t* z, int32_t* direction)
{
    if (input == nullptr || input->element == nullptr)
        return false;

    auto inputElement = input->element->AsTrack();
    if (inputElement == nullptr)
        return false;

    auto rideIndex = inputElement->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return false;

    const auto& ted = GetTrackElementDescriptor(inputElement->GetTrackType());
    const auto* trackBlock = ted.Block;
    if (trackBlock == nullptr)
        return false;

    // The sequence index may be higher than the amount of sequences actually present.
    // We don’t know the amount of sequences present in the block upfront, but there is an end marker consisting of all 255s.
    const auto sequenceIndex = inputElement->GetSequenceIndex();
    for (auto i = 0; i < sequenceIndex; i++)
    {
        trackBlock++;

        if (trackBlock == nullptr || trackBlock->index == 255)
            return false;
    }

    const auto& trackCoordinate = ted.Coordinates;

    int32_t x = input->x;
    int32_t y = input->y;
    int32_t OriginZ = inputElement->GetBaseZ();

    uint8_t rotation = inputElement->GetDirection();

    CoordsXY coords = { x, y };
    CoordsXY trackCoordOffset = { trackCoordinate.x, trackCoordinate.y };
    CoordsXY trackBlockOffset = { trackBlock->x, trackBlock->y };
    coords += trackCoordOffset.Rotate(rotation);
    coords += trackBlockOffset.Rotate(DirectionReverse(rotation));

    OriginZ -= trackBlock->z;
    OriginZ += trackCoordinate.z_end;

    uint8_t directionStart = ((trackCoordinate.rotation_end + rotation) & TILE_ELEMENT_DIRECTION_MASK)
        | (trackCoordinate.rotation_end & TRACK_BLOCK_2);

    return track_block_get_next_from_zero({ coords, OriginZ }, ride, directionStart, output, z, direction, false);
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
    const CoordsXYZ& startPos, Ride* ride, uint8_t direction, track_begin_end* outTrackBeginEnd)
{
    uint8_t directionStart = direction;
    direction = DirectionReverse(direction);
    auto trackPos = startPos;

    if (!(direction & TRACK_BLOCK_2))
    {
        trackPos += CoordsDirectionDelta[direction];
    }

    TileElement* tileElement = MapGetFirstElementAt(trackPos);
    if (tileElement == nullptr)
    {
        outTrackBeginEnd->end_x = trackPos.x;
        outTrackBeginEnd->end_y = trackPos.y;
        outTrackBeginEnd->begin_element = nullptr;
        outTrackBeginEnd->begin_direction = DirectionReverse(directionStart);
        return false;
    }

    do
    {
        auto trackElement = tileElement->AsTrack();
        if (trackElement == nullptr)
            continue;

        if (trackElement->GetRideIndex() != ride->id)
            continue;

        const auto* ted = &GetTrackElementDescriptor(trackElement->GetTrackType());
        const auto* nextTrackBlock = ted->Block;
        if (nextTrackBlock == nullptr)
            continue;
        const auto& nextTrackCoordinate = ted->Coordinates;

        nextTrackBlock += trackElement->GetSequenceIndex();
        if ((nextTrackBlock + 1)->index != 255)
            continue;

        uint8_t nextRotation = tileElement->GetDirectionWithOffset(nextTrackCoordinate.rotation_end)
            | (nextTrackCoordinate.rotation_end & TRACK_BLOCK_2);

        if (nextRotation != directionStart)
            continue;

        int16_t nextZ = nextTrackCoordinate.z_end - nextTrackBlock->z + tileElement->GetBaseZ();
        if (nextZ != trackPos.z)
            continue;

        nextRotation = tileElement->GetDirectionWithOffset(nextTrackCoordinate.rotation_begin)
            | (nextTrackCoordinate.rotation_begin & TRACK_BLOCK_2);
        outTrackBeginEnd->begin_element = tileElement;
        outTrackBeginEnd->begin_x = trackPos.x;
        outTrackBeginEnd->begin_y = trackPos.y;
        outTrackBeginEnd->end_x = trackPos.x;
        outTrackBeginEnd->end_y = trackPos.y;

        CoordsXY coords = { outTrackBeginEnd->begin_x, outTrackBeginEnd->begin_y };
        CoordsXY offsets = { nextTrackCoordinate.x, nextTrackCoordinate.y };
        coords += offsets.Rotate(DirectionReverse(nextRotation));
        outTrackBeginEnd->begin_x = coords.x;
        outTrackBeginEnd->begin_y = coords.y;

        outTrackBeginEnd->begin_z = tileElement->GetBaseZ();

        ted = &GetTrackElementDescriptor(trackElement->GetTrackType());
        const auto* nextTrackBlock2 = ted->Block;
        if (nextTrackBlock2 == nullptr)
            continue;

        outTrackBeginEnd->begin_z += nextTrackBlock2->z - nextTrackBlock->z;
        outTrackBeginEnd->begin_direction = nextRotation;
        outTrackBeginEnd->end_direction = DirectionReverse(directionStart);
        return true;
    } while (!(tileElement++)->IsLastForTile());

    outTrackBeginEnd->end_x = trackPos.x;
    outTrackBeginEnd->end_y = trackPos.y;
    outTrackBeginEnd->begin_z = trackPos.z;
    outTrackBeginEnd->begin_element = nullptr;
    outTrackBeginEnd->end_direction = DirectionReverse(directionStart);
    return false;
}

/**
 *
 *  rct2: 0x006C6402
 *
 * @remarks outTrackBeginEnd.begin_x and outTrackBeginEnd.begin_y will be in the
 * higher two bytes of ecx and edx where as outTrackBeginEnd.end_x and
 * outTrackBeginEnd.end_y will be in the lower two bytes (cx and dx).
 */
bool track_block_get_previous(const CoordsXYE& trackPos, track_begin_end* outTrackBeginEnd)
{
    if (trackPos.element == nullptr)
        return false;

    auto trackElement = trackPos.element->AsTrack();
    if (trackElement == nullptr)
        return false;

    const auto& ted = GetTrackElementDescriptor(trackElement->GetTrackType());

    auto rideIndex = trackElement->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return false;

    const auto* trackBlock = ted.Block;
    if (trackBlock == nullptr)
        return false;

    trackBlock += trackElement->GetSequenceIndex();

    auto trackCoordinate = ted.Coordinates;

    int32_t z = trackElement->GetBaseZ();

    uint8_t rotation = trackElement->GetDirection();
    CoordsXY coords = CoordsXY{ trackPos };
    CoordsXY offsets = { trackBlock->x, trackBlock->y };
    coords += offsets.Rotate(DirectionReverse(rotation));

    z -= trackBlock->z;
    z += trackCoordinate.z_begin;

    rotation = ((trackCoordinate.rotation_begin + rotation) & TILE_ELEMENT_DIRECTION_MASK)
        | (trackCoordinate.rotation_begin & TRACK_BLOCK_2);

    return track_block_get_previous_from_zero({ coords, z }, ride, rotation, outTrackBeginEnd);
}

/**
 *
 * Make sure to pass in the x and y of the start track element too.
 *  rct2: 0x006CB02F
 * ax result x
 * bx result y
 * esi input / output map element
 */
int32_t ride_find_track_gap(const Ride* ride, CoordsXYE* input, CoordsXYE* output)
{
    if (ride == nullptr || input == nullptr || input->element == nullptr || input->element->GetType() != TileElementType::Track)
        return 0;

    if (ride->type == RIDE_TYPE_MAZE)
    {
        return 0;
    }

    rct_window* w = window_find_by_class(WindowClass::RideConstruction);
    if (w != nullptr && _rideConstructionState != RideConstructionState::State0 && _currentRideIndex == ride->id)
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
        // #2081: prevent an infinite loop
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

void Ride::FormatStatusTo(Formatter& ft) const
{
    if (lifecycle_flags & RIDE_LIFECYCLE_CRASHED)
    {
        ft.Add<StringId>(STR_CRASHED);
    }
    else if (lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        ft.Add<StringId>(STR_BROKEN_DOWN);
    }
    else if (status == RideStatus::Closed)
    {
        if (!GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
        {
            if (num_riders != 0)
            {
                ft.Add<StringId>(num_riders == 1 ? STR_CLOSED_WITH_PERSON : STR_CLOSED_WITH_PEOPLE);
                ft.Add<uint16_t>(num_riders);
            }
            else
            {
                ft.Add<StringId>(STR_CLOSED);
            }
        }
        else
        {
            ft.Add<StringId>(STR_CLOSED);
        }
    }
    else if (status == RideStatus::Simulating)
    {
        ft.Add<StringId>(STR_SIMULATING);
    }
    else if (status == RideStatus::Testing)
    {
        ft.Add<StringId>(STR_TEST_RUN);
    }
    else if (mode == RideMode::Race && !(lifecycle_flags & RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING) && !race_winner.IsNull())
    {
        auto peep = GetEntity<Guest>(race_winner);
        if (peep != nullptr)
        {
            ft.Add<StringId>(STR_RACE_WON_BY);
            peep->FormatNameTo(ft);
        }
        else
        {
            ft.Add<StringId>(STR_RACE_WON_BY);
            ft.Add<StringId>(STR_NONE);
        }
    }
    else if (!GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
    {
        ft.Add<StringId>(num_riders == 1 ? STR_PERSON_ON_RIDE : STR_PEOPLE_ON_RIDE);
        ft.Add<uint16_t>(num_riders);
    }
    else
    {
        ft.Add<StringId>(STR_OPEN);
    }
}

int32_t Ride::GetTotalLength() const
{
    int32_t i, totalLength = 0;
    for (i = 0; i < num_stations; i++)
        totalLength += stations[i].SegmentLength;
    return totalLength;
}

int32_t Ride::GetTotalTime() const
{
    int32_t i, totalTime = 0;
    for (i = 0; i < num_stations; i++)
        totalTime += stations[i].SegmentTime;
    return totalTime;
}

bool Ride::CanHaveMultipleCircuits() const
{
    if (!(GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_ALLOW_MULTIPLE_CIRCUITS)))
        return false;

    // Only allow circuit or launch modes
    if (mode != RideMode::ContinuousCircuit && mode != RideMode::ReverseInclineLaunchedShuttle
        && mode != RideMode::PoweredLaunchPasstrough)
    {
        return false;
    }

    // Must have no more than one vehicle and one station
    if (NumTrains > 1 || num_stations > 1)
        return false;

    return true;
}

bool Ride::SupportsStatus(RideStatus s) const
{
    const auto& rtd = GetRideTypeDescriptor();

    switch (s)
    {
        case RideStatus::Closed:
        case RideStatus::Open:
            return true;
        case RideStatus::Simulating:
            return (!rtd.HasFlag(RIDE_TYPE_FLAG_NO_TEST_MODE) && rtd.HasFlag(RIDE_TYPE_FLAG_HAS_TRACK));
        case RideStatus::Testing:
            return !rtd.HasFlag(RIDE_TYPE_FLAG_NO_TEST_MODE);
        case RideStatus::Count: // Meaningless but necessary to satisfy -Wswitch
            return false;
    }
    // Unreachable
    return false;
}

#pragma region Initialisation functions

/**
 *
 *  rct2: 0x006ACA89
 */
void ride_init_all()
{
    _rides.clear();
    _rides.shrink_to_fit();
}

/**
 *
 *  rct2: 0x006B7A38
 */
void reset_all_ride_build_dates()
{
    for (auto& ride : GetRideManager())
    {
        ride.build_date -= gDateMonthsElapsed;
    }
}

#pragma endregion

#pragma region Construction

#pragma endregion

#pragma region Update functions

/**
 *
 *  rct2: 0x006ABE4C
 */
void Ride::UpdateAll()
{
    PROFILED_FUNCTION();

    // Remove all rides if scenario editor
    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
    {
        switch (gEditorStep)
        {
            case EditorStep::ObjectSelection:
            case EditorStep::LandscapeEditor:
            case EditorStep::InventionsListSetUp:
                for (auto& ride : GetRideManager())
                    ride.Delete();
                break;
            case EditorStep::OptionsSelection:
            case EditorStep::ObjectiveSelection:
            case EditorStep::SaveScenario:
            case EditorStep::RollercoasterDesigner:
            case EditorStep::DesignsManager:
            case EditorStep::Invalid:
                break;
        }
        return;
    }

    window_update_viewport_ride_music();

    // Update rides
    for (auto& ride : GetRideManager())
        ride.Update();

    OpenRCT2::RideAudio::UpdateMusicChannels();
}

std::unique_ptr<TrackDesign> Ride::SaveToTrackDesign(TrackDesignState& tds) const
{
    if (!(lifecycle_flags & RIDE_LIFECYCLE_TESTED))
    {
        context_show_error(STR_CANT_SAVE_TRACK_DESIGN, STR_NONE, {});
        return nullptr;
    }

    if (!ride_has_ratings(this))
    {
        context_show_error(STR_CANT_SAVE_TRACK_DESIGN, STR_NONE, {});
        return nullptr;
    }

    auto td = std::make_unique<TrackDesign>();
    auto errMessage = td->CreateTrackDesign(tds, *this);
    if (errMessage != STR_NONE)
    {
        context_show_error(STR_CANT_SAVE_TRACK_DESIGN, errMessage, {});
        return nullptr;
    }

    return td;
}

RideStation& Ride::GetStation(StationIndex stationIndex)
{
    return stations[stationIndex.ToUnderlying()];
}

const RideStation& Ride::GetStation(StationIndex stationIndex) const
{
    return stations[stationIndex.ToUnderlying()];
}

std::array<RideStation, OpenRCT2::Limits::MaxStationsPerRide>& Ride::GetStations()
{
    return stations;
}

const std::array<RideStation, OpenRCT2::Limits::MaxStationsPerRide>& Ride::GetStations() const
{
    return stations;
}

StationIndex Ride::GetStationIndex(const RideStation* station) const
{
    auto distance = std::distance(stations.data(), station);
    Guard::Assert(distance >= 0 && distance < int32_t(std::size(stations)));
    return StationIndex::FromUnderlying(distance);
}

/**
 *
 *  rct2: 0x006ABE73
 */
void Ride::Update()
{
    if (vehicle_change_timeout != 0)
        vehicle_change_timeout--;

    RideMusicUpdate(this);

    // Update stations
    if (type != RIDE_TYPE_MAZE)
        for (StationIndex::UnderlyingType i = 0; i < OpenRCT2::Limits::MaxStationsPerRide; i++)
            ride_update_station(this, StationIndex::FromUnderlying(i));

    // Update financial statistics
    num_customers_timeout++;

    if (num_customers_timeout >= 960)
    {
        // This is meant to update about every 30 seconds
        num_customers_timeout = 0;

        // Shift number of customers history, start of the array is the most recent one
        for (int32_t i = OpenRCT2::Limits::CustomerHistorySize - 1; i > 0; i--)
        {
            num_customers[i] = num_customers[i - 1];
        }
        num_customers[0] = cur_num_customers;

        cur_num_customers = 0;
        window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;

        income_per_hour = CalculateIncomePerHour();
        window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

        if (upkeep_cost != MONEY16_UNDEFINED)
            profit = (income_per_hour - (static_cast<money32>(upkeep_cost * 16)));
    }

    // Ride specific updates
    const auto& rtd = GetRideTypeDescriptor();
    if (rtd.RideUpdate != nullptr)
        rtd.RideUpdate(*this);

    ride_breakdown_update(this);

    // Various things include news messages
    if (lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION))
    {
        // Breakdown updates originally were performed when (id == (gCurrentTicks / 2) & 0xFF)
        // with the increased MAX_RIDES the update is tied to the first byte of the id this allows
        // for identical balance with vanilla.
        const auto updatingRideByte = static_cast<uint8_t>((gCurrentTicks / 2) & 0xFF);
        if (updatingRideByte == static_cast<uint8_t>(id.ToUnderlying()))
            ride_breakdown_status_update(this);
    }

    ride_inspection_update(this);

    // If ride is simulating but crashed, reset the vehicles
    if (status == RideStatus::Simulating && (lifecycle_flags & RIDE_LIFECYCLE_CRASHED))
    {
        if (mode == RideMode::ContinuousCircuitBlockSectioned || mode == RideMode::PoweredLaunchBlockSectioned)
        {
            // We require this to execute right away during the simulation, always ignore network and queue.
            RideSetStatusAction gameAction = RideSetStatusAction(id, RideStatus::Closed);
            GameActions::ExecuteNested(&gameAction);
        }
        else
        {
            // We require this to execute right away during the simulation, always ignore network and queue.
            RideSetStatusAction gameAction = RideSetStatusAction(id, RideStatus::Simulating);
            GameActions::ExecuteNested(&gameAction);
        }
    }
}

/**
 *
 *  rct2: 0x006AC489
 */
void UpdateChairlift(Ride& ride)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
        return;
    if ((ride.lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        && ride.breakdown_reason_pending == 0)
        return;

    uint16_t old_chairlift_bullwheel_rotation = ride.chairlift_bullwheel_rotation >> 14;
    ride.chairlift_bullwheel_rotation += ride.speed * 2048;
    if (old_chairlift_bullwheel_rotation == ride.speed / 8)
        return;

    auto bullwheelLoc = ride.ChairliftBullwheelLocation[0].ToCoordsXYZ();
    MapInvalidateTileZoom1({ bullwheelLoc, bullwheelLoc.z, bullwheelLoc.z + (4 * COORDS_Z_STEP) });

    bullwheelLoc = ride.ChairliftBullwheelLocation[1].ToCoordsXYZ();
    MapInvalidateTileZoom1({ bullwheelLoc, bullwheelLoc.z, bullwheelLoc.z + (4 * COORDS_Z_STEP) });
}

/**
 *
 *  rct2: 0x0069A3A2
 * edi: ride (in code as bytes offset from start of rides list)
 * bl: happiness
 */
void Ride::UpdateSatisfaction(const uint8_t happiness)
{
    satisfaction_next += happiness;
    satisfaction_time_out++;
    if (satisfaction_time_out >= 20)
    {
        satisfaction = satisfaction_next >> 2;
        satisfaction_next = 0;
        satisfaction_time_out = 0;
        window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
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
void Ride::UpdatePopularity(const uint8_t pop_amount)
{
    popularity_next += pop_amount;
    popularity_time_out++;
    if (popularity_time_out < 25)
        return;

    popularity = popularity_next;
    popularity_next = 0;
    popularity_time_out = 0;
    window_invalidate_flags |= RIDE_INVALIDATE_RIDE_CUSTOMER;
}

/** rct2: 0x0098DDB8, 0x0098DDBA */
static constexpr const CoordsXY ride_spiral_slide_main_tile_offset[][4] = {
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

void UpdateSpiralSlide(Ride& ride)
{
    if (gCurrentTicks & 3)
        return;
    if (ride.slide_in_use == 0)
        return;

    ride.spiral_slide_progress++;
    if (ride.spiral_slide_progress >= 48)
    {
        ride.slide_in_use--;

        auto* peep = GetEntity<Guest>(ride.slide_peep);
        if (peep != nullptr)
        {
            auto destination = peep->GetDestination();
            destination.x++;
            peep->SetDestination(destination);
        }
    }

    const uint8_t current_rotation = get_current_rotation();
    // Invalidate something related to station start
    for (int32_t i = 0; i < OpenRCT2::Limits::MaxStationsPerRide; i++)
    {
        if (ride.stations[i].Start.IsNull())
            continue;

        auto startLoc = ride.stations[i].Start;

        TileElement* tileElement = ride_get_station_start_track_element(&ride, StationIndex::FromUnderlying(i));
        if (tileElement == nullptr)
            continue;

        int32_t rotation = tileElement->GetDirection();
        startLoc += ride_spiral_slide_main_tile_offset[rotation][current_rotation];

        MapInvalidateTileZoom0({ startLoc, tileElement->GetBaseZ(), tileElement->GetClearanceZ() });
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
    3,  // BREAKDOWN_CONTROL_FAILURE
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

    if (ride->GetRideTypeDescriptor().AvailableBreakdowns == 0)
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

    auto stationIndex = ride_get_first_valid_station_exit(ride);
    ride->inspection_station = (!stationIndex.IsNull()) ? stationIndex : StationIndex::FromUnderlying(0);
}

static int32_t get_age_penalty(Ride* ride)
{
    auto years = date_get_year(ride->GetAge());
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

        for (int32_t i = 0; i < OpenRCT2::Limits::DowntimeHistorySize; i++)
        {
            totalDowntime += ride->downtime_history[i];
        }

        ride->downtime = std::min(totalDowntime / 2, 100);

        for (int32_t i = OpenRCT2::Limits::DowntimeHistorySize - 1; i > 0; i--)
        {
            ride->downtime_history[i] = ride->downtime_history[i - 1];
        }
        ride->downtime_history[0] = 0;

        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
    }

    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        return;
    if (ride->status == RideStatus::Closed || ride->status == RideStatus::Simulating)
        return;

    if (!ride->CanBreakDown())
    {
        ride->reliability = RIDE_INITIAL_RELIABILITY;
        return;
    }

    // Calculate breakdown probability?
    int32_t unreliabilityAccumulator = ride->unreliability_factor + get_age_penalty(ride);
    ride->reliability = static_cast<uint16_t>(std::max(0, (ride->reliability - unreliabilityAccumulator)));
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;

    // Random probability of a breakdown. Roughly this is 1 in
    //
    // (25000 - reliability) / 3 000 000
    //
    // a 0.8% chance, less the breakdown factor which accumulates as the game
    // continues.
    if ((ride->reliability == 0
         || static_cast<int32_t>(scenario_rand() & 0x2FFFFF) <= 1 + RIDE_INITIAL_RELIABILITY - ride->reliability)
        && !gCheatsDisableAllBreakdowns)
    {
        int32_t breakdownReason = ride_get_new_breakdown_problem(ride);
        if (breakdownReason != -1)
            ride_prepare_breakdown(ride, breakdownReason);
    }
}

/**
 *
 *  rct2: 0x006B7294
 */
static int32_t ride_get_new_breakdown_problem(Ride* ride)
{
    int32_t availableBreakdownProblems, totalProbability, randomProbability, problemBits, breakdownProblem;

    // Brake failure is more likely when it's raining
    _breakdownProblemProbabilities[BREAKDOWN_BRAKES_FAILURE] = ClimateIsRaining() ? 20 : 3;

    if (!ride->CanBreakDown())
        return -1;

    availableBreakdownProblems = ride->GetRideTypeDescriptor().AvailableBreakdowns;

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
    if (ride->IsBlockSectioned())
        if (ride->NumTrains != 1)
            return -1;

    // If brakes failure is disabled, also take it out of the equation (see above comment why)
    if (gCheatsDisableBrakesFailure)
        return -1;

    auto monthsOld = ride->GetAge();
    if (monthsOld < 16 || ride->reliability_percentage > 50)
        return -1;

    return BREAKDOWN_BRAKES_FAILURE;
}

bool Ride::CanBreakDown() const
{
    if (GetRideTypeDescriptor().AvailableBreakdowns == 0)
    {
        return false;
    }

    rct_ride_entry* entry = GetRideEntry();
    return entry != nullptr && !(entry->flags & RIDE_ENTRY_FLAG_CANNOT_BREAK_DOWN);
}

static void choose_random_train_to_breakdown_safe(Ride* ride)
{
    // Prevent integer division by zero in case of hacked ride.
    if (ride->NumTrains == 0)
        return;

    ride->broken_vehicle = scenario_rand() % ride->NumTrains;

    // Prevent crash caused by accessing SPRITE_INDEX_NULL on hacked rides.
    // This should probably be cleaned up on import instead.
    while (ride->vehicles[ride->broken_vehicle].IsNull() && ride->broken_vehicle != 0)
    {
        --ride->broken_vehicle;
    }
}

/**
 *
 *  rct2: 0x006B7348
 */
void ride_prepare_breakdown(Ride* ride, int32_t breakdownReason)
{
    StationIndex i;
    Vehicle* vehicle;

    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
        return;

    ride->lifecycle_flags |= RIDE_LIFECYCLE_BREAKDOWN_PENDING;

    ride->breakdown_reason_pending = breakdownReason;
    ride->breakdown_sound_modifier = 0;
    ride->not_fixed_timeout = 0;
    ride->inspection_station = StationIndex::FromUnderlying(0); // ensure set to something.

    switch (breakdownReason)
    {
        case BREAKDOWN_SAFETY_CUT_OUT:
        case BREAKDOWN_CONTROL_FAILURE:
            // Inspect first station with an exit
            i = ride_get_first_valid_station_exit(ride);
            if (!i.IsNull())
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
            if (ride->num_cars_per_train != 0)
            {
                ride->broken_car = scenario_rand() % ride->num_cars_per_train;

                // Set flag on broken car
                vehicle = GetEntity<Vehicle>(ride->vehicles[ride->broken_vehicle]);
                if (vehicle != nullptr)
                {
                    vehicle = vehicle->GetCar(ride->broken_car);
                }
                if (vehicle != nullptr)
                {
                    vehicle->SetUpdateFlag(VEHICLE_UPDATE_FLAG_BROKEN_CAR);
                }
            }
            break;
        case BREAKDOWN_VEHICLE_MALFUNCTION:
            // Choose a random train
            choose_random_train_to_breakdown_safe(ride);
            ride->broken_car = 0;

            // Set flag on broken train, first car
            vehicle = GetEntity<Vehicle>(ride->vehicles[ride->broken_vehicle]);
            if (vehicle != nullptr)
            {
                vehicle->SetUpdateFlag(VEHICLE_UPDATE_FLAG_BROKEN_TRAIN);
            }
            break;
        case BREAKDOWN_BRAKES_FAILURE:
            // Original code generates a random number but does not use it
            // Unsure if this was supposed to choose a random station (or random station with an exit)
            i = ride_get_first_valid_station_exit(ride);
            if (!i.IsNull())
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
void ride_breakdown_add_news_item(Ride* ride)
{
    if (gConfigNotifications.RideBrokenDown)
    {
        Formatter ft;
        ride->FormatNameTo(ft);
        News::AddItemToQueue(News::ItemType::Ride, STR_RIDE_IS_BROKEN_DOWN, ride->id.ToUnderlying(), ft);
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
            if (gConfigNotifications.RideWarnings)
            {
                Formatter ft;
                ride->FormatNameTo(ft);
                News::AddItemToQueue(News::ItemType::Ride, STR_RIDE_IS_STILL_NOT_FIXED, ride->id.ToUnderlying(), ft);
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
            ride_breakdown_add_news_item(ride);
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
            if (ride->GetRideTypeDescriptor().AvailableBreakdowns == 0)
            {
                ride->lifecycle_flags &= ~(
                    RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_DUE_INSPECTION);
                break;
            }

            ride_call_closest_mechanic(ride);
            break;
        case RIDE_MECHANIC_STATUS_HEADING:
        {
            auto mechanic = ride_get_mechanic(ride);
            bool rideNeedsRepair = (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN));
            if (mechanic == nullptr
                || (mechanic->State != PeepState::HeadingToInspection && mechanic->State != PeepState::Answering)
                || mechanic->CurrentRide != ride->id)
            {
                ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
                ride_mechanic_status_update(ride, RIDE_MECHANIC_STATUS_CALLING);
            }
            // if the ride is broken down, but a mechanic was heading for an inspection, update orders to fix
            else if (rideNeedsRepair && mechanic->State == PeepState::HeadingToInspection)
            {
                // updates orders for mechanic already heading to inspect ride
                // forInspection == false means start repair (goes to PeepState::Answering)
                ride_call_mechanic(ride, mechanic, false);
            }
            break;
        }
        case RIDE_MECHANIC_STATUS_FIXING:
        {
            auto mechanic = ride_get_mechanic(ride);
            if (mechanic == nullptr
                || (mechanic->State != PeepState::HeadingToInspection && mechanic->State != PeepState::Fixing
                    && mechanic->State != PeepState::Inspecting && mechanic->State != PeepState::Answering))
            {
                ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
                ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
                ride_mechanic_status_update(ride, RIDE_MECHANIC_STATUS_CALLING);
            }
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006B796C
 */
static void ride_call_mechanic(Ride* ride, Peep* mechanic, int32_t forInspection)
{
    mechanic->SetState(forInspection ? PeepState::HeadingToInspection : PeepState::Answering);
    mechanic->SubState = 0;
    ride->mechanic_status = RIDE_MECHANIC_STATUS_HEADING;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
    ride->mechanic = mechanic->sprite_index;
    mechanic->CurrentRide = ride->id;
    mechanic->CurrentRideStation = ride->inspection_station;
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

Staff* ride_find_closest_mechanic(Ride* ride, int32_t forInspection)
{
    // Get either exit position or entrance position if there is no exit
    auto& station = ride->GetStation(ride->inspection_station);
    TileCoordsXYZD location = station.Exit;
    if (location.IsNull())
    {
        location = station.Entrance;
        if (station.Entrance.IsNull())
            return nullptr;
    }

    // Get station start track element and position
    auto mapLocation = location.ToCoordsXYZ();
    TileElement* tileElement = ride_get_station_exit_element(mapLocation);
    if (tileElement == nullptr)
        return nullptr;

    // Set x,y to centre of the station exit for the mechanic search.
    auto centreMapLocation = mapLocation.ToTileCentre();

    return find_closest_mechanic(centreMapLocation, forInspection);
}

/**
 *
 *  rct2: 0x006B774B (forInspection = 0)
 *  rct2: 0x006B78C3 (forInspection = 1)
 */
Staff* find_closest_mechanic(const CoordsXY& entrancePosition, int32_t forInspection)
{
    Staff* closestMechanic = nullptr;
    uint32_t closestDistance = std::numeric_limits<uint32_t>::max();

    for (auto peep : EntityList<Staff>())
    {
        if (!peep->IsMechanic())
            continue;

        if (!forInspection)
        {
            if (peep->State == PeepState::HeadingToInspection)
            {
                if (peep->SubState >= 4)
                    continue;
            }
            else if (peep->State != PeepState::Patrolling)
                continue;

            if (!(peep->StaffOrders & STAFF_ORDERS_FIX_RIDES))
                continue;
        }
        else
        {
            if (peep->State != PeepState::Patrolling || !(peep->StaffOrders & STAFF_ORDERS_INSPECT_RIDES))
                continue;
        }

        auto location = entrancePosition.ToTileStart();
        if (MapIsLocationInPark(location))
            if (!peep->IsLocationInPatrol(location))
                continue;

        if (peep->x == LOCATION_NULL)
            continue;

        // Manhattan distance
        uint32_t distance = std::abs(peep->x - entrancePosition.x) + std::abs(peep->y - entrancePosition.y);
        if (distance < closestDistance)
        {
            closestDistance = distance;
            closestMechanic = peep;
        }
    }

    return closestMechanic;
}

Staff* ride_get_mechanic(Ride* ride)
{
    auto staff = GetEntity<Staff>(ride->mechanic);
    if (staff != nullptr && staff->IsMechanic())
    {
        return staff;
    }
    return nullptr;
}

Staff* ride_get_assigned_mechanic(Ride* ride)
{
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        if (ride->mechanic_status == RIDE_MECHANIC_STATUS_HEADING || ride->mechanic_status == RIDE_MECHANIC_STATUS_FIXING
            || ride->mechanic_status == RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES)
        {
            return ride_get_mechanic(ride);
        }
    }

    return nullptr;
}

#pragma endregion

#pragma region Music functions

/**
 *
 *  Calculates the sample rate for ride music.
 */
static int32_t RideMusicSampleRate(Ride* ride)
{
    int32_t sampleRate = 22050;

    // Alter sample rate for a power cut effect
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN))
    {
        sampleRate = ride->breakdown_sound_modifier * 70;
        if (ride->breakdown_reason_pending != BREAKDOWN_CONTROL_FAILURE)
            sampleRate *= -1;
        sampleRate += 22050;
    }

    return sampleRate;
}

/**
 *
 *  Ride music slows down upon breaking. If it's completely broken, no music should play.
 */
static bool RideMusicBreakdownEffect(Ride* ride)
{
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
                ride->music_tune_id = TUNE_ID_NULL;
                return true;
            }
        }
    }
    return false;
}

/**
 *
 *  Circus music is a sound effect, rather than music. Needs separate processing.
 */
void CircusMusicUpdate(Ride* ride)
{
    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[0]);
    if (vehicle == nullptr || vehicle->status != Vehicle::Status::DoingCircusShow)
    {
        ride->music_position = 0;
        ride->music_tune_id = TUNE_ID_NULL;
        return;
    }

    if (RideMusicBreakdownEffect(ride))
    {
        return;
    }

    CoordsXYZ rideCoords = ride->GetStation().GetStart().ToTileCentre();

    const auto sampleRate = RideMusicSampleRate(ride);

    OpenRCT2::RideAudio::UpdateMusicInstance(*ride, rideCoords, sampleRate);
}

/**
 *
 *  rct2: 0x006ABE85
 */
void DefaultMusicUpdate(Ride* ride)
{
    if (ride->status != RideStatus::Open || !(ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC))
    {
        ride->music_tune_id = TUNE_ID_NULL;
        return;
    }

    if (RideMusicBreakdownEffect(ride))
    {
        return;
    }

    // Select random tune from available tunes for a music style (of course only merry-go-rounds have more than one tune)
    if (ride->music_tune_id == TUNE_ID_NULL)
    {
        auto& objManager = GetContext()->GetObjectManager();
        auto musicObj = static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, ride->music));
        if (musicObj != nullptr)
        {
            auto numTracks = musicObj->GetTrackCount();
            ride->music_tune_id = static_cast<uint8_t>(util_rand() % numTracks);
            ride->music_position = 0;
        }
        return;
    }

    CoordsXYZ rideCoords = ride->GetStation().GetStart().ToTileCentre();

    int32_t sampleRate = RideMusicSampleRate(ride);

    OpenRCT2::RideAudio::UpdateMusicInstance(*ride, rideCoords, sampleRate);
}

static void RideMusicUpdate(Ride* ride)
{
    const auto& rtd = ride->GetRideTypeDescriptor();

    if (!rtd.HasFlag(RIDE_TYPE_FLAG_MUSIC_ON_DEFAULT) && !rtd.HasFlag(RIDE_TYPE_FLAG_ALLOW_MUSIC))
        return;
    rtd.MusicUpdateFunction(ride);
}

#pragma endregion

#pragma region Measurement functions

/**
 *
 *  rct2: 0x006B64F2
 */
static void ride_measurement_update(Ride& ride, RideMeasurement& measurement)
{
    if (measurement.vehicle_index >= std::size(ride.vehicles))
        return;

    auto vehicle = GetEntity<Vehicle>(ride.vehicles[measurement.vehicle_index]);
    if (vehicle == nullptr)
        return;

    if (measurement.flags & RIDE_MEASUREMENT_FLAG_UNLOADING)
    {
        if (vehicle->status != Vehicle::Status::Departing && vehicle->status != Vehicle::Status::TravellingCableLift)
            return;

        measurement.flags &= ~RIDE_MEASUREMENT_FLAG_UNLOADING;
        if (measurement.current_station == vehicle->current_station)
            measurement.current_item = 0;
    }

    if (vehicle->status == Vehicle::Status::UnloadingPassengers)
    {
        measurement.flags |= RIDE_MEASUREMENT_FLAG_UNLOADING;
        return;
    }

    auto trackType = vehicle->GetTrackType();
    if (trackType == TrackElemType::BlockBrakes || trackType == TrackElemType::CableLiftHill
        || trackType == TrackElemType::Up25ToFlat || trackType == TrackElemType::Up60ToFlat
        || trackType == TrackElemType::DiagUp25ToFlat || trackType == TrackElemType::DiagUp60ToFlat)
        if (vehicle->velocity == 0)
            return;

    if (measurement.current_item >= RideMeasurement::MAX_ITEMS)
        return;

    if (measurement.flags & RIDE_MEASUREMENT_FLAG_G_FORCES)
    {
        auto gForces = vehicle->GetGForces();
        gForces.VerticalG = std::clamp(gForces.VerticalG / 8, -127, 127);
        gForces.LateralG = std::clamp(gForces.LateralG / 8, -127, 127);

        if (gCurrentTicks & 1)
        {
            gForces.VerticalG = (gForces.VerticalG + measurement.vertical[measurement.current_item]) / 2;
            gForces.LateralG = (gForces.LateralG + measurement.lateral[measurement.current_item]) / 2;
        }

        measurement.vertical[measurement.current_item] = gForces.VerticalG & 0xFF;
        measurement.lateral[measurement.current_item] = gForces.LateralG & 0xFF;
    }

    auto velocity = std::min(std::abs((vehicle->velocity * 5) >> 16), 255);
    auto altitude = std::min(vehicle->z / 8, 255);

    if (gCurrentTicks & 1)
    {
        velocity = (velocity + measurement.velocity[measurement.current_item]) / 2;
        altitude = (altitude + measurement.altitude[measurement.current_item]) / 2;
    }

    measurement.velocity[measurement.current_item] = velocity & 0xFF;
    measurement.altitude[measurement.current_item] = altitude & 0xFF;

    if (gCurrentTicks & 1)
    {
        measurement.current_item++;
        measurement.num_items = std::max(measurement.num_items, measurement.current_item);
    }
}

/**
 *
 *  rct2: 0x006B6456
 */
void ride_measurements_update()
{
    PROFILED_FUNCTION();

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return;

    // For each ride measurement
    for (auto& ride : GetRideManager())
    {
        auto measurement = ride.measurement.get();
        if (measurement != nullptr && (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) && ride.status != RideStatus::Simulating)
        {
            if (measurement->flags & RIDE_MEASUREMENT_FLAG_RUNNING)
            {
                ride_measurement_update(ride, *measurement);
            }
            else
            {
                // For each vehicle
                for (int32_t j = 0; j < ride.NumTrains; j++)
                {
                    auto vehicleSpriteIdx = ride.vehicles[j];
                    auto vehicle = GetEntity<Vehicle>(vehicleSpriteIdx);
                    if (vehicle != nullptr)
                    {
                        if (vehicle->status == Vehicle::Status::Departing
                            || vehicle->status == Vehicle::Status::TravellingCableLift)
                        {
                            measurement->vehicle_index = j;
                            measurement->current_station = vehicle->current_station;
                            measurement->flags |= RIDE_MEASUREMENT_FLAG_RUNNING;
                            measurement->flags &= ~RIDE_MEASUREMENT_FLAG_UNLOADING;
                            ride_measurement_update(ride, *measurement);
                            break;
                        }
                    }
                }
            }
        }
    }
}

/**
 * If there are more than the threshold of allowed ride measurements, free the non-LRU one.
 */
static void ride_free_old_measurements()
{
    size_t numRideMeasurements;
    do
    {
        Ride* lruRide{};
        numRideMeasurements = 0;
        for (auto& ride : GetRideManager())
        {
            if (ride.measurement != nullptr)
            {
                if (lruRide == nullptr || ride.measurement->last_use_tick > lruRide->measurement->last_use_tick)
                {
                    lruRide = &ride;
                }
                numRideMeasurements++;
            }
        }
        if (numRideMeasurements > MAX_RIDE_MEASUREMENTS && lruRide != nullptr)
        {
            lruRide->measurement = {};
            numRideMeasurements--;
        }
    } while (numRideMeasurements > MAX_RIDE_MEASUREMENTS);
}

std::pair<RideMeasurement*, OpenRCT2String> Ride::GetMeasurement()
{
    const auto& rtd = GetRideTypeDescriptor();

    // Check if ride type supports data logging
    if (!rtd.HasFlag(RIDE_TYPE_FLAG_HAS_DATA_LOGGING))
    {
        return { nullptr, { STR_DATA_LOGGING_NOT_AVAILABLE_FOR_THIS_TYPE_OF_RIDE, {} } };
    }

    // Check if a measurement already exists for this ride
    if (measurement == nullptr)
    {
        measurement = std::make_unique<RideMeasurement>();
        if (rtd.HasFlag(RIDE_TYPE_FLAG_HAS_G_FORCES))
        {
            measurement->flags |= RIDE_MEASUREMENT_FLAG_G_FORCES;
        }
        ride_free_old_measurements();
        assert(measurement != nullptr);
    }

    measurement->last_use_tick = gCurrentTicks;
    if (measurement->flags & 1)
    {
        return { measurement.get(), { STR_EMPTY, {} } };
    }

    auto ft = Formatter();
    ft.Add<StringId>(GetRideComponentName(GetRideTypeDescriptor().NameConvention.vehicle).singular);
    ft.Add<StringId>(GetRideComponentName(GetRideTypeDescriptor().NameConvention.station).singular);
    return { nullptr, { STR_DATA_LOGGING_WILL_START_WHEN_NEXT_LEAVES, ft } };
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

VehicleColour ride_get_vehicle_colour(Ride* ride, int32_t vehicleIndex)
{
    // Prevent indexing array out of bounds
    vehicleIndex = std::min<int32_t>(vehicleIndex, OpenRCT2::Limits::MaxCarsPerTrain);
    return ride->vehicle_colours[vehicleIndex];
}

static bool RideTypeVehicleColourExists(ObjectEntryIndex subType, const VehicleColour& vehicleColour)
{
    for (auto& ride : GetRideManager())
    {
        if (ride.subtype != subType)
            continue;
        if (ride.vehicle_colours[0].Body != vehicleColour.Body)
            continue;
        return true;
    }
    return false;
}

int32_t ride_get_unused_preset_vehicle_colour(ObjectEntryIndex subType)
{
    const auto* rideEntry = get_ride_entry(subType);
    if (rideEntry == nullptr)
        return 0;

    const auto* colourPresets = rideEntry->vehicle_preset_list;
    if (colourPresets == nullptr || colourPresets->count == 0)
        return 0;
    if (colourPresets->count == 255)
        return 255;

    // Find all the presets that haven't yet been used in the park for this ride type
    std::vector<uint8_t> unused;
    unused.reserve(colourPresets->count);
    for (uint8_t i = 0; i < colourPresets->count; i++)
    {
        const auto& preset = colourPresets->list[i];
        if (!RideTypeVehicleColourExists(subType, preset))
        {
            unused.push_back(i);
        }
    }

    // If all presets have been used, just go with a random preset
    if (unused.size() == 0)
        return util_rand() % colourPresets->count;

    // Choose a random preset from the list of unused presets
    auto unusedIndex = util_rand() % unused.size();
    return unused[unusedIndex];
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

        ride->colour_scheme_type = RIDE_COLOUR_SCHEME_MODE_ALL_SAME;
        VehicleColour* preset = &presetList->list[preset_index];
        ride->vehicle_colours[0] = *preset;
    }
    else
    {
        ride->colour_scheme_type = RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_TRAIN;
        uint32_t count = presetList->count;
        for (uint32_t i = 0; i < count; i++)
        {
            auto index = i % static_cast<uint8_t>(32);
            VehicleColour* preset = &presetList->list[index];
            ride->vehicle_colours[i] = *preset;
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
    for (auto& ride : GetRideManager())
    {
        if (ride.connected_message_throttle != 0)
            ride.connected_message_throttle--;
        if (ride.status != RideStatus::Open || ride.connected_message_throttle != 0)
            continue;

        if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
            ride_shop_connected(&ride);
        else
            ride_entrance_exit_connected(&ride);
    }
}

/**
 *
 *  rct2: 0x006B7C59
 * @return true if the coordinate is reachable or has no entrance, false otherwise
 */
static bool ride_entrance_exit_is_reachable(const TileCoordsXYZD& coordinates)
{
    if (coordinates.IsNull())
        return true;

    TileCoordsXYZ loc{ coordinates.x, coordinates.y, coordinates.z };
    loc -= TileDirectionDelta[coordinates.direction];

    return MapCoordIsConnected(loc, coordinates.direction);
}

static void ride_entrance_exit_connected(Ride* ride)
{
    for (auto& station : ride->GetStations())
    {
        auto station_start = station.Start;
        auto entrance = station.Entrance;
        auto exit = station.Exit;

        if (station_start.IsNull())
            continue;
        if (!entrance.IsNull() && !ride_entrance_exit_is_reachable(entrance))
        {
            // name of ride is parameter of the format string
            Formatter ft;
            ride->FormatNameTo(ft);
            if (gConfigNotifications.RideWarnings)
            {
                News::AddItemToQueue(News::ItemType::Ride, STR_ENTRANCE_NOT_CONNECTED, ride->id.ToUnderlying(), ft);
            }
            ride->connected_message_throttle = 3;
        }

        if (!exit.IsNull() && !ride_entrance_exit_is_reachable(exit))
        {
            // name of ride is parameter of the format string
            Formatter ft;
            ride->FormatNameTo(ft);
            if (gConfigNotifications.RideWarnings)
            {
                News::AddItemToQueue(News::ItemType::Ride, STR_EXIT_NOT_CONNECTED, ride->id.ToUnderlying(), ft);
            }
            ride->connected_message_throttle = 3;
        }
    }
}

static void ride_shop_connected(Ride* ride)
{
    auto shopLoc = TileCoordsXY(ride->GetStation().Start);
    if (shopLoc.IsNull())
        return;

    TrackElement* trackElement = nullptr;
    TileElement* tileElement = MapGetFirstElementAt(shopLoc);
    do
    {
        if (tileElement == nullptr)
            break;
        if (tileElement->GetType() == TileElementType::Track && tileElement->AsTrack()->GetRideIndex() == ride->id)
        {
            trackElement = tileElement->AsTrack();
            break;
        }
    } while (!(tileElement++)->IsLastForTile());

    if (trackElement == nullptr)
        return;

    auto track_type = trackElement->GetTrackType();
    ride = get_ride(trackElement->GetRideIndex());
    if (ride == nullptr)
    {
        return;
    }

    const auto& ted = GetTrackElementDescriptor(track_type);
    uint8_t entrance_directions = std::get<0>(ted.SequenceProperties) & 0xF;
    uint8_t tile_direction = trackElement->GetDirection();
    entrance_directions = Numerics::rol4(entrance_directions, tile_direction);

    // Now each bit in entrance_directions stands for an entrance direction to check
    if (entrance_directions == 0)
        return;

    for (auto count = 0; entrance_directions != 0; count++)
    {
        if (!(entrance_directions & 1))
        {
            entrance_directions >>= 1;
            continue;
        }
        entrance_directions >>= 1;

        // Flip direction north<->south, east<->west
        uint8_t face_direction = DirectionReverse(count);

        int32_t y2 = shopLoc.y - TileDirectionDelta[face_direction].y;
        int32_t x2 = shopLoc.x - TileDirectionDelta[face_direction].x;

        if (MapCoordIsConnected({ x2, y2, tileElement->base_height }, face_direction))
            return;
    }

    // Name of ride is parameter of the format string
    if (gConfigNotifications.RideWarnings)
    {
        Formatter ft;
        ride->FormatNameTo(ft);
        News::AddItemToQueue(News::ItemType::Ride, STR_ENTRANCE_NOT_CONNECTED, ride->id.ToUnderlying(), ft);
    }

    ride->connected_message_throttle = 3;
}

#pragma endregion

#pragma region Interface

static void ride_track_set_map_tooltip(TileElement* tileElement)
{
    auto rideIndex = tileElement->AsTrack()->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        auto ft = Formatter();
        ft.Add<StringId>(STR_RIDE_MAP_TIP);
        ride->FormatNameTo(ft);
        ride->FormatStatusTo(ft);
        auto intent = Intent(INTENT_ACTION_SET_MAP_TOOLTIP);
        intent.putExtra(INTENT_EXTRA_FORMATTER, &ft);
        context_broadcast_intent(&intent);
    }
}

static void ride_queue_banner_set_map_tooltip(TileElement* tileElement)
{
    auto rideIndex = tileElement->AsPath()->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        auto ft = Formatter();
        ft.Add<StringId>(STR_RIDE_MAP_TIP);
        ride->FormatNameTo(ft);
        ride->FormatStatusTo(ft);
        auto intent = Intent(INTENT_ACTION_SET_MAP_TOOLTIP);
        intent.putExtra(INTENT_EXTRA_FORMATTER, &ft);
        context_broadcast_intent(&intent);
    }
}

static void ride_station_set_map_tooltip(TileElement* tileElement)
{
    auto rideIndex = tileElement->AsTrack()->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        auto stationIndex = tileElement->AsTrack()->GetStationIndex();
        for (int32_t i = stationIndex.ToUnderlying(); i >= 0; i--)
            if (ride->GetStations()[i].Start.IsNull())
                stationIndex = StationIndex::FromUnderlying(stationIndex.ToUnderlying() - 1);

        auto ft = Formatter();
        ft.Add<StringId>(STR_RIDE_MAP_TIP);
        ft.Add<StringId>(ride->num_stations <= 1 ? STR_RIDE_STATION : STR_RIDE_STATION_X);
        ride->FormatNameTo(ft);
        ft.Add<StringId>(GetRideComponentName(ride->GetRideTypeDescriptor().NameConvention.station).capitalised);
        ft.Add<uint16_t>(stationIndex.ToUnderlying() + 1);
        ride->FormatStatusTo(ft);
        auto intent = Intent(INTENT_ACTION_SET_MAP_TOOLTIP);
        intent.putExtra(INTENT_EXTRA_FORMATTER, &ft);
        context_broadcast_intent(&intent);
    }
}

static void ride_entrance_set_map_tooltip(TileElement* tileElement)
{
    auto rideIndex = tileElement->AsEntrance()->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        // Get the station
        auto stationIndex = tileElement->AsEntrance()->GetStationIndex();
        for (int32_t i = stationIndex.ToUnderlying(); i >= 0; i--)
            if (ride->GetStations()[i].Start.IsNull())
                stationIndex = StationIndex::FromUnderlying(stationIndex.ToUnderlying() - 1);

        if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
        {
            // Get the queue length
            int32_t queueLength = 0;
            if (!ride->GetStation(stationIndex).Entrance.IsNull())
                queueLength = ride->GetStation(stationIndex).QueueLength;

            auto ft = Formatter();
            ft.Add<StringId>(STR_RIDE_MAP_TIP);
            ft.Add<StringId>(ride->num_stations <= 1 ? STR_RIDE_ENTRANCE : STR_RIDE_STATION_X_ENTRANCE);
            ride->FormatNameTo(ft);

            // String IDs have an extra pop16 for some reason
            ft.Increment(sizeof(uint16_t));

            ft.Add<uint16_t>(stationIndex.ToUnderlying() + 1);
            if (queueLength == 0)
            {
                ft.Add<StringId>(STR_QUEUE_EMPTY);
            }
            else if (queueLength == 1)
            {
                ft.Add<StringId>(STR_QUEUE_ONE_PERSON);
            }
            else
            {
                ft.Add<StringId>(STR_QUEUE_PEOPLE);
            }
            ft.Add<uint16_t>(queueLength);
            auto intent = Intent(INTENT_ACTION_SET_MAP_TOOLTIP);
            intent.putExtra(INTENT_EXTRA_FORMATTER, &ft);
            context_broadcast_intent(&intent);
        }
        else
        {
            // Get the station
            stationIndex = tileElement->AsEntrance()->GetStationIndex();
            for (int32_t i = stationIndex.ToUnderlying(); i >= 0; i--)
                if (ride->GetStations()[i].Start.IsNull())
                    stationIndex = StationIndex::FromUnderlying(stationIndex.ToUnderlying() - 1);

            auto ft = Formatter();
            ft.Add<StringId>(ride->num_stations <= 1 ? STR_RIDE_EXIT : STR_RIDE_STATION_X_EXIT);
            ride->FormatNameTo(ft);

            // String IDs have an extra pop16 for some reason
            ft.Increment(sizeof(uint16_t));

            ft.Add<uint16_t>(stationIndex.ToUnderlying() + 1);
            auto intent = Intent(INTENT_ACTION_SET_MAP_TOOLTIP);
            intent.putExtra(INTENT_EXTRA_FORMATTER, &ft);
            context_broadcast_intent(&intent);
        }
    }
}

void ride_set_map_tooltip(TileElement* tileElement)
{
    if (tileElement->GetType() == TileElementType::Entrance)
    {
        ride_entrance_set_map_tooltip(tileElement);
    }
    else if (tileElement->GetType() == TileElementType::Track)
    {
        if (tileElement->AsTrack()->IsStation())
        {
            ride_station_set_map_tooltip(tileElement);
        }
        else
        {
            ride_track_set_map_tooltip(tileElement);
        }
    }
    else if (tileElement->GetType() == TileElementType::Path)
    {
        ride_queue_banner_set_map_tooltip(tileElement);
    }
}

#pragma endregion

/**
 *
 *  rct2: 0x006B4CC1
 */
static ResultWithMessage ride_mode_check_valid_station_numbers(Ride* ride)
{
    uint16_t numStations = 0;
    for (const auto& station : ride->GetStations())
    {
        if (!station.Start.IsNull())
        {
            numStations++;
        }
    }

    switch (ride->mode)
    {
        case RideMode::ReverseInclineLaunchedShuttle:
        case RideMode::PoweredLaunchPasstrough:
        case RideMode::PoweredLaunch:
        case RideMode::LimPoweredLaunch:
            if (numStations <= 1)
                return { true };
            return { false, STR_UNABLE_TO_OPERATE_WITH_MORE_THAN_ONE_STATION_IN_THIS_MODE };
        case RideMode::Shuttle:
            if (numStations >= 2)
                return { true };
            return { false, STR_UNABLE_TO_OPERATE_WITH_LESS_THAN_TWO_STATIONS_IN_THIS_MODE };
        default:
        {
            // This is workaround for multiple compilation errors of type "enumeration value ‘RIDE_MODE_*' not handled
            // in switch [-Werror=switch]"
        }
    }

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (rtd.HasFlag(RIDE_TYPE_FLAG_HAS_ONE_STATION) && numStations > 1)
        return { false, STR_UNABLE_TO_OPERATE_WITH_MORE_THAN_ONE_STATION_IN_THIS_MODE };

    return { true };
}

/**
 * returns stationIndex of first station on success
 * STATION_INDEX_NULL on failure.
 */
static StationIndexWithMessage ride_mode_check_station_present(Ride* ride)
{
    auto stationIndex = ride_get_first_valid_station_start(ride);

    if (stationIndex.IsNull())
    {
        if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_NO_TRACK))
            return { StationIndex::GetNull(), STR_NOT_YET_CONSTRUCTED };

        if (ride->type == RIDE_TYPE_MAZE)
            return { StationIndex::GetNull(), STR_NOT_YET_CONSTRUCTED };

        return { StationIndex::GetNull(), STR_REQUIRES_A_STATION_PLATFORM };
    }

    return { stationIndex };
}

/**
 *
 *  rct2: 0x006B5872
 */
static ResultWithMessage ride_check_for_entrance_exit(RideId rideIndex)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return { false };

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
        return { true };

    uint8_t entrance = 0;
    uint8_t exit = 0;
    for (const auto& station : ride->GetStations())
    {
        if (station.Start.IsNull())
            continue;

        if (!station.Entrance.IsNull())
        {
            entrance = 1;
        }

        if (!station.Exit.IsNull())
        {
            exit = 1;
        }

        // If station start and no entrance/exit
        // Sets same error message as no entrance
        if (station.Exit.IsNull() && station.Entrance.IsNull())
        {
            entrance = 0;
            break;
        }
    }

    if (entrance == 0)
    {
        return { false, STR_ENTRANCE_NOT_YET_BUILT };
    }

    if (exit == 0)
    {
        return { false, STR_EXIT_NOT_YET_BUILT };
    }

    return { true };
}

/**
 * Calls FootpathChainRideQueue for all entrances of the ride
 *  rct2: 0x006B5952
 */
void Ride::ChainQueues() const
{
    for (const auto& station : stations)
    {
        if (station.Entrance.IsNull())
            continue;

        auto mapLocation = station.Entrance.ToCoordsXYZ();

        // This will fire for every entrance on this x, y and z, regardless whether that actually belongs to
        // the ride or not.
        TileElement* tileElement = MapGetFirstElementAt(station.Entrance);
        if (tileElement != nullptr)
        {
            do
            {
                if (tileElement->GetType() != TileElementType::Entrance)
                    continue;
                if (tileElement->GetBaseZ() != mapLocation.z)
                    continue;

                int32_t direction = tileElement->GetDirection();
                FootpathChainRideQueue(id, GetStationIndex(&station), mapLocation, tileElement, DirectionReverse(direction));
            } while (!(tileElement++)->IsLastForTile());
        }
    }
}

/**
 *
 *  rct2: 0x006D3319
 */
static ResultWithMessage RideCheckBlockBrakes(const CoordsXYE& input, CoordsXYE* output)
{
    if (input.element == nullptr || input.element->GetType() != TileElementType::Track)
        return { false };

    RideId rideIndex = input.element->AsTrack()->GetRideIndex();
    rct_window* w = window_find_by_class(WindowClass::RideConstruction);
    if (w != nullptr && _rideConstructionState != RideConstructionState::State0 && _currentRideIndex == rideIndex)
        ride_construction_invalidate_current_track();

    track_circuit_iterator it;
    track_circuit_iterator_begin(&it, input);
    while (track_circuit_iterator_next(&it))
    {
        if (it.current.element->AsTrack()->GetTrackType() == TrackElemType::BlockBrakes)
        {
            auto type = it.last.element->AsTrack()->GetTrackType();
            if (type == TrackElemType::EndStation)
            {
                *output = it.current;
                return { false, STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_STATION };
            }
            if (type == TrackElemType::BlockBrakes)
            {
                *output = it.current;
                return { false, STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_EACH_OTHER };
            }
            if (it.last.element->AsTrack()->HasChain() && type != TrackElemType::LeftCurvedLiftHill
                && type != TrackElemType::RightCurvedLiftHill)
            {
                *output = it.current;
                return { false, STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_THE_TOP_OF_THIS_LIFT_HILL };
            }
        }
    }
    if (!it.looped)
    {
        // Not sure why this is the case...
        *output = it.last;
        return { false, STR_BLOCK_BRAKES_CANNOT_BE_USED_DIRECTLY_AFTER_STATION };
    }

    return { true };
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
    if (input->element == nullptr)
        return false;

    const auto* trackElement = input->element->AsTrack();
    if (trackElement == nullptr)
        return false;

    RideId rideIndex = trackElement->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride != nullptr && ride->type == RIDE_TYPE_MAZE)
        return true;

    rct_window* w = window_find_by_class(WindowClass::RideConstruction);
    if (w != nullptr && _rideConstructionState != RideConstructionState::State0 && rideIndex == _currentRideIndex)
    {
        ride_construction_invalidate_current_track();
    }

    bool moveSlowIt = true;
    track_circuit_iterator it, slowIt;
    track_circuit_iterator_begin(&it, *input);
    slowIt = it;

    while (track_circuit_iterator_next(&it))
    {
        auto trackType = it.current.element->AsTrack()->GetTrackType();
        const auto& ted = GetTrackElementDescriptor(trackType);
        if (ted.Flags & TRACK_ELEM_FLAG_INVERSION_TO_NORMAL)
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
    if (input->element == nullptr)
        return false;

    const auto* trackElement = input->element->AsTrack();
    if (trackElement == nullptr)
        return false;

    auto rideIndex = trackElement->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return false;

    if (ride->type == RIDE_TYPE_MAZE)
        return true;

    rct_window* w = window_find_by_class(WindowClass::RideConstruction);
    if (w != nullptr && _rideConstructionState != RideConstructionState::State0 && rideIndex == _currentRideIndex)
    {
        ride_construction_invalidate_current_track();
    }

    bool moveSlowIt = true;
    track_circuit_iterator it, slowIt;
    track_circuit_iterator_begin(&it, *input);
    slowIt = it;

    while (track_circuit_iterator_next(&it))
    {
        auto trackType = output->element->AsTrack()->GetTrackType();
        const auto& ted = GetTrackElementDescriptor(trackType);
        if (ted.Flags & TRACK_ELEM_FLAG_BANKED)
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
    rct_window* w = window_find_by_class(WindowClass::RideConstruction);
    if (w != nullptr && _rideConstructionState != RideConstructionState::State0
        && _currentRideIndex == input->element->AsTrack()->GetRideIndex())
    {
        ride_construction_invalidate_current_track();
    }

    output->x = input->x;
    output->y = input->y;
    output->element = input->element;
    track_begin_end trackBeginEnd;
    while (track_block_get_previous(*output, &trackBeginEnd))
    {
        output->x = trackBeginEnd.begin_x;
        output->y = trackBeginEnd.begin_y;
        output->element = trackBeginEnd.begin_element;
    }

    int32_t num_station_elements = 0;
    CoordsXYE last_good_station = *output;

    do
    {
        const auto& ted = GetTrackElementDescriptor(output->element->AsTrack()->GetTrackType());
        if (std::get<0>(ted.SequenceProperties) & TRACK_SEQUENCE_FLAG_ORIGIN)
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
    CoordsXYE trackBack, trackFront;

    RideId rideIndex = input->element->AsTrack()->GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return false;

    auto w = window_find_by_class(WindowClass::RideConstruction);
    if (w != nullptr && _rideConstructionState != RideConstructionState::State0 && rideIndex == _currentRideIndex)
    {
        ride_construction_invalidate_current_track();
    }

    // Check back of the track
    track_get_back(input, &trackBack);
    auto trackType = trackBack.element->AsTrack()->GetTrackType();
    const auto* ted = &GetTrackElementDescriptor(trackType);
    if (!(std::get<0>(ted->SequenceProperties) & TRACK_SEQUENCE_FLAG_ORIGIN))
    {
        return false;
    }
    ride->ChairliftBullwheelLocation[0] = TileCoordsXYZ{ CoordsXYZ{ trackBack.x, trackBack.y, trackBack.element->GetBaseZ() } };

    // Check front of the track
    track_get_front(input, &trackFront);
    trackType = trackFront.element->AsTrack()->GetTrackType();
    ted = &GetTrackElementDescriptor(trackType);
    if (!(std::get<0>(ted->SequenceProperties) & TRACK_SEQUENCE_FLAG_ORIGIN))
    {
        return false;
    }
    ride->ChairliftBullwheelLocation[1] = TileCoordsXYZ{ CoordsXYZ{ trackFront.x, trackFront.y,
                                                                    trackFront.element->GetBaseZ() } };
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
    auto returnPos = *startElement;
    int32_t startX = returnPos.x;
    int32_t startY = returnPos.y;
    track_begin_end trackBeginEnd;
    while (track_block_get_previous(returnPos, &trackBeginEnd))
    {
        // If previous track is back to the starting x, y, then break loop (otherwise possible infinite loop)
        if (trackType != -1 && startX == trackBeginEnd.begin_x && startY == trackBeginEnd.begin_y)
            break;

        auto trackCoords = CoordsXYZ{ trackBeginEnd.begin_x, trackBeginEnd.begin_y, trackBeginEnd.begin_z };
        int32_t direction = trackBeginEnd.begin_direction;
        trackType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
        auto newCoords = GetTrackElementOriginAndApplyChanges(
            { trackCoords, static_cast<Direction>(direction) }, trackType, 0, &returnPos.element, 0);
        returnPos = newCoords.has_value() ? CoordsXYE{ newCoords.value(), returnPos.element }
                                          : CoordsXYE{ trackCoords, returnPos.element };
    };

    trackType = returnPos.element->AsTrack()->GetTrackType();
    const auto& ted = GetTrackElementDescriptor(trackType);
    int32_t elementReturnDirection = ted.Coordinates.rotation_begin;
    ride->boat_hire_return_direction = returnPos.element->GetDirectionWithOffset(elementReturnDirection);
    ride->boat_hire_return_position = TileCoordsXY{ returnPos };
}

/**
 *
 *  rct2: 0x006B4D39
 */
static void ride_set_maze_entrance_exit_points(Ride* ride)
{
    // Needs room for an entrance and an exit per station, plus one position for the list terminator.
    TileCoordsXYZD positions[(OpenRCT2::Limits::MaxStationsPerRide * 2) + 1];

    // Create a list of all the entrance and exit positions
    TileCoordsXYZD* position = positions;
    for (const auto& station : ride->GetStations())
    {
        if (!station.Entrance.IsNull())
        {
            *position++ = station.Entrance;
        }
        if (!station.Exit.IsNull())
        {
            *position++ = station.Exit;
        }
    }
    (*position++).SetNull();

    // Enumerate entrance and exit positions
    for (position = positions; !(*position).IsNull(); position++)
    {
        auto entranceExitMapPos = position->ToCoordsXYZ();

        TileElement* tileElement = MapGetFirstElementAt(*position);
        do
        {
            if (tileElement == nullptr)
                break;
            if (tileElement->GetType() != TileElementType::Entrance)
                continue;
            if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE
                && tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_EXIT)
            {
                continue;
            }
            if (tileElement->GetBaseZ() != entranceExitMapPos.z)
                continue;

            MazeEntranceHedgeRemoval({ entranceExitMapPos, tileElement });
        } while (!(tileElement++)->IsLastForTile());
    }
}

/**
 * Opens all block brakes of a ride.
 *  rct2: 0x006B4E6B
 */
static void RideOpenBlockBrakes(CoordsXYE* startElement)
{
    CoordsXYE currentElement = *startElement;
    do
    {
        auto trackType = currentElement.element->AsTrack()->GetTrackType();
        switch (trackType)
        {
            case TrackElemType::EndStation:
            case TrackElemType::CableLiftHill:
            case TrackElemType::Up25ToFlat:
            case TrackElemType::Up60ToFlat:
            case TrackElemType::DiagUp25ToFlat:
            case TrackElemType::DiagUp60ToFlat:
            case TrackElemType::BlockBrakes:
                currentElement.element->AsTrack()->SetBlockBrakeClosed(false);
                break;
        }
    } while (track_block_get_next(&currentElement, &currentElement, nullptr, nullptr)
             && currentElement.element != startElement->element);
}

/**
 *
 *  rct2: 0x006B4D26
 */
static void ride_set_start_finish_points(RideId rideIndex, CoordsXYE* startElement)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return;

    switch (ride->type)
    {
        case RIDE_TYPE_BOAT_HIRE:
            ride_set_boat_hire_return_point(ride, startElement);
            break;
        case RIDE_TYPE_MAZE:
            ride_set_maze_entrance_exit_points(ride);
            break;
    }

    if (ride->IsBlockSectioned() && !(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
    {
        RideOpenBlockBrakes(startElement);
    }
}

/**
 *
 *  rct2: 0x0069ED9E
 */
static int32_t count_free_misc_sprite_slots()
{
    int32_t miscSpriteCount = GetMiscEntityCount();
    int32_t remainingSpriteCount = GetNumFreeEntities();
    return std::max(0, miscSpriteCount + remainingSpriteCount - 300);
}

static constexpr const CoordsXY word_9A3AB4[4] = {
    { 0, 0 },
    { 0, -96 },
    { -96, -96 },
    { -96, 0 },
};

// clang-format off
static constexpr const CoordsXY word_9A2A60[] = {
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
static Vehicle* vehicle_create_car(
    RideId rideIndex, int32_t carEntryIndex, int32_t carIndex, int32_t vehicleIndex, const CoordsXYZ& carPosition,
    int32_t* remainingDistance, TrackElement* trackElement)
{
    if (trackElement == nullptr)
        return nullptr;

    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return nullptr;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return nullptr;

    auto carEntry = &rideEntry->Cars[carEntryIndex];
    auto vehicle = CreateEntity<Vehicle>();
    if (vehicle == nullptr)
        return nullptr;

    vehicle->ride = rideIndex;
    vehicle->ride_subtype = ride->subtype;

    vehicle->vehicle_type = carEntryIndex;
    vehicle->SubType = carIndex == 0 ? Vehicle::Type::Head : Vehicle::Type::Tail;
    vehicle->var_44 = Numerics::ror32(carEntry->spacing, 10) & 0xFFFF;
    auto edx = carEntry->spacing >> 1;
    *remainingDistance -= edx;
    vehicle->remaining_distance = *remainingDistance;
    if (!(carEntry->flags & CAR_ENTRY_FLAG_GO_KART))
    {
        *remainingDistance -= edx;
    }

    // loc_6DD9A5:
    vehicle->sprite_width = carEntry->sprite_width;
    vehicle->sprite_height_negative = carEntry->sprite_height_negative;
    vehicle->sprite_height_positive = carEntry->sprite_height_positive;
    vehicle->mass = carEntry->car_mass;
    vehicle->num_seats = carEntry->num_seats;
    vehicle->speed = carEntry->powered_max_speed;
    vehicle->powered_acceleration = carEntry->powered_acceleration;
    vehicle->velocity = 0;
    vehicle->acceleration = 0;
    vehicle->SwingSprite = 0;
    vehicle->SwingPosition = 0;
    vehicle->SwingSpeed = 0;
    vehicle->restraints_position = 0;
    vehicle->spin_sprite = 0;
    vehicle->spin_speed = 0;
    vehicle->sound2_flags = 0;
    vehicle->sound1_id = OpenRCT2::Audio::SoundId::Null;
    vehicle->sound2_id = OpenRCT2::Audio::SoundId::Null;
    vehicle->next_vehicle_on_train = EntityId::GetNull();
    vehicle->CollisionDetectionTimer = 0;
    vehicle->animation_frame = 0;
    vehicle->animationState = 0;
    vehicle->scream_sound_id = OpenRCT2::Audio::SoundId::Null;
    vehicle->Pitch = 0;
    vehicle->bank_rotation = 0;
    vehicle->target_seat_rotation = 4;
    vehicle->seat_rotation = 4;
    for (size_t i = 0; i < std::size(vehicle->peep); i++)
    {
        vehicle->peep[i] = EntityId::GetNull();
    }

    if (carEntry->flags & CAR_ENTRY_FLAG_DODGEM_CAR_PLACEMENT)
    {
        // loc_6DDCA4:
        vehicle->TrackSubposition = VehicleTrackSubposition::Default;
        int32_t direction = trackElement->GetDirection();
        auto dodgemPos = carPosition + CoordsXYZ{ word_9A3AB4[direction], 0 };
        vehicle->TrackLocation = dodgemPos;
        vehicle->current_station = trackElement->GetStationIndex();

        dodgemPos.z += ride->GetRideTypeDescriptor().Heights.VehicleZOffset;

        vehicle->SetTrackDirection(0);
        vehicle->SetTrackType(trackElement->GetTrackType());
        vehicle->track_progress = 0;
        vehicle->SetState(Vehicle::Status::MovingToEndOfStation);
        vehicle->update_flags = 0;

        CoordsXY chosenLoc;
        auto numAttempts = 0;
        // loc_6DDD26:
        do
        {
            numAttempts++;
            // This can happen when trying to spawn dozens of cars in a tiny area.
            if (numAttempts > 10000)
                return nullptr;

            vehicle->sprite_direction = scenario_rand() & 0x1E;
            chosenLoc.y = dodgemPos.y + (scenario_rand() & 0xFF);
            chosenLoc.x = dodgemPos.x + (scenario_rand() & 0xFF);
        } while (vehicle->DodgemsCarWouldCollideAt(chosenLoc).has_value());

        vehicle->MoveTo({ chosenLoc, dodgemPos.z });
    }
    else
    {
        VehicleTrackSubposition subposition = VehicleTrackSubposition::Default;
        if (carEntry->flags & CAR_ENTRY_FLAG_CHAIRLIFT)
        {
            subposition = VehicleTrackSubposition::ChairliftGoingOut;
        }

        if (carEntry->flags & CAR_ENTRY_FLAG_GO_KART)
        {
            // Choose which lane Go Kart should start in
            subposition = VehicleTrackSubposition::GoKartsLeftLane;
            if (vehicleIndex & 1)
            {
                subposition = VehicleTrackSubposition::GoKartsRightLane;
            }
        }
        if (carEntry->flags & CAR_ENTRY_FLAG_MINI_GOLF)
        {
            subposition = VehicleTrackSubposition::MiniGolfStart9;
            vehicle->var_D3 = 0;
            vehicle->mini_golf_current_animation = MiniGolfAnimation::Walk;
            vehicle->mini_golf_flags = 0;
        }
        if (carEntry->flags & CAR_ENTRY_FLAG_REVERSER_BOGIE)
        {
            if (vehicle->IsHead())
            {
                subposition = VehicleTrackSubposition::ReverserRCFrontBogie;
            }
        }
        if (carEntry->flags & CAR_ENTRY_FLAG_REVERSER_PASSENGER_CAR)
        {
            subposition = VehicleTrackSubposition::ReverserRCRearBogie;
        }
        vehicle->TrackSubposition = subposition;

        auto chosenLoc = carPosition;
        vehicle->TrackLocation = chosenLoc;

        int32_t direction = trackElement->GetDirection();
        vehicle->sprite_direction = direction << 3;

        if (ride->type == RIDE_TYPE_SPACE_RINGS)
        {
            direction = 4;
        }
        else
        {
            if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL))
            {
                if (ride->GetRideTypeDescriptor().StartTrackPiece != TrackElemType::FlatTrack1x4B)
                {
                    if (ride->GetRideTypeDescriptor().StartTrackPiece != TrackElemType::FlatTrack1x4A)
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

        chosenLoc += CoordsXYZ{ word_9A2A60[direction], ride->GetRideTypeDescriptor().Heights.VehicleZOffset };

        vehicle->current_station = trackElement->GetStationIndex();

        vehicle->MoveTo(chosenLoc);
        vehicle->SetTrackType(trackElement->GetTrackType());
        vehicle->SetTrackDirection(vehicle->sprite_direction >> 3);
        vehicle->track_progress = 31;
        if (carEntry->flags & CAR_ENTRY_FLAG_MINI_GOLF)
        {
            vehicle->track_progress = 15;
        }
        vehicle->update_flags = VEHICLE_UPDATE_FLAG_COLLISION_DISABLED;
        if (carEntry->flags & CAR_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET)
        {
            if (trackElement->IsInverted())
            {
                vehicle->SetUpdateFlag(VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES);
            }
        }
        vehicle->SetState(Vehicle::Status::MovingToEndOfStation);
    }

    // loc_6DDD5E:
    vehicle->num_peeps = 0;
    vehicle->next_free_seat = 0;
    vehicle->BoatLocation.SetNull();
    vehicle->IsCrashedVehicle = false;
    return vehicle;
}

/**
 *
 *  rct2: 0x006DD84C
 */
static train_ref vehicle_create_train(
    RideId rideIndex, const CoordsXYZ& trainPos, int32_t vehicleIndex, int32_t* remainingDistance, TrackElement* trackElement)
{
    train_ref train = { nullptr, nullptr };
    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        for (int32_t carIndex = 0; carIndex < ride->num_cars_per_train; carIndex++)
        {
            auto vehicle = ride_entry_get_vehicle_at_position(ride->subtype, ride->num_cars_per_train, carIndex);
            auto car = vehicle_create_car(
                rideIndex, vehicle, carIndex, vehicleIndex, trainPos, remainingDistance, trackElement);
            if (car == nullptr)
                break;

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
    }
    return train;
}

static bool vehicle_create_trains(RideId rideIndex, const CoordsXYZ& trainsPos, TrackElement* trackElement)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return false;

    train_ref firstTrain = {};
    train_ref lastTrain = {};
    int32_t remainingDistance = 0;
    bool allTrainsCreated = true;

    for (int32_t vehicleIndex = 0; vehicleIndex < ride->NumTrains; vehicleIndex++)
    {
        if (ride->IsBlockSectioned())
        {
            remainingDistance = 0;
        }
        train_ref train = vehicle_create_train(rideIndex, trainsPos, vehicleIndex, &remainingDistance, trackElement);
        if (train.head == nullptr || train.tail == nullptr)
        {
            allTrainsCreated = false;
            continue;
        }

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

        for (int32_t i = 0; i <= OpenRCT2::Limits::MaxTrainsPerRide; i++)
        {
            if (ride->vehicles[i].IsNull())
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

    return allTrainsCreated;
}

/**
 *
 *  rct2: 0x006DDE9E
 */
static void ride_create_vehicles_find_first_block(Ride* ride, CoordsXYE* outXYElement)
{
    Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[0]);
    if (vehicle == nullptr)
        return;

    auto curTrackPos = vehicle->TrackLocation;
    auto curTrackElement = MapGetTrackElementAt(curTrackPos);

    assert(curTrackElement != nullptr);

    CoordsXY trackPos = curTrackPos;
    auto trackElement = curTrackElement;
    track_begin_end trackBeginEnd;
    while (track_block_get_previous({ trackPos, reinterpret_cast<TileElement*>(trackElement) }, &trackBeginEnd))
    {
        trackPos = { trackBeginEnd.end_x, trackBeginEnd.end_y };
        trackElement = trackBeginEnd.begin_element->AsTrack();
        if (trackPos == curTrackPos && trackElement == curTrackElement)
        {
            break;
        }

        auto trackType = trackElement->GetTrackType();
        switch (trackType)
        {
            case TrackElemType::Up25ToFlat:
            case TrackElemType::Up60ToFlat:
                if (trackElement->HasChain())
                {
                    *outXYElement = { trackPos, reinterpret_cast<TileElement*>(trackElement) };
                    return;
                }
                break;
            case TrackElemType::DiagUp25ToFlat:
            case TrackElemType::DiagUp60ToFlat:
                if (trackElement->HasChain())
                {
                    TileElement* tileElement = MapGetTrackElementAtOfTypeSeq(
                        { trackBeginEnd.begin_x, trackBeginEnd.begin_y, trackBeginEnd.begin_z }, trackType, 0);

                    if (tileElement != nullptr)
                    {
                        outXYElement->x = trackBeginEnd.begin_x;
                        outXYElement->y = trackBeginEnd.begin_y;
                        outXYElement->element = tileElement;
                        return;
                    }
                }
                break;
            case TrackElemType::EndStation:
            case TrackElemType::CableLiftHill:
            case TrackElemType::BlockBrakes:
                *outXYElement = { trackPos, reinterpret_cast<TileElement*>(trackElement) };
                return;
        }
    }

    outXYElement->x = curTrackPos.x;
    outXYElement->y = curTrackPos.y;
    outXYElement->element = reinterpret_cast<TileElement*>(curTrackElement);
}

/**
 * Create and place the rides vehicles
 *  rct2: 0x006DD84C
 */
ResultWithMessage Ride::CreateVehicles(const CoordsXYE& element, bool isApplying)
{
    UpdateMaxVehicles();
    if (subtype == OBJECT_ENTRY_INDEX_NULL)
    {
        return { true };
    }

    // Check if there are enough free sprite slots for all the vehicles
    int32_t totalCars = NumTrains * num_cars_per_train;
    if (totalCars > count_free_misc_sprite_slots())
    {
        return { false, STR_UNABLE_TO_CREATE_ENOUGH_VEHICLES };
    }

    if (!isApplying)
    {
        return { true };
    }

    auto* trackElement = element.element->AsTrack();
    auto vehiclePos = CoordsXYZ{ element, element.element->GetBaseZ() };
    int32_t direction = trackElement->GetDirection();

    //
    if (mode == RideMode::StationToStation)
    {
        vehiclePos -= CoordsXYZ{ CoordsDirectionDelta[direction], 0 };

        trackElement = MapGetTrackElementAt(vehiclePos);

        vehiclePos.z = trackElement->GetBaseZ();
    }

    if (!vehicle_create_trains(id, vehiclePos, trackElement))
    {
        // This flag is needed for Ride::RemoveVehicles()
        lifecycle_flags |= RIDE_LIFECYCLE_ON_TRACK;
        RemoveVehicles();
        return { false, STR_UNABLE_TO_CREATE_ENOUGH_VEHICLES };
    }
    // return true;

    // Initialise station departs
    // 006DDDD0:
    lifecycle_flags |= RIDE_LIFECYCLE_ON_TRACK;
    for (int32_t i = 0; i < OpenRCT2::Limits::MaxStationsPerRide; i++)
    {
        stations[i].Depart = (stations[i].Depart & STATION_DEPART_FLAG) | 1;
    }

    //
    if (type != RIDE_TYPE_SPACE_RINGS && !GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL))
    {
        if (IsBlockSectioned())
        {
            CoordsXYE firstBlock{};
            ride_create_vehicles_find_first_block(this, &firstBlock);
            MoveTrainsToBlockBrakes(firstBlock.element->AsTrack());
        }
        else
        {
            for (int32_t i = 0; i < NumTrains; i++)
            {
                Vehicle* vehicle = GetEntity<Vehicle>(vehicles[i]);
                if (vehicle == nullptr)
                {
                    continue;
                }

                auto carEntry = vehicle->Entry();

                if (!(carEntry->flags & CAR_ENTRY_FLAG_DODGEM_CAR_PLACEMENT))
                {
                    vehicle->UpdateTrackMotion(nullptr);
                }

                vehicle->EnableCollisionsForTrain();
            }
        }
    }
    ride_update_vehicle_colours(this);
    return { true };
}

/**
 * Move all the trains so each one will be placed at the block brake of a different block.
 * The first vehicle will placed into the first block and all other vehicles in the blocks
 * preceding that block.
 *  rct2: 0x006DDF9C
 */
void Ride::MoveTrainsToBlockBrakes(TrackElement* firstBlock)
{
    for (int32_t i = 0; i < NumTrains; i++)
    {
        auto train = GetEntity<Vehicle>(vehicles[i]);
        if (train == nullptr)
            continue;

        train->UpdateTrackMotion(nullptr);

        if (i == 0)
        {
            train->EnableCollisionsForTrain();
            continue;
        }

        size_t numIterations = 0;
        do
        {
            // Fixes both freezing issues in #15503.
            // TODO: refactor the code so a tortoise-and-hare algorithm can be used.
            if (numIterations++ > 1000000)
            {
                break;
            }

            firstBlock->SetBlockBrakeClosed(true);
            for (Vehicle* car = train; car != nullptr; car = GetEntity<Vehicle>(car->next_vehicle_on_train))
            {
                car->velocity = 0;
                car->acceleration = 0;
                car->SwingSprite = 0;
                car->remaining_distance += 13962;
            }
        } while (!(train->UpdateTrackMotion(nullptr) & VEHICLE_UPDATE_MOTION_TRACK_FLAG_VEHICLE_AT_BLOCK_BRAKE));

        firstBlock->SetBlockBrakeClosed(true);
        for (Vehicle* car = train; car != nullptr; car = GetEntity<Vehicle>(car->next_vehicle_on_train))
        {
            car->ClearUpdateFlag(VEHICLE_UPDATE_FLAG_COLLISION_DISABLED);
            car->SetState(Vehicle::Status::Travelling, car->sub_state);
            if ((car->GetTrackType()) == TrackElemType::EndStation)
            {
                car->SetState(Vehicle::Status::MovingToEndOfStation, car->sub_state);
            }
        }
    }
}

/**
 * Checks and initialises the cable lift track returns false if unable to find
 * appropriate track.
 *  rct2: 0x006D31A6
 */
static ResultWithMessage ride_initialise_cable_lift_track(Ride* ride, bool isApplying)
{
    CoordsXYZ location;
    location.SetNull();
    for (const auto& station : ride->GetStations())
    {
        location = station.GetStart();
        if (!location.IsNull())
            break;
    }

    if (location.IsNull())
    {
        return { false, STR_CABLE_LIFT_HILL_MUST_START_IMMEDIATELY_AFTER_STATION };
    }

    bool success = false;
    TileElement* tileElement = MapGetFirstElementAt(location);
    if (tileElement == nullptr)
        return { false };
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
            continue;
        if (tileElement->GetBaseZ() != location.z)
            continue;

        const auto& ted = GetTrackElementDescriptor(tileElement->AsTrack()->GetTrackType());
        if (!(std::get<0>(ted.SequenceProperties) & TRACK_SEQUENCE_FLAG_ORIGIN))
        {
            continue;
        }
        success = true;
        break;
    } while (!(tileElement++)->IsLastForTile());

    if (!success)
        return { false };

    enum
    {
        STATE_FIND_CABLE_LIFT,
        STATE_FIND_STATION,
        STATE_REST_OF_TRACK
    };
    int32_t state = STATE_FIND_CABLE_LIFT;

    track_circuit_iterator it;
    track_circuit_iterator_begin(&it, { location, tileElement });
    while (track_circuit_iterator_previous(&it))
    {
        tileElement = it.current.element;
        auto trackType = tileElement->AsTrack()->GetTrackType();

        uint16_t flags = TRACK_ELEMENT_SET_HAS_CABLE_LIFT_FALSE;
        switch (state)
        {
            case STATE_FIND_CABLE_LIFT:
                // Search for a cable lift hill track element
                if (trackType == TrackElemType::CableLiftHill)
                {
                    flags = TRACK_ELEMENT_SET_HAS_CABLE_LIFT_TRUE;
                    state = STATE_FIND_STATION;
                }
                break;
            case STATE_FIND_STATION:
                // Search for the start of the hill
                switch (trackType)
                {
                    case TrackElemType::Flat:
                    case TrackElemType::Up25:
                    case TrackElemType::Up60:
                    case TrackElemType::FlatToUp25:
                    case TrackElemType::Up25ToFlat:
                    case TrackElemType::Up25ToUp60:
                    case TrackElemType::Up60ToUp25:
                    case TrackElemType::FlatToUp60LongBase:
                        flags = TRACK_ELEMENT_SET_HAS_CABLE_LIFT_TRUE;
                        break;
                    case TrackElemType::EndStation:
                        state = STATE_REST_OF_TRACK;
                        break;
                    default:
                        return { false, STR_CABLE_LIFT_HILL_MUST_START_IMMEDIATELY_AFTER_STATION };
                }
                break;
        }
        if (isApplying)
        {
            auto tmpLoc = CoordsXYZ{ it.current, tileElement->GetBaseZ() };
            auto direction = tileElement->GetDirection();
            trackType = tileElement->AsTrack()->GetTrackType();
            GetTrackElementOriginAndApplyChanges({ tmpLoc, direction }, trackType, 0, &tileElement, flags);
        }
    }
    return { true };
}

/**
 *
 *  rct2: 0x006DF4D4
 */
static ResultWithMessage ride_create_cable_lift(RideId rideIndex, bool isApplying)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return { false };

    if (ride->mode != RideMode::ContinuousCircuitBlockSectioned && ride->mode != RideMode::ContinuousCircuit)
    {
        return { false, STR_CABLE_LIFT_UNABLE_TO_WORK_IN_THIS_OPERATING_MODE };
    }

    if (ride->num_circuits > 1)
    {
        return { false, STR_MULTICIRCUIT_NOT_POSSIBLE_WITH_CABLE_LIFT_HILL };
    }

    if (count_free_misc_sprite_slots() <= 5)
    {
        return { false, STR_UNABLE_TO_CREATE_ENOUGH_VEHICLES };
    }

    auto cableLiftInitialiseResult = ride_initialise_cable_lift_track(ride, isApplying);
    if (!cableLiftInitialiseResult.Successful)
    {
        return { false, cableLiftInitialiseResult.Message };
    }

    if (!isApplying)
    {
        return { true };
    }

    auto cableLiftLoc = ride->CableLiftLoc;
    auto tileElement = MapGetTrackElementAt(cableLiftLoc);
    int32_t direction = tileElement->GetDirection();

    Vehicle* head = nullptr;
    Vehicle* tail = nullptr;
    uint32_t ebx = 0;
    for (int32_t i = 0; i < 5; i++)
    {
        uint32_t edx = Numerics::ror32(0x15478, 10);
        uint16_t var_44 = edx & 0xFFFF;
        edx = Numerics::rol32(edx, 10) >> 1;
        ebx -= edx;
        int32_t remaining_distance = ebx;
        ebx -= edx;

        Vehicle* current = cable_lift_segment_create(
            *ride, cableLiftLoc.x, cableLiftLoc.y, cableLiftLoc.z / 8, direction, var_44, remaining_distance, i == 0);
        current->next_vehicle_on_train = EntityId::GetNull();
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
    head->CableLiftUpdateTrackMotion();
    return { true };
}

/**
 * Opens the construction window prompting to construct a missing entrance or exit.
 * This will also move the screen to the first station missing the entrance or exit.
 *  rct2: 0x006B51C0
 */
void Ride::ConstructMissingEntranceOrExit() const
{
    auto* w = window_get_main();
    if (w == nullptr)
        return;

    int8_t entranceOrExit = -1;
    const RideStation* incompleteStation = nullptr;
    for (const auto& station : stations)
    {
        if (station.Start.IsNull())
            continue;

        if (station.Entrance.IsNull())
        {
            entranceOrExit = WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE;
            incompleteStation = &station;
            break;
        }

        if (station.Exit.IsNull())
        {
            entranceOrExit = WC_RIDE_CONSTRUCTION__WIDX_EXIT;
            incompleteStation = &station;
            break;
        }
    }

    if (incompleteStation == nullptr)
    { // No station with a missing entrance or exit was found
        return;
    }

    if (type != RIDE_TYPE_MAZE)
    {
        auto location = incompleteStation->GetStart();
        window_scroll_to_location(*w, location);

        CoordsXYE trackElement;
        ride_try_get_origin_element(this, &trackElement);
        ride_find_track_gap(this, &trackElement, &trackElement);
        int32_t ok = ride_modify(trackElement);
        if (ok == 0)
        {
            return;
        }

        w = window_find_by_class(WindowClass::RideConstruction);
        if (w != nullptr)
            window_event_mouse_up_call(w, entranceOrExit);
    }
}

/**
 *
 *  rct2: 0x006B528A
 */
static void ride_scroll_to_track_error(const CoordsXYE& trackElement)
{
    if (trackElement.element == nullptr)
        return;

    auto* w = window_get_main();
    if (w != nullptr)
    {
        window_scroll_to_location(*w, { trackElement, trackElement.element->GetBaseZ() });
        ride_modify(trackElement);
    }
}

/**
 *
 *  rct2: 0x006B4F6B
 */
TrackElement* Ride::GetOriginElement(StationIndex stationIndex) const
{
    auto stationLoc = GetStation(stationIndex).Start;
    TileElement* tileElement = MapGetFirstElementAt(stationLoc);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
            continue;

        auto* trackElement = tileElement->AsTrack();
        const auto& ted = GetTrackElementDescriptor(trackElement->GetTrackType());
        if (!(std::get<0>(ted.SequenceProperties) & TRACK_SEQUENCE_FLAG_ORIGIN))
            continue;

        if (trackElement->GetRideIndex() == id)
            return trackElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

ResultWithMessage Ride::Test(RideStatus newStatus, bool isApplying)
{
    CoordsXYE trackElement, problematicTrackElement = {};

    if (type == RIDE_TYPE_NULL)
    {
        log_warning("Invalid ride type for ride %u", id.ToUnderlying());
        return { false };
    }

    if (newStatus != RideStatus::Simulating)
    {
        window_close_by_number(WindowClass::RideConstruction, id.ToUnderlying());
    }

    auto stationIndexCheck = ride_mode_check_station_present(this);
    auto stationIndex = stationIndexCheck.StationIndex;
    if (stationIndex.IsNull())
        return { false, stationIndexCheck.Message };

    auto stationNumbersCheck = ride_mode_check_valid_station_numbers(this);
    if (!stationNumbersCheck.Successful)
        return { false, stationNumbersCheck.Message };

    if (newStatus != RideStatus::Simulating)
    {
        auto entranceExitCheck = ride_check_for_entrance_exit(id);
        if (!entranceExitCheck.Successful)
        {
            ConstructMissingEntranceOrExit();
            return { false, entranceExitCheck.Message };
        }
    }

    // z = ride->stations[i].GetBaseZ();
    auto startLoc = GetStation(stationIndex).Start;
    trackElement.x = startLoc.x;
    trackElement.y = startLoc.y;
    trackElement.element = reinterpret_cast<TileElement*>(GetOriginElement(stationIndex));
    if (trackElement.element == nullptr)
    {
        // Maze is strange, station start is 0... investigation required
        if (type != RIDE_TYPE_MAZE)
            return { false };
    }

    if (mode == RideMode::ContinuousCircuit || IsBlockSectioned())
    {
        if (ride_find_track_gap(this, &trackElement, &problematicTrackElement)
            && (newStatus != RideStatus::Simulating || IsBlockSectioned()))
        {
            ride_scroll_to_track_error(problematicTrackElement);
            return { false, STR_TRACK_IS_NOT_A_COMPLETE_CIRCUIT };
        }
    }

    if (IsBlockSectioned())
    {
        auto blockBrakeCheck = RideCheckBlockBrakes(trackElement, &problematicTrackElement);
        if (!blockBrakeCheck.Successful)
        {
            ride_scroll_to_track_error(problematicTrackElement);
            return { false, blockBrakeCheck.Message };
        }
    }

    if (subtype != OBJECT_ENTRY_INDEX_NULL && !gCheatsEnableAllDrawableTrackPieces)
    {
        rct_ride_entry* rideType = get_ride_entry(subtype);
        if (rideType->flags & RIDE_ENTRY_FLAG_NO_INVERSIONS)
        {
            if (ride_check_track_contains_inversions(&trackElement, &problematicTrackElement))
            {
                ride_scroll_to_track_error(problematicTrackElement);
                return { false, STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN };
            }
        }
        if (rideType->flags & RIDE_ENTRY_FLAG_NO_BANKED_TRACK)
        {
            if (ride_check_track_contains_banked(&trackElement, &problematicTrackElement))
            {
                ride_scroll_to_track_error(problematicTrackElement);
                return { false, STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN };
            }
        }
    }

    if (mode == RideMode::StationToStation)
    {
        if (!ride_find_track_gap(this, &trackElement, &problematicTrackElement))
        {
            return { false, STR_RIDE_MUST_START_AND_END_WITH_STATIONS };
        }

        if (!ride_check_station_length(&trackElement, &problematicTrackElement))
        {
            ride_scroll_to_track_error(problematicTrackElement);
            return { false, STR_STATION_NOT_LONG_ENOUGH };
        }

        if (!ride_check_start_and_end_is_station(&trackElement))
        {
            ride_scroll_to_track_error(problematicTrackElement);
            return { false, STR_RIDE_MUST_START_AND_END_WITH_STATIONS };
        }
    }

    if (isApplying)
        ride_set_start_finish_points(id, &trackElement);

    const auto& rtd = GetRideTypeDescriptor();
    if (!rtd.HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES) && !(lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
    {
        const auto createVehicleResult = CreateVehicles(trackElement, isApplying);
        if (!createVehicleResult.Successful)
        {
            return { false, createVehicleResult.Message };
        }
    }

    if (rtd.HasFlag(RIDE_TYPE_FLAG_ALLOW_CABLE_LIFT_HILL) && (lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED)
        && !(lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT))
    {
        const auto createCableLiftResult = ride_create_cable_lift(id, isApplying);
        if (!createCableLiftResult.Successful)
            return { false, createCableLiftResult.Message };
    }

    return { true };
}
/**
 *
 *  rct2: 0x006B4EEA
 */
ResultWithMessage Ride::Open(bool isApplying)
{
    CoordsXYE trackElement, problematicTrackElement = {};

    // Check to see if construction tool is in use. If it is close the construction window
    // to set the track to its final state and clean up ghosts.
    // We can't just call close as it would cause a stack overflow during shop creation
    // with auto open on.
    if (WindowClass::RideConstruction == gCurrentToolWidget.window_classification
        && id.ToUnderlying() == gCurrentToolWidget.window_number && (input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
    {
        window_close_by_number(WindowClass::RideConstruction, id.ToUnderlying());
    }

    auto stationIndexCheck = ride_mode_check_station_present(this);
    auto stationIndex = stationIndexCheck.StationIndex;
    if (stationIndex.IsNull())
        return { false, stationIndexCheck.Message };

    auto stationNumbersCheck = ride_mode_check_valid_station_numbers(this);
    if (!stationNumbersCheck.Successful)
        return { false, stationNumbersCheck.Message };

    auto entranceExitCheck = ride_check_for_entrance_exit(id);
    if (!entranceExitCheck.Successful)
    {
        ConstructMissingEntranceOrExit();
        return { false, entranceExitCheck.Message };
    }

    if (isApplying)
    {
        ChainQueues();
        lifecycle_flags |= RIDE_LIFECYCLE_EVER_BEEN_OPENED;
    }

    // z = ride->stations[i].GetBaseZ();
    auto startLoc = GetStation(stationIndex).Start;
    trackElement.x = startLoc.x;
    trackElement.y = startLoc.y;
    trackElement.element = reinterpret_cast<TileElement*>(GetOriginElement(stationIndex));
    if (trackElement.element == nullptr)
    {
        // Maze is strange, station start is 0... investigation required
        if (type != RIDE_TYPE_MAZE)
            return { false };
    }

    if (mode == RideMode::Race || mode == RideMode::ContinuousCircuit || IsBlockSectioned())
    {
        if (ride_find_track_gap(this, &trackElement, &problematicTrackElement))
        {
            ride_scroll_to_track_error(problematicTrackElement);
            return { false, STR_TRACK_IS_NOT_A_COMPLETE_CIRCUIT };
        }
    }

    if (IsBlockSectioned())
    {
        auto blockBrakeCheck = RideCheckBlockBrakes(trackElement, &problematicTrackElement);
        if (!blockBrakeCheck.Successful)
        {
            ride_scroll_to_track_error(problematicTrackElement);
            return { false, blockBrakeCheck.Message };
        }
    }

    if (subtype != OBJECT_ENTRY_INDEX_NULL && !gCheatsEnableAllDrawableTrackPieces)
    {
        rct_ride_entry* rideEntry = get_ride_entry(subtype);
        if (rideEntry->flags & RIDE_ENTRY_FLAG_NO_INVERSIONS)
        {
            if (ride_check_track_contains_inversions(&trackElement, &problematicTrackElement))
            {
                ride_scroll_to_track_error(problematicTrackElement);
                return { false, STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN };
            }
        }
        if (rideEntry->flags & RIDE_ENTRY_FLAG_NO_BANKED_TRACK)
        {
            if (ride_check_track_contains_banked(&trackElement, &problematicTrackElement))
            {
                ride_scroll_to_track_error(problematicTrackElement);
                return { false, STR_TRACK_UNSUITABLE_FOR_TYPE_OF_TRAIN };
            }
        }
    }

    if (mode == RideMode::StationToStation)
    {
        if (!ride_find_track_gap(this, &trackElement, &problematicTrackElement))
        {
            return { false, STR_RIDE_MUST_START_AND_END_WITH_STATIONS };
        }

        if (!ride_check_station_length(&trackElement, &problematicTrackElement))
        {
            ride_scroll_to_track_error(problematicTrackElement);
            return { false, STR_STATION_NOT_LONG_ENOUGH };
        }

        if (!ride_check_start_and_end_is_station(&trackElement))
        {
            ride_scroll_to_track_error(problematicTrackElement);
            return { false, STR_RIDE_MUST_START_AND_END_WITH_STATIONS };
        }
    }

    if (isApplying)
        ride_set_start_finish_points(id, &trackElement);

    if (!GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_NO_VEHICLES) && !(lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
    {
        const auto createVehicleResult = CreateVehicles(trackElement, isApplying);
        if (!createVehicleResult.Successful)
        {
            return { false, createVehicleResult.Message };
        }
    }

    if ((GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_ALLOW_CABLE_LIFT_HILL))
        && (lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED) && !(lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT))
    {
        const auto createCableLiftResult = ride_create_cable_lift(id, isApplying);
        if (!createCableLiftResult.Successful)
            return { false, createCableLiftResult.Message };
    }

    return { true };
}

/**
 * Given a track element of the ride, find the start of the track.
 * It has to do this as a backwards loop in case this is an incomplete track.
 */
void ride_get_start_of_track(CoordsXYE* output)
{
    track_begin_end trackBeginEnd;
    CoordsXYE trackElement = *output;
    if (track_block_get_previous(trackElement, &trackBeginEnd))
    {
        TileElement* initial_map = trackElement.element;
        track_begin_end slowIt = trackBeginEnd;
        bool moveSlowIt = true;
        do
        {
            // Because we are working backwards, begin_element is the section at the end of a piece of track, whereas
            // begin_x and begin_y are the coordinates at the start of a piece of track, so we need to pass end_x and
            // end_y
            CoordsXYE lastGood = {
                /* .x = */ trackBeginEnd.end_x,
                /* .y = */ trackBeginEnd.end_y,
                /* .element = */ trackBeginEnd.begin_element,
            };

            if (!track_block_get_previous(
                    { trackBeginEnd.end_x, trackBeginEnd.end_y, trackBeginEnd.begin_element }, &trackBeginEnd))
            {
                trackElement = lastGood;
                break;
            }

            moveSlowIt = !moveSlowIt;
            if (moveSlowIt)
            {
                if (!track_block_get_previous({ slowIt.end_x, slowIt.end_y, slowIt.begin_element }, &slowIt)
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
 *  rct2: 0x00696707
 */
void Ride::StopGuestsQueuing()
{
    for (auto peep : EntityList<Guest>())
    {
        if (peep->State != PeepState::Queuing)
            continue;
        if (peep->CurrentRide != id)
            continue;

        peep->RemoveFromQueue();
        peep->SetState(PeepState::Falling);
    }
}

RideMode Ride::GetDefaultMode() const
{
    return GetRideTypeDescriptor().DefaultMode;
}

static bool RideTypeWithTrackColoursExists(ride_type_t rideType, const TrackColour& colours)
{
    for (auto& ride : GetRideManager())
    {
        if (ride.type != rideType)
            continue;
        if (ride.track_colour[0].main != colours.main)
            continue;
        if (ride.track_colour[0].additional != colours.additional)
            continue;
        if (ride.track_colour[0].supports != colours.supports)
            continue;

        return true;
    }
    return false;
}

bool Ride::NameExists(std::string_view name, RideId excludeRideId)
{
    char buffer[256]{};
    for (auto& ride : GetRideManager())
    {
        if (ride.id != excludeRideId)
        {
            Formatter ft;
            ride.FormatNameTo(ft);
            format_string(buffer, 256, STR_STRINGID, ft.Data());
            if (name == buffer && ride_has_any_track_elements(&ride))
            {
                return true;
            }
        }
    }
    return false;
}

int32_t ride_get_random_colour_preset_index(ride_type_t rideType)
{
    if (rideType >= std::size(RideTypeDescriptors))
    {
        return 0;
    }

    // Find all the presets that haven't yet been used in the park for this ride type
    const auto& colourPresets = GetRideTypeDescriptor(rideType).ColourPresets;
    std::vector<uint8_t> unused;
    unused.reserve(colourPresets.count);
    for (uint8_t i = 0; i < colourPresets.count; i++)
    {
        const auto& colours = colourPresets.list[i];
        if (!RideTypeWithTrackColoursExists(rideType, colours))
        {
            unused.push_back(static_cast<uint8_t>(i));
        }
    }

    // If all presets have been used, just go with a random preset
    if (unused.size() == 0)
        return util_rand() % colourPresets.count;

    // Choose a random preset from the list of unused presets
    auto unusedIndex = util_rand() % unused.size();
    return unused[unusedIndex];
}

/**
 *
 *  Based on rct2: 0x006B4776
 */
void Ride::SetColourPreset(uint8_t index)
{
    const track_colour_preset_list* colourPresets = &GetRideTypeDescriptor().ColourPresets;
    TrackColour colours = { COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK };
    // Stalls save their default colour in the vehicle settings (since they share a common ride type)
    if (!IsRide())
    {
        auto rideEntry = get_ride_entry(subtype);
        if (rideEntry != nullptr && rideEntry->vehicle_preset_list->count > 0)
        {
            auto list = rideEntry->vehicle_preset_list->list[0];
            colours = { list.Body, list.Trim, list.Tertiary };
        }
    }
    else if (index < colourPresets->count)
    {
        colours = colourPresets->list[index];
    }
    for (int32_t i = 0; i < OpenRCT2::Limits::NumColourSchemes; i++)
    {
        track_colour[i].main = colours.main;
        track_colour[i].additional = colours.additional;
        track_colour[i].supports = colours.supports;
    }
    colour_scheme_type = 0;
}

money32 ride_get_common_price(Ride* forRide)
{
    for (const auto& ride : GetRideManager())
    {
        if (ride.type == forRide->type && &ride != forRide)
        {
            return ride.price[0];
        }
    }

    return MONEY32_UNDEFINED;
}

void Ride::SetNameToDefault()
{
    char rideNameBuffer[256]{};

    // Increment default name number until we find a unique name
    custom_name = {};
    default_name_number = 0;
    do
    {
        default_name_number++;
        Formatter ft;
        FormatNameTo(ft);
        format_string(rideNameBuffer, 256, STR_STRINGID, ft.Data());
    } while (Ride::NameExists(rideNameBuffer, id));
}

/**
 * This will return the name of the ride, as seen in the New Ride window.
 */
RideNaming get_ride_naming(const ride_type_t rideType, rct_ride_entry* rideEntry)
{
    if (!GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
    {
        return GetRideTypeDescriptor(rideType).Naming;
    }

    return rideEntry->naming;
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

bool Ride::HasSpinningTunnel() const
{
    return special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
}

bool Ride::HasWaterSplash() const
{
    return special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
}

bool Ride::HasRapids() const
{
    return special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS;
}

bool Ride::HasLogReverser() const
{
    return special_track_elements & RIDE_ELEMENT_REVERSER_OR_WATERFALL;
}

bool Ride::HasWaterfall() const
{
    return special_track_elements & RIDE_ELEMENT_REVERSER_OR_WATERFALL;
}

bool Ride::HasWhirlpool() const
{
    return special_track_elements & RIDE_ELEMENT_WHIRLPOOL;
}

uint8_t ride_get_helix_sections(Ride* ride)
{
    // Helix sections stored in the low 5 bits.
    return ride->special_track_elements & 0x1F;
}

bool Ride::IsPoweredLaunched() const
{
    return mode == RideMode::PoweredLaunchPasstrough || mode == RideMode::PoweredLaunch
        || mode == RideMode::PoweredLaunchBlockSectioned;
}

bool Ride::IsBlockSectioned() const
{
    return mode == RideMode::ContinuousCircuitBlockSectioned || mode == RideMode::PoweredLaunchBlockSectioned;
}

bool ride_has_any_track_elements(const Ride* ride)
{
    tile_element_iterator it;

    TileElementIteratorBegin(&it);
    while (TileElementIteratorNext(&it))
    {
        if (it.element->GetType() != TileElementType::Track)
            continue;
        if (it.element->AsTrack()->GetRideIndex() != ride->id)
            continue;
        if (it.element->IsGhost())
            continue;

        return true;
    }

    return false;
}

/**
 *
 *  rct2: 0x006847BA
 */
void set_vehicle_type_image_max_sizes(CarEntry* vehicle_type, int32_t num_images)
{
    uint8_t bitmap[200][200] = { 0 };

    rct_drawpixelinfo dpi = {
        /*.bits = */ reinterpret_cast<uint8_t*>(bitmap),
        /*.x = */ -100,
        /*.y = */ -100,
        /*.width = */ 200,
        /*.height = */ 200,
        /*.pitch = */ 0,
        /*.zoom_level = */ ZoomLevel{ 0 },
    };

    for (int32_t i = 0; i < num_images; ++i)
    {
        gfx_draw_sprite_software(&dpi, ImageId(vehicle_type->base_image_id + i), { 0, 0 });
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

    if (vehicle_type->flags & CAR_ENTRY_FLAG_SPRITE_BOUNDS_INCLUDE_INVERTED_SET)
    {
        bl += 16;
    }

    vehicle_type->sprite_width = al;
    vehicle_type->sprite_height_negative = bl;
    vehicle_type->sprite_height_positive = bh;
}

/**
 *
 *  rct2: 0x006B59C6
 */
void invalidate_test_results(Ride* ride)
{
    ride->measurement = {};
    ride->excitement = RIDE_RATING_UNDEFINED;
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_TESTED;
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_TEST_IN_PROGRESS;
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        for (int32_t i = 0; i < ride->NumTrains; i++)
        {
            Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[i]);
            if (vehicle != nullptr)
            {
                vehicle->ClearUpdateFlag(VEHICLE_UPDATE_FLAG_TESTING);
            }
        }
    }
    window_invalidate_by_number(WindowClass::Ride, ride->id.ToUnderlying());
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
        for (int32_t i = 0; i < ride->NumTrains; i++)
        {
            for (Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[i]); vehicle != nullptr;
                 vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
            {
                vehicle->ClearUpdateFlag(VEHICLE_UPDATE_FLAG_ZERO_VELOCITY);
                vehicle->ClearUpdateFlag(VEHICLE_UPDATE_FLAG_BROKEN_CAR);
                vehicle->ClearUpdateFlag(VEHICLE_UPDATE_FLAG_BROKEN_TRAIN);
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
    if (ride->type == RIDE_TYPE_SPACE_RINGS || ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL))
    {
        gfx_invalidate_screen();
    }

    for (int32_t i = 0; i <= OpenRCT2::Limits::MaxTrainsPerRide; i++)
    {
        int32_t carIndex = 0;
        VehicleColour colours = {};

        for (Vehicle* vehicle = GetEntity<Vehicle>(ride->vehicles[i]); vehicle != nullptr;
             vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
        {
            switch (ride->colour_scheme_type & 3)
            {
                case RIDE_COLOUR_SCHEME_MODE_ALL_SAME:
                    colours = ride->vehicle_colours[0];
                    break;
                case RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_TRAIN:
                    colours = ride->vehicle_colours[i];
                    break;
                case RIDE_COLOUR_SCHEME_MODE_DIFFERENT_PER_CAR:
                    colours = ride->vehicle_colours[std::min(carIndex, OpenRCT2::Limits::MaxCarsPerTrain - 1)];
                    break;
            }

            vehicle->colours = colours;
            vehicle->Invalidate();
            carIndex++;
        }
    }
}

uint8_t ride_entry_get_vehicle_at_position(int32_t rideEntryIndex, int32_t numCarsPerTrain, int32_t position)
{
    rct_ride_entry* rideEntry = get_ride_entry(rideEntryIndex);
    if (position == 0 && rideEntry->FrontCar != 255)
    {
        return rideEntry->FrontCar;
    }
    if (position == 1 && rideEntry->SecondCar != 255)
    {
        return rideEntry->SecondCar;
    }
    if (position == 2 && rideEntry->ThirdCar != 255)
    {
        return rideEntry->ThirdCar;
    }
    if (position == numCarsPerTrain - 1 && rideEntry->RearCar != 255)
    {
        return rideEntry->RearCar;
    }

    return rideEntry->DefaultCar;
}

using namespace OpenRCT2::Entity::Yaw;

struct NecessarySpriteGroup
{
    SpriteGroupType VehicleSpriteGroup;
    SpritePrecision MinPrecision;
};

// Finds track pieces that a given ride entry has sprites for
OpenRCT2::BitSet<TRACK_GROUP_COUNT> ride_entry_get_supported_track_pieces(const rct_ride_entry* rideEntry)
{
    // TODO: Use a std::span when C++20 available as 6 is due to jagged array
    static const std::array<NecessarySpriteGroup, 6> trackPieceRequiredSprites[TRACK_GROUP_COUNT] = {
        { SpriteGroupType::SlopeFlat, SpritePrecision::None },     // TRACK_FLAT
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4 }, // TRACK_STRAIGHT
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4 }, // TRACK_STATION_END
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4 },  // TRACK_LIFT_HILL
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60,
          SpritePrecision::Sprites4 },                             // TRACK_LIFT_HILL_STEEP
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites16 }, // TRACK_LIFT_HILL_CURVE
        { SpriteGroupType::FlatBanked22, SpritePrecision::Sprites4, SpriteGroupType::FlatBanked45,
          SpritePrecision::Sprites16 }, // TRACK_FLAT_ROLL_BANKING
        { SpriteGroupType::Slopes60, SpritePrecision::Sprites4, SpriteGroupType::Slopes75, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes90, SpritePrecision::Sprites4, SpriteGroupType::SlopesLoop, SpritePrecision::Sprites4,
          SpriteGroupType::SlopeInverted, SpritePrecision::Sprites4 }, // TRACK_VERTICAL_LOOP
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4 },      // TRACK_SLOPE
        { SpriteGroupType::Slopes60, SpritePrecision::Sprites4 },      // TRACK_SLOPE_STEEP_DOWN
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60,
          SpritePrecision::Sprites4 },                              // TRACK_SLOPE_LONG
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites16 },  // TRACK_SLOPE_CURVE
        { SpriteGroupType::Slopes60, SpritePrecision::Sprites16 },  // TRACK_SLOPE_CURVE_STEEP
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites16 }, // TRACK_S_BEND
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites16 }, // TRACK_CURVE_VERY_SMALL
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites16 }, // TRACK_CURVE_SMALL
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites16 }, // TRACK_CURVE
        { SpriteGroupType::FlatBanked22, SpritePrecision::Sprites4, SpriteGroupType::FlatBanked45, SpritePrecision::Sprites4,
          SpriteGroupType::FlatBanked67, SpritePrecision::Sprites4, SpriteGroupType::FlatBanked90, SpritePrecision::Sprites4,
          SpriteGroupType::InlineTwists, SpritePrecision::Sprites4, SpriteGroupType::SlopeInverted,
          SpritePrecision::Sprites4 }, // TRACK_TWIST
        { SpriteGroupType::Slopes60, SpritePrecision::Sprites4, SpriteGroupType::Slopes75, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes90, SpritePrecision::Sprites4, SpriteGroupType::SlopesLoop, SpritePrecision::Sprites4,
          SpriteGroupType::SlopeInverted, SpritePrecision::Sprites4 }, // TRACK_HALF_LOOP
        { SpriteGroupType::Corkscrews, SpritePrecision::Sprites4, SpriteGroupType::SlopeInverted,
          SpritePrecision::Sprites4 },                                 // TRACK_CORKSCREW
        { SpriteGroupType::SlopeFlat, SpritePrecision::None },         // TRACK_TOWER_BASE
        { SpriteGroupType::FlatBanked45, SpritePrecision::Sprites16 }, // TRACK_HELIX_SMALL
        { SpriteGroupType::FlatBanked45, SpritePrecision::Sprites16 }, // TRACK_HELIX_LARGE
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites16 },    // TRACK_HELIX_LARGE_UNBANKED
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4 },     // TRACK_BRAKES
        {},                                                            // TRACK_25
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4 },     // TRACK_ON_RIDE_PHOTO
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4, SpriteGroupType::Slopes12,
          SpritePrecision::Sprites4 }, // TRACK_WATER_SPLASH
        { SpriteGroupType::Slopes75, SpritePrecision::Sprites4, SpriteGroupType::Slopes90,
          SpritePrecision::Sprites4 }, // TRACK_SLOPE_VERTICAL
        { SpriteGroupType::FlatBanked22, SpritePrecision::Sprites4, SpriteGroupType::FlatBanked45, SpritePrecision::Sprites4,
          SpriteGroupType::InlineTwists, SpritePrecision::Sprites4, SpriteGroupType::SlopeInverted,
          SpritePrecision::Sprites4 },                            // TRACK_BARREL_ROLL
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4 }, // TRACK_POWERED_LIFT
        { SpriteGroupType::Slopes60, SpritePrecision::Sprites4, SpriteGroupType::Slopes75, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes90, SpritePrecision::Sprites4, SpriteGroupType::SlopesLoop, SpritePrecision::Sprites4,
          SpriteGroupType::SlopeInverted, SpritePrecision::Sprites4 },     // TRACK_HALF_LOOP_LARGE
        { SpriteGroupType::Slopes25Banked45, SpritePrecision::Sprites16 }, // TRACK_SLOPE_CURVE_BANKED
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites16 },        // TRACK_LOG_FLUME_REVERSER
        { SpriteGroupType::FlatBanked22, SpritePrecision::Sprites4, SpriteGroupType::FlatBanked45, SpritePrecision::Sprites4,
          SpriteGroupType::InlineTwists, SpritePrecision::Sprites4, SpriteGroupType::SlopeInverted,
          SpritePrecision::Sprites4 },                              // TRACK_HEARTLINE_ROLL
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites16 }, // TRACK_REVERSER
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4, SpriteGroupType::Slopes25, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes60, SpritePrecision::Sprites4, SpriteGroupType::Slopes75, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes90, SpritePrecision::Sprites4 }, // TRACK_REVERSE_FREEFALL
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4, SpriteGroupType::Slopes25, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes60, SpritePrecision::Sprites4, SpriteGroupType::Slopes75, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes90, SpritePrecision::Sprites4 },         // TRACK_SLOPE_TO_FLAT
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4 },        // TRACK_BLOCK_BRAKES
        { SpriteGroupType::Slopes25Banked22, SpritePrecision::Sprites4 }, // TRACK_SLOPE_ROLL_BANKING
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60,
          SpritePrecision::Sprites4 },                             // TRACK_SLOPE_STEEP_LONG
        { SpriteGroupType::Slopes90, SpritePrecision::Sprites16 }, // TRACK_CURVE_VERTICAL
        {},                                                        // TRACK_42
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60,
          SpritePrecision::Sprites4 },                                   // TRACK_LIFT_HILL_CABLE
        { SpriteGroupType::CurvedLiftHill, SpritePrecision::Sprites16 }, // TRACK_LIFT_HILL_CURVED
        { SpriteGroupType::Slopes90, SpritePrecision::Sprites4, SpriteGroupType::SlopesLoop, SpritePrecision::Sprites4,
          SpriteGroupType::SlopeInverted, SpritePrecision::Sprites4 }, // TRACK_QUARTER_LOOP
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4 },     // TRACK_SPINNING_TUNNEL
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4 },     // TRACK_BOOSTER
        { SpriteGroupType::FlatBanked22, SpritePrecision::Sprites4, SpriteGroupType::FlatBanked45, SpritePrecision::Sprites4,
          SpriteGroupType::FlatBanked67, SpritePrecision::Sprites4, SpriteGroupType::FlatBanked90, SpritePrecision::Sprites4,
          SpriteGroupType::InlineTwists, SpritePrecision::Sprites4, SpriteGroupType::SlopeInverted,
          SpritePrecision::Sprites4 }, // TRACK_INLINE_TWIST_UNINVERTED
        { SpriteGroupType::FlatBanked22, SpritePrecision::Sprites4, SpriteGroupType::FlatBanked45, SpritePrecision::Sprites4,
          SpriteGroupType::FlatBanked67, SpritePrecision::Sprites4, SpriteGroupType::FlatBanked90, SpritePrecision::Sprites4,
          SpriteGroupType::InlineTwists, SpritePrecision::Sprites4, SpriteGroupType::SlopeInverted,
          SpritePrecision::Sprites4 }, // TRACK_INLINE_TWIST_INVERTED
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes75, SpritePrecision::Sprites4, SpriteGroupType::Slopes90,
          SpritePrecision::Sprites4 }, // TRACK_QUARTER_LOOP_UNINVERTED_UP
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes75, SpritePrecision::Sprites4, SpriteGroupType::Slopes90,
          SpritePrecision::Sprites4 }, // TRACK_QUARTER_LOOP_UNINVERTED_DOWN
        { SpriteGroupType::Slopes90, SpritePrecision::Sprites4, SpriteGroupType::SlopesLoop, SpritePrecision::Sprites4,
          SpriteGroupType::SlopeInverted, SpritePrecision::Sprites4 }, // TRACK_QUARTER_LOOP_INVERTED_UP
        { SpriteGroupType::Slopes90, SpritePrecision::Sprites4, SpriteGroupType::SlopesLoop, SpritePrecision::Sprites4,
          SpriteGroupType::SlopeInverted, SpritePrecision::Sprites4 }, // TRACK_QUARTER_LOOP_INVERTED_DOWN
        { SpriteGroupType::Slopes12, SpritePrecision::Sprites4 },      // TRACK_RAPIDS
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes75, SpritePrecision::Sprites4, SpriteGroupType::Slopes90,
          SpritePrecision::Sprites4 }, // TRACK_FLYING_HALF_LOOP_UNINVERTED_UP
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes75, SpritePrecision::Sprites4, SpriteGroupType::Slopes90, SpritePrecision::Sprites4,
          SpriteGroupType::SlopesLoop, SpritePrecision::Sprites4, SpriteGroupType::SlopeInverted,
          SpritePrecision::Sprites4 },                             // TRACK_FLYING_HALF_LOOP_INVERTED_DOWN
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4 }, // TRACK_WATERFALL
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4 }, // TRACK_WHIRLPOOL
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60,
          SpritePrecision::Sprites4 }, // TRACK_BRAKE_FOR_DROP
        { SpriteGroupType::Corkscrews, SpritePrecision::Sprites4, SpriteGroupType::SlopeInverted,
          SpritePrecision::Sprites4 }, // TRACK_CORKSCREW_UNINVERTED
        { SpriteGroupType::Corkscrews, SpritePrecision::Sprites4, SpriteGroupType::SlopeInverted,
          SpritePrecision::Sprites4 }, // TRACK_CORKSCREW_INVERTED
        { SpriteGroupType::Slopes12, SpritePrecision::Sprites4, SpriteGroupType::Slopes25,
          SpritePrecision::Sprites4 },                             // TRACK_HEARTLINE_TRANSFER
        {},                                                        // TRACK_MINI_GOLF_HOLE
        { SpriteGroupType::SlopeFlat, SpritePrecision::Sprites4 }, // TRACK_ROTATION_CONTROL_TOGGLE
        { SpriteGroupType::Slopes60, SpritePrecision::Sprites4 },  // TRACK_SLOPE_STEEP_UP
        {},                                                        // TRACK_CORKSCREW_LARGE
        {},                                                        // TRACK_HALF_LOOP_MEDIUM
        {},                                                        // TRACK_ZERO_G_ROLL
        {},                                                        // TRACK_ZERO_G_ROLL_LARGE
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes75, SpritePrecision::Sprites4, SpriteGroupType::Slopes90,
          SpritePrecision::Sprites4 }, // TRACK_FLYING_LARGE_HALF_LOOP_UNINVERTED_UP
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes75, SpritePrecision::Sprites4, SpriteGroupType::Slopes90, SpritePrecision::Sprites4,
          SpriteGroupType::SlopesLoop, SpritePrecision::Sprites4, SpriteGroupType::SlopeInverted,
          SpritePrecision::Sprites4 }, // TRACK_FLYING_LARGE_HALF_LOOP_INVERTED_DOWN
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes75, SpritePrecision::Sprites4, SpriteGroupType::Slopes90, SpritePrecision::Sprites4,
          SpriteGroupType::SlopesLoop, SpritePrecision::Sprites4, SpriteGroupType::SlopeInverted,
          SpritePrecision::Sprites4 }, // TRACK_FLYING_LARGE_HALF_LOOP_UNINVERTED_DOWN
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes75, SpritePrecision::Sprites4, SpriteGroupType::Slopes90,
          SpritePrecision::Sprites4 }, // TRACK_FLYING_LARGE_HALF_LOOP_INVERTED_UP
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes75, SpritePrecision::Sprites4, SpriteGroupType::Slopes90,
          SpritePrecision::Sprites4 }, // TRACK_FLYING_HALF_LOOP_INVERTED_UP
        { SpriteGroupType::Slopes25, SpritePrecision::Sprites4, SpriteGroupType::Slopes60, SpritePrecision::Sprites4,
          SpriteGroupType::Slopes75, SpritePrecision::Sprites4, SpriteGroupType::Slopes90,
          SpritePrecision::Sprites4 }, // TRACK_FLYING_HALF_LOOP_UNINVERTED_DOWN
    };

    // Only check default vehicle; it's assumed the others will have correct sprites if this one does (I've yet to find an
    // exception, at least)
    auto supportedPieces = OpenRCT2::BitSet<TRACK_GROUP_COUNT>();
    supportedPieces.flip();
    auto defaultVehicle = rideEntry->GetDefaultCar();
    if (defaultVehicle != nullptr)
    {
        for (size_t i = 0; i < std::size(trackPieceRequiredSprites); i++)
        {
            for (auto& group : trackPieceRequiredSprites[i])
            {
                auto precision = defaultVehicle->SpriteGroups[static_cast<uint8_t>(group.VehicleSpriteGroup)].spritePrecision;
                if (precision < group.MinPrecision)
                    supportedPieces.set(i, false);
            }
        }
    }
    return supportedPieces;
}

static std::optional<int32_t> ride_get_smallest_station_length(Ride* ride)
{
    std::optional<int32_t> result;
    for (const auto& station : ride->GetStations())
    {
        if (!station.Start.IsNull())
        {
            if (!result.has_value() || station.Length < result.value())
            {
                result = station.Length;
            }
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
    TileElement* tileElement = nullptr;
    track_type_t trackType;
    CoordsXYZ trackStart;
    bool foundTrack = false;

    for (const auto& station : ride->GetStations())
    {
        trackStart = station.GetStart();
        if (trackStart.IsNull())
            continue;

        tileElement = MapGetFirstElementAt(trackStart);
        if (tileElement == nullptr)
            continue;
        do
        {
            if (tileElement->GetType() != TileElementType::Track)
                continue;

            trackType = tileElement->AsTrack()->GetTrackType();
            const auto& ted = GetTrackElementDescriptor(trackType);
            if (!(std::get<0>(ted.SequenceProperties) & TRACK_SEQUENCE_FLAG_ORIGIN))
                continue;

            if (tileElement->GetBaseZ() != trackStart.z)
                continue;

            foundTrack = true;
        } while (!foundTrack && !(tileElement++)->IsLastForTile());

        if (foundTrack)
            break;
    }

    if (!foundTrack)
        return 0;

    RideId rideIndex = tileElement->AsTrack()->GetRideIndex();

    rct_window* w = window_find_by_class(WindowClass::RideConstruction);
    if (w != nullptr && _rideConstructionState != RideConstructionState::State0 && _currentRideIndex == rideIndex)
    {
        ride_construction_invalidate_current_track();
    }

    bool moveSlowIt = true;
    int32_t result = 0;

    track_circuit_iterator it;
    track_circuit_iterator_begin(&it, { trackStart.x, trackStart.y, tileElement });

    track_circuit_iterator slowIt = it;
    while (track_circuit_iterator_next(&it))
    {
        trackType = it.current.element->AsTrack()->GetTrackType();
        const auto& ted = GetTrackElementDescriptor(trackType);
        result += ted.PieceLength;

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

/**
 *
 *  rct2: 0x006DD57D
 */
void Ride::UpdateMaxVehicles()
{
    if (subtype == OBJECT_ENTRY_INDEX_NULL)
        return;

    rct_ride_entry* rideEntry = get_ride_entry(subtype);
    if (rideEntry == nullptr)
    {
        return;
    }
    CarEntry* carEntry;
    uint8_t numCarsPerTrain, numTrains;
    int32_t maxNumTrains;

    if (rideEntry->cars_per_flat_ride == NoFlatRideCars)
    {
        int32_t trainLength;
        num_cars_per_train = std::max(rideEntry->min_cars_in_train, num_cars_per_train);
        MinCarsPerTrain = rideEntry->min_cars_in_train;
        MaxCarsPerTrain = rideEntry->max_cars_in_train;

        // Calculate maximum train length based on smallest station length
        auto stationNumTiles = ride_get_smallest_station_length(this);
        if (!stationNumTiles.has_value())
            return;

        auto stationLength = (stationNumTiles.value() * 0x44180) - 0x16B2A;
        int32_t maxMass = GetRideTypeDescriptor().MaxMass << 8;
        int32_t maxCarsPerTrain = 1;
        for (int32_t numCars = rideEntry->max_cars_in_train; numCars > 0; numCars--)
        {
            trainLength = 0;
            int32_t totalMass = 0;
            for (int32_t i = 0; i < numCars; i++)
            {
                carEntry = &rideEntry->Cars[ride_entry_get_vehicle_at_position(subtype, numCars, i)];
                trainLength += carEntry->spacing;
                totalMass += carEntry->car_mass;
            }

            if (trainLength <= stationLength && totalMass <= maxMass)
            {
                maxCarsPerTrain = numCars;
                break;
            }
        }
        int32_t newCarsPerTrain = std::max(proposed_num_cars_per_train, rideEntry->min_cars_in_train);
        maxCarsPerTrain = std::max(maxCarsPerTrain, static_cast<int32_t>(rideEntry->min_cars_in_train));
        if (!gCheatsDisableTrainLengthLimit)
        {
            newCarsPerTrain = std::min(maxCarsPerTrain, newCarsPerTrain);
        }
        MaxCarsPerTrain = maxCarsPerTrain;
        MinCarsPerTrain = rideEntry->min_cars_in_train;

        switch (mode)
        {
            case RideMode::ContinuousCircuitBlockSectioned:
            case RideMode::PoweredLaunchBlockSectioned:
                maxNumTrains = std::clamp<int32_t>(num_stations + num_block_brakes - 1, 1, OpenRCT2::Limits::MaxTrainsPerRide);
                break;
            case RideMode::ReverseInclineLaunchedShuttle:
            case RideMode::PoweredLaunchPasstrough:
            case RideMode::Shuttle:
            case RideMode::LimPoweredLaunch:
            case RideMode::PoweredLaunch:
                maxNumTrains = 1;
                break;
            default:
                // Calculate maximum number of trains
                trainLength = 0;
                for (int32_t i = 0; i < newCarsPerTrain; i++)
                {
                    carEntry = &rideEntry->Cars[ride_entry_get_vehicle_at_position(subtype, newCarsPerTrain, i)];
                    trainLength += carEntry->spacing;
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

                if ((mode != RideMode::StationToStation && mode != RideMode::ContinuousCircuit)
                    || !(GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_ALLOW_MORE_VEHICLES_THAN_STATION_FITS)))
                {
                    maxNumTrains = std::min(maxNumTrains, int32_t(OpenRCT2::Limits::MaxTrainsPerRide));
                }
                else
                {
                    carEntry = &rideEntry->Cars[ride_entry_get_vehicle_at_position(subtype, newCarsPerTrain, 0)];
                    int32_t poweredMaxSpeed = carEntry->powered_max_speed;

                    int32_t totalSpacing = 0;
                    for (int32_t i = 0; i < newCarsPerTrain; i++)
                    {
                        carEntry = &rideEntry->Cars[ride_entry_get_vehicle_at_position(subtype, newCarsPerTrain, i)];
                        totalSpacing += carEntry->spacing;
                    }

                    totalSpacing >>= 13;
                    int32_t trackLength = ride_get_track_length(this) / 4;
                    if (poweredMaxSpeed > 10)
                        trackLength = (trackLength * 3) / 4;
                    if (poweredMaxSpeed > 25)
                        trackLength = (trackLength * 3) / 4;
                    if (poweredMaxSpeed > 40)
                        trackLength = (trackLength * 3) / 4;

                    maxNumTrains = 0;
                    int32_t length = 0;
                    do
                    {
                        maxNumTrains++;
                        length += totalSpacing;
                    } while (maxNumTrains < OpenRCT2::Limits::MaxTrainsPerRide && length < trackLength);
                }
                break;
        }
        max_trains = maxNumTrains;

        numCarsPerTrain = std::min(proposed_num_cars_per_train, static_cast<uint8_t>(newCarsPerTrain));
    }
    else
    {
        max_trains = rideEntry->cars_per_flat_ride;
        MinCarsPerTrain = rideEntry->min_cars_in_train;
        MaxCarsPerTrain = rideEntry->max_cars_in_train;
        numCarsPerTrain = rideEntry->max_cars_in_train;
        maxNumTrains = rideEntry->cars_per_flat_ride;
    }

    if (gCheatsDisableTrainLengthLimit)
    {
        maxNumTrains = OpenRCT2::Limits::MaxTrainsPerRide;
    }
    numTrains = std::min(ProposedNumTrains, static_cast<uint8_t>(maxNumTrains));

    // Refresh new current num vehicles / num cars per vehicle
    if (numTrains != NumTrains || numCarsPerTrain != num_cars_per_train)
    {
        num_cars_per_train = numCarsPerTrain;
        NumTrains = numTrains;
        window_invalidate_by_number(WindowClass::Ride, id.ToUnderlying());
    }
}

void Ride::UpdateNumberOfCircuits()
{
    if (!CanHaveMultipleCircuits())
    {
        num_circuits = 1;
    }
}

void Ride::SetRideEntry(ObjectEntryIndex entryIndex)
{
    auto colour = ride_get_unused_preset_vehicle_colour(entryIndex);
    auto rideSetVehicleAction = RideSetVehicleAction(id, RideSetVehicleType::RideEntry, entryIndex, colour);
    GameActions::Execute(&rideSetVehicleAction);
}

void Ride::SetNumTrains(int32_t numTrains)
{
    auto rideSetVehicleAction = RideSetVehicleAction(id, RideSetVehicleType::NumTrains, numTrains);
    GameActions::Execute(&rideSetVehicleAction);
}

void Ride::SetNumCarsPerVehicle(int32_t numCarsPerVehicle)
{
    auto rideSetVehicleAction = RideSetVehicleAction(id, RideSetVehicleType::NumCarsPerTrain, numCarsPerVehicle);
    GameActions::Execute(&rideSetVehicleAction);
}

void Ride::SetToDefaultInspectionInterval()
{
    uint8_t defaultInspectionInterval = gConfigGeneral.DefaultInspectionInterval;
    if (inspection_interval != defaultInspectionInterval)
    {
        if (defaultInspectionInterval <= RIDE_INSPECTION_NEVER)
        {
            set_operating_setting(id, RideSetSetting::InspectionInterval, defaultInspectionInterval);
        }
    }
}

/**
 *
 *  rct2: 0x006B752C
 */
void Ride::Crash(uint8_t vehicleIndex)
{
    Vehicle* vehicle = GetEntity<Vehicle>(vehicles[vehicleIndex]);

    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && vehicle != nullptr)
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

    if (gConfigNotifications.RideCrashed)
    {
        Formatter ft;
        FormatNameTo(ft);
        News::AddItemToQueue(News::ItemType::Ride, STR_RIDE_HAS_CRASHED, id.ToUnderlying(), ft);
    }
}

void ride_reset_all_names()
{
    for (auto& ride : GetRideManager())
    {
        ride.SetNameToDefault();
    }
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

    for (int32_t i = 0; i < OpenRCT2::Limits::CustomerHistorySize; i++)
    {
        sum += ride->num_customers[i];
    }

    return sum;
}

Vehicle* ride_get_broken_vehicle(const Ride* ride)
{
    auto vehicleIndex = ride->vehicles[ride->broken_vehicle];
    Vehicle* vehicle = GetEntity<Vehicle>(vehicleIndex);
    if (vehicle != nullptr)
    {
        return vehicle->GetCar(ride->broken_car);
    }
    return nullptr;
}

/**
 *
 *  rct2: 0x006D235B
 */
void Ride::Delete()
{
    custom_name = {};
    measurement = {};
    type = RIDE_TYPE_NULL;
}

void Ride::Renew()
{
    // Set build date to current date (so the ride is brand new)
    build_date = gDateMonthsElapsed;
    reliability = RIDE_INITIAL_RELIABILITY;
}

RideClassification Ride::GetClassification() const
{
    const auto& rtd = GetRideTypeDescriptor();
    return rtd.Classification;
}

bool Ride::IsRide() const
{
    return GetClassification() == RideClassification::Ride;
}

money16 ride_get_price(const Ride* ride)
{
    if (gParkFlags & PARK_FLAGS_NO_MONEY)
        return 0;
    if (ride->IsRide())
    {
        if (!ParkRidePricesUnlocked())
        {
            return 0;
        }
    }
    return ride->price[0];
}

/**
 * Return the tile_element of an adjacent station at x,y,z(+-2).
 * Returns nullptr if no suitable tile_element is found.
 */
TileElement* get_station_platform(const CoordsXYRangedZ& coords)
{
    bool foundTileElement = false;
    TileElement* tileElement = MapGetFirstElementAt(coords);
    if (tileElement != nullptr)
    {
        do
        {
            if (tileElement->GetType() != TileElementType::Track)
                continue;
            /* Check if tileElement is a station platform. */
            if (!tileElement->AsTrack()->IsStation())
                continue;

            if (coords.baseZ > tileElement->GetBaseZ() || coords.clearanceZ < tileElement->GetBaseZ())
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
static bool check_for_adjacent_station(const CoordsXYZ& stationCoords, uint8_t direction)
{
    bool found = false;
    int32_t adjX = stationCoords.x;
    int32_t adjY = stationCoords.y;
    for (uint32_t i = 0; i <= RIDE_ADJACENCY_CHECK_DISTANCE; i++)
    {
        adjX += CoordsDirectionDelta[direction].x;
        adjY += CoordsDirectionDelta[direction].y;
        TileElement* stationElement = get_station_platform(
            { { adjX, adjY, stationCoords.z }, stationCoords.z + 2 * COORDS_Z_STEP });
        if (stationElement != nullptr)
        {
            auto rideIndex = stationElement->AsTrack()->GetRideIndex();
            auto ride = get_ride(rideIndex);
            if (ride != nullptr && (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS))
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
    for (const auto& station : ride->GetStations())
    {
        auto stationStart = station.GetStart();
        if (!stationStart.IsNull())
        {
            /* Get the map element for the station start. */
            TileElement* stationElement = get_station_platform({ stationStart, stationStart.z + 0 });
            if (stationElement == nullptr)
            {
                continue;
            }
            /* Check the first side of the station */
            int32_t direction = stationElement->GetDirectionWithOffset(1);
            found = check_for_adjacent_station(stationStart, direction);
            if (found)
                break;
            /* Check the other side of the station */
            direction = DirectionReverse(direction);
            found = check_for_adjacent_station(stationStart, direction);
            if (found)
                break;
        }
    }
    return found;
}

bool ride_has_station_shelter(Ride* ride)
{
    const auto* stationObj = ride->GetStationObject();
    return stationObj != nullptr && (stationObj->Flags & STATION_OBJECT_FLAGS::HAS_SHELTER);
}

bool ride_has_ratings(const Ride* ride)
{
    return ride->excitement != RIDE_RATING_UNDEFINED;
}

int32_t get_booster_speed(ride_type_t rideType, int32_t rawSpeed)
{
    int8_t shiftFactor = GetRideTypeDescriptor(rideType).OperatingSettings.BoosterSpeedFactor;
    if (shiftFactor == 0)
    {
        return rawSpeed;
    }
    if (shiftFactor > 0)
    {
        return (rawSpeed << shiftFactor);
    }

    // Workaround for an issue with older compilers (GCC 6, Clang 4) which would fail the build
    int8_t shiftFactorAbs = std::abs(shiftFactor);
    return (rawSpeed >> shiftFactorAbs);
}

void fix_invalid_vehicle_sprite_sizes()
{
    for (const auto& ride : GetRideManager())
    {
        for (auto entityIndex : ride.vehicles)
        {
            for (Vehicle* vehicle = TryGetEntity<Vehicle>(entityIndex); vehicle != nullptr;
                 vehicle = TryGetEntity<Vehicle>(vehicle->next_vehicle_on_train))
            {
                auto carEntry = vehicle->Entry();
                if (carEntry == nullptr)
                {
                    break;
                }

                if (vehicle->sprite_width == 0)
                {
                    vehicle->sprite_width = carEntry->sprite_width;
                }
                if (vehicle->sprite_height_negative == 0)
                {
                    vehicle->sprite_height_negative = carEntry->sprite_height_negative;
                }
                if (vehicle->sprite_height_positive == 0)
                {
                    vehicle->sprite_height_positive = carEntry->sprite_height_positive;
                }
            }
        }
    }
}

bool ride_entry_has_category(const rct_ride_entry* rideEntry, uint8_t category)
{
    auto rideType = rideEntry->GetFirstNonNullRideType();
    return GetRideTypeDescriptor(rideType).Category == category;
}

int32_t ride_get_entry_index(int32_t rideType, int32_t rideSubType)
{
    int32_t subType = rideSubType;

    if (subType == OBJECT_ENTRY_INDEX_NULL)
    {
        auto& objManager = GetContext()->GetObjectManager();
        auto& rideEntries = objManager.GetAllRideEntries(rideType);
        if (rideEntries.size() > 0)
        {
            subType = rideEntries[0];
            for (auto rideEntryIndex : rideEntries)
            {
                auto rideEntry = get_ride_entry(rideEntryIndex);
                if (rideEntry == nullptr)
                {
                    return OBJECT_ENTRY_INDEX_NULL;
                }

                // Can happen in select-by-track-type mode
                if (!ride_entry_is_invented(rideEntryIndex) && !gCheatsIgnoreResearchStatus)
                {
                    continue;
                }

                if (!GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
                {
                    subType = rideEntryIndex;
                    break;
                }
            }
        }
    }

    return subType;
}

const StationObject* Ride::GetStationObject() const
{
    auto& objManager = GetContext()->GetObjectManager();
    return static_cast<StationObject*>(objManager.GetLoadedObject(ObjectType::Station, entrance_style));
}

const MusicObject* Ride::GetMusicObject() const
{
    auto& objManager = GetContext()->GetObjectManager();
    return static_cast<MusicObject*>(objManager.GetLoadedObject(ObjectType::Music, music));
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

    for (auto& ride : GetRideManager())
    {
        for (auto& station : ride.GetStations())
        {
            auto stationIndex = ride.GetStationIndex(&station);
            TileCoordsXYZD entranceLoc = station.Entrance;
            TileCoordsXYZD exitLoc = station.Exit;
            bool fixEntrance = false;
            bool fixExit = false;

            // Skip if the station has no entrance
            if (!entranceLoc.IsNull())
            {
                const EntranceElement* entranceElement = MapGetRideEntranceElementAt(entranceLoc.ToCoordsXYZD(), false);

                if (entranceElement == nullptr || entranceElement->GetRideIndex() != ride.id
                    || entranceElement->GetStationIndex() != stationIndex)
                {
                    fixEntrance = true;
                }
                else
                {
                    station.Entrance.direction = static_cast<uint8_t>(entranceElement->GetDirection());
                }
            }

            if (!exitLoc.IsNull())
            {
                const EntranceElement* entranceElement = MapGetRideExitElementAt(exitLoc.ToCoordsXYZD(), false);

                if (entranceElement == nullptr || entranceElement->GetRideIndex() != ride.id
                    || entranceElement->GetStationIndex() != stationIndex)
                {
                    fixExit = true;
                }
                else
                {
                    station.Exit.direction = static_cast<uint8_t>(entranceElement->GetDirection());
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
            for (int32_t y = 1; y < gMapSize.y - 1; y++)
            {
                for (int32_t x = 1; x < gMapSize.x - 1; x++)
                {
                    TileElement* tileElement = MapGetFirstElementAt(TileCoordsXY{ x, y });

                    if (tileElement != nullptr)
                    {
                        do
                        {
                            if (tileElement->GetType() != TileElementType::Entrance)
                            {
                                continue;
                            }
                            const EntranceElement* entranceElement = tileElement->AsEntrance();
                            if (entranceElement->GetRideIndex() != ride.id)
                            {
                                continue;
                            }
                            if (entranceElement->GetStationIndex() != stationIndex)
                            {
                                continue;
                            }

                            // The expected height is where entrances and exit reside in non-hacked parks.
                            const uint8_t expectedHeight = station.Height;

                            if (fixEntrance && entranceElement->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
                            {
                                if (alreadyFoundEntrance)
                                {
                                    if (station.Entrance.z == expectedHeight)
                                        continue;
                                    if (station.Entrance.z > entranceElement->base_height)
                                        continue;
                                }

                                // Found our entrance
                                station.Entrance = { x, y, entranceElement->base_height,
                                                     static_cast<uint8_t>(entranceElement->GetDirection()) };
                                alreadyFoundEntrance = true;

                                log_verbose(
                                    "Fixed disconnected entrance of ride %d, station %d to x = %d, y = %d and z = %d.", ride.id,
                                    stationIndex, x, y, entranceElement->base_height);
                            }
                            else if (fixExit && entranceElement->GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT)
                            {
                                if (alreadyFoundExit)
                                {
                                    if (station.Exit.z == expectedHeight)
                                        continue;
                                    if (station.Exit.z > entranceElement->base_height)
                                        continue;
                                }

                                // Found our exit
                                station.Exit = { x, y, entranceElement->base_height,
                                                 static_cast<uint8_t>(entranceElement->GetDirection()) };
                                alreadyFoundExit = true;

                                log_verbose(
                                    "Fixed disconnected exit of ride %d, station %d to x = %d, y = %d and z = %d.", ride.id,
                                    stationIndex, x, y, entranceElement->base_height);
                            }
                        } while (!(tileElement++)->IsLastForTile());
                    }
                }
            }

            if (fixEntrance && !alreadyFoundEntrance)
            {
                station.Entrance.SetNull();
                log_verbose("Cleared disconnected entrance of ride %d, station %d.", ride.id, stationIndex);
            }
            if (fixExit && !alreadyFoundExit)
            {
                station.Exit.SetNull();
                log_verbose("Cleared disconnected exit of ride %d, station %d.", ride.id, stationIndex);
            }
        }
    }
}

void ride_clear_leftover_entrances(Ride* ride)
{
    for (TileCoordsXY tilePos = {}; tilePos.x < gMapSize.x; ++tilePos.x)
    {
        for (tilePos.y = 0; tilePos.y < gMapSize.y; ++tilePos.y)
        {
            for (auto* entrance : TileElementsView<EntranceElement>(tilePos.ToCoordsXY()))
            {
                const bool isRideEntranceExit = entrance->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE
                    || entrance->GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT;
                if (!isRideEntranceExit)
                    continue;
                if (entrance->GetRideIndex() != ride->id)
                    continue;

                TileElementRemove(entrance->as<TileElement>());
            }
        }
    }
}

std::string Ride::GetName() const
{
    Formatter ft;
    FormatNameTo(ft);
    return format_string(STR_STRINGID, ft.Data());
}

void Ride::FormatNameTo(Formatter& ft) const
{
    if (!custom_name.empty())
    {
        auto str = custom_name.c_str();
        ft.Add<StringId>(STR_STRING);
        ft.Add<const char*>(str);
    }
    else
    {
        auto rideTypeName = GetRideTypeDescriptor().Naming.Name;
        if (GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
        {
            auto rideEntry = GetRideEntry();
            if (rideEntry != nullptr)
            {
                rideTypeName = rideEntry->naming.Name;
            }
        }
        ft.Add<StringId>(1).Add<StringId>(rideTypeName).Add<uint16_t>(default_name_number);
    }
}

uint64_t Ride::GetAvailableModes() const
{
    if (gCheatsShowAllOperatingModes)
        return AllRideModesAvailable;

    return GetRideTypeDescriptor().RideModes;
}

const RideTypeDescriptor& Ride::GetRideTypeDescriptor() const
{
    return ::GetRideTypeDescriptor(type);
}

uint8_t Ride::GetNumShelteredSections() const
{
    return num_sheltered_sections & ShelteredSectionsBits::NumShelteredSectionsMask;
}

void Ride::IncreaseNumShelteredSections()
{
    auto newNumShelteredSections = GetNumShelteredSections();
    if (newNumShelteredSections != 0x1F)
        newNumShelteredSections++;
    num_sheltered_sections &= ~ShelteredSectionsBits::NumShelteredSectionsMask;
    num_sheltered_sections |= newNumShelteredSections;
}

void Ride::UpdateRideTypeForAllPieces()
{
    for (int32_t y = 0; y < gMapSize.y; y++)
    {
        for (int32_t x = 0; x < gMapSize.x; x++)
        {
            auto* tileElement = MapGetFirstElementAt(TileCoordsXY(x, y));
            if (tileElement == nullptr)
                continue;

            do
            {
                if (tileElement->GetType() != TileElementType::Track)
                    continue;

                auto* trackElement = tileElement->AsTrack();
                if (trackElement->GetRideIndex() != id)
                    continue;

                trackElement->SetRideType(type);

            } while (!(tileElement++)->IsLastForTile());
        }
    }
}

bool Ride::HasLifecycleFlag(uint32_t flag) const
{
    return (lifecycle_flags & flag) != 0;
}

void Ride::SetLifecycleFlag(uint32_t flag, bool on)
{
    if (on)
        lifecycle_flags |= flag;
    else
        lifecycle_flags &= ~flag;
}

bool Ride::HasRecolourableShopItems() const
{
    const auto rideEntry = GetRideEntry();
    if (rideEntry == nullptr)
        return false;

    for (size_t itemIndex = 0; itemIndex < std::size(rideEntry->shop_item); itemIndex++)
    {
        const ShopItem currentItem = rideEntry->shop_item[itemIndex];
        if (currentItem != ShopItem::None && GetShopItemDescriptor(currentItem).IsRecolourable())
        {
            return true;
        }
    }
    return false;
}

bool Ride::HasStation() const
{
    return num_stations != 0;
}

std::vector<RideId> GetTracklessRides()
{
    // Iterate map and build list of seen ride IDs
    std::vector<bool> seen;
    seen.resize(256);
    tile_element_iterator it;
    TileElementIteratorBegin(&it);
    while (TileElementIteratorNext(&it))
    {
        auto trackEl = it.element->AsTrack();
        if (trackEl != nullptr && !trackEl->IsGhost())
        {
            auto rideId = trackEl->GetRideIndex().ToUnderlying();
            if (rideId >= seen.size())
            {
                seen.resize(rideId + 1);
            }
            seen[rideId] = true;
        }
    }

    // Get all rides that did not get seen during map iteration
    const auto& rideManager = GetRideManager();
    std::vector<RideId> result;
    for (const auto& ride : rideManager)
    {
        const auto rideIndex = ride.id.ToUnderlying();
        if (seen.size() <= rideIndex || !seen[rideIndex])
        {
            result.push_back(ride.id);
        }
    }
    return result;
}
