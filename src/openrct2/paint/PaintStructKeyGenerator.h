#pragma once

#include <vector>
#include <array>
#include <optional>
#include "../core/Json.hpp"
#include "../Limits.h"

struct PaintStructDescriptorKey;
struct PaintStructKeyJson
{
    using VehicleParam = std::array<std::optional<uint32_t>, OpenRCT2::Limits::MaxTrainsPerRide + 1>;
    std::optional<uint32_t> Element;
    std::optional<uint32_t> Direction;
    std::optional<uint32_t> TrackSequence;
    VehicleParam VehicleSpriteDirection;
    VehicleParam VehiclePitch;
    VehicleParam VehicleNumPeeps;

    void FromJson(const json_t& element);
};

class PaintStructKeyGenerator
{
public:
    PaintStructKeyGenerator(const std::vector<PaintStructKeyJson>& keysJson);
    std::vector<PaintStructDescriptorKey> GenerateKeys(const PaintStructKeyJson& keyJson) const;

private:
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

    using VehicleParam = std::array<std::vector<uint32_t>, OpenRCT2::Limits::MaxTrainsPerRide + 1>;
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
};
