/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    enum class LandSetRightSetting : uint8_t
    {
        UnownLand,
        UnownConstructionRights,
        SetForSale,
        SetConstructionRightsForSale,
        SetOwnershipWithChecks,
        Count
    };

    class LandSetRightsAction final : public GameActionBase<GameCommand::SetLandOwnership>
    {
    private:
        MapRange _range;
        LandSetRightSetting _setting{ LandSetRightSetting::Count };
        uint8_t _ownership{};

    public:
        LandSetRightsAction() = default;
        LandSetRightsAction(const MapRange& range, LandSetRightSetting setting, uint8_t ownership = 0);
        LandSetRightsAction(const CoordsXY& coord, LandSetRightSetting setting, uint8_t ownership = 0);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        Result QueryExecute(GameState_t& gameState, bool isExecuting) const;
        Result MapBuyLandRightsForTile(GameState_t& gameState, const CoordsXY& loc, bool isExecuting) const;
    };
} // namespace OpenRCT2::GameActions
