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

class SurfaceSetStyleAction final : public GameActionBase<GameCommand::ChangeSurfaceStyle>
{
private:
    MapRange _range;
    ObjectEntryIndex _surfaceStyle{};
    ObjectEntryIndex _edgeStyle{};

public:
    SurfaceSetStyleAction() = default;
    SurfaceSetStyleAction(MapRange range, ObjectEntryIndex surfaceStyle, ObjectEntryIndex edgeStyle);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;
};
