/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <openrct2/common.h>
#include <openrct2/drawing/Drawing.h>

void graph_draw_uint8_t(rct_drawpixelinfo *dpi, uint8_t *history, int32_t count, int32_t baseX, int32_t baseY);
void graph_draw_money32(rct_drawpixelinfo *dpi, money32 *history, int32_t count, int32_t baseX, int32_t baseY, int32_t modifier, int32_t offset);

#endif
