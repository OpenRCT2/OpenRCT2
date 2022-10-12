/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GuestPathfinding.h"

#include "../core/Guard.hpp"
#include "../entity/Guest.h"
#include "../entity/Staff.h"
#include "../profiling/Profiling.h"
#include "../ride/RideData.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"

#include <bitset>
#include <cstring>

using namespace OpenRCT2;

static bool _peepPathFindIsStaff;
static int8_t _peepPathFindNumJunctions;
static int8_t _peepPathFindMaxJunctions;
static int32_t _peepPathFindTilesChecked;
static uint8_t _peepPathFindFewestNumSteps;

TileCoordsXYZ gPeepPathFindGoalPosition;
bool gPeepPathFindIgnoreForeignQueues;
RideId gPeepPathFindQueueRideIndex;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
// Use to guard calls to log messages
static bool _pathFindDebug = false;
// Use to put the peep name in the log message
static utf8 _pathFindDebugPeepName[256];
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

static int32_t guest_surface_path_finding(Peep& peep);

/* A junction history for the peep pathfinding heuristic search
 * The magic number 16 is the largest value returned by
 * peep_pathfind_get_max_number_junctions() which should eventually
 * be declared properly. */
static struct
{
    TileCoordsXYZ location;
    Direction direction;
} _peepPathFindHistory[16];

enum
{
    PATH_SEARCH_DEAD_END,
    PATH_SEARCH_WIDE,
    PATH_SEARCH_THIN,
    PATH_SEARCH_JUNCTION,
    PATH_SEARCH_RIDE_QUEUE,
    PATH_SEARCH_RIDE_ENTRANCE,
    PATH_SEARCH_RIDE_EXIT,
    PATH_SEARCH_PARK_EXIT,
    PATH_SEARCH_SHOP_ENTRANCE,
    PATH_SEARCH_LIMIT_REACHED,
    PATH_SEARCH_LOOP,
    PATH_SEARCH_OTHER,
    PATH_SEARCH_FAILED
};

static TileElement* get_banner_on_path(TileElement* path_element)
{
    // This is an improved version of original.
    // That only checked for one fence in the way.
    if (path_element->IsLastForTile())
        return nullptr;

    TileElement* bannerElement = path_element + 1;
    do
    {
        // Path on top, so no banners
        if (bannerElement->GetType() == TileElementType::Path)
            return nullptr;
        // Found a banner
        if (bannerElement->GetType() == TileElementType::Banner)
            return bannerElement;
        // Last element so there can't be any other banners
        if (bannerElement->IsLastForTile())
            return nullptr;

    } while (bannerElement++ != nullptr);

    return nullptr;
}

static int32_t banner_clear_path_edges(PathElement* pathElement, int32_t edges)
{
    if (_peepPathFindIsStaff)
        return edges;
    TileElement* bannerElement = get_banner_on_path(reinterpret_cast<TileElement*>(pathElement));
    if (bannerElement != nullptr)
    {
        do
        {
            edges &= bannerElement->AsBanner()->GetAllowedEdges();
        } while ((bannerElement = get_banner_on_path(bannerElement)) != nullptr);
    }
    return edges;
}

/**
 * Gets the connected edges of a path that are permitted (i.e. no 'no entry' signs)
 */
static int32_t path_get_permitted_edges(PathElement* pathElement)
{
    return banner_clear_path_edges(pathElement, pathElement->GetEdgesAndCorners()) & 0x0F;
}

/**
 *
 *  rct2: 0x0069524E
 */
static int32_t peep_move_one_tile(Direction direction, Peep& peep)
{
    assert(DirectionValid(direction));
    auto newTile = CoordsXY{ CoordsXY{ peep.NextLoc } + CoordsDirectionDelta[direction] }.ToTileCentre();

    if (newTile.x >= MAXIMUM_MAP_SIZE_BIG || newTile.y >= MAXIMUM_MAP_SIZE_BIG)
    {
        // This could loop!
        return guest_surface_path_finding(peep);
    }

    peep.PeepDirection = direction;
    if (peep.State != PeepState::Queuing)
    {
        // When peeps are walking along a path, we would like them to be spread out across the width of the path,
        // instead of all walking along the exact centre line of the path.
        //
        // Setting a random DestinationTolerance does not work very well for this. It means that peeps will make
        // their new pathfinding decision at a random time, and so will distribute a bit when they are turning
        // corners (which is good); but, as they walk along a straight path, they will - eventually - have had a
        // low tolerance value which forced them back to the centre of the path, where they stay until they turn
        // a corner.
        //
        // What we want instead is to apply that randomness in the direction they are walking ONLY, and keep their
        // other coordinate constant.
        //
        // However, we have also seen some situations where guests end up too far from the centre of paths. We've
        // not identified exactly what causes this yet, but to limit the impact of it, we don't just keep the other
        // coordinate constant, but instead clamp it to an acceptable range. This brings in 'outlier' guests from
        // the edges of the path, while allowing guests who are already in an acceptable position to stay there.

        const int8_t offset = (scenario_rand() & 7) - 3;
        if (direction == 0 || direction == 2)
        {
            // Peep is moving along X, so apply the offset to the X position of the destination and clamp their current Y
            const int32_t centreLine = (peep.y & 0xFFE0) + COORDS_XY_HALF_TILE;
            newTile.x += offset;
            newTile.y = std::clamp<int32_t>(peep.y, centreLine - 3, centreLine + 3);
        }
        else
        {
            // Peep is moving along Y, so apply the offset to the Y position of the destination and clamp their current X
            const int32_t centreLine = (peep.x & 0xFFE0) + COORDS_XY_HALF_TILE;
            newTile.x = std::clamp<int32_t>(peep.x, centreLine - 3, centreLine + 3);
            newTile.y += offset;
        }
    }
    peep.SetDestination(newTile, 2);

    return 0;
}

/**
 *
 *  rct2: 0x00694C41
 */
static int32_t guest_surface_path_finding(Peep& peep)
{
    auto pathPos = CoordsXYRangedZ{ peep.NextLoc, peep.NextLoc.z, peep.NextLoc.z + PATH_CLEARANCE };
    Direction randDirection = scenario_rand() & 3;

    if (!WallInTheWay(pathPos, randDirection))
    {
        pathPos.x += CoordsDirectionDelta[randDirection].x;
        pathPos.y += CoordsDirectionDelta[randDirection].y;
        Direction backwardsDirection = DirectionReverse(randDirection);

        if (!WallInTheWay(pathPos, backwardsDirection))
        {
            if (!MapSurfaceIsBlocked(pathPos))
            {
                return peep_move_one_tile(randDirection, peep);
            }
        }
    }

    randDirection++;
    uint8_t rand_backwards = scenario_rand() & 1;
    if (rand_backwards)
    {
        randDirection -= 2;
    }
    randDirection &= 3;

    pathPos.x = peep.NextLoc.x;
    pathPos.y = peep.NextLoc.y;
    if (!WallInTheWay(pathPos, randDirection))
    {
        pathPos.x += CoordsDirectionDelta[randDirection].x;
        pathPos.y += CoordsDirectionDelta[randDirection].y;
        Direction backwardsDirection = DirectionReverse(randDirection);

        if (!WallInTheWay(pathPos, backwardsDirection))
        {
            if (!MapSurfaceIsBlocked(pathPos))
            {
                return peep_move_one_tile(randDirection, peep);
            }
        }
    }

    randDirection -= 2;
    randDirection &= 3;

    pathPos.x = peep.NextLoc.x;
    pathPos.y = peep.NextLoc.y;
    if (!WallInTheWay(pathPos, randDirection))
    {
        pathPos.x += CoordsDirectionDelta[randDirection].x;
        pathPos.y += CoordsDirectionDelta[randDirection].y;
        Direction backwardsDirection = DirectionReverse(randDirection);

        if (!WallInTheWay(pathPos, backwardsDirection))
        {
            if (!MapSurfaceIsBlocked(pathPos))
            {
                return peep_move_one_tile(randDirection, peep);
            }
        }
    }

    randDirection--;
    if (rand_backwards)
    {
        randDirection += 2;
    }
    randDirection &= 3;
    return peep_move_one_tile(randDirection, peep);
}

/**
 *
 * Returns:
 *   1 - PATH_SEARCH_WIDE (path with wide flag set)
 *   4 - PATH_SEARCH_RIDE_QUEUE (queue path connected to a ride)
 *   11 - PATH_SEARCH_OTHER (other path than the above)
 *   12 - PATH_SEARCH_FAILED (no path element found)
 *
 *  rct2: 0x00694BAE
 *
 * Returns the type of the next footpath tile a peep can get to from x,y,z /
 * inputTileElement in the given direction.
 */
static uint8_t footpath_element_next_in_direction(TileCoordsXYZ loc, PathElement* pathElement, Direction chosenDirection)
{
    TileElement* nextTileElement;

    if (pathElement->IsSloped())
    {
        if (pathElement->GetSlopeDirection() == chosenDirection)
        {
            loc.z += 2;
        }
    }

    loc += TileDirectionDelta[chosenDirection];
    nextTileElement = MapGetFirstElementAt(loc);
    do
    {
        if (nextTileElement == nullptr)
            break;
        if (nextTileElement->IsGhost())
            continue;
        if (nextTileElement->GetType() != TileElementType::Path)
            continue;
        if (!GuestPathfinding::IsValidPathZAndDirection(nextTileElement, loc.z, chosenDirection))
            continue;
        if (nextTileElement->AsPath()->IsWide())
            return PATH_SEARCH_WIDE;
        // Only queue tiles that are connected to a ride are returned as ride queues.
        if (nextTileElement->AsPath()->IsQueue() && !nextTileElement->AsPath()->GetRideIndex().IsNull())
            return PATH_SEARCH_RIDE_QUEUE;

        return PATH_SEARCH_OTHER;
    } while (!(nextTileElement++)->IsLastForTile());

    return PATH_SEARCH_FAILED;
}

