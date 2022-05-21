/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

struct BannerPlaceActionResult
{
    BannerIndex bannerId = BannerIndex::GetNull();
};

class BannerPlaceAction final : public GameActionBase<GameCommand::PlaceBanner>
{
private:
    CoordsXYZD _loc;
    ObjectEntryIndex _bannerType{ BANNER_NULL };
    uint8_t _primaryColour{};

public:
    BannerPlaceAction() = default;
    BannerPlaceAction(const CoordsXYZD& loc, ObjectEntryIndex bannerType, colour_t primaryColour);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    PathElement* GetValidPathElement() const;
};
