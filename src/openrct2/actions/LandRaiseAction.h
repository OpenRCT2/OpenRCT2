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

class LandRaiseAction final : public GameActionBase<GameCommand::RaiseLand>
{
private:
    CoordsXY _coords;
    MapRange _range;
    uint8_t _selectionType{};

public:
    LandRaiseAction() = default;
    LandRaiseAction(const CoordsXY& coords, MapRange range, uint8_t selectionType);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;

private:
    OpenRCT2::GameActions::Result QueryExecute(bool isExecuting) const;
};
