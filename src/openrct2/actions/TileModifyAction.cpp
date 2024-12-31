/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TileModifyAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../windows/Intent.h"
#include "../world/TileInspector.h"

using namespace OpenRCT2;

TileModifyAction::TileModifyAction(
    CoordsXY loc, TileModifyType setting, uint32_t value1, uint32_t value2, TileElement pasteElement, Banner pasteBanner)
    : _loc(loc)
    , _setting(setting)
    , _value1(value1)
    , _value2(value2)
    , _pasteElement(pasteElement)
    , _pasteBanner(pasteBanner)
{
}

void TileModifyAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("setting", _setting);
    visitor.Visit("value1", _value1);
    visitor.Visit("value2", _value2);
}

uint16_t TileModifyAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void TileModifyAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_setting) << DS_TAG(_value1) << DS_TAG(_value2) << DS_TAG(_pasteElement)
           << DS_TAG(_pasteBanner);
}

GameActions::Result TileModifyAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result TileModifyAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result TileModifyAction::QueryExecute(bool isExecuting) const
{
    if (!LocationValid(_loc))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_CHANGE_THIS, STR_OFF_EDGE_OF_MAP);
    }
    auto res = GameActions::Result();
    switch (_setting)
    {
        case TileModifyType::AnyRemove:
        {
            const auto elementIndex = _value1;
            res = TileInspector::RemoveElementAt(_loc, elementIndex, isExecuting);
            break;
        }
        case TileModifyType::AnySwap:
        {
            const auto firstIndex = _value1;
            const auto secondIndex = _value2;
            res = TileInspector::SwapElementsAt(_loc, firstIndex, secondIndex, isExecuting);
            break;
        }
        case TileModifyType::AnyToggleInvisilibity:
        {
            const auto elementIndex = _value1;
            res = TileInspector::ToggleInvisibilityOfElementAt(_loc, elementIndex, isExecuting);
            break;
        }
        case TileModifyType::AnyRotate:
        {
            const auto elementIndex = _value1;
            res = TileInspector::RotateElementAt(_loc, elementIndex, isExecuting);
            break;
        }
        case TileModifyType::AnyPaste:
        {
            res = TileInspector::PasteElementAt(_loc, _pasteElement, _pasteBanner, isExecuting);
            break;
        }
        case TileModifyType::AnySort:
        {
            res = TileInspector::SortElementsAt(_loc, isExecuting);
            break;
        }
        case TileModifyType::AnyBaseHeightOffset:
        {
            const auto elementIndex = _value1;
            const auto heightOffset = _value2;
            res = TileInspector::AnyBaseHeightOffset(_loc, elementIndex, heightOffset, isExecuting);
            break;
        }
        case TileModifyType::SurfaceShowParkFences:
        {
            const bool showFences = _value1;
            res = TileInspector::SurfaceShowParkFences(_loc, showFences, isExecuting);
            break;
        }
        case TileModifyType::SurfaceToggleCorner:
        {
            const auto cornerIndex = _value1;
            res = TileInspector::SurfaceToggleCorner(_loc, cornerIndex, isExecuting);
            break;
        }
        case TileModifyType::SurfaceToggleDiagonal:
        {
            res = TileInspector::SurfaceToggleDiagonal(_loc, isExecuting);
            break;
        }
        case TileModifyType::PathSetSlope:
        {
            const auto elementIndex = _value1;
            const bool sloped = _value2;
            res = TileInspector::PathSetSloped(_loc, elementIndex, sloped, isExecuting);
            break;
        }
        case TileModifyType::PathSetJunctionRailings:
        {
            const auto elementIndex = _value1;
            const bool hasJunctionRailing = _value2;
            res = TileInspector::PathSetJunctionRailings(_loc, elementIndex, hasJunctionRailing, isExecuting);
            break;
        }
        case TileModifyType::PathSetBroken:
        {
            const auto elementIndex = _value1;
            const bool broken = _value2;
            res = TileInspector::PathSetBroken(_loc, elementIndex, broken, isExecuting);
            break;
        }
        case TileModifyType::PathToggleEdge:
        {
            const auto elementIndex = _value1;
            const auto edgeIndex = _value2;
            res = TileInspector::PathToggleEdge(_loc, elementIndex, edgeIndex, isExecuting);
            break;
        }
        case TileModifyType::EntranceMakeUsable:
        {
            const auto elementIndex = _value1;
            res = TileInspector::EntranceMakeUsable(_loc, elementIndex, isExecuting);
            break;
        }
        case TileModifyType::WallSetSlope:
        {
            const auto elementIndex = _value1;
            const auto slopeValue = _value2;
            res = TileInspector::WallSetSlope(_loc, elementIndex, slopeValue, isExecuting);
            break;
        }
        case TileModifyType::WallSetAnimationFrame:
        {
            const auto elementIndex = _value1;
            const auto animationFrameOffset = _value2;
            res = TileInspector::WallAnimationFrameOffset(_loc, elementIndex, animationFrameOffset, isExecuting);
            break;
        }
        case TileModifyType::TrackBaseHeightOffset:
        {
            const auto elementIndex = _value1;
            const auto heightOffset = _value2;
            res = TileInspector::TrackBaseHeightOffset(_loc, elementIndex, heightOffset, isExecuting);
            break;
        }
        case TileModifyType::TrackSetChainBlock:
        {
            const auto elementIndex = _value1;
            const bool setChain = _value2;
            res = TileInspector::TrackSetChain(_loc, elementIndex, true, setChain, isExecuting);
            break;
        }
        case TileModifyType::TrackSetChain:
        {
            const auto elementIndex = _value1;
            const bool setChain = _value2;
            res = TileInspector::TrackSetChain(_loc, elementIndex, false, setChain, isExecuting);
            break;
        }
        case TileModifyType::TrackSetBrake:
        {
            const auto elementIndex = _value1;
            const bool isClosed = _value2;
            res = TileInspector::TrackSetBrakeClosed(_loc, elementIndex, isClosed, isExecuting);
            break;
        }
        case TileModifyType::TrackSetIndestructible:
        {
            const auto elementIndex = _value1;
            const bool isIndestructible = _value2;
            res = TileInspector::TrackSetIndestructible(_loc, elementIndex, isIndestructible, isExecuting);
            break;
        }
        case TileModifyType::ScenerySetQuarterLocation:
        {
            const auto elementIndex = _value1;
            const auto quarterIndex = _value2;
            res = TileInspector::ScenerySetQuarterLocation(_loc, elementIndex, quarterIndex, isExecuting);
            break;
        }
        case TileModifyType::ScenerySetQuarterCollision:
        {
            const auto elementIndex = _value1;
            const auto quarterIndex = _value2;
            res = TileInspector::ScenerySetQuarterCollision(_loc, elementIndex, quarterIndex, isExecuting);
            break;
        }
        case TileModifyType::BannerToggleBlockingEdge:
        {
            const auto elementIndex = _value1;
            const auto edgeIndex = _value2;
            res = TileInspector::BannerToggleBlockingEdge(_loc, elementIndex, edgeIndex, isExecuting);
            break;
        }
        case TileModifyType::WallSetAnimationIsBackwards:
        {
            const auto elementIndex = _value1;
            const bool broken = _value2;
            res = TileInspector::WallSetAnimationIsBackwards(_loc, elementIndex, broken, isExecuting);
            break;
        }
        default:
            LOG_ERROR("Invalid tile modification type %u", _setting);
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    res.Position.x = _loc.x;
    res.Position.y = _loc.y;
    res.Position.z = TileElementHeight(_loc);

    if (isExecuting)
    {
        MapInvalidateTileFull(_loc);
        auto intent = Intent(INTENT_ACTION_TILE_MODIFY);
        ContextBroadcastIntent(&intent);
    }

    return res;
}
