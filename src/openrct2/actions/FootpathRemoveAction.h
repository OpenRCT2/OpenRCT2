/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../management/Finance.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    class FootpathRemoveAction final : public GameActionBase<GameCommand::RemovePath>
    {
    private:
        CoordsXYZ _loc;

    public:
        FootpathRemoveAction() = default;
        FootpathRemoveAction(const CoordsXYZ& location);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query() const override;
        Result Execute() const override;

    private:
        TileElement* GetFootpathElement() const;
        money64 GetRefundPrice(TileElement* footpathElement) const;
        Result RemoveBannersAtElement(const CoordsXY& loc, TileElement* tileElement) const;
    };
} // namespace OpenRCT2::GameActions
