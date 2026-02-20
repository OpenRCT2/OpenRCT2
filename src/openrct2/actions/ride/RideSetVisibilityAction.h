/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../GameAction.hpp"

namespace OpenRCT2::GameActions
{
    enum class RideSetVisibilityType : uint8_t
    {
        invisible,
        visible,
    };

    class RideSetVisibilityAction final : public GameActionBase<GameCommand::setRideVisibility>
    {
    private:
        RideId _rideIndex{ RideId::GetNull() };
        RideSetVisibilityType _visibility{};

    public:
        RideSetVisibilityAction() = default;
        RideSetVisibilityAction(RideId rideIndex, RideSetVisibilityType visibility);

        void AcceptParameters(GameActionParameterVisitor& visitor) override;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
