/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetPatrolAreaAction.h"

#include "../interface/Window.h"
#include "../peep/Peep.h"
#include "../peep/Staff.h"
#include "../world/Entity.h"

StaffSetPatrolAreaAction::StaffSetPatrolAreaAction(uint16_t spriteId, const CoordsXY& loc, const StaffSetPatrolAreaMode mode)
    : _spriteId(spriteId)
    , _loc(loc)
    , _mode(mode)
{
}

uint16_t StaffSetPatrolAreaAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void StaffSetPatrolAreaAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_spriteId) << DS_TAG(_loc) << DS_TAG(_mode);
}

GameActions::Result::Ptr StaffSetPatrolAreaAction::Query() const
{
    if (_spriteId >= MAX_ENTITIES)
    {
        log_error("Invalid spriteId. spriteId = %u", _spriteId);
        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    if (!LocationValid(_loc))
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    auto staff = TryGetEntity<Staff>(_spriteId);
    if (staff == nullptr)
    {
        log_error("Invalid spriteId. spriteId = %u", _spriteId);
        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    return MakeResult();
}

static void UpdateStaffMode(const Staff& staff)
{
    bool isPatrolling = false;
    const auto peepOffset = staff.StaffId * STAFF_PATROL_AREA_SIZE;
    for (size_t i = peepOffset; i < peepOffset + STAFF_PATROL_AREA_SIZE; i++)
    {
        if (gStaffPatrolAreas[i] != 0)
        {
            isPatrolling = true;
            break;
        }
    }

    if (isPatrolling)
    {
        gStaffModes[staff.StaffId] = StaffMode::Patrol;
    }
    else if (gStaffModes[staff.StaffId] == StaffMode::Patrol)
    {
        gStaffModes[staff.StaffId] = StaffMode::Walk;
    }
}

static void InvalidatePatrolTile(const CoordsXY& loc)
{
    // Align the location to the top left of the patrol square
    const auto alignedLoc = CoordsXY{ loc.x & 0x1F80, loc.y & 0x1F80 };
    for (int32_t y = 0; y < 4 * COORDS_XY_STEP; y += COORDS_XY_STEP)
    {
        for (int32_t x = 0; x < 4 * COORDS_XY_STEP; x += COORDS_XY_STEP)
        {
            map_invalidate_tile_full(alignedLoc + CoordsXY{ x, y });
        }
    }
}

GameActions::Result::Ptr StaffSetPatrolAreaAction::Execute() const
{
    auto staff = TryGetEntity<Staff>(_spriteId);
    if (staff == nullptr)
    {
        log_error("Invalid spriteId. spriteId = %u", _spriteId);
        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    switch (_mode)
    {
        case StaffSetPatrolAreaMode::Set:
            staff->SetPatrolArea(_loc, true);
            UpdateStaffMode(*staff);
            InvalidatePatrolTile(_loc);
            break;
        case StaffSetPatrolAreaMode::Unset:
            staff->SetPatrolArea(_loc, false);
            UpdateStaffMode(*staff);
            InvalidatePatrolTile(_loc);
            break;
        case StaffSetPatrolAreaMode::ClearAll:
            staff->ClearPatrolArea();
            gfx_invalidate_screen();
            break;
    }

    staff_update_greyed_patrol_areas();

    return MakeResult();
}
