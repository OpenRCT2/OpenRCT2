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

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
}

namespace OpenRCT2::GameActions
{
    class BannerSetColourAction final : public GameActionBase<GameCommand::SetBannerColour>
    {
    private:
        CoordsXYZD _loc;
        Drawing::Colour _primaryColour{};

    public:
        BannerSetColourAction() = default;
        BannerSetColourAction(const CoordsXYZD& loc, Drawing::Colour primaryColour);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        Result QueryExecute(bool isExecuting) const;
    };
} // namespace OpenRCT2::GameActions
