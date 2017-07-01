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

#include "../config/Config.h"
#include "../Context.h"
#include "../drawing/drawing.h"
#include "../game.h"
#include "../input.h"
#include "../localisation/localisation.h"
#include "../paint/paint.h"
#include "../paint/supports.h"
#include "../peep/staff.h"
#include "../rct2.h"
#include "../ride/ride_data.h"
#include "../ride/track_data.h"
#include "../world/banner.h"
#include "../world/Climate.h"
#include "../world/entrance.h"
#include "../world/footpath.h"
#include "../world/map.h"
#include "../world/scenery.h"
#include "../world/sprite.h"
#include "colour.h"
#include "viewport.h"
#include "window.h"

//#define DEBUG_SHOW_DIRTY_BOX
uint8 gShowGridLinesRefCount;
uint8 gShowLandRightsRefCount;
uint8 gShowConstuctionRightsRefCount;

rct_viewport g_viewport_list[MAX_VIEWPORT_COUNT];
rct_viewport *g_music_tracking_viewport;

rct_map_element *_interaction_element = NULL;

sint16 gSavedViewX;
sint16 gSavedViewY;
uint8 gSavedViewZoom;
uint8 gSavedViewRotation;

#ifdef NO_RCT2
paint_entry *gNextFreePaintStruct;
uint8 gCurrentRotation;
uint32 gCurrentViewportFlags = 0;
#endif

uint32 gUnkEDF81C;

static rct_drawpixelinfo _viewportDpi1;
static rct_drawpixelinfo _viewportDpi2;
static uint8 _interactionSpriteType;
static uint8 _unk9AC149;
static sint16 _interactionMapX;
static sint16 _interactionMapY;
static uint16 _unk9AC154;
static sint16 _unk9ABDAE;

static void viewport_paint_column(rct_drawpixelinfo * dpi, uint32 viewFlags);
static void viewport_paint_weather_gloom(rct_drawpixelinfo * dpi);

/**
 * This is not a viewport function. It is used to setup many variables for
 * multiple things.
 *  rct2: 0x006E6EAC
 */
void viewport_init_all()
{
    colours_init_maps();
    if (gWindowNextSlot != NULL) {
        window_close_all();
    }
    gWindowNextSlot = g_window_list;

    // Setting up viewports
    for (sint32 i = 0; i < MAX_VIEWPORT_COUNT; i++) {
        g_viewport_list[i].width = 0;
    }

    // ?
    input_reset_flags();
    input_set_state(INPUT_STATE_RESET);
    gPressedWidget.window_classification = 255;
    gPickupPeepImage = UINT32_MAX;
    reset_tooltip_not_shown();
    gMapSelectFlags = 0;
    gStaffDrawPatrolAreas = 0xFFFF;
    textinput_cancel();
}

/**
 * Converts between 3d point of a sprite to 2d coordinates for centring on that
 * sprite
 *  rct2: 0x006EB0C1
 * x : ax
 * y : bx
 * z : cx
 * out_x : ax
 * out_y : bx
 */
void centre_2d_coordinates(sint32 x, sint32 y, sint32 z, sint32 * out_x, sint32 * out_y, rct_viewport * viewport){
    sint32 start_x = x;

    rct_xyz16 coord_3d = {
        .x = x,
        .y = y,
        .z = z
    };

    rct_xy16 coord_2d = coordinate_3d_to_2d(&coord_3d, get_current_rotation());

    // If the start location was invalid
    // propagate the invalid location to the output.
    // This fixes a bug that caused the game to enter an infinite loop.
    if (start_x == ((sint16)(uint16)0x8000)){
        *out_x = ((sint16)(uint16)0x8000);
        *out_y = 0;
        return;
    }

    *out_x = coord_2d.x - viewport->view_width / 2;
    *out_y = coord_2d.y - viewport->view_height / 2;
}

/**
* Viewport will look at sprite or at coordinates as specified in flags 0b_1X
* for sprite 0b_0X for coordinates
*
*  rct2: 0x006EB009
*  x:      ax
*  y:      eax (top 16)
*  width:  bx
*  height: ebx (top 16)
*  zoom:   cl (8 bits)
*  centre_x: edx lower 16 bits
*  centre_y: edx upper 16 bits
*  centre_z: ecx upper 16 bits
*  sprite: edx lower 16 bits
*  flags:  edx top most 2 bits 0b_X1 for zoom clear see below for 2nd bit.
*  w:      esi
*/
void viewport_create(rct_window *w, sint32 x, sint32 y, sint32 width, sint32 height, sint32 zoom, sint32 centre_x, sint32 centre_y, sint32 centre_z, char flags, sint16 sprite)
{
    rct_viewport* viewport = NULL;
    for (sint32 i = 0; i < MAX_VIEWPORT_COUNT; i++) {
        if (g_viewport_list[i].width == 0) {
            viewport = &g_viewport_list[i];
            break;
        }
    }
    if (viewport == NULL) {
        log_error("No more viewport slots left to allocate.");
        return;
    }

    viewport->x = x;
    viewport->y = y;
    viewport->width = width;
    viewport->height = height;

    if (!(flags & VIEWPORT_FOCUS_TYPE_COORDINATE)){
        zoom = 0;
    }

    viewport->view_width = width << zoom;
    viewport->view_height = height << zoom;
    viewport->zoom = zoom;
    viewport->flags = 0;

    if (gConfigGeneral.always_show_gridlines)
        viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
    w->viewport = viewport;

    if (flags & VIEWPORT_FOCUS_TYPE_SPRITE){
        w->viewport_target_sprite = sprite;
        rct_sprite* centre_sprite = get_sprite(sprite);
        centre_x = centre_sprite->unknown.x;
        centre_y = centre_sprite->unknown.y;
        centre_z = centre_sprite->unknown.z;
    }
    else{
        w->viewport_target_sprite = SPRITE_INDEX_NULL;
    }

    sint32 view_x, view_y;
    centre_2d_coordinates(centre_x, centre_y, centre_z, &view_x, &view_y, viewport);

    w->saved_view_x = view_x;
    w->saved_view_y = view_y;
    viewport->view_x = view_x;
    viewport->view_y = view_y;

    viewport_update_pointers();
}

/**
 *
 *  rct2: 0x006EE510
 */
void viewport_update_pointers()
{

}

/**
 *
 *  rct2: 0x00689174
 * edx is assumed to be (and always is) the current rotation, so it is not
 * needed as parameter.
 */
