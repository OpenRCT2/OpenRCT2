/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Staff.h"

#include "../Context.h"
#include "../Game.h"
#include "../Input.h"
#include "../actions/StaffSetOrdersAction.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/DataSerialiser.h"
#include "../entity/EntityRegistry.h"
#include "../interface/Viewport.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../object/FootpathItemEntry.h"
#include "../object/ObjectEntryManager.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/SceneryGroupEntry.h"
#include "../object/SmallSceneryEntry.h"
#include "../object/TerrainSurfaceObject.h"
#include "../paint/tile_element/Paint.TileElement.h"
#include "../peep/GuestPathfinding.h"
#include "../ride/RideData.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"
#include "PatrolArea.h"
#include "Peep.h"

#include <algorithm>
#include <iterator>

// clang-format off
const StringId StaffCostumeNames[] = {
        STR_STAFF_OPTION_COSTUME_PANDA,
        STR_STAFF_OPTION_COSTUME_TIGER,
        STR_STAFF_OPTION_COSTUME_ELEPHANT,
        STR_STAFF_OPTION_COSTUME_ROMAN,
        STR_STAFF_OPTION_COSTUME_GORILLA,
        STR_STAFF_OPTION_COSTUME_SNOWMAN,
        STR_STAFF_OPTION_COSTUME_KNIGHT,
        STR_STAFF_OPTION_COSTUME_ASTRONAUT,
        STR_STAFF_OPTION_COSTUME_BANDIT,
        STR_STAFF_OPTION_COSTUME_SHERIFF,
        STR_STAFF_OPTION_COSTUME_PIRATE,
};
// clang-format on

colour_t gStaffHandymanColour;
colour_t gStaffMechanicColour;
colour_t gStaffSecurityColour;

// Maximum manhattan distance that litter can be for a handyman to seek to it
const uint16_t MAX_LITTER_DISTANCE = 3 * COORDS_XY_STEP;

template<> bool EntityBase::Is<Staff>() const
{
    return Type == EntityType::Staff;
}

/**
 *
 *  rct2: 0x006C0905
 */
bool Staff::IsLocationInPatrol(const CoordsXY& loc) const
{
    // Check if location is in the park
    if (!MapIsLocationOwnedOrHasRights(loc))
        return false;

    // Check if staff has patrol area
    if (!HasPatrolArea())
        return true;

    return IsPatrolAreaSet(loc);
}

// Check whether the location x,y is inside and on the edge of the
// patrol zone for mechanic.
bool Staff::IsLocationOnPatrolEdge(const CoordsXY& loc) const
{
    bool onZoneEdge = false;
    for (uint8_t neighbourDir = 0; !onZoneEdge && neighbourDir <= 7; neighbourDir++)
    {
        auto neighbourPos = loc + CoordsDirectionDelta[neighbourDir];
        onZoneEdge = !IsLocationInPatrol(neighbourPos);
    }
    return onZoneEdge;
}

