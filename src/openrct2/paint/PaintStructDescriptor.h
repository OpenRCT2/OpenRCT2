#pragma once

#include <optional>
#include <array>
#include <vector>
#include "../ride/TrackData.h"

struct PaintStructDescriptor
{
    using PaintStructSequenceMapping = std::array<std::vector<uint32_t>, 4>;
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
        SetSegmentsSupportsHeight,
    };
    enum class Scheme
    {
        Misc,
        Track
    };

    struct HeightSupportsTable
    {
        uint32_t HeightOffset;
        std::map<uint8_t, uint32_t> Segments;
    };

    // key fields
    track_type_t Element;
    std::optional<uint32_t> Direction;
    std::optional<uint32_t> TrackSequence;
    std::optional<PaintStructSequenceMapping> TrackSequenceMapping;

    // output fields
    std::optional<SupportsType> Supports;
    std::optional<FloorType> Floor;
    std::optional<PaintStructEdgesTable> Edges;
    std::optional<FenceType> Fences;
    std::optional<PaintType> PaintType;
    Scheme ImageIdScheme;
    ImageIdBase ImageIdBase;
    uint32_t ImageIdOffset;
    CoordsXYZ Offset;
    BoundBoxXYZ BoundBox;
    HeightSupportsTable HeightSupports;

    // helper method to check for the keys
    bool MatchWithKeys(track_type_t trackElement, uint32_t direction, uint32_t trackSequence) const;

    void Paint(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement) const;
};
