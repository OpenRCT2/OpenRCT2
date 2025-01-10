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

class RideSetColourSchemeAction final : public GameActionBase<GameCommand::SetColourScheme>
{
private:
    CoordsXYZD _loc;
    OpenRCT2::TrackElemType _trackType{};
    uint16_t _newColourScheme{};

public:
    RideSetColourSchemeAction() = default;
    RideSetColourSchemeAction(const CoordsXYZD& location, OpenRCT2::TrackElemType trackType, uint16_t newColourScheme);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};
