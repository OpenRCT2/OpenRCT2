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
    void ScPathConnection::registerClass(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry kFuncs[] = {
            JS_CGETSET_DEF("position", ScPathConnection::getPosition, nullptr),
            JS_CGETSET_DEF("direction", ScPathConnection::getDirection, nullptr),
            JS_CGETSET_DEF("elementIndex", ScPathConnection::getElementIndex, nullptr),
            JS_CGETSET_DEF("isSloped", ScPathConnection::isSloped, nullptr),
            JS_CGETSET_DEF("slopeDirection", ScPathConnection::getSlopeDirection, nullptr),
            JS_CGETSET_DEF("isQueue", ScPathConnection::isQueue, nullptr),
            JS_CGETSET_DEF("isWide", ScPathConnection::isWide, nullptr),
            JS_CGETSET_DEF("ride", ScPathConnection::getRide, nullptr),
            JS_CGETSET_DEF("station", ScPathConnection::getStation, nullptr),
        };
        RegisterBase(ctx, "PathConnection", finalize, kFuncs);
    }

    JSValue ScPathConnection::create(JSContext* ctx, const TileCoordsXYZ& position, int32_t elementIndex, int32_t direction)
    {
        return MakeWithOpaque(ctx, new PathConnectionData{ position, elementIndex, direction });
    }

    void ScPathConnection::finalize(JSRuntime* rt, JSValue thisVal)
    {
        PathConnectionData* data = getData(thisVal);
        if (data)
            delete data;
    }

    ScPathConnection::PathConnectionData* ScPathConnection::getData(JSValue thisVal)
    {
        return gScPathConnection.GetOpaque<PathConnectionData*>(thisVal);
    }

    const PathElement* ScPathConnection::findPathElement(const PathConnectionData* data)
    {
        auto coords = data->position.ToCoordsXY();
        auto* el = MapGetNthElementAt(coords, data->elementIndex);
        if (el == nullptr)
            return nullptr;
        if (el->getType() != TileElementType::Path)
            return nullptr;
        if (el->baseHeight != data->position.z)
            return nullptr;
        if (el->isGhost())
            return nullptr;
        return el->asPath();
    }

    JSValue ScPathConnection::getPosition(JSContext* ctx, JSValue thisVal)
    {
        auto* data = getData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        return ToJSValue(ctx, data->position.ToCoordsXYZ());
    }

    JSValue ScPathConnection::getDirection(JSContext* ctx, JSValue thisVal)
    {
        auto* data = getData(thisVal);
        if (data == nullptr || data->direction < 0)
            return JS_NULL;
        return JS_NewInt32(ctx, data->direction);
    }

    JSValue ScPathConnection::getElementIndex(JSContext* ctx, JSValue thisVal)
    {
        auto* data = getData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        return JS_NewInt32(ctx, data->elementIndex);
    }

    JSValue ScPathConnection::isSloped(JSContext* ctx, JSValue thisVal)
    {
        auto* data = getData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = findPathElement(data);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewBool(ctx, pathEl->IsSloped());
    }

    JSValue ScPathConnection::getSlopeDirection(JSContext* ctx, JSValue thisVal)
    {
        auto* data = getData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = findPathElement(data);
        if (pathEl == nullptr || !pathEl->IsSloped())
            return JS_NULL;
        return JS_NewInt32(ctx, pathEl->GetSlopeDirection());
    }

    JSValue ScPathConnection::isQueue(JSContext* ctx, JSValue thisVal)
    {
        auto* data = getData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = findPathElement(data);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewBool(ctx, pathEl->IsQueue());
    }

    JSValue ScPathConnection::isWide(JSContext* ctx, JSValue thisVal)
    {
        auto* data = getData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = findPathElement(data);
        if (pathEl == nullptr)
            return JS_NULL;
        return JS_NewBool(ctx, pathEl->IsWide());
    }

    JSValue ScPathConnection::getRide(JSContext* ctx, JSValue thisVal)
    {
        auto* data = getData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = findPathElement(data);
        if (pathEl == nullptr || !pathEl->IsQueue())
            return JS_NULL;
        auto rideIndex = pathEl->GetRideIndex();
        if (rideIndex.IsNull())
            return JS_NULL;
        return JS_NewInt32(ctx, rideIndex.ToUnderlying());
    }

    JSValue ScPathConnection::getStation(JSContext* ctx, JSValue thisVal)
    {
        auto* data = getData(thisVal);
        if (data == nullptr)
            return JS_NULL;
        auto* pathEl = findPathElement(data);
        if (pathEl == nullptr || !pathEl->IsQueue())
            return JS_NULL;
        auto stationIndex = pathEl->GetStationIndex();
        if (stationIndex.IsNull())
            return JS_NULL;
        return JS_NewInt32(ctx, stationIndex.ToUnderlying());
    }

} // namespace OpenRCT2::Scripting

#endif
