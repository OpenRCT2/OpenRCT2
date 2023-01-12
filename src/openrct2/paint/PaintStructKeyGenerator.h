#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <optional>
#include "../core/Json.hpp"
#include "../Limits.h"

struct PaintStructKeyJson;
struct PaintStructDescriptorKey;
class PaintStructKeyGenerator
{
public:
    PaintStructKeyGenerator();
    void Initialize(const std::vector<PaintStructKeyJson>& keysJson);
    std::vector<std::vector<uint32_t>> GetParams(const PaintStructKeyJson& key) const;
    std::vector<uint32_t> GetParams(const PaintStructDescriptorKey& key) const;
private:
    std::vector<PaintStructDescriptorKey> GenerateKeys(const PaintStructKeyJson& keyJson) const;
    enum class KeyType
    {
        Element,
        Direction,
        TrackSequence,
        VehicleSpriteDirection,
        VehiclePitch,
        VehicleNumPeeps
    };

    std::vector<uint32_t> _elements;
    std::vector<uint32_t> _directions;
    std::vector<uint32_t> _trackSequences;

    std::vector<uint32_t> _vehicleIndices;
    using VehicleParam = std::unordered_map<uint32_t, std::vector<uint32_t>>;
    VehicleParam _vehicleSpriteDirections;
    VehicleParam _vehiclePitches;
    VehicleParam _vehicleNumPeeps;

    void PushElement(std::vector<uint32_t>& vector, const std::optional<uint32_t>& value);
    void GenerateKeyField(
        std::vector<uint32_t>& vector, const std::optional<uint32_t>& value, const std::vector<uint32_t>& refVector) const;

    void SetKeyField(
        KeyType keyType, const PaintStructKeyJson& keyJson, const std::vector<PaintStructKeyJson>& oldKeys,
        const std::optional<uint32_t>& value, const std::vector<uint32_t>& values,
        std::vector<PaintStructKeyJson>& newKeys) const;
    void SetKeyField(
        KeyType keyType, uint32_t vehicleIndex, const PaintStructKeyJson& keyJson,
        const std::vector<PaintStructKeyJson>& oldKeys, const std::optional<uint32_t>& value,
        const std::vector<uint32_t>& values, std::vector<PaintStructKeyJson>& newKeys) const;

    bool HasVehicleKey(const PaintStructKeyJson& keyJson, uint32_t vehicleIndex) const;
};
