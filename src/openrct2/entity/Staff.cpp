/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Staff.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../audio/Audio.h"
#include "../core/DataSerialiser.h"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../localisation/StringIds.h"
#include "../object/ObjectManager.h"
#include "../object/PathAdditionEntry.h"
#include "../object/PeepAnimationsObject.h"
#include "../object/SmallSceneryEntry.h"
#include "../paint/tile_element/Paint.TileElement.h"
#include "../peep/GuestPathfinding.h"
#include "../ride/RideData.h"
#include "../ride/TrackIteration.h"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/Scenery.h"
#include "../world/Wall.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"
#include "PatrolArea.h"
#include "Peep.h"

#include <cassert>
#include <iterator>

namespace OpenRCT2
{
    template<>
    bool EntityBase::is<Staff>() const
    {
        return type == EntityType::staff;
    }

    /**
     *
     *  rct2: 0x006C0905
     */
    bool Staff::isLocationInPatrol(const CoordsXY& loc) const
    {
        // Check if location is in the park
        if (!MapIsLocationOwnedOrHasRights(loc))
            return false;

        // Check if staff has patrol area
        if (!hasPatrolArea())
            return true;

        return isPatrolAreaSet(loc);
    }

    // Check whether the location x,y is inside and on the edge of the
    // patrol zone for mechanic.
    bool Staff::isLocationOnPatrolEdge(const CoordsXY& loc) const
    {
        bool onZoneEdge = false;
        for (uint8_t neighbourDir = 0; !onZoneEdge && neighbourDir <= 7; neighbourDir++)
        {
            auto neighbourPos = loc + CoordsDirectionDelta[neighbourDir];
            onZoneEdge = !isLocationInPatrol(neighbourPos);
        }
        return onZoneEdge;
    }

    bool Staff::canIgnoreWideFlag(const CoordsXYZ& staffPos, TileElement* path) const
    {
        /* Wide flags can potentially wall off parts of a staff patrol zone
         * for the heuristic search.
         * This function provide doors through such "walls" by defining
         * the conditions under which staff can ignore the wide path flag. */
        /* Staff can ignore the wide flag on a path on the edge of the patrol
         * zone based on its adjacent tiles that are also in the patrol zone
         * but not on the patrol zone edge:
         * Basic points of interest are:
         * - how many such tiles there are;
         * - whether there are connected paths on those tiles;
         * - whether the connected paths have the wide flag set.
         * If there are no such tiles, the path is a concave corner of
         * the patrol zone and the wide flag can be ignored.
         * If there is one such tile, the path is on a straight side of the
         * patrol zone. If this one tile is either a connected wide path or
         * not a connected path, the wide flag can be ignored.
         * If there are two such tiles, the path is a convex corner of the
         * patrol zone. If at most one of these tiles is a connected path or
         * both of these tiles are connected wide paths, the wide flag can be
         * ignored. */

        if (!isLocationOnPatrolEdge(staffPos))
        {
            return false;
        }

        /* Check the connected adjacent paths that are also inside the patrol
         * zone but are not on the patrol zone edge have the wide flag set. */
        uint8_t total = 0;
        uint8_t pathcount = 0;
        uint8_t widecount = 0;
        for (Direction adjac_dir : kAllDirections)
        {
            auto adjacPos = staffPos + CoordsXYZ{ CoordsDirectionDelta[adjac_dir].x, CoordsDirectionDelta[adjac_dir].y, 0 };

            /* Ignore adjacent tiles outside the patrol zone. */
            if (!isLocationInPatrol(adjacPos))
                continue;

            /* Ignore adjacent tiles on the patrol zone edge. */
            if (isLocationOnPatrolEdge(adjacPos))
                continue;

            /* Adjacent tile is inside the patrol zone but not on the
             * patrol zone edge. */
            total++;

            /* Check if path has an edge in adjac_dir */
            if (!(path->asPath()->getEdges() & (1u << adjac_dir)))
            {
                continue;
            }

            if (path->asPath()->isSloped())
            {
                if (path->asPath()->getSlopeDirection() == adjac_dir)
                {
                    adjacPos.z += kPathHeightStep;
                }
            }

            /* Search through all adjacent map elements */
            TileElement* test_element = MapGetFirstElementAt(adjacPos);
            if (test_element == nullptr)
                return false;
            bool pathfound = false;
            bool widefound = false;
            do
            {
                if (test_element->getType() != TileElementType::path)
                {
                    continue;
                }

                /* test_element is a path */
                const auto* adjacentPathElement = test_element->asPath();
                if (!FootpathIsZAndDirectionValid(*adjacentPathElement, adjacPos.z / kCoordsZStep, adjac_dir))
                    continue;

                /* test_element is a connected path */
                if (!pathfound)
                {
                    pathfound = true;
                    pathcount++;
                }

                if (adjacentPathElement->isWide())
                {
                    if (!widefound)
                    {
                        widefound = true;
                        widecount++;
                    }
                }
            } while (!(test_element++)->isLastForTile());
        }

        switch (total)
        {
            case 0: /* Concave corner */
                return true;
            case 1: /* Straight side */
            case 2: /* Convex corner */
                if (pathcount <= total - 1 || widecount == total)
                {
                    return true;
                }
        }

        return false;
    }

    /**
     *
     *  rct2: 0x006C095B
     *  returns 0xF if not in a valid patrol area
     */
    uint8_t Staff::getValidPatrolDirections(const CoordsXY& loc) const
    {
        uint8_t directions = 0;

        if (isLocationInPatrol({ loc.x - kCoordsXYStep, loc.y }))
        {
            directions |= (1 << 0);
        }

        if (isLocationInPatrol({ loc.x, loc.y + kCoordsXYStep }))
        {
            directions |= (1 << 1);
        }

        if (isLocationInPatrol({ loc.x + kCoordsXYStep, loc.y }))
        {
            directions |= (1 << 2);
        }

        if (isLocationInPatrol({ loc.x, loc.y - kCoordsXYStep }))
        {
            directions |= (1 << 3);
        }

        if (directions == 0)
        {
            directions = 0xF;
        }

        return directions;
    }

    /**
     *
     *  rct2: 0x006C1955
     */
    void Staff::resetStats()
    {
        for (auto peep : EntityList<Staff>())
        {
            peep->setHireDate(GetDate().GetMonthsElapsed());
            peep->staffLawnsMown = 0;
            peep->staffRidesFixed = 0;
            peep->staffGardensWatered = 0;
            peep->staffRidesInspected = 0;
            peep->staffLitterSwept = 0;
            peep->staffVandalsStopped = 0;
            peep->staffBinsEmptied = 0;
            peep->staffGuestsEntertained = 0;
        }
    }

    bool Staff::isPatrolAreaSet(const CoordsXY& coords) const
    {
        if (patrolInfo != nullptr)
        {
            return patrolInfo->get(coords);
        }
        return false;
    }

    void Staff::setPatrolArea(const CoordsXY& coords, bool value)
    {
        if (patrolInfo == nullptr)
        {
            if (value)
            {
                patrolInfo = new PatrolArea();
            }
            else
            {
                return;
            }
        }

        patrolInfo->set(coords, value);
    }

    void Staff::setPatrolArea(const MapRange& range, bool value)
    {
        for (int32_t yy = range.GetY1(); yy <= range.GetY2(); yy += kCoordsXYStep)
        {
            for (int32_t xx = range.GetX1(); xx <= range.GetX2(); xx += kCoordsXYStep)
            {
                setPatrolArea({ xx, yy }, value);
            }
        }
    }

    void Staff::clearPatrolArea()
    {
        delete patrolInfo;
        patrolInfo = nullptr;
    }

    bool Staff::hasPatrolArea() const
    {
        return patrolInfo == nullptr ? false : !patrolInfo->isEmpty();
    }

    /**
     *
     *  rct2: 0x006BFBE8
     *
     * Returns kInvalidDirection when no nearby litter or unpathable litter
     */
    Direction Staff::handymanDirectionToNearestLitter() const
    {
        // Maximum manhattan distance that litter can be for a handyman to seek to it
        constexpr auto kTileRadius = 3;
        constexpr auto kLookupRadius = kCoordsXYStep * kTileRadius;

        auto nearestLitterDist = std::numeric_limits<int32_t>::max();
        Litter* nearestLitter = nullptr;

        for (int32_t tileX = x - kLookupRadius; tileX <= x + kLookupRadius; tileX += kCoordsXYStep)
        {
            for (int32_t tileY = y - kLookupRadius; tileY <= y + kLookupRadius; tileY += kCoordsXYStep)
            {
                for (auto* litter : EntityTileList<Litter>({ tileX, tileY }))
                {
                    // Calculate Manhattan distance with z-weighting
                    uint16_t distance = std::abs(litter->x - x) + std::abs(litter->y - y) + std::abs(litter->z - z) * 4;

                    if (distance < nearestLitterDist)
                    {
                        nearestLitterDist = distance;
                        nearestLitter = litter;
                    }
                }
            }
        }

        if (nearestLitterDist > kLookupRadius || nearestLitter == nullptr)
        {
            return kInvalidDirection;
        }

        auto litterTile = CoordsXY{ nearestLitter->x, nearestLitter->y }.ToTileStart();

        if (!isLocationInPatrol(litterTile))
        {
            return kInvalidDirection;
        }

        Direction nextDirection = DirectionFromTo(CoordsXY(x, y), litterTile.ToTileCentre());

        CoordsXY nextTile = litterTile.ToTileStart() - CoordsDirectionDelta[nextDirection];

        int16_t nextZ = ((z + kCoordsZStep) & 0xFFF0) / kCoordsZStep;

        TileElement* tileElement = MapGetFirstElementAt(nextTile);
        if (tileElement == nullptr)
            return kInvalidDirection;
        do
        {
            if (tileElement->baseHeight != nextZ)
                continue;
            if (tileElement->getType() == TileElementType::entrance || tileElement->getType() == TileElementType::track)
            {
                return kInvalidDirection;
            }
        } while (!(tileElement++)->isLastForTile());

        nextTile = CoordsXY(x, y).ToTileStart() + CoordsDirectionDelta[nextDirection];

        tileElement = MapGetFirstElementAt(nextTile);
        if (tileElement == nullptr)
            return kInvalidDirection;

        do
        {
            if (tileElement->baseHeight != nextZ)
                continue;
            if (tileElement->getType() == TileElementType::entrance || tileElement->getType() == TileElementType::track)
            {
                return kInvalidDirection;
            }
        } while (!(tileElement++)->isLastForTile());

        return nextDirection;
    }

