/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GuestSetFlagsAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../OpenRCT2.h"
#include "../entity/EntityRegistry.h"

using namespace OpenRCT2;

GuestSetFlagsAction::GuestSetFlagsAction(EntityId peepId, uint32_t flags)
    : _peepId(peepId)
    , _newFlags(flags)
{
}

void GuestSetFlagsAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("peep", _peepId);
    visitor.Visit("guestFlags", _newFlags);
}

uint16_t GuestSetFlagsAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void GuestSetFlagsAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_peepId) << DS_TAG(_newFlags);
}

GameActions::Result GuestSetFlagsAction::Query() const
{
    auto* peep = TryGetEntity<Guest>(_peepId);
    if (peep == nullptr)
    {
        LOG_ERROR("Guest entity not found for peepID %u", _peepId.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_THIS, kStringIdNone);
    }
    return GameActions::Result();
}

GameActions::Result GuestSetFlagsAction::Execute() const
{
    auto* peep = TryGetEntity<Guest>(_peepId);
    if (peep == nullptr)
    {
        LOG_ERROR("Guest entity not found for peepID %u", _peepId.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_THIS, kStringIdNone);
    }

    peep->PeepFlags = _newFlags;

    return GameActions::Result();
}
