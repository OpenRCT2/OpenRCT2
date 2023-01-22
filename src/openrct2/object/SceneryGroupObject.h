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
#include "Object.h"

#include <vector>
namespace OpenRCT2
{
    struct ObjectRepositoryItem;

    enum class EntertainerCostume : uint8_t;

    class SceneryGroupObject final : public Object
    {
    private:
        SceneryGroupEntry _legacyType = {};
        std::vector<ObjectEntryDescriptor> _items;

    public:
        void* GetLegacyData() override
        {
            return &_legacyType;
        }
        void ReadJson(IReadObjectContext* context, json_t& root) override;

        void ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream) override;
        void Load() override;
        void Unload() override;
        void UpdateEntryIndexes();

        void DrawPreview(DrawPixelInfo* dpi, int32_t width, int32_t height) const override;

        void SetRepositoryItem(ObjectRepositoryItem* item) const override;

        uint16_t GetNumIncludedObjects() const;

    private:
        static std::vector<ObjectEntryDescriptor> ReadItems(OpenRCT2::IStream* stream);
        static uint32_t ReadJsonEntertainerCostumes(json_t& jCostumes);
        static EntertainerCostume ParseEntertainerCostume(const std::string& s);
        static std::vector<ObjectEntryDescriptor> ReadJsonEntries(IReadObjectContext* context, json_t& jEntries);
    };
} // namespace OpenRCT2
