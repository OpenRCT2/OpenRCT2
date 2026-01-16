/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WallSceneryEntry.h"

namespace OpenRCT2
{
    Audio::DoorSoundType WallSceneryEntry::getDoorSoundType() const
    {
        return static_cast<Audio::DoorSoundType>((flags2 & WALL_SCENERY_2_DOOR_SOUND_MASK) >> WALL_SCENERY_2_DOOR_SOUND_SHIFT);
    }
} // namespace OpenRCT2
