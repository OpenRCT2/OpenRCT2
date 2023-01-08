#include "PaintStructDescriptor.h"

#include "../paint/Supports.h"
#include "../ride/Ride.h"
#include "../core/Json.hpp"

// helper method to check for the keys
bool PaintStructDescriptor::MatchWithKeys(track_type_t trackElement, uint32_t direction, uint32_t trackSequence) const
{
    // when the field is not set, it is intended as a wild card
    if (Element != trackElement)
        return false;

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
            const auto& sequenceMapping = TrackSequenceMapping.value();
            trackSequence = sequenceMapping.Values[direction][trackSequence];
            if (TrackSequence != trackSequence)
                return false;
        }
        else if (TrackSequence != trackSequence)
            return false;
    }
    return true;
}

void PaintStructDescriptor::Paint(
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
        const auto& sequenceMapping = TrackSequenceMapping.value().Values;
        const auto& mapping = sequenceMapping[direction];
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
        if (type == PaintType::AddImageAsParent)
        {
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

            auto newOffset = Offset + CoordsXYZ{ 0, 0, height };
            auto newBoundBox = BoundBox;
            newBoundBox.offset.z += height;
            PaintAddImageAsParent(session, imageTemplate.WithIndex(imageIndex), newOffset, newBoundBox);
        }
        else if (type == PaintType::SetSegmentsSupportsHeight)
        {
            uint32_t segments = 0;
            if (HeightSupports.Segments.find(trackSequence) != HeightSupports.Segments.end())
            {
                segments = HeightSupports.Segments.at(trackSequence);
            }

            PaintUtilSetSegmentSupportHeight(session, segments, height + 2, 0x20);
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL & ~segments, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + HeightSupports.HeightOffset, 0x20);
        }
    }
}

void PaintStructDescriptor::ToJson(json_t& json) const
{
    switch (Element)
    {
        case TrackElemType::FlatTrack3x3:
            json["trackElement"] = "flat_track_3x3";
            break;
    }

    if (Direction.has_value())
    {
        json["direction"] = Direction.value();
    }

    if (TrackSequence.has_value())
    {
        json["trackSequence"] = TrackSequence.value();
    }

    if (TrackSequenceMapping.has_value())
    {
        //just store the id
        json["trackSequenceMapping"] = TrackSequenceMapping.value().Id;
    }

    if (Supports.has_value())
    {
        switch (Supports.value())
        {
            case SupportsType::WoodenA:
                json["supports"] = "wooden_a";
                break;
        }
    }

    if (Floor.has_value())
    {
        switch (Floor.value())
        {
            case FloorType::Cork:
                json["floor"] = "cork";
                break;
        }
    }

    if (Edges.has_value())
    {
    }
}
