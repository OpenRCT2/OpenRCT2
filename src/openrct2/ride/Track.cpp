/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Track.h"

#include "../Cheats.h"
#include "../Game.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../interface/Viewport.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../platform/Platform.h"
#include "../rct1/RCT1.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"
#include "Ride.h"
#include "RideData.h"
#include "RideRatings.h"
#include "Station.h"
#include "TrackData.h"
#include "TrackDesign.h"

using namespace OpenRCT2::TrackMetaData;

PitchAndRoll TrackPitchAndRollStart(track_type_t trackType)
{
    const auto& ted = GetTrackElementDescriptor(trackType);
    return { ted.Definition.vangle_start, ted.Definition.bank_start };
}

PitchAndRoll TrackPitchAndRollEnd(track_type_t trackType)
{
    const auto& ted = GetTrackElementDescriptor(trackType);
    return { ted.Definition.vangle_end, ted.Definition.bank_end };
}

/**
 * Helper method to determine if a connects to b by its bank and angle, not location.
 */
int32_t track_is_connected_by_shape(TileElement* a, TileElement* b)
{
    int32_t trackType, aBank, aAngle, bBank, bAngle;

    trackType = a->AsTrack()->GetTrackType();
    const auto* ted = &GetTrackElementDescriptor(trackType);
    aBank = ted->Definition.bank_end;
    aAngle = ted->Definition.vangle_end;
    aBank = track_get_actual_bank(a, aBank);

    trackType = b->AsTrack()->GetTrackType();
    ted = &GetTrackElementDescriptor(trackType);
    bBank = ted->Definition.bank_start;
    bAngle = ted->Definition.vangle_start;
    bBank = track_get_actual_bank(b, bBank);

    return aBank == bBank && aAngle == bAngle;
}

static TileElement* find_station_element(const CoordsXYZD& loc, RideId rideIndex)
{
    TileElement* tileElement = map_get_first_element_at(loc);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (loc.z != tileElement->GetBaseZ())
            continue;
        if (tileElement->GetType() != TileElementType::Track)
            continue;
        if (tileElement->GetDirection() != loc.direction)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;
        if (!tileElement->AsTrack()->IsStation())
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());
    return nullptr;
}

