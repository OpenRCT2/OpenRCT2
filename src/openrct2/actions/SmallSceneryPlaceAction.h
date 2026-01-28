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
    struct SmallSceneryPlaceActionResult
    {
        uint8_t GroundFlags{};
        int32_t BaseHeight{};
        uint8_t SceneryQuadrant{};
    };

    class SmallSceneryPlaceAction final : public GameActionBase<GameCommand::PlaceScenery>
    {
    private:
        CoordsXYZD _loc;
        uint8_t _quadrant{};
        ObjectEntryIndex _sceneryType{};
        Drawing::Colour _primaryColour{};
        Drawing::Colour _secondaryColour{};
        Drawing::Colour _tertiaryColour{};

    public:
        SmallSceneryPlaceAction() = default;
        SmallSceneryPlaceAction(
            const CoordsXYZD& loc, uint8_t quadrant, ObjectEntryIndex sceneryType, Drawing::Colour primaryColour,
            Drawing::Colour secondaryColour, Drawing::Colour tertiaryColour);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint32_t GetCooldownTime() const final;
        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
