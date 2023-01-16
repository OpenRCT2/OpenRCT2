/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SetLocation.h"

#include "../../OpenRCT2.h"
#include "../../interface/Window.h"
#include "../../interface/Window_internal.h"
#include "../../world/Map.h"

namespace OpenRCT2::Title
{
    int16_t SetLocationCommand::operator()(int16_t timer)
    {
        rct_window* w = WindowGetMain();
        if (w != nullptr)
        {
            auto loc = TileCoordsXY(Location.X, Location.Y).ToCoordsXY().ToTileCentre();
            int32_t z = TileElementHeight(loc);

            // Prevent scroll adjustment due to window placement when in-game
            auto oldScreenFlags = gScreenFlags;
            gScreenFlags = SCREEN_FLAGS_TITLE_DEMO;
            w->SetLocation({ loc, z });
            gScreenFlags = oldScreenFlags;

            viewport_update_position(w);
        }

        return 0;
    }
} // namespace OpenRCT2::Title
