/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../scenario/Scenario.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/Entrance.h"
#include "../world/Park.h"
#include "../world/Sprite.h"
#include "GameAction.h"

static constexpr const rct_string_id staffNames[] = {
    STR_HANDYMAN_X,
    STR_MECHANIC_X,
    STR_SECURITY_GUARD_X,
    STR_ENTERTAINER_X,
};

/* rct2: 0x009929FC */
static constexpr const PeepSpriteType spriteTypes[] = {
    PEEP_SPRITE_TYPE_HANDYMAN,
    PEEP_SPRITE_TYPE_MECHANIC,
    PEEP_SPRITE_TYPE_SECURITY,
    PEEP_SPRITE_TYPE_ENTERTAINER_PANDA,
};

class StaffHireNewActionResult final : public GameActionResult
{
public:
    StaffHireNewActionResult()
        : GameActionResult(GA_ERROR::OK, STR_CANT_HIRE_NEW_STAFF)
    {
    }
    StaffHireNewActionResult(GA_ERROR error, rct_string_id message)
        : GameActionResult(error, STR_CANT_HIRE_NEW_STAFF, message)
    {
    }

    uint32_t peepSriteIndex = SPRITE_INDEX_NULL;
};

DEFINE_GAME_ACTION(StaffHireNewAction, GAME_COMMAND_HIRE_NEW_STAFF_MEMBER, StaffHireNewActionResult)
{
private:
    bool _autoPosition = false;
    uint8_t _staffType = STAFF_TYPE::STAFF_TYPE_COUNT;
    uint8_t _entertainerType = ENTERTAINER_COSTUME::ENTERTAINER_COSTUME_COUNT;
    uint32_t _staffOrders = 0;

public:
    StaffHireNewAction() = default;
    StaffHireNewAction(bool autoPosition, STAFF_TYPE staffType, ENTERTAINER_COSTUME entertainerType, uint32_t staffOrders)
        : _autoPosition(autoPosition)
        , _staffType(staffType)
        , _entertainerType(entertainerType)
        , _staffOrders(staffOrders)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags();
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_autoPosition) << DS_TAG(_staffType) << DS_TAG(_entertainerType) << DS_TAG(_staffOrders);
    }

    GameActionResult::Ptr Query() const override
    {
        return QueryExecute(false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return QueryExecute(true);
    }

private:
    GameActionResult::Ptr QueryExecute(bool execute) const
    {
        auto res = std::make_unique<StaffHireNewActionResult>();

        res->ExpenditureType = RCT_EXPENDITURE_TYPE_WAGES;

        if (_staffType >= STAFF_TYPE_COUNT)
        {
            // Invalid staff type.
            log_error("Tried to use invalid staff type: %u", (uint32_t)_staffType);

            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        if (gSpriteListCount[SPRITE_LIST_NULL] < 400)
        {
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_TOO_MANY_PEOPLE_IN_GAME);
        }

        if (_staffType == STAFF_TYPE_ENTERTAINER)
        {
            if (_entertainerType >= ENTERTAINER_COSTUME_COUNT)
            {
                // Invalid entertainer costume
                log_error("Tried to use invalid entertainer type: %u", (uint32_t)_entertainerType);

                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
            }

            uint32_t availableCostumes = staff_get_available_entertainer_costumes();
            if (!(availableCostumes & (1 << _entertainerType)))
            {
                // Entertainer costume unavailable
                log_error("Tried to use unavailable entertainer type: %u", (uint32_t)_entertainerType);

                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
            }
        }

        // Look for a free slot in the staff modes.
        int32_t staffIndex;
        for (staffIndex = 0; staffIndex < STAFF_MAX_COUNT; ++staffIndex)
        {
            if (!(gStaffModes[staffIndex] & 1))
                break;
        }

        if (staffIndex == STAFF_MAX_COUNT)
        {
            // Too many staff members exist already.
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_TOO_MANY_STAFF_IN_GAME);
        }

        Peep* newPeep = &(create_sprite(GetFlags())->peep);
        if (newPeep == nullptr)
        {
            // Too many peeps exist already.
            return MakeResult(GA_ERROR::NO_FREE_ELEMENTS, STR_TOO_MANY_PEOPLE_IN_GAME);
        }

        if (execute == false)
        {
            // In query we just want to see if we can obtain a sprite slot.
            sprite_remove((rct_sprite*)newPeep);
        }
        else
        {
            move_sprite_to_list((rct_sprite*)newPeep, SPRITE_LIST_PEEP * 2);

            newPeep->sprite_identifier = 1;
            newPeep->window_invalidate_flags = 0;
            newPeep->action = PEEP_ACTION_NONE_2;
            newPeep->special_sprite = 0;
            newPeep->action_sprite_image_offset = 0;
            newPeep->no_action_frame_num = 0;
            newPeep->action_sprite_type = PEEP_ACTION_SPRITE_TYPE_NONE;
            newPeep->path_check_optimisation = 0;
            newPeep->type = PEEP_TYPE_STAFF;
            newPeep->outside_of_park = 0;
            newPeep->peep_flags = 0;
            newPeep->paid_to_enter = 0;
            newPeep->paid_on_rides = 0;
            newPeep->paid_on_food = 0;
            newPeep->paid_on_souvenirs = 0;
            newPeep->staff_orders = _staffOrders;

            uint16_t idSearchSpriteIndex;
            Peep* idSearchPeep;

            // We search for the first available id for a given staff type
            uint32_t newStaffId = 0;
            for (;;)
            {
                bool found = false;
                ++newStaffId;

                FOR_ALL_STAFF (idSearchSpriteIndex, idSearchPeep)
                {
                    if (idSearchPeep->staff_type != _staffType)
                        continue;

                    if (idSearchPeep->id == newStaffId)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    break;
            }

            newPeep->id = newStaffId;
            newPeep->staff_type = _staffType;

            PeepSpriteType spriteType = spriteTypes[_staffType];
            if (_staffType == STAFF_TYPE_ENTERTAINER)
            {
                spriteType = static_cast<PeepSpriteType>(PEEP_SPRITE_TYPE_ENTERTAINER_PANDA + _entertainerType);
            }
            newPeep->name_string_idx = staffNames[_staffType];
            newPeep->sprite_type = spriteType;

            const rct_sprite_bounds* spriteBounds = g_peep_animation_entries[spriteType].sprite_bounds;
            newPeep->sprite_width = spriteBounds->sprite_width;
            newPeep->sprite_height_negative = spriteBounds->sprite_height_negative;
            newPeep->sprite_height_positive = spriteBounds->sprite_height_positive;

            if (_autoPosition)
            {
                AutoPositionNewStaff(newPeep);
            }
            else
            {
                // NOTE: This state is required for the window to act.
                newPeep->state = PEEP_STATE_PICKED;

                sprite_move(newPeep->x, newPeep->y, newPeep->z, (rct_sprite*)newPeep);
                invalidate_sprite_2((rct_sprite*)newPeep);
            }

            // Staff uses this
            newPeep->time_in_park = gDateMonthsElapsed;
            newPeep->pathfind_goal.x = 0xFF;
            newPeep->pathfind_goal.y = 0xFF;
            newPeep->pathfind_goal.z = 0xFF;
            newPeep->pathfind_goal.direction = 0xFF;

            uint8_t colour = staff_get_colour(_staffType);
            newPeep->tshirt_colour = colour;
            newPeep->trousers_colour = colour;

            // Staff energy determines their walking speed
            newPeep->energy = 0x60;
            newPeep->energy_target = 0x60;
            newPeep->staff_mowing_timeout = 0;

            peep_update_name_sort(newPeep);

            newPeep->staff_id = staffIndex;

            gStaffModes[staffIndex] = STAFF_MODE_WALK;

            for (int32_t newStaffIndex = 0; newStaffIndex < STAFF_PATROL_AREA_SIZE; newStaffIndex++)
            {
                gStaffPatrolAreas[newStaffIndex * STAFF_PATROL_AREA_SIZE + newStaffId] = 0;
            }

            res->peepSriteIndex = newPeep->sprite_index;
        }

        return res;
    }

    void AutoPositionNewStaff(Peep * newPeep) const
    {
        // Find a location to place new staff member

        newPeep->state = PEEP_STATE_FALLING;

        int16_t x, y, z;
        uint32_t count = 0;
        uint16_t sprite_index;
        Peep* guest = nullptr;
        TileElement* guest_tile = nullptr;

        // Count number of walking guests
        FOR_ALL_GUESTS (sprite_index, guest)
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
            FOR_ALL_GUESTS (sprite_index, guest)
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
            if (!gParkEntrances.empty())
            {
                auto rand = scenario_rand_max((uint32_t)gParkEntrances.size());
                const auto& entrance = gParkEntrances[rand];
                auto dir = entrance.direction;
                x = entrance.x;
                y = entrance.y;
                z = entrance.z;
                x += 16 + ((dir & 1) == 0 ? ((dir & 2) ? 32 : -32) : 0);
                y += 16 + ((dir & 1) == 1 ? ((dir & 2) ? -32 : 32) : 0);
            }
            else
            {
                // User must pick a location
                newPeep->state = PEEP_STATE_PICKED;
                x = newPeep->x;
                y = newPeep->y;
                z = newPeep->z;
            }
        }

        sprite_move(x, y, z + 16, (rct_sprite*)newPeep);
        invalidate_sprite_2((rct_sprite*)newPeep);
    }
};
