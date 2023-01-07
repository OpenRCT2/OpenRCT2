/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../paint/Boundbox.h"
#include "../paint/tile_element/Paint.TileElement.h"
#include "../ride/Track.h"
#include "Object.h"

#include <array>
#include <optional>
#include <vector>

struct TrackElement;
class PaintObject final : public Object
{
private:
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

    using PaintStructSequenceMapping = std::array<std::vector<uint32_t>, 4>;
    using PaintStructEdgesTable = std::vector<edge_t>;

    struct HeightSupportsTable
    {
        uint32_t HeightOffset;
        std::map<uint8_t, uint32_t> Segments;
    };

    struct PaintStruct
    {
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
    std::vector<PaintStruct> _paintStructs;

public:
    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    void Paint(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement) const;

    // used to load the paint objects at the start of the game
    // when RTD become a thing, load these when they are required by rtds
    static void LoadPaintObjects();
};
