/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GuestSetNameAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../entity/EntityRegistry.h"
#include "../localisation/StringIds.h"
#include "../windows/Intent.h"
#include "../world/Park.h"

using namespace OpenRCT2;

GuestSetNameAction::GuestSetNameAction(EntityId spriteIndex, const std::string& name)
    : _spriteIndex(spriteIndex)
    , _name(name)
{
}

EntityId GuestSetNameAction::GetSpriteIndex() const
{
    return _spriteIndex;
}

std::string GuestSetNameAction::GetGuestName() const
{
    return _name;
}

void GuestSetNameAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("peep", _spriteIndex);
    visitor.Visit("name", _name);
}

uint16_t GuestSetNameAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void GuestSetNameAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_spriteIndex) << DS_TAG(_name);
}

GameActions::Result GuestSetNameAction::Query() const
{
    if (_spriteIndex.ToUnderlying() >= kMaxEntities || _spriteIndex.IsNull())
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_NAME_GUEST, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    auto guest = TryGetEntity<Guest>(_spriteIndex);
    if (guest == nullptr)
    {
        LOG_ERROR("Guest entity not found for spriteIndex %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_NAME_GUEST, kStringIdNone);
    }

    return GameActions::Result();
}

GameActions::Result GuestSetNameAction::Execute() const
{
    auto guest = TryGetEntity<Guest>(_spriteIndex);
    if (guest == nullptr)
    {
        LOG_ERROR("Guest entity not found for spriteIndex %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_NAME_GUEST, kStringIdNone);
    }

    auto curName = guest->GetName();
    if (curName == _name)
    {
        return GameActions::Result();
    }

    if (!guest->SetName(_name))
    {
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_NAME_GUEST, kStringIdNone);
    }

    // Easter egg functions are for guests only
    guest->HandleEasterEggName();

    GfxInvalidateScreen();

    auto intent = Intent(INTENT_ACTION_REFRESH_GUEST_LIST);
    ContextBroadcastIntent(&intent);

    auto res = GameActions::Result();
    res.Position = guest->GetLocation();

    return res;
}
