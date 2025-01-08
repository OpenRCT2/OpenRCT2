/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "GameAction.h"

enum class PeepPickupType : uint8_t
{
    Pickup,
    Cancel,
    Place,
    Count
};

class PeepPickupAction final : public GameActionBase<GameCommand::PickupGuest>
{
private:
    PeepPickupType _type{ PeepPickupType::Count };
    EntityId _entityId{ EntityId::GetNull() };
    CoordsXYZ _loc;
    NetworkPlayerId_t _owner{ -1 };

public:
    PeepPickupAction() = default;
    PeepPickupAction(PeepPickupType type, EntityId entityId, const CoordsXYZ& loc, NetworkPlayerId_t owner);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;

private:
    void CancelConcurrentPickups(Peep* pickedPeep) const;
};
