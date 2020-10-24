/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../interface/Window.h"
#include "../peep/Peep.h"
#include "../peep/Staff.h"
#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(StaffSetPatrolAreaAction, GAME_COMMAND_SET_STAFF_PATROL, GameActionResult)
{
private:
    uint16_t _spriteId{ SPRITE_INDEX_NULL };
    CoordsXY _loc;

public:
    StaffSetPatrolAreaAction()
    {
    }
    StaffSetPatrolAreaAction(uint16_t spriteId, const CoordsXY& loc)
        : _spriteId(spriteId)
        , _loc(loc)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_spriteId) << DS_TAG(_loc);
    }

    GameActionResult::Ptr Query() const override
    {
        if (_spriteId >= MAX_SPRITES)
        {
            log_error("Invalid spriteId. spriteId = %u", _spriteId);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        if (!LocationValid(_loc))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        auto staff = TryGetEntity<Staff>(_spriteId);
        if (staff == nullptr)
        {
            log_error("Invalid spriteId. spriteId = %u", _spriteId);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto staff = TryGetEntity<Staff>(_spriteId);
        if (staff == nullptr)
        {
            log_error("Invalid spriteId. spriteId = %u", _spriteId);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        int32_t patrolOffset = staff->StaffId * STAFF_PATROL_AREA_SIZE;

        staff_toggle_patrol_area(staff->StaffId, _loc);

        bool isPatrolling = false;
        for (int32_t i = 0; i < 128; i++)
        {
            if (gStaffPatrolAreas[patrolOffset + i])
            {
                isPatrolling = true;
                break;
            }
        }

        if (isPatrolling)
        {
            gStaffModes[staff->StaffId] = StaffMode::Patrol;
        }
        else if (gStaffModes[staff->StaffId] == StaffMode::Patrol)
        {
            gStaffModes[staff->StaffId] = StaffMode::Walk;
        }

        for (int32_t y = 0; y < 4 * COORDS_XY_STEP; y += COORDS_XY_STEP)
        {
            for (int32_t x = 0; x < 4 * COORDS_XY_STEP; x += COORDS_XY_STEP)
            {
                map_invalidate_tile_full({ (_loc.x & 0x1F80) + x, (_loc.y & 0x1F80) + y });
            }
        }
        staff_update_greyed_patrol_areas();

        return MakeResult();
    }
};
