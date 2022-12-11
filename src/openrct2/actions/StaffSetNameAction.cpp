/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetNameAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../windows/Intent.h"
#include "../world/Park.h"

StaffSetNameAction::StaffSetNameAction(EntityId spriteIndex, const std::string& name)
    : _spriteIndex(spriteIndex)
    , _name(name)
{
}

void StaffSetNameAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("id", _spriteIndex);
    visitor.Visit("name", _name);
}

uint16_t StaffSetNameAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void StaffSetNameAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_spriteIndex) << DS_TAG(_name);
}

GameActions::Result StaffSetNameAction::Query() const
{
    if (_spriteIndex.ToUnderlying() >= MAX_ENTITIES || _spriteIndex.IsNull())
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_STAFF_ERROR_CANT_NAME_STAFF_MEMBER, STR_NONE);
    }

    auto staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr)
    {
        log_warning("Invalid game command for sprite %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_STAFF_ERROR_CANT_NAME_STAFF_MEMBER, STR_NONE);
    }

    return GameActions::Result();
}

GameActions::Result StaffSetNameAction::Execute() const
{
    auto staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr)
    {
        log_warning("Invalid game command for sprite %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_STAFF_ERROR_CANT_NAME_STAFF_MEMBER, STR_NONE);
    }

    auto curName = staff->GetName();
    if (curName == _name)
    {
        return GameActions::Result();
    }

    if (!staff->SetName(_name))
    {
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_NAME_GUEST, STR_NONE);
    }

    gfx_invalidate_screen();

    auto intent = Intent(INTENT_ACTION_REFRESH_STAFF_LIST);
    ContextBroadcastIntent(&intent);

    auto res = GameActions::Result();
    res.Position = staff->GetLocation();

    return res;
}
