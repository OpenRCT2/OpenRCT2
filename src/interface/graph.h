#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "../common.h"
#include "../drawing/drawing.h"

void graph_draw_uint8(rct_drawpixelinfo *dpi, uint8 *history, int count, int baseX, int baseY);
void graph_draw_money32(rct_drawpixelinfo *dpi, money32 *history, int count, int baseX, int baseY, int modifier, int offset);

#endif
