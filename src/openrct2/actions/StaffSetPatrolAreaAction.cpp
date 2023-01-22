/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
namespace OpenRCT2
{
    StaffSetPatrolAreaAction::StaffSetPatrolAreaAction(
        EntityId spriteId, const MapRange& range, const StaffSetPatrolAreaMode mode)
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
            LOG_ERROR("Invalid entity ID: %u", _spriteId.ToUnderlying());
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
        }

        auto validRange = ClampRangeWithinMap(_range);
        for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += COORDS_XY_STEP)
        {
            for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += COORDS_XY_STEP)
            {
                if (!LocationValid({ x, y }))
                {
                    return GameActions::Result(GameActions::Status::NotOwned, STR_SET_PATROL_AREA, STR_LAND_NOT_OWNED_BY_PARK);
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
} // namespace OpenRCT2
