/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetPatrolAreaAction.h"

#include "../Diagnostic.h"
#include "../entity/EntityRegistry.h"
#include "../entity/PatrolArea.h"
#include "../entity/Peep.h"
#include "../entity/Staff.h"

using namespace OpenRCT2;

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
    return QueryExecute(false);
}

GameActions::Result StaffSetPatrolAreaAction::Execute() const
{
    return QueryExecute(true);
}

static void InvalidatePatrolTiles(const MapRange& range)
{
    MapInvalidateRegion(range.Point1, range.Point2);
}

GameActions::Result StaffSetPatrolAreaAction::QueryExecute(bool executing) const
{
    auto staff = TryGetEntity<Staff>(_spriteId);
    if (staff == nullptr)
    {
        LOG_ERROR("Staff entity not found for spriteID %u", _spriteId.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_STAFF_NOT_FOUND);
    }

    auto validRange = ClampRangeWithinMap(_range);
    for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += kCoordsXYStep)
    {
        for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += kCoordsXYStep)
        {
            if (!LocationValid({ x, y }))
            {
                return GameActions::Result(GameActions::Status::InvalidParameters, STR_SET_PATROL_AREA, STR_OFF_EDGE_OF_MAP);
            }
        }
    }

    if (executing)
    {
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
                GfxInvalidateScreen();
                break;
        }
        UpdateConsolidatedPatrolAreas();
    }
    return GameActions::Result();
}
