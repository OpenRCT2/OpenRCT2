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
#include "../paint/PaintStructDescriptor.h"
#include "Object.h"

#include <array>
#include <optional>
#include <vector>

struct TrackElement;
class PaintObject final : public Object
{
private:
    std::vector<PaintStructDescriptor> _paintStructs;
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