void viewport_adjust_for_map_height(sint16* x, sint16* y, sint16 *z)
{
    sint16 start_x = *x;
    sint16 start_y = *y;
    sint16 height = 0;

    uint32 rotation = get_current_rotation();
    rct_xy16 pos;
    for (sint32 i = 0; i < 6; i++) {
        pos = viewport_coord_to_map_coord(start_x, start_y, height);
        height = map_element_height((0xFFFF) & pos.x, (0xFFFF) & pos.y);

        // HACK: This is to prevent the x and y values being set to values outside
        // of the map. This can happen when the height is larger than the map size.
        sint16 max = gMapSizeMinus2;
        if (pos.x > max && pos.y > max) {
            sint32 x_corr[] = { -1, 1, 1, -1 };
            sint32 y_corr[] = { -1, -1, 1, 1 };
            pos.x += x_corr[rotation] * height;
            pos.y += y_corr[rotation] * height;
        }
    }

    *x = pos.x;
    *y = pos.y;
    *z = height;
}

/*
*  rct2: 0x006E7FF3
*/
static void viewport_redraw_after_shift(rct_drawpixelinfo *dpi, rct_window *window, rct_viewport *viewport, sint32 x, sint32 y)
{
    // sub-divide by intersecting windows
    if (window < gWindowNextSlot)
    {
        // skip current window and non-intersecting windows
        if (viewport == window->viewport                                ||
            viewport->x + viewport->width  <= window->x                 ||
            viewport->x                    >= window->x + window->width ||
            viewport->y + viewport->height <= window->y                 ||
            viewport->y                    >= window->y + window->height){
            viewport_redraw_after_shift(dpi, window + 1, viewport, x, y);
            return;
        }

        // save viewport
        rct_viewport view_copy;
        memcpy(&view_copy, viewport, sizeof(rct_viewport));

        if (viewport->x < window->x)
        {
            viewport->width = window->x - viewport->x;
            viewport->view_width = viewport->width << viewport->zoom;
            viewport_redraw_after_shift(dpi, window, viewport, x, y);

            viewport->x += viewport->width;
            viewport->view_x += viewport->width << viewport->zoom;
            viewport->width = view_copy.width - viewport->width;
            viewport->view_width = viewport->width << viewport->zoom;
            viewport_redraw_after_shift(dpi, window, viewport, x, y);
        }
        else if (viewport->x + viewport->width > window->x + window->width)
        {
            viewport->width = window->x + window->width - viewport->x;
            viewport->view_width = viewport->width << viewport->zoom;
            viewport_redraw_after_shift(dpi, window, viewport, x, y);

            viewport->x += viewport->width;
            viewport->view_x += viewport->width << viewport->zoom;
            viewport->width = view_copy.width - viewport->width;
            viewport->view_width = viewport->width << viewport->zoom;
            viewport_redraw_after_shift(dpi, window, viewport, x, y);
        }
        else if (viewport->y < window->y)
        {
            viewport->height = window->y - viewport->y;
            viewport->view_width = viewport->width << viewport->zoom;
            viewport_redraw_after_shift(dpi, window, viewport, x, y);

            viewport->y += viewport->height;
            viewport->view_y += viewport->height << viewport->zoom;
            viewport->height = view_copy.height - viewport->height;
            viewport->view_width = viewport->width << viewport->zoom;
            viewport_redraw_after_shift(dpi, window, viewport, x, y);
        }
        else if (viewport->y + viewport->height > window->y + window->height)
        {
            viewport->height = window->y + window->height - viewport->y;
            viewport->view_width = viewport->width << viewport->zoom;
            viewport_redraw_after_shift(dpi, window, viewport, x, y);

            viewport->y += viewport->height;
            viewport->view_y += viewport->height << viewport->zoom;
            viewport->height = view_copy.height - viewport->height;
            viewport->view_width = viewport->width << viewport->zoom;
            viewport_redraw_after_shift(dpi, window, viewport, x, y);
        }

        // restore viewport
        memcpy(viewport, &view_copy, sizeof(rct_viewport));
    }
    else
    {
        sint16 left = viewport->x;
        sint16 right = viewport->x + viewport->width;
        sint16 top = viewport->y;
        sint16 bottom = viewport->y + viewport->height;

        // if moved more than the viewport size
        if (abs(x) < viewport->width && abs(y) < viewport->height)
        {
            // update whole block ?
            drawing_engine_copy_rect(viewport->x, viewport->y, viewport->width, viewport->height, x, y);

            if (x > 0)
            {
                // draw left
                sint16 _right = viewport->x + x;
                window_draw_all(dpi, left, top, _right, bottom);
                left += x;
            }
            else if (x < 0)
            {
                // draw right
                sint16 _left = viewport->x + viewport->width + x;
                window_draw_all(dpi, _left, top, right, bottom);
                right += x;
            }

            if (y > 0)
            {
                // draw top
                bottom = viewport->y + y;
                window_draw_all(dpi, left, top, right, bottom);
            }
            else if (y < 0)
            {
                // draw bottom
                top = viewport->y + viewport->height + y;
                window_draw_all(dpi, left, top, right, bottom);
            }
        }
        else
        {
            // redraw whole viewport
            window_draw_all(dpi, left, top, right, bottom);
        }
    }
}

static void viewport_shift_pixels(rct_drawpixelinfo *dpi, rct_window* w, rct_viewport* viewport, sint16 x_diff, sint16 y_diff)
{
    rct_window* orignal_w = w;
    sint32 left = 0, right = 0, top = 0, bottom = 0;

    for (; w < gWindowNextSlot; w++){
        if (!(w->flags & WF_TRANSPARENT)) continue;
        if (w->viewport == viewport) continue;

        if (viewport->x + viewport->width <= w->x)continue;
        if (w->x + w->width <= viewport->x) continue;

        if (viewport->y + viewport->height <= w->y)continue;
        if (w->y + w->height <= viewport->y) continue;

        left = w->x;
        right = w->x + w->width;
        top = w->y;
        bottom = w->y + w->height;

        if (left < viewport->x)left = viewport->x;
        if (right > viewport->x + viewport->width) right = viewport->x + viewport->width;

        if (top < viewport->y)top = viewport->y;
        if (bottom > viewport->y + viewport->height) bottom = viewport->y + viewport->height;

        if (left >= right) continue;
        if (top >= bottom) continue;

        window_draw_all(dpi, left, top, right, bottom);
    }

    w = orignal_w;
    viewport_redraw_after_shift(dpi, w, viewport, x_diff, y_diff);
}

