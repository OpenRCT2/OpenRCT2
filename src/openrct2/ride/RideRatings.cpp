/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideRatings.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/Guard.hpp"
#include "../core/Money.hpp"
#include "../core/UnitConversion.h"
#include "../profiling/Profiling.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/WindowManager.h"
#include "../world/Map.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "Ride.h"
#include "RideData.h"
#include "RideManager.hpp"
#include "Station.h"
#include "Track.h"
#include "TrackData.h"

#include <iterator>

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;
using namespace OpenRCT2::TrackMetaData;

enum
{
    RIDE_RATINGS_STATE_FIND_NEXT_RIDE,
    RIDE_RATINGS_STATE_INITIALISE,
    RIDE_RATINGS_STATE_2,
    RIDE_RATINGS_STATE_CALCULATE,
    RIDE_RATINGS_STATE_4,
    RIDE_RATINGS_STATE_5
};

enum
{
    RIDE_RATING_STATION_FLAG_NO_ENTRANCE = 1 << 0
};

enum
{
    PROXIMITY_WATER_OVER,                   // 0x0138B596
    PROXIMITY_WATER_TOUCH,                  // 0x0138B598
    PROXIMITY_WATER_LOW,                    // 0x0138B59A
    PROXIMITY_WATER_HIGH,                   // 0x0138B59C
    PROXIMITY_SURFACE_TOUCH,                // 0x0138B59E
    PROXIMITY_QUEUE_PATH_OVER,              // 0x0138B5A0
    PROXIMITY_QUEUE_PATH_TOUCH_ABOVE,       // 0x0138B5A2
    PROXIMITY_QUEUE_PATH_TOUCH_UNDER,       // 0x0138B5A4
    PROXIMITY_PATH_TOUCH_ABOVE,             // 0x0138B5A6
    PROXIMITY_PATH_TOUCH_UNDER,             // 0x0138B5A8
    PROXIMITY_OWN_TRACK_TOUCH_ABOVE,        // 0x0138B5AA
    PROXIMITY_OWN_TRACK_CLOSE_ABOVE,        // 0x0138B5AC
    PROXIMITY_FOREIGN_TRACK_ABOVE_OR_BELOW, // 0x0138B5AE
    PROXIMITY_FOREIGN_TRACK_TOUCH_ABOVE,    // 0x0138B5B0
    PROXIMITY_FOREIGN_TRACK_CLOSE_ABOVE,    // 0x0138B5B2
    PROXIMITY_SCENERY_SIDE_BELOW,           // 0x0138B5B4
    PROXIMITY_SCENERY_SIDE_ABOVE,           // 0x0138B5B6
    PROXIMITY_OWN_STATION_TOUCH_ABOVE,      // 0x0138B5B8
    PROXIMITY_OWN_STATION_CLOSE_ABOVE,      // 0x0138B5BA
    PROXIMITY_TRACK_THROUGH_VERTICAL_LOOP,  // 0x0138B5BC
    PROXIMITY_PATH_TROUGH_VERTICAL_LOOP,    // 0x0138B5BE
    PROXIMITY_INTERSECTING_VERTICAL_LOOP,   // 0x0138B5C0
    PROXIMITY_THROUGH_VERTICAL_LOOP,        // 0x0138B5C2
    PROXIMITY_PATH_SIDE_CLOSE,              // 0x0138B5C4
    PROXIMITY_FOREIGN_TRACK_SIDE_CLOSE,     // 0x0138B5C6
    PROXIMITY_SURFACE_SIDE_CLOSE,           // 0x0138B5C8
    PROXIMITY_COUNT
};

struct ShelteredEights
{
    uint8_t TrackShelteredEighths;
    uint8_t TotalShelteredEighths;
};

// Amount of updates allowed per updating state on the current tick.
// The total amount would be MaxRideRatingSubSteps * RideRating::kMaxUpdateStates which
// would be currently 80, this is the worst case of sub-steps and may break out earlier.
static constexpr size_t MaxRideRatingUpdateSubSteps = 20;

static void ride_ratings_update_state(RideRating::UpdateState& state);
static void ride_ratings_update_state_0(RideRating::UpdateState& state);
static void ride_ratings_update_state_1(RideRating::UpdateState& state);
static void ride_ratings_update_state_2(RideRating::UpdateState& state);
static void ride_ratings_update_state_3(RideRating::UpdateState& state);
static void ride_ratings_update_state_4(RideRating::UpdateState& state);
static void ride_ratings_update_state_5(RideRating::UpdateState& state);
static void ride_ratings_begin_proximity_loop(RideRating::UpdateState& state);
static void RideRatingsCalculate(RideRating::UpdateState& state, Ride& ride);
static void RideRatingsCalculateValue(Ride& ride);
static void ride_ratings_score_close_proximity(RideRating::UpdateState& state, TileElement* inputTileElement);
static void RideRatingsAdd(RideRating::Tuple& ratings, int32_t excitement, int32_t intensity, int32_t nausea);

static ShelteredEights GetNumOfShelteredEighths(const Ride& ride);
static money64 RideComputeUpkeep(RideRating::UpdateState& state, const Ride& ride);
static void SetUnreliabilityFactor(Ride& ride);

static void RideRatingsApplyAdjustments(const Ride& ride, RideRating::Tuple& ratings);
static void RideRatingsApplyIntensityPenalty(RideRating::Tuple& ratings);

