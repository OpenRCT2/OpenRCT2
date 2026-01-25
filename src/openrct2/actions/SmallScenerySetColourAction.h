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
    class SmallScenerySetColourAction final : public GameActionBase<GameCommand::SetSceneryColour>
    {
    private:
        CoordsXYZ _loc;
        uint8_t _quadrant{};
        ObjectEntryIndex _sceneryType{};
        Drawing::Colour _primaryColour{};
        Drawing::Colour _secondaryColour{};
        Drawing::Colour _tertiaryColour{};

    public:
        SmallScenerySetColourAction() = default;
        SmallScenerySetColourAction(
            const CoordsXYZ& loc, uint8_t quadrant, ObjectEntryIndex sceneryType, Drawing::Colour primaryColour,
            Drawing::Colour secondaryColour, Drawing::Colour tertiaryColour);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        Result QueryExecute(GameState_t& gameState, bool isExecuting) const;
    };
} // namespace OpenRCT2::GameActions
