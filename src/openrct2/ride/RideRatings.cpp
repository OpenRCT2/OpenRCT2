/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma optimize("", off)

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

static void ride_ratings_add(RatingTuple* rating, int32_t excitement, int32_t intensity, int32_t nausea);

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

RideRatingUpdateState& RideRatingGetUpdateState(size_t index)
{
    Guard::IndexInRange(index, gRideRatingUpdateStates);
    return gRideRatingUpdateStates[index];
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
        ride_ratings_update_state(updateState);
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

static RideId GetNextRideToUpdate(RideId currentRide)
{
    auto rm = GetRideManager();
    if (rm.size() == 0)
    {
        return RideId::GetNull();
    }
    // Skip all empty ride ids
    auto nextRide = std::next(rm.get(currentRide));
    // If at end, loop around
    if (nextRide == rm.end())
    {
        nextRide = rm.begin();
    }
    return (*nextRide).id;
}

static bool RideRatingIsUpdatingRide(RideId id)
{
    return std::any_of(gRideRatingUpdateStates.begin(), gRideRatingUpdateStates.end(), [id](auto& state) {
        return state.CurrentRide == id && state.State != RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
    });
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

    auto nextRideId = GetNextRideToUpdate(state.CurrentRide);
    auto nextRide = GetRide(nextRideId);
    if (nextRide != nullptr && nextRide->status != RideStatus::Closed
        && !(nextRide->lifecycle_flags & RIDE_LIFECYCLE_FIXED_RATINGS))
    {
        if (!RideRatingIsUpdatingRide(nextRideId))
        {
            state.State = RIDE_RATINGS_STATE_INITIALISE;
        }
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
    auto calcFunc = RideRatingsGetCalculateFunc(ride.type);
    if (calcFunc != nullptr)
    {
        calcFunc(ride, state);
    }

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
static void ride_ratings_apply_adjustments(const Ride& ride, RatingTuple* ratings)
{
    const auto* rideEntry = GetRideEntryByIndex(ride.subtype);

    if (rideEntry == nullptr)
    {
        return;
    }

    // Apply ride entry multipliers
    ride_ratings_add(
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

        ride_ratings_add(ratings, excitementModifier, 0, nauseaModifier);
    }
#endif
}

/**
 * Lowers excitement, the higher the intensity.
 *  rct2: 0x0065E7A3
 */
static void ride_ratings_apply_intensity_penalty(RatingTuple* ratings)
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
    ride_ratings_add(
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

static void ride_ratings_set(RatingTuple* ratings, int32_t excitement, int32_t intensity, int32_t nausea)
{
    ratings->Excitement = 0;
    ratings->Intensity = 0;
    ratings->Nausea = 0;
    ride_ratings_add(ratings, excitement, intensity, nausea);
}

/**
 * Add to a ride rating with overflow protection.
 */
static void ride_ratings_add(RatingTuple* rating, int32_t excitement, int32_t intensity, int32_t nausea)
{
    int32_t newExcitement = rating->Excitement + excitement;
    int32_t newIntensity = rating->Intensity + intensity;
    int32_t newNausea = rating->Nausea + nausea;
    rating->Excitement = std::clamp<int32_t>(newExcitement, 0, INT16_MAX);
    rating->Intensity = std::clamp<int32_t>(newIntensity, 0, INT16_MAX);
    rating->Nausea = std::clamp<int32_t>(newNausea, 0, INT16_MAX);
}

static void ride_ratings_apply_length(RatingTuple* ratings, const Ride& ride, int32_t maxLength, int32_t excitementMultiplier)
{
    ride_ratings_add(ratings, (std::min(ride.GetTotalLength() >> 16, maxLength) * excitementMultiplier) >> 16, 0, 0);
}

static void ride_ratings_apply_synchronisation(RatingTuple* ratings, const Ride& ride, int32_t excitement, int32_t intensity)
{
    if ((ride.depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS) && RideHasAdjacentStation(ride))
    {
        ride_ratings_add(ratings, excitement, intensity, 0);
    }
}

static void ride_ratings_apply_train_length(RatingTuple* ratings, const Ride& ride, int32_t excitementMultiplier)
{
    ride_ratings_add(ratings, ((ride.num_cars_per_train - 1) * excitementMultiplier) >> 16, 0, 0);
}

static void ride_ratings_apply_max_speed(
    RatingTuple* ratings, const Ride& ride, int32_t excitementMultiplier, int32_t intensityMultiplier, int32_t nauseaMultiplier)
{
    int32_t modifier = ride.max_speed >> 16;
    ride_ratings_add(
        ratings, (modifier * excitementMultiplier) >> 16, (modifier * intensityMultiplier) >> 16,
        (modifier * nauseaMultiplier) >> 16);
}

static void ride_ratings_apply_average_speed(
    RatingTuple* ratings, const Ride& ride, int32_t excitementMultiplier, int32_t intensityMultiplier)
{
    int32_t modifier = ride.average_speed >> 16;
    ride_ratings_add(ratings, (modifier * excitementMultiplier) >> 16, (modifier * intensityMultiplier) >> 16, 0);
}

static void ride_ratings_apply_duration(
    RatingTuple* ratings, const Ride& ride, int32_t maxDuration, int32_t excitementMultiplier)
{
    ride_ratings_add(ratings, (std::min(ride.GetTotalTime(), maxDuration) * excitementMultiplier) >> 16, 0, 0);
}

static void ride_ratings_apply_gforces(
    RatingTuple* ratings, const Ride& ride, int32_t excitementMultiplier, int32_t intensityMultiplier, int32_t nauseaMultiplier)
{
    RatingTuple subRating = ride_ratings_get_gforce_ratings(ride);
    ride_ratings_add(
        ratings, (subRating.Excitement * excitementMultiplier) >> 16, (subRating.Intensity * intensityMultiplier) >> 16,
        (subRating.Nausea * nauseaMultiplier) >> 16);
}

static void ride_ratings_apply_turns(
    RatingTuple* ratings, const Ride& ride, int32_t excitementMultiplier, int32_t intensityMultiplier, int32_t nauseaMultiplier)
{
    RatingTuple subRating = ride_ratings_get_turns_ratings(ride);
    ride_ratings_add(
        ratings, (subRating.Excitement * excitementMultiplier) >> 16, (subRating.Intensity * intensityMultiplier) >> 16,
        (subRating.Nausea * nauseaMultiplier) >> 16);
}

static void ride_ratings_apply_drops(
    RatingTuple* ratings, const Ride& ride, int32_t excitementMultiplier, int32_t intensityMultiplier, int32_t nauseaMultiplier)
{
    RatingTuple subRating = ride_ratings_get_drop_ratings(ride);
    ride_ratings_add(
        ratings, (subRating.Excitement * excitementMultiplier) >> 16, (subRating.Intensity * intensityMultiplier) >> 16,
        (subRating.Nausea * nauseaMultiplier) >> 16);
}

static void ride_ratings_apply_sheltered_ratings(
    RatingTuple* ratings, const Ride& ride, int32_t excitementMultiplier, int32_t intensityMultiplier, int32_t nauseaMultiplier)
{
    RatingTuple subRating = ride_ratings_get_sheltered_ratings(ride);
    ride_ratings_add(
        ratings, (subRating.Excitement * excitementMultiplier) >> 16, (subRating.Intensity * intensityMultiplier) >> 16,
        (subRating.Nausea * nauseaMultiplier) >> 16);
}

static void ride_ratings_apply_operation_option(
    RatingTuple* ratings, const Ride& ride, int32_t excitementMultiplier, int32_t intensityMultiplier, int32_t nauseaMultiplier)
{
    ride_ratings_add(
        ratings, (ride.operation_option * excitementMultiplier) >> 16, (ride.operation_option * intensityMultiplier) >> 16,
        (ride.operation_option * nauseaMultiplier) >> 16);
}

static void ride_ratings_apply_rotations(
    RatingTuple* ratings, const Ride& ride, int32_t excitementMultiplier, int32_t intensityMultiplier, int32_t nauseaMultiplier)
{
    ride_ratings_add(
        ratings, ride.rotations * excitementMultiplier, ride.rotations * intensityMultiplier,
        ride.rotations * nauseaMultiplier);
}

static void ride_ratings_apply_proximity(RideRatingUpdateState& state, RatingTuple* ratings, int32_t excitementMultiplier)
{
    ride_ratings_add(ratings, (ride_ratings_get_proximity_score(state) * excitementMultiplier) >> 16, 0, 0);
}

static void ride_ratings_apply_scenery(RatingTuple* ratings, const Ride& ride, int32_t excitementMultiplier)
{
    ride_ratings_add(ratings, (ride_ratings_get_scenery_score(ride) * excitementMultiplier) >> 16, 0, 0);
}

static void ride_ratings_apply_highest_drop_height_penalty(
    RatingTuple* ratings, const Ride& ride, int32_t minHighestDropHeight, int32_t excitementPenalty, int32_t intensityPenalty,
    int32_t nauseaPenalty)
{
    if (ride.highest_drop_height < minHighestDropHeight)
    {
        ratings->Excitement /= excitementPenalty;
        ratings->Intensity /= intensityPenalty;
        ratings->Nausea /= nauseaPenalty;
    }
}

static void ride_ratings_apply_max_speed_penalty(
    RatingTuple* ratings, const Ride& ride, int32_t minMaxSpeed, int32_t excitementPenalty, int32_t intensityPenalty,
    int32_t nauseaPenalty)
{
    if (ride.max_speed < minMaxSpeed)
    {
        ratings->Excitement /= excitementPenalty;
        ratings->Intensity /= intensityPenalty;
        ratings->Nausea /= nauseaPenalty;
    }
}

static void ride_ratings_apply_num_drops_penalty(
    RatingTuple* ratings, const Ride& ride, int32_t minNumDrops, int32_t excitementPenalty, int32_t intensityPenalty,
    int32_t nauseaPenalty)
{
    if ((ride.drops & 0x3F) < minNumDrops)
    {
        ratings->Excitement /= excitementPenalty;
        ratings->Intensity /= intensityPenalty;
        ratings->Nausea /= nauseaPenalty;
    }
}

static void ride_ratings_apply_max_negative_g_penalty(
    RatingTuple* ratings, const Ride& ride, int32_t maxMaxNegativeVerticalG, int32_t excitementPenalty,
    int32_t intensityPenalty, int32_t nauseaPenalty)
{
    if (ride.max_negative_vertical_g >= maxMaxNegativeVerticalG)
    {
        ratings->Excitement /= excitementPenalty;
        ratings->Intensity /= intensityPenalty;
        ratings->Nausea /= nauseaPenalty;
    }
}

static void ride_ratings_apply_max_lateral_g_penalty(
    RatingTuple* ratings, const Ride& ride, int32_t minMaxLateralG, int32_t excitementPenalty, int32_t intensityPenalty,
    int32_t nauseaPenalty)
{
    if (ride.max_lateral_g < minMaxLateralG)
    {
        ratings->Excitement /= excitementPenalty;
        ratings->Intensity /= intensityPenalty;
        ratings->Nausea /= nauseaPenalty;
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

static void ride_ratings_apply_excessive_lateral_g_penalty(
    RatingTuple* ratings, const Ride& ride, int32_t excitementMultiplier, int32_t intensityMultiplier, int32_t nauseaMultiplier)
{
#ifndef ORIGINAL_RATINGS
    RatingTuple subRating = ride_ratings_get_excessive_lateral_g_penalty(ride);
    ride_ratings_add(
        ratings, (subRating.Excitement * excitementMultiplier) >> 16, (subRating.Intensity * intensityMultiplier) >> 16,
        (subRating.Nausea * nauseaMultiplier) >> 16);
#endif
}

static void ride_ratings_apply_first_length_penalty(
    RatingTuple* ratings, const Ride& ride, int32_t minFirstLength, int32_t excitementPenalty, int32_t intensityPenalty,
    int32_t nauseaPenalty)
{
    if (ride.GetStation().SegmentLength < minFirstLength)
    {
        ratings->Excitement /= excitementPenalty;
        ratings->Intensity /= intensityPenalty;
        ratings->Nausea /= nauseaPenalty;
    }
}

#pragma endregion

#pragma region Ride rating calculation functions

void RideRatingsCalculateSpiralRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 14;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 30), RIDE_RATING(0, 30), RIDE_RATING(0, 30));
    ride_ratings_apply_length(&ratings, ride, 6000, 819);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 140434);
    ride_ratings_apply_max_speed(&ratings, ride, 51366, 85019, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 364088, 400497);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 36864, 30384, 49648);
    ride_ratings_apply_turns(&ratings, ride, 28235, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 43690, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 6693);

    if (ride.inversions == 0)
        ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

    if (ride.inversions == 0)
    {
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
        ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
    }

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 36864, 30384, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateStandUpRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 17;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 50), RIDE_RATING(3, 00), RIDE_RATING(3, 00));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 10));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 123987, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 59578);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 34952, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 12850, 28398, 30427);
    ride_ratings_apply_proximity(state, &ratings, 17893);
    ride_ratings_apply_scenery(&ratings, ride, 5577);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
    ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 50), 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 35746, 59578);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateSuspendedSwingingCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 18;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 30), RIDE_RATING(2, 90), RIDE_RATING(3, 50));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 10));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 32768, 23831, 79437);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 48036);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 6971);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xC0000, 2, 2, 2);
    ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 60), 2, 2, 2);
    ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1, 50), 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 32768, 23831, 79437);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateInvertedRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 17;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 60), RIDE_RATING(2, 80), RIDE_RATING(3, 20));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 42), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 29789, 55606);
    ride_ratings_apply_turns(&ratings, ride, 26749, 29552, 57186);
    ride_ratings_apply_drops(&ratings, ride, 29127, 39009, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 15291, 35108);
    ride_ratings_apply_proximity(state, &ratings, 15657);
    ride_ratings_apply_scenery(&ratings, ride, 8366);

    if (ride.inversions == 0)
        ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

    if (ride.inversions == 0)
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 30), 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 29789, 55606);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateJuniorRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 13;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 40), RIDE_RATING(2, 50), RIDE_RATING(1, 80));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 20480, 23831, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 25700, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 9760);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 1, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 20480, 23831, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateMiniatureRailway(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 11;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 50), RIDE_RATING(0, 00), RIDE_RATING(0, 00));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_train_length(&ratings, ride, 140434);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, -6425, 6553, 23405);
    ride_ratings_apply_proximity(state, &ratings, 8946);
    ride_ratings_apply_scenery(&ratings, ride, 20915);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 2, 2, 2);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    auto shelteredEighths = get_num_of_sheltered_eighths(ride);
    if (shelteredEighths.TrackShelteredEighths >= 4)
        ride.excitement /= 4;

    ride.sheltered_eighths = shelteredEighths.TotalShelteredEighths;
}

