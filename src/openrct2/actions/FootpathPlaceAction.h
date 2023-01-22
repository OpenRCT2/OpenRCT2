/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Footpath.h"
#include "GameAction.h"
namespace OpenRCT2
{
    class FootpathPlaceAction final : public GameActionBase<GameCommand::PlacePath>
    {
    private:
        CoordsXYZ _loc;
        uint8_t _slope{};
        ObjectEntryIndex _type{};
        ObjectEntryIndex _railingsType{};
        Direction _direction{ INVALID_DIRECTION };
        PathConstructFlags _constructFlags{};

    public:
        FootpathPlaceAction() = default;
        FootpathPlaceAction(
            const CoordsXYZ& loc, uint8_t slope, ObjectEntryIndex type, ObjectEntryIndex railingsType,
            Direction direction = INVALID_DIRECTION, PathConstructFlags constructFlags = 0);
        void AcceptParameters(GameActionParameterVisitor& visitor) override;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        GameActions::Result Query() const override;
        GameActions::Result Execute() const override;

    private:
        GameActions::Result ElementUpdateQuery(PathElement* pathElement, GameActions::Result res) const;
        GameActions::Result ElementUpdateExecute(PathElement* pathElement, GameActions::Result res) const;
        GameActions::Result ElementInsertQuery(GameActions::Result res) const;
        GameActions::Result ElementInsertExecute(GameActions::Result res) const;
        void AutomaticallySetPeepSpawn() const;
        void RemoveIntersectingWalls(PathElement* pathElement) const;
        PathElement* MapGetFootpathElementSlope(const CoordsXYZ& footpathPos, int32_t slope) const;
        bool IsSameAsPathElement(const PathElement* pathElement) const;
        bool IsSameAsEntranceElement(const EntranceElement& entranceElement) const;
    };
} // namespace OpenRCT2
