/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffFireAction.h"

#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../interface/Window.h"

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
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void StaffFireAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_spriteId);
}

GameActions::Result StaffFireAction::Query() const
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

GameActions::Result StaffFireAction::Execute() const
{
    auto staff = TryGetEntity<Staff>(_spriteId);
    if (staff == nullptr)
    {
        log_error("Invalid spriteId. spriteId = %u", _spriteId);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }
    window_close_by_class(WindowClass::FirePrompt);
    peep_sprite_remove(staff);
    // Due to patrol areas best to invalidate the whole screen on removal of staff
    gfx_invalidate_screen();
    return GameActions::Result();
}
