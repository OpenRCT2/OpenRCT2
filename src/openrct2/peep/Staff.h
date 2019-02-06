/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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

enum STAFF_MODE
{
    STAFF_MODE_NONE,
    STAFF_MODE_WALK,
    STAFF_MODE_PATROL = 3
};

enum STAFF_TYPE
{
    STAFF_TYPE_HANDYMAN,
    STAFF_TYPE_MECHANIC,
    STAFF_TYPE_SECURITY,
    STAFF_TYPE_ENTERTAINER,

    STAFF_TYPE_COUNT
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

enum ENTERTAINER_COSTUME
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

extern const rct_string_id StaffCostumeNames[ENTERTAINER_COSTUME_COUNT];

extern uint32_t gStaffPatrolAreas[(STAFF_MAX_COUNT + STAFF_TYPE_COUNT) * STAFF_PATROL_AREA_SIZE];
extern uint8_t gStaffModes[STAFF_MAX_COUNT + STAFF_TYPE_COUNT];
extern uint16_t gStaffDrawPatrolAreas;
extern colour_t gStaffHandymanColour;
extern colour_t gStaffMechanicColour;
extern colour_t gStaffSecurityColour;

void game_command_hire_new_staff_member(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_callback_hire_new_staff_member(
    int32_t eax, int32_t ebx, int32_t ecx, int32_t edx, int32_t esi, int32_t edi, int32_t ebp);
void game_command_set_staff_patrol(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_fire_staff_member(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_set_staff_name(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_pickup_staff(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);

void staff_reset_modes();
void staff_set_name(uint16_t spriteIndex, const char* name);
uint16_t hire_new_staff_member(uint8_t staffType);
void staff_update_greyed_patrol_areas();
bool staff_is_location_in_patrol(rct_peep* mechanic, int32_t x, int32_t y);
bool staff_is_location_on_patrol_edge(rct_peep* mechanic, int32_t x, int32_t y);
bool staff_can_ignore_wide_flag(rct_peep* mechanic, int32_t x, int32_t y, uint8_t z, TileElement* path);
int32_t staff_path_finding(rct_peep* peep);
void staff_reset_stats();
bool staff_is_patrol_area_set(int32_t staffIndex, int32_t x, int32_t y);
void staff_set_patrol_area(int32_t staffIndex, int32_t x, int32_t y, bool value);
void staff_toggle_patrol_area(int32_t staffIndex, int32_t x, int32_t y);
colour_t staff_get_colour(uint8_t staffType);
bool staff_set_colour(uint8_t staffType, colour_t value);
uint32_t staff_get_available_entertainer_costumes();
int32_t staff_get_available_entertainer_costume_list(uint8_t* costumeList);

#endif
