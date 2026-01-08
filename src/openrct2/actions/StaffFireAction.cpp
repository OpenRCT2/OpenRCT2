/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffFireAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../drawing/Drawing.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../ui/WindowManager.h"

namespace OpenRCT2::GameActions
{
    StaffFireAction::StaffFireAction(EntityId spriteId)
        : _spriteId(spriteId)
    {
    }

    void StaffFireAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("id", _spriteId);
    }

    uint16_t StaffFireAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void StaffFireAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_spriteId);
    }

    Result StaffFireAction::Query(GameState_t& gameState) const
    {
        if (_spriteId.ToUnderlying() >= kMaxEntities || _spriteId.IsNull())
        {
            LOG_ERROR("Invalid spriteId %u", _spriteId);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        auto staff = gameState.entities.TryGetEntity<Staff>(_spriteId);
        if (staff == nullptr)
        {
            LOG_ERROR("Staff entity not found for spriteId %u", _spriteId);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_STAFF_NOT_FOUND);
        }

        if (staff->State == PeepState::fixing)
        {
            return Result(Status::disallowed, STR_CANT_FIRE_STAFF_FIXING, kStringIdNone);
        }
        else if (staff->State == PeepState::inspecting)
        {
            return Result(Status::disallowed, STR_CANT_FIRE_STAFF_INSPECTING, kStringIdNone);
        }

        return Result();
    }

    Result StaffFireAction::Execute(GameState_t& gameState) const
    {
        auto staff = gameState.entities.TryGetEntity<Staff>(_spriteId);
        if (staff == nullptr)
        {
            LOG_ERROR("Staff entity not found for spriteId %u", _spriteId);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_STAFF_NOT_FOUND);
        }

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::firePrompt);

        PeepEntityRemove(staff);
        // Due to patrol areas best to invalidate the whole screen on removal of staff
        GfxInvalidateScreen();

        return Result();
    }
} // namespace OpenRCT2::GameActions
