#include "PaintStructDescriptor.h"

#include "../core/Json.hpp"
#include "../entity/EntityRegistry.h"
#include "../object/PaintObject.h"
#include "../paint/Supports.h"
#include "../ride/Ride.h"
#include "../ride/Vehicle.h"

PaintStructDescriptor::PaintStructDescriptor()
    : PrimaryColour(Colour::VehicleBody)
    , PrimaryColourIndex(0)
    , SecondaryColour(Colour::VehicleTrim)
    , SecondaryColourIndex(0)
    , ImageIdOffsetIndex(0)
    , ImageIdOffset(nullptr)
    , HeightSupports(nullptr)
    , ImageId(PaintStructDescriptor::ImageIdBase::Car0)
    , BoundBoxTable(nullptr)
{
}

bool operator==(const VehicleKey& lhs, const VehicleKey& rhs)
{
    return lhs.SpriteDirection == rhs.SpriteDirection && lhs.Pitch == rhs.Pitch && lhs.NumPeeps == rhs.NumPeeps;
}

const std::optional<uint32_t>& PaintStructKey::Get(uint32_t location) const
{
    return *_fields[location];
}

void PaintStructKey::Set(uint32_t location, uint32_t value)
{
    *_fields[location] = value;
}

void PaintStructDescriptor::Paint(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, const PaintStructKey& key, const Vehicle* vehicle) const
{
    // if (!Key.MatchWithKeys(trackElement.GetTrackType(), direction, trackSequence, vehicle))
    // return;

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

    uint8_t edges = GetEdges(trackElement.GetTrackType(), trackSequence);

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

    if (PaintCode.has_value())
    {
        auto type = PaintCode.value();
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

            ::ImageId imageTemplate;
            if (SecondaryColour.has_value())
            {
                switch (SecondaryColour.value())
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
                imageTemplate = ::ImageId(0, primaryColour, secondaryColour);
            }
            else
            {
                imageTemplate = ::ImageId(0, primaryColour);
            }
            

            if (ImageIdScheme.has_value())
            {
                ::ImageId imageFlags;
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
            switch (ImageId)
            {
                case ImageIdBase::Car0:
                default:
                    imageIndex = rideEntry->Cars[0].base_image_id;
                    break;
            }

            if (ImageIdOffset != nullptr)
            {
                if (ImageIdOffset->Entries != nullptr)
                {
                    auto offset = ImageIdOffset->Entries->Get(key);
                    if (offset != nullptr)
                    {
                        if (offset->size() > ImageIdOffsetIndex)
                            imageIndex = imageIndex + (*offset)[ImageIdOffsetIndex];
                    }
                        

                    if (BoundBoxTable != nullptr)
                    {
                        auto boundBoxes = BoundBoxTable->Values->Get(key);
                        if (boundBoxes != nullptr)
                        {
                            for (const auto& bb : *boundBoxes)
                            {
                                auto Offset = bb->Coords;
                                auto newOffset = Offset + CoordsXYZ{ 0, 0, height };
                                auto newBoundBox = bb->Boundbox;
                                newBoundBox.offset.z += height;

                                if (type == PaintType::AddImageAsParent)
                                    PaintAddImageAsParent(session, imageTemplate.WithIndex(imageIndex), newOffset, newBoundBox);
                                else if (type == PaintType::AddImageAsChild)
                                    PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), newOffset, newBoundBox);
                            }

                        }
                    }
                }
            }
        }

        uint32_t segments = 0;
        if (HeightSupports != nullptr)
        {
            if (HeightSupports->Segments.find(trackSequence) != HeightSupports->Segments.end())
            {
                segments = HeightSupports->Segments.at(trackSequence);
            }
            PaintUtilSetSegmentSupportHeight(session, segments, height + 2, 0x20);
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL & ~segments, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + HeightSupports->HeightOffset, 0x20);
        }
    }
}

