/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetOrdersAction.h"

#include "../Context.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../peep/Staff.h"
#include "../windows/Intent.h"
#include "../world/Entity.h"

StaffSetOrdersAction::StaffSetOrdersAction(uint16_t spriteIndex, uint8_t ordersId)
    : _spriteIndex(spriteIndex)
    , _ordersId(ordersId)
{
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

GameActions::Result::Ptr StaffSetOrdersAction::Query() const
{
    if (_spriteIndex >= MAX_ENTITIES)
    {
        return std::make_unique<GameActions::Result>(GameActions::Status::InvalidParameters, STR_NONE);
    }

    auto* staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr
        || (staff->AssignedStaffType != StaffType::Handyman && staff->AssignedStaffType != StaffType::Mechanic))
    {
        log_warning("Invalid game command for sprite %u", _spriteIndex);
        return std::make_unique<GameActions::Result>(GameActions::Status::InvalidParameters, STR_NONE);
    }

    return std::make_unique<GameActions::Result>();
}

GameActions::Result::Ptr StaffSetOrdersAction::Execute() const
{
    auto* staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr)
    {
        log_warning("Invalid game command for sprite %u", _spriteIndex);
        return std::make_unique<GameActions::Result>(GameActions::Status::InvalidParameters, STR_NONE);
    }
    staff->StaffOrders = _ordersId;

    window_invalidate_by_number(WC_PEEP, _spriteIndex);
    auto intent = Intent(INTENT_ACTION_REFRESH_STAFF_LIST);
    context_broadcast_intent(&intent);

    auto res = std::make_unique<GameActions::Result>();
    res->Position.x = staff->x;
    res->Position.y = staff->y;
    res->Position.z = staff->z;
    return res;
}
