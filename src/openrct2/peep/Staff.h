/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _STAFF_H_
#define _STAFF_H_

#include "../common.h"
#include "Peep.h"

#define STAFF_MAX_COUNT 200
// The number of elements in the gStaffPatrolAreas array per staff member. Every bit in the array represents a 4x4 square.
// Right now, it's a 32-bit array like in RCT2. 32 * 128 = 4096 bits, which is also the number of 4x4 squares on a 256x256 map.
#define STAFF_PATROL_AREA_SIZE 128

enum class StaffMode : uint8_t
{
    None,
    Walk,
    Patrol = 3
};

enum STAFF_ORDERS
{
    STAFF_ORDERS_SWEEPING = (1 << 0),
    STAFF_ORDERS_WATER_FLOWERS = (1 << 1),
    STAFF_ORDERS_EMPTY_BINS = (1 << 2),
    STAFF_ORDERS_MOWING = (1 << 3),
    STAFF_ORDERS_INSPECT_RIDES = (1 << 0),
    STAFF_ORDERS_FIX_RIDES = (1 << 1)
};

enum ENTERTAINER_COSTUME : uint8_t
{
    ENTERTAINER_COSTUME_PANDA,
    ENTERTAINER_COSTUME_TIGER,
    ENTERTAINER_COSTUME_ELEPHANT,
    ENTERTAINER_COSTUME_ROMAN,
    ENTERTAINER_COSTUME_GORILLA,
    ENTERTAINER_COSTUME_SNOWMAN,
    ENTERTAINER_COSTUME_KNIGHT,
    ENTERTAINER_COSTUME_ASTRONAUT,
    ENTERTAINER_COSTUME_BANDIT,
    ENTERTAINER_COSTUME_SHERIFF,
    ENTERTAINER_COSTUME_PIRATE,

    ENTERTAINER_COSTUME_COUNT
};

extern const money32 gStaffWageTable[static_cast<uint8_t>(StaffType::Count)];

extern const rct_string_id StaffCostumeNames[ENTERTAINER_COSTUME_COUNT];

extern uint32_t gStaffPatrolAreas[(STAFF_MAX_COUNT + static_cast<uint8_t>(StaffType::Count)) * STAFF_PATROL_AREA_SIZE];
extern StaffMode gStaffModes[STAFF_MAX_COUNT + static_cast<uint8_t>(StaffType::Count)];
extern uint16_t gStaffDrawPatrolAreas;
extern colour_t gStaffHandymanColour;
extern colour_t gStaffMechanicColour;
extern colour_t gStaffSecurityColour;

void staff_reset_modes();
void staff_set_name(uint16_t spriteIndex, const char* name);
bool staff_hire_new_member(StaffType staffType, ENTERTAINER_COSTUME entertainerType);
void staff_update_greyed_patrol_areas();
bool staff_is_location_on_patrol_edge(Peep* mechanic, const CoordsXY& loc);
bool staff_can_ignore_wide_flag(Peep* mechanic, const CoordsXYZ& staffPos, TileElement* path);
void staff_reset_stats();
bool staff_is_patrol_area_set_for_type(StaffType type, const CoordsXY& coords);
void staff_set_patrol_area(int32_t staffIndex, const CoordsXY& coords, bool value);
void staff_toggle_patrol_area(int32_t staffIndex, const CoordsXY& coords);
colour_t staff_get_colour(StaffType staffType);
bool staff_set_colour(StaffType staffType, colour_t value);
uint32_t staff_get_available_entertainer_costumes();
int32_t staff_get_available_entertainer_costume_list(uint8_t* costumeList);

#endif
