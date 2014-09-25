/*****************************************************************************
 * Copyright (c) 2014 Ted John
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

#include "map.h"
#include "map_helpers.h"

int map_smooth(int l, int t, int r, int b)
{
	int i, x, y, highest, count, cornerHeights[4], doubleCorner, raisedLand = 0;
	rct_map_element *mapElement, *mapElement2;
	for (y = t; y < b; y++) {
		for (x = l; x < r; x++) {
			mapElement = map_get_surface_element_at(x, y);
			mapElement->properties.surface.slope &= ~0x1F;

			// Raise to edge height - 2
			highest = mapElement->base_height;
			highest = max(highest, map_get_surface_element_at(x - 1, y + 0)->base_height);
			highest = max(highest, map_get_surface_element_at(x + 1, y + 0)->base_height);
			highest = max(highest, map_get_surface_element_at(x + 0, y - 1)->base_height);
			highest = max(highest, map_get_surface_element_at(x + 0, y + 1)->base_height);
			if (mapElement->base_height < highest - 2) {
				raisedLand = 1;
				mapElement->base_height = mapElement->clearance_height = highest - 2;
			}

			// Check corners
			doubleCorner = -1;
			cornerHeights[0] = map_get_surface_element_at(x - 1, y - 1)->base_height;
			cornerHeights[1] = map_get_surface_element_at(x + 1, y - 1)->base_height;
			cornerHeights[2] = map_get_surface_element_at(x + 1, y + 1)->base_height;
			cornerHeights[3] = map_get_surface_element_at(x - 1, y + 1)->base_height;
			highest = mapElement->base_height;
			for (i = 0; i < 4; i++)
				highest = max(highest, cornerHeights[i]);

			if (highest >= mapElement->base_height + 4) {
				count = 0;
				for (i = 0; i < 4; i++)
					if (cornerHeights[i] == highest)
						count++;

				if (count == 1) {
					if (mapElement->base_height < highest - 4) {
						mapElement->base_height = mapElement->clearance_height = highest - 4;
						raisedLand = 1;
					}
					if (cornerHeights[0] == highest && cornerHeights[2] <= cornerHeights[0] - 4)
						doubleCorner = 0;
					else if (cornerHeights[1] == highest && cornerHeights[3] <= cornerHeights[1] - 4)
						doubleCorner = 1;
					else if (cornerHeights[2] == highest && cornerHeights[0] <= cornerHeights[2] - 4)
						doubleCorner = 2;
					else if (cornerHeights[3] == highest && cornerHeights[1] <= cornerHeights[3] - 4)
						doubleCorner = 3;
				} else {
					if (mapElement->base_height < highest - 2) {
						mapElement->base_height = mapElement->clearance_height = highest - 2;
						raisedLand = 1;
					}
				}
			}

			if (doubleCorner != -1) {
				mapElement->properties.surface.slope |= 16;
				switch (doubleCorner) {
				case 0:
					mapElement->properties.surface.slope |= 2 | 4 | 8;
					break;
				case 1:
					mapElement->properties.surface.slope |= 1 | 2 | 4;
					break;
				case 2:
					mapElement->properties.surface.slope |= 1 | 2 | 8;
					break;
				case 3:
					mapElement->properties.surface.slope |= 1 | 4 | 8;
					break;
				}
			} else {
				// Corners
				mapElement2 = map_get_surface_element_at(x + 1, y + 1);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 1;

				mapElement2 = map_get_surface_element_at(x - 1, y + 1);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 8;

				mapElement2 = map_get_surface_element_at(x + 1, y - 1);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 2;

				mapElement2 = map_get_surface_element_at(x - 1, y - 1);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 4;

				// Sides
				mapElement2 = map_get_surface_element_at(x + 1, y + 0);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 1 | 2;

				mapElement2 = map_get_surface_element_at(x - 1, y + 0);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 4 | 8;

				mapElement2 = map_get_surface_element_at(x + 0, y - 1);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 2 | 4;

				mapElement2 = map_get_surface_element_at(x + 0, y + 1);
				if (mapElement2->base_height > mapElement->base_height)
					mapElement->properties.surface.slope |= 1 | 8;

				// Raise
				if (mapElement->properties.surface.slope == (1 | 2 | 4 | 8)) {
					mapElement->properties.surface.slope &= ~0x1F;
					mapElement->base_height = mapElement->clearance_height += 2;
				}
			}
		}
	}

	return raisedLand;
}