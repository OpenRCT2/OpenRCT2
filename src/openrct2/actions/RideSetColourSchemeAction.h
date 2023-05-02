/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

class RideSetColourSchemeAction final : public GameActionBase<GameCommand::SetColourScheme>
{
private:
    CoordsXYZD _loc;
    track_type_t _trackType{};
    uint16_t _newColourScheme{};

public:
    RideSetColourSchemeAction() = default;
    RideSetColourSchemeAction(const CoordsXYZD& location, track_type_t trackType, uint16_t newColourScheme);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;
};
