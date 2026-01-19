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

enum class MapSelectType : uint8_t;

namespace OpenRCT2::GameActions
{
    class LandSmoothAction final : public GameActionBase<GameCommand::EditLandSmooth>
    {
    private:
        CoordsXY _coords;
        MapRange _range;
        MapSelectType _selectionType{};
        bool _isLowering{};

        constexpr static StringId kErrorTitles[] = {
            STR_CANT_LOWER_LAND_HERE,
            STR_CANT_RAISE_LAND_HERE,
        };

    public:
        LandSmoothAction() = default;
        LandSmoothAction(const CoordsXY& coords, MapRange range, MapSelectType selectionType, bool isLowering);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        Result SmoothLandTile(
            GameState_t& gameState, int32_t direction, bool isExecuting, const CoordsXY& loc,
            SurfaceElement* surfaceElement) const;
        money64 SmoothLandRowByEdge(
            GameState_t& gameState, bool isExecuting, const CoordsXY& loc, int32_t expectedLandHeight1,
            int32_t expectedLandHeight2, int32_t stepX, int32_t stepY, int32_t direction1, int32_t direction2,
            int32_t checkDirection1, int32_t checkDirection2) const;
        money64 SmoothLandRowByCorner(
            GameState_t& gameState, bool isExecuting, const CoordsXY& loc, int32_t expectedLandHeight, int32_t stepX,
            int32_t stepY, int32_t direction, int32_t checkDirection) const;
        Result SmoothLand(GameState_t& gameState, bool isExecuting) const;
    };
} // namespace OpenRCT2::GameActions
