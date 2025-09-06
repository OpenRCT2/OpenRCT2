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

namespace OpenRCT2::GameActions
{
    class FootpathAdditionRemoveAction final : public GameActionBase<GameCommand::RemoveFootpathAddition>
    {
    private:
        CoordsXYZ _loc;

    public:
        FootpathAdditionRemoveAction() = default;
        FootpathAdditionRemoveAction(const CoordsXYZ& loc);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query() const override;
        Result Execute() const override;
    };
} // namespace OpenRCT2::GameActions
