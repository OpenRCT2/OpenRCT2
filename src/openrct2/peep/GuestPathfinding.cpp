/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GuestPathfinding.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/Guard.hpp"
#include "../entity/Guest.h"
#include "../entity/Staff.h"
#include "../profiling/Profiling.h"
#include "../ride/RideData.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/tile_element/BannerElement.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"

#include <bit>
#include <bitset>
#include <cassert>
#include <cstring>

namespace OpenRCT2::PathFinding
{
    // The search limits the maximum junctions by certain conditions.
    static constexpr uint8_t kMaxJunctionsStaff = 8;
    static constexpr uint8_t kMaxJunctionsGuest = 5;
    static constexpr uint8_t kMaxJunctionsGuestWithMap = 7;
    static constexpr uint8_t kMaxJunctionsGuestLeavingPark = 7;
    static constexpr uint8_t kMaxJunctionsGuestLeavingParkLost = 8;

    // Maximum amount of junctions.
    static constexpr uint8_t kMaxJunctions = std::max({ kMaxJunctionsStaff, kMaxJunctionsGuest, kMaxJunctionsGuestWithMap,
                                                        kMaxJunctionsGuestLeavingPark, kMaxJunctionsGuestLeavingParkLost });

    struct PathFindingState
    {
        int8_t junctionCount;
        int8_t maxJunctions;
        int32_t countTilesChecked;
        // TODO: Move them, those are query parameters not really state, but for now its easier to pass it down.
        bool ignoreForeignQueues;
        RideId queueRideIndex;
        // A junction history for the peep path finding heuristic search.
        struct
        {
            TileCoordsXYZ location;
            Direction direction;
        } history[kMaxJunctions + 1];
    };

    static int32_t GuestSurfacePathFinding(Peep& peep);

    enum class PathSearchResult
    {
        DeadEnd,      // Path is a dead end, i.e. < 2 edges.
        Wide,         // Path with wide flag set.
        Thin,         // Path is simple.
        Junction,     // Path is a junction, i.e. > 2 edges.
        RideQueue,    // Queue path connected to a ride.
        RideEntrance, // Map element is a ride entrance.
        RideExit,     // Map element is a ride exit.
        ParkExit,     // Park entrance / exit (map element is a park entrance/exit).
        ShopEntrance, // Map element is a shop entrance.
        Other,        // Path is other than the above.
        Loop,         // Loop detected.
        LimitReached, // Search limit reached without reaching path end.
        Failed,       // No path element found.
    };

#pragma region Pathfinding Logging
    // In case this is set to true it will enable code paths that log path finding. The peep will additionally
    // require to have PEEP_FLAGS_DEBUG_PATHFINDING set in PeepFlags in order to activate logging.
    static constexpr bool kLogPathfinding = false;

    template<typename... TArgs>
    static void LogPathfinding([[maybe_unused]] const Peep* peep, [[maybe_unused]] const char* format, TArgs&&... args)
    {
        if constexpr (kLogPathfinding)
        {
            if ((peep->PeepFlags & PEEP_FLAGS_DEBUG_PATHFINDING) == 0)
                return;

            char buffer[256];
            snprintf(buffer, sizeof(buffer), format, std::forward<TArgs>(args)...);

            if (peep != nullptr)
            {
                LOG_INFO("[%05u:%s] %s", peep->Id.ToUnderlying(), peep->GetName().c_str(), buffer);
            }
            else
            {
                LOG_INFO("%s", buffer);
            }
        }
    }

    static constexpr const char* PathSearchToString(PathSearchResult pathFindSearchResult)
    {
        switch (pathFindSearchResult)
        {
            case PathSearchResult::DeadEnd:
                return "DeadEnd";
            case PathSearchResult::Wide:
                return "Wide";
            case PathSearchResult::Thin:
                return "Thin";
            case PathSearchResult::Junction:
                return "Junction";
            case PathSearchResult::RideQueue:
                return "RideQueue";
            case PathSearchResult::RideEntrance:
                return "RideEntrance";
            case PathSearchResult::RideExit:
                return "RideExit";
            case PathSearchResult::ParkExit:
                return "ParkEntryExit";
            case PathSearchResult::ShopEntrance:
                return "ShopEntrance";
            case PathSearchResult::LimitReached:
                return "LimitReached";
            case PathSearchResult::Other:
                return "Other";
            case PathSearchResult::Loop:
                return "Loop";
            case PathSearchResult::Failed:
                return "Failed";
                // The default case is omitted intentionally.
        }

        return "Unknown";
    }
#pragma endregion

