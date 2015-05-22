/*****************************************************************************
* Copyright (c) 2014 Maciek Baron
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
#include "../config.h"
#include "../audio/audio.h"
#include "../game.h"
#include "../management/news_item.h"
#include "../management/research.h"
#include "../ride/ride.h"
#include "../localisation/localisation.h"
#include "../world/scenery.h"
#include "../ride/track.h"
#include "../interface/widget.h"
#include "../interface/window.h"

#define _window_new_ride_current_tab RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_RIDE_LIST_SELECTED_TAB, uint8)

#pragma region Ride type view order

/**
 * The order of ride types shown in the new ride window so that the order stays consistent across games and rides of the same
 * type are kept together.
 */
const char RideTypeViewOrder[] = {
	// Transport rides
	RIDE_TYPE_MINIATURE_RAILWAY,
	RIDE_TYPE_MONORAIL,
	RIDE_TYPE_SUSPENDED_MONORAIL,
	RIDE_TYPE_CHAIRLIFT,
	RIDE_TYPE_ELEVATOR,

	// Roller Coasters
	RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER,
	RIDE_TYPE_VIRGINIA_REEL,
	RIDE_TYPE_REVERSER_ROLLER_COASTER,
	RIDE_TYPE_WOODEN_ROLLER_COASTER,
	RIDE_TYPE_WOODEN_WILD_MOUSE,
	RIDE_TYPE_WILD_MOUSE,
	RIDE_TYPE_INVERTED_HAIRPIN_COASTER,
	RIDE_TYPE_JUNIOR_ROLLER_COASTER,
	RIDE_TYPE_MINI_ROLLER_COASTER,
	RIDE_TYPE_SPIRAL_ROLLER_COASTER,
	RIDE_TYPE_MINE_TRAIN_COASTER,
	RIDE_TYPE_LOOPING_ROLLER_COASTER,
	RIDE_TYPE_STAND_UP_ROLLER_COASTER,
	RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
	RIDE_TYPE_90,
	RIDE_TYPE_TWISTER_ROLLER_COASTER,
	RIDE_TYPE_GIGA_COASTER,
	RIDE_TYPE_SUSPENDED_SWINGING_COASTER,
	RIDE_TYPE_COMPACT_INVERTED_COASTER,
	RIDE_TYPE_INVERTED_ROLLER_COASTER,
	RIDE_TYPE_INVERTED_IMPULSE_COASTER,
	RIDE_TYPE_MINI_SUSPENDED_COASTER,
	RIDE_TYPE_STEEPLECHASE,
	RIDE_TYPE_BOBSLEIGH_COASTER,
	RIDE_TYPE_MINE_RIDE,
	RIDE_TYPE_HEARTLINE_TWISTER_COASTER,
	RIDE_TYPE_LAY_DOWN_ROLLER_COASTER,
	RIDE_TYPE_FLYING_ROLLER_COASTER,
	RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER,
	RIDE_TYPE_REVERSE_FREEFALL_COASTER,
	RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER,
	RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER,

	// Gentle rides
	RIDE_TYPE_MONORAIL_CYCLES,
	RIDE_TYPE_CROOKED_HOUSE,
	RIDE_TYPE_HAUNTED_HOUSE,
	RIDE_TYPE_FERRIS_WHEEL,
	RIDE_TYPE_MAZE,
	RIDE_TYPE_MERRY_GO_ROUND,
	RIDE_TYPE_MINI_GOLF,
	RIDE_TYPE_OBSERVATION_TOWER,
	RIDE_TYPE_CAR_RIDE,
	RIDE_TYPE_MINI_HELICOPTERS,
	RIDE_TYPE_SPIRAL_SLIDE,
	RIDE_TYPE_BUMPER_CARS,
	RIDE_TYPE_SPACE_RINGS,
	RIDE_TYPE_CIRCUS_SHOW,
	RIDE_TYPE_GHOST_TRAIN,
	RIDE_TYPE_FLYING_SAUCERS,

	// Thrill rides
	RIDE_TYPE_TWIST,
	RIDE_TYPE_MAGIC_CARPET,
	RIDE_TYPE_LAUNCHED_FREEFALL,
	RIDE_TYPE_PIRATE_SHIP,
	RIDE_TYPE_GO_KARTS,
	RIDE_TYPE_SWINGING_INVERTER_SHIP,
	RIDE_TYPE_MOTION_SIMULATOR,
	RIDE_TYPE_3D_CINEMA,
	RIDE_TYPE_TOP_SPIN,
	RIDE_TYPE_ROTO_DROP,
	RIDE_TYPE_ENTERPRISE,

	// Water rides
	RIDE_TYPE_DINGHY_SLIDE,
	RIDE_TYPE_LOG_FLUME,
	RIDE_TYPE_RIVER_RAPIDS,
	RIDE_TYPE_SPLASH_BOATS,
	RIDE_TYPE_SUBMARINE_RIDE,
	RIDE_TYPE_BUMPER_BOATS,
	RIDE_TYPE_RIVER_RAFTS,
	RIDE_TYPE_WATER_COASTER,

	// Shops / stalls
	RIDE_TYPE_FOOD_STALL,
	RIDE_TYPE_1D,
	RIDE_TYPE_DRINK_STALL,
	RIDE_TYPE_1F,
	RIDE_TYPE_SHOP,
	RIDE_TYPE_22,
	RIDE_TYPE_INFORMATION_KIOSK,
	RIDE_TYPE_FIRST_AID,
	RIDE_TYPE_ATM,
	RIDE_TYPE_BATHROOM
};

