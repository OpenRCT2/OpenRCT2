/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Track.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/SawyerCoding.h"
#include "../interface/Viewport.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../platform/Platform.h"
#include "../rct1/RCT1.h"
#include "../ride/RideColour.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "Ride.h"
#include "RideData.h"
#include "RideRatings.h"
#include "Station.h"
#include "TrackData.h"
#include "TrackDesign.h"

#include <cassert>

using namespace OpenRCT2;
using namespace OpenRCT2::TrackMetaData;

PitchAndRoll TrackPitchAndRollStart(OpenRCT2::TrackElemType trackType)
{
    const auto& ted = GetTrackElementDescriptor(trackType);
    return { ted.definition.pitchStart, ted.definition.rollStart };
}

PitchAndRoll TrackPitchAndRollEnd(OpenRCT2::TrackElemType trackType)
{
    const auto& ted = GetTrackElementDescriptor(trackType);
    return { ted.definition.pitchEnd, ted.definition.rollEnd };
}

/**
 * Helper method to determine if a connects to b by its bank and angle, not location.
 */
int32_t TrackIsConnectedByShape(TileElement* a, TileElement* b)
{
    auto trackType = a->AsTrack()->GetTrackType();
    const auto* ted = &GetTrackElementDescriptor(trackType);
    auto aBank = ted->definition.rollEnd;
    auto aAngle = ted->definition.pitchEnd;
    aBank = TrackGetActualBank(a, aBank);

    trackType = b->AsTrack()->GetTrackType();
    ted = &GetTrackElementDescriptor(trackType);
    auto bBank = ted->definition.rollStart;
    auto bAngle = ted->definition.pitchStart;
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

    if (ride->GetRideTypeDescriptor().HasFlag(RtdFlag::hasSinglePieceStation))
    {
        if (ride->num_stations >= Limits::kMaxStationsPerRide)
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
            station.Height = loc.z / kCoordsZStep;
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
            if (stationElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
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
            if (stationElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
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
    if (stationBackLoc == stationFrontLoc && ride->num_stations >= Limits::kMaxStationsPerRide && !fromTrackDesign)
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
                OpenRCT2::TrackElemType targetTrackType;
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
                        station.Height = loc.z / kCoordsZStep;
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

    if (ride->GetRideTypeDescriptor().HasFlag(RtdFlag::hasSinglePieceStation))
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
        if (stationElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
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
            if (stationElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
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
        if ((removeLoc != stationBackLoc) && (removeLoc != stationFrontLoc)
            && ride->num_stations >= Limits::kMaxStationsPerRide)
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
                OpenRCT2::TrackElemType targetTrackType;
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
                        station.Height = currentLoc.z / kCoordsZStep;
                        station.Depart = 1;
                        station.Length = stationLength != 0 ? stationLength : ByteF441D1;
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

TrackElement* TrackGetPreviousBlock(CoordsXYZ& location, TileElement* tileElement)
{
    CoordsXYZ startLocation = location;
    TrackBeginEnd trackBeginEnd, slowTrackBeginEnd;
    TileElement slowTileElement = *tileElement;
    bool counter = true;
    CoordsXY slowLocation = location;
    do
    {
        if (!TrackBlockGetPrevious({ location, tileElement }, &trackBeginEnd))
        {
            return nullptr;
        }
        if (trackBeginEnd.begin_x == startLocation.x && trackBeginEnd.begin_y == startLocation.y
            && tileElement == trackBeginEnd.begin_element)
        {
            return nullptr;
        }

        location.x = trackBeginEnd.end_x;
        location.y = trackBeginEnd.end_y;
        location.z = trackBeginEnd.begin_z;
        tileElement = trackBeginEnd.begin_element;

        // #2081: prevent infinite loop
        counter = !counter;
        if (counter)
        {
            TrackBlockGetPrevious({ slowLocation, &slowTileElement }, &slowTrackBeginEnd);
            slowLocation.x = slowTrackBeginEnd.end_x;
            slowLocation.y = slowTrackBeginEnd.end_y;
            slowTileElement = *(slowTrackBeginEnd.begin_element);
            if (slowLocation == location && slowTileElement.GetBaseZ() == tileElement->GetBaseZ()
                && slowTileElement.GetType() == tileElement->GetType()
                && slowTileElement.GetDirection() == tileElement->GetDirection())
            {
                return nullptr;
            }
        }
    } while (!(trackBeginEnd.begin_element->AsTrack()->IsBlockStart()));

    // Get the start of the track block instead of the end
    location = { trackBeginEnd.begin_x, trackBeginEnd.begin_y, trackBeginEnd.begin_z };
    auto trackOrigin = MapGetTrackElementAtOfTypeSeq(location, trackBeginEnd.begin_element->AsTrack()->GetTrackType(), 0);
    if (trackOrigin == nullptr)
    {
        return nullptr;
    }

    return trackOrigin->AsTrack();
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

TrackRoll TrackGetActualBank2(ride_type_t rideType, bool isInverted, TrackRoll bank)
{
    if (GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::hasInvertedVariant))
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
    auto bankStart = ted.definition.rollStart;
    auto ride = GetRide(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr)
        return bankStart;

    bool isInverted = useInvertedSprites ^ tileElement->AsTrack()->IsInverted();
    return TrackGetActualBank2(ride->type, isInverted, bankStart);
}

bool TrackTypeIsStation(OpenRCT2::TrackElemType trackType)
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

bool TrackTypeIsBrakes(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Brakes:
        case TrackElemType::DiagBrakes:
        case TrackElemType::Down25Brakes:
            return true;
        default:
            return false;
    }
}

bool TrackTypeIsBlockBrakes(OpenRCT2::TrackElemType trackType)
{
    return (trackType == TrackElemType::BlockBrakes) || (trackType == TrackElemType::DiagBlockBrakes);
}

bool TrackTypeIsBooster(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Booster:
        case TrackElemType::DiagBooster:
            return true;
        default:
            return false;
    }
}

bool TrackElementIsCovered(OpenRCT2::TrackElemType trackElementType)
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

OpenRCT2::TrackElemType UncoverTrackElement(OpenRCT2::TrackElemType trackElementType)
{
    switch (trackElementType)
    {
        case TrackElemType::FlatCovered:
            return TrackElemType::Flat;
        case TrackElemType::Up25Covered:
            return TrackElemType::Up25;
        case TrackElemType::Up60Covered:
            return TrackElemType::Up60;
        case TrackElemType::FlatToUp25Covered:
            return TrackElemType::FlatToUp25;
        case TrackElemType::Up25ToUp60Covered:
            return TrackElemType::Up25ToUp60;
        case TrackElemType::Up60ToUp25Covered:
            return TrackElemType::Up60ToUp25;
        case TrackElemType::Up25ToFlatCovered:
            return TrackElemType::Up25ToFlat;
        case TrackElemType::Down25Covered:
            return TrackElemType::Down25;
        case TrackElemType::Down60Covered:
            return TrackElemType::Down60;
        case TrackElemType::FlatToDown25Covered:
            return TrackElemType::FlatToDown25;
        case TrackElemType::Down25ToDown60Covered:
            return TrackElemType::Down25ToDown60;
        case TrackElemType::Down60ToDown25Covered:
            return TrackElemType::Down60ToDown25;
        case TrackElemType::Down25ToFlatCovered:
            return TrackElemType::Down25ToFlat;
        case TrackElemType::LeftQuarterTurn5TilesCovered:
            return TrackElemType::LeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5TilesCovered:
            return TrackElemType::RightQuarterTurn5Tiles;
        case TrackElemType::SBendLeftCovered:
            return TrackElemType::SBendLeft;
        case TrackElemType::SBendRightCovered:
            return TrackElemType::SBendRight;
        case TrackElemType::LeftQuarterTurn3TilesCovered:
            return TrackElemType::LeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3TilesCovered:
            return TrackElemType::RightQuarterTurn3Tiles;
        default:
            return trackElementType;
    }
}

bool TrackTypeHasSpeedSetting(OpenRCT2::TrackElemType trackType)
{
    return TrackTypeIsBooster(trackType) || TrackTypeIsBrakes(trackType) || TrackTypeIsBlockBrakes(trackType);
}

bool TrackTypeIsHelix(OpenRCT2::TrackElemType trackType)
{
    if (trackType >= TrackElemType::LeftHalfBankedHelixUpSmall && trackType <= TrackElemType::RightHalfBankedHelixDownLarge)
        return true;

    if (trackType >= TrackElemType::LeftQuarterBankedHelixLargeUp && trackType <= TrackElemType::RightQuarterHelixLargeDown)
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
    auto sequenceIndex = trackEl->GetSequenceIndex();
    if (sequenceIndex >= ted.numSequences)
        return {};

    const auto& trackBlock = ted.sequences[sequenceIndex].clearance;
    CoordsXY trackBlockOffset = { trackBlock.x, trackBlock.y };
    coords += trackBlockOffset.Rotate(DirectionReverse(direction));
    coords.z -= trackBlock.z;

    return CoordsXYZD(coords, direction);
}
