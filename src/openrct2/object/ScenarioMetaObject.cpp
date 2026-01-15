/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScenarioMetaObject.h"

#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"

namespace OpenRCT2
{
    void ScenarioMetaObject::Load()
    {
        auto numImages = GetImageTable().GetCount();
        if (numImages == 0)
            return;

        _imageOffsetId = LoadImages();
    }

    void ScenarioMetaObject::Unload()
    {
        UnloadImages();
    }

    void ScenarioMetaObject::ReadJson(IReadObjectContext* context, json_t& root)
    {
        Guard::Assert(root.is_object(), "ScenarioMetaObject::ReadJson expects parameter root to be an object");
        PopulateTablesFromJson(context, root);
    }

    std::string ScenarioMetaObject::GetScenarioName()
    {
        return GetStringTable().GetString(ObjectStringID::name); // scenarioName
    }

    std::string ScenarioMetaObject::GetParkName()
    {
        return GetStringTable().GetString(ObjectStringID::parkName);
    }

    std::string ScenarioMetaObject::GetScenarioDetails()
    {
        return GetStringTable().GetString(ObjectStringID::scenarioDetails);
    }

    PreviewImage ScenarioMetaObject::GetMiniMapImage() const
    {
        PreviewImage preview{};
        preview.type = PreviewImageType::miniMap;

        auto* g1 = GfxGetG1Element(_imageOffsetId);
        if (g1 == nullptr)
            return preview;

        preview.width = g1->width;
        preview.height = g1->height;

        std::copy_n(g1->offset, g1->width * g1->height, reinterpret_cast<uint8_t*>(preview.pixels));

        return preview;
    }

    PreviewImage ScenarioMetaObject::GetPreviewImage() const
    {
        PreviewImage preview{};
        preview.type = PreviewImageType::screenshot;

        auto* g1 = GfxGetG1Element(_imageOffsetId + 1);
        if (g1 == nullptr)
            return preview;

        preview.width = g1->width;
        preview.height = g1->height;

        std::copy_n(g1->offset, g1->width * g1->height, reinterpret_cast<uint8_t*>(preview.pixels));

        return preview;
    }
} // namespace OpenRCT2