bool Staff::CanIgnoreWideFlag(const CoordsXYZ& staffPos, TileElement* path) const
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

    if (!IsLocationOnPatrolEdge(staffPos))
    {
        return false;
    }

    /* Check the connected adjacent paths that are also inside the patrol
     * zone but are not on the patrol zone edge have the wide flag set. */
    uint8_t total = 0;
    uint8_t pathcount = 0;
    uint8_t widecount = 0;
    for (Direction adjac_dir : ALL_DIRECTIONS)
    {
        auto adjacPos = staffPos + CoordsXYZ{ CoordsDirectionDelta[adjac_dir].x, CoordsDirectionDelta[adjac_dir].y, 0 };

        /* Ignore adjacent tiles outside the patrol zone. */
        if (!IsLocationInPatrol(adjacPos))
            continue;

        /* Ignore adjacent tiles on the patrol zone edge. */
        if (IsLocationOnPatrolEdge(adjacPos))
            continue;

        /* Adjacent tile is inside the patrol zone but not on the
         * patrol zone edge. */
        total++;

        /* Check if path has an edge in adjac_dir */
        if (!(path->AsPath()->GetEdges() & (1u << adjac_dir)))
        {
            continue;
        }

        if (path->AsPath()->IsSloped())
        {
            if (path->AsPath()->GetSlopeDirection() == adjac_dir)
            {
                adjacPos.z += PATH_HEIGHT_STEP;
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
            if (test_element->GetType() != TileElementType::Path)
            {
                continue;
            }

            /* test_element is a path */
            if (!GuestPathfinding::IsValidPathZAndDirection(test_element, adjacPos.z / COORDS_Z_STEP, adjac_dir))
                continue;

            /* test_element is a connected path */
            if (!pathfound)
            {
                pathfound = true;
                pathcount++;
            }

            if (test_element->AsPath()->IsWide())
            {
                if (!widefound)
                {
                    widefound = true;
                    widecount++;
                }
            }
        } while (!(test_element++)->IsLastForTile());
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
uint8_t Staff::GetValidPatrolDirections(const CoordsXY& loc) const
{
    uint8_t directions = 0;

    if (IsLocationInPatrol({ loc.x - COORDS_XY_STEP, loc.y }))
    {
        directions |= (1 << 0);
    }

    if (IsLocationInPatrol({ loc.x, loc.y + COORDS_XY_STEP }))
    {
        directions |= (1 << 1);
    }

    if (IsLocationInPatrol({ loc.x + COORDS_XY_STEP, loc.y }))
    {
        directions |= (1 << 2);
    }

    if (IsLocationInPatrol({ loc.x, loc.y - COORDS_XY_STEP }))
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
void Staff::ResetStats()
{
    for (auto peep : EntityList<Staff>())
    {
        peep->SetHireDate(gDate.GetMonthsElapsed());
        peep->StaffLawnsMown = 0;
        peep->StaffRidesFixed = 0;
        peep->StaffGardensWatered = 0;
        peep->StaffRidesInspected = 0;
        peep->StaffLitterSwept = 0;
        peep->StaffVandalsStopped = 0;
        peep->StaffBinsEmptied = 0;
    }
}

bool Staff::IsPatrolAreaSet(const CoordsXY& coords) const
{
    if (PatrolInfo != nullptr)
    {
        return PatrolInfo->Get(coords);
    }
    return false;
}

void Staff::SetPatrolArea(const CoordsXY& coords, bool value)
{
    if (PatrolInfo == nullptr)
    {
        if (value)
        {
            PatrolInfo = new PatrolArea();
        }
        else
        {
            return;
        }
    }

    PatrolInfo->Set(coords, value);
}

void Staff::SetPatrolArea(const MapRange& range, bool value)
{
    for (int32_t yy = range.GetTop(); yy <= range.GetBottom(); yy += COORDS_XY_STEP)
    {
        for (int32_t xx = range.GetLeft(); xx <= range.GetRight(); xx += COORDS_XY_STEP)
        {
            SetPatrolArea({ xx, yy }, value);
        }
    }
}

void Staff::ClearPatrolArea()
{
    delete PatrolInfo;
    PatrolInfo = nullptr;
}

bool Staff::HasPatrolArea() const
{
    return PatrolInfo == nullptr ? false : !PatrolInfo->IsEmpty();
}

/**
 *
 *  rct2: 0x006BFBE8
 *
 * Returns INVALID_DIRECTION when no nearby litter or unpathable litter
 */
Direction Staff::HandymanDirectionToNearestLitter() const
{
    uint16_t nearestLitterDist = 0xFFFF;
    Litter* nearestLitter = nullptr;
    for (auto litter : EntityList<Litter>())
    {
        uint16_t distance = abs(litter->x - x) + abs(litter->y - y) + abs(litter->z - z) * 4;

        if (distance < nearestLitterDist)
        {
            nearestLitterDist = distance;
            nearestLitter = litter;
        }
    }

    if (nearestLitterDist > MAX_LITTER_DISTANCE)
    {
        return INVALID_DIRECTION;
    }

    auto litterTile = CoordsXY{ nearestLitter->x, nearestLitter->y }.ToTileStart();

    if (!IsLocationInPatrol(litterTile))
    {
        return INVALID_DIRECTION;
    }

    Direction nextDirection = DirectionFromTo(CoordsXY(x, y), litterTile.ToTileCentre());

    CoordsXY nextTile = litterTile.ToTileStart() - CoordsDirectionDelta[nextDirection];

    int16_t nextZ = ((z + COORDS_Z_STEP) & 0xFFF0) / COORDS_Z_STEP;

    TileElement* tileElement = MapGetFirstElementAt(nextTile);
    if (tileElement == nullptr)
        return INVALID_DIRECTION;
    do
    {
        if (tileElement->BaseHeight != nextZ)
            continue;
        if (tileElement->GetType() == TileElementType::Entrance || tileElement->GetType() == TileElementType::Track)
        {
            return INVALID_DIRECTION;
        }
    } while (!(tileElement++)->IsLastForTile());

    nextTile = CoordsXY(x, y).ToTileStart() + CoordsDirectionDelta[nextDirection];

    tileElement = MapGetFirstElementAt(nextTile);
    if (tileElement == nullptr)
        return INVALID_DIRECTION;

    do
    {
        if (tileElement->BaseHeight != nextZ)
            continue;
        if (tileElement->GetType() == TileElementType::Entrance || tileElement->GetType() == TileElementType::Track)
        {
            return INVALID_DIRECTION;
        }
    } while (!(tileElement++)->IsLastForTile());

    return nextDirection;
}

/**
 *
 *  rct2: 0x006BF931
 */
uint8_t Staff::HandymanDirectionToUncutGrass(uint8_t valid_directions) const
{
    if (!(GetNextIsSurface()))
    {
        auto surfaceElement = MapGetSurfaceElementAt(NextLoc);
        if (surfaceElement == nullptr)
            return INVALID_DIRECTION;

        if (NextLoc.z != surfaceElement->GetBaseZ())
            return INVALID_DIRECTION;

        if (GetNextIsSloped())
        {
            if (surfaceElement->GetSlope() != PathSlopeToLandSlope[GetNextDirection()])
                return INVALID_DIRECTION;
        }
        else if (surfaceElement->GetSlope() != TILE_ELEMENT_SLOPE_FLAT)
            return INVALID_DIRECTION;
    }

    uint8_t chosenDirection = ScenarioRand() & 0x3;
    for (uint8_t i = 0; i < 4; ++i, ++chosenDirection)
    {
        chosenDirection &= 0x3;

        if (!(valid_directions & (1 << chosenDirection)))
        {
            continue;
        }

        CoordsXY chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[chosenDirection];

        if (!MapIsLocationValid(chosenTile))
            continue;

        auto surfaceElement = MapGetSurfaceElementAt(chosenTile);
        if (surfaceElement != nullptr)
        {
            if (std::abs(surfaceElement->GetBaseZ() - NextLoc.z) <= 2 * COORDS_Z_STEP)
            {
                if (surfaceElement->CanGrassGrow() && (surfaceElement->GetGrassLength() & 0x7) >= GRASS_LENGTH_CLEAR_1)
                {
                    return chosenDirection;
                }
            }
        }
    }
    return INVALID_DIRECTION;
}

/**
 *
 *  rct2: 0x006BFD9C
 */
Direction Staff::HandymanDirectionRandSurface(uint8_t validDirections) const
{
    Direction newDirection = ScenarioRand() % NumOrthogonalDirections;
    for (int32_t i = 0; i < NumOrthogonalDirections; ++i, ++newDirection)
    {
        newDirection %= NumOrthogonalDirections;
        if (!(validDirections & (1 << newDirection)))
            continue;

        CoordsXY chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];

        if (MapSurfaceIsBlocked(chosenTile))
            continue;

        break;
    }
    // If it tries all directions this is required
    // to make it back to the first direction and
    // override validDirections
    newDirection %= NumOrthogonalDirections;
    return newDirection;
}

/**
 *
 *  rct2: 0x006BFBA8
 */
bool Staff::DoHandymanPathFinding()
{
    StaffMowingTimeout++;

    Direction litterDirection = INVALID_DIRECTION;
    uint8_t validDirections = GetValidPatrolDirections(NextLoc);

    if ((StaffOrders & STAFF_ORDERS_SWEEPING) && ((gCurrentTicks + Id.ToUnderlying()) & 0xFFF) > 110)
    {
        litterDirection = HandymanDirectionToNearestLitter();
    }

    Direction newDirection = INVALID_DIRECTION;
    if (litterDirection == INVALID_DIRECTION && (StaffOrders & STAFF_ORDERS_MOWING) && StaffMowingTimeout >= 12)
    {
        newDirection = HandymanDirectionToUncutGrass(validDirections);
    }

    if (newDirection == INVALID_DIRECTION)
    {
        if (GetNextIsSurface())
        {
            newDirection = HandymanDirectionRandSurface(validDirections);
        }
        else
        {
            auto* pathElement = MapGetPathElementAt(TileCoordsXYZ{ NextLoc });

            if (pathElement == nullptr)
                return true;

            uint8_t pathDirections = (pathElement->GetEdges() & validDirections) & 0xF;
            if (pathDirections == 0)
            {
                newDirection = HandymanDirectionRandSurface(validDirections);
            }
            else
            {
                bool chooseRandom = true;
                if (litterDirection != INVALID_DIRECTION && pathDirections & (1 << litterDirection))
                {
                    /// Check whether path is a queue path and connected to a ride
                    bool connectedQueue = (pathElement->IsQueue() && !pathElement->GetRideIndex().IsNull());
                    /// When in a queue path make the probability of following litter much lower (10% instead of 90%)
                    /// as handymen often get stuck when there is litter on a normal path next to a queue they are in
                    uint32_t chooseRandomProbability = connectedQueue ? 0xE666 : 0x1999;
                    if ((ScenarioRand() & 0xFFFF) >= chooseRandomProbability)
                    {
                        chooseRandom = false;
                        newDirection = litterDirection;
                    }
                }
                else
                {
                    pathDirections &= ~(1 << DirectionReverse(PeepDirection));
                    if (pathDirections == 0)
                    {
                        pathDirections |= 1 << DirectionReverse(PeepDirection);
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

    CoordsXY chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];

    while (!MapIsLocationValid(chosenTile))
    {
        newDirection = HandymanDirectionRandSurface(validDirections);
        chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];
    }

    PeepDirection = newDirection;
    SetDestination(chosenTile + CoordsXY{ 16, 16 }, 3);
    if (State == PeepState::Queuing)
    {
        DestinationTolerance = (ScenarioRand() & 7) + 2;
    }
    return false;
}

Direction Staff::DirectionSurface(Direction initialDirection) const
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

        if (WallInTheWay({ NextLoc, NextLoc.z, NextLoc.z + PEEP_CLEARANCE_HEIGHT }, direction))
            continue;

        if (WallInTheWay({ NextLoc, NextLoc.z, NextLoc.z + PEEP_CLEARANCE_HEIGHT }, DirectionReverse(direction)))
            continue;

        CoordsXY chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[direction];

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
Direction Staff::MechanicDirectionSurface() const
{
    Direction direction = ScenarioRand() & 3;

    auto ride = GetRide(CurrentRide);
    if (ride != nullptr && (State == PeepState::Answering || State == PeepState::HeadingToInspection) && (ScenarioRand() & 1))
    {
        auto location = ride->GetStation(CurrentRideStation).Exit;
        if (location.IsNull())
        {
            location = ride->GetStation(CurrentRideStation).Entrance;
        }

        direction = DirectionFromTo(CoordsXY(x, y), location.ToCoordsXY());
    }

    return DirectionSurface(direction);
}

/**
 *
 *  rct2: 0x006C02D1
 */
Direction Staff::MechanicDirectionPathRand(uint8_t pathDirections) const
{
    if (ScenarioRand() & 1)
    {
        if (pathDirections & (1 << PeepDirection))
            return PeepDirection;
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
    return PeepDirection;
}

/**
 *
 *  rct2: 0x006C0121
 */
Direction Staff::MechanicDirectionPath(uint8_t validDirections, PathElement* pathElement)
{
    uint8_t pathDirections = pathElement->GetEdges();
    pathDirections &= validDirections;

    if (pathDirections == 0)
    {
        return MechanicDirectionSurface();
    }

    // Check if this is dead end - i.e. only way out is the reverse direction.
    pathDirections &= ~(1 << DirectionReverse(PeepDirection));
    if (pathDirections == 0)
    {
        pathDirections |= (1 << DirectionReverse(PeepDirection));
    }

    Direction direction = UtilBitScanForward(pathDirections);
    pathDirections &= ~(1 << direction);
    if (pathDirections == 0)
    {
        if (State != PeepState::Answering && State != PeepState::HeadingToInspection)
        {
            return direction;
        }

        if (SubState != 2)
        {
            return direction;
        }
        SubState = 3;
    }

    pathDirections |= (1 << direction);

    // Mechanic is heading to ride (either broken down or for inspection).
    auto ride = GetRide(CurrentRide);
    if (ride != nullptr && (State == PeepState::Answering || State == PeepState::HeadingToInspection))
    {
        /* Find location of the exit for the target ride station
         * or if the ride has no exit, the entrance. */
        TileCoordsXYZD location = ride->GetStation(CurrentRideStation).Exit;
        if (location.IsNull())
        {
            location = ride->GetStation(CurrentRideStation).Entrance;

            // If no entrance is present either. This is an incorrect state.
            if (location.IsNull())
            {
                return MechanicDirectionPathRand(pathDirections);
            }
        }

        gPeepPathFindGoalPosition.x = location.x;
        gPeepPathFindGoalPosition.y = location.y;
        gPeepPathFindGoalPosition.z = location.z;

        gPeepPathFindIgnoreForeignQueues = false;
        gPeepPathFindQueueRideIndex = RideId::GetNull();

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        PathfindLoggingEnable(*this);
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

        Direction pathfindDirection = gGuestPathfinder->ChooseDirection(TileCoordsXYZ{ NextLoc }, *this);

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        PathfindLoggingDisable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

        if (pathfindDirection == INVALID_DIRECTION)
        {
            /* Heuristic search failed for all directions.
             * Reset the PathfindGoal - this means that the PathfindHistory
             * will be reset in the next call to GuestPathfinding::ChooseDirection().
             * This lets the heuristic search "try again" in case the player has
             * edited the path layout or the mechanic was already stuck in the
             * save game (e.g. with a worse version of the pathfinding). */
            ResetPathfindGoal();
            return MechanicDirectionPathRand(pathDirections);
        }

        return pathfindDirection;
    }
    return MechanicDirectionPathRand(pathDirections);
}

/**
 *
 *  rct2: 0x006BFF2C
 */
bool Staff::DoMechanicPathFinding()
{
    uint8_t validDirections = GetValidPatrolDirections(NextLoc);
    Direction newDirection = INVALID_DIRECTION;
    if (GetNextIsSurface())
    {
        newDirection = MechanicDirectionSurface();
    }
    else
    {
        auto* pathElement = MapGetPathElementAt(TileCoordsXYZ{ NextLoc });
        if (pathElement == nullptr)
            return true;

        newDirection = MechanicDirectionPath(validDirections, pathElement);
    }

    // countof(CoordsDirectionDelta)
    assert(DirectionValid(newDirection));

    CoordsXY chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];

    while (!MapIsLocationValid(chosenTile))
    {
        newDirection = MechanicDirectionSurface();
        chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];
    }

    PeepDirection = newDirection;
    auto tolerance = (ScenarioRand() & 7) + 2;
    SetDestination(chosenTile + CoordsXY{ 16, 16 }, tolerance);

    return false;
}

/**
 *
 *  rct2: 0x006C050B
 */
Direction Staff::DirectionPath(uint8_t validDirections, PathElement* pathElement) const
{
    uint8_t pathDirections = pathElement->GetEdges();
    if (State != PeepState::Answering && State != PeepState::HeadingToInspection)
    {
        pathDirections &= validDirections;
    }

    if (pathDirections == 0)
    {
        return DirectionSurface(ScenarioRand() & 3);
    }

    pathDirections &= ~(1 << DirectionReverse(PeepDirection));
    if (pathDirections == 0)
    {
        pathDirections |= (1 << DirectionReverse(PeepDirection));
    }

    Direction direction = UtilBitScanForward(pathDirections);
    // If this is the only direction they can go, then go
    if (pathDirections == (1 << direction))
    {
        return direction;
    }

    direction = ScenarioRand() & 3;
    for (uint8_t i = 0; i < NumOrthogonalDirections; ++i, direction = DirectionNext(direction))
    {
        if (pathDirections & (1 << direction))
            return direction;
    }

    // This will never happen as pathDirections will always have a bit set
    return direction;
}

/**
 *
 *  rct2: 0x006C0351
 */
bool Staff::DoMiscPathFinding()
{
    uint8_t validDirections = GetValidPatrolDirections(NextLoc);

    Direction newDirection = INVALID_DIRECTION;
    if (GetNextIsSurface())
    {
        newDirection = DirectionSurface(ScenarioRand() & 3);
    }
    else
    {
        auto* pathElement = MapGetPathElementAt(TileCoordsXYZ{ NextLoc });
        if (pathElement == nullptr)
            return true;

        newDirection = DirectionPath(validDirections, pathElement);
    }

    CoordsXY chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];

    while (!MapIsLocationValid(chosenTile))
    {
        newDirection = DirectionSurface(ScenarioRand() & 3);
        chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];
    }

    PeepDirection = newDirection;
    auto tolerance = (ScenarioRand() & 7) + 2;
    SetDestination(chosenTile + CoordsXY{ 16, 16 }, tolerance);

    return false;
}

bool Staff::IsMechanicHeadingToFixRideBlockingPath()
{
    if (!IsMechanic())
        return false;

    auto tileCoords = TileCoordsXYZ(CoordsXYZ{ GetDestination(), NextLoc.z });
    auto trackElement = MapGetFirstTileElementWithBaseHeightBetween<TrackElement>(
        { tileCoords, tileCoords.z + PATH_HEIGHT_STEP });
    if (trackElement == nullptr)
        return false;

    auto ride = GetRide(trackElement->GetRideIndex());
    if (ride == nullptr)
        return false;

    return ride->id == CurrentRide && ride->breakdown_reason == BREAKDOWN_SAFETY_CUT_OUT;
}

/**
 *
 *  rct2: 0x006C086D
 */
void Staff::EntertainerUpdateNearbyPeeps() const
{
    for (auto guest : EntityList<Guest>())
    {
        if (guest->x == LOCATION_NULL)
            continue;

        int16_t z_dist = abs(z - guest->z);
        if (z_dist > 48)
            continue;

        int16_t x_dist = abs(x - guest->x);
        int16_t y_dist = abs(y - guest->y);

        if (x_dist > 96)
            continue;

        if (y_dist > 96)
            continue;

        if (guest->State == PeepState::Walking)
        {
            guest->HappinessTarget = std::min(guest->HappinessTarget + 4, PEEP_MAX_HAPPINESS);
        }
        else if (guest->State == PeepState::Queuing)
        {
            guest->TimeInQueue = std::max(0, guest->TimeInQueue - 200);
            guest->HappinessTarget = std::min(guest->HappinessTarget + 3, PEEP_MAX_HAPPINESS);
        }
    }
}

/**
 *
 *  rct2: 0x006C05AE
 */
bool Staff::DoEntertainerPathFinding()
{
    if (((ScenarioRand() & 0xFFFF) <= 0x4000) && IsActionInterruptable())
    {
        Action = (ScenarioRand() & 1) ? PeepActionType::Wave2 : PeepActionType::Joy;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;

        UpdateCurrentActionSpriteType();
        EntertainerUpdateNearbyPeeps();
    }

    return DoMiscPathFinding();
}

/**
 *
 *  rct2: 0x006BF926
 */
bool Staff::DoPathFinding()
{
    switch (AssignedStaffType)
    {
        case StaffType::Handyman:
            return DoHandymanPathFinding();
        case StaffType::Mechanic:
            return DoMechanicPathFinding();
        case StaffType::Security:
            return DoMiscPathFinding();
        case StaffType::Entertainer:
            return DoEntertainerPathFinding();

        default:
            assert(false);
            return 0;
    }
}

uint8_t Staff::GetCostume() const
{
    return EnumValue(SpriteType) - EnumValue(PeepSpriteType::EntertainerPanda);
}

void Staff::SetCostume(uint8_t value)
{
    auto costume = static_cast<EntertainerCostume>(value);
    SpriteType = EntertainerCostumeToSprite(costume);
}

void Staff::SetHireDate(int32_t hireDate)
{
    HireDate = hireDate;
}

int32_t Staff::GetHireDate() const
{
    return HireDate;
}

PeepSpriteType EntertainerCostumeToSprite(EntertainerCostume entertainerType)
{
    uint8_t value = static_cast<uint8_t>(entertainerType);
    PeepSpriteType newSpriteType = static_cast<PeepSpriteType>(value + EnumValue(PeepSpriteType::EntertainerPanda));
    return newSpriteType;
}

colour_t StaffGetColour(StaffType staffType)
{
    switch (staffType)
    {
        case StaffType::Handyman:
            return gStaffHandymanColour;
        case StaffType::Mechanic:
            return gStaffMechanicColour;
        case StaffType::Security:
            return gStaffSecurityColour;
        case StaffType::Entertainer:
            return 0;
        default:
            assert(false);
            return 0;
    }
}

bool StaffSetColour(StaffType staffType, colour_t value)
{
    switch (staffType)
    {
        case StaffType::Handyman:
            gStaffHandymanColour = value;
            break;
        case StaffType::Mechanic:
            gStaffMechanicColour = value;
            break;
        case StaffType::Security:
            gStaffSecurityColour = value;
            break;
        default:
            return false;
    }
    return true;
}

uint32_t StaffGetAvailableEntertainerCostumes()
{
    uint32_t entertainerCostumes = 0;
    for (int32_t i = 0; i < MAX_SCENERY_GROUP_OBJECTS; i++)
    {
        if (SceneryGroupIsInvented(i))
        {
            const auto sgEntry = OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(i);
            entertainerCostumes |= sgEntry->entertainer_costumes;
        }
    }

    // For some reason the flags are +4 from the actual costume IDs
    entertainerCostumes >>= 4;

    // Fix #6593: force enable the default costumes, which normally get enabled through the default scenery groups.
    entertainerCostumes |= (1 << static_cast<uint8_t>(EntertainerCostume::Panda))
        | (1 << static_cast<uint8_t>(EntertainerCostume::Tiger)) | (1 << static_cast<uint8_t>(EntertainerCostume::Elephant));

    return entertainerCostumes;
}

int32_t StaffGetAvailableEntertainerCostumeList(EntertainerCostume* costumeList)
{
    uint32_t availableCostumes = StaffGetAvailableEntertainerCostumes();
    int32_t numCostumes = 0;
    for (uint8_t i = 0; i < static_cast<uint8_t>(EntertainerCostume::Count); i++)
    {
        if (availableCostumes & (1 << i))
        {
            costumeList[numCostumes++] = static_cast<EntertainerCostume>(i);
        }
    }
    return numCostumes;
}

/** rct2: 0x009929C8 */
static constexpr const CoordsXY _MowingWaypoints[] = {
    { 28, 28 }, { 28, 4 }, { 20, 4 }, { 20, 28 }, { 12, 28 }, { 12, 4 }, { 4, 4 }, { 4, 28 },
};

/**
 *
 *  rct2: 0x006BF567
 */
void Staff::UpdateMowing()
{
    if (!CheckForPath())
        return;

    while (true)
    {
        if (auto loc = UpdateAction(); loc.has_value())
        {
            int16_t checkZ = TileElementHeight(*loc);
            MoveTo({ loc.value(), checkZ });
            return;
        }

        Var37++;

        if (Var37 == 1)
        {
            SwitchToSpecialSprite(2);
        }

        if (Var37 == std::size(_MowingWaypoints))
        {
            StateReset();
            return;
        }

        auto destination = _MowingWaypoints[Var37] + NextLoc;
        SetDestination(destination);

        if (Var37 != 7)
            continue;

        auto surfaceElement = MapGetSurfaceElementAt(NextLoc);
        if (surfaceElement != nullptr && surfaceElement->CanGrassGrow())
        {
            surfaceElement->SetGrassLength(GRASS_LENGTH_MOWED);
            MapInvalidateTileZoom0({ NextLoc, surfaceElement->GetBaseZ(), surfaceElement->GetBaseZ() + 16 });
        }
        StaffLawnsMown++;
        WindowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
    }
}

/**
 *
 *  rct2: 0x006BF7E6
 */
void Staff::UpdateWatering()
{
    StaffMowingTimeout = 0;
    if (SubState == 0)
    {
        if (!CheckForPath())
            return;

        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        sprite_direction = (Var37 & 3) << 3;
        Action = PeepActionType::StaffWatering;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;
        UpdateCurrentActionSpriteType();

        SubState = 1;
    }
    else if (SubState == 1)
    {
        if (!IsActionWalking())
        {
            UpdateAction();
            Invalidate();
            return;
        }

        auto actionLoc = CoordsXY{ NextLoc } + CoordsDirectionDelta[Var37];

        TileElement* tile_element = MapGetFirstElementAt(actionLoc);
        if (tile_element == nullptr)
            return;

        do
        {
            if (tile_element->GetType() != TileElementType::SmallScenery)
                continue;

            if (abs(NextLoc.z - tile_element->GetBaseZ()) > 4 * COORDS_Z_STEP)
                continue;

            const auto* sceneryEntry = tile_element->AsSmallScenery()->GetEntry();

            if (sceneryEntry == nullptr || !sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_CAN_BE_WATERED))
                continue;

            tile_element->AsSmallScenery()->SetAge(0);
            MapInvalidateTileZoom0({ actionLoc, tile_element->GetBaseZ(), tile_element->GetClearanceZ() });
            StaffGardensWatered++;
            WindowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
        } while (!(tile_element++)->IsLastForTile());

        StateReset();
    }
}

/**
 *
 *  rct2: 0x006BF6C9
 */
void Staff::UpdateEmptyingBin()
{
    StaffMowingTimeout = 0;

    if (SubState == 0)
    {
        if (!CheckForPath())
            return;

        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        sprite_direction = (Var37 & 3) << 3;
        Action = PeepActionType::StaffEmptyBin;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;
        UpdateCurrentActionSpriteType();

        SubState = 1;
    }
    else if (SubState == 1)
    {
        if (IsActionWalking())
        {
            StateReset();
            return;
        }

        UpdateAction();
        Invalidate();

        if (ActionFrame != 11)
            return;

        TileElement* tile_element = MapGetFirstElementAt(NextLoc);
        if (tile_element == nullptr)
            return;

        for (;; tile_element++)
        {
            if (tile_element->GetType() == TileElementType::Path)
            {
                if (NextLoc.z == tile_element->GetBaseZ())
                    break;
            }
            if ((tile_element)->IsLastForTile())
            {
                StateReset();
                return;
            }
        }

        if (!tile_element->AsPath()->HasAddition())
        {
            StateReset();
            return;
        }

        auto* pathAddEntry = tile_element->AsPath()->GetAdditionEntry();
        if (!(pathAddEntry->flags & PATH_BIT_FLAG_IS_BIN) || tile_element->AsPath()->IsBroken()
            || tile_element->AsPath()->AdditionIsGhost())
        {
            StateReset();
            return;
        }

        uint8_t additionStatus = tile_element->AsPath()->GetAdditionStatus() | ((3 << Var37) << Var37);
        tile_element->AsPath()->SetAdditionStatus(additionStatus);

        MapInvalidateTileZoom0({ NextLoc, tile_element->GetBaseZ(), tile_element->GetClearanceZ() });
        StaffBinsEmptied++;
        WindowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
    }
}

/**
 *
 *  rct2: 0x6BF641
 */
void Staff::UpdateSweeping()
{
    StaffMowingTimeout = 0;
    if (!CheckForPath())
        return;

    if (Action == PeepActionType::StaffSweep && ActionFrame == 8)
    {
        // Remove sick at this location
        Litter::RemoveAt(GetLocation());
        StaffLitterSwept++;
        WindowInvalidateFlags |= PEEP_INVALIDATE_STAFF_STATS;
    }
    if (auto loc = UpdateAction(); loc.has_value())
    {
        int16_t actionZ = GetZOnSlope((*loc).x, (*loc).y);
        MoveTo({ loc.value(), actionZ });
        return;
    }

    Var37++;
    if (Var37 != 2)
    {
        Action = PeepActionType::StaffSweep;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;
        UpdateCurrentActionSpriteType();
        return;
    }
    StateReset();
}

/**
 *
 *  rct2: 0x006C16D7
 */
void Staff::UpdateHeadingToInspect()
{
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr)
    {
        SetState(PeepState::Falling);
        return;
    }

    if (ride->GetStation(CurrentRideStation).Exit.IsNull())
    {
        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
        SetState(PeepState::Falling);
        return;
    }

    if (ride->mechanic_status != RIDE_MECHANIC_STATUS_HEADING || !(ride->lifecycle_flags & RIDE_LIFECYCLE_DUE_INSPECTION))
    {
        SetState(PeepState::Falling);
        return;
    }

    if (SubState == 0)
    {
        MechanicTimeSinceCall = 0;
        ResetPathfindGoal();
        SubState = 2;
    }

    if (SubState <= 3)
    {
        MechanicTimeSinceCall++;
        if (MechanicTimeSinceCall > 2500)
        {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_DUE_INSPECTION && ride->mechanic_status == RIDE_MECHANIC_STATUS_HEADING)
            {
                ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            }
            SetState(PeepState::Falling);
            return;
        }

        if (!CheckForPath())
            return;

        if (ShouldWaitForLevelCrossing() && !IsMechanicHeadingToFixRideBlockingPath())
            return;

        uint8_t pathingResult;
        TileElement* rideEntranceExitElement;
        PerformNextAction(pathingResult, rideEntranceExitElement);

        if (!(pathingResult & PATHING_RIDE_EXIT) && !(pathingResult & PATHING_RIDE_ENTRANCE))
        {
            return;
        }

        if (CurrentRide != rideEntranceExitElement->AsEntrance()->GetRideIndex())
            return;

        StationIndex exitIndex = rideEntranceExitElement->AsEntrance()->GetStationIndex();
        if (CurrentRideStation != exitIndex)
            return;

        if (pathingResult & PATHING_RIDE_ENTRANCE)
        {
            if (!ride->GetStation(exitIndex).Exit.IsNull())
            {
                return;
            }
        }

        PeepDirection = rideEntranceExitElement->GetDirection();

        auto newDestination = CoordsXY{ 16, 16 } + NextLoc + (DirectionOffsets[PeepDirection] * 53);
        SetDestination(newDestination, 2);
        sprite_direction = PeepDirection << 3;

        z = rideEntranceExitElement->BaseHeight * 4;
        SubState = 4;
        // Falls through into SubState 4
    }

    int16_t delta_y = abs(GetLocation().y - GetDestination().y);
    if (auto loc = UpdateAction(); loc.has_value())
    {
        auto newZ = ride->GetStation(CurrentRideStation).GetBaseZ();
        if (delta_y < 20)
        {
            newZ += ride->GetRideTypeDescriptor().Heights.PlatformHeight;
        }

        MoveTo({ loc.value(), newZ });
        return;
    }

    SetState(PeepState::Inspecting);
    SubState = 0;
}

