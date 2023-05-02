/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Drawing.h"

#include <cmath>
#include <cstdlib>

/**
 * Draws a horizontal line of specified colour to a buffer.
 *  rct2: 0x0068474C
 */
static void GfxDrawLineOnBuffer(DrawPixelInfo& dpi, char colour, const ScreenCoordsXY& coords, int32_t no_pixels)
{
    ScreenCoordsXY offset{ coords.x - dpi.x, coords.y - dpi.y };

    // Check to make sure point is in the y range
    if (offset.y < 0)
        return;
    if (offset.y >= dpi.height)
        return;
    // Check to make sure we are drawing at least a pixel
    if (!no_pixels)
        no_pixels++;

    // If x coord outside range leave
    if (offset.x < 0)
    {
        // Unless the number of pixels is enough to be in range
        no_pixels += offset.x;
        if (no_pixels <= 0)
            return;
        // Resets starting point to 0 as we don't draw outside the range
        offset.x = 0;
    }

    // Ensure that the end point of the line is within range
    if (offset.x + no_pixels - dpi.width > 0)
    {
        // If the end point has any pixels outside range
        // cut them off. If there are now no pixels return.
        no_pixels -= offset.x + no_pixels - dpi.width;
        if (no_pixels <= 0)
            return;
    }

    // Get the buffer we are drawing to and move to the first coordinate.
    uint8_t* bits_pointer = dpi.bits
        + offset.y * (static_cast<int64_t>(static_cast<int64_t>(dpi.pitch) + static_cast<int64_t>(dpi.width))) + offset.x;

    // Draw the line to the specified colour
    for (; no_pixels > 0; --no_pixels, ++bits_pointer)
    {
        *bits_pointer = colour;
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

void GfxDrawLineSoftware(DrawPixelInfo& dpi, const ScreenLine& line, int32_t colour)
{
    int32_t x1 = line.GetX1();
    int32_t x2 = line.GetX2();
    int32_t y1 = line.GetY1();
    int32_t y2 = line.GetY2();
    // Check to make sure the line is within the drawing area
    if ((x1 < dpi.x) && (x2 < dpi.x))
    {
        return;
    }

    if ((y1 < dpi.y) && (y2 < dpi.y))
    {
        return;
    }

    if ((x1 > (dpi.x + dpi.width)) && (x2 > (dpi.x + dpi.width)))
    {
        return;
    }

    if ((y1 > (dpi.y + dpi.height)) && (y2 > (dpi.y + dpi.height)))
    {
        return;
    }

    // Bresenham's algorithm

    // If vertical plot points upwards
    int32_t steep = std::abs(y2 - y1) > std::abs(x2 - x1);
    if (steep)
    {
        int32_t temp_y2 = y2;
        int32_t temp_x2 = x2;
        y2 = x1;
        x2 = y1;
        y1 = temp_x2;
        x1 = temp_y2;
    }

    // If line is right to left swap direction
    if (x1 > x2)
    {
        int32_t temp_y2 = y2;
        int32_t temp_x2 = x2;
        y2 = y1;
        x2 = x1;
        y1 = temp_y2;
        x1 = temp_x2;
    }

    int32_t delta_x = x2 - x1;
    int32_t delta_y = std::abs(y2 - y1);
    int32_t error = delta_x / 2;
    int32_t y_step;
    int32_t y = y1;

    // Direction of step
    if (y1 < y2)
        y_step = 1;
    else
        y_step = -1;

    for (int32_t x = x1, x_start = x1, no_pixels = 1; x < x2; ++x, ++no_pixels)
    {
        // Vertical lines are drawn 1 pixel at a time
        if (steep)
            GfxDrawLineOnBuffer(dpi, colour, { y, x }, 1);

        error -= delta_y;
        if (error < 0)
        {
            // Non vertical lines are drawn with as many pixels in a horizontal line as possible
            if (!steep)
                GfxDrawLineOnBuffer(dpi, colour, { x_start, y }, no_pixels);

            // Reset non vertical line vars
            x_start = x + 1;
            no_pixels = 1;
            y += y_step;
            error += delta_x;
        }

        // Catch the case of the last line
        if (x + 1 == x2 && !steep)
        {
            GfxDrawLineOnBuffer(dpi, colour, { x_start, y }, no_pixels);
        }
    }
}
