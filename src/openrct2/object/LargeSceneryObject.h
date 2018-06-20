/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <vector>
#include "../world/Location.hpp"
#include "../world/Scenery.h"
#include "SceneryObject.h"

class LargeSceneryObject final : public SceneryObject
{
private:
    rct_scenery_entry                       _legacyType = {};
    uint32_t                                  _baseImageId = 0;
    std::vector<rct_large_scenery_tile>     _tiles;
    std::unique_ptr<rct_large_scenery_text> _3dFont;

public:
    explicit LargeSceneryObject(const rct_object_entry &entry) : SceneryObject(entry) { }

    void * GetLegacyData()  override { return &_legacyType; }

    void ReadLegacy(IReadObjectContext * context, IStream * stream) override;
    void ReadJson(IReadObjectContext * context, const json_t * root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo * dpi, int32_t width, int32_t height) const override;

private:
    static std::vector<rct_large_scenery_tile> ReadTiles(IStream * stream);
    static std::vector<rct_large_scenery_tile> ReadJsonTiles(const json_t * jTiles);
    static std::unique_ptr<rct_large_scenery_text> ReadJson3dFont(const json_t * j3dFont);
    static std::vector<LocationXY16> ReadJsonOffsets(const json_t * jOffsets);
    static std::vector<rct_large_scenery_text_glyph> ReadJsonGlyphs(const json_t * jGlpyhs);
};
