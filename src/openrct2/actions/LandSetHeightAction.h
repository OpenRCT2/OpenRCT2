/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

class LandSetHeightAction final : public GameActionBase<GameCommand::SetLandHeight>
{
private:
    CoordsXY _coords;
    uint8_t _height{};
    uint8_t _style{};

public:
    LandSetHeightAction() = default;
    LandSetHeightAction(const CoordsXY& coords, uint8_t height, uint8_t style);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    StringId CheckParameters() const;
    TileElement* CheckTreeObstructions() const;
    money32 GetSmallSceneryRemovalCost() const;
    void SmallSceneryRemoval() const;
    StringId CheckRideSupports() const;
    TileElement* CheckFloatingStructures(TileElement* surfaceElement, uint8_t zCorner) const;
    money32 GetSurfaceHeightChangeCost(SurfaceElement* surfaceElement) const;
    void SetSurfaceHeight(TileElement* surfaceElement) const;

    /**
     *
     *  rct2: 0x00663CB9
     */
    static int32_t map_set_land_height_clear_func(
        TileElement** tile_element, [[maybe_unused]] const CoordsXY& coords, [[maybe_unused]] uint8_t flags,
        [[maybe_unused]] money32* price);
};