static void viewport_move(sint16 x, sint16 y, rct_window* w, rct_viewport* viewport)
{
    uint8 zoom = (1 << viewport->zoom);

    // Note: do not do the subtraction and then divide!
    // Note: Due to arithmetic shift != /zoom a shift will have to be used
    // hopefully when 0x006E7FF3 is finished this can be converted to /zoom.
    sint16 x_diff = (viewport->view_x >> viewport->zoom) - (x >> viewport->zoom);
    sint16 y_diff = (viewport->view_y >> viewport->zoom) - (y >> viewport->zoom);

    viewport->view_x = x;
    viewport->view_y = y;

    // If no change in viewing area
    if ((!x_diff) && (!y_diff))return;

    if (w->flags & WF_7){
        sint32 left = max(viewport->x, 0);
        sint32 top = max(viewport->y, 0);
        sint32 right = min(viewport->x + viewport->width, context_get_width());
        sint32 bottom = min(viewport->y + viewport->height, context_get_height());

        if (left >= right) return;
        if (top >= bottom) return;

        if (drawing_engine_has_dirty_optimisations()) {
            rct_drawpixelinfo *dpi = drawing_engine_get_dpi();
            window_draw_all(dpi, left, top, right, bottom);
            return;
        }
    }

    rct_viewport view_copy;
    memcpy(&view_copy, viewport, sizeof(rct_viewport));

    if (viewport->x < 0){
        viewport->width += viewport->x;
        viewport->view_width += viewport->x * zoom;
        viewport->view_x -= viewport->x * zoom;
        viewport->x = 0;
    }

    sint32 eax = viewport->x + viewport->width - context_get_width();
    if (eax > 0){
        viewport->width -= eax;
        viewport->view_width -= eax * zoom;
    }

    if (viewport->width <= 0){
        memcpy(viewport, &view_copy, sizeof(rct_viewport));
        return;
    }

    if (viewport->y < 0){
        viewport->height += viewport->y;
        viewport->view_height += viewport->y * zoom;
        viewport->view_y -= viewport->y * zoom;
        viewport->y = 0;
    }

    eax = viewport->y + viewport->height - context_get_height();
    if (eax > 0){
        viewport->height -= eax;
        viewport->view_height -= eax * zoom;
    }

    if (viewport->height <= 0){
        memcpy(viewport, &view_copy, sizeof(rct_viewport));
        return;
    }

    if (drawing_engine_has_dirty_optimisations()) {
        rct_drawpixelinfo *dpi = drawing_engine_get_dpi();
        viewport_shift_pixels(dpi, w, viewport, x_diff, y_diff);
    }

    memcpy(viewport, &view_copy, sizeof(rct_viewport));
}

//rct2: 0x006E7A15
static void viewport_set_underground_flag(sint32 underground, rct_window* window, rct_viewport* viewport)
{
    if (window->classification != WC_MAIN_WINDOW)
    {
        if (!underground)
        {
            sint32 bit = viewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE;
            viewport->flags &= ~VIEWPORT_FLAG_UNDERGROUND_INSIDE;
            if (!bit) return;
        }
        else
        {
            sint32 bit = viewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE;
            viewport->flags |= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
            if (bit) return;
        }
        window_invalidate(window);
    }
}

/**
 *
 *  rct2: 0x006E7A3A
 */
void viewport_update_position(rct_window *window)
{
    window_event_resize_call(window);

    rct_viewport* viewport = window->viewport;
    if (!viewport)return;

    if (window->viewport_target_sprite != SPRITE_INDEX_NULL) {
        viewport_update_sprite_follow(window);
        return;
    }

    sint16 x = viewport->view_width / 2 + window->saved_view_x;
    sint16 y = viewport->view_height / 2 + window->saved_view_y;
    sint16 z;

    viewport_adjust_for_map_height(&x, &y, &z);

    viewport_set_underground_flag(0, window, viewport);

    //Clamp to the map minimum value
    sint32 at_map_edge_x = 0;
    if (x < MAP_MINIMUM_X_Y){
        x = MAP_MINIMUM_X_Y;
        at_map_edge_x = 1;
    }
    sint32 at_map_edge_y = 0;
    if (y < MAP_MINIMUM_X_Y){
        y = MAP_MINIMUM_X_Y;
        at_map_edge_y = 1;
    }

    //Clamp to the map maximum value (scenario specific)
    if (x > gMapSizeMinus2){
        x = gMapSizeMinus2;
        at_map_edge_x = 1;
    }
    if (y > gMapSizeMinus2){
        y = gMapSizeMinus2;
        at_map_edge_y = 1;
    }

    if (at_map_edge_x || at_map_edge_y) {
        // The &0xFFFF is to prevent the sign extension messing the
        // function up.
        sint32 zz = map_element_height(x & 0xFFFF, y & 0xFFFF);
        sint32 _2d_x, _2d_y;
        centre_2d_coordinates(x, y, zz, &_2d_x, &_2d_y, viewport);

        if (at_map_edge_x)
            window->saved_view_x = _2d_x;
        if (at_map_edge_y)
            window->saved_view_y = _2d_y;
    }

    x = window->saved_view_x;
    y = window->saved_view_y;
    if (window->flags & WF_SCROLLING_TO_LOCATION){
        // Moves the viewport if focusing in on an item
        uint8 flags = 0;
        x -= viewport->view_x;
        if (x < 0){
            x = -x;
            flags |= 1;
        }
        y -= viewport->view_y;
        if (y < 0){
            y = -y;
            flags |= 2;
        }
        x = (x + 7) / 8;
        y = (y + 7) / 8;

        //If we are at the final zoom position
        if (!x && !y){
            window->flags &= ~WF_SCROLLING_TO_LOCATION;
        }
        if (flags & 1){
            x = -x;
        }
        if (flags & 2){
            y = -y;
        }
        x += viewport->view_x;
        y += viewport->view_y;
    }

    viewport_move(x, y, window, viewport);
}

void viewport_update_sprite_follow(rct_window *window)
{
    if (window->viewport_target_sprite != SPRITE_INDEX_NULL && window->viewport) {
        rct_sprite* sprite = get_sprite(window->viewport_target_sprite);

        sint32 height = (map_element_height(0xFFFF & sprite->unknown.x, 0xFFFF & sprite->unknown.y) & 0xFFFF) - 16;
        sint32 underground = sprite->unknown.z < height;

        viewport_set_underground_flag(underground, window, window->viewport);

        sint32 centre_x, centre_y;
        centre_2d_coordinates(sprite->unknown.x, sprite->unknown.y, sprite->unknown.z, &centre_x, &centre_y,
                              window->viewport);

        viewport_move(centre_x, centre_y, window, window->viewport);
    }
}