void RideRatingsCalculateMonorail(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 14;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 00), RIDE_RATING(0, 00), RIDE_RATING(0, 00));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_train_length(&ratings, ride, 93622);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 70849, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 218453);
    ride_ratings_apply_duration(&ratings, ride, 150, 21845);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 5140, 6553, 18724);
    ride_ratings_apply_proximity(state, &ratings, 8946);
    ride_ratings_apply_scenery(&ratings, ride, 16732);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    auto shelteredEighths = get_num_of_sheltered_eighths(ride);
    if (shelteredEighths.TrackShelteredEighths >= 4)
        ride.excitement /= 4;

    ride.sheltered_eighths = shelteredEighths.TotalShelteredEighths;
}

void RideRatingsCalculateMiniSuspendedCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 15;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 80), RIDE_RATING(2, 50), RIDE_RATING(2, 70));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 45), RIDE_RATING(0, 15));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 49648);
    ride_ratings_apply_turns(&ratings, ride, 34179, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 58254, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 19275, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 13943);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x80000, 2, 2, 2);
    ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1, 30), 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 35746, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateBoatHire(Ride& ride, RideRatingUpdateState& state)
{
    ride.unreliability_factor = 7;
    set_unreliability_factor(ride);

    // NOTE In the original game, the ratings were zeroed before calling set_unreliability_factor which is unusual as rest
    // of the calculation functions do this before hand. This is because set_unreliability_factor alters the value of ebx
    // (excitement). This is assumed to be a bug and therefore fixed.

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 90), RIDE_RATING(0, 80), RIDE_RATING(0, 90));

    // Most likely checking if the ride has does not have a circuit
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
    {
        ride_ratings_add(&ratings, RIDE_RATING(0, 20), 0, 0);
    }

    ride_ratings_apply_proximity(state, &ratings, 11183);
    ride_ratings_apply_scenery(&ratings, ride, 22310);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 0;
}

