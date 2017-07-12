#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "graph.h"

static void graph_draw_months_uint8(rct_drawpixelinfo *dpi, uint8 *history, sint32 count, sint32 baseX, sint32 baseY)
{
    sint32 i, x, y, yearOver32, currentMonth, currentDay;

    currentMonth = date_get_month(gDateMonthsElapsed);
    currentDay = gDateMonthTicks;
    yearOver32 = (currentMonth * 4) + (currentDay >> 14) - 31;
    x = baseX;
    y = baseY;
    for (i = count - 1; i >= 0; i--) {
        if (history[i] != 0 && history[i] != 255 && yearOver32 % 4 == 0) {
            // Draw month text
            set_format_arg(0, uint32, DateGameShortMonthNames[((yearOver32 / 4) + 8) % 8]);
            gfx_draw_string_centred(dpi, STR_GRAPH_LABEL, x, y - 10, COLOUR_BLACK, gCommonFormatArgs);

            // Draw month mark
            gfx_fill_rect(dpi, x, y, x, y + 3, PALETTE_INDEX_10);
        }

        yearOver32 = (yearOver32 + 1) % 32;
        x += 6;
    }
}

static void graph_draw_line_a_uint8(rct_drawpixelinfo *dpi, uint8 *history, sint32 count, sint32 baseX, sint32 baseY)
{
    sint32 i, x, y, lastX, lastY;
    lastX = -1;
    lastY = -1;
    x = baseX;
    for (i = count - 1; i >= 0; i--) {
        if (history[i] != 0 && history[i] != 255) {
            y = baseY + ((255 - history[i]) * 100) / 256;

            if (lastX != -1) {
                gfx_draw_line(dpi, lastX + 1, lastY + 1, x + 1, y + 1, PALETTE_INDEX_10);
                gfx_draw_line(dpi, lastX, lastY + 1, x, y + 1, PALETTE_INDEX_10);
            }
            if (i == 0)
                gfx_fill_rect(dpi, x, y, x + 2, y + 2, PALETTE_INDEX_10);

            lastX = x;
            lastY = y;
        }
        x += 6;
    }
}

static void graph_draw_line_b_uint8(rct_drawpixelinfo *dpi, uint8 *history, sint32 count, sint32 baseX, sint32 baseY)
{
    sint32 i, x, y, lastX, lastY;

    lastX = -1;
    lastY = -1;
    x = baseX;
    for (i = count - 1; i >= 0; i--) {
        if (history[i] != 0 && history[i] != 255) {
            y = baseY + ((255 - history[i]) * 100) / 256;

            if (lastX != -1)
                gfx_draw_line(dpi, lastX, lastY, x, y, PALETTE_INDEX_21);
            if (i == 0)
                gfx_fill_rect(dpi, x - 1, y - 1, x + 1, y + 1, PALETTE_INDEX_21);

            lastX = x;
            lastY = y;
        }
        x += 6;
    }
}

void graph_draw_uint8(rct_drawpixelinfo *dpi, uint8 *history, sint32 count, sint32 baseX, sint32 baseY)
{
    graph_draw_months_uint8(dpi, history, count, baseX, baseY);
    graph_draw_line_a_uint8(dpi, history, count, baseX, baseY);
    graph_draw_line_b_uint8(dpi, history, count, baseX, baseY);
}

static void graph_draw_months_money32(rct_drawpixelinfo *dpi, money32 *history, sint32 count, sint32 baseX, sint32 baseY)
{
    sint32 i, x, y, yearOver32, currentMonth, currentDay;

    currentMonth = date_get_month(gDateMonthsElapsed);
    currentDay = gDateMonthTicks;
    yearOver32 = (currentMonth * 4) + (currentDay >> 14) - 31;
    x = baseX;
    y = baseY;
    for (i = count - 1; i >= 0; i--) {
        if (history[i] != 0x80000000 && yearOver32 % 4 == 0) {
            // Draw month text
            sint32 monthFormat = DateGameShortMonthNames[((yearOver32 / 4) + 8) % 8];
            gfx_draw_string_centred(dpi, STR_GRAPH_LABEL, x, y - 10, COLOUR_BLACK, &monthFormat);

            // Draw month mark
            gfx_fill_rect(dpi, x, y, x, y + 3, PALETTE_INDEX_10);
        }

        yearOver32 = (yearOver32 + 1) % 32;
        x += 6;
    }
}

static void graph_draw_line_a_money32(rct_drawpixelinfo *dpi, money32 *history, sint32 count, sint32 baseX, sint32 baseY, sint32 modifier, sint32 offset)
{
    sint32 i, x, y, lastX, lastY;
    lastX = -1;
    lastY = -1;
    x = baseX;
    for (i = count - 1; i >= 0; i--) {
        if (history[i] != 0x80000000) {
            y = baseY + 170 - 6 - ((((history[i] >> modifier) + offset) * 170) / 256);

            if (lastX != -1) {
                gfx_draw_line(dpi, lastX + 1, lastY + 1, x + 1, y + 1, PALETTE_INDEX_10);
                gfx_draw_line(dpi, lastX, lastY + 1, x, y + 1, PALETTE_INDEX_10);
            }
            if (i == 0)
                gfx_fill_rect(dpi, x, y, x + 2, y + 2, PALETTE_INDEX_10);

            lastX = x;
            lastY = y;
        }
        x += 6;
    }
}

static void graph_draw_line_b_money32(rct_drawpixelinfo *dpi, money32 *history, sint32 count, sint32 baseX, sint32 baseY, sint32 modifier, sint32 offset)
{
    sint32 i, x, y, lastX, lastY;

    lastX = -1;
    lastY = -1;
    x = baseX;
    for (i = count - 1; i >= 0; i--) {
        if (history[i] != 0x80000000) {
            y = baseY + 170 - 6 - ((((history[i] >> modifier) + offset) * 170) / 256);

            if (lastX != -1)
                gfx_draw_line(dpi, lastX, lastY, x, y, PALETTE_INDEX_21);
            if (i == 0)
                gfx_fill_rect(dpi, x - 1, y - 1, x + 1, y + 1, PALETTE_INDEX_21);

            lastX = x;
            lastY = y;
        }
        x += 6;
    }
}

void graph_draw_money32(rct_drawpixelinfo *dpi, money32 *history, sint32 count, sint32 baseX, sint32 baseY, sint32 modifier, sint32 offset)
{
    graph_draw_months_money32(dpi, history, count, baseX, baseY);
    graph_draw_line_a_money32(dpi, history, count, baseX, baseY, modifier, offset);
    graph_draw_line_b_money32(dpi, history, count, baseX, baseY, modifier, offset);
}
