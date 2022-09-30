/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

class PlacePeepSpawnAction final : public GameActionBase<GameCommand::PlacePeepSpawn>
{
private:
    CoordsXYZD _location;

public:
    PlacePeepSpawnAction() = default;
    PlacePeepSpawnAction(const CoordsXYZD& location);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;
};
