/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
namespace OpenRCT2
{
    struct DrawPixelInfo;
    struct GamePalette;
    enum class DrawingEngine : int32_t;

    extern StringId DrawingEngineStringIds[3];

    DrawingEngine drawing_engine_get_type();
    bool DrawingEngineRequiresNewWindow(DrawingEngine srcEngine, DrawingEngine dstEngine);
    void DrawingEngineInit();
    void DrawingEngineResize();
    void DrawingEngineSetPalette(const GamePalette& colours);
    void DrawingEngineCopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy);
    void DrawingEngineDispose();

    DrawPixelInfo* DrawingEngineGetDpi();
    bool DrawingEngineHasDirtyOptimisations();
    void DrawingEngineInvalidateImage(uint32_t image);
    void DrawingEngineSetVSync(bool vsync);
} // namespace OpenRCT2
