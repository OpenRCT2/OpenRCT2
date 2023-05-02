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

class LargeSceneryRemoveAction final : public GameActionBase<GameCommand::RemoveLargeScenery>
{
private:
    CoordsXYZD _loc;
    uint16_t _tileIndex{};

public:
    LargeSceneryRemoveAction() = default;
    LargeSceneryRemoveAction(const CoordsXYZD& location, uint16_t tileIndex);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    TileElement* FindLargeSceneryElement(const CoordsXYZ& pos, int32_t sequenceIndex) const;
};
