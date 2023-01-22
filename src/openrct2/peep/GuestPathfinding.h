/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../ride/RideTypes.h"
#include "../world/Location.hpp"

#include <memory>
namespace OpenRCT2
{
    struct Peep;
    struct Guest;
    struct TileElement;

    // The tile position of the place the peep is trying to get to (park entrance/exit, ride
    // entrance/exit, or the end of the queue line for a ride).
    //
    // This gets copied into Peep::PathfindGoal. The two separate variables are needed because
    // when the goal changes the peep's pathfind history needs to be reset.
    extern TileCoordsXYZ gPeepPathFindGoalPosition;

    // When the heuristic pathfinder is examining neighboring tiles, one possibility is that it finds a
    // queue tile; furthermore, this queue tile may or may not be for the ride that the peep is trying
    // to get to, if any. This first var is used to store the ride that the peep is currently headed to.
    extern RideId gPeepPathFindQueueRideIndex;

    // Furthermore, staff members don't care about this stuff; even if they are e.g. a mechanic headed
    // to a particular ride, they have no issues with walking over queues for other rides to get there.
    // This bool controls that behaviour - if true, the peep will not path over queues for rides other
    // than their target ride, and if false, they will treat it like a regular path.
    //
    // In practice, if this is false, gPeepPathFindQueueRideIndex is always RIDE_ID_NULL.
    extern bool gPeepPathFindIgnoreForeignQueues;

    class GuestPathfinding
    {
    public:
        virtual ~GuestPathfinding() = default;

        /**
         * Given a peep 'peep' at tile 'loc', who is trying to get to 'gPeepPathFindGoalPosition', decide the direction the peep
         * should walk in from the current tile.
         *
         * @param loc Reference to the peep's current tile location
         * @param peep Reference to the current peep struct
         * @return The direction the peep should walk in
         */
        virtual Direction ChooseDirection(const TileCoordsXYZ& loc, Peep& peep) = 0;

        /**
         * Test whether the given tile can be walked onto, if the peep is currently at height currentZ and
         * moving in direction currentDirection
         *
         * @param tileElement A pointer to the tile that is being tested
         * @param currentZ The height coord the peep is at
         * @param currentDirection The direction the peep is facing in
         * @return True if the given tile can be walked onto, false otherwise
         */
        static bool IsValidPathZAndDirection(TileElement* tileElement, int32_t currentZ, int32_t currentDirection);

        /**
         * Overall guest pathfinding AI. Sets up Peep::DestinationX/DestinationY (which they move to in a
         * straight line, no pathfinding). Called whenever the guest has arrived at their previously set destination.
         *
         * @param peep A reference to a guest struct
         * @returns 0 if the guest has successfully had a new destination set up, nonzero otherwise.
         */
        virtual int32_t CalculateNextDestination(Guest& peep) = 0;
    };

    class OriginalPathfinding final : public GuestPathfinding
    {
    public:
        Direction ChooseDirection(const TileCoordsXYZ& loc, Peep& peep) final override;

        int32_t CalculateNextDestination(Guest& peep) final override;

    private:
        int32_t GuestPathFindParkEntranceEntering(Peep& peep, uint8_t edges);

        int32_t GuestPathFindPeepSpawn(Peep& peep, uint8_t edges);

        int32_t GuestPathFindParkEntranceLeaving(Peep& peep, uint8_t edges);
    };

    extern std::unique_ptr<GuestPathfinding> gGuestPathfinder;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
#    define PATHFIND_DEBUG                                                                                                     \
        0 // Set to 0 to disable pathfinding debugging;
          // Set to 1 to enable pathfinding debugging.

    // When PATHFIND_DEBUG is 1 (nonzero):
    // The following calls configure debug logging for the given peep
    // If they're a guest, pathfinding will be logged if they have PEEP_FLAGS_TRACKING set
    // If they're staff, pathfinding will be logged if their name is "Mechanic Debug"
    void PathfindLoggingEnable(Peep& peep);
    void PathfindLoggingDisable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
} // namespace OpenRCT2
