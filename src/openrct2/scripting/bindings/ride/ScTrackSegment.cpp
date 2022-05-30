/*****************************************************************************
 * Copyright (c) 2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScTrackSegment.h"

#    include "../../../Context.h"
#    include "../../../ride/TrackData.h"
#    include "../../ScriptEngine.h"

using namespace OpenRCT2::Scripting;
using namespace OpenRCT2::TrackMetaData;

ScTrackSegment::ScTrackSegment(track_type_t type)
    : _type(type)
{
}

void ScTrackSegment::Register(duk_context* ctx)
{
    dukglue_register_property(ctx, &ScTrackSegment::type_get, nullptr, "type");
    dukglue_register_property(ctx, &ScTrackSegment::description_get, nullptr, "description");
    dukglue_register_property(ctx, &ScTrackSegment::elements_get, nullptr, "elements");
    dukglue_register_property(ctx, &ScTrackSegment::beginZ_get, nullptr, "beginZ");
    dukglue_register_property(ctx, &ScTrackSegment::beginDirection_get, nullptr, "beginDirection");
    dukglue_register_property(ctx, &ScTrackSegment::endX_get, nullptr, "endX");
    dukglue_register_property(ctx, &ScTrackSegment::endY_get, nullptr, "endY");
    dukglue_register_property(ctx, &ScTrackSegment::endZ_get, nullptr, "endZ");
    dukglue_register_property(ctx, &ScTrackSegment::endDirection_get, nullptr, "endDirection");
    dukglue_register_property(ctx, &ScTrackSegment::length_get, nullptr, "length");
}

int32_t ScTrackSegment::type_get() const
{
    return _type;
}

std::string ScTrackSegment::description_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return language_get_string(ted.Description);
}

int32_t ScTrackSegment::beginZ_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.Coordinates.z_begin;
}

int32_t ScTrackSegment::beginDirection_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.Coordinates.rotation_begin;
}

int32_t ScTrackSegment::beginAngle_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.Definition.vangle_start;
}

int32_t ScTrackSegment::beginBank_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.Definition.bank_start;
}

int32_t ScTrackSegment::endX_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.Coordinates.x;
}

int32_t ScTrackSegment::endY_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.Coordinates.y;
}

int32_t ScTrackSegment::endZ_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.Coordinates.z_end;
}

int32_t ScTrackSegment::endDirection_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.Coordinates.rotation_end;
}

int32_t ScTrackSegment::endAngle_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.Definition.vangle_end;
}

int32_t ScTrackSegment::endBank_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.Definition.bank_end;
}

int32_t ScTrackSegment::length_get() const
{
    const auto& ted = GetTrackElementDescriptor(_type);
    return ted.PieceLength;
}

DukValue ScTrackSegment::elements_get() const
{
    auto& scriptEngine = GetContext()->GetScriptEngine();
    auto ctx = scriptEngine.GetContext();

    const auto& ted = GetTrackElementDescriptor(_type);

    duk_push_array(ctx);

    duk_uarridx_t index = 0;
    for (auto* block = ted.Block; block->index != 0xFF; block++)
    {
        duk_push_object(ctx);
        duk_push_number(ctx, block->x);
        duk_put_prop_string(ctx, -2, "x");
        duk_push_number(ctx, block->y);
        duk_put_prop_string(ctx, -2, "y");
        duk_push_number(ctx, block->z);
        duk_put_prop_string(ctx, -2, "z");

        duk_put_prop_index(ctx, -2, index);
        index++;
    }

    return DukValue::take_from_stack(ctx);
}

#endif
