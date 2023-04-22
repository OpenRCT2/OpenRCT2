/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideRatings.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../OpenRCT2.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../profiling/Profiling.h"
#include "../scripting/ScriptEngine.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/Surface.h"
#include "Ride.h"
#include "RideData.h"
#include "Station.h"
#include "Track.h"

#include <algorithm>
#include <iterator>

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

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

static RideRatingUpdateStates gRideRatingUpdateStates;

// Amount of updates allowed per updating state on the current tick.
// The total amount would be MaxRideRatingSubSteps * RideRatingMaxUpdateStates which
// would be currently 80, this is the worst case of sub-steps and may break out earlier.
static constexpr size_t MaxRideRatingUpdateSubSteps = 20;

static void ride_ratings_update_state(RideRatingUpdateState& state);
static void ride_ratings_update_state_0(RideRatingUpdateState& state);
static void ride_ratings_update_state_1(RideRatingUpdateState& state);
static void ride_ratings_update_state_2(RideRatingUpdateState& state);
static void ride_ratings_update_state_3(RideRatingUpdateState& state);
static void ride_ratings_update_state_4(RideRatingUpdateState& state);
static void ride_ratings_update_state_5(RideRatingUpdateState& state);
static void ride_ratings_begin_proximity_loop(RideRatingUpdateState& state);
static void RideRatingsCalculate(RideRatingUpdateState& state, Ride& ride);
static void RideRatingsCalculateValue(Ride& ride);
static void ride_ratings_score_close_proximity(RideRatingUpdateState& state, TileElement* inputTileElement);
static void RideRatingsAdd(RatingTuple* rating, int32_t excitement, int32_t intensity, int32_t nausea);

RideRatingUpdateStates& RideRatingGetUpdateStates()
{
    return gRideRatingUpdateStates;
}

void RideRatingResetUpdateStates()
{
    RideRatingUpdateState nullState{};
    nullState.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;

    std::fill(gRideRatingUpdateStates.begin(), gRideRatingUpdateStates.end(), nullState);
}

/**
 * This is a small hack function to keep calling the ride rating processor until
 * the given ride's ratings have been calculated. Whatever is currently being
 * processed will be overwritten.
 * Only purpose of this function currently is for testing.
 */