void RideRatingsCalculateWoodenWildMouse(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 14;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 90), RIDE_RATING(2, 90), RIDE_RATING(2, 10));
    ride_ratings_apply_length(&ratings, ride, 6000, 873);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 8));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 102400, 35746, 49648);
    ride_ratings_apply_turns(&ratings, ride, 29721, 43458, 45749);
    ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 17893);
    ride_ratings_apply_scenery(&ratings, ride, 5577);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
    ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 10), 2, 2, 2);
    ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1, 50), 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 3, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 102400, 35746, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateSteeplechase(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 14;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 70), RIDE_RATING(2, 40), RIDE_RATING(1, 80));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 75), RIDE_RATING(0, 9));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 20480, 20852, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 25700, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 9760);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 4, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x80000, 2, 2, 2);
    ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 50), 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xF00000, 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 20480, 20852, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateCarRide(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 12;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 00), RIDE_RATING(0, 50), RIDE_RATING(0, 00));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 15), RIDE_RATING(0, 00));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_turns(&ratings, ride, 14860, 0, 11437);
    ride_ratings_apply_drops(&ratings, ride, 8738, 0, 0);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 12850, 6553, 4681);
    ride_ratings_apply_proximity(state, &ratings, 11183);
    ride_ratings_apply_scenery(&ratings, ride, 8366);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 8, 2, 2);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateLaunchedFreefall(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 70), RIDE_RATING(3, 00), RIDE_RATING(3, 50));

    if (ride.mode == RideMode::DownwardLaunch)
    {
        ride_ratings_add(&ratings, RIDE_RATING(0, 30), RIDE_RATING(0, 65), RIDE_RATING(0, 45));
    }

    int32_t excitementModifier = ((ride.GetTotalLength() >> 16) * 32768) >> 16;
    ride_ratings_add(&ratings, excitementModifier, 0, 0);

#ifdef ORIGINAL_RATINGS
    ride_ratings_apply_operation_option(&ratings, ride, 0, 1355917, 451972);
#else
    // Only apply "launch speed" effects when the setting can be modified
    if (ride.mode == RideMode::UpwardLaunch)
    {
        ride_ratings_apply_operation_option(&ratings, ride, 0, 1355917, 451972);
    }
    else
    {
        // Fix #3282: When the ride mode is in downward launch mode, the intensity and
        //            nausea were fixed regardless of how high the ride is. The following
        //            calculation is based on roto-drop which is a similar mechanic.
        int32_t lengthFactor = ((ride.GetTotalLength() >> 16) * 209715) >> 16;
        ride_ratings_add(&ratings, lengthFactor, lengthFactor * 2, lengthFactor * 2);
    }
#endif

    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 25098);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateBobsleighCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 80), RIDE_RATING(3, 20), RIDE_RATING(2, 50));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 20), RIDE_RATING(0, 00));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 65536, 23831, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 5577);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xC0000, 2, 2, 2);
    ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1, 20), 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 65536, 23831, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateObservationTower(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 15;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 50), RIDE_RATING(0, 00), RIDE_RATING(0, 10));
    ride_ratings_add(&ratings, ((ride.GetTotalLength() >> 16) * 45875) >> 16, 0, ((ride.GetTotalLength() >> 16) * 26214) >> 16);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 83662);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 7;

    auto shelteredEighths = get_num_of_sheltered_eighths(ride);
    if (shelteredEighths.TrackShelteredEighths >= 5)
        ride.excitement /= 4;
}

void RideRatingsCalculateLoopingRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = ride.IsPoweredLaunched() ? 20 : 15;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 00), RIDE_RATING(0, 50), RIDE_RATING(0, 20));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 6693);

    if (ride.inversions == 0)
        ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 14, 2, 2, 2);

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

    if (ride.inversions == 0)
    {
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 10), 2, 2, 2);
        ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
    }

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 35746, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateDinghySlide(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 13;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 70), RIDE_RATING(2, 00), RIDE_RATING(1, 50));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 50), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 65536, 29789, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 11183);
    ride_ratings_apply_scenery(&ratings, ride, 5577);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0x8C0000, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 65536, 29789, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateMineTrainCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 90), RIDE_RATING(2, 30), RIDE_RATING(2, 10));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 40960, 35746, 49648);
    ride_ratings_apply_turns(&ratings, ride, 29721, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 19275, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 21472);
    ride_ratings_apply_scenery(&ratings, ride, 16732);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
    ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 10), 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 40960, 35746, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateChairlift(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 14 + (ride.speed * 2);
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 60), RIDE_RATING(0, 40), RIDE_RATING(0, 50));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_turns(&ratings, ride, 7430, 3476, 4574);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, -19275, 21845, 23405);
    ride_ratings_apply_proximity(state, &ratings, 11183);
    ride_ratings_apply_scenery(&ratings, ride, 25098);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0x960000, 2, 2, 2);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    if (ride.num_stations <= 1)
    {
        ratings.Excitement = 0;
        ratings.Intensity /= 2;
    }

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    auto shelteredEighths = get_num_of_sheltered_eighths(ride);
    if (shelteredEighths.TrackShelteredEighths >= 4)
        ride.excitement /= 4;

    ride.sheltered_eighths = shelteredEighths.TotalShelteredEighths;
}

void RideRatingsCalculateCorkscrewRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 00), RIDE_RATING(0, 50), RIDE_RATING(0, 20));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 6693);

    if (ride.inversions == 0)
        ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

    if (ride.inversions == 0)
    {
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
        ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
    }

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 35746, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateMaze(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 8;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 30), RIDE_RATING(0, 50), RIDE_RATING(0, 00));

    int32_t size = std::min<uint16_t>(ride.maze_tiles, 100);
    ride_ratings_add(&ratings, size, size * 2, 0);

    ride_ratings_apply_scenery(&ratings, ride, 22310);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 0;
}

void RideRatingsCalculateSpiralSlide(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 8;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 50), RIDE_RATING(1, 40), RIDE_RATING(0, 90));

    // Unlimited slides boost
    if (ride.mode == RideMode::UnlimitedRidesPerAdmission)
    {
        ride_ratings_add(&ratings, RIDE_RATING(0, 40), RIDE_RATING(0, 20), RIDE_RATING(0, 25));
    }

    ride_ratings_apply_scenery(&ratings, ride, 25098);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 2;
}

void RideRatingsCalculateGoKarts(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 42), RIDE_RATING(1, 73), RIDE_RATING(0, 40));
    ride_ratings_apply_length(&ratings, ride, 700, 32768);

    if (ride.mode == RideMode::Race && ride.NumTrains >= 4)
    {
        ride_ratings_add(&ratings, RIDE_RATING(1, 40), RIDE_RATING(0, 50), 0);

        int32_t lapsFactor = (ride.NumLaps - 1) * 30;
        ride_ratings_add(&ratings, lapsFactor, lapsFactor / 2, 0);
    }

    ride_ratings_apply_turns(&ratings, ride, 4458, 3476, 5718);
    ride_ratings_apply_drops(&ratings, ride, 8738, 5461, 6553);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 2570, 8738, 2340);
    ride_ratings_apply_proximity(state, &ratings, 11183);
    ride_ratings_apply_scenery(&ratings, ride, 16732);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    auto shelteredEighths = get_num_of_sheltered_eighths(ride);
    ride.sheltered_eighths = shelteredEighths.TotalShelteredEighths;

    if (shelteredEighths.TrackShelteredEighths >= 6)
        ride.excitement /= 2;
}

void RideRatingsCalculateLogFlume(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 15;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 50), RIDE_RATING(0, 55), RIDE_RATING(0, 30));
    ride_ratings_apply_length(&ratings, ride, 2000, 7208);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_max_speed(&ratings, ride, 531372, 655360, 301111);
    ride_ratings_apply_duration(&ratings, ride, 300, 13107);
    ride_ratings_apply_turns(&ratings, ride, 22291, 20860, 4574);
    ride_ratings_apply_drops(&ratings, ride, 69905, 62415, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 22367);
    ride_ratings_apply_scenery(&ratings, ride, 11155);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateRiverRapids(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 20), RIDE_RATING(0, 70), RIDE_RATING(0, 50));
    ride_ratings_apply_length(&ratings, ride, 2000, 6225);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 30), RIDE_RATING(0, 05));
    ride_ratings_apply_max_speed(&ratings, ride, 115130, 159411, 106274);
    ride_ratings_apply_duration(&ratings, ride, 500, 13107);
    ride_ratings_apply_turns(&ratings, ride, 29721, 22598, 5718);
    ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 31314);
    ride_ratings_apply_scenery(&ratings, ride, 13943);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 2, 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 2, 2, 2);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateDodgems(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 30), RIDE_RATING(0, 50), RIDE_RATING(0, 35));

    if (ride.NumTrains >= 4)
    {
        ride_ratings_add(&ratings, RIDE_RATING(0, 40), 0, 0);
    }

    ride_ratings_add(&ratings, ride.operation_option, ride.operation_option / 2, 0);

    if (ride.NumTrains >= 4)
    {
        ride_ratings_add(&ratings, RIDE_RATING(0, 40), 0, 0);
    }

    ride_ratings_apply_scenery(&ratings, ride, 5577);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 7;
}

void RideRatingsCalculateSwingingShip(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 10;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 50), RIDE_RATING(1, 90), RIDE_RATING(1, 41));

    ride_ratings_add(&ratings, ride.operation_option * 5, ride.operation_option * 5, ride.operation_option * 10);

    ride_ratings_apply_scenery(&ratings, ride, 16732);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 0;
}

void RideRatingsCalculateInverterShip(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 50), RIDE_RATING(2, 70), RIDE_RATING(2, 74));

    ride_ratings_add(&ratings, ride.operation_option * 11, ride.operation_option * 22, ride.operation_option * 22);

    ride_ratings_apply_scenery(&ratings, ride, 11155);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 0;
}

