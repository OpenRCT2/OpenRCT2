/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Track.h"

#include "../Cheats.h"
#include "../Game.h"
#include "../GameState.h"
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

using namespace OpenRCT2;
using namespace OpenRCT2::TrackMetaData;

PitchAndRoll TrackPitchAndRollStart(track_type_t trackType)
{
    const auto& ted = GetTrackElementDescriptor(trackType);
    return { ted.Definition.PitchStart, ted.Definition.RollStart };
}

PitchAndRoll TrackPitchAndRollEnd(track_type_t trackType)
{
    const auto& ted = GetTrackElementDescriptor(trackType);
    return { ted.Definition.PitchEnd, ted.Definition.RollEnd };
}

/**
 * Helper method to determine if a connects to b by its bank and angle, not location.
 */
int32_t TrackIsConnectedByShape(TileElement* a, TileElement* b)
{
    auto trackType = a->AsTrack()->GetTrackType();
    const auto* ted = &GetTrackElementDescriptor(trackType);
    auto aBank = ted->Definition.RollEnd;
    auto aAngle = ted->Definition.PitchEnd;
    aBank = TrackGetActualBank(a, aBank);

    trackType = b->AsTrack()->GetTrackType();
    ted = &GetTrackElementDescriptor(trackType);
    auto bBank = ted->Definition.RollStart;
    auto bAngle = ted->Definition.PitchStart;
    bBank = TrackGetActualBank(b, bBank);

    return aBank == bBank && aAngle == bAngle;
}

static TileElement* find_station_element(const CoordsXYZD& loc, RideId rideIndex)
{
    TileElement* tileElement = MapGetFirstElementAt(loc);
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

static void ride_remove_station(Ride& ride, const CoordsXYZ& location)
{
    for (auto& station : ride.GetStations())
    {
        auto stationStart = station.GetStart();
        if (stationStart == location)
        {
            station.Start.SetNull();
            ride.num_stations--;
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006C4D89
 */
ResultWithMessage TrackAddStationElement(CoordsXYZD loc, RideId rideIndex, int32_t flags, bool fromTrackDesign)
{
    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
        return { false };

    CoordsXY stationBackLoc = loc;
    CoordsXY stationFrontLoc = loc;
    int32_t stationLength = 1;

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION))
    {
        if (ride->num_stations >= Limits::MaxStationsPerRide)
        {
            return { false, STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE };
        }
        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            auto stationIndex = RideGetFirstEmptyStationStart(*ride);
            assert(!stationIndex.IsNull());

            auto& station = ride->GetStation(stationIndex);
            station.Start.x = loc.x;
            station.Start.y = loc.y;
            station.Height = loc.z / COORDS_Z_STEP;
            station.Depart = 1;
            station.Length = 0;
            ride->num_stations++;
        }
        return { true };
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
            if (stationElement->AsTrack()->GetTrackType() == TrackElemType::kEndStation)
            {
                if (flags & GAME_COMMAND_FLAG_APPLY)
                {
                    ride_remove_station(*ride, loc);
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
            if (stationElement->AsTrack()->GetTrackType() == TrackElemType::kEndStation)
            {
                if (flags & GAME_COMMAND_FLAG_APPLY)
                {
                    ride_remove_station(*ride, loc);
                }
            }

            stationFrontLoc = loc;
            stationLength++;
        }
    } while (stationElement != nullptr);

    // When attempting to place a track design, it sometimes happens that the front and back of station 0 are built,
    // but the middle is not. Allow this, so the track place function can actually finish building all 4 stations.
    // This _might_ cause issues if the track designs is bugged and actually has 5.
    if (stationBackLoc == stationFrontLoc && ride->num_stations >= Limits::MaxStationsPerRide && !fromTrackDesign)
    {
        return { false, STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE };
    }

    if (stationLength > kMaxStationPlatformLength)
    {
        return { false, STR_STATION_PLATFORM_TOO_LONG };
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
                    auto stationIndex = RideGetFirstEmptyStationStart(*ride);
                    if (stationIndex.IsNull())
                    {
                        LOG_VERBOSE("No empty station starts, not updating metadata! This can happen with hacked rides.");
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

                    targetTrackType = TrackElemType::kEndStation;
                }
                else if (stationBackLoc == loc)
                {
                    targetTrackType = TrackElemType::kBeginStation;
                }
                else
                {
                    targetTrackType = TrackElemType::kMiddleStation;
                }
                stationElement->AsTrack()->SetTrackType(targetTrackType);

                MapInvalidateElement(loc, stationElement);

                if (stationBackLoc != loc)
                {
                    loc -= CoordsDirectionDelta[loc.direction];
                    finaliseStationDone = false;
                }
            }
        } while (!finaliseStationDone);
    }
    return { true };
}

