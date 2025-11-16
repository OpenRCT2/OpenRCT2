/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetOrdersAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../localisation/StringIds.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"

namespace OpenRCT2::GameActions
{
    StaffSetOrdersAction::StaffSetOrdersAction(EntityId spriteIndex, uint8_t ordersId)
        : _spriteIndex(spriteIndex)
        , _ordersId(ordersId)
    {
    }

    void StaffSetOrdersAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("id", _spriteIndex);
        visitor.Visit("staffOrders", _ordersId);
    }

    uint16_t StaffSetOrdersAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void StaffSetOrdersAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_spriteIndex) << DS_TAG(_ordersId);
    }

    Result StaffSetOrdersAction::Query(GameState_t& gameState) const
    {
        if (_spriteIndex.ToUnderlying() >= kMaxEntities || _spriteIndex.IsNull())
        {
            return Result(Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        auto* staff = getGameState().entities.TryGetEntity<Staff>(_spriteIndex);
        if (staff == nullptr
            || (staff->AssignedStaffType != StaffType::handyman && staff->AssignedStaffType != StaffType::mechanic))
        {
            return Result(Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_ACTION_INVALID_FOR_THAT_STAFF_TYPE);
        }

        return Result();
    }

    Result StaffSetOrdersAction::Execute(GameState_t& gameState) const
    {
        auto* staff = getGameState().entities.TryGetEntity<Staff>(_spriteIndex);
        if (staff == nullptr)
        {
            LOG_ERROR("Staff entity not found for spriteIndex %u", _spriteIndex);
            return Result(Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_STAFF_NOT_FOUND);
        }
        staff->StaffOrders = _ordersId;

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::peep, _spriteIndex);
        auto intent = Intent(INTENT_ACTION_REFRESH_STAFF_LIST);
        ContextBroadcastIntent(&intent);

        auto res = Result();
        res.Position = staff->GetLocation();

        return res;
    }
} // namespace OpenRCT2::GameActions