/**
 *
 *  rct2: 0x00685C02
 *  ax: left
 *  bx: top
 *  dx: right
 *  esi: viewport
 *  edi: dpi
 *  ebp: bottom
 */
void viewport_render(rct_drawpixelinfo *dpi, rct_viewport *viewport, sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    if (right <= viewport->x) return;
    if (bottom <= viewport->y) return;
    if (left >= viewport->x + viewport->width)return;
    if (top >= viewport->y + viewport->height)return;

#ifdef DEBUG_SHOW_DIRTY_BOX
    sint32 l = left, t = top, r = right, b = bottom;
#endif

    left = max(left - viewport->x, 0);
    right = min(right - viewport->x, viewport->width);
    top = max(top - viewport->y, 0);
    bottom = min(bottom - viewport->y, viewport->height);

    left <<= viewport->zoom;
    right <<= viewport->zoom;
    top <<= viewport->zoom;
    bottom <<= viewport->zoom;

    left += viewport->view_x;
    right += viewport->view_x;
    top += viewport->view_y;
    bottom += viewport->view_y;

    viewport_paint(viewport, dpi, left, top, right, bottom);

#ifdef DEBUG_SHOW_DIRTY_BOX
    if (viewport != g_viewport_list){
        gfx_fill_rect_inset(dpi, l, t, r-1, b-1, 0x2, INSET_RECT_F_30);
        return;
    }
#endif
}

/**
 *
 *  rct2: 0x00685CBF
 *  eax: left
 *  ebx: top
 *  edx: right
 *  esi: viewport
 *  edi: dpi
 *  ebp: bottom
 */
void viewport_paint(rct_viewport* viewport, rct_drawpixelinfo* dpi, sint16 left, sint16 top, sint16 right, sint16 bottom)
{
    uint32 viewFlags = viewport->flags;
    uint16 width = right - left;
    uint16 height = bottom - top;
    uint16 bitmask = 0xFFFF & (0xFFFF << viewport->zoom);

    width &= bitmask;
    height &= bitmask;
    left &= bitmask;
    top &= bitmask;
    right = left + width;
    bottom = top + height;

    sint16 x = (sint16)(left - (sint16)(viewport->view_x & bitmask));
    x >>= viewport->zoom;
    x += viewport->x;

    sint16 y = (sint16)(top - (sint16)(viewport->view_y & bitmask));
    y >>= viewport->zoom;
    y += viewport->y;

    rct_drawpixelinfo dpi1;
    dpi1.bits = dpi->bits + (x - dpi->x) + ((y - dpi->y) * (dpi->width + dpi->pitch));
    dpi1.x = left;
    dpi1.y = top;
    dpi1.width = width;
    dpi1.height = height;
    dpi1.pitch = (dpi->width + dpi->pitch) - (width >> viewport->zoom);
    dpi1.zoom_level = viewport->zoom;

    // Splits the area into 32 pixel columns and renders them
    for (x = floor2(dpi1.x, 32); x < dpi1.x + dpi1.width; x += 32) {
        rct_drawpixelinfo dpi2 = dpi1;
        if (x >= dpi2.x) {
            sint16 leftPitch = x - dpi2.x;
            dpi2.width -= leftPitch;
            dpi2.bits += leftPitch >> dpi2.zoom_level;
            dpi2.pitch += leftPitch >> dpi2.zoom_level;
            dpi2.x = x;
        }

        sint16 paintRight = dpi2.x + dpi2.width;
        if (paintRight >= x + 32) {
            sint16 rightPitch = paintRight - x - 32;
            paintRight -= rightPitch;
            dpi2.pitch += rightPitch >> dpi2.zoom_level;
        }
        dpi2.width = paintRight - dpi2.x;

        viewport_paint_column(&dpi2, viewFlags);
    }
}

static void viewport_paint_column(rct_drawpixelinfo * dpi, uint32 viewFlags)
{
    gCurrentViewportFlags = viewFlags;

    if (viewFlags & (VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT)) {
        uint8 colour = 10;
        if (viewFlags & VIEWPORT_FLAG_INVISIBLE_SPRITES) {
            colour = 0;
        }
        gfx_clear(dpi, colour);
    }
    paint_init(dpi);
    paint_generate_structs(dpi);
    paint_struct ps = paint_arrange_structs();
    paint_draw_structs(dpi, &ps, viewFlags);

    if (gConfigGeneral.render_weather_gloom &&
        !gTrackDesignSaveMode &&
        !(viewFlags & VIEWPORT_FLAG_INVISIBLE_SPRITES)
    ) {
        viewport_paint_weather_gloom(dpi);
    }

    if (gPaintPSStringHead != NULL) {
        paint_draw_money_structs(dpi, gPaintPSStringHead);
    }
}

static void viewport_paint_weather_gloom(rct_drawpixelinfo * dpi)
{
    uint8 gloom = gClimateCurrentWeatherGloom;
    if (gloom != 0) {
        gfx_filter_rect(
            dpi,
            dpi->x,
            dpi->y,
            dpi->width + dpi->x - 1,
            dpi->height + dpi->y - 1,
            ClimateWeatherGloomColours[gloom]
        );
    }
}

/**
 *
 *  rct2: 0x0068958D
 */
void screen_pos_to_map_pos(sint16 *x, sint16 *y, sint32 *direction)
{
    screen_get_map_xy(*x, *y, x, y, NULL);
    if (*x == MAP_LOCATION_NULL)
        return;

    sint32 my_direction;
    sint32 dist_from_centre_x = abs(*x % 32);
    sint32 dist_from_centre_y = abs(*y % 32);
    if (dist_from_centre_x > 8 && dist_from_centre_x < 24 &&
        dist_from_centre_y > 8 && dist_from_centre_y < 24) {
        my_direction = 4;
    } else {
        sint16 mod_x = *x & 0x1F;
        sint16 mod_y = *y & 0x1F;
        if (mod_x <= 16) {
            if (mod_y < 16) {
                my_direction = 2;
            } else {
                my_direction = 3;
            }
        } else {
            if (mod_y < 16) {
                my_direction = 1;
            } else {
                my_direction = 0;
            }
        }
    }

    *x = *x & ~0x1F;
    *y = *y & ~0x1F;
    if (direction != NULL) *direction = my_direction;
}

rct_xy16 screen_coord_to_viewport_coord(rct_viewport *viewport, uint16 x, uint16 y)
{
    rct_xy16 ret;
    ret.x = ((x - viewport->x) << viewport->zoom) + viewport->view_x;
    ret.y = ((y - viewport->y) << viewport->zoom) + viewport->view_y;
    return ret;
}

