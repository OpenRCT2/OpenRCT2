#include "PaintObject.h"

#include "../Context.h"
#include "../core/Json.hpp"
#include "../paint/Paint.h"
#include "../paint/Supports.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/TrackPaint.h"
#include "ObjectManager.h"
#include "ObjectRepository.h"

#include <map>

void PaintObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    // read the track sequence mapping tables and edges tables first
    std::map<std::string, PaintStructDescriptor::PaintStructSequenceMapping> paintMapping;
    std::map<std::string, PaintStructDescriptor::PaintStructEdgesTable> edgesMapping;

    if (root.contains("trackSequenceTables"))
    {
        auto trackSequenceTables = root["trackSequenceTables"];
        if (trackSequenceTables.is_array())
        {
            for (const auto& trackSequenceTable : trackSequenceTables)
            {
                PaintStructDescriptor::PaintStructSequenceMapping table;
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
    }

    if (root.contains("edgesTables"))
    {
        auto edgesTables = root["edgesTables"];
        if (edgesTables.is_array())
        {
            for (const auto& edgeTable : edgesTables)
            {
                PaintStructDescriptor::PaintStructEdgesTable table;

                auto edges = edgeTable["edges"];
                if (edges.is_array())
                {
                    for (const auto& edge : edges)
                    {
                        uint32_t result = 0;
                        if (edge.is_array())
                        {
                            for (const auto& side : edge)
                            {
                                if (side == "ne")
                                    result |= EDGE_NE;
                                else if (side == "se")
                                    result |= EDGE_SE;
                                else if (side == "sw")
                                    result |= EDGE_SW;
                                else if (side == "nw")
                                    result |= EDGE_NW;
                            }
                        }
                        table.push_back(static_cast<edge_t>(result));
                    }
                }

                auto id = edgeTable["id"];
                edgesMapping[id] = table;
            }
        }
    }

    auto paintStructs = root["paintStructs"];
    if (paintStructs.is_array())
    {
        for (const auto& paintStruct : paintStructs)
        {
            PaintStructDescriptor paint;
            if (paintStruct.contains("trackElement"))
            {
                auto trackElement = paintStruct["trackElement"];
                if (trackElement == "flat_track_3x3")
                    paint.Element = TrackElemType::FlatTrack3x3;
            }

            if (paintStruct.contains("supports"))
            {
                auto supports = paintStruct["supports"];
                if (supports.is_string())
                {
                    if (supports == "wooden_a")
                        paint.Supports = PaintStructDescriptor::SupportsType::WoodenA;
                }
            }

            if (paintStruct.contains("floor"))
            {
                auto floor = paintStruct["floor"];
                if (floor.is_string())
                {
                    if (floor == "cork")
                        paint.Floor = PaintStructDescriptor::FloorType::Cork;
                }
            }

            if (paintStruct.contains("edges"))
            {
                auto edges = paintStruct["edges"];
                if (edges.is_string())
                {
                    if (edgesMapping.find(edges) != edgesMapping.end())
                        paint.Edges = edgesMapping[edges];
                }
            }

            if (paintStruct.contains("fences"))
            {
                auto fences = paintStruct["fences"];
                if (fences.is_string())
                {
                    if (fences == "ropes")
                        paint.Fences = PaintStructDescriptor::FenceType::Ropes;
                }
            }

            if (paintStruct.contains("trackSequence"))
            {
                auto trackSequence = paintStruct["trackSequence"];
                if (trackSequence.is_number())
                {
                    paint.TrackSequence = trackSequence;
                }
            }

            if (paintStruct.contains("trackSequenceMapping"))
            {
                auto trackSequenceMapping = paintStruct["trackSequenceMapping"];
                if (trackSequenceMapping.is_string())
                {
                    if (paintMapping.find(trackSequenceMapping) != paintMapping.end())
                        paint.TrackSequenceMapping = paintMapping[trackSequenceMapping];
                }
            }

            if (paintStruct.contains("direction"))
            {
                auto direction = paintStruct["direction"];
                if (direction.is_number())
                {
                    paint.Direction = direction;
                }
            }

            if (paintStruct.contains("paintType"))
            {
                auto paintType = paintStruct["paintType"];
                if (paintType.is_string())
                {
                    if (paintType == "addImageAsParent")
                        paint.PaintType = PaintStructDescriptor::PaintType::AddImageAsParent;
                    else if (paintType == "setSegmentSupportHeight")
                        paint.PaintType = PaintStructDescriptor::PaintType::SetSegmentsSupportsHeight;
                }
            }

            if (paintStruct.contains("imageIdBase"))
            {
                auto imageIdBase = paintStruct["imageIdBase"];
                if (imageIdBase.is_string())
                {
                    if (imageIdBase == "car0")
                        paint.ImageIdBase = PaintStructDescriptor::ImageIdBase::Car0;
                }
            }

            if (paintStruct.contains("imageIdOffset"))
            {
                auto imageIdOffset = paintStruct["imageIdOffset"];
                if (imageIdOffset.is_number())
                {
                    paint.ImageIdOffset = imageIdOffset;
                }
            }

            if (paintStruct.contains("imageIdScheme"))
            {
                auto imageIdScheme = paintStruct["imageIdScheme"];
                if (imageIdScheme.is_string())
                {
                    if (imageIdScheme == "misc")
                        paint.ImageIdScheme = PaintStructDescriptor::Scheme::Misc;
                }
            }

            if (paintStruct.contains("offset_x"))
            {
                auto offsetX = paintStruct["offset_x"];
                if (offsetX.is_number())
                    paint.Offset.x = offsetX;
            }

            if (paintStruct.contains("offset_y"))
            {
                auto offsetY = paintStruct["offset_y"];
                if (offsetY.is_number())
                    paint.Offset.y = offsetY;
            }

            if (paintStruct.contains("offset_z"))
            {
                auto offsetZ = paintStruct["offset_z"];
                if (offsetZ.is_number())
                    paint.Offset.z = offsetZ;
            }

            if (paintStruct.contains("bb_offset_x"))
            {
                auto bbOffsetX = paintStruct["bb_offset_x"];
                if (bbOffsetX.is_number())
                    paint.BoundBox.offset.x = bbOffsetX;
            }

            if (paintStruct.contains("bb_offset_y"))
            {
                auto bbOffsetY = paintStruct["bb_offset_y"];
                if (bbOffsetY.is_number())
                    paint.BoundBox.offset.y = bbOffsetY;
            }

            if (paintStruct.contains("bb_offset_z"))
            {
                auto bbOffsetZ = paintStruct["bb_offset_z"];
                if (bbOffsetZ.is_number())
                    paint.BoundBox.offset.z = bbOffsetZ;
            }

            if (paintStruct.contains("bb_length_x"))
            {
                auto bbLengthX = paintStruct["bb_length_x"];
                if (bbLengthX.is_number())
                    paint.BoundBox.length.x = bbLengthX;
            }

            if (paintStruct.contains("bb_length_y"))
            {
                auto bbLengthY = paintStruct["bb_length_y"];
                if (bbLengthY.is_number())
                    paint.BoundBox.length.y = bbLengthY;
            }

            if (paintStruct.contains("bb_length_z"))
            {
                auto bbLengthZ = paintStruct["bb_length_z"];
                if (bbLengthZ.is_number())
                    paint.BoundBox.length.z = bbLengthZ;
            }

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
    const TrackElement& trackElement) const
{
    // iterate through the paint structs
    for (const auto& paintStruct : _paintStructs)
    {
        if (paintStruct.MatchWithKeys(trackElement.GetTrackType(), direction, trackSequence))
            paintStruct.Paint(session, ride, trackSequence, direction, height, trackElement);
    }
}

void PaintObject::LoadPaintObjects()
{
    auto& repoManager = OpenRCT2::GetContext()->GetObjectRepository();
    for (ObjectEntryIndex i = 0; i < std::size(RideTypeDescriptors); i++)
    {
        auto& rtd = GetRideTypeDescriptor(i);

        if (!rtd.PaintObjectId.empty())
        {
            auto repoItem = repoManager.FindObject(rtd.PaintObjectId);
            auto test = repoManager.LoadObject(repoItem);

            if (repoItem->LoadedObject == nullptr)
            {
                repoManager.RegisterLoadedObject(repoItem, std::move(test));
                repoItem->LoadedObject->Load();
            }
        }
    }
}
