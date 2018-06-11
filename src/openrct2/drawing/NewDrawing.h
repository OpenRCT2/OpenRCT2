/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#pragma once

#include "../common.h"

struct rct_drawpixelinfo;
struct rct_palette_entry;

extern rct_string_id DrawingEngineStringIds[3];

sint32 drawing_engine_get_type();
bool drawing_engine_requires_new_window(sint32 srcEngine, sint32 dstEngine);
void drawing_engine_init();
void drawing_engine_resize();
void drawing_engine_set_palette(const rct_palette_entry * colours);
void drawing_engine_copy_rect(sint32 x, sint32 y, sint32 width, sint32 height, sint32 dx, sint32 dy);
void drawing_engine_dispose();

rct_drawpixelinfo * drawing_engine_get_dpi();
bool drawing_engine_has_dirty_optimisations();
void drawing_engine_invalidate_image(uint32 image);
void drawing_engine_set_vsync(bool vsync);
