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

struct VehicleKey
{
    uint32_t SpriteDirection;
    uint32_t Pitch;
    uint32_t NumPeeps;

    VehicleKey()
        : SpriteDirection(0)
        , Pitch(0)
        , NumPeeps(0)
    {
    }
};

bool operator==(const VehicleKey& lhs, const VehicleKey& rhs);

struct PaintStructDescriptorKey
{
    PaintStructDescriptorKey()
        : Element(0)
        , Direction(0)
        , TrackSequence(0)
    {
    }

    uint32_t Element;
    uint32_t Direction;
    uint32_t TrackSequence;
    std::array<VehicleKey, OpenRCT2::Limits::MaxTrainsPerRide + 1> VehicleKey;
};

bool operator==(const PaintStructDescriptorKey& lhs, const PaintStructDescriptorKey& rhs);

class PaintStructKeyInserter
{
public:
    std::vector<size_t> GetParams(const PaintStructDescriptorKey& key) const;
};

class ImageIdKeyInserter
{
public:
    std::vector<size_t> GetParams(const PaintStructDescriptorKey& key) const;
};

struct ImageIdOffset
{
    std::string Id;
    TreeContainer<PaintStructDescriptorKey, uint32_t, ImageIdKeyInserter> Entries;
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

    std::optional<SupportsType> Supports;
    std::optional<FloorType> Floor;
    PaintStructEdgesTable* Edges;
    std::optional<FenceType> Fences;
    std::optional<PaintType> PaintType;
    std::optional<Scheme> ImageIdScheme;
    ImageIdBase ImageIdBase;
    Colour PrimaryColour;
    uint32_t PrimaryColourIndex;
    Colour SecondaryColour;
    uint32_t SecondaryColourIndex;

    ImageIdOffset* ImageIdOffset;
    uint32_t ImageIdOffsetIndex;

    CoordsXYZ Offset;
    BoundBoxXYZ BoundBox;
    HeightSupportsTable* HeightSupports;

    //to-do : in the future, send a list of vehicle pointers so we can get the peep tshirt colours from every vehicle...
    void Paint(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, const PaintStructDescriptorKey& key, const Vehicle* vehicle) const;

    PaintStructDescriptor();
};

