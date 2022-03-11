/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetPatrolAreaAction.h"

#include "../entity/EntityRegistry.h"
#include "../entity/PatrolArea.h"
#include "../entity/Peep.h"
#include "../entity/Staff.h"
#include "../interface/Window.h"

StaffSetPatrolAreaAction::StaffSetPatrolAreaAction(EntityId spriteId, const MapRange& range, const StaffSetPatrolAreaMode mode)
    : _spriteId(spriteId)
    , _range(range)
    , _mode(mode)
{
}

void StaffSetPatrolAreaAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("id", _spriteId);
    visitor.Visit(_range);
    visitor.Visit("mode", _mode);
}

uint16_t StaffSetPatrolAreaAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void StaffSetPatrolAreaAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_spriteId) << DS_TAG(_range) << DS_TAG(_mode);
}

GameActions::Result StaffSetPatrolAreaAction::Query() const
{
    if (_spriteId.ToUnderlying() >= MAX_ENTITIES || _spriteId.IsNull())
    {
        log_error("Invalid spriteId. spriteId = %u", _spriteId);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    auto staff = TryGetEntity<Staff>(_spriteId);
    if (staff == nullptr)
    {
        log_error("Invalid spriteId. spriteId = %u", _spriteId);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    return GameActions::Result();
}

static void InvalidatePatrolTiles(const MapRange& range)
{
    map_invalidate_region(range.Point1, range.Point2);
}

GameActions::Result StaffSetPatrolAreaAction::Execute() const
{
    auto staff = TryGetEntity<Staff>(_spriteId);
    if (staff == nullptr)
    {
        log_error("Invalid spriteId. spriteId = %u", _spriteId);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    switch (_mode)
    {
        case StaffSetPatrolAreaMode::Set:
            staff->SetPatrolArea(_range, true);
            InvalidatePatrolTiles(_range);
            break;
        case StaffSetPatrolAreaMode::Unset:
            staff->SetPatrolArea(_range, false);
            if (!staff->HasPatrolArea())
            {
                staff->ClearPatrolArea();
            }
            InvalidatePatrolTiles(_range);
            break;
        case StaffSetPatrolAreaMode::ClearAll:
            staff->ClearPatrolArea();
            gfx_invalidate_screen();
            break;
    }

    UpdateConsolidatedPatrolAreas();

    return GameActions::Result();
}
