/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CarEntry.h"

#include "../drawing/Drawing.h"
#include "../entity/Yaw.hpp"

#include <cstdint>

uint32_t CarEntry::NumRotationSprites(SpriteGroupType spriteGroup) const
{
    return NumSpritesPrecision(SpriteGroups[static_cast<uint8_t>(spriteGroup)].spritePrecision);
}

int32_t CarEntry::SpriteByYaw(int32_t yaw, SpriteGroupType spriteGroup) const
{
    return YawToPrecision(yaw, SpriteGroups[static_cast<uint8_t>(spriteGroup)].spritePrecision);
}

bool CarEntry::GroupEnabled(SpriteGroupType spriteGroup) const
{
    return SpriteGroups[static_cast<uint8_t>(spriteGroup)].Enabled();
}

uint32_t CarEntry::GroupImageId(SpriteGroupType spriteGroup) const
{
    return SpriteGroups[static_cast<uint8_t>(spriteGroup)].imageId;
}

uint32_t CarEntry::SpriteOffset(SpriteGroupType spriteGroup, int32_t imageDirection, uint8_t rankIndex) const
{
    return ((SpriteByYaw(imageDirection, spriteGroup) + NumRotationSprites(spriteGroup) * rankIndex) * base_num_frames)
        + GroupImageId(spriteGroup);
}

/**
 *
 *  rct2: 0x006847BA
 */
void CarEntrySetImageMaxSizes(CarEntry& carEntry, int32_t numImages)
{
    uint8_t bitmap[200][200] = { 0 };

    rct_drawpixelinfo dpi = {
        /*.bits = */ reinterpret_cast<uint8_t*>(bitmap),
        /*.x = */ -100,
        /*.y = */ -100,
        /*.width = */ 200,
        /*.height = */ 200,
        /*.pitch = */ 0,
        /*.zoom_level = */ ZoomLevel{ 0 },
    };

    for (int32_t i = 0; i < numImages; ++i)
    {
        GfxDrawSpriteSoftware(&dpi, ImageId(carEntry.base_image_id + i), { 0, 0 });
    }
    int32_t spriteWidth = -1;
    for (int32_t i = 99; i != 0; --i)
    {
        for (int32_t j = 0; j < 200; j++)
        {
            if (bitmap[j][100 - i] != 0)
            {
                spriteWidth = i;
                break;
            }
        }

        if (spriteWidth != -1)
            break;

        for (int32_t j = 0; j < 200; j++)
        {
            if (bitmap[j][100 + i] != 0)
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

    for (int32_t i = 99; i != 0; --i)
    {
        for (int32_t j = 0; j < 200; j++)
        {
            if (bitmap[100 - i][j] != 0)
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

    for (int32_t i = 99; i != 0; --i)
    {
        for (int32_t j = 0; j < 200; j++)
        {
            if (bitmap[100 + i][j] != 0)
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

    if (carEntry.flags & CAR_ENTRY_FLAG_SPRITE_BOUNDS_INCLUDE_INVERTED_SET)
    {
        spriteHeightNegative += 16;
    }

    carEntry.sprite_width = spriteWidth;
    carEntry.sprite_height_negative = spriteHeightNegative;
    carEntry.sprite_height_positive = spriteHeightPositive;
}
