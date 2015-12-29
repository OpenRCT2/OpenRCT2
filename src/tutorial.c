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

#include "addresses.h"
#include "interface/window.h"
#include "localisation/localisation.h"
#include "tutorial.h"
#include "windows/error.h"
#include "windows/tooltip.h"

static void sub_6EA2AA(rct_window *w, int widgetIndex, int x, int y, int edi);

/**
 *
 *  rct2: 0x0066ECC1
 */
void tutorial_start(int type)
{
	strcpy((char*)0x009BC677, "Tutorial not implemented.");
	window_error_open(3165, STR_NONE);
}

/**
 *
 *  rct2: 0x0066EE25
 */
void tutorial_stop()
{
	// RCT2_CALLPROC_EBPSAFE(0x0066EE25);
}

void game_handle_keyboard_input_for_tutorial()
{
#ifdef ENABLE_TUTORIAL
	rct_window *w;
	
	if (RCT2_GLOBAL(RCT2_ADDRESS_ON_TUTORIAL, uint8) == 1) {
		int eax, ebx, ecx, edx, esi, edi, ebp;
		RCT2_CALLFUNC_X(0x0066EEB4, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
		eax &= 0xFF;
		RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) = eax;
		if (RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & 4) {
			window_tooltip_close();
			if ((w = window_get_main()) != NULL) {
				RCT2_CALLPROC_X(0x006EA2AA, 0, 0, 0, 0, (int)w, RCT2_GLOBAL(0x009DEA72, uint16), 0);
				RCT2_GLOBAL(0x009DEA72, uint16)++;
			}
		}
	} else {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & 4)) {
			window_tooltip_close();
			if ((w = window_get_main()) != NULL) {
				sub_6EA2AA(w, 0, 0, 0, RCT2_GLOBAL(0x009DEA72, uint16));
				RCT2_GLOBAL(0x009DEA72, uint16)++;
			}
		}
	
		// Write tutorial input
		RCT2_CALLPROC_X(0x0066EEE1, RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8), 0, 0, 0, 0, 0, 0);
	}
#endif
}

static void sub_6EA2AA(rct_window *w, int widgetIndex, int x, int y, int edi)
{
#ifdef ENABLE_TUTORIAL
	RCT2_CALLPROC_X(0x006EA2AA, 0, 0, 0, 0, (int)w, RCT2_GLOBAL(0x009DEA72, uint16), 0);
	return;

	rct_window *tooltipWindow;

	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, rct_windowclass) = w->classification;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_NUMBER, rct_windownumber) = w->number;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WIDGET_INDEX, uint16) = widgetIndex;

	rct_string_id stringId = window_event_tooltip_call(w, widgetIndex);
	if (stringId == (rct_string_id)STR_NONE)
		return;

	tooltipWindow = window_find_by_class(WC_TOOLTIP);
	if (tooltipWindow == NULL)
		return;

	char *buffer = (char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER;

	RCT2_GLOBAL(0x0142006C, uint32) = edi;
	format_string(buffer, edi, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	int width = gfx_get_string_width_new_lined(buffer);
	width = min(width, 196);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;

	int numLines, fontHeight;
	gfx_wrap_string(buffer, width + 1, &numLines, &fontHeight);

	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TEXT_HEIGHT, uint16) = numLines;
	tooltipWindow->widgets[0].right = width + 3;
	tooltipWindow->widgets[0].bottom = ((numLines + 1) * 10) + 4;

	char *tooltipBuffer = (char*)RCT2_ADDRESS_TOOLTIP_TEXT_BUFFER;
	memcpy(tooltipBuffer, buffer, 512);

	window_tooltip_open(w, widgetIndex, x, y);
#endif
}

/**
 *
 *  rct2: 0x0066EE54
 */
void sub_66EE54()
{
	// RCT2_CALLPROC_EBPSAFE(0x0066EE54);
}
