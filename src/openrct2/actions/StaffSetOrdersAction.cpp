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
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../localisation/StringIds.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"

using namespace OpenRCT2;

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
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void StaffSetOrdersAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_spriteIndex) << DS_TAG(_ordersId);
}

GameActions::Result StaffSetOrdersAction::Query() const
{
    if (_spriteIndex.ToUnderlying() >= kMaxEntities || _spriteIndex.IsNull())
    {
        LOG_ERROR("Invalid sprite index %u", _spriteIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    auto* staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr
        || (staff->AssignedStaffType != StaffType::Handyman && staff->AssignedStaffType != StaffType::Mechanic))
    {
        LOG_ERROR("Staff orders can't be changed for staff of type %u", _spriteIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_ACTION_INVALID_FOR_THAT_STAFF_TYPE);
    }

    return GameActions::Result();
}

GameActions::Result StaffSetOrdersAction::Execute() const
{
    auto* staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr)
    {
        LOG_ERROR("Staff entity not found for spriteIndex %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_STAFF_NOT_FOUND);
    }
    staff->StaffOrders = _ordersId;

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::Peep, _spriteIndex);
    auto intent = Intent(INTENT_ACTION_REFRESH_STAFF_LIST);
    ContextBroadcastIntent(&intent);

    auto res = GameActions::Result();
    res.Position = staff->GetLocation();

    return res;
}
