/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScPathConnection.h"

    #include "../../../world/Map.h"
    #include "../../../world/tile_element/PathElement.h"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    void ScPathConnection::Register(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("position", ScPathConnection::position_get, nullptr),
            JS_CGETSET_DEF("direction", ScPathConnection::direction_get, nullptr),
            JS_CGETSET_DEF("elementIndex", ScPathConnection::elementIndex_get, nullptr),
            JS_CGETSET_DEF("isSloped", ScPathConnection::isSloped_get, nullptr),
            JS_CGETSET_DEF("slopeDirection", ScPathConnection::slopeDirection_get, nullptr),
            JS_CGETSET_DEF("isQueue", ScPathConnection::isQueue_get, nullptr),
            JS_CGETSET_DEF("isWide", ScPathConnection::isWide_get, nullptr),
            JS_CGETSET_DEF("ride", ScPathConnection::ride_get, nullptr),
            JS_CGETSET_DEF("station", ScPathConnection::station_get, nullptr),
        };
        RegisterBase(ctx, "PathConnection", Finalize, funcs);
    }

    JSValue ScPathConnection::New(JSContext* ctx, const TileCoordsXYZ& position, int32_t elementIndex, int32_t direction)
    {
        return MakeWithOpaque(ctx, new PathConnectionData{ position, elementIndex, direction });
    }

    void ScPathConnection::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        PathConnectionData* data = GetData(thisVal);
        if (data)
            delete data;
    }

    ScPathConnection::PathConnectionData* ScPathConnection::GetData(JSValue thisVal)
    {
        return gScPathConnection.GetOpaque<PathConnectionData*>(thisVal);
    }

    const PathElement* ScPathConnection::FindPathElement(const PathConnectionData* data)
    {
        auto coords = data->_position.ToCoordsXY();
        auto* el = MapGetNthElementAt(coords, data->_elementIndex);
        if (el == nullptr)
            return nullptr;
        if (el->GetType() != TileElementType::Path)
            return nullptr;
        if (el->BaseHeight != data->_position.z)
            return nullptr;
        if (el->IsGhost())
            return nullptr;
        return el->AsPath();
    }

    JSValue ScPathConnection::position_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        return ToJSValue(ctx, data->_position.ToCoordsXYZ());
    }

    JSValue ScPathConnection::direction_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetData(thisVal);
        if (data == nullptr || data->_direction < 0)
            return JS_NULL;
        return JS_NewInt32(ctx, data->_direction);
    }

    JSValue ScPathConnection::elementIndex_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        return JS_NewInt32(ctx, data->_elementIndex);
    }

    JSValue ScPathConnection::isSloped_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewBool(ctx, pathEl->IsSloped());
    }

    JSValue ScPathConnection::slopeDirection_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data);
        if (pathEl == nullptr || !pathEl->IsSloped())
            return JS_NULL;
        return JS_NewInt32(ctx, pathEl->GetSlopeDirection());
    }

    JSValue ScPathConnection::isQueue_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewBool(ctx, pathEl->IsQueue());
    }

    JSValue ScPathConnection::isWide_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewBool(ctx, pathEl->IsWide());
    }

    JSValue ScPathConnection::ride_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data);
        if (pathEl == nullptr || !pathEl->IsQueue())
            return JS_NULL;
        auto rideIndex = pathEl->GetRideIndex();
        if (rideIndex.IsNull())
            return JS_NULL;
        return JS_NewInt32(ctx, rideIndex.ToUnderlying());
    }

    JSValue ScPathConnection::station_get(JSContext* ctx, JSValue thisVal)
    {
        auto* data = GetData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = FindPathElement(data);
        if (pathEl == nullptr || !pathEl->IsQueue())
            return JS_NULL;
        auto stationIndex = pathEl->GetStationIndex();
        if (stationIndex.IsNull())
            return JS_NULL;
        return JS_NewInt32(ctx, stationIndex.ToUnderlying());
    }

} // namespace OpenRCT2::Scripting

#endif