/**
 *
 * Returns:
 *   0 - PATH_SEARCH_DEAD_END (path is a dead end, i.e. < 2 edges)
 *   1 - PATH_SEARCH_WIDE (path with wide flag set)
 *   3 - PATH_SEARCH_JUNCTION (path is a junction, i.e. > 2 edges)
 *   5 - PATH_SEARCH_RIDE_ENTRANCE (map element is a ride entrance)
 *   6 - PATH_SEARCH_RIDE_EXIT (map element is a ride exit)
 *   7 - PATH_SEARCH_PARK_EXIT park entrance / exit (map element is a park entrance/exit)
 *   8 - PATH_SEARCH_SHOP_ENTRANCE (map element is a shop entrance)
 *   9 - PATH_SEARCH_LIMIT_REACHED (search limit reached without reaching path end)
 *   12 - PATH_SEARCH_FAILED (no path element found)
 * For return values 5, 6 & 8 the rideIndex is stored in outRideIndex.
 *
 *  rct2: 0x006949B9
 *
 * This is the recursive portion of footpath_element_destination_in_direction().
 */
static uint8_t footpath_element_dest_in_dir(TileCoordsXYZ loc, Direction chosenDirection, RideId* outRideIndex, int32_t level)
{
    TileElement* tileElement;
    Direction direction;

    if (level > 25)
        return PATH_SEARCH_LIMIT_REACHED;

    loc += TileDirectionDelta[chosenDirection];
    tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
    {
        return PATH_SEARCH_FAILED;
    }
    do
    {
        if (tileElement->IsGhost())
            continue;

        switch (tileElement->GetType())
        {
            case TileElementType::Track:
            {
                if (loc.z != tileElement->base_height)
                    continue;
                RideId rideIndex = tileElement->AsTrack()->GetRideIndex();
                auto ride = get_ride(rideIndex);
                if (ride != nullptr && ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
                {
                    *outRideIndex = rideIndex;
                    return PATH_SEARCH_SHOP_ENTRANCE;
                }
            }
            break;
            case TileElementType::Entrance:
                if (loc.z != tileElement->base_height)
                    continue;
                switch (tileElement->AsEntrance()->GetEntranceType())
                {
                    case ENTRANCE_TYPE_RIDE_ENTRANCE:
                        direction = tileElement->GetDirection();
                        if (direction == chosenDirection)
                        {
                            *outRideIndex = tileElement->AsEntrance()->GetRideIndex();
                            return PATH_SEARCH_RIDE_ENTRANCE;
                        }
                        break;
                    case ENTRANCE_TYPE_RIDE_EXIT:
                        direction = tileElement->GetDirection();
                        if (direction == chosenDirection)
                        {
                            *outRideIndex = tileElement->AsEntrance()->GetRideIndex();
                            return PATH_SEARCH_RIDE_EXIT;
                        }
                        break;
                    case ENTRANCE_TYPE_PARK_ENTRANCE:
                        return PATH_SEARCH_PARK_EXIT;
                }
                break;
            case TileElementType::Path:
            {
                if (!GuestPathfinding::IsValidPathZAndDirection(tileElement, loc.z, chosenDirection))
                    continue;
                if (tileElement->AsPath()->IsWide())
                    return PATH_SEARCH_WIDE;

                uint8_t edges = path_get_permitted_edges(tileElement->AsPath());
                edges &= ~(1 << DirectionReverse(chosenDirection));
                loc.z = tileElement->base_height;

                for (Direction dir : ALL_DIRECTIONS)
                {
                    if (!(edges & (1 << dir)))
                        continue;

                    edges &= ~(1 << dir);
                    if (edges != 0)
                        return PATH_SEARCH_JUNCTION;

                    if (tileElement->AsPath()->IsSloped())
                    {
                        if (tileElement->AsPath()->GetSlopeDirection() == dir)
                        {
                            loc.z += 2;
                        }
                    }
                    return footpath_element_dest_in_dir(loc, dir, outRideIndex, level + 1);
                }
                return PATH_SEARCH_DEAD_END;
            }
            default:
                break;
        }
    } while (!(tileElement++)->IsLastForTile());

    return PATH_SEARCH_FAILED;
}

/**
 * Returns:
 *   0 - PATH_SEARCH_DEAD_END (path is a dead end, i.e. < 2 edges)
 *   1 - PATH_SEARCH_WIDE (path with wide flag set)
 *   3 - PATH_SEARCH_JUNCTION (path is a junction, i.e. > 2 edges)
 *   5 - PATH_SEARCH_RIDE_ENTRANCE (map element is a ride entrance)
 *   6 - PATH_SEARCH_RIDE_EXIT (map element is a ride exit)
 *   7 - PATH_SEARCH_PARK_EXIT park entrance / exit (map element is a park entrance/exit)
 *   8 - PATH_SEARCH_SHOP_ENTRANCE (map element is a shop entrance)
 *   9 - PATH_SEARCH_LIMIT_REACHED (search limit reached without reaching path end)
 *   12 - PATH_SEARCH_FAILED (no path element found)
 * For return values 5, 6 & 8 the rideIndex is stored in outRideIndex.
 *
 *  rct2: 0x006949A4
 *
 * Returns the destination tile type a peep can get to from x,y,z /
 * inputTileElement in the given direction following single width paths only
 * and stopping as soon as a path junction is encountered.
 * Note that a junction is a path with > 2 reachable neighbouring path tiles,
 * so wide paths have LOTS of junctions.
 * This is useful for finding out what is at the end of a short single
 * width path, for example that leads from a ride exit back to the main path.
 */
static uint8_t footpath_element_destination_in_direction(
    TileCoordsXYZ loc, PathElement* pathElement, Direction chosenDirection, RideId* outRideIndex)
{
    if (pathElement->IsSloped())
    {
        if (pathElement->GetSlopeDirection() == chosenDirection)
        {
            loc.z += 2;
        }
    }

    return footpath_element_dest_in_dir(loc, chosenDirection, outRideIndex, 0);
}

/**
 *
 *  rct2: 0x00695225
 */
static int32_t guest_path_find_aimless(Peep& peep, uint8_t edges)
{
    if (scenario_rand() & 1)
    {
        // If possible go straight
        if (edges & (1 << peep.PeepDirection))
        {
            return peep_move_one_tile(peep.PeepDirection, peep);
        }
    }

    while (true)
    {
        Direction direction = scenario_rand() & 3;
        // Otherwise go in a random direction allowed from the tile.
        if (edges & (1 << direction))
        {
            return peep_move_one_tile(direction, peep);
        }
    }
}

/**
 *
 *  rct2: 0x0069A60A
 */
static uint8_t peep_pathfind_get_max_number_junctions(Peep& peep)
{
    if (peep.Is<Staff>())
        return 8;

    // PEEP_FLAGS_2? It's cleared here but not set anywhere!
    if ((peep.PeepFlags & PEEP_FLAGS_2))
    {
        if ((scenario_rand() & 0xFFFF) <= 7281)
            peep.PeepFlags &= ~PEEP_FLAGS_2;

        return 8;
    }

    auto* guest = peep.As<Guest>();
    if (guest == nullptr)
        return 8;

    if (guest->PeepFlags & PEEP_FLAGS_LEAVING_PARK && guest->GuestIsLostCountdown < 90)
    {
        return 8;
    }

    if (guest->HasItem(ShopItem::Map))
        return 7;

    if (guest->PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        return 7;

    return 5;
}

/**
 * Returns if the path as xzy is a 'thin' junction.
 * A junction is considered 'thin' if it has more than 2 edges
 * leading to/from non-wide path elements; edges leading to/from non-path
 * elements (e.g. ride/shop entrances) or ride queues are not counted,
 * since entrances and ride queues coming off a path should not result in
 * the path being considered a junction.
 */
static bool path_is_thin_junction(PathElement* path, const TileCoordsXYZ& loc)
{
    PROFILED_FUNCTION();

    uint8_t edges = path->GetEdges();

    int32_t test_edge = bitscanforward(edges);
    if (test_edge == -1)
        return false;

    bool thin_junction = false;
    int32_t thin_count = 0;
    do
    {
        int32_t fp_result = footpath_element_next_in_direction(loc, path, test_edge);

        /* Ignore non-paths (e.g. ride entrances, shops), wide paths
         * and ride queues (per ignoreQueues) when counting
         * neighbouring tiles. */
        if (fp_result != PATH_SEARCH_FAILED && fp_result != PATH_SEARCH_WIDE && fp_result != PATH_SEARCH_RIDE_QUEUE)
        {
            thin_count++;
        }

        if (thin_count > 2)
        {
            thin_junction = true;
            break;
        }
        edges &= ~(1 << test_edge);
    } while ((test_edge = bitscanforward(edges)) != -1);
    return thin_junction;
}

static int32_t CalculateHeuristicPathingScore(const TileCoordsXYZ& loc1, const TileCoordsXYZ& loc2)
{
    auto xDelta = abs(loc1.x - loc2.x) * 32;
    auto yDelta = abs(loc1.y - loc2.y) * 32;
    auto zDelta = abs(loc1.z - loc2.z) * 2;

    if (xDelta < yDelta)
        xDelta >>= 4;
    else
        yDelta >>= 4;

    return xDelta + yDelta + zDelta;
}

#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
static constexpr const char* pathSearchToString(uint8_t pathFindSearchResult)
{
    switch (pathFindSearchResult)
    {
        case PATH_SEARCH_DEAD_END:
            return "DeadEnd";
        case PATH_SEARCH_WIDE:
            return "Wide";
        case PATH_SEARCH_THIN:
            return "Thin";
        case PATH_SEARCH_JUNCTION:
            return "Junction";
        case PATH_SEARCH_RIDE_QUEUE:
            return "RideQueue";
        case PATH_SEARCH_RIDE_ENTRANCE:
            return "RideEntrance";
        case PATH_SEARCH_RIDE_EXIT:
            return "RideExit";
        case PATH_SEARCH_PARK_EXIT:
            return "ParkEntryExit";
        case PATH_SEARCH_SHOP_ENTRANCE:
            return "ShopEntrance";
        case PATH_SEARCH_LIMIT_REACHED:
            return "LimitReached";
        case PATH_SEARCH_OTHER:
            return "Other";
        case PATH_SEARCH_FAILED:
            return "Failed";
            // The default case is omitted intentionally.
    }

    return "Unknown";
}
#endif

/**
 * Searches for the tile with the best heuristic score within the search limits
 * starting from the given tile x,y,z and going in the given direction test_edge.
 * The best heuristic score is tracked and returned in the call parameters
 * along with the corresponding tile location and search path telemetry
 * (junctions passed through and directions taken).
 *
 * The primary heuristic used is distance from the goal; the secondary
 * heuristic used (when the primary heuristic gives equal scores) is the number
 * of steps. i.e. the search gets as close as possible to the goal in as few
 * steps as possible.
 *
 * Each tile is checked to determine if the goal is reached.
 * When the goal is not reached the search result is only updated at the END
 * of each search path (some map element that is not a path or a path at which
 * a search limit is reached), NOT at each step along the way.
 * This means that the search ignores thin paths that are "no through paths"
 * no matter how close to the goal they get, but will follow possible "through
 * paths".
 *
 * The implementation is a depth first search of the path layout in xyz
 * according to the search limits.
 * Unlike an A* search, which tracks for each tile a heuristic score (a
 * function of the xyz distances to the goal) and cost of reaching that tile
 * (steps to the tile), a single best result "so far" (best heuristic score
 * with least cost) is tracked via the score parameter.
 * With this approach, explicit loop detection is necessary to limit the
 * search space, and each alternate route through the same tile can be
 * returned as the best result, rather than only the shortest route with A*.
 *
 * The parameters that hold the best search result so far are:
 *   - score - the least heuristic distance from the goal
 *   - endSteps - the least number of steps that achieve the score.
 *
 * The following parameters provide telemetry information on best search path so far:
 *   - endXYZ tracks the end location of the search path.
 *   - endSteps tracks the number of steps to the end of the search path.
 *   - endJunctions tracks the number of junctions passed through in the
 *     search path.
 *   - junctionList[] and directionList[] track the junctions and
 *     corresponding directions of the search path.
 * Other than debugging purposes, these could potentially be used to visualise
 * the pathfinding on the map.
 *
 * The parameters/variables that limit the search space are:
 *   - counter (param) - number of steps walked in the current search path;
 *   - _peepPathFindTilesChecked (variable) - cumulative number of tiles that can be
 *     checked in the entire search;
 *   - _peepPathFindNumJunctions (variable) - number of thin junctions that can be
 *     checked in a single search path;
 *
 * Other global variables/state that affect the search space are:
 *   - Wide paths - to handle broad paths (> 1 tile wide), the search navigates
 *     along non-wide (or 'thin' paths) and stops as soon as it encounters a
 *     wide path. This means peeps heading for a destination will only leave
 *     thin paths if walking 1 tile onto a wide path is closer than following
 *     non-wide paths;
 *   - gPeepPathFindIgnoreForeignQueues
 *   - gPeepPathFindQueueRideIndex - the ride the peep is heading for
 *   - _peepPathFindHistory - the search path telemetry consisting of the
 *     starting point and all thin junctions with directions navigated
 *     in the current search path - also used to detect path loops.
 *
 * The score is only updated when:
 *   - the goal is reached;
 *   - a wide tile is encountered with a better search result - the goal may
 *     still be reachable from here (only if the current tile is also wide);
 *   - a junction is encountered with a better search result and
 *     maxNumJunctions is exceeded - the goal may still be reachable from here;
 *   - returning from a recursive call if a search limit (i.e. either
 *     maxNumStep or maxTilesChecked) was reached and the current tile has a
 *     better search result and the goal may still be reachable from here
 *     (i.e. not a dead end path tile).
 *
 *  rct2: 0x0069A997
 */
static void peep_pathfind_heuristic_search(
    TileCoordsXYZ loc, Peep& peep, TileElement* currentTileElement, bool inPatrolArea, uint8_t counter, uint16_t* endScore,
    Direction test_edge, uint8_t* endJunctions, TileCoordsXYZ junctionList[16], uint8_t directionList[16],
    TileCoordsXYZ* endXYZ, uint8_t* endSteps)
{
    uint8_t searchResult = PATH_SEARCH_FAILED;

    bool currentElementIsWide = currentTileElement->AsPath()->IsWide();
    if (currentElementIsWide)
    {
        const Staff* staff = peep.As<Staff>();
        if (staff != nullptr && staff->CanIgnoreWideFlag(loc.ToCoordsXYZ(), currentTileElement))
            currentElementIsWide = false;
    }

    loc += TileDirectionDelta[test_edge];

    ++counter;
    _peepPathFindTilesChecked--;

    /* If this is where the search started this is a search loop and the
     * current search path ends here.
     * Return without updating the parameters (best result so far). */
    if (_peepPathFindHistory[0].location == loc)
    {
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug)
        {
            log_info("[%03d] Return from %d,%d,%d; At start", counter, loc.x >> 5, loc.y >> 5, loc.z);
        }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        return;
    }

    bool nextInPatrolArea = inPatrolArea;
    auto* staff = peep.As<Staff>();
    if (staff != nullptr && staff->IsMechanic())
    {
        nextInPatrolArea = staff->IsLocationInPatrol(loc.ToCoordsXY());
        if (inPatrolArea && !nextInPatrolArea)
        {
/* The mechanic will leave his patrol area by taking
 * the test_edge so the current search path ends here.
 * Return without updating the parameters (best result so far). */
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info("[%03d] Return from %d,%d,%d; Left patrol area", counter, loc.x >> 5, loc.y >> 5, loc.z);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            return;
        }
    }

    /* Get the next map element of interest in the direction of test_edge. */
    bool found = false;
    TileElement* tileElement = MapGetFirstElementAt(loc);
    if (tileElement == nullptr)
    {
        return;
    }
    do
    {
        /* Look for all map elements that the peep could walk onto while
         * navigating to the goal, including the goal tile. */

        if (tileElement->IsGhost())
            continue;

        RideId rideIndex = RideId::GetNull();
        switch (tileElement->GetType())
        {
            case TileElementType::Track:
            {
                if (loc.z != tileElement->base_height)
                    continue;
                /* For peeps heading for a shop, the goal is the shop
                 * tile. */
                rideIndex = tileElement->AsTrack()->GetRideIndex();
                auto ride = get_ride(rideIndex);
                if (ride == nullptr || !ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
                    continue;

                found = true;
                searchResult = PATH_SEARCH_SHOP_ENTRANCE;
                break;
            }
            case TileElementType::Entrance:
                if (loc.z != tileElement->base_height)
                    continue;
                Direction direction;
                searchResult = PATH_SEARCH_OTHER;
                switch (tileElement->AsEntrance()->GetEntranceType())
                {
                    case ENTRANCE_TYPE_RIDE_ENTRANCE:
                        /* For peeps heading for a ride without a queue, the
                         * goal is the ride entrance tile.
                         * For mechanics heading for the ride entrance
                         * (in the case when the station has no exit),
                         * the goal is the ride entrance tile. */
                        direction = tileElement->GetDirection();
                        if (direction == test_edge)
                        {
                            /* The rideIndex will be useful for
                             * adding transport rides later. */
                            rideIndex = tileElement->AsEntrance()->GetRideIndex();
                            searchResult = PATH_SEARCH_RIDE_ENTRANCE;
                            found = true;
                            break;
                        }
                        continue; // Ride entrance is not facing the right direction.
                    case ENTRANCE_TYPE_PARK_ENTRANCE:
                        /* For peeps leaving the park, the goal is the park
                         * entrance/exit tile. */
                        searchResult = PATH_SEARCH_PARK_EXIT;
                        found = true;
                        break;
                    case ENTRANCE_TYPE_RIDE_EXIT:
                        /* For mechanics heading for the ride exit, the
                         * goal is the ride exit tile. */
                        direction = tileElement->GetDirection();
                        if (direction == test_edge)
                        {
                            searchResult = PATH_SEARCH_RIDE_EXIT;
                            found = true;
                            break;
                        }
                        continue; // Ride exit is not facing the right direction.
                    default:
                        continue;
                }
                break;
            case TileElementType::Path:
            {
                /* For peeps heading for a ride with a queue, the goal is the last
                 * queue path.
                 * Otherwise, peeps walk on path tiles to get to the goal. */

                if (!GuestPathfinding::IsValidPathZAndDirection(tileElement, loc.z, test_edge))
                    continue;

                // Path may be sloped, so set z to path base height.
                loc.z = tileElement->base_height;

                if (tileElement->AsPath()->IsWide())
                {
                    /* Check if staff can ignore this wide flag. */
                    if (staff == nullptr || !staff->CanIgnoreWideFlag(loc.ToCoordsXYZ(), tileElement))
                    {
                        searchResult = PATH_SEARCH_WIDE;
                        found = true;
                        break;
                    }
                }

                searchResult = PATH_SEARCH_THIN;

                uint8_t numEdges = bitcount(tileElement->AsPath()->GetEdges());

                if (numEdges < 2)
                {
                    searchResult = PATH_SEARCH_DEAD_END;
                }
                else if (numEdges > 2)
                {
                    searchResult = PATH_SEARCH_JUNCTION;
                }
                else
                { // numEdges == 2
                    if (tileElement->AsPath()->IsQueue()
                        && tileElement->AsPath()->GetRideIndex() != gPeepPathFindQueueRideIndex)
                    {
                        if (gPeepPathFindIgnoreForeignQueues && !tileElement->AsPath()->GetRideIndex().IsNull())
                        {
                            // Path is a queue we aren't interested in
                            /* The rideIndex will be useful for
                             * adding transport rides later. */
                            rideIndex = tileElement->AsPath()->GetRideIndex();
                            searchResult = PATH_SEARCH_RIDE_QUEUE;
                        }
                    }
                }
                found = true;
            }
            break;
            default:
                continue;
        }

#
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug)
        {
            log_info(
                "[%03d] Checking map element at %d,%d,%d; Type: %s", counter, loc.x >> 5, loc.y >> 5, loc.z,
                pathSearchToString(searchResult));
        }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2

        /* At this point tileElement is of interest to the pathfinding. */

        /* Should we check that this tileElement is connected in the
         * reverse direction? For some tileElement types this was
         * already done above (e.g. ride entrances), but for others not.
         * Ignore for now. */

        // Calculate the heuristic score of this map element.
        uint16_t new_score = CalculateHeuristicPathingScore(loc, gPeepPathFindGoalPosition);

        /* If this map element is the search goal the current search path ends here. */
        if (new_score == 0)
        {
            /* If the search result is better than the best so far (in the parameters),
             * then update the parameters with this search before continuing to the next map element. */
            if (new_score < *endScore || (new_score == *endScore && counter < *endSteps))
            {
                // Update the search results
                *endScore = new_score;
                *endSteps = counter;
                // Update the end x,y,z
                *endXYZ = loc;
                // Update the telemetry
                *endJunctions = _peepPathFindMaxJunctions - _peepPathFindNumJunctions;
                for (uint8_t junctInd = 0; junctInd < *endJunctions; junctInd++)
                {
                    uint8_t histIdx = _peepPathFindMaxJunctions - junctInd;
                    junctionList[junctInd].x = _peepPathFindHistory[histIdx].location.x;
                    junctionList[junctInd].y = _peepPathFindHistory[histIdx].location.y;
                    junctionList[junctInd].z = _peepPathFindHistory[histIdx].location.z;
                    directionList[junctInd] = _peepPathFindHistory[histIdx].direction;
                }
            }
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info(
                    "[%03d] Search path ends at %d,%d,%d; At goal; Score: %d", counter, loc.x >> 5, loc.y >> 5, loc.z,
                    new_score);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        /* At this point the map element tile is not the goal. */

        /* If this map element is not a path, the search cannot be continued.
         * Continue to the next map element without updating the parameters (best result so far). */
        if (searchResult != PATH_SEARCH_DEAD_END && searchResult != PATH_SEARCH_THIN && searchResult != PATH_SEARCH_JUNCTION
            && searchResult != PATH_SEARCH_WIDE)
        {
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info("[%03d] Search path ends at %d,%d,%d; Not a path", counter, loc.x >> 5, loc.y >> 5, loc.z);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        /* At this point the map element is a path. */

        /* If this is a wide path the search ends here. */
        if (searchResult == PATH_SEARCH_WIDE)
        {
            /* Ignore Wide paths as continuing paths UNLESS
             * the current path is also Wide (and, for staff, not ignored).
             * This permits a peep currently on a wide path to
             * cross other wide paths to reach a thin path.
             *
             * So, if the current path is also wide the goal could
             * still be reachable from here.
             * If the search result is better than the best so far
             * (in the parameters), then update the parameters with
             * this search before continuing to the next map element. */
            if (currentElementIsWide && (new_score < *endScore || (new_score == *endScore && counter < *endSteps)))
            {
                // Update the search results
                *endScore = new_score;
                *endSteps = counter;
                // Update the end x,y,z
                *endXYZ = loc;
                // Update the telemetry
                *endJunctions = _peepPathFindMaxJunctions - _peepPathFindNumJunctions;
                for (uint8_t junctInd = 0; junctInd < *endJunctions; junctInd++)
                {
                    uint8_t histIdx = _peepPathFindMaxJunctions - junctInd;
                    junctionList[junctInd].x = _peepPathFindHistory[histIdx].location.x;
                    junctionList[junctInd].y = _peepPathFindHistory[histIdx].location.y;
                    junctionList[junctInd].z = _peepPathFindHistory[histIdx].location.z;
                    directionList[junctInd] = _peepPathFindHistory[histIdx].direction;
                }
            }
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info(
                    "[%03d] Search path ends at %d,%d,%d; Wide path; Score: %d", counter, loc.x >> 5, loc.y >> 5, loc.z,
                    new_score);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        /* At this point the map element is a non-wide path.*/

        /* Get all the permitted_edges of the map element. */
        Guard::Assert(tileElement->AsPath() != nullptr);
        uint8_t edges = path_get_permitted_edges(tileElement->AsPath());

#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug)
        {
            log_info(
                "[%03d] Path element at %d,%d,%d; Edges (0123):%d%d%d%d; Reverse: %d", counter, loc.x >> 5, loc.y >> 5, loc.z,
                edges & 1, (edges & 2) >> 1, (edges & 4) >> 2, (edges & 8) >> 3, test_edge ^ 2);
        }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2

        /* Remove the reverse edge (i.e. the edge back to the previous map element.) */
        edges &= ~(1 << DirectionReverse(test_edge));

        int32_t next_test_edge = bitscanforward(edges);

        /* If there are no other edges the current search ends here.
         * Continue to the next map element without updating the parameters (best result so far). */
        if (next_test_edge == -1)
        {
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info("[%03d] Search path ends at %d,%d,%d; No more edges/dead end", counter, loc.x >> 5, loc.y >> 5, loc.z);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        /* Check if either of the search limits has been reached:
         * - max number of steps or max tiles checked. */
        if (counter >= 200 || _peepPathFindTilesChecked <= 0)
        {
            /* The current search ends here.
             * The path continues, so the goal could still be reachable from here.
             * If the search result is better than the best so far (in the parameters),
             * then update the parameters with this search before continuing to the next map element. */
            if (new_score < *endScore || (new_score == *endScore && counter < *endSteps))
            {
                // Update the search results
                *endScore = new_score;
                *endSteps = counter;
                // Update the end x,y,z
                *endXYZ = loc;
                // Update the telemetry
                *endJunctions = _peepPathFindMaxJunctions - _peepPathFindNumJunctions;
                for (uint8_t junctInd = 0; junctInd < *endJunctions; junctInd++)
                {
                    uint8_t histIdx = _peepPathFindMaxJunctions - junctInd;
                    junctionList[junctInd].x = _peepPathFindHistory[histIdx].location.x;
                    junctionList[junctInd].y = _peepPathFindHistory[histIdx].location.y;
                    junctionList[junctInd].z = _peepPathFindHistory[histIdx].location.z;
                    directionList[junctInd] = _peepPathFindHistory[histIdx].direction;
                }
            }
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info(
                    "[%03d] Search path ends at %d,%d,%d; Search limit reached; Score: %d", counter, loc.x >> 5, loc.y >> 5,
                    loc.z, new_score);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        bool thin_junction = false;
        if (searchResult == PATH_SEARCH_JUNCTION)
        {
            /* Check if this is a thin junction. And perform additional
             * necessary checks. */
            thin_junction = path_is_thin_junction(tileElement->AsPath(), loc);

            if (thin_junction)
            {
                /* The current search path is passing through a thin
                 * junction on this map element. Only 'thin' junctions
                 * are counted towards the junction search limit. */

                /* First check if going through the junction would be
                 * a loop.  If so, the current search path ends here.
                 * Path finding loop detection can take advantage of both the
                 * peep.PathfindHistory - loops through remembered junctions
                 *     the peep has already passed through getting to its
                 *     current position while on the way to its current goal;
                 * _peepPathFindHistory - loops in the current search path. */
                bool pathLoop = false;
                /* Check the peep.PathfindHistory to see if this junction has
                 * already been visited by the peep while heading for this goal. */
                for (auto& pathfindHistory : peep.PathfindHistory)
                {
                    if (pathfindHistory == loc)
                    {
                        if (pathfindHistory.direction == 0)
                        {
                            /* If all directions have already been tried while
                             * heading to this goal, this is a loop. */
                            pathLoop = true;
                        }
                        else
                        {
                            /* The peep remembers walking through this junction
                             * before, but has not yet tried all directions.
                             * Limit the edges to search to those not yet tried. */
                            edges &= pathfindHistory.direction;
                        }
                        break;
                    }
                }

                if (!pathLoop)
                {
                    /* Check the _peepPathFindHistory to see if this junction has been
                     * previously passed through in the current search path.
                     * i.e. this is a loop in the current search path. */
                    for (int32_t junctionNum = _peepPathFindNumJunctions + 1; junctionNum <= _peepPathFindMaxJunctions;
                         junctionNum++)
                    {
                        if (_peepPathFindHistory[junctionNum].location == loc)
                        {
                            pathLoop = true;
                            break;
                        }
                    }
                }
                if (pathLoop)
                {
/* Loop detected.  The current search path ends here.
 * Continue to the next map element without updating the parameters (best result so far). */
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
                    if (gPathFindDebug)
                    {
                        log_info("[%03d] Search path ends at %d,%d,%d; Loop", counter, loc.x >> 5, loc.y >> 5, loc.z);
                    }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
                    continue;
                }

                /* If the junction search limit is reached, the
                 * current search path ends here. The goal may still
                 * be reachable from here.
                 * If the search result is better than the best so far (in the parameters),
                 * then update the parameters with this search before continuing to the next map element. */
                if (_peepPathFindNumJunctions <= 0)
                {
                    if (new_score < *endScore || (new_score == *endScore && counter < *endSteps))
                    {
                        // Update the search results
                        *endScore = new_score;
                        *endSteps = counter;
                        // Update the end x,y,z
                        *endXYZ = loc;
                        // Update the telemetry
                        *endJunctions = _peepPathFindMaxJunctions; // - _peepPathFindNumJunctions;
                        for (uint8_t junctInd = 0; junctInd < *endJunctions; junctInd++)
                        {
                            uint8_t histIdx = _peepPathFindMaxJunctions - junctInd;
                            junctionList[junctInd] = _peepPathFindHistory[histIdx].location;
                            directionList[junctInd] = _peepPathFindHistory[histIdx].direction;
                        }
                    }
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
                    if (gPathFindDebug)
                    {
                        log_info(
                            "[%03d] Search path ends at %d,%d,%d; NumJunctions < 0; Score: %d", counter, loc.x >> 5, loc.y >> 5,
                            loc.z, new_score);
                    }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
                    continue;
                }

                /* This junction was NOT previously visited in the current
                 * search path, so add the junction to the history. */
                _peepPathFindHistory[_peepPathFindNumJunctions].location = loc;
                // .direction take is added below.

                _peepPathFindNumJunctions--;
            }
        }

        /* Continue searching down each remaining edge of the path
         * (recursive call). */
        do
        {
            edges &= ~(1 << next_test_edge);
            uint8_t savedNumJunctions = _peepPathFindNumJunctions;

            uint8_t height = loc.z;
            if (tileElement->AsPath()->IsSloped() && tileElement->AsPath()->GetSlopeDirection() == next_test_edge)
            {
                height += 2;
            }
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                if (searchResult == PATH_SEARCH_JUNCTION)
                {
                    if (thin_junction)
                        log_info(
                            "[%03d] Recurse from %d,%d,%d edge: %d; Thin-Junction", counter, loc.x >> 5, loc.y >> 5, loc.z,
                            next_test_edge);
                    else
                        log_info(
                            "[%03d] Recurse from %d,%d,%d edge: %d; Wide-Junction", counter, loc.x >> 5, loc.y >> 5, loc.z,
                            next_test_edge);
                }
                else
                {
                    log_info(
                        "[%03d] Recurse from %d,%d,%d edge: %d; Segment", counter, loc.x >> 5, loc.y >> 5, loc.z,
                        next_test_edge);
                }
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2

            if (thin_junction)
            {
                /* Add the current test_edge to the history. */
                _peepPathFindHistory[_peepPathFindNumJunctions + 1].direction = next_test_edge;
            }

            peep_pathfind_heuristic_search(
                { loc.x, loc.y, height }, peep, tileElement, nextInPatrolArea, counter, endScore, next_test_edge, endJunctions,
                junctionList, directionList, endXYZ, endSteps);
            _peepPathFindNumJunctions = savedNumJunctions;

#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info(
                    "[%03d] Returned to %d,%d,%d edge: %d; Score: %d", counter, loc.x >> 5, loc.y >> 5, loc.z, next_test_edge,
                    *endScore);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        } while ((next_test_edge = bitscanforward(edges)) != -1);

    } while (!(tileElement++)->IsLastForTile());

    if (!found)
    {
/* No map element could be found.
 * Return without updating the parameters (best result so far). */
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug)
        {
            log_info("[%03d] Returning from %d,%d,%d; No relevant map element found", counter, loc.x >> 5, loc.y >> 5, loc.z);
        }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
    }
    else
    {
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug)
        {
            log_info("[%03d] Returning from %d,%d,%d; All map elements checked", counter, loc.x >> 5, loc.y >> 5, loc.z);
        }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
    }
}

/**
 * Returns:
 *   -1   - no direction chosen
 *   0..3 - chosen direction
 *
 *  rct2: 0x0069A5F0
 */
Direction OriginalPathfinding::ChooseDirection(const TileCoordsXYZ& loc, Peep& peep)
{
    PROFILED_FUNCTION();

    // The max number of thin junctions searched - a per-search-path limit.
    _peepPathFindMaxJunctions = peep_pathfind_get_max_number_junctions(peep);

    /* The max number of tiles to check - a whole-search limit.
     * Mainly to limit the performance impact of the path finding. */
    int32_t maxTilesChecked = (peep.Is<Staff>()) ? 50000 : 15000;
    // Used to allow walking through no entry banners
    _peepPathFindIsStaff = peep.Is<Staff>();

    TileCoordsXYZ goal = gPeepPathFindGoalPosition;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    if (_pathFindDebug)
    {
        log_verbose(
            "Choose direction for %s for goal %d,%d,%d from %d,%d,%d", _pathFindDebugPeepName, goal.x, goal.y, goal.z, loc.x,
            loc.y, loc.z);
    }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    // Get the path element at this location
    TileElement* dest_tile_element = MapGetFirstElementAt(loc);
    /* Where there are multiple matching map elements placed with zero
     * clearance, save the first one for later use to determine the path
     * slope - this maintains the original behaviour (which only processes
     * the first matching map element found) and is consistent with peep
     * placement (i.e. height) on such paths with differing slopes.
     *
     * I cannot see a legitimate reason for building overlaid paths with
     * differing slopes and do not recall ever seeing this in practise.
     * Normal cases I have seen in practise are overlaid paths with the
     * same slope (flat) in order to place scenery (e.g. benches) in the
     * middle of a wide path that can still be walked through.
     * Anyone attempting to overlay paths with different slopes should
     * EXPECT to experience path finding irregularities due to those paths!
     * In particular common edges at different heights will not work
     * in a useful way. Simply do not do it! :-) */
    TileElement* first_tile_element = nullptr;

    bool found = false;
    uint8_t permitted_edges = 0;
    bool isThin = false;
    do
    {
        if (dest_tile_element == nullptr)
            break;
        if (dest_tile_element->base_height != loc.z)
            continue;
        if (dest_tile_element->GetType() != TileElementType::Path)
            continue;
        found = true;
        if (first_tile_element == nullptr)
        {
            first_tile_element = dest_tile_element;
        }

        /* Check if this path element is a thin junction.
         * Only 'thin' junctions are remembered in peep.PathfindHistory.
         * NO attempt is made to merge the overlaid path elements and
         * check if the combination is 'thin'!
         * The junction is considered 'thin' simply if any of the
         * overlaid path elements there is a 'thin junction'. */
        isThin = isThin || path_is_thin_junction(dest_tile_element->AsPath(), loc);

        // Collect the permitted edges of ALL matching path elements at this location.
        permitted_edges |= path_get_permitted_edges(dest_tile_element->AsPath());
    } while (!(dest_tile_element++)->IsLastForTile());
    // Peep is not on a path.
    if (!found)
        return INVALID_DIRECTION;

    permitted_edges &= 0xF;
    uint8_t edges = permitted_edges;
    if (isThin && peep.PathfindGoal == goal)
    {
        /* Use of peep.PathfindHistory[]:
         * When walking to a goal, the peep PathfindHistory stores
         * the last 4 thin junctions that the peep walked through.
         * For each of these 4 thin junctions the peep remembers
         * those edges it has not yet taken.
         * If a peep returns to one of the 4 thin junctions that it
         * remembers, it will only choose from the directions that it
         * did not try yet.
         * This forces to the peep pathfinding to try the "next best"
         * direction after trying the "best" direction(s) and finding
         * that the goal could not be reached. */

        /* If the peep remembers walking through this junction
         * previously while heading for its goal, retrieve the
         * directions it has not yet tried. */
        for (auto& pathfindHistory : peep.PathfindHistory)
        {
            if (pathfindHistory == loc)
            {
                /* Fix broken PathfindHistory[i].direction
                 * which have untried directions that are not
                 * currently possible - could be due to pathing
                 * changes or in earlier code .directions was
                 * initialised to 0xF rather than the permitted
                 * edges. */
                pathfindHistory.direction &= permitted_edges;

                edges = pathfindHistory.direction;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                if (_pathFindDebug)
                {
                    log_verbose(
                        "Getting untried edges from pf_history for %d,%d,%d:  %s,%s,%s,%s", loc.x, loc.y, loc.z,
                        (edges & 1) ? "0" : "-", (edges & 2) ? "1" : "-", (edges & 4) ? "2" : "-", (edges & 8) ? "3" : "-");
                }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

                if (edges == 0)
                {
                    /* If peep has tried all edges, reset to
                     * all edges are untried.
                     * This permits the pathfinding to try
                     * again, which is good for getting
                     * unstuck when the player has edited
                     * the paths or the pathfinding itself
                     * has changed (been fixed) since
                     * the game was saved. */
                    pathfindHistory.direction = permitted_edges;
                    edges = pathfindHistory.direction;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                    if (_pathFindDebug)
                    {
                        log_verbose("All edges tried for %d,%d,%d - resetting to all untried", loc.x, loc.y, loc.z);
                    }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                }
                break;
            }
        }
    }

    /* If this is a new goal for the peep. Store it and reset the peep's
     * PathfindHistory. */
    if (!DirectionValid(peep.PathfindGoal.direction) || peep.PathfindGoal != goal)
    {
        peep.PathfindGoal = { goal, 0 };

        // Clear pathfinding history
        TileCoordsXYZD nullPos;
        nullPos.SetNull();

        std::fill(std::begin(peep.PathfindHistory), std::end(peep.PathfindHistory), nullPos);
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (_pathFindDebug)
        {
            log_verbose("New goal; clearing pf_history.");
        }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    }

    // Peep has tried all edges.
    if (edges == 0)
        return INVALID_DIRECTION;

    int32_t chosen_edge = bitscanforward(edges);

    // Peep has multiple edges still to try.
    if (edges & ~(1 << chosen_edge))
    {
        uint16_t best_score = 0xFFFF;
        uint8_t best_sub = 0xFF;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        uint8_t bestJunctions = 0;
        TileCoordsXYZ bestJunctionList[16];
        uint8_t bestDirectionList[16];
        TileCoordsXYZ bestXYZ;

        if (_pathFindDebug)
        {
            log_verbose("Pathfind start for goal %d,%d,%d from %d,%d,%d", goal.x, goal.y, goal.z, loc.x, loc.y, loc.z);
        }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

        /* Call the search heuristic on each edge, keeping track of the
         * edge that gives the best (i.e. smallest) value (best_score)
         * or for different edges with equal value, the edge with the
         * least steps (best_sub). */
        int32_t numEdges = bitcount(edges);
        for (int32_t test_edge = chosen_edge; test_edge != -1; test_edge = bitscanforward(edges))
        {
            edges &= ~(1 << test_edge);
            uint8_t height = loc.z;

            if (first_tile_element->AsPath()->IsSloped() && first_tile_element->AsPath()->GetSlopeDirection() == test_edge)
            {
                height += 0x2;
            }

            _peepPathFindFewestNumSteps = 255;
            /* Divide the maxTilesChecked global search limit
             * between the remaining edges to ensure the search
             * covers all of the remaining edges. */
            _peepPathFindTilesChecked = maxTilesChecked / numEdges;
            _peepPathFindNumJunctions = _peepPathFindMaxJunctions;

            // Initialise _peepPathFindHistory.

            for (auto& entry : _peepPathFindHistory)
            {
                entry.location.SetNull();
                entry.direction = INVALID_DIRECTION;
            }

            /* The pathfinding will only use elements
             * 1.._peepPathFindMaxJunctions, so the starting point
             * is placed in element 0 */
            _peepPathFindHistory[0].location = loc;
            _peepPathFindHistory[0].direction = 0xF;

            uint16_t score = 0xFFFF;
            /* Variable endXYZ contains the end location of the
             * search path. */
            TileCoordsXYZ endXYZ;
            endXYZ.x = 0;
            endXYZ.y = 0;
            endXYZ.z = 0;

            uint8_t endSteps = 255;

            /* Variable endJunctions is the number of junctions
             * passed through in the search path.
             * Variables endJunctionList and endDirectionList
             * contain the junctions and corresponding directions
             * of the search path.
             * In the future these could be used to visualise the
             * pathfinding on the map. */
            uint8_t endJunctions = 0;
            TileCoordsXYZ endJunctionList[16];
            uint8_t endDirectionList[16] = { 0 };

            bool inPatrolArea = false;
            auto* staff = peep.As<Staff>();
            if (staff != nullptr && staff->IsMechanic())
            {
                /* Mechanics are the only staff type that
                 * pathfind to a destination. Determine if the
                 * mechanic is in their patrol area. */
                inPatrolArea = staff->IsLocationInPatrol(peep.NextLoc);
            }

#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_verbose("Pathfind searching in direction: %d from %d,%d,%d", test_edge, loc.x >> 5, loc.y >> 5, loc.z);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2

            peep_pathfind_heuristic_search(
                { loc.x, loc.y, height }, peep, first_tile_element, inPatrolArea, 0, &score, test_edge, &endJunctions,
                endJunctionList, endDirectionList, &endXYZ, &endSteps);

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
            if (_pathFindDebug)
            {
                log_verbose(
                    "Pathfind test edge: %d score: %d steps: %d end: %d,%d,%d junctions: %d", test_edge, score, endSteps,
                    endXYZ.x, endXYZ.y, endXYZ.z, endJunctions);
                for (uint8_t listIdx = 0; listIdx < endJunctions; listIdx++)
                {
                    log_info(
                        "Junction#%d %d,%d,%d Direction %d", listIdx + 1, endJunctionList[listIdx].x,
                        endJunctionList[listIdx].y, endJunctionList[listIdx].z, endDirectionList[listIdx]);
                }
            }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

            if (score < best_score || (score == best_score && endSteps < best_sub))
            {
                chosen_edge = test_edge;
                best_score = score;
                best_sub = endSteps;
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                bestJunctions = endJunctions;
                for (uint8_t index = 0; index < endJunctions; index++)
                {
                    bestJunctionList[index].x = endJunctionList[index].x;
                    bestJunctionList[index].y = endJunctionList[index].y;
                    bestJunctionList[index].z = endJunctionList[index].z;
                    bestDirectionList[index] = endDirectionList[index];
                }
                bestXYZ.x = endXYZ.x;
                bestXYZ.y = endXYZ.y;
                bestXYZ.z = endXYZ.z;
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
            }
        }

        /* Check if the heuristic search failed. e.g. all connected
         * paths are within the search limits and none reaches the
         * goal. */
        if (best_score == 0xFFFF)
        {
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
            if (_pathFindDebug)
            {
                log_verbose("Pathfind heuristic search failed.");
            }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
            return INVALID_DIRECTION;
        }
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (_pathFindDebug)
        {
            log_verbose("Pathfind best edge %d with score %d steps %d", chosen_edge, best_score, best_sub);
            for (uint8_t listIdx = 0; listIdx < bestJunctions; listIdx++)
            {
                log_verbose(
                    "Junction#%d %d,%d,%d Direction %d", listIdx + 1, bestJunctionList[listIdx].x, bestJunctionList[listIdx].y,
                    bestJunctionList[listIdx].z, bestDirectionList[listIdx]);
            }
            log_verbose("End at %d,%d,%d", bestXYZ.x, bestXYZ.y, bestXYZ.z);
        }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    }

    if (isThin)
    {
        for (int32_t i = 0; i < 4; ++i)
        {
            if (peep.PathfindHistory[i] == loc)
            {
                /* Peep remembers this junction, so remove the
                 * chosen_edge from those left to try. */
                peep.PathfindHistory[i].direction &= ~(1 << chosen_edge);
                /* Also remove the edge through which the peep
                 * entered the junction from those left to try. */
                peep.PathfindHistory[i].direction &= ~(1 << DirectionReverse(peep.PeepDirection));
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                if (_pathFindDebug)
                {
                    log_verbose(
                        "Updating existing pf_history (in index: %d) for %d,%d,%d without entry edge %d & exit edge %d.", i,
                        loc.x, loc.y, loc.z, DirectionReverse(peep.PeepDirection), chosen_edge);
                }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                return chosen_edge;
            }
        }

        /* Peep does not remember this junction, so forget a junction
         * and remember this junction. */
        int32_t i = peep.PathfindGoal.direction++;
        peep.PathfindGoal.direction &= 3;
        peep.PathfindHistory[i] = { loc, permitted_edges };
        /* Remove the chosen_edge from those left to try. */
        peep.PathfindHistory[i].direction &= ~(1 << chosen_edge);
        /* Also remove the edge through which the peep
         * entered the junction from those left to try. */
        peep.PathfindHistory[i].direction &= ~(1 << DirectionReverse(peep.PeepDirection));
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (_pathFindDebug)
        {
            log_verbose(
                "Storing new pf_history (in index: %d) for %d,%d,%d without entry edge %d & exit edge %d.", i, loc.x, loc.y,
                loc.z, DirectionReverse(peep.PeepDirection), chosen_edge);
        }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    }

    return chosen_edge;
}

/**
 * Gets the nearest park entrance relative to point, by using Manhattan distance.
 * @param x x coordinate of location
 * @param y y coordinate of location
 * @return Index of gParkEntrance (or 0xFF if no park entrances exist).
 */
static std::optional<CoordsXYZ> GetNearestParkEntrance(const CoordsXY& loc)
{
    std::optional<CoordsXYZ> chosenEntrance = std::nullopt;
    uint16_t nearestDist = 0xFFFF;
    for (const auto& parkEntrance : gParkEntrances)
    {
        auto dist = abs(parkEntrance.x - loc.x) + abs(parkEntrance.y - loc.y);
        if (dist < nearestDist)
        {
            nearestDist = dist;
            chosenEntrance = parkEntrance;
        }
    }
    return chosenEntrance;
}

/**
 *
 *  rct2: 0x006952C0
 */
int32_t OriginalPathfinding::GuestPathFindParkEntranceEntering(Peep& peep, uint8_t edges)
{
    // Send peeps to the nearest park entrance.
    auto chosenEntrance = GetNearestParkEntrance(peep.NextLoc);

    // If no defined park entrances are found, walk aimlessly.
    if (!chosenEntrance.has_value())
        return guest_path_find_aimless(peep, edges);

    gPeepPathFindGoalPosition = TileCoordsXYZ(chosenEntrance.value());
    gPeepPathFindIgnoreForeignQueues = true;
    gPeepPathFindQueueRideIndex = RideId::GetNull();

    Direction chosenDirection = ChooseDirection(TileCoordsXYZ{ peep.NextLoc }, peep);

    if (chosenDirection == INVALID_DIRECTION)
        return guest_path_find_aimless(peep, edges);

    return peep_move_one_tile(chosenDirection, peep);
}

/**
 * Gets the nearest peep spawn relative to point, by using Manhattan distance.
 * @param x x coordinate of location
 * @param y y coordinate of location
 * @return Index of gPeepSpawns (or 0xFF if no peep spawns exist).
 */
static uint8_t get_nearest_peep_spawn_index(uint16_t x, uint16_t y)
{
    uint8_t chosenSpawn = 0xFF;
    uint16_t nearestDist = 0xFFFF;
    uint8_t i = 0;
    for (const auto& spawn : gPeepSpawns)
    {
        uint16_t dist = abs(spawn.x - x) + abs(spawn.y - y);
        if (dist < nearestDist)
        {
            nearestDist = dist;
            chosenSpawn = i;
        }
        i++;
    }
    return chosenSpawn;
}

/**
 *
 *  rct2: 0x0069536C
 */
int32_t OriginalPathfinding::GuestPathFindPeepSpawn(Peep& peep, uint8_t edges)
{
    // Send peeps to the nearest spawn point.
    uint8_t chosenSpawn = get_nearest_peep_spawn_index(peep.NextLoc.x, peep.NextLoc.y);

    // If no defined spawns were found, walk aimlessly.
    if (chosenSpawn == 0xFF)
        return guest_path_find_aimless(peep, edges);

    const auto peepSpawnLoc = gPeepSpawns[chosenSpawn].ToTileStart();
    Direction direction = peepSpawnLoc.direction;

    gPeepPathFindGoalPosition = TileCoordsXYZ(peepSpawnLoc);
    if (peepSpawnLoc.x == peep.NextLoc.x && peepSpawnLoc.y == peep.NextLoc.y)
    {
        return peep_move_one_tile(direction, peep);
    }

    gPeepPathFindIgnoreForeignQueues = true;
    gPeepPathFindQueueRideIndex = RideId::GetNull();
    direction = ChooseDirection(TileCoordsXYZ{ peep.NextLoc }, peep);
    if (direction == INVALID_DIRECTION)
        return guest_path_find_aimless(peep, edges);

    return peep_move_one_tile(direction, peep);
}

/**
 *
 *  rct2: 0x00695161
 */
int32_t OriginalPathfinding::GuestPathFindParkEntranceLeaving(Peep& peep, uint8_t edges)
{
    TileCoordsXYZ entranceGoal{};
    if (peep.PeepFlags & PEEP_FLAGS_PARK_ENTRANCE_CHOSEN)
    {
        entranceGoal = peep.PathfindGoal;
        auto* entranceElement = MapGetParkEntranceElementAt(entranceGoal.ToCoordsXYZ(), false);
        // If entrance no longer exists, choose a new one
        if (entranceElement == nullptr)
        {
            peep.PeepFlags &= ~(PEEP_FLAGS_PARK_ENTRANCE_CHOSEN);
        }
    }

    if (!(peep.PeepFlags & PEEP_FLAGS_PARK_ENTRANCE_CHOSEN))
    {
        auto chosenEntrance = GetNearestParkEntrance(peep.NextLoc);

        if (!chosenEntrance.has_value())
            return guest_path_find_aimless(peep, edges);

        peep.PeepFlags |= PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
        entranceGoal = TileCoordsXYZ(*chosenEntrance);
    }

    gPeepPathFindGoalPosition = entranceGoal;
    gPeepPathFindIgnoreForeignQueues = true;
    gPeepPathFindQueueRideIndex = RideId::GetNull();

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    PathfindLoggingEnable(peep);
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    Direction chosenDirection = ChooseDirection(TileCoordsXYZ{ peep.NextLoc }, peep);

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    PathfindLoggingDisable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    if (chosenDirection == INVALID_DIRECTION)
        return guest_path_find_aimless(peep, edges);

    return peep_move_one_tile(chosenDirection, peep);
}

/**
 *
 *  rct2: 0x006A72C5
 *  param dist is not used.
 *
 * In case where the map element at (x, y) is invalid or there is no entrance
 * or queue leading to it the function will not update its arguments.
 */
static void get_ride_queue_end(TileCoordsXYZ& loc)
{
    TileCoordsXY queueEnd = { 0, 0 };
    TileElement* tileElement = MapGetFirstElementAt(loc);

    if (tileElement == nullptr)
    {
        return;
    }

    bool found = false;
    do
    {
        if (tileElement->GetType() != TileElementType::Entrance)
            continue;

        if (loc.z != tileElement->base_height)
            continue;

        found = true;
        break;
    } while (!(tileElement++)->IsLastForTile());

    if (!found)
        return;

    Direction direction = DirectionReverse(tileElement->GetDirection());
    TileElement* lastPathElement = nullptr;
    TileElement* firstPathElement = nullptr;

    int16_t baseZ = tileElement->base_height;
    TileCoordsXY nextTile = { loc.x, loc.y };

    while (true)
    {
        if (tileElement->GetType() == TileElementType::Path)
        {
            lastPathElement = tileElement;
            // Update the current queue end
            queueEnd = nextTile;
            // queueEnd.direction = direction;
            if (tileElement->AsPath()->IsSloped())
            {
                if (tileElement->AsPath()->GetSlopeDirection() == direction)
                {
                    baseZ += 2;
                }
            }
        }
        nextTile += TileDirectionDelta[direction];

        tileElement = MapGetFirstElementAt(nextTile);
        found = false;
        if (tileElement == nullptr)
            break;
        do
        {
            if (tileElement == firstPathElement)
                continue;

            if (tileElement->GetType() != TileElementType::Path)
                continue;

            if (baseZ == tileElement->base_height)
            {
                if (tileElement->AsPath()->IsSloped())
                {
                    if (tileElement->AsPath()->GetSlopeDirection() != direction)
                    {
                        break;
                    }
                }
                found = true;
                break;
            }

            if (baseZ - 2 == tileElement->base_height)
            {
                if (!tileElement->AsPath()->IsSloped())
                    break;

                if (tileElement->AsPath()->GetSlopeDirection() != DirectionReverse(direction))
                    break;

                baseZ -= 2;
                found = true;
                break;
            }
        } while (!(tileElement++)->IsLastForTile());

        if (!found)
            break;

        if (!tileElement->AsPath()->IsQueue())
            break;

        if (!(tileElement->AsPath()->GetEdges() & (1 << DirectionReverse(direction))))
            break;

        if (firstPathElement == nullptr)
            firstPathElement = tileElement;

        // More queue to go.
        if (tileElement->AsPath()->GetEdges() & (1 << (direction)))
            continue;

        direction++;
        direction &= 3;
        // More queue to go.
        if (tileElement->AsPath()->GetEdges() & (1 << (direction)))
            continue;

        direction = DirectionReverse(direction);
        // More queue to go.
        if (tileElement->AsPath()->GetEdges() & (1 << (direction)))
            continue;

        break;
    }

    if (loc.z == MAX_ELEMENT_HEIGHT)
        return;

    tileElement = lastPathElement;
    if (tileElement == nullptr)
        return;

    if (!tileElement->AsPath()->IsQueue())
        return;

    loc.x = queueEnd.x;
    loc.y = queueEnd.y;
    loc.z = tileElement->base_height;
}

/*
 * If a ride has multiple entrance stations and is set to sync with
 * adjacent stations, cycle through the entrance stations (based on
 * number of rides the peep has been on) so the peep will try the
 * different sections of the ride.
 * In this case, the ride's various entrance stations will typically,
 * though not necessarily, be adjacent to one another and consequently
 * not too far for the peep to walk when cycling between them.
 * Note: the same choice of station must made while the peep navigates
 * to the station. Consequently a truly random station selection here is not
 * appropriate.
 */
static StationIndex guest_pathfinding_select_random_station(
    const Guest& guest, int32_t numEntranceStations, BitSet<OpenRCT2::Limits::MaxStationsPerRide>& entranceStations)
{
    int32_t select = guest.GuestNumRides % numEntranceStations;
    while (select > 0)
    {
        for (StationIndex::UnderlyingType i = 0; i < OpenRCT2::Limits::MaxStationsPerRide; i++)
        {
            if (entranceStations[i])
            {
                entranceStations[i] = false;
                select--;
                break;
            }
        }
    }
    for (StationIndex::UnderlyingType i = 0; i < OpenRCT2::Limits::MaxStationsPerRide; i++)
    {
        if (entranceStations[i])
        {
            return StationIndex::FromUnderlying(i);
        }
    }

    return StationIndex::FromUnderlying(0);
}
/**
 *
 *  rct2: 0x00694C35
 */
int32_t OriginalPathfinding::CalculateNextDestination(Guest& peep)
{
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    PathfindLoggingEnable(peep);
    if (_pathFindDebug)
    {
        log_info("Starting CalculateNextDestination for %s", _pathFindDebugPeepName);
    }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    if (peep.GetNextIsSurface())
    {
        return guest_surface_path_finding(peep);
    }

    TileCoordsXYZ loc{ peep.NextLoc };

    auto* pathElement = MapGetPathElementAt(loc);
    if (pathElement == nullptr)
    {
        return 1;
    }

    _peepPathFindIsStaff = false;
    uint8_t edges = path_get_permitted_edges(pathElement);

    if (edges == 0)
    {
        return guest_surface_path_finding(peep);
    }

    if (!peep.OutsideOfPark && peep.HeadingForRideOrParkExit())
    {
        /* If this tileElement is adjacent to any non-wide paths,
         * remove all of the edges to wide paths. */
        uint8_t adjustedEdges = edges;
        for (Direction chosenDirection : ALL_DIRECTIONS)
        {
            // If there is no path in that direction try another
            if (!(adjustedEdges & (1 << chosenDirection)))
                continue;

            /* If there is a wide path in that direction,
                remove that edge and try another */
            if (footpath_element_next_in_direction(loc, pathElement, chosenDirection) == PATH_SEARCH_WIDE)
            {
                adjustedEdges &= ~(1 << chosenDirection);
            }
        }
        if (adjustedEdges != 0)
            edges = adjustedEdges;
    }

    int32_t direction = DirectionReverse(peep.PeepDirection);
    // Check if in a dead end (i.e. only edge is where the peep came from)
    if (!(edges & ~(1 << direction)))
    {
        // In a dead end.  Check if peep is lost, etc.
        peep.CheckIfLost();
        peep.CheckCantFindRide();
        peep.CheckCantFindExit();
    }
    else
    {
        /* Not a dead end. Remove edge peep came from so peep will
         * continue on rather than going back where it came from */
        edges &= ~(1 << direction);
    }

    direction = bitscanforward(edges);
    // IF only one edge to choose from
    if ((edges & ~(1 << direction)) == 0)
    {
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (_pathFindDebug)
        {
            log_info(
                "Completed CalculateNextDestination for %s - taking only direction available: %d.", _pathFindDebugPeepName,
                direction);
        }
        PathfindLoggingDisable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        return peep_move_one_tile(direction, peep);
    }

    // Peep still has multiple edges to choose from.

    // Peep is outside the park.
    // loc_694F19:
    if (peep.OutsideOfPark)
    {
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (_pathFindDebug)
        {
            log_info("Completed CalculateNextDestination for %s - peep is outside the park.", _pathFindDebugPeepName);
        }
        PathfindLoggingDisable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        switch (peep.State)
        {
            case PeepState::EnteringPark:
                return GuestPathFindParkEntranceEntering(peep, edges);
            case PeepState::LeavingPark:
                return GuestPathFindPeepSpawn(peep, edges);
            default:
                return guest_path_find_aimless(peep, edges);
        }
    }

    /* Peep is inside the park.
     * If the peep does not have food, randomly cull the useless directions
     * (dead ends, ride exits, wide paths) from the edges.
     * In principle, peeps with food are not paying as much attention to
     * where they are going and are consequently more like to walk up
     * dead end paths, paths to ride exits, etc. */
    if (!peep.HasFoodOrDrink() && (scenario_rand() & 0xFFFF) >= 2184)
    {
        uint8_t adjustedEdges = edges;
        for (Direction chosenDirection : ALL_DIRECTIONS)
        {
            // If there is no path in that direction try another
            if (!(adjustedEdges & (1 << chosenDirection)))
                continue;

            RideId rideIndex = RideId::GetNull();
            auto pathSearchResult = footpath_element_destination_in_direction(loc, pathElement, chosenDirection, &rideIndex);
            switch (pathSearchResult)
            {
                case PATH_SEARCH_DEAD_END:
                case PATH_SEARCH_RIDE_EXIT:
                case PATH_SEARCH_WIDE:
                    adjustedEdges &= ~(1 << chosenDirection);
                    break;
            }
        }
        if (adjustedEdges != 0)
            edges = adjustedEdges;
    }

    /* If there are still multiple directions to choose from,
     * peeps with maps will randomly read the map: probability of doing so
     * is much higher when heading for a ride or the park exit. */
    if (peep.HasItem(ShopItem::Map))
    {
        // If at least 2 directions consult map
        if (bitcount(edges) >= 2)
        {
            uint16_t probability = 1638;
            if (peep.HeadingForRideOrParkExit())
            {
                probability = 9362;
            }
            if ((scenario_rand() & 0xFFFF) < probability)
            {
                peep.ReadMap();
            }
        }
    }

    if (peep.PeepFlags & PEEP_FLAGS_LEAVING_PARK)
    {
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (_pathFindDebug)
        {
            log_info("Completed CalculateNextDestination for %s - peep is leaving the park.", _pathFindDebugPeepName);
        }
        PathfindLoggingDisable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        return GuestPathFindParkEntranceLeaving(peep, edges);
    }

    if (peep.GuestHeadingToRideId.IsNull())
    {
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (_pathFindDebug)
        {
            log_info("Completed CalculateNextDestination for %s - peep is aimless.", _pathFindDebugPeepName);
        }
        PathfindLoggingDisable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        return guest_path_find_aimless(peep, edges);
    }

    // Peep is heading for a ride.
    RideId rideIndex = peep.GuestHeadingToRideId;
    auto ride = get_ride(rideIndex);
    if (ride == nullptr || ride->status != RideStatus::Open)
    {
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (_pathFindDebug)
        {
            log_info(
                "Completed CalculateNextDestination for %s - peep is heading to closed ride == aimless.",
                _pathFindDebugPeepName);
        }
        PathfindLoggingDisable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        return guest_path_find_aimless(peep, edges);
    }

    // The ride is open.
    gPeepPathFindQueueRideIndex = rideIndex;

    /* Find the ride's closest entrance station to the peep.
     * At the same time, count how many entrance stations there are and
     * which stations are entrance stations. */
    auto bestScore = std::numeric_limits<int32_t>::max();
    StationIndex closestStationNum = StationIndex::FromUnderlying(0);

    int32_t numEntranceStations = 0;
    BitSet<OpenRCT2::Limits::MaxStationsPerRide> entranceStations = {};

    for (const auto& station : ride->GetStations())
    {
        // Skip if stationNum has no entrance (so presumably an exit only station)
        if (station.Entrance.IsNull())
            continue;

        const auto stationIndex = ride->GetStationIndex(&station);

        numEntranceStations++;
        entranceStations[stationIndex.ToUnderlying()] = true;

        TileCoordsXYZD entranceLocation = station.Entrance;
        auto score = CalculateHeuristicPathingScore(entranceLocation, TileCoordsXYZ{ peep.NextLoc });
        if (score < bestScore)
        {
            bestScore = score;
            closestStationNum = stationIndex;
            continue;
        }
    }

    // Ride has no stations with an entrance, so head to station 0.
    if (numEntranceStations == 0)
        closestStationNum = StationIndex::FromUnderlying(0);

    if (numEntranceStations > 1 && (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS))
    {
        closestStationNum = guest_pathfinding_select_random_station(peep, numEntranceStations, entranceStations);
    }

    if (numEntranceStations == 0)
    {
        // closestStationNum is always 0 here.
        const auto& closestStation = ride->GetStation(closestStationNum);
        auto entranceXY = TileCoordsXY(closestStation.Start);
        loc.x = entranceXY.x;
        loc.y = entranceXY.y;
        loc.z = closestStation.Height;
    }
    else
    {
        TileCoordsXYZD entranceXYZD = ride->GetStation(closestStationNum).Entrance;
        loc.x = entranceXYZD.x;
        loc.y = entranceXYZD.y;
        loc.z = entranceXYZD.z;
    }

    get_ride_queue_end(loc);

    gPeepPathFindGoalPosition = loc;
    gPeepPathFindIgnoreForeignQueues = true;

    direction = ChooseDirection(TileCoordsXYZ{ peep.NextLoc }, peep);

    if (direction == INVALID_DIRECTION)
    {
        /* Heuristic search failed for all directions.
         * Reset the PathfindGoal - this means that the PathfindHistory
         * will be reset in the next call to ChooseDirection().
         * This lets the heuristic search "try again" in case the player has
         * edited the path layout or the mechanic was already stuck in the
         * save game (e.g. with a worse version of the pathfinding). */
        peep.ResetPathfindGoal();

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (_pathFindDebug)
        {
            log_info(
                "Completed CalculateNextDestination for %s - failed to choose a direction == aimless.", _pathFindDebugPeepName);
        }
        PathfindLoggingDisable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

        return guest_path_find_aimless(peep, edges);
    }
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    if (_pathFindDebug)
    {
        log_info("Completed CalculateNextDestination for %s - direction chosen: %d.", _pathFindDebugPeepName, direction);
    }
    PathfindLoggingDisable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    return peep_move_one_tile(direction, peep);
}

bool GuestPathfinding::IsValidPathZAndDirection(TileElement* tileElement, int32_t currentZ, int32_t currentDirection)
{
    if (tileElement->AsPath()->IsSloped())
    {
        int32_t slopeDirection = tileElement->AsPath()->GetSlopeDirection();
        if (slopeDirection == currentDirection)
        {
            if (currentZ != tileElement->base_height)
                return false;
        }
        else
        {
            slopeDirection = DirectionReverse(slopeDirection);
            if (slopeDirection != currentDirection)
                return false;
            if (currentZ != tileElement->base_height + 2)
                return false;
        }
    }
    else
    {
        if (currentZ != tileElement->base_height)
            return false;
    }
    return true;
}

/**
 *
 *  rct2: 0x0069A98C
 */
void Peep::ResetPathfindGoal()
{
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    if (_pathFindDebug)
    {
        log_info("Resetting PathfindGoal for %s", _pathFindDebugPeepName);
    }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    PathfindGoal.SetNull();
    PathfindGoal.direction = INVALID_DIRECTION;
}

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
void PathfindLoggingEnable([[maybe_unused]] Peep& peep)
{
#    if defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
    /* Determine if the pathfinding debugging is wanted for this peep. */
    format_string(gPathFindDebugPeepName, sizeof(gPathFindDebugPeepName), peep.name_string_idx, &(peep.Id));

    /* For guests, use the existing PEEP_FLAGS_TRACKING flag to
     * determine for which guest(s) the pathfinding debugging will
     * be output for. */
    if (peep.type == PEEP_TYPE_GUEST)
    {
        gPathFindDebug = peep.PeepFlags & PEEP_FLAGS_TRACKING;
    }
    /* For staff, there is no tracking button (any other similar
     * suitable existing mechanism?), so fall back to a crude
     * string comparison with a compile time hardcoded name. */
    else
    {
        gPathFindDebug = strcmp(gPathFindDebugPeepName, "Mechanic Debug") == 0;
    }
#    endif // defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
}

void PathfindLoggingDisable()
{
#    if defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
    gPathFindDebug = false;
#    endif // defined(PATHFIND_DEBUG) && PATHFIND_DEBUG
}
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
