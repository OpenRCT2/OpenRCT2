/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

enum class PeepPickupType : uint8_t
{
    Pickup,
    Cancel,
    Place,
    Count
};

DEFINE_GAME_ACTION(PeepPickupAction, GameCommand::PickupGuest, GameActions::Result)
{
private:
    PeepPickupType _type{ PeepPickupType::Count };
    uint32_t _spriteId{ SPRITE_INDEX_NULL };
    CoordsXYZ _loc;
    NetworkPlayerId_t _owner{ -1 };

public:
    PeepPickupAction() = default;
    PeepPickupAction(PeepPickupType type, uint32_t spriteId, const CoordsXYZ& loc, NetworkPlayerId_t owner);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    void CancelConcurrentPickups(Peep * pickedPeep) const;
};
