/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScPathNavigator.h"

    #include "../../../world/Footpath.h"
    #include "../../../world/Map.h"
    #include "../../../world/tile_element/BannerElement.h"
    #include "../../../world/tile_element/PathElement.h"
    #include "../../ScriptEngine.h"
    #include "ScPathConnection.h"

namespace OpenRCT2::Scripting
{

    JSValue ScPathNavigator::fromElement(
        JSContext* ctx, const CoordsXY& position, int32_t elementIndex, const PathNavigationOptions& options)
    {
        auto el = MapGetNthElementAt(position, elementIndex);
        if (el == nullptr)
            return JS_NULL;

        auto* pathEl = el->asPath();
        if (pathEl == nullptr)
            return JS_NULL;

        TileCoordsXYZ tilePos(TileCoordsXY(position), el->baseHeight);
        return gScPathNavigator.create(ctx, tilePos, elementIndex, -1, options);
    }

    JSValue ScPathNavigator::fromPosition(JSContext* ctx, const CoordsXYZ& position, const PathNavigationOptions& options)
    {
        TileCoordsXYZ tilePos(position);
        auto coords = tilePos.ToCoordsXY();
        TileElement* tileElement = MapGetFirstElementAt(coords);
        if (tileElement == nullptr)
            return JS_NULL;

        int32_t index = 0;
        do
        {
            if (tileElement->getType() != TileElementType::Path || tileElement->baseHeight != tilePos.z
                || (tileElement->isGhost() && !options.includeGhosts))
            {
                index++;
                continue;
            }
            return gScPathNavigator.create(ctx, tilePos, index, -1, options);
        } while (!(tileElement++)->isLastForTile());

        return JS_NULL;
    }

    void ScPathNavigator::registerClass(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry kFuncs[] = {
            JS_CGETSET_DEF("current", ScPathNavigator::getCurrent, nullptr),
            JS_CGETSET_DEF("edges", ScPathNavigator::getEdges, nullptr),
            JS_CGETSET_DEF("permittedEdges", ScPathNavigator::getPermittedEdges, nullptr),
            JS_CFUNC_DEF("getConnectedPaths", 0, ScPathNavigator::getConnectedPaths),
            JS_CFUNC_DEF("moveTo", 1, ScPathNavigator::moveTo),
        };
        RegisterBase(ctx, "PathNavigator", finalize, kFuncs);
    }

    JSValue ScPathNavigator::create(
        JSContext* ctx, const TileCoordsXYZ& position, int32_t elementIndex, int32_t lastDirection,
        const PathNavigationOptions& options)
    {
        return MakeWithOpaque(ctx, new PathNavigatorData{ position, elementIndex, lastDirection, options });
    }

    void ScPathNavigator::finalize(JSRuntime* rt, JSValue thisVal)
    {
        PathNavigatorData* data = getPathNavigatorData(thisVal);
        if (data)
            delete data;
    }

    ScPathNavigator::PathNavigatorData* ScPathNavigator::getPathNavigatorData(JSValue thisVal)
    {
        return gScPathNavigator.GetOpaque<PathNavigatorData*>(thisVal);
    }

    const PathElement* ScPathNavigator::findPathElement(const PathNavigatorData* data)
    {
        const bool includeGhost = data->options.includeGhosts;
        auto coords = data->position.ToCoordsXY();
        auto* el = MapGetNthElementAt(coords, data->elementIndex);
        if (el != nullptr && el->getType() == TileElementType::Path && el->baseHeight == data->position.z
            && (includeGhost || !el->isGhost()))
        {
            return el->asPath();
        }

        // Fall back to scanning in case the tile was re-ordered.
        TileElement* scan = MapGetFirstElementAt(coords);
        if (scan == nullptr)
            return nullptr;
        do
        {
            if (scan->getType() != TileElementType::Path)
                continue;
            if (scan->baseHeight != data->position.z)
                continue;
            if (scan->isGhost() && !includeGhost)
                continue;
            return scan->asPath();
        } while (!(scan++)->isLastForTile());

        return nullptr;
    }

    static const TileElement* getBannerOnPath(const TileElement* pathElement)
    {
        if (pathElement->isLastForTile())
            return nullptr;

        const TileElement* bannerElement = pathElement + 1;
        do
        {
            if (bannerElement->getType() == TileElementType::Path)
                return nullptr;
            if (bannerElement->getType() == TileElementType::Banner)
                return bannerElement;
            if (bannerElement->isLastForTile())
                return nullptr;
        } while (bannerElement++ != nullptr);

        return nullptr;
    }

    int32_t ScPathNavigator::computePermittedEdges(const PathElement* pathElement, const PathNavigationOptions& options)
    {
        int32_t edges = pathElement->GetEdgesAndCorners() & 0x0F;
        if (!options.respectBanners)
            return edges;

        const TileElement* bannerElement = getBannerOnPath(reinterpret_cast<const TileElement*>(pathElement));
        while (bannerElement != nullptr)
        {
            edges &= bannerElement->asBanner()->GetAllowedEdges();
            bannerElement = getBannerOnPath(bannerElement);
        }
        return edges;
    }

