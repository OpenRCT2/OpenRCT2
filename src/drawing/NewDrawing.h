#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <SDL_video.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "../rct2.h"
#include "drawing.h"

extern rct_string_id DrawingEngineStringIds[3];

sint32 drawing_engine_get_type();
void drawing_engine_init();
void drawing_engine_resize();
void drawing_engine_set_palette(SDL_Color * colours);
void drawing_engine_draw();
void drawing_engine_copy_rect(int x, int y, int width, int height, int dx, int dy);
void drawing_engine_dispose();

rct_drawpixelinfo * drawing_engine_get_dpi();
bool drawing_engine_has_dirty_optimisations();
void drawing_engine_invalidate_image(uint32 image);
void drawing_engine_set_fps_uncapped(bool uncapped);

#ifdef __cplusplus
}
#endif

