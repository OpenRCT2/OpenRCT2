/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../world/MapOwnership.h"
#include "../GameAction.hpp"

namespace OpenRCT2::GameActions
{
    enum class LandSetRightSetting : uint8_t
    {
        unownLand,
        unownConstructionRights,
        setForSale,
        setConstructionRightsForSale,
        setOwnershipWithChecks,
        count
    };

    class LandSetRightsAction final : public GameActionBase<GameCommand::setLandOwnership>
    {
    private:
        MapRange _range;
        LandSetRightSetting _setting{ LandSetRightSetting::count };
        OwnershipFlags _ownership{};

    public:
        LandSetRightsAction() = default;
        LandSetRightsAction(const MapRange& range, LandSetRightSetting setting, OwnershipFlags ownership = kUnowned);
        LandSetRightsAction(const CoordsXY& coord, LandSetRightSetting setting, OwnershipFlags ownership = kUnowned);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState, Park::ParkData& park) const override;
        Result Execute(GameState_t& gameState, Park::ParkData& park) const override;

    private:
        Result QueryExecute(GameState_t& gameState, bool isExecuting) const;
        Result MapBuyLandRightsForTile(GameState_t& gameState, const CoordsXY& loc, bool isExecuting) const;
    };
} // namespace OpenRCT2::GameActions