/**
 *
 *  rct2: 0x006C494B
 */
ResultWithMessage TrackRemoveStationElement(const CoordsXYZD& loc, RideId rideIndex, int32_t flags)
{
    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
        return { false };

    CoordsXYZD removeLoc = loc;
    CoordsXYZD stationBackLoc = loc;
    CoordsXYZD stationFrontLoc = loc;
    int32_t stationLength = 0;
    int32_t ByteF441D1 = -1;

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION))
    {
        TileElement* tileElement = MapGetTrackElementAtWithDirectionFromRide(loc, rideIndex);
        if (tileElement != nullptr)
        {
            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride_remove_station(*ride, loc);
            }
        }
        return { true };
    }

    TileElement* stationElement;

    // Search backwards for more station
    CoordsXYZD currentLoc = stationBackLoc;
    while ((stationElement = find_station_element(currentLoc, rideIndex)) != nullptr)
    {
        if (stationElement->AsTrack()->GetTrackType() == TrackElemType::kEndStation)
        {
            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride_remove_station(*ride, currentLoc);
            }
        }

        stationBackLoc = currentLoc;
        ByteF441D1++;

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
            if (stationElement->AsTrack()->GetTrackType() == TrackElemType::kEndStation)
            {
                if (flags & GAME_COMMAND_FLAG_APPLY)
                {
                    ride_remove_station(*ride, currentLoc);
                }
            }
            stationFrontLoc = currentLoc;
            stationLength++;
        }
    } while (stationElement != nullptr);

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
    {
        if ((removeLoc != stationBackLoc) && (removeLoc != stationFrontLoc) && ride->num_stations >= Limits::MaxStationsPerRide)
        {
            return { false, STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE };
        }

        return { true };
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
                    auto stationIndex = RideGetFirstEmptyStationStart(*ride);
                    if (stationIndex.IsNull())
                    {
                        LOG_VERBOSE("No empty station starts, not updating metadata! This can happen with hacked rides.");
                    }
                    else
                    {
                        auto& station = ride->GetStation(stationIndex);
                        station.Start = currentLoc;
                        station.Height = currentLoc.z / COORDS_Z_STEP;
                        station.Depart = 1;
                        station.Length = stationLength != 0 ? stationLength : ByteF441D1;
                        ride->num_stations++;
                    }

                    stationLength = 0;
                    targetTrackType = TrackElemType::kEndStation;
                }
                else
                {
                    if (currentLoc - CoordsDirectionDelta[currentLoc.direction] == removeLoc)
                    {
                        targetTrackType = TrackElemType::kBeginStation;
                    }
                    else
                    {
                        if (currentLoc == stationBackLoc)
                        {
                            targetTrackType = TrackElemType::kBeginStation;
                        }
                        else
                        {
                            targetTrackType = TrackElemType::kMiddleStation;
                        }
                    }
                }
                stationElement->AsTrack()->SetTrackType(targetTrackType);

                MapInvalidateElement(currentLoc, stationElement);
            }
        }

        if (currentLoc != stationBackLoc)
        {
            currentLoc -= CoordsDirectionDelta[currentLoc.direction];
            finaliseStationDone = false;
        }
    } while (!finaliseStationDone);

    return { true };
}

void TrackCircuitIteratorBegin(TrackCircuitIterator* it, CoordsXYE first)
{
    it->last = first;
    it->first = nullptr;
    it->firstIteration = true;
    it->looped = false;
}

