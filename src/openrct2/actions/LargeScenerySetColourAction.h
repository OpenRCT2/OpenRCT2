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

class LargeScenerySetColourAction final : public GameActionBase<GameCommand::SetLargeSceneryColour>
{
private:
    CoordsXYZD _loc;
    uint8_t _tileIndex{};
    uint8_t _primaryColour{};
    uint8_t _secondaryColour{};
    uint8_t _tertiaryColour{};

public:
    LargeScenerySetColourAction() = default;
    LargeScenerySetColourAction(
        const CoordsXYZD& loc, uint8_t tileIndex, uint8_t primaryColour, uint8_t secondaryColour, uint8_t tertiaryColour);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    GameActions::Result QueryExecute(bool isExecuting) const;
};
