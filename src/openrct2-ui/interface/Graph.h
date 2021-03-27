/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include <openrct2/world/Location.hpp>

namespace Graph
{
    void Draw(rct_drawpixelinfo* dpi, uint8_t* history, int32_t count, int32_t baseX, int32_t baseY);
    void Draw(
        rct_drawpixelinfo* dpi, const money32* history, const int32_t count, const ScreenCoordsXY& coords,
        const int32_t modifier, const int32_t offset);
} // namespace Graph

#endif