rct_xy16 viewport_coord_to_map_coord(sint32 x, sint32 y, sint32 z)
{
    rct_xy16 ret = { 0 };
    switch (get_current_rotation()) {
    case 0:
        ret.x = -x / 2 + y + z;
        ret.y = x / 2 + y + z;
        break;
    case 1:
        ret.x = -x / 2 - y - z;
        ret.y = -x / 2 + y + z;
        break;
    case 2:
        ret.x = x / 2 - y - z;
        ret.y = -x / 2 - y - z;
        break;
    case 3:
        ret.x = x / 2 + y + z;
        ret.y = x / 2 - y - z;
        break;
    }
    return ret;
}

/**
 *
 *  rct2: 0x00664689
 */
void show_gridlines()
{
    if (gShowGridLinesRefCount == 0) {
        rct_window *mainWindow = window_get_main();
        if (mainWindow != NULL) {
            if (!(mainWindow->viewport->flags & VIEWPORT_FLAG_GRIDLINES)) {
                mainWindow->viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
                window_invalidate(mainWindow);
            }
        }
    }
    gShowGridLinesRefCount++;
}

/**
 *
 *  rct2: 0x006646B4
 */
void hide_gridlines()
{
    gShowGridLinesRefCount--;
    if (gShowGridLinesRefCount == 0) {
        rct_window *mainWindow = window_get_main();
        if (mainWindow != NULL) {
            if (!gConfigGeneral.always_show_gridlines) {
                mainWindow->viewport->flags &= ~VIEWPORT_FLAG_GRIDLINES;
                window_invalidate(mainWindow);
            }
        }
    }
}

/**
 *
 *  rct2: 0x00664E8E
 */
void show_land_rights()
{
    if (gShowLandRightsRefCount == 0) {
        rct_window *mainWindow = window_get_main();
        if (mainWindow != NULL) {
            if (!(mainWindow->viewport->flags & VIEWPORT_FLAG_LAND_OWNERSHIP)) {
                mainWindow->viewport->flags |= VIEWPORT_FLAG_LAND_OWNERSHIP;
                window_invalidate(mainWindow);
            }
        }
    }
    gShowLandRightsRefCount++;
}

/**
 *
 *  rct2: 0x00664EB9
 */
void hide_land_rights()
{
    gShowLandRightsRefCount--;
    if (gShowLandRightsRefCount == 0) {
        rct_window *mainWindow = window_get_main();
        if (mainWindow != NULL) {
            if (mainWindow->viewport->flags & VIEWPORT_FLAG_LAND_OWNERSHIP) {
                mainWindow->viewport->flags &= ~VIEWPORT_FLAG_LAND_OWNERSHIP;
                window_invalidate(mainWindow);
            }
        }
    }
}

/**
 *
 *  rct2: 0x00664EDD
 */
void show_construction_rights()
{
    if (gShowConstuctionRightsRefCount == 0) {
        rct_window *mainWindow = window_get_main();
        if (mainWindow != NULL) {
            if (!(mainWindow->viewport->flags & VIEWPORT_FLAG_CONSTRUCTION_RIGHTS)) {
                mainWindow->viewport->flags |= VIEWPORT_FLAG_CONSTRUCTION_RIGHTS;
                window_invalidate(mainWindow);
            }
        }
    }
    gShowConstuctionRightsRefCount++;
}

/**
 *
 *  rct2: 0x00664F08
 */
void hide_construction_rights()
{
    gShowConstuctionRightsRefCount--;
    if (gShowConstuctionRightsRefCount == 0) {
        rct_window *mainWindow = window_get_main();
        if (mainWindow != NULL) {
            if (mainWindow->viewport->flags & VIEWPORT_FLAG_CONSTRUCTION_RIGHTS) {
                mainWindow->viewport->flags &= ~VIEWPORT_FLAG_CONSTRUCTION_RIGHTS;
                window_invalidate(mainWindow);
            }
        }
    }
}

/**
 *
 *  rct2: 0x006CB70A
 */
void viewport_set_visibility(uint8 mode)
{
    rct_window* window = window_get_main();

    if (window != NULL) {
        rct_viewport* edi = window->viewport;
        uint32 invalidate = 0;

        switch (mode) {
        case 0: { //Set all these flags to 0, and invalidate if any were active
            uint32 mask = VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_SEETHROUGH_RIDES |
                VIEWPORT_FLAG_SEETHROUGH_SCENERY | VIEWPORT_FLAG_SEETHROUGH_PATHS | VIEWPORT_FLAG_INVISIBLE_SUPPORTS |
                VIEWPORT_FLAG_LAND_HEIGHTS | VIEWPORT_FLAG_TRACK_HEIGHTS |
                VIEWPORT_FLAG_PATH_HEIGHTS | VIEWPORT_FLAG_INVISIBLE_PEEPS |
                VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_VERTICAL;

            invalidate += edi->flags & mask;
            edi->flags &= ~mask;
            break;
        }
        case 1: //6CB79D
        case 4: //6CB7C4
            //Set underground on, invalidate if it was off
            invalidate += !(edi->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE);
            edi->flags |= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
            break;
        case 2: //6CB7EB
            //Set track heights on, invalidate if off
            invalidate += !(edi->flags & VIEWPORT_FLAG_TRACK_HEIGHTS);
            edi->flags |= VIEWPORT_FLAG_TRACK_HEIGHTS;
            break;
        case 3: //6CB7B1
        case 5: //6CB7D8
            //Set underground off, invalidate if it was on
            invalidate += edi->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE;
            edi->flags &= ~((uint16)VIEWPORT_FLAG_UNDERGROUND_INSIDE);
            break;
        }
        if (invalidate != 0)
            window_invalidate(window);
    }
}

/**
 * Stores some info about the element pointed at, if requested for this particular type through the interaction mask.
 * Originally checked 0x0141F569 at start
 *  rct2: 0x00688697
 */
static void store_interaction_info(paint_struct *ps)
{
    if (ps->sprite_type == VIEWPORT_INTERACTION_ITEM_NONE
        || ps->sprite_type == 11 // 11 as a type seems to not exist, maybe part of the typo mentioned later on.
        || ps->sprite_type > VIEWPORT_INTERACTION_ITEM_BANNER) return;

    uint16 mask;
    if (ps->sprite_type == VIEWPORT_INTERACTION_ITEM_BANNER)
        // I think CS made a typo here. Let's replicate the original behaviour.
        mask = 1 << (ps->sprite_type - 3);
    else
        mask = 1 << (ps->sprite_type - 1);

    if (!(_unk9AC154 & mask)) {
        _interactionSpriteType = ps->sprite_type;
        _unk9AC149 = ps->var_29;
        _interactionMapX = ps->map_x;
        _interactionMapY = ps->map_y;
        _interaction_element = ps->mapElement;
    }
}

