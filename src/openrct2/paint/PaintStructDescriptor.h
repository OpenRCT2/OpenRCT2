#pragma once

#include "../Limits.h"
#include "../ride/TrackData.h"
#include "TreeContainer.h"

#include <array>
#include <optional>
#include <tuple>
#include <vector>

struct Vehicle;
using PaintStructSequenceMapping = std::array<std::vector<uint32_t>, 4>;

struct PaintStructDescriptorKey
{
    PaintStructDescriptorKey()
        : Element(0)
        , Direction(0)
        , TrackSequence(0)
        , VehicleIndex(0)
        , VehicleSpriteDirection(0)
        , VehiclePitch(0)
        , VehicleNumPeeps(0)
    {
    }

    track_type_t Element;
    uint32_t Direction;
    uint32_t TrackSequence;
    PaintStructSequenceMapping* TrackSequenceMapping = nullptr;
    uint32_t VehicleIndex;
    uint32_t VehicleSpriteDirection;
    uint32_t VehiclePitch;
    uint32_t VehicleNumPeeps;

    // helper method to check for the keys
    bool MatchWithKeys(track_type_t trackElement, uint32_t direction, uint32_t trackSequence, const Vehicle* vehicle) const;
};

struct ImageIdOffset
{
    std::string Id;
    TreeContainer<uint32_t> Entries;
};

struct HeightSupportsTable
{
    std::string Id;
    uint32_t HeightOffset;
    std::map<uint8_t, uint32_t> Segments;
};

using PaintStructEdgesTable = std::vector<edge_t>;
struct PaintStructDescriptor
{
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
    };
    enum class Scheme
    {
        Misc,
        Track
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
    PaintStructEdgesTable* Edges = nullptr;
    std::optional<FenceType> Fences;
    std::optional<PaintType> PaintType;
    std::optional<Scheme> ImageIdScheme;
    ImageIdBase ImageIdBase;
    Colour PrimaryColour;
    uint32_t PrimaryColourIndex;
    Colour SecondaryColour;
    uint32_t SecondaryColourIndex;

    ImageIdOffset* ImageIdOffset = nullptr;
    uint32_t ImageIdOffsetIndex;

    CoordsXYZ Offset;
    BoundBoxXYZ BoundBox;
    HeightSupportsTable* HeightSupports = nullptr;

    void Paint(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement) const;

    void ToJson(json_t& json) const;

    PaintStructDescriptor();
};