    static bool isHandymanAlreadyServicingTile(const CoordsXY& tile, PeepState state)
    {
        if (state == PeepState::watering)
        {
            for (auto* staff : EntityList<Staff>())
            {
                if (staff->state == PeepState::watering
                    && CoordsXY{ staff->nextLoc } + CoordsDirectionDelta[staff->var37] == tile)
                    return true;
            }
        }
        else
        {
            for (auto* staff : EntityTileList<Staff>(tile))
            {
                if (staff->state == state)
                    return true;
            }
        }
        return false;
    }

    /**
     *
     *  rct2: 0x006BF931
     */
    uint8_t Staff::handymanDirectionToUncutGrass(uint8_t valid_directions) const
    {
        if (!(getNextIsSurface()))
        {
            auto surfaceElement = MapGetSurfaceElementAt(nextLoc);
            if (surfaceElement == nullptr)
                return kInvalidDirection;

            if (nextLoc.z != surfaceElement->getBaseZ())
                return kInvalidDirection;

            if (getNextIsSloped())
            {
                if (surfaceElement->getSlope() != kPathSlopeToLandSlope[getNextDirection()])
                    return kInvalidDirection;
            }
            else if (surfaceElement->getSlope() != kTileSlopeFlat)
                return kInvalidDirection;
        }

        uint8_t chosenDirection = ScenarioRand() & 0x3;
        for (uint8_t i = 0; i < 4; ++i, ++chosenDirection)
        {
            chosenDirection &= 0x3;

            if (!(valid_directions & (1 << chosenDirection)))
            {
                continue;
            }

            CoordsXY chosenTile = CoordsXY{ nextLoc } + CoordsDirectionDelta[chosenDirection];

            if (!MapIsLocationValid(chosenTile))
                continue;

            auto surfaceElement = MapGetSurfaceElementAt(chosenTile);
            if (surfaceElement != nullptr)
            {
                if (std::abs(surfaceElement->getBaseZ() - nextLoc.z) <= 2 * kCoordsZStep)
                {
                    if (surfaceElement->canGrassGrow() && (surfaceElement->getGrassLength() & 0x7) >= GRASS_LENGTH_CLEAR_1)
                    {
                        if (!isHandymanAlreadyServicingTile(chosenTile, PeepState::mowing))
                            return chosenDirection;
                    }
                }
            }
        }
        return kInvalidDirection;
    }

    /**
     *
     *  rct2: 0x006BFD9C
     */
    Direction Staff::handymanDirectionRandSurface(uint8_t validDirections) const
    {
        Direction newDirection = ScenarioRand() % kNumOrthogonalDirections;
        for (int32_t i = 0; i < kNumOrthogonalDirections; ++i, ++newDirection)
        {
            newDirection %= kNumOrthogonalDirections;
            if (!(validDirections & (1 << newDirection)))
                continue;

            CoordsXY chosenTile = CoordsXY{ nextLoc } + CoordsDirectionDelta[newDirection];

            if (MapSurfaceIsBlocked(chosenTile))
                continue;

            break;
        }
        // If it tries all directions this is required
        // to make it back to the first direction and
        // override validDirections
        newDirection %= kNumOrthogonalDirections;
        return newDirection;
    }

    /**
     *
     *  rct2: 0x006BFBA8
     */
    bool Staff::doHandymanPathFinding()
    {
        staffMowingTimeout++;

        Direction litterDirection = kInvalidDirection;
        uint8_t validDirections = getValidPatrolDirections(nextLoc);

        if ((staffOrders & STAFF_ORDERS_SWEEPING) && ((getGameState().currentTicks + id.ToUnderlying()) & 0xFFF) > 110)
        {
            litterDirection = handymanDirectionToNearestLitter();
        }

        Direction newDirection = kInvalidDirection;
        if (litterDirection == kInvalidDirection && (staffOrders & STAFF_ORDERS_MOWING) && staffMowingTimeout >= 12)
        {
            newDirection = handymanDirectionToUncutGrass(validDirections);
        }

        if (newDirection == kInvalidDirection)
        {
            if (getNextIsSurface())
            {
                newDirection = handymanDirectionRandSurface(validDirections);
            }
            else
            {
                auto* pathElement = MapGetPathElementAt(TileCoordsXYZ{ nextLoc });

                if (pathElement == nullptr)
                    return true;

                uint8_t pathDirections = (pathElement->getEdges() & validDirections) & 0xF;
                if (pathDirections == 0)
                {
                    newDirection = handymanDirectionRandSurface(validDirections);
                }
                else
                {
                    bool chooseRandom = true;
                    if (litterDirection != kInvalidDirection && pathDirections & (1 << litterDirection))
                    {
                        // Check whether path is a queue path and connected to a ride
                        bool connectedQueue = (pathElement->isQueue() && !pathElement->getRideIndex().IsNull());
                        // When in a queue path make the probability of following litter much lower (10% instead of 90%)
                        // as handymen often get stuck when there is litter on a normal path next to a queue they are in
                        uint32_t chooseRandomProbability = connectedQueue ? 0xE666 : 0x1999;
                        if ((ScenarioRand() & 0xFFFF) >= chooseRandomProbability)
                        {
                            chooseRandom = false;
                            newDirection = litterDirection;
                        }
                    }
                    else
                    {
                        pathDirections &= ~(1 << DirectionReverse(peepDirection));
                        if (pathDirections == 0)
                        {
                            pathDirections |= 1 << DirectionReverse(peepDirection);
                        }
                    }

                    if (chooseRandom)
                    {
                        do
                        {
                            newDirection = ScenarioRand() & 3;
                        } while ((pathDirections & (1 << newDirection)) == 0);
                    }
                }
            }
        }

        // newDirection can only contain a cardinal direction at this point, no diagonals
        assert(DirectionValid(newDirection));

        CoordsXY chosenTile = CoordsXY{ nextLoc } + CoordsDirectionDelta[newDirection];

        while (!MapIsLocationValid(chosenTile))
        {
            newDirection = handymanDirectionRandSurface(validDirections);
            chosenTile = CoordsXY{ nextLoc } + CoordsDirectionDelta[newDirection];
        }

        peepDirection = newDirection;
        setDestination(chosenTile + CoordsXY{ 16, 16 }, 3);
        if (state == PeepState::queuing)
        {
            destinationTolerance = (ScenarioRand() & 7) + 2;
        }
        return false;
    }

    Direction Staff::directionSurface(Direction initialDirection) const
    {
        uint8_t direction = initialDirection;
        for (int32_t i = 0; i < 3; ++i)
        {
            // Looks left and right from initial direction
            switch (i)
            {
                case 1:
                    direction++;
                    if (ScenarioRand() & 1)
                    {
                        direction -= 2;
                    }
                    break;
                case 2:
                    direction -= 2;
                    break;
            }

            direction &= 3;

            if (WallInTheWay({ nextLoc, nextLoc.z, nextLoc.z + kPeepClearanceHeight }, direction))
                continue;

            if (WallInTheWay({ nextLoc, nextLoc.z, nextLoc.z + kPeepClearanceHeight }, DirectionReverse(direction)))
                continue;

            CoordsXY chosenTile = CoordsXY{ nextLoc } + CoordsDirectionDelta[direction];

            if (!MapSurfaceIsBlocked(chosenTile))
            {
                return direction;
            }
        }
        return initialDirection;
    }

    /**
     *
     *  rct2: 0x006BFF45
     */
    Direction Staff::mechanicDirectionSurface() const
    {
        Direction direction = ScenarioRand() & 3;

        auto ride = GetRide(currentRide);
        if (ride != nullptr && (state == PeepState::answering || state == PeepState::headingToInspection)
            && (ScenarioRand() & 1))
        {
            auto location = ride->getStation(currentRideStation).Exit;
            if (location.IsNull())
            {
                location = ride->getStation(currentRideStation).Entrance;
            }

            direction = DirectionFromTo(CoordsXY(x, y), location.ToCoordsXY());
        }

        return directionSurface(direction);
    }

