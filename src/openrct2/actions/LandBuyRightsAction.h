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
    enum class LandBuyRightSetting : uint8_t
    {
        BuyLand,
        BuyConstructionRights,
        Count
    };

    class LandBuyRightsAction final : public GameActionBase<GameCommand::BuyLandRights>
    {
    private:
        MapRange _range;
        LandBuyRightSetting _setting{ LandBuyRightSetting::Count };

        constexpr static StringId kErrorTitles[] = {
            STR_CANT_BUY_LAND,
            STR_CANT_BUY_CONSTRUCTION_RIGHTS_HERE,
        };

    public:
        LandBuyRightsAction() = default;
        LandBuyRightsAction(const MapRange& range, LandBuyRightSetting setting);
        LandBuyRightsAction(const CoordsXY& coord, LandBuyRightSetting setting);

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