/**
 *
 *  rct2: 0x006C0CB8
 */
void Staff::UpdateAnswering()
{
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr || ride->mechanic_status != RIDE_MECHANIC_STATUS_HEADING)
    {
        SetState(PeepState::Falling);
        return;
    }

    if (SubState == 0)
    {
        Action = PeepActionType::StaffAnswerCall;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;

        UpdateCurrentActionSpriteType();

        SubState = 1;
        PeepWindowStateUpdate(this);
        return;
    }
    if (SubState == 1)
    {
        if (IsActionWalking())
        {
            SubState = 2;
            PeepWindowStateUpdate(this);
            MechanicTimeSinceCall = 0;
            ResetPathfindGoal();
            return;
        }
        UpdateAction();
        Invalidate();
        return;
    }
    if (SubState <= 3)
    {
        MechanicTimeSinceCall++;
        if (MechanicTimeSinceCall > 2500)
        {
            ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
            SetState(PeepState::Falling);
            return;
        }

        if (!CheckForPath())
            return;

        if (ShouldWaitForLevelCrossing() && !IsMechanicHeadingToFixRideBlockingPath())
            return;

        uint8_t pathingResult;
        TileElement* rideEntranceExitElement;
        PerformNextAction(pathingResult, rideEntranceExitElement);

        if (!(pathingResult & PATHING_RIDE_EXIT) && !(pathingResult & PATHING_RIDE_ENTRANCE))
        {
            return;
        }

        if (CurrentRide != rideEntranceExitElement->AsEntrance()->GetRideIndex())
            return;

        StationIndex exitIndex = rideEntranceExitElement->AsEntrance()->GetStationIndex();
        if (CurrentRideStation != exitIndex)
            return;

        if (pathingResult & PATHING_RIDE_ENTRANCE)
        {
            if (!ride->GetStation(exitIndex).Exit.IsNull())
            {
                return;
            }
        }

        PeepDirection = rideEntranceExitElement->GetDirection();

        int32_t destX = NextLoc.x + 16 + DirectionOffsets[PeepDirection].x * 53;
        int32_t destY = NextLoc.y + 16 + DirectionOffsets[PeepDirection].y * 53;

        SetDestination({ destX, destY }, 2);
        sprite_direction = PeepDirection << 3;

        z = rideEntranceExitElement->BaseHeight * 4;
        SubState = 4;
        // Falls through into SubState 4
    }

    int16_t delta_y = abs(y - GetDestination().y);
    if (auto loc = UpdateAction(); loc.has_value())
    {
        auto newZ = ride->GetStation(CurrentRideStation).GetBaseZ();
        if (delta_y < 20)
        {
            newZ += ride->GetRideTypeDescriptor().Heights.PlatformHeight;
        }

        MoveTo({ loc.value(), newZ });
        return;
    }

    SetState(PeepState::Fixing);
    SubState = 0;
}