void RideRatingsCalculateFoodStall(Ride& ride, RideRatingUpdateState& state)
{
    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

void RideRatingsCalculateDrinkStall(Ride& ride, RideRatingUpdateState& state)
{
    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

void RideRatingsCalculateShop(Ride& ride, RideRatingUpdateState& state)
{
    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

void RideRatingsCalculateMerryGoRound(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(0, 60), RIDE_RATING(0, 15), RIDE_RATING(0, 30));
    ride_ratings_apply_rotations(&ratings, ride, 5, 5, 5);
    ride_ratings_apply_scenery(&ratings, ride, 19521);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 7;
}

void RideRatingsCalculateInformationKiosk(Ride& ride, RideRatingUpdateState& state)
{
    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

void RideRatingsCalculateToilets(Ride& ride, RideRatingUpdateState& state)
{
    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

void RideRatingsCalculateFerrisWheel(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(0, 60), RIDE_RATING(0, 25), RIDE_RATING(0, 30));
    ride_ratings_apply_rotations(&ratings, ride, 25, 25, 25);
    ride_ratings_apply_scenery(&ratings, ride, 41831);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 0;
}

void RideRatingsCalculateMotionSimulator(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 21;
    set_unreliability_factor(ride);

    // Base ratings
    RatingTuple ratings;
    if (ride.mode == RideMode::FilmThrillRiders)
    {
        ratings.Excitement = RIDE_RATING(3, 25);
        ratings.Intensity = RIDE_RATING(4, 10);
        ratings.Nausea = RIDE_RATING(3, 30);
    }
    else
    {
        ratings.Excitement = RIDE_RATING(2, 90);
        ratings.Intensity = RIDE_RATING(3, 50);
        ratings.Nausea = RIDE_RATING(3, 00);
    }

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 7;
}

void RideRatingsCalculate3dCinema(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 21;
    set_unreliability_factor(ride);

    // Base ratings
    RatingTuple ratings;
    switch (ride.mode)
    {
        default:
        case RideMode::MouseTails3DFilm:
            ratings.Excitement = RIDE_RATING(3, 50);
            ratings.Intensity = RIDE_RATING(2, 40);
            ratings.Nausea = RIDE_RATING(1, 40);
            break;
        case RideMode::StormChasers3DFilm:
            ratings.Excitement = RIDE_RATING(4, 00);
            ratings.Intensity = RIDE_RATING(2, 65);
            ratings.Nausea = RIDE_RATING(1, 55);
            break;
        case RideMode::SpaceRaiders3DFilm:
            ratings.Excitement = RIDE_RATING(4, 20);
            ratings.Intensity = RIDE_RATING(2, 60);
            ratings.Nausea = RIDE_RATING(1, 48);
            break;
    }

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths |= 7;
}

void RideRatingsCalculateTopSpin(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 19;
    set_unreliability_factor(ride);

    // Base ratings
    RatingTuple ratings;
    switch (ride.mode)
    {
        default:
        case RideMode::Beginners:
            ratings.Excitement = RIDE_RATING(2, 00);
            ratings.Intensity = RIDE_RATING(4, 80);
            ratings.Nausea = RIDE_RATING(5, 74);
            break;
        case RideMode::Intense:
            ratings.Excitement = RIDE_RATING(3, 00);
            ratings.Intensity = RIDE_RATING(5, 75);
            ratings.Nausea = RIDE_RATING(6, 64);
            break;
        case RideMode::Berserk:
            ratings.Excitement = RIDE_RATING(3, 20);
            ratings.Intensity = RIDE_RATING(6, 80);
            ratings.Nausea = RIDE_RATING(7, 94);
            break;
    }

    ride_ratings_apply_scenery(&ratings, ride, 11155);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 0;
}

void RideRatingsCalculateSpaceRings(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 7;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 50), RIDE_RATING(2, 10), RIDE_RATING(6, 50));
    ride_ratings_apply_scenery(&ratings, ride, 25098);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 0;
}

void RideRatingsCalculateReverseFreefallCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 25;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 00), RIDE_RATING(3, 20), RIDE_RATING(2, 80));
    ride_ratings_apply_length(&ratings, ride, 6000, 327);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 60), RIDE_RATING(0, 15));
    ride_ratings_apply_max_speed(&ratings, ride, 436906, 436906, 320398);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 41704, 59578);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 12850, 28398, 11702);
    ride_ratings_apply_proximity(state, &ratings, 17893);
    ride_ratings_apply_scenery(&ratings, ride, 11155);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 34, 2, 2, 2);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateLift(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 15;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 11), RIDE_RATING(0, 35), RIDE_RATING(0, 30));

    int32_t totalLength = ride.GetTotalLength() >> 16;
    ride_ratings_add(&ratings, (totalLength * 45875) >> 16, 0, (totalLength * 26214) >> 16);

    ride_ratings_apply_proximity(state, &ratings, 11183);
    ride_ratings_apply_scenery(&ratings, ride, 83662);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 7;

    if ((get_num_of_sheltered_eighths(ride).TrackShelteredEighths) >= 5)
        ride.excitement /= 4;
}

void RideRatingsCalculateVerticalDropRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 20), RIDE_RATING(0, 80), RIDE_RATING(0, 30));
    ride_ratings_apply_length(&ratings, ride, 4000, 1146);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_max_speed(&ratings, ride, 97418, 141699, 70849);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 40960, 35746, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 58254, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 6693);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 20, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
    ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 10), 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 1, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 40960, 35746, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateCashMachine(Ride& ride, RideRatingUpdateState& state)
{
    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

void RideRatingsCalculateTwist(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 13), RIDE_RATING(0, 97), RIDE_RATING(1, 90));
    ride_ratings_apply_rotations(&ratings, ride, 20, 20, 20);
    ride_ratings_apply_scenery(&ratings, ride, 13943);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 0;
}

void RideRatingsCalculateHauntedHouse(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 8;
    set_unreliability_factor(ride);

    RatingTuple ratings = {
        /* .excitement =  */ RIDE_RATING(3, 41),
        /* .intensity =  */ RIDE_RATING(1, 53),
        /* .nausea =  */ RIDE_RATING(0, 10),
    };

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 7;
}

void RideRatingsCalculateFlyingRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 17;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(4, 35), RIDE_RATING(1, 85), RIDE_RATING(4, 33));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 38130, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 6693);

    if (ride.inversions == 0)
        ratings.Excitement /= 2;

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 1, 1);

    if (ride.inversions == 0)
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 1, 1);

    ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 1, 1);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 38130, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateVirginiaReel(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 19;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 10), RIDE_RATING(1, 90), RIDE_RATING(3, 70));
    ride_ratings_apply_length(&ratings, ride, 6000, 873);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 110592, 29789, 59578);
    ride_ratings_apply_turns(&ratings, ride, 52012, 26075, 45749);
    ride_ratings_apply_drops(&ratings, ride, 43690, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 22367);
    ride_ratings_apply_scenery(&ratings, ride, 11155);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xD20000, 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 110592, 29789, 59578);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateSplashBoats(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 15;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 46), RIDE_RATING(0, 35), RIDE_RATING(0, 30));
    ride_ratings_apply_length(&ratings, ride, 2000, 7208);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_max_speed(&ratings, ride, 797059, 655360, 301111);
    ride_ratings_apply_duration(&ratings, ride, 500, 13107);
    ride_ratings_apply_turns(&ratings, ride, 22291, 20860, 4574);
    ride_ratings_apply_drops(&ratings, ride, 87381, 93622, 62259);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 22367);
    ride_ratings_apply_scenery(&ratings, ride, 11155);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateMiniHelicopters(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 12;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 60), RIDE_RATING(0, 40), RIDE_RATING(0, 00));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 15), RIDE_RATING(0, 00));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_turns(&ratings, ride, 14860, 0, 4574);
    ride_ratings_apply_drops(&ratings, ride, 8738, 0, 0);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 12850, 6553, 4681);
    ride_ratings_apply_proximity(state, &ratings, 8946);
    ride_ratings_apply_scenery(&ratings, ride, 8366);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xA00000, 2, 2, 2);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 6;
}

void RideRatingsCalculateLayDownRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 18;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 85), RIDE_RATING(1, 15), RIDE_RATING(2, 75));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 38130, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 6693);

    if (ride.inversions == 0)
    {
        ratings.Excitement /= 4;
        ratings.Intensity /= 2;
        ratings.Nausea /= 2;
    }

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

    if (ride.inversions == 0)
    {
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
        ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
    }

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 38130, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateSuspendedMonorail(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 14;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 15), RIDE_RATING(0, 23), RIDE_RATING(0, 8));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_train_length(&ratings, ride, 93622);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 70849, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 218453);
    ride_ratings_apply_duration(&ratings, ride, 150, 21845);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 5140, 6553, 18724);
    ride_ratings_apply_proximity(state, &ratings, 12525);
    ride_ratings_apply_scenery(&ratings, ride, 25098);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    auto shelteredEighths = get_num_of_sheltered_eighths(ride);
    if (shelteredEighths.TrackShelteredEighths >= 4)
        ride.excitement /= 4;

    ride.sheltered_eighths = shelteredEighths.TotalShelteredEighths;
}

void RideRatingsCalculateReverserRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 19;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 40), RIDE_RATING(1, 80), RIDE_RATING(1, 70));
    ride_ratings_apply_length(&ratings, ride, 6000, 873);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);

    int32_t numReversers = std::min<uint16_t>(state.AmountOfReversers, 6);
    ride_rating reverserRating = numReversers * RIDE_RATING(0, 20);
    ride_ratings_add(&ratings, reverserRating, reverserRating, reverserRating);

    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 28672, 23831, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 43458, 45749);
    ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 22367);
    ride_ratings_apply_scenery(&ratings, ride, 11155);

    if (state.AmountOfReversers < 1)
    {
        ratings.Excitement /= 8;
    }

    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 2, 1, 1);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 1, 1);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 28672, 23831, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateHeartlineTwisterCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 18;
    set_unreliability_factor(ride);

    RatingTuple ratings;
#ifdef ORIGINAL_RATINGS
    ride_ratings_set(&ratings, RIDE_RATING(1, 40), RIDE_RATING(1, 70), RIDE_RATING(1, 65));
#else
    ride_ratings_set(&ratings, RIDE_RATING(3, 00), RIDE_RATING(1, 70), RIDE_RATING(1, 65));