    static const TileElement* GetBannerOnPath(const TileElement* pathElement)
    {
        // This is an improved version of original.
        // That only checked for one fence in the way.
        if (pathElement->IsLastForTile())
            return nullptr;

        const TileElement* bannerElement = pathElement + 1;
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

    static int32_t BannerClearPathEdges(bool ignoreBanners, const PathElement* pathElement, int32_t edges)
    {
        if (ignoreBanners)
            return edges;
        const TileElement* bannerElement = GetBannerOnPath(reinterpret_cast<const TileElement*>(pathElement));
        if (bannerElement != nullptr)
        {
            do
            {
                edges &= bannerElement->AsBanner()->GetAllowedEdges();
            } while ((bannerElement = GetBannerOnPath(bannerElement)) != nullptr);
        }
        return edges;
    }

    /**
     * Gets the connected edges of a path that are permitted (i.e. no 'no entry' signs)
     */
    static int32_t PathGetPermittedEdges(bool ignoreBanners, const PathElement* pathElement)
    {
        return BannerClearPathEdges(ignoreBanners, pathElement, pathElement->GetEdgesAndCorners()) & 0x0F;
    }

    /**
     *
     *  rct2: 0x0069524E
     */
    static int32_t PeepMoveOneTile(Direction direction, Peep& peep)
    {
        assert(DirectionValid(direction));
        auto newTile = CoordsXY{ CoordsXY{ peep.NextLoc } + CoordsDirectionDelta[direction] }.ToTileCentre();

        if (newTile.x >= kMaximumMapSizeBig || newTile.y >= kMaximumMapSizeBig)
        {
            // This could loop!
            return GuestSurfacePathFinding(peep);
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

            const int8_t offset = (ScenarioRand() & 7) - 3;
            if (direction == 0 || direction == 2)
            {
                // Peep is moving along X, so apply the offset to the X position of the destination and clamp their current Y
                const int32_t centreLine = (peep.y & 0xFFE0) + kCoordsXYHalfTile;
                newTile.x += offset;
                newTile.y = std::clamp<int32_t>(peep.y, centreLine - 3, centreLine + 3);
            }
            else
            {
                // Peep is moving along Y, so apply the offset to the Y position of the destination and clamp their current X
                const int32_t centreLine = (peep.x & 0xFFE0) + kCoordsXYHalfTile;
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
    static int32_t GuestSurfacePathFinding(Peep& peep)
    {
        auto pathPos = CoordsXYRangedZ{ peep.NextLoc, peep.NextLoc.z, peep.NextLoc.z + kPathClearance };
        Direction randDirection = ScenarioRand() & 3;

        if (!WallInTheWay(pathPos, randDirection))
        {
            pathPos.x += CoordsDirectionDelta[randDirection].x;
            pathPos.y += CoordsDirectionDelta[randDirection].y;
            Direction backwardsDirection = DirectionReverse(randDirection);

            if (!WallInTheWay(pathPos, backwardsDirection))
            {
                if (!MapSurfaceIsBlocked(pathPos))
                {
                    return PeepMoveOneTile(randDirection, peep);
                }
            }
        }

        randDirection++;
        uint8_t rand_backwards = ScenarioRand() & 1;
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
                    return PeepMoveOneTile(randDirection, peep);
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
                    return PeepMoveOneTile(randDirection, peep);
                }
            }
        }

        randDirection--;
        if (rand_backwards)
        {
            randDirection += 2;
        }
        randDirection &= 3;
        return PeepMoveOneTile(randDirection, peep);
    }

    /**
     *
     * Returns:
     *   PathSearchResult::Wide - path with wide flag set
     *   PathSearchResult::RideQueue - queue path connected to a ride
     *   PathSearchResult::Other - other path than the above
     *   PathSearchResult::Failed - no path element found
     *
     *  rct2: 0x00694BAE
     *
     * Returns the type of the next footpath tile a peep can get to from x,y,z /
     * inputTileElement in the given direction.
     */
    static PathSearchResult FootpathElementNextInDirection(
        TileCoordsXYZ loc, PathElement* pathElement, Direction chosenDirection)
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
            const auto* nextPathElement = nextTileElement->AsPath();
            if (!FootpathIsZAndDirectionValid(*nextPathElement, loc.z, chosenDirection))
                continue;
            if (nextPathElement->IsWide())
                return PathSearchResult::Wide;
            // Only queue tiles that are connected to a ride are returned as ride queues.
            if (nextPathElement->IsQueue() && !nextPathElement->GetRideIndex().IsNull())
                return PathSearchResult::RideQueue;

            return PathSearchResult::Other;
        } while (!(nextTileElement++)->IsLastForTile());