/** rct2: 0x00992A5C */
static constexpr const CoordsXY _WateringUseOffsets[] = {
    { 3, 16 }, { 16, 29 }, { 29, 16 }, { 16, 3 }, { 3, 29 }, { 29, 29 }, { 29, 3 }, { 3, 3 },
};

/**
 *
 *  rct2: 0x006BF483
 */
bool Staff::UpdatePatrollingFindWatering()
{
    if (!(StaffOrders & STAFF_ORDERS_WATER_FLOWERS))
        return false;

    uint8_t chosen_position = ScenarioRand() & 7;
    for (int32_t i = 0; i < 8; ++i, ++chosen_position)
    {
        chosen_position &= 7;

        auto chosenLoc = CoordsXY{ NextLoc } + CoordsDirectionDelta[chosen_position];

        TileElement* tile_element = MapGetFirstElementAt(chosenLoc);

        // This seems to happen in some SV4 files.
        if (tile_element == nullptr)
        {
            continue;
        }

        do
        {
            if (tile_element->GetType() != TileElementType::SmallScenery)
            {
                continue;
            }

            auto z_diff = abs(NextLoc.z - tile_element->GetBaseZ());

            if (z_diff >= 4 * COORDS_Z_STEP)
            {
                continue;
            }

            auto* sceneryEntry = tile_element->AsSmallScenery()->GetEntry();

            if (sceneryEntry == nullptr || !sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_CAN_BE_WATERED))
            {
                continue;
            }

            if (tile_element->AsSmallScenery()->GetAge() < SCENERY_WITHER_AGE_THRESHOLD_2)
            {
                if (chosen_position >= 4)
                {
                    continue;
                }

                if (tile_element->AsSmallScenery()->GetAge() < SCENERY_WITHER_AGE_THRESHOLD_1)
                {
                    continue;
                }
            }

            SetState(PeepState::Watering);
            Var37 = chosen_position;

            SubState = 0;
            auto destination = _WateringUseOffsets[chosen_position] + GetLocation().ToTileStart();
            SetDestination(destination, 3);

            return true;
        } while (!(tile_element++)->IsLastForTile());
    }
    return false;
}

