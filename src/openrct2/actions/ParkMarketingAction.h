/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

class ParkMarketingAction final : public GameActionBase<GameCommand::StartMarketingCampaign>
{
private:
    int32_t _type{};
    int32_t _item{};
    int32_t _numWeeks{};

public:
    ParkMarketingAction() = default;
    ParkMarketingAction(int32_t type, int32_t item, int32_t numWeeks);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;

private:
    OpenRCT2::GameActions::Result CreateResult() const;
    money64 CalculatePrice() const;
};
