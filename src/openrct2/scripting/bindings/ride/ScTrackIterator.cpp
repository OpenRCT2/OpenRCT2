/*****************************************************************************
 * Copyright (c) 2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScTrackIterator.h"

#    include "../../../Context.h"
#    include "../../../ride/Ride.h"
#    include "../../../ride/TrackData.h"
#    include "../../ScriptEngine.h"
#    include "ScTrackSegment.h"

using namespace OpenRCT2::Scripting;
using namespace OpenRCT2::TrackMetaData;

std::shared_ptr<ScTrackIterator> ScTrackIterator::FromElement(const CoordsXY& position, int32_t elementIndex)
{
    auto el = map_get_nth_element_at(position, elementIndex);
    auto origin = GetTrackSegmentOrigin(CoordsXYE(position, el));
    if (!origin)
        return nullptr;

    auto trackEl = el->AsTrack();
    return std::make_shared<ScTrackIterator>(*origin, trackEl->GetTrackType(), trackEl->GetRideIndex());
}

ScTrackIterator::ScTrackIterator(const CoordsXYZD& position, track_type_t type, RideId ride)
    : _position(position)
    , _type(type)
    , _ride(ride)
{
}

void ScTrackIterator::Register(duk_context* ctx)
{
    dukglue_register_property(ctx, &ScTrackIterator::position_get, nullptr, "position");
    dukglue_register_property(ctx, &ScTrackIterator::segment_get, nullptr, "segment");
    dukglue_register_property(ctx, &ScTrackIterator::previousPosition_get, nullptr, "previousPosition");
    dukglue_register_property(ctx, &ScTrackIterator::nextPosition_get, nullptr, "nextPosition");
    dukglue_register_method(ctx, &ScTrackIterator::previous, "previous");
    dukglue_register_method(ctx, &ScTrackIterator::next, "next");
}

DukValue ScTrackIterator::position_get() const
{
    auto* scriptEngine = GetContext()->GetScriptEngine();
    auto ctx = scriptEngine->GetContext();
    return ToDuk(ctx, _position);
}

DukValue ScTrackIterator::segment_get() const
{
    auto* scriptEngine = GetContext()->GetScriptEngine();
    auto ctx = scriptEngine->GetContext();

    if (_type >= TrackElemType::Count)
        return ToDuk(ctx, nullptr);

    return GetObjectAsDukValue(ctx, std::make_shared<ScTrackSegment>(_type));
}

DukValue ScTrackIterator::previousPosition_get() const
{
    auto* scriptEngine = GetContext()->GetScriptEngine();
    auto ctx = scriptEngine->GetContext();

    auto& ted = GetTrackElementDescriptor(_type);
    auto& seq0 = ted.Block;
    auto pos = _position + CoordsXYZ(seq0->x, seq0->y, seq0->z);

    auto el = map_get_track_element_at_of_type_seq(pos, _type, 0);
    if (el == nullptr)
        return ToDuk(ctx, nullptr);

    auto posEl = CoordsXYE(pos.x, pos.y, reinterpret_cast<TileElement*>(el));
    track_begin_end tbe{};
    track_block_get_previous(posEl, &tbe);
    CoordsXYZD result(tbe.end_x, tbe.end_y, tbe.begin_z, tbe.begin_direction);
    return ToDuk(ctx, result);
}

DukValue ScTrackIterator::nextPosition_get() const
{
    auto* scriptEngine = GetContext()->GetScriptEngine();
    auto ctx = scriptEngine->GetContext();

    auto& ted = GetTrackElementDescriptor(_type);
    auto& seq0 = ted.Block;
    auto pos = _position + CoordsXYZ(seq0->x, seq0->y, seq0->z);

    auto el = map_get_track_element_at_of_type_seq(pos, _type, 0);
    if (el == nullptr)
        return ToDuk(ctx, nullptr);

    auto posEl = CoordsXYE(_position.x, _position.y, reinterpret_cast<TileElement*>(el));
    CoordsXYE next;
    int32_t z{};
    int32_t direction{};
    track_block_get_next(&posEl, &next, &z, &direction);
    CoordsXYZD result(next.x, next.y, z, direction);
    return ToDuk(ctx, result);
}

bool ScTrackIterator::previous()
{
    auto& ted = GetTrackElementDescriptor(_type);
    auto& seq0 = ted.Block;
    auto pos = _position + CoordsXYZ(seq0->x, seq0->y, seq0->z);

    auto el = map_get_track_element_at_of_type_seq(pos, _type, 0);
    if (el == nullptr)
        return false;

    auto posEl = CoordsXYE(pos.x, pos.y, reinterpret_cast<TileElement*>(el));
    track_begin_end tbe{};
    if (track_block_get_previous(posEl, &tbe))
    {
        auto prev = CoordsXYE(tbe.end_x, tbe.end_y, tbe.begin_element);
        auto origin = GetTrackSegmentOrigin(prev);
        if (origin)
        {
            _position = *origin;
            _type = prev.element->AsTrack()->GetTrackType();
            return true;
        }
    }
    return false;
}

bool ScTrackIterator::next()
{
    auto& ted = GetTrackElementDescriptor(_type);
    auto& seq0 = ted.Block;
    auto pos = _position + CoordsXYZ(seq0->x, seq0->y, seq0->z);

    auto el = map_get_track_element_at_of_type_seq(pos, _type, 0);
    if (el == nullptr)
        return false;

    auto posEl = CoordsXYE(_position.x, _position.y, reinterpret_cast<TileElement*>(el));
    CoordsXYE next;
    int32_t z{};
    int32_t direction{};
    if (track_block_get_next(&posEl, &next, &z, &direction))
    {
        auto origin = GetTrackSegmentOrigin(next);
        if (origin)
        {
            _position = *origin;
            _type = next.element->AsTrack()->GetTrackType();
            return true;
        }
    }
    return false;
}

#endif
