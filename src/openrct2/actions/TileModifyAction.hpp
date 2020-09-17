/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/TileInspector.h"
#include "GameAction.h"

enum class TileModifyType : uint8_t
{
    AnyRemove,
    AnySwap,
    AnyInsertCorrupt,
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
    EntranceMakeUsable,
    WallSetSlope,
    WallSetAnimationFrame,
    TrackBaseHeightOffset,
    TrackSetChain,
    TrackSetChainBlock,
    TrackSetBlockBrake,
    TrackSetIndestructible,
    TrackSetSpeed,
    ScenerySetQuarterLocation,
    ScenerySetQuarterCollision,
    BannerToggleBlockingEdge,
    CorruptClamp,
    Count,
};

DEFINE_GAME_ACTION(TileModifyAction, GAME_COMMAND_MODIFY_TILE, GameActionResult)
{
private:
    CoordsXY _loc;
    uint8_t _setting{ 0 };
    uint32_t _value1{ 0 };
    uint32_t _value2{ 0 };
    TileElement _pasteElement{};

public:
    TileModifyAction()
    {
    }
    TileModifyAction(
        CoordsXY loc, TileModifyType setting, uint32_t value1 = 0, uint32_t value2 = 0, TileElement pasteElement = {})
        : _loc(loc)
        , _setting(static_cast<uint8_t>(setting))
        , _value1(value1)
        , _value2(value2)
        , _pasteElement(pasteElement)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_setting) << DS_TAG(_value1) << DS_TAG(_value2) << DS_TAG(_pasteElement);
    }

    GameActionResult::Ptr Query() const override
    {
        return QueryExecute(false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return QueryExecute(true);
    }

private:
    GameActionResult::Ptr QueryExecute(bool isExecuting) const
    {
        if (!LocationValid(_loc))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_LAND_NOT_OWNED_BY_PARK);
        }
        auto res = MakeResult();

        switch (static_cast<TileModifyType>(_setting))
        {
            case TileModifyType::AnyRemove:
            {
                uint32_t elementIndex = _value1;
                res = tile_inspector_remove_element_at(_loc, elementIndex, isExecuting);
                break;
            }
            case TileModifyType::AnySwap:
            {
                const auto firstIndex = _value1;
                const auto secondIndex = _value2;
                res = tile_inspector_swap_elements_at(_loc, firstIndex, secondIndex, isExecuting);
                break;
            }
            case TileModifyType::AnyInsertCorrupt:
            {
                uint32_t elementIndex = _value1;
                res = tile_inspector_insert_corrupt_at(_loc, elementIndex, isExecuting);
                break;
            }
            case TileModifyType::AnyRotate:
            {
                uint32_t elementIndex = _value1;
                res = tile_inspector_rotate_element_at(_loc, elementIndex, isExecuting);
                break;
            }
            case TileModifyType::AnyPaste:
            {
                res = tile_inspector_paste_element_at(_loc, _pasteElement, isExecuting);
                break;
            }
            case TileModifyType::AnySort:
            {
                res = tile_inspector_sort_elements_at(_loc, isExecuting);
                break;
            }
            case TileModifyType::AnyBaseHeightOffset:
            {
                uint32_t elementIndex = _value1;
                const auto heightOffset = _value2;
                res = tile_inspector_any_base_height_offset(_loc, elementIndex, heightOffset, isExecuting);
                break;
            }
            case TileModifyType::SurfaceShowParkFences:
            {
                const bool showFences = _value1;
                res = tile_inspector_surface_show_park_fences(_loc, showFences, isExecuting);
                break;
            }
            case TileModifyType::SurfaceToggleCorner:
            {
                const auto cornerIndex = _value1;
                res = tile_inspector_surface_toggle_corner(_loc, cornerIndex, isExecuting);
                break;
            }
            case TileModifyType::SurfaceToggleDiagonal:
            {
                res = tile_inspector_surface_toggle_diagonal(_loc, isExecuting);
                break;
            }
            case TileModifyType::PathSetSlope:
            {
                uint32_t elementIndex = _value1;
                const bool sloped = _value2;
                res = tile_inspector_path_set_sloped(_loc, elementIndex, sloped, isExecuting);
                break;
            }
            case TileModifyType::PathSetBroken:
            {
                uint32_t elementIndex = _value1;
                const bool broken = _value2;
                res = tile_inspector_path_set_broken(_loc, elementIndex, broken, isExecuting);
                break;
            }
            case TileModifyType::PathToggleEdge:
            {
                uint32_t elementIndex = _value1;
                const auto edgeIndex = _value2;
                res = tile_inspector_path_toggle_edge(_loc, elementIndex, edgeIndex, isExecuting);
                break;
            }
            case TileModifyType::EntranceMakeUsable:
            {
                uint32_t elementIndex = _value1;
                res = tile_inspector_entrance_make_usable(_loc, elementIndex, isExecuting);
                break;
            }
            case TileModifyType::WallSetSlope:
            {
                uint32_t elementIndex = _value1;
                const auto slopeValue = _value2;
                res = tile_inspector_wall_set_slope(_loc, elementIndex, slopeValue, isExecuting);
                break;
            }
            case TileModifyType::WallSetAnimationFrame:
            {
                uint32_t elementIndex = _value1;
                const auto animationFrameOffset = _value2;
                res = tile_inspector_wall_animation_frame_offset(_loc, elementIndex, animationFrameOffset, isExecuting);
                break;
            }
            case TileModifyType::TrackBaseHeightOffset:
            {
                uint32_t elementIndex = _value1;
                const auto heightOffset = _value2;
                res = tile_inspector_track_base_height_offset(_loc, elementIndex, heightOffset, isExecuting);
                break;
            }
            case TileModifyType::TrackSetChainBlock:
            {
                uint32_t elementIndex = _value1;
                const bool setChain = _value2;
                res = tile_inspector_track_set_chain(_loc, elementIndex, true, setChain, isExecuting);
                break;
            }
            case TileModifyType::TrackSetChain:
            {
                uint32_t elementIndex = _value1;
                const bool setChain = _value2;
                res = tile_inspector_track_set_chain(_loc, elementIndex, false, setChain, isExecuting);
                break;
            }
            case TileModifyType::TrackSetSpeed:
            {
                uint32_t elementIndex = _value1;
                uint8_t speed = 0xFF;
                if (_value2 <= 0xFF)
                    speed = static_cast<uint8_t>(_value2);
                res = tile_inspector_track_set_speed(_loc, elementIndex, speed, isExecuting);
                break;
            }

            case TileModifyType::TrackSetBlockBrake:
            {
                uint32_t elementIndex = _value1;
                const bool blockBrake = _value2;
                res = tile_inspector_track_set_block_brake(_loc, elementIndex, blockBrake, isExecuting);
                break;
            }
            case TileModifyType::TrackSetIndestructible:
            {
                uint32_t elementIndex = _value1;
                const bool isIndestructible = _value2;
                res = tile_inspector_track_set_indestructible(_loc, elementIndex, isIndestructible, isExecuting);
                break;
            }
            case TileModifyType::ScenerySetQuarterLocation:
            {
                uint32_t elementIndex = _value1;
                const auto quarterIndex = _value2;
                res = tile_inspector_scenery_set_quarter_location(_loc, elementIndex, quarterIndex, isExecuting);
                break;
            }
            case TileModifyType::ScenerySetQuarterCollision:
            {
                uint32_t elementIndex = _value1;
                const auto quarterIndex = _value2;
                res = tile_inspector_scenery_set_quarter_collision(_loc, elementIndex, quarterIndex, isExecuting);
                break;
            }
            case TileModifyType::BannerToggleBlockingEdge:
            {
                uint32_t elementIndex = _value1;
                const auto edgeIndex = _value2;
                res = tile_inspector_banner_toggle_blocking_edge(_loc, elementIndex, edgeIndex, isExecuting);
                break;
            }
            case TileModifyType::CorruptClamp:
            {
                uint32_t elementIndex = _value1;
                res = tile_inspector_corrupt_clamp(_loc, elementIndex, isExecuting);
                break;
            }
            default:
                log_error("invalid instruction");
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
                break;
        }

        res->Position.x = _loc.x;
        res->Position.y = _loc.y;
        res->Position.z = tile_element_height(_loc);

        return res;
    }
};
