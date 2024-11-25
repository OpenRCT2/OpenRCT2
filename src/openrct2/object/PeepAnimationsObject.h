/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/IStream.hpp"
#include "Object.h"

#include <string>
#include <vector>

class PeepAnimationsObject final : public Object
{
private:
    ImageIndex _imageOffsetId;

public:
    static constexpr ObjectType kObjectType = ObjectType::PeepAnimations;

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const override;
};