    /**
     *
     *  rct2: 0x006C02D1
     */
    Direction Staff::mechanicDirectionPathRand(uint8_t pathDirections) const
    {
        if (ScenarioRand() & 1)
        {
            if (pathDirections & (1 << peepDirection))
                return peepDirection;
        }

        // Modified from original to spam scenario_rand less
        uint8_t direction = ScenarioRand() & 3;
        for (int32_t i = 0; i < 4; ++i, ++direction)
        {
            direction &= 3;
            if (pathDirections & (1 << direction))
                return direction;
        }
        // This will never happen as pathDirections always has a bit set.
        return peepDirection;
    }

    /**
     *
     *  rct2: 0x006C0121
     */
    Direction Staff::mechanicDirectionPath(uint8_t validDirections, PathElement* pathElement)
    {
        uint32_t pathDirections = pathElement->getEdges();
        pathDirections &= validDirections;

        if (pathDirections == 0)
        {
            return mechanicDirectionSurface();
        }

        // Check if this is dead end - i.e. only way out is the reverse direction.
        pathDirections &= ~(1 << DirectionReverse(peepDirection));
        if (pathDirections == 0)
        {
            pathDirections |= (1 << DirectionReverse(peepDirection));
        }

        Direction direction = Numerics::bitScanForward(pathDirections);
        pathDirections &= ~(1 << direction);
        if (pathDirections == 0)
        {
            if (state != PeepState::answering && state != PeepState::headingToInspection)
            {
                return direction;
            }

            if (subState != 2)
            {
                return direction;
            }
            subState = 3;
        }

        pathDirections |= (1 << direction);

        // Mechanic is heading to ride (either broken down or for inspection).
        auto ride = GetRide(currentRide);
        if (ride != nullptr && (state == PeepState::answering || state == PeepState::headingToInspection))
        {
            /* Find location of the exit for the target ride station
             * or if the ride has no exit, the entrance. */
            TileCoordsXYZD location = ride->getStation(currentRideStation).Exit;
            if (location.IsNull())
            {
                location = ride->getStation(currentRideStation).Entrance;

                // If no entrance is present either. This is an incorrect state.
                if (location.IsNull())
                {
                    return mechanicDirectionPathRand(pathDirections);
                }
            }

            const auto goalPos = TileCoordsXYZ{ location };
            Direction pathfindDirection = PathFinding::ChooseDirection(
                TileCoordsXYZ{ nextLoc }, goalPos, *this, false, RideId::GetNull());
            if (pathfindDirection == kInvalidDirection)
            {
                /* Heuristic search failed for all directions.
                 * Reset the PathfindGoal - this means that the PathfindHistory
                 * will be reset in the next call to GuestPathfinding::ChooseDirection().
                 * This lets the heuristic search "try again" in case the player has
                 * edited the path layout or the mechanic was already stuck in the
                 * save game (e.g. with a worse version of the pathfinding). */
                resetPathfindGoal();
                return mechanicDirectionPathRand(pathDirections);
            }

            return pathfindDirection;
        }
        return mechanicDirectionPathRand(pathDirections);
    }

    /**
     *
     *  rct2: 0x006BFF2C
     */
    bool Staff::doMechanicPathFinding()
    {
        uint8_t validDirections = getValidPatrolDirections(nextLoc);
        Direction newDirection = kInvalidDirection;
        if (getNextIsSurface())
        {
            newDirection = mechanicDirectionSurface();
        }
        else
        {
            auto* pathElement = MapGetPathElementAt(TileCoordsXYZ{ nextLoc });
            if (pathElement == nullptr)
                return true;

            newDirection = mechanicDirectionPath(validDirections, pathElement);
        }

        // countof(CoordsDirectionDelta)
        assert(DirectionValid(newDirection));

        CoordsXY chosenTile = CoordsXY{ nextLoc } + CoordsDirectionDelta[newDirection];

        while (!MapIsLocationValid(chosenTile))
        {
            newDirection = mechanicDirectionSurface();
            chosenTile = CoordsXY{ nextLoc } + CoordsDirectionDelta[newDirection];
        }

        peepDirection = newDirection;
        auto tolerance = (ScenarioRand() & 7) + 2;
        setDestination(chosenTile + CoordsXY{ 16, 16 }, tolerance);

        return false;
    }

    /**
     *
     *  rct2: 0x006C050B
     */
    Direction Staff::directionPath(uint8_t validDirections, PathElement* pathElement) const
    {
        uint32_t pathDirections = pathElement->getEdges();
        if (state != PeepState::answering && state != PeepState::headingToInspection)
        {
            pathDirections &= validDirections;
        }

        if (pathDirections == 0)
        {
            return directionSurface(ScenarioRand() & 3);
        }

        pathDirections &= ~(1u << DirectionReverse(peepDirection));
        if (pathDirections == 0)
        {
            pathDirections |= (1u << DirectionReverse(peepDirection));
        }

        Direction direction = Numerics::bitScanForward(pathDirections);
        // If this is the only direction they can go, then go
        if (pathDirections == (1u << direction))
        {
            return direction;
        }

        direction = ScenarioRand() & 3;
        for (uint8_t i = 0; i < kNumOrthogonalDirections; ++i, direction = DirectionNext(direction))
        {
            if (pathDirections & (1u << direction))
                return direction;
        }

        // This will never happen as pathDirections will always have a bit set
        return direction;
    }

    /**
     *
     *  rct2: 0x006C0351
     */
    bool Staff::doMiscPathFinding()
    {
        uint8_t validDirections = getValidPatrolDirections(nextLoc);

        Direction newDirection = kInvalidDirection;
        if (getNextIsSurface())
        {
            newDirection = directionSurface(ScenarioRand() & 3);
        }
        else
        {
            auto* pathElement = MapGetPathElementAt(TileCoordsXYZ{ nextLoc });
            if (pathElement == nullptr)
                return true;

            newDirection = directionPath(validDirections, pathElement);
        }

        CoordsXY chosenTile = CoordsXY{ nextLoc } + CoordsDirectionDelta[newDirection];

        while (!MapIsLocationValid(chosenTile))
        {
            newDirection = directionSurface(ScenarioRand() & 3);
            chosenTile = CoordsXY{ nextLoc } + CoordsDirectionDelta[newDirection];
        }

        peepDirection = newDirection;
        auto tolerance = (ScenarioRand() & 7) + 2;
        setDestination(chosenTile + CoordsXY{ 16, 16 }, tolerance);

        return false;
    }

    bool Staff::isMechanicHeadingToFixRideBlockingPath()
    {
        if (!isMechanic())
            return false;

        auto tileCoords = TileCoordsXYZ(CoordsXYZ{ getDestination(), nextLoc.z });
        auto trackElement = MapGetFirstTileElementWithBaseHeightBetween<TrackElement>(
            { tileCoords, tileCoords.z + kPathHeightStep });
        if (trackElement == nullptr)
            return false;

        auto ride = GetRide(trackElement->getRideIndex());
        if (ride == nullptr)
            return false;

        return ride->id == currentRide;
    }

    /**
     *
     *  rct2: 0x006C086D
     */
    void Staff::entertainerUpdateNearbyPeeps()
    {
        // Iterate over tiles within a 3-tile radius (96 units)
        constexpr auto kTileRadius = 3;
        constexpr auto kLookupRadius = kCoordsXYStep * kTileRadius;

        for (int32_t tileX = x - kLookupRadius; tileX <= x + kLookupRadius; tileX += kCoordsXYStep)
        {
            for (int32_t tileY = y - kLookupRadius; tileY <= y + kLookupRadius; tileY += kCoordsXYStep)
            {
                for (auto* guest : EntityTileList<Guest>({ tileX, tileY }))
                {
                    if (guest->x == kLocationNull)
                        continue;

                    int16_t z_dist = std::abs(z - guest->z);
                    if (z_dist > kTileRadius / 2)
                        continue;

                    int16_t x_dist = std::abs(x - guest->x);
                    int16_t y_dist = std::abs(y - guest->y);

                    if (x_dist > kLookupRadius)
                        continue;

                    if (y_dist > kLookupRadius)
                        continue;

                    if (guest->state == PeepState::walking)
                    {
                        guest->happinessTarget = std::min(guest->happinessTarget + 4, kPeepMaxHappiness);
                        staffGuestsEntertained = AddClamp(staffGuestsEntertained, 1u);
                        windowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
                    }
                    else if (guest->state == PeepState::queuing)
                    {
                        guest->timeInQueue = std::max(0, guest->timeInQueue - 200);
                        guest->happinessTarget = std::min(guest->happinessTarget + 3, kPeepMaxHappiness);
                        staffGuestsEntertained = AddClamp(staffGuestsEntertained, 1u);
                        windowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
                    }
                }
            }
        }
    }

    /**
     *
     *  rct2: 0x006C05AE
     */
    bool Staff::doEntertainerPathFinding()
    {
        if (((ScenarioRand() & 0xFFFF) <= 0x4000) && isActionInterruptableSafely())
        {
            action = (ScenarioRand() & 1) ? PeepActionType::wave2 : PeepActionType::joy;
            animationFrameNum = 0;
            animationImageIdOffset = 0;

            updateCurrentAnimationType();
            entertainerUpdateNearbyPeeps();
        }

        return doMiscPathFinding();
    }

    /**
     *
     *  rct2: 0x006BF926
     */
    bool Staff::doPathFinding()
    {
        switch (assignedStaffType)
        {
            case StaffType::handyman:
                return doHandymanPathFinding();
            case StaffType::mechanic:
                return doMechanicPathFinding();
            case StaffType::security:
                return doMiscPathFinding();
            case StaffType::entertainer:
                return doEntertainerPathFinding();

            default:
                assert(false);
                return false;
        }
    }

