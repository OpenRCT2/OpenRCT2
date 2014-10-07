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

#include "drawing.h"

/*
 * Draws a horizontal line of specified colour to a buffer.
 *  rct2: 0x0068474C
 */
void gfx_draw_line_on_buffer(rct_drawpixelinfo *dpi, char colour, int y, int x, int no_pixels)
{
	y -= dpi->y;

	//Check to make sure point is in the y range
	if (y < 0)return;
	if (y >= dpi->height)return;
	//Check to make sure we are drawing at least a pixel
	if (!no_pixels) no_pixels++;

	x -= dpi->x;

	//If x coord outside range leave
	if (x < 0){
		//Unless the number of pixels is enough to be in range
		no_pixels += x;
		if (no_pixels <= 0)return;
		//Resets starting point to 0 as we don't draw outside the range
		x = 0;
	}

	//Ensure that the end point of the line is within range
	if (x + no_pixels - dpi->width > 0){
		//If the end point has any pixels outside range
		//cut them off. If there are now no pixels return.
		no_pixels -= x + no_pixels - dpi->width;
		if (no_pixels <= 0)return;
	}

	char* bits_pointer;
	//Get the buffer we are drawing to and move to the first coordinate.
	bits_pointer = dpi->bits + y*(dpi->pitch + dpi->width) + x;

	//Draw the line to the specified colour
	for (; no_pixels > 0; --no_pixels, ++bits_pointer){
		*((uint8*)bits_pointer) = colour;
	}
}

/**
 * Draws a line on dpi if within dpi boundaries
 *  rct2: 0x00684466
 * dpi (edi)
 * x1 (ax)
 * y1 (bx)
 * x2 (cx)
 * y2 (dx)
 * colour (ebp)
 */
void gfx_draw_line(rct_drawpixelinfo *dpi, int x1, int y1, int x2, int y2, int colour)
{
	// Check to make sure the line is within the drawing area
	if ((x1 < dpi->x) && (x2 < dpi->x)){
		return;
	}

	if ((y1 < dpi->y) && (y2 < dpi->y)){
		return;
	}

	if ((x1 >(dpi->x + dpi->width)) && (x2 >(dpi->x + dpi->width))){
		return;
	}

	if ((y1 > (dpi->y + dpi->height)) && (y2 > (dpi->y + dpi->height))){
		return;
	}

	//Bresenhams algorithm

	//If vertical plot points upwards
	int steep = abs(y2 - y1) > abs(x2 - x1);
	if (steep){
		int temp_y2 = y2;
		int temp_x2 = x2;
		y2 = x1;
		x2 = y1;
		y1 = temp_x2;
		x1 = temp_y2;
	}

	//If line is right to left swap direction
	if (x1 > x2){
		int temp_y2 = y2;
		int temp_x2 = x2;
		y2 = y1;
		x2 = x1;
		y1 = temp_y2;
		x1 = temp_x2;
	}

	int delta_x = x2 - x1;
	int delta_y = abs(y2 - y1);
	int error = delta_x / 2;
	int y_step;
	int y = y1;

	//Direction of step
	if (y1 < y2)y_step = 1;
	else y_step = -1;

	for (int x = x1, x_start = x1, no_pixels = 1; x < x2; ++x,++no_pixels){
		//Vertical lines are drawn 1 pixel at a time
		if (steep)gfx_draw_line_on_buffer(dpi, colour, x, y, 1);

		error -= delta_y;
		if (error < 0){
			//Non vertical lines are drawn with as many pixels in a horizontal line as possible
			if (!steep)gfx_draw_line_on_buffer(dpi, colour, y, x_start, no_pixels);

			//Reset non vertical line vars
			x_start = x + 1;
			no_pixels = 1;
			y += y_step;
			error += delta_x;
		}

		//Catch the case of the last line
		if (x + 1 == x2 && !steep){
			gfx_draw_line_on_buffer(dpi, colour, y, x_start, no_pixels);
		}
	}
	return;
}