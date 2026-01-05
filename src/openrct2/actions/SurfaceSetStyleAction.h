/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    class SurfaceSetStyleAction final : public GameActionBase<GameCommand::ChangeSurfaceStyle>
    {
    private:
        MapRange _range;
        ObjectEntryIndex _surfaceStyle{};
        ObjectEntryIndex _edgeStyle{};

    public:
        SurfaceSetStyleAction() = default;
        SurfaceSetStyleAction(MapRange range, ObjectEntryIndex surfaceStyle, ObjectEntryIndex edgeStyle);

        void AcceptParameters(GameActionParameterVisitor&) final;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