static void ride_remove_station(Ride* ride, const CoordsXYZ& location)
{
    for (auto& station : ride->GetStations())
    {
        auto stationStart = station.GetStart();
        if (stationStart == location)
        {
            station.Start.SetNull();
            ride->num_stations--;
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006C4D89
 */
bool track_add_station_element(CoordsXYZD loc, RideId rideIndex, int32_t flags, bool fromTrackDesign)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return false;

    CoordsXY stationBackLoc = loc;
    CoordsXY stationFrontLoc = loc;
    int32_t stationLength = 1;

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION))
    {
        if (ride->num_stations >= OpenRCT2::Limits::MaxStationsPerRide)
        {
            gGameCommandErrorText = STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE;
            return false;
        }
        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            auto stationIndex = ride_get_first_empty_station_start(ride);
            assert(!stationIndex.IsNull());

            auto& station = ride->GetStation(stationIndex);
            station.Start.x = loc.x;
            station.Start.y = loc.y;
            station.Height = loc.z / COORDS_Z_STEP;
            station.Depart = 1;
            station.Length = 0;
            ride->num_stations++;
        }
        return true;
    }

    TileElement* stationElement;

    // Search backwards for more station
    loc = { stationBackLoc, loc.z, loc.direction };
    do
    {
        loc -= CoordsDirectionDelta[loc.direction];

        stationElement = find_station_element(loc, rideIndex);
        if (stationElement != nullptr)
        {
            if (stationElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
            {
                if (flags & GAME_COMMAND_FLAG_APPLY)
                {
                    ride_remove_station(ride, loc);
                }
            }

            stationBackLoc = loc;
            stationLength++;
        }
    } while (stationElement != nullptr);

    // Search forwards for more station
    loc = { stationFrontLoc, loc.z, loc.direction };
    do
    {
        loc += CoordsDirectionDelta[loc.direction];

        stationElement = find_station_element(loc, rideIndex);
        if (stationElement != nullptr)
        {
            if (stationElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
            {
                if (flags & GAME_COMMAND_FLAG_APPLY)
                {
                    ride_remove_station(ride, loc);
                }
            }

            stationFrontLoc = loc;
            stationLength++;
        }
    } while (stationElement != nullptr);

    // When attempting to place a track design, it sometimes happens that the front and back of station 0 are built,
    // but the middle is not. Allow this, so the track place function can actually finish building all 4 stations.
    // This _might_ cause issues if the track designs is bugged and actually has 5.
    if (stationBackLoc == stationFrontLoc && ride->num_stations >= OpenRCT2::Limits::MaxStationsPerRide && !fromTrackDesign)
    {
        gGameCommandErrorText = STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE;
        return false;
    }

    if (stationLength > MAX_STATION_PLATFORM_LENGTH)
    {
        gGameCommandErrorText = STR_STATION_PLATFORM_TOO_LONG;
        return false;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        loc = { stationFrontLoc, loc.z, loc.direction };

        bool finaliseStationDone;
        do
        {
            finaliseStationDone = true;

            stationElement = find_station_element(loc, rideIndex);
            if (stationElement != nullptr)
            {
                track_type_t targetTrackType;
                if (stationFrontLoc == loc)
                {
                    auto stationIndex = ride_get_first_empty_station_start(ride);
                    if (stationIndex.IsNull())
                    {
                        log_verbose("No empty station starts, not updating metadata! This can happen with hacked rides.");
                    }
                    else
                    {
                        auto& station = ride->GetStation(stationIndex);
                        station.Start = loc;
                        station.Height = loc.z / COORDS_Z_STEP;
                        station.Depart = 1;
                        station.Length = stationLength;
                        ride->num_stations++;
                    }

                    targetTrackType = TrackElemType::EndStation;
                }
                else if (stationBackLoc == loc)
                {
                    targetTrackType = TrackElemType::BeginStation;
                }
                else
                {
                    targetTrackType = TrackElemType::MiddleStation;
                }
                stationElement->AsTrack()->SetTrackType(targetTrackType);

                map_invalidate_element(loc, stationElement);

                if (stationBackLoc != loc)
                {
                    loc -= CoordsDirectionDelta[loc.direction];
                    finaliseStationDone = false;
                }
            }
        } while (!finaliseStationDone);
    }
    return true;
}

/**
 *
 *  rct2: 0x006C494B
 */
bool track_remove_station_element(const CoordsXYZD& loc, RideId rideIndex, int32_t flags)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return false;

    CoordsXYZD removeLoc = loc;
    CoordsXYZD stationBackLoc = loc;
    CoordsXYZD stationFrontLoc = loc;
    int32_t stationLength = 0;
    int32_t byte_F441D1 = -1;

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION))
    {
        TileElement* tileElement = map_get_track_element_at_with_direction_from_ride(loc, rideIndex);
        if (tileElement != nullptr)
        {
            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride_remove_station(ride, loc);
            }
        }
        return true;
    }

    TileElement* stationElement;

    // Search backwards for more station
    CoordsXYZD currentLoc = stationBackLoc;
    while ((stationElement = find_station_element(currentLoc, rideIndex)) != nullptr)
    {
        if (stationElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
        {
            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride_remove_station(ride, currentLoc);
            }
        }

        stationBackLoc = currentLoc;
        byte_F441D1++;

        currentLoc -= CoordsDirectionDelta[currentLoc.direction];
    }

    // Search forwards for more station
    currentLoc = stationFrontLoc;
    do
    {
        currentLoc += CoordsDirectionDelta[currentLoc.direction];

        stationElement = find_station_element(currentLoc, rideIndex);
        if (stationElement != nullptr)
        {
            if (stationElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
            {
                if (flags & GAME_COMMAND_FLAG_APPLY)
                {
                    ride_remove_station(ride, currentLoc);
                }
            }
            stationFrontLoc = currentLoc;
            stationLength++;
        }
    } while (stationElement != nullptr);

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
    {
        if ((removeLoc != stationBackLoc) && (removeLoc != stationFrontLoc)
            && ride->num_stations >= OpenRCT2::Limits::MaxStationsPerRide)
        {
            gGameCommandErrorText = STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE;
            return false;
        }

        return true;
    }

    currentLoc = stationFrontLoc;
    bool finaliseStationDone;
    do
    {
        finaliseStationDone = true;

        if (currentLoc != removeLoc)
        {
            stationElement = find_station_element(currentLoc, rideIndex);
            if (stationElement != nullptr)
            {
                track_type_t targetTrackType;
                if ((currentLoc == stationFrontLoc) || (currentLoc + CoordsDirectionDelta[currentLoc.direction] == removeLoc))
                {
                    auto stationIndex = ride_get_first_empty_station_start(ride);
                    if (stationIndex.IsNull())
                    {
                        log_verbose("No empty station starts, not updating metadata! This can happen with hacked rides.");
                    }
                    else
                    {
                        auto& station = ride->GetStation(stationIndex);
                        station.Start = currentLoc;
                        station.Height = currentLoc.z / COORDS_Z_STEP;
                        station.Depart = 1;
                        station.Length = stationLength != 0 ? stationLength : byte_F441D1;
                        ride->num_stations++;
                    }

                    stationLength = 0;
                    targetTrackType = TrackElemType::EndStation;
                }
                else
                {
                    if (currentLoc - CoordsDirectionDelta[currentLoc.direction] == removeLoc)
                    {
                        targetTrackType = TrackElemType::BeginStation;
                    }
                    else
                    {
                        if (currentLoc == stationBackLoc)
                        {
                            targetTrackType = TrackElemType::BeginStation;
                        }
                        else
                        {
                            targetTrackType = TrackElemType::MiddleStation;
                        }
                    }
                }
                stationElement->AsTrack()->SetTrackType(targetTrackType);

                map_invalidate_element(currentLoc, stationElement);
            }
        }

        if (currentLoc != stationBackLoc)
        {
            currentLoc -= CoordsDirectionDelta[currentLoc.direction];
            finaliseStationDone = false;
        }
    } while (!finaliseStationDone);

    return true;
}

