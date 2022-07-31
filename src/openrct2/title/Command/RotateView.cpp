/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RotateView.h"

#include "../../interface/Window.h"

namespace OpenRCT2::Title
{
    int16_t RotateViewCommand::operator()(int16_t timer)
    {
        rct_window* w = window_get_main();
        if (w != nullptr)
        {
            for (uint_fast8_t i = 0; i < Rotations; i++)
            {
                window_rotate_camera(*w, 1);
            }
        }

        return 0;
    }
} // namespace OpenRCT2::Title