void PaintStructJson::FromJson(const json_t& paintStruct)
{
    if (paintStruct.contains("supports"))
    {
        auto supports = paintStruct["supports"];
        if (supports.is_string())
        {
            if (supports == "wooden_a")
                Supports = PaintStructDescriptor::SupportsType::WoodenA;
        }
    }

    if (paintStruct.contains("floor"))
    {
        auto floor = paintStruct["floor"];
        if (floor.is_string())
        {
            if (floor == "cork")
                Floor = PaintStructDescriptor::FloorType::Cork;
        }
    }

    if (paintStruct.contains("edges"))
    {
        auto edges = paintStruct["edges"];
        if (edges.is_string())
        {
            EdgesId = edges;
        }
    }

    if (paintStruct.contains("fences"))
    {
        auto fences = paintStruct["fences"];
        if (fences.is_string())
        {
            if (fences == "ropes")
                Fences = PaintStructDescriptor::FenceType::Ropes;
        }
    }

    if (paintStruct.contains("paintType"))
    {
        auto paintType = paintStruct["paintType"];
        if (paintType.is_string())
        {
            if (paintType == "addImageAsParent")
                PaintType = PaintStructDescriptor::PaintType::AddImageAsParent;
            else if (paintType == "addImageAsChild")
                PaintType = PaintStructDescriptor::PaintType::AddImageAsChild;
        }
    }

    if (paintStruct.contains("imageIdBase"))
    {
        auto imageIdBase = paintStruct["imageIdBase"];
        if (imageIdBase.is_string())
        {
            if (imageIdBase == "car0")
                ImageIdBase = PaintStructDescriptor::ImageIdBase::Car0;
        }
    }

    if (paintStruct.contains("primaryColour"))
    {
        auto primaryColour = paintStruct["primaryColour"];
        if (primaryColour.is_string())
        {
            if (primaryColour == "vehicleBody")
                PrimaryColour = PaintStructDescriptor::Colour::VehicleBody;
            else if (primaryColour == "vehicleTrim")
                PrimaryColour = PaintStructDescriptor::Colour::VehicleTrim;
            else if (primaryColour == "peepTShirt")
                PrimaryColour = PaintStructDescriptor::Colour::PeepTShirt;
        }
    }

    if (paintStruct.contains("secondaryColour"))
    {
        auto secondaryColour = paintStruct["secondaryColour"];
        if (secondaryColour.is_string())
        {
            if (secondaryColour == "vehicleBody")
                SecondaryColour = PaintStructDescriptor::Colour::VehicleBody;
            else if (secondaryColour == "vehicleTrim")
                SecondaryColour = PaintStructDescriptor::Colour::VehicleTrim;
            else if (secondaryColour == "peepTShirt")
                SecondaryColour = PaintStructDescriptor::Colour::PeepTShirt;
        }
    }

    if (paintStruct.contains("primaryColourIndex"))
    {
        auto primaryColourIndex = paintStruct["primaryColourIndex"];
        if (primaryColourIndex.is_number())
            PrimaryColourIndex = primaryColourIndex;
    }

    if (paintStruct.contains("secondaryColourIndex"))
    {
        auto secondaryColourIndex = paintStruct["secondaryColourIndex"];
        if (secondaryColourIndex.is_number())
            SecondaryColourIndex = secondaryColourIndex;
    }

    if (paintStruct.contains("imageIdOffset"))
    {
        auto imageIdOffset = paintStruct["imageIdOffset"];
        if (imageIdOffset.is_string())
        {
            ImageIdOffsetId = imageIdOffset;
        }
    }

    if (paintStruct.contains("imageIdOffsetIndex"))
    {
        auto imageIdOffsetIndex = paintStruct["imageIdOffsetIndex"];
        if (imageIdOffsetIndex.is_number())
            ImageIdOffsetIndex = imageIdOffsetIndex;
    }

    if (paintStruct.contains("imageIdScheme"))
    {
        auto imageIdScheme = paintStruct["imageIdScheme"];
        if (imageIdScheme.is_string())
        {
            if (imageIdScheme == "misc")
                ImageIdScheme = PaintStructDescriptor::Scheme::Misc;
        }
    }

    if (paintStruct.contains("boundBoxId"))
    {
        BoundBoxId = paintStruct["boundBoxId"];
    }

    if (paintStruct.contains("supportsHeightId"))
    {
        HeightSupportsId = paintStruct["supportsHeightId"];
    }
}