void track_circuit_iterator_begin(track_circuit_iterator* it, CoordsXYE first)
{
    it->last = first;
    it->first = nullptr;
    it->firstIteration = true;
    it->looped = false;
}

bool track_circuit_iterator_previous(track_circuit_iterator* it)
{
    track_begin_end trackBeginEnd;

    if (it->first == nullptr)
    {
        if (!track_block_get_previous({ it->last.x, it->last.y, it->last.element }, &trackBeginEnd))
            return false;

        it->current.x = trackBeginEnd.begin_x;
        it->current.y = trackBeginEnd.begin_y;
        it->current.element = trackBeginEnd.begin_element;
        it->currentZ = trackBeginEnd.begin_z;
        it->currentDirection = trackBeginEnd.begin_direction;

        it->first = it->current.element;
        return true;
    }

    if (!it->firstIteration && it->first == it->current.element)
    {
        it->looped = true;
        return false;
    }

    it->firstIteration = false;
    it->last = it->current;

    if (track_block_get_previous({ it->last.x, it->last.y, it->last.element }, &trackBeginEnd))
    {
        it->current.x = trackBeginEnd.end_x;
        it->current.y = trackBeginEnd.end_y;
        it->current.element = trackBeginEnd.begin_element;
        it->currentZ = trackBeginEnd.begin_z;
        it->currentDirection = trackBeginEnd.begin_direction;
        return true;
    }

    return false;
}