#pragma endregion

enum {
	WINDOW_NEW_RIDE_PAGE_TRANSPORT,
	WINDOW_NEW_RIDE_PAGE_GENTLE,
	WINDOW_NEW_RIDE_PAGE_ROLLER_COASTER,
	WINDOW_NEW_RIDE_PAGE_THRILL,
	WINDOW_NEW_RIDE_PAGE_WATER,
	WINDOW_NEW_RIDE_PAGE_SHOP,
	WINDOW_NEW_RIDE_PAGE_RESEARCH,
	WINDOW_NEW_RIDE_PAGE_COUNT
};

#pragma region Widgets

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,
	WIDX_TAB_4,
	WIDX_TAB_5,
	WIDX_TAB_6,
	WIDX_TAB_7,
	WIDX_RIDE_LIST,
	WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP,
	WIDX_LAST_DEVELOPMENT_GROUP,
	WIDX_LAST_DEVELOPMENT_BUTTON,
	WIDX_RESEARCH_FUNDING_BUTTON
};

static rct_widget window_new_ride_widgets[] = {
	{ WWT_FRAME,			0,	0,		600,	0,		369,	0xFFFFFFFF,								STR_NONE							},
	{ WWT_CAPTION,			0,	1,		599,	1,		14,		0xFFFFFFFF,								STR_WINDOW_TITLE_TIP				},
	{ WWT_CLOSEBOX,			0,	588,	598,	2,		13,		STR_CLOSE_X,							STR_CLOSE_WINDOW_TIP				},
	{ WWT_RESIZE,			1,	0,		600,	43,		369,	0xFFFFFFFF,								STR_NONE							},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,								STR_TRANSPORT_RIDES_TIP				},
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,								STR_GENTLE_RIDES_TIP				},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,								STR_ROLLER_COASTERS_TIP				},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,								STR_THRILL_RIDES_TIP				},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,								STR_WATER_RIDES_TIP					},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,								STR_SHOPS_STALLS_TIP				},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,								STR_RESEARCH_AND_DEVELOPMENT_TIP	},
	{ WWT_SCROLL,			1,	3,		597,	46,		317,	2,										STR_NONE							},
	{ WWT_GROUPBOX,			2,	3,		292,	47,		116,	STR_CURRENTLY_IN_DEVELOPMENT,			STR_NONE							},
	{ WWT_GROUPBOX,			2,	3,		292,	124,	188,	STR_LAST_DEVELOPMENT,					STR_NONE							},
	{ WWT_FLATBTN,			2,	265,	288,	161,	184,	0xFFFFFFFF,								STR_RESEARCH_SHOW_DETAILS_TIP		},
	{ WWT_FLATBTN,			2,	265,	288,	68,		91,		5190,									STR_FINANCES_RESEARCH				},
	{ WIDGETS_END },
};

#pragma endregion

#pragma region Events

static void window_new_ride_emptysub() { }

static void window_new_ride_mouseup();
static void window_new_ride_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_new_ride_update(rct_window *w);
static void window_new_ride_scrollgetsize();
static void window_new_ride_scrollmousedown();
static void window_new_ride_scrollmouseover();
static void window_new_ride_tooltip();
static void window_new_ride_invalidate();
static void window_new_ride_paint();
static void window_new_ride_scrollpaint();

// 0x0098E354
static void* window_new_ride_events[] = {
	window_new_ride_emptysub,
	window_new_ride_mouseup,
	window_new_ride_emptysub,
	window_new_ride_mousedown,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_update,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_scrollgetsize,
	window_new_ride_scrollmousedown,
	window_new_ride_emptysub,
	window_new_ride_scrollmouseover,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_tooltip,
	window_new_ride_emptysub,
	window_new_ride_emptysub,
	window_new_ride_invalidate,
	window_new_ride_paint,
	window_new_ride_scrollpaint
};