void PaintStructKey::FromJson(const json_t& paintStruct)
{
    if (paintStruct.contains("trackElement"))
    {
        Element = PaintStructDescriptor::TrackElemTypeFromString(paintStruct["trackElement"]);
    }

    if (paintStruct.contains("trackSequence"))
    {
        auto trackSequence = paintStruct["trackSequence"];
        if (trackSequence.is_number())
        {
            TrackSequence = trackSequence;
        }
    }

    if (paintStruct.contains("direction"))
    {
        auto direction = paintStruct["direction"];
        if (direction.is_number())
        {
            Direction = direction;
        }
    }

    if (paintStruct.contains("vehicleSpriteDirection"))
    {
        auto vehicleSpriteDirection = paintStruct["vehicleSpriteDirection"];
        if (vehicleSpriteDirection.is_number())
            VehicleSpriteDirection[0] = vehicleSpriteDirection;
    }

    if (paintStruct.contains("vehiclePitch"))
    {
        auto vehiclePitch = paintStruct["vehiclePitch"];
        if (vehiclePitch.is_number())
            VehiclePitch[0] = vehiclePitch;
    }

    if (paintStruct.contains("vehicleNumPeeps"))
    {
        auto vehicleNumPeeps = paintStruct["vehicleNumPeeps"];
        if (vehicleNumPeeps.is_number())
            VehicleNumPeeps[0] = vehicleNumPeeps;
    }

    if (paintStruct.contains("sessionCurrentRotation"))
    {
        SessionCurrentRotation = paintStruct["sessionCurrentRotation"];
    }

    if (paintStruct.contains("vehicleRestraintsPosition"))
    {
        VehicleRestraintsPosition[0] = paintStruct["vehicleRestraintsPosition"];
    }
}

void ImageIdOffsetJson::FromJson(const json_t& imageIdOffset)
{
    Id = imageIdOffset["id"];

    const auto& entries = imageIdOffset["entries"];
    if (entries.is_array())
    {
        for (const auto& entry : entries)
        {
            PaintStructKey keyJson;
            keyJson.FromJson(entry);
            Keys.push_back(keyJson);

            const auto& table = entry["imageIdOffset"];
            std::vector<uint32_t> imageIds;

            for (const auto& elem : table)
                imageIds.push_back(elem);
            Values.push_back(imageIds);
        }
    }
}

PaintStructDescriptor PaintStructJson::Value() const
{
    PaintStructDescriptor result;
    result.Supports = Supports;
    result.Floor = Floor;
    result.Fences = Fences;
    result.PaintCode = PaintType;
    result.ImageIdScheme = ImageIdScheme;
    result.ImageId = ImageIdBase;
    result.PrimaryColour = PrimaryColour;
    result.PrimaryColourIndex = PrimaryColourIndex;
    result.SecondaryColour = SecondaryColour;
    result.SecondaryColourIndex = SecondaryColourIndex;

    auto it1 = _object._imageIdOffsetMapping.find(ImageIdOffsetId);
    if (it1 != _object._imageIdOffsetMapping.end())
        result.ImageIdOffset = &it1->second;

    result.ImageIdOffsetIndex = ImageIdOffsetIndex;

    auto it3 = _object._boundBoxMapping.find(BoundBoxId);
    if (it3 != _object._boundBoxMapping.end())
        result.BoundBoxTable = &it3->second;

    auto it2 = _object._heightMapping.find(HeightSupportsId);
    if (it2 != _object._heightMapping.end())
        result.HeightSupports = &it2->second;
    return result;
}

constexpr const uint8_t PaintStructDescriptor::GetEdges(track_type_t element, uint8_t trackSequence) const
{
    switch (element)
    {
        case TrackElemType::FlatTrack4x4:
            return edges_4x4[trackSequence];
        case TrackElemType::FlatTrack3x3:
            return edges_3x3[trackSequence];
        case TrackElemType::FlatTrack2x2:
            return edges_2x2[trackSequence];
        default:
            return 0;
    }
}

track_type_t PaintStructDescriptor::TrackElemTypeFromString(const std::string& elem)
{
    if (elem == "flat_track_3x3")
        return TrackElemType::FlatTrack3x3;
    else if (elem == "flat_track_4x4")
        return TrackElemType::FlatTrack4x4;
    else if (elem == "flat_track_2x2")
        return TrackElemType::FlatTrack2x2;
    return TrackElemType::None;
}
