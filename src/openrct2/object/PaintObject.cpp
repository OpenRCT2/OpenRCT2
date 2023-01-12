#include "PaintObject.h"

#include "../Context.h"
#include "../core/Console.hpp"
#include "../core/Json.hpp"
#include "../entity/EntityRegistry.h"
#include "../paint/Paint.h"
#include "../paint/PaintStructKeyGenerator.h"
#include "../paint/Supports.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/TrackPaint.h"
#include "ObjectManager.h"
#include "ObjectRepository.h"

PaintObject::PaintObject()
    : _paintStructsTree(_keyGen)
{
}

void PaintObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    try
    {
        if (root.contains("trackSequenceTables"))
        {
            auto trackSequenceTables = root["trackSequenceTables"];
            if (trackSequenceTables.is_array())
            {
                for (const auto& trackSequenceTable : trackSequenceTables)
                {
                    PaintStructSequenceMapping table;
                    auto sequences = trackSequenceTable["sequences"];
                    if (sequences.is_array())
                    {
                        size_t index = 0;
                        for (const auto& sequence : sequences)
                        {
                            if (sequence.is_array())
                            {
                                for (const auto& value : sequence)
                                {
                                    if (value.is_number())
                                        table[index].push_back(value);
                                }
                            }
                            index++;
                        }
                    }

                    const auto& trackElement = trackSequenceTable["trackElement"];
                    track_type_t element = 0;
                    if (trackElement == "flat_track_3x3")
                        element = TrackElemType::FlatTrack3x3;
                    _sequenceMappings[element] = table;
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
                    PaintStructEdgesTable table;

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
                    _edgeMappings[id] = table;
                }
            }
        }

        if (root.contains("heightSupportsTables"))
        {
            auto heightTables = root["heightSupportsTables"];
            if (heightTables.is_array())
            {
                for (const auto& heightTable : heightTables)
                {
                    HeightSupportsTable table;
                    table.Id = heightTable["id"];
                    table.HeightOffset = heightTable["heightOffset"];

                    const auto& segments = heightTable["segments"];
                    for (const auto& segment : segments)
                    {
                        uint32_t trackSequence = segment["trackSequence"];
                        uint32_t edge = 0;

                        const auto& values = segment["values"];
                        for (const auto& value : values)
                        {
                            if (value == "b4")
                                edge |= SEGMENT_B4;
                            else if (value == "cc")
                                edge |= SEGMENT_CC;
                            else if (value == "bc")
                                edge |= SEGMENT_BC;
                            else if (value == "d4")
                                edge |= SEGMENT_D4;
                            else if (value == "c0")
                                edge |= SEGMENT_C0;
                            else if (value == "d0")
                                edge |= SEGMENT_D0;
                            else if (value == "b8")
                                edge |= SEGMENT_B8;
                            else if (value == "c8")
                                edge |= SEGMENT_C8;
                            else if (value == "c4")
                                edge |= SEGMENT_C4;
                        }

                        table.Segments[trackSequence] = edge;
                    }
                    _heightMapping[table.Id] = table;
                }
            }
        }

        auto imageIdOffsets = root["imageIdOffsets"];
        if (imageIdOffsets.is_array())
        {
            for (const auto& imageIdOffset : imageIdOffsets)
            {
                ImageIdOffset offset(_keyGen);
                offset.Id = imageIdOffset["id"];

                const auto& entries = imageIdOffset["entries"];
                if (entries.is_array())
                {
                    std::vector<PaintStructKeyJson> keysJson;
                    std::vector<std::vector<uint32_t>> imageIdTables;
                    for (const auto& entry : entries)
                    {
                        PaintStructDescriptorKey key;
                        PaintStructKeyJson keyJson;
                        keyJson.FromJson(entry);
                        keysJson.push_back(keyJson);

                        const auto& table = entry["imageIdOffset"];
                        std::vector<uint32_t> imageIds;

                        for (const auto& elem : table)
                            imageIds.push_back(elem);
                        imageIdTables.push_back(imageIds);
                        for (const auto& imageId : imageIds)
                            offset.Entries.Add(key, imageId);
                    }

                    _keyGen.Initialize(keysJson);
                    for (size_t index = 0; index < keysJson.size(); index++)
                    {
                        const auto& keyJson = keysJson[index];
                        auto keys = _keyGen.GenerateKeys(keyJson);
                        for (const auto& key : keys)
                        {
                            const auto& imageIdTable = imageIdTables[index];
                            for (const auto& imageId : imageIdTable)
                                offset.Entries.Add(key, imageId);
                        }
                    }
                }
                _imageIdOffsetMapping[offset.Id] = std::make_shared<ImageIdOffset>(offset);
            }
        }

        if (root.contains("vehicleIndices"))
        {
            const auto& vehicleIndices = root["vehicleIndices"];
            if (vehicleIndices.is_array())
            {
                for (const auto& vehicleIndex : vehicleIndices)
                {
                    _vehicleIndices.push_back(vehicleIndex);
                }
            }
        }

        auto paintStructs = root["paintStructs"];
        if (paintStructs.is_array())
        {
            std::vector<PaintStructKeyJson> keysJson;

            for (const auto& paintStruct : paintStructs)
            {
                PaintStructDescriptor paint;
                PaintStructDescriptorKey key;
                PaintStructKeyJson keyJson;
                keyJson.FromJson(paintStruct);

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
                        if (_edgeMappings.find(edges) != _edgeMappings.end())
                        {
                            paint.Edges = &_edgeMappings[edges];
                        }
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

                if (paintStruct.contains("paintType"))
                {
                    auto paintType = paintStruct["paintType"];
                    if (paintType.is_string())
                    {
                        if (paintType == "addImageAsParent")
                            paint.PaintType = PaintStructDescriptor::PaintType::AddImageAsParent;
                        else if (paintType == "addImageAsChild")
                            paint.PaintType = PaintStructDescriptor::PaintType::AddImageAsChild;
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

                if (paintStruct.contains("primaryColour"))
                {
                    auto primaryColour = paintStruct["primaryColour"];
                    if (primaryColour.is_string())
                    {
                        if (primaryColour == "vehicleBody")
                            paint.PrimaryColour = PaintStructDescriptor::Colour::VehicleBody;
                        else if (primaryColour == "vehicleTrim")
                            paint.PrimaryColour = PaintStructDescriptor::Colour::VehicleTrim;
                        else if (primaryColour == "peepTShirt")
                            paint.PrimaryColour = PaintStructDescriptor::Colour::PeepTShirt;
                    }
                }

                if (paintStruct.contains("secondaryColour"))
                {
                    auto secondaryColour = paintStruct["secondaryColour"];
                    if (secondaryColour.is_string())
                    {
                        if (secondaryColour == "vehicleBody")
                            paint.SecondaryColour = PaintStructDescriptor::Colour::VehicleBody;
                        else if (secondaryColour == "vehicleTrim")
                            paint.SecondaryColour = PaintStructDescriptor::Colour::VehicleTrim;
                        else if (secondaryColour == "peepTShirt")
                            paint.SecondaryColour = PaintStructDescriptor::Colour::PeepTShirt;
                    }
                }

                if (paintStruct.contains("primaryColourIndex"))
                {
                    auto primaryColourIndex = paintStruct["primaryColourIndex"];
                    if (primaryColourIndex.is_number())
                        paint.PrimaryColourIndex = primaryColourIndex;
                }

                if (paintStruct.contains("secondaryColourIndex"))
                {
                    auto secondaryColourIndex = paintStruct["secondaryColourIndex"];
                    if (secondaryColourIndex.is_number())
                        paint.SecondaryColourIndex = secondaryColourIndex;
                }

                if (paintStruct.contains("imageIdOffset"))
                {
                    auto imageIdOffset = paintStruct["imageIdOffset"];
                    if (imageIdOffset.is_string())
                    {
                        paint.ImageIdOffset = _imageIdOffsetMapping[imageIdOffset].get();
                    }
                }

                if (paintStruct.contains("imageIdOffsetIndex"))
                {
                    auto imageIdOffsetIndex = paintStruct["imageIdOffsetIndex"];
                    if (imageIdOffsetIndex.is_number())
                        paint.ImageIdOffsetIndex = imageIdOffsetIndex;
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

                if (paintStruct.contains("supportsHeightId"))
                {
                    const auto& id = paintStruct["supportsHeightId"];
                    if (_heightMapping.find(id) != _heightMapping.end())
                    {
                        paint.HeightSupports = &_heightMapping[id];
                    }
                }

                // we need to check if the key exists
                //_keys.push_back(key);
                keysJson.push_back(keyJson);
                _paintStructs.push_back(paint);
            }

            //auto keyGen = PaintStructKeyGenerator(keysJson);
            for (size_t index = 0; index < keysJson.size(); index++)
            {
                const auto& keyJson = keysJson[index];
                _keyGen.Initialize(keysJson);
                auto keys = _keyGen.GenerateKeys(keyJson);

                for (const auto& key : keys)
                {
                    const auto& paintStruct = _paintStructs[index];
                    auto ptr = std::make_shared<PaintStructDescriptor>(paintStruct);
                    _paintStructsTree.Add(key, ptr);
                }
            }
        }
    }
    catch (JsonException& e)
    {
        Console::Error::WriteLine(e.what());
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
    // for (const auto& paintStruct : _paintStructs)
    // paintStruct.Paint(session, ride, trackSequence, direction, height, trackElement);

    PaintStructDescriptorKey key;
    key.Direction = direction;
    key.Element = trackElement.GetTrackType();

    auto it = _sequenceMappings.find(key.Element);
    if (it != _sequenceMappings.end())
        trackSequence = it->second[direction][trackSequence];
    key.TrackSequence = trackSequence;

    // check the first vehicle in the list
    // to-do: add vehicle state variables in the key for every vehicle index in the list
    Vehicle* vehicle = nullptr;
    if (!_vehicleIndices.empty())
    {
        // first, check if the paint struct key matches with the call
        if (ride.lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
        {
            vehicle = GetEntity<Vehicle>(ride.vehicles[_vehicleIndices[0]]);

            session.InteractionType = ViewportInteractionItem::Entity;
            session.CurrentlyDrawnEntity = vehicle;
        }
    }

    // to-do: in the future, add a key val for every vehicle index, not just the first in the list
    if (vehicle != nullptr)
    {
        key.VehicleKey[0].NumPeeps = vehicle->num_peeps;
        key.VehicleKey[0].Pitch = vehicle->Pitch;
        key.VehicleKey[0].SpriteDirection = vehicle->sprite_direction;
    }

    auto paintStructs = _paintStructsTree.Get(key);
    if (paintStructs != nullptr)
    {
        for (const auto paintStruct : *paintStructs)
        {
            paintStruct->Paint(session, ride, trackSequence, direction, height, trackElement, key, vehicle);
        }
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
            if (repoItem != nullptr)
            {
                auto test = repoManager.LoadObject(repoItem);

                if (repoItem->LoadedObject == nullptr)
                {
                    repoManager.RegisterLoadedObject(repoItem, std::move(test));

                    if (repoItem->LoadedObject != nullptr)
                        repoItem->LoadedObject->Load();
                }
            }
        }
    }
}
