#include <openrct2/interface/Widget.h>

void widget_scroll_update_thumbs(rct_window *w, rct_widgetindex widget_index) { }
void widget_draw(rct_drawpixelinfo *dpi, rct_window *w, rct_widgetindex widgetIndex){ }
bool widget_is_enabled(rct_window *w, rct_widgetindex widgetIndex) { return false; }
bool widget_is_disabled(rct_window *w, rct_widgetindex widgetIndex) { return false; }
bool widget_is_pressed(rct_window *w, rct_widgetindex widgetIndex) { return false; }
bool widget_is_highlighted(rct_window *w, rct_widgetindex widgetIndex) { return false; }
bool widget_is_active_tool(rct_window *w, rct_widgetindex widgetIndex) { return false; }
void widget_scroll_get_part(rct_window *w, rct_widget* widget, sint32 x, sint32 y, sint32 *output_x, sint32 *output_y, sint32 *output_scroll_area, sint32 *scroll_id) { }
void widget_set_enabled(rct_window *w, rct_widgetindex widgetIndex, bool enabled) { }
void widget_set_checkbox_value(rct_window *w, rct_widgetindex widgetIndex, sint32 value) { }
