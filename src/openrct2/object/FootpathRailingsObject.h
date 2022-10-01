/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Footpath.h"
#include "Object.h"

class FootpathRailingsObject final : public Object
{
public:
    StringId NameStringId{};
    uint32_t PreviewImageId{};
    uint32_t BridgeImageId{};
    uint32_t RailingsImageId{};
    RailingEntrySupportType SupportType{};
    uint8_t Flags{};
    uint8_t ScrollingMode{};
    colour_t Colour{};
    PathRailingsDescriptor _descriptor = {};

public:
    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const override;

    const PathRailingsDescriptor& GetDescriptor() const
    {
        return _descriptor;
    }

private:
    RailingEntrySupportType ParseSupportType(std::string_view s);
};