    void Staff::setHireDate(int32_t value)
    {
        this->hireDate = value;
    }

    int32_t Staff::getHireDate() const
    {
        return hireDate;
    }

    Drawing::Colour StaffGetColour(StaffType staffType)
    {
        const auto& park = getGameState().park;
        switch (staffType)
        {
            case StaffType::handyman:
                return park.staffHandymanColour;
            case StaffType::mechanic:
                return park.staffMechanicColour;
            case StaffType::security:
                return park.staffSecurityColour;
            case StaffType::entertainer:
                return Drawing::Colour::black;
            default:
                assert(false);
                return Drawing::Colour::black;
        }
    }

    GameActions::Result StaffSetColour(StaffType staffType, OpenRCT2::Drawing::Colour value)
    {
        auto& park = getGameState().park;
        switch (staffType)
        {
            case StaffType::handyman:
                park.staffHandymanColour = value;
                break;
            case StaffType::mechanic:
                park.staffMechanicColour = value;
                break;
            case StaffType::security:
                park.staffSecurityColour = value;
                break;
            default:
                return GameActions::Result(
                    GameActions::Status::invalidParameters, STR_ERR_INVALID_PARAMETER,
                    STR_ERR_ACTION_INVALID_FOR_THAT_STAFF_TYPE);
        }
        return GameActions::Result();
    }

    /** rct2: 0x009929C8 */
    static constexpr CoordsXY kMowingWaypoints[] = {
        { 28, 28 }, { 28, 4 }, { 20, 4 }, { 20, 28 }, { 12, 28 }, { 12, 4 }, { 4, 4 }, { 4, 28 },
    };

    /**
     *
     *  rct2: 0x006BF567
     */
    void Staff::updateMowing()
    {
        if (!checkForPath())
            return;

        while (true)
        {
            if (auto loc = updateAction(); loc.has_value())
            {
                int16_t checkZ = TileElementHeight(*loc);
                moveTo({ loc.value(), checkZ });
                return;
            }

            var37++;

            if (var37 == 1)
            {
                switchToSpecialSprite(2);
            }

            if (var37 == std::size(kMowingWaypoints))
            {
                stateReset();
                return;
            }

            auto destination = kMowingWaypoints[var37] + nextLoc;
            setDestination(destination);

            if (var37 != 7)
                continue;

            auto surfaceElement = MapGetSurfaceElementAt(nextLoc);
            if (surfaceElement != nullptr && surfaceElement->canGrassGrow())
            {
                surfaceElement->setGrassLength(GRASS_LENGTH_MOWED);
                MapInvalidateTileZoom0({ nextLoc, surfaceElement->getBaseZ(), surfaceElement->getBaseZ() + 16 });
            }
            staffLawnsMown = AddClamp(staffLawnsMown, 1u);
            windowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
        }
    }

    /**
     *
     *  rct2: 0x006BF7E6
     */
    void Staff::updateWatering()
    {
        staffMowingTimeout = 0;
        if (subState == 0)
        {
            if (!checkForPath())
                return;

            const auto [pathingResult, _] = performNextAction();
            if (!(pathingResult & PATHING_DESTINATION_REACHED))
                return;

            orientation = (var37 & 3) << 3;
            action = PeepActionType::staffWatering;
            animationFrameNum = 0;
            animationImageIdOffset = 0;
            updateCurrentAnimationType();

            subState = 1;
        }
        else if (subState == 1)
        {
            if (!isActionWalking())
            {
                updateAction();
                invalidate();
                return;
            }

            auto actionLoc = CoordsXY{ nextLoc } + CoordsDirectionDelta[var37];

            TileElement* tile_element = MapGetFirstElementAt(actionLoc);
            if (tile_element == nullptr)
                return;

            do
            {
                if (tile_element->getType() != TileElementType::smallScenery)
                    continue;

                if (abs(nextLoc.z - tile_element->getBaseZ()) > 4 * kCoordsZStep)
                    continue;

                const auto* sceneryEntry = tile_element->asSmallScenery()->getEntry();

                if (sceneryEntry == nullptr || !sceneryEntry->flags.has(SmallSceneryFlag::canBeWatered))
                    continue;

                tile_element->asSmallScenery()->setAge(0);
                MapInvalidateTileZoom0({ actionLoc, tile_element->getBaseZ(), tile_element->getClearanceZ() });
                staffGardensWatered = AddClamp(staffGardensWatered, 1u);
                windowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
            } while (!(tile_element++)->isLastForTile());

            stateReset();
        }
    }

    /**
     *
     *  rct2: 0x006BF6C9
     */
    void Staff::updateEmptyingBin()
    {
        staffMowingTimeout = 0;

        if (subState == 0)
        {
            if (!checkForPath())
                return;

            const auto [pathingResult, _] = performNextAction();
            if (!(pathingResult & PATHING_DESTINATION_REACHED))
                return;

            orientation = (var37 & 3) << 3;
            action = PeepActionType::staffEmptyBin;
            animationFrameNum = 0;
            animationImageIdOffset = 0;
            updateCurrentAnimationType();

            subState = 1;
        }
        else if (subState == 1)
        {
            if (isActionWalking())
            {
                stateReset();
                return;
            }

            updateAction();
            invalidate();

            if (animationFrameNum != 11)
                return;

            TileElement* tile_element = MapGetFirstElementAt(nextLoc);
            if (tile_element == nullptr)
                return;

            for (;; tile_element++)
            {
                if (tile_element->getType() == TileElementType::path)
                {
                    if (nextLoc.z == tile_element->getBaseZ())
                        break;
                }
                if ((tile_element)->isLastForTile())
                {
                    stateReset();
                    return;
                }
            }

            if (!tile_element->asPath()->hasAddition())
            {
                stateReset();
                return;
            }

            auto* pathAddEntry = tile_element->asPath()->getAdditionEntry();
            if (!(pathAddEntry->flags & PATH_ADDITION_FLAG_IS_BIN) || tile_element->asPath()->isBroken()
                || tile_element->asPath()->additionIsGhost())
            {
                stateReset();
                return;
            }

            uint8_t additionStatus = tile_element->asPath()->getAdditionStatus() | ((3 << var37) << var37);
            tile_element->asPath()->setAdditionStatus(additionStatus);

            MapInvalidateTileZoom0({ nextLoc, tile_element->getBaseZ(), tile_element->getClearanceZ() });
            staffBinsEmptied = AddClamp(staffBinsEmptied, 1u);
            windowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
        }
    }

    /**
     *
     *  rct2: 0x6BF641
     */
    void Staff::updateSweeping()
    {
        staffMowingTimeout = 0;
        if (!checkForPath())
            return;

        if (action == PeepActionType::staffSweep && animationFrameNum == 8)
        {
            // Remove sick at this location
            Litter::removeAt(getLocation());
            staffLitterSwept = AddClamp(staffLitterSwept, 1u);
            windowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
        }
        if (auto loc = updateAction(); loc.has_value())
        {
            int16_t actionZ = getZOnSlope(loc->x, loc->y);
            moveTo({ loc.value(), actionZ });
            return;
        }

        var37++;
        if (var37 != 2)
        {
            action = PeepActionType::staffSweep;
            animationFrameNum = 0;
            animationImageIdOffset = 0;
            updateCurrentAnimationType();
            return;
        }
        stateReset();
    }

    /**
     *
     *  rct2: 0x006C16D7
     */
    void Staff::updateHeadingToInspect()
    {
        auto ride = GetRide(currentRide);
        if (ride == nullptr)
        {
            setState(PeepState::falling);
            return;
        }

        if (ride->getStation(currentRideStation).Exit.IsNull())
        {
            ride->flags.unset(RideFlag::dueInspection);
            setState(PeepState::falling);
            return;
        }

        if (ride->mechanicStatus != MechanicStatus::heading || !ride->flags.has(RideFlag::dueInspection))
        {
            setState(PeepState::falling);
            return;
        }

        if (subState == 0)
        {
            mechanicTimeSinceCall = 0;
            resetPathfindGoal();
            subState = 2;
        }

        if (subState <= 3)
        {
            mechanicTimeSinceCall++;
            if (mechanicTimeSinceCall > 2500)
            {
                if (ride->flags.has(RideFlag::dueInspection) && ride->mechanicStatus == MechanicStatus::heading)
                {
                    ride->mechanicStatus = MechanicStatus::calling;
                }
                setState(PeepState::falling);
                return;
            }

            if (!checkForPath())
                return;

            if (shouldWaitForLevelCrossing() && !isMechanicHeadingToFixRideBlockingPath())
                return;

            const auto [pathingResult, rideEntranceExitElement] = performNextAction();
            if (!(pathingResult & PATHING_RIDE_EXIT) && !(pathingResult & PATHING_RIDE_ENTRANCE))
            {
                return;
            }

            if (currentRide != rideEntranceExitElement->asEntrance()->getRideIndex())
                return;

            StationIndex exitIndex = rideEntranceExitElement->asEntrance()->getStationIndex();
            if (currentRideStation != exitIndex)
                return;

            if (pathingResult & PATHING_RIDE_ENTRANCE)
            {
                if (!ride->getStation(exitIndex).Exit.IsNull())
                {
                    return;
                }
            }

            peepDirection = rideEntranceExitElement->getDirection();

            auto newDestination = CoordsXY{ 16, 16 } + nextLoc + (DirectionOffsets[peepDirection] * 53);
            setDestination(newDestination, 2);
            orientation = peepDirection << 3;

            z = rideEntranceExitElement->baseHeight * 4;
            subState = 4;
            // Falls through into SubState 4
        }

        int16_t delta_y = abs(getLocation().y - getDestination().y);
        if (auto loc = updateAction(); loc.has_value())
        {
            auto newZ = ride->getStation(currentRideStation).GetBaseZ();
            if (delta_y < 20)
            {
                newZ += ride->getRideTypeDescriptor().Heights.PlatformHeight;
            }

            moveTo({ loc.value(), newZ });
            return;
        }

        setState(PeepState::inspecting);
        subState = 0;
    }

