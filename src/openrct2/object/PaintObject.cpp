#include "PaintObject.h"

#include "../paint/Paint.h"
#include "../paint/Supports.h"
#include "../ride/Ride.h"
#include "../ride/TrackPaint.h"
#include "../core/Json.hpp"
#include <map>

void PaintObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    //read the track sequence mapping tables and edges tables first
    std::map<std::string, PaintStructSequenceMapping> paintMapping;
    std::map<std::string, PaintStructEdgesTable> edgesMapping;

    auto trackSequenceTables = root["trackSequenceTables"];
    if (trackSequenceTables.is_array())
    {
        for (const auto& trackSequenceTable : trackSequenceTables)
        {
            PaintStructSequenceMapping table;
            auto dir0 = trackSequenceTable["dir0"];
            if (dir0.is_array())
            {
                for (const auto& sequence : dir0)
                    table[0].push_back(sequence);
            }

            auto dir1 = trackSequenceTable["dir1"];
            if (dir1.is_array())
            {
                for (const auto& sequence : dir1)
                    table[1].push_back(sequence);
            }

            auto dir2 = trackSequenceTable["dir2"];
            if (dir2.is_array())
            {
                for (const auto& sequence : dir2)
                    table[2].push_back(sequence);
            }

            auto dir3 = trackSequenceTable["dir3"];
            if (dir3.is_array())
            {
                for (const auto& sequence : dir3)
                    table[3].push_back(sequence);
            }

            auto id = trackSequenceTable["id"];
            paintMapping[id] = table;
        }
    }

    auto edgesTables = root["edgesTables"];
    if (edgesTables.is_array())
    {
        for (const auto& edgeTable : edgesTables)
        {
            PaintStructEdgesTable table;

            auto edges = edgeTable["edges"];
            if (edges.is_array())
            {
                for (const auto& edge : edges)
                {
                    if (edge == "none")
                        table.push_back(static_cast<edge_t>(0));
                    else if (edge == "ne_nw")
                        table.push_back(static_cast<edge_t>(EDGE_NE | EDGE_NW));
                    else if (edge == "ne")
                        table.push_back(EDGE_NE);
                    else if (edge == "ne_se")
                        table.push_back(static_cast<edge_t>(EDGE_NE | EDGE_SE));
                    else if (edge == "nw")
                        table.push_back(EDGE_NW);
                    else if (edge == "se")
                        table.push_back(EDGE_SE);
                    else if (edge == "sw_nw")
                        table.push_back(static_cast<edge_t>(EDGE_SW | EDGE_NW));
                    else if (edge == "sw_se")
                        table.push_back(static_cast<edge_t>(EDGE_SW | EDGE_SE));
                    else if (edge == "sw")
                        table.push_back(EDGE_SW);
                }
            }

            auto id = edgeTable["id"];
            edgesMapping[id] = table;
        }
    }

    auto paintStructs = root["paintStructs"];
    if (paintStructs.is_array())
    {
        for (const auto& paintStruct : paintStructs)
        {
            PaintStruct paint;
            auto trackElement = paintStruct["trackElement"];
            if (trackElement == "flat_track_3x3")
                paint.Element = TrackElemType::FlatTrack3x3;

            auto supports = paintStruct["supports"];
            if (supports.is_string())
            {
                if (supports == "wooden_a")
                    paint.Supports = SupportsType::WoodenA;
            }

            auto floor = paintStruct["floor"];
            if (floor.is_string())
            {
                if (floor == "cork")
                    paint.Floor = FloorType::Cork;
            }

            auto edges = paintStruct["edges"];
            if (edges.is_string())
            {
                if (edgesMapping.find(edges) != edgesMapping.end())
                    paint.Edges = edgesMapping[edges];
            }

            auto fences = paintStruct["fences"];
            if (fences.is_string())
            {
                if (fences == "ropes")
                    paint.Fences = FenceType::Ropes;
            }

            auto trackSequence = paintStruct["trackSequence"];
            if (trackSequence.is_number())
            {
                paint.TrackSequence = trackSequence;
            }

            auto trackSequenceMapping = paintStruct["trackSequenceMapping"];
            if (trackSequenceMapping.is_string())
            {
                if (paintMapping.find(trackSequenceMapping) != paintMapping.end())
                    paint.TrackSequenceMapping = paintMapping[trackSequenceMapping];
            }

            auto direction = paintStruct["direction"];
            if (direction.is_number())
            {
                paint.Direction = direction;
            }

            auto paintType = paintStruct["paintType"];
            if (paintType.is_string())
            {
                if (paintType == "addImageAsParent")
                    paint.PaintType = PaintType::AddImageAsParent;
            }

            auto imageIdBase = paintStruct["imageIdBase"];
            if (imageIdBase.is_string())
            {
                if (imageIdBase == "car0")
                    paint.ImageIdBase = ImageIdBase::Car0;
            }

            auto imageIdOffset = paintStruct["imageIdOffset"];
            if (imageIdOffset.is_number())
            {
                paint.ImageIdOffset = imageIdOffset;
            }

            auto imageIdScheme = paintStruct["imageIdScheme"];
            if (imageIdScheme.is_string())
            {
                if (imageIdScheme == "misc")
                    paint.ImageIdScheme = Scheme::Misc;
            }

            auto offsetX = paintStruct["offset_x"];
            if (offsetX.is_number())
                paint.Offset.x = offsetX;

            auto offsetY = paintStruct["offset_y"];
            if (offsetY.is_number())
                paint.Offset.y = offsetY;

            auto offsetZ = paintStruct["offset_z"];
            if (offsetZ.is_number())
                paint.Offset.z = offsetZ;


            auto bbOffsetX = paintStruct["bb_offset_x"];
            if (bbOffsetX.is_number())
                paint.BoundBox.offset.x = bbOffsetX;

            auto bbOffsetY = paintStruct["bb_offset_y"];
            if (bbOffsetY.is_number())
                paint.BoundBox.offset.y = bbOffsetY;

            auto bbOffsetZ = paintStruct["bb_offset_z"];
            if (bbOffsetZ.is_number())
                paint.BoundBox.offset.z = bbOffsetZ;

            auto bbLengthX = paintStruct["bb_length_x"];
            if (bbLengthX.is_number())
                paint.BoundBox.length.x = bbLengthX;

            auto bbLengthY = paintStruct["bb_length_y"];
            if (bbLengthY.is_number())
                paint.BoundBox.length.y = bbLengthY;

            auto bbLengthZ = paintStruct["bb_length_z"];
            if (bbLengthZ.is_number())
                paint.BoundBox.length.z = bbLengthZ;

            _paintStructs.push_back(paint);
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
