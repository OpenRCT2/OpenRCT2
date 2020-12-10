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

DEFINE_GAME_ACTION(SurfaceSetStyleAction, GAME_COMMAND_CHANGE_SURFACE_STYLE, GameActions::Result)
{
private:
    MapRange _range;
    ObjectEntryIndex _surfaceStyle{};
    ObjectEntryIndex _edgeStyle{};

public:
    SurfaceSetStyleAction() = default;

    SurfaceSetStyleAction(MapRange range, ObjectEntryIndex surfaceStyle, ObjectEntryIndex edgeStyle)
        : _range(range)
        , _surfaceStyle(surfaceStyle)
        , _edgeStyle(edgeStyle)
    {
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
