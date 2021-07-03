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

DEFINE_GAME_ACTION(SmallScenerySetColourAction, GameCommand::SetSceneryColour, GameActions::Result)
{
private:
    CoordsXYZ _loc;
    uint8_t _quadrant{};
    ObjectEntryIndex _sceneryType{};
    uint8_t _primaryColour{};
    uint8_t _secondaryColour{};

public:
    SmallScenerySetColourAction() = default;
    SmallScenerySetColourAction(
        const CoordsXYZ& loc, uint8_t quadrant, ObjectEntryIndex sceneryType, uint8_t primaryColour, uint8_t secondaryColour);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    GameActions::Result::Ptr QueryExecute(bool isExecuting) const;
};
