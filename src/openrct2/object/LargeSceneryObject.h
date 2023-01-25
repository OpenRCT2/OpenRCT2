/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Scenery.h"
#include "SceneryObject.h"

#include <memory>
#include <vector>

class LargeSceneryObject final : public SceneryObject
{
private:
    LargeSceneryEntry _legacyType = {};
    uint32_t _baseImageId = 0;
    std::vector<LargeSceneryTile> _tiles;
    std::unique_ptr<LargeSceneryText> _3dFont;

public:
    void* GetLegacyData() override
    {
        return &_legacyType;
    }

    void ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream) override;
    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(DrawPixelInfo* dpi, int32_t width, int32_t height) const override;
    const LargeSceneryTile* GetTileForSequence(uint8_t SequenceIndex) const;

private:
    [[nodiscard]] static std::vector<LargeSceneryTile> ReadTiles(OpenRCT2::IStream* stream);
    [[nodiscard]] static std::vector<LargeSceneryTile> ReadJsonTiles(json_t& jTiles);
    [[nodiscard]] static std::unique_ptr<LargeSceneryText> ReadJson3dFont(json_t& j3dFont);
    [[nodiscard]] static std::vector<CoordsXY> ReadJsonOffsets(json_t& jOffsets);
    [[nodiscard]] static std::vector<LargeSceneryTextGlyph> ReadJsonGlyphs(json_t& jGlyphs);
};