static void RideRatingsApplyBonusLength(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusSynchronisation(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusTrainLength(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusMaxSpeed(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusAverageSpeed(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusDuration(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusGForces(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusTurns(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusDrops(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusSheltered(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusRotations(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusOperationOption(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusReversedTrains(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusGoKartRace(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusTowerRide(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusRotoDrop(RideRating::Tuple& ratings, const Ride& ride);
static void RideRatingsApplyBonusMazeSize(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusBoatHireNoCircuit(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusSlideUnlimitedRides(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusMotionSimulatorMode(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonus3DCinemaMode(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusTopSpinMode(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusReversals(
    RideRating::Tuple& ratings, const Ride& ride, RideRating::UpdateState& state, RatingsModifier modifier);
static void RideRatingsApplyBonusHoles(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusNumTrains(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusDownwardLaunch(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyBonusLaunchedFreefallSpecial(
    RideRating::Tuple& ratings, const Ride& ride, RideRating::UpdateState& state, RatingsModifier modifier);
static void RideRatingsApplyBonusProximity(
    RideRating::Tuple& ratings, const Ride& ride, RideRating::UpdateState& state, RatingsModifier modifier);
static void RideRatingsApplyBonusScenery(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyRequirementLength(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyRequirementDropHeight(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyRequirementMaxSpeed(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyRequirementNumDrops(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyRequirementNegativeGs(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyRequirementLateralGs(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyRequirementInversions(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyRequirementUnsheltered(
    RideRating::Tuple& ratings, const Ride& ride, uint8_t shelteredEighths, RatingsModifier modifier);
static void RideRatingsApplyRequirementReversals(
    RideRating::Tuple& ratings, const Ride& ride, RideRating::UpdateState& state, RatingsModifier modifier);
static void RideRatingsApplyRequirementHoles(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyRequirementStations(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyRequirementSplashdown(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);
static void RideRatingsApplyPenaltyLateralGs(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier);

void RideRating::ResetUpdateStates()
{
    UpdateState nullState{};
    nullState.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;

    auto& updateStates = getGameState().rideRatingUpdateStates;
    std::fill(updateStates.begin(), updateStates.end(), nullState);
}

/**
 * This is a small hack function to keep calling the ride rating processor until
 * the given ride's ratings have been calculated. Whatever is currently being
 * processed will be overwritten.
 * Only purpose of this function currently is for testing.
 */
void RideRating::UpdateRide(const Ride& ride)
{
    if (ride.status != RideStatus::closed)
    {
        UpdateState state;
        state.CurrentRide = ride.id;
        state.State = RIDE_RATINGS_STATE_INITIALISE;
        while (state.State != RIDE_RATINGS_STATE_FIND_NEXT_RIDE)
        {
            ride_ratings_update_state(state);
        }
    }
}

/**
 *
 *  rct2: 0x006B5A2A
 */
void RideRating::UpdateAll()
{
    PROFILED_FUNCTION();

    if (gLegacyScene == LegacyScene::scenarioEditor)
        return;

    for (auto& updateState : getGameState().rideRatingUpdateStates)
    {
        for (size_t i = 0; i < MaxRideRatingUpdateSubSteps; ++i)
        {
            ride_ratings_update_state(updateState);

            // We need to abort the loop if the state machine requested to find the next ride.
            if (updateState.State == RIDE_RATINGS_STATE_FIND_NEXT_RIDE)
                break;
        }
    }
}

static void ride_ratings_update_state(RideRating::UpdateState& state)
{
    switch (state.State)
    {
        case RIDE_RATINGS_STATE_FIND_NEXT_RIDE:
            ride_ratings_update_state_0(state);
            break;
        case RIDE_RATINGS_STATE_INITIALISE:
            ride_ratings_update_state_1(state);
            break;
        case RIDE_RATINGS_STATE_2:
            ride_ratings_update_state_2(state);
            break;
        case RIDE_RATINGS_STATE_CALCULATE:
            ride_ratings_update_state_3(state);
            break;
        case RIDE_RATINGS_STATE_4:
            ride_ratings_update_state_4(state);
            break;
        case RIDE_RATINGS_STATE_5:
            ride_ratings_update_state_5(state);
            break;
    }
}

static bool RideRatingIsUpdatingRide(RideId id)
{
    const auto& updateStates = getGameState().rideRatingUpdateStates;
    return std::any_of(updateStates.begin(), updateStates.end(), [id](auto& state) {
        return state.CurrentRide == id && state.State != RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
    });
}

static bool ShouldSkipRatingCalculation(const Ride& ride)
{
    // Skip rides that are closed.
    if (ride.status == RideStatus::closed)
    {
        return true;
    }

    // Skip anything that is already updating.
    if (RideRatingIsUpdatingRide(ride.id))
    {
        return true;
    }

    // Skip rides that have a fixed rating.
    if (ride.lifecycleFlags & RIDE_LIFECYCLE_FIXED_RATINGS)
    {
        return true;
    }

    return false;
}

static RideId GetNextRideToUpdate(RideId currentRide)
{
    auto& gameState = getGameState();
    auto rm = RideManager(gameState);
    if (rm.size() == 0)
    {
        return RideId::GetNull();
    }

    auto it = rm.get(currentRide);
    if (it == rm.end())
    {
        // Start at the beginning, ride is missing.
        it = rm.begin();
    }
    else
    {
        it = std::next(it);
    }

    // Filter out rides to avoid wasting a tick to find the next ride.
    while (it != rm.end() && ShouldSkipRatingCalculation(*it))
    {
        it++;
    }

    // If we reached the end of the list we start over,
    // in case the next ride doesn't pass the filter function it will
    // look for the next matching ride in the next tick.
    if (it == rm.end())
        it = rm.begin();

    return (*it).id;
}

/**
 *
 *  rct2: 0x006B5A5C
 */
static void ride_ratings_update_state_0(RideRating::UpdateState& state)
{
    // It is possible that the current ride being calculated has
    // been removed or due to import invalid. For both, reset
    // ratings and start check at the start
    if (GetRide(state.CurrentRide) == nullptr)
    {
        state.CurrentRide = {};
    }

    const auto nextRideId = GetNextRideToUpdate(state.CurrentRide);
    const auto* nextRide = GetRide(nextRideId);
    if (nextRide != nullptr && !ShouldSkipRatingCalculation(*nextRide))
    {
        Guard::Assert(!RideRatingIsUpdatingRide(nextRideId));
        state.State = RIDE_RATINGS_STATE_INITIALISE;
    }
    state.CurrentRide = nextRideId;
}

/**
 *
 *  rct2: 0x006B5A94
 */
static void ride_ratings_update_state_1(RideRating::UpdateState& state)
{
    state.ProximityTotal = 0;
    for (int32_t i = 0; i < PROXIMITY_COUNT; i++)
    {
        state.ProximityScores[i] = 0;
    }
    state.AmountOfBrakes = 0;
    state.amountOfBoosters = 0;
    state.AmountOfReversers = 0;
    state.State = RIDE_RATINGS_STATE_2;
    state.StationFlags = 0;
    ride_ratings_begin_proximity_loop(state);
}

/**
 *
 *  rct2: 0x006B5C66
 */
static void ride_ratings_update_state_2(RideRating::UpdateState& state)
{
    const RideId rideIndex = state.CurrentRide;
    auto ride = GetRide(rideIndex);
    if (ride == nullptr || ride->status == RideStatus::closed || ride->type >= RIDE_TYPE_COUNT)
    {
        state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
        return;
    }

    auto loc = state.Proximity;
    TrackElemType trackType = state.ProximityTrackType;

    TileElement* tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
    {
        state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
        return;
    }
    do
    {
        if (tileElement->IsGhost())
            continue;
        if (tileElement->GetType() != TileElementType::Track)
            continue;
        if (tileElement->GetBaseZ() != loc.z)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != ride->id)
        {
            // Only check that the track belongs to the same ride if ride does not have buildable track
            if (!ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
                continue;
        }

        if (trackType == TrackElemType::none
            || (tileElement->AsTrack()->GetSequenceIndex() == 0 && trackType == tileElement->AsTrack()->GetTrackType()))
        {
            if (trackType == TrackElemType::endStation)
            {
                auto entranceIndex = tileElement->AsTrack()->GetStationIndex();
                state.StationFlags &= ~RIDE_RATING_STATION_FLAG_NO_ENTRANCE;
                if (ride->getStation(entranceIndex).Entrance.IsNull())
                {
                    state.StationFlags |= RIDE_RATING_STATION_FLAG_NO_ENTRANCE;
                }
            }

            ride_ratings_score_close_proximity(state, tileElement);

            CoordsXYE trackElement = { state.Proximity, tileElement };
            CoordsXYE nextTrackElement;
            if (!TrackBlockGetNext(&trackElement, &nextTrackElement, nullptr, nullptr))
            {
                state.State = RIDE_RATINGS_STATE_4;
                return;
            }

            loc = { nextTrackElement, nextTrackElement.element->GetBaseZ() };
            tileElement = nextTrackElement.element;
            if (loc == state.ProximityStart)
            {
                state.State = RIDE_RATINGS_STATE_CALCULATE;
                return;
            }
            state.Proximity = loc;
            state.ProximityTrackType = tileElement->AsTrack()->GetTrackType();
            return;
        }
    } while (!(tileElement++)->IsLastForTile());

    state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

/**
 *
 *  rct2: 0x006B5E4D
 */
static void ride_ratings_update_state_3(RideRating::UpdateState& state)
{
    auto ride = GetRide(state.CurrentRide);
    if (ride == nullptr || ride->status == RideStatus::closed)
    {
        state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
        return;
    }

    RideRatingsCalculate(state, *ride);
    RideRatingsCalculateValue(*ride);

    state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

/**
 *
 *  rct2: 0x006B5BAB
 */
static void ride_ratings_update_state_4(RideRating::UpdateState& state)
{
    state.State = RIDE_RATINGS_STATE_5;
    ride_ratings_begin_proximity_loop(state);
}

/**
 *
 *  rct2: 0x006B5D72
 */
static void ride_ratings_update_state_5(RideRating::UpdateState& state)
{
    auto ride = GetRide(state.CurrentRide);
    if (ride == nullptr || ride->status == RideStatus::closed)
    {
        state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
        return;
    }

    auto loc = state.Proximity;
    TrackElemType trackType = state.ProximityTrackType;

    TileElement* tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
    {
        state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
        return;
    }
    do
    {
        if (tileElement->IsGhost())
            continue;
        if (tileElement->GetType() != TileElementType::Track)
            continue;
        if (tileElement->GetBaseZ() != loc.z)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != ride->id)
        {
            // Only check that the track belongs to the same ride if ride does not have buildable track
            if (!ride->getRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
                continue;
        }

        if (trackType == TrackElemType::none || trackType == tileElement->AsTrack()->GetTrackType())
        {
            ride_ratings_score_close_proximity(state, tileElement);

            TrackBeginEnd trackBeginEnd;
            if (!TrackBlockGetPrevious({ state.Proximity, tileElement }, &trackBeginEnd))
            {
                state.State = RIDE_RATINGS_STATE_CALCULATE;
                return;
            }

            loc.x = trackBeginEnd.begin_x;
            loc.y = trackBeginEnd.begin_y;
            loc.z = trackBeginEnd.begin_z;
            if (loc == state.ProximityStart)
            {
                state.State = RIDE_RATINGS_STATE_CALCULATE;
                return;
            }
            state.Proximity = loc;
            state.ProximityTrackType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
            return;
        }
    } while (!(tileElement++)->IsLastForTile());

    state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

/**
 *
 *  rct2: 0x006B5BB2
 */
static void ride_ratings_begin_proximity_loop(RideRating::UpdateState& state)
{
    auto ride = GetRide(state.CurrentRide);
    if (ride == nullptr || ride->status == RideStatus::closed)
    {
        state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
        return;
    }

    const auto& rtd = ride->getRideTypeDescriptor();
    if (rtd.specialType == RtdSpecialType::maze)
    {
        state.State = RIDE_RATINGS_STATE_CALCULATE;
        return;
    }

    for (auto& station : ride->getStations())
    {
        if (!station.Start.IsNull())
        {
            state.StationFlags &= ~RIDE_RATING_STATION_FLAG_NO_ENTRANCE;
            if (station.Entrance.IsNull())
            {
                state.StationFlags |= RIDE_RATING_STATION_FLAG_NO_ENTRANCE;
            }

            auto location = station.GetStart();
            state.Proximity = location;
            state.ProximityTrackType = TrackElemType::none;
            state.ProximityStart = location;
            return;
        }
    }

    state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

static void proximity_score_increment(RideRating::UpdateState& state, int32_t type)
{
    state.ProximityScores[type]++;
}

/**
 *
 *  rct2: 0x006B6207
 */
static void ride_ratings_score_close_proximity_in_direction(
    RideRating::UpdateState& state, TileElement* inputTileElement, int32_t direction)
{
    auto scorePos = CoordsXY{ CoordsXY{ state.Proximity } + CoordsDirectionDelta[direction] };
    if (!MapIsLocationValid(scorePos))
        return;

    TileElement* tileElement = MapGetFirstElementAt(scorePos);
    if (tileElement == nullptr)
        return;
    do
    {
        if (tileElement->IsGhost())
            continue;

        switch (tileElement->GetType())
        {
            case TileElementType::Surface:
                if (state.ProximityBaseHeight <= inputTileElement->BaseHeight)
                {
                    if (inputTileElement->ClearanceHeight <= tileElement->BaseHeight)
                    {
                        proximity_score_increment(state, PROXIMITY_SURFACE_SIDE_CLOSE);
                    }
                }
                break;
            case TileElementType::Path:
                if (abs(inputTileElement->GetBaseZ() - tileElement->GetBaseZ()) <= 2 * kCoordsZStep)
                {
                    proximity_score_increment(state, PROXIMITY_PATH_SIDE_CLOSE);
                }
                break;
            case TileElementType::Track:
                if (inputTileElement->AsTrack()->GetRideIndex() != tileElement->AsTrack()->GetRideIndex())
                {
                    if (abs(inputTileElement->GetBaseZ() - tileElement->GetBaseZ()) <= 2 * kCoordsZStep)
                    {
                        proximity_score_increment(state, PROXIMITY_FOREIGN_TRACK_SIDE_CLOSE);
                    }
                }
                break;
            case TileElementType::SmallScenery:
            case TileElementType::LargeScenery:
                if (tileElement->GetBaseZ() < inputTileElement->GetClearanceZ())
                {
                    if (inputTileElement->GetBaseZ() > tileElement->GetClearanceZ())
                    {
                        proximity_score_increment(state, PROXIMITY_SCENERY_SIDE_ABOVE);
                    }
                    else
                    {
                        proximity_score_increment(state, PROXIMITY_SCENERY_SIDE_BELOW);
                    }
                }
                break;
            default:
                break;
        }
    } while (!(tileElement++)->IsLastForTile());
}

static void ride_ratings_score_close_proximity_loops_helper(RideRating::UpdateState& state, const CoordsXYE& coordsElement)
{
    TileElement* tileElement = MapGetFirstElementAt(coordsElement);
    if (tileElement == nullptr)
        return;
    do
    {
        if (tileElement->IsGhost())
            continue;

        auto type = tileElement->GetType();
        if (type == TileElementType::Path)
        {
            int32_t zDiff = static_cast<int32_t>(tileElement->BaseHeight)
                - static_cast<int32_t>(coordsElement.element->BaseHeight);
            if (zDiff >= 0 && zDiff <= 16)
            {
                proximity_score_increment(state, PROXIMITY_PATH_TROUGH_VERTICAL_LOOP);
            }
        }
        else if (type == TileElementType::Track)
        {
            bool elementsAreAt90DegAngle = ((tileElement->GetDirection() ^ coordsElement.element->GetDirection()) & 1) != 0;
            if (elementsAreAt90DegAngle)
            {
                int32_t zDiff = static_cast<int32_t>(tileElement->BaseHeight)
                    - static_cast<int32_t>(coordsElement.element->BaseHeight);
                if (zDiff >= 0 && zDiff <= 16)
                {
                    proximity_score_increment(state, PROXIMITY_TRACK_THROUGH_VERTICAL_LOOP);
                    if (tileElement->AsTrack()->GetTrackType() == TrackElemType::leftVerticalLoop
                        || tileElement->AsTrack()->GetTrackType() == TrackElemType::rightVerticalLoop)
                    {
                        proximity_score_increment(state, PROXIMITY_INTERSECTING_VERTICAL_LOOP);
                    }
                }
            }
        }
    } while (!(tileElement++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006B62DA
 */
static void ride_ratings_score_close_proximity_loops(RideRating::UpdateState& state, TileElement* inputTileElement)
{
    auto trackType = inputTileElement->AsTrack()->GetTrackType();
    if (trackType == TrackElemType::leftVerticalLoop || trackType == TrackElemType::rightVerticalLoop)
    {
        ride_ratings_score_close_proximity_loops_helper(state, { state.Proximity, inputTileElement });

        int32_t direction = inputTileElement->GetDirection();
        ride_ratings_score_close_proximity_loops_helper(
            state, { CoordsXY{ state.Proximity } + CoordsDirectionDelta[direction], inputTileElement });
    }
}

/**
 *
 *  rct2: 0x006B5F9D
 */
static void ride_ratings_score_close_proximity(RideRating::UpdateState& state, TileElement* inputTileElement)
{
    if (state.StationFlags & RIDE_RATING_STATION_FLAG_NO_ENTRANCE)
    {
        return;
    }

    state.ProximityTotal++;
    TileElement* tileElement = MapGetFirstElementAt(state.Proximity);
    if (tileElement == nullptr)
        return;
    do
    {
        if (tileElement->IsGhost())
            continue;

        int32_t waterHeight;
        switch (tileElement->GetType())
        {
            case TileElementType::Surface:
                state.ProximityBaseHeight = tileElement->BaseHeight;
                if (tileElement->GetBaseZ() == state.Proximity.z)
                {
                    proximity_score_increment(state, PROXIMITY_SURFACE_TOUCH);
                }
                waterHeight = tileElement->AsSurface()->GetWaterHeight();
                if (waterHeight != 0)
                {
                    auto z = waterHeight;
                    if (z <= state.Proximity.z)
                    {
                        proximity_score_increment(state, PROXIMITY_WATER_OVER);
                        if (z == state.Proximity.z)
                        {
                            proximity_score_increment(state, PROXIMITY_WATER_TOUCH);
                        }
                        z += 16;
                        if (z == state.Proximity.z)
                        {
                            proximity_score_increment(state, PROXIMITY_WATER_LOW);
                        }
                        z += 112;
                        if (z <= state.Proximity.z)
                        {
                            proximity_score_increment(state, PROXIMITY_WATER_HIGH);
                        }
                    }
                }
                break;
            case TileElementType::Path:
                if (!tileElement->AsPath()->IsQueue())
                {
                    if (tileElement->GetClearanceZ() == inputTileElement->GetBaseZ())
                    {
                        proximity_score_increment(state, PROXIMITY_PATH_TOUCH_ABOVE);
                    }
                    if (tileElement->GetBaseZ() == inputTileElement->GetClearanceZ())
                    {
                        proximity_score_increment(state, PROXIMITY_PATH_TOUCH_UNDER);
                    }
                }
                else
                {
                    if (tileElement->GetClearanceZ() <= inputTileElement->GetBaseZ())
                    {
                        proximity_score_increment(state, PROXIMITY_QUEUE_PATH_OVER);
                    }
                    if (tileElement->GetClearanceZ() == inputTileElement->GetBaseZ())
                    {
                        proximity_score_increment(state, PROXIMITY_QUEUE_PATH_TOUCH_ABOVE);
                    }
                    if (tileElement->GetBaseZ() == inputTileElement->GetClearanceZ())
                    {
                        proximity_score_increment(state, PROXIMITY_QUEUE_PATH_TOUCH_UNDER);
                    }
                }
                break;
            case TileElementType::Track:
            {
                auto trackType = tileElement->AsTrack()->GetTrackType();
                if (trackType == TrackElemType::leftVerticalLoop || trackType == TrackElemType::rightVerticalLoop)
                {
                    int32_t sequence = tileElement->AsTrack()->GetSequenceIndex();
                    if (sequence == 3 || sequence == 6)
                    {
                        if (tileElement->BaseHeight - inputTileElement->ClearanceHeight <= 10)
                        {
                            proximity_score_increment(state, PROXIMITY_THROUGH_VERTICAL_LOOP);
                        }
                    }
                }
                if (inputTileElement->AsTrack()->GetRideIndex() != tileElement->AsTrack()->GetRideIndex())
                {
                    proximity_score_increment(state, PROXIMITY_FOREIGN_TRACK_ABOVE_OR_BELOW);
                    if (tileElement->GetClearanceZ() == inputTileElement->GetBaseZ())
                    {
                        proximity_score_increment(state, PROXIMITY_FOREIGN_TRACK_TOUCH_ABOVE);
                    }
                    if (tileElement->ClearanceHeight + 2 <= inputTileElement->BaseHeight)
                    {
                        if (tileElement->ClearanceHeight + 10 >= inputTileElement->BaseHeight)
                        {
                            proximity_score_increment(state, PROXIMITY_FOREIGN_TRACK_CLOSE_ABOVE);
                        }
                    }
                    if (inputTileElement->ClearanceHeight == tileElement->BaseHeight)
                    {
                        proximity_score_increment(state, PROXIMITY_FOREIGN_TRACK_TOUCH_ABOVE);
                    }
                    if (inputTileElement->ClearanceHeight + 2 == tileElement->BaseHeight)
                    {
                        if (static_cast<uint8_t>(inputTileElement->ClearanceHeight + 10) >= tileElement->BaseHeight)
                        {
                            proximity_score_increment(state, PROXIMITY_FOREIGN_TRACK_CLOSE_ABOVE);
                        }
                    }
                }
                else
                {
                    bool isStation = tileElement->AsTrack()->IsStation();
                    if (tileElement->ClearanceHeight == inputTileElement->BaseHeight)
                    {
                        proximity_score_increment(state, PROXIMITY_OWN_TRACK_TOUCH_ABOVE);
                        if (isStation)
                        {
                            proximity_score_increment(state, PROXIMITY_OWN_STATION_TOUCH_ABOVE);
                        }
                    }
                    if (tileElement->ClearanceHeight + 2 <= inputTileElement->BaseHeight)
                    {
                        if (tileElement->ClearanceHeight + 10 >= inputTileElement->BaseHeight)
                        {
                            proximity_score_increment(state, PROXIMITY_OWN_TRACK_CLOSE_ABOVE);
                            if (isStation)
                            {
                                proximity_score_increment(state, PROXIMITY_OWN_STATION_CLOSE_ABOVE);
                            }
                        }
                    }

                    if (inputTileElement->GetClearanceZ() == tileElement->GetBaseZ())
                    {
                        proximity_score_increment(state, PROXIMITY_OWN_TRACK_TOUCH_ABOVE);
                        if (isStation)
                        {
                            proximity_score_increment(state, PROXIMITY_OWN_STATION_TOUCH_ABOVE);
                        }
                    }
                    if (inputTileElement->ClearanceHeight + 2 <= tileElement->BaseHeight)
                    {
                        if (inputTileElement->ClearanceHeight + 10 >= tileElement->BaseHeight)
                        {
                            proximity_score_increment(state, PROXIMITY_OWN_TRACK_CLOSE_ABOVE);
                            if (isStation)
                            {
                                proximity_score_increment(state, PROXIMITY_OWN_STATION_CLOSE_ABOVE);
                            }
                        }
                    }
                }
                break;
            }
            default:
                break;
        } // switch tileElement->GetType
    } while (!(tileElement++)->IsLastForTile());

    uint8_t direction = inputTileElement->GetDirection();
    ride_ratings_score_close_proximity_in_direction(state, inputTileElement, (direction + 1) & 3);
    ride_ratings_score_close_proximity_in_direction(state, inputTileElement, (direction - 1) & 3);
    ride_ratings_score_close_proximity_loops(state, inputTileElement);

    if (TrackTypeIsBrakes(state.ProximityTrackType))
        state.AmountOfBrakes++;
    else if (TrackTypeIsBooster(state.ProximityTrackType))
        state.amountOfBoosters++;
    else if (TrackTypeIsReverser(state.ProximityTrackType))
        state.AmountOfReversers++;
}

static void RideRatingsCalculate(RideRating::UpdateState& state, Ride& ride)
{
    const auto& rrd = ride.getRideTypeDescriptor().RatingsData;

    switch (rrd.Type)
    {
        case RatingsCalculationType::Normal:
            if (!(ride.lifecycleFlags & RIDE_LIFECYCLE_TESTED))
                return;
            break;
        case RatingsCalculationType::FlatRide:
            ride.lifecycleFlags |= RIDE_LIFECYCLE_TESTED;
            ride.lifecycleFlags |= RIDE_LIFECYCLE_NO_RAW_STATS;
            break;
        case RatingsCalculationType::Stall:
            ride.upkeepCost = RideComputeUpkeep(state, ride);
            ride.windowInvalidateFlags.set(RideInvalidateFlag::income);
            // Exit ratings
            return;
    }

    ride.unreliabilityFactor = rrd.Unreliability;
    SetUnreliabilityFactor(ride);

    const auto shelteredEighths = GetNumOfShelteredEighths(ride);
    ride.shelteredEighths = (rrd.RideShelter == kDynamicRideShelterRating) ? shelteredEighths.TotalShelteredEighths
                                                                           : rrd.RideShelter;

    RideRating::Tuple ratings = rrd.BaseRatings;
    // Apply Modifiers
    for (const auto& modifier : rrd.Modifiers)
    {
        switch (modifier.type)
        {
            case RatingsModifierType::BonusLength:
                RideRatingsApplyBonusLength(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusSynchronisation:
                RideRatingsApplyBonusSynchronisation(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusTrainLength:
                RideRatingsApplyBonusTrainLength(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusMaxSpeed:
                RideRatingsApplyBonusMaxSpeed(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusAverageSpeed:
                RideRatingsApplyBonusAverageSpeed(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusDuration:
                RideRatingsApplyBonusDuration(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusGForces:
                RideRatingsApplyBonusGForces(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusTurns:
                RideRatingsApplyBonusTurns(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusDrops:
                RideRatingsApplyBonusDrops(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusSheltered:
                RideRatingsApplyBonusSheltered(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusProximity:
                RideRatingsApplyBonusProximity(ratings, ride, state, modifier);
                break;
            case RatingsModifierType::BonusScenery:
                RideRatingsApplyBonusScenery(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusRotations:
                RideRatingsApplyBonusRotations(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusOperationOption:
                RideRatingsApplyBonusOperationOption(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusReversedTrains:
                RideRatingsApplyBonusReversedTrains(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusGoKartRace:
                RideRatingsApplyBonusGoKartRace(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusTowerRide:
                RideRatingsApplyBonusTowerRide(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusRotoDrop:
                RideRatingsApplyBonusRotoDrop(ratings, ride);
                break;
            case RatingsModifierType::BonusMazeSize:
                RideRatingsApplyBonusMazeSize(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusBoatHireNoCircuit:
                RideRatingsApplyBonusBoatHireNoCircuit(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusSlideUnlimitedRides:
                RideRatingsApplyBonusSlideUnlimitedRides(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusMotionSimulatorMode:
                RideRatingsApplyBonusMotionSimulatorMode(ratings, ride, modifier);
                break;
            case RatingsModifierType::Bonus3DCinemaMode:
                RideRatingsApplyBonus3DCinemaMode(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusTopSpinMode:
                RideRatingsApplyBonusTopSpinMode(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusReversals:
                RideRatingsApplyBonusReversals(ratings, ride, state, modifier);
                break;
            case RatingsModifierType::BonusHoles:
                RideRatingsApplyBonusHoles(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusNumTrains:
                RideRatingsApplyBonusNumTrains(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusDownwardLaunch:
                RideRatingsApplyBonusDownwardLaunch(ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusLaunchedFreefallSpecial:
                RideRatingsApplyBonusLaunchedFreefallSpecial(ratings, ride, state, modifier);
                break;
            case RatingsModifierType::RequirementLength:
                RideRatingsApplyRequirementLength(ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementMaxSpeed:
                RideRatingsApplyRequirementMaxSpeed(ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementLateralGs:
                RideRatingsApplyRequirementLateralGs(ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementInversions:
                RideRatingsApplyRequirementInversions(ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementUnsheltered:
                RideRatingsApplyRequirementUnsheltered(ratings, ride, shelteredEighths.TrackShelteredEighths, modifier);
                break;
            case RatingsModifierType::RequirementReversals:
                RideRatingsApplyRequirementReversals(ratings, ride, state, modifier);
                break;
            case RatingsModifierType::RequirementHoles:
                RideRatingsApplyRequirementHoles(ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementStations:
                RideRatingsApplyRequirementStations(ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementSplashdown:
                RideRatingsApplyRequirementSplashdown(ratings, ride, modifier);
                break;
            case RatingsModifierType::PenaltyLateralGs:
                RideRatingsApplyPenaltyLateralGs(ratings, ride, modifier);
                break;
            default:
                break;
        }

        // Requirements that may be ignored if the ride has inversions
        if (ride.numInversions == 0 || !rrd.RelaxRequirementsIfInversions)
        {
            switch (modifier.type)
            {
                case RatingsModifierType::RequirementDropHeight:
                    RideRatingsApplyRequirementDropHeight(ratings, ride, modifier);
                    break;
                case RatingsModifierType::RequirementNumDrops:
                    RideRatingsApplyRequirementNumDrops(ratings, ride, modifier);
                    break;
                case RatingsModifierType::RequirementNegativeGs:
                    RideRatingsApplyRequirementNegativeGs(ratings, ride, modifier);
                    break;
                default:
                    break;
            }
        }
    }
    // Universl ratings adjustments
    RideRatingsApplyIntensityPenalty(ratings);
    RideRatingsApplyAdjustments(ride, ratings);
    if (ride.ratings != ratings)
    {
        ride.ratings = ratings;
        ride.windowInvalidateFlags.set(RideInvalidateFlag::ratings);
    }

    ride.upkeepCost = RideComputeUpkeep(state, ride);
    ride.windowInvalidateFlags.set(RideInvalidateFlag::income);

#ifdef ORIGINAL_RATINGS
    if (!ride.ratings.isNull())
    {
        // Address underflows allowed by original RCT2 code
        ride.ratings.excitement = std::max<uint16_t>(0, ride.ratings.excitement);
        ride.ratings.intensity = std::max<uint16_t>(0, ride.ratings.intensity);
        ride.ratings.nausea = std::max<uint16_t>(0, ride.ratings.nausea);
    }
#endif

#ifdef ENABLE_SCRIPTING
    // Only call the 'ride.ratings.calculate' API hook if testing of the ride is complete
    if (ride.lifecycleFlags & RIDE_LIFECYCLE_TESTED)
    {
        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        if (hookEngine.HasSubscriptions(HookType::rideRatingsCalculate))
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto originalRatings = ride.ratings;

            // Create event args object
            auto obj = DukObject(ctx);
            obj.Set("rideId", ride.id.ToUnderlying());
            obj.Set("excitement", originalRatings.excitement);
            obj.Set("intensity", originalRatings.intensity);
            obj.Set("nausea", originalRatings.nausea);

            // Call the subscriptions
            auto e = obj.Take();
            hookEngine.Call(HookType::rideRatingsCalculate, e, true);

            auto scriptExcitement = AsOrDefault(e["excitement"], static_cast<int32_t>(originalRatings.excitement));
            auto scriptIntensity = AsOrDefault(e["intensity"], static_cast<int32_t>(originalRatings.intensity));
            auto scriptNausea = AsOrDefault(e["nausea"], static_cast<int32_t>(originalRatings.nausea));

            ride.ratings.excitement = std::clamp<int32_t>(scriptExcitement, 0, INT16_MAX);
            ride.ratings.intensity = std::clamp<int32_t>(scriptIntensity, 0, INT16_MAX);
            ride.ratings.nausea = std::clamp<int32_t>(scriptNausea, 0, INT16_MAX);
        }
    }
#endif
}

static void RideRatingsCalculateValue(Ride& ride)
{
    struct Row
    {
        int32_t months, multiplier, divisor, summand;
    };
    static constexpr auto kAgeTable = std::to_array<Row>({
        { 5, 1, 1, 30 },      // +30
        { 13, 1, 1, 10 },     // +10
        { 40, 1, 1, 0 },      // 1x
        { 64, 3, 4, 0 },      // 0.75x
        { 88, 9, 16, 0 },     // 0.56x
        { 104, 27, 64, 0 },   // 0.42x
        { 120, 81, 256, 0 },  // 0.32x
        { 128, 81, 512, 0 },  // 0.16x
        { 200, 81, 1024, 0 }, // 0.08x
        { 200, 9, 16, 0 },    // 0.56x "easter egg"
    });

    if (!RideHasRatings(ride))
    {
        return;
    }

    // Start with the base ratings, multiplied by the ride type specific weights for excitement, intensity and nausea.
    const auto& ratingsMultipliers = ride.getRideTypeDescriptor().RatingsMultipliers;
    money64 value = (((ride.ratings.excitement * ratingsMultipliers.excitement) * 32) >> 15)
        + (((ride.ratings.intensity * ratingsMultipliers.intensity) * 32) >> 15)
        + (((ride.ratings.nausea * ratingsMultipliers.nausea) * 32) >> 15);

    int32_t monthsOld = 0;
    if (!getGameState().cheats.disableRideValueAging)
    {
        monthsOld = ride.getAge();
    }

    Row lastRow = kAgeTable[kAgeTable.size() - 1];

    // Ride is older than oldest age in the table?
    if (monthsOld >= lastRow.months)
    {
        value = (value * lastRow.multiplier) / lastRow.divisor + lastRow.summand;
    }
    else
    {
        // Find the first hit in the table that matches this ride's age
        for (const Row& curr : kAgeTable)
        {
            if (monthsOld < curr.months)
            {
                value = (value * curr.multiplier) / curr.divisor + curr.summand;
                break;
            }
        }
    }

    // Other ride of same type penalty
    const auto& gameState = getGameState();
    const auto& rideManager = RideManager(gameState);
    auto rideType = ride.type;
    auto otherRidesOfSameType = std::count_if(rideManager.begin(), rideManager.end(), [rideType](const Ride& r) {
        return r.status == RideStatus::open && r.type == rideType;
    });
    if (otherRidesOfSameType > 1)
        value -= value / 4;

    ride.value = std::max(0.00_GBP, value);
}

/**
 * I think this function computes ride upkeep? Though it is weird that the
 *  rct2: Sub65E621
 * inputs
 * - edi: ride ptr
 */
static money64 RideComputeUpkeep(RideRating::UpdateState& state, const Ride& ride)
{
    // data stored at 0x0057E3A8, incrementing 18 bytes at a time
    auto upkeep = ride.getRideTypeDescriptor().UpkeepCosts.BaseCost;

    auto trackCost = ride.getRideTypeDescriptor().UpkeepCosts.CostPerTrackPiece;
    upkeep += trackCost * ride.numPoweredLifts;

    uint32_t totalLength = ToHumanReadableRideLength(ride.getTotalLength());

    // The data originally here was 20's and 0's. The 20's all represented
    // rides that had tracks. The 0's were fixed rides like crooked house or
    // dodgems.
    // Data source is 0x0097E3AC
    totalLength *= ride.getRideTypeDescriptor().UpkeepCosts.TrackLengthMultiplier;
    upkeep += static_cast<uint16_t>(totalLength >> 10);

    if (ride.lifecycleFlags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
    {
        // The original code read from a table starting at 0x0097E3AE and
        // incrementing by 0x12 bytes between values. However, all of these
        // values were 40. I have replaced the table lookup with the constant
        // 40 in this case.
        upkeep += 40;
    }

    // Add maintenance cost for reverser track pieces
    upkeep += 10 * state.AmountOfReversers;

    // Add maintenance cost for brake track pieces
    upkeep += 20 * state.AmountOfBrakes;

    // Add maintenance cost for booster track pieces
    upkeep += 80 * state.amountOfBoosters;

    // these seem to be adhoc adjustments to a ride's upkeep/cost, times
    // various variables set on the ride itself.

    // https://gist.github.com/kevinburke/e19b803cd2769d96c540
    upkeep += ride.getRideTypeDescriptor().UpkeepCosts.CostPerTrain * ride.numTrains;
    upkeep += ride.getRideTypeDescriptor().UpkeepCosts.CostPerCar * ride.numCarsPerTrain;

    // slight upkeep boosts for some rides - 5 for mini railway, 10 for log
    // flume/rapids, 10 for roller coaster, 28 for giga coaster
    upkeep += ride.getRideTypeDescriptor().UpkeepCosts.CostPerStation * ride.numStations;

    if (ride.mode == RideMode::reverseInclineLaunchedShuttle)
    {
        upkeep += 30;
    }
    else if (ride.mode == RideMode::poweredLaunchPasstrough)
    {
        upkeep += 160;
    }
    else if (ride.mode == RideMode::limPoweredLaunch)
    {
        upkeep += 320;
    }
    else if (ride.mode == RideMode::poweredLaunch || ride.mode == RideMode::poweredLaunchBlockSectioned)
    {
        upkeep += 220;
    }

    // multiply by 5/8
    upkeep *= 10;
    upkeep >>= 4;
    return upkeep;
}

/**
 *
 *  rct2: 0x0065E7FB
 *
 * inputs
 * - bx: excitement
 * - cx: intensity
 * - bp: nausea
 * - edi: ride ptr
 */
static void RideRatingsApplyAdjustments(const Ride& ride, RideRating::Tuple& ratings)
{
    const auto* rideEntry = GetRideEntryByIndex(ride.subtype);

    if (rideEntry == nullptr)
    {
        return;
    }

    // Apply ride entry multipliers
    RideRatingsAdd(
        ratings, ((static_cast<int32_t>(ratings.excitement) * rideEntry->excitement_multiplier) >> 7),
        ((static_cast<int32_t>(ratings.intensity) * rideEntry->intensity_multiplier) >> 7),
        ((static_cast<int32_t>(ratings.nausea) * rideEntry->nausea_multiplier) >> 7));

    // Apply total air time
#ifdef ORIGINAL_RATINGS
    if (ride.getRideTypeDescriptor().HasFlag(RtdFlag::hasAirTime))
    {
        uint16_t totalAirTime = ride.totalAirTime;
        if (rideEntry->flags & RIDE_ENTRY_FLAG_LIMIT_AIRTIME_BONUS)
        {
            if (totalAirTime >= 96)
            {
                totalAirTime -= 96;
                ratings.excitement -= totalAirTime / 8;
                ratings.nausea += totalAirTime / 16;
            }
        }
        else
        {
            ratings.excitement += totalAirTime / 8;
            ratings.nausea += totalAirTime / 16;
        }
    }
#else
    if (ride.getRideTypeDescriptor().HasFlag(RtdFlag::hasAirTime))
    {
        int32_t excitementModifier;
        if (rideEntry->flags & RIDE_ENTRY_FLAG_LIMIT_AIRTIME_BONUS)
        {
            // Limit airtime bonus for heartline twister coaster (see issues #2031 and #2064)
            excitementModifier = std::min<uint16_t>(ride.totalAirTime, 96) / 8;
        }
        else
        {
            excitementModifier = ride.totalAirTime / 8;
        }
        int32_t nauseaModifier = ride.totalAirTime / 16;

        RideRatingsAdd(ratings, excitementModifier, 0, nauseaModifier);
    }
#endif
}

/**
 * Lowers excitement, the higher the intensity.
 *  rct2: 0x0065E7A3
 */
static void RideRatingsApplyIntensityPenalty(RideRating::Tuple& ratings)
{
    static constexpr RideRating_t intensityBounds[] = { 1000, 1100, 1200, 1320, 1450 };
    RideRating_t excitement = ratings.excitement;
    for (auto intensityBound : intensityBounds)
    {
        if (ratings.intensity >= intensityBound)
        {
            excitement -= excitement / 4;
        }
    }
    ratings.excitement = excitement;
}

/**
 *
 *  rct2: 0x00655FD6
 */
static void SetUnreliabilityFactor(Ride& ride)
{
    const auto& rtd = ride.getRideTypeDescriptor();
    // Special unreliability for a few ride types
    if (rtd.HasFlag(RtdFlag::reverseInclineLaunchAffectsReliability) && ride.mode == RideMode::reverseInclineLaunchedShuttle)
    {
        ride.unreliabilityFactor += 10;
    }
    else if (rtd.HasFlag(RtdFlag::poweredLaunchAffectsReliability) && ride.isPoweredLaunched())
    {
        ride.unreliabilityFactor += 5;
    }
    else if (rtd.HasFlag(RtdFlag::runningSpeedAffectsReliability))
    {
        ride.unreliabilityFactor += (ride.speed * 2);
    }
    // The bigger the difference in lift speed and minimum the higher the unreliability
    uint8_t minLiftSpeed = ride.getRideTypeDescriptor().LiftData.minimum_speed;
    ride.unreliabilityFactor += (ride.liftHillSpeed - minLiftSpeed) * 2;
}

static uint32_t get_proximity_score_helper_1(uint16_t x, uint16_t max, uint32_t multiplier)
{
    return (std::min(x, max) * multiplier) >> 16;
}

static uint32_t get_proximity_score_helper_2(uint16_t x, uint16_t additionIfNotZero, uint16_t max, uint32_t multiplier)
{
    uint32_t result = x;
    if (result != 0)
        result += additionIfNotZero;
    return (std::min<int32_t>(result, max) * multiplier) >> 16;
}

static uint32_t get_proximity_score_helper_3(uint16_t x, uint16_t resultIfNotZero)
{
    return x == 0 ? 0 : resultIfNotZero;
}

/**
 *
 *  rct2: 0x0065E277
 */
static uint32_t ride_ratings_get_proximity_score(RideRating::UpdateState& state)
{
    const uint16_t* scores = state.ProximityScores;

    uint32_t result = 0;
    result += get_proximity_score_helper_1(scores[PROXIMITY_WATER_OVER], 60, 0x00AAAA);
    result += get_proximity_score_helper_1(scores[PROXIMITY_WATER_TOUCH], 22, 0x0245D1);
    result += get_proximity_score_helper_1(scores[PROXIMITY_WATER_LOW], 10, 0x020000);
    result += get_proximity_score_helper_1(scores[PROXIMITY_WATER_HIGH], 40, 0x00A000);
    result += get_proximity_score_helper_1(scores[PROXIMITY_SURFACE_TOUCH], 70, 0x01B6DB);
    result += get_proximity_score_helper_1(scores[PROXIMITY_QUEUE_PATH_OVER] + 8, 12, 0x064000);
    result += get_proximity_score_helper_3(scores[PROXIMITY_QUEUE_PATH_TOUCH_ABOVE], 40);
    result += get_proximity_score_helper_3(scores[PROXIMITY_QUEUE_PATH_TOUCH_UNDER], 45);
    result += get_proximity_score_helper_2(scores[PROXIMITY_PATH_TOUCH_ABOVE], 10, 20, 0x03C000);
    result += get_proximity_score_helper_2(scores[PROXIMITY_PATH_TOUCH_UNDER], 10, 20, 0x044000);
    result += get_proximity_score_helper_2(scores[PROXIMITY_OWN_TRACK_TOUCH_ABOVE], 10, 15, 0x035555);
    result += get_proximity_score_helper_1(scores[PROXIMITY_OWN_TRACK_CLOSE_ABOVE], 5, 0x060000);
    result += get_proximity_score_helper_2(scores[PROXIMITY_FOREIGN_TRACK_ABOVE_OR_BELOW], 10, 15, 0x02AAAA);
    result += get_proximity_score_helper_2(scores[PROXIMITY_FOREIGN_TRACK_TOUCH_ABOVE], 10, 15, 0x04AAAA);
    result += get_proximity_score_helper_1(scores[PROXIMITY_FOREIGN_TRACK_CLOSE_ABOVE], 5, 0x090000);
    result += get_proximity_score_helper_1(scores[PROXIMITY_SCENERY_SIDE_BELOW], 35, 0x016DB6);
    result += get_proximity_score_helper_1(scores[PROXIMITY_SCENERY_SIDE_ABOVE], 35, 0x00DB6D);
    result += get_proximity_score_helper_3(scores[PROXIMITY_OWN_STATION_TOUCH_ABOVE], 55);
    result += get_proximity_score_helper_3(scores[PROXIMITY_OWN_STATION_CLOSE_ABOVE], 25);
    result += get_proximity_score_helper_2(scores[PROXIMITY_TRACK_THROUGH_VERTICAL_LOOP], 4, 6, 0x140000);
    result += get_proximity_score_helper_2(scores[PROXIMITY_PATH_TROUGH_VERTICAL_LOOP], 4, 6, 0x0F0000);
    result += get_proximity_score_helper_3(scores[PROXIMITY_INTERSECTING_VERTICAL_LOOP], 100);
    result += get_proximity_score_helper_2(scores[PROXIMITY_THROUGH_VERTICAL_LOOP], 4, 6, 0x0A0000);
    result += get_proximity_score_helper_2(scores[PROXIMITY_PATH_SIDE_CLOSE], 10, 20, 0x01C000);
    result += get_proximity_score_helper_2(scores[PROXIMITY_FOREIGN_TRACK_SIDE_CLOSE], 10, 20, 0x024000);
    result += get_proximity_score_helper_2(scores[PROXIMITY_SURFACE_SIDE_CLOSE], 10, 20, 0x028000);
    return result;
}

/**
 * Calculates how much of the track is sheltered in eighths.
 *  rct2: 0x0065E72D
 */
static ShelteredEights GetNumOfShelteredEighths(const Ride& ride)
{
    int32_t totalLength = ride.getTotalLength();
    int32_t shelteredLength = ride.shelteredLength;
    int32_t lengthEighth = totalLength / 8;
    int32_t lengthCounter = lengthEighth;
    uint8_t numShelteredEighths = 0;
    for (int32_t i = 0; i < 7; i++)
    {
        if (shelteredLength >= lengthCounter)
        {
            lengthCounter += lengthEighth;
            numShelteredEighths++;
        }
    }

    uint8_t trackShelteredEighths = numShelteredEighths;
    const auto* rideType = GetRideEntryByIndex(ride.subtype);
    if (rideType == nullptr)
    {
        return { 0, 0 };
    }
    if (rideType->flags & RIDE_ENTRY_FLAG_COVERED_RIDE)
        numShelteredEighths = 7;

    return { trackShelteredEighths, numShelteredEighths };
}

static RideRating::Tuple get_flat_turns_rating(const Ride& ride)
{
    int32_t num3PlusTurns = GetTurnCount3Elements(ride, 0);
    int32_t num2Turns = GetTurnCount2Elements(ride, 0);
    int32_t num1Turns = GetTurnCount1Element(ride, 0);

    RideRating::Tuple rating;
    rating.excitement = (num3PlusTurns * 0x28000) >> 16;
    rating.excitement += (num2Turns * 0x30000) >> 16;
    rating.excitement += (num1Turns * 63421) >> 16;

    rating.intensity = (num3PlusTurns * 81920) >> 16;
    rating.intensity += (num2Turns * 49152) >> 16;
    rating.intensity += (num1Turns * 21140) >> 16;

    rating.nausea = (num3PlusTurns * 0x50000) >> 16;
    rating.nausea += (num2Turns * 0x32000) >> 16;
    rating.nausea += (num1Turns * 42281) >> 16;

    return rating;
}

/**
 *
 *  rct2: 0x0065DF72
 */
static RideRating::Tuple get_banked_turns_rating(const Ride& ride)
{
    int32_t num3PlusTurns = GetTurnCount3Elements(ride, 1);
    int32_t num2Turns = GetTurnCount2Elements(ride, 1);
    int32_t num1Turns = GetTurnCount1Element(ride, 1);

    RideRating::Tuple rating;
    rating.excitement = (num3PlusTurns * 0x3C000) >> 16;
    rating.excitement += (num2Turns * 0x3C000) >> 16;
    rating.excitement += (num1Turns * 73992) >> 16;

    rating.intensity = (num3PlusTurns * 0x14000) >> 16;
    rating.intensity += (num2Turns * 49152) >> 16;
    rating.intensity += (num1Turns * 21140) >> 16;

    rating.nausea = (num3PlusTurns * 0x50000) >> 16;
    rating.nausea += (num2Turns * 0x32000) >> 16;
    rating.nausea += (num1Turns * 48623) >> 16;

    return rating;
}

/**
 *
 *  rct2: 0x0065E047
 */
static RideRating::Tuple get_sloped_turns_rating(const Ride& ride)
{
    RideRating::Tuple rating;

    int32_t num4PlusTurns = GetTurnCount4PlusElements(ride, 2);
    int32_t num3Turns = GetTurnCount3Elements(ride, 2);
    int32_t num2Turns = GetTurnCount2Elements(ride, 2);
    int32_t num1Turns = GetTurnCount1Element(ride, 2);

    rating.excitement = (std::min(num4PlusTurns, 4) * 0x78000) >> 16;
    rating.excitement += (std::min(num3Turns, 6) * 273066) >> 16;
    rating.excitement += (std::min(num2Turns, 6) * 0x3AAAA) >> 16;
    rating.excitement += (std::min(num1Turns, 7) * 187245) >> 16;
    rating.intensity = 0;
    rating.nausea = (std::min(num4PlusTurns, 8) * 0x78000) >> 16;

    return rating;
}

/**
 *
 *  rct2: 0x0065E0F2
 */
static RideRating::Tuple getInversionsRatings(uint16_t inversions)
{
    RideRating::Tuple rating;

    rating.excitement = (std::min<int32_t>(inversions, 6) * 0x1AAAAA) >> 16;
    rating.intensity = (inversions * 0x320000) >> 16;
    rating.nausea = (inversions * 0x15AAAA) >> 16;

    return rating;
}

void SpecialTrackElementRatingsAjustment_Default(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea)
{
    if (ride.hasWaterSplash())
    {
        excitement += 50;
        intensity += 30;
        nausea += 20;
    }
    if (ride.hasWaterfall())
    {
        excitement += 55;
        intensity += 30;
    }
    if (ride.hasWhirlpool())
    {
        excitement += 35;
        intensity += 20;
        nausea += 23;
    }
}

void SpecialTrackElementRatingsAjustment_GhostTrain(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea)
{
    if (ride.hasSpinningTunnel())
    {
        excitement += 40;
        intensity += 25;
        nausea += 55;
    }
}

void SpecialTrackElementRatingsAjustment_LogFlume(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea)
{
    if (ride.hasLogReverser())
    {
        excitement += 48;
        intensity += 55;
        nausea += 65;
    }
}

static RideRating::Tuple GetSpecialTrackElementsRating(uint8_t type, const Ride& ride)
{
    int32_t excitement = 0, intensity = 0, nausea = 0;
    const auto& rtd = ride.getRideTypeDescriptor();
    rtd.SpecialElementRatingAdjustment(ride, excitement, intensity, nausea);

    auto helixSections = ride.numHelices;

    int32_t helixesUpTo9 = std::min<int32_t>(helixSections, 9);
    excitement += (helixesUpTo9 * 254862) >> 16;

    int32_t helixesUpTo11 = std::min<int32_t>(helixSections, 11);
    intensity += (helixesUpTo11 * 148945) >> 16;

    int32_t helixesOver5UpTo10 = std::clamp<int32_t>(helixSections - 5, 0, 10);
    nausea += (helixesOver5UpTo10 * 0x140000) >> 16;

    RideRating::Tuple rating = { static_cast<RideRating_t>(excitement), static_cast<RideRating_t>(intensity),
                                 static_cast<RideRating_t>(nausea) };
    return rating;
}

/**
 *
 *  rct2: 0x0065DDD1
 */
static RideRating::Tuple ride_ratings_get_turns_ratings(const Ride& ride)
{
    int32_t excitement = 0, intensity = 0, nausea = 0;

    RideRating::Tuple specialTrackElementsRating = GetSpecialTrackElementsRating(ride.type, ride);
    excitement += specialTrackElementsRating.excitement;
    intensity += specialTrackElementsRating.intensity;
    nausea += specialTrackElementsRating.nausea;

    RideRating::Tuple flatTurnsRating = get_flat_turns_rating(ride);
    excitement += flatTurnsRating.excitement;
    intensity += flatTurnsRating.intensity;
    nausea += flatTurnsRating.nausea;

    RideRating::Tuple bankedTurnsRating = get_banked_turns_rating(ride);
    excitement += bankedTurnsRating.excitement;
    intensity += bankedTurnsRating.intensity;
    nausea += bankedTurnsRating.nausea;

    RideRating::Tuple slopedTurnsRating = get_sloped_turns_rating(ride);
    excitement += slopedTurnsRating.excitement;
    intensity += slopedTurnsRating.intensity;
    nausea += slopedTurnsRating.nausea;

    RideRating::Tuple inversionsRating = getInversionsRatings(ride.numInversions);
    excitement += inversionsRating.excitement;
    intensity += inversionsRating.intensity;
    nausea += inversionsRating.nausea;

    RideRating::Tuple rating = { static_cast<RideRating_t>(excitement), static_cast<RideRating_t>(intensity),
                                 static_cast<RideRating_t>(nausea) };
    return rating;
}

/**
 *
 *  rct2: 0x0065E1C2
 */
static RideRating::Tuple ride_ratings_get_sheltered_ratings(const Ride& ride)
{
    int32_t shelteredLengthShifted = (ride.shelteredLength) >> 16;

    uint32_t shelteredLengthUpTo1000 = std::min(shelteredLengthShifted, 1000);
    uint32_t shelteredLengthUpTo2000 = std::min(shelteredLengthShifted, 2000);

    int32_t excitement = (shelteredLengthUpTo1000 * 9175) >> 16;
    int32_t intensity = (shelteredLengthUpTo2000 * 0x2666) >> 16;
    int32_t nausea = (shelteredLengthUpTo1000 * 0x4000) >> 16;

    /*eax = (ride.var11C * 30340) >> 16;*/
    /*nausea += eax;*/

    if (ride.numShelteredSections & ShelteredSectionsBits::kBankingWhileSheltered)
    {
        excitement += 20;
        nausea += 15;
    }

    if (ride.numShelteredSections & ShelteredSectionsBits::kRotatingWhileSheltered)
    {
        excitement += 20;
        nausea += 15;
    }

    uint8_t lowerVal = ride.getNumShelteredSections();
    lowerVal = std::min<uint8_t>(lowerVal, 11);
    excitement += (lowerVal * 774516) >> 16;

    RideRating::Tuple rating = { static_cast<RideRating_t>(excitement), static_cast<RideRating_t>(intensity),
                                 static_cast<RideRating_t>(nausea) };
    return rating;
}

/**
 *
 *  rct2: 0x0065DCDC
 */
static RideRating::Tuple ride_ratings_get_gforce_ratings(const Ride& ride)
{
    RideRating::Tuple result = {
        .excitement = 0,
        .intensity = 0,
        .nausea = 0,
    };

    // Apply maximum positive G force factor
    result.excitement += (ride.maxPositiveVerticalG * 5242) >> 16;
    result.intensity += (ride.maxPositiveVerticalG * 52428) >> 16;
    result.nausea += (ride.maxPositiveVerticalG * 17039) >> 16;

    // Apply maximum negative G force factor
    fixed16_2dp gforce = ride.maxNegativeVerticalG;
    result.excitement += (std::clamp<fixed16_2dp>(gforce, -RideRating::make(2, 50), RideRating::make(0, 00)) * -15728) >> 16;
    result.intensity += ((gforce - RideRating::make(1, 00)) * -52428) >> 16;
    result.nausea += ((gforce - RideRating::make(1, 00)) * -14563) >> 16;

    // Apply lateral G force factor
    result.excitement += (std::min<fixed16_2dp>(RideRating::make(1, 50), ride.maxLateralG) * 26214) >> 16;
    result.intensity += ride.maxLateralG;
    result.nausea += (ride.maxLateralG * 21845) >> 16;

// Very high lateral G force penalty
#ifdef ORIGINAL_RATINGS
    if (ride.maxLateralG > MakeFixed16_2dp(2, 80))
    {
        result.intensity += RideRating::make(3, 75);
        result.nausea += RideRating::make(2, 00);
    }
    if (ride.maxLateralG > MakeFixed16_2dp(3, 10))
    {
        result.excitement /= 2;
        result.intensity += RideRating::make(8, 50);
        result.nausea += RideRating::make(4, 00);
    }
#endif

    return result;
}

/**
 *
 *  rct2: 0x0065E139
 */
static RideRating::Tuple ride_ratings_get_drop_ratings(const Ride& ride)
{
    RideRating::Tuple result = {
        /* .excitement = */ 0,
        /* .intensity = */ 0,
        /* .nausea = */ 0,
    };

    // Apply number of drops factor
    int32_t drops = ride.numDrops;
    result.excitement += (std::min(9, drops) * 728177) >> 16;
    result.intensity += (drops * 928426) >> 16;
    result.nausea += (drops * 655360) >> 16;

    // Apply highest drop factor
    RideRatingsAdd(
        result, ((ride.highestDropHeight * 2) * 16000) >> 16, ((ride.highestDropHeight * 2) * 32000) >> 16,
        ((ride.highestDropHeight * 2) * 10240) >> 16);

    return result;
}

/**
 * Calculates a score based on the surrounding scenery.
 *  rct2: 0x0065E557
 */
static int32_t ride_ratings_get_scenery_score(const Ride& ride)
{
    auto stationIndex = RideGetFirstValidStationStart(ride);
    CoordsXY location;

    if (stationIndex.IsNull())
    {
        return 0;
    }

    const auto& rtd = ride.getRideTypeDescriptor();
    if (rtd.specialType == RtdSpecialType::maze)
    {
        location = ride.getStation().Entrance.ToCoordsXY();
    }
    else
    {
        location = ride.getStation(stationIndex).Start;
    }

    int32_t z = TileElementHeight(location);

    // Check if station is underground, returns a fixed mediocre score since you can't have scenery underground
    if (z > ride.getStation(stationIndex).GetBaseZ())
    {
        return 40;
    }

    // Count surrounding scenery items
    int32_t numSceneryItems = 0;
    auto tileLocation = TileCoordsXY(location);
    auto& gameState = getGameState();
    for (int32_t yy = std::max(tileLocation.y - 5, 0); yy <= std::min(tileLocation.y + 5, gameState.mapSize.y - 1); yy++)
    {
        for (int32_t xx = std::max(tileLocation.x - 5, 0); xx <= std::min(tileLocation.x + 5, gameState.mapSize.x - 1); xx++)
        {
            // Count scenery items on this tile
            TileElement* tileElement = MapGetFirstElementAt(TileCoordsXY{ xx, yy });
            if (tileElement == nullptr)
                continue;
            do
            {
                if (tileElement->IsGhost())
                    continue;

                const auto type = tileElement->GetType();
                if (type == TileElementType::SmallScenery || type == TileElementType::LargeScenery)
                    numSceneryItems++;
            } while (!(tileElement++)->IsLastForTile());
        }
    }

    return std::min(numSceneryItems, 47) * 5;
}

#pragma region Ride rating calculation helpers

static void RideRatingsSet(RideRating::Tuple& ratings, int32_t excitement, int32_t intensity, int32_t nausea)
{
    ratings.excitement = 0;
    ratings.intensity = 0;
    ratings.nausea = 0;
    RideRatingsAdd(ratings, excitement, intensity, nausea);
}

/**
 * Add to a ride rating with overflow protection.
 */
static void RideRatingsAdd(RideRating::Tuple& ratings, int32_t excitement, int32_t intensity, int32_t nausea)
{
    int32_t newExcitement = ratings.excitement + excitement;
    int32_t newIntensity = ratings.intensity + intensity;
    int32_t newNausea = ratings.nausea + nausea;
    ratings.excitement = std::clamp<int32_t>(newExcitement, 0, INT16_MAX);
    ratings.intensity = std::clamp<int32_t>(newIntensity, 0, INT16_MAX);
    ratings.nausea = std::clamp<int32_t>(newNausea, 0, INT16_MAX);
}

static void RideRatingsApplyBonusLength(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(
        ratings, (std::min(ToHumanReadableRideLength(ride.getTotalLength()), modifier.threshold) * modifier.excitement) >> 16,
        0, 0);
}

static void RideRatingsApplyBonusSynchronisation(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if ((ride.departFlags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS) && RideHasAdjacentStation(ride))
    {
        RideRatingsAdd(ratings, modifier.excitement, modifier.intensity, modifier.nausea);
    }
}

static void RideRatingsApplyBonusTrainLength(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(ratings, ((ride.numCarsPerTrain - 1) * modifier.excitement) >> 16, 0, 0);
}

static void RideRatingsApplyBonusMaxSpeed(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    int32_t maxSpeedMod = ride.maxSpeed >> 16;
    RideRatingsAdd(
        ratings, (maxSpeedMod * modifier.excitement) >> 16, (maxSpeedMod * modifier.intensity) >> 16,
        (maxSpeedMod * modifier.nausea) >> 16);
}

static void RideRatingsApplyBonusAverageSpeed(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    int32_t avgSpeedMod = ride.averageSpeed >> 16;
    RideRatingsAdd(ratings, (avgSpeedMod * modifier.excitement) >> 16, (avgSpeedMod * modifier.intensity) >> 16, 0);
}

static void RideRatingsApplyBonusDuration(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(ratings, (std::min(ride.getTotalTime(), modifier.threshold) * modifier.excitement) >> 16, 0, 0);
}

static void RideRatingsApplyBonusGForces(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRating::Tuple subRating = ride_ratings_get_gforce_ratings(ride);
    RideRatingsAdd(
        ratings, (subRating.excitement * modifier.excitement) >> 16, (subRating.intensity * modifier.intensity) >> 16,
        (subRating.nausea * modifier.nausea) >> 16);
}

static void RideRatingsApplyBonusTurns(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRating::Tuple subRating = ride_ratings_get_turns_ratings(ride);
    RideRatingsAdd(
        ratings, (subRating.excitement * modifier.excitement) >> 16, (subRating.intensity * modifier.intensity) >> 16,
        (subRating.nausea * modifier.nausea) >> 16);
}

static void RideRatingsApplyBonusDrops(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRating::Tuple subRating = ride_ratings_get_drop_ratings(ride);
    RideRatingsAdd(
        ratings, (subRating.excitement * modifier.excitement) >> 16, (subRating.intensity * modifier.intensity) >> 16,
        (subRating.nausea * modifier.nausea) >> 16);
}

static void RideRatingsApplyBonusSheltered(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRating::Tuple subRating = ride_ratings_get_sheltered_ratings(ride);
    RideRatingsAdd(
        ratings, (subRating.excitement * modifier.excitement) >> 16, (subRating.intensity * modifier.intensity) >> 16,
        (subRating.nausea * modifier.nausea) >> 16);
}

static void RideRatingsApplyBonusRotations(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(
        ratings, ride.rotations * modifier.excitement, ride.rotations * modifier.intensity, ride.rotations * modifier.nausea);
}

static void RideRatingsApplyBonusOperationOption(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    int32_t intensity = (modifier.intensity >= 0) ? (ride.operationOption * modifier.intensity)
                                                  : (ride.operationOption / std::abs(modifier.intensity));
    RideRatingsAdd(ratings, ride.operationOption * modifier.excitement, intensity, ride.operationOption * modifier.nausea);
}

static void RideRatingsApplyBonusReversedTrains(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.hasLifecycleFlag(RIDE_LIFECYCLE_REVERSED_TRAINS))
    {
        RideRatingsAdd(
            ratings, ((ratings.excitement * modifier.excitement) >> 7), (ratings.intensity * modifier.intensity) >> 7,
            (ratings.nausea * modifier.nausea) >> 7);
    }
}

static void RideRatingsApplyBonusGoKartRace(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.mode == RideMode::race && ride.numTrains >= modifier.threshold)
    {
        RideRatingsAdd(ratings, modifier.excitement, modifier.intensity, modifier.nausea);

        int32_t lapsFactor = (ride.numLaps - 1) * 30;
        RideRatingsAdd(ratings, lapsFactor, lapsFactor / 2, 0);
    }
}

static void RideRatingsApplyBonusTowerRide(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    int32_t lengthFactor = ToHumanReadableRideLength(ride.getTotalLength());
    RideRatingsAdd(
        ratings, (lengthFactor * modifier.excitement) >> 16, (lengthFactor * modifier.intensity) >> 16,
        (lengthFactor * modifier.nausea) >> 16);
}

static void RideRatingsApplyBonusRotoDrop(RideRating::Tuple& ratings, const Ride& ride)
{
    int32_t lengthFactor = (ToHumanReadableRideLength(ride.getTotalLength()) * 209715) >> 16;
    RideRatingsAdd(ratings, lengthFactor, lengthFactor * 2, lengthFactor * 2);
}

static void RideRatingsApplyBonusMazeSize(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    int32_t size = std::min<uint16_t>(ride.mazeTiles, modifier.threshold);
    RideRatingsAdd(ratings, size * modifier.excitement, size * modifier.intensity, size * modifier.nausea);
}

static void RideRatingsApplyBonusBoatHireNoCircuit(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    // Most likely checking if the ride has does not have a circuit
    if (!(ride.lifecycleFlags & RIDE_LIFECYCLE_TESTED))
    {
        RideRatingsAdd(ratings, modifier.excitement, modifier.intensity, modifier.nausea);
    }
}

static void RideRatingsApplyBonusSlideUnlimitedRides(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.mode == RideMode::unlimitedRidesPerAdmission)
    {
        RideRatingsAdd(ratings, modifier.excitement, modifier.intensity, modifier.nausea);
    }
}

static void RideRatingsApplyBonusMotionSimulatorMode(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    // Hardcoded until ride mode refactor
    if (ride.mode == RideMode::filmThrillRiders)
    {
        RideRatingsSet(ratings, RideRating::make(3, 25), RideRating::make(4, 10), RideRating::make(3, 30));
    }
    else
    {
        RideRatingsSet(ratings, RideRating::make(2, 90), RideRating::make(3, 50), RideRating::make(3, 00));
    }
}

static void RideRatingsApplyBonus3DCinemaMode(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    // Hardcoded until ride mode refactor
    switch (ride.mode)
    {
        default:
        case RideMode::mouseTails3DFilm:
            RideRatingsSet(ratings, RideRating::make(3, 50), RideRating::make(2, 40), RideRating::make(1, 40));
            break;
        case RideMode::stormChasers3DFilm:
            RideRatingsSet(ratings, RideRating::make(4, 00), RideRating::make(2, 65), RideRating::make(1, 55));
            break;
        case RideMode::spaceRaiders3DFilm:
            RideRatingsSet(ratings, RideRating::make(4, 20), RideRating::make(2, 60), RideRating::make(1, 48));
            break;
    }
}

static void RideRatingsApplyBonusTopSpinMode(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    // Hardcoded until ride mode refactor
    switch (ride.mode)
    {
        default:
        case RideMode::beginners:
            RideRatingsSet(ratings, RideRating::make(2, 00), RideRating::make(4, 80), RideRating::make(5, 74));
            break;
        case RideMode::intense:
            RideRatingsSet(ratings, RideRating::make(3, 00), RideRating::make(5, 75), RideRating::make(6, 64));
            break;
        case RideMode::berserk:
            RideRatingsSet(ratings, RideRating::make(3, 20), RideRating::make(6, 80), RideRating::make(7, 94));
            break;
    }
}

static void RideRatingsApplyBonusReversals(
    RideRating::Tuple& ratings, const Ride& ride, RideRating::UpdateState& state, RatingsModifier modifier)
{
    int32_t numReversers = std::min<uint16_t>(state.AmountOfReversers, modifier.threshold);
    RideRatingsAdd(
        ratings, numReversers * modifier.excitement, numReversers * modifier.intensity, numReversers * modifier.nausea);
}

static void RideRatingsApplyBonusHoles(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    auto bonusHoles = std::min<uint8_t>(modifier.threshold, ride.numHoles);
    RideRatingsAdd(ratings, bonusHoles * modifier.excitement, bonusHoles * modifier.intensity, bonusHoles * modifier.nausea);
}

static void RideRatingsApplyBonusNumTrains(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    // For some reason the original code ran this twice, before and after the operation option bonus
    // Has been changed to call once with double value
    if (ride.numTrains >= modifier.threshold)
    {
        RideRatingsAdd(ratings, modifier.excitement, modifier.intensity, modifier.nausea);
    }
}

static void RideRatingsApplyBonusDownwardLaunch(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.mode == RideMode::downwardLaunch)
    {
        RideRatingsAdd(ratings, modifier.excitement, modifier.intensity, modifier.nausea);
    }
}

static void RideRatingsApplyBonusOperationOptionFreefall(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(
        ratings, (ride.operationOption * modifier.excitement) >> 16, (ride.operationOption * modifier.intensity) >> 16,
        (ride.operationOption * modifier.nausea) >> 16);
}

static void RideRatingsApplyBonusLaunchedFreefallSpecial(
    RideRating::Tuple& ratings, const Ride& ride, RideRating::UpdateState& state, RatingsModifier modifier)
{
    int32_t excitement = (ToHumanReadableRideLength(ride.getTotalLength()) * 32768) >> 16;
    RideRatingsAdd(ratings, excitement, 0, 0);

#ifdef ORIGINAL_RATINGS
    RideRatingsApplyBonusOperationOptionFreefall(ratings, ride, modifier);
#else
    // Only apply "launch speed" effects when the setting can be modified
    if (ride.mode == RideMode::upwardLaunch)
    {
        RideRatingsApplyBonusOperationOptionFreefall(ratings, ride, modifier);
    }
    else
    {
        // Fix #3282: When the ride mode is in downward launch mode, the intensity and
        //            nausea were fixed regardless of how high the ride is. The following
        //            calculation is based on roto-drop which is a similar mechanic.
        int32_t lengthFactor = (ToHumanReadableRideLength(ride.getTotalLength()) * 209715) >> 16;
        RideRatingsAdd(ratings, lengthFactor, lengthFactor * 2, lengthFactor * 2);
    }
#endif
}

static void RideRatingsApplyBonusProximity(
    RideRating::Tuple& ratings, const Ride& ride, RideRating::UpdateState& state, RatingsModifier modifier)
{
    RideRatingsAdd(ratings, (ride_ratings_get_proximity_score(state) * modifier.excitement) >> 16, 0, 0);
}

static void RideRatingsApplyBonusScenery(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(ratings, (ride_ratings_get_scenery_score(ride) * modifier.excitement) >> 16, 0, 0);
}

static void RideRatingsApplyRequirementLength(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.getStation().SegmentLength < modifier.threshold)
    {
        ratings.excitement /= modifier.excitement;
        ratings.intensity /= modifier.intensity;
        ratings.nausea /= modifier.nausea;
    }
}

static void RideRatingsApplyRequirementDropHeight(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.highestDropHeight < modifier.threshold)
    {
        ratings.excitement /= modifier.excitement;
        ratings.intensity /= modifier.intensity;
        ratings.nausea /= modifier.nausea;
    }
}

static void RideRatingsApplyRequirementMaxSpeed(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.maxSpeed < modifier.threshold)
    {
        ratings.excitement /= modifier.excitement;
        ratings.intensity /= modifier.intensity;
        ratings.nausea /= modifier.nausea;
    }
}

static void RideRatingsApplyRequirementNumDrops(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.numDrops < modifier.threshold)
    {
        ratings.excitement /= modifier.excitement;
        ratings.intensity /= modifier.intensity;
        ratings.nausea /= modifier.nausea;
    }
}

static void RideRatingsApplyRequirementNegativeGs(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.maxNegativeVerticalG >= modifier.threshold)
    {
        ratings.excitement /= modifier.excitement;
        ratings.intensity /= modifier.intensity;
        ratings.nausea /= modifier.nausea;
    }
}

static void RideRatingsApplyRequirementLateralGs(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.maxLateralG < modifier.threshold)
    {
        ratings.excitement /= modifier.excitement;
        ratings.intensity /= modifier.intensity;
        ratings.nausea /= modifier.nausea;
    }
}

static void RideRatingsApplyRequirementInversions(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.numInversions < modifier.threshold)
    {
        ratings.excitement /= modifier.excitement;
        ratings.intensity /= modifier.intensity;
        ratings.nausea /= modifier.nausea;
    }
}

static void RideRatingsApplyRequirementUnsheltered(
    RideRating::Tuple& ratings, const Ride& ride, uint8_t shelteredEighths, RatingsModifier modifier)
{
    if (shelteredEighths >= modifier.threshold)
    {
        ratings.excitement /= modifier.excitement;
        ratings.intensity /= modifier.intensity;
        ratings.nausea /= modifier.nausea;
    }
}

static void RideRatingsApplyRequirementReversals(
    RideRating::Tuple& ratings, const Ride& ride, RideRating::UpdateState& state, RatingsModifier modifier)
{
    if (state.AmountOfReversers < modifier.threshold)
    {
        ratings.excitement /= modifier.excitement;
        ratings.intensity /= modifier.intensity;
        ratings.nausea /= modifier.nausea;
    }
}

static void RideRatingsApplyRequirementHoles(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.numHoles < modifier.threshold)
    {
        ratings.excitement /= modifier.excitement;
        ratings.intensity /= modifier.intensity;
        ratings.nausea /= modifier.nausea;
    }
}

static void RideRatingsApplyRequirementStations(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.numStations <= modifier.threshold)
    {
        // Excitement is set to 0 in original code - this could be changed for consistency
        ratings.excitement = 0;
        ratings.intensity /= modifier.intensity;
        ratings.nausea /= modifier.nausea;
    }
}

static void RideRatingsApplyRequirementSplashdown(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
    if (!ride.specialTrackElements.has(SpecialElement::splash))
    {
        ratings.excitement /= modifier.excitement;
        ratings.intensity /= modifier.intensity;
        ratings.nausea /= modifier.nausea;
    }
}

#ifndef ORIGINAL_RATINGS
static RideRating::Tuple ride_ratings_get_excessive_lateral_g_penalty(const Ride& ride)
{
    RideRating::Tuple result{};
    if (ride.maxLateralG > MakeFixed16_2dp(2, 80))
    {
        result.intensity = RideRating::make(3, 75);
        result.nausea = RideRating::make(2, 00);
    }

    if (ride.maxLateralG > MakeFixed16_2dp(3, 10))
    {
        // Remove half of the ride_ratings_get_gforce_ratings
        result.excitement = (ride.maxPositiveVerticalG * 5242) >> 16;

        // Apply maximum negative G force factor
        fixed16_2dp gforce = ride.maxNegativeVerticalG;
        result.excitement += (std::clamp<fixed16_2dp>(gforce, -RideRating::make(2, 50), RideRating::make(0, 00)) * -15728)
            >> 16;

        // Apply lateral G force factor
        result.excitement += (std::min<fixed16_2dp>(RideRating::make(1, 50), ride.maxLateralG) * 26214) >> 16;

        // Remove half of the ride_ratings_get_gforce_ratings
        result.excitement /= 2;
        result.excitement *= -1;
        result.intensity = RideRating::make(12, 25);
        result.nausea = RideRating::make(6, 00);
    }
    return result;
}
#endif

static void RideRatingsApplyPenaltyLateralGs(RideRating::Tuple& ratings, const Ride& ride, RatingsModifier modifier)
{
#ifndef ORIGINAL_RATINGS
    RideRating::Tuple subRating = ride_ratings_get_excessive_lateral_g_penalty(ride);
    RideRatingsAdd(
        ratings, (subRating.excitement * modifier.excitement) >> 16, (subRating.intensity * modifier.intensity) >> 16,
        (subRating.nausea * modifier.nausea) >> 16);
#endif
}

#pragma endregion

bool RideRating::Tuple::isNull() const
{
    return excitement == kUndefined;
}

void RideRating::Tuple::setNull()
{
    excitement = kUndefined;
}
