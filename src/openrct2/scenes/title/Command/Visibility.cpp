/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Visibility.h"

#include "../../../interface/Viewport.h"
#include "../../../interface/WindowBase.h"
#include "../../../interface/ZoomLevel.h"

namespace OpenRCT2::Title
{
    int16_t VisibilityCommand::operator()(int16_t timer)
    {
        WindowBase* w = WindowGetMain();
        if (w != nullptr)
        {
            const uint32_t allowedFlags = VIEWPORT_HIDDEN_FLAGS | VIEWPORT_VISIBILITY_FLAGS;
            w->viewport->flags &= ~allowedFlags;
            w->viewport->flags |= Flags & allowedFlags;
            w->invalidate();
        }

        return 0;
    }
} // namespace OpenRCT2::Title