bool TrackCircuitIteratorPrevious(TrackCircuitIterator* it)
{
    TrackBeginEnd trackBeginEnd;

    if (it->first == nullptr)
    {
        if (!TrackBlockGetPrevious({ it->last.x, it->last.y, it->last.element }, &trackBeginEnd))
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

    if (TrackBlockGetPrevious({ it->last.x, it->last.y, it->last.element }, &trackBeginEnd))
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

bool TrackCircuitIteratorNext(TrackCircuitIterator* it)
{
    if (it->first == nullptr)
    {
        if (!TrackBlockGetNext(&it->last, &it->current, &it->currentZ, &it->currentDirection))
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
    return TrackBlockGetNext(&it->last, &it->current, &it->currentZ, &it->currentDirection);
}

bool TrackCircuitIteratorsMatch(const TrackCircuitIterator* firstIt, const TrackCircuitIterator* secondIt)
{
    return (
        firstIt->currentZ == secondIt->currentZ && firstIt->currentDirection == secondIt->currentDirection
        && firstIt->current.x == secondIt->current.x && firstIt->current.y == secondIt->current.y);
}

void TrackGetBack(const CoordsXYE& input, CoordsXYE* output)
{
    CoordsXYE lastTrack;
    TrackBeginEnd currentTrack;
    bool result;

    lastTrack = input;
    do
    {
        result = TrackBlockGetPrevious(lastTrack, &currentTrack);
        if (result)
        {
            lastTrack.x = currentTrack.begin_x;
            lastTrack.y = currentTrack.begin_y;
            lastTrack.element = currentTrack.begin_element;
        }
    } while (result);
    *output = lastTrack;
}

void TrackGetFront(const CoordsXYE& input, CoordsXYE* output)
{
    CoordsXYE lastTrack, currentTrack;
    int32_t z, direction;
    bool result;

    lastTrack = input;
    do
    {
        result = TrackBlockGetNext(&lastTrack, &currentTrack, &z, &direction);
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
        case TrackElemType::kEndStation:
        case TrackElemType::kCableLiftHill:
        case TrackElemType::kBlockBrakes:
        case TrackElemType::kDiagBlockBrakes:
            return true;
        case TrackElemType::kUp25ToFlat:
        case TrackElemType::kUp60ToFlat:
        case TrackElemType::kDiagUp25ToFlat:
        case TrackElemType::kDiagUp60ToFlat:
            if (HasChain())
            {
                return true;
            }
            break;
    }
    return false;
}

TrackRoll TrackGetActualBank(TileElement* tileElement, TrackRoll bank)
{
    auto ride = GetRide(tileElement->AsTrack()->GetRideIndex());
    if (ride != nullptr)
    {
        bool isInverted = tileElement->AsTrack()->IsInverted();
        return TrackGetActualBank2(ride->type, isInverted, bank);
    }
    return bank;
}

TrackRoll TrackGetActualBank2(int32_t rideType, bool isInverted, TrackRoll bank)
{
    if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE))
    {
        if (isInverted)
        {
            if (bank == TrackRoll::None)
            {
                bank = TrackRoll::UpsideDown;
            }
            else if (bank == TrackRoll::UpsideDown)
            {
                bank = TrackRoll::None;
            }
        }
    }
    return bank;
}

TrackRoll TrackGetActualBank3(bool useInvertedSprites, TileElement* tileElement)
{
    auto trackType = tileElement->AsTrack()->GetTrackType();
    const auto& ted = GetTrackElementDescriptor(trackType);
    auto bankStart = ted.Definition.RollStart;
    auto ride = GetRide(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr)
        return bankStart;

    bool isInverted = useInvertedSprites ^ tileElement->AsTrack()->IsInverted();
    return TrackGetActualBank2(ride->type, isInverted, bankStart);
}

bool TrackElement::IsStation() const
{
    return TrackTypeIsStation(GetTrackType());
}

bool TrackTypeIsStation(track_type_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::kEndStation:
        case TrackElemType::kBeginStation:
        case TrackElemType::kMiddleStation:
            return true;
        default:
            return false;
    }
}

bool TrackTypeIsBrakes(track_type_t trackType)
{
    return (trackType == TrackElemType::kBrakes) || (trackType == TrackElemType::kDiagBrakes);
}

bool TrackTypeIsBlockBrakes(track_type_t trackType)
{
    return (trackType == TrackElemType::kBlockBrakes) || (trackType == TrackElemType::kDiagBlockBrakes);
}

bool TrackTypeIsBooster(track_type_t trackType)
{
    return trackType == TrackElemType::kBooster;
}