#pragma endregion

const int window_new_ride_tab_animation_loops[] = { 20, 32, 10, 72, 24, 28, 16 };
const int window_new_ride_tab_animation_divisor[] = { 4, 8, 2, 4, 4, 4, 2 };

static void window_new_ride_set_page(rct_window *w, int page);
static void window_new_ride_refresh_widget_sizing(rct_window *w);
static ride_list_item window_new_ride_scroll_get_ride_list_item_at(rct_window *w, int x, int y);
static void window_new_ride_paint_ride_information(rct_window *w, rct_drawpixelinfo *dpi, ride_list_item item, int x, int y, int width);
static void window_new_ride_select(rct_window *w);

static ride_list_item _lastTrackDesignCountRideType;
static int _lastTrackDesignCount;

/**
*
*  rct2: 0x006ACA58
*/
void window_new_ride_init_vars() {
	// If we are in the track designer, default to the Roller Coaster tab
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER) {
		RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_RIDE_LIST_SELECTED_TAB, uint8) = WINDOW_NEW_RIDE_PAGE_ROLLER_COASTER;
	}
	else {
		RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_RIDE_LIST_SELECTED_TAB, uint8) = WINDOW_NEW_RIDE_PAGE_TRANSPORT;
	}

	for (short i = 0; i < 6; i++) {
		/*
		Reset what is highlighted in each tab.
		Each 16bit number represents the item in its respective tab.
		*/
		RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_RIDE_LIST_HIGHLIGHTED_ITEM, uint16)[i] = 0xFFFF;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_RIDE_LIST_INFORMATION_TYPE, uint8) = 0;
}

/**
 *
 *  rct2: 0x006B6F3E
 */
static void window_new_ride_populate_list()
{
	int i, quadIndex, bitIndex;

	uint8 currentCategory = _window_new_ride_current_tab;
	ride_list_item *nextListItem = (ride_list_item*)0x00F43523;
	rct_ride_type **rideEntries = (rct_ride_type**)0x009ACFA4;

	// For each ride type in the view order list
	for (i = 0; i < countof(RideTypeViewOrder); i++) {
		uint8 rideType = RideTypeViewOrder[i];
		if (rideType == RIDE_TYPE_NULL)
			continue;

		quadIndex = rideType >> 5;
		bitIndex = rideType & 0x1F;
		if (RCT2_ADDRESS(0x01357404, uint32)[quadIndex] & (1 << bitIndex)) {
			int dh = 0;
			uint8 *rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(rideType);

			// For each ride entry for this ride type
			while (*rideEntryIndexPtr != 255) {
				uint8 rideEntryIndex = *rideEntryIndexPtr++;

				quadIndex = rideEntryIndex >> 5;
				bitIndex = rideEntryIndex & 0x1F;
				if (!(RCT2_ADDRESS(0x01357424, uint32)[quadIndex] & (1 << bitIndex)))
					continue;

				// Ride entries
				rct_ride_type *rideEntry = rideEntries[rideEntryIndex];

				// Check if ride is in this category
				if (currentCategory != rideEntry->category[0] && currentCategory != rideEntry->category[1])
					continue;

				if (rideEntry->var_008 & 0x2000) {
					dh &= ~4;
					nextListItem->type = rideType;
					nextListItem->entry_index = rideEntryIndex;
					nextListItem++;
				} else if (!(dh & 1)) {
					dh |= 5;
					nextListItem->type = rideType;
					nextListItem->entry_index = rideEntryIndex;
					nextListItem++;
				} else if (dh & 4) {
					if (rideType == rideEntry->ride_type[0]) {
						nextListItem--;
						nextListItem->type = rideType;
						nextListItem->entry_index = rideEntryIndex;
						nextListItem++;
					}
				}
			}
		}
	}

	nextListItem->type = 255;
	nextListItem->entry_index = 255;
}

/**
 *
 *  rct2: 0x006B7220
 */
