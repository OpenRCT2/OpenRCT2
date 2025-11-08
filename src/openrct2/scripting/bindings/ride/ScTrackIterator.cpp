/*****************************************************************************
 * Copyright (c) 2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScTrackIterator.h"

    #include "../../../Context.h"
    #include "../../../ride/Ride.h"
    #include "../../../ride/TrackData.h"
    #include "../../../world/Map.h"
    #include "../../../world/tile_element/TrackElement.h"
    #include "../../ScriptEngine.h"
    #include "ScTrackSegment.h"

using namespace OpenRCT2::Scripting;
using namespace OpenRCT2::TrackMetaData;

JSValue ScTrackIterator::FromElement(JSContext* ctx, const CoordsXY& position, int32_t elementIndex)
{
    auto el = MapGetNthElementAt(position, elementIndex);
    if (el == nullptr)
        return JS_NULL;
    auto origin = GetTrackSegmentOrigin(CoordsXYE(position, el));
    if (!origin)
        return JS_NULL;

    auto trackEl = el->AsTrack();
    return gScTrackIterator.New(ctx, *origin, trackEl->GetTrackType());
}

void ScTrackIterator::Register(JSContext* ctx)
{
    RegisterBaseStr(ctx, "TrackIterator", Finalize);
}

JSValue ScTrackIterator::New(JSContext* ctx, const CoordsXYZD& position, OpenRCT2::TrackElemType type)
{
    static constexpr JSCFunctionListEntry funcs[] = {
        JS_CGETSET_DEF("position", ScTrackIterator::position_get, nullptr),
        JS_CGETSET_DEF("segment", ScTrackIterator::segment_get, nullptr),
        JS_CGETSET_DEF("previousPosition", ScTrackIterator::previousPosition_get, nullptr),
        JS_CGETSET_DEF("nextPosition", ScTrackIterator::nextPosition_get, nullptr),
        JS_CFUNC_DEF("previous", 0, ScTrackIterator::previous),
        JS_CFUNC_DEF("next", 0, ScTrackIterator::next),
    };
    return MakeWithOpaque(ctx, funcs, new TrackIteratorData{ position, type });
}

void ScTrackIterator::Finalize(JSRuntime* rt, JSValue thisVal)
{
    TrackIteratorData* data = GetTrackIteratorData(thisVal);
    if (data)
        delete data;
}

ScTrackIterator::TrackIteratorData* ScTrackIterator::GetTrackIteratorData(JSValue thisVal)
{
    return gScTrackIterator.GetOpaque<TrackIteratorData*>(thisVal);
}

JSValue ScTrackIterator::position_get(JSContext* ctx, JSValue thisVal)
{
    auto* data = GetTrackIteratorData(thisVal);
    return ToJSValue(ctx, data->_position);
}

JSValue ScTrackIterator::segment_get(JSContext* ctx, JSValue thisVal)
{
    auto* data = GetTrackIteratorData(thisVal);

    if (data->_type >= TrackElemType::count)
        return JS_NULL;

    return gScTrackSegment.New(ctx, data->_type);
}

JSValue ScTrackIterator::previousPosition_get(JSContext* ctx, JSValue thisVal)
{
    auto* data = GetTrackIteratorData(thisVal);

    auto& ted = GetTrackElementDescriptor(data->_type);
    const auto& seq0 = ted.sequences[0].clearance;
    auto pos = data->_position + CoordsXYZ(seq0.x, seq0.y, seq0.z);

    auto el = MapGetTrackElementAtOfTypeSeq(pos, data->_type, 0);
    if (el == nullptr)
        return JS_NULL;

    auto posEl = CoordsXYE(pos.x, pos.y, reinterpret_cast<TileElement*>(el));
    TrackBeginEnd tbe{};
    TrackBlockGetPrevious(posEl, &tbe);
    CoordsXYZD result(tbe.end_x, tbe.end_y, tbe.begin_z, tbe.begin_direction);
    return ToJSValue(ctx, result);
}

JSValue ScTrackIterator::nextPosition_get(JSContext* ctx, JSValue thisVal)
{
    auto* data = GetTrackIteratorData(thisVal);

    auto& ted = GetTrackElementDescriptor(data->_type);
    const auto& seq0 = ted.sequences[0].clearance;
    auto pos = data->_position + CoordsXYZ(seq0.x, seq0.y, seq0.z);

    auto el = MapGetTrackElementAtOfTypeSeq(pos, data->_type, 0);
    if (el == nullptr)
        return JS_NULL;

    auto posEl = CoordsXYE(data->_position.x, data->_position.y, reinterpret_cast<TileElement*>(el));
    CoordsXYE next;
    int32_t z{};
    int32_t direction{};
    TrackBlockGetNext(&posEl, &next, &z, &direction);
    CoordsXYZD result(next.x, next.y, z, direction);
    return ToJSValue(ctx, result);
}

JSValue ScTrackIterator::previous(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
{
    auto* data = GetTrackIteratorData(thisVal);

    auto& ted = GetTrackElementDescriptor(data->_type);
    const auto& seq0 = ted.sequences[0].clearance;
    auto pos = data->_position + CoordsXYZ(seq0.x, seq0.y, seq0.z);

    auto el = MapGetTrackElementAtOfTypeSeq(pos, data->_type, 0);
    if (el == nullptr)
        return JS_NewBool(ctx, false);

    auto posEl = CoordsXYE(pos.x, pos.y, reinterpret_cast<TileElement*>(el));
    TrackBeginEnd tbe{};
    if (TrackBlockGetPrevious(posEl, &tbe))
    {
        auto prev = CoordsXYE(tbe.end_x, tbe.end_y, tbe.begin_element);
        auto origin = GetTrackSegmentOrigin(prev);
        if (origin)
        {
            data->_position = *origin;
            data->_type = prev.element->AsTrack()->GetTrackType();
            return JS_NewBool(ctx, true);
        }
    }
    return JS_NewBool(ctx, false);
}

JSValue ScTrackIterator::next(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
{
    auto* data = GetTrackIteratorData(thisVal);

    auto& ted = GetTrackElementDescriptor(data->_type);
    const auto& seq0 = ted.sequences[0].clearance;
    auto pos = data->_position + CoordsXYZ(seq0.x, seq0.y, seq0.z);

    auto el = MapGetTrackElementAtOfTypeSeq(pos, data->_type, 0);
    if (el == nullptr)
        return JS_NewBool(ctx, false);

    auto posEl = CoordsXYE(data->_position.x, data->_position.y, reinterpret_cast<TileElement*>(el));
    CoordsXYE next;
    int32_t z{};
    int32_t direction{};
    if (TrackBlockGetNext(&posEl, &next, &z, &direction))
    {
        auto origin = GetTrackSegmentOrigin(next);
        if (origin)
        {
            data->_position = *origin;
            data->_type = next.element->AsTrack()->GetTrackType();
            return JS_NewBool(ctx, true);
        }
    }
    return JS_NewBool(ctx, false);
}

#endif
