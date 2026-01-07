/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../localisation/StringIdType.h"
#include "ColourPalette.h"

namespace OpenRCT2::Drawing
{
    struct RenderTarget;
}

enum class DrawingEngine : int32_t;

DrawingEngine drawing_engine_get_type();
bool DrawingEngineRequiresNewWindow(DrawingEngine srcEngine, DrawingEngine dstEngine);
void DrawingEngineInit();
void DrawingEngineResize();
void DrawingEngineSetPalette(const OpenRCT2::Drawing::GamePalette& colours);
void DrawingEngineCopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy);
void DrawingEngineDispose();

OpenRCT2::Drawing::RenderTarget& DrawingEngineGetRT();
bool DrawingEngineHasDirtyOptimisations();
void DrawingEngineInvalidateImage(uint32_t image);
void DrawingEngineSetVSync(bool vsync);