/**
 * rct2: 0x00679236, 0x00679662, 0x00679B0D, 0x00679FF1
 */
static bool sub_679236_679662_679B0D_679FF1(uint32 ebx, rct_g1_element *image, uint8 *esi) {
    // Probably used to check for corruption
    if (!(image->flags & G1_FLAG_BMP)) {
        return false;
    }

    if (ebx & 0x20000000) {
        uint8 *ebx_palette = unk_9ABDA4;

        uint8 al = *esi;
        uint8 al2 = *(al + ebx_palette);

        return (al2 != 0);
    }

    if (ebx & 0x40000000) {
        return false;
    }

    return (*esi != 0);
}

/**
 * rct2: 0x0067933B, 0x00679788, 0x00679C4A, 0x0067A117
 */
static bool sub_67933B_679788_679C4A_67A117(uint8 *esi, sint16 x_start_point, sint16 y_start_point, sint32 round) {
    const uint8 *ebx = esi + ((uint16 *) esi)[y_start_point];

    uint8 last_data_line = 0;
    while (!last_data_line) {
        sint32 no_pixels = *ebx++;
        uint8 gap_size = *ebx++;

        last_data_line = no_pixels & 0x80;

        no_pixels &= 0x7F;

        ebx += no_pixels;

        if (round > 1) {
            if (gap_size % 2) {
                gap_size++;
                no_pixels--;
                if (no_pixels == 0) {
                    continue;
                }
            }
        }

        if (round == 4) {
            if (gap_size % 4) {
                gap_size += 2;
                no_pixels -= 2;
                if (no_pixels <= 0) {
                    continue;
                }
            }
        }

        sint32 x_start = gap_size - x_start_point;
        if (x_start <= 0) {
            no_pixels += x_start;
            if (no_pixels <= 0) {
                continue;
            }

            x_start = 0;
        } else {
            // Do nothing?
        }

        x_start += no_pixels;
        x_start--;
        if (x_start > 0) {
            no_pixels -= x_start;
            if (no_pixels <= 0) {
                continue;
            }
        }

        if (round > 1) {
            // This matches the original implementation, but allows empty lines to cause false positives on zoom 0
            if (ceil2(no_pixels, round) == 0) continue;
        }

        return true;
    }

    return false;
}

/**
 * rct2: 0x00679074
 *
 * @param dpi (edi)
 * @param imageId (ebx)
 * @param x (cx)
 * @param y (dx)
 * @return value originally stored in 0x00141F569
 */
static bool sub_679074(rct_drawpixelinfo *dpi, sint32 imageId, sint16 x, sint16 y)
{
    rct_g1_element *image = gfx_get_g1_element(imageId & 0x7FFFF);

    if (dpi->zoom_level != 0) {
        if (image->flags & 0x20) {
            return false;
        }

        if (image->flags & 0x10) {
            // TODO: SAR in dpi done with `>> 1`, in coordinates with `/ 2`
            rct_drawpixelinfo zoomed_dpi = {
                    .bits = dpi->bits,
                    .x = dpi->x >> 1,
                    .y = dpi->y >> 1,
                    .height = dpi->height,
                    .width = dpi->width,
                    .pitch = dpi->pitch,
                    .zoom_level = dpi->zoom_level - 1
            };

            return sub_679074(&zoomed_dpi, imageId - image->zoomed_offset, x / 2, y / 2);
        }
    }

    sint32 round = 1 << dpi->zoom_level;

    if (image->flags & G1_FLAG_RLE_COMPRESSION) {
        y -= (round - 1);
    }

    y += image->y_offset;
    sint16 yStartPoint = 0;
    sint16 height = image->height;
    if (dpi->zoom_level != 0) {
        if (height % 2) {
            height--;
            yStartPoint++;
        }

        if (dpi->zoom_level == 2) {
            if (height % 4) {
                height -= 2;
                yStartPoint += 2;
            }
        }

        if (height == 0) {
            return false;
        }
    }

    y = floor2(y, round);
    sint16 yEndPoint = height;
    y -= dpi->y;
    if (y < 0) {
        yEndPoint += y;
        if (yEndPoint <= 0) {
            return false;
        }

        yStartPoint -= y;
        y = 0;
    }

    y += yEndPoint;
    y--;
    if (y > 0) {
        yEndPoint -= y;
        if (yEndPoint <= 0) {
            return false;
        }
    }

    sint16 xStartPoint = 0;
    sint16 xEndPoint = image->width;
    if (!(image->flags & G1_FLAG_RLE_COMPRESSION)) {
        _unk9ABDAE = 0;
    }

    x += image->x_offset;
    x = floor2(x, round);
    x -= dpi->x;
    if (x < 0) {
        xEndPoint += x;
        if (xEndPoint <= 0) {
            return false;
        }

        if (!(image->flags & G1_FLAG_RLE_COMPRESSION)) {
            _unk9ABDAE -= x;
        }

        xStartPoint -= x;
        x = 0;
    }

    x += xEndPoint;
    x--;
    if (x > 0) {
        xEndPoint -= x;
        if (xEndPoint <= 0) {
            return false;
        }

        if (!(image->flags & G1_FLAG_RLE_COMPRESSION)) {
            _unk9ABDAE += x;
        }
    }

    if (image->flags & G1_FLAG_RLE_COMPRESSION) {
        return sub_67933B_679788_679C4A_67A117(image->offset, xStartPoint, yStartPoint, round);
    }

    uint8 *offset = image->offset + (yStartPoint * image->width) + xStartPoint;
    uint32 ebx = gUnkEDF81C;

    if (!(image->flags & 2)) {
        return sub_679236_679662_679B0D_679FF1(ebx, image, offset);
    }

    // The code below is untested.
    sint32 total_no_pixels = image->width * image->height;
    uint8 *source_pointer = image->offset;
    uint8 *new_source_pointer_start = malloc(total_no_pixels);
    uint8 *new_source_pointer = (*&new_source_pointer_start);// 0x9E3D28;
    intptr_t ebx1;
    sint32 ecx;
    while (total_no_pixels > 0) {
        sint8 no_pixels = *source_pointer;
        if (no_pixels >= 0) {
            source_pointer++;
            total_no_pixels -= no_pixels;
            memcpy((char *) new_source_pointer, (char *) source_pointer, no_pixels);
            new_source_pointer += no_pixels;
            source_pointer += no_pixels;
            continue;
        }
        ecx = no_pixels;
        no_pixels &= 0x7;
        ecx >>= 3;//SAR
        uintptr_t eax = ((sint32) no_pixels) << 8;
        ecx = -ecx;//Odd
        eax = (eax & 0xFF00) + *(source_pointer + 1);
        total_no_pixels -= ecx;
        source_pointer += 2;
        ebx1 = (uintptr_t) new_source_pointer - eax;
        eax = (uintptr_t) source_pointer;
        source_pointer = (uint8 *) ebx1;
        ebx1 = eax;
        eax = 0;
        memcpy((char *) new_source_pointer, (char *) source_pointer, ecx);
        new_source_pointer += ecx;
        source_pointer = (uint8 *) ebx1;
    }

    bool output = sub_679236_679662_679B0D_679FF1(ebx, image, new_source_pointer_start + (uintptr_t) offset);
    free(new_source_pointer_start);

    return output;
}