    /**
     *
     *  rct2: 0x006C0CB8
     */
    void Staff::updateAnswering()
    {
        auto ride = GetRide(currentRide);
        if (ride == nullptr || ride->mechanicStatus != MechanicStatus::heading)
        {
            setState(PeepState::falling);
            return;
        }

        if (subState == 0)
        {
            action = PeepActionType::staffAnswerCall;
            animationFrameNum = 0;
            animationImageIdOffset = 0;

            updateCurrentAnimationType();

            subState = 1;
            PeepWindowStateUpdate(this);
            return;
        }
        if (subState == 1)
        {
            if (isActionWalking())
            {
                subState = 2;
                PeepWindowStateUpdate(this);
                mechanicTimeSinceCall = 0;
                resetPathfindGoal();
                return;
            }
            updateAction();
            invalidate();
            return;
        }
        if (subState <= 3)
        {
            mechanicTimeSinceCall++;
            if (mechanicTimeSinceCall > 2500)
            {
                ride->mechanicStatus = MechanicStatus::calling;
                ride->windowInvalidateFlags.set(RideInvalidateFlag::maintenance);
                setState(PeepState::falling);
                return;
            }

            if (!checkForPath())
                return;

            if (shouldWaitForLevelCrossing() && !isMechanicHeadingToFixRideBlockingPath())
                return;

            const auto [pathingResult, rideEntranceExitElement] = performNextAction();
            if (!(pathingResult & PATHING_RIDE_EXIT) && !(pathingResult & PATHING_RIDE_ENTRANCE))
            {
                return;
            }

            if (currentRide != rideEntranceExitElement->asEntrance()->getRideIndex())
                return;

            StationIndex exitIndex = rideEntranceExitElement->asEntrance()->getStationIndex();
            if (currentRideStation != exitIndex)
                return;

            if (pathingResult & PATHING_RIDE_ENTRANCE)
            {
                if (!ride->getStation(exitIndex).Exit.IsNull())
                {
                    return;
                }
            }

            peepDirection = rideEntranceExitElement->getDirection();

            int32_t destX = nextLoc.x + 16 + DirectionOffsets[peepDirection].x * 53;
            int32_t destY = nextLoc.y + 16 + DirectionOffsets[peepDirection].y * 53;

            setDestination({ destX, destY }, 2);
            orientation = peepDirection << 3;

            z = rideEntranceExitElement->baseHeight * 4;
            subState = 4;
            // Falls through into SubState 4
        }

        int16_t delta_y = abs(y - getDestination().y);
        if (auto loc = updateAction(); loc.has_value())
        {
            auto newZ = ride->getStation(currentRideStation).GetBaseZ();
            if (delta_y < 20)
            {
                newZ += ride->getRideTypeDescriptor().Heights.PlatformHeight;
            }

            moveTo({ loc.value(), newZ });
            return;
        }

        setState(PeepState::fixing);
        subState = 0;
    }

    /** rct2: 0x00992A5C */
    static constexpr CoordsXY kWateringUseOffsets[] = {
        { 3, 16 }, { 16, 29 }, { 29, 16 }, { 16, 3 }, { 3, 29 }, { 29, 29 }, { 29, 3 }, { 3, 3 },
    };

    /**
     *
     *  rct2: 0x006BF483
     */
    bool Staff::updatePatrollingFindWatering()
    {
        if (!(staffOrders & STAFF_ORDERS_WATER_FLOWERS))
            return false;

        uint8_t chosen_position = ScenarioRand() & 7;
        for (int32_t i = 0; i < 8; ++i, ++chosen_position)
        {
            chosen_position &= 7;

            auto chosenLoc = CoordsXY{ nextLoc } + CoordsDirectionDelta[chosen_position];

            TileElement* tile_element = MapGetFirstElementAt(chosenLoc);

            // This seems to happen in some SV4 files.
            if (tile_element == nullptr)
            {
                continue;
            }

            do
            {
                if (tile_element->getType() != TileElementType::smallScenery)
                {
                    continue;
                }

                auto z_diff = abs(nextLoc.z - tile_element->getBaseZ());

                if (z_diff >= 4 * kCoordsZStep)
                {
                    continue;
                }

                auto* sceneryEntry = tile_element->asSmallScenery()->getEntry();

                if (sceneryEntry == nullptr || !sceneryEntry->flags.has(SmallSceneryFlag::canBeWatered))
                {
                    continue;
                }

                if (tile_element->asSmallScenery()->getAge() < kSceneryWitherAgeThreshold2)
                {
                    if (chosen_position >= 4)
                    {
                        continue;
                    }

                    if (tile_element->asSmallScenery()->getAge() < kSceneryWitherAgeThreshold1)
                    {
                        continue;
                    }
                }

                if (isHandymanAlreadyServicingTile(chosenLoc, PeepState::watering))
                    continue;

                setState(PeepState::watering);
                var37 = chosen_position;

                subState = 0;
                auto destination = kWateringUseOffsets[chosen_position] + getLocation().ToTileStart();
                setDestination(destination, 3);

                return true;
            } while (!(tile_element++)->isLastForTile());
        }
        return false;
    }

    /**
     *
     *  rct2: 0x006BF3A1
     */
    bool Staff::updatePatrollingFindBin()
    {
        if (!(staffOrders & STAFF_ORDERS_EMPTY_BINS))
            return false;

        if (getNextIsSurface())
            return false;

        TileElement* tileElement = MapGetFirstElementAt(nextLoc);
        if (tileElement == nullptr)
            return false;

        for (;; tileElement++)
        {
            if (tileElement->getType() == TileElementType::path && (tileElement->getBaseZ() == nextLoc.z))
                break;

            if (tileElement->isLastForTile())
                return false;
        }

        if (!tileElement->asPath()->hasAddition())
            return false;
        auto* pathAddEntry = tileElement->asPath()->getAdditionEntry();
        if (pathAddEntry == nullptr)
            return false;

        if (!(pathAddEntry->flags & PATH_ADDITION_FLAG_IS_BIN))
            return false;

        if (tileElement->asPath()->isBroken())
            return false;

        if (tileElement->asPath()->additionIsGhost())
            return false;

        uint8_t bin_positions = tileElement->asPath()->getEdges();
        uint8_t bin_quantity = tileElement->asPath()->getAdditionStatus();
        uint8_t chosen_position = 0;

        for (; chosen_position < 4; ++chosen_position)
        {
            if (!(bin_positions & 1) && !(bin_quantity & 3))
                break;
            bin_positions >>= 1;
            bin_quantity >>= 2;
        }

        if (chosen_position == 4)
            return false;

        if (isHandymanAlreadyServicingTile(CoordsXY{ nextLoc }, PeepState::emptyingBin))
            return false;

        var37 = chosen_position;
        setState(PeepState::emptyingBin);

        subState = 0;
        auto destination = BinUseOffsets[chosen_position] + getLocation().ToTileStart();
        setDestination(destination, 3);
        return true;
    }

    /**
     *
     *  rct2: 0x006BF322
     */
    bool Staff::updatePatrollingFindGrass()
    {
        if (!(staffOrders & STAFF_ORDERS_MOWING))
            return false;

        if (staffMowingTimeout < 12)
            return false;

        if (!(getNextIsSurface()))
            return false;

        auto surfaceElement = MapGetSurfaceElementAt(nextLoc);
        if (surfaceElement != nullptr && surfaceElement->canGrassGrow())
        {
            if ((surfaceElement->getGrassLength() & 0x7) >= GRASS_LENGTH_CLEAR_1
                && !isHandymanAlreadyServicingTile(CoordsXY{ nextLoc }, PeepState::mowing))
            {
                setState(PeepState::mowing);
                var37 = 0;
                // Original code used .y for both x and y. Changed to .x to make more sense (both x and y are 28)

                auto destination = kMowingWaypoints[0] + nextLoc;
                setDestination(destination, 3);
                return true;
            }
        }
        return false;
    }

    /**
     *
     *  rct2: 0x006BF295
     */
    bool Staff::updatePatrollingFindSweeping()
    {
        if (!(staffOrders & STAFF_ORDERS_SWEEPING))
            return false;
        auto quad = EntityTileList<Litter>({ x, y });
        for (auto litter : quad)
        {
            uint16_t z_diff = abs(z - litter->z);

            if (z_diff >= 16)
                continue;

            if (isHandymanAlreadyServicingTile(litter->getLocation(), PeepState::sweeping))
                continue;

            setState(PeepState::sweeping);

            var37 = 0;
            setDestination(litter->getLocation(), 5);
            return true;
        }

        return false;
    }

