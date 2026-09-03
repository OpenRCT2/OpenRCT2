/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GuestSetFlagsAction.h"

#include "../../Diagnostic.h"
#include "../../GameState.h"
#include "../../entity/EntityRegistry.h"

namespace OpenRCT2::GameActions
{
    GuestSetFlagsAction::GuestSetFlagsAction(EntityId peepId, PeepFlags flags)
        : _peepId(peepId)
        , _newFlags(flags)
    {
    }

    void GuestSetFlagsAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("peep", _peepId);
        visitor.Visit("guestFlags", _newFlags.holder);
    }

    uint16_t GuestSetFlagsAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void GuestSetFlagsAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_peepId) << DS_TAG(_newFlags.holder);
    }

    Result GuestSetFlagsAction::Query(GameState_t& gameState, Park::ParkData& park) const
    {
        auto* peep = gameState.entities.tryGetEntity<Guest>(_peepId);
        if (peep == nullptr)
        {
            LOG_ERROR("Guest entity not found for peepID %u", _peepId.ToUnderlying());
            return Result(Status::invalidParameters, STR_CANT_CHANGE_THIS, kStringIdNone);
        }
        return Result();
    }

    Result GuestSetFlagsAction::Execute(GameState_t& gameState, Park::ParkData& park) const
    {
        auto* peep = gameState.entities.tryGetEntity<Guest>(_peepId);
        if (peep == nullptr)
        {
            LOG_ERROR("Guest entity not found for peepID %u", _peepId.ToUnderlying());
            return Result(Status::invalidParameters, STR_CANT_CHANGE_THIS, kStringIdNone);
        }

        peep->peepFlags = _newFlags;

        return Result();
    }
} // namespace OpenRCT2::GameActions
