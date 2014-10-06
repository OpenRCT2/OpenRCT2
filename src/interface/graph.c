/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill, Duncan Frost
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "../addresses.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "graph.h"

static void graph_draw_months_uint8(rct_drawpixelinfo *dpi, uint8 *history, int count, int baseX, int baseY)
{
	int i, x, y, yearOver32, currentMonth, currentDay;

	currentMonth = date_get_month(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16));
	currentDay = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16);
	yearOver32 = (currentMonth * 4) + (currentDay >> 14) - 31;
	x = baseX;
	y = baseY;
	for (i = count - 1; i >= 0; i--) {
		if (history[i] != 0 && history[i] != 255 && yearOver32 % 4 == 0) {
			// Draw month text
			RCT2_GLOBAL(0x013CE952, uint32) = ((yearOver32 / 4) + 8) % 8 + STR_MONTH_SHORT_MAR;
			gfx_draw_string_centred(dpi, 2222, x, y - 10, 0, (void*)0x013CE952);

			// Draw month mark
			gfx_fill_rect(dpi, x, y, x, y + 3, 10);
		}

		yearOver32 = (yearOver32 + 1) % 32;
		x += 6;
	}
}

static void graph_draw_line_a_uint8(rct_drawpixelinfo *dpi, uint8 *history, int count, int baseX, int baseY)
{
	int i, x, y, lastX, lastY;
	lastX = -1;
	x = baseX;
	for (i = count - 1; i >= 0; i--) {
		if (history[i] != 0 && history[i] != 255) {
			y = baseY + ((255 - history[i]) * 100) / 256;

			if (lastX != -1) {
				gfx_draw_line(dpi, lastX + 1, lastY + 1, x + 1, y + 1, 10);
				gfx_draw_line(dpi, lastX, lastY + 1, x, y + 1, 10);
			}
			if (i == 0)
				gfx_fill_rect(dpi, x, y, x + 2, y + 2, 10);

			lastX = x;
			lastY = y;
		}
		x += 6;
	}
}

static void graph_draw_line_b_uint8(rct_drawpixelinfo *dpi, uint8 *history, int count, int baseX, int baseY)
{
	int i, x, y, lastX, lastY;

	lastX = -1;
	x = baseX;
	for (i = count - 1; i >= 0; i--) {
		if (history[i] != 0 && history[i] != 255) {
			y = baseY + ((255 - history[i]) * 100) / 256;

			if (lastX != -1)
				gfx_draw_line(dpi, lastX, lastY, x, y, 21);
			if (i == 0)
				gfx_fill_rect(dpi, x - 1, y - 1, x + 1, y + 1, 21);

			lastX = x;
			lastY = y;
		}
		x += 6;
	}
}

void graph_draw_uint8(rct_drawpixelinfo *dpi, uint8 *history, int count, int baseX, int baseY)
{
	graph_draw_months_uint8(dpi, history, count, baseX, baseY);
	graph_draw_line_a_uint8(dpi, history, count, baseX, baseY);
	graph_draw_line_b_uint8(dpi, history, count, baseX, baseY);
}

static void graph_draw_months_money32(rct_drawpixelinfo *dpi, money32 *history, int count, int baseX, int baseY)
{
	int i, x, y, yearOver32, currentMonth, currentDay;

	currentMonth = date_get_month(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16));
	currentDay = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16);
	yearOver32 = (currentMonth * 4) + (currentDay >> 14) - 31;
	x = baseX;
	y = baseY;
	for (i = count - 1; i >= 0; i--) {
		if (history[i] != 0x80000000 && yearOver32 % 4 == 0) {
			// Draw month text
			sint32 monthFormat = ((yearOver32 / 4) + 8) % 8 + STR_MONTH_SHORT_MAR;
			gfx_draw_string_centred(dpi, 2222, x, y - 10, 0, &monthFormat);

			// Draw month mark
			gfx_fill_rect(dpi, x, y, x, y + 3, 10);
		}

		yearOver32 = (yearOver32 + 1) % 32;
		x += 6;
	}
}

static void graph_draw_line_a_money32(rct_drawpixelinfo *dpi, money32 *history, int count, int baseX, int baseY, int modifier, int offset)
{
	int i, x, y, lastX, lastY;
	lastX = -1;
	x = baseX;
	for (i = count - 1; i >= 0; i--) {
		if (history[i] != 0x80000000) {
			y = baseY + 170 - 6 - ((((history[i] >> modifier) + offset) * 170) / 256);

			if (lastX != -1) {
				gfx_draw_line(dpi, lastX + 1, lastY + 1, x + 1, y + 1, 10);
				gfx_draw_line(dpi, lastX, lastY + 1, x, y + 1, 10);
			}
			if (i == 0)
				gfx_fill_rect(dpi, x, y, x + 2, y + 2, 10);

			lastX = x;
			lastY = y;
		}
		x += 6;
	}
}

static void graph_draw_line_b_money32(rct_drawpixelinfo *dpi, money32 *history, int count, int baseX, int baseY, int modifier, int offset)
{
	int i, x, y, lastX, lastY;

	lastX = -1;
	x = baseX;
	for (i = count - 1; i >= 0; i--) {
		if (history[i] != 0x80000000) {
			y = baseY + 170 - 6 - ((((history[i] >> modifier) + offset) * 170) / 256);

			if (lastX != -1)
				gfx_draw_line(dpi, lastX, lastY, x, y, 21);
			if (i == 0)
				gfx_fill_rect(dpi, x - 1, y - 1, x + 1, y + 1, 21);

			lastX = x;
			lastY = y;
		}
		x += 6;
	}
}

void graph_draw_money32(rct_drawpixelinfo *dpi, money32 *history, int count, int baseX, int baseY, int modifier, int offset)
{
	graph_draw_months_money32(dpi, history, count, baseX, baseY);
	graph_draw_line_a_money32(dpi, history, count, baseX, baseY, modifier, offset);
	graph_draw_line_b_money32(dpi, history, count, baseX, baseY, modifier, offset);
}