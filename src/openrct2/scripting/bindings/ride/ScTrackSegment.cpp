/*****************************************************************************
 * Copyright (c) 2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScTrackSegment.h"

    #include "../../../Context.h"
    #include "../../../ride/TrackData.h"
    #include "../../../ride/Vehicle.h"
    #include "../../ScriptEngine.h"

using namespace OpenRCT2::Scripting;
using namespace OpenRCT2::TrackMetaData;

ScTrackSegment::ScTrackSegment(OpenRCT2::TrackElemType type)
    : _type(type)
{
}

void ScTrackSegment::Register(duk_context* ctx)
{
    dukglue_register_property(ctx, &ScTrackSegment::type_get, nullptr, "type");
    dukglue_register_property(ctx, &ScTrackSegment::description_get, nullptr, "description");
    dukglue_register_property(ctx, &ScTrackSegment::elements_get, nullptr, "elements");
    dukglue_register_property(ctx, &ScTrackSegment::beginDirection_get, nullptr, "beginDirection");
    dukglue_register_property(ctx, &ScTrackSegment::endDirection_get, nullptr, "endDirection");
    dukglue_register_property(ctx, &ScTrackSegment::beginSlope_get, nullptr, "beginSlope");
    dukglue_register_property(ctx, &ScTrackSegment::endSlope_get, nullptr, "endSlope");
    dukglue_register_property(ctx, &ScTrackSegment::beginBank_get, nullptr, "beginBank");
    dukglue_register_property(ctx, &ScTrackSegment::endBank_get, nullptr, "endBank");
    dukglue_register_property(ctx, &ScTrackSegment::beginZ_get, nullptr, "beginZ");
    dukglue_register_property(ctx, &ScTrackSegment::endZ_get, nullptr, "endZ");
    dukglue_register_property(ctx, &ScTrackSegment::endX_get, nullptr, "endX");
    dukglue_register_property(ctx, &ScTrackSegment::endY_get, nullptr, "endY");
    dukglue_register_property(ctx, &ScTrackSegment::length_get, nullptr, "length");
    dukglue_register_property(ctx, &ScTrackSegment::nextCurveElement_get, nullptr, "nextSuggestedSegment");
    dukglue_register_property(ctx, &ScTrackSegment::previousCurveElement_get, nullptr, "previousSuggestedSegment");
    dukglue_register_property(ctx, &ScTrackSegment::getMirrorElement, nullptr, "mirrorSegment");
    dukglue_register_property(ctx, &ScTrackSegment::getAlternativeElement, nullptr, "alternateTypeSegment");
    dukglue_register_property(ctx, &ScTrackSegment::getPriceModifier, nullptr, "priceModifier");
    dukglue_register_property(ctx, &ScTrackSegment::getTrackGroup, nullptr, "trackGroup");
    dukglue_register_property(ctx, &ScTrackSegment::getTrackCurvature, nullptr, "turnDirection");
    dukglue_register_property(ctx, &ScTrackSegment::getTrackPitchDirection, nullptr, "slopeDirection");

    dukglue_register_property(
        ctx, &ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_ONLY_UNDERWATER>, nullptr, "onlyAllowedUnderwater");
    dukglue_register_property(
        ctx, &ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND>, nullptr, "onlyAllowedAboveGround");
    dukglue_register_property(ctx, &ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_ALLOW_LIFT_HILL>, nullptr, "allowsChainLift");
    dukglue_register_property(ctx, &ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_BANKED>, nullptr, "isBanked");
    dukglue_register_property(ctx, &ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_INVERSION_TO_NORMAL>, nullptr, "isInversion");
    dukglue_register_property(ctx, &ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_IS_STEEP_UP>, nullptr, "isSteepUp");
    dukglue_register_property(
        ctx, &ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT>, nullptr, "startsHalfHeightUp");
    dukglue_register_property(ctx, &ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_IS_GOLF_HOLE>, nullptr, "countsAsInversion");
    dukglue_register_property(ctx, &ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_TURN_BANKED>, nullptr, "isBankedTurn");
    dukglue_register_property(ctx, &ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_TURN_SLOPED>, nullptr, "isSlopedTurn");
    dukglue_register_property(ctx, &ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_HELIX>, nullptr, "isHelix");
    dukglue_register_property(
        ctx, &ScTrackSegment::getTrackFlag<TRACK_ELEM_FLAG_NORMAL_TO_INVERSION>, nullptr, "countsAsInversion");

    dukglue_register_method(ctx, &ScTrackSegment::getSubpositionLength, "getSubpositionLength");
    dukglue_register_method(ctx, &ScTrackSegment::getSubpositions, "getSubpositions");
}

int32_t ScTrackSegment::type_get() const
{
    return EnumValue(_type);
}

std::string ScTrackSegment::description_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return LanguageGetString(ted.description);
}

int32_t ScTrackSegment::beginZ_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.coordinates.zBegin;
}

int32_t ScTrackSegment::beginDirection_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.coordinates.rotationBegin;
}

int32_t ScTrackSegment::beginSlope_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return EnumValue(ted.definition.pitchStart);
}

int32_t ScTrackSegment::beginBank_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return EnumValue(ted.definition.rollStart);
}

int32_t ScTrackSegment::endX_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.coordinates.x;
}

int32_t ScTrackSegment::endY_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.coordinates.y;
}

int32_t ScTrackSegment::endZ_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.coordinates.zEnd;
}

int32_t ScTrackSegment::endDirection_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.coordinates.rotationEnd;
}

int32_t ScTrackSegment::endSlope_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return EnumValue(ted.definition.pitchEnd);
}

int32_t ScTrackSegment::endBank_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return EnumValue(ted.definition.rollEnd);
}

int32_t ScTrackSegment::length_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.pieceLength;
}

DukValue ScTrackSegment::elements_get() const
{
    auto& scriptEngine = GetContext()->GetScriptEngine();
    auto ctx = scriptEngine.GetContext();

    const auto& ted = GetTrackElementDescriptor(_type);

    duk_push_array(ctx);

    duk_uarridx_t index = 0;
    for (uint8_t i = 0; i < ted.numSequences; i++)
    {
        auto& block = ted.sequences[i].clearance;
        duk_push_object(ctx);
        duk_push_number(ctx, block.x);
        duk_put_prop_string(ctx, -2, "x");
        duk_push_number(ctx, block.y);
        duk_put_prop_string(ctx, -2, "y");
        duk_push_number(ctx, block.z);
        duk_put_prop_string(ctx, -2, "z");

        duk_put_prop_index(ctx, -2, index);
        index++;
    }

    return DukValue::take_from_stack(ctx);
}

uint16_t ScTrackSegment::getSubpositionLength(uint8_t trackSubposition, uint8_t direction) const
{
    return VehicleGetMoveInfoSize(static_cast<VehicleTrackSubposition>(trackSubposition), _type, direction);
}

std::vector<DukValue> ScTrackSegment::getSubpositions(uint8_t trackSubposition, uint8_t direction) const
{
    const auto ctx = GetContext()->GetScriptEngine().GetContext();
    const uint16_t size = getSubpositionLength(trackSubposition, direction);
    const uint16_t typeAndDirection = (EnumValue(_type) << 2) | (direction & 3);

    std::vector<DukValue> result;

    for (auto idx = 0; idx < size; idx++)
    {
        result.push_back(ToDuk<VehicleInfo>(ctx, gTrackVehicleInfo[trackSubposition][typeAndDirection]->info[idx]));
    }
    return result;
}

static DukValue _trackCurveToString(duk_context* ctx, TrackCurve curve)
{
    static const EnumMap<TrackCurve> map({
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

    u8string text = u8string(map[curve]);
    return ToDuk<std::string>(ctx, text);
}

DukValue ScTrackSegment::nextCurveElement_get() const
{
    const auto ctx = GetContext()->GetScriptEngine().GetContext();
    const auto& ted = GetTrackElementDescriptor(_type);

    auto nextInChain = ted.curveChain.next;
    if (nextInChain.isTrackType)
        return ToDuk<int32_t>(ctx, EnumValue(nextInChain.trackType));

    return _trackCurveToString(ctx, nextInChain.curve);
}

DukValue ScTrackSegment::previousCurveElement_get() const
{
    const auto ctx = GetContext()->GetScriptEngine().GetContext();
    const auto& ted = GetTrackElementDescriptor(_type);

    auto previousInChain = ted.curveChain.previous;
    if (previousInChain.isTrackType)
        return ToDuk<int32_t>(ctx, EnumValue(previousInChain.trackType));

    return _trackCurveToString(ctx, previousInChain.curve);
}

DukValue ScTrackSegment::getMirrorElement() const
{
    const auto ctx = GetContext()->GetScriptEngine().GetContext();
    const auto& ted = GetTrackElementDescriptor(_type);
    if (ted.mirrorElement == TrackElemType::None)
        return ToDuk(ctx, nullptr);
    return ToDuk<int32_t>(ctx, EnumValue(ted.mirrorElement));
}

DukValue ScTrackSegment::getAlternativeElement() const
{
    const auto ctx = GetContext()->GetScriptEngine().GetContext();
    const auto& ted = GetTrackElementDescriptor(_type);
    if (ted.alternativeType == TrackElemType::None)
        return ToDuk(ctx, nullptr);
    return ToDuk<int32_t>(ctx, EnumValue(ted.alternativeType));
}

int32_t ScTrackSegment::getPriceModifier() const
{
    const auto& ted = GetTrackElementDescriptor(_type);

    return ted.priceModifier;
}

template<uint16_t flag>
bool ScTrackSegment::getTrackFlag() const
{
    const auto& ted = GetTrackElementDescriptor(_type);

    return ted.flags & flag;
}

int32_t ScTrackSegment::getTrackGroup() const
{
    const auto& ted = GetTrackElementDescriptor(_type);

    return EnumValue(ted.definition.group);
}

std::string ScTrackSegment::getTrackCurvature() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    if (ted.flags & TRACK_ELEM_FLAG_TURN_LEFT)
        return "left";
    if (ted.flags & TRACK_ELEM_FLAG_TURN_RIGHT)
        return "right";
    return "straight";
}

std::string ScTrackSegment::getTrackPitchDirection() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    if (ted.flags & TRACK_ELEM_FLAG_UP)
        return "up";
    if (ted.flags & TRACK_ELEM_FLAG_DOWN)
        return "down";
    return "flat";
}

#endif
