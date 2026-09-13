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
    class SmallSceneryRemoveAction final : public GameActionBase<GameCommand::RemoveScenery>
    {
    private:
        CoordsXYZ _loc;
        uint8_t _quadrant{};
        ObjectEntryIndex _sceneryType{};

    public:
        SmallSceneryRemoveAction() = default;
        SmallSceneryRemoveAction(const CoordsXYZ& location, uint8_t quadrant, ObjectEntryIndex sceneryType);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState, Park::ParkData& park) const override;
        Result Execute(GameState_t& gameState, Park::ParkData& park) const override;

    private:
        TileElement* FindSceneryElement() const;
    };
} // namespace OpenRCT2::GameActions
