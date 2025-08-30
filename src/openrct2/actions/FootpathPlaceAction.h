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

namespace OpenRCT2::GameActions
{
    class FootpathPlaceAction final : public GameActionBase<GameCommand::PlacePath>
    {
    private:
        CoordsXYZ _loc;
        uint8_t _slope{};
        ObjectEntryIndex _type{};
        ObjectEntryIndex _railingsType{};
        Direction _direction{ kInvalidDirection };
        PathConstructFlags _constructFlags{};

    public:
        FootpathPlaceAction() = default;
        FootpathPlaceAction(
            const CoordsXYZ& loc, uint8_t slope, ObjectEntryIndex type, ObjectEntryIndex railingsType,
            Direction direction = kInvalidDirection, PathConstructFlags constructFlags = 0);
        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query() const override;
        Result Execute() const override;

    private:
        Result ElementUpdateQuery(PathElement* pathElement, Result res) const;
        Result ElementUpdateExecute(PathElement* pathElement, Result res) const;
        Result ElementInsertQuery(Result res) const;
        Result ElementInsertExecute(Result res) const;
        void AutomaticallySetPeepSpawn() const;
        void RemoveIntersectingWalls(PathElement* pathElement) const;
        PathElement* MapGetFootpathElementSlope(const CoordsXYZ& footpathPos, int32_t slope) const;
        bool IsSameAsPathElement(const PathElement* pathElement) const;
        bool IsSameAsEntranceElement(const EntranceElement& entranceElement) const;
    };
} // namespace OpenRCT2::GameActions
