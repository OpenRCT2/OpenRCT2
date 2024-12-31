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
#include "FootpathEntry.h"
#include "Object.h"

class FootpathObject final : public Object
{
private:
    FootpathEntry _legacyType = {};
    PathSurfaceDescriptor _pathSurfaceDescriptor = {};
    PathSurfaceDescriptor _queueSurfaceDescriptor = {};
    PathRailingsDescriptor _pathRailingsDescriptor = {};

public:
    static constexpr ObjectType kObjectType = ObjectType::Paths;

    void* GetLegacyData() override
    {
        return &_legacyType;
    }

    const void* GetLegacyData() const
    {
        return &_legacyType;
    }

    const PathSurfaceDescriptor& GetPathSurfaceDescriptor() const
    {
        return _pathSurfaceDescriptor;
    }

    const PathSurfaceDescriptor& GetQueueSurfaceDescriptor() const
    {
        return _queueSurfaceDescriptor;
    }

    const PathRailingsDescriptor& GetPathRailingsDescriptor() const
    {
        return _pathRailingsDescriptor;
    }

    void ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream) override;
    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const override;
};
