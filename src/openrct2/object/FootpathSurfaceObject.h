/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Footpath.h"
#include "Object.h"

class FootpathSurfaceObject final : public Object
{
public:
    StringId NameStringId{};
    uint32_t PreviewImageId{};
    uint32_t BaseImageId{};
    uint8_t Flags{};
    PathSurfaceDescriptor _descriptor = {};

public:
    static constexpr ObjectType kObjectType = ObjectType::FootpathSurface;

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const override;

    void SetRepositoryItem(ObjectRepositoryItem* item) const override;

    const PathSurfaceDescriptor& GetDescriptor() const
    {
        return _descriptor;
    }
};