bool TrackElementIsCovered(track_type_t trackElementType)
{
    switch (trackElementType)
    {
        case TrackElemType::kFlatCovered:
        case TrackElemType::kUp25Covered:
        case TrackElemType::kUp60Covered:
        case TrackElemType::kFlatToUp25Covered:
        case TrackElemType::kUp25ToUp60Covered:
        case TrackElemType::kUp60ToUp25Covered:
        case TrackElemType::kUp25ToFlatCovered:
        case TrackElemType::kDown25Covered:
        case TrackElemType::kDown60Covered:
        case TrackElemType::kFlatToDown25Covered:
        case TrackElemType::kDown25ToDown60Covered:
        case TrackElemType::kDown60ToDown25Covered:
        case TrackElemType::kDown25ToFlatCovered:
        case TrackElemType::kLeftQuarterTurn5TilesCovered:
        case TrackElemType::kRightQuarterTurn5TilesCovered:
        case TrackElemType::kSBendLeftCovered:
        case TrackElemType::kSBendRightCovered:
        case TrackElemType::kLeftQuarterTurn3TilesCovered:
        case TrackElemType::kRightQuarterTurn3TilesCovered:
            return true;
        default:
            return false;
    }
}

track_type_t UncoverTrackElement(track_type_t trackElementType)
{
    switch (trackElementType)
    {
        case TrackElemType::kFlatCovered:
            return TrackElemType::kFlat;
        case TrackElemType::kUp25Covered:
            return TrackElemType::kUp25;
        case TrackElemType::kUp60Covered:
            return TrackElemType::kUp60;
        case TrackElemType::kFlatToUp25Covered:
            return TrackElemType::kFlatToUp25;
        case TrackElemType::kUp25ToUp60Covered:
            return TrackElemType::kUp25ToUp60;
        case TrackElemType::kUp60ToUp25Covered:
            return TrackElemType::kUp60ToUp25;
        case TrackElemType::kUp25ToFlatCovered:
            return TrackElemType::kUp25ToFlat;
        case TrackElemType::kDown25Covered:
            return TrackElemType::kDown25;
        case TrackElemType::kDown60Covered:
            return TrackElemType::kDown60;
        case TrackElemType::kFlatToDown25Covered:
            return TrackElemType::kFlatToDown25;
        case TrackElemType::kDown25ToDown60Covered:
            return TrackElemType::kDown25ToDown60;
        case TrackElemType::kDown60ToDown25Covered:
            return TrackElemType::kDown60ToDown25;
        case TrackElemType::kDown25ToFlatCovered:
            return TrackElemType::kDown25ToFlat;
        case TrackElemType::kLeftQuarterTurn5TilesCovered:
            return TrackElemType::kLeftQuarterTurn5Tiles;
        case TrackElemType::kRightQuarterTurn5TilesCovered:
            return TrackElemType::kRightQuarterTurn5Tiles;
        case TrackElemType::kSBendLeftCovered:
            return TrackElemType::kSBendLeft;
        case TrackElemType::kSBendRightCovered:
            return TrackElemType::kSBendRight;
        case TrackElemType::kLeftQuarterTurn3TilesCovered:
            return TrackElemType::kLeftQuarterTurn3Tiles;
        case TrackElemType::kRightQuarterTurn3TilesCovered:
            return TrackElemType::kRightQuarterTurn3Tiles;
        default:
            return trackElementType;
    }
}

bool TrackTypeHasSpeedSetting(track_type_t trackType)
{
    return TrackTypeIsBooster(trackType) || TrackTypeIsBrakes(trackType) || TrackTypeIsBlockBrakes(trackType);
}

bool TrackTypeIsHelix(track_type_t trackType)
{
    if (trackType >= TrackElemType::kLeftHalfBankedHelixUpSmall && trackType <= TrackElemType::kRightHalfBankedHelixDownLarge)
        return true;

    if (trackType >= TrackElemType::kLeftQuarterBankedHelixLargeUp && trackType <= TrackElemType::kRightQuarterHelixLargeDown)
        return true;

    return false;
}

