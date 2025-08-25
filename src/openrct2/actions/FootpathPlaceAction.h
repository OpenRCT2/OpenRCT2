/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Footpath.h"
#include "GameAction.h"

class FootpathPlaceAction final : public GameActionBase<GameCommand::PlacePath>
{
private:
    CoordsXYZ _loc;
    uint8_t _slope{};
    OpenRCT2::ObjectEntryIndex _type{};
    OpenRCT2::ObjectEntryIndex _railingsType{};
    Direction _direction{ kInvalidDirection };
    PathConstructFlags _constructFlags{};

public:
    FootpathPlaceAction() = default;
    FootpathPlaceAction(
        const CoordsXYZ& loc, uint8_t slope, OpenRCT2::ObjectEntryIndex type, OpenRCT2::ObjectEntryIndex railingsType,
        Direction direction = kInvalidDirection, PathConstructFlags constructFlags = 0);
    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;

private:
    OpenRCT2::GameActions::Result ElementUpdateQuery(
        OpenRCT2::PathElement* pathElement, OpenRCT2::GameActions::Result res) const;
    OpenRCT2::GameActions::Result ElementUpdateExecute(
        OpenRCT2::PathElement* pathElement, OpenRCT2::GameActions::Result res) const;
    OpenRCT2::GameActions::Result ElementInsertQuery(OpenRCT2::GameActions::Result res) const;
    OpenRCT2::GameActions::Result ElementInsertExecute(OpenRCT2::GameActions::Result res) const;
    void AutomaticallySetPeepSpawn() const;
    void RemoveIntersectingWalls(OpenRCT2::PathElement* pathElement) const;
    OpenRCT2::PathElement* MapGetFootpathElementSlope(const CoordsXYZ& footpathPos, int32_t slope) const;
    bool IsSameAsPathElement(const OpenRCT2::PathElement* pathElement) const;
    bool IsSameAsEntranceElement(const OpenRCT2::EntranceElement& entranceElement) const;
};
