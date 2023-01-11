/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CarEntry.h"

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