/**
 *
 *  rct2: 0x006BF3A1
 */
bool Staff::UpdatePatrollingFindBin()
{
    if (!(StaffOrders & STAFF_ORDERS_EMPTY_BINS))
        return false;

    if (GetNextIsSurface())
        return false;

    TileElement* tileElement = MapGetFirstElementAt(NextLoc);
    if (tileElement == nullptr)
        return false;

    for (;; tileElement++)
    {
        if (tileElement->GetType() == TileElementType::Path && (tileElement->GetBaseZ() == NextLoc.z))
            break;

        if (tileElement->IsLastForTile())
            return false;
    }

    if (!tileElement->AsPath()->HasAddition())
        return false;
    auto* pathAddEntry = tileElement->AsPath()->GetAdditionEntry();
    if (pathAddEntry == nullptr)
        return false;

    if (!(pathAddEntry->flags & PATH_BIT_FLAG_IS_BIN))
        return false;

    if (tileElement->AsPath()->IsBroken())
        return false;

    if (tileElement->AsPath()->AdditionIsGhost())
        return false;

    uint8_t bin_positions = tileElement->AsPath()->GetEdges();
    uint8_t bin_quantity = tileElement->AsPath()->GetAdditionStatus();
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

    Var37 = chosen_position;
    SetState(PeepState::EmptyingBin);

    SubState = 0;
    auto destination = BinUseOffsets[chosen_position] + GetLocation().ToTileStart();
    SetDestination(destination, 3);
    return true;
}

