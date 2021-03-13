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

DEFINE_GAME_ACTION(FootpathPlaceFromTrackAction, GameCommand::PlacePathFromTrack, GameActions::Result)
{
private:
    CoordsXYZ _loc;
    uint8_t _slope{};
    ObjectEntryIndex _type{};
    uint8_t _edges{};

public:
    FootpathPlaceFromTrackAction() = default;
    FootpathPlaceFromTrackAction(const CoordsXYZ& loc, uint8_t slope, ObjectEntryIndex type, uint8_t edges);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;

private:
    GameActions::Result::Ptr ElementInsertQuery(GameActions::Result::Ptr res) const;
    GameActions::Result::Ptr ElementInsertExecute(GameActions::Result::Ptr res) const;
};
