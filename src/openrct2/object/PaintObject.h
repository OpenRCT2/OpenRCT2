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
#include "../paint/PaintStructDescriptor.h"
#include "../paint/tile_element/Paint.TileElement.h"
#include "../ride/Track.h"
#include "Object.h"

#include <array>
#include <map>
#include <memory>
#include <optional>
#include <vector>

using PaintStructTree = TreeContainer<PaintStructKey, std::shared_ptr<PaintStructDescriptor>>;

struct TrackElement;
class PaintObject final : public Object
{
private:
    PaintStructTree _paintStructsTree;
    std::map<std::string, HeightSupportsTable> _heightMapping;
    std::map<std::string, ImageIdOffset> _imageIdOffsetMapping;
    std::map<std::string, BoundBoxEntry> _boundBoxMapping;

    std::vector<uint32_t> _vehicleIndices;
    KeyRange<PaintStructKey> _keyRange;

    constexpr const uint8_t* GetTrackSequenceMapping(const uint32_t direction, const track_type_t trackElement) const;



public:
    PaintObject();

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    void Paint(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement) const;

    // used to load the paint objects at the start of the game
    // when RTD become a thing, load these when they are required by rtds
    static void LoadPaintObjects();

    friend struct PaintStructJson;
};

constexpr const uint8_t* PaintObject::GetTrackSequenceMapping(const uint32_t direction, const track_type_t trackElement) const
{
    switch (trackElement)
    {
        case TrackElemType::FlatTrack3x3:
            return track_map_3x3[direction];
        case TrackElemType::FlatTrack4x4:
            return track_map_4x4[direction];
        case TrackElemType::FlatTrack2x2:
            return track_map_2x2[direction];
        default:
            return nullptr;
    }
    
}
