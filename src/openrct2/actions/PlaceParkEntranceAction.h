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

class PlaceParkEntranceAction final : public GameActionBase<GameCommand::PlaceParkEntrance>
{
private:
    CoordsXYZD _loc;
    ObjectEntryIndex _pathType;

public:
    PlaceParkEntranceAction() = default;
    PlaceParkEntranceAction(const CoordsXYZD& location, ObjectEntryIndex pathType);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    bool CheckMapCapacity(int16_t numTiles) const;
};