    bool Staff::securityGuardPathIsCrowded() const
    {
        // Iterate over tiles within a 3-tile radius (96 units)
        constexpr auto kTileRadius = 3;
        constexpr auto kLookupRadius = kCoordsXYStep * kTileRadius;
        constexpr auto kSecurityPathCrowdedThreshold = 20;

        int16_t guestCount = 0;

        for (int32_t tileX = x - kLookupRadius; tileX <= x + kLookupRadius; tileX += kCoordsXYStep)
        {
            for (int32_t tileY = y - kLookupRadius; tileY <= y + kLookupRadius; tileY += kCoordsXYStep)
            {
                for (auto* guest : EntityTileList<Guest>({ tileX, tileY }))
                {
                    if (guest->x == kLocationNull)
                        continue;

                    int16_t zDist = std::abs(z - guest->z);
                    if (zDist > kTileRadius / 2)
                        continue;

                    int16_t xDist = std::abs(x - guest->x);
                    if (xDist > kLookupRadius)
                        continue;

                    int16_t yDist = std::abs(y - guest->y);
                    if (yDist > kLookupRadius)
                        continue;

                    if (!guest->isActionWalking())
                        continue;

                    guestCount++;
                    if (guestCount >= kSecurityPathCrowdedThreshold)
                        return true;
                }
            }
        }

        return false;
    }

    void Staff::tick128UpdateStaff()
    {
        if (assignedStaffType != StaffType::security)
            return;

        // Alternate between walking animations based on crowd size
        auto newAnimationGroup = PeepAnimationGroup::normal;
        if (state == PeepState::patrolling && securityGuardPathIsCrowded())
            newAnimationGroup = PeepAnimationGroup::alternate;

        if (animationGroup == newAnimationGroup)
            return;

        animationGroup = newAnimationGroup;
        animationImageIdOffset = 0;
        walkingAnimationFrameNum = 0;
        if (action < PeepActionType::idle)
            action = PeepActionType::walking;

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(animationObjectIndex);

        // NB: security staff have two animations groups: one regular, and one slow-walking
        peepFlags.unset(PeepFlag::slowWalk);
        if (animObj->IsSlowWalking(newAnimationGroup))
        {
            peepFlags.set(PeepFlag::slowWalk);
        }

        animationType = PeepAnimationType::invalid;
        updateCurrentAnimationType();
    }

    bool Staff::isMechanic() const
    {
        return assignedStaffType == StaffType::mechanic;
    }

    bool Staff::isEntertainer() const
    {
        return assignedStaffType == StaffType::entertainer;
    }

    void Staff::update()
    {
        if (peepFlags.has(PeepFlag::positionFrozen))
        {
            if (!peepFlags.has(PeepFlag::animationFrozen))
            {
                // This is circumventing other logic, so only update every few ticks
                if ((getGameState().currentTicks & 3) == 0)
                {
                    if (isActionWalking())
                        updateWalkingAnimation();
                    else
                        updateActionAnimation();
                    invalidate();
                }
            }
            return;
        }
        else if (peepFlags.has(PeepFlag::animationFrozen))
        {
            // Animation is frozen while position is not. This allows a peep to walk
            // around without its sprite being updated, which looks very glitchy.
            // We'll just remove the flag and continue as normal, in this case.
            peepFlags.unset(PeepFlag::animationFrozen);
        }

        // Walking speed logic
        const auto stepsToTake = getStepsToTake();
        const auto carryCheck = stepProgress + stepsToTake;
        stepProgress = carryCheck;

        if (carryCheck <= 255)
        {
            // No-op: Keep replay working for now, can be eliminate with a replay update.
        }
        else
        {
            // Loc68FD2F
            switch (state)
            {
                case PeepState::falling:
                    updateFalling();
                    break;
                case PeepState::one:
                    update1();
                    break;
                case PeepState::onRide:
                    // No action
                    break;
                case PeepState::picked:
                    updatePicked();
                    break;
                case PeepState::patrolling:
                    updatePatrolling();
                    break;
                case PeepState::mowing:
                    updateMowing();
                    break;
                case PeepState::sweeping:
                    updateSweeping();
                    break;
                case PeepState::answering:
                    updateAnswering();
                    break;
                case PeepState::fixing:
                    updateFixing(stepsToTake);
                    break;
                case PeepState::inspecting:
                    updateFixing(stepsToTake);
                    break;
                case PeepState::emptyingBin:
                    updateEmptyingBin();
                    break;
                case PeepState::watering:
                    updateWatering();
                    break;
                case PeepState::headingToInspection:
                    updateHeadingToInspect();
                    break;
                default:
                    // TODO reset to default state
                    assert(false);
                    break;
            }
        }
    }

    /**
     *
     *  rct2: 0x006BF1FD
     */
    void Staff::updatePatrolling()
    {
        if (!checkForPath())
            return;

        if (shouldWaitForLevelCrossing() && !isMechanicHeadingToFixRideBlockingPath())
        {
            updateWaitingAtCrossing();
            return;
        }

        const auto [pathingResult, _] = performNextAction();
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        if (getNextIsSurface())
        {
            auto surfaceElement = MapGetSurfaceElementAt(nextLoc);

            if (surfaceElement != nullptr)
            {
                int32_t water_height = surfaceElement->getWaterHeight();
                if (water_height > 0)
                {
                    moveTo({ x, y, water_height });
                    setState(PeepState::falling);
                    return;
                }
            }
        }

        if (assignedStaffType != StaffType::handyman)
            return;

        if (updatePatrollingFindSweeping())
            return;

        if (updatePatrollingFindGrass())
            return;

        if (updatePatrollingFindBin())
            return;

        updatePatrollingFindWatering();
    }

    enum
    {
        PEEP_FIXING_ENTER_STATION = 0,
        PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE = 1,
        PEEP_FIXING_FIX_VEHICLE_CLOSED_RESTRAINTS = 2,
        PEEP_FIXING_FIX_VEHICLE_CLOSED_DOORS = 3,
        PEEP_FIXING_FIX_VEHICLE_OPEN_RESTRAINTS = 4,
        PEEP_FIXING_FIX_VEHICLE_OPEN_DOORS = 5,
        PEEP_FIXING_FIX_VEHICLE_MALFUNCTION = 6,
        PEEP_FIXING_MOVE_TO_STATION_END = 7,
        PEEP_FIXING_FIX_STATION_END = 8,
        PEEP_FIXING_MOVE_TO_STATION_START = 9,
        PEEP_FIXING_FIX_STATION_START = 10,
        PEEP_FIXING_FIX_STATION_BRAKES = 11,
        PEEP_FIXING_MOVE_TO_STATION_EXIT = 12,
        PEEP_FIXING_FINISH_FIX_OR_INSPECT = 13,
        PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT = 14,
    };

