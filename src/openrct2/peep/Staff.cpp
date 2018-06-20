/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../audio/audio.h"
#include "../core/Math.hpp"
#include "../core/Util.hpp"
#include "../Context.h"
#include "../config/Config.h"
#include "../Game.h"
#include "../Input.h"
#include "../interface/Viewport.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../object/ObjectList.h"
#include "../ride/Station.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../paint/tile_element/Paint.TileElement.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
#include "Peep.h"
#include "Staff.h"

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
uint32_t   gStaffPatrolAreas[(STAFF_MAX_COUNT + STAFF_TYPE_COUNT) * STAFF_PATROL_AREA_SIZE];
uint8_t    gStaffModes[STAFF_MAX_COUNT + STAFF_TYPE_COUNT];
uint16_t   gStaffDrawPatrolAreas;
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

static inline void staff_autoposition_new_staff_member(rct_peep * newPeep)
{
    // Find a location to place new staff member

    newPeep->state = PEEP_STATE_FALLING;

    int16_t             x, y, z;
    uint32_t             count = 0;
    uint16_t             sprite_index;
    rct_peep *         guest = nullptr;
    rct_tile_element * guest_tile = nullptr;

    // Count number of walking guests
    FOR_ALL_GUESTS(sprite_index, guest)
    {
        if (guest->state == PEEP_STATE_WALKING)
        {
            // Check the walking guest's tile. Only count them if they're on a path tile.
            guest_tile = map_get_path_element_at(guest->next_x / 32, guest->next_y / 32, guest->next_z);
            if (guest_tile != nullptr)
                ++count;
        }
    }

    if (count > 0)
    {
        // Place staff at a random guest
        uint32_t rand = scenario_rand_max(count);
        FOR_ALL_GUESTS(sprite_index, guest)
        {
            if (guest->state == PEEP_STATE_WALKING)
            {
                guest_tile = map_get_path_element_at(guest->next_x / 32, guest->next_y / 32, guest->next_z);
                if (guest_tile != nullptr)
                {
                    if (rand == 0)
                        break;
                    --rand;
                }
            }
        }

        x = guest->x;
        y = guest->y;
        z = guest->z;
    }
    else
    {
        // No walking guests; pick random park entrance
        count = 0;
        uint8_t i;
        for (i = 0; i < MAX_PARK_ENTRANCES; ++i)
        {
            if (gParkEntrances[i].x != LOCATION_NULL)
                ++count;
        }

        if (count > 0)
        {
            uint32_t rand = scenario_rand_max(count);
            for (i = 0; i < MAX_PARK_ENTRANCES; ++i)
            {
                if (gParkEntrances[i].x != LOCATION_NULL)
                {
                    if (rand == 0)
                        break;
                    --rand;
                }
            }

            uint8_t dir = gParkEntrances[i].direction;
            x         = gParkEntrances[i].x;
            y         = gParkEntrances[i].y;
            z         = gParkEntrances[i].z;
            x += 16 + ((dir & 1) == 0 ? ((dir & 2) ? 32 : -32) : 0);
            y += 16 + ((dir & 1) == 1 ? ((dir & 2) ? -32 : 32) : 0);
        }
        else
        {
            // No more options; user must pick a location
            newPeep->state = PEEP_STATE_PICKED;
            x              = newPeep->x;
            y              = newPeep->y;
            z              = newPeep->z;
        }
    }

    sprite_move(x, y, z + 16, (rct_sprite *)newPeep);
    invalidate_sprite_2((rct_sprite *)newPeep);
}

static money32 staff_hire_new_staff_member(uint8_t staff_type, uint8_t flags, int16_t command_x, int16_t command_y, int16_t command_z,
                                           int32_t autoposition, int32_t * newPeep_sprite_index)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_WAGES;
    gCommandPosition.x      = command_x;
    gCommandPosition.y      = command_y;
    gCommandPosition.z      = command_z;

    if (gSpriteListCount[SPRITE_LIST_NULL] < 400)
    {
        gGameCommandErrorText = STR_TOO_MANY_PEOPLE_IN_GAME;
        return MONEY32_UNDEFINED;
    }

    // Staff type matches STAFF_TYPE enum, but ENTERTAINER onwards will match
    // the ENTERTAINER_COSTUME enum
    uint8_t entertainerType = ENTERTAINER_COSTUME_PANDA;
    if (staff_type >= STAFF_TYPE_ENTERTAINER)
    {
        entertainerType = staff_type - STAFF_TYPE_ENTERTAINER;
        if (entertainerType >= ENTERTAINER_COSTUME_COUNT)
        {
            // Invalid entertainer costume
            return MONEY32_UNDEFINED;
        }

        uint32_t availableCostumes = staff_get_available_entertainer_costumes();
        if (!(availableCostumes & (1 << entertainerType)))
        {
            // Entertainer costume unavailable
            return MONEY32_UNDEFINED;
        }

        staff_type = STAFF_TYPE_ENTERTAINER;
    }

    int32_t i;
    for (i = 0; i < STAFF_MAX_COUNT; ++i)
    {
        if (!(gStaffModes[i] & 1))
            break;
    }

    if (i == STAFF_MAX_COUNT)
    {
        gGameCommandErrorText = STR_TOO_MANY_STAFF_IN_GAME;
        return MONEY32_UNDEFINED;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        int32_t                    newStaffId = i;
        const rct_sprite_bounds * spriteBounds;
        rct_peep *                newPeep = &(create_sprite(flags)->peep);

        if (newPeep == nullptr)
        {
            gGameCommandErrorText = STR_TOO_MANY_PEOPLE_IN_GAME;
            return MONEY32_UNDEFINED;
        }

        if (flags == 0)
        {
            sprite_remove((rct_sprite *)newPeep);
        }
        else
        {
            move_sprite_to_list((rct_sprite *)newPeep, SPRITE_LIST_PEEP * 2);

            newPeep->sprite_identifier          = 1;
            newPeep->window_invalidate_flags    = 0;
            newPeep->action                     = PEEP_ACTION_NONE_2;
            newPeep->special_sprite             = 0;
            newPeep->action_sprite_image_offset = 0;
            newPeep->no_action_frame_num         = 0;
            newPeep->action_sprite_type         = 0;
            newPeep->path_check_optimisation                     = 0;
            newPeep->type                       = PEEP_TYPE_STAFF;
            newPeep->outside_of_park            = 0;
            newPeep->peep_flags                 = 0;
            newPeep->paid_to_enter              = 0;
            newPeep->paid_on_rides              = 0;
            newPeep->paid_on_food               = 0;
            newPeep->paid_on_souvenirs          = 0;

            if (staff_type == STAFF_TYPE_HANDYMAN)
                newPeep->staff_orders = STAFF_ORDERS_SWEEPING | STAFF_ORDERS_WATER_FLOWERS | STAFF_ORDERS_EMPTY_BINS;
            else if (staff_type == STAFF_TYPE_MECHANIC)
                newPeep->staff_orders = STAFF_ORDERS_INSPECT_RIDES | STAFF_ORDERS_FIX_RIDES;
            else
                newPeep->staff_orders = 0;

            uint16_t     idSearchSpriteIndex;
            rct_peep * idSearchPeep;

            // We search for the first available id for a given staff type
            uint32_t newStaffIndex = 0;
            for (;;)
            {
                bool found = false;
                ++newStaffIndex;

                FOR_ALL_STAFF(idSearchSpriteIndex, idSearchPeep)
                {
                    if (idSearchPeep->staff_type != staff_type)
                        continue;

                    if (idSearchPeep->id == newStaffIndex)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    break;
            }

            newPeep->id         = newStaffIndex;
            newPeep->staff_type = staff_type;

            static constexpr const rct_string_id staffNames[] = {
                STR_HANDYMAN_X,
                STR_MECHANIC_X,
                STR_SECURITY_GUARD_X,
                STR_ENTERTAINER_X,
            };

            /* rct2: 0x009929FC */
            static constexpr const uint8_t spriteTypes[] = {
                PEEP_SPRITE_TYPE_HANDYMAN,
                PEEP_SPRITE_TYPE_MECHANIC,
                PEEP_SPRITE_TYPE_SECURITY,
                PEEP_SPRITE_TYPE_ENTERTAINER_PANDA,
            };

            uint8_t sprite_type = spriteTypes[staff_type];
            if (staff_type == STAFF_TYPE_ENTERTAINER)
            {
                sprite_type = PEEP_SPRITE_TYPE_ENTERTAINER_PANDA + entertainerType;
            }
            newPeep->name_string_idx = staffNames[staff_type];
            newPeep->sprite_type     = sprite_type;

            spriteBounds                    = g_peep_animation_entries[sprite_type].sprite_bounds;
            newPeep->sprite_width           = spriteBounds->sprite_width;
            newPeep->sprite_height_negative = spriteBounds->sprite_height_negative;
            newPeep->sprite_height_positive = spriteBounds->sprite_height_positive;

            if (autoposition)
            {
                staff_autoposition_new_staff_member(newPeep);
            }
            else
            {
                newPeep->state = PEEP_STATE_PICKED;

                sprite_move(newPeep->x, newPeep->y, newPeep->z, (rct_sprite *)newPeep);
                invalidate_sprite_2((rct_sprite *)newPeep);
            }

            newPeep->time_in_park            = gDateMonthsElapsed;
            newPeep->pathfind_goal.x         = 0xFF;
            newPeep->pathfind_goal.y         = 0xFF;
            newPeep->pathfind_goal.z         = 0xFF;
            newPeep->pathfind_goal.direction = 0xFF;

            uint8_t colour             = staff_get_colour(staff_type);
            newPeep->tshirt_colour   = colour;
            newPeep->trousers_colour = colour;

            // Staff energy determines their walking speed
            newPeep->energy        = 0x60;
            newPeep->energy_target = 0x60;
            newPeep->staff_mowing_timeout        = 0;

            peep_update_name_sort(newPeep);

            newPeep->staff_id = newStaffId;

            gStaffModes[newStaffId] = STAFF_MODE_WALK;

            for (i = 0; i < STAFF_PATROL_AREA_SIZE; i++)
            {
                gStaffPatrolAreas[newStaffId * STAFF_PATROL_AREA_SIZE + i] = 0;
            }
        }

        *newPeep_sprite_index = newPeep->sprite_index;
    }
    return 0;
}