static void window_new_ride_scroll_to_focused_ride(rct_window *w)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;

	// Get the scroll height
	eax = 0;
	esi = (int)w;
	RCT2_CALLFUNC_X(w->event_handlers[WE_SCROLL_GETSIZE], &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	int scrollHeight = edx;
	ebx = 0;
	
	// Find row index of the focused ride type
	rct_widget *listWidget = &window_new_ride_widgets[WIDX_RIDE_LIST];
	int focusRideType = RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_RIDE_LIST_HIGHLIGHTED_ITEM, uint16)[_window_new_ride_current_tab];
	int count = 0, row = 0;
	ride_list_item *listItem = (ride_list_item*)0x00F43523;
	while (listItem->type != 255 || listItem->entry_index != 255) {
		if (listItem->type == focusRideType) {
			row = count / 5;
			break;
		}

		count++;
		listItem++;
	};

	// Update the Y scroll position
	int listWidgetHeight = listWidget->bottom - listWidget->top - 1;
	scrollHeight = max(0, scrollHeight - listWidgetHeight);
	w->scrolls[0].v_top = min(row * 116, scrollHeight);
	widget_scroll_update_thumbs(w, WIDX_RIDE_LIST);
}

/**
 *
 *  rct2: 0x006B3CFF
 */
rct_window *window_new_ride_open()
{
	rct_window *w;

	w = window_bring_to_front_by_class(WC_CONSTRUCT_RIDE);
	if (w != NULL)
		return w;

	// Not sure what these windows are
	window_close_by_class(WC_TRACK_DESIGN_LIST);
	window_close_by_class(WC_TRACK_DESIGN_PLACE);

	w = window_create_auto_pos(601, 370, (uint32*)window_new_ride_events, WC_CONSTRUCT_RIDE, 0x400);
	w->widgets = window_new_ride_widgets;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_TAB_1) |
		(1 << WIDX_TAB_2) |
		(1 << WIDX_TAB_3) |
		(1 << WIDX_TAB_4) |
		(1 << WIDX_TAB_5) |
		(1 << WIDX_TAB_6) |
		(1 << WIDX_TAB_7) |
		(1 << 14) |
		(1 << 15);
	window_init_scroll_widgets(w);

	w->frame_no = 0;

	w->new_ride.selected_ride_id = -1;
	w->new_ride.highlighted_ride_id = -1;
	_lastTrackDesignCountRideType.type = 255;
	_lastTrackDesignCountRideType.entry_index = 255;
	
	window_new_ride_populate_list();
	
	w->new_ride.highlighted_ride_id = RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_RIDE_LIST_HIGHLIGHTED_ITEM, sint16)[_window_new_ride_current_tab];
	if (w->new_ride.highlighted_ride_id == -1)
		w->new_ride.highlighted_ride_id = RCT2_GLOBAL(0x00F43523, sint16);
	
	w->width = 1;
	window_new_ride_refresh_widget_sizing(w);
	window_new_ride_scroll_to_focused_ride(w);

	return w;
}

rct_window *window_new_ride_open_research()
{
	rct_window *w;
	
	w = window_new_ride_open();
	window_new_ride_set_page(w, WINDOW_NEW_RIDE_PAGE_RESEARCH);
	return w;
}

/**
 *
 *  rct2: 0x006B3EBA
 */
void window_new_ride_focus(ride_list_item rideItem)
{
	rct_window *w;
	rct_ride_type *rideType;

	w = window_find_by_class(WC_CONSTRUCT_RIDE);
	if (w == NULL)
		return;

	rideType = GET_RIDE_ENTRY(rideItem.entry_index);
	window_new_ride_set_page(w, rideType->category[0]);

	ride_list_item *listItem = (ride_list_item*)0x00F43523;
	while (listItem->type != RIDE_TYPE_NULL) {
		if (listItem->type == rideItem.type && listItem->entry_index == rideItem.entry_index) {
			RCT2_GLOBAL(0x00F43825, uint8) = rideItem.type;
			RCT2_GLOBAL(0x00F43826, uint8) = rideItem.entry_index;
			w->new_ride.highlighted_ride_id = (rideItem.entry_index << 8) | rideItem.type;
			window_new_ride_scroll_to_focused_ride(w);
		}
		listItem++;
	}
}

static void window_new_ride_set_page(rct_window *w, int page)
{
	_window_new_ride_current_tab = page;
	w->frame_no = 0;
	w->new_ride.highlighted_ride_id = -1;
	w->new_ride.selected_ride_countdown = -1;
	window_new_ride_populate_list();
	if (page < WINDOW_NEW_RIDE_PAGE_RESEARCH) {
		w->new_ride.highlighted_ride_id = RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_RIDE_LIST_HIGHLIGHTED_ITEM, sint16)[page];
		if (w->new_ride.highlighted_ride_id == -1)
			w->new_ride.highlighted_ride_id = RCT2_GLOBAL(0x00F43523, sint16);
	}

	window_new_ride_refresh_widget_sizing(w);
	window_invalidate(w);
	window_new_ride_scroll_to_focused_ride(w);
}

/**
 *
 *  rct2: 0x006B3DF1
 */