/**
 *
 *  rct2: 0x006BF322
 */
bool Staff::UpdatePatrollingFindGrass()
{
    if (!(StaffOrders & STAFF_ORDERS_MOWING))
        return false;

    if (StaffMowingTimeout < 12)
        return false;

    if (!(GetNextIsSurface()))
        return false;

    auto surfaceElement = MapGetSurfaceElementAt(NextLoc);
    if (surfaceElement != nullptr && surfaceElement->CanGrassGrow())
    {
        if ((surfaceElement->GetGrassLength() & 0x7) >= GRASS_LENGTH_CLEAR_1)
        {
            SetState(PeepState::Mowing);
            Var37 = 0;
            // Original code used .y for both x and y. Changed to .x to make more sense (both x and y are 28)

            auto destination = _MowingWaypoints[0] + NextLoc;
            SetDestination(destination, 3);
            return true;
        }
    }
    return false;
}

/**
 *
 *  rct2: 0x006BF295
 */
bool Staff::UpdatePatrollingFindSweeping()
{
    if (!(StaffOrders & STAFF_ORDERS_SWEEPING))
        return false;
    auto quad = EntityTileList<Litter>({ x, y });
    for (auto litter : quad)
    {
        uint16_t z_diff = abs(z - litter->z);

        if (z_diff >= 16)
            continue;

        SetState(PeepState::Sweeping);

        Var37 = 0;
        SetDestination(litter->GetLocation(), 5);
        return true;
    }

    return false;
}

void Staff::Tick128UpdateStaff()
{
    if (AssignedStaffType != StaffType::Security)
        return;

    PeepSpriteType newSpriteType = PeepSpriteType::SecurityAlt;
    if (State != PeepState::Patrolling)
        newSpriteType = PeepSpriteType::Security;

    if (SpriteType == newSpriteType)
        return;

    SpriteType = newSpriteType;
    ActionSpriteImageOffset = 0;
    WalkingFrameNum = 0;
    if (Action < PeepActionType::Idle)
        Action = PeepActionType::Walking;

    PeepFlags &= ~PEEP_FLAGS_SLOW_WALK;
    if (gSpriteTypeToSlowWalkMap[EnumValue(newSpriteType)])
    {
        PeepFlags |= PEEP_FLAGS_SLOW_WALK;
    }

    ActionSpriteType = PeepActionSpriteType::Invalid;
    UpdateCurrentActionSpriteType();
}

bool Staff::IsMechanic() const
{
    return AssignedStaffType == StaffType::Mechanic;
}

void Staff::UpdateStaff(uint32_t stepsToTake)
{
    switch (State)
    {
        case PeepState::Patrolling:
            UpdatePatrolling();
            break;
        case PeepState::Mowing:
            UpdateMowing();
            break;
        case PeepState::Sweeping:
            UpdateSweeping();
            break;
        case PeepState::Answering:
            UpdateAnswering();
            break;
        case PeepState::Fixing:
            UpdateFixing(stepsToTake);
            break;
        case PeepState::Inspecting:
            UpdateFixing(stepsToTake);
            break;
        case PeepState::EmptyingBin:
            UpdateEmptyingBin();
            break;
        case PeepState::Watering:
            UpdateWatering();
            break;
        case PeepState::HeadingToInspection:
            UpdateHeadingToInspect();
            break;
        default:
            // TODO reset to default state
            assert(false);
            break;
    }
}

/**
 *
 *  rct2: 0x006BF1FD
 */
