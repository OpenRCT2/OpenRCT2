/*****************************************************************************
 * Copyright (c) 2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScTrackSegment.h"

    #include "../../../Context.h"
    #include "../../../core/EnumMap.hpp"
    #include "../../../ride/TrackData.h"
    #include "../../../ride/Vehicle.h"
    #include "../../ScriptEngine.h"
    #include "../../ScriptUtil.hpp"

using namespace OpenRCT2::Scripting;
using namespace OpenRCT2::TrackMetaData;

static JSValue VehicleInfoToJSValue(JSContext* ctx, const VehicleInfo& value)
{
    JSValue obj = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, obj, "x", JS_NewInt32(ctx, value.x));
    JS_SetPropertyStr(ctx, obj, "y", JS_NewInt32(ctx, value.y));
    JS_SetPropertyStr(ctx, obj, "z", JS_NewInt32(ctx, value.z));
    JS_SetPropertyStr(ctx, obj, "yaw", JS_NewInt32(ctx, value.direction));
    JS_SetPropertyStr(ctx, obj, "pitch", JS_NewInt32(ctx, EnumValue(value.pitch)));
    JS_SetPropertyStr(ctx, obj, "roll", JS_NewInt32(ctx, EnumValue(value.roll)));
    return obj;
}

void ScTrackSegment::Register(JSContext* ctx)
{
    RegisterBaseStr(ctx, "TrackSegment", Finalize);
}

JSValue ScTrackSegment::New(JSContext* ctx, OpenRCT2::TrackElemType type)
{
    static constexpr JSCFunctionListEntry funcs[] = {
        JS_CGETSET_DEF("type", ScTrackSegment::type_get, nullptr),
        JS_CGETSET_DEF("description", ScTrackSegment::description_get, nullptr),
        JS_CGETSET_DEF("elements", ScTrackSegment::elements_get, nullptr),
        JS_CGETSET_DEF("beginDirection", ScTrackSegment::beginDirection_get, nullptr),
        JS_CGETSET_DEF("endDirection", ScTrackSegment::endDirection_get, nullptr),
        JS_CGETSET_DEF("beginSlope", ScTrackSegment::beginSlope_get, nullptr),
        JS_CGETSET_DEF("endSlope", ScTrackSegment::endSlope_get, nullptr),
        JS_CGETSET_DEF("beginBank", ScTrackSegment::beginBank_get, nullptr),
        JS_CGETSET_DEF("endBank", ScTrackSegment::endBank_get, nullptr),
        JS_CGETSET_DEF("beginZ", ScTrackSegment::beginZ_get, nullptr),
        JS_CGETSET_DEF("endZ", ScTrackSegment::endZ_get, nullptr),
        JS_CGETSET_DEF("endX", ScTrackSegment::endX_get, nullptr),
        JS_CGETSET_DEF("endY", ScTrackSegment::endY_get, nullptr),
        JS_CGETSET_DEF("length", ScTrackSegment::length_get, nullptr),
        JS_CGETSET_DEF("nextSuggestedSegment", ScTrackSegment::nextCurveElement_get, nullptr),
        JS_CGETSET_DEF("previousSuggestedSegment", ScTrackSegment::previousCurveElement_get, nullptr),
        JS_CGETSET_DEF("mirrorSegment", ScTrackSegment::getMirrorElement, nullptr),
        JS_CGETSET_DEF("alternateTypeSegment", ScTrackSegment::getAlternativeElement, nullptr),
        JS_CGETSET_DEF("priceModifier", ScTrackSegment::getPriceModifier, nullptr),
        JS_CGETSET_DEF("trackGroup", ScTrackSegment::getTrackGroup, nullptr),
        JS_CGETSET_DEF("turnDirection", ScTrackSegment::getTrackCurvature, nullptr),
        JS_CGETSET_DEF("slopeDirection", ScTrackSegment::getTrackPitchDirection, nullptr),
        JS_CGETSET_DEF("onlyAllowedUnderwater", ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_ONLY_UNDERWATER>, nullptr),
        JS_CGETSET_DEF("onlyAllowedAboveGround", ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND>, nullptr),
        JS_CGETSET_DEF("allowsChainLift", ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_ALLOW_LIFT_HILL>, nullptr),
        JS_CGETSET_DEF("isBanked", ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_BANKED>, nullptr),
        JS_CGETSET_DEF("isInversion", ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_INVERSION_TO_NORMAL>, nullptr),
        JS_CGETSET_DEF("isSteepUp", ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_IS_STEEP_UP>, nullptr),
        JS_CGETSET_DEF("startsHalfHeightUp", ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT>, nullptr),
        JS_CGETSET_DEF("countsAsGolfHole", ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_IS_GOLF_HOLE>, nullptr),
        JS_CGETSET_DEF("isBankedTurn", ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_TURN_BANKED>, nullptr),
        JS_CGETSET_DEF("isSlopedTurn", ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_TURN_SLOPED>, nullptr),
        JS_CGETSET_DEF("isHelix", ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_HELIX>, nullptr),
        JS_CGETSET_DEF("countsAsInversion", ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_NORMAL_TO_INVERSION>, nullptr),
        JS_CFUNC_DEF("getSubpositionLength", 2, ScTrackSegment::getSubpositionLength),
        JS_CFUNC_DEF("getSubpositions", 2, ScTrackSegment::getSubpositions),
    };
    return MakeWithOpaque(ctx, funcs, new TrackSegmentData{ type });
}

void ScTrackSegment::Finalize(JSRuntime* rt, JSValue thisVal)
{
    TrackSegmentData* data = GetTrackSegmentData(thisVal);
    if (data)
        delete data;
}

ScTrackSegment::TrackSegmentData* ScTrackSegment::GetTrackSegmentData(JSValue thisVal)
{
    return gScTrackSegment.GetOpaque<TrackSegmentData*>(thisVal);
}

JSValue ScTrackSegment::type_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    return JS_NewInt32(ctx, EnumValue(data->_type));
}

JSValue ScTrackSegment::description_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewString(ctx, LanguageGetString(ted.description));
}

JSValue ScTrackSegment::beginZ_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewInt32(ctx, ted.coordinates.zBegin);
}

JSValue ScTrackSegment::beginDirection_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewInt32(ctx, ted.coordinates.rotationBegin);
}

JSValue ScTrackSegment::beginSlope_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewInt32(ctx, EnumValue(ted.definition.pitchStart));
}

JSValue ScTrackSegment::beginBank_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewInt32(ctx, EnumValue(ted.definition.rollStart));
}

JSValue ScTrackSegment::endX_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewInt32(ctx, ted.coordinates.x);
}

JSValue ScTrackSegment::endY_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewInt32(ctx, ted.coordinates.y);
}

JSValue ScTrackSegment::endZ_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewInt32(ctx, ted.coordinates.zEnd);
}

JSValue ScTrackSegment::endDirection_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewInt32(ctx, ted.coordinates.rotationEnd);
}

JSValue ScTrackSegment::endSlope_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewUint32(ctx, EnumValue(ted.definition.pitchEnd));
}

JSValue ScTrackSegment::endBank_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewUint32(ctx, EnumValue(ted.definition.rollEnd));
}

JSValue ScTrackSegment::length_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewUint32(ctx, ted.pieceLength);
}

JSValue ScTrackSegment::elements_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);

    JSValue result = JS_NewArray(ctx);

    for (int64_t i = 0; i < ted.numSequences; i++)
    {
        auto& block = ted.sequences[i].clearance;
        JSValue element = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, element, "x", JS_NewInt32(ctx, block.x));
        JS_SetPropertyStr(ctx, element, "y", JS_NewInt32(ctx, block.y));
        JS_SetPropertyStr(ctx, element, "z", JS_NewInt32(ctx, block.z));
        JS_SetPropertyInt64(ctx, result, i, element);
    }

    return result;
}

JSValue ScTrackSegment::getSubpositionLength(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
{
    JS_UNPACK_UINT32(trackSubposition, ctx, argv[0]);
    JS_UNPACK_UINT32(direction, ctx, argv[1]);

    const auto* data = GetTrackSegmentData(thisVal);
    uint16_t length = VehicleGetMoveInfoSize(
        static_cast<VehicleTrackSubposition>(trackSubposition), data->_type, static_cast<uint8_t>(direction));
    return JS_NewUint32(ctx, length);
}

JSValue ScTrackSegment::getSubpositions(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
{
    JS_UNPACK_UINT32(trackSubposition, ctx, argv[0]);
    JS_UNPACK_UINT32(direction, ctx, argv[1]);

    const auto* data = GetTrackSegmentData(thisVal);
    const uint16_t size = VehicleGetMoveInfoSize(
        static_cast<VehicleTrackSubposition>(trackSubposition), data->_type, static_cast<uint8_t>(direction));
    const uint16_t typeAndDirection = (EnumValue(data->_type) << 2) | (direction & 3);

    JSValue result = JS_NewArray(ctx);

    for (int64_t idx = 0; idx < size; idx++)
    {
        JSValue subposition = VehicleInfoToJSValue(ctx, gTrackVehicleInfo[trackSubposition][typeAndDirection]->info[idx]);
        JS_SetPropertyInt64(ctx, result, idx, subposition);
    }

    return result;
}

static JSValue _trackCurveToString(JSContext* ctx, TrackCurve curve)
{
    static const EnumMap<TrackCurve> map(
        {
            { "straight", TrackCurve::None },
            { "left", TrackCurve::Left },
            { "right", TrackCurve::Right },
            { "left_small", TrackCurve::LeftSmall },
            { "right_small", TrackCurve::RightSmall },
            { "left_very_small", TrackCurve::LeftVerySmall },
            { "right_very_small", TrackCurve::RightVerySmall },
            { "left_large", TrackCurve::LeftLarge },
            { "right_large", TrackCurve::RightLarge },
        });

    const auto text = std::string(map[curve]);
    return JSFromStdString(ctx, text);
}

JSValue ScTrackSegment::nextCurveElement_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);

    auto nextInChain = ted.curveChain.next;
    if (nextInChain.isTrackType)
        return JS_NewInt32(ctx, EnumValue(nextInChain.trackType));

    return _trackCurveToString(ctx, nextInChain.curve);
}

JSValue ScTrackSegment::previousCurveElement_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);

    auto previousInChain = ted.curveChain.previous;
    if (previousInChain.isTrackType)
        return JS_NewInt32(ctx, EnumValue(previousInChain.trackType));

    return _trackCurveToString(ctx, previousInChain.curve);
}

JSValue ScTrackSegment::getMirrorElement(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    if (ted.mirrorElement == TrackElemType::None)
        return JS_NULL;
    return JS_NewInt32(ctx, EnumValue(ted.mirrorElement));
}

JSValue ScTrackSegment::getAlternativeElement(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    if (ted.alternativeType == TrackElemType::None)
        return JS_NULL;
    return JS_NewInt32(ctx, EnumValue(ted.alternativeType));
}

JSValue ScTrackSegment::getPriceModifier(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewInt32(ctx, ted.priceModifier);
}

template<uint16_t flag>
JSValue ScTrackSegment::getTrackFlag(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewBool(ctx, ted.flags & flag);
}

JSValue ScTrackSegment::getTrackGroup(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    return JS_NewInt32(ctx, EnumValue(ted.definition.group));
}

JSValue ScTrackSegment::getTrackCurvature(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    std::string curvature;
    if (ted.flags & TRACK_ELEM_FLAG_TURN_LEFT)
        curvature = "left";
    else if (ted.flags & TRACK_ELEM_FLAG_TURN_RIGHT)
        curvature = "right";
    else
        curvature = "straight";
    return JSFromStdString(ctx, curvature);
}

JSValue ScTrackSegment::getTrackPitchDirection(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetTrackSegmentData(thisVal);
    const auto& ted = GetTrackElementDescriptor(data->_type);
    std::string pitch;
    if (ted.flags & TRACK_ELEM_FLAG_UP)
        pitch = "up";
    else if (ted.flags & TRACK_ELEM_FLAG_DOWN)
        pitch = "down";
    else
        pitch = "flat";
    return JSFromStdString(ctx, pitch);
}

#endif