/**
 *
 *  rct2: 0x006BEFA1
 */
void game_command_hire_new_staff_member(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, [[maybe_unused]] int32_t* ebp)
{
    *ebx = staff_hire_new_staff_member((*ebx & 0xFF00) >> 8, *ebx & 0xFF, *eax & 0xFFFF, *ecx & 0xFFFF, *edx & 0xFFFF,
                                       (*ebx & 0xFF0000) >> 16, edi);
}

/** rct2: 0x00982134 */
static constexpr const bool peep_slow_walking_types[] = {
    false, // PEEP_SPRITE_TYPE_NORMAL
    false, // PEEP_SPRITE_TYPE_HANDYMAN
    false, // PEEP_SPRITE_TYPE_MECHANIC
    false, // PEEP_SPRITE_TYPE_SECURITY
    false, // PEEP_SPRITE_TYPE_ENTERTAINER_PANDA
    false, // PEEP_SPRITE_TYPE_ENTERTAINER_TIGER
    false, // PEEP_SPRITE_TYPE_ENTERTAINER_ELEPHANT
    false, // PEEP_SPRITE_TYPE_ENTERTAINER_ROMAN
    false, // PEEP_SPRITE_TYPE_ENTERTAINER_GORILLA
    false, // PEEP_SPRITE_TYPE_ENTERTAINER_SNOWMAN
    false, // PEEP_SPRITE_TYPE_ENTERTAINER_KNIGHT
    true,  // PEEP_SPRITE_TYPE_ENTERTAINER_ASTRONAUT
    false, // PEEP_SPRITE_TYPE_ENTERTAINER_BANDIT
    false, // PEEP_SPRITE_TYPE_ENTERTAINER_SHERIFF
    true,  // PEEP_SPRITE_TYPE_ENTERTAINER_PIRATE
    true,  // PEEP_SPRITE_TYPE_BALLOON
};

/**
 *
 *  rct2: 0x006C0BB5
 */
void game_command_set_staff_order(
    [[maybe_unused]] int32_t* eax,
    int32_t* ebx,
    [[maybe_unused]] int32_t* ecx,
    int32_t* edx,
    [[maybe_unused]] int32_t* esi,
    [[maybe_unused]] int32_t* edi,
    [[maybe_unused]] int32_t* ebp)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_WAGES;
    uint8_t  order_id         = *ebx >> 8;
    uint16_t sprite_id        = *edx;
    if (sprite_id >= MAX_SPRITES)
    {
        log_warning("Invalid game command, sprite_id = %u", sprite_id);
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    if (*ebx & GAME_COMMAND_FLAG_APPLY)
    {
        rct_peep * peep = &get_sprite(sprite_id)->peep;
        if (order_id & 0x80)
        { // change costume
            uint8_t sprite_type = order_id & ~0x80;
            sprite_type += 4;
            if (sprite_type >= Util::CountOf(peep_slow_walking_types))
            {
                log_error("Invalid change costume order for sprite_type %u", sprite_type);
                *ebx = MONEY32_UNDEFINED;
                return;
            }
            peep->sprite_type = sprite_type;
            peep->peep_flags &= ~PEEP_FLAGS_SLOW_WALK;
            if (peep_slow_walking_types[sprite_type])
            {
                peep->peep_flags |= PEEP_FLAGS_SLOW_WALK;
            }
            peep->action_frame = 0;
            peep->UpdateCurrentActionSpriteType();
            peep->Invalidate();
            window_invalidate_by_number(WC_PEEP, sprite_id);
            window_invalidate_by_class(WC_STAFF_LIST);
        }
        else
        {
            peep->staff_orders = order_id;
            window_invalidate_by_number(WC_PEEP, sprite_id);
            window_invalidate_by_class(WC_STAFF_LIST);
        }
    }
    *ebx = 0;
}

/**
 *
 *  rct2: 0x006C09D1
 */
void game_command_set_staff_patrol(
    int32_t* eax,
    int32_t* ebx,
    int32_t* ecx,
    int32_t* edx,
    [[maybe_unused]] int32_t* esi,
    [[maybe_unused]] int32_t* edi,
    [[maybe_unused]] int32_t* ebp)
{
    if (*ebx & GAME_COMMAND_FLAG_APPLY)
    {
        int32_t x         = *eax;
        int32_t y         = *ecx;
        uint16_t sprite_id = *edx;
        if (sprite_id >= MAX_SPRITES)
        {
            *ebx = MONEY32_UNDEFINED;
            log_warning("Invalid sprite id %u", sprite_id);
            return;
        }
        rct_sprite * sprite = get_sprite(sprite_id);
        if (sprite->unknown.sprite_identifier != SPRITE_IDENTIFIER_PEEP || sprite->peep.type != PEEP_TYPE_STAFF)
        {
            *ebx = MONEY32_UNDEFINED;
            log_warning("Invalid type of sprite %u for game command", sprite_id);
            return;
        }
        rct_peep * peep         = &sprite->peep;
        int32_t     patrolOffset = peep->staff_id * STAFF_PATROL_AREA_SIZE;

        staff_toggle_patrol_area(peep->staff_id, x, y);

        int32_t ispatrolling = 0;
        for (int32_t i = 0; i < 128; i++)
        {
            ispatrolling |= gStaffPatrolAreas[patrolOffset + i];
        }

        gStaffModes[peep->staff_id] &= ~2;
        if (ispatrolling)
        {
            gStaffModes[peep->staff_id] |= 2;
        }

        for (int32_t y2 = 0; y2 < 4; y2++)
        {
            for (int32_t x2 = 0; x2 < 4; x2++)
            {
                map_invalidate_tile_full((x & 0x1F80) + (x2 * 32), (y & 0x1F80) + (y2 * 32));
            }
        }
        staff_update_greyed_patrol_areas();
    }
    *ebx = 0;
}

/**
 *
 *  rct2: 0x006C0B83
 */
