/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Climate.h"
#include "GameAction.h"
namespace OpenRCT2
{
    class ClimateSetAction final : public GameActionBase<GameCommand::SetClimate>
    {
    private:
        ClimateType _climate{};

    public:
        ClimateSetAction() = default;
        ClimateSetAction(ClimateType climate);

        void AcceptParameters(GameActionParameterVisitor& visitor) override;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        GameActions::Result Query() const override;
        GameActions::Result Execute() const override;
    };
} // namespace OpenRCT2