    /**
     * FixingSubstatesForBreakdown[] defines the applicable peep sub_states for
     * mechanics fixing a ride. The array is indexed by breakdownReason:
     * - indexes 0-7 are the 8 breakdown reasons (see BREAKDOWN_* in Ride.h)
     *   when fixing a broken down ride;
     * - index 8 is for inspecting a ride.
     */
    // clang-format off
    static constexpr uint32_t FixingSubstatesForBreakdown[9] = {
        ( // Breakdown::safetyCutOut
            (1 << PEEP_FIXING_MOVE_TO_STATION_END) |
            (1 << PEEP_FIXING_FIX_STATION_END) |
            (1 << PEEP_FIXING_MOVE_TO_STATION_START) |
            (1 << PEEP_FIXING_FIX_STATION_START) |
            (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
            (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
            (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
        ),
        ( // Breakdown::restraintsStuckClosed
            (1 << PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE) |
            (1 << PEEP_FIXING_FIX_VEHICLE_CLOSED_RESTRAINTS) |
            (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
            (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
            (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
        ),
        ( // Breakdown::restraintsStuckOpen
            (1 << PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE) |
            (1 << PEEP_FIXING_FIX_VEHICLE_OPEN_RESTRAINTS) |
            (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
            (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
            (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
        ),
        ( // Breakdown::doorsStuckClosed
            (1 << PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE) |
            (1 << PEEP_FIXING_FIX_VEHICLE_CLOSED_DOORS) |
            (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
            (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
            (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
        ),
        ( // Breakdown::doorsStuckOpen
            (1 << PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE) |
            (1 << PEEP_FIXING_FIX_VEHICLE_OPEN_DOORS) |
            (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
            (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
            (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
        ),
        ( // Breakdown::vehicleMalfunction
            (1 << PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE) |
            (1 << PEEP_FIXING_FIX_VEHICLE_MALFUNCTION) |
            (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
            (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
            (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
        ),
        ( // Breakdown::brakesFailure
            (1 << PEEP_FIXING_MOVE_TO_STATION_START) |
            (1 << PEEP_FIXING_FIX_STATION_BRAKES) |
            (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
            (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
            (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
        ),
        ( // Breakdown::controlFailure
            (1 << PEEP_FIXING_MOVE_TO_STATION_END) |
            (1 << PEEP_FIXING_FIX_STATION_END) |
            (1 << PEEP_FIXING_MOVE_TO_STATION_START) |
            (1 << PEEP_FIXING_FIX_STATION_START) |
            (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
            (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
            (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
        ),
        ( // INSPECTION
            (1 << PEEP_FIXING_MOVE_TO_STATION_END) |
            (1 << PEEP_FIXING_FIX_STATION_END) |
            (1 << PEEP_FIXING_MOVE_TO_STATION_START) |
            (1 << PEEP_FIXING_FIX_STATION_START) |
            (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
            (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
            (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
        ),
    };
    // clang-format on

    /**
     *
     *  rct2: 0x006C0E8B
     * Also used by inspecting.
     */
    void Staff::updateFixing(int32_t steps)
    {
        auto ride = GetRide(currentRide);
        if (ride == nullptr)
        {
            setState(PeepState::falling);
            return;
        }

        bool progressToNextSubstate = true;
        bool firstRun = true;

        if ((state == PeepState::inspecting) && (ride->flags.hasAny(RideFlag::breakdownPending, RideFlag::brokenDown)))
        {
            // Ride has broken down since Mechanic was called to inspect it.
            // Mechanic identifies the breakdown and switches to fixing it.
            state = PeepState::fixing;
        }

        while (progressToNextSubstate)
        {
            switch (subState)
            {
                case PEEP_FIXING_ENTER_STATION:
                    nextFlags &= ~PEEP_NEXT_FLAG_IS_SLOPED;
                    progressToNextSubstate = updateFixingEnterStation(*ride);
                    break;

                case PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE:
                    progressToNextSubstate = updateFixingMoveToBrokenDownVehicle(firstRun, *ride);
                    break;

                case PEEP_FIXING_FIX_VEHICLE_CLOSED_RESTRAINTS:
                case PEEP_FIXING_FIX_VEHICLE_CLOSED_DOORS:
                case PEEP_FIXING_FIX_VEHICLE_OPEN_RESTRAINTS:
                case PEEP_FIXING_FIX_VEHICLE_OPEN_DOORS:
                    progressToNextSubstate = updateFixingFixVehicle(firstRun, *ride);
                    break;

                case PEEP_FIXING_FIX_VEHICLE_MALFUNCTION:
                    progressToNextSubstate = updateFixingFixVehicleMalfunction(firstRun, *ride);
                    break;

                case PEEP_FIXING_MOVE_TO_STATION_END:
                    progressToNextSubstate = updateFixingMoveToStationEnd(firstRun, *ride);
                    break;

                case PEEP_FIXING_FIX_STATION_END:
                    progressToNextSubstate = updateFixingFixStationEnd(firstRun);
                    break;

                case PEEP_FIXING_MOVE_TO_STATION_START:
                    progressToNextSubstate = updateFixingMoveToStationStart(firstRun, *ride);
                    break;

                case PEEP_FIXING_FIX_STATION_START:
                    progressToNextSubstate = updateFixingFixStationStart(firstRun, *ride);
                    break;

                case PEEP_FIXING_FIX_STATION_BRAKES:
                    progressToNextSubstate = updateFixingFixStationBrakes(firstRun, *ride);
                    break;

                case PEEP_FIXING_MOVE_TO_STATION_EXIT:
                    progressToNextSubstate = updateFixingMoveToStationExit(firstRun, *ride);
                    break;

                case PEEP_FIXING_FINISH_FIX_OR_INSPECT:
                    progressToNextSubstate = updateFixingFinishFixOrInspect(firstRun, steps, *ride);
                    break;

                case PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT:
                    progressToNextSubstate = updateFixingLeaveByEntranceExit(firstRun, *ride);
                    break;

                default:
                    LOG_ERROR("Invalid substate");
                    progressToNextSubstate = false;
            }

            firstRun = false;

            if (!progressToNextSubstate)
            {
                break;
            }

            int32_t newSubState = subState;
            uint32_t sub_state_sequence_mask = FixingSubstatesForBreakdown[8];

            if (state != PeepState::inspecting)
            {
                sub_state_sequence_mask = FixingSubstatesForBreakdown[EnumValue(ride->breakdownReasonPending)];
            }

            do
            {
                newSubState++;
            } while ((sub_state_sequence_mask & (1 << newSubState)) == 0);

            subState = newSubState & 0xFF;
        }
    }

    /**
     * rct2: 0x006C0EEC
     * fixing SubState: enter_station - applies to fixing all break down reasons and ride inspections.
     */
    bool Staff::updateFixingEnterStation(Ride& ride) const
    {
        ride.mechanicStatus = MechanicStatus::fixing;
        ride.windowInvalidateFlags.set(RideInvalidateFlag::maintenance);

        return true;
    }

    /**
     * rct2: 0x006C0F09
     * fixing SubState: move_to_broken_down_vehicle - applies to fixing all vehicle specific breakdown reasons
     * - see FixingSubstatesForBreakdown[]
     */
    bool Staff::updateFixingMoveToBrokenDownVehicle(bool firstRun, const Ride& ride)
    {
        if (!firstRun)
        {
            Vehicle* vehicle = RideGetBrokenVehicle(ride);
            if (vehicle == nullptr)
            {
                return true;
            }

            while (true)
            {
                if (vehicle->IsHead())
                {
                    break;
                }

                auto trackType = vehicle->GetTrackType();
                if (trackTypeIsStation(trackType))
                {
                    break;
                }

                vehicle = getGameState().entities.getEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
                if (vehicle == nullptr)
                {
                    return true;
                }
            }

            CoordsXY offset = DirectionOffsets[peepDirection];
            auto destination = (offset * -12) + vehicle->getLocation();
            setDestination(destination, 2);
        }

        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return false;
        }

        return true;
    }

    /**
     * rct2: 0x006C0FD3
     * fixing SubState: fix_vehicle - applies to fixing vehicle with:
     * 1. restraints stuck closed,
     * 2. doors stuck closed,
     * 3. restrains stuck open,
     * 4. doors stuck open.
     * - see FixingSubstatesForBreakdown[]
     */
    bool Staff::updateFixingFixVehicle(bool firstRun, const Ride& ride)
    {
        if (!firstRun)
        {
            orientation = peepDirection << 3;

            action = (ScenarioRand() & 1) ? PeepActionType::staffFix2 : PeepActionType::staffFix;
            animationImageIdOffset = 0;
            animationFrameNum = 0;
            updateCurrentAnimationType();
        }

        if (isActionWalking())
        {
            return true;
        }

        updateAction();
        invalidate();

        uint8_t actionFrame = (action == PeepActionType::staffFix) ? 0x25 : 0x50;
        if (animationFrameNum != actionFrame)
        {
            return false;
        }

        Vehicle* vehicle = RideGetBrokenVehicle(ride);
        if (vehicle == nullptr)
        {
            return true;
        }

        vehicle->flags.unset(VehicleFlag::carIsBroken);

        return false;
    }

    /**
     * rct2: 0x006C107B
     * fixing SubState: fix_vehicle_malfunction - applies fixing to vehicle malfunction.
     * - see FixingSubstatesForBreakdown[]
     */
    bool Staff::updateFixingFixVehicleMalfunction(bool firstRun, const Ride& ride)
    {
        if (!firstRun)
        {
            orientation = peepDirection << 3;
            action = PeepActionType::staffFix3;
            animationImageIdOffset = 0;
            animationFrameNum = 0;

            updateCurrentAnimationType();
        }

        if (isActionWalking())
        {
            return true;
        }

        updateAction();
        invalidate();

        if (animationFrameNum != 0x65)
        {
            return false;
        }

        Vehicle* vehicle = RideGetBrokenVehicle(ride);
        if (vehicle == nullptr)
        {
            return true;
        }

        vehicle->flags.unset(VehicleFlag::trainIsBroken);

        return false;
    }

    /** rct2: 0x00992A3C */
    static constexpr CoordsXY kStationFixingOffsets[] = {
        { -12, 0 },
        { 0, 12 },
        { 12, 0 },
        { 0, -12 },
    };

    /**
     * rct2: 0x006C1114
     * fixing SubState: move_to_station_end - applies to fixing station specific breakdowns: safety cut-out, control failure,
     * inspection.
     * - see FixingSubstatesForBreakdown[]
     */
    bool Staff::updateFixingMoveToStationEnd(bool firstRun, const Ride& ride)
    {
        if (!firstRun)
        {
            if (ride.getRideTypeDescriptor().flags.has(RtdFlag::hasSinglePieceStation)
                || !ride.getRideTypeDescriptor().flags.has(RtdFlag::hasTrack))
            {
                return true;
            }

            auto stationPos = ride.getStation(currentRideStation).GetStart();
            if (stationPos.IsNull())
            {
                return true;
            }

            auto tileElement = MapGetTrackElementAt(stationPos);
            if (tileElement == nullptr)
            {
                LOG_ERROR("Couldn't find tile_element");
                return false;
            }

            int32_t trackDirection = tileElement->getDirection();
            CoordsXY offset = kStationFixingOffsets[trackDirection];

            stationPos.x += 16 + offset.x;
            if (offset.x == 0)
            {
                stationPos.x = getDestination().x;
            }

            stationPos.y += 16 + offset.y;
            if (offset.y == 0)
            {
                stationPos.y = getDestination().y;
            }

            setDestination(stationPos, 2);
        }

        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return false;
        }

        return true;
    }

    /**
     * rct2: 0x006C11F5
     * fixing SubState: fix_station_end - applies to fixing station specific breakdowns: safety cut-out, control failure,
     * inspection.
     * - see FixingSubstatesForBreakdown[]
     */
    bool Staff::updateFixingFixStationEnd(bool firstRun)
    {
        if (!firstRun)
        {
            orientation = peepDirection << 3;
            action = PeepActionType::staffCheckBoard;
            animationFrameNum = 0;
            animationImageIdOffset = 0;

            updateCurrentAnimationType();
        }

        if (isActionWalking())
        {
            return true;
        }

        updateAction();
        invalidate();

        return false;
    }

    /**
     * rct2: 0x006C1239
     * fixing SubState: move_to_station_start
     * 1. applies to fixing station specific breakdowns: safety cut-out, control failure,
     * 2. applies to fixing brake failure,
     * 3. applies to inspection.
     * - see FixingSubstatesForBreakdown[]
     */
    bool Staff::updateFixingMoveToStationStart(bool firstRun, const Ride& ride)
    {
        if (!firstRun)
        {
            if (ride.getRideTypeDescriptor().flags.has(RtdFlag::hasSinglePieceStation)
                || !ride.getRideTypeDescriptor().flags.has(RtdFlag::hasTrack))
            {
                return true;
            }

            auto stationPosition = ride.getStation(currentRideStation).GetStart();
            if (stationPosition.IsNull())
            {
                return true;
            }

            CoordsXYE input;
            input.x = stationPosition.x;
            input.y = stationPosition.y;
            input.element = MapGetTrackElementAtFromRide({ input.x, input.y, stationPosition.z }, currentRide);
            if (input.element == nullptr)
            {
                return true;
            }

            Direction stationDirection = 0;
            TrackBeginEnd trackBeginEnd;
            while (trackBlockGetPrevious(input, &trackBeginEnd))
            {
                if (trackBeginEnd.begin_element->asTrack()->isStation())
                {
                    input.x = trackBeginEnd.begin_x;
                    input.y = trackBeginEnd.begin_y;
                    input.element = trackBeginEnd.begin_element;

                    stationDirection = trackBeginEnd.begin_element->getDirection();
                    continue;
                }

                break;
            }

            // Loc6C12ED:
            auto destination = CoordsXY{ input.x + 16, input.y + 16 };
            auto offset = kStationFixingOffsets[stationDirection];

            destination.x -= offset.x;
            if (offset.x == 0)
            {
                destination.x = getDestination().x;
            }

            destination.y -= offset.y;
            if (offset.y == 0)
            {
                destination.y = getDestination().y;
            }

            setDestination(destination, 2);
        }

        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return false;
        }

        return true;
    }

    /**
     * rct2: 0x006C1368
     * fixing SubState: fix_station_start
     * 1. applies to fixing station specific breakdowns: safety cut-out, control failure,
     * 2. applies to inspection.
     * - see FixingSubstatesForBreakdown[]
     */
    bool Staff::updateFixingFixStationStart(bool firstRun, const Ride& ride)
    {
        if (!firstRun)
        {
            if (ride.getRideTypeDescriptor().flags.has(RtdFlag::hasSinglePieceStation)
                || !ride.getRideTypeDescriptor().flags.has(RtdFlag::hasTrack))
            {
                return true;
            }

            orientation = peepDirection << 3;

            action = PeepActionType::staffFix;
            animationFrameNum = 0;
            animationImageIdOffset = 0;

            updateCurrentAnimationType();
        }

        if (isActionWalking())
        {
            return true;
        }

        updateAction();

        return false;
    }

    /**
     * rct2: 0x006C13CE
     * fixing SubState: fix_station_brakes - applies to fixing brake failure
     * - see FixingSubstatesForBreakdown[]
     */
    bool Staff::updateFixingFixStationBrakes(bool firstRun, Ride& ride)
    {
        if (!firstRun)
        {
            orientation = peepDirection << 3;

            action = PeepActionType::staffFixGround;
            animationFrameNum = 0;
            animationImageIdOffset = 0;

            updateCurrentAnimationType();
        }

        if (isActionWalking())
        {
            return true;
        }

        updateAction();
        invalidate();

        if (animationFrameNum == 0x28)
        {
            ride.mechanicStatus = MechanicStatus::hasFixedStationBrakes;
            ride.windowInvalidateFlags.set(RideInvalidateFlag::maintenance);
        }

        if (animationFrameNum == 0x13 || animationFrameNum == 0x19 || animationFrameNum == 0x1F || animationFrameNum == 0x25
            || animationFrameNum == 0x2B)
        {
            Audio::Play3D(Audio::SoundId::mechanicFix, getLocation());
        }

        return false;
    }

    /**
     * rct2: 0x006C1474
     * fixing SubState: move_to_station_exit - applies to fixing all failures & inspections
     * - see FixingSubstatesForBreakdown[]
     */
    bool Staff::updateFixingMoveToStationExit(bool firstRun, const Ride& ride)
    {
        if (!firstRun)
        {
            auto stationPosition = ride.getStation(currentRideStation).Exit.ToCoordsXY();
            if (stationPosition.IsNull())
            {
                stationPosition = ride.getStation(currentRideStation).Entrance.ToCoordsXY();

                if (stationPosition.IsNull())
                {
                    return true;
                }
            }

            stationPosition = stationPosition.ToTileCentre();

            CoordsXY stationPlatformDirection = DirectionOffsets[peepDirection];
            stationPosition.x += stationPlatformDirection.x * 20;
            stationPosition.y += stationPlatformDirection.y * 20;

            setDestination(stationPosition, 2);
        }

        if (auto loc = updateAction(); loc.has_value())
        {
            moveTo({ loc.value(), z });
            return false;
        }

        return true;
    }

    /**
     * rct2: 0x006C1504
     * fixing SubState: finish_fix_or_inspect - applies to fixing all failures & inspections
     * - see FixingSubstatesForBreakdown[]
     */
    bool Staff::updateFixingFinishFixOrInspect(bool firstRun, int32_t steps, Ride& ride)
    {
        if (!firstRun)
        {
            if (state == PeepState::inspecting)
            {
                updateRideInspected(currentRide);

                staffRidesInspected = AddClamp(staffRidesInspected, 1u);
                windowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
                ride.mechanicStatus = MechanicStatus::undefined;
                return true;
            }

            staffRidesFixed = AddClamp(staffRidesFixed, 1u);
            windowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;

            orientation = peepDirection << 3;
            action = PeepActionType::staffAnswerCall2;
            animationFrameNum = 0;
            animationImageIdOffset = 0;

            updateCurrentAnimationType();
        }

        if (!isActionWalking())
        {
            updateAction();
            invalidate();
            return false;
        }

        RideFixBreakdown(ride, steps);
        ride.mechanicStatus = MechanicStatus::undefined;
        return true;
    }

    /**
     * rct2: 0x006C157E
     * fixing SubState: leave_by_entrance_exit - applies to fixing all failures & inspections
     * - see FixingSubstatesForBreakdown[]
     */
    bool Staff::updateFixingLeaveByEntranceExit(bool firstRun, const Ride& ride)
    {
        if (!firstRun)
        {
            auto exitPosition = ride.getStation(currentRideStation).Exit.ToCoordsXY();
            if (exitPosition.IsNull())
            {
                exitPosition = ride.getStation(currentRideStation).Entrance.ToCoordsXY();

                if (exitPosition.IsNull())
                {
                    setState(PeepState::falling);
                    return false;
                }
            }

            exitPosition = exitPosition.ToTileCentre();

            CoordsXY ebx_direction = DirectionOffsets[peepDirection];
            exitPosition.x -= ebx_direction.x * 19;
            exitPosition.y -= ebx_direction.y * 19;

            setDestination(exitPosition, 2);
        }

        int16_t xy_distance;
        if (auto loc = updateAction(xy_distance); loc.has_value())
        {
            auto stationHeight = ride.getStation(currentRideStation).GetBaseZ();
            if (xy_distance >= 16)
            {
                stationHeight += ride.getRideTypeDescriptor().Heights.PlatformHeight;
            }

            moveTo({ loc.value(), stationHeight });
            return false;
        }
        setState(PeepState::falling);
        return false;
    }

    /**
     * rct2: 0x6B7588
     */
    void Staff::updateRideInspected(RideId rideIndex)
    {
        auto ride = GetRide(rideIndex);
        if (ride != nullptr)
        {
            ride->flags.unset(RideFlag::dueInspection);
            ride->reliability += ((100 - ride->reliabilityPercentage) / 4) * (ScenarioRand() & 0xFF);
            ride->lastInspection = 0;
            ride->windowInvalidateFlags.set(
                RideInvalidateFlag::maintenance, RideInvalidateFlag::main, RideInvalidateFlag::list);
        }
    }

    money64 GetStaffWage(StaffType type)
    {
        switch (type)
        {
            default:
            case StaffType::handyman:
                return 50.00_GBP;
            case StaffType::mechanic:
                return 80.00_GBP;
            case StaffType::security:
                return 60.00_GBP;
            case StaffType::entertainer:
                return 55.00_GBP;
        }
    }

    void Staff::serialise(DataSerialiser& stream)
    {
        Peep::serialise(stream);
        stream << assignedStaffType;
        stream << mechanicTimeSinceCall;
        stream << hireDate;
        stream << staffOrders;
        stream << staffMowingTimeout;
        stream << staffLawnsMown;      // union with staffRidesFixed, staffGuestsEntertained
        stream << staffGardensWatered; // union with staffRidesInspected
        stream << staffLitterSwept;    // union with staffVandalsStopped
        stream << staffBinsEmptied;
    }
} // namespace OpenRCT2