static void window_new_ride_refresh_widget_sizing(rct_window *w)
{
	int width, height;

	// Show or hide unrelated widgets
	if (_window_new_ride_current_tab != WINDOW_NEW_RIDE_PAGE_RESEARCH) {
		window_new_ride_widgets[WIDX_RIDE_LIST].type = WWT_SCROLL;
		window_new_ride_widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].type = WWT_EMPTY;
		window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_GROUP].type = WWT_EMPTY;
		window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_EMPTY;
		window_new_ride_widgets[WIDX_RESEARCH_FUNDING_BUTTON].type = WWT_EMPTY;

		width = 601;
		height = 370;
	} else {
		window_new_ride_widgets[WIDX_RIDE_LIST].type = WWT_EMPTY;
		window_new_ride_widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].type = WWT_GROUPBOX;
		window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_GROUP].type = WWT_GROUPBOX;
		window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_FLATBTN;
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY))
			window_new_ride_widgets[WIDX_RESEARCH_FUNDING_BUTTON].type = WWT_FLATBTN;

		width = 300;
		height = 196;
	}

	// Handle new window size
	if (w->width != width || w->height != height) {
		window_invalidate(w);

		// Resize widgets to new window size
		window_new_ride_widgets[WIDX_BACKGROUND].right = width - 1;
		window_new_ride_widgets[WIDX_BACKGROUND].bottom = height - 1;
		window_new_ride_widgets[WIDX_PAGE_BACKGROUND].right = width - 1;
		window_new_ride_widgets[WIDX_PAGE_BACKGROUND].bottom = height - 1;
		window_new_ride_widgets[WIDX_TITLE].right = width - 2;
		window_new_ride_widgets[WIDX_CLOSE].left = width - 13;
		window_new_ride_widgets[WIDX_CLOSE].right = width - 3;

		w->width = width;
		w->height = height;
		window_invalidate(w);
	}

	window_init_scroll_widgets(w);
}

static void window_new_ride_set_pressed_tab(rct_window *w)
{
	int i;
	for (i = 0; i < WINDOW_NEW_RIDE_PAGE_COUNT; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + _window_new_ride_current_tab);
}

const int ThrillRidesTabAnimationSequence[] = {
	5, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0
};

static void window_new_ride_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, int page, int spriteIndex)
{
	int widgetIndex = WIDX_TAB_1 + page;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		int frame = 0;
		if (_window_new_ride_current_tab == page)
			frame = w->frame_no / window_new_ride_tab_animation_divisor[page];

		spriteIndex += page == WINDOW_NEW_RIDE_PAGE_THRILL ?
				ThrillRidesTabAnimationSequence[frame] : frame;

		spriteIndex |= w->colours[1] << 19;

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

static void window_new_ride_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_TRANSPORT, 0x200015A1);
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_GENTLE, 5542);
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_ROLLER_COASTER, 0x200015AA);
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_THRILL, 5557);
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_WATER, 5551);
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_SHOP, 5530);
	window_new_ride_draw_tab_image(dpi, w, WINDOW_NEW_RIDE_PAGE_RESEARCH, 5327);
}

/**
 *
 *  rct2: 0x006B6B38
 */
static void window_new_ride_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_LAST_DEVELOPMENT_BUTTON:
		news_item_open_subject(NEWS_ITEM_RESEARCH, RCT2_GLOBAL(RCT2_ADDRESS_LAST_RESEARCHED_ITEM_SUBJECT, sint32));
		break;
	case WIDX_RESEARCH_FUNDING_BUTTON:
		window_finances_research_open();
		break;
	}
}

/**
 *
 *  rct2: 0x006B6B4F
 */
static void window_new_ride_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
		window_new_ride_set_page(w, widgetIndex - WIDX_TAB_1);
}

/**
 *
 *  rct2: 0x006B6CE7
 */
static void window_new_ride_update(rct_window *w)
{
	w->frame_no++;
	if (w->frame_no >= window_new_ride_tab_animation_loops[_window_new_ride_current_tab])
		w->frame_no = 0;

	widget_invalidate(w, WIDX_TAB_1 + _window_new_ride_current_tab);

	if (w->new_ride.selected_ride_id != -1 && w->new_ride.selected_ride_countdown-- == 0)
		window_new_ride_select(w);
}

/**
 *
 *  rct2: 0x006B6BC9
 */
