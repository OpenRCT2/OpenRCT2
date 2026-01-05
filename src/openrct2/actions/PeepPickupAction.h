/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
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
        Network::PlayerId_t _owner{ -1 };

    public:
        PeepPickupAction() = default;
        PeepPickupAction(PeepPickupType type, EntityId entityId, const CoordsXYZ& loc, Network::PlayerId_t owner);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        void CancelConcurrentPickups(Peep* pickedPeep) const;
    };
} // namespace OpenRCT2::GameActions
