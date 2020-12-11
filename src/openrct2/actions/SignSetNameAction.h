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

DEFINE_GAME_ACTION(SignSetNameAction, GAME_COMMAND_SET_SIGN_NAME, GameActions::Result)
{
private:
    BannerIndex _bannerIndex{ BANNER_INDEX_NULL };
    std::string _name;

public:
    SignSetNameAction() = default;
    SignSetNameAction(BannerIndex bannerIndex, const std::string& name);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