static void window_new_ride_scrollgetsize()
{
	ride_list_item *listItem = (ride_list_item*)0x00F43523;
	int scrollWidth, scrollHeight;

	int count = 0;
	while (listItem->type != 255 || listItem->entry_index != 255) {
		count++;
		listItem++;
	}
	scrollWidth = 0;
	scrollHeight = ((count + 4) / 5) * 116;

	#ifdef _MSC_VER
	__asm mov ecx, scrollWidth
	#else
	__asm__ ( "mov ecx, %[scrollWidth] " : [scrollWidth] "+m" (scrollWidth) );
	#endif

	#ifdef _MSC_VER
	__asm mov edx, scrollHeight
	#else
	__asm__ ( "mov edx, %[scrollHeight] " : [scrollHeight] "+m" (scrollHeight) );
	#endif
}

/**
 *
 *  rct2: 0x006B6C89
 */
static void window_new_ride_scrollmousedown()
{
	short x, y, scrollIndex;
	rct_window *w;
	ride_list_item item;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) != 0)
		return;

	item = window_new_ride_scroll_get_ride_list_item_at(w, x, y);
	if (item.type == 255 && item.entry_index == 255)
		return;

	RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_RIDE_LIST_HIGHLIGHTED_ITEM, ride_list_item)[_window_new_ride_current_tab] = item;
	w->new_ride.selected_ride_id = *((sint16*)&item);

	sound_play_panned(SOUND_CLICK_1, w->x + (w->width / 2), 0, 0, 0);
	w->new_ride.selected_ride_countdown = 8;
	window_invalidate(w);
}

/**
 *
 *  rct2: 0x006B6C51
 */
static void window_new_ride_scrollmouseover()
{
	short x, y, scrollIndex;
	rct_window *w;
	ride_list_item item;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	if (w->new_ride.selected_ride_id != -1)
		return;

	item = window_new_ride_scroll_get_ride_list_item_at(w, x, y);

	if (w->new_ride.highlighted_ride_id == *((sint16*)&item))
		return;

	w->new_ride.highlighted_ride_id = *((sint16*)&item);
	RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_RIDE_LIST_HIGHLIGHTED_ITEM, ride_list_item)[_window_new_ride_current_tab] = item;
	window_invalidate(w);
}

/**
 *
 *  rct2: 0x006B6BBF
 */
static void window_new_ride_tooltip()
{
	RCT2_GLOBAL(0x013CE952, uint16) = 3159;
}

/**
 *
 *  rct2: 0x006B6819
 */
static void window_new_ride_invalidate()
{
	rct_window *w;

	window_get_register(w);

	if(!gConfigInterface.rct1_colour_scheme)
	{
		w->colours[0] = 24;
		w->colours[1] = 26;
		w->colours[2] = 26;
	}
	else
	{
		w->colours[0] = 26;
		w->colours[1] = 1;
		w->colours[2] = 1;
	}

	window_new_ride_set_pressed_tab(w);

	window_new_ride_widgets[WIDX_TITLE].image = STR_NEW_TRANSPORT_RIDES + _window_new_ride_current_tab;
	window_new_ride_widgets[WIDX_TAB_7].type = WWT_TAB;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 4)
		window_new_ride_widgets[WIDX_TAB_7].type = WWT_EMPTY;

	if (_window_new_ride_current_tab == WINDOW_NEW_RIDE_PAGE_RESEARCH) {
		window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_EMPTY;
		uint32 typeId = RCT2_GLOBAL(RCT2_ADDRESS_LAST_RESEARCHED_ITEM_SUBJECT, uint32);
		if (typeId != 0xFFFFFFFF) {
			window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WWT_FLATBTN;
			window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_BUTTON].image = typeId >= 0x10000 ? 5189 : 5191;
		}
	}
}

/**
 *
 *  rct2: 0x006B689B
 */
