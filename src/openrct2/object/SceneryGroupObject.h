/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2
{
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

        void ReadLegacy(IReadObjectContext* context, IStream* stream) override;
        void Load() override;
        void Unload() override;
        void UpdateEntryIndexes();

        void DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const override;

        void SetRepositoryItem(ObjectRepositoryItem* item) const override;

        uint16_t GetNumIncludedObjects() const;
        const std::vector<ObjectEntryDescriptor>& GetItems() const;

    private:
        static std::vector<ObjectEntryDescriptor> ReadItems(IStream* stream);
        static std::vector<ObjectEntryDescriptor> ReadJsonEntries(IReadObjectContext* context, json_t& jEntries);
    };
} // namespace OpenRCT2
