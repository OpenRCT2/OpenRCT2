/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

enum class TileModifyType : uint8_t
{
    AnyRemove,
    AnySwap,
    AnyToggleInvisilibity,
    AnyRotate,
    AnyPaste,
    AnySort,
    AnyBaseHeightOffset,
    SurfaceShowParkFences,
    SurfaceToggleCorner,
    SurfaceToggleDiagonal,
    PathSetSlope,
    PathSetBroken,
    PathToggleEdge,
    PathSetJunctionRailings,
    EntranceMakeUsable,
    WallSetSlope,
    WallSetAnimationFrame,
    TrackBaseHeightOffset,
    TrackSetChain,
    TrackSetChainBlock,
    TrackSetBrake,
    TrackSetIndestructible,
    ScenerySetQuarterLocation,
    ScenerySetQuarterCollision,
    BannerToggleBlockingEdge,
    WallSetAnimationIsBackwards,
    Count,
};

class TileModifyAction final : public GameActionBase<GameCommand::ModifyTile>
{
private:
    CoordsXY _loc;
    TileModifyType _setting{};
    uint32_t _value1{};
    uint32_t _value2{};
    TileElement _pasteElement{};
    Banner _pasteBanner{};

public:
    TileModifyAction() = default;
    TileModifyAction(
        CoordsXY loc, TileModifyType setting, uint32_t value1 = 0, uint32_t value2 = 0, TileElement pasteElement = {},
        Banner _pasteBanner = {});

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;

private:
    OpenRCT2::GameActions::Result QueryExecute(bool isExecuting) const;
};