    bool ScPathNavigator::isTraversableNeighbor(const PathElement* pathElement, const PathNavigationOptions& options)
    {
        if (pathElement->IsQueue() && !options.includeQueues)
            return false;
        if (pathElement->IsWide() && !options.includeWidePaths)
            return false;
        return true;
    }

    JSValue ScPathNavigator::getCurrent(JSContext* ctx, JSValue thisVal)
    {
        auto* data = getPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        return gScPathConnection.create(ctx, data->position, data->elementIndex, data->lastDirection);
    }

    JSValue ScPathNavigator::getEdges(JSContext* ctx, JSValue thisVal)
    {
        auto* data = getPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = findPathElement(data);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewInt32(ctx, pathEl->GetEdges());
    }

    JSValue ScPathNavigator::getPermittedEdges(JSContext* ctx, JSValue thisVal)
    {
        auto* data = getPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = findPathElement(data);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewInt32(ctx, computePermittedEdges(pathEl, data->options));
    }

    JSValue ScPathNavigator::getConnectedPaths(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        auto* data = getPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;

        auto* pathElement = findPathElement(data);
        if (pathElement == nullptr)
            return JS_NULL;

        const auto& options = data->options;
        int32_t edges = computePermittedEdges(pathElement, options);

        JSValue result = JS_NewArray(ctx);
        int32_t resultIndex = 0;

        for (int32_t direction = 0; direction < 4; direction++)
        {
            if (!(edges & (1 << direction)))
                continue;

            TileCoordsXYZ neighborPos = data->position;

            if (pathElement->IsSloped() && pathElement->GetSlopeDirection() == direction)
            {
                neighborPos.z += 2;
            }

            neighborPos.x += TileDirectionDelta[direction].x;
            neighborPos.y += TileDirectionDelta[direction].y;

            auto neighborCoords = neighborPos.ToCoordsXY();
            TileElement* nextEl = MapGetFirstElementAt(neighborCoords);
            if (nextEl == nullptr)
                continue;

            int32_t nextIndex = 0;
            do
            {
                if ((nextEl->isGhost() && !options.includeGhosts) || nextEl->getType() != TileElementType::Path)
                {
                    nextIndex++;
                    continue;
                }

                const auto* nextPath = nextEl->asPath();
                if (!FootpathIsZAndDirectionValid(*nextPath, neighborPos.z, direction))
                {
                    nextIndex++;
                    continue;
                }

                if (!isTraversableNeighbor(nextPath, options))
                {
                    nextIndex++;
                    continue;
                }

                TileCoordsXYZ actualPos(neighborPos.x, neighborPos.y, nextEl->baseHeight);
                JSValue conn = gScPathConnection.create(ctx, actualPos, nextIndex, direction);
                JS_SetPropertyUint32(ctx, result, resultIndex++, conn);
                break;
            } while (!(nextEl++)->isLastForTile());
        }

        return result;
    }

    JSValue ScPathNavigator::moveTo(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        auto* data = getPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NewBool(ctx, false);

        JS_UNPACK_INT32(direction, ctx, argv[0]);
        if (direction < 0 || direction > 3)
            return JS_NewBool(ctx, false);

        auto* pathElement = findPathElement(data);
        if (pathElement == nullptr)
            return JS_NewBool(ctx, false);

        const auto& options = data->options;
        int32_t edges = computePermittedEdges(pathElement, options);
        if (!(edges & (1 << direction)))
            return JS_NewBool(ctx, false);

        TileCoordsXYZ neighborPos = data->position;

        if (pathElement->IsSloped() && pathElement->GetSlopeDirection() == direction)
        {
            neighborPos.z += 2;
        }

        neighborPos.x += TileDirectionDelta[direction].x;
        neighborPos.y += TileDirectionDelta[direction].y;

        auto neighborCoords = neighborPos.ToCoordsXY();
        TileElement* nextEl = MapGetFirstElementAt(neighborCoords);
        if (nextEl == nullptr)
            return JS_NewBool(ctx, false);

        int32_t nextIndex = 0;
        do
        {
            if ((nextEl->isGhost() && !options.includeGhosts) || nextEl->getType() != TileElementType::Path)
            {
                nextIndex++;
                continue;
            }

            const auto* nextPath = nextEl->asPath();
            if (!FootpathIsZAndDirectionValid(*nextPath, neighborPos.z, direction))
            {
                nextIndex++;
                continue;
            }

            if (!isTraversableNeighbor(nextPath, options))
            {
                nextIndex++;
                continue;
            }

            data->position = TileCoordsXYZ(neighborPos.x, neighborPos.y, nextEl->baseHeight);
            data->elementIndex = nextIndex;
            data->lastDirection = direction;
            return JS_NewBool(ctx, true);
        } while (!(nextEl++)->isLastForTile());

        return JS_NewBool(ctx, false);
    }

} // namespace OpenRCT2::Scripting

#endif
