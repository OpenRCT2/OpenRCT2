#pragma once

#include "../ride/TrackData.h"

#include <array>
#include <optional>
#include <vector>

struct Vehicle;

struct PaintStructSequenceMapping
{
    std::string Id;
    std::array<std::vector<uint32_t>, 4> Values;
};

struct PaintStructDescriptorKey
{
    track_type_t Element;
    std::optional<uint32_t> Direction;
    std::optional<uint32_t> TrackSequence;
    std::optional<PaintStructSequenceMapping> TrackSequenceMapping;
    std::optional<uint32_t> VehicleIndex;
    std::optional<uint32_t> VehicleSpriteDirection;
    std::optional<uint32_t> VehiclePitch;
    std::optional<uint32_t> VehicleNumPeeps;

    // helper method to check for the keys
    bool MatchWithKeys(track_type_t trackElement, uint32_t direction, uint32_t trackSequence, const Vehicle* vehicle) const;
};

struct PaintStructDescriptor
{
    using PaintStructEdgesTable = std::vector<edge_t>;
    enum class SupportsType
    {
        WoodenA
    };
    enum class FloorType
    {
        Cork
    };
    enum class FenceType
    {
        Ropes
    };
    enum class ImageIdBase
    {
        Car0
    };
    enum class PaintType
    {
        AddImageAsParent,
        AddImageAsChild,
        SetSegmentsSupportsHeight,
    };
    enum class Scheme
    {
        Misc,
        Track
    };

    struct HeightSupportsTable
    {
        std::string Id;
        uint32_t HeightOffset;
        std::map<uint8_t, uint32_t> Segments;
    };

    enum class Colour
    {
        VehicleBody,
        VehicleTrim,
        PeepTShirt
    };

    // key fields
    PaintStructDescriptorKey Key;

    // output fields
    std::optional<SupportsType> Supports;
    std::optional<FloorType> Floor;
    std::optional<PaintStructEdgesTable> Edges;
    std::optional<FenceType> Fences;
    std::optional<PaintType> PaintType;
    std::optional<Scheme> ImageIdScheme;
    ImageIdBase ImageIdBase;
    Colour PrimaryColour;
    uint32_t PrimaryColourIndex;
    Colour SecondaryColour;
    uint32_t SecondaryColourIndex;
    uint32_t ImageIdOffset;
    CoordsXYZ Offset;
    BoundBoxXYZ BoundBox;
    HeightSupportsTable HeightSupports;

    void Paint(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement) const;

    void ToJson(json_t& json) const;

    PaintStructDescriptor();
};
