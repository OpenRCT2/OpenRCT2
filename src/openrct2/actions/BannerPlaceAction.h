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

class BannerPlaceActionResult final : public GameActions::Result
{
public:
    BannerPlaceActionResult();
    BannerPlaceActionResult(GameActions::Status err);
    BannerPlaceActionResult(GameActions::Status err, rct_string_id msg);
    BannerPlaceActionResult(GameActions::Status err, rct_string_id title, rct_string_id message);

    BannerIndex bannerId = BANNER_INDEX_NULL;
};

DEFINE_GAME_ACTION(BannerPlaceAction, GameCommand::PlaceBanner, BannerPlaceActionResult)
{
private:
    CoordsXYZD _loc;
    ObjectEntryIndex _bannerType{ BANNER_NULL };
    uint8_t _primaryColour{};

public:
    BannerPlaceAction() = default;
    BannerPlaceAction(const CoordsXYZD& loc, ObjectEntryIndex bannerType, uint8_t primaryColour);

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    PathElement* GetValidPathElement() const;
};
