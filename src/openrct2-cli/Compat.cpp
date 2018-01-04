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

#include <openrct2/drawing/Rain.h>
void DrawRain(rct_drawpixelinfo * dpi, OpenRCT2::Drawing::IRainDrawer * rainDrawer) { }

#include <openrct2/interface/window.h>
#include <openrct2/interface/window_private.h>
colour_t gCurrentWindowColours[4];
uint16 gWindowUpdateTicks;
uint8 gToolbarDirtyFlags;
uint16 gWindowMapFlashingFlags;
rct_window g_window_list[WINDOW_LIMIT_MAX + WINDOW_LIMIT_RESERVED];
rct_window * gWindowNextSlot = NULL;

void textinput_cancel() {}
void tool_cancel() {}
bool tool_set(rct_window * w, rct_widgetindex widgetIndex, TOOL_IDX tool) { return false; }
void widget_invalidate(rct_window * w, rct_widgetindex widgetIndex) {}
void window_zoom_set(rct_window * w, sint32 zoomLevel, bool atCursor) {}
void window_update_viewport_ride_music() {}
void window_update_all_viewports() {}
void window_update_all() {}
void window_unfollow_sprite(rct_window * w) {}
void window_set_window_limit(sint32 value) {}
void window_set_location(rct_window * w, sint32 x, sint32 y, sint32 z) {}
rct_window * window_bring_to_front_by_class_with_flags(rct_windowclass cls, uint16 flags) { return nullptr; }
rct_window * window_bring_to_front_by_number(rct_windowclass cls, rct_windownumber number) { return nullptr; }
rct_window * window_find_by_number(rct_windowclass cls, rct_windownumber number) { return nullptr; }
rct_window * window_find_by_class(rct_windowclass cls) { return nullptr; }
rct_window * window_bring_to_front_by_class(rct_windowclass cls) { return nullptr; }
void window_event_resize_call(rct_window * w) {}
void window_close(rct_window * window) {}
void window_close_all() {}
void window_close_all_except_flags(uint16 flags) {}
void window_close_by_class(rct_windowclass cls) {}
void window_close_by_number(rct_windowclass cls, rct_windownumber number) {}
rct_viewport * window_get_viewport(rct_window * w) { return nullptr; }
void window_close_construction_windows() {}
void window_draw_all(rct_drawpixelinfo * dpi, sint16 left, sint16 top, sint16 right, sint16 bottom) {}
void window_event_invalidate_call(rct_window * w) {}
void window_invalidate_by_class(rct_windowclass cls) {}
void window_dispatch_update_all() {}
void window_event_mouse_down_call(rct_window * w, rct_widgetindex widgetIndex) {}
void window_event_mouse_up_call(rct_window * w, rct_widgetindex widgetIndex) {}
rct_window * window_find_from_point(sint32 x, sint32 y) { return nullptr; }
void window_invalidate(rct_window * window) {}
void window_follow_sprite(rct_window * w, size_t spriteIndex) {}
rct_window * window_get_main() { return nullptr; }
rct_viewport * window_get_previous_viewport(rct_viewport * current) { return nullptr; }
void window_init_all() {}
void window_invalidate_all() {}
void window_invalidate_by_number(rct_windowclass cls, rct_windownumber number) {}
bool window_is_visible(rct_window * w) { return false; }
void window_reset_visibilities() {}
void window_resize_gui(sint32 width, sint32 height) {}
void window_rotate_camera(rct_window * w, sint32 direction) {}
void window_scroll_to_location(rct_window * w, sint32 x, sint32 y, sint32 z) {}