#endif
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 20), RIDE_RATING(0, 04));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 97418, 123987, 70849);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 44683, 89367);
    ride_ratings_apply_turns(&ratings, ride, 26749, 52150, 57186);
    ride_ratings_apply_drops(&ratings, ride, 29127, 53052, 55705);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 34952, 35108);
    ride_ratings_apply_proximity(state, &ratings, 9841);
    ride_ratings_apply_scenery(&ratings, ride, 3904);

    if (ride.inversions == 0)
        ratings.Excitement /= 4;

    ride_ratings_apply_num_drops_penalty(&ratings, ride, 1, 4, 1, 1);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 44683, 89367);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateMiniGolf(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 0;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 50), RIDE_RATING(0, 90), RIDE_RATING(0, 00));
    ride_ratings_apply_length(&ratings, ride, 6000, 873);
    ride_ratings_apply_turns(&ratings, ride, 14860, 0, 0);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 5140, 6553, 4681);
    ride_ratings_apply_proximity(state, &ratings, 15657);
    ride_ratings_apply_scenery(&ratings, ride, 27887);

    // Apply golf holes factor
    ride_ratings_add(&ratings, (ride.holes) * 5, 0, 0);

    // Apply no golf holes penalty
    if (ride.holes == 0)
    {
        ratings.Excitement /= 8;
        ratings.Intensity /= 2;
        ratings.Nausea /= 2;
    }

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateFirstAid(Ride& ride, RideRatingUpdateState& state)
{
    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

void RideRatingsCalculateCircus(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 9;
    set_unreliability_factor(ride);

    RatingTuple ratings = {
        /* .excitement = */ RIDE_RATING(2, 10),
        /* .intensity  = */ RIDE_RATING(0, 30),
        /* .nausea     = */ RIDE_RATING(0, 0),
    };

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 7;
}

void RideRatingsCalculateGhostTrain(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 12;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 00), RIDE_RATING(0, 20), RIDE_RATING(0, 03));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 15), RIDE_RATING(0, 00));
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_turns(&ratings, ride, 14860, 0, 11437);
    ride_ratings_apply_drops(&ratings, ride, 8738, 0, 0);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 25700, 6553, 4681);
    ride_ratings_apply_proximity(state, &ratings, 11183);
    ride_ratings_apply_scenery(&ratings, ride, 8366);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xB40000, 2, 2, 2);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateTwisterRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 15;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 50), RIDE_RATING(0, 40), RIDE_RATING(0, 30));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 32768, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 6693);

    if (ride.inversions == 0)
        ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

    if (ride.inversions == 0)
    {
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
        ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
    }

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 32768, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateWoodenRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 19;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 20), RIDE_RATING(2, 60), RIDE_RATING(2, 00));
    ride_ratings_apply_length(&ratings, ride, 6000, 873);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 40960, 34555, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 43458, 45749);
    ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 22367);
    ride_ratings_apply_scenery(&ratings, ride, 11155);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
    ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 10), 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 40960, 34555, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateSideFrictionRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 19;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 50), RIDE_RATING(2, 00), RIDE_RATING(1, 50));
    ride_ratings_apply_length(&ratings, ride, 6000, 873);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 28672, 35746, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 43458, 45749);
    ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 22367);
    ride_ratings_apply_scenery(&ratings, ride, 11155);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x50000, 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xFA0000, 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 28672, 35746, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateWildMouse(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 14;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 80), RIDE_RATING(2, 50), RIDE_RATING(2, 10));
    ride_ratings_apply_length(&ratings, ride, 6000, 873);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 8));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 102400, 35746, 49648);
    ride_ratings_apply_turns(&ratings, ride, 29721, 43458, 45749);
    ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 17893);
    ride_ratings_apply_scenery(&ratings, ride, 5577);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
    ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1, 50), 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 102400, 35746, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateMultiDimensionRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 18;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 75), RIDE_RATING(1, 95), RIDE_RATING(4, 79));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 38130, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 6693);

    if (ride.inversions == 0)
        ratings.Excitement /= 4;

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 1, 1);
    if (ride.inversions == 0)
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 1, 1);

    if (ride.inversions == 0)
        ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 1, 1);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 38130, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateGigaCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 14;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 85), RIDE_RATING(0, 40), RIDE_RATING(0, 35));
    ride_ratings_apply_length(&ratings, ride, 6000, 819);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 140434);
    ride_ratings_apply_max_speed(&ratings, ride, 51366, 85019, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 364088, 400497);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 36864, 30384, 49648);
    ride_ratings_apply_turns(&ratings, ride, 28235, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 43690, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 6693);

    if (ride.inversions == 0)
        ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 16, 2, 2, 2);

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

    if (ride.inversions == 0)
    {
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
        ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
    }

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 36864, 30384, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateRotoDrop(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 24;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 80), RIDE_RATING(3, 50), RIDE_RATING(3, 50));

    int32_t lengthFactor = ((ride.GetTotalLength() >> 16) * 209715) >> 16;
    ride_ratings_add(&ratings, lengthFactor, lengthFactor * 2, lengthFactor * 2);

    ride_ratings_apply_proximity(state, &ratings, 11183);
    ride_ratings_apply_scenery(&ratings, ride, 25098);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateFlyingSaucers(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 32;
    set_unreliability_factor(ride);

    RatingTuple ratings = {
        /* .excitement = */ RIDE_RATING(2, 40),
        /* .intensity =  */ RIDE_RATING(0, 55),
        /* .nausea =     */ RIDE_RATING(0, 39),
    };

    if (ride.NumTrains >= 4)
    {
        ride_ratings_add(&ratings, RIDE_RATING(0, 40), 0, 0);
    }

    ride_ratings_add(&ratings, ride.time_limit, ride.time_limit / 2, 0);

    if (ride.NumTrains >= 4)
    {
        ride_ratings_add(&ratings, RIDE_RATING(0, 40), 0, 0);
    }

    ride_ratings_apply_scenery(&ratings, ride, 5577);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 0;
}

void RideRatingsCalculateCrookedHouse(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 5;
    set_unreliability_factor(ride);

    RatingTuple ratings = {
        /* .excitement = */ RIDE_RATING(2, 15),
        /* .intensity  = */ RIDE_RATING(0, 62),
        /* .nausea     = */ RIDE_RATING(0, 34),
    };

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 7;
}

void RideRatingsCalculateMonorailCycles(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 4;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 40), RIDE_RATING(0, 20), RIDE_RATING(0, 00));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 15), RIDE_RATING(0, 00));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_turns(&ratings, ride, 14860, 0, 4574);
    ride_ratings_apply_drops(&ratings, ride, 8738, 0, 0);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 5140, 6553, 2340);
    ride_ratings_apply_proximity(state, &ratings, 8946);
    ride_ratings_apply_scenery(&ratings, ride, 11155);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0x8C0000, 2, 2, 2);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateCompactInvertedCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = ride.mode == RideMode::ReverseInclineLaunchedShuttle ? 31 : 21;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 15), RIDE_RATING(2, 80), RIDE_RATING(3, 20));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 42), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 30980, 55606);
    ride_ratings_apply_turns(&ratings, ride, 26749, 29552, 57186);
    ride_ratings_apply_drops(&ratings, ride, 29127, 39009, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 15291, 35108);
    ride_ratings_apply_proximity(state, &ratings, 15657);
    ride_ratings_apply_scenery(&ratings, ride, 8366);

    if (ride.inversions == 0)
        ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

    if (ride.inversions == 0)
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 30), 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 30980, 55606);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateWaterCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 14;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 70), RIDE_RATING(2, 80), RIDE_RATING(2, 10));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 20480, 23831, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 25700, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 9760);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 1, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 20480, 23831, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    if (!(ride.special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS))
        ratings.Excitement /= 8;

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateAirPoweredVerticalCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 28;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(4, 13), RIDE_RATING(2, 50), RIDE_RATING(2, 80));
    ride_ratings_apply_length(&ratings, ride, 6000, 327);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 60), RIDE_RATING(0, 05));
    ride_ratings_apply_max_speed(&ratings, ride, 509724, 364088, 320398);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 59578);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 21845, 11702);
    ride_ratings_apply_proximity(state, &ratings, 17893);
    ride_ratings_apply_scenery(&ratings, ride, 11155);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 34, 4, 1, 1);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 35746, 59578);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateInvertedHairpinCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 14;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 00), RIDE_RATING(2, 65), RIDE_RATING(2, 25));
    ride_ratings_apply_length(&ratings, ride, 6000, 873);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 8));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 102400, 35746, 49648);
    ride_ratings_apply_turns(&ratings, ride, 29721, 43458, 45749);
    ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 17893);
    ride_ratings_apply_scenery(&ratings, ride, 5577);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
    ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 10), 2, 2, 2);
    ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1, 50), 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 3, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 102400, 35746, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateMagicCarpet(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 45), RIDE_RATING(1, 60), RIDE_RATING(2, 60));

    ride_ratings_add(&ratings, ride.operation_option * 10, ride.operation_option * 20, ride.operation_option * 20);

    ride_ratings_apply_scenery(&ratings, ride, 11155);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 0;
}

