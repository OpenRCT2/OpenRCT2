/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"

namespace OpenRCT2
{
    enum class WindowFlag : uint8_t
    {
        stickToBack,
        stickToFront,
        /**
         * User is unable to scroll this viewport
         */
        noScrolling,
        scrollingToLocation,
        transparent,
        /**
         * Instead of half transparency, completely remove the window background
         */
        noBackground,
        /**
         * Window is closed and will be deleted in the next update.
         */
        dead,
        resizable,
        /**
         * Don't auto close this window if too many windows are open
         */
        noAutoClose,
        // TODO: investigate why exactly this is used.
        higherContrastOnPress,
        noTitleBar,
        noSnapping,

        // *ONLY* create only flags below
        autoPosition,
        centreScreen,
    };
    using WindowFlags = FlagHolder<uint16_t, WindowFlag>;
} // namespace OpenRCT2
