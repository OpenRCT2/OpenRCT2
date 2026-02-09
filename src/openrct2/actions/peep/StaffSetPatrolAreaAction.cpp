/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetPatrolAreaAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../drawing/Drawing.h"
#include "../entity/EntityRegistry.h"
#include "../entity/PatrolArea.h"
#include "../entity/Peep.h"
#include "../entity/Staff.h"
#include "../world/Map.h"

namespace OpenRCT2::GameActions
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void StaffSetPatrolAreaAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_spriteId) << DS_TAG(_range) << DS_TAG(_mode);
    }

    Result StaffSetPatrolAreaAction::Query(GameState_t& gameState) const
    {
        return QueryExecute(gameState, false);
    }

    Result StaffSetPatrolAreaAction::Execute(GameState_t& gameState) const
    {
        return QueryExecute(gameState, true);
    }

    static void InvalidatePatrolTiles(const MapRange& range)
    {
        MapInvalidateRegion(range.Point1, range.Point2);
    }

    Result StaffSetPatrolAreaAction::QueryExecute(GameState_t& gameState, bool executing) const
    {
        auto staff = gameState.entities.TryGetEntity<Staff>(_spriteId);
        if (staff == nullptr)
        {
            LOG_ERROR("Staff entity not found for spriteID %u", _spriteId.ToUnderlying());
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_STAFF_NOT_FOUND);
        }

        auto validRange = ClampRangeWithinMap(_range);
        for (int32_t y = validRange.GetY1(); y <= validRange.GetY2(); y += kCoordsXYStep)
        {
            for (int32_t x = validRange.GetX1(); x <= validRange.GetX2(); x += kCoordsXYStep)
            {
                if (!LocationValid({ x, y }))
                {
                    return Result(Status::invalidParameters, STR_SET_PATROL_AREA, STR_OFF_EDGE_OF_MAP);
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
        return Result();
    }
} // namespace OpenRCT2::GameActions
