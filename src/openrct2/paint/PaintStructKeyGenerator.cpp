#include "PaintStructKeyGenerator.h"
#include "../ride/TrackData.h"
#include "PaintStructDescriptor.h"

void PaintStructKeyJson::FromJson(const json_t& paintStruct)
{
    if (paintStruct.contains("trackElement"))
    {
        auto trackElement = paintStruct["trackElement"];
        if (trackElement == "flat_track_3x3")
            Element = TrackElemType::FlatTrack3x3;
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
}

PaintStructKeyGenerator::PaintStructKeyGenerator(const std::vector<PaintStructKeyJson>& keysJson)
{
    for (const auto& key : keysJson)
    {
        PushElement(_elements, key.Element);
        PushElement(_directions, key.Direction);
        PushElement(_trackSequences, key.TrackSequence);
        for (size_t index = 0; index < OpenRCT2::Limits::MaxTrainsPerRide + 1; index++)
        {
            PushElement(_vehicleSpriteDirections[index], key.VehicleSpriteDirection[index]);
            PushElement(_vehiclePitches[index], key.VehiclePitch[index]);
            PushElement(_vehicleNumPeeps[index], key.VehicleNumPeeps[index]);
        }
    }
}

std::vector<PaintStructDescriptorKey> PaintStructKeyGenerator::GenerateKeys(const PaintStructKeyJson& keyJson) const
{
    std::vector<uint32_t> elements;
    std::vector<uint32_t> directions;
    std::vector<uint32_t> trackSequences;

    VehicleParam vehicleSpriteDirections;
    VehicleParam vehiclePitches;
    VehicleParam vehicleNumPeeps;

    GenerateKeyField(elements, keyJson.Element, _elements);
    GenerateKeyField(directions, keyJson.Direction, _directions);
    GenerateKeyField(trackSequences, keyJson.TrackSequence, _trackSequences);

    for (size_t index = 0; index < OpenRCT2::Limits::MaxTrainsPerRide + 1; index++)
    {
        GenerateKeyField(
            vehicleSpriteDirections[index], keyJson.VehicleSpriteDirection[index], _vehicleSpriteDirections[index]);
        GenerateKeyField(vehiclePitches[index], keyJson.VehiclePitch[index], _vehiclePitches[index]);
        GenerateKeyField(vehicleNumPeeps[index], keyJson.VehicleNumPeeps[index], _vehicleNumPeeps[index]);
    }

    std::vector<PaintStructKeyJson> oldKeys, newKeys;
    newKeys.push_back(keyJson);

    oldKeys = newKeys;
    SetKeyField(KeyType::Element, keyJson, oldKeys, keyJson.Element, elements, newKeys);

    oldKeys = newKeys;
    SetKeyField(KeyType::Direction, keyJson, oldKeys, keyJson.Direction, directions, newKeys);

    oldKeys = newKeys;
    SetKeyField(KeyType::TrackSequence, keyJson, oldKeys, keyJson.TrackSequence, trackSequences, newKeys);

    for (uint32_t index = 0; index < OpenRCT2::Limits::MaxTrainsPerRide + 1; index++)
    {
        oldKeys = newKeys;
        SetKeyField(
            KeyType::VehicleNumPeeps, index, keyJson, oldKeys, keyJson.VehicleNumPeeps[index], vehicleNumPeeps[index], newKeys);

        oldKeys = newKeys;
        SetKeyField(
            KeyType::VehiclePitch, index, keyJson, oldKeys, keyJson.VehiclePitch[index], vehiclePitches[index], newKeys);

        oldKeys = newKeys;
        SetKeyField(
            KeyType::VehicleSpriteDirection, index, keyJson, oldKeys, keyJson.VehicleSpriteDirection[index],
            vehicleSpriteDirections[index], newKeys);
    }

    // now, every key in newkeys should have all optionals completed, lets convert that to actual keys
    std::vector<PaintStructDescriptorKey> result;
    for (const auto& keyJ : newKeys)
    {
        PaintStructDescriptorKey key;
        key.Element = keyJ.Element.value();
        key.Direction = keyJ.Direction.value();
        key.TrackSequence = keyJ.TrackSequence.value();

        for (size_t index = 0; index < OpenRCT2::Limits::MaxTrainsPerRide + 1; index++)
        {
            key.VehicleKey[index].NumPeeps = keyJ.VehicleNumPeeps[index].value();
            key.VehicleKey[index].Pitch = keyJ.VehiclePitch[index].value();
            key.VehicleKey[index].SpriteDirection = keyJ.VehicleSpriteDirection[index].value();
        }
        result.push_back(key);
    }

    return result;
}

void PaintStructKeyGenerator::SetKeyField(
    KeyType keyType, const PaintStructKeyJson& keyJson, const std::vector<PaintStructKeyJson>& oldKeys,
    const std::optional<uint32_t>& value, const std::vector<uint32_t>& values, std::vector<PaintStructKeyJson>& newKeys) const
{
    newKeys = std::vector<PaintStructKeyJson>();

    for (const auto& key : oldKeys)
    {
        if (value.has_value())
        {
            auto newKey = key;
            switch (keyType)
            {
                case KeyType::Element:
                    newKey.Element = value.value();
                    break;
                case KeyType::Direction:
                    newKey.Direction = value.value();
                    break;
                case KeyType::TrackSequence:
                    newKey.TrackSequence = value.value();
                    break;
            }
            newKeys.push_back(newKey);
        }
        else
        {
            for (const auto& val : values)
            {
                auto newKey = key;
                switch (keyType)
                {
                    case KeyType::Element:
                        newKey.Element = val;
                        break;
                    case KeyType::Direction:
                        newKey.Direction = val;
                        break;
                    case KeyType::TrackSequence:
                        newKey.TrackSequence = val;
                        break;
                }
                newKeys.push_back(newKey);
            }
        }
    }
}

void PaintStructKeyGenerator::SetKeyField(
    KeyType keyType, uint32_t vehicleIndex, const PaintStructKeyJson& keyJson, const std::vector<PaintStructKeyJson>& oldKeys,
    const std::optional<uint32_t>& value, const std::vector<uint32_t>& values, std::vector<PaintStructKeyJson>& newKeys) const
{
    newKeys = std::vector<PaintStructKeyJson>();

    for (const auto& key : oldKeys)
    {
        if (value.has_value())
        {
            auto newKey = key;
            switch (keyType)
            {
                case KeyType::VehicleNumPeeps:
                    newKey.VehicleNumPeeps[vehicleIndex] = value.value();
                    break;
                case KeyType::VehiclePitch:
                    newKey.VehiclePitch[vehicleIndex] = value.value();
                    break;
                case KeyType::VehicleSpriteDirection:
                    newKey.VehicleSpriteDirection[vehicleIndex] = value.value();
                    break;
            }
            newKeys.push_back(newKey);
        }
        else
        {
            for (const auto& val : values)
            {
                auto newKey = key;
                switch (keyType)
                {
                    case KeyType::VehicleNumPeeps:
                        newKey.VehicleNumPeeps[vehicleIndex] = val;
                        break;
                    case KeyType::VehiclePitch:
                        newKey.VehiclePitch[vehicleIndex] = val;
                        break;
                    case KeyType::VehicleSpriteDirection:
                        newKey.VehicleSpriteDirection[vehicleIndex] = val;
                        break;
                }
                newKeys.push_back(newKey);
            }
        }
    }
}

void PaintStructKeyGenerator::PushElement(std::vector<uint32_t>& vector, const std::optional<uint32_t>& value)
{
    if (value.has_value())
    {
        if (!vector.empty())
        {
            auto it = std::find(vector.begin(), vector.end(), value.value());
            if (it == vector.end())
                vector.push_back(value.value());
        }
        else
        {
            vector.push_back(value.value());
        }
    }
}

void PaintStructKeyGenerator::GenerateKeyField(
    std::vector<uint32_t>& vector, const std::optional<uint32_t>& value, const std::vector<uint32_t>& refVector) const
{
    if (value.has_value())
        vector.push_back(value.value());
    else if (refVector.empty())
        vector.push_back(0);
    else
    {
        vector = refVector;
    }
}
