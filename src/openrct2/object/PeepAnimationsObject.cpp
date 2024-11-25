/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PeepAnimationsObject.h"

#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../peep/PeepAnimations.h"

using namespace OpenRCT2;

void PeepAnimationsObject::Load()
{
    auto numImages = GetImageTable().GetCount();
    if (numImages != 0)
    {
        _imageOffsetId = LoadImages();
    }
}

void PeepAnimationsObject::Unload()
{
    UnloadImages();
}

void PeepAnimationsObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "PeepAnimationsObject::ReadJson expects parameter root to be an object");
    PopulateTablesFromJson(context, root);
}

void PeepAnimationsObject::DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const
{
    auto centre = ScreenCoordsXY{ width / 2, height / 2 };

    // Draw inline sprite in the centre
    GfxDrawSprite(dpi, ImageId(_imageOffsetId + 0), centre + ScreenCoordsXY{ -8, -8 });

    // Draw four cardinal directions around the inline sprite
    GfxDrawSprite(dpi, ImageId(_imageOffsetId + 4, COLOUR_BRIGHT_RED, COLOUR_TEAL), centre + ScreenCoordsXY{ -32, -24 });
    GfxDrawSprite(dpi, ImageId(_imageOffsetId + 2, COLOUR_BRIGHT_RED, COLOUR_TEAL), centre + ScreenCoordsXY{ +32, +32 });
    GfxDrawSprite(dpi, ImageId(_imageOffsetId + 1, COLOUR_BRIGHT_RED, COLOUR_TEAL), centre + ScreenCoordsXY{ +32, -24 });
    GfxDrawSprite(dpi, ImageId(_imageOffsetId + 3, COLOUR_BRIGHT_RED, COLOUR_TEAL), centre + ScreenCoordsXY{ -32, +32 });
}