/**
 *
 *  rct2: 0x00679023
 */
static bool sub_679023(rct_drawpixelinfo *dpi, sint32 imageId, sint32 x, sint32 y)
{
    imageId &= 0xBFFFFFFF;
    if (imageId & 0x20000000) {
        gUnkEDF81C = 0x20000000;
        sint32 index = (imageId >> 19) & 0x7F;
        if (imageId & 0x80000000) {
            index &= 0x1F;
        }
        sint32 g1Index = palette_to_g1_offset[index];
        unk_9ABDA4 = g1Elements[g1Index].offset;
    } else {
        gUnkEDF81C = 0;
    }
    return sub_679074(dpi, imageId, x, y);
}

/**
 *
 *  rct2: 0x0068862C
 */
static void sub_68862C(rct_drawpixelinfo * dpi, paint_struct * ps)
{
    while ((ps = ps->next_quadrant_ps) != NULL) {
        paint_struct * old_ps = ps;
        paint_struct * next_ps = ps;
        while (next_ps != NULL) {
            ps = next_ps;
            if (sub_679023(dpi, ps->image_id, ps->x, ps->y))
                store_interaction_info(ps);

            next_ps = ps->var_20;
        }

        for (attached_paint_struct *attached_ps = ps->attached_ps; attached_ps != NULL; attached_ps = attached_ps->next) {
            if (sub_679023(
                dpi,
                attached_ps->image_id,
                (attached_ps->x + ps->x) & 0xFFFF,
                (attached_ps->y + ps->y) & 0xFFFF
            )) {
                store_interaction_info(ps);
            }
        }

        ps = old_ps;
    }
}

/**
 *
 *  rct2: 0x00685ADC
 * screenX: eax
 * screenY: ebx
 * flags: edx
 * x: ax
 * y: cx
 * interactionType: bl
 * mapElement: edx
 * viewport: edi
 */
void get_map_coordinates_from_pos(sint32 screenX, sint32 screenY, sint32 flags, sint16 *x, sint16 *y, sint32 *interactionType, rct_map_element **mapElement, rct_viewport **viewport)
{
    _unk9AC154 = flags & 0xFFFF;
    _interactionSpriteType = 0;
    rct_window* window = window_find_from_point(screenX, screenY);
    if (window != NULL && window->viewport != NULL)
    {
        rct_viewport* myviewport = window->viewport;
        screenX -= (sint32)myviewport->x;
        screenY -= (sint32)myviewport->y;
        if (screenX >= 0 && screenX < (sint32)myviewport->width && screenY >= 0 && screenY < (sint32)myviewport->height)
        {
            screenX <<= myviewport->zoom;
            screenY <<= myviewport->zoom;
            screenX += (sint32)myviewport->view_x;
            screenY += (sint32)myviewport->view_y;
            _viewportDpi1.zoom_level = myviewport->zoom;
            screenX &= (0xFFFF << myviewport->zoom) & 0xFFFF;
            screenY &= (0xFFFF << myviewport->zoom) & 0xFFFF;
            _viewportDpi1.x = screenX;
            _viewportDpi1.y = screenY;
            rct_drawpixelinfo* dpi = &_viewportDpi2;
            dpi->y = _viewportDpi1.y;
            dpi->height = 1;
            dpi->zoom_level = _viewportDpi1.zoom_level;
            dpi->x = _viewportDpi1.x;
            dpi->width = 1;
            paint_init(dpi);
            paint_generate_structs(dpi);
            paint_struct ps = paint_arrange_structs();
            sub_68862C(dpi, &ps);
        }
        if (viewport != NULL) *viewport = myviewport;
    }
    if (interactionType != NULL) *interactionType = _interactionSpriteType;
    if (x != NULL) *x = _interactionMapX;
    if (y != NULL) *y = _interactionMapY;
    if (mapElement != NULL) *mapElement = _interaction_element;
}

/**
 * Left, top, right and bottom represent 2D map coordinates at zoom 0.
 */
void viewport_invalidate(rct_viewport *viewport, sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    // if unknown viewport visibility, use the containing window to discover the status
    if (viewport->visibility == VC_UNKNOWN)
    {
        for (rct_window *w = g_window_list; w < gWindowNextSlot; w++)
        {
            if (w->classification != WC_MAIN_WINDOW && w->viewport != NULL && w->viewport == viewport)
            {
                // note, window_is_visible will update viewport->visibility, so this should have a low hit count
                if (!window_is_visible(w)) {
                    return;
                }
            }
        }
    }

    if (viewport->visibility == VC_COVERED) return;

    sint32 viewportLeft = viewport->view_x;
    sint32 viewportTop = viewport->view_y;
    sint32 viewportRight = viewport->view_x + viewport->view_width;
    sint32 viewportBottom = viewport->view_y + viewport->view_height;
    if (right > viewportLeft && bottom > viewportTop) {
        left = max(left, viewportLeft);
        top = max(top, viewportTop);
        right = min(right, viewportRight);
        bottom = min(bottom, viewportBottom);

        uint8 zoom = 1 << viewport->zoom;
        left -= viewportLeft;
        top -= viewportTop;
        right -= viewportLeft;
        bottom -= viewportTop;
        left /= zoom;
        top /= zoom;
        right /= zoom;
        bottom /= zoom;
        left += viewport->x;
        top += viewport->y;
        right += viewport->x;
        bottom += viewport->y;
        gfx_set_dirty_blocks(left, top, right, bottom);
    }
}