static void window_new_ride_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_new_ride_draw_tab_images(dpi, w);

	if (_window_new_ride_current_tab != WINDOW_NEW_RIDE_PAGE_RESEARCH) {
		ride_list_item item = *((ride_list_item*)&w->new_ride.highlighted_ride_id);
		if (item.type == 255 && item.entry_index == 255)
			return;

		window_new_ride_paint_ride_information(w, dpi, item, w->x + 3, w->y + w->height - 52, w->width - 6);
		return;
	}

	int x = w->x + 10;
	int y = w->y + window_new_ride_widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].top + 12;
	rct_string_id stringId;

	if (RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) == RESEARCH_STAGE_FINISHED_ALL){
		stringId = STR_RESEARCH_UNKNOWN;
		gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 296, STR_RESEARCH_TYPE_LABEL, 0);
		y += 25;
		// Progress
		stringId = 2680;
		gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 296, STR_RESEARCH_PROGRESS_LABEL, 0);
		y += 15;

		RCT2_GLOBAL(0x013CE952, uint16) = STR_UNKNOWN;
		gfx_draw_string_left(dpi, STR_RESEARCH_EXPECTED_LABEL, (void*)0x013CE952, 0, x, y);
	}
	else{
		// Research type
		stringId = STR_RESEARCH_UNKNOWN;
		if (RCT2_GLOBAL(0x01357CF3, uint8) != 0) {
			stringId = STR_TRANSPORT_RIDE + RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_CATEGORY, uint8);
			if (RCT2_GLOBAL(0x01357CF3, uint8) != 1) {
				uint32 typeId = RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_ITEM, uint32);
				if (typeId >= 0x10000) {
					rct_ride_type *rideEntry = RCT2_GLOBAL(0x009ACFA4 + (typeId & 0xFF) * 4, rct_ride_type*);
					stringId = rideEntry->var_008 & 0x1000 ?
						rideEntry->name :
						(typeId & 0xFF00) + 2;
				}
				else {
					stringId = g_scenerySetEntries[typeId]->name;
				}
			}
		}
		gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 296, STR_RESEARCH_TYPE_LABEL, 0);
		y += 25;

		// Progress
		stringId = 2285 + RCT2_GLOBAL(0x01357CF3, uint8);
		gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 296, STR_RESEARCH_PROGRESS_LABEL, 0);
		y += 15;

		// Expected
		RCT2_GLOBAL(0x013CE952, uint16) = STR_UNKNOWN;
		if (RCT2_GLOBAL(0x01357CF3, uint8) != 0) {
			uint16 expectedDay = RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_EXPECTED_DAY, uint8);
			if (expectedDay != 255) {
				RCT2_GLOBAL(0x013CE952 + 2, uint16) = STR_DATE_DAY_1 + expectedDay;
				RCT2_GLOBAL(0x013CE952 + 4, uint16) = STR_MONTH_MARCH + RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_EXPECTED_MONTH, uint8);
				RCT2_GLOBAL(0x013CE952, uint16) = 2289;
			}
		}
		gfx_draw_string_left(dpi, STR_RESEARCH_EXPECTED_LABEL, (void*)0x013CE952, 0, x, y);
	}
	// Last development
	x = w->x + 10;
	y = w->y + window_new_ride_widgets[WIDX_LAST_DEVELOPMENT_GROUP].top + 12;

	uint32 typeId = RCT2_GLOBAL(RCT2_ADDRESS_LAST_RESEARCHED_ITEM_SUBJECT, uint32);
	int lastDevelopmentFormat;
	if (typeId != 0xFFFFFFFF) {
		if (typeId >= 0x10000) {
			rct_ride_type *rideEntry = RCT2_GLOBAL(0x009ACFA4 + (typeId & 0xFF) * 4, rct_ride_type*);
			stringId = rideEntry->var_008 & 0x1000 ?
				rideEntry->name :
				((typeId >> 8) & 0xFF) + 2;

			lastDevelopmentFormat = STR_RESEARCH_RIDE_LABEL;
		} else {
			stringId = g_scenerySetEntries[typeId]->name;
			lastDevelopmentFormat = STR_RESEARCH_SCENERY_LABEL;
		}
		gfx_draw_string_left_wrapped(dpi, &stringId, x, y, 266, lastDevelopmentFormat, 0);
	}
}

/**
 *
 *  rct2: 0x006B6ABF
 */
static void window_new_ride_scrollpaint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_ride_type **rideEntries = (rct_ride_type**)0x009ACFA4;

	window_paint_get_registers(w, dpi);

	if (_window_new_ride_current_tab == WINDOW_NEW_RIDE_PAGE_RESEARCH)
		return;

	gfx_clear(dpi, RCT2_GLOBAL(0x0141FC48 + (w->colours[1] * 8), uint8) * 0x1010101);

	int x = 1;
	int y = 1;
	ride_list_item *listItem = (ride_list_item*)0x00F43523;
	while (listItem->type != 255 || listItem->entry_index != 255) {
		rct_ride_type *rideEntry;
		// Draw flat button rectangle
		int flags = 0;
		if (w->new_ride.selected_ride_id == *((sint16*)listItem))
			flags |= 0x20;
		if (w->new_ride.highlighted_ride_id == *((sint16*)listItem) || flags != 0)
			gfx_fill_rect_inset(dpi, x, y, x + 115, y + 115, w->colours[1], 0x80 | flags);
		
		// Draw ride image
		rideEntry = rideEntries[listItem->entry_index];
		int image_id = rideEntry->images_offset;
		if (listItem->type != rideEntry->ride_type[0]) {
			image_id++;
			if (listItem->type != rideEntry->ride_type[1])
				image_id++;
		}
		RCT2_CALLPROC_X(0x00681DE2, 0, 29013, x + 2, y + 2, 0xA0, (int)dpi, image_id);

		// Next position
		x += 116;
		if (x >= 116 * 5 + 1) {
			x = 1;
			y += 116;
		}

		// Next item
		listItem++;
	}
}

