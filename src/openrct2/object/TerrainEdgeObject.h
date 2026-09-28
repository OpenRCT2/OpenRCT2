/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../audio/Audio.h"
#include "../core/FlagHolder.hpp"
#include "../drawing/Colour.h"
#include "Object.h"

namespace OpenRCT2
{
    constexpr auto kDefaultTerrainEdgeColour1 = Drawing::Colour::beige;

    enum class TerrainEdgeFlag : uint8_t
    {
        hasDoors,
        hasPrimaryColour,
    };
    using TerrainEdgeFlags = FlagHolder<uint8_t, TerrainEdgeFlag>;

    class TerrainEdgeObject final : public Object
    {
    private:
    public:
        static constexpr ObjectType kObjectType = ObjectType::terrainEdge;

        StringId NameStringId{};
        uint32_t IconImageId{};
        uint32_t BaseImageId{};
        Audio::DoorSoundType doorSound{};
        TerrainEdgeFlags flags;
        Drawing::Colour colour = Drawing::kColourNull;

        void ReadJson(IReadObjectContext* context, json_t& root) override;
        void Load() override;
        void Unload() override;

        void DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const override;

        static TerrainEdgeObject* GetById(ObjectEntryIndex entryIndex);
        Drawing::Colour getPreviewColour() const;
    };
} // namespace OpenRCT2