std::optional<CoordsXYZD> GetTrackSegmentOrigin(const CoordsXYE& posEl)
{
    if (posEl.element == nullptr)
        return {};

    auto trackEl = posEl.element->AsTrack();
    if (trackEl == nullptr)
        return {};

    const auto& ted = GetTrackElementDescriptor(trackEl->GetTrackType());
    auto direction = trackEl->GetDirection();
    auto coords = CoordsXYZ(posEl.x, posEl.y, trackEl->GetBaseZ());

    // Subtract the current sequence's offset
    const auto* trackBlock = ted.GetBlockForSequence(trackEl->GetSequenceIndex());
    if (trackBlock == nullptr)
        return {};

    CoordsXY trackBlockOffset = { trackBlock->x, trackBlock->y };
    coords += trackBlockOffset.Rotate(DirectionReverse(direction));
    coords.z -= trackBlock->z;

    return CoordsXYZD(coords, direction);
}

uint8_t TrackElement::GetSeatRotation() const
{
    const auto* ride = GetRide(GetRideIndex());
    if (ride != nullptr && ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_LANDSCAPE_DOORS))
        return kDefaultSeatRotation;

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

bool TrackElement::IsBrakeClosed() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_BRAKE_CLOSED) != 0;
}

void TrackElement::SetBrakeClosed(bool isClosed)
{
    if (isClosed)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_BRAKE_CLOSED;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_BRAKE_CLOSED;
    }
}

bool TrackElement::IsIndestructible() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_INDESTRUCTIBLE_TRACK_PIECE) != 0 && !GetGameState().Cheats.MakeAllDestructible;
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