void RideRatingsCalculateSubmarineRide(Ride& ride, RideRatingUpdateState& state)
{
    ride.unreliability_factor = 7;
    set_unreliability_factor(ride);

    // NOTE Fixed bug from original game, see boat Hire.

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 20), RIDE_RATING(1, 80), RIDE_RATING(1, 40));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_proximity(state, &ratings, 11183);
    ride_ratings_apply_scenery(&ratings, ride, 22310);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    // Originally, this was always to zero, even though the default vehicle is completely enclosed.
    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateRiverRafts(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 12;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(1, 45), RIDE_RATING(0, 25), RIDE_RATING(0, 34));
    ride_ratings_apply_length(&ratings, ride, 2000, 7208);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_max_speed(&ratings, ride, 531372, 655360, 301111);
    ride_ratings_apply_duration(&ratings, ride, 500, 13107);
    ride_ratings_apply_turns(&ratings, ride, 22291, 20860, 4574);
    ride_ratings_apply_drops(&ratings, ride, 78643, 93622, 62259);
    ride_ratings_apply_proximity(state, &ratings, 13420);
    ride_ratings_apply_scenery(&ratings, ride, 11155);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateEnterprise(Ride& ride, RideRatingUpdateState& state)
{
    ride.lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
    ride.lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
    ride.unreliability_factor = 22;
    set_unreliability_factor(ride);

    // Base ratings
    RatingTuple ratings = {
        /* .excitement = */ RIDE_RATING(3, 60),
        /* .intensity  = */ RIDE_RATING(4, 55),
        /* .nausea     = */ RIDE_RATING(5, 72),
    };

    ride_ratings_add(&ratings, ride.operation_option, ride.operation_option * 16, ride.operation_option * 16);

    ride_ratings_apply_scenery(&ratings, ride, 19521);

    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = 3;
}

void RideRatingsCalculateInvertedImpulseCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 20;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(4, 00), RIDE_RATING(3, 00), RIDE_RATING(3, 20));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 42), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 29789, 55606);
    ride_ratings_apply_turns(&ratings, ride, 26749, 29552, 57186);
    ride_ratings_apply_drops(&ratings, ride, 29127, 39009, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 15291, 35108);
    ride_ratings_apply_proximity(state, &ratings, 15657);
    ride_ratings_apply_scenery(&ratings, ride, 9760);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 20, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 29789, 55606);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateMiniRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 13;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 55), RIDE_RATING(2, 40), RIDE_RATING(1, 85));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 20480, 23831, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 25700, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 9760);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
    ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 50), 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 20480, 23831, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateMineRide(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 75), RIDE_RATING(1, 00), RIDE_RATING(1, 80));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 40960, 29789, 49648);
    ride_ratings_apply_turns(&ratings, ride, 29721, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 19275, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 21472);
    ride_ratings_apply_scenery(&ratings, ride, 16732);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0x10E0000, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 40960, 29789, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateLimLaunchedRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 25;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 90), RIDE_RATING(1, 50), RIDE_RATING(2, 20));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 6693);

    if (ride.inversions == 0)
        ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 10, 2, 2, 2);

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

    if (ride.inversions == 0)
    {
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, 10, 2, 2, 2);
        ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
    }

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 35746, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateClassicMiniRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 13;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 40), RIDE_RATING(2, 80), RIDE_RATING(1, 90));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 20480, 23831, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 25700, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 20130);
    ride_ratings_apply_scenery(&ratings, ride, 9760);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 20480, 23831, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateHybridCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 80), RIDE_RATING(1, 00), RIDE_RATING(0, 45));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 364088, 400497);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 40960, 35746, 49648);
    ride_ratings_apply_turns(&ratings, ride, 34179, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 34952, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 22367);
    ride_ratings_apply_scenery(&ratings, ride, 6693);

    if (ride.inversions == 0)
    {
        ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 14, 2, 2, 2);
    }

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

    if (ride.inversions == 0)
    {
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
        ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
    }

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 35746, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);
    ride.ratings = ratings;
    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateSingleRailRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 16;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(3, 50), RIDE_RATING(0, 60), RIDE_RATING(0, 40));
    ride_ratings_apply_length(&ratings, ride, 6000, 764);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 364088, 436906);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 36864, 35746, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 22367);
    ride_ratings_apply_scenery(&ratings, ride, 6693);

    if (ride.inversions == 0)
        ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 14, 2, 2, 2); // Done

    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2); // Done

    if (ride.inversions == 0)
    {
        ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2); // Done
        ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);                     // Done
    }

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 24576, 35746, 49648); // Done
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateAlpineCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 7;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 30), RIDE_RATING(2, 10), RIDE_RATING(1, 4));
    ride_ratings_apply_length(&ratings, ride, 6000, 873);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 75), RIDE_RATING(0, 5));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
    ride_ratings_apply_duration(&ratings, ride, 300, 26214);
    ride_ratings_apply_turns(&ratings, ride, 29721, 34767, 45749);
    ride_ratings_apply_drops(&ratings, ride, 8738, 5461, 6553);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
    ride_ratings_apply_proximity(state, &ratings, 22367);
    ride_ratings_apply_scenery(&ratings, ride, 11155);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x50000, 2, 2, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

void RideRatingsCalculateClassicWoodenRollerCoaster(Ride& ride, RideRatingUpdateState& state)
{
    if (!(ride.lifecycle_flags & RIDE_LIFECYCLE_TESTED))
        return;

    ride.unreliability_factor = 19;
    set_unreliability_factor(ride);

    RatingTuple ratings;
    ride_ratings_set(&ratings, RIDE_RATING(2, 80), RIDE_RATING(2, 60), RIDE_RATING(2, 00));
    ride_ratings_apply_length(&ratings, ride, 6000, 873);
    ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0, 40), RIDE_RATING(0, 05));
    ride_ratings_apply_train_length(&ratings, ride, 187245);
    ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
    ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
    ride_ratings_apply_duration(&ratings, ride, 150, 26214);
    ride_ratings_apply_gforces(&ratings, ride, 40960, 34555, 49648);
    ride_ratings_apply_turns(&ratings, ride, 26749, 43458, 45749);
    ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
    ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
    ride_ratings_apply_proximity(state, &ratings, 22367);
    ride_ratings_apply_scenery(&ratings, ride, 11155);
    ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 1, 2);
    ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 1, 2);
    ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 10), 2, 1, 2);
    ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 1, 2);
    ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 1, 2);

    ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride, 40960, 34555, 49648);
    ride_ratings_apply_intensity_penalty(&ratings);
    ride_ratings_apply_adjustments(ride, &ratings);

    ride.ratings = ratings;

    ride.upkeep_cost = ride_compute_upkeep(state, ride);
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

    ride.sheltered_eighths = get_num_of_sheltered_eighths(ride).TotalShelteredEighths;
}

#pragma endregion

#pragma region Ride rating calculation function table

ride_ratings_calculation RideRatingsGetCalculateFunc(ride_type_t rideType)
{
    return GetRideTypeDescriptor(rideType).RatingsCalculationFunction;
}

#pragma endregion
