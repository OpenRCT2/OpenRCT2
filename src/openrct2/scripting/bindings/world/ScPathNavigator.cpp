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

    JSValue ScPathNavigator::FromElement(JSContext* ctx, const CoordsXY& position, int32_t elementIndex)
    {
        auto el = MapGetNthElementAt(position, elementIndex);
        if (el == nullptr)
            return JS_NULL;

        auto* pathEl = el->AsPath();
        if (pathEl == nullptr)
            return JS_NULL;

        TileCoordsXYZ tilePos(TileCoordsXY(position), el->BaseHeight);
        return gScPathNavigator.New(ctx, tilePos, elementIndex, -1);
    }

    JSValue ScPathNavigator::FromPosition(JSContext* ctx, const CoordsXYZ& position)
    {
        TileCoordsXYZ tilePos(position);
        auto coords = tilePos.ToCoordsXY();
        TileElement* tileElement = MapGetFirstElementAt(coords);
        if (tileElement == nullptr)
            return JS_NULL;

        int32_t index = 0;
        do
        {
            if (tileElement->GetType() != TileElementType::Path || tileElement->BaseHeight != tilePos.z
                || tileElement->IsGhost())
            {
                index++;
                continue;
            }
            return gScPathNavigator.New(ctx, tilePos, index, -1);
        } while (!(tileElement++)->IsLastForTile());

        return JS_NULL;
    }

    void ScPathNavigator::Register(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("current", ScPathNavigator::current_get, nullptr),
            JS_CGETSET_DEF("edges", ScPathNavigator::edges_get, nullptr),
            JS_CGETSET_DEF("permittedEdges", ScPathNavigator::permittedEdges_get, nullptr),
            JS_CFUNC_DEF("getConnectedPaths", 0, ScPathNavigator::getConnectedPaths),
            JS_CFUNC_DEF("moveTo", 1, ScPathNavigator::moveTo),
        };
        RegisterBase(ctx, "PathNavigator", Finalize, funcs);
    }

    JSValue ScPathNavigator::New(JSContext* ctx, const TileCoordsXYZ& position, int32_t elementIndex, int32_t lastDirection)
    {
        return MakeWithOpaque(ctx, new PathNavigatorData{ position, elementIndex, lastDirection });
    }

    void ScPathNavigator::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        PathNavigatorData* data = GetPathNavigatorData(thisVal);
        if (data)
            delete data;
    }

    ScPathNavigator::PathNavigatorData* ScPathNavigator::GetPathNavigatorData(JSValue thisVal)
    {
        return gScPathNavigator.GetOpaque<PathNavigatorData*>(thisVal);
    }

    const PathElement* ScPathNavigator::FindPathElement(const PathNavigatorData* data)
    {
        auto coords = data->_position.ToCoordsXY();
        auto* el = MapGetNthElementAt(coords, data->_elementIndex);
        if (el != nullptr && el->GetType() == TileElementType::Path && el->BaseHeight == data->_position.z && !el->IsGhost())
        {
            return el->AsPath();
        }

        // Fall back to scanning in case the tile was re-ordered.
        TileElement* scan = MapGetFirstElementAt(coords);
        if (scan == nullptr)
            return nullptr;
        do
        {
            if (scan->GetType() != TileElementType::Path)
                continue;
            if (scan->BaseHeight != data->_position.z)
                continue;
            if (scan->IsGhost())
                continue;
            return scan->AsPath();
        } while (!(scan++)->IsLastForTile());

        return nullptr;
    }

    static const TileElement* GetBannerOnPath(const TileElement* pathElement)
    {
        if (pathElement->IsLastForTile())
            return nullptr;

        const TileElement* bannerElement = pathElement + 1;
        do
        {
            if (bannerElement->GetType() == TileElementType::Path)
                return nullptr;
            if (bannerElement->GetType() == TileElementType::Banner)
                return bannerElement;
            if (bannerElement->IsLastForTile())
                return nullptr;
        } while (bannerElement++ != nullptr);

        return nullptr;
    }

    int32_t ScPathNavigator::GetPermittedEdges(const PathElement* pathElement)
    {
        int32_t edges = pathElement->GetEdgesAndCorners() & 0x0F;
        const TileElement* bannerElement = GetBannerOnPath(reinterpret_cast<const TileElement*>(pathElement));
        while (bannerElement != nullptr)
        {
            edges &= bannerElement->AsBanner()->GetAllowedEdges();
            bannerElement = GetBannerOnPath(bannerElement);
        }
        return edges;
    }

    JSValue ScPathNavigator::current_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        return gScPathConnection.New(ctx, data->_position, data->_elementIndex, data->_lastDirection);
    }

    JSValue ScPathNavigator::edges_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewInt32(ctx, pathEl->GetEdges());
    }

    JSValue ScPathNavigator::permittedEdges_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewInt32(ctx, GetPermittedEdges(pathEl));
    }

    JSValue ScPathNavigator::getConnectedPaths(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;

        auto* pathElement = FindPathElement(data);
        if (pathElement == nullptr)
            return JS_NULL;

        int32_t edges = GetPermittedEdges(pathElement);

        JSValue result = JS_NewArray(ctx);
        int32_t resultIndex = 0;

        for (int32_t direction = 0; direction < 4; direction++)
        {
            if (!(edges & (1 << direction)))
                continue;

            TileCoordsXYZ neighborPos = data->_position;

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
                if (nextEl->IsGhost() || nextEl->GetType() != TileElementType::Path)
                {
                    nextIndex++;
                    continue;
                }

                const auto* nextPath = nextEl->AsPath();
                if (!FootpathIsZAndDirectionValid(*nextPath, neighborPos.z, direction))
                {
                    nextIndex++;
                    continue;
                }

                TileCoordsXYZ actualPos(neighborPos.x, neighborPos.y, nextEl->BaseHeight);
                JSValue conn = gScPathConnection.New(ctx, actualPos, nextIndex, direction);
                JS_SetPropertyUint32(ctx, result, resultIndex++, conn);
                break;
            } while (!(nextEl++)->IsLastForTile());
        }

        return result;
    }

    JSValue ScPathNavigator::moveTo(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NewBool(ctx, false);

        JS_UNPACK_INT32(direction, ctx, argv[0]);
        if (direction < 0 || direction > 3)
            return JS_NewBool(ctx, false);

        auto* pathElement = FindPathElement(data);
        if (pathElement == nullptr)
            return JS_NewBool(ctx, false);

        int32_t edges = GetPermittedEdges(pathElement);
        if (!(edges & (1 << direction)))
            return JS_NewBool(ctx, false);

        TileCoordsXYZ neighborPos = data->_position;

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
            if (nextEl->IsGhost() || nextEl->GetType() != TileElementType::Path)
            {
                nextIndex++;
                continue;
            }

            const auto* nextPath = nextEl->AsPath();
            if (!FootpathIsZAndDirectionValid(*nextPath, neighborPos.z, direction))
            {
                nextIndex++;
                continue;
            }

            data->_position = TileCoordsXYZ(neighborPos.x, neighborPos.y, nextEl->BaseHeight);
            data->_elementIndex = nextIndex;
            data->_lastDirection = direction;
            return JS_NewBool(ctx, true);
        } while (!(nextEl++)->IsLastForTile());

        return JS_NewBool(ctx, false);
    }

} // namespace OpenRCT2::Scripting

#endif
