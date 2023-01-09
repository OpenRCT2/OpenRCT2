#include "PaintStructDescriptor.h"

#include "../core/Json.hpp"
#include "../entity/EntityRegistry.h"
#include "../paint/Supports.h"
#include "../ride/Ride.h"
#include "../ride/Vehicle.h"

PaintStructDescriptor::PaintStructDescriptor()
    : PrimaryColour(Colour::VehicleBody)
    , PrimaryColourIndex(0)
    , SecondaryColour(Colour::VehicleTrim)
    , SecondaryColourIndex(0)
{
}

// helper method to check for the keys
bool PaintStructDescriptorKey::MatchWithKeys(
    track_type_t trackElement, uint32_t direction, uint32_t trackSequence, const Vehicle* vehicle) const
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

    if (vehicle != nullptr)
    {
        if (VehicleSpriteDirection.has_value())
        {
            if (vehicle->sprite_direction != VehicleSpriteDirection.value())
                return false;
        }

        if (VehiclePitch.has_value())
        {
            if (vehicle->Pitch != VehiclePitch.value())
                return false;
        }

        if (VehicleNumPeeps.has_value())
        {
            if (vehicle->num_peeps != VehicleNumPeeps.value())
                return false;
        }
    }
    return true;
}

void PaintStructDescriptor::Paint(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement) const
{
    // first, check if the paint struct key matches with the call
    Vehicle* vehicle = nullptr;
    if (Key.VehicleIndex.has_value())
    {
        if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
        {
            vehicle = GetEntity<Vehicle>(ride.vehicles[Key.VehicleIndex.value()]);

            session.InteractionType = ViewportInteractionItem::Entity;
            session.CurrentlyDrawnEntity = vehicle;
        }
    }

    if (!Key.MatchWithKeys(trackElement.GetTrackType(), direction, trackSequence, vehicle))
        return;

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
    if (Key.TrackSequenceMapping.has_value())
    {
        const auto& sequenceMapping = Key.TrackSequenceMapping.value().Values;
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
        if (type == PaintType::AddImageAsParent || type == PaintType::AddImageAsChild)
        {
            colour_t primaryColour = 0;
            colour_t secondaryColour = 0;

            switch (PrimaryColour)
            {
                case Colour::VehicleBody:
                    primaryColour = ride.vehicle_colours[PrimaryColourIndex].Body;
                    break;
                case Colour::VehicleTrim:
                    primaryColour = ride.vehicle_colours[PrimaryColourIndex].Trim;
                    break;
                case Colour::PeepTShirt:
                    if (vehicle != nullptr)
                    {
                        primaryColour = vehicle->peep_tshirt_colours[PrimaryColourIndex];
                    }
                    break;
            }

            switch (SecondaryColour)
            {
                case Colour::VehicleBody:
                    secondaryColour = ride.vehicle_colours[SecondaryColourIndex].Body;
                    break;
                case Colour::VehicleTrim:
                    secondaryColour = ride.vehicle_colours[SecondaryColourIndex].Trim;
                    break;
                case Colour::PeepTShirt:
                    if (vehicle != nullptr)
                    {
                        secondaryColour = vehicle->peep_tshirt_colours[SecondaryColourIndex];
                    }
                    break;
            }
            ImageId imageTemplate = ImageId(0, primaryColour, secondaryColour);

            if (ImageIdScheme.has_value())
            {
                ImageId imageFlags;
                switch (ImageIdScheme.value())
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

            if (type == PaintType::AddImageAsParent)
                PaintAddImageAsParent(session, imageTemplate.WithIndex(imageIndex), newOffset, newBoundBox);
            else if (type == PaintType::AddImageAsChild)
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), newOffset, newBoundBox);
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
    switch (Key.Element)
    {
        case TrackElemType::FlatTrack3x3:
            json["trackElement"] = "flat_track_3x3";
            break;
    }

    if (Key.Direction.has_value())
    {
        json["direction"] = Key.Direction.value();
    }

    if (Key.TrackSequence.has_value())
    {
        json["trackSequence"] = Key.TrackSequence.value();
    }

    if (Key.TrackSequenceMapping.has_value())
    {
        // just store the id
        json["trackSequenceMapping"] = Key.TrackSequenceMapping.value().Id;
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