bool track_circuit_iterator_next(track_circuit_iterator* it)
{
    if (it->first == nullptr)
    {
        if (!track_block_get_next(&it->last, &it->current, &it->currentZ, &it->currentDirection))
            return false;

        it->first = it->current.element;
        return true;
    }

    if (!it->firstIteration && it->first == it->current.element)
    {
        it->looped = true;
        return false;
    }

    it->firstIteration = false;
    it->last = it->current;
    return track_block_get_next(&it->last, &it->current, &it->currentZ, &it->currentDirection);
}

bool track_circuit_iterators_match(const track_circuit_iterator* firstIt, const track_circuit_iterator* secondIt)
{
    return (
        firstIt->currentZ == secondIt->currentZ && firstIt->currentDirection == secondIt->currentDirection
        && firstIt->current.x == secondIt->current.x && firstIt->current.y == secondIt->current.y);
}

void track_get_back(CoordsXYE* input, CoordsXYE* output)
{
    CoordsXYE lastTrack;
    track_begin_end currentTrack;
    bool result;

    lastTrack = *input;
    do
    {
        result = track_block_get_previous(lastTrack, &currentTrack);
        if (result)
        {
            lastTrack.x = currentTrack.begin_x;
            lastTrack.y = currentTrack.begin_y;
            lastTrack.element = currentTrack.begin_element;
        }
    } while (result);
    *output = lastTrack;
}

void track_get_front(CoordsXYE* input, CoordsXYE* output)
{
    CoordsXYE lastTrack, currentTrack;
    int32_t z, direction;
    bool result;

    lastTrack = *input;
    do
    {
        result = track_block_get_next(&lastTrack, &currentTrack, &z, &direction);
        if (result)
        {
            lastTrack = currentTrack;
        }
    } while (result);
    *output = lastTrack;
}

bool TrackElement::HasChain() const
{
    return Flags2 & TRACK_ELEMENT_FLAGS2_CHAIN_LIFT;
}

void TrackElement::SetHasChain(bool on)
{
    if (on)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_CHAIN_LIFT;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_CHAIN_LIFT;
    }
}

/**
 * Checks if a track element is recognised as the beginning of a block.
 * A beginning of a block can be the end of a station, the end of a lift hill,
 * or a block brake.
 */
bool TrackElement::IsBlockStart() const
{
    switch (GetTrackType())
    {
        case TrackElemType::EndStation:
        case TrackElemType::CableLiftHill:
        case TrackElemType::BlockBrakes:
            return true;
        case TrackElemType::Up25ToFlat:
        case TrackElemType::Up60ToFlat:
        case TrackElemType::DiagUp25ToFlat:
        case TrackElemType::DiagUp60ToFlat:
            if (HasChain())
            {
                return true;
            }
            break;
    }
    return false;
}

roll_type_t track_get_actual_bank(TileElement* tileElement, roll_type_t bank)
{
    auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
    if (ride != nullptr)
    {
        bool isInverted = tileElement->AsTrack()->IsInverted();
        return track_get_actual_bank_2(ride->type, isInverted, bank);
    }
    return bank;
}

roll_type_t track_get_actual_bank_2(int32_t rideType, bool isInverted, roll_type_t bank)
{
    if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE))
    {
        if (isInverted)
        {
            if (bank == TRACK_BANK_NONE)
            {
                bank = TRACK_BANK_UPSIDE_DOWN;
            }
            else if (bank == TRACK_BANK_UPSIDE_DOWN)
            {
                bank = TRACK_BANK_NONE;
            }
        }
    }
    return bank;
}

roll_type_t track_get_actual_bank_3(bool useInvertedSprites, TileElement* tileElement)
{
    auto trackType = tileElement->AsTrack()->GetTrackType();
    const auto& ted = GetTrackElementDescriptor(trackType);
    auto bankStart = ted.Definition.bank_start;
    auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr)
        return bankStart;

    bool isInverted = useInvertedSprites ^ tileElement->AsTrack()->IsInverted();
    return track_get_actual_bank_2(ride->type, isInverted, bankStart);
}

