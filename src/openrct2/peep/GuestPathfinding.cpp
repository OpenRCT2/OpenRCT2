#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include "Peep.h"


/**
 *
 *  rct2: 0x00694C41
 */
static sint32 guest_surface_path_finding(rct_peep * peep)
{
    sint16 x             = peep->next_x;
    sint16 y             = peep->next_y;
    sint16 z             = peep->next_z;
    uint8  randDirection = scenario_rand() & 3;

    if (!fence_in_the_way(x, y, z, z + 4, randDirection))
    {
        x += TileDirectionDelta[randDirection].x;
        y += TileDirectionDelta[randDirection].y;
        uint8 backwardsDirection = randDirection ^ (1 << 1);

        if (!fence_in_the_way(x, y, z, z + 4, backwardsDirection))
        {
            if (!map_surface_is_blocked(x, y))
            {
                return peep_move_one_tile(randDirection, peep);
            }
        }
    }

    randDirection++;
    uint8 rand_backwards = scenario_rand() & 1;
    if (rand_backwards)
    {
        randDirection -= 2;
    }
    randDirection &= 3;

    x = peep->next_x;
    y = peep->next_y;
    if (!fence_in_the_way(x, y, z, z + 4, randDirection))
    {
        x += TileDirectionDelta[randDirection].x;
        y += TileDirectionDelta[randDirection].y;
        uint8 backwardsDirection = randDirection ^ (1 << 1);

        if (!fence_in_the_way(x, y, z, z + 4, backwardsDirection))
        {
            if (!map_surface_is_blocked(x, y))
            {
                return peep_move_one_tile(randDirection, peep);
            }
        }
    }

    randDirection -= 2;
    randDirection &= 3;

    x = peep->next_x;
    y = peep->next_y;
    if (!fence_in_the_way(x, y, z, z + 4, randDirection))
    {
        x += TileDirectionDelta[randDirection].x;
        y += TileDirectionDelta[randDirection].y;
        uint8 backwardsDirection = randDirection ^ (1 << 1);

        if (!fence_in_the_way(x, y, z, z + 4, backwardsDirection))
        {
            if (!map_surface_is_blocked(x, y))
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
static uint8 footpath_element_next_in_direction(sint16 x, sint16 y, sint16 z, rct_tile_element * tileElement,
                                                uint8 chosenDirection)
{
    rct_tile_element * nextTileElement;

    if (footpath_element_is_sloped(tileElement))
    {
        if (footpath_element_get_slope_direction(tileElement) == chosenDirection)
        {
            z += 2;
        }
    }

    x += TileDirectionDelta[chosenDirection].x;
    y += TileDirectionDelta[chosenDirection].y;
    nextTileElement = map_get_first_element_at(x / 32, y / 32);
    do
    {
        if (nextTileElement->flags & TILE_ELEMENT_FLAG_GHOST)
            continue;
        if (tile_element_get_type(nextTileElement) != TILE_ELEMENT_TYPE_PATH)
            continue;
        if (!is_valid_path_z_and_direction(nextTileElement, z, chosenDirection))
            continue;
        if (footpath_element_is_wide(nextTileElement))
            return PATH_SEARCH_WIDE;
        // Only queue tiles that are connected to a ride are returned as ride queues.
        if (footpath_element_is_queue(nextTileElement) && nextTileElement->properties.path.ride_index != 0xFF)
            return PATH_SEARCH_RIDE_QUEUE;

        return PATH_SEARCH_OTHER;
    } while (!tile_element_is_last_for_tile(nextTileElement++));

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
static uint8 footpath_element_dest_in_dir(sint16 x, sint16 y, sint16 z, rct_tile_element * inputTileElement,
                                          uint8 chosenDirection, uint8 * outRideIndex, sint32 level)
{
    rct_tile_element * tileElement;
    sint32            direction;

    if (level > 25)
        return PATH_SEARCH_LIMIT_REACHED;

    x += TileDirectionDelta[chosenDirection].x;
    y += TileDirectionDelta[chosenDirection].y;
    tileElement = map_get_first_element_at(x / 32, y / 32);
    if (tileElement == nullptr)
    {
        return PATH_SEARCH_FAILED;
    }
    do
    {
        if (tileElement->flags & TILE_ELEMENT_FLAG_GHOST)
            continue;

        switch (tile_element_get_type(tileElement))
        {
        case TILE_ELEMENT_TYPE_TRACK:
        {
            if (z != tileElement->base_height)
                continue;
            sint32 rideIndex = track_element_get_ride_index(tileElement);
            Ride * ride      = get_ride(rideIndex);
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
            {
                *outRideIndex = rideIndex;
                return PATH_SEARCH_SHOP_ENTRANCE;
            }
        }
        break;
        case TILE_ELEMENT_TYPE_ENTRANCE:
            if (z != tileElement->base_height)
                continue;
            switch (tileElement->properties.entrance.type)
            {
            case ENTRANCE_TYPE_RIDE_ENTRANCE:
                direction = tile_element_get_direction(tileElement);
                if (direction == chosenDirection)
                {
                    *outRideIndex = tileElement->properties.entrance.ride_index;
                    return PATH_SEARCH_RIDE_ENTRANCE;
                }
                break;
            case ENTRANCE_TYPE_RIDE_EXIT:
                direction = tile_element_get_direction(tileElement);
                if (direction == chosenDirection)
                {
                    *outRideIndex = tileElement->properties.entrance.ride_index;
                    return PATH_SEARCH_RIDE_EXIT;
                }
                break;
            case ENTRANCE_TYPE_PARK_ENTRANCE:
                return PATH_SEARCH_PARK_EXIT;
            }
            break;
        case TILE_ELEMENT_TYPE_PATH:
            if (!is_valid_path_z_and_direction(tileElement, z, chosenDirection))
                continue;
            if (footpath_element_is_wide(tileElement))
                return PATH_SEARCH_WIDE;

            uint8 edges = path_get_permitted_edges(tileElement);
            edges &= ~(1 << (chosenDirection ^ 2));
            z = tileElement->base_height;

            for (direction = 0; direction < 4; direction++)
            {
                if (!(edges & (1 << direction)))
                    continue;

                edges &= ~(1 << direction);
                if (edges != 0)
                    return PATH_SEARCH_JUNCTION;

                if (footpath_element_is_sloped(tileElement))
                {
                    if (footpath_element_get_slope_direction(tileElement) == direction)
                    {
                        z += 2;
                    }
                }
                return footpath_element_dest_in_dir(x, y, z, tileElement, direction, outRideIndex, level + 1);
            }
            return PATH_SEARCH_DEAD_END;
        }
    } while (!tile_element_is_last_for_tile(tileElement++));

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
static uint8 footpath_element_destination_in_direction(sint16 x, sint16 y, sint16 z, rct_tile_element * inputTileElement,
                                                       uint8 chosenDirection, uint8 * outRideIndex)
{
    if (footpath_element_is_sloped(inputTileElement))
    {
        if (footpath_element_get_slope_direction(inputTileElement) == chosenDirection)
        {
            z += 2;
        }
    }

    return footpath_element_dest_in_dir(x, y, z, inputTileElement, chosenDirection, outRideIndex, 0);
}

/**
 *
 *  rct2: 0x00695225
 */
static sint32 guest_path_find_aimless(rct_peep * peep, uint8 edges)
{
    if (scenario_rand() & 1)
    {
        // If possible go straight
        if (edges & (1 << peep->direction))
        {
            return peep_move_one_tile(peep->direction, peep);
        }
    }

    while (true)
    {
        uint8 direction = scenario_rand() & 3;
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
static uint8 peep_pathfind_get_max_number_junctions(rct_peep * peep)
{
    if (peep->type == PEEP_TYPE_STAFF)
        return 8;

    // PEEP_FLAGS_2? It's cleared here but not set anywhere!
    if ((peep->peep_flags & PEEP_FLAGS_2))
    {
        if ((scenario_rand() & 0xFFFF) <= 7281)
            peep->peep_flags &= ~PEEP_FLAGS_2;

        return 8;
    }

    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK && peep->peep_is_lost_countdown < 90)
    {
        return 8;
    }

    if (peep->item_standard_flags & PEEP_ITEM_MAP)
        return 7;

    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
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
static bool path_is_thin_junction(rct_tile_element * path, sint16 x, sint16 y, uint8 z)
{
    uint8 edges = footpath_get_edges(path);

    sint32 test_edge = bitscanforward(edges);
    if (test_edge == -1)
        return false;

    bool   thin_junction = false;
    sint32 thin_count    = 0;
    do
    {
        sint32 fp_result = footpath_element_next_in_direction(x, y, z, path, test_edge);

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
static void peep_pathfind_heuristic_search(sint16 x, sint16 y, uint8 z, rct_peep * peep, rct_tile_element * currentTileElement,
                                           bool inPatrolArea, uint8 counter, uint16 * endScore, sint32 test_edge,
                                           uint8 * endJunctions, TileCoordsXYZ junctionList[16], uint8 directionList[16],
                                           TileCoordsXYZ * endXYZ, uint8 * endSteps)
{
    uint8 searchResult = PATH_SEARCH_FAILED;

    bool currentElementIsWide =
        (footpath_element_is_wide(currentTileElement) && !staff_can_ignore_wide_flag(peep, x, y, z, currentTileElement));

    x += TileDirectionDelta[test_edge].x;
    y += TileDirectionDelta[test_edge].y;

    ++counter;
    _peepPathFindTilesChecked--;

    /* If this is where the search started this is a search loop and the
     * current search path ends here.
     * Return without updating the parameters (best result so far). */
    if ((_peepPathFindHistory[0].location.x == (uint8)(x >> 5)) && (_peepPathFindHistory[0].location.y == (uint8)(y >> 5)) &&
        (_peepPathFindHistory[0].location.z == z))
    {
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug)
        {
            log_info("[%03d] Return from %d,%d,%d; At start", counter, x >> 5, y >> 5, z);
        }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        return;
    }

    bool nextInPatrolArea = inPatrolArea;
    if (peep->type == PEEP_TYPE_STAFF && peep->staff_type == STAFF_TYPE_MECHANIC)
    {
        nextInPatrolArea = staff_is_location_in_patrol(peep, x, y);
        if (inPatrolArea && !nextInPatrolArea)
        {
/* The mechanic will leave his patrol area by taking
 * the test_edge so the current search path ends here.
 * Return without updating the parameters (best result so far). */
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info("[%03d] Return from %d,%d,%d; Left patrol area", counter, x >> 5, y >> 5, z);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            return;
        }
    }

    /* Get the next map element of interest in the direction of test_edge. */
    bool               found       = false;
    rct_tile_element * tileElement = map_get_first_element_at(x / 32, y / 32);
    if (tileElement == nullptr)
    {
        return;
    }
    do
    {
        /* Look for all map elements that the peep could walk onto while
         * navigating to the goal, including the goal tile. */

        if (tileElement->flags & TILE_ELEMENT_FLAG_GHOST)
            continue;

        uint8 rideIndex = 0xFF;
        switch (tile_element_get_type(tileElement))
        {
        case TILE_ELEMENT_TYPE_TRACK:
        {
            if (z != tileElement->base_height)
                continue;
            /* For peeps heading for a shop, the goal is the shop
             * tile. */
            rideIndex   = track_element_get_ride_index(tileElement);
            Ride * ride = get_ride(rideIndex);
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
            {
                found        = true;
                searchResult = PATH_SEARCH_SHOP_ENTRANCE;
                break;
            }
            else
            {
                continue;
            }
        }
        case TILE_ELEMENT_TYPE_ENTRANCE:
            if (z != tileElement->base_height)
                continue;
            sint32 direction;
            searchResult = PATH_SEARCH_OTHER;
            switch (tileElement->properties.entrance.type)
            {
            case ENTRANCE_TYPE_RIDE_ENTRANCE:
                /* For peeps heading for a ride without a queue, the
                 * goal is the ride entrance tile.
                 * For mechanics heading for the ride entrance
                 * (in the case when the station has no exit),
                 * the goal is the ride entrance tile. */
                direction = tile_element_get_direction(tileElement);
                if (direction == test_edge)
                {
                    /* The rideIndex will be useful for
                     * adding transport rides later. */
                    rideIndex    = tileElement->properties.entrance.ride_index;
                    searchResult = PATH_SEARCH_RIDE_ENTRANCE;
                    found        = true;
                    break;
                }
                continue; // Ride entrance is not facing the right direction.
            case ENTRANCE_TYPE_PARK_ENTRANCE:
                /* For peeps leaving the park, the goal is the park
                 * entrance/exit tile. */
                searchResult = PATH_SEARCH_PARK_EXIT;
                found        = true;
                break;
            case ENTRANCE_TYPE_RIDE_EXIT:
                /* For mechanics heading for the ride exit, the
                 * goal is the ride exit tile. */
                direction = tile_element_get_direction(tileElement);
                if (direction == test_edge)
                {
                    searchResult = PATH_SEARCH_RIDE_EXIT;
                    found        = true;
                    break;
                }
                continue; // Ride exit is not facing the right direction.
            default:
                continue;
            }
            break;
        case TILE_ELEMENT_TYPE_PATH:
        {
            /* For peeps heading for a ride with a queue, the goal is the last
             * queue path.
             * Otherwise, peeps walk on path tiles to get to the goal. */

            if (!is_valid_path_z_and_direction(tileElement, z, test_edge))
                continue;

            // Path may be sloped, so set z to path base height.
            z = tileElement->base_height;

            if (footpath_element_is_wide(tileElement))
            {
                /* Check if staff can ignore this wide flag. */
                if (!staff_can_ignore_wide_flag(peep, x, y, z, tileElement))
                {
                    searchResult = PATH_SEARCH_WIDE;
                    found        = true;
                    break;
                }
            }

            searchResult = PATH_SEARCH_THIN;

            uint8 numEdges = bitcount(footpath_get_edges(tileElement));

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
                if (footpath_element_is_queue(tileElement) &&
                    tileElement->properties.path.ride_index != gPeepPathFindQueueRideIndex)
                {
                    if (gPeepPathFindIgnoreForeignQueues && (tileElement->properties.path.ride_index != 0xFF))
                    {
                        // Path is a queue we aren't interested in
                        /* The rideIndex will be useful for
                         * adding transport rides later. */
                        rideIndex    = tileElement->properties.path.ride_index;
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
            log_info("[%03d] Checking map element at %d,%d,%d; Type: %s", counter, x >> 5, y >> 5, z,
                     gPathFindSearchText[searchResult]);
        }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2

        /* At this point tileElement is of interest to the pathfinding. */

        /* Should we check that this tileElement is connected in the
         * reverse direction? For some tileElement types this was
         * already done above (e.g. ride entrances), but for others not.
         * Ignore for now. */

        // Calculate the heuristic score of this map element.
        uint16 x_delta = abs(gPeepPathFindGoalPosition.x - x);
        uint16 y_delta = abs(gPeepPathFindGoalPosition.y - y);
        if (x_delta < y_delta)
            x_delta >>= 4;
        else
            y_delta >>= 4;
        uint16 new_score = x_delta + y_delta;
        uint16 z_delta   = abs(gPeepPathFindGoalPosition.z - z);
        z_delta <<= 1;
        new_score += z_delta;

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
                endXYZ->x = x >> 5;
                endXYZ->y = y >> 5;
                endXYZ->z = z;
                // Update the telemetry
                *endJunctions = _peepPathFindMaxJunctions - _peepPathFindNumJunctions;
                for (uint8 junctInd = 0; junctInd < *endJunctions; junctInd++)
                {
                    uint8 histIdx            = _peepPathFindMaxJunctions - junctInd;
                    junctionList[junctInd].x = _peepPathFindHistory[histIdx].location.x;
                    junctionList[junctInd].y = _peepPathFindHistory[histIdx].location.y;
                    junctionList[junctInd].z = _peepPathFindHistory[histIdx].location.z;
                    directionList[junctInd]  = _peepPathFindHistory[histIdx].direction;
                }
            }
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info("[%03d] Search path ends at %d,%d,%d; At goal; Score: %d", counter, x >> 5, y >> 5, z, new_score);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        /* At this point the map element tile is not the goal. */

        /* If this map element is not a path, the search cannot be continued.
         * Continue to the next map element without updating the parameters (best result so far). */
        if (searchResult != PATH_SEARCH_DEAD_END && searchResult != PATH_SEARCH_THIN && searchResult != PATH_SEARCH_JUNCTION &&
            searchResult != PATH_SEARCH_WIDE)
        {
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info("[%03d] Search path ends at %d,%d,%d; Not a path", counter, x >> 5, y >> 5, z);
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
                endXYZ->x = x >> 5;
                endXYZ->y = y >> 5;
                endXYZ->z = z;
                // Update the telemetry
                *endJunctions = _peepPathFindMaxJunctions - _peepPathFindNumJunctions;
                for (uint8 junctInd = 0; junctInd < *endJunctions; junctInd++)
                {
                    uint8 histIdx            = _peepPathFindMaxJunctions - junctInd;
                    junctionList[junctInd].x = _peepPathFindHistory[histIdx].location.x;
                    junctionList[junctInd].y = _peepPathFindHistory[histIdx].location.y;
                    junctionList[junctInd].z = _peepPathFindHistory[histIdx].location.z;
                    directionList[junctInd]  = _peepPathFindHistory[histIdx].direction;
                }
            }
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info("[%03d] Search path ends at %d,%d,%d; Wide path; Score: %d", counter, x >> 5, y >> 5, z, new_score);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        /* At this point the map element is a non-wide path.*/

        /* Get all the permitted_edges of the map element. */
        uint8 edges = path_get_permitted_edges(tileElement);

#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug)
        {
            log_info("[%03d] Path element at %d,%d,%d; Edges (0123):%d%d%d%d; Reverse: %d", counter, x >> 5, y >> 5, z,
                     edges & 1, (edges & 2) >> 1, (edges & 4) >> 2, (edges & 8) >> 3, test_edge ^ 2);
        }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2

        /* Remove the reverse edge (i.e. the edge back to the previous map element.) */
        edges &= ~(1 << (test_edge ^ 2));

        sint32 next_test_edge = bitscanforward(edges);

        /* If there are no other edges the current search ends here.
         * Continue to the next map element without updating the parameters (best result so far). */
        if (next_test_edge == -1)
        {
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info("[%03d] Search path ends at %d,%d,%d; No more edges/dead end", counter, x >> 5, y >> 5, z);
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
                endXYZ->x = x >> 5;
                endXYZ->y = y >> 5;
                endXYZ->z = z;
                // Update the telemetry
                *endJunctions = _peepPathFindMaxJunctions - _peepPathFindNumJunctions;
                for (uint8 junctInd = 0; junctInd < *endJunctions; junctInd++)
                {
                    uint8 histIdx            = _peepPathFindMaxJunctions - junctInd;
                    junctionList[junctInd].x = _peepPathFindHistory[histIdx].location.x;
                    junctionList[junctInd].y = _peepPathFindHistory[histIdx].location.y;
                    junctionList[junctInd].z = _peepPathFindHistory[histIdx].location.z;
                    directionList[junctInd]  = _peepPathFindHistory[histIdx].direction;
                }
            }
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info("[%03d] Search path ends at %d,%d,%d; Search limit reached; Score: %d", counter, x >> 5, y >> 5, z,
                         new_score);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            continue;
        }

        bool thin_junction = false;
        if (searchResult == PATH_SEARCH_JUNCTION)
        {
            /* Check if this is a thin junction. And perform additional
             * necessary checks. */
            thin_junction = path_is_thin_junction(tileElement, x, y, z);

            if (thin_junction)
            {
                /* The current search path is passing through a thin
                 * junction on this map element. Only 'thin' junctions
                 * are counted towards the junction search limit. */

                /* First check if going through the junction would be
                 * a loop.  If so, the current search path ends here.
                 * Path finding loop detection can take advantage of both the
                 * peep->pathfind_history - loops through remembered junctions
                 *     the peep has already passed through getting to its
                 *     current position while on the way to its current goal;
                 * _peepPathFindHistory - loops in the current search path. */
                bool pathLoop = false;
                /* Check the peep->pathfind_history to see if this junction has
                 * already been visited by the peep while heading for this goal. */
                for (auto &pathfindHistory : peep->pathfind_history)
                {
                    if (pathfindHistory.x == x >> 5 && pathfindHistory.y == y >> 5 &&
                        pathfindHistory.z == z)
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
                    for (sint32 junctionNum = _peepPathFindNumJunctions + 1; junctionNum <= _peepPathFindMaxJunctions;
                         junctionNum++)
                    {
                        if ((_peepPathFindHistory[junctionNum].location.x == (uint8)(x >> 5)) &&
                            (_peepPathFindHistory[junctionNum].location.y == (uint8)(y >> 5)) &&
                            (_peepPathFindHistory[junctionNum].location.z == z))
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
                        log_info("[%03d] Search path ends at %d,%d,%d; Loop", counter, x >> 5, y >> 5, z);
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
                        endXYZ->x = x >> 5;
                        endXYZ->y = y >> 5;
                        endXYZ->z = z;
                        // Update the telemetry
                        *endJunctions = _peepPathFindMaxJunctions; // - _peepPathFindNumJunctions;
                        for (uint8 junctInd = 0; junctInd < *endJunctions; junctInd++)
                        {
                            uint8 histIdx            = _peepPathFindMaxJunctions - junctInd;
                            junctionList[junctInd].x = _peepPathFindHistory[histIdx].location.x;
                            junctionList[junctInd].y = _peepPathFindHistory[histIdx].location.y;
                            junctionList[junctInd].z = _peepPathFindHistory[histIdx].location.z;
                            directionList[junctInd]  = _peepPathFindHistory[histIdx].direction;
                        }
                    }
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
                    if (gPathFindDebug)
                    {
                        log_info("[%03d] Search path ends at %d,%d,%d; NumJunctions < 0; Score: %d", counter, x >> 5, y >> 5, z,
                                 new_score);
                    }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
                    continue;
                }

                /* This junction was NOT previously visited in the current
                 * search path, so add the junction to the history. */
                _peepPathFindHistory[_peepPathFindNumJunctions].location.x = (uint8)(x >> 5);
                _peepPathFindHistory[_peepPathFindNumJunctions].location.y = (uint8)(y >> 5);
                _peepPathFindHistory[_peepPathFindNumJunctions].location.z = z;
                // .direction take is added below.

                _peepPathFindNumJunctions--;
            }
        }

        /* Continue searching down each remaining edge of the path
         * (recursive call). */
        do
        {
            edges &= ~(1 << next_test_edge);
            uint8 savedNumJunctions = _peepPathFindNumJunctions;

            uint8 height = z;
            if (footpath_element_is_sloped(tileElement) && footpath_element_get_slope_direction(tileElement) == next_test_edge)
            {
                height += 2;
            }
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                if (searchResult == PATH_SEARCH_JUNCTION)
                {
                    if (thin_junction)
                        log_info("[%03d] Recurse from %d,%d,%d edge: %d; Thin-Junction", counter, x >> 5, y >> 5, z,
                                 next_test_edge);
                    else
                        log_info("[%03d] Recurse from %d,%d,%d edge: %d; Wide-Junction", counter, x >> 5, y >> 5, z,
                                 next_test_edge);
                }
                else
                {
                    log_info("[%03d] Recurse from %d,%d,%d edge: %d; Segment", counter, x >> 5, y >> 5, z, next_test_edge);
                }
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2

            if (thin_junction)
            {
                /* Add the current test_edge to the history. */
                _peepPathFindHistory[_peepPathFindNumJunctions + 1].direction = next_test_edge;
            }

            peep_pathfind_heuristic_search(x, y, height, peep, tileElement, nextInPatrolArea, counter, endScore, next_test_edge,
                                           endJunctions, junctionList, directionList, endXYZ, endSteps);
            _peepPathFindNumJunctions = savedNumJunctions;

#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_info("[%03d] Returned to %d,%d,%d edge: %d; Score: %d", counter, x >> 5, y >> 5, z, next_test_edge,
                         *endScore);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        } while ((next_test_edge = bitscanforward(edges)) != -1);

    } while (!tile_element_is_last_for_tile(tileElement++));

    if (!found)
    {
/* No map element could be found.
 * Return without updating the parameters (best result so far). */
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug)
        {
            log_info("[%03d] Returning from %d,%d,%d; No relevant map element found", counter, x >> 5, y >> 5, z);
        }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
    }
    else
    {
#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
        if (gPathFindDebug)
        {
            log_info("[%03d] Returning from %d,%d,%d; All map elements checked", counter, x >> 5, y >> 5, z);
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
sint32 peep_pathfind_choose_direction(sint16 x, sint16 y, uint8 z, rct_peep * peep)
{
    // The max number of thin junctions searched - a per-search-path limit.
    _peepPathFindMaxJunctions = peep_pathfind_get_max_number_junctions(peep);

    /* The max number of tiles to check - a whole-search limit.
     * Mainly to limit the performance impact of the path finding. */
    sint32 maxTilesChecked = (peep->type == PEEP_TYPE_STAFF) ? 50000 : 15000;
    // Used to allow walking through no entry banners
    _peepPathFindIsStaff = (peep->type == PEEP_TYPE_STAFF);

    TileCoordsXYZ goal = { (uint8)(gPeepPathFindGoalPosition.x >> 5),
                      (uint8)(gPeepPathFindGoalPosition.y >> 5),
                      (uint8)(gPeepPathFindGoalPosition.z) };

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    if (gPathFindDebug)
    {
        log_verbose("Choose direction for %s for goal %d,%d,%d from %d,%d,%d", gPathFindDebugPeepName, goal.x, goal.y, goal.z,
                    x >> 5, y >> 5, z);
    }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    // Get the path element at this location
    rct_tile_element * dest_tile_element = map_get_first_element_at(x / 32, y / 32);
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
    rct_tile_element * first_tile_element = nullptr;

    bool  found           = false;
    uint8 permitted_edges = 0;
    bool  isThin          = false;
    do
    {
        if (dest_tile_element->base_height != z)
            continue;
        if (tile_element_get_type(dest_tile_element) != TILE_ELEMENT_TYPE_PATH)
            continue;
        found = true;
        if (first_tile_element == nullptr)
        {
            first_tile_element = dest_tile_element;
        }

        /* Check if this path element is a thin junction.
         * Only 'thin' junctions are remembered in peep->pathfind_history.
         * NO attempt is made to merge the overlaid path elements and
         * check if the combination is 'thin'!
         * The junction is considered 'thin' simply if any of the
         * overlaid path elements there is a 'thin junction'. */
        isThin = isThin || path_is_thin_junction(dest_tile_element, x, y, z);

        // Collect the permitted edges of ALL matching path elements at this location.
        permitted_edges |= path_get_permitted_edges(dest_tile_element);
    } while (!tile_element_is_last_for_tile(dest_tile_element++));
    // Peep is not on a path.
    if (!found)
        return -1;

    permitted_edges &= 0xF;
    uint8 edges = permitted_edges;
    if (isThin && peep->pathfind_goal.x == goal.x && peep->pathfind_goal.y == goal.y && peep->pathfind_goal.z == goal.z)
    {
        /* Use of peep->pathfind_history[]:
         * When walking to a goal, the peep pathfind_history stores
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
        for (auto &pathfindHistory : peep->pathfind_history)
        {
            if (pathfindHistory.x == x / 32 && pathfindHistory.y == y / 32 &&
                pathfindHistory.z == z)
            {

                /* Fix broken pathfind_history[i].direction
                 * which have untried directions that are not
                 * currently possible - could be due to pathing
                 * changes or in earlier code .directions was
                 * initialised to 0xF rather than the permitted
                 * edges. */
                pathfindHistory.direction &= permitted_edges;

                edges = pathfindHistory.direction;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                if (gPathFindDebug)
                {
                    log_verbose("Getting untried edges from pf_history for %d,%d,%d:  %s,%s,%s,%s", x >> 5, y >> 5, z,
                                (edges & 1) ? "0" : "-", (edges & 2) ? "1" : "-", (edges & 4) ? "2" : "-",
                                (edges & 8) ? "3" : "-");
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
                    edges                     = pathfindHistory.direction;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                    if (gPathFindDebug)
                    {
                        log_verbose("All edges tried for %d,%d,%d - resetting to all untried", x >> 5, y >> 5, z);
                    }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                }
                break;
            }
        }
    }

    /* If this is a new goal for the peep. Store it and reset the peep's
     * pathfind_history. */
    if (peep->pathfind_goal.direction > 3 || peep->pathfind_goal.x != goal.x || peep->pathfind_goal.y != goal.y ||
        peep->pathfind_goal.z != goal.z)
    {
        peep->pathfind_goal.x         = goal.x;
        peep->pathfind_goal.y         = goal.y;
        peep->pathfind_goal.z         = goal.z;
        peep->pathfind_goal.direction = 0;

        // Clear pathfinding history
        memset(peep->pathfind_history, 0xFF, sizeof(peep->pathfind_history));
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug)
        {
            log_verbose("New goal; clearing pf_history.");
        }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    }

    // Peep has tried all edges.
    if (edges == 0)
        return -1;

    sint32 chosen_edge = bitscanforward(edges);

    // Peep has multiple edges still to try.
    if (edges & ~(1 << chosen_edge))
    {
        uint16 best_score = 0xFFFF;
        uint8  best_sub   = 0xFF;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        uint8         bestJunctions = 0;
        TileCoordsXYZ bestJunctionList[16];
        uint8         bestDirectionList[16];
        TileCoordsXYZ bestXYZ;

        if (gPathFindDebug)
        {
            log_verbose("Pathfind start for goal %d,%d,%d from %d,%d,%d", goal.x, goal.y, goal.z, x >> 5, y >> 5, z);
        }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

        /* Call the search heuristic on each edge, keeping track of the
         * edge that gives the best (i.e. smallest) value (best_score)
         * or for different edges with equal value, the edge with the
         * least steps (best_sub). */
        sint32 numEdges = bitcount(edges);
        for (sint32 test_edge = chosen_edge; test_edge != -1; test_edge = bitscanforward(edges))
        {
            edges &= ~(1 << test_edge);
            uint8 height = z;

            if (footpath_element_is_sloped(first_tile_element) &&
                footpath_element_get_slope_direction(first_tile_element) == test_edge)
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
            memset(_peepPathFindHistory, 0xFF, sizeof(_peepPathFindHistory));

            /* The pathfinding will only use elements
             * 1.._peepPathFindMaxJunctions, so the starting point
             * is placed in element 0 */
            _peepPathFindHistory[0].location.x = (uint8)(x >> 5);
            _peepPathFindHistory[0].location.y = (uint8)(y >> 5);
            _peepPathFindHistory[0].location.z = z;
            _peepPathFindHistory[0].direction  = 0xF;

            uint16 score = 0xFFFF;
            /* Variable endXYZ contains the end location of the
             * search path. */
            TileCoordsXYZ endXYZ;
            endXYZ.x = 0;
            endXYZ.y = 0;
            endXYZ.z = 0;

            uint8 endSteps = 255;

            /* Variable endJunctions is the number of junctions
             * passed through in the search path.
             * Variables endJunctionList and endDirectionList
             * contain the junctions and corresponding directions
             * of the search path.
             * In the future these could be used to visualise the
             * pathfinding on the map. */
            uint8          endJunctions         = 0;
            TileCoordsXYZ endJunctionList[16];
            uint8          endDirectionList[16] = { 0 };

            bool inPatrolArea = false;
            if (peep->type == PEEP_TYPE_STAFF && peep->staff_type == STAFF_TYPE_MECHANIC)
            {
                /* Mechanics are the only staff type that
                 * pathfind to a destination. Determine if the
                 * mechanic is in their patrol area. */
                inPatrolArea = staff_is_location_in_patrol(peep, peep->next_x, peep->next_y);
            }

#if defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2
            if (gPathFindDebug)
            {
                log_verbose("Pathfind searching in direction: %d from %d,%d,%d", test_edge, x >> 5, y >> 5, z);
            }
#endif // defined(DEBUG_LEVEL_2) && DEBUG_LEVEL_2

            peep_pathfind_heuristic_search(x, y, height, peep, first_tile_element, inPatrolArea, 0, &score, test_edge,
                                           &endJunctions, endJunctionList, endDirectionList, &endXYZ, &endSteps);

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
            if (gPathFindDebug)
            {
                log_verbose("Pathfind test edge: %d score: %d steps: %d end: %d,%d,%d junctions: %d", test_edge, score,
                            endSteps, endXYZ.x, endXYZ.y, endXYZ.z, endJunctions);
                for (uint8 listIdx = 0; listIdx < endJunctions; listIdx++)
                {
                    log_info("Junction#%d %d,%d,%d Direction %d", listIdx + 1, endJunctionList[listIdx].x,
                             endJunctionList[listIdx].y, endJunctionList[listIdx].z, endDirectionList[listIdx]);
                }
            }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

            if (score < best_score || (score == best_score && endSteps < best_sub))
            {
                chosen_edge = test_edge;
                best_score  = score;
                best_sub    = endSteps;
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                bestJunctions = endJunctions;
                for (uint8 index = 0; index < endJunctions; index++)
                {
                    bestJunctionList[index].x = endJunctionList[index].x;
                    bestJunctionList[index].y = endJunctionList[index].y;
                    bestJunctionList[index].z = endJunctionList[index].z;
                    bestDirectionList[index]  = endDirectionList[index];
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
            if (gPathFindDebug)
            {
                log_verbose("Pathfind heuristic search failed.");
            }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
            return -1;
        }
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug)
        {
            log_verbose("Pathfind best edge %d with score %d steps %d", chosen_edge, best_score, best_sub);
            for (uint8 listIdx = 0; listIdx < bestJunctions; listIdx++)
            {
                log_verbose("Junction#%d %d,%d,%d Direction %d", listIdx + 1, bestJunctionList[listIdx].x,
                            bestJunctionList[listIdx].y, bestJunctionList[listIdx].z, bestDirectionList[listIdx]);
            }
            log_verbose("End at %d,%d,%d", bestXYZ.x, bestXYZ.y, bestXYZ.z);
        }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    }

    if (isThin)
    {
        for (sint32 i = 0; i < 4; ++i)
        {
            if (peep->pathfind_history[i].x == x >> 5 && peep->pathfind_history[i].y == y >> 5 &&
                peep->pathfind_history[i].z == z)
            {
                /* Peep remembers this junction, so remove the
                 * chosen_edge from those left to try. */
                peep->pathfind_history[i].direction &= ~(1 << chosen_edge);
                /* Also remove the edge through which the peep
                 * entered the junction from those left to try. */
                peep->pathfind_history[i].direction &= ~(1 << (peep->direction ^ 2));
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                if (gPathFindDebug)
                {
                    log_verbose(
                        "Updating existing pf_history (in index: %d) for %d,%d,%d without entry edge %d & exit edge %d.", i,
                        x >> 5, y >> 5, z, peep->direction ^ 2, chosen_edge);
                }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
                return chosen_edge;
            }
        }

        /* Peep does not remember this junction, so forget a junction
         * and remember this junction. */
        sint32 i = peep->pathfind_goal.direction++;
        peep->pathfind_goal.direction &= 3;
        peep->pathfind_history[i].x         = x >> 5;
        peep->pathfind_history[i].y         = y >> 5;
        peep->pathfind_history[i].z         = z;
        peep->pathfind_history[i].direction = permitted_edges;
        /* Remove the chosen_edge from those left to try. */
        peep->pathfind_history[i].direction &= ~(1 << chosen_edge);
        /* Also remove the edge through which the peep
         * entered the junction from those left to try. */
        peep->pathfind_history[i].direction &= ~(1 << (peep->direction ^ 2));
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug)
        {
            log_verbose("Storing new pf_history (in index: %d) for %d,%d,%d without entry edge %d & exit edge %d.", i, x >> 5,
                        y >> 5, z, peep->direction ^ 2, chosen_edge);
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
static uint8 get_nearest_park_entrance_index(uint16 x, uint16 y)
{
    uint8  chosenEntrance = 0xFF;
    uint16 nearestDist    = 0xFFFF;
    for (uint8 i = 0; i < MAX_PARK_ENTRANCES; i++)
    {
        if (gParkEntrances[i].x == LOCATION_NULL)
            continue;

        uint16 dist = abs(gParkEntrances[i].x - x) + abs(gParkEntrances[i].y - y);

        if (dist >= nearestDist)
            continue;

        nearestDist    = dist;
        chosenEntrance = i;
    }
    return chosenEntrance;
}

/**
 *
 *  rct2: 0x006952C0
 */
static sint32 guest_path_find_entering_park(rct_peep * peep, rct_tile_element * tile_element, uint8 edges)
{
    // Send peeps to the nearest park entrance.
    uint8 chosenEntrance = get_nearest_park_entrance_index(peep->next_x, peep->next_y);

    // If no defined park entrances are found, walk aimlessly.
    if (chosenEntrance == 0xFF)
        return guest_path_find_aimless(peep, edges);

    sint16 x = gParkEntrances[chosenEntrance].x;
    sint16 y = gParkEntrances[chosenEntrance].y;
    sint16 z = gParkEntrances[chosenEntrance].z;

    gPeepPathFindGoalPosition        = { x, y, static_cast<sint16>(z >> 3) };
    gPeepPathFindIgnoreForeignQueues = true;
    gPeepPathFindQueueRideIndex      = 255;

    sint32 chosenDirection = peep_pathfind_choose_direction(peep->next_x, peep->next_y, peep->next_z, peep);

    if (chosenDirection == -1)
        return guest_path_find_aimless(peep, edges);
    else
        return peep_move_one_tile(chosenDirection, peep);
}

/**
 * Gets the nearest peep spawn relative to point, by using Manhattan distance.
 * @param x x coordinate of location
 * @param y y coordinate of location
 * @return Index of gPeepSpawns (or 0xFF if no peep spawns exist).
 */
static uint8 get_nearest_peep_spawn_index(uint16 x, uint16 y)
{
    uint8  chosenSpawn = 0xFF;
    uint16 nearestDist = 0xFFFF;
    for (uint8 i = 0; i < MAX_PEEP_SPAWNS; ++i)
    {
        if (gPeepSpawns[i].x == PEEP_SPAWN_UNDEFINED)
            continue;

        uint16 dist = abs(gPeepSpawns[i].x - x) + abs(gPeepSpawns[i].y - y);

        if (dist >= nearestDist)
            continue;

        nearestDist = dist;
        chosenSpawn = i;
    }
    return chosenSpawn;
}

/**
 *
 *  rct2: 0x0069536C
 */
static sint32 guest_path_find_leaving_park(rct_peep * peep, rct_tile_element * tile_element, uint8 edges)
{
    // Send peeps to the nearest spawn point.
    uint8 chosenSpawn = get_nearest_peep_spawn_index(peep->next_x, peep->next_y);

    // If no defined spawns were found, walk aimlessly.
    if (chosenSpawn == 0xFF)
        return guest_path_find_aimless(peep, edges);

    PeepSpawn * peepSpawn = &gPeepSpawns[chosenSpawn];

    sint16 x         = peepSpawn->x & 0xFFE0;
    sint16 y         = peepSpawn->y & 0xFFE0;
    uint8  z         = peepSpawn->z / 8;
    uint8  direction = peepSpawn->direction;

    gPeepPathFindGoalPosition = { x, y, z };
    if (x == peep->next_x && y == peep->next_y)
    {
        return peep_move_one_tile(direction, peep);
    }

    gPeepPathFindIgnoreForeignQueues = true;
    gPeepPathFindQueueRideIndex      = 255;
    direction                        = peep_pathfind_choose_direction(peep->next_x, peep->next_y, peep->next_z, peep);
    if (direction == 0xFF)
        return guest_path_find_aimless(peep, edges);
    else
        return peep_move_one_tile(direction, peep);
}

/**
 *
 *  rct2: 0x00695161
 */
static sint32 guest_path_find_park_entrance(rct_peep * peep, rct_tile_element * tile_element, uint8 edges)
{
    uint8 entranceNum;

    // Resolves already-corrupt guests (e.g. loaded from save)
    if (peep->peep_flags & PEEP_FLAGS_PARK_ENTRANCE_CHOSEN &&
        (peep->current_ride >= 4 || gParkEntrances[peep->current_ride].x == LOCATION_NULL))
    {
        peep->peep_flags &= ~(PEEP_FLAGS_PARK_ENTRANCE_CHOSEN);
    }

    if (!(peep->peep_flags & PEEP_FLAGS_PARK_ENTRANCE_CHOSEN))
    {
        uint8  chosenEntrance = 0xFF;
        uint16 nearestDist    = 0xFFFF;
        for (entranceNum = 0; entranceNum < MAX_PARK_ENTRANCES; ++entranceNum)
        {
            if (gParkEntrances[entranceNum].x == LOCATION_NULL)
                continue;

            uint16 dist = abs(gParkEntrances[entranceNum].x - peep->next_x) + abs(gParkEntrances[entranceNum].y - peep->next_y);

            if (dist >= nearestDist)
                continue;

            nearestDist    = dist;
            chosenEntrance = entranceNum;
        }

        if (chosenEntrance == 0xFF)
            return guest_path_find_aimless(peep, edges);

        peep->current_ride = chosenEntrance;
        peep->peep_flags |= PEEP_FLAGS_PARK_ENTRANCE_CHOSEN;
    }

    entranceNum = peep->current_ride;
    sint16 x    = gParkEntrances[entranceNum].x;
    sint16 y    = gParkEntrances[entranceNum].y;
    sint16 z    = gParkEntrances[entranceNum].z;

    gPeepPathFindGoalPosition        = { x, y, static_cast<sint16>(z >> 3) };
    gPeepPathFindIgnoreForeignQueues = true;
    gPeepPathFindQueueRideIndex      = 255;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    pathfind_logging_enable(peep);
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    sint32 chosenDirection = peep_pathfind_choose_direction(peep->next_x, peep->next_y, peep->next_z, peep);

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    pathfind_logging_disable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    if (chosenDirection == -1)
        return guest_path_find_aimless(peep, edges);
    else
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
static void get_ride_queue_end(sint16 * x, sint16 * y, sint16 * z)
{
    LocationXY16      result     = { 0, 0 };
    rct_tile_element * tileElement = map_get_first_element_at(*x / 32, *y / 32);

    if (tileElement == nullptr)
    {
        return;
    }

    bool found = false;
    do
    {
        if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_ENTRANCE)
            continue;

        if (*z != tileElement->base_height)
            continue;

        found = true;
        break;
    } while (!tile_element_is_last_for_tile(tileElement++));

    if (!found)
        return;

    uint8             direction        = tile_element_get_direction_with_offset(tileElement, 2);
    rct_tile_element * lastPathElement  = nullptr;
    rct_tile_element * firstPathElement = nullptr;

    sint16 baseZ = tileElement->base_height;
    sint16 nextX = *x;
    sint16 nextY = *y;
    while (true)
    {
        if (tile_element_get_type(tileElement) == TILE_ELEMENT_TYPE_PATH)
        {
            lastPathElement = tileElement;
            result.x        = nextX;
            result.y        = nextY;
            // result.direction = direction;
            if (footpath_element_is_sloped(tileElement))
            {
                if (footpath_element_get_slope_direction(tileElement) == direction)
                {
                    baseZ += 2;
                }
            }
        }
        nextX += TileDirectionDelta[direction].x;
        nextY += TileDirectionDelta[direction].y;

        tileElement = map_get_first_element_at(nextX / 32, nextY / 32);
        found      = false;
        do
        {
            if (tileElement == firstPathElement)
                continue;

            if (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_PATH)
                continue;

            if (baseZ == tileElement->base_height)
            {
                if (footpath_element_is_sloped(tileElement))
                {
                    if (footpath_element_get_slope_direction(tileElement) != direction)
                    {
                        break;
                    }
                }
                found = true;
                break;
            }

            if (baseZ - 2 == tileElement->base_height)
            {
                if (!footpath_element_is_sloped(tileElement))
                    break;

                if (footpath_element_get_slope_direction(tileElement) != (direction ^ 2))
                    break;

                baseZ -= 2;
                found = true;
                break;
            }
        } while (!tile_element_is_last_for_tile(tileElement++));

        if (found == false)
            break;

        if (!footpath_element_is_queue(tileElement))
            break;

        if (!(tileElement->properties.path.edges & (1 << (direction ^ (1 << 1)))))
            break;

        if (firstPathElement == nullptr)
            firstPathElement = tileElement;

        // More queue to go.
        if (tileElement->properties.path.edges & (1 << (direction)))
            continue;

        direction++;
        direction &= 3;
        // More queue to go.
        if (tileElement->properties.path.edges & (1 << (direction)))
            continue;

        direction ^= (1 << 1);
        // More queue to go.
        if (tileElement->properties.path.edges & (1 << (direction)))
            continue;

        break;
    }

    if ((uint8)*z == 0xFF)
        return;

    tileElement = lastPathElement;
    if (tileElement == nullptr)
        return;

    if (!footpath_element_is_queue(tileElement))
        return;

    *x = result.x;
    *y = result.y;
    *z = tileElement->base_height;
}

/**
 *
 *  rct2: 0x00694C35
 */
static sint32 guest_path_finding(rct_peep * peep)
{
    sint16 x, y, z;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    pathfind_logging_enable(peep);
    if (gPathFindDebug)
    {
        log_info("Starting guest_path_finding for %s", gPathFindDebugPeepName);
    }
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

    if (peep->next_var_29 & 0x18)
    {
        return guest_surface_path_finding(peep);
    }

    x = peep->next_x;
    y = peep->next_y;
    z = peep->next_z;

    rct_tile_element * tileElement = map_get_path_element_at(x / 32, y / 32, z);
    if (tileElement == nullptr)
    {
        return 1;
    }

    _peepPathFindIsStaff = false;
    uint8 edges          = path_get_permitted_edges(tileElement);

    if (edges == 0)
    {
        return guest_surface_path_finding(peep);
    }

    if (peep->outside_of_park == 0 && peep_heading_for_ride_or_park_exit(peep))
    {
        /* If this tileElement is adjacent to any non-wide paths,
         * remove all of the edges to wide paths. */
        uint8 adjustedEdges = edges;
        for (sint32 chosenDirection = 0; chosenDirection < 4; chosenDirection++)
        {
            // If there is no path in that direction try another
            if (!(adjustedEdges & (1 << chosenDirection)))
                continue;

            /* If there is a wide path in that direction,
                remove that edge and try another */
            if (footpath_element_next_in_direction(peep->next_x, peep->next_y, peep->next_z, tileElement, chosenDirection) ==
                PATH_SEARCH_WIDE)
            {
                adjustedEdges &= ~(1 << chosenDirection);
            }
        }
        if (adjustedEdges != 0)
            edges = adjustedEdges;
    }

    sint8 direction = peep->direction ^ (1 << 1);
    // Check if in a dead end (i.e. only edge is where the peep came from)
    if (!(edges & ~(1 << direction)))
    {
        // In a dead end.  Check if peep is lost, etc.
        peep->CheckIfLost();
        peep->CheckCantFindRide();
        peep->CheckCantFindExit();
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
        if (gPathFindDebug)
        {
            log_info("Completed guest_path_finding for %s - taking only direction available: %d.", gPathFindDebugPeepName,
                     direction);
        }
        pathfind_logging_disable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        return peep_move_one_tile(direction, peep);
    }

    // Peep still has multiple edges to choose from.

    // Peep is outside the park.
    // loc_694F19:
    if (peep->outside_of_park != 0)
    {
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug)
        {
            log_info("Completed guest_path_finding for %s - peep is outside the park.", gPathFindDebugPeepName);
        }
        pathfind_logging_disable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        switch (peep->state)
        {
        case PEEP_STATE_ENTERING_PARK:
            return guest_path_find_entering_park(peep, tileElement, edges);
        case PEEP_STATE_LEAVING_PARK:
            return guest_path_find_leaving_park(peep, tileElement, edges);
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
    if (!peep->HasFood() && (scenario_rand() & 0xFFFF) >= 2184)
    {
        uint8 adjustedEdges = edges;
        for (sint32 chosenDirection = 0; chosenDirection < 4; chosenDirection++)
        {
            // If there is no path in that direction try another
            if (!(adjustedEdges & (1 << chosenDirection)))
                continue;

            uint8 rideIndex, pathSearchResult;
            pathSearchResult = footpath_element_destination_in_direction(peep->next_x, peep->next_y, peep->next_z, tileElement,
                                                                         chosenDirection, &rideIndex);
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
    if (peep->item_standard_flags & PEEP_ITEM_MAP)
    {
        // If at least 2 directions consult map
        if (bitcount(edges) >= 2)
        {
            uint16 probability = 1638;
            if (peep_heading_for_ride_or_park_exit(peep))
            {
                probability = 9362;
            }
            if ((scenario_rand() & 0xFFFF) < probability)
            {
                peep->ReadMap();
            }
        }
    }

    if (peep->peep_flags & PEEP_FLAGS_LEAVING_PARK)
    {
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug)
        {
            log_info("Completed guest_path_finding for %s - peep is leaving the park.", gPathFindDebugPeepName);
        }
        pathfind_logging_disable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        return guest_path_find_park_entrance(peep, tileElement, edges);
    }

    if (peep->guest_heading_to_ride_id == 0xFF)
    {
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug)
        {
            log_info("Completed guest_path_finding for %s - peep is aimless.", gPathFindDebugPeepName);
        }
        pathfind_logging_disable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        return guest_path_find_aimless(peep, edges);
    }

    // Peep is heading for a ride.
    uint8  rideIndex = peep->guest_heading_to_ride_id;
    Ride * ride      = get_ride(rideIndex);

    if (ride->status != RIDE_STATUS_OPEN)
    {
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug)
        {
            log_info("Completed guest_path_finding for %s - peep is heading to closed ride == aimless.",
                     gPathFindDebugPeepName);
        }
        pathfind_logging_disable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        return guest_path_find_aimless(peep, edges);
    }

    // The ride is open.
    gPeepPathFindQueueRideIndex = rideIndex;

    /* Find the ride's closest entrance station to the peep.
     * At the same time, count how many entrance stations there are and
     * which stations are entrance stations. */
    uint16 closestDist       = 0xFFFF;
    uint8  closestStationNum = 0;

    sint32 numEntranceStations = 0;
    uint8  entranceStations    = 0;

    for (uint8 stationNum = 0; stationNum < MAX_STATIONS; ++stationNum)
    {
        // Skip if stationNum has no entrance (so presumably an exit only station)
        if (ride_get_entrance_location(rideIndex, stationNum).isNull())
            continue;

        numEntranceStations++;
        entranceStations |= (1 << stationNum);

        TileCoordsXYZD entranceLocation = ride_get_entrance_location(rideIndex, stationNum);

        sint16 stationX = (sint16)(entranceLocation.x * 32);
        sint16 stationY = (sint16)(entranceLocation.y * 32);
        uint16 dist     = abs(stationX - peep->next_x) + abs(stationY - peep->next_y);

        if (dist < closestDist)
        {
            closestDist       = dist;
            closestStationNum = stationNum;
            continue;
        }
    }

    // Ride has no stations with an entrance, so head to station 0.
    if (numEntranceStations == 0)
        closestStationNum = 0;

    /* If a ride has multiple entrance stations and is set to sync with
     * adjacent stations, cycle through the entrance stations (based on
     * number of rides the peep has been on) so the peep will try the
     * different sections of the ride.
     * In this case, the ride's various entrance stations will typically,
     * though not necessarily, be adjacent to one another and consequently
     * not too far for the peep to walk when cycling between them.
     * Note: the same choice of station must made while the peep navigates
     * to the station. Consequently a random station selection here is not
     * appropriate. */
    if (numEntranceStations > 1 && (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS))
    {
        sint32 select = peep->no_of_rides % numEntranceStations;
        while (select > 0)
        {
            closestStationNum = bitscanforward(entranceStations);
            entranceStations &= ~(1 << closestStationNum);
            select--;
        }
        closestStationNum = bitscanforward(entranceStations);
    }

    if (numEntranceStations == 0)
    {
        // closestStationNum is always 0 here.
        LocationXY8 entranceXY = ride->station_starts[closestStationNum];
        x = entranceXY.x * 32;
        y = entranceXY.y * 32;
        z = ride->station_heights[closestStationNum];
    }
    else
    {
        TileCoordsXYZD entranceXYZD = ride_get_entrance_location(rideIndex, closestStationNum);
        x = entranceXYZD.x * 32;
        y = entranceXYZD.y * 32;
        z = entranceXYZD.z;
    }

    get_ride_queue_end(&x, &y, &z);

    gPeepPathFindGoalPosition        = { x, y, z };
    gPeepPathFindIgnoreForeignQueues = true;

    direction = peep_pathfind_choose_direction(peep->next_x, peep->next_y, peep->next_z, peep);

    if (direction == -1)
    {
        /* Heuristic search failed for all directions.
         * Reset the pathfind_goal - this means that the pathfind_history
         * will be reset in the next call to peep_pathfind_choose_direction().
         * This lets the heuristic search "try again" in case the player has
         * edited the path layout or the mechanic was already stuck in the
         * save game (e.g. with a worse version of the pathfinding). */
        peep_reset_pathfind_goal(peep);

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        if (gPathFindDebug)
        {
            log_info("Completed guest_path_finding for %s - failed to choose a direction == aimless.", gPathFindDebugPeepName);
        }
        pathfind_logging_disable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

        return guest_path_find_aimless(peep, edges);
    }
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    if (gPathFindDebug)
    {
        log_info("Completed guest_path_finding for %s - direction chosen: %d.", gPathFindDebugPeepName, direction);
    }
    pathfind_logging_disable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    return peep_move_one_tile(direction, peep);
}
