/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Object.h"
#include "SceneryGroupEntry.h"

#include <vector>

struct ObjectRepositoryItem;

class SceneryGroupObject final : public Object
{
private:
    SceneryGroupEntry _legacyType = {};
    std::vector<ObjectEntryDescriptor> _items;

public:
    static constexpr ObjectType kObjectType = ObjectType::sceneryGroup;

    void* GetLegacyData() override
    {
        return &_legacyType;
    }
    void ReadJson(IReadObjectContext* context, json_t& root) override;

    void ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream) override;
    void Load() override;
    void Unload() override;
    void UpdateEntryIndexes();

    void DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const override;

    void SetRepositoryItem(ObjectRepositoryItem* item) const override;

    uint16_t GetNumIncludedObjects() const;
    const std::vector<ObjectEntryDescriptor>& GetItems() const;

private:
    static std::vector<ObjectEntryDescriptor> ReadItems(OpenRCT2::IStream* stream);
    static std::vector<ObjectEntryDescriptor> ReadJsonEntries(IReadObjectContext* context, json_t& jEntries);
};
