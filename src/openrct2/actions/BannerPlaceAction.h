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
    struct BannerPlaceActionResult
    {
        BannerIndex bannerId = BannerIndex::GetNull();
    };

    class BannerPlaceAction final : public GameActionBase<GameCommand::PlaceBanner>
    {
    private:
        CoordsXYZD _loc;
        ObjectEntryIndex _bannerType{ kBannerNull };
        Drawing::Colour _primaryColour{};

    public:
        BannerPlaceAction() = default;
        BannerPlaceAction(const CoordsXYZD& loc, ObjectEntryIndex bannerType, Drawing::Colour primaryColour);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        PathElement* GetValidPathElement() const;
    };
} // namespace OpenRCT2::GameActions
