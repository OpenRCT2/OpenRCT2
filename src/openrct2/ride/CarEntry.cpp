/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CarEntry.h"

#include "../drawing/Drawing.Sprite.h"
#include "../drawing/RenderTarget.h"
#include "../entity/Yaw.hpp"

#include <cstdint>

uint32_t CarEntry::numRotationSprites(SpriteGroupType spriteGroup) const
{
    return NumSpritesPrecision(spriteGroups[EnumValue(spriteGroup)].spritePrecision);
}

int32_t CarEntry::spriteByYaw(int32_t yaw, SpriteGroupType spriteGroup) const
{
    return YawToPrecision(yaw, spriteGroups[EnumValue(spriteGroup)].spritePrecision);
}

bool CarEntry::groupEnabled(SpriteGroupType spriteGroup) const
{
    return spriteGroups[EnumValue(spriteGroup)].isEnabled();
}

uint32_t CarEntry::groupImageId(SpriteGroupType spriteGroup) const
{
    return spriteGroups[EnumValue(spriteGroup)].imageId;
}

uint32_t CarEntry::getSpriteOffset(SpriteGroupType spriteGroup, int32_t imageDirection, uint8_t rankIndex) const
{
    return ((spriteByYaw(imageDirection, spriteGroup) + numRotationSprites(spriteGroup) * rankIndex) * baseNumFrames)
        + groupImageId(spriteGroup);
}

/**
 *
 *  rct2: 0x006847BA
 */
void CarEntrySetImageMaxSizes(CarEntry& carEntry, int32_t numImages)
{
    constexpr uint8_t kWidth = 200;
    constexpr uint8_t kHeight = 200;
    constexpr uint8_t kCentreX = kWidth / 2;
    constexpr uint8_t kCentreY = kHeight / 2;

    OpenRCT2::Drawing::PaletteIndex bitmap[kHeight][kWidth]{};

    OpenRCT2::Drawing::RenderTarget rt = {
        .bits = reinterpret_cast<OpenRCT2::Drawing::PaletteIndex*>(bitmap),
        .x = -(kWidth / 2),
        .y = -(kHeight / 2),
        .width = kWidth,
        .height = kHeight,
        .pitch = 0,
        .zoom_level = ZoomLevel{ 0 },
    };

    for (int32_t i = 0; i < numImages; ++i)
    {
        GfxDrawSpriteSoftware(rt, ImageId(carEntry.baseImageId + i), { 0, 0 });
    }

    int32_t spriteWidth = -1;
    for (int32_t i = kCentreX - 1; i != 0; --i)
    {
        for (int32_t j = 0; j < kWidth; j++)
        {
            if (bitmap[j][kCentreX - i] != OpenRCT2::Drawing::PaletteIndex::transparent)
            {
                spriteWidth = i;
                break;
            }
        }

        if (spriteWidth != -1)
            break;

        for (int32_t j = 0; j < kWidth; j++)
        {
            if (bitmap[j][kCentreX + i] != OpenRCT2::Drawing::PaletteIndex::transparent)
            {
                spriteWidth = i;
                break;
            }
        }

        if (spriteWidth != -1)
            break;
    }
    spriteWidth++;

    int32_t spriteHeightNegative = -1;
    for (int32_t i = kCentreY - 1; i != 0; --i)
    {
        for (int32_t j = 0; j < kWidth; j++)
        {
            if (bitmap[kCentreY - i][j] != OpenRCT2::Drawing::PaletteIndex::transparent)
            {
                spriteHeightNegative = i;
                break;
            }
        }

        if (spriteHeightNegative != -1)
            break;
    }
    spriteHeightNegative++;

    int32_t spriteHeightPositive = -1;
    for (int32_t i = kCentreY - 1; i != 0; --i)
    {
        for (int32_t j = 0; j < kWidth; j++)
        {
            if (bitmap[kCentreY + i][j] != OpenRCT2::Drawing::PaletteIndex::transparent)
            {
                spriteHeightPositive = i;
                break;
            }
        }

        if (spriteHeightPositive != -1)
            break;
    }
    spriteHeightPositive++;

    // Moved from object paint

    if (carEntry.flags.has(CarEntryFlag::spriteBoundsIncludeInvertedSet))
    {
        spriteHeightNegative += 16;
    }

    carEntry.spriteWidth = spriteWidth;
    carEntry.spriteHeightNegative = spriteHeightNegative;
    carEntry.spriteHeightPositive = spriteHeightPositive;
}

bool CarEntry::isVisible() const
{
    return tabRotationMask != 0;
}
