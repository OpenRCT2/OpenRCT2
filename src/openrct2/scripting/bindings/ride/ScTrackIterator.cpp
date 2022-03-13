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
    dukglue_register_method(ctx, &ScTrackIterator::previous, "previous");
    dukglue_register_method(ctx, &ScTrackIterator::next, "next");
}

DukValue ScTrackIterator::position_get() const
{
    auto& scriptEngine = GetContext()->GetScriptEngine();
    auto ctx = scriptEngine.GetContext();
    return ToDuk(ctx, _position);
}

DukValue ScTrackIterator::segment_get() const
{
    auto& scriptEngine = GetContext()->GetScriptEngine();
    auto ctx = scriptEngine.GetContext();

    if (_type >= TrackElemType::Count)
        return ToDuk(ctx, undefined);

    return GetObjectAsDukValue(ctx, std::make_shared<ScTrackSegment>(_type));
}

bool ScTrackIterator::previous()
{
    auto& ted = GetTrackElementDescriptor(_type);
    auto& seq0 = ted.Block;
    auto pos = _position + CoordsXYZ(seq0->x, seq0->y, seq0->z);

    auto el = map_get_track_element_at(pos);
    if (el == nullptr)
        return false;

    auto posEl = CoordsXYE(pos.x, pos.y, reinterpret_cast<TileElement*>(el));
    track_begin_end tbe{};
    auto value = track_block_get_previous(posEl, &tbe);
    if (tbe.begin_element != nullptr)
    {
        auto prev = CoordsXYE(tbe.begin_x, tbe.begin_y, tbe.begin_element);
        auto origin = GetTrackSegmentOrigin(prev);
        if (origin)
        {
            _position = *origin;
            _type = prev.element->AsTrack()->GetTrackType();
            return value;
        }
    }
    return false;
}

bool ScTrackIterator::next()
{
    auto& ted = GetTrackElementDescriptor(_type);
    auto& seq0 = ted.Block;
    auto pos = _position + CoordsXYZ(seq0->x, seq0->y, seq0->z);

    auto el = map_get_track_element_at(pos);
    if (el == nullptr)
        return false;

    auto posEl = CoordsXYE(_position.x, _position.y, reinterpret_cast<TileElement*>(el));
    CoordsXYE next;
    int32_t z{};
    int32_t direction = -1;
    auto value = track_block_get_next(&posEl, &next, &z, &direction);
    if (next.element != nullptr)
    {
        auto origin = GetTrackSegmentOrigin(next);
        if (origin)
        {
            _position = *origin;
            _type = next.element->AsTrack()->GetTrackType();
            return value;
        }
    }
    return false;
}

#endif
