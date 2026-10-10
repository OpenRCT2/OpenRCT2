/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.Screen.h"

#include "../Context.h"
#include "../config/Config.h"
#include "../interface/ScreenCoords.hpp"
#include "NewDrawing.h"
#include "Palette.h"

namespace OpenRCT2::Drawing
{
    /**
     *
     *  rct2: 0x006ED7E5
     */
    void GfxInvalidateScreen()
    {
        GfxSetDirtyBlocks({ { 0, 0 }, { ContextGetWidth(), ContextGetHeight() } });
    }

    void RefreshVideo()
    {
        ContextRecreateWindow();
        DrawingEngineSetPalette(gPalette);
        GfxInvalidateScreen();
    }

    void ToggleWindowedMode()
    {
        int32_t rt = Config::Get().general.fullscreenMode == 0 ? 2 : 0;
        ContextSetFullscreenMode(rt);
        Config::Get().general.fullscreenMode = rt;
        Config::Save();
    }
} // namespace OpenRCT2::Drawing