static rct_viewport *viewport_find_from_point(sint32 screenX, sint32 screenY)
{
    rct_window *w = window_find_from_point(screenX, screenY);
    if (w == NULL)
        return NULL;

    rct_viewport *viewport = w->viewport;
    if (viewport == NULL)
        return NULL;

    if (screenX < viewport->x || screenY < viewport->y)
        return NULL;
    if (screenX >= viewport->x + viewport->width || screenY >= viewport->y + viewport->height)
        return NULL;

    return viewport;
}

/**
 *
 *  rct2: 0x00688972
 * In:
 *      screen_x: eax
 *      screen_y: ebx
 * Out:
 *      x: ax
 *      y: bx
 *      map_element: edx ?
 *      viewport: edi
 */
void screen_get_map_xy(sint32 screenX, sint32 screenY, sint16 *x, sint16 *y, rct_viewport **viewport) {
    sint16 my_x, my_y;
    sint32 interactionType;
    rct_viewport *myViewport;
    get_map_coordinates_from_pos(screenX, screenY, VIEWPORT_INTERACTION_MASK_TERRAIN, &my_x, &my_y, &interactionType, NULL, &myViewport);
    if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE) {
        *x = 0x8000;
        return;
    }

    rct_xy16 start_vp_pos = screen_coord_to_viewport_coord(myViewport, screenX, screenY);
    rct_xy16 map_pos = { my_x + 16, my_y + 16 };

    for (sint32 i = 0; i < 5; i++) {
        sint32 z = map_element_height(map_pos.x, map_pos.y);
        map_pos = viewport_coord_to_map_coord(start_vp_pos.x, start_vp_pos.y, z);
        map_pos.x = clamp(my_x, map_pos.x, my_x + 31);
        map_pos.y = clamp(my_y, map_pos.y, my_y + 31);
    }

    *x = map_pos.x;
    *y = map_pos.y;

    if (viewport != NULL) *viewport = myViewport;
}

/**
 *
 *  rct2: 0x006894D4
 */
void screen_get_map_xy_with_z(sint16 screenX, sint16 screenY, sint16 z, sint16 *mapX, sint16 *mapY)
{
    rct_viewport *viewport = viewport_find_from_point(screenX, screenY);
    if (viewport == NULL) {
        *mapX = 0x8000;
        return;
    }

    screenX = viewport->view_x + ((screenX - viewport->x) << viewport->zoom);
    screenY = viewport->view_y + ((screenY - viewport->y) << viewport->zoom);

    rct_xy16 mapPosition = viewport_coord_to_map_coord(screenX, screenY + z, 0);
    if (mapPosition.x < 0 || mapPosition.x >= (256 * 32) || mapPosition.y < 0 || mapPosition.y >(256 * 32)) {
        *mapX = 0x8000;
        return;
    }

    *mapX = mapPosition.x;
    *mapY = mapPosition.y;
}

/**
 *
 *  rct2: 0x00689604
 */
void screen_get_map_xy_quadrant(sint16 screenX, sint16 screenY, sint16 *mapX, sint16 *mapY, uint8 *quadrant)
{
    screen_get_map_xy(screenX, screenY, mapX, mapY, NULL);
    if (*mapX == MAP_LOCATION_NULL)
        return;

    *quadrant = map_get_tile_quadrant(*mapX, *mapY);
    *mapX = floor2(*mapX, 32);
    *mapY = floor2(*mapY, 32);
}

/**
 *
 *  rct2: 0x0068964B
 */
void screen_get_map_xy_quadrant_with_z(sint16 screenX, sint16 screenY, sint16 z, sint16 *mapX, sint16 *mapY, uint8 *quadrant)
{
    screen_get_map_xy_with_z(screenX, screenY, z, mapX, mapY);
    if (*mapX == MAP_LOCATION_NULL)
        return;

    *quadrant = map_get_tile_quadrant(*mapX, *mapY);
    *mapX = floor2(*mapX, 32);
    *mapY = floor2(*mapY, 32);
}

/**
 *
 *  rct2: 0x00689692
 */
void screen_get_map_xy_side(sint16 screenX, sint16 screenY, sint16 *mapX, sint16 *mapY, uint8 *side)
{
    screen_get_map_xy(screenX, screenY, mapX, mapY, NULL);
    if (*mapX == MAP_LOCATION_NULL)
        return;

    *side = map_get_tile_side(*mapX, *mapY);
    *mapX = floor2(*mapX, 32);
    *mapY = floor2(*mapY, 32);
}

/**
 *
 *  rct2: 0x006896DC
 */
void screen_get_map_xy_side_with_z(sint16 screenX, sint16 screenY, sint16 z, sint16 *mapX, sint16 *mapY, uint8 *side)
{
    screen_get_map_xy_with_z(screenX, screenY, z, mapX, mapY);
    if (*mapX == MAP_LOCATION_NULL)
        return;

    *side = map_get_tile_side(*mapX, *mapY);
    *mapX = floor2(*mapX, 32);
    *mapY = floor2(*mapY, 32);
}

/**
 * Get current viewport rotation.
 *
 * If an invalid rotation is detected and DEBUG_LEVEL_1 is enabled, an error
 * will be reported.
 *
 * @returns rotation in range 0-3 (inclusive)
 */
uint8 get_current_rotation()
{
    uint8 rotation = gCurrentRotation;
    uint8 rotation_masked = rotation & 3;
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    if (rotation != rotation_masked) {
        log_error("Found wrong rotation %d! Will return %d instead.", (uint32)rotation, (uint32)rotation_masked);
    }
#endif // DEBUG_LEVEL_1
    return rotation_masked;
}

sint16 get_height_marker_offset()
{
    // Height labels in units
    if (gConfigGeneral.show_height_as_units)
        return 0;

    // Height labels in feet
    if (gConfigGeneral.measurement_format == MEASUREMENT_FORMAT_IMPERIAL)
        return 1 * 256;

    // Height labels in metres
    return 2 * 256;
}

void viewport_set_saved_view()
{
    sint16 viewX = 0;
    sint16 viewY = 0;
    uint8 viewZoom = 0;
    uint8 viewRotation = 0;

    rct_window * w = window_get_main();
    if (w != NULL) {
        rct_viewport *viewport = w->viewport;

        viewX = viewport->view_width / 2 + viewport->view_x;
        viewY = viewport->view_height / 2 + viewport->view_y;
        viewZoom = viewport->zoom;
        viewRotation = get_current_rotation();
    }

    gSavedViewX = viewX;
    gSavedViewY = viewY;
    gSavedViewZoom = viewZoom;
    gSavedViewRotation = viewRotation;
}
