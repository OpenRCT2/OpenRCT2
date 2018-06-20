/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <vector>
#include "../world/Scenery.h"
#include "SceneryObject.h"

class SmallSceneryObject final : public SceneryObject
{
private:
    rct_scenery_entry   _legacyType = {};
    std::vector<uint8_t>  _frameOffsets;

public:
    explicit SmallSceneryObject(const rct_object_entry &entry) : SceneryObject(entry) { }

    void * GetLegacyData()  override { return &_legacyType; }

    void ReadLegacy(IReadObjectContext * context, IStream * stream) override;
    void ReadJson(IReadObjectContext * context, const json_t * root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo * dpi, int32_t width, int32_t height) const override;

private:
    static std::vector<uint8_t> ReadFrameOffsets(IStream * stream);
    static std::vector<uint8_t> ReadJsonFrameOffsets(const json_t * jFrameOffsets);
    void PerformFixes();
    rct_object_entry GetScgPiratHeader();
    rct_object_entry GetScgMineHeader();
    rct_object_entry GetScgAbstrHeader();
};
