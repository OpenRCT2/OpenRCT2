#include "../addresses.h"
#include "../config.h"
#include "../game.h"
#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/themes.h"

static rct_widget window_title_options_widgets[] = {
	{ WWT_DROPDOWN_BUTTON, 2, 0, 79, 0, 11, STR_OPTIONS, STR_NONE },
	{ WIDGETS_END },
};

static void window_title_options_mouseup(rct_window *w, int widgetIndex);
static void window_title_options_invalidate(rct_window *w);
static void window_title_options_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_title_options_events = {
	NULL,
	window_title_options_mouseup,
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
	window_title_options_invalidate,
	window_title_options_paint,
	NULL
};

/**
 * Creates the window containing the options button on the title screen.
 */
void window_title_options_open()
{
	rct_window* window;

	window = window_create(
		gScreenWidth - 80, 0,
		80, 12,
		&window_title_options_events,
		WC_TITLE_OPTIONS,
		WF_STICK_TO_BACK | WF_TRANSPARENT
	);
	window->widgets = window_title_options_widgets;
	window->enabled_widgets |= 1;
	window_init_scroll_widgets(window);
}

static void window_title_options_mouseup(rct_window *w, int widgetIndex)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) != 0)
		return;

	if (widgetIndex == 0)
		window_options_open();
}

static void window_title_options_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
}

static void window_title_options_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}