void game_command_fire_staff_member(
    [[maybe_unused]] int32_t* eax,
    int32_t* ebx,
    [[maybe_unused]] int32_t* ecx,
    int32_t* edx,
    [[maybe_unused]] int32_t* esi,
    [[maybe_unused]] int32_t* edi,
    [[maybe_unused]] int32_t* ebp)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_WAGES;
    if (*ebx & GAME_COMMAND_FLAG_APPLY)
    {
        window_close_by_class(WC_FIRE_PROMPT);
        uint16_t sprite_id = *edx;
        if (sprite_id >= MAX_SPRITES)
        {
            log_warning("Invalid game command, sprite_id = %u", sprite_id);
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        rct_peep * peep = &get_sprite(sprite_id)->peep;
        if (peep->sprite_identifier != SPRITE_IDENTIFIER_PEEP || peep->type != PEEP_TYPE_STAFF)
        {
            log_warning("Invalid game command, peep->sprite_identifier = %u, peep->type = %u", peep->sprite_identifier,
                        peep->type);
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        peep_sprite_remove(peep);
    }
    *ebx = 0;
}

/**
 * Hires a new staff member of the given type. If the hire cannot be completed (eg. the maximum number of staff is reached or
 * there are too many peeps) it returns SPRITE_INDEX_NULL.
 */
uint16_t hire_new_staff_member(uint8_t staffType)
{
    gGameCommandErrorTitle = STR_CANT_HIRE_NEW_STAFF;

    int32_t command_x, ebx, command_y, command_z, esi, new_sprite_index, ebp;
    command_y = command_z = esi = new_sprite_index = ebp = 0;
    command_x                                            = 0x8000;

    int32_t autoposition = gConfigGeneral.auto_staff_placement;
    if (gInputPlaceObjectModifier & PLACE_OBJECT_MODIFIER_SHIFT_Z)
    {
        autoposition = autoposition ^ 1;
    }

    ebx = autoposition << 16 | staffType << 8 | GAME_COMMAND_FLAG_APPLY;

    game_command_callback = game_command_callback_hire_new_staff_member;
    int32_t result = game_do_command_p(GAME_COMMAND_HIRE_NEW_STAFF_MEMBER, &command_x, &ebx, &command_y, &command_z, &esi,
                                      &new_sprite_index, &ebp);

    if (result == MONEY32_UNDEFINED)
        return SPRITE_INDEX_NULL;

    if ((staffType == STAFF_TYPE_HANDYMAN) && gConfigGeneral.handymen_mow_default)
    {
        rct_peep * newPeep    = GET_PEEP(new_sprite_index);
        uint8_t      new_orders = newPeep->staff_orders | STAFF_ORDERS_MOWING;
        game_do_command(newPeep->x, ((int32_t)new_orders << 8) | GAME_COMMAND_FLAG_APPLY, newPeep->y, new_sprite_index,
                        GAME_COMMAND_SET_STAFF_ORDER, 0, 0);
    }

    return new_sprite_index;
}

/**
 *
 *  rct2: 0x006C0C3F
 */
void staff_update_greyed_patrol_areas()
{
    rct_peep * peep;

    for (int32_t staff_type = 0; staff_type < STAFF_TYPE_COUNT; ++staff_type)
    {
        int32_t staffPatrolOffset = (staff_type + STAFF_MAX_COUNT) * STAFF_PATROL_AREA_SIZE;
        for (int32_t i = 0; i < STAFF_PATROL_AREA_SIZE; i++)
        {
            gStaffPatrolAreas[staffPatrolOffset + i] = 0;
        }

        for (uint16_t sprite_index = gSpriteListHead[SPRITE_LIST_PEEP]; sprite_index != SPRITE_INDEX_NULL;
             sprite_index        = peep->next)
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

static bool staff_is_location_in_patrol_area(rct_peep * peep, int32_t x, int32_t y)
{
    // Patrol quads are stored in a bit map (8 patrol quads per byte)
    // Each patrol quad is 4x4
    // Therefore there are in total 64 x 64 patrol quads in the 256 x 256 map
    return staff_is_patrol_area_set(peep->staff_id, x, y);
}

/**
 *
 *  rct2: 0x006C0905
 */
bool staff_is_location_in_patrol(rct_peep * staff, int32_t x, int32_t y)
{
    // Check if location is in the park
    if (!map_is_location_owned_or_has_rights(x, y))
        return false;

    // Check if staff has patrol area
    if (!(gStaffModes[staff->staff_id] & 2))
        return true;

    return staff_is_location_in_patrol_area(staff, x, y);
}

bool staff_is_location_on_patrol_edge(rct_peep * mechanic, int32_t x, int32_t y)
{
    // Check whether the location x,y is inside and on the edge of the
    // patrol zone for mechanic.
    bool   onZoneEdge   = false;
    int32_t neighbourDir = 0;
    while (!onZoneEdge && neighbourDir <= 7)
    {
        int32_t neighbourX = x + CoordsDirectionDelta[neighbourDir].x;
        int32_t neighbourY = y + CoordsDirectionDelta[neighbourDir].y;
        onZoneEdge        = !staff_is_location_in_patrol(mechanic, neighbourX, neighbourY);
        neighbourDir++;
    }
    return onZoneEdge;
}

bool staff_can_ignore_wide_flag(rct_peep * staff, int32_t x, int32_t y, uint8_t z, rct_tile_element * path)
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
    uint8_t total     = 0;
    uint8_t pathcount = 0;
    uint8_t widecount = 0;
    for (int32_t adjac_dir = 0; adjac_dir <= 3; adjac_dir++)
    {
        int32_t adjac_x = x + CoordsDirectionDelta[adjac_dir].x;
        int32_t adjac_y = y + CoordsDirectionDelta[adjac_dir].y;
        uint8_t  adjac_z = z;

        /* Ignore adjacent tiles outside the patrol zone. */
        if (!staff_is_location_in_patrol(staff, adjac_x, adjac_y))
            continue;

        /* Ignore adjacent tiles on the patrol zone edge. */
        if (staff_is_location_on_patrol_edge(staff, adjac_x, adjac_y))
            continue;

        /* Adjacent tile is inside the patrol zone but not on the
         * patrol zone edge. */
        total++;

        /* Check if path has an edge in adjac_dir */
        if (!(path->properties.path.edges & (1u << adjac_dir)))
        {
            continue;
        }

        if (footpath_element_is_sloped(path))
        {
            if (footpath_element_get_slope_direction(path) == adjac_dir)
            {
                adjac_z = z + 2;
            }
        }

        /* Search through all adjacent map elements */
        rct_tile_element * test_element = map_get_first_element_at(adjac_x / 32, adjac_y / 32);
        bool              pathfound    = false;
        bool              widefound    = false;
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

            if (footpath_element_is_wide(test_element))
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
static uint8_t staff_get_valid_patrol_directions(rct_peep * peep, int16_t x, int16_t y)
{
    uint8_t directions = 0;

    if (staff_is_location_in_patrol(peep, x - 32, y))
    {
        directions |= (1 << 0);
    }

    if (staff_is_location_in_patrol(peep, x, y + 32))
    {
        directions |= (1 << 1);
    }

    if (staff_is_location_in_patrol(peep, x + 32, y))
    {
        directions |= (1 << 2);
    }

    if (staff_is_location_in_patrol(peep, x, y - 32))
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
    uint16_t     spriteIndex;
    rct_peep * peep;

    FOR_ALL_STAFF(spriteIndex, peep)
    {
        peep->time_in_park          = gDateMonthsElapsed;
        peep->staff_lawns_mown      = 0;
        peep->staff_rides_fixed     = 0;
        peep->staff_gardens_watered = 0;
        peep->staff_rides_inspected = 0;
        peep->staff_litter_swept    = 0;
        peep->staff_bins_emptied    = 0;
    }
}

bool staff_is_patrol_area_set(int32_t staffIndex, int32_t x, int32_t y)
{
    x = (x & 0x1F80) >> 7;
    y = (y & 0x1F80) >> 1;

    int32_t peepOffset = staffIndex * STAFF_PATROL_AREA_SIZE;
    int32_t offset     = (x | y) >> 5;
    int32_t bitIndex   = (x | y) & 0x1F;
    return gStaffPatrolAreas[peepOffset + offset] & (((uint32_t)1) << bitIndex);
}

void staff_set_patrol_area(int32_t staffIndex, int32_t x, int32_t y, bool value)
{
    x = (x & 0x1F80) >> 7;
    y = (y & 0x1F80) >> 1;

    int32_t   peepOffset = staffIndex * STAFF_PATROL_AREA_SIZE;
    int32_t   offset     = (x | y) >> 5;
    int32_t   bitIndex   = (x | y) & 0x1F;
    uint32_t * addr       = &gStaffPatrolAreas[peepOffset + offset];
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
    int32_t offset     = (x | y) >> 5;
    int32_t bitIndex   = (x | y) & 0x1F;
    gStaffPatrolAreas[peepOffset + offset] ^= (1 << bitIndex);
}

/**
 *
 *  rct2: 0x006BFBE8
 *
 * Returns 0xFF when no nearby litter or unpathable litter
 */
static uint8_t staff_handyman_direction_to_nearest_litter(rct_peep * peep)
{
    uint16_t       nearestLitterDist = (uint16_t)-1;
    rct_litter * nearestLitter     = nullptr;
    rct_litter * litter            = nullptr;

    for (uint16_t litterIndex = gSpriteListHead[SPRITE_LIST_LITTER]; litterIndex != 0xFFFF; litterIndex = litter->next)
    {
        litter = &get_sprite(litterIndex)->litter;

        uint16_t distance = abs(litter->x - peep->x) + abs(litter->y - peep->y) + abs(litter->z - peep->z) * 4;

        if (distance < nearestLitterDist)
        {
            nearestLitterDist = distance;
            nearestLitter     = litter;
        }
    }

    if (nearestLitterDist > 0x60)
    {
        return 0xFF;
    }

    LocationXY16 litterTile = { static_cast<int16_t>(nearestLitter->x & 0xFFE0), static_cast<int16_t>(nearestLitter->y & 0xFFE0) };

    if (!staff_is_location_in_patrol(peep, litterTile.x, litterTile.y))
    {
        return 0xFF;
    }

    litterTile.x += 16;
    litterTile.y += 16;

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

    rct_tile_element * tileElement = map_get_first_element_at(nextTile.x / 32, nextTile.y / 32);

    do
    {
        if (tileElement->base_height != nextZ)
            continue;
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE ||
            tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
        {
            return 0xFF;
        }
    } while (!(tileElement++)->IsLastForTile());

    nextTile.x = (peep->x & 0xFFE0) + CoordsDirectionDelta[nextDirection].x;
    nextTile.y = (peep->y & 0xFFE0) + CoordsDirectionDelta[nextDirection].y;

    tileElement = map_get_first_element_at(nextTile.x / 32, nextTile.y / 32);

    do
    {
        if (tileElement->base_height != nextZ)
            continue;
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE ||
            tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK)
        {
            return 0xFF;
        }
    } while (!(tileElement++)->IsLastForTile());

    return nextDirection;
}


/**
 *
 *  rct2: 0x006BF931
 */
static uint8_t staff_handyman_direction_to_uncut_grass(rct_peep * peep, uint8_t valid_directions)
{
    if (!(peep->GetNextIsSurface()))
    {

        rct_tile_element * tileElement = map_get_surface_element_at({peep->next_x, peep->next_y});

        if (peep->next_z != tileElement->base_height)
            return 0xFF;

        if (peep->GetNextIsSloped())
        {
            if ((tileElement->properties.surface.slope & TILE_ELEMENT_SURFACE_SLOPE_MASK) != byte_98D800[peep->GetNextDirection()])
                return 0xFF;
        }
        else if ((tileElement->properties.surface.slope & TILE_ELEMENT_SURFACE_SLOPE_MASK) != 0)
            return 0xFF;
    }

    uint8_t chosenDirection = scenario_rand() & 0x3;
    for (uint8_t i = 0; i < 4; ++i, ++chosenDirection)
    {
        chosenDirection &= 0x3;

        if (!(valid_directions & (1 << chosenDirection)))
        {
            continue;
        }

        CoordsXY chosenTile = { static_cast<int32_t>(peep->next_x + CoordsDirectionDelta[chosenDirection].x),
                                static_cast<int32_t>(peep->next_y + CoordsDirectionDelta[chosenDirection].y) };

        if (chosenTile.x > 0x1FFF || chosenTile.y > 0x1FFF)
            continue;

        rct_tile_element * tileElement = map_get_surface_element_at(chosenTile);

        if (surface_get_terrain(tileElement) != 0)
            continue;

        if (abs(tileElement->base_height - peep->next_z) > 2)
            continue;

        if ((tileElement->properties.surface.grass_length & 0x7) < GRASS_LENGTH_CLEAR_1)
            continue;

        return chosenDirection;
    }
    return 0xFF;
}

/**
 *
 *  rct2: 0x006BFD9C
 */
static int32_t staff_handyman_direction_rand_surface(rct_peep * peep, uint8_t validDirections)
{
    uint8_t direction = scenario_rand() & 3;
    for (int32_t i = 0; i < 4; ++i, ++direction)
    {
        direction &= 3;
        if (!(validDirections & (1 << direction)))
            continue;

        LocationXY16 chosenTile = { static_cast<int16_t>(peep->next_x + CoordsDirectionDelta[direction].x),
                                static_cast<int16_t>(peep->next_y + CoordsDirectionDelta[direction].y) };

        if (map_surface_is_blocked(chosenTile.x, chosenTile.y))
            continue;

        break;
    }
    // If it tries all directions this is required
    // to make it back to the first direction and
    // override validDirections
    direction &= 3;
    return direction;
}

/**
 *
 *  rct2: 0x006BFBA8
 */
static bool staff_path_finding_handyman(rct_peep * peep)
{
    peep->staff_mowing_timeout++;

    uint8_t litterDirection = 0xFF;
    uint8_t validDirections = staff_get_valid_patrol_directions(peep, peep->next_x, peep->next_y);

    if ((peep->staff_orders & STAFF_ORDERS_SWEEPING) && ((gCurrentTicks + peep->sprite_index) & 0xFFF) > 110)
    {
        litterDirection = staff_handyman_direction_to_nearest_litter(peep);
    }

    uint8_t direction = 0xFF;
    if (litterDirection == 0xFF && (peep->staff_orders & STAFF_ORDERS_MOWING) && peep->staff_mowing_timeout >= 12)
    {
        direction = staff_handyman_direction_to_uncut_grass(peep, validDirections);
    }

    if (direction == 0xFF)
    {
        if (peep->GetNextIsSurface())
        {
            direction = staff_handyman_direction_rand_surface(peep, validDirections);
        }
        else
        {
            rct_tile_element * tileElement = map_get_path_element_at(peep->next_x / 32, peep->next_y / 32, peep->next_z);

            if (tileElement == nullptr)
                return true;

            uint8_t pathDirections = (tileElement->properties.path.edges & validDirections) & 0xF;
            if (pathDirections == 0)
            {
                direction = staff_handyman_direction_rand_surface(peep, validDirections);
            }
            else
            {
                bool chooseRandom = true;
                if (litterDirection != 0xFF && pathDirections & (1 << litterDirection))
                {
                    if ((scenario_rand() & 0xFFFF) >= 0x1999)
                    {
                        chooseRandom = false;
                        direction    = litterDirection;
                    }
                }
                else
                {
                    pathDirections &= ~(1 << (peep->direction ^ (1 << 1)));
                    if (pathDirections == 0)
                    {
                        pathDirections |= 1 << (peep->direction ^ (1 << 1));
                    }
                }

                if (chooseRandom == true)
                {
                    do
                    {
                        direction = scenario_rand() & 3;
                    } while ((pathDirections & (1 << direction)) == 0);
                }
            }
        }
    }

    // countof(CoordsDirectionDelta)
    assert(direction < 8);

    LocationXY16 chosenTile = { static_cast<int16_t>(peep->next_x + CoordsDirectionDelta[direction].x),
                            static_cast<int16_t>(peep->next_y + CoordsDirectionDelta[direction].y) };

    while (chosenTile.x > 0x1FFF || chosenTile.y > 0x1FFF)
    {
        direction    = staff_handyman_direction_rand_surface(peep, validDirections);
        chosenTile.x = peep->next_x + CoordsDirectionDelta[direction].x;
        chosenTile.y = peep->next_y + CoordsDirectionDelta[direction].y;
    }

    peep->direction             = direction;
    peep->destination_x         = chosenTile.x + 16;
    peep->destination_y         = chosenTile.y + 16;
    peep->destination_tolerance = 3;
    if (peep->state == PEEP_STATE_QUEUING)
    {
        peep->destination_tolerance = (scenario_rand() & 7) + 2;
    }
    return false;
}

static uint8_t staff_direction_surface(rct_peep * peep, uint8_t initialDirection)
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

        if (fence_in_the_way(peep->next_x, peep->next_y, peep->next_z, peep->next_z + 4, direction) == true)
            continue;

        if (fence_in_the_way(peep->next_x, peep->next_y, peep->next_z, peep->next_z + 4, direction ^ (1 << 1)) == true)
            continue;

        LocationXY16 chosenTile = { static_cast<int16_t>(peep->next_x + CoordsDirectionDelta[direction].x),
                                static_cast<int16_t>(peep->next_y + CoordsDirectionDelta[direction].y) };

        if (map_surface_is_blocked(chosenTile.x, chosenTile.y) == false)
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
static uint8_t staff_mechanic_direction_surface(rct_peep * peep)
{
    uint8_t direction = scenario_rand() & 3;

    if ((peep->state == PEEP_STATE_ANSWERING || peep->state == PEEP_STATE_HEADING_TO_INSPECTION) && scenario_rand() & 1)
    {
        TileCoordsXYZD location = ride_get_exit_location(peep->current_ride, peep->current_ride_station);
        if (location.isNull())
        {
            location = ride_get_entrance_location(peep->current_ride, peep->current_ride_station);
        }

        LocationXY16 chosenTile = { static_cast<int16_t>(location.x * 32), static_cast<int16_t>(location.y * 32) };

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
static uint8_t staff_mechanic_direction_path_rand(rct_peep * peep, uint8_t pathDirections)
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
static uint8_t staff_mechanic_direction_path(rct_peep * peep, uint8_t validDirections, rct_tile_element * pathElement)
{

    uint8_t direction      = 0xFF;
    uint8_t pathDirections = pathElement->properties.path.edges & 0xF;
    pathDirections &= validDirections;

    if (pathDirections == 0)
    {
        return staff_mechanic_direction_surface(peep);
    }

    // Check if this is dead end - i.e. only way out is the reverse direction.
    pathDirections &= ~(1 << (peep->direction ^ (1 << 1)));
    if (pathDirections == 0)
    {
        pathDirections |= (1 << (peep->direction ^ (1 << 1)));
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
    if (peep->state == PEEP_STATE_ANSWERING || peep->state == PEEP_STATE_HEADING_TO_INSPECTION)
    {
        /* Find location of the exit for the target ride station
         * or if the ride has no exit, the entrance. */
        TileCoordsXYZD location = ride_get_exit_location(peep->current_ride, peep->current_ride_station);
        if (location.isNull())
        {
            location = ride_get_entrance_location(peep->current_ride, peep->current_ride_station);

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
        gPeepPathFindQueueRideIndex      = 255;

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        pathfind_logging_enable(peep);
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

        int32_t pathfindDirection = peep_pathfind_choose_direction({ peep->next_x / 32, peep->next_y / 32, peep->next_z }, peep);

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
        pathfind_logging_disable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

        if (pathfindDirection == -1)
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
static bool staff_path_finding_mechanic(rct_peep * peep)
{
    uint8_t validDirections = staff_get_valid_patrol_directions(peep, peep->next_x, peep->next_y);
    uint8_t direction       = 0xFF;
    if (peep->GetNextIsSurface())
    {
        direction = staff_mechanic_direction_surface(peep);
    }
    else
    {
        rct_tile_element * pathElement = map_get_path_element_at(peep->next_x / 32, peep->next_y / 32, peep->next_z);
        if (pathElement == nullptr)
            return true;

        direction = staff_mechanic_direction_path(peep, validDirections, pathElement);
    }

    // countof(CoordsDirectionDelta)
    assert(direction < 8);

    LocationXY16 chosenTile = { static_cast<int16_t>(peep->next_x + CoordsDirectionDelta[direction].x),
                            static_cast<int16_t>(peep->next_y + CoordsDirectionDelta[direction].y) };

    while (chosenTile.x > 0x1FFF || chosenTile.y > 0x1FFF)
    {
        direction    = staff_mechanic_direction_surface(peep);
        chosenTile.x = peep->next_x + CoordsDirectionDelta[direction].x;
        chosenTile.y = peep->next_y + CoordsDirectionDelta[direction].y;
    }

    peep->direction             = direction;
    peep->destination_x         = chosenTile.x + 16;
    peep->destination_y         = chosenTile.y + 16;
    peep->destination_tolerance = (scenario_rand() & 7) + 2;

    return false;
}

/**
 *
 *  rct2: 0x006C050B
 */
static uint8_t staff_direction_path(rct_peep * peep, uint8_t validDirections, rct_tile_element * pathElement)
{
    uint8_t direction      = 0xFF;
    uint8_t pathDirections = pathElement->properties.path.edges & 0xF;
    if (peep->state != PEEP_STATE_ANSWERING && peep->state != PEEP_STATE_HEADING_TO_INSPECTION)
    {
        pathDirections &= validDirections;
    }

    if (pathDirections == 0)
    {
        return staff_direction_surface(peep, scenario_rand() & 3);
    }

    pathDirections &= ~(1 << (peep->direction ^ (1 << 1)));
    if (pathDirections == 0)
    {
        pathDirections |= (1 << (peep->direction ^ (1 << 1)));
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
static bool staff_path_finding_misc(rct_peep * peep)
{
    uint8_t validDirections = staff_get_valid_patrol_directions(peep, peep->next_x, peep->next_y);

    uint8_t direction = 0xFF;
    if (peep->GetNextIsSurface())
    {
        direction = staff_direction_surface(peep, scenario_rand() & 3);
    }
    else
    {
        rct_tile_element * pathElement = map_get_path_element_at(peep->next_x / 32, peep->next_y / 32, peep->next_z);
        if (pathElement == nullptr)
            return true;

        direction = staff_direction_path(peep, validDirections, pathElement);
    }

    LocationXY16 chosenTile = { static_cast<int16_t>(peep->next_x + CoordsDirectionDelta[direction].x),
                            static_cast<int16_t>(peep->next_y + CoordsDirectionDelta[direction].y) };

    while (chosenTile.x > 0x1FFF || chosenTile.y > 0x1FFF)
    {
        direction    = staff_direction_surface(peep, scenario_rand() & 3);
        chosenTile.x = peep->next_x + CoordsDirectionDelta[direction].x;
        chosenTile.y = peep->next_y + CoordsDirectionDelta[direction].y;
    }

    peep->direction             = direction;
    peep->destination_x         = chosenTile.x + 16;
    peep->destination_y         = chosenTile.y + 16;
    peep->destination_tolerance = (scenario_rand() & 7) + 2;

    return false;
}

/**
 *
 *  rct2: 0x006C086D
 */
static void staff_entertainer_update_nearby_peeps(rct_peep * peep)
{
    uint16_t     spriteIndex;
    rct_peep * guest;

    FOR_ALL_GUESTS(spriteIndex, guest)
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
static int32_t staff_path_finding_entertainer(rct_peep * peep)
{

    if (((scenario_rand() & 0xFFFF) <= 0x4000) && (peep->action == PEEP_ACTION_NONE_1 || peep->action == PEEP_ACTION_NONE_2))
    {

        peep->Invalidate();

        peep->action                     = (scenario_rand() & 1) ? PEEP_ACTION_WAVE_2 : PEEP_ACTION_JOY;
        peep->action_frame               = 0;
        peep->action_sprite_image_offset = 0;

        peep->UpdateCurrentActionSpriteType();
        peep->Invalidate();
        staff_entertainer_update_nearby_peeps(peep);
    }

    return staff_path_finding_misc(peep);
}

/**
 *
 *  rct2: 0x006BF926
 */
int32_t staff_path_finding(rct_peep * peep)
{
    switch (peep->staff_type)
    {
    case STAFF_TYPE_HANDYMAN:
        return staff_path_finding_handyman(peep);
    case STAFF_TYPE_MECHANIC:
        return staff_path_finding_mechanic(peep);
    case STAFF_TYPE_SECURITY:
        return staff_path_finding_misc(peep);
    case STAFF_TYPE_ENTERTAINER:
        return staff_path_finding_entertainer(peep);

    default:
        assert(false);
        return 0;
    }
}

void game_command_pickup_staff(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, int32_t* ebp)
{
    int32_t peepnum = *eax;
    int32_t x       = *edi;
    int32_t y       = *ebp;
    int32_t z       = *edx;
    int32_t action  = *ecx;
    if (peep_pickup_command(peepnum, x, y, z, action, *ebx & GAME_COMMAND_FLAG_APPLY))
    {
        *ebx = 0;
    }
    else
    {
        *ebx = MONEY32_UNDEFINED;
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
    entertainerCostumes |= (1 << ENTERTAINER_COSTUME_PANDA) | (1 << ENTERTAINER_COSTUME_TIGER) | (1 << ENTERTAINER_COSTUME_ELEPHANT);

    return entertainerCostumes;
}

int32_t staff_get_available_entertainer_costume_list(uint8_t * costumeList)
{
    uint32_t availableCostumes = staff_get_available_entertainer_costumes();
    int32_t numCostumes       = 0;
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
static constexpr const LocationXY16 _MowingWaypoints[] = {
    { 28, 28 }, { 28, 4 }, { 20, 4 }, { 20, 28 }, { 12, 28 }, { 12, 4 }, { 4, 4 }, { 4, 28 },
};

/**
 *
 *  rct2: 0x006BF567
 */
void rct_peep::UpdateMowing()
{
    if (!CheckForPath())
        return;

    Invalidate();
    while (true)
    {
        int16_t actionX = 0;
        int16_t actionY = 0;
        int16_t xy_distance;
        if (UpdateAction(&actionX, &actionY, &xy_distance))
        {
            int16_t checkZ = tile_element_height(actionX, actionY) & 0xFFFF;
            MoveTo(actionX, actionY, checkZ);
            Invalidate();
            return;
        }

        var_37++;

        if (var_37 == 1)
        {
            SwitchToSpecialSprite(2);
        }

        if (var_37 == Util::CountOf(_MowingWaypoints))
        {
            StateReset();
            return;
        }

        destination_x = _MowingWaypoints[var_37].x + next_x;
        destination_y = _MowingWaypoints[var_37].y + next_y;

        if (var_37 != 7)
            continue;

        rct_tile_element * tile_element = map_get_first_element_at(next_x / 32, next_y / 32);

        for (; (tile_element->GetType() != TILE_ELEMENT_TYPE_SURFACE); tile_element++)
            ;

        if ((tile_element->properties.surface.terrain & TILE_ELEMENT_SURFACE_TERRAIN_MASK) == (TERRAIN_GRASS << 5))
        {
            tile_element->properties.surface.grass_length = GRASS_LENGTH_MOWED;
            map_invalidate_tile_zoom0(next_x, next_y, tile_element->base_height * 8,
                                      tile_element->base_height * 8 + 16);
        }
        staff_lawns_mown++;
        window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
    }
}

/**
 *
 *  rct2: 0x006BF7E6
 */
void rct_peep::UpdateWatering()
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

        sprite_direction           = (var_37 & 3) << 3;
        action                     = PEEP_ACTION_STAFF_WATERING;
        action_frame               = 0;
        action_sprite_image_offset = 0;
        UpdateCurrentActionSpriteType();
        Invalidate();

        sub_state = 1;
    }
    else if (sub_state == 1)
    {
        if (action != PEEP_ACTION_NONE_2)
        {
            int16_t actionX, actionY, xy_distance;
            UpdateAction(&actionX, &actionY, &xy_distance);
            return;
        }

        int32_t actionX = next_x + CoordsDirectionDelta[var_37].x;
        int32_t actionY = next_y + CoordsDirectionDelta[var_37].y;

        rct_tile_element * tile_element = map_get_first_element_at(actionX / 32, actionY / 32);

        do
        {
            if (tile_element->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
                continue;

            if (abs(((int32_t)next_z) - tile_element->base_height) > 4)
                continue;

            rct_scenery_entry * scenery_entry = get_small_scenery_entry(tile_element->properties.scenery.type);

            if (!scenery_small_entry_has_flag(scenery_entry, SMALL_SCENERY_FLAG_CAN_BE_WATERED))
                continue;

            tile_element->properties.scenery.age = 0;
            map_invalidate_tile_zoom0(actionX, actionY, tile_element->base_height * 8, tile_element->clearance_height * 8);
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
void rct_peep::UpdateEmptyingBin()
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

        sprite_direction           = (var_37 & 3) << 3;
        action                     = PEEP_ACTION_STAFF_EMPTY_BIN;
        action_frame               = 0;
        action_sprite_image_offset = 0;
        UpdateCurrentActionSpriteType();
        Invalidate();

        sub_state = 1;
    }
    else if (sub_state == 1)
    {

        if (action == PEEP_ACTION_NONE_2)
        {
            StateReset();
            return;
        }

        int16_t actionX = 0;
        int16_t actionY = 0;
        int16_t xy_distance;
        UpdateAction(&actionX, &actionY, &xy_distance);

        if (action_frame != 11)
            return;

        rct_tile_element * tile_element = map_get_first_element_at(next_x / 32, next_y / 32);

        for (;; tile_element++)
        {
            if (tile_element->GetType() == TILE_ELEMENT_TYPE_PATH)
            {
                if (next_z == tile_element->base_height)
                    break;
            }
            if ((tile_element)->IsLastForTile())
            {
                StateReset();
                return;
            }
        }

        if (!footpath_element_has_path_scenery(tile_element))
        {
            StateReset();
            return;
        }

        rct_scenery_entry * scenery_entry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tile_element));
        if (!(scenery_entry->path_bit.flags & PATH_BIT_FLAG_IS_BIN) || tile_element->flags & (1 << 5) ||
            footpath_element_path_scenery_is_ghost(tile_element))
        {
            StateReset();
            return;
        }

        tile_element->properties.path.addition_status |= ((3 << var_37) << var_37);

        map_invalidate_tile_zoom0(next_x, next_y, tile_element->base_height * 8, tile_element->clearance_height * 8);

        staff_bins_emptied++;
        window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
    }
}

/**
 *
 *  rct2: 0x6BF641
 */
void rct_peep::UpdateSweeping()
{
    staff_mowing_timeout = 0;
    if (!CheckForPath())
        return;

    Invalidate();

    if (action == PEEP_ACTION_STAFF_SWEEP && action_frame == 8)
    {
        // Remove sick at this location
        litter_remove_at(x, y, z);
        staff_litter_swept++;
        window_invalidate_flags |= PEEP_INVALIDATE_STAFF_STATS;
    }
    int16_t actionX = 0;
    int16_t actionY = 0;
    int16_t xy_distance;
    if (UpdateAction(&actionX, &actionY, &xy_distance))
    {
        int16_t actionZ = GetZOnSlope(actionX, actionY);
        MoveTo(actionX, actionY, actionZ);
        Invalidate();
        return;
    }

    var_37++;
    if (var_37 != 2)
    {
        action                     = PEEP_ACTION_STAFF_SWEEP;
        action_frame               = 0;
        action_sprite_image_offset = 0;
        UpdateCurrentActionSpriteType();
        Invalidate();
        return;
    }
    StateReset();
}

/**
 *
 *  rct2: 0x006C16D7
 */
void rct_peep::UpdateHeadingToInspect()
{
    Ride * ride = get_ride(current_ride);

    if (ride->type == RIDE_TYPE_NULL)
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
        rct_tile_element * rideEntranceExitElement;
        PerformNextAction(pathingResult, rideEntranceExitElement);

        if (!(pathingResult & PATHING_RIDE_EXIT) && !(pathingResult & PATHING_RIDE_ENTRANCE))
        {
            return;
        }

        if (current_ride != rideEntranceExitElement->properties.entrance.ride_index)
            return;

        uint8_t exit_index = ((rideEntranceExitElement->properties.entrance.index & 0x70) >> 4);

        if (current_ride_station != exit_index)
            return;

        if (pathingResult & PATHING_RIDE_ENTRANCE)
        {
            if (!ride_get_exit_location(ride, exit_index).isNull())
            {
                return;
            }
        }

        direction = tile_element_get_direction(rideEntranceExitElement);

        int32_t destX = next_x + 16 + word_981D6C[direction].x * 53;
        int32_t destY = next_y + 16 + word_981D6C[direction].y * 53;

        destination_x         = destX;
        destination_y         = destY;
        destination_tolerance = 2;
        sprite_direction      = direction << 3;

        z         = rideEntranceExitElement->base_height * 4;
        sub_state = 4;
        // Falls through into sub_state 4
    }

    Invalidate();

    int16_t delta_y = abs(y - destination_y);

    int16_t actionX, actionY, xy_distance;
    if (!UpdateAction(&actionX, &actionY, &xy_distance))
    {
        SetState(PEEP_STATE_INSPECTING);
        sub_state = 0;
        return;
    }

    int32_t newZ = ride->station_heights[current_ride_station] * 8;

    if (delta_y < 20)
    {
        newZ += RideData5[ride->type].z;
    }

    MoveTo(actionX, actionY, newZ);
    Invalidate();
}

/**
 *
 *  rct2: 0x006C0CB8
 */
void rct_peep::UpdateAnswering()
{
    Ride * ride = get_ride(current_ride);

    if (ride->type == RIDE_TYPE_NULL || ride->mechanic_status != RIDE_MECHANIC_STATUS_HEADING)
    {

        SetState(PEEP_STATE_FALLING);
        return;
    }

    if (sub_state == 0)
    {
        action                     = PEEP_ACTION_STAFF_ANSWER_CALL;
        action_frame               = 0;
        action_sprite_image_offset = 0;

        UpdateCurrentActionSpriteType();
        Invalidate();

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
        int16_t actionX, actionY, xy_distance;
        UpdateAction(&actionX, &actionY, &xy_distance);
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
        rct_tile_element * rideEntranceExitElement;
        PerformNextAction(pathingResult, rideEntranceExitElement);

        if (!(pathingResult & PATHING_RIDE_EXIT) && !(pathingResult & PATHING_RIDE_ENTRANCE))
        {
            return;
        }

        if (current_ride != rideEntranceExitElement->properties.entrance.ride_index)
            return;

        uint8_t exit_index = ((rideEntranceExitElement->properties.entrance.index & 0x70) >> 4);

        if (current_ride_station != exit_index)
            return;

        if (pathingResult & PATHING_RIDE_ENTRANCE)
        {
            if (!ride_get_exit_location(ride, exit_index).isNull())
            {
                return;
            }
        }

        direction = tile_element_get_direction(rideEntranceExitElement);

        int32_t destX = next_x + 16 + word_981D6C[direction].x * 53;
        int32_t destY = next_y + 16 + word_981D6C[direction].y * 53;

        destination_x         = destX;
        destination_y         = destY;
        destination_tolerance = 2;
        sprite_direction      = direction << 3;

        z         = rideEntranceExitElement->base_height * 4;
        sub_state = 4;
        // Falls through into sub_state 4
    }

    Invalidate();

    int16_t delta_y = abs(y - destination_y);

    int16_t actionX, actionY, xy_distance;
    if (!UpdateAction(&actionX, &actionY, &xy_distance))
    {
        SetState(PEEP_STATE_FIXING);
        sub_state = 0;
        return;
    }

    int32_t newZ = ride->station_heights[current_ride_station] * 8;

    if (delta_y < 20)
    {
        newZ += RideData5[ride->type].z;
    }

    MoveTo(actionX, actionY, newZ);
    Invalidate();
}

/** rct2: 0x00992A5C */
static constexpr const LocationXY16 _WateringUseOffsets[] = {
    { 3, 16 }, { 16, 29 }, { 29, 16 }, { 16, 3 }, { 3, 29 }, { 29, 29 }, { 29, 3 }, { 3, 3 },
};

/**
 *
 *  rct2: 0x006BF483
 */
static int32_t peep_update_patrolling_find_watering(rct_peep * peep)
{
    if (!(peep->staff_orders & STAFF_ORDERS_WATER_FLOWERS))
        return 0;

    uint8_t chosen_position = scenario_rand() & 7;
    for (int32_t i = 0; i < 8; ++i, ++chosen_position)
    {
        chosen_position &= 7;

        int32_t x = peep->next_x + CoordsDirectionDelta[chosen_position].x;
        int32_t y = peep->next_y + CoordsDirectionDelta[chosen_position].y;

        rct_tile_element * tile_element = map_get_first_element_at(x / 32, y / 32);

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

            uint8_t z_diff = abs(peep->next_z - tile_element->base_height);

            if (z_diff >= 4)
            {
                continue;
            }

            rct_scenery_entry * sceneryEntry = get_small_scenery_entry(tile_element->properties.scenery.type);

            if (sceneryEntry == nullptr || !scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_CAN_BE_WATERED))
            {
                continue;
            }

            if (tile_element->properties.scenery.age < SCENERY_WITHER_AGE_THRESHOLD_2)
            {
                if (chosen_position >= 4)
                {
                    continue;
                }

                if (tile_element->properties.scenery.age < SCENERY_WITHER_AGE_THRESHOLD_1)
                {
                    continue;
                }
            }

            peep->SetState(PEEP_STATE_WATERING);
            peep->var_37 = chosen_position;

            peep->sub_state             = 0;
            peep->destination_x         = (peep->x & 0xFFE0) + _WateringUseOffsets[chosen_position].x;
            peep->destination_y         = (peep->y & 0xFFE0) + _WateringUseOffsets[chosen_position].y;
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
static int32_t peep_update_patrolling_find_bin(rct_peep * peep)
{
    if (!(peep->staff_orders & STAFF_ORDERS_EMPTY_BINS))
        return 0;

    if (peep->GetNextIsSurface())
        return 0;

    rct_tile_element * tileElement = map_get_first_element_at(peep->next_x / 32, peep->next_y / 32);
    if (tileElement == nullptr)
        return 0;

    for (;; tileElement++)
    {

        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH && (tileElement->base_height == peep->next_z))
            break;

        if (tileElement->IsLastForTile())
            return 0;
    }

    if (!footpath_element_has_path_scenery(tileElement))
        return 0;
    rct_scenery_entry * sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tileElement));

    if (!(sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BIN))
        return 0;

    if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
        return 0;

    if (footpath_element_path_scenery_is_ghost(tileElement))
        return 0;

    uint8_t bin_positions   = tileElement->properties.path.edges & 0xF;
    uint8_t bin_quantity    = tileElement->properties.path.addition_status;
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

    peep->sub_state             = 0;
    peep->destination_x         = (peep->x & 0xFFE0) + BinUseOffsets[chosen_position].x;
    peep->destination_y         = (peep->y & 0xFFE0) + BinUseOffsets[chosen_position].y;
    peep->destination_tolerance = 3;
    return 1;
}

/**
 *
 *  rct2: 0x006BF322
 */
static int32_t peep_update_patrolling_find_grass(rct_peep * peep)
{
    if (!(peep->staff_orders & STAFF_ORDERS_MOWING))
        return 0;

    if (peep->staff_mowing_timeout < 12)
        return 0;

    if (!(peep->GetNextIsSurface()))
        return 0;

    rct_tile_element * tile_element = map_get_surface_element_at({peep->next_x, peep->next_y});

    if ((tile_element->properties.surface.terrain & TILE_ELEMENT_SURFACE_TERRAIN_MASK) != TERRAIN_GRASS)
        return 0;

    if ((tile_element->properties.surface.grass_length & 0x7) < GRASS_LENGTH_CLEAR_1)
        return 0;

    peep->SetState(PEEP_STATE_MOWING);
    peep->var_37 = 0;
    // Original code used .y for both x and y. Changed to .x to make more sense (both x and y are 28)
    peep->destination_x         = peep->next_x + _MowingWaypoints[0].x;
    peep->destination_y         = peep->next_y + _MowingWaypoints[0].y;
    peep->destination_tolerance = 3;
    return 1;
}

/**
 *
 *  rct2: 0x006BF295
 */
static int32_t peep_update_patrolling_find_sweeping(rct_peep * peep)
{
    if (!(peep->staff_orders & STAFF_ORDERS_SWEEPING))
        return 0;

    uint16_t sprite_id = sprite_get_first_in_quadrant(peep->x, peep->y);

    for (rct_sprite * sprite = nullptr; sprite_id != SPRITE_INDEX_NULL; sprite_id = sprite->unknown.next_in_quadrant)
    {

        sprite = get_sprite(sprite_id);

        if (sprite->unknown.linked_list_type_offset != SPRITE_LIST_LITTER * 2)
            continue;

        uint16_t z_diff = abs(peep->z - sprite->litter.z);

        if (z_diff >= 16)
            continue;

        peep->SetState(PEEP_STATE_SWEEPING);
        peep->var_37                = 0;
        peep->destination_x         = sprite->litter.x;
        peep->destination_y         = sprite->litter.y;
        peep->destination_tolerance = 5;
        return 1;
    }

    return 0;
}

void rct_peep::Tick128UpdateStaff()
{
    if (staff_type != STAFF_TYPE_SECURITY)
        return;

    uint8_t newSpriteType = PEEP_SPRITE_TYPE_SECURITY_ALT;
    if (state != PEEP_STATE_PATROLLING)
        newSpriteType = PEEP_SPRITE_TYPE_SECURITY;

    if (sprite_type == newSpriteType)
        return;

    sprite_type                = newSpriteType;
    action_sprite_image_offset = 0;
    no_action_frame_num        = 0;
    if (action < PEEP_ACTION_NONE_1)
        action = PEEP_ACTION_NONE_2;

    peep_flags &= ~PEEP_FLAGS_SLOW_WALK;
    if (gSpriteTypeToSlowWalkMap[newSpriteType])
    {
        peep_flags |= PEEP_FLAGS_SLOW_WALK;
    }

    action_sprite_type = 0xFF;
    UpdateCurrentActionSpriteType();
}

bool rct_peep::IsMechanic() const
{
    return (sprite_identifier == SPRITE_IDENTIFIER_PEEP && type == PEEP_TYPE_STAFF && staff_type == STAFF_TYPE_MECHANIC);
}

/**
 *
 *  rct2: 0x006BF1FD
 */
void rct_peep::UpdatePatrolling()
{
    if (!CheckForPath())
        return;

    uint8_t pathingResult;
    PerformNextAction(pathingResult);
    if (!(pathingResult & PATHING_DESTINATION_REACHED))
        return;

    if (GetNextIsSurface())
    {
        rct_tile_element * tile_element = map_get_surface_element_at({next_x, next_y});

        if (tile_element != nullptr)
        {
            int32_t water_height = surface_get_water_height(tile_element);
            if (water_height)
            {
                Invalidate();
                water_height *= 16;
                MoveTo(x, y, water_height);
                Invalidate();

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
    PEEP_FIXING_ENTER_STATION                 = 0,
    PEEP_FIXING_MOVE_TO_BROKEN_DOWN_VEHICLE   = 1,
    PEEP_FIXING_FIX_VEHICLE_CLOSED_RESTRAINTS = 2,
    PEEP_FIXING_FIX_VEHICLE_CLOSED_DOORS      = 3,
    PEEP_FIXING_FIX_VEHICLE_OPEN_RESTRAINTS   = 4,
    PEEP_FIXING_FIX_VEHICLE_OPEN_DOORS        = 5,
    PEEP_FIXING_FIX_VEHICLE_MALFUNCTION       = 6,
    PEEP_FIXING_MOVE_TO_STATION_END           = 7,
    PEEP_FIXING_FIX_STATION_END               = 8,
    PEEP_FIXING_MOVE_TO_STATION_START         = 9,
    PEEP_FIXING_FIX_STATION_START             = 10,
    PEEP_FIXING_FIX_STATION_BRAKES            = 11,
    PEEP_FIXING_MOVE_TO_STATION_EXIT          = 12,
    PEEP_FIXING_FINISH_FIX_OR_INSPECT         = 13,
    PEEP_FIXING_LEAVE_BY_ENTRANCE_EXIT        = 14,
};

/**
 * FixingSubstatesForBreakdown[] defines the applicable peep sub_states for
 * mechanics fixing a ride. The array is indexed by breakdown_reason:
 * - indexes 0-7 are the 8 breakdown reasons (see BREAKDOWN_* in Ride.h)
 *   when fixing a broken down ride;
 * - index 8 is for inspecting a ride.
 */
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

/**
 *
 *  rct2: 0x006C0E8B
 * Also used by inspecting.
 */
void rct_peep::UpdateFixing(int32_t steps)
{
    Ride * ride = get_ride(current_ride);

    if (ride->type == RIDE_TYPE_NULL)
    {
        SetState(PEEP_STATE_FALLING);
        return;
    }

    bool progressToNextSubstate = true;
    bool firstRun               = true;

    if ((state == PEEP_STATE_INSPECTING) &&
        (ride->lifecycle_flags & ( RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)))
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

        int32_t subState                = sub_state;
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
bool rct_peep::UpdateFixingEnterStation(Ride * ride)
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
bool rct_peep::UpdateFixingMoveToBrokenDownVehicle(bool firstRun, Ride * ride)
{
    if (!firstRun)
    {
        rct_vehicle * vehicle = ride_get_broken_vehicle(ride);
        if (vehicle == nullptr)
        {
            return true;
        }

        while (true)
        {
            if (vehicle->is_child == 0)
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

        LocationXY16 offset   = word_981D6C[direction];
        destination_x         = (offset.x * -12) + vehicle->x;
        destination_y         = (offset.y * -12) + vehicle->y;
        destination_tolerance = 2;
    }

    Invalidate();
    int16_t actionX, actionY, tmp_xy_distance;
    if (UpdateAction(&actionX, &actionY, &tmp_xy_distance))
    {
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        Invalidate();
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
bool rct_peep::UpdateFixingFixVehicle(bool firstRun, Ride * ride)
{
    if (!firstRun)
    {
        sprite_direction = direction << 3;

        action                     = (scenario_rand() & 1) ? PEEP_ACTION_STAFF_FIX_2 : PEEP_ACTION_STAFF_FIX;
        action_sprite_image_offset = 0;
        action_frame               = 0;
        UpdateCurrentActionSpriteType();
        Invalidate();
    }

    if (action == PEEP_ACTION_NONE_2)
    {
        return true;
    }

    UpdateAction();

    uint8_t actionFrame = (action == PEEP_ACTION_STAFF_FIX) ? 0x25 : 0x50;
    if (action_frame != actionFrame)
    {
        return false;
    }

    rct_vehicle * vehicle = ride_get_broken_vehicle(ride);
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
bool rct_peep::UpdateFixingFixVehicleMalfunction(bool firstRun, Ride * ride)
{
    if (!firstRun)
    {
        sprite_direction           = direction << 3;
        action                     = PEEP_ACTION_STAFF_FIX_3;
        action_sprite_image_offset = 0;
        action_frame               = 0;

        UpdateCurrentActionSpriteType();
        Invalidate();
    }

    if (action == PEEP_ACTION_NONE_2)
    {
        return true;
    }

    UpdateAction();
    if (action_frame != 0x65)
    {
        return false;
    }

    rct_vehicle * vehicle = ride_get_broken_vehicle(ride);
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
 * fixing sub_state: move_to_station_end - applies to fixing station specific breakdowns: safety cut-out, control failure, inspection.
 * - see FixingSubstatesForBreakdown[]
 */
bool rct_peep::UpdateFixingMoveToStationEnd(bool firstRun, Ride * ride)
{
    if (!firstRun)
    {
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3 | RIDE_TYPE_FLAG_HAS_NO_TRACK))
        {
            return true;
        }

        LocationXY8 stationPosition = ride->station_starts[current_ride_station];
        if (stationPosition.xy == RCT_XY8_UNDEFINED)
        {
            return true;
        }

        uint8_t  stationZ = ride->station_heights[current_ride_station];
        uint16_t stationX = stationPosition.x * 32;
        uint16_t stationY = stationPosition.y * 32;

        rct_tile_element * tileElement = map_get_track_element_at(stationX, stationY, stationZ);
        if (tileElement == nullptr)
        {
            log_error("Couldn't find tile_element");
            return false;
        }

        int32_t   trackDirection = tile_element_get_direction(tileElement);
        CoordsXY offset         = _StationFixingOffsets[trackDirection];

        stationX += 16 + offset.x;
        if (offset.x == 0)
        {
            stationX = destination_x;
        }

        stationY += 16 + offset.y;
        if (offset.y == 0)
        {
            stationY = destination_y;
        }

        destination_x         = stationX;
        destination_y         = stationY;
        destination_tolerance = 2;
    }

    Invalidate();
    int16_t actionX, actionY, tmp_distance;
    if (!UpdateAction(&actionX, &actionY, &tmp_distance))
    {
        return true;
    }

    sprite_move(actionX, actionY, z, (rct_sprite *)this);
    Invalidate();

    return false;
}

/**
 * rct2: 0x006C11F5
 * fixing sub_state: fix_station_end - applies to fixing station specific breakdowns: safety cut-out, control failure, inspection.
 * - see FixingSubstatesForBreakdown[]
 */
bool rct_peep::UpdateFixingFixStationEnd(bool firstRun)
{
    if (!firstRun)
    {
        sprite_direction           = direction << 3;
        action                     = PEEP_ACTION_STAFF_CHECKBOARD;
        action_frame               = 0;
        action_sprite_image_offset = 0;

        UpdateCurrentActionSpriteType();
        Invalidate();
    }

    if (action == PEEP_ACTION_NONE_2)
    {
        return true;
    }

    UpdateAction();

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
bool rct_peep::UpdateFixingMoveToStationStart(bool firstRun, Ride * ride)
{
    if (!firstRun)
    {
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3 | RIDE_TYPE_FLAG_HAS_NO_TRACK))
        {
            return true;
        }

        LocationXY8 stationPosition = ride->station_starts[current_ride_station];
        if (stationPosition.xy == RCT_XY8_UNDEFINED)
        {
            return true;
        }

        uint8_t stationZ = ride->station_heights[current_ride_station];

        CoordsXYE input;
        input.x       = stationPosition.x * 32;
        input.y       = stationPosition.y * 32;
        input.element = map_get_track_element_at_from_ride(input.x, input.y, stationZ, current_ride);
        if (input.element == nullptr)
        {
            return true;
        }

        uint8_t           stationDirection = 0;
        track_begin_end trackBeginEnd;
        while (track_block_get_previous(input.x, input.y, input.element, &trackBeginEnd))
        {
            if (track_element_is_station(trackBeginEnd.begin_element))
            {
                input.x       = trackBeginEnd.begin_x;
                input.y       = trackBeginEnd.begin_y;
                input.element = trackBeginEnd.begin_element;

                stationDirection = tile_element_get_direction(trackBeginEnd.begin_element);
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

        destination_x         = destinationX;
        destination_y         = destinationY;
        destination_tolerance = 2;
    }

    Invalidate();
    int16_t actionX, actionY, tmp_xy_distance;
    if (!UpdateAction(&actionX, &actionY, &tmp_xy_distance))
    {
        return true;
    }

    sprite_move(actionX, actionY, z, (rct_sprite *)this);
    Invalidate();

    return false;
}

/**
 * rct2: 0x006C1368
 * fixing sub_state: fix_station_start
 * 1. applies to fixing station specific breakdowns: safety cut-out, control failure,
 * 2. applies to inspection.
 * - see FixingSubstatesForBreakdown[]
 */
bool rct_peep::UpdateFixingFixStationStart(bool firstRun, Ride * ride)
{
    if (!firstRun)
    {
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3 | RIDE_TYPE_FLAG_HAS_NO_TRACK))
        {
            return true;
        }

        sprite_direction = direction << 3;

        action                     = PEEP_ACTION_STAFF_FIX;
        action_frame               = 0;
        action_sprite_image_offset = 0;

        UpdateCurrentActionSpriteType();
        Invalidate();
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
bool rct_peep::UpdateFixingFixStationBrakes(bool firstRun, Ride * ride)
{
    if (!firstRun)
    {
        sprite_direction = direction << 3;

        action                     = PEEP_ACTION_STAFF_FIX_GROUND;
        action_frame               = 0;
        action_sprite_image_offset = 0;

        UpdateCurrentActionSpriteType();
        Invalidate();
    }

    if (action == PEEP_ACTION_NONE_2)
    {
        return true;
    }

    UpdateAction();
    if (action_frame == 0x28)
    {
        ride->mechanic_status = RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES;
        ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE;
    }

    if (action_frame == 0x13 || action_frame == 0x19 || action_frame == 0x1F || action_frame == 0x25 ||
        action_frame == 0x2B)
    {
        audio_play_sound_at_location(SOUND_MECHANIC_FIX, x, y, z);
    }

    return false;
}

/**
 * rct2: 0x006C1474
 * fixing sub_state: move_to_station_exit - applies to fixing all failures & inspections
 * - see FixingSubstatesForBreakdown[]
 */
bool rct_peep::UpdateFixingMoveToStationExit(bool firstRun, Ride * ride)
{
    if (!firstRun)
    {
        TileCoordsXYZD stationPosition = ride_get_exit_location(ride, current_ride_station);
        if (stationPosition.isNull())
        {
            stationPosition = ride_get_entrance_location(ride, current_ride_station);

            if (stationPosition.isNull())
            {
                return true;
            }
        }

        uint16_t stationX = stationPosition.x * 32;
        uint16_t stationY = stationPosition.y * 32;

        stationX += 16;
        stationY += 16;

        LocationXY16 stationPlatformDirection = word_981D6C[direction];
        stationX += stationPlatformDirection.x * 20;
        stationY += stationPlatformDirection.y * 20;

        destination_x         = stationX;
        destination_y         = stationY;
        destination_tolerance = 2;
    }

    Invalidate();
    int16_t actionX, actionY, tmp_xy_distance;
    if (!UpdateAction(&actionX, &actionY, &tmp_xy_distance))
    {
        return true;
    }
    else
    {
        sprite_move(actionX, actionY, z, (rct_sprite *)this);
        Invalidate();
    }

    return false;
}

/**
 * rct2: 0x006C1504
 * fixing sub_state: finish_fix_or_inspect - applies to fixing all failures & inspections
 * - see FixingSubstatesForBreakdown[]
 */
bool rct_peep::UpdateFixingFinishFixOrInspect(bool firstRun, int32_t steps, Ride * ride)
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

        sprite_direction           = direction << 3;
        action                     = PEEP_ACTION_STAFF_ANSWER_CALL_2;
        action_frame               = 0;
        action_sprite_image_offset = 0;

        UpdateCurrentActionSpriteType();
        Invalidate();
    }

    if (action != 0xFF)
    {
        UpdateAction();
        return false;
    }

    ride_fix_breakdown(current_ride, steps);

    return true;
}

/**
 * rct2: 0x006C157E
 * fixing sub_state: leave_by_entrance_exit - applies to fixing all failures & inspections
 * - see FixingSubstatesForBreakdown[]
 */
bool rct_peep::UpdateFixingLeaveByEntranceExit(bool firstRun, Ride * ride)
{
    if (!firstRun)
    {
        TileCoordsXYZD exitPosition = ride_get_exit_location(ride, current_ride_station);
        if (exitPosition.isNull())
        {
            exitPosition = ride_get_entrance_location(ride, current_ride_station);

            if (exitPosition.isNull())
            {
                SetState(PEEP_STATE_FALLING);
                return false;
            }
        }

        uint16_t exitX = exitPosition.x * 32;
        uint16_t exitY = exitPosition.y * 32;

        exitX += 16;
        exitY += 16;

        LocationXY16 ebx_direction = word_981D6C[direction];
        exitX -= ebx_direction.x * 19;
        exitY -= ebx_direction.y * 19;

        destination_x         = exitX;
        destination_y         = exitY;
        destination_tolerance = 2;
    }

    Invalidate();
    int16_t actionX, actionY, xy_distance;
    if (!UpdateAction(&actionX, &actionY, &xy_distance))
    {
        SetState(PEEP_STATE_FALLING);
        return false;
    }

    uint16_t stationHeight = ride->station_heights[current_ride_station] * 8;

    if (xy_distance >= 16)
    {
        stationHeight += RideData5[ride->type].z;
    }

    sprite_move(actionX, actionY, stationHeight, (rct_sprite *)this);
    Invalidate();

    return false;
}

/**
 * rct2: 0x6B7588
 */
void rct_peep::UpdateRideInspected(int32_t rideIndex)
{
    Ride * ride = get_ride(rideIndex);
    ride->lifecycle_flags &= ~RIDE_LIFECYCLE_DUE_INSPECTION;

    ride->reliability += ((100 - ride->reliability_percentage) / 4) * (scenario_rand() & 0xFF);
    ride->last_inspection = 0;
    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_MAINTENANCE | RIDE_INVALIDATE_RIDE_MAIN | RIDE_INVALIDATE_RIDE_LIST;
}
