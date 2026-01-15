/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../entity/EntityRegistry.h"
#include "../localisation/StringIds.h"
#include "../windows/Intent.h"
#include "../world/Park.h"

namespace OpenRCT2::GameActions
{
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void GuestSetNameAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_spriteIndex) << DS_TAG(_name);
    }

    Result GuestSetNameAction::Query(GameState_t& gameState) const
    {
        if (_spriteIndex.ToUnderlying() >= kMaxEntities || _spriteIndex.IsNull())
        {
            return Result(Status::invalidParameters, STR_CANT_NAME_GUEST, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        auto guest = gameState.entities.TryGetEntity<Guest>(_spriteIndex);
        if (guest == nullptr)
        {
            LOG_ERROR("Guest entity not found for spriteIndex %u", _spriteIndex);
            return Result(Status::invalidParameters, STR_CANT_NAME_GUEST, kStringIdNone);
        }

        return Result();
    }

    Result GuestSetNameAction::Execute(GameState_t& gameState) const
    {
        auto guest = gameState.entities.TryGetEntity<Guest>(_spriteIndex);
        if (guest == nullptr)
        {
            LOG_ERROR("Guest entity not found for spriteIndex %u", _spriteIndex);
            return Result(Status::invalidParameters, STR_CANT_NAME_GUEST, kStringIdNone);
        }

        auto curName = guest->GetName();
        if (curName == _name)
        {
            return Result();
        }

        if (!guest->SetName(_name))
        {
            return Result(Status::unknown, STR_CANT_NAME_GUEST, kStringIdNone);
        }

        // Easter egg functions are for guests only
        guest->HandleEasterEggName();

        GfxInvalidateScreen();

        auto intent = Intent(INTENT_ACTION_REFRESH_GUEST_LIST);
        ContextBroadcastIntent(&intent);

        auto res = Result();
        res.position = guest->GetLocation();

        return res;
    }
} // namespace OpenRCT2::GameActions