/**
 *
 *  rct2: 0x006B6D3C
 */
static ride_list_item window_new_ride_scroll_get_ride_list_item_at(rct_window *w, int x, int y)
{
	ride_list_item result;
	result.type = 255;
	result.entry_index = 255;

	if (--x < 0 || --y < 0)
		return result;

	int column = x / 116;
	int row = y / 116;
	if (column >= 5)
		return result;

	int index = column + (row * 5);

	ride_list_item *listItem = (ride_list_item*)0x00F43523;
	while (listItem->type != 255 || listItem->entry_index != 255) {
		if (index-- == 0)
			return *listItem;
		listItem++;
	}

	return result;
}

static int get_num_track_designs(ride_list_item item)
{
	track_load_list(item);

	uint8 *trackDesignList = (uint8*)0x00F441EC;
	int count = 0;
	while (*trackDesignList != 0 && trackDesignList < (uint8*)0x00F635EC) {
		trackDesignList += 128;
		count++;
	}
	return count;
}

/**
 *
 *  rct2: 0x006B701C
 */
static void window_new_ride_paint_ride_information(rct_window *w, rct_drawpixelinfo *dpi, ride_list_item item, int x, int y, int width)
{
	rct_ride_type **rideEntries = (rct_ride_type**)0x009ACFA4;
	rct_ride_type *rideEntry = rideEntries[item.entry_index];

	// Ride name and description
	rct_string_id rideName = rideEntry->name;
	rct_string_id rideDescription = rideEntry->description;
	if (!(rideEntry->var_008 & 0x1000)) {
		rideName = item.type + 2;
		rideDescription = item.type + 512;
	}

	RCT2_GLOBAL(0x013CE952 + 0, rct_string_id) = rideName;
	RCT2_GLOBAL(0x013CE952 + 2, rct_string_id) = rideDescription;
	gfx_draw_string_left_wrapped(dpi, (void*)0x013CE952, x, y, width, 1690, 0);

	// Number of designs available
	if (ride_type_has_flag(item.type, RIDE_TYPE_FLAG_HAS_TRACK)) {
		if (item.type != _lastTrackDesignCountRideType.type || item.entry_index != _lastTrackDesignCountRideType.entry_index) {
			_lastTrackDesignCountRideType = item;
			_lastTrackDesignCount = get_num_track_designs(item);
		}

		rct_string_id stringId;
		switch (_lastTrackDesignCount) {
		case 0:
			stringId = 3338;
			break;
		case 1:
			stringId = 3339;
			break;
		default:
			stringId = 3340;
			break;
		}
		gfx_draw_string_left(dpi, stringId, &_lastTrackDesignCount, 0, x, y + 40);
	}

	// Price
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
		// Get price of ride
		int unk2 = RCT2_GLOBAL(0x0097CC68 + (item.type * 2), uint8);
		money32 price = RCT2_GLOBAL(0x0097DD78 + (item.type * 4), uint16);
		if (ride_type_has_flag(item.type, RIDE_TYPE_FLAG_SELLS_FOOD)) {
			price *= RCT2_ADDRESS(0x0099DE34, uint32)[unk2];
		} else {
			price *= RCT2_ADDRESS(0x0099DA34, uint32)[unk2];
		}
		price = (price >> 17) * 10 * RCT2_GLOBAL(0x0097D21D + (item.type * 8), uint8);

		// 
		rct_string_id stringId = 1691;
		if (!ride_type_has_flag(item.type, RIDE_TYPE_FLAG_15))
			stringId++;

		gfx_draw_string_right(dpi, stringId, &price, 0, x + width, y + 40);
	}
}

/**
 *
 *  rct2: 0x006B6B78
 */
static void window_new_ride_select(rct_window *w)
{
	ride_list_item item = *((ride_list_item*)&w->new_ride.selected_ride_id);
	if (item.type == 255)
		return;

	window_close(w);

	if (ride_type_has_flag(item.type, RIDE_TYPE_FLAG_HAS_TRACK)) {
		track_load_list(item);

		uint8 *trackDesignList = (uint8*)0x00F441EC;
		if (*trackDesignList != 0) {
			window_track_list_open(item);
			return;
		}
	}

	ride_construct_new(item);
}
