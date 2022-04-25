/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/TileElement.h"
#include "GameAction.h"

class SmallSceneryRemoveAction final : public GameActionBase<GameCommand::RemoveScenery>
{
private:
    CoordsXYZ _loc;
    uint8_t _quadrant{};
    ObjectEntryIndex _sceneryType{};

public:
    SmallSceneryRemoveAction() = default;
    SmallSceneryRemoveAction(const CoordsXYZ& location, uint8_t quadrant, ObjectEntryIndex sceneryType);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    TileElement* FindSceneryElement() const;
};
