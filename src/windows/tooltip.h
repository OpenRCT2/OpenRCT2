#ifndef _WINDOW_TOOLTIP_H_
#define _WINDOW_TOOLTIP_H_

#include "../interface/window.h"

void window_tooltip_reset(int x, int y);
void window_tooltip_show(rct_string_id id, int x, int y);
void window_tooltip_open(rct_window *widgetWindow, int widgetIndex, int x, int y);
void window_tooltip_close();

#endif
