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
#include "Object.h"

struct ObjectRepositoryItem;

class SceneryGroupObject final : public Object
{
private:
    rct_scenery_group_entry         _legacyType = {};
    std::vector<rct_object_entry>   _items;

public:
    explicit SceneryGroupObject(const rct_object_entry &entry) : Object(entry) { }

    void * GetLegacyData()  override { return &_legacyType; }
    void ReadJson(IReadObjectContext * context, const json_t * root) override;

    void ReadLegacy(IReadObjectContext * context, IStream * stream) override;
    void Load() override;
    void Unload() override;
    void UpdateEntryIndexes();

    void DrawPreview(rct_drawpixelinfo * dpi, int32_t width, int32_t height) const override;

    void SetRepositoryItem(ObjectRepositoryItem * item) const override;

private:
    static std::vector<rct_object_entry> ReadItems(IStream * stream);
    static uint32_t ReadJsonEntertainerCostumes(const json_t * jCostumes);
    static uint32_t ParseEntertainerCostume(const std::string &s);
    static std::vector<rct_object_entry> ReadJsonEntries(const json_t * jEntries);
};
