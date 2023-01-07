#include "PaintObject.h"

#include "../paint/Paint.h"
#include "../paint/Supports.h"
#include "../ride/Ride.h"
#include "../ride/TrackPaint.h"
#include "../core/Json.hpp"

void PaintObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    auto paintStructs = root["paintStructs"];
    if (paintStructs.is_array())
    {
        for (const auto& paintStruct : paintStructs)
        {
        }
    }
}

void PaintObject::Load()
{
}

void PaintObject::Unload()
{
}

void PaintObject::Paint(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    // iterate through the paint structs
    for (const auto& paintStruct : _paintStructs)
    {
        if (paintStruct.MatchWithKeys(trackElement.GetTrackType(), direction, trackSequence))
            paintStruct.Paint(session, ride, trackSequence, direction, height, trackElement);
    }
}

// helper method to check for the keys
bool PaintObject::PaintStruct::MatchWithKeys(track_type_t trackElement, uint32_t direction, uint32_t trackSequence) const
{
    // when the field is not set, it is intended as a wild card
    if (Element.has_value())
    {
        if (Element != trackElement)
            return false;
    }

    if (Direction.has_value())
    {
        if (Direction != direction)
            return false;
    }

    if (TrackSequence.has_value())
    {
        // check if we need to transform the track sequence with the mapping
        if (TrackSequenceMapping.has_value())
        {
            trackSequence = TrackSequenceMapping.value()[direction][trackSequence];
            if (TrackSequence != trackSequence)
                return false;
        }
        else if (TrackSequence != trackSequence)
            return false;
    }
    return true;
}

void PaintObject::PaintStruct::Paint(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement) const
{
    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    if (Supports.has_value())
    {
        switch (Supports.value())
        {
            case SupportsType::WoodenA:
            default:
                WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);
                break;
        }
    }

    // transform the track sequence with the track mapping if there is one
    if (TrackSequenceMapping.has_value())
    {
        const auto& mapping = TrackSequenceMapping.value()[direction];
        if (trackSequence < mapping.size())
            trackSequence = mapping[trackSequence];
    }

    uint8_t edges = 0;
    if (Edges.has_value())
    {
        if (trackSequence < Edges.value().size())
            edges = Edges.value()[trackSequence];
    }

    if (Floor.has_value())
    {
        const uint32_t* sprites = nullptr;
        switch (Floor.value())
        {
            case FloorType::Cork:
            default:
                sprites = floorSpritesCork;
                break;
        }

        const StationObject* stationObject = ride.GetStationObject();

        if (stationObject != nullptr)
        {
            track_paint_util_paint_floor(session, edges, session.TrackColours[SCHEME_TRACK], height, sprites, stationObject);
        }
    }

    if (Fences.has_value())
    {
        const uint32_t* sprites = nullptr;
        switch (Fences.value())
        {
            case FenceType::Ropes:
            default:
                sprites = fenceSpritesRope;
                break;
        }

        track_paint_util_paint_fences(
            session, edges, session.MapPosition, trackElement, ride, session.TrackColours[SCHEME_SUPPORTS], height, sprites,
            session.CurrentRotation);
    }

    if (PaintType.has_value())
    {
        auto type = PaintType.value();

        ImageId imageTemplate = ImageId(0, ride.vehicle_colours[0].Body, ride.vehicle_colours[0].Trim);
        ImageId imageFlags;
        switch (ImageIdScheme)
        {
            case Scheme::Track:
                imageFlags = session.TrackColours[SCHEME_TRACK];
                break;
            default:
            case Scheme::Misc:
                imageFlags = session.TrackColours[SCHEME_MISC];
                break;
        }
        if (imageFlags != TrackGhost)
        {
            imageTemplate = imageFlags;
        }

        uint32_t imageIndex = 0;
        switch (ImageIdBase)
        {
            case ImageIdBase::Car0:
            default:
                imageIndex = rideEntry->Cars[0].base_image_id;
                break;
        }
        imageIndex = imageIndex + ImageIdOffset;

        switch (type)
        {
            case PaintType::AddImageAsParent:
            default:
                PaintAddImageAsParent(session, imageTemplate.WithIndex(imageIndex), Offset, BoundBox);
                break;
        }
    }
}
