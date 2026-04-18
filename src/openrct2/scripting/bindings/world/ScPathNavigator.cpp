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
        return gScPathNavigator.New(ctx, tilePos);
    }

    JSValue ScPathNavigator::FromPosition(JSContext* ctx, const CoordsXYZ& position)
    {
        TileCoordsXYZ tilePos(position);
        auto* pathEl = FindPathElement(tilePos);
        if (pathEl == nullptr)
            return JS_NULL;

        return gScPathNavigator.New(ctx, tilePos);
    }

    void ScPathNavigator::Register(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("position", ScPathNavigator::position_get, nullptr),
            JS_CGETSET_DEF("edges", ScPathNavigator::edges_get, nullptr),
            JS_CGETSET_DEF("permittedEdges", ScPathNavigator::permittedEdges_get, nullptr),
            JS_CGETSET_DEF("isSloped", ScPathNavigator::isSloped_get, nullptr),
            JS_CGETSET_DEF("slopeDirection", ScPathNavigator::slopeDirection_get, nullptr),
            JS_CGETSET_DEF("isQueue", ScPathNavigator::isQueue_get, nullptr),
            JS_CGETSET_DEF("isWide", ScPathNavigator::isWide_get, nullptr),
            JS_CGETSET_DEF("ride", ScPathNavigator::ride_get, nullptr),
            JS_CGETSET_DEF("station", ScPathNavigator::station_get, nullptr),
            JS_CFUNC_DEF("getConnectedPaths", 0, ScPathNavigator::getConnectedPaths),
            JS_CFUNC_DEF("moveTo", 1, ScPathNavigator::moveTo),
        };
        RegisterBase(ctx, "PathNavigator", Finalize, funcs);
    }

    JSValue ScPathNavigator::New(JSContext* ctx, const TileCoordsXYZ& position)
    {
        return MakeWithOpaque(ctx, new PathNavigatorData{ position });
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

    const PathElement* ScPathNavigator::FindPathElement(const TileCoordsXYZ& pos)
    {
        auto coords = pos.ToCoordsXY();
        TileElement* tileElement = MapGetFirstElementAt(coords);
        if (tileElement == nullptr)
            return nullptr;

        do
        {
            if (tileElement->GetType() != TileElementType::Path)
                continue;
            if (tileElement->BaseHeight != pos.z)
                continue;
            if (tileElement->IsGhost())
                continue;
            return tileElement->AsPath();
        } while (!(tileElement++)->IsLastForTile());

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

    JSValue ScPathNavigator::position_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        return ToJSValue(ctx, data->_position.ToCoordsXYZ());
    }

    JSValue ScPathNavigator::edges_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data->_position);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewInt32(ctx, pathEl->GetEdges());
    }

    JSValue ScPathNavigator::permittedEdges_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data->_position);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewInt32(ctx, GetPermittedEdges(pathEl));
    }

    JSValue ScPathNavigator::isSloped_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data->_position);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewBool(ctx, pathEl->IsSloped());
    }

    JSValue ScPathNavigator::slopeDirection_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data->_position);
        if (pathEl == nullptr)
            return JS_NULL;
        if (!pathEl->IsSloped())
            return JS_NULL;
        return JS_NewInt32(ctx, pathEl->GetSlopeDirection());
    }

    JSValue ScPathNavigator::isQueue_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data->_position);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewBool(ctx, pathEl->IsQueue());
    }

    JSValue ScPathNavigator::isWide_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data->_position);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewBool(ctx, pathEl->IsWide());
    }

    JSValue ScPathNavigator::ride_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data->_position);
        if (pathEl == nullptr)
            return JS_NULL;
        if (!pathEl->IsQueue())
            return JS_NULL;
        auto rideIndex = pathEl->GetRideIndex();
        if (rideIndex.IsNull())
            return JS_NULL;
        return JS_NewInt32(ctx, rideIndex.ToUnderlying());
    }

    JSValue ScPathNavigator::station_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data->_position);
        if (pathEl == nullptr)
            return JS_NULL;
        if (!pathEl->IsQueue())
            return JS_NULL;
        auto stationIndex = pathEl->GetStationIndex();
        if (stationIndex.IsNull())
            return JS_NULL;
        return JS_NewInt32(ctx, stationIndex.ToUnderlying());
    }

    JSValue ScPathNavigator::getConnectedPaths(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        auto* data = GetPathNavigatorData(thisVal);
        if (data == nullptr)
            return JS_NULL;

        auto* pathElement = FindPathElement(data->_position);
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

            // Adjust Z for slope when moving in the slope direction
            if (pathElement->IsSloped() && pathElement->GetSlopeDirection() == direction)
            {
                neighborPos.z += 2;
            }

            neighborPos.x += TileDirectionDelta[direction].x;
            neighborPos.y += TileDirectionDelta[direction].y;

            // Find path element at neighbor position
            auto neighborCoords = neighborPos.ToCoordsXY();
            TileElement* nextEl = MapGetFirstElementAt(neighborCoords);
            if (nextEl == nullptr)
                continue;

            do
            {
                if (nextEl->IsGhost())
                    continue;
                if (nextEl->GetType() != TileElementType::Path)
                    continue;

                const auto* nextPath = nextEl->AsPath();
                if (!FootpathIsZAndDirectionValid(*nextPath, neighborPos.z, direction))
                    continue;

                // Found a valid connected path — build result object
                // Use the actual base height of the found element for the position
                TileCoordsXYZ actualPos(neighborPos.x, neighborPos.y, nextEl->BaseHeight);

                JSValue conn = JS_NewObject(ctx);
                JS_SetPropertyStr(ctx, conn, "position", ToJSValue(ctx, actualPos.ToCoordsXYZ()));
                JS_SetPropertyStr(ctx, conn, "direction", JS_NewInt32(ctx, direction));
                JS_SetPropertyStr(ctx, conn, "isSloped", JS_NewBool(ctx, nextPath->IsSloped()));
                JS_SetPropertyStr(
                    ctx, conn, "slopeDirection",
                    nextPath->IsSloped() ? JS_NewInt32(ctx, nextPath->GetSlopeDirection()) : JS_NULL);
                JS_SetPropertyStr(ctx, conn, "isQueue", JS_NewBool(ctx, nextPath->IsQueue()));
                JS_SetPropertyStr(ctx, conn, "isWide", JS_NewBool(ctx, nextPath->IsWide()));

                auto rideIndex = nextPath->GetRideIndex();
                JS_SetPropertyStr(
                    ctx, conn, "ride",
                    (nextPath->IsQueue() && !rideIndex.IsNull()) ? JS_NewInt32(ctx, rideIndex.ToUnderlying()) : JS_NULL);

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

        auto* pathElement = FindPathElement(data->_position);
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

        do
        {
            if (nextEl->IsGhost())
                continue;
            if (nextEl->GetType() != TileElementType::Path)
                continue;

            const auto* nextPath = nextEl->AsPath();
            if (!FootpathIsZAndDirectionValid(*nextPath, neighborPos.z, direction))
                continue;

            // Move to the neighbor
            data->_position = TileCoordsXYZ(neighborPos.x, neighborPos.y, nextEl->BaseHeight);
            return JS_NewBool(ctx, true);
        } while (!(nextEl++)->IsLastForTile());

        return JS_NewBool(ctx, false);
    }

} // namespace OpenRCT2::Scripting

#endif
