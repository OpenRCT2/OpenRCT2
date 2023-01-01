/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetOrdersAction.h"

#include "../Context.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../windows/Intent.h"

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
    if (_spriteIndex.ToUnderlying() >= MAX_ENTITIES || _spriteIndex.IsNull())
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    auto* staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr
        || (staff->AssignedStaffType != StaffType::Handyman && staff->AssignedStaffType != StaffType::Mechanic))
    {
        log_warning("Invalid game command for sprite %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    return GameActions::Result();
}

GameActions::Result StaffSetOrdersAction::Execute() const
{
    auto* staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr)
    {
        log_warning("Invalid game command for sprite %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }
    staff->StaffOrders = _ordersId;

    window_invalidate_by_number(WindowClass::Peep, _spriteIndex);
    auto intent = Intent(INTENT_ACTION_REFRESH_STAFF_LIST);
    ContextBroadcastIntent(&intent);

    auto res = GameActions::Result();
    res.Position = staff->GetLocation();

    return res;
}
