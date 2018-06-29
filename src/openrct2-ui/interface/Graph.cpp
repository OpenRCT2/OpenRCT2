/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2-ui/interface/Graph.h>

static void graph_draw_months_uint8_t(rct_drawpixelinfo * dpi, const uint8_t * history, int32_t count, int32_t baseX, int32_t baseY)
{
    int32_t i, x, y, yearOver32, currentMonth, currentDay;

    currentMonth = date_get_month(gDateMonthsElapsed);
    currentDay = gDateMonthTicks;
    yearOver32 = (currentMonth * 4) + (currentDay >> 14) - 31;
    x = baseX;
    y = baseY;
    for (i = count - 1; i >= 0; i--)
    {
        if (history[i] != 255 && yearOver32 % 4 == 0)
        {
            // Draw month text
            set_format_arg(0, uint32_t, DateGameShortMonthNames[date_get_month((yearOver32 / 4) + MONTH_COUNT)]);
            gfx_draw_string_centred(dpi, STR_GRAPH_LABEL, x, y - 10, COLOUR_BLACK, gCommonFormatArgs);

            // Draw month mark
            gfx_fill_rect(dpi, x, y, x, y + 3, PALETTE_INDEX_10);
        }

        yearOver32 = (yearOver32 + 1) % 32;
        x += 6;
    }
}

static void graph_draw_line_a_uint8_t(rct_drawpixelinfo * dpi, const uint8_t * history, int32_t count, int32_t baseX, int32_t baseY)
{
    int32_t i, x, y, lastX, lastY;
    lastX = -1;
    lastY = -1;
    x = baseX;
    for (i = count - 1; i >= 0; i--)
    {
        if (history[i] != 255)
        {
            y = baseY + ((255 - history[i]) * 100) / 256;

            if (lastX != -1)
            {
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

static void graph_draw_line_b_uint8_t(rct_drawpixelinfo * dpi, const uint8_t * history, int32_t count, int32_t baseX, int32_t baseY)
{
    int32_t i, x, y, lastX, lastY;

    lastX = -1;
    lastY = -1;
    x = baseX;
    for (i = count - 1; i >= 0; i--)
    {
        if (history[i] != 255)
        {
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

void graph_draw_uint8_t(rct_drawpixelinfo * dpi, uint8_t * history, int32_t count, int32_t baseX, int32_t baseY)
{
    graph_draw_months_uint8_t(dpi, history, count, baseX, baseY);
    graph_draw_line_a_uint8_t(dpi, history, count, baseX, baseY);
    graph_draw_line_b_uint8_t(dpi, history, count, baseX, baseY);
}

static void graph_draw_months_money32(rct_drawpixelinfo *dpi, const money32 *history, int32_t count, int32_t baseX, int32_t baseY)
{
    int32_t i, x, y, yearOver32, currentMonth, currentDay;

    currentMonth = date_get_month(gDateMonthsElapsed);
    currentDay = gDateMonthTicks;
    yearOver32 = (currentMonth * 4) + (currentDay >> 14) - 31;
    x = baseX;
    y = baseY;
    for (i = count - 1; i >= 0; i--) {
        if (history[i] != MONEY32_UNDEFINED && yearOver32 % 4 == 0) {
            // Draw month text
            int32_t monthFormat = DateGameShortMonthNames[date_get_month((yearOver32 / 4) + MONTH_COUNT)];
            gfx_draw_string_centred(dpi, STR_GRAPH_LABEL, x, y - 10, COLOUR_BLACK, &monthFormat);

            // Draw month mark
            gfx_fill_rect(dpi, x, y, x, y + 3, PALETTE_INDEX_10);
        }

        yearOver32 = (yearOver32 + 1) % 32;
        x += 6;
    }
}

static void graph_draw_line_a_money32(rct_drawpixelinfo *dpi, const money32 *history, int32_t count, int32_t baseX, int32_t baseY, int32_t modifier, int32_t offset)
{
    int32_t i, x, y, lastX, lastY;
    lastX = -1;
    lastY = -1;
    x = baseX;
    for (i = count - 1; i >= 0; i--) {
        if (history[i] != MONEY32_UNDEFINED) {
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

static void graph_draw_line_b_money32(rct_drawpixelinfo *dpi, const money32 *history, int32_t count, int32_t baseX, int32_t baseY, int32_t modifier, int32_t offset)
{
    int32_t i, x, y, lastX, lastY;

    lastX = -1;
    lastY = -1;
    x = baseX;
    for (i = count - 1; i >= 0; i--) {
        if (history[i] != MONEY32_UNDEFINED) {
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

void graph_draw_money32(rct_drawpixelinfo *dpi, money32 *history, int32_t count, int32_t baseX, int32_t baseY, int32_t modifier, int32_t offset)
{
    graph_draw_months_money32(dpi, history, count, baseX, baseY);
    graph_draw_line_a_money32(dpi, history, count, baseX, baseY, modifier, offset);
    graph_draw_line_b_money32(dpi, history, count, baseX, baseY, modifier, offset);
}
