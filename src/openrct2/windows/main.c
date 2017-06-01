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

#include "../Context.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../rct2.h"
#include "../world/footpath.h"

rct_widget window_main_widgets[] = {
	{ WWT_VIEWPORT, 0, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0xFFFFFFFE, 0xFFFF },
	{ WIDGETS_END },
};

void window_main_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_main_events = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_main_paint,
	NULL
};

/**
 * Creates the main window that holds the main viewport.
 *  rct2: 0x0066B3E8
 */
void window_main_open()
{
	window_main_widgets[0].right = context_get_width();
	window_main_widgets[0].bottom = context_get_height();
	rct_window * window = window_create(
		0, 0,
		window_main_widgets[0].right, window_main_widgets[0].bottom,
		&window_main_events,
		WC_MAIN_WINDOW,
		WF_STICK_TO_BACK
	);
	window->widgets = window_main_widgets;

	viewport_create(window, window->x, window->y, window->width, window->height, 0,0x0FFF,0x0FFF, 0, 0x1, -1);
	window->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
	gCurrentRotation = 0;
	gShowGridLinesRefCount = 0;
	gShowLandRightsRefCount = 0;
	gShowConstuctionRightsRefCount = 0;
	gFootpathSelectedType = 0;
}


/**
 *
 *  rct2: 0x66CCAE
 * This function immediately jumps to 0x00685BE1 this is the second function
 * decompiled.
 */
void window_main_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	viewport_render(dpi, w->viewport, dpi->x, dpi->y, dpi->x + dpi->width, dpi->y + dpi->height);
}
