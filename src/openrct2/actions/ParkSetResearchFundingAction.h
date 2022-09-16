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

class ParkSetResearchFundingAction final : public GameActionBase<GameCommand::SetResearchFunding>
{
private:
    // TODO change to std::optional when C++17
    uint32_t _priorities{};
    uint8_t _fundingAmount{};

public:
    ParkSetResearchFundingAction() = default;
    ParkSetResearchFundingAction(uint32_t priorities, uint8_t fundingAmount);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;
};
