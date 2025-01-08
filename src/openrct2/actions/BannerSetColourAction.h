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

class BannerSetColourAction final : public GameActionBase<GameCommand::SetBannerColour>
{
private:
    CoordsXYZD _loc;
    uint8_t _primaryColour{};

public:
    BannerSetColourAction() = default;
    BannerSetColourAction(const CoordsXYZD& loc, uint8_t primaryColour);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;

private:
    OpenRCT2::GameActions::Result QueryExecute(bool isExecuting) const;
};