void RideRatingsUpdateRide(const Ride& ride)
{
    RideRatingUpdateState state;
    if (ride.status != RideStatus::Closed)
    {
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
void RideRatingsUpdateAll()
{
    PROFILED_FUNCTION();

    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        return;

    for (auto& updateState : gRideRatingUpdateStates)
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

static void ride_ratings_update_state(RideRatingUpdateState& state)
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
    return std::any_of(gRideRatingUpdateStates.begin(), gRideRatingUpdateStates.end(), [id](auto& state) {
        return state.CurrentRide == id && state.State != RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
    });
}

static bool ShouldSkipRatingCalculation(const Ride& ride)
{
    // Skip rides that are closed.
    if (ride.status == RideStatus::Closed)
    {
        return true;
    }

    // Skip anything that is already updating.
    if (RideRatingIsUpdatingRide(ride.id))
    {
        return true;
    }

    // Skip rides that have a fixed rating.
    if (ride.lifecycle_flags & RIDE_LIFECYCLE_FIXED_RATINGS)
    {
        return true;
    }

    return false;
}

static RideId GetNextRideToUpdate(RideId currentRide)
{
    auto rm = GetRideManager();
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
static void ride_ratings_update_state_0(RideRatingUpdateState& state)
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
static void ride_ratings_update_state_1(RideRatingUpdateState& state)
{
    state.ProximityTotal = 0;
    for (int32_t i = 0; i < PROXIMITY_COUNT; i++)
    {
        state.ProximityScores[i] = 0;
    }
    state.AmountOfBrakes = 0;
    state.AmountOfReversers = 0;
    state.State = RIDE_RATINGS_STATE_2;
    state.StationFlags = 0;
    ride_ratings_begin_proximity_loop(state);
}

/**
 *
 *  rct2: 0x006B5C66
 */
static void ride_ratings_update_state_2(RideRatingUpdateState& state)
{
    const RideId rideIndex = state.CurrentRide;
    auto ride = GetRide(rideIndex);
    if (ride == nullptr || ride->status == RideStatus::Closed || ride->type >= RIDE_TYPE_COUNT)
    {
        state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
        return;
    }

    auto loc = state.Proximity;
    track_type_t trackType = state.ProximityTrackType;

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
            if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK))
                continue;
        }

        if (trackType == TrackElemType::None
            || (tileElement->AsTrack()->GetSequenceIndex() == 0 && trackType == tileElement->AsTrack()->GetTrackType()))
        {
            if (trackType == TrackElemType::EndStation)
            {
                auto entranceIndex = tileElement->AsTrack()->GetStationIndex();
                state.StationFlags &= ~RIDE_RATING_STATION_FLAG_NO_ENTRANCE;
                if (ride->GetStation(entranceIndex).Entrance.IsNull())
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
static void ride_ratings_update_state_3(RideRatingUpdateState& state)
{
    auto ride = GetRide(state.CurrentRide);
    if (ride == nullptr || ride->status == RideStatus::Closed)
    {
        state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
        return;
    }

    RideRatingsCalculate(state, *ride);
    RideRatingsCalculateValue(*ride);

    WindowInvalidateByNumber(WindowClass::Ride, state.CurrentRide.ToUnderlying());
    state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

/**
 *
 *  rct2: 0x006B5BAB
 */
static void ride_ratings_update_state_4(RideRatingUpdateState& state)
{
    state.State = RIDE_RATINGS_STATE_5;
    ride_ratings_begin_proximity_loop(state);
}

/**
 *
 *  rct2: 0x006B5D72
 */
static void ride_ratings_update_state_5(RideRatingUpdateState& state)
{
    auto ride = GetRide(state.CurrentRide);
    if (ride == nullptr || ride->status == RideStatus::Closed)
    {
        state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
        return;
    }

    auto loc = state.Proximity;
    track_type_t trackType = state.ProximityTrackType;

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
            if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK))
                continue;
        }

        if (trackType == TrackElemType::None || trackType == tileElement->AsTrack()->GetTrackType())
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
static void ride_ratings_begin_proximity_loop(RideRatingUpdateState& state)
{
    auto ride = GetRide(state.CurrentRide);
    if (ride == nullptr || ride->status == RideStatus::Closed)
    {
        state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
        return;
    }

    const auto& rtd = ride->GetRideTypeDescriptor();
    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
    {
        state.State = RIDE_RATINGS_STATE_CALCULATE;
        return;
    }

    for (auto& station : ride->GetStations())
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
            state.ProximityTrackType = TrackElemType::None;
            state.ProximityStart = location;
            return;
        }
    }

    state.State = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

static void proximity_score_increment(RideRatingUpdateState& state, int32_t type)
{
    state.ProximityScores[type]++;
}

/**
 *
 *  rct2: 0x006B6207
 */
static void ride_ratings_score_close_proximity_in_direction(
    RideRatingUpdateState& state, TileElement* inputTileElement, int32_t direction)
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
                if (abs(inputTileElement->GetBaseZ() - tileElement->GetBaseZ()) <= 2 * COORDS_Z_STEP)
                {
                    proximity_score_increment(state, PROXIMITY_PATH_SIDE_CLOSE);
                }
                break;
            case TileElementType::Track:
                if (inputTileElement->AsTrack()->GetRideIndex() != tileElement->AsTrack()->GetRideIndex())
                {
                    if (abs(inputTileElement->GetBaseZ() - tileElement->GetBaseZ()) <= 2 * COORDS_Z_STEP)
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

static void ride_ratings_score_close_proximity_loops_helper(RideRatingUpdateState& state, const CoordsXYE& coordsElement)
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
                    if (tileElement->AsTrack()->GetTrackType() == TrackElemType::LeftVerticalLoop
                        || tileElement->AsTrack()->GetTrackType() == TrackElemType::RightVerticalLoop)
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
static void ride_ratings_score_close_proximity_loops(RideRatingUpdateState& state, TileElement* inputTileElement)
{
    auto trackType = inputTileElement->AsTrack()->GetTrackType();
    if (trackType == TrackElemType::LeftVerticalLoop || trackType == TrackElemType::RightVerticalLoop)
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
static void ride_ratings_score_close_proximity(RideRatingUpdateState& state, TileElement* inputTileElement)
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
                if (trackType == TrackElemType::LeftVerticalLoop || trackType == TrackElemType::RightVerticalLoop)
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

    switch (state.ProximityTrackType)
    {
        case TrackElemType::Brakes:
            state.AmountOfBrakes++;
            break;
        case TrackElemType::LeftReverser:
        case TrackElemType::RightReverser:
            state.AmountOfReversers++;
            break;
    }
}

static void RideRatingsCalculate(RideRatingUpdateState& state, Ride& ride)
{
    RideRatingsDescriptor rrd = ride.GetRideTypeDescriptor().RatingsData;

    switch (rrd.Type)
    {
        case RatingsCalculationType::Normal:
            if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
                return;
            break;
        case RatingsCalculationType::FlatRide:
            ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
            ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
            break;
        case RatingsCalculationType::Stall:
            ride.upkeep_cost = ride_compute_upkeep(state, ride);
            ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
            // Exit ratings
            return;
    }

    ride.unreliability_factor = rrd.Unreliability;
    set_unreliability_factor(ride);

    auto shelteredEighths = get_num_of_sheltered_eighths(ride);
    if (rrd.RideShelter == -1)
    {
        ride.sheltered_eighths = shelteredEighths.TotalShelteredEighths;
    }
    else
    {
        ride.sheltered_eighths = rrd.RideShelter;
    }

    RatingTuple ratings = rrd.BaseRatings;
    // Apply Modifiers
    for (auto modifier : rrd.Modifiers)
    {
        switch (modifier.Type)
        {
            case RatingsModifierType::BonusLength:
                RideRatingsApplyBonusLength(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusSynchronisation:
                RideRatingsApplyBonusSynchronisation(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusTrainLength:
                RideRatingsApplyBonusLength(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusMaxSpeed:
                RideRatingsApplyBonusMaxSpeed(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusAverageSpeed:
                RideRatingsApplyBonusAverageSpeed(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusDuration:
                RideRatingsApplyBonusDuration(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusGForces:
                RideRatingsApplyBonusGForces(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusTurns:
                RideRatingsApplyBonusTurns(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusDrops:
                RideRatingsApplyBonusDrops(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusSheltered:
                RideRatingsApplyBonusSheltered(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusProximity:
                RideRatingsApplyBonusProximity(&ratings, ride, state, modifier);
                break;
            case RatingsModifierType::BonusScenery:
                RideRatingsApplyBonusScenery(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusRotations:
                RideRatingsApplyBonusRotations(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusOperationOption:
                RideRatingsApplyBonusOperationOption(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusGoKartRace:
                RideRatingsApplyBonusGoKartRace(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusTowerRide:
                RideRatingsApplyBonusTowerRide(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusMazeSize:
                RideRatingsApplyBonusMazeSize(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusBoatHireNoCircuit:
                RideRatingsApplyBonusBoatHireNoCircuit(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusSlideUnlimitedRides:
                RideRatingsApplyBonusSlideUnlimitedRides(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusMotionSimulatorMode:
                RideRatingsApplyBonusMotionSimulatorMode(&ratings, ride, modifier);
                break;
            case RatingsModifierType::Bonus3DCinemaMode:
                RideRatingsApplyBonus3DCinemaMode(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusTopSpinMode:
                RideRatingsApplyBonusTopSpinMode(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusReversals:
                RideRatingsApplyBonusReversals(&ratings, ride, state, modifier);
                break;
            case RatingsModifierType::BonusHoles:
                RideRatingsApplyBonusHoles(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusNumTrains:
                RideRatingsApplyBonusNumTrains(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusDownwardLaunch:
                RideRatingsApplyBonusDownwardLaunch(&ratings, ride, modifier);
                break;
            case RatingsModifierType::BonusLaunchedFreefallSpecial:
                RideRatingsApplyBonusLaunchedFreefallSpecial(&ratings, ride, state, modifier);
                break;
            case RatingsModifierType::RequirementLength:
                RideRatingsApplyRequirementLength(&ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementDropHeight:
                RideRatingsApplyRequirementDropHeight(&ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementNumDrops:
                RideRatingsApplyRequirementNumDrops(&ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementMaxSpeed:
                RideRatingsApplyRequirementMaxSpeed(&ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementNegativeGs:
                RideRatingsApplyRequirementNegativeGs(&ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementLateralGs:
                RideRatingsApplyRequirementLateralGs(&ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementInversions:
                RideRatingsApplyRequirementInversions(&ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementUnsheltered:
                RideRatingsApplyRequirementUnsheltered(&ratings, ride, shelteredEighths.TrackShelteredEighths, modifier);
                break;
            case RatingsModifierType::RequirementReversals:
                RideRatingsApplyRequirementReversals(&ratings, ride, state, modifier);
                break;
            case RatingsModifierType::RequirementHoles:
                RideRatingsApplyRequirementHoles(&ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementStations:
                RideRatingsApplyRequirementStations(&ratings, ride, modifier);
                break;
            case RatingsModifierType::RequirementSplashdown:
                RideRatingsApplyRequirementSplashdown(&ratings, ride, modifier);
                break;
            case RatingsModifierType::PenaltyLateralGs:
                RideRatingsApplyPenaltyLateralGs(&ratings, ride, modifier);
                break;
        }
    }
    // Universl ratings adjustments
    RideRatingsApplyIntensityPenalty(&ratings);
    RideRatingsApplyAdjustments(ride, &ratings);
    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

#ifdef ORIGINAL_RATINGS
    if (ride.ratings.excitement != -1)
    {
        // Address underflows allowed by original RCT2 code
        ride.ratings.excitement = max(0, ride.ratings.excitement);
        ride.ratings.intensity = max(0, ride.ratings.intensity);
        ride.ratings.nausea = max(0, ride.ratings.nausea);
    }
#endif

#ifdef ENABLE_SCRIPTING
    // Only call the 'ride.ratings.calculate' API hook if testing of the ride is complete
    if (ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED)
    {
        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        if (hookEngine.HasSubscriptions(HOOK_TYPE::RIDE_RATINGS_CALCULATE))
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto originalExcitement = ride.excitement;
            auto originalIntensity = ride.intensity;
            auto originalNausea = ride.nausea;

            // Create event args object
            auto obj = DukObject(ctx);
            obj.Set("rideId", ride.id.ToUnderlying());
            obj.Set("excitement", originalExcitement);
            obj.Set("intensity", originalIntensity);
            obj.Set("nausea", originalNausea);

            // Call the subscriptions
            auto e = obj.Take();
            hookEngine.Call(HOOK_TYPE::RIDE_RATINGS_CALCULATE, e, true);

            auto scriptExcitement = AsOrDefault(e["excitement"], static_cast<int32_t>(originalExcitement));
            auto scriptIntensity = AsOrDefault(e["intensity"], static_cast<int32_t>(originalIntensity));
            auto scriptNausea = AsOrDefault(e["nausea"], static_cast<int32_t>(originalNausea));

            ride.excitement = std::clamp<int32_t>(scriptExcitement, 0, INT16_MAX);
            ride.intensity = std::clamp<int32_t>(scriptIntensity, 0, INT16_MAX);
            ride.nausea = std::clamp<int32_t>(scriptNausea, 0, INT16_MAX);
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
    static const Row ageTableNew[] = {
        { 5, 3, 2, 0 },       // 1.5x
        { 13, 6, 5, 0 },      // 1.2x
        { 40, 1, 1, 0 },      // 1x
        { 64, 3, 4, 0 },      // 0.75x
        { 88, 9, 16, 0 },     // 0.56x
        { 104, 27, 64, 0 },   // 0.42x
        { 120, 81, 256, 0 },  // 0.32x
        { 128, 81, 512, 0 },  // 0.16x
        { 200, 81, 1024, 0 }, // 0.08x
        { 200, 9, 16, 0 },    // 0.56x "easter egg"
    };

#ifdef ORIGINAL_RATINGS
    static const Row ageTableOld[] = {
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
    };
#endif

    if (!RideHasRatings(ride))
    {
        return;
    }

    // Start with the base ratings, multiplied by the ride type specific weights for excitement, intensity and nausea.
    const auto& ratingsMultipliers = ride.GetRideTypeDescriptor().RatingsMultipliers;
    money64 value = (((ride.excitement * ratingsMultipliers.Excitement) * 32) >> 15)
        + (((ride.intensity * ratingsMultipliers.Intensity) * 32) >> 15)
        + (((ride.nausea * ratingsMultipliers.Nausea) * 32) >> 15);

    int32_t monthsOld = 0;
    if (!gCheatsDisableRideValueAging)
    {
        monthsOld = ride.GetAge();
    }

    const Row* ageTable = ageTableNew;
    size_t tableSize = std::size(ageTableNew);

#ifdef ORIGINAL_RATINGS
    ageTable = ageTableOld;
    tableSize = std::size(ageTableOld);
#endif

    Row lastRow = ageTable[tableSize - 1];

    // Ride is older than oldest age in the table?
    if (monthsOld >= lastRow.months)
    {
        value = (value * lastRow.multiplier) / lastRow.divisor + lastRow.summand;
    }
    else
    {
        // Find the first hit in the table that matches this ride's age
        for (size_t it = 0; it < tableSize; it++)
        {
            Row curr = ageTable[it];

            if (monthsOld < curr.months)
            {
                value = (value * curr.multiplier) / curr.divisor + curr.summand;
                break;
            }
        }
    }

    // Other ride of same type penalty
    const auto& rideManager = GetRideManager();
    auto rideType = ride.type;
    auto otherRidesOfSameType = std::count_if(rideManager.begin(), rideManager.end(), [rideType](const Ride& r) {
        return r.status == RideStatus::Open && r.type == rideType;
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
static money64 ride_compute_upkeep(RideRatingUpdateState& state, const Ride& ride)
{
    // data stored at 0x0057E3A8, incrementing 18 bytes at a time
    auto upkeep = ride.GetRideTypeDescriptor().UpkeepCosts.BaseCost;

    auto trackCost = ride.GetRideTypeDescriptor().UpkeepCosts.CostPerTrackPiece;
    uint8_t dropFactor = ride.drops;

    dropFactor >>= 6;
    dropFactor &= 3;
    upkeep += trackCost * dropFactor;

    uint32_t totalLength = ride.GetTotalLength() >> 16;

    // The data originally here was 20's and 0's. The 20's all represented
    // rides that had tracks. The 0's were fixed rides like crooked house or
    // dodgems.
    // Data source is 0x0097E3AC
    totalLength *= ride.GetRideTypeDescriptor().UpkeepCosts.TrackLengthMultiplier;
    upkeep += static_cast<uint16_t>(totalLength >> 10);

    if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
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

    // these seem to be adhoc adjustments to a ride's upkeep/cost, times
    // various variables set on the ride itself.

    // https://gist.github.com/kevinburke/e19b803cd2769d96c540
    upkeep += ride.GetRideTypeDescriptor().UpkeepCosts.CostPerTrain * ride.NumTrains;
    upkeep += ride.GetRideTypeDescriptor().UpkeepCosts.CostPerCar * ride.num_cars_per_train;

    // slight upkeep boosts for some rides - 5 for mini railway, 10 for log
    // flume/rapids, 10 for roller coaster, 28 for giga coaster
    upkeep += ride.GetRideTypeDescriptor().UpkeepCosts.CostPerStation * ride.num_stations;

    if (ride.mode == RideMode::ReverseInclineLaunchedShuttle)
    {
        upkeep += 30;
    }
    else if (ride.mode == RideMode::PoweredLaunchPasstrough)
    {
        upkeep += 160;
    }
    else if (ride.mode == RideMode::LimPoweredLaunch)
    {
        upkeep += 320;
    }
    else if (ride.mode == RideMode::PoweredLaunch || ride.mode == RideMode::PoweredLaunchBlockSectioned)
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
static void RideRatingsApplyAdjustments(const Ride& ride, RatingTuple* ratings)
{
    const auto* rideEntry = GetRideEntryByIndex(ride.subtype);

    if (rideEntry == nullptr)
    {
        return;
    }

    // Apply ride entry multipliers
    RideRatingsAdd(
        ratings, ((static_cast<int32_t>(ratings->Excitement) * rideEntry->excitement_multiplier) >> 7),
        ((static_cast<int32_t>(ratings->Intensity) * rideEntry->intensity_multiplier) >> 7),
        ((static_cast<int32_t>(ratings->Nausea) * rideEntry->nausea_multiplier) >> 7));

    // Apply total air time
#ifdef ORIGINAL_RATINGS
    if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_AIR_TIME))
    {
        uint16_t totalAirTime = ride.total_air_time;
        if (rideEntry->flags & RIDE_ENTRY_FLAG_LIMIT_AIRTIME_BONUS)
        {
            if (totalAirTime >= 96)
            {
                totalAirTime -= 96;
                ratings->excitement -= totalAirTime / 8;
                ratings->nausea += totalAirTime / 16;
            }
        }
        else
        {
            ratings->excitement += totalAirTime / 8;
            ratings->nausea += totalAirTime / 16;
        }
    }
#else
    if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_AIR_TIME))
    {
        int32_t excitementModifier;
        if (rideEntry->flags & RIDE_ENTRY_FLAG_LIMIT_AIRTIME_BONUS)
        {
            // Limit airtime bonus for heartline twister coaster (see issues #2031 and #2064)
            excitementModifier = std::min<uint16_t>(ride.total_air_time, 96) / 8;
        }
        else
        {
            excitementModifier = ride.total_air_time / 8;
        }
        int32_t nauseaModifier = ride.total_air_time / 16;

        RideRatingsAdd(ratings, excitementModifier, 0, nauseaModifier);
    }
#endif
}

/**
 * Lowers excitement, the higher the intensity.
 *  rct2: 0x0065E7A3
 */
static void RideRatingsApplyIntensityPenalty(RatingTuple* ratings)
{
    static constexpr ride_rating intensityBounds[] = { 1000, 1100, 1200, 1320, 1450 };
    ride_rating excitement = ratings->Excitement;
    for (auto intensityBound : intensityBounds)
    {
        if (ratings->Intensity >= intensityBound)
        {
            excitement -= excitement / 4;
        }
    }
    ratings->Excitement = excitement;
}

/**
 *
 *  rct2: 0x00655FD6
 */
static void set_unreliability_factor(Ride& ride)
{
    // The bigger the difference in lift speed and minimum the higher the unreliability
    uint8_t minLiftSpeed = ride.GetRideTypeDescriptor().LiftData.minimum_speed;
    ride.unreliability_factor += (ride.lift_hill_speed - minLiftSpeed) * 2;
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
static uint32_t ride_ratings_get_proximity_score(RideRatingUpdateState& state)
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
static ShelteredEights get_num_of_sheltered_eighths(const Ride& ride)
{
    int32_t totalLength = ride.GetTotalLength();
    int32_t shelteredLength = ride.sheltered_length;
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

static RatingTuple get_flat_turns_rating(const Ride& ride)
{
    int32_t num3PlusTurns = GetTurnCount3Elements(ride, 0);
    int32_t num2Turns = GetTurnCount2Elements(ride, 0);
    int32_t num1Turns = GetTurnCount1Element(ride, 0);

    RatingTuple rating;
    rating.Excitement = (num3PlusTurns * 0x28000) >> 16;
    rating.Excitement += (num2Turns * 0x30000) >> 16;
    rating.Excitement += (num1Turns * 63421) >> 16;

    rating.Intensity = (num3PlusTurns * 81920) >> 16;
    rating.Intensity += (num2Turns * 49152) >> 16;
    rating.Intensity += (num1Turns * 21140) >> 16;

    rating.Nausea = (num3PlusTurns * 0x50000) >> 16;
    rating.Nausea += (num2Turns * 0x32000) >> 16;
    rating.Nausea += (num1Turns * 42281) >> 16;

    return rating;
}

/**
 *
 *  rct2: 0x0065DF72
 */
static RatingTuple get_banked_turns_rating(const Ride& ride)
{
    int32_t num3PlusTurns = GetTurnCount3Elements(ride, 1);
    int32_t num2Turns = GetTurnCount2Elements(ride, 1);
    int32_t num1Turns = GetTurnCount1Element(ride, 1);

    RatingTuple rating;
    rating.Excitement = (num3PlusTurns * 0x3C000) >> 16;
    rating.Excitement += (num2Turns * 0x3C000) >> 16;
    rating.Excitement += (num1Turns * 73992) >> 16;

    rating.Intensity = (num3PlusTurns * 0x14000) >> 16;
    rating.Intensity += (num2Turns * 49152) >> 16;
    rating.Intensity += (num1Turns * 21140) >> 16;

    rating.Nausea = (num3PlusTurns * 0x50000) >> 16;
    rating.Nausea += (num2Turns * 0x32000) >> 16;
    rating.Nausea += (num1Turns * 48623) >> 16;

    return rating;
}

/**
 *
 *  rct2: 0x0065E047
 */
static RatingTuple get_sloped_turns_rating(const Ride& ride)
{
    RatingTuple rating;

    int32_t num4PlusTurns = GetTurnCount4PlusElements(ride, 2);
    int32_t num3Turns = GetTurnCount3Elements(ride, 2);
    int32_t num2Turns = GetTurnCount2Elements(ride, 2);
    int32_t num1Turns = GetTurnCount1Element(ride, 2);

    rating.Excitement = (std::min(num4PlusTurns, 4) * 0x78000) >> 16;
    rating.Excitement += (std::min(num3Turns, 6) * 273066) >> 16;
    rating.Excitement += (std::min(num2Turns, 6) * 0x3AAAA) >> 16;
    rating.Excitement += (std::min(num1Turns, 7) * 187245) >> 16;
    rating.Intensity = 0;
    rating.Nausea = (std::min(num4PlusTurns, 8) * 0x78000) >> 16;

    return rating;
}

/**
 *
 *  rct2: 0x0065E0F2
 */
static RatingTuple get_inversions_ratings(uint16_t inversions)
{
    RatingTuple rating;

    rating.Excitement = (std::min<int32_t>(inversions, 6) * 0x1AAAAA) >> 16;
    rating.Intensity = (inversions * 0x320000) >> 16;
    rating.Nausea = (inversions * 0x15AAAA) >> 16;

    return rating;
}

void SpecialTrackElementRatingsAjustment_Default(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea)
{
    if (ride.HasWaterSplash())
    {
        excitement += 50;
        intensity += 30;
        nausea += 20;
    }
    if (ride.HasWaterfall())
    {
        excitement += 55;
        intensity += 30;
    }
    if (ride.HasWhirlpool())
    {
        excitement += 35;
        intensity += 20;
        nausea += 23;
    }
}

void SpecialTrackElementRatingsAjustment_GhostTrain(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea)
{
    if (ride.HasSpinningTunnel())
    {
        excitement += 40;
        intensity += 25;
        nausea += 55;
    }
}

void SpecialTrackElementRatingsAjustment_LogFlume(const Ride& ride, int32_t& excitement, int32_t& intensity, int32_t& nausea)
{
    if (ride.HasLogReverser())
    {
        excitement += 48;
        intensity += 55;
        nausea += 65;
    }
}

static RatingTuple GetSpecialTrackElementsRating(uint8_t type, const Ride& ride)
{
    int32_t excitement = 0, intensity = 0, nausea = 0;
    const auto& rtd = ride.GetRideTypeDescriptor();
    rtd.SpecialElementRatingAdjustment(ride, excitement, intensity, nausea);

    uint8_t helixSections = RideGetHelixSections(ride);

    int32_t helixesUpTo9 = std::min<int32_t>(helixSections, 9);
    excitement += (helixesUpTo9 * 254862) >> 16;

    int32_t helixesUpTo11 = std::min<int32_t>(helixSections, 11);
    intensity += (helixesUpTo11 * 148945) >> 16;

    int32_t helixesOver5UpTo10 = std::clamp<int32_t>(helixSections - 5, 0, 10);
    nausea += (helixesOver5UpTo10 * 0x140000) >> 16;

    RatingTuple rating = { static_cast<ride_rating>(excitement), static_cast<ride_rating>(intensity),
                           static_cast<ride_rating>(nausea) };
    return rating;
}

/**
 *
 *  rct2: 0x0065DDD1
 */
static RatingTuple ride_ratings_get_turns_ratings(const Ride& ride)
{
    int32_t excitement = 0, intensity = 0, nausea = 0;

    RatingTuple specialTrackElementsRating = GetSpecialTrackElementsRating(ride.type, ride);
    excitement += specialTrackElementsRating.Excitement;
    intensity += specialTrackElementsRating.Intensity;
    nausea += specialTrackElementsRating.Nausea;

    RatingTuple flatTurnsRating = get_flat_turns_rating(ride);
    excitement += flatTurnsRating.Excitement;
    intensity += flatTurnsRating.Intensity;
    nausea += flatTurnsRating.Nausea;

    RatingTuple bankedTurnsRating = get_banked_turns_rating(ride);
    excitement += bankedTurnsRating.Excitement;
    intensity += bankedTurnsRating.Intensity;
    nausea += bankedTurnsRating.Nausea;

    RatingTuple slopedTurnsRating = get_sloped_turns_rating(ride);
    excitement += slopedTurnsRating.Excitement;
    intensity += slopedTurnsRating.Intensity;
    nausea += slopedTurnsRating.Nausea;

    auto inversions = (ride.type == RIDE_TYPE_MINI_GOLF) ? ride.holes : ride.inversions;
    RatingTuple inversionsRating = get_inversions_ratings(inversions);
    excitement += inversionsRating.Excitement;
    intensity += inversionsRating.Intensity;
    nausea += inversionsRating.Nausea;

    RatingTuple rating = { static_cast<ride_rating>(excitement), static_cast<ride_rating>(intensity),
                           static_cast<ride_rating>(nausea) };
    return rating;
}

/**
 *
 *  rct2: 0x0065E1C2
 */
static RatingTuple ride_ratings_get_sheltered_ratings(const Ride& ride)
{
    int32_t shelteredLengthShifted = (ride.sheltered_length) >> 16;

    uint32_t shelteredLengthUpTo1000 = std::min(shelteredLengthShifted, 1000);
    uint32_t shelteredLengthUpTo2000 = std::min(shelteredLengthShifted, 2000);

    int32_t excitement = (shelteredLengthUpTo1000 * 9175) >> 16;
    int32_t intensity = (shelteredLengthUpTo2000 * 0x2666) >> 16;
    int32_t nausea = (shelteredLengthUpTo1000 * 0x4000) >> 16;

    /*eax = (ride.var_11C * 30340) >> 16;*/
    /*nausea += eax;*/

    if (ride.num_sheltered_sections & ShelteredSectionsBits::BankingWhileSheltered)
    {
        excitement += 20;
        nausea += 15;
    }

    if (ride.num_sheltered_sections & ShelteredSectionsBits::RotatingWhileSheltered)
    {
        excitement += 20;
        nausea += 15;
    }

    uint8_t lowerVal = ride.GetNumShelteredSections();
    lowerVal = std::min<uint8_t>(lowerVal, 11);
    excitement += (lowerVal * 774516) >> 16;

    RatingTuple rating = { static_cast<ride_rating>(excitement), static_cast<ride_rating>(intensity),
                           static_cast<ride_rating>(nausea) };
    return rating;
}

/**
 *
 *  rct2: 0x0065DCDC
 */
static RatingTuple ride_ratings_get_gforce_ratings(const Ride& ride)
{
    RatingTuple result = {
        /* .excitement = */ 0,
        /* .intensity = */ 0,
        /* .nausea = */ 0,
    };

    // Apply maximum positive G force factor
    result.Excitement += (ride.max_positive_vertical_g * 5242) >> 16;
    result.Intensity += (ride.max_positive_vertical_g * 52428) >> 16;
    result.Nausea += (ride.max_positive_vertical_g * 17039) >> 16;

    // Apply maximum negative G force factor
    fixed16_2dp gforce = ride.max_negative_vertical_g;
    result.Excitement += (std::clamp<fixed16_2dp>(gforce, -FIXED_2DP(2, 50), FIXED_2DP(0, 00)) * -15728) >> 16;
    result.Intensity += ((gforce - FIXED_2DP(1, 00)) * -52428) >> 16;
    result.Nausea += ((gforce - FIXED_2DP(1, 00)) * -14563) >> 16;

    // Apply lateral G force factor
    result.Excitement += (std::min<fixed16_2dp>(FIXED_2DP(1, 50), ride.max_lateral_g) * 26214) >> 16;
    result.Intensity += ride.max_lateral_g;
    result.Nausea += (ride.max_lateral_g * 21845) >> 16;

// Very high lateral G force penalty
#ifdef ORIGINAL_RATINGS
    if (ride.max_lateral_g > FIXED_2DP(2, 80))
    {
        result.intensity += FIXED_2DP(3, 75);
        result.nausea += FIXED_2DP(2, 00);
    }
    if (ride.max_lateral_g > FIXED_2DP(3, 10))
    {
        result.excitement /= 2;
        result.intensity += FIXED_2DP(8, 50);
        result.nausea += FIXED_2DP(4, 00);
    }
#endif

    return result;
}

/**
 *
 *  rct2: 0x0065E139
 */
static RatingTuple ride_ratings_get_drop_ratings(const Ride& ride)
{
    RatingTuple result = {
        /* .excitement = */ 0,
        /* .intensity = */ 0,
        /* .nausea = */ 0,
    };

    // Apply number of drops factor
    int32_t drops = ride.drops & 0x3F;
    result.Excitement += (std::min(9, drops) * 728177) >> 16;
    result.Intensity += (drops * 928426) >> 16;
    result.Nausea += (drops * 655360) >> 16;

    // Apply highest drop factor
    RideRatingsAdd(
        &result, ((ride.highest_drop_height * 2) * 16000) >> 16, ((ride.highest_drop_height * 2) * 32000) >> 16,
        ((ride.highest_drop_height * 2) * 10240) >> 16);

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

    const auto& rtd = ride.GetRideTypeDescriptor();
    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
    {
        location = ride.GetStation().Entrance.ToCoordsXY();
    }
    else
    {
        location = ride.GetStation(stationIndex).Start;
    }

    int32_t z = TileElementHeight(location);

    // Check if station is underground, returns a fixed mediocre score since you can't have scenery underground
    if (z > ride.GetStation(stationIndex).GetBaseZ())
    {
        return 40;
    }

    // Count surrounding scenery items
    int32_t numSceneryItems = 0;
    auto tileLocation = TileCoordsXY(location);
    for (int32_t yy = std::max(tileLocation.y - 5, 0); yy <= std::min(tileLocation.y + 5, gMapSize.y - 1); yy++)
    {
        for (int32_t xx = std::max(tileLocation.x - 5, 0); xx <= std::min(tileLocation.x + 5, gMapSize.x - 1); xx++)
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

static void RideRatingsSet(RatingTuple* ratings, int32_t excitement, int32_t intensity, int32_t nausea)
{
    ratings->Excitement = 0;
    ratings->Intensity = 0;
    ratings->Nausea = 0;
    RideRatingsAdd(ratings, excitement, intensity, nausea);
}

/**
 * Add to a ride rating with overflow protection.
 */
static void RideRatingsAdd(RatingTuple* rating, int32_t excitement, int32_t intensity, int32_t nausea)
{
    int32_t newExcitement = rating->Excitement + excitement;
    int32_t newIntensity = rating->Intensity + intensity;
    int32_t newNausea = rating->Nausea + nausea;
    rating->Excitement = std::clamp<int32_t>(newExcitement, 0, INT16_MAX);
    rating->Intensity = std::clamp<int32_t>(newIntensity, 0, INT16_MAX);
    rating->Nausea = std::clamp<int32_t>(newNausea, 0, INT16_MAX);
}

static void RideRatingsApplyBonusLength(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(ratings, (std::min(ride.GetTotalLength() >> 16, modifier.Threshold) * modifier.Excitement) >> 16, 0, 0);
}

static void RideRatingsApplyBonusSynchronisation(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if ((ride.depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS) && RideHasAdjacentStation(ride))
    {
        RideRatingsAdd(ratings, modifier.Excitement, modifier.Intensity, modifier.Nausea);
    }
}

static void RideRatingsApplyBonusTrainLength(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(ratings, ((ride.num_cars_per_train - 1) * modifier.Excitement) >> 16, 0, 0);
}

static void RideRatingsApplyBonusMaxSpeed(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    int32_t maxSpeedMod = ride.max_speed >> 16;
    RideRatingsAdd(
        ratings, (maxSpeedMod * modifier.Excitement) >> 16, (maxSpeedMod * modifier.Intensity) >> 16,
        (maxSpeedMod * modifier.Nausea) >> 16);
}

static void RideRatingsApplyBonusAverageSpeed(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    int32_t avgSpeedMod = ride.average_speed >> 16;
    RideRatingsAdd(ratings, (avgSpeedMod * modifier.Excitement) >> 16, (avgSpeedMod * modifier.Intensity) >> 16, 0);
}

static void RideRatingsApplyBonusDuration(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(ratings, (std::min(ride.GetTotalTime(), modifier.Threshold) * modifier.Excitement) >> 16, 0, 0);
}

static void RideRatingsApplyBonusGForces(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    RatingTuple subRating = ride_ratings_get_gforce_ratings(ride);
    RideRatingsAdd(
        ratings, (subRating.Excitement * modifier.Excitement) >> 16, (subRating.Intensity * modifier.Intensity) >> 16,
        (subRating.Nausea * modifier.Nausea) >> 16);
}

static void RideRatingsApplyBonusTurns(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    RatingTuple subRating = ride_ratings_get_turns_ratings(ride);
    RideRatingsAdd(
        ratings, (subRating.Excitement * modifier.Excitement) >> 16, (subRating.Intensity * modifier.Intensity) >> 16,
        (subRating.Nausea * modifier.Nausea) >> 16);
}

static void RideRatingsApplyBonusDrops(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    RatingTuple subRating = ride_ratings_get_drop_ratings(ride);
    RideRatingsAdd(
        ratings, (subRating.Excitement * modifier.Excitement) >> 16, (subRating.Intensity * modifier.Intensity) >> 16,
        (subRating.Nausea * modifier.Nausea) >> 16);
}

static void RideRatingsApplyBonusSheltered(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    RatingTuple subRating = ride_ratings_get_sheltered_ratings(ride);
    RideRatingsAdd(
        ratings, (subRating.Excitement * modifier.Excitement) >> 16, (subRating.Intensity * modifier.Intensity) >> 16,
        (subRating.Nausea * modifier.Nausea) >> 16);
}

static void RideRatingsApplyBonusRotations(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(
        ratings, ride.rotations * modifier.Excitement, ride.rotations * modifier.Intensity, ride.rotations * modifier.Nausea);
}

static void RideRatingsApplyBonusOperationOption(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    int32_t excitement = (modifier.Excitement >= 0) ? (ride.operation_option * modifier.Excitement)
                                                   : (ride.operation_option / modifier.Excitement);
    int32_t intensity = (modifier.Intensity >= 0) ? (ride.operation_option * modifier.Intensity)
                                                 : (ride.operation_option / modifier.Intensity);
    int32_t nausea = (modifier.Nausea >= 0) ? (ride.operation_option * modifier.Nausea)
                                           : (ride.operation_option / modifier.Nausea);
    RideRatingsAdd(ratings, excitement, intensity, nausea);
}

static void RideRatingsApplyBonusGoKartRace(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.mode == RideMode::Race && ride.NumTrains >= modifier.Threshold)
    {
        RideRatingsAdd(ratings, modifier.Excitement, modifier.Intensity, modifier.Nausea);

        int32_t lapsFactor = (ride.NumLaps - 1) * 30;
        RideRatingsAdd(ratings, lapsFactor, lapsFactor / 2, 0);
    }
}

static void RideRatingsApplyBonusTowerRide(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(
        ratings, ((ride.GetTotalLength() >> 16) * modifier.Excitement) >> 16, modifier.Intensity,
        ((ride.GetTotalLength() >> 16) * modifier.Nausea) >> 16);
}

static void RideRatingsApplyBonusMazeSize(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    int32_t size = std::min<uint16_t>(ride.maze_tiles, modifier.Threshold);
    RideRatingsAdd(ratings, size * modifier.Excitement, size * modifier.Intensity, size * modifier.Nausea);
}

static void RideRatingsApplyBonusBoatHireNoCircuit(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    // Most likely checking if the ride has does not have a circuit
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
    {
        RideRatingsAdd(ratings, modifier.Excitement, modifier.Intensity, modifier.Nausea);
    }
}

static void RideRatingsApplyBonusSlideUnlimitedRides(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.mode == RideMode::UnlimitedRidesPerAdmission)
    {
        RideRatingsAdd(ratings, modifier.Excitement, modifier.Intensity, modifier.Nausea);
    }
}

static void RideRatingsApplyBonusMotionSimulatorMode(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    // Hardcoded until ride mode refactor
    if (ride.mode == RideMode::FilmThrillRiders)
    {
        RideRatingsSet(ratings, RIDE_RATING(3, 25), RIDE_RATING(4, 10), RIDE_RATING(3, 30));
    }
    else
    {
        RideRatingsSet(ratings, RIDE_RATING(2, 90), RIDE_RATING(3, 50), RIDE_RATING(3, 00));
    }
}

static void RideRatingsApplyBonus3DCinemaMode(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    // Hardcoded until ride mode refactor
    switch (ride.mode)
    {
        default:
        case RideMode::MouseTails3DFilm:
            RideRatingsSet(ratings, RIDE_RATING(3, 50), RIDE_RATING(2, 40), RIDE_RATING(1, 40));
            break;
        case RideMode::StormChasers3DFilm:
            RideRatingsSet(ratings, RIDE_RATING(4, 00), RIDE_RATING(2, 65), RIDE_RATING(1, 55));
            break;
        case RideMode::SpaceRaiders3DFilm:
            RideRatingsSet(ratings, RIDE_RATING(4, 20), RIDE_RATING(2, 60), RIDE_RATING(1, 48));
            break;
    }
}

static void RideRatingsApplyBonusTopSpinMode(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    // Hardcoded until ride mode refactor
    switch (ride.mode)
    {
        default:
        case RideMode::Beginners:
            RideRatingsSet(ratings, RIDE_RATING(2, 00), RIDE_RATING(4, 80), RIDE_RATING(5, 74));
            break;
        case RideMode::Intense:
            RideRatingsSet(ratings, RIDE_RATING(3, 00), RIDE_RATING(5, 75), RIDE_RATING(6, 64));
            break;
        case RideMode::Berserk:
            RideRatingsSet(ratings, RIDE_RATING(3, 20), RIDE_RATING(6, 80), RIDE_RATING(7, 94));
            break;
    }
}

static void RideRatingsApplyBonusReversals(
    RatingTuple* ratings, const Ride& ride, RideRatingUpdateState& state, RatingsModifier modifier)
{
    int32_t numReversers = std::min<uint16_t>(state.AmountOfReversers, modifier.Threshold);
    RideRatingsAdd(ratings, numReversers * modifier.Excitement, numReversers * modifier.Intensity, numReversers * modifier.Nausea);
}

static void RideRatingsApplyBonusHoles(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(
        ratings, (ride.holes) * modifier.Excitement, (ride.holes) * modifier.Intensity, (ride.holes) * modifier.Nausea);
}

static void RideRatingsApplyBonusNumTrains(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    // For some reason the original code ran this twice, before and after the operation option bonus
    // Has been changed to call once with double value
    if (ride.NumTrains >= modifier.Threshold)
    {
        RideRatingsAdd(ratings, modifier.Excitement, modifier.Intensity, modifier.Nausea);
    }
}

static void RideRatingsApplyBonusDownwardLaunch(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.mode == RideMode::DownwardLaunch)
    {
        RideRatingsAdd(ratings, modifier.Excitement, modifier.Intensity, modifier.Nausea);
    }
}

static void RideRatingsApplyBonusLaunchedFreefallSpecial(
    RatingTuple* ratings, const Ride& ride, RideRatingUpdateState& state, RatingsModifier modifier)
{
    int32_t excitement = ((ride.GetTotalLength() >> 16) * 32768) >> 16;
    RideRatingsAdd(ratings, excitement, 0, 0);

#ifdef ORIGINAL_RATINGS
    RideRatingsApplyBonusOperationOption(&ratings, ride, modifier);
#else
    // Only apply "launch speed" effects when the setting can be modified
    if (ride.mode == RideMode::UpwardLaunch)
    {
        RideRatingsApplyBonusOperationOption(ratings, ride, modifier);
    }
    else
    {
        // Fix #3282: When the ride mode is in downward launch mode, the intensity and
        //            nausea were fixed regardless of how high the ride is. The following
        //            calculation is based on roto-drop which is a similar mechanic.
        int32_t lengthFactor = ((ride.GetTotalLength() >> 16) * 209715) >> 16;
        RideRatingsAdd(ratings, lengthFactor, lengthFactor * 2, lengthFactor * 2);
    }
#endif
}

static void RideRatingsApplyBonusProximity(
    RatingTuple* ratings, const Ride& ride, RideRatingUpdateState& state, RatingsModifier modifier)
{
    RideRatingsAdd(ratings, (ride_ratings_get_proximity_score(state) * modifier.Excitement) >> 16, 0, 0);
}

static void RideRatingsApplyBonusScenery(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    RideRatingsAdd(ratings, (ride_ratings_get_scenery_score(ride) * modifier.Excitement) >> 16, 0, 0);
}

static void RideRatingsApplyRequirementLength(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.GetStation().SegmentLength < modifier.Threshold)
    {
        ratings->Excitement /= modifier.Excitement;
        ratings->Intensity /= modifier.Intensity;
        ratings->Nausea /= modifier.Nausea;
    }
}

static void RideRatingsApplyRequirementDropHeight(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.highest_drop_height < modifier.Threshold)
    {
        ratings->Excitement /= modifier.Excitement;
        ratings->Intensity /= modifier.Intensity;
        ratings->Nausea /= modifier.Nausea;
    }
}

static void RideRatingsApplyRequirementMaxSpeed(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.max_speed < modifier.Threshold)
    {
        ratings->Excitement /= modifier.Excitement;
        ratings->Intensity /= modifier.Intensity;
        ratings->Nausea /= modifier.Nausea;
    }
}

static void RideRatingsApplyRequirementNumDrops(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if ((ride.drops & 0x3F) < modifier.Threshold)
    {
        ratings->Excitement /= modifier.Excitement;
        ratings->Intensity /= modifier.Intensity;
        ratings->Nausea /= modifier.Nausea;
    }
}

static void RideRatingsApplyRequirementNegativeGs(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.max_negative_vertical_g >= modifier.Threshold)
    {
        ratings->Excitement /= modifier.Excitement;
        ratings->Intensity /= modifier.Intensity;
        ratings->Nausea /= modifier.Nausea;
    }
}

static void RideRatingsApplyRequirementLateralGs(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.max_lateral_g < modifier.Threshold)
    {
        ratings->Excitement /= modifier.Excitement;
        ratings->Intensity /= modifier.Intensity;
        ratings->Nausea /= modifier.Nausea;
    }
}

static void RideRatingsApplyRequirementInversions(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.inversions < modifier.Threshold)
    {
        ratings->Excitement /= modifier.Excitement;
        ratings->Intensity /= modifier.Intensity;
        ratings->Nausea /= modifier.Nausea;
    }
}

static void RideRatingsApplyRequirementUnsheltered(
    RatingTuple* ratings, const Ride& ride, uint8_t shelteredEighths, RatingsModifier modifier)
{
    if (shelteredEighths >= modifier.Threshold)
    {
        ratings->Excitement /= modifier.Excitement;
        ratings->Intensity /= modifier.Intensity;
        ratings->Nausea /= modifier.Nausea;
    }
}

static void RideRatingsApplyRequirementReversals(RatingTuple* ratings, const Ride& ride, RideRatingUpdateState& state, RatingsModifier modifier)
{
    if (state.AmountOfReversers < modifier.Threshold)
    {
        ratings->Excitement /= modifier.Excitement;
        ratings->Intensity /= modifier.Intensity;
        ratings->Nausea /= modifier.Nausea;
    }
}

static void RideRatingsApplyRequirementHoles(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.holes < modifier.Threshold)
    {
        ratings->Excitement /= modifier.Excitement;
        ratings->Intensity /= modifier.Intensity;
        ratings->Nausea /= modifier.Nausea;
    }
}

static void RideRatingsApplyRequirementStations(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if (ride.num_stations <= modifier.Threshold)
    {
        // Excitement is set to 0 in original code - this could be changed for consistency
        ratings->Excitement = 0;
        ratings->Intensity /= modifier.Intensity;
        ratings->Nausea /= modifier.Nausea;
    }
}

static void RideRatingsApplyRequirementSplashdown(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
    if (!(ride.special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS))
    {
        ratings->Excitement /= modifier.Excitement;
        ratings->Intensity /= modifier.Intensity;
        ratings->Nausea /= modifier.Nausea;
    }
}

static RatingTuple ride_ratings_get_excessive_lateral_g_penalty(const Ride& ride)
{
    RatingTuple result{};
    if (ride.max_lateral_g > FIXED_2DP(2, 80))
    {
        result.Intensity = FIXED_2DP(3, 75);
        result.Nausea = FIXED_2DP(2, 00);
    }

    if (ride.max_lateral_g > FIXED_2DP(3, 10))
    {
        // Remove half of the ride_ratings_get_gforce_ratings
        result.Excitement = (ride.max_positive_vertical_g * 5242) >> 16;

        // Apply maximum negative G force factor
        fixed16_2dp gforce = ride.max_negative_vertical_g;
        result.Excitement += (std::clamp<fixed16_2dp>(gforce, -FIXED_2DP(2, 50), FIXED_2DP(0, 00)) * -15728) >> 16;

        // Apply lateral G force factor
        result.Excitement += (std::min<fixed16_2dp>(FIXED_2DP(1, 50), ride.max_lateral_g) * 26214) >> 16;

        // Remove half of the ride_ratings_get_gforce_ratings
        result.Excitement /= 2;
        result.Excitement *= -1;
        result.Intensity = FIXED_2DP(12, 25);
        result.Nausea = FIXED_2DP(6, 00);
    }
    return result;
}

static void RideRatingsApplyPenaltyLateralGs(RatingTuple* ratings, const Ride& ride, RatingsModifier modifier)
{
#ifndef ORIGINAL_RATINGS
    RatingTuple subRating = ride_ratings_get_excessive_lateral_g_penalty(ride);
    RideRatingsAdd(
        ratings, (subRating.Excitement * modifier.Excitement) >> 16, (subRating.Intensity * modifier.Intensity) >> 16,
        (subRating.Nausea * modifier.Nausea) >> 16);
#endif
}

#pragma endregion
