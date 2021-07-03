/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../management/Finance.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(FootpathRemoveAction, GameCommand::RemovePath, GameActions::Result)
{
private:
    CoordsXYZ _loc;

public:
    FootpathRemoveAction() = default;
    FootpathRemoveAction(const CoordsXYZ& location);

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    TileElement* GetFootpathElement() const;
    money32 GetRefundPrice(TileElement * footpathElement) const;
    GameActions::Result::Ptr RemoveBannersAtElement(const CoordsXY& loc, TileElement* tileElement) const;
};