bool TrackTypeMustBeMadeInvisible(ride_type_t rideType, track_type_t trackType, int32_t parkFileVersion)
{
    // Lots of Log Flumes exist where the downward slopes are simulated by using other track
    // types like the Splash Boats, but not actually made invisible, because they never needed
    // to be.
    if (rideType == RIDE_TYPE_LOG_FLUME && parkFileVersion <= 15)
    {
        if (trackType == TrackElemType::kDown25ToDown60 || trackType == TrackElemType::kDown60
            || trackType == TrackElemType::kDown60ToDown25)
        {
            return true;
        }
    }
    else if (rideType == RIDE_TYPE_GIGA_COASTER && parkFileVersion <= 30)
    {
        switch (trackType)
        {
            case TrackElemType::kUp90:
            case TrackElemType::kDown90:
            case TrackElemType::kUp60ToUp90:
            case TrackElemType::kDown90ToDown60:
            case TrackElemType::kUp90ToUp60:
            case TrackElemType::kDown60ToDown90:
            case TrackElemType::kLeftQuarterTurn1TileUp90:
            case TrackElemType::kRightQuarterTurn1TileUp90:
            case TrackElemType::kLeftQuarterTurn1TileDown90:
            case TrackElemType::kRightQuarterTurn1TileDown90:
            case TrackElemType::kLeftBarrelRollUpToDown:
            case TrackElemType::kRightBarrelRollUpToDown:
            case TrackElemType::kLeftBarrelRollDownToUp:
            case TrackElemType::kRightBarrelRollDownToUp:
            case TrackElemType::kHalfLoopUp:
            case TrackElemType::kHalfLoopDown:
            case TrackElemType::kLeftVerticalLoop:
            case TrackElemType::kRightVerticalLoop:
            case TrackElemType::kLeftCorkscrewUp:
            case TrackElemType::kRightCorkscrewUp:
            case TrackElemType::kLeftCorkscrewDown:
            case TrackElemType::kRightCorkscrewDown:
            case TrackElemType::kLeftLargeCorkscrewUp:
            case TrackElemType::kRightLargeCorkscrewUp:
            case TrackElemType::kLeftLargeCorkscrewDown:
            case TrackElemType::kRightLargeCorkscrewDown:
            case TrackElemType::kLeftZeroGRollUp:
            case TrackElemType::kRightZeroGRollUp:
            case TrackElemType::kLeftZeroGRollDown:
            case TrackElemType::kRightZeroGRollDown:
            case TrackElemType::kLeftLargeZeroGRollUp:
            case TrackElemType::kRightLargeZeroGRollUp:
            case TrackElemType::kLeftLargeZeroGRollDown:
            case TrackElemType::kRightLargeZeroGRollDown:
            case TrackElemType::kUp90ToInvertedFlatQuarterLoop:
            case TrackElemType::kInvertedFlatToDown90QuarterLoop:
            case TrackElemType::kLeftBankToLeftQuarterTurn3TilesUp25:
            case TrackElemType::kRightBankToRightQuarterTurn3TilesUp25:
            case TrackElemType::kLeftQuarterTurn3TilesDown25ToLeftBank:
            case TrackElemType::kRightQuarterTurn3TilesDown25ToRightBank:
            case TrackElemType::kLeftMediumHalfLoopUp:
            case TrackElemType::kRightMediumHalfLoopUp:
            case TrackElemType::kLeftMediumHalfLoopDown:
            case TrackElemType::kRightMediumHalfLoopDown:
            case TrackElemType::kLeftLargeHalfLoopUp:
            case TrackElemType::kRightLargeHalfLoopUp:
            case TrackElemType::kRightLargeHalfLoopDown:
            case TrackElemType::kLeftLargeHalfLoopDown:
            case TrackElemType::kFlatToUp60:
            case TrackElemType::kUp60ToFlat:
            case TrackElemType::kFlatToDown60:
            case TrackElemType::kDown60ToFlat:
            case TrackElemType::kDiagFlatToUp60:
            case TrackElemType::kDiagUp60ToFlat:
            case TrackElemType::kDiagFlatToDown60:
            case TrackElemType::kDiagDown60ToFlat:
            case TrackElemType::kLeftEighthToDiagUp25:
            case TrackElemType::kRightEighthToDiagUp25:
            case TrackElemType::kLeftEighthToDiagDown25:
            case TrackElemType::kRightEighthToDiagDown25:
            case TrackElemType::kLeftEighthToOrthogonalUp25:
            case TrackElemType::kRightEighthToOrthogonalUp25:
            case TrackElemType::kLeftEighthToOrthogonalDown25:
            case TrackElemType::kRightEighthToOrthogonalDown25:
            case TrackElemType::kDiagUp25ToLeftBankedUp25:
            case TrackElemType::kDiagUp25ToRightBankedUp25:
            case TrackElemType::kDiagLeftBankedUp25ToUp25:
            case TrackElemType::kDiagRightBankedUp25ToUp25:
            case TrackElemType::kDiagDown25ToLeftBankedDown25:
            case TrackElemType::kDiagDown25ToRightBankedDown25:
            case TrackElemType::kDiagLeftBankedDown25ToDown25:
            case TrackElemType::kDiagRightBankedDown25ToDown25:
            case TrackElemType::kDiagLeftBankedFlatToLeftBankedUp25:
            case TrackElemType::kDiagRightBankedFlatToRightBankedUp25:
            case TrackElemType::kDiagLeftBankedUp25ToLeftBankedFlat:
            case TrackElemType::kDiagRightBankedUp25ToRightBankedFlat:
            case TrackElemType::kDiagLeftBankedFlatToLeftBankedDown25:
            case TrackElemType::kDiagRightBankedFlatToRightBankedDown25:
            case TrackElemType::kDiagLeftBankedDown25ToLeftBankedFlat:
            case TrackElemType::kDiagRightBankedDown25ToRightBankedFlat:
            case TrackElemType::kDiagUp25LeftBanked:
            case TrackElemType::kDiagUp25RightBanked:
            case TrackElemType::kDiagDown25LeftBanked:
            case TrackElemType::kDiagDown25RightBanked:
            case TrackElemType::kDiagFlatToLeftBankedUp25:
            case TrackElemType::kDiagFlatToRightBankedUp25:
            case TrackElemType::kDiagLeftBankedUp25ToFlat:
            case TrackElemType::kDiagRightBankedUp25ToFlat:
            case TrackElemType::kDiagFlatToLeftBankedDown25:
            case TrackElemType::kDiagFlatToRightBankedDown25:
            case TrackElemType::kDiagLeftBankedDown25ToFlat:
            case TrackElemType::kDiagRightBankedDown25ToFlat:
            case TrackElemType::kLeftEighthBankToDiagUp25:
            case TrackElemType::kRightEighthBankToDiagUp25:
            case TrackElemType::kLeftEighthBankToDiagDown25:
            case TrackElemType::kRightEighthBankToDiagDown25:
            case TrackElemType::kLeftEighthBankToOrthogonalUp25:
            case TrackElemType::kRightEighthBankToOrthogonalUp25:
            case TrackElemType::kLeftEighthBankToOrthogonalDown25:
            case TrackElemType::kRightEighthBankToOrthogonalDown25:
                return true;
        }
    }

    return false;
}
