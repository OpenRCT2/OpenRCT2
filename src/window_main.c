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
#include "widget.h"
#include "window.h"

rct_widget window_main_widgets[] = {
	{ WWT_VIEWPORT, 0, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0xFFFFFFFF, 0xFFFF },
	{ WIDGETS_END },
};

/**
 * Creates the main window and the toolbars or title screen buttons.
 *  rct2: 0x0066B3E8
 */
void window_main_open()
{
	rct_window* window;
	rct_widget* main_widgets = (rct_widget*)0x009A9414;

	main_widgets[0].right = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16);
	main_widgets[0].bottom = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16);
	window = window_create(
		0, 0,
		window_main_widgets[0].right, window_main_widgets[0].bottom,
		(uint32*)0x0097C0BC,
		WC_MAIN_WINDOW,
		WF_STICK_TO_BACK
	);
	window->widgets = main_widgets;

	// RCT2_CALLPROC_X(0x006EB009, window->x, window->y, 0x4000000, 0x0FFF0FFF, window, 0, 0);
	#ifdef _MSC_VER
	__asm {
		mov esi, window
		mov edx, 0FFF0FFFh
		mov eax, [esi+2Ch]
		mov ebx, [esi+30h]
		mov ecx, 4000000h
		push ebp
		mov ebp, 6EB009h
		call ebp
	}
	__asm {
		pop ebp
		or word ptr [edi+12h], 400h
	}
	#else
	__asm__ ( ".intel_syntax noprefix\n \
	\n\
		push ebx \n\
		mov esi, %[window] 	\n\
		mov edx, 0x0FFF0FFF 	\n\
		mov eax, [esi+0x2C] 	\n\
		mov ebx, [esi+0x30] 	\n\
		mov ecx, 0x4000000 	\n\
		push ebp 	\n\
		mov ebp, 0x6EB009 	\n\
		call ebp 	\n\
	\n\
		pop ebp 	\n\
		or word ptr [edi+0x12], 0x400 	\n\
		pop ebx \n\
		" : [window] "+m" (window) : : "esi","edx","eax","ecx" );
	#endif
	
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, sint32) = 0;
	RCT2_GLOBAL(0x009E32B0, uint8) = 0;
	RCT2_GLOBAL(0x009E32B2, uint8) = 0;
	RCT2_GLOBAL(0x009E32B3, uint8) = 0;
	RCT2_GLOBAL(0x00F3EFA2, uint8) = 0;
}