bool TrackElement::IsStation() const
{
    return track_type_is_station(GetTrackType());
}

bool track_type_is_station(track_type_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return true;
        default:
            return false;
    }
}

bool track_element_is_covered(track_type_t trackElementType)
{
    switch (trackElementType)
    {
        case TrackElemType::FlatCovered:
        case TrackElemType::Up25Covered:
        case TrackElemType::Up60Covered:
        case TrackElemType::FlatToUp25Covered:
        case TrackElemType::Up25ToUp60Covered:
        case TrackElemType::Up60ToUp25Covered:
        case TrackElemType::Up25ToFlatCovered:
        case TrackElemType::Down25Covered:
        case TrackElemType::Down60Covered:
        case TrackElemType::FlatToDown25Covered:
        case TrackElemType::Down25ToDown60Covered:
        case TrackElemType::Down60ToDown25Covered:
        case TrackElemType::Down25ToFlatCovered:
        case TrackElemType::LeftQuarterTurn5TilesCovered:
        case TrackElemType::RightQuarterTurn5TilesCovered:
        case TrackElemType::SBendLeftCovered:
        case TrackElemType::SBendRightCovered:
        case TrackElemType::LeftQuarterTurn3TilesCovered:
        case TrackElemType::RightQuarterTurn3TilesCovered:
            return true;
        default:
            return false;
    }
}

bool TrackTypeHasSpeedSetting(track_type_t trackType)
{
    // This does not check if the element is really a Spinning Control track instead of a booster,
    // but this does not cause problems.
    return trackType == TrackElemType::Brakes || trackType == TrackElemType::Booster;
}

uint8_t TrackElement::GetSeatRotation() const
{
    const auto* ride = get_ride(GetRideIndex());
    if (ride != nullptr && ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_LANDSCAPE_DOORS))
        return DEFAULT_SEAT_ROTATION;

    return URide.ColourScheme >> 4;
}

void TrackElement::SetSeatRotation(uint8_t newSeatRotation)
{
    URide.ColourScheme &= ~TRACK_ELEMENT_COLOUR_SEAT_ROTATION_MASK;
    URide.ColourScheme |= (newSeatRotation << 4);
}

bool TrackElement::IsTakingPhoto() const
{
    return URide.OnridePhotoBits != 0;
}

void TrackElement::SetPhotoTimeout()
{
    URide.OnridePhotoBits = 3;
}

void TrackElement::SetPhotoTimeout(uint8_t value)
{
    URide.OnridePhotoBits = value;
}

uint8_t TrackElement::GetPhotoTimeout() const
{
    return URide.OnridePhotoBits;
}

void TrackElement::DecrementPhotoTimeout()
{
    URide.OnridePhotoBits = std::max(0, URide.OnridePhotoBits - 1);
}

uint16_t TrackElement::GetMazeEntry() const
{
    return UMaze.MazeEntry;
}

void TrackElement::SetMazeEntry(uint16_t newMazeEntry)
{
    UMaze.MazeEntry = newMazeEntry;
}

void TrackElement::MazeEntryAdd(uint16_t addVal)
{
    UMaze.MazeEntry |= addVal;
}

void TrackElement::MazeEntrySubtract(uint16_t subVal)
{
    UMaze.MazeEntry &= ~subVal;
}

track_type_t TrackElement::GetTrackType() const
{
    return TrackType;
}

void TrackElement::SetTrackType(uint16_t newType)
{
    TrackType = newType;
}

ride_type_t TrackElement::GetRideType() const
{
    return RideType;
}

void TrackElement::SetRideType(const ride_type_t rideType)
{
    RideType = rideType;
}

uint8_t TrackElement::GetSequenceIndex() const
{
    return URide.Sequence;
}

void TrackElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    URide.Sequence = newSequenceIndex;
}

StationIndex TrackElement::GetStationIndex() const
{
    return URide.stationIndex;
}

