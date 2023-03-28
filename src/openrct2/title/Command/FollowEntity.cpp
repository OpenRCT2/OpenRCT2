/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FollowEntity.h"

#include "../../interface/Window.h"

namespace OpenRCT2::Title
{
    int16_t FollowEntityCommand::operator()(int16_t timer)
    {
        auto* w = WindowGetMain();
        if (w != nullptr)
        {
            WindowFollowSprite(*w, Follow.SpriteIndex);
        }

        return 0;
    }
} // namespace OpenRCT2::Title
