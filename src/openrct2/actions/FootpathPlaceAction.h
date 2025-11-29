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
        FootpathSlope _slope{};
        ObjectEntryIndex _type{};
        ObjectEntryIndex _railingsType{};
        Direction _direction{ kInvalidDirection };
        PathConstructFlags _constructFlags{};

    public:
        FootpathPlaceAction() = default;
        FootpathPlaceAction(
            const CoordsXYZ& loc, FootpathSlope slope, ObjectEntryIndex type, ObjectEntryIndex railingsType,
            Direction direction = kInvalidDirection, PathConstructFlags constructFlags = 0);
        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        Result ElementUpdateQuery(PathElement* pathElement, Result res) const;
        Result ElementUpdateExecute(PathElement* pathElement, Result res) const;
        Result ElementInsertQuery(Result res) const;
        Result ElementInsertExecute(Result res) const;
        void AutomaticallySetPeepSpawn() const;
        void RemoveIntersectingWalls(PathElement* pathElement) const;
        PathElement* MapGetFootpathElementWithSlope(const CoordsXYZ& footpathPos, FootpathSlope slope) const;
        bool IsSameAsPathElement(const PathElement* pathElement) const;
        bool IsSameAsEntranceElement(const EntranceElement& entranceElement) const;
        bool landFitsUnderPath(const SurfaceElement& surfaceElement, int32_t zLow, int32_t zHigh) const;
        void autoshapeLand(Result& res, SurfaceElement& surfaceElement, int32_t zLow, int32_t zHigh) const;
    };
} // namespace OpenRCT2::GameActions