        return PathSearchResult::Failed;
    }

    /**
     *
     * Returns:
     *   PathSearchResult::DeadEnd - path is a dead end, i.e. < 2 edges
     *   PathSearchResult::Wide - path with wide flag set
     *   PathSearchResult::Junction - path is a junction, i.e. > 2 edges
     *   PathSearchResult::RideEntrance - map element is a ride entrance
     *   PathSearchResult::RideExit - map element is a ride exit
     *   PathSearchResult::ParkExit - park entrance / exit (map element is a park entrance/exit)
     *   PathSearchResult::ShopEntrance - map element is a shop entrance
     *   PathSearchResult::LimitReached - search limit reached without reaching path end
     *   PathSearchResult::Failed - no path element found
     * For return values RideEntrance, RideExit & ShopEntrance the rideIndex is stored in outRideIndex.
     *
     *  rct2: 0x006949B9
     *
     * This is the recursive portion of FootpathElementDestinationInDirection().
     */
    static PathSearchResult FootpathElementDestInDir(
        bool ignoreBanners, TileCoordsXYZ loc, Direction chosenDirection, RideId* outRideIndex, int32_t level)
    {
        TileElement* tileElement;
        Direction direction;

        if (level > 25)
            return PathSearchResult::LimitReached;

        loc += TileDirectionDelta[chosenDirection];
        tileElement = MapGetFirstElementAt(loc);
        if (tileElement == nullptr)
        {
            return PathSearchResult::Failed;
        }
        do
        {
            if (tileElement->IsGhost())
                continue;

            switch (tileElement->GetType())
            {
                case TileElementType::Track:
                {
                    if (loc.z != tileElement->BaseHeight)
                        continue;
                    RideId rideIndex = tileElement->AsTrack()->GetRideIndex();
                    auto ride = GetRide(rideIndex);
                    if (ride != nullptr && ride->GetRideTypeDescriptor().HasFlag(RtdFlag::isShopOrFacility))
                    {
                        *outRideIndex = rideIndex;
                        return PathSearchResult::ShopEntrance;
                    }
                }
                break;
                case TileElementType::Entrance:
                    if (loc.z != tileElement->BaseHeight)
                        continue;
                    switch (tileElement->AsEntrance()->GetEntranceType())
                    {
                        case ENTRANCE_TYPE_RIDE_ENTRANCE:
                            direction = tileElement->GetDirection();
                            if (direction == chosenDirection)
                            {
                                *outRideIndex = tileElement->AsEntrance()->GetRideIndex();
                                return PathSearchResult::RideEntrance;
                            }
                            break;
                        case ENTRANCE_TYPE_RIDE_EXIT:
                            direction = tileElement->GetDirection();
                            if (direction == chosenDirection)
                            {
                                *outRideIndex = tileElement->AsEntrance()->GetRideIndex();
                                return PathSearchResult::RideExit;
                            }
                            break;
                        case ENTRANCE_TYPE_PARK_ENTRANCE:
                            return PathSearchResult::ParkExit;
                    }
                    break;
                case TileElementType::Path:
                {
                    const auto* pathElement = tileElement->AsPath();
                    if (!FootpathIsZAndDirectionValid(*pathElement, loc.z, chosenDirection))
                        continue;
                    if (tileElement->AsPath()->IsWide())
                        return PathSearchResult::Wide;

                    uint8_t edges = PathGetPermittedEdges(ignoreBanners, pathElement);
                    edges &= ~(1 << DirectionReverse(chosenDirection));
                    loc.z = tileElement->BaseHeight;

                    for (Direction dir : kAllDirections)
                    {
                        if (!(edges & (1 << dir)))
                            continue;

                        edges &= ~(1 << dir);
                        if (edges != 0)
                            return PathSearchResult::Junction;

                        if (tileElement->AsPath()->IsSloped())
                        {
                            if (tileElement->AsPath()->GetSlopeDirection() == dir)
                            {
                                loc.z += 2;
                            }
                        }
                        return FootpathElementDestInDir(ignoreBanners, loc, dir, outRideIndex, level + 1);
                    }
                    return PathSearchResult::DeadEnd;
                }
                default:
                    break;
            }
        } while (!(tileElement++)->IsLastForTile());

        return PathSearchResult::Failed;
    }

    /**
     * Returns:
     *   PathSearchResult::DeadEnd - path is a dead end, i.e. < 2 edges
     *   PathSearchResult::Wide - path with wide flag set
     *   PathSearchResult::Junction - path is a junction, i.e. > 2 edges
     *   PathSearchResult::RideEntrance - map element is a ride entrance
     *   PathSearchResult::RideExit - map element is a ride exit
     *   PathSearchResult::ParkExit - ark entrance / exit (map element is a park entrance/exit
     *   PathSearchResult::ShopEntrance - map element is a shop entrance
     *   PathSearchResult::LimitReached - search limit reached without reaching path end
     *   PathSearchResult::Failed - no path element found
     * For return values RideEntrance, RideExit & ShopEntrance the rideIndex is stored in outRideIndex.
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
    static PathSearchResult FootpathElementDestinationInDirection(
        TileCoordsXYZ loc, PathElement* pathElement, Direction chosenDirection, RideId* outRideIndex)
    {
        if (pathElement->IsSloped())
        {
            if (pathElement->GetSlopeDirection() == chosenDirection)
            {
                loc.z += 2;
            }
        }

        // This function is only called for guests, never ignore the banners.
        return FootpathElementDestInDir(false, loc, chosenDirection, outRideIndex, 0);
    }

    /**
     *
     *  rct2: 0x00695225
     */
    static int32_t GuestPathfindAimless(Peep& peep, uint8_t edges)
    {
        if (ScenarioRand() & 1)
        {
            // If possible go straight
            if (edges & (1 << peep.PeepDirection))
            {
                return PeepMoveOneTile(peep.PeepDirection, peep);
            }
        }

        while (true)
        {
            Direction direction = ScenarioRand() & 3;
            // Otherwise go in a random direction allowed from the tile.
            if (edges & (1 << direction))
            {
                return PeepMoveOneTile(direction, peep);
            }
        }
    }

    /**
     *
     *  rct2: 0x0069A60A
     */
    static uint8_t PeepPathfindGetMaxNumberJunctions(Peep& peep)
    {
        if (peep.Is<Staff>())
            return kMaxJunctionsStaff;

        auto* guest = peep.As<Guest>();
        if (guest == nullptr)
            return kMaxJunctionsStaff;

        bool isLeavingPark = (guest->PeepFlags & PEEP_FLAGS_LEAVING_PARK) != 0;
        if (isLeavingPark && guest->GuestIsLostCountdown < 90)
        {
            return kMaxJunctionsGuestLeavingParkLost;
        }

        if (isLeavingPark)
            return kMaxJunctionsGuestLeavingPark;

        if (guest->HasItem(ShopItem::Map))
            return kMaxJunctionsGuestWithMap;

        return kMaxJunctionsGuest;
    }

    /**
     * Returns if the path as xzy is a 'thin' junction.
     * A junction is considered 'thin' if it has more than 2 edges
     * leading to/from non-wide path elements; edges leading to/from non-path
     * elements (e.g. ride/shop entrances) or ride queues are not counted,
     * since entrances and ride queues coming off a path should not result in
     * the path being considered a junction.
     */
    static bool PathIsThinJunction(PathElement* path, const TileCoordsXYZ& loc)
    {
        PROFILED_FUNCTION();

        uint32_t edges = path->GetEdges();

        int32_t testEdge = Numerics::bitScanForward(edges);
        if (testEdge == -1)
            return false;

        bool isThinJunction = false;
        int32_t thinCount = 0;
        do
        {
            auto nextFootpathResult = FootpathElementNextInDirection(loc, path, testEdge);

            /* Ignore non-paths (e.g. ride entrances, shops), wide paths
             * and ride queues (per ignoreQueues) when counting
             * neighbouring tiles. */
            if (nextFootpathResult != PathSearchResult::Failed && nextFootpathResult != PathSearchResult::Wide
                && nextFootpathResult != PathSearchResult::RideQueue)
            {
                thinCount++;
            }

            if (thinCount > 2)
            {
                isThinJunction = true;
                break;
            }
            edges &= ~(1 << testEdge);
        } while ((testEdge = Numerics::bitScanForward(edges)) != -1);
        return isThinJunction;
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
    static void PeepPathfindHeuristicSearch(
        PathFindingState& state, TileCoordsXYZ loc, const TileCoordsXYZ& goal, const Peep& peep,
        TileElement* currentTileElement, const bool inPatrolArea, uint8_t numSteps, uint16_t* endScore, Direction testEdge,
        uint8_t* endJunctions, TileCoordsXYZ junctionList[16], uint8_t directionList[16], TileCoordsXYZ* endXYZ,
        uint8_t* endSteps)
    {
        PathSearchResult searchResult = PathSearchResult::Failed;

        bool currentElementIsWide = currentTileElement->AsPath()->IsWide();
        if (currentElementIsWide)
        {
            const Staff* staff = peep.As<Staff>();
            if (staff != nullptr && staff->CanIgnoreWideFlag(loc.ToCoordsXYZ(), currentTileElement))
                currentElementIsWide = false;
        }

        loc += TileDirectionDelta[testEdge];

        ++numSteps;
        state.countTilesChecked--;

        /* If this is where the search started this is a search loop and the
         * current search path ends here.
         * Return without updating the parameters (best result so far). */
        if (state.history[0].location == loc)
        {
            LogPathfinding(&peep, "Return from %d,%d,%d; Steps: %u; At start", loc.x >> 5, loc.y >> 5, loc.z, numSteps);
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
                LogPathfinding(
                    &peep, "Return from %d,%d,%d; Steps: %u; Left patrol area", loc.x >> 5, loc.y >> 5, loc.z, numSteps);
                return;
            }
        }

        /* Get the next map element of interest in the direction of testEdge. */
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
                    if (loc.z != tileElement->BaseHeight)
                        continue;
                    /* For peeps heading for a shop, the goal is the shop
                     * tile. */
                    rideIndex = tileElement->AsTrack()->GetRideIndex();
                    auto ride = GetRide(rideIndex);
                    if (ride == nullptr || !ride->GetRideTypeDescriptor().HasFlag(RtdFlag::isShopOrFacility))
                        continue;

                    found = true;
                    searchResult = PathSearchResult::ShopEntrance;
                    break;
                }
                case TileElementType::Entrance:
                    if (loc.z != tileElement->BaseHeight)
                        continue;
                    Direction direction;
                    searchResult = PathSearchResult::Other;
                    switch (tileElement->AsEntrance()->GetEntranceType())
                    {
                        case ENTRANCE_TYPE_RIDE_ENTRANCE:
                            /* For peeps heading for a ride without a queue, the
                             * goal is the ride entrance tile.
                             * For mechanics heading for the ride entrance
                             * (in the case when the station has no exit),
                             * the goal is the ride entrance tile. */
                            direction = tileElement->GetDirection();
                            if (direction == testEdge)
                            {
                                /* The rideIndex will be useful for
                                 * adding transport rides later. */
                                rideIndex = tileElement->AsEntrance()->GetRideIndex();
                                searchResult = PathSearchResult::RideEntrance;
                                found = true;
                                break;
                            }
                            continue; // Ride entrance is not facing the right direction.
                        case ENTRANCE_TYPE_PARK_ENTRANCE:
                            /* For peeps leaving the park, the goal is the park
                             * entrance/exit tile. */
                            searchResult = PathSearchResult::ParkExit;
                            found = true;
                            break;
                        case ENTRANCE_TYPE_RIDE_EXIT:
                            /* For mechanics heading for the ride exit, the
                             * goal is the ride exit tile. */
                            direction = tileElement->GetDirection();
                            if (direction == testEdge)
                            {
                                searchResult = PathSearchResult::RideExit;
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
                    const auto* pathElement = tileElement->AsPath();
                    /* For peeps heading for a ride with a queue, the goal is the last
                     * queue path.
                     * Otherwise, peeps walk on path tiles to get to the goal. */
                    if (!FootpathIsZAndDirectionValid(*pathElement, loc.z, testEdge))
                        continue;

                    // Path may be sloped, so set z to path base height.
                    loc.z = tileElement->BaseHeight;

                    if (pathElement->IsWide())
                    {
                        /* Check if staff can ignore this wide flag. */
                        if (staff == nullptr || !staff->CanIgnoreWideFlag(loc.ToCoordsXYZ(), tileElement))
                        {
                            searchResult = PathSearchResult::Wide;
                            found = true;
                            break;
                        }
                    }

                    searchResult = PathSearchResult::Thin;

                    uint8_t numEdges = std::popcount(pathElement->GetEdges());

                    if (numEdges < 2)
                    {
                        searchResult = PathSearchResult::DeadEnd;
                    }
                    else if (numEdges > 2)
                    {
                        searchResult = PathSearchResult::Junction;
                    }
                    else
                    { // numEdges == 2
                        if (pathElement->IsQueue() && pathElement->GetRideIndex() != state.queueRideIndex)
                        {
                            if (state.ignoreForeignQueues && !pathElement->GetRideIndex().IsNull())
                            {
                                // Path is a queue we aren't interested in
                                /* The rideIndex will be useful for
                                 * adding transport rides later. */
                                rideIndex = pathElement->GetRideIndex();
                                searchResult = PathSearchResult::RideQueue;
                            }
                        }
                    }
                    found = true;
                }
                break;
                default:
                    continue;
            }

            LogPathfinding(
                &peep, "Checking map element at %d,%d,%d; Type: %s; Steps: %u", loc.x >> 5, loc.y >> 5, loc.z,
                PathSearchToString(searchResult), numSteps);

            /* At this point tileElement is of interest to the pathfinding. */

            /* Should we check that this tileElement is connected in the
             * reverse direction? For some tileElement types this was
             * already done above (e.g. ride entrances), but for others not.
             * Ignore for now. */

            // Calculate the heuristic score of this map element.
            uint16_t newScore = CalculateHeuristicPathingScore(loc, goal);

            /* If this map element is the search goal the current search path ends here. */
            if (newScore == 0)
            {
                /* If the search result is better than the best so far (in the parameters),
                 * then update the parameters with this search before continuing to the next map element. */
                if (newScore < *endScore || (newScore == *endScore && numSteps < *endSteps))
                {
                    // Update the search results
                    *endScore = newScore;
                    *endSteps = numSteps;
                    // Update the end x,y,z
                    *endXYZ = loc;
                    // Update the telemetry
                    *endJunctions = state.maxJunctions - state.junctionCount;
                    for (uint8_t junctInd = 0; junctInd < *endJunctions; junctInd++)
                    {
                        uint8_t histIdx = state.maxJunctions - junctInd;
                        junctionList[junctInd].x = state.history[histIdx].location.x;
                        junctionList[junctInd].y = state.history[histIdx].location.y;
                        junctionList[junctInd].z = state.history[histIdx].location.z;
                        directionList[junctInd] = state.history[histIdx].direction;
                    }
                }
                LogPathfinding(
                    &peep, "Search path ends at %d,%d,%d; Steps: %u; At goal; Score: %d", loc.x >> 5, loc.y >> 5, loc.z,
                    numSteps, newScore);
                continue;
            }

            /* At this point the map element tile is not the goal. */

            /* If this map element is not a path, the search cannot be continued.
             * Continue to the next map element without updating the parameters (best result so far). */
            if (searchResult != PathSearchResult::DeadEnd && searchResult != PathSearchResult::Thin
                && searchResult != PathSearchResult::Junction && searchResult != PathSearchResult::Wide)
            {
                LogPathfinding(
                    &peep, "Search path ends at %d,%d,%d; Steps: %u; Not a path", loc.x >> 5, loc.y >> 5, loc.z, numSteps);
                continue;
            }

            /* At this point the map element is a path. */

            /* If this is a wide path the search ends here. */
            if (searchResult == PathSearchResult::Wide)
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
                if (currentElementIsWide && (newScore < *endScore || (newScore == *endScore && numSteps < *endSteps)))
                {
                    // Update the search results
                    *endScore = newScore;
                    *endSteps = numSteps;
                    // Update the end x,y,z
                    *endXYZ = loc;
                    // Update the telemetry
                    *endJunctions = state.maxJunctions - state.junctionCount;
                    for (uint8_t junctInd = 0; junctInd < *endJunctions; junctInd++)
                    {
                        uint8_t histIdx = state.maxJunctions - junctInd;
                        junctionList[junctInd].x = state.history[histIdx].location.x;
                        junctionList[junctInd].y = state.history[histIdx].location.y;
                        junctionList[junctInd].z = state.history[histIdx].location.z;
                        directionList[junctInd] = state.history[histIdx].direction;
                    }
                }
                LogPathfinding(
                    &peep, "Search path ends at %d,%d,%d; Steps: %u; Wide path; Score: %d", loc.x >> 5, loc.y >> 5, loc.z,
                    numSteps, newScore);
                continue;
            }

            /* At this point the map element is a non-wide path.*/

            /* Get all the permitted_edges of the map element. */
            Guard::Assert(tileElement->AsPath() != nullptr);
            uint32_t edges = PathGetPermittedEdges(staff != nullptr, tileElement->AsPath());

            LogPathfinding(
                &peep, "Path element at %d,%d,%d; Steps: %u; Edges (0123):%d%d%d%d; Reverse: %d", loc.x >> 5, loc.y >> 5, loc.z,
                numSteps, edges & 1, (edges & 2) >> 1, (edges & 4) >> 2, (edges & 8) >> 3, testEdge ^ 2);

            /* Remove the reverse edge (i.e. the edge back to the previous map element.) */
            edges &= ~(1 << DirectionReverse(testEdge));

            int32_t nextTestEdge = Numerics::bitScanForward(edges);

            /* If there are no other edges the current search ends here.
             * Continue to the next map element without updating the parameters (best result so far). */
            if (nextTestEdge == -1)
            {
                LogPathfinding(
                    &peep, "Search path ends at %d,%d,%d; Steps: %u; No more edges/dead end", loc.x >> 5, loc.y >> 5, loc.z,
                    numSteps);
                continue;
            }

            /* Check if either of the search limits has been reached:
             * - max number of steps or max tiles checked. */
            if (numSteps >= 200 || state.countTilesChecked <= 0)
            {
                /* The current search ends here.
                 * The path continues, so the goal could still be reachable from here.
                 * If the search result is better than the best so far (in the parameters),
                 * then update the parameters with this search before continuing to the next map element. */
                if (newScore < *endScore || (newScore == *endScore && numSteps < *endSteps))
                {
                    // Update the search results
                    *endScore = newScore;
                    *endSteps = numSteps;
                    // Update the end x,y,z
                    *endXYZ = loc;
                    // Update the telemetry
                    *endJunctions = state.maxJunctions - state.junctionCount;
                    for (uint8_t junctInd = 0; junctInd < *endJunctions; junctInd++)
                    {
                        uint8_t histIdx = state.maxJunctions - junctInd;
                        junctionList[junctInd].x = state.history[histIdx].location.x;
                        junctionList[junctInd].y = state.history[histIdx].location.y;
                        junctionList[junctInd].z = state.history[histIdx].location.z;
                        directionList[junctInd] = state.history[histIdx].direction;
                    }
                }
                LogPathfinding(
                    &peep, "Search path ends at %d,%d,%d; Steps: %u; Search limit reached; Score: %d", loc.x >> 5, loc.y >> 5,
                    loc.z, numSteps, newScore);
                continue;
            }

            bool isThinJunction = false;
            if (searchResult == PathSearchResult::Junction)
            {
                /* Check if this is a thin junction. And perform additional
                 * necessary checks. */
                isThinJunction = PathIsThinJunction(tileElement->AsPath(), loc);

                if (isThinJunction)
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
                        for (int32_t junctionNum = state.junctionCount + 1; junctionNum <= state.maxJunctions; junctionNum++)
                        {
                            if (state.history[junctionNum].location == loc)
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
                        LogPathfinding(
                            &peep, "Search path ends at %d,%d,%d; Steps: %u; Loop", loc.x >> 5, loc.y >> 5, loc.z, numSteps);
                        continue;
                    }

                    /* If the junction search limit is reached, the
                     * current search path ends here. The goal may still
                     * be reachable from here.
                     * If the search result is better than the best so far (in the parameters),
                     * then update the parameters with this search before continuing to the next map element. */
                    if (state.junctionCount <= 0)
                    {
                        if (newScore < *endScore || (newScore == *endScore && numSteps < *endSteps))
                        {
                            // Update the search results
                            *endScore = newScore;
                            *endSteps = numSteps;
                            // Update the end x,y,z
                            *endXYZ = loc;
                            // Update the telemetry
                            *endJunctions = state.maxJunctions; // - _peepPathFindNumJunctions;
                            for (uint8_t junctInd = 0; junctInd < *endJunctions; junctInd++)
                            {
                                uint8_t histIdx = state.maxJunctions - junctInd;
                                junctionList[junctInd] = state.history[histIdx].location;
                                directionList[junctInd] = state.history[histIdx].direction;
                            }
                        }
                        LogPathfinding(
                            &peep, "Search path ends at %d,%d,%d; Steps: %u; NumJunctions < 0; Score: %d", loc.x >> 5,
                            loc.y >> 5, loc.z, numSteps, newScore);
                        continue;
                    }

                    /* This junction was NOT previously visited in the current
                     * search path, so add the junction to the history. */
                    state.history[state.junctionCount].location = loc;
                    // .direction take is added below.

                    state.junctionCount--;
                }
            }

            /* Continue searching down each remaining edge of the path
             * (recursive call). */
            do
            {
                edges &= ~(1 << nextTestEdge);
                uint8_t savedNumJunctions = state.junctionCount;

                uint8_t height = loc.z;
                if (tileElement->AsPath()->IsSloped() && tileElement->AsPath()->GetSlopeDirection() == nextTestEdge)
                {
                    height += 2;
                }

                if constexpr (kLogPathfinding)
                {
                    if (searchResult == PathSearchResult::Junction)
                    {
                        if (isThinJunction)
                            LogPathfinding(
                                &peep, "Recurse from %d,%d,%d; Steps: %u; edge: %d; Thin-Junction", loc.x >> 5, loc.y >> 5,
                                loc.z, numSteps, nextTestEdge);
                        else
                            LogPathfinding(
                                &peep, "Recurse from %d,%d,%d; Steps: %u; edge: %d; Wide-Junction", loc.x >> 5, loc.y >> 5,
                                loc.z, numSteps, nextTestEdge);
                    }
                    else
                    {
                        LogPathfinding(
                            &peep, "Recurse from %d,%d,%d; Steps: %u; edge: %d; Segment", loc.x >> 5, loc.y >> 5, loc.z,
                            numSteps, nextTestEdge);
                    }
                }

                if (isThinJunction)
                {
                    /* Add the current test_edge to the history. */
                    state.history[state.junctionCount + 1].direction = nextTestEdge;
                }

                PeepPathfindHeuristicSearch(
                    state, { loc.x, loc.y, height }, goal, peep, tileElement, nextInPatrolArea, numSteps, endScore,
                    nextTestEdge, endJunctions, junctionList, directionList, endXYZ, endSteps);
                state.junctionCount = savedNumJunctions;

                LogPathfinding(
                    &peep, "Returned to %d,%d,%d; Steps: %u; edge: %d; Score: %d", loc.x >> 5, loc.y >> 5, loc.z, numSteps,
                    nextTestEdge, *endScore);
            } while ((nextTestEdge = Numerics::bitScanForward(edges)) != -1);

        } while (!(tileElement++)->IsLastForTile());

        if (!found)
        {
            /* No map element could be found.
             * Return without updating the parameters (best result so far). */
            LogPathfinding(
                &peep, "Returning from %d,%d,%d; Steps: %u; No relevant map element found", loc.x >> 5, loc.y >> 5, loc.z,
                numSteps);
        }
        else
        {
            LogPathfinding(
                &peep, "Returning from %d,%d,%d; Steps: %u; All map elements checked", loc.x >> 5, loc.y >> 5, loc.z, numSteps);
        }
    }

    /**
     * Returns:
     *   -1   - no direction chosen
     *   0..3 - chosen direction
     *
     *  rct2: 0x0069A5F0
     */
    Direction ChooseDirection(
        const TileCoordsXYZ& loc, const TileCoordsXYZ& goal, Peep& peep, bool ignoreForeignQueues, RideId queueRideIndex)
    {
        PROFILED_FUNCTION();

        PathFindingState state{};

        state.ignoreForeignQueues = ignoreForeignQueues;
        state.queueRideIndex = queueRideIndex;

        // The max number of thin junctions searched - a per-search-path limit.
        state.maxJunctions = PeepPathfindGetMaxNumberJunctions(peep);

        /* The max number of tiles to check - a whole-search limit.
         * Mainly to limit the performance impact of the path finding. */
        int32_t maxTilesChecked = (peep.Is<Staff>()) ? 50000 : 15000;

        LogPathfinding(&peep, "Choose direction for goal %d,%d,%d from %d,%d,%d", goal.x, goal.y, goal.z, loc.x, loc.y, loc.z);

        // Get the path element at this location
        TileElement* destTileElement = MapGetFirstElementAt(loc);
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
        TileElement* firstTileElement = nullptr;

        bool found = false;
        uint8_t permittedEdges = 0;
        bool isThin = false;
        do
        {
            if (destTileElement == nullptr)
                break;
            if (destTileElement->BaseHeight != loc.z)
                continue;
            if (destTileElement->GetType() != TileElementType::Path)
                continue;
            found = true;
            if (firstTileElement == nullptr)
            {
                firstTileElement = destTileElement;
            }

            /* Check if this path element is a thin junction.
             * Only 'thin' junctions are remembered in peep.PathfindHistory.
             * NO attempt is made to merge the overlaid path elements and
             * check if the combination is 'thin'!
             * The junction is considered 'thin' simply if any of the
             * overlaid path elements there is a 'thin junction'. */
            isThin = isThin || PathIsThinJunction(destTileElement->AsPath(), loc);

            // Collect the permitted edges of ALL matching path elements at this location.
            permittedEdges |= PathGetPermittedEdges(peep.Is<Staff>(), destTileElement->AsPath());
        } while (!(destTileElement++)->IsLastForTile());
        // Peep is not on a path.
        if (!found)
            return INVALID_DIRECTION;

        permittedEdges &= 0xF;
        uint32_t edges = permittedEdges;
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
                    pathfindHistory.direction &= permittedEdges;

                    edges = pathfindHistory.direction;

                    LogPathfinding(
                        &peep, "Getting untried edges from pf_history for %d,%d,%d:  %s,%s,%s,%s", loc.x, loc.y, loc.z,
                        (edges & 1) ? "0" : "-", (edges & 2) ? "1" : "-", (edges & 4) ? "2" : "-", (edges & 8) ? "3" : "-");

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
                        pathfindHistory.direction = permittedEdges;
                        edges = pathfindHistory.direction;

                        LogPathfinding(&peep, "All edges tried for %d,%d,%d - resetting to all untried", loc.x, loc.y, loc.z);
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

            LogPathfinding(&peep, "New goal; clearing pf_history.");
        }

        // Peep has tried all edges.
        if (edges == 0)
            return INVALID_DIRECTION;

        int32_t chosenEdge = Numerics::bitScanForward(edges);

        // Peep has multiple edges still to try.
        if (edges & ~(1 << chosenEdge))
        {
            uint8_t bestJunctions = 0;
            TileCoordsXYZ bestJunctionList[16];
            uint8_t bestDirectionList[16];
            TileCoordsXYZ bestXYZ;

            uint16_t bestScore = 0xFFFF;
            uint8_t bestSub = 0xFF;

            LogPathfinding(
                &peep, "Pathfind start for goal %d,%d,%d from %d,%d,%d", goal.x, goal.y, goal.z, loc.x, loc.y, loc.z);

            /* Call the search heuristic on each edge, keeping track of the
             * edge that gives the best (i.e. smallest) value (best_score)
             * or for different edges with equal value, the edge with the
             * least steps (best_sub). */
            int32_t numEdges = std::popcount(edges);
            for (int32_t testEdge = chosenEdge; testEdge != -1; testEdge = Numerics::bitScanForward(edges))
            {
                edges &= ~(1 << testEdge);
                uint8_t height = loc.z;

                if (firstTileElement->AsPath()->IsSloped() && firstTileElement->AsPath()->GetSlopeDirection() == testEdge)
                {
                    height += 0x2;
                }

                /* Divide the maxTilesChecked global search limit
                 * between the remaining edges to ensure the search
                 * covers all of the remaining edges. */
                state.countTilesChecked = maxTilesChecked / numEdges;
                state.junctionCount = state.maxJunctions;

                // Initialise _peepPathFindHistory.

                for (auto& entry : state.history)
                {
                    entry.location.SetNull();
                    entry.direction = INVALID_DIRECTION;
                }

                /* The pathfinding will only use elements
                 * 1.._peepPathFindMaxJunctions, so the starting point
                 * is placed in element 0 */
                state.history[0].location = loc;
                state.history[0].direction = 0xF;

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

                LogPathfinding(
                    &peep, "Pathfind searching in direction: %d from %d,%d,%d", testEdge, loc.x >> 5, loc.y >> 5, loc.z);

                PeepPathfindHeuristicSearch(
                    state, { loc.x, loc.y, height }, goal, peep, firstTileElement, inPatrolArea, 0, &score, testEdge,
                    &endJunctions, endJunctionList, endDirectionList, &endXYZ, &endSteps);

                if constexpr (kLogPathfinding)
                {
                    LogPathfinding(
                        &peep, "Pathfind test edge: %d score: %d steps: %d end: %d,%d,%d junctions: %d", testEdge, score,
                        endSteps, endXYZ.x, endXYZ.y, endXYZ.z, endJunctions);
                    for (uint8_t listIdx = 0; listIdx < endJunctions; listIdx++)
                    {
                        LogPathfinding(
                            &peep, "Junction#%d %d,%d,%d Direction %d", listIdx + 1, endJunctionList[listIdx].x,
                            endJunctionList[listIdx].y, endJunctionList[listIdx].z, endDirectionList[listIdx]);
                    }
                }

                if (score < bestScore || (score == bestScore && endSteps < bestSub))
                {
                    chosenEdge = testEdge;
                    bestScore = score;
                    bestSub = endSteps;

                    if constexpr (kLogPathfinding)
                    {
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
                    }
                }
            }

            /* Check if the heuristic search failed. e.g. all connected
             * paths are within the search limits and none reaches the
             * goal. */
            if (bestScore == 0xFFFF)
            {
                LogPathfinding(&peep, "Pathfind heuristic search failed.");
                return INVALID_DIRECTION;
            }

            if constexpr (kLogPathfinding)
            {
                LogPathfinding(&peep, "Pathfind best edge %d with score %d steps %d", chosenEdge, bestScore, bestSub);
                for (uint8_t listIdx = 0; listIdx < bestJunctions; listIdx++)
                {
                    LogPathfinding(
                        &peep, "Junction#%d %d,%d,%d Direction %d", listIdx + 1, bestJunctionList[listIdx].x,
                        bestJunctionList[listIdx].y, bestJunctionList[listIdx].z, bestDirectionList[listIdx]);
                }
                LogPathfinding(&peep, "End at %d,%d,%d", bestXYZ.x, bestXYZ.y, bestXYZ.z);
            }
        }

        if (isThin)
        {
            for (std::size_t i = 0; i < peep.PathfindHistory.size(); ++i)
            {
                if (peep.PathfindHistory[i] == loc)
                {
                    /* Peep remembers this junction, so remove the
                     * chosen_edge from those left to try. */
                    peep.PathfindHistory[i].direction &= ~(1 << chosenEdge);
                    /* Also remove the edge through which the peep
                     * entered the junction from those left to try. */
                    peep.PathfindHistory[i].direction &= ~(1 << DirectionReverse(peep.PeepDirection));

                    LogPathfinding(
                        &peep, "Updating existing pf_history (in index: %u) for %d,%d,%d without entry edge %d & exit edge %d.",
                        i, loc.x, loc.y, loc.z, DirectionReverse(peep.PeepDirection), chosenEdge);

                    return chosenEdge;
                }
            }

            /* Peep does not remember this junction, so forget a junction
             * and remember this junction. */
            int32_t i = peep.PathfindGoal.direction++;
            peep.PathfindGoal.direction &= 3;
            peep.PathfindHistory[i] = { loc, permittedEdges };
            /* Remove the chosen_edge from those left to try. */
            peep.PathfindHistory[i].direction &= ~(1 << chosenEdge);
            /* Also remove the edge through which the peep
             * entered the junction from those left to try. */
            peep.PathfindHistory[i].direction &= ~(1 << DirectionReverse(peep.PeepDirection));

            LogPathfinding(
                &peep, "Storing new pf_history (in index: %d) for %d,%d,%d without entry edge %d & exit edge %d.", i, loc.x,
                loc.y, loc.z, DirectionReverse(peep.PeepDirection), chosenEdge);
        }

        return chosenEdge;
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
        for (const auto& parkEntrance : GetGameState().Park.Entrances)
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
    int32_t GuestPathFindParkEntranceEntering(Peep& peep, uint8_t edges)
    {
        // Send peeps to the nearest park entrance.
        auto chosenEntrance = GetNearestParkEntrance(peep.NextLoc);

        // If no defined park entrances are found, walk aimlessly.
        if (!chosenEntrance.has_value())
            return GuestPathfindAimless(peep, edges);

        const auto goalPos = TileCoordsXYZ(chosenEntrance.value());
        Direction chosenDirection = ChooseDirection(TileCoordsXYZ{ peep.NextLoc }, goalPos, peep, true, RideId::GetNull());

        if (chosenDirection == INVALID_DIRECTION)
            return GuestPathfindAimless(peep, edges);

        return PeepMoveOneTile(chosenDirection, peep);
    }

    /**
     * Gets the nearest peep spawn relative to point, by using Manhattan distance.
     * @param x x coordinate of location
     * @param y y coordinate of location
     * @return Index of gameState.PeepSpawns (or 0xFF if no peep spawns exist).
     */
    static uint8_t GetNearestPeepSpawnIndex(uint16_t x, uint16_t y)
    {
        uint8_t chosenSpawn = 0xFF;
        uint16_t nearestDist = 0xFFFF;
        uint8_t i = 0;
        for (const auto& spawn : GetGameState().PeepSpawns)
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
    int32_t GuestPathFindPeepSpawn(Peep& peep, uint8_t edges)
    {
        // Send peeps to the nearest spawn point.
        uint8_t chosenSpawn = GetNearestPeepSpawnIndex(peep.NextLoc.x, peep.NextLoc.y);

        // If no defined spawns were found, walk aimlessly.
        if (chosenSpawn == 0xFF)
            return GuestPathfindAimless(peep, edges);

        const auto peepSpawnLoc = GetGameState().PeepSpawns[chosenSpawn].ToTileStart();
        Direction direction = peepSpawnLoc.direction;

        if (peepSpawnLoc.x == peep.NextLoc.x && peepSpawnLoc.y == peep.NextLoc.y)
        {
            return PeepMoveOneTile(direction, peep);
        }

        const auto goalPos = TileCoordsXYZ(peepSpawnLoc);
        direction = ChooseDirection(TileCoordsXYZ{ peep.NextLoc }, goalPos, peep, true, RideId::GetNull());
        if (direction == INVALID_DIRECTION)
            return GuestPathfindAimless(peep, edges);

        return PeepMoveOneTile(direction, peep);
    }

    /**
     *
     *  rct2: 0x00695161
     */
    int32_t GuestPathFindParkEntranceLeaving(Peep& peep, uint8_t edges)
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
                return GuestPathfindAimless(peep, edges);

            peep.PeepFlags |= PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
            entranceGoal = TileCoordsXYZ(*chosenEntrance);
        }

        Direction chosenDirection = ChooseDirection(TileCoordsXYZ{ peep.NextLoc }, entranceGoal, peep, true, RideId::GetNull());
        if (chosenDirection == INVALID_DIRECTION)
            return GuestPathfindAimless(peep, edges);

        return PeepMoveOneTile(chosenDirection, peep);
    }

    /**
     *
     *  rct2: 0x006A72C5
     *  param dist is not used.
     *
     * In case where the map element at (x, y) is invalid or there is no entrance
     * or queue leading to it the function will not update its arguments.
     */
    static void GetRideQueueEnd(TileCoordsXYZ& loc)
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

            if (loc.z != tileElement->BaseHeight)
                continue;

            found = true;
            break;
        } while (!(tileElement++)->IsLastForTile());

        if (!found)
            return;

        Direction direction = DirectionReverse(tileElement->GetDirection());
        TileElement* lastPathElement = nullptr;
        TileElement* firstPathElement = nullptr;

        int16_t baseZ = tileElement->BaseHeight;
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

                if (baseZ == tileElement->BaseHeight)
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

                if (baseZ - 2 == tileElement->BaseHeight)
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

        if (loc.z == kMaxTileElementHeight)
            return;

        tileElement = lastPathElement;
        if (tileElement == nullptr)
            return;

        if (!tileElement->AsPath()->IsQueue())
            return;

        loc.x = queueEnd.x;
        loc.y = queueEnd.y;
        loc.z = tileElement->BaseHeight;
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
    static StationIndex GuestPathfindingSelectRandomStation(
        const Guest& guest, int32_t numEntranceStations, BitSet<OpenRCT2::Limits::kMaxStationsPerRide>& entranceStations)
    {
        int32_t select = guest.GuestNumRides % numEntranceStations;
        while (select > 0)
        {
            for (StationIndex::UnderlyingType i = 0; i < OpenRCT2::Limits::kMaxStationsPerRide; i++)
            {
                if (entranceStations[i])
                {
                    entranceStations[i] = false;
                    select--;
                    break;
                }
            }
        }
        for (StationIndex::UnderlyingType i = 0; i < OpenRCT2::Limits::kMaxStationsPerRide; i++)
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
    int32_t CalculateNextDestination(Guest& peep)
    {
        LogPathfinding(&peep, "Starting CalculateNextDestination");

        if (peep.GetNextIsSurface())
        {
            return GuestSurfacePathFinding(peep);
        }

        TileCoordsXYZ loc{ peep.NextLoc };

        auto* pathElement = MapGetPathElementAt(loc);
        if (pathElement == nullptr)
        {
            return 1;
        }

        // Because this function is called for guests only, never ignore banners.
        uint32_t edges = PathGetPermittedEdges(false, pathElement);

        if (edges == 0)
        {
            return GuestSurfacePathFinding(peep);
        }

        if (!peep.OutsideOfPark && peep.HeadingForRideOrParkExit())
        {
            /* If this tileElement is adjacent to any non-wide paths,
             * remove all of the edges to wide paths. */
            uint8_t adjustedEdges = edges;
            for (Direction chosenDirection : kAllDirections)
            {
                // If there is no path in that direction try another
                if (!(adjustedEdges & (1 << chosenDirection)))
                    continue;

                /* If there is a wide path in that direction,
                    remove that edge and try another */
                if (FootpathElementNextInDirection(loc, pathElement, chosenDirection) == PathSearchResult::Wide)
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

        direction = Numerics::bitScanForward(edges);
        // IF only one edge to choose from
        if ((edges & ~(1 << direction)) == 0)
        {
            LogPathfinding(&peep, "Completed CalculateNextDestination - taking only direction available: %d.", direction);

            return PeepMoveOneTile(direction, peep);
        }

        // Peep still has multiple edges to choose from.

        // Peep is outside the park.
        // Loc694F19:
        if (peep.OutsideOfPark)
        {
            LogPathfinding(&peep, "Completed CalculateNextDestination - peep is outside the park.");

            switch (peep.State)
            {
                case PeepState::EnteringPark:
                    return GuestPathFindParkEntranceEntering(peep, edges);
                case PeepState::LeavingPark:
                    return GuestPathFindPeepSpawn(peep, edges);
                default:
                    return GuestPathfindAimless(peep, edges);
            }
        }

        /* Peep is inside the park.
         * If the peep does not have food, randomly cull the useless directions
         * (dead ends, ride exits, wide paths) from the edges.
         * In principle, peeps with food are not paying as much attention to
         * where they are going and are consequently more like to walk up
         * dead end paths, paths to ride exits, etc. */
        if (!peep.HasFoodOrDrink() && (ScenarioRand() & 0xFFFF) >= 2184)
        {
            uint8_t adjustedEdges = edges;
            for (Direction chosenDirection : kAllDirections)
            {
                // If there is no path in that direction try another
                if (!(adjustedEdges & (1 << chosenDirection)))
                    continue;

                RideId rideIndex = RideId::GetNull();
                auto pathSearchResult = FootpathElementDestinationInDirection(loc, pathElement, chosenDirection, &rideIndex);
                switch (pathSearchResult)
                {
                    case PathSearchResult::DeadEnd:
                    case PathSearchResult::RideExit:
                    case PathSearchResult::Wide:
                        adjustedEdges &= ~(1 << chosenDirection);
                        break;
                    default:
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
            if (std::popcount(edges) >= 2)
            {
                uint16_t probability = 1638;
                if (peep.HeadingForRideOrParkExit())
                {
                    probability = 9362;
                }
                if ((ScenarioRand() & 0xFFFF) < probability)
                {
                    peep.ReadMap();
                }
            }
        }

        if (peep.PeepFlags & PEEP_FLAGS_LEAVING_PARK)
        {
            LogPathfinding(&peep, "Completed CalculateNextDestination - peep is leaving the park.");

            return GuestPathFindParkEntranceLeaving(peep, edges);
        }

        if (peep.GuestHeadingToRideId.IsNull())
        {
            LogPathfinding(&peep, "Completed CalculateNextDestination - peep is aimless.");

            return GuestPathfindAimless(peep, edges);
        }

        // Peep is heading for a ride.
        RideId rideIndex = peep.GuestHeadingToRideId;
        auto ride = GetRide(rideIndex);
        if (ride == nullptr || ride->status != RideStatus::Open)
        {
            LogPathfinding(&peep, "Completed CalculateNextDestination - peep is heading to closed ride == aimless.");

            return GuestPathfindAimless(peep, edges);
        }

        /* Find the ride's closest entrance station to the peep.
         * At the same time, count how many entrance stations there are and
         * which stations are entrance stations. */
        auto bestScore = std::numeric_limits<int32_t>::max();
        StationIndex closestStationNum = StationIndex::FromUnderlying(0);

        int32_t numEntranceStations = 0;
        BitSet<OpenRCT2::Limits::kMaxStationsPerRide> entranceStations = {};

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
            closestStationNum = GuestPathfindingSelectRandomStation(peep, numEntranceStations, entranceStations);
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

        GetRideQueueEnd(loc);

        direction = ChooseDirection(TileCoordsXYZ{ peep.NextLoc }, loc, peep, true, rideIndex);

        if (direction == INVALID_DIRECTION)
        {
            /* Heuristic search failed for all directions.
             * Reset the PathfindGoal - this means that the PathfindHistory
             * will be reset in the next call to ChooseDirection().
             * This lets the heuristic search "try again" in case the player has
             * edited the path layout or the mechanic was already stuck in the
             * save game (e.g. with a worse version of the pathfinding). */
            peep.ResetPathfindGoal();

            LogPathfinding(&peep, "Completed CalculateNextDestination - failed to choose a direction == aimless.");

            return GuestPathfindAimless(peep, edges);
        }

        LogPathfinding(&peep, "Completed CalculateNextDestination - direction chosen: %d.", direction);

        return PeepMoveOneTile(direction, peep);
    }

} // namespace OpenRCT2::PathFinding
