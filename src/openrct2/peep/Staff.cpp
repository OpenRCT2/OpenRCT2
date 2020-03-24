/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
#include "../actions/StaffHireNewAction.hpp"
#include "../actions/StaffSetOrdersAction.hpp"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../interface/Viewport.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/TerrainSurfaceObject.h"
#include "../paint/tile_element/Paint.TileElement.h"
#include "../ride/RideData.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
#include "Peep.h"

#include <algorithm>
#include <iterator>

/**
 * Monthly staff wages
 *
 * rct2: 0x00992A00
 */
const money32 gStaffWageTable[STAFF_TYPE_COUNT] = {
    MONEY(50, 00), // Handyman
    MONEY(80, 00), // Mechanic
    MONEY(60, 00), // Security guard
    MONEY(55, 00), // Entertainer
};

// clang-format off
const rct_string_id StaffCostumeNames[] = {
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

// Every staff member has STAFF_PATROL_AREA_SIZE elements assigned to in this array, indexed by their staff_id
// Additionally there is a patrol area for each staff type, which is the union of the patrols of all staff members of that type
uint32_t gStaffPatrolAreas[(STAFF_MAX_COUNT + STAFF_TYPE_COUNT) * STAFF_PATROL_AREA_SIZE];
uint8_t gStaffModes[STAFF_MAX_COUNT + STAFF_TYPE_COUNT];
uint16_t gStaffDrawPatrolAreas;
colour_t gStaffHandymanColour;
colour_t gStaffMechanicColour;
colour_t gStaffSecurityColour;

/**
 *
 *  rct2: 0x006BD3A4
 */
void staff_reset_modes()
{
    for (int32_t i = 0; i < STAFF_MAX_COUNT; i++)
        gStaffModes[i] = STAFF_MODE_NONE;

    for (int32_t i = STAFF_MAX_COUNT; i < (STAFF_MAX_COUNT + STAFF_TYPE_COUNT); i++)
        gStaffModes[i] = STAFF_MODE_WALK;

    staff_update_greyed_patrol_areas();
}

/**
 * Hires a new staff member of the given type.
 */
bool staff_hire_new_member(STAFF_TYPE staffType, ENTERTAINER_COSTUME entertainerType)
{
    bool autoPosition = gConfigGeneral.auto_staff_placement;
    if (gInputPlaceObjectModifier & PLACE_OBJECT_MODIFIER_SHIFT_Z)
    {
        autoPosition = autoPosition ^ 1;
    }

    uint32_t staffOrders = 0;

    if (staffType == STAFF_TYPE_HANDYMAN)
    {
        staffOrders = STAFF_ORDERS_SWEEPING | STAFF_ORDERS_WATER_FLOWERS | STAFF_ORDERS_EMPTY_BINS;
        if (gConfigGeneral.handymen_mow_default)
        {
            staffOrders |= STAFF_ORDERS_MOWING;
        }
    }
    else if (staffType == STAFF_TYPE_MECHANIC)
    {
        staffOrders = STAFF_ORDERS_INSPECT_RIDES | STAFF_ORDERS_FIX_RIDES;
    }

    auto hireStaffAction = StaffHireNewAction(autoPosition, staffType, entertainerType, staffOrders);
    hireStaffAction.SetCallback([=](const GameAction*, const StaffHireNewActionResult* res) -> void {
        if (res->Error != GA_ERROR::OK)
            return;

        // Open window for new staff.
        Peep* peep = &get_sprite(res->peepSriteIndex)->peep;
        auto intent = Intent(WC_PEEP);
        intent.putExtra(INTENT_EXTRA_PEEP, peep);
        context_open_intent(&intent);
    });

    auto res = GameActions::Execute(&hireStaffAction);
    return res->Error == GA_ERROR::OK;
}

/**
 *
 *  rct2: 0x006C0C3F
 */
void staff_update_greyed_patrol_areas()
{
    Peep* peep;

    for (int32_t staff_type = 0; staff_type < STAFF_TYPE_COUNT; ++staff_type)
    {
        int32_t staffPatrolOffset = (staff_type + STAFF_MAX_COUNT) * STAFF_PATROL_AREA_SIZE;
        for (int32_t i = 0; i < STAFF_PATROL_AREA_SIZE; i++)
        {
            gStaffPatrolAreas[staffPatrolOffset + i] = 0;
        }

        for (uint16_t sprite_index = gSpriteListHead[SPRITE_LIST_PEEP]; sprite_index != SPRITE_INDEX_NULL;
             sprite_index = peep->next)
        {
            peep = GET_PEEP(sprite_index);

            if (peep->type == PEEP_TYPE_STAFF && staff_type == peep->staff_type)
            {
                int32_t peepPatrolOffset = peep->staff_id * STAFF_PATROL_AREA_SIZE;
                for (int32_t i = 0; i < STAFF_PATROL_AREA_SIZE; i++)
                {
                    gStaffPatrolAreas[staffPatrolOffset + i] |= gStaffPatrolAreas[peepPatrolOffset + i];
                }
            }
        }
    }
}

/**
 *
 *  rct2: 0x006C0905
 */
bool Staff::IsLocationInPatrol(const CoordsXY& loc) const
{
    // Check if location is in the park
    if (!map_is_location_owned_or_has_rights(loc))
        return false;

    // Check if staff has patrol area
    if (!(gStaffModes[staff_id] & 2))
        return true;

    return IsPatrolAreaSet(loc);
}

bool staff_is_location_on_patrol_edge(Peep* mechanic, int32_t x, int32_t y)
{
    // Check whether the location x,y is inside and on the edge of the
    // patrol zone for mechanic.
    bool onZoneEdge = false;
    int32_t neighbourDir = 0;
    while (!onZoneEdge && neighbourDir <= 7)
    {
        int32_t neighbourX = x + CoordsDirectionDelta[neighbourDir].x;
        int32_t neighbourY = y + CoordsDirectionDelta[neighbourDir].y;
        onZoneEdge = !mechanic->AsStaff()->IsLocationInPatrol({ neighbourX, neighbourY });
        neighbourDir++;
    }
    return onZoneEdge;
}

bool staff_can_ignore_wide_flag(Peep* staff, int32_t x, int32_t y, uint8_t z, TileElement* path)
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

    if (staff->type != PEEP_TYPE_STAFF)
        return false;

    if (!staff_is_location_on_patrol_edge(staff, x, y))
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
        int32_t adjac_x = x + CoordsDirectionDelta[adjac_dir].x;
        int32_t adjac_y = y + CoordsDirectionDelta[adjac_dir].y;
        uint8_t adjac_z = z;

        /* Ignore adjacent tiles outside the patrol zone. */
        if (!staff->AsStaff()->IsLocationInPatrol({ adjac_x, adjac_y }))
            continue;

        /* Ignore adjacent tiles on the patrol zone edge. */
        if (staff_is_location_on_patrol_edge(staff, adjac_x, adjac_y))
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
                adjac_z = z + 2;
            }
        }

        /* Search through all adjacent map elements */
        TileElement* test_element = map_get_first_element_at({ adjac_x, adjac_y });
        if (test_element == nullptr)
            return false;
        bool pathfound = false;
        bool widefound = false;
        do
        {
            if (test_element->GetType() != TILE_ELEMENT_TYPE_PATH)
            {
                continue;
            }

            /* test_element is a path */
            if (!is_valid_path_z_and_direction(test_element, adjac_z, adjac_dir))
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
static uint8_t staff_get_valid_patrol_directions(Staff* staff, const CoordsXY& loc)
{
    uint8_t directions = 0;

    if (staff->IsLocationInPatrol({ loc.x - COORDS_XY_STEP, loc.y }))
    {
        directions |= (1 << 0);
    }

    if (staff->IsLocationInPatrol({ loc.x, loc.y + COORDS_XY_STEP }))
    {
        directions |= (1 << 1);
    }

    if (staff->IsLocationInPatrol({ loc.x + COORDS_XY_STEP, loc.y }))
    {
        directions |= (1 << 2);
    }

    if (staff->IsLocationInPatrol({ loc.x, loc.y - COORDS_XY_STEP }))
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
void staff_reset_stats()
{
    uint16_t spriteIndex;
    Peep* peep;

    FOR_ALL_STAFF (spriteIndex, peep)
    {
        peep->time_in_park = gDateMonthsElapsed;
        peep->staff_lawns_mown = 0;
        peep->staff_rides_fixed = 0;
        peep->staff_gardens_watered = 0;
        peep->staff_rides_inspected = 0;
        peep->staff_litter_swept = 0;
        peep->staff_bins_emptied = 0;
    }
}

static bool staff_is_patrol_area_set(int32_t staffIndex, int32_t x, int32_t y)
{
    // Patrol quads are stored in a bit map (8 patrol quads per byte).
    // Each patrol quad is 4x4.
    // Therefore there are in total 64 x 64 patrol quads in the 256 x 256 map.
    // At the end of the array (after the slots for individual staff members),
    // there are slots that save the combined patrol area for every staff type.

    x = (x & 0x1F80) >> 7;
    y = (y & 0x1F80) >> 1;

    int32_t peepOffset = staffIndex * STAFF_PATROL_AREA_SIZE;
    int32_t offset = (x | y) >> 5;
    int32_t bitIndex = (x | y) & 0x1F;
    return gStaffPatrolAreas[peepOffset + offset] & (((uint32_t)1) << bitIndex);
}

bool Staff::IsPatrolAreaSet(const CoordsXY& coords) const
{
    return staff_is_patrol_area_set(staff_id, coords.x, coords.y);
}

bool staff_is_patrol_area_set_for_type(STAFF_TYPE type, const CoordsXY& coords)
{
    return staff_is_patrol_area_set(STAFF_MAX_COUNT + type, coords.x, coords.y);
}

void staff_set_patrol_area(int32_t staffIndex, int32_t x, int32_t y, bool value)
{
    x = (x & 0x1F80) >> 7;
    y = (y & 0x1F80) >> 1;

    int32_t peepOffset = staffIndex * STAFF_PATROL_AREA_SIZE;
    int32_t offset = (x | y) >> 5;
    int32_t bitIndex = (x | y) & 0x1F;
    uint32_t* addr = &gStaffPatrolAreas[peepOffset + offset];
    if (value)
    {
        *addr |= (1 << bitIndex);
    }
    else
    {
        *addr &= ~(1 << bitIndex);
    }
}

void staff_toggle_patrol_area(int32_t staffIndex, int32_t x, int32_t y)
{
    x = (x & 0x1F80) >> 7;
    y = (y & 0x1F80) >> 1;

    int32_t peepOffset = staffIndex * STAFF_PATROL_AREA_SIZE;
    int32_t offset = (x | y) >> 5;
    int32_t bitIndex = (x | y) & 0x1F;
    gStaffPatrolAreas[peepOffset + offset] ^= (1 << bitIndex);
}

/**
 *
 *  rct2: 0x006BFBE8
 *
 * Returns INVALID_DIRECTION when no nearby litter or unpathable litter
 */
static uint8_t staff_handyman_direction_to_nearest_litter(Peep* peep)
{
    uint16_t nearestLitterDist = (uint16_t)-1;
    Litter* nearestLitter = nullptr;
    Litter* litter = nullptr;

    for (uint16_t litterIndex = gSpriteListHead[SPRITE_LIST_LITTER]; litterIndex != 0xFFFF; litterIndex = litter->next)
    {
        litter = &get_sprite(litterIndex)->litter;

        uint16_t distance = abs(litter->x - peep->x) + abs(litter->y - peep->y) + abs(litter->z - peep->z) * 4;

        if (distance < nearestLitterDist)
        {
            nearestLitterDist = distance;
            nearestLitter = litter;
        }
    }

    if (nearestLitterDist > 0x60)
    {
        return INVALID_DIRECTION;
    }

    auto litterTile = CoordsXY{ nearestLitter->x, nearestLitter->y }.ToTileStart();

    if (!peep->AsStaff()->IsLocationInPatrol(litterTile))
    {
        return INVALID_DIRECTION;
    }

    litterTile = litterTile.ToTileCentre();

    int16_t x_diff = litterTile.x - peep->x;
    int16_t y_diff = litterTile.y - peep->y;

    uint8_t nextDirection = 0;

    if (abs(x_diff) <= abs(y_diff))
    {
        nextDirection = y_diff < 0 ? 3 : 1;
    }
    else
    {
        nextDirection = x_diff < 0 ? 0 : 2;
    }

    CoordsXY nextTile = { static_cast<int32_t>((nearestLitter->x & 0xFFE0) - CoordsDirectionDelta[nextDirection].x),
                          static_cast<int32_t>((nearestLitter->y & 0xFFE0) - CoordsDirectionDelta[nextDirection].y) };

    int16_t nextZ = ((peep->z + 8) & 0xFFF0) / 8;

    TileElement* tileElement = map_get_first_element_at(nextTile);
    if (tileElement == nullptr)
        return INVALID_DIRECTION;
    do
    {
        if (tileElement->base_height != nextZ)
            continue;
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE || tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
        {
            return INVALID_DIRECTION;
        }
    } while (!(tileElement++)->IsLastForTile());

    nextTile.x = (peep->x & 0xFFE0) + CoordsDirectionDelta[nextDirection].x;
    nextTile.y = (peep->y & 0xFFE0) + CoordsDirectionDelta[nextDirection].y;

    tileElement = map_get_first_element_at(nextTile);
    if (tileElement == nullptr)
        return INVALID_DIRECTION;

    do
    {
        if (tileElement->base_height != nextZ)
            continue;
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE || tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
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
static uint8_t staff_handyman_direction_to_uncut_grass(Peep* peep, uint8_t valid_directions)
{
    if (!(peep->GetNextIsSurface()))
    {
        auto surfaceElement = map_get_surface_element_at(peep->NextLoc);
        if (surfaceElement == nullptr)
            return INVALID_DIRECTION;

        if (peep->NextLoc.z != surfaceElement->GetBaseZ())
            return INVALID_DIRECTION;

        if (peep->GetNextIsSloped())
        {
            if (surfaceElement->GetSlope() != PathSlopeToLandSlope[peep->GetNextDirection()])
                return INVALID_DIRECTION;
        }
        else if (surfaceElement->GetSlope() != TILE_ELEMENT_SLOPE_FLAT)
            return INVALID_DIRECTION;
    }

    uint8_t chosenDirection = scenario_rand() & 0x3;
    for (uint8_t i = 0; i < 4; ++i, ++chosenDirection)
    {
        chosenDirection &= 0x3;

        if (!(valid_directions & (1 << chosenDirection)))
        {
            continue;
        }

        CoordsXY chosenTile = CoordsXY{ peep->NextLoc } + CoordsDirectionDelta[chosenDirection];

        if (!map_is_location_valid(chosenTile))
            continue;

        auto surfaceElement = map_get_surface_element_at(chosenTile);
        if (surfaceElement != nullptr)
        {
            if (std::abs(surfaceElement->GetBaseZ() - peep->NextLoc.z) <= 2 * COORDS_Z_STEP)
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
int32_t Staff::HandymanDirectionRandSurface(uint8_t validDirections)
{
    uint8_t newDirection = scenario_rand() % NumOrthogonalDirections;
    for (int32_t i = 0; i < NumOrthogonalDirections; ++i, ++newDirection)
    {
        newDirection %= NumOrthogonalDirections;
        if (!(validDirections & (1 << newDirection)))
            continue;

        CoordsXY chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];

        if (map_surface_is_blocked(chosenTile))
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
    staff_mowing_timeout++;

    uint8_t litterDirection = INVALID_DIRECTION;
    uint8_t validDirections = staff_get_valid_patrol_directions(this, NextLoc);

    if ((staff_orders & STAFF_ORDERS_SWEEPING) && ((gCurrentTicks + sprite_index) & 0xFFF) > 110)
    {
        litterDirection = staff_handyman_direction_to_nearest_litter(this);
    }

    Direction newDirection = INVALID_DIRECTION;
    if (litterDirection == INVALID_DIRECTION && (staff_orders & STAFF_ORDERS_MOWING) && staff_mowing_timeout >= 12)
    {
        newDirection = staff_handyman_direction_to_uncut_grass(this, validDirections);
    }

    if (newDirection == INVALID_DIRECTION)
    {
        if (GetNextIsSurface())
        {
            newDirection = HandymanDirectionRandSurface(validDirections);
        }
        else
        {
            auto* pathElement = map_get_path_element_at(TileCoordsXYZ{ NextLoc });

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
                    if ((scenario_rand() & 0xFFFF) >= 0x1999)
                    {
                        chooseRandom = false;
                        newDirection = litterDirection;
                    }
                }
                else
                {
                    pathDirections &= ~(1 << direction_reverse(direction));
                    if (pathDirections == 0)
                    {
                        pathDirections |= 1 << direction_reverse(direction);
                    }
                }

                if (chooseRandom)
                {
                    do
                    {
                        newDirection = scenario_rand() & 3;
                    } while ((pathDirections & (1 << newDirection)) == 0);
                }
            }
        }
    }

    // countof(CoordsDirectionDelta)
    assert(newDirection < 8);

    CoordsXY chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];

    while (!map_is_location_valid(chosenTile))
    {
        newDirection = HandymanDirectionRandSurface(validDirections);
        chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];
    }

    direction = newDirection;
    destination_x = chosenTile.x + 16;
    destination_y = chosenTile.y + 16;
    destination_tolerance = 3;
    if (state == PEEP_STATE_QUEUING)
    {
        destination_tolerance = (scenario_rand() & 7) + 2;
    }
    return false;
}

static uint8_t staff_direction_surface(Peep* peep, uint8_t initialDirection)
{
    uint8_t direction = initialDirection;
    for (int32_t i = 0; i < 3; ++i)
    {
        // Looks left and right from initial direction
        switch (i)
        {
            case 1:
                direction++;
                if (scenario_rand() & 1)
                {
                    direction -= 2;
                }
                break;
            case 2:
                direction -= 2;
                break;
        }

        direction &= 3;

        if (fence_in_the_way({ peep->NextLoc, peep->NextLoc.z, peep->NextLoc.z + PEEP_CLEARANCE_HEIGHT }, direction))
            continue;

        if (fence_in_the_way(
                { peep->NextLoc, peep->NextLoc.z, peep->NextLoc.z + PEEP_CLEARANCE_HEIGHT }, direction_reverse(direction)))
            continue;

        CoordsXY chosenTile = CoordsXY{ peep->NextLoc } + CoordsDirectionDelta[direction];

        if (!map_surface_is_blocked(chosenTile))
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
static uint8_t staff_mechanic_direction_surface(Peep* peep)
{
    uint8_t direction = scenario_rand() & 3;

    auto ride = get_ride(peep->current_ride);
    if (ride != nullptr && (peep->state == PEEP_STATE_ANSWERING || peep->state == PEEP_STATE_HEADING_TO_INSPECTION)
        && (scenario_rand() & 1))
    {
        auto location = ride_get_exit_location(ride, peep->current_ride_station);
        if (location.isNull())
        {
            location = ride_get_entrance_location(ride, peep->current_ride_station);
        }

        CoordsXY chosenTile = location.ToCoordsXY();

        int16_t x_diff = chosenTile.x - peep->x;
        int16_t y_diff = chosenTile.y - peep->y;

        if (abs(x_diff) <= abs(y_diff))
        {
            direction = y_diff < 0 ? 3 : 1;
        }
        else
        {
            direction = x_diff < 0 ? 0 : 2;
        }
    }

    return staff_direction_surface(peep, direction);
}

/**
 *
 *  rct2: 0x006C02D1
 */
static uint8_t staff_mechanic_direction_path_rand(Peep* peep, uint8_t pathDirections)
{
    if (scenario_rand() & 1)
    {
        if (pathDirections & (1 << peep->direction))
            return peep->direction;
    }

    // Modified from original to spam scenario_rand less
    uint8_t direction = scenario_rand() & 3;
    for (int32_t i = 0; i < 4; ++i, ++direction)
    {
        direction &= 3;
        if (pathDirections & (1 << direction))
            return direction;
    }
    // This will never happen as pathDirections always has a bit set.
    return peep->direction;
}

/**
 *
 *  rct2: 0x006C0121
 */
static uint8_t staff_mechanic_direction_path(Peep* peep, uint8_t validDirections, PathElement* pathElement)
{
    Direction direction = INVALID_DIRECTION;
    uint8_t pathDirections = pathElement->GetEdges();
    pathDirections &= validDirections;

    if (pathDirections == 0)
    {
        return staff_mechanic_direction_surface(peep);
    }

    // Check if this is dead end - i.e. only way out is the reverse direction.
    pathDirections &= ~(1 << direction_reverse(peep->direction));
    if (pathDirections == 0)
    {
        pathDirections |= (1 << direction_reverse(peep->direction));
    }

    direction = bitscanforward(pathDirections);
    pathDirections &= ~(1 << direction);
    if (pathDirections == 0)
    {
        if (peep->state != PEEP_STATE_ANSWERING && peep->state != PEEP_STATE_HEADING_TO_INSPECTION)
        {
            return direction;
        }

        if (peep->sub_state != 2)
        {
            return direction;
        }
        peep->sub_state = 3;
    }

    pathDirections |= (1 << direction);

    // Mechanic is heading to ride (either broken down or for inspection).
    auto ride = get_ride(peep->current_ride);
    if (ride != nullptr && (peep->state == PEEP_STATE_ANSWERING || peep->state == PEEP_STATE_HEADING_TO_INSPECTION))
    {
        /* Find location of the exit for the target ride station
         * or if the ride has no exit, the entrance. */
        TileCoordsXYZD location = ride_get_exit_location(ride, peep->current_ride_station);
        if (location.isNull())
        {
            location = ride_get_entrance_location(ride, peep->current_ride_station);

            // If no entrance is present either. This is an incorrect state.
            if (location.isNull())
            {
                return staff_mechanic_direction_path_rand(peep, pathDirections);
            }
        }

        gPeepPathFindGoalPosition.x = location.x;
        gPeepPathFindGoalPosition.y = location.y;
        gPeepPathFindGoalPosition.z = location.z;

        gPeepPathFindIgnoreForeignQueues = false;
        gPeepPathFindQueueRideIndex = RIDE_ID_NULL;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        pathfind_logging_enable(peep);
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

        Direction pathfindDirection = peep_pathfind_choose_direction(TileCoordsXYZ{ peep->NextLoc }, peep);

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        pathfind_logging_disable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

        if (pathfindDirection == INVALID_DIRECTION)
        {
            /* Heuristic search failed for all directions.
             * Reset the pathfind_goal - this means that the pathfind_history
             * will be reset in the next call to peep_pathfind_choose_direction().
             * This lets the heuristic search "try again" in case the player has
             * edited the path layout or the mechanic was already stuck in the
             * save game (e.g. with a worse version of the pathfinding). */
            peep_reset_pathfind_goal(peep);
            return staff_mechanic_direction_path_rand(peep, pathDirections);
        }

        return (uint8_t)pathfindDirection;
    }
    return staff_mechanic_direction_path_rand(peep, pathDirections);
}

/**
 *
 *  rct2: 0x006BFF2C
 */
bool Staff::DoMechanicPathFinding()
{
    uint8_t validDirections = staff_get_valid_patrol_directions(this, NextLoc);
    Direction newDirection = INVALID_DIRECTION;
    if (GetNextIsSurface())
    {
        newDirection = staff_mechanic_direction_surface(this);
    }
    else
    {
        auto* pathElement = map_get_path_element_at(TileCoordsXYZ{ NextLoc });
        if (pathElement == nullptr)
            return true;

        newDirection = staff_mechanic_direction_path(this, validDirections, pathElement);
    }

    // countof(CoordsDirectionDelta)
    assert(newDirection < 8);

    CoordsXY chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];

    while (!map_is_location_valid(chosenTile))
    {
        newDirection = staff_mechanic_direction_surface(this);
        chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];
    }

    direction = newDirection;
    destination_x = chosenTile.x + 16;
    destination_y = chosenTile.y + 16;
    destination_tolerance = (scenario_rand() & 7) + 2;

    return false;
}

/**
 *
 *  rct2: 0x006C050B
 */
static uint8_t staff_direction_path(Peep* peep, uint8_t validDirections, PathElement* pathElement)
{
    Direction direction = INVALID_DIRECTION;
    uint8_t pathDirections = pathElement->GetEdges();
    if (peep->state != PEEP_STATE_ANSWERING && peep->state != PEEP_STATE_HEADING_TO_INSPECTION)
    {
        pathDirections &= validDirections;
    }

    if (pathDirections == 0)
    {
        return staff_direction_surface(peep, scenario_rand() & 3);
    }

    pathDirections &= ~(1 << direction_reverse(peep->direction));
    if (pathDirections == 0)
    {
        pathDirections |= (1 << direction_reverse(peep->direction));
    }

    direction = bitscanforward(pathDirections);
    pathDirections &= ~(1 << direction);
    if (pathDirections == 0)
    {
        return direction;
    }

    pathDirections |= (1 << direction);

    direction = scenario_rand() & 3;
    for (int32_t i = 0; i < 4; ++i, ++direction)
    {
        direction &= 3;
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
    uint8_t validDirections = staff_get_valid_patrol_directions(this, NextLoc);

    Direction newDirection = INVALID_DIRECTION;
    if (GetNextIsSurface())
    {
        newDirection = staff_direction_surface(this, scenario_rand() & 3);
    }
    else
    {
        auto* pathElement = map_get_path_element_at(TileCoordsXYZ{ NextLoc });
        if (pathElement == nullptr)
            return true;

        newDirection = staff_direction_path(this, validDirections, pathElement);
    }

    CoordsXY chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];

    while (!map_is_location_valid(chosenTile))
    {
        newDirection = staff_direction_surface(this, scenario_rand() & 3);
        chosenTile = CoordsXY{ NextLoc } + CoordsDirectionDelta[newDirection];
    }

    direction = newDirection;
    destination_x = chosenTile.x + 16;
    destination_y = chosenTile.y + 16;
    destination_tolerance = (scenario_rand() & 7) + 2;

    return false;
}

/**
 *
 *  rct2: 0x006C086D
 */
static void staff_entertainer_update_nearby_peeps(Peep* peep)
{
    uint16_t spriteIndex;
    Peep* guest;

    FOR_ALL_GUESTS (spriteIndex, guest)
    {
        if (guest->x == LOCATION_NULL)
            continue;

        int16_t z_dist = abs(peep->z - guest->z);
        if (z_dist > 48)
            continue;

        int16_t x_dist = abs(peep->x - guest->x);
        int16_t y_dist = abs(peep->y - guest->y);

        if (x_dist > 96)
            continue;

        if (y_dist > 96)
            continue;

        if (peep->state == PEEP_STATE_WALKING)
        {
            peep->happiness_target = std::min(peep->happiness_target + 4, PEEP_MAX_HAPPINESS);
        }
        else if (peep->state == PEEP_STATE_QUEUING)
        {
            if (peep->time_in_queue > 200)
            {
                peep->time_in_queue -= 200;
            }
            else
            {
                peep->time_in_queue = 0;
            }
            peep->happiness_target = std::min(peep->happiness_target + 3, PEEP_MAX_HAPPINESS);
        }
    }
}

/**
 *
 *  rct2: 0x006C05AE
 */
bool Staff::DoEntertainerPathFinding()
{
    if (((scenario_rand() & 0xFFFF) <= 0x4000) && (action == PEEP_ACTION_NONE_1 || action == PEEP_ACTION_NONE_2))
    {
        action = (scenario_rand() & 1) ? PEEP_ACTION_WAVE_2 : PEEP_ACTION_JOY;
        action_frame = 0;
        action_sprite_image_offset = 0;

        UpdateCurrentActionSpriteType();
        staff_entertainer_update_nearby_peeps(this);
    }

    return DoMiscPathFinding();
}

/**
 *
 *  rct2: 0x006BF926
 */
bool Staff::DoPathFinding()
{
    switch (staff_type)
    {
        case STAFF_TYPE_HANDYMAN:
            return DoHandymanPathFinding();
        case STAFF_TYPE_MECHANIC:
            return DoMechanicPathFinding();
        case STAFF_TYPE_SECURITY:
            return DoMiscPathFinding();
        case STAFF_TYPE_ENTERTAINER:
            return DoEntertainerPathFinding();

        default:
            assert(false);
            return 0;
    }
}

colour_t staff_get_colour(uint8_t staffType)
{
    switch (staffType)
    {
        case STAFF_TYPE_HANDYMAN:
            return gStaffHandymanColour;
        case STAFF_TYPE_MECHANIC:
            return gStaffMechanicColour;
        case STAFF_TYPE_SECURITY:
            return gStaffSecurityColour;
        case STAFF_TYPE_ENTERTAINER:
            return 0;
        default:
            assert(false);
            return 0;
    }
}

bool staff_set_colour(uint8_t staffType, colour_t value)
{
    switch (staffType)
    {
        case STAFF_TYPE_HANDYMAN:
            gStaffHandymanColour = value;
            break;
        case STAFF_TYPE_MECHANIC:
            gStaffMechanicColour = value;
            break;
        case STAFF_TYPE_SECURITY:
            gStaffSecurityColour = value;
            break;
        default:
            return false;
    }
    return true;
}

uint32_t staff_get_available_entertainer_costumes()
{
    uint32_t entertainerCostumes = 0;
    for (int32_t i = 0; i < MAX_SCENERY_GROUP_OBJECTS; i++)
    {
        if (scenery_group_is_invented(i))
        {
            const auto sgEntry = get_scenery_group_entry(i);
            entertainerCostumes |= sgEntry->entertainer_costumes;
        }
    }

    // For some reason the flags are +4 from the actual costume IDs
    entertainerCostumes >>= 4;

    // Fix #6593: force enable the default costumes, which normally get enabled through the default scenery groups.
    entertainerCostumes |= (1 << ENTERTAINER_COSTUME_PANDA) | (1 << ENTERTAINER_COSTUME_TIGER)
        | (1 << ENTERTAINER_COSTUME_ELEPHANT);

    return entertainerCostumes;
}

int32_t staff_get_available_entertainer_costume_list(uint8_t* costumeList)
{
    uint32_t availableCostumes = staff_get_available_entertainer_costumes();
    int32_t numCostumes = 0;
    for (uint8_t i = 0; i < ENTERTAINER_COSTUME_COUNT; i++)
    {
        if (availableCostumes & (1 << i))
        {
            costumeList[numCostumes++] = i;
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
        if (auto loc = UpdateAction())
        {
            int16_t checkZ = tile_element_height(*loc);
            MoveTo((*loc).x, (*loc).y, checkZ);
            return;
        }

        var_37++;

        if (var_37 == 1)
        {
            SwitchToSpecialSprite(2);
        }

        if (var_37 == std::size(_MowingWaypoints))
        {
            StateReset();
            return;
        }

        destination_x = _MowingWaypoints[var_37].x + NextLoc.x;
        destination_y = _MowingWaypoints[var_37].y + NextLoc.y;

        if (var_37 != 7)
            continue;

        auto surfaceElement = map_get_surface_element_at(NextLoc);
        if (surfaceElement != nullptr && surfaceElement->CanGrassGrow())
        {
            surfaceElement->SetGrassLength(GRASS_LENGTH_MOWED);
            map_invalidate_tile_zoom0({ NextLoc, surfaceElement->GetBaseZ(), surfaceElement->GetBaseZ() + 16 });
        }
        staff_lawns_mown++;
        window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
    }
}

/**
 *
 *  rct2: 0x006BF7E6
 */
void Staff::UpdateWatering()
{
    staff_mowing_timeout = 0;
    if (sub_state == 0)
    {
        if (!CheckForPath())
            return;

        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        sprite_direction = (var_37 & 3) << 3;
        action = PEEP_ACTION_STAFF_WATERING;
        action_frame = 0;
        action_sprite_image_offset = 0;
        UpdateCurrentActionSpriteType();

        sub_state = 1;
    }
    else if (sub_state == 1)
    {
        if (action != PEEP_ACTION_NONE_2)
        {
            UpdateAction();
            Invalidate();
            return;
        }

        auto actionLoc = CoordsXY{ NextLoc } + CoordsDirectionDelta[var_37];

        TileElement* tile_element = map_get_first_element_at(actionLoc);
        if (tile_element == nullptr)
            return;

        do
        {
            if (tile_element->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
                continue;

            if (abs(NextLoc.z - tile_element->GetBaseZ()) > 4 * COORDS_Z_STEP)
                continue;

            rct_scenery_entry* scenery_entry = tile_element->AsSmallScenery()->GetEntry();

            if (!scenery_small_entry_has_flag(scenery_entry, SMALL_SCENERY_FLAG_CAN_BE_WATERED))
                continue;

            tile_element->AsSmallScenery()->SetAge(0);
            map_invalidate_tile_zoom0({ actionLoc, tile_element->GetBaseZ(), tile_element->GetClearanceZ() });
            staff_gardens_watered++;
            window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
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
    staff_mowing_timeout = 0;

    if (sub_state == 0)
    {
        if (!CheckForPath())
            return;

        uint8_t pathingResult;
        PerformNextAction(pathingResult);
        if (!(pathingResult & PATHING_DESTINATION_REACHED))
            return;

        sprite_direction = (var_37 & 3) << 3;
        action = PEEP_ACTION_STAFF_EMPTY_BIN;
        action_frame = 0;
        action_sprite_image_offset = 0;
        UpdateCurrentActionSpriteType();

        sub_state = 1;
    }
    else if (sub_state == 1)
    {
        if (action == PEEP_ACTION_NONE_2)
        {
            StateReset();
            return;
        }

        UpdateAction();
        Invalidate();

        if (action_frame != 11)
            return;

        TileElement* tile_element = map_get_first_element_at(NextLoc);
        if (tile_element == nullptr)
            return;

        for (;; tile_element++)
        {
            if (tile_element->GetType() == TILE_ELEMENT_TYPE_PATH)
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

        rct_scenery_entry* scenery_entry = tile_element->AsPath()->GetAdditionEntry();
        if (!(scenery_entry->path_bit.flags & PATH_BIT_FLAG_IS_BIN) || tile_element->AsPath()->IsBroken()
            || tile_element->AsPath()->AdditionIsGhost())
        {
            StateReset();
            return;
        }

        uint8_t additionStatus = tile_element->AsPath()->GetAdditionStatus() | ((3 << var_37) << var_37);
        tile_element->AsPath()->SetAdditionStatus(additionStatus);

        map_invalidate_tile_zoom0({ NextLoc, tile_element->GetBaseZ(), tile_element->GetClearanceZ() });
        staff_bins_emptied++;
        window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
    }
}

/**
 *
 *  rct2: 0x6BF641
 */
void Staff::UpdateSweeping()
{
    staff_mowing_timeout = 0;
    if (!CheckForPath())
        return;

    if (action == PEEP_ACTION_STAFF_SWEEP && action_frame == 8)
    {
        // Remove sick at this location
        litter_remove_at(x, y, z);
        staff_litter_swept++;
        window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
    }
    if (auto loc = UpdateAction())
    {
        int16_t actionZ = GetZOnSlope((*loc).x, (*loc).y);
        MoveTo((*loc).x, (*loc).y, actionZ);
        return;
    }

    var_37++;
    if (var_37 != 2)
    {
        action = PEEP_ACTION_STAFF_SWEEP;
        action_frame = 0;
        action_sprite_image_offset = 0;
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
    auto ride = get_ride(current_ride);
    if (ride == nullptr)
    {
        SetState(PEEP_STATE_FALLING);
        return;
    }

    if (ride_get_exit_location(ride, current_ride_station).isNull())
    {
        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
        SetState(PEEP_STATE_FALLING);
        return;
    }

    if (ride->mechanic_status != RIDE_MECHANIC_STATUS_HEADING || !(ride->lifecycle_flags & RIDE_LIFECYCLE_DUE_INSPECTION))
    {
        SetState(PEEP_STATE_FALLING);
        return;
    }

    if (sub_state == 0)
    {
        mechanic_time_since_call = 0;
        peep_reset_pathfind_goal(this);
        sub_state = 2;
    }

    if (sub_state <= 3)
    {
        mechanic_time_since_call++;
        if (mechanic_time_since_call > 2500)
        {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_DUE_INSPECTION && ride->mechanic_status == RIDE_MECHANIC_STATUS_HEADING)
            {
                ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            }
            SetState(PEEP_STATE_FALLING);
            return;
        }

        if (!CheckForPath())
            return;

        uint8_t pathingResult;
        TileElement* rideEntranceExitElement;
        PerformNextAction(pathingResult, rideEntranceExitElement);

        if (!(pathingResult & PATHING_RIDE_EXIT) && !(pathingResult & PATHING_RIDE_ENTRANCE))
        {
            return;
        }

        if (current_ride != rideEntranceExitElement->AsEntrance()->GetRideIndex())
            return;

        uint8_t exit_index = rideEntranceExitElement->AsEntrance()->GetStationIndex();

        if (current_ride_station != exit_index)
            return;

        if (pathingResult & PATHING_RIDE_ENTRANCE)
        {
            if (!ride_get_exit_location(ride, exit_index).isNull())
            {
                return;
            }
        }

        direction = rideEntranceExitElement->GetDirection();

        int32_t destX = NextLoc.x + 16 + DirectionOffsets[direction].x * 53;
        int32_t destY = NextLoc.y + 16 + DirectionOffsets[direction].y * 53;

        destination_x = destX;
        destination_y = destY;
        destination_tolerance = 2;
        sprite_direction = direction << 3;

        z = rideEntranceExitElement->base_height * 4;
        sub_state = 4;
        // Falls through into sub_state 4
    }

    int16_t delta_y = abs(y - destination_y);
    if (auto loc = UpdateAction())
    {
        int32_t newZ = ride->stations[current_ride_station].GetBaseZ();

        if (delta_y < 20)
        {
            newZ += RideData5[ride->type].z;
        }

        MoveTo((*loc).x, (*loc).y, newZ);
        return;
    }

    SetState(PEEP_STATE_INSPECTING);
    sub_state = 0;
}

/**
 *
 *  rct2: 0x006C0CB8
 */
void Staff::UpdateAnswering()
{
    auto ride = get_ride(current_ride);
    if (ride == nullptr || ride->mechanic_status != RIDE_MECHANIC_STATUS_HEADING)
    {
        SetState(PEEP_STATE_FALLING);
        return;
    }

    if (sub_state == 0)
    {
        action = PEEP_ACTION_STAFF_ANSWER_CALL;
        action_frame = 0;
        action_sprite_image_offset = 0;

        UpdateCurrentActionSpriteType();

        sub_state = 1;
        peep_window_state_update(this);
        return;
    }
    else if (sub_state == 1)
    {
        if (action == PEEP_ACTION_NONE_2)
        {
            sub_state = 2;
            peep_window_state_update(this);
            mechanic_time_since_call = 0;
            peep_reset_pathfind_goal(this);
            return;
        }
        UpdateAction();
        Invalidate();
        return;
    }
    else if (sub_state <= 3)
    {
        mechanic_time_since_call++;
        if (mechanic_time_since_call > 2500)
        {
            ride->mechanic_status = RIDE_MECHANIC_STATUS_CALLING;
            ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
            SetState(PEEP_STATE_FALLING);
            return;
        }

        if (!CheckForPath())
            return;

        uint8_t pathingResult;
        TileElement* rideEntranceExitElement;
        PerformNextAction(pathingResult, rideEntranceExitElement);

        if (!(pathingResult & PATHING_RIDE_EXIT) && !(pathingResult & PATHING_RIDE_ENTRANCE))
        {
            return;
        }

        if (current_ride != rideEntranceExitElement->AsEntrance()->GetRideIndex())
            return;

        uint8_t exit_index = rideEntranceExitElement->AsEntrance()->GetStationIndex();

        if (current_ride_station != exit_index)
            return;

        if (pathingResult & PATHING_RIDE_ENTRANCE)
        {
            if (!ride_get_exit_location(ride, exit_index).isNull())
            {
                return;
            }
        }

        direction = rideEntranceExitElement->GetDirection();

        int32_t destX = NextLoc.x + 16 + DirectionOffsets[direction].x * 53;
        int32_t destY = NextLoc.y + 16 + DirectionOffsets[direction].y * 53;

        destination_x = destX;
        destination_y = destY;
        destination_tolerance = 2;
        sprite_direction = direction << 3;

        z = rideEntranceExitElement->base_height * 4;
        sub_state = 4;
        // Falls through into sub_state 4
    }

    int16_t delta_y = abs(y - destination_y);
    if (auto loc = UpdateAction())
    {
        int32_t newZ = ride->stations[current_ride_station].GetBaseZ();

        if (delta_y < 20)
        {
            newZ += RideData5[ride->type].z;
        }

        MoveTo((*loc).x, (*loc).y, newZ);
        return;
    }

    SetState(PEEP_STATE_FIXING);
    sub_state = 0;
}

/** rct2: 0x00992A5C */
static constexpr const CoordsXY _WateringUseOffsets[] = {
    { 3, 16 }, { 16, 29 }, { 29, 16 }, { 16, 3 }, { 3, 29 }, { 29, 29 }, { 29, 3 }, { 3, 3 },
};

/**
 *
 *  rct2: 0x006BF483
 */
static int32_t peep_update_patrolling_find_watering(Peep* peep)
{
    if (!(peep->staff_orders & STAFF_ORDERS_WATER_FLOWERS))
        return 0;

    uint8_t chosen_position = scenario_rand() & 7;
    for (int32_t i = 0; i < 8; ++i, ++chosen_position)
    {
        chosen_position &= 7;

        auto chosenLoc = CoordsXY{ peep->NextLoc } + CoordsDirectionDelta[chosen_position];

        TileElement* tile_element = map_get_first_element_at(chosenLoc);

        // This seems to happen in some SV4 files.
        if (tile_element == nullptr)
        {
            continue;
        }

        do
        {
            if (tile_element->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
            {
                continue;
            }

            auto z_diff = abs(peep->NextLoc.z - tile_element->GetBaseZ());

            if (z_diff >= 4 * COORDS_Z_STEP)
            {
                continue;
            }

            rct_scenery_entry* sceneryEntry = tile_element->AsSmallScenery()->GetEntry();

            if (sceneryEntry == nullptr || !scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_CAN_BE_WATERED))
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

            peep->SetState(PEEP_STATE_WATERING);
            peep->var_37 = chosen_position;

            peep->sub_state = 0;
            peep->destination_x = (peep->x & 0xFFE0) + _WateringUseOffsets[chosen_position].x;
            peep->destination_y = (peep->y & 0xFFE0) + _WateringUseOffsets[chosen_position].y;
            peep->destination_tolerance = 3;

            return 1;
        } while (!(tile_element++)->IsLastForTile());
    }
    return 0;
}

/**
 *
 *  rct2: 0x006BF3A1
 */
static int32_t peep_update_patrolling_find_bin(Peep* peep)
{
    if (!(peep->staff_orders & STAFF_ORDERS_EMPTY_BINS))
        return 0;

    if (peep->GetNextIsSurface())
        return 0;

    TileElement* tileElement = map_get_first_element_at(peep->NextLoc);
    if (tileElement == nullptr)
        return 0;

    for (;; tileElement++)
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH && (tileElement->GetBaseZ() == peep->NextLoc.z))
            break;

        if (tileElement->IsLastForTile())
            return 0;
    }

    if (!tileElement->AsPath()->HasAddition())
        return 0;
    rct_scenery_entry* sceneryEntry = tileElement->AsPath()->GetAdditionEntry();
    if (sceneryEntry == nullptr)
        return 0;

    if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BIN))
        return 0;

    if (tileElement->AsPath()->IsBroken())
        return 0;

    if (tileElement->AsPath()->AdditionIsGhost())
        return 0;

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
        return 0;

    peep->var_37 = chosen_position;
    peep->SetState(PEEP_STATE_EMPTYING_BIN);

    peep->sub_state = 0;
    peep->destination_x = (peep->x & 0xFFE0) + BinUseOffsets[chosen_position].x;
    peep->destination_y = (peep->y & 0xFFE0) + BinUseOffsets[chosen_position].y;
    peep->destination_tolerance = 3;
    return 1;
}

/**
 *
 *  rct2: 0x006BF322
 */
static int32_t peep_update_patrolling_find_grass(Peep* peep)
{
    if (!(peep->staff_orders & STAFF_ORDERS_MOWING))
        return 0;

    if (peep->staff_mowing_timeout < 12)
        return 0;

    if (!(peep->GetNextIsSurface()))
        return 0;

    auto surfaceElement = map_get_surface_element_at(peep->NextLoc);
    if (surfaceElement != nullptr && surfaceElement->CanGrassGrow())
    {
        if ((surfaceElement->GetGrassLength() & 0x7) >= GRASS_LENGTH_CLEAR_1)
        {
            peep->SetState(PEEP_STATE_MOWING);
            peep->var_37 = 0;
            // Original code used .y for both x and y. Changed to .x to make more sense (both x and y are 28)
            peep->destination_x = peep->NextLoc.x + _MowingWaypoints[0].x;
            peep->destination_y = peep->NextLoc.y + _MowingWaypoints[0].y;
            peep->destination_tolerance = 3;
            return 1;
        }
    }
    return 0;
}

/**
 *
 *  rct2: 0x006BF295
 */
static int32_t peep_update_patrolling_find_sweeping(Peep* peep)
{
    if (!(peep->staff_orders & STAFF_ORDERS_SWEEPING))
        return 0;

    uint16_t sprite_id = sprite_get_first_in_quadrant(peep->x, peep->y);

    for (rct_sprite* sprite = nullptr; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->generic.next_in_quadrant)
    {
        sprite = get_sprite(sprite_id);

        if (sprite->generic.linked_list_index != SPRITE_LIST_LITTER)
            continue;

        uint16_t z_diff = abs(peep->z - sprite->litter.z);

        if (z_diff >= 16)
            continue;

        peep->SetState(PEEP_STATE_SWEEPING);
        peep->var_37 = 0;
        peep->destination_x = sprite->litter.x;
        peep->destination_y = sprite->litter.y;
        peep->destination_tolerance = 5;
        return 1;
    }

    return 0;
}

void Staff::Tick128UpdateStaff()
{
    if (staff_type != STAFF_TYPE_SECURITY)
        return;

    PeepSpriteType newSpriteType = PEEP_SPRITE_TYPE_SECURITY_ALT;
    if (state != PEEP_STATE_PATROLLING)
        newSpriteType = PEEP_SPRITE_TYPE_SECURITY;

    if (sprite_type == newSpriteType)
        return;

    sprite_type = newSpriteType;
    action_sprite_image_offset = 0;
    no_action_frame_num = 0;
    if (action < PEEP_ACTION_NONE_1)
        action = PEEP_ACTION_NONE_2;

    peep_flags &= ~PEEP_FLAGS_SLOW_WALK;
    if (gSpriteTypeToSlowWalkMap[newSpriteType])
    {
        peep_flags |= PEEP_FLAGS_SLOW_WALK;
    }

    action_sprite_type = PEEP_ACTION_SPRITE_TYPE_INVALID;
    UpdateCurrentActionSpriteType();
}

bool Staff::IsMechanic() const
{
    return (sprite_identifier == SPRITE_IDENTIFIER_PEEP && type == PEEP_TYPE_STAFF && staff_type == STAFF_TYPE_MECHANIC);
}

void Staff::UpdateStaff(uint32_t stepsToTake)
{
    switch (state)
    {
        case PEEP_STATE_PATROLLING:
            UpdatePatrolling();
            break;
        case PEEP_STATE_MOWING:
            UpdateMowing();
            break;
        case PEEP_STATE_SWEEPING:
            UpdateSweeping();
            break;
        case PEEP_STATE_ANSWERING:
            UpdateAnswering();
            break;
        case PEEP_STATE_FIXING:
            UpdateFixing(stepsToTake);
            break;
        case PEEP_STATE_INSPECTING:
            UpdateFixing(stepsToTake);
            break;
        case PEEP_STATE_EMPTYING_BIN:
            UpdateEmptyingBin();
            break;
        case PEEP_STATE_WATERING:
            UpdateWatering();
            break;
        case PEEP_STATE_HEADING_TO_INSPECTION:
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

    uint8_t pathingResult;
    PerformNextAction(pathingResult);
    if (!(pathingResult & PATHING_DESTINATION_REACHED))
        return;

    if (GetNextIsSurface())
    {
        auto surfaceElement = map_get_surface_element_at(NextLoc);

        if (surfaceElement != nullptr)
        {
            int32_t water_height = surfaceElement->GetWaterHeight();
            if (water_height > 0)
            {
                MoveTo(x, y, water_height);
                SetState(PEEP_STATE_FALLING);
                return;
            }
        }
    }

    if (staff_type != STAFF_TYPE_HANDYMAN)
        return;

    if (peep_update_patrolling_find_sweeping(this))
        return;

    if (peep_update_patrolling_find_grass(this))
        return;

    if (peep_update_patrolling_find_bin(this))
        return;

    peep_update_patrolling_find_watering(this);
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
    )
};
// clang-format on

/**
 *
 *  rct2: 0x006C0E8B
 * Also used by inspecting.
 */
void Staff::UpdateFixing(int32_t steps)
{
    auto ride = get_ride(current_ride);
    if (ride == nullptr)
    {
        SetState(PEEP_STATE_FALLING);
        return;
    }

    bool progressToNextSubstate = true;
    bool firstRun = true;

    if ((state == PEEP_STATE_INSPECTING)
        && (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)))
    {
        // Ride has broken down since Mechanic was called to inspect it.
        // Mechanic identifies the breakdown and switches to fixing it.
        state = PEEP_STATE_FIXING;
    }

    while (progressToNextSubstate)
    {
        switch (sub_state)
        {
            case PEEP_FIXING_ENTER_STATION:
                next_flags &= ~PEEP_NEXT_FLAG_IS_SLOPED;
                progressToNextSubstate = UpdateFixingEnterStation(ride);
                break;

            case PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE:
                progressToNextSubstate = UpdateFixingMoveToBrokenDownVehicle(firstRun, ride);
                break;

            case PEEP_FIXING_FIX_VEHICLE_CLOSED_RESTRAINTS:
            case PEEP_FIXING_FIX_VEHICLE_CLOSED_DOORS:
            case PEEP_FIXING_FIX_VEHICLE_OPEN_RESTRAINTS:
            case PEEP_FIXING_FIX_VEHICLE_OPEN_DOORS:
                progressToNextSubstate = UpdateFixingFixVehicle(firstRun, ride);
                break;

            case PEEP_FIXING_FIX_VEHICLE_MALFUNCTION:
                progressToNextSubstate = UpdateFixingFixVehicleMalfunction(firstRun, ride);
                break;

            case PEEP_FIXING_MOVE_TO_STATION_END:
                progressToNextSubstate = UpdateFixingMoveToStationEnd(firstRun, ride);
                break;

            case PEEP_FIXING_FIX_STATION_END:
                progressToNextSubstate = UpdateFixingFixStationEnd(firstRun);
                break;

            case PEEP_FIXING_MOVE_TO_STATION_START:
                progressToNextSubstate = UpdateFixingMoveToStationStart(firstRun, ride);
                break;

            case PEEP_FIXING_FIX_STATION_START:
                progressToNextSubstate = UpdateFixingFixStationStart(firstRun, ride);
                break;

            case PEEP_FIXING_FIX_STATION_BRAKES:
                progressToNextSubstate = UpdateFixingFixStationBrakes(firstRun, ride);
                break;

            case PEEP_FIXING_MOVE_TO_STATION_EXIT:
                progressToNextSubstate = UpdateFixingMoveToStationExit(firstRun, ride);
                break;

            case PEEP_FIXING_FINISH_FIX_OR_INSPECT:
                progressToNextSubstate = UpdateFixingFinishFixOrInspect(firstRun, steps, ride);
                break;

            case PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT:
                progressToNextSubstate = UpdateFixingLeaveByEntranceExit(firstRun, ride);
                break;

            default:
                log_error("Invalid substate");
                progressToNextSubstate = false;
        }

        firstRun = false;

        if (!progressToNextSubstate)
        {
            break;
        }

        int32_t subState = sub_state;
        uint32_t sub_state_sequence_mask = FixingSubstatesForBreakdown[8];

        if (state != PEEP_STATE_INSPECTING)
        {
            sub_state_sequence_mask = FixingSubstatesForBreakdown[ride->breakdown_reason_pending];
        }

        do
        {
            subState++;
        } while ((sub_state_sequence_mask & (1 << subState)) == 0);

        sub_state = subState & 0xFF;
    }
}

/**
 * rct2: 0x006C0EEC
 * fixing sub_state: enter_station - applies to fixing all break down reasons and ride inspections.
 */
bool Staff::UpdateFixingEnterStation(Ride* ride)
{
    ride->mechanic_status = RIDE_MECHANIC_STATUS_FIXING;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;

    return true;
}

/**
 * rct2: 0x006C0F09
 * fixing sub_state: move_to_broken_down_vehicle - applies to fixing all vehicle specific breakdown reasons
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingMoveToBrokenDownVehicle(bool firstRun, Ride* ride)
{
    if (!firstRun)
    {
        Vehicle* vehicle = ride_get_broken_vehicle(ride);
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

            uint8_t trackType = vehicle->track_type >> 2;
            if (trackType == TRACK_ELEM_END_STATION)
            {
                break;
            }

            if (trackType == TRACK_ELEM_BEGIN_STATION)
            {
                break;
            }

            if (trackType == TRACK_ELEM_MIDDLE_STATION)
            {
                break;
            }

            vehicle = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
        }

        CoordsXY offset = DirectionOffsets[direction];
        destination_x = (offset.x * -12) + vehicle->x;
        destination_y = (offset.y * -12) + vehicle->y;
        destination_tolerance = 2;
    }

    if (auto loc = UpdateAction())
    {
        MoveTo(loc->x, loc->y, z);
        return false;
    }

    return true;
}

/**
 * rct2: 0x006C0FD3
 * fixing sub_state: fix_vehicle - applies to fixing vehicle with:
 * 1. restraints stuck closed,
 * 2. doors stuck closed,
 * 3. restrains stuck open,
 * 4. doors stuck open.
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingFixVehicle(bool firstRun, Ride* ride)
{
    if (!firstRun)
    {
        sprite_direction = direction << 3;

        action = (scenario_rand() & 1) ? PEEP_ACTION_STAFF_FIX_2 : PEEP_ACTION_STAFF_FIX;
        action_sprite_image_offset = 0;
        action_frame = 0;
        UpdateCurrentActionSpriteType();
    }

    if (action == PEEP_ACTION_NONE_2)
    {
        return true;
    }

    UpdateAction();
    Invalidate();

    uint8_t actionFrame = (action == PEEP_ACTION_STAFF_FIX) ? 0x25 : 0x50;
    if (action_frame != actionFrame)
    {
        return false;
    }

    Vehicle* vehicle = ride_get_broken_vehicle(ride);
    if (vehicle == nullptr)
    {
        return true;
    }

    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_BROKEN_CAR;

    return false;
}

/**
 * rct2: 0x006C107B
 * fixing sub_state: fix_vehicle_malfunction - applies fixing to vehicle malfunction.
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingFixVehicleMalfunction(bool firstRun, Ride* ride)
{
    if (!firstRun)
    {
        sprite_direction = direction << 3;
        action = PEEP_ACTION_STAFF_FIX_3;
        action_sprite_image_offset = 0;
        action_frame = 0;

        UpdateCurrentActionSpriteType();
    }

    if (action == PEEP_ACTION_NONE_2)
    {
        return true;
    }

    UpdateAction();
    Invalidate();

    if (action_frame != 0x65)
    {
        return false;
    }

    Vehicle* vehicle = ride_get_broken_vehicle(ride);
    if (vehicle == nullptr)
    {
        return true;
    }

    vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_BROKEN_TRAIN;

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
 * fixing sub_state: move_to_station_end - applies to fixing station specific breakdowns: safety cut-out, control failure,
 * inspection.
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingMoveToStationEnd(bool firstRun, Ride* ride)
{
    if (!firstRun)
    {
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_HAS_NO_TRACK))
        {
            return true;
        }

        auto stationPos = ride->stations[current_ride_station].GetStart();
        if (stationPos.isNull())
        {
            return true;
        }

        auto tileElement = map_get_track_element_at(stationPos);
        if (tileElement == nullptr)
        {
            log_error("Couldn't find tile_element");
            return false;
        }

        int32_t trackDirection = tileElement->GetDirection();
        CoordsXY offset = _StationFixingOffsets[trackDirection];

        stationPos.x += 16 + offset.x;
        if (offset.x == 0)
        {
            stationPos.x = destination_x;
        }

        stationPos.y += 16 + offset.y;
        if (offset.y == 0)
        {
            stationPos.y = destination_y;
        }

        destination_x = stationPos.x;
        destination_y = stationPos.y;
        destination_tolerance = 2;
    }

    if (auto loc = UpdateAction())
    {
        MoveTo(loc->x, loc->y, z);
        return false;
    }

    return true;
}

/**
 * rct2: 0x006C11F5
 * fixing sub_state: fix_station_end - applies to fixing station specific breakdowns: safety cut-out, control failure,
 * inspection.
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingFixStationEnd(bool firstRun)
{
    if (!firstRun)
    {
        sprite_direction = direction << 3;
        action = PEEP_ACTION_STAFF_CHECKBOARD;
        action_frame = 0;
        action_sprite_image_offset = 0;

        UpdateCurrentActionSpriteType();
    }

    if (action == PEEP_ACTION_NONE_2)
    {
        return true;
    }

    UpdateAction();
    Invalidate();

    return false;
}

/**
 * rct2: 0x006C1239
 * fixing sub_state: move_to_station_start
 * 1. applies to fixing station specific breakdowns: safety cut-out, control failure,
 * 2. applies to fixing brake failure,
 * 3. applies to inspection.
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingMoveToStationStart(bool firstRun, Ride* ride)
{
    if (!firstRun)
    {
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_HAS_NO_TRACK))
        {
            return true;
        }

        auto stationPosition = ride->stations[current_ride_station].GetStart();
        if (stationPosition.isNull())
        {
            return true;
        }

        CoordsXYE input;
        input.x = stationPosition.x;
        input.y = stationPosition.y;
        input.element = map_get_track_element_at_from_ride({ input.x, input.y, stationPosition.z }, current_ride);
        if (input.element == nullptr)
        {
            return true;
        }

        uint8_t stationDirection = 0;
        track_begin_end trackBeginEnd;
        while (track_block_get_previous(input.x, input.y, input.element, &trackBeginEnd))
        {
            if (track_element_is_station(trackBeginEnd.begin_element))
            {
                input.x = trackBeginEnd.begin_x;
                input.y = trackBeginEnd.begin_y;
                input.element = trackBeginEnd.begin_element;

                stationDirection = trackBeginEnd.begin_element->GetDirection();
                continue;
            }

            break;
        }

        // loc_6C12ED:
        uint16_t destinationX = input.x + 16;
        uint16_t destinationY = input.y + 16;

        CoordsXY offset = _StationFixingOffsets[stationDirection];

        destinationX -= offset.x;
        if (offset.x == 0)
        {
            destinationX = destination_x;
        }

        destinationY -= offset.y;
        if (offset.y == 0)
        {
            destinationY = destination_y;
        }

        destination_x = destinationX;
        destination_y = destinationY;
        destination_tolerance = 2;
    }

    if (auto loc = UpdateAction())
    {
        MoveTo(loc->x, loc->y, z);
        return false;
    }

    return true;
}

/**
 * rct2: 0x006C1368
 * fixing sub_state: fix_station_start
 * 1. applies to fixing station specific breakdowns: safety cut-out, control failure,
 * 2. applies to inspection.
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingFixStationStart(bool firstRun, Ride* ride)
{
    if (!firstRun)
    {
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_HAS_NO_TRACK))
        {
            return true;
        }

        sprite_direction = direction << 3;

        action = PEEP_ACTION_STAFF_FIX;
        action_frame = 0;
        action_sprite_image_offset = 0;

        UpdateCurrentActionSpriteType();
    }

    if (action == PEEP_ACTION_NONE_2)
    {
        return true;
    }

    UpdateAction();

    return false;
}

/**
 * rct2: 0x006C13CE
 * fixing sub_state: fix_station_brakes - applies to fixing brake failure
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingFixStationBrakes(bool firstRun, Ride* ride)
{
    if (!firstRun)
    {
        sprite_direction = direction << 3;

        action = PEEP_ACTION_STAFF_FIX_GROUND;
        action_frame = 0;
        action_sprite_image_offset = 0;

        UpdateCurrentActionSpriteType();
    }

    if (action == PEEP_ACTION_NONE_2)
    {
        return true;
    }

    UpdateAction();
    Invalidate();

    if (action_frame == 0x28)
    {
        ride->mechanic_status = RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
    }

    if (action_frame == 0x13 || action_frame == 0x19 || action_frame == 0x1F || action_frame == 0x25 || action_frame == 0x2B)
    {
        audio_play_sound_at_location(SoundId::MechanicFix, { x, y, z });
    }

    return false;
}

/**
 * rct2: 0x006C1474
 * fixing sub_state: move_to_station_exit - applies to fixing all failures & inspections
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingMoveToStationExit(bool firstRun, Ride* ride)
{
    if (!firstRun)
    {
        auto stationPosition = ride_get_exit_location(ride, current_ride_station).ToCoordsXY();
        if (stationPosition.isNull())
        {
            stationPosition = ride_get_entrance_location(ride, current_ride_station).ToCoordsXY();

            if (stationPosition.isNull())
            {
                return true;
            }
        }

        stationPosition = stationPosition.ToTileCentre();

        CoordsXY stationPlatformDirection = DirectionOffsets[direction];
        stationPosition.x += stationPlatformDirection.x * 20;
        stationPosition.y += stationPlatformDirection.y * 20;

        destination_x = stationPosition.x;
        destination_y = stationPosition.y;
        destination_tolerance = 2;
    }

    if (auto loc = UpdateAction())
    {
        MoveTo(loc->x, loc->y, z);
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * rct2: 0x006C1504
 * fixing sub_state: finish_fix_or_inspect - applies to fixing all failures & inspections
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingFinishFixOrInspect(bool firstRun, int32_t steps, Ride* ride)
{
    if (!firstRun)
    {
        ride->mechanic_status = RIDE_MECHANIC_STATUS_UNDEFINED;

        if (state == PEEP_STATE_INSPECTING)
        {
            UpdateRideInspected(current_ride);

            staff_rides_inspected++;
            window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME | RIDE_INVALIDATE_RIDE_LIST;

            return true;
        }

        staff_rides_fixed++;
        window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME | RIDE_INVALIDATE_RIDE_LIST;

        sprite_direction = direction << 3;
        action = PEEP_ACTION_STAFF_ANSWER_CALL_2;
        action_frame = 0;
        action_sprite_image_offset = 0;

        UpdateCurrentActionSpriteType();
    }

    if (action != PEEP_ACTION_NONE_2)
    {
        UpdateAction();
        Invalidate();
        return false;
    }

    ride_fix_breakdown(ride, steps);

    return true;
}

/**
 * rct2: 0x006C157E
 * fixing sub_state: leave_by_entrance_exit - applies to fixing all failures & inspections
 * - see FixingSubstatesForBreakdown[]
 */
bool Staff::UpdateFixingLeaveByEntranceExit(bool firstRun, Ride* ride)
{
    if (!firstRun)
    {
        auto exitPosition = ride_get_exit_location(ride, current_ride_station).ToCoordsXY();
        if (exitPosition.isNull())
        {
            exitPosition = ride_get_entrance_location(ride, current_ride_station).ToCoordsXY();

            if (exitPosition.isNull())
            {
                SetState(PEEP_STATE_FALLING);
                return false;
            }
        }

        exitPosition = exitPosition.ToTileCentre();

        CoordsXY ebx_direction = DirectionOffsets[direction];
        exitPosition.x -= ebx_direction.x * 19;
        exitPosition.y -= ebx_direction.y * 19;

        destination_x = exitPosition.x;
        destination_y = exitPosition.y;
        destination_tolerance = 2;
    }

    int16_t xy_distance;
    if (auto loc = UpdateAction(xy_distance))
    {
        uint16_t stationHeight = ride->stations[current_ride_station].GetBaseZ();

        if (xy_distance >= 16)
        {
            stationHeight += RideData5[ride->type].z;
        }

        MoveTo(loc->x, loc->y, stationHeight);
        return false;
    }
    SetState(PEEP_STATE_FALLING);
    return false;
}

/**
 * rct2: 0x6B7588
 */
void Staff::UpdateRideInspected(ride_id_t rideIndex)
{
    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;
        ride->reliability += ((100 - ride->reliability_percentage) / 4) * (scenario_rand() & 0xFF);
        ride->last_inspection = 0;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE | RIDE_INVALIDATE_RIDE_MAIN
            | RIDE_INVALIDATE_RIDE_LIST;
    }
}
