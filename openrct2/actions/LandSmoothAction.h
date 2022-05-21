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

class LandSmoothAction final : public GameActionBase<GameCommand::EditLandSmooth>
{
private:
    CoordsXY _coords;
    MapRange _range;
    uint8_t _selectionType{};
    bool _isLowering{};

    constexpr static rct_string_id _ErrorTitles[] = {
        STR_CANT_LOWER_LAND_HERE,
        STR_CANT_RAISE_LAND_HERE,
    };

public:
    LandSmoothAction() = default;
    LandSmoothAction(const CoordsXY& coords, MapRange range, uint8_t selectionType, bool isLowering);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    GameActions::Result SmoothLandTile(
        int32_t direction, bool isExecuting, const CoordsXY& loc, SurfaceElement* surfaceElement) const;
    money32 SmoothLandRowByEdge(
        bool isExecuting, const CoordsXY& loc, int32_t expectedLandHeight1, int32_t expectedLandHeight2, int32_t stepX,
        int32_t stepY, int32_t direction1, int32_t direction2, int32_t checkDirection1, int32_t checkDirection2) const;
    money32 SmoothLandRowByCorner(
        bool isExecuting, const CoordsXY& loc, int32_t expectedLandHeight, int32_t stepX, int32_t stepY, int32_t direction,
        int32_t checkDirection) const;
    GameActions::Result SmoothLand(bool isExecuting) const;
};
