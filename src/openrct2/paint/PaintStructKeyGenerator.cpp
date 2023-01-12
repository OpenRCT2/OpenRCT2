#include "PaintStructKeyGenerator.h"
#include "../ride/TrackData.h"
#include "PaintStructDescriptor.h"

PaintStructKeyGenerator::PaintStructKeyGenerator()
{
}

void PaintStructKeyGenerator::Initialize(const std::vector<PaintStructKeyJson>& keysJson)
{
    _elements.clear();
    _directions.clear();
    _trackSequences.clear();
    _vehicleSpriteDirections.clear();
    _vehiclePitches.clear();
    _vehicleNumPeeps.clear();
    _vehicleIndices.clear();

    for (const auto& key : keysJson)
    {
        PushElement(_elements, key.Element);
        PushElement(_directions, key.Direction);
        PushElement(_trackSequences, key.TrackSequence);
        for (uint32_t index = 0; index < OpenRCT2::Limits::MaxTrainsPerRide + 1; index++)
        {
            if (HasVehicleKey(key, index))
            {
                auto it = std::find(_vehicleIndices.begin(), _vehicleIndices.end(), index);
                if (it == _vehicleIndices.end())
                    _vehicleIndices.push_back(index);
            }
                
        }

        for (const auto& vehicleIndex : _vehicleIndices)
        {
            PushElement(_vehicleSpriteDirections[vehicleIndex], key.VehicleSpriteDirection[vehicleIndex]);
            PushElement(_vehiclePitches[vehicleIndex], key.VehiclePitch[vehicleIndex]);
            PushElement(_vehicleNumPeeps[vehicleIndex], key.VehicleNumPeeps[vehicleIndex]);
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

    for (const auto& vehicleIndex : _vehicleIndices)
    {
        GenerateKeyField(
            vehicleSpriteDirections[vehicleIndex], keyJson.VehicleSpriteDirection[vehicleIndex],
            _vehicleSpriteDirections.at(vehicleIndex));
        GenerateKeyField(vehiclePitches[vehicleIndex], keyJson.VehiclePitch[vehicleIndex], _vehiclePitches.at(vehicleIndex));
        GenerateKeyField(vehicleNumPeeps[vehicleIndex], keyJson.VehicleNumPeeps[vehicleIndex], _vehicleNumPeeps.at(vehicleIndex));
    }

    std::vector<PaintStructKeyJson> oldKeys, newKeys;
    newKeys.push_back(keyJson);

    oldKeys = newKeys;
    SetKeyField(KeyType::Element, keyJson, oldKeys, keyJson.Element, elements, newKeys);

    oldKeys = newKeys;
    SetKeyField(KeyType::Direction, keyJson, oldKeys, keyJson.Direction, directions, newKeys);

    oldKeys = newKeys;
    SetKeyField(KeyType::TrackSequence, keyJson, oldKeys, keyJson.TrackSequence, trackSequences, newKeys);

    for (const auto& vehicleIndex : _vehicleIndices)
    {
        oldKeys = newKeys;
        SetKeyField(
            KeyType::VehicleNumPeeps, vehicleIndex, keyJson, oldKeys, keyJson.VehicleNumPeeps[vehicleIndex],
            vehicleNumPeeps[vehicleIndex],
            newKeys);

        oldKeys = newKeys;
        SetKeyField(
            KeyType::VehiclePitch, vehicleIndex, keyJson, oldKeys, keyJson.VehiclePitch[vehicleIndex],
            vehiclePitches[vehicleIndex],
            newKeys);

        oldKeys = newKeys;
        SetKeyField(
            KeyType::VehicleSpriteDirection, vehicleIndex, keyJson, oldKeys, keyJson.VehicleSpriteDirection[vehicleIndex],
            vehicleSpriteDirections[vehicleIndex], newKeys);
    }

    // now, every key in newkeys should have all optionals completed, lets convert that to actual keys
    std::vector<PaintStructDescriptorKey> result;
    for (const auto& keyJ : newKeys)
    {
        PaintStructDescriptorKey key;
        key.Element = keyJ.Element.value();
        key.Direction = keyJ.Direction.value();
        key.TrackSequence = keyJ.TrackSequence.value();

        for (const auto& vehicleIndex : _vehicleIndices)
        {
            key.VehicleKey[vehicleIndex].NumPeeps = keyJ.VehicleNumPeeps[vehicleIndex].value();
            key.VehicleKey[vehicleIndex].Pitch = keyJ.VehiclePitch[vehicleIndex].value();
            key.VehicleKey[vehicleIndex].SpriteDirection = keyJ.VehicleSpriteDirection[vehicleIndex].value();
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

std::vector<uint32_t> PaintStructKeyGenerator::GetParams(const PaintStructDescriptorKey& key) const
{
    std::vector<uint32_t> result;
    result.reserve(32);

    if (_directions.size() != 0)
        result.push_back(key.Direction);

    if (_elements.size() != 0)
        result.push_back(key.Element);

    if (_trackSequences.size() != 0)
        result.push_back(key.TrackSequence);

    for (const auto& vehicleIndex : _vehicleIndices)
    {
        if (_vehicleNumPeeps.at(vehicleIndex).size() != 0)
            result.push_back(key.VehicleKey[vehicleIndex].NumPeeps);

        if (_vehiclePitches.at(vehicleIndex).size() != 0)
            result.push_back(key.VehicleKey[vehicleIndex].Pitch);

        if (_vehicleSpriteDirections.at(vehicleIndex).size() != 0)
            result.push_back(key.VehicleKey[vehicleIndex].SpriteDirection);
    }

    return result;
}

std::vector<std::vector<uint32_t>> PaintStructKeyGenerator::GetParams(const PaintStructKeyJson& keyJson) const
{
    std::vector<std::vector<uint32_t>> result;
    auto keys = GenerateKeys(keyJson);

    for (const auto& key : keys)
        result.push_back(GetParams(key));
    return result;
}

bool PaintStructKeyGenerator::HasVehicleKey(const PaintStructKeyJson& keyJson, uint32_t vehicleIndex) const
{
    return keyJson.VehicleNumPeeps[vehicleIndex].has_value() || keyJson.VehiclePitch[vehicleIndex].has_value()
        || keyJson.VehicleSpriteDirection[vehicleIndex].has_value();
}