void TrackElement::SetStationIndex(StationIndex newStationIndex)
{
    URide.stationIndex = newStationIndex;
}

uint8_t TrackElement::GetDoorAState() const
{
    return (URide.ColourScheme & TRACK_ELEMENT_COLOUR_DOOR_A_MASK) >> 2;
}

uint8_t TrackElement::GetDoorBState() const
{
    return (URide.ColourScheme & TRACK_ELEMENT_COLOUR_DOOR_B_MASK) >> 5;
}

void TrackElement::SetDoorAState(uint8_t newState)
{
    URide.ColourScheme &= ~TRACK_ELEMENT_COLOUR_DOOR_A_MASK;
    URide.ColourScheme |= ((newState << 2) & TRACK_ELEMENT_COLOUR_DOOR_A_MASK);
}

void TrackElement::SetDoorBState(uint8_t newState)
{
    URide.ColourScheme &= ~TRACK_ELEMENT_COLOUR_DOOR_B_MASK;
    URide.ColourScheme |= ((newState << 5) & TRACK_ELEMENT_COLOUR_DOOR_B_MASK);
}

RideId TrackElement::GetRideIndex() const
{
    return RideIndex;
}

void TrackElement::SetRideIndex(RideId newRideIndex)
{
    RideIndex = newRideIndex;
}

uint8_t TrackElement::GetColourScheme() const
{
    return URide.ColourScheme & TRACK_ELEMENT_COLOUR_SCHEME_MASK;
}

void TrackElement::SetColourScheme(uint8_t newColourScheme)
{
    URide.ColourScheme &= ~TRACK_ELEMENT_COLOUR_SCHEME_MASK;
    URide.ColourScheme |= (newColourScheme & TRACK_ELEMENT_COLOUR_SCHEME_MASK);
}

bool TrackElement::HasCableLift() const
{
    return Flags2 & TRACK_ELEMENT_FLAGS2_CABLE_LIFT;
}

void TrackElement::SetHasCableLift(bool on)
{
    Flags2 &= ~TRACK_ELEMENT_FLAGS2_CABLE_LIFT;
    if (on)
        Flags2 |= TRACK_ELEMENT_FLAGS2_CABLE_LIFT;
}

bool TrackElement::IsInverted() const
{
    return Flags2 & TRACK_ELEMENT_FLAGS2_INVERTED;
}

void TrackElement::SetInverted(bool inverted)
{
    if (inverted)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_INVERTED;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_INVERTED;
    }
}

bool TrackElement::BlockBrakeClosed() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_BLOCK_BRAKE_CLOSED) != 0;
}

void TrackElement::SetBlockBrakeClosed(bool isClosed)
{
    if (isClosed)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_BLOCK_BRAKE_CLOSED;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_BLOCK_BRAKE_CLOSED;
    }
}

bool TrackElement::IsIndestructible() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_INDESTRUCTIBLE_TRACK_PIECE) != 0;
}

void TrackElement::SetIsIndestructible(bool isIndestructible)
{
    if (isIndestructible)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_INDESTRUCTIBLE_TRACK_PIECE;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_INDESTRUCTIBLE_TRACK_PIECE;
    }
}

uint8_t TrackElement::GetBrakeBoosterSpeed() const
{
    return URide.BrakeBoosterSpeed << 1;
}

void TrackElement::SetBrakeBoosterSpeed(uint8_t speed)
{
    URide.BrakeBoosterSpeed = (speed >> 1);
}

bool TrackElement::HasGreenLight() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT) != 0;
}

void TrackElement::SetHasGreenLight(bool on)
{
    Flags2 &= ~TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT;
    if (on)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT;
    }
}

bool TrackElement::IsHighlighted() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_HIGHLIGHT);
}

void TrackElement::SetHighlight(bool on)
{
    Flags2 &= ~TRACK_ELEMENT_FLAGS2_HIGHLIGHT;
    if (on)
        Flags2 |= TRACK_ELEMENT_FLAGS2_HIGHLIGHT;
}
