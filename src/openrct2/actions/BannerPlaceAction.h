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

DEFINE_GAME_ACTION(BannerPlaceAction, GAME_COMMAND_PLACE_BANNER, GameActions::Result)
{
private:
    CoordsXYZD _loc;
    ObjectEntryIndex _bannerType{ BANNER_NULL };
    BannerIndex _bannerIndex{ BANNER_INDEX_NULL };
    uint8_t _primaryColour{};

public:
    BannerPlaceAction() = default;
    BannerPlaceAction(const CoordsXYZD& loc, uint8_t bannerType, BannerIndex bannerIndex, uint8_t primaryColour);

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    PathElement* GetValidPathElement() const;
};