void Staff::UpdatePatrolling()
{
    if (!CheckForPath())
        return;

    if (ShouldWaitForLevelCrossing() && !IsMechanicHeadingToFixRideBlockingPath())
        return;

    uint8_t pathingResult;
    PerformNextAction(pathingResult);
    if (!(pathingResult & PATHING_DESTINATION_REACHED))
        return;

    if (GetNextIsSurface())
    {
        auto surfaceElement = MapGetSurfaceElementAt(NextLoc);

        if (surfaceElement != nullptr)
        {
            int32_t water_height = surfaceElement->GetWaterHeight();
            if (water_height > 0)
            {
                MoveTo({ x, y, water_height });
                SetState(PeepState::Falling);
                return;
            }
        }
    }

    if (AssignedStaffType != StaffType::Handyman)
        return;

    if (UpdatePatrollingFindSweeping())
        return;

    if (UpdatePatrollingFindGrass())
        return;

    if (UpdatePatrollingFindBin())
        return;

    UpdatePatrollingFindWatering();
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
 * mechanics fixing a ride. The array is indexed by breakdown_reason:
 * - indexes 0-7 are the 8 breakdown reasons (see BREAKDOWN_* in Ride.h)
 *   when fixing a broken down ride;
 * - index 8 is for inspecting a ride.
 */
// clang-format off
static constexpr const uint32_t FixingSubstatesForBreakdown[9] = {
    ( // BREAKDOWN_SAFETY_CUT_OUT
        (1 << PEEP_FIXING_MOVE_TO_STATION_END) |
        (1 << PEEP_FIXING_FIX_STATION_END) |
        (1 << PEEP_FIXING_MOVE_TO_STATION_START) |
        (1 << PEEP_FIXING_FIX_STATION_START) |
        (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
        (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
        (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
    ),
    ( // BREAKDOWN_RESTRAINTS_STUCK_CLOSED
        (1 << PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE) |
        (1 << PEEP_FIXING_FIX_VEHICLE_CLOSED_RESTRAINTS) |
        (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
        (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
        (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
    ),
    ( // BREAKDOWN_RESTRAINTS_STUCK_OPEN
        (1 << PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE) |
        (1 << PEEP_FIXING_FIX_VEHICLE_OPEN_RESTRAINTS) |
        (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
        (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
        (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
    ),
    ( // BREAKDOWN_DOORS_STUCK_CLOSED
        (1 << PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE) |
        (1 << PEEP_FIXING_FIX_VEHICLE_CLOSED_DOORS) |
        (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
        (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
        (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
    ),
    ( // BREAKDOWN_DOORS_STUCK_OPEN
        (1 << PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE) |
        (1 << PEEP_FIXING_FIX_VEHICLE_OPEN_DOORS) |
        (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
        (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
        (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
    ),
    ( // BREAKDOWN_VEHICLE_MALFUNCTION
        (1 << PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE) |
        (1 << PEEP_FIXING_FIX_VEHICLE_MALFUNCTION) |
        (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
        (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
        (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
    ),
    ( // BREAKDOWN_BRAKES_FAILURE
        (1 << PEEP_FIXING_MOVE_TO_STATION_START) |
        (1 << PEEP_FIXING_FIX_STATION_BRAKES) |
        (1 << PEEP_FIXING_MOVE_TO_STATION_EXIT) |
        (1 << PEEP_FIXING_FINISH_FIX_OR_INSPECT) |
        (1 << PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT)
    ),
    ( // BREAKDOWN_CONTROL_FAILURE
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
void Staff::UpdateFixing(int32_t steps)
{
    auto ride = GetRide(CurrentRide);
    if (ride == nullptr)
    {
        SetState(PeepState::Falling);
        return;
    }

    bool progressToNextSubstate = true;
    bool firstRun = true;

    if ((State == PeepState::Inspecting)
        && (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)))
    {
        // Ride has broken down since Mechanic was called to inspect it.
        // Mechanic identifies the breakdown and switches to fixing it.
        State = PeepState::Fixing;
    }

    while (progressToNextSubstate)
    {
        switch (SubState)
        {
            case PEEP_FIXING_ENTER_STATION:
                NextFlags &= ~PEEP_NEXT_FLAG_IS_SLOPED;
                progressToNextSubstate = UpdateFixingEnterStation(*ride);
                break;

            case PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE:
                progressToNextSubstate = UpdateFixingMoveToBrokenDownVehicle(firstRun, *ride);
                break;

            case PEEP_FIXING_FIX_VEHICLE_CLOSED_RESTRAINTS:
            case PEEP_FIXING_FIX_VEHICLE_CLOSED_DOORS:
            case PEEP_FIXING_FIX_VEHICLE_OPEN_RESTRAINTS:
            case PEEP_FIXING_FIX_VEHICLE_OPEN_DOORS:
                progressToNextSubstate = UpdateFixingFixVehicle(firstRun, *ride);
                break;

            case PEEP_FIXING_FIX_VEHICLE_MALFUNCTION:
                progressToNextSubstate = UpdateFixingFixVehicleMalfunction(firstRun, *ride);
                break;

            case PEEP_FIXING_MOVE_TO_STATION_END:
                progressToNextSubstate = UpdateFixingMoveToStationEnd(firstRun, *ride);
                break;

            case PEEP_FIXING_FIX_STATION_END:
                progressToNextSubstate = UpdateFixingFixStationEnd(firstRun);
                break;

            case PEEP_FIXING_MOVE_TO_STATION_START:
                progressToNextSubstate = UpdateFixingMoveToStationStart(firstRun, *ride);
                break;

            case PEEP_FIXING_FIX_STATION_START:
                progressToNextSubstate = UpdateFixingFixStationStart(firstRun, *ride);
                break;

            case PEEP_FIXING_FIX_STATION_BRAKES:
                progressToNextSubstate = UpdateFixingFixStationBrakes(firstRun, *ride);
                break;

            case PEEP_FIXING_MOVE_TO_STATION_EXIT:
                progressToNextSubstate = UpdateFixingMoveToStationExit(firstRun, *ride);
                break;

            case PEEP_FIXING_FINISH_FIX_OR_INSPECT:
                progressToNextSubstate = UpdateFixingFinishFixOrInspect(firstRun, steps, *ride);
                break;

            case PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT:
                progressToNextSubstate = UpdateFixingLeaveByEntranceExit(firstRun, *ride);
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

        int32_t subState = SubState;
        uint32_t sub_state_sequence_mask = FixingSubstatesForBreakdown[8];

        if (State != PeepState::Inspecting)
        {
            sub_state_sequence_mask = FixingSubstatesForBreakdown[ride->breakdown_reason_pending];
        }

        do
        {
            subState++;
        } while ((sub_state_sequence_mask & (1 << subState)) == 0);

        SubState = subState & 0xFF;
    }
}

/**
 * rct2: 0x006C0EEC
 * fixing SubState: enter_station - applies to fixing all break down reasons and ride inspections.
 */
bool Staff::UpdateFixingEnterStation(Ride& ride) const
{
    ride.mechanic_status = RIDE_MECHANIC_STATUS_FIXING;
    ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;

    return true;
}

/**
 * rct2: 0x006C0F09
 * fixing SubState: move_to_broken_down_vehicle - applies to fixing all vehicle specific breakdown reasons
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingMoveToBrokenDownVehicle(bool firstRun, const Ride& ride)
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
            if (TrackTypeIsStation(trackType))
            {
                break;
            }

            vehicle = GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
            if (vehicle == nullptr)
            {
                return true;
            }
        }

        CoordsXY offset = DirectionOffsets[PeepDirection];
        auto destination = (offset * -12) + vehicle->GetLocation();
        SetDestination(destination, 2);
    }

    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
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
bool Staff::UpdateFixingFixVehicle(bool firstRun, const Ride& ride)
{
    if (!firstRun)
    {
        sprite_direction = PeepDirection << 3;

        Action = (ScenarioRand() & 1) ? PeepActionType::StaffFix2 : PeepActionType::StaffFix;
        ActionSpriteImageOffset = 0;
        ActionFrame = 0;
        UpdateCurrentActionSpriteType();
    }

    if (IsActionWalking())
    {
        return true;
    }

    UpdateAction();
    Invalidate();

    uint8_t actionFrame = (Action == PeepActionType::StaffFix) ? 0x25 : 0x50;
    if (ActionFrame != actionFrame)
    {
        return false;
    }

    Vehicle* vehicle = RideGetBrokenVehicle(ride);
    if (vehicle == nullptr)
    {
        return true;
    }

    vehicle->ClearFlag(VehicleFlags::CarIsBroken);

    return false;
}

/**
 * rct2: 0x006C107B
 * fixing SubState: fix_vehicle_malfunction - applies fixing to vehicle malfunction.
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingFixVehicleMalfunction(bool firstRun, const Ride& ride)
{
    if (!firstRun)
    {
        sprite_direction = PeepDirection << 3;
        Action = PeepActionType::StaffFix3;
        ActionSpriteImageOffset = 0;
        ActionFrame = 0;

        UpdateCurrentActionSpriteType();
    }

    if (IsActionWalking())
    {
        return true;
    }

    UpdateAction();
    Invalidate();

    if (ActionFrame != 0x65)
    {
        return false;
    }

    Vehicle* vehicle = RideGetBrokenVehicle(ride);
    if (vehicle == nullptr)
    {
        return true;
    }

    vehicle->ClearFlag(VehicleFlags::TrainIsBroken);

    return false;
}

/** rct2: 0x00992A3C */
static constexpr const CoordsXY _StationFixingOffsets[] = {
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
bool Staff::UpdateFixingMoveToStationEnd(bool firstRun, const Ride& ride)
{
    if (!firstRun)
    {
        if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_HAS_NO_TRACK))
        {
            return true;
        }

        auto stationPos = ride.GetStation(CurrentRideStation).GetStart();
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

        int32_t trackDirection = tileElement->GetDirection();
        CoordsXY offset = _StationFixingOffsets[trackDirection];

        stationPos.x += 16 + offset.x;
        if (offset.x == 0)
        {
            stationPos.x = GetDestination().x;
        }

        stationPos.y += 16 + offset.y;
        if (offset.y == 0)
        {
            stationPos.y = GetDestination().y;
        }

        SetDestination(stationPos, 2);
    }

    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
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
bool Staff::UpdateFixingFixStationEnd(bool firstRun)
{
    if (!firstRun)
    {
        sprite_direction = PeepDirection << 3;
        Action = PeepActionType::StaffCheckboard;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;

        UpdateCurrentActionSpriteType();
    }

    if (IsActionWalking())
    {
        return true;
    }

    UpdateAction();
    Invalidate();

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
bool Staff::UpdateFixingMoveToStationStart(bool firstRun, const Ride& ride)
{
    if (!firstRun)
    {
        if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_HAS_NO_TRACK))
        {
            return true;
        }

        auto stationPosition = ride.GetStation(CurrentRideStation).GetStart();
        if (stationPosition.IsNull())
        {
            return true;
        }

        CoordsXYE input;
        input.x = stationPosition.x;
        input.y = stationPosition.y;
        input.element = MapGetTrackElementAtFromRide({ input.x, input.y, stationPosition.z }, CurrentRide);
        if (input.element == nullptr)
        {
            return true;
        }

        Direction stationDirection = 0;
        TrackBeginEnd trackBeginEnd;
        while (TrackBlockGetPrevious(input, &trackBeginEnd))
        {
            if (trackBeginEnd.begin_element->AsTrack()->IsStation())
            {
                input.x = trackBeginEnd.begin_x;
                input.y = trackBeginEnd.begin_y;
                input.element = trackBeginEnd.begin_element;

                stationDirection = trackBeginEnd.begin_element->GetDirection();
                continue;
            }

            break;
        }

        // Loc6C12ED:
        auto destination = CoordsXY{ input.x + 16, input.y + 16 };
        auto offset = _StationFixingOffsets[stationDirection];

        destination.x -= offset.x;
        if (offset.x == 0)
        {
            destination.x = GetDestination().x;
        }

        destination.y -= offset.y;
        if (offset.y == 0)
        {
            destination.y = GetDestination().y;
        }

        SetDestination(destination, 2);
    }

    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
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
bool Staff::UpdateFixingFixStationStart(bool firstRun, const Ride& ride)
{
    if (!firstRun)
    {
        if (ride.GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_HAS_NO_TRACK))
        {
            return true;
        }

        sprite_direction = PeepDirection << 3;

        Action = PeepActionType::StaffFix;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;

        UpdateCurrentActionSpriteType();
    }

    if (IsActionWalking())
    {
        return true;
    }

    UpdateAction();

    return false;
}

/**
 * rct2: 0x006C13CE
 * fixing SubState: fix_station_brakes - applies to fixing brake failure
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingFixStationBrakes(bool firstRun, Ride& ride)
{
    if (!firstRun)
    {
        sprite_direction = PeepDirection << 3;

        Action = PeepActionType::StaffFixGround;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;

        UpdateCurrentActionSpriteType();
    }

    if (IsActionWalking())
    {
        return true;
    }

    UpdateAction();
    Invalidate();

    if (ActionFrame == 0x28)
    {
        ride.mechanic_status = RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES;
        ride.window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
    }

    if (ActionFrame == 0x13 || ActionFrame == 0x19 || ActionFrame == 0x1F || ActionFrame == 0x25 || ActionFrame == 0x2B)
    {
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::MechanicFix, GetLocation());
    }

    return false;
}

/**
 * rct2: 0x006C1474
 * fixing SubState: move_to_station_exit - applies to fixing all failures & inspections
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingMoveToStationExit(bool firstRun, const Ride& ride)
{
    if (!firstRun)
    {
        auto stationPosition = ride.GetStation(CurrentRideStation).Exit.ToCoordsXY();
        if (stationPosition.IsNull())
        {
            stationPosition = ride.GetStation(CurrentRideStation).Entrance.ToCoordsXY();

            if (stationPosition.IsNull())
            {
                return true;
            }
        }

        stationPosition = stationPosition.ToTileCentre();

        CoordsXY stationPlatformDirection = DirectionOffsets[PeepDirection];
        stationPosition.x += stationPlatformDirection.x * 20;
        stationPosition.y += stationPlatformDirection.y * 20;

        SetDestination(stationPosition, 2);
    }

    if (auto loc = UpdateAction(); loc.has_value())
    {
        MoveTo({ loc.value(), z });
        return false;
    }

    return true;
}

/**
 * rct2: 0x006C1504
 * fixing SubState: finish_fix_or_inspect - applies to fixing all failures & inspections
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingFinishFixOrInspect(bool firstRun, int32_t steps, Ride& ride)
{
    if (!firstRun)
    {
        if (State == PeepState::Inspecting)
        {
            UpdateRideInspected(CurrentRide);

            StaffRidesInspected++;
            WindowInvalidateFlags |= RIDE_INVALIDATE_RIDE_INCOME | RIDE_INVALIDATE_RIDE_LIST;
            ride.mechanic_status = RIDE_MECHANIC_STATUS_UNDEFINED;
            return true;
        }

        StaffRidesFixed++;
        WindowInvalidateFlags |= RIDE_INVALIDATE_RIDE_INCOME | RIDE_INVALIDATE_RIDE_LIST;

        sprite_direction = PeepDirection << 3;
        Action = PeepActionType::StaffAnswerCall2;
        ActionFrame = 0;
        ActionSpriteImageOffset = 0;

        UpdateCurrentActionSpriteType();
    }

    if (!IsActionWalking())
    {
        UpdateAction();
        Invalidate();
        return false;
    }

    RideFixBreakdown(ride, steps);
    ride.mechanic_status = RIDE_MECHANIC_STATUS_UNDEFINED;
    return true;
}

/**
 * rct2: 0x006C157E
 * fixing SubState: leave_by_entrance_exit - applies to fixing all failures & inspections
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingLeaveByEntranceExit(bool firstRun, const Ride& ride)
{
    if (!firstRun)
    {
        auto exitPosition = ride.GetStation(CurrentRideStation).Exit.ToCoordsXY();
        if (exitPosition.IsNull())
        {
            exitPosition = ride.GetStation(CurrentRideStation).Entrance.ToCoordsXY();

            if (exitPosition.IsNull())
            {
                SetState(PeepState::Falling);
                return false;
            }
        }

        exitPosition = exitPosition.ToTileCentre();

        CoordsXY ebx_direction = DirectionOffsets[PeepDirection];
        exitPosition.x -= ebx_direction.x * 19;
        exitPosition.y -= ebx_direction.y * 19;

        SetDestination(exitPosition, 2);
    }

    int16_t xy_distance;
    if (auto loc = UpdateAction(xy_distance); loc.has_value())
    {
        auto stationHeight = ride.GetStation(CurrentRideStation).GetBaseZ();
        if (xy_distance >= 16)
        {
            stationHeight += ride.GetRideTypeDescriptor().Heights.PlatformHeight;
        }

        MoveTo({ loc.value(), stationHeight });
        return false;
    }
    SetState(PeepState::Falling);
    return false;
}

/**
 * rct2: 0x6B7588
 */
void Staff::UpdateRideInspected(RideId rideIndex)
{
    auto ride = GetRide(rideIndex);
    if (ride != nullptr)
    {
        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
        ride->reliability += ((100 - ride->reliability_percentage) / 4) * (ScenarioRand() & 0xFF);
        ride->last_inspection = 0;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE | RIDE_INVALIDATE_RIDE_MAIN
            | RIDE_INVALIDATE_RIDE_LIST;
    }
}

money64 GetStaffWage(StaffType type)
{
    switch (type)
    {
        default:
        case StaffType::Handyman:
            return 50.00_GBP;
        case StaffType::Mechanic:
            return 80.00_GBP;
        case StaffType::Security:
            return 60.00_GBP;
        case StaffType::Entertainer:
            return 55.00_GBP;
    }
}

void Staff::Serialise(DataSerialiser& stream)
{
    Peep::Serialise(stream);
    stream << AssignedStaffType;
    stream << MechanicTimeSinceCall;
    stream << HireDate;
    stream << StaffOrders;
    stream << StaffMowingTimeout;
    stream << StaffLawnsMown;
    stream << StaffGardensWatered;
    stream << StaffLitterSwept;
    stream << StaffBinsEmptied;
}
