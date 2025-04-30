/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetNameAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Staff.h"
#include "../localisation/StringIds.h"
#include "../windows/Intent.h"
#include "../world/Park.h"

using namespace OpenRCT2;

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
    if (_spriteIndex.ToUnderlying() >= kMaxEntities || _spriteIndex.IsNull())
    {
        LOG_ERROR("Invalid sprite index %u", _spriteIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_STAFF_ERROR_CANT_NAME_STAFF_MEMBER, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    auto staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr)
    {
        LOG_ERROR("Staff entity not found for spriteIndex %u", _spriteIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_STAFF_ERROR_CANT_NAME_STAFF_MEMBER, STR_ERR_STAFF_NOT_FOUND);
    }

    return GameActions::Result();
}

GameActions::Result StaffSetNameAction::Execute() const
{
    auto staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr)
    {
        LOG_ERROR("Staff entity not found for spriteIndex %u", _spriteIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_STAFF_ERROR_CANT_NAME_STAFF_MEMBER, STR_ERR_STAFF_NOT_FOUND);
    }

    auto curName = staff->GetName();
    if (curName == _name)
    {
        return GameActions::Result();
    }

    if (!staff->SetName(_name))
    {
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_NAME_GUEST, kStringIdNone);
    }

    GfxInvalidateScreen();

    auto intent = Intent(INTENT_ACTION_REFRESH_STAFF_LIST);
    ContextBroadcastIntent(&intent);

    auto res = GameActions::Result();
    res.Position = staff->GetLocation();

    return res;
}
