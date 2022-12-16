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

class FootpathPlaceFromTrackAction final : public GameActionBase<GameCommand::PlacePathFromTrack>
{
private:
    CoordsXYZ _loc;
    uint8_t _slope{};
    ObjectEntryIndex _type{};
    ObjectEntryIndex _railingsType{};
    uint8_t _edges{};
    PathConstructFlags _constructFlags{};

public:
    FootpathPlaceFromTrackAction() = default;
    FootpathPlaceFromTrackAction(
        const CoordsXYZ& loc, uint8_t slope, ObjectEntryIndex type, ObjectEntryIndex railingsType, uint8_t edges,
        PathConstructFlags constructFlags = 0);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    GameActions::Result ElementInsertQuery(GameActions::Result res) const;
    GameActions::Result ElementInsertExecute(GameActions::Result res) const;
    bool IsSameAsEntranceElement(const EntranceElement& entranceElement) const;
};
