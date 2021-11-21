/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Map.h"
#include "GameAction.h"

class ChangeMapSizeAction final : public GameActionBase<GameCommand::ChangeMapSize>
{
public:
    ChangeMapSizeAction() = default;
    ChangeMapSizeAction(const int32_t targetSize);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;
    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    int32_t _targetSize;
};
