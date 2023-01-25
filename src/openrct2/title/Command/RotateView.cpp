/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
        WindowBase* w = WindowGetMain();
        if (w != nullptr)
        {
            for (uint_fast8_t i = 0; i < Rotations; i++)
            {
                WindowRotateCamera(*w, 1);
            }
        }

        return 0;
    }
} // namespace OpenRCT2::Title
