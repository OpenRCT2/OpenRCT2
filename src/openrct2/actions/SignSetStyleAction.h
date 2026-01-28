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
    class SignSetStyleAction final : public GameActionBase<GameCommand::SetSignStyle>
    {
    private:
        BannerIndex _bannerIndex{ BannerIndex::GetNull() };
        Drawing::Colour _mainColour{};
        Drawing::Colour _textColour{};
        bool _isLarge{};

    public:
        SignSetStyleAction() = default;
        SignSetStyleAction(BannerIndex bannerIndex, Drawing::Colour mainColour, Drawing::Colour textColour, bool isLarge);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
