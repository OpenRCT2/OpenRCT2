/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SetZoom.h"

#include "../../interface/Window.h"
#include "../../interface/ZoomLevel.h"

namespace OpenRCT2::Title
{
    int16_t SetZoomCommand::operator()(int16_t timer)
    {
        WindowBase* w = WindowGetMain();
        if (w != nullptr)
        {
            WindowZoomSet(*w, ZoomLevel{ static_cast<int8_t>(Zoom) }, false);
        }

        return 0;
    }
} // namespace OpenRCT2::Title
