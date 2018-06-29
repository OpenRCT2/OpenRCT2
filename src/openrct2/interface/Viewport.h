/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include "Window.h"
#include "../world/Location.hpp"

struct paint_session;
struct paint_struct;
struct rct_drawpixelinfo;
struct rct_peep;
struct rct_tile_element;
struct rct_vehicle;
struct rct_window;
union paint_entry;
union rct_sprite;

enum {
    VIEWPORT_FLAG_UNDERGROUND_INSIDE = (1 << 0),
    VIEWPORT_FLAG_SEETHROUGH_RIDES = (1 << 1),
    VIEWPORT_FLAG_SEETHROUGH_SCENERY = (1 << 2),
    VIEWPORT_FLAG_INVISIBLE_SUPPORTS = (1 << 3),
    VIEWPORT_FLAG_LAND_HEIGHTS = (1 << 4),
    VIEWPORT_FLAG_TRACK_HEIGHTS = (1 << 5),
    VIEWPORT_FLAG_PATH_HEIGHTS = (1 << 6),
    VIEWPORT_FLAG_GRIDLINES = (1 << 7),
    VIEWPORT_FLAG_LAND_OWNERSHIP = (1 << 8),
    VIEWPORT_FLAG_CONSTRUCTION_RIGHTS = (1 << 9),
    VIEWPORT_FLAG_SOUND_ON = (1 << 10),
    VIEWPORT_FLAG_INVISIBLE_PEEPS = (1 << 11),
    VIEWPORT_FLAG_HIDE_BASE = (1 << 12),
    VIEWPORT_FLAG_HIDE_VERTICAL = (1 << 13),
    VIEWPORT_FLAG_INVISIBLE_SPRITES = (1 << 14),
    VIEWPORT_FLAG_15 = (1 << 15),
    VIEWPORT_FLAG_SEETHROUGH_PATHS = (1 << 16),
    VIEWPORT_FLAG_CLIP_VIEW = (1 << 17),
    VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES = (1 << 18),
};

enum {
    VIEWPORT_INTERACTION_ITEM_NONE,
    VIEWPORT_INTERACTION_ITEM_TERRAIN,
    VIEWPORT_INTERACTION_ITEM_SPRITE,
    VIEWPORT_INTERACTION_ITEM_RIDE,
    VIEWPORT_INTERACTION_ITEM_WATER,
    VIEWPORT_INTERACTION_ITEM_SCENERY,
    VIEWPORT_INTERACTION_ITEM_FOOTPATH,
    VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM,
    VIEWPORT_INTERACTION_ITEM_PARK,
    VIEWPORT_INTERACTION_ITEM_WALL,
    VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY,
    VIEWPORT_INTERACTION_ITEM_LABEL,
    VIEWPORT_INTERACTION_ITEM_BANNER,

};

enum {
    VIEWPORT_INTERACTION_MASK_NONE = 0,
    VIEWPORT_INTERACTION_MASK_TERRAIN = ~(1 << (VIEWPORT_INTERACTION_ITEM_TERRAIN - 1)),
    VIEWPORT_INTERACTION_MASK_SPRITE = ~(1 << (VIEWPORT_INTERACTION_ITEM_SPRITE - 1)),
    VIEWPORT_INTERACTION_MASK_RIDE = ~(1 << (VIEWPORT_INTERACTION_ITEM_RIDE - 1)),
    VIEWPORT_INTERACTION_MASK_WATER = ~(1 << (VIEWPORT_INTERACTION_ITEM_WATER - 1)),
    VIEWPORT_INTERACTION_MASK_SCENERY = ~(1 << (VIEWPORT_INTERACTION_ITEM_SCENERY - 1)),
    VIEWPORT_INTERACTION_MASK_FOOTPATH = ~(1 << (VIEWPORT_INTERACTION_ITEM_FOOTPATH - 1)),
    VIEWPORT_INTERACTION_MASK_FOOTPATH_ITEM = ~(1 << (VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM - 1)),
    VIEWPORT_INTERACTION_MASK_PARK = ~(1 << (VIEWPORT_INTERACTION_ITEM_PARK - 1)),
    VIEWPORT_INTERACTION_MASK_WALL = ~(1 << (VIEWPORT_INTERACTION_ITEM_WALL - 1)),
    VIEWPORT_INTERACTION_MASK_LARGE_SCENERY = ~(1 << (VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY - 1)),
    VIEWPORT_INTERACTION_MASK_BANNER = ~(1 << (VIEWPORT_INTERACTION_ITEM_BANNER - 2)), // Note the -2 for BANNER
};

struct viewport_interaction_info {
    int32_t type;
    int32_t x;
    int32_t y;
    union {
        rct_tile_element *tileElement;
        rct_sprite *sprite;
        rct_peep *peep;
        rct_vehicle *vehicle;
    };
};

#define MAX_VIEWPORT_COUNT WINDOW_LIMIT_MAX
#define MAX_ZOOM_LEVEL 3

/**
 * A reference counter for whether something is forcing the grid lines to show. When the counter
 * is decremented to 0, the grid lines are hidden.
 */
extern uint8_t gShowGridLinesRefCount;
extern uint8_t gShowLandRightsRefCount;
extern uint8_t gShowConstuctionRightsRefCount;

// rct2: 0x014234BC
extern rct_viewport g_viewport_list[MAX_VIEWPORT_COUNT];
extern rct_viewport *g_music_tracking_viewport;
extern int16_t gSavedViewX;
extern int16_t gSavedViewY;
extern uint8_t gSavedViewZoom;
extern uint8_t gSavedViewRotation;

extern paint_entry *gNextFreePaintStruct;
extern uint8_t gCurrentRotation;
extern uint32_t gCurrentViewportFlags;

void viewport_init_all();
void centre_2d_coordinates(int32_t x, int32_t y, int32_t z, int32_t * out_x, int32_t * out_y, rct_viewport * viewport);
void viewport_create(rct_window *w, int32_t x, int32_t y, int32_t width, int32_t height, int32_t zoom, int32_t centre_x, int32_t centre_y, int32_t centre_z, char flags, int16_t sprite);
void viewport_update_position(rct_window *window);
void viewport_update_sprite_follow(rct_window *window);
void viewport_update_smart_sprite_follow(rct_window * window);
void viewport_update_smart_guest_follow(rct_window * window, rct_peep * peep);
void viewport_update_smart_staff_follow(rct_window * window, rct_peep * peep);
void viewport_update_smart_vehicle_follow(rct_window * window);
void viewport_render(rct_drawpixelinfo *dpi, rct_viewport *viewport, int32_t left, int32_t top, int32_t right, int32_t bottom);
void viewport_paint(rct_viewport* viewport, rct_drawpixelinfo* dpi, int16_t left, int16_t top, int16_t right, int16_t bottom);

void viewport_adjust_for_map_height(int16_t* x, int16_t* y, int16_t *z);

LocationXY16 screen_coord_to_viewport_coord(rct_viewport *viewport, uint16_t x, uint16_t y);
LocationXY16 viewport_coord_to_map_coord(int32_t x, int32_t y, int32_t z);
void screen_pos_to_map_pos(int16_t *x, int16_t *y, int32_t *direction);

void show_gridlines();
void hide_gridlines();
void show_land_rights();
void hide_land_rights();
void show_construction_rights();
void hide_construction_rights();
void viewport_set_visibility(uint8_t mode);

void get_map_coordinates_from_pos(int32_t screenX, int32_t screenY, int32_t flags, int16_t *x, int16_t *y, int32_t *interactionType, rct_tile_element **tileElement, rct_viewport **viewport);
void get_map_coordinates_from_pos_window(rct_window * window, int32_t screenX, int32_t screenY, int32_t flags, int16_t * x, int16_t * y,
    int32_t * interactionType, rct_tile_element ** tileElement, rct_viewport ** viewport);

int32_t viewport_interaction_get_item_left(int32_t x, int32_t y, viewport_interaction_info *info);
int32_t viewport_interaction_left_over(int32_t x, int32_t y);
int32_t viewport_interaction_left_click(int32_t x, int32_t y);
int32_t viewport_interaction_get_item_right(int32_t x, int32_t y, viewport_interaction_info *info);
int32_t viewport_interaction_right_over(int32_t x, int32_t y);
int32_t viewport_interaction_right_click(int32_t x, int32_t y);
void sub_68A15E(int32_t screenX, int32_t screenY, int16_t *x, int16_t *y, int32_t *direction, rct_tile_element **tileElement);

void sub_68B2B7(paint_session * session, int32_t x, int32_t y);
void sub_68862C(rct_drawpixelinfo * dpi, paint_struct * ps);

void viewport_invalidate(rct_viewport *viewport, int32_t left, int32_t top, int32_t right, int32_t bottom);

void screen_get_map_xy(int32_t screenX, int32_t screenY, int16_t *x, int16_t *y, rct_viewport **viewport);
void screen_get_map_xy_with_z(int16_t screenX, int16_t screenY, int16_t z, int16_t *mapX, int16_t *mapY);
void screen_get_map_xy_quadrant(int16_t screenX, int16_t screenY, int16_t *mapX, int16_t *mapY, uint8_t *quadrant);
void screen_get_map_xy_quadrant_with_z(int16_t screenX, int16_t screenY, int16_t z, int16_t *mapX, int16_t *mapY, uint8_t *quadrant);
void screen_get_map_xy_side(int16_t screenX, int16_t screenY, int16_t *mapX, int16_t *mapY, uint8_t *side);
void screen_get_map_xy_side_with_z(int16_t screenX, int16_t screenY, int16_t z, int16_t *mapX, int16_t *mapY, uint8_t *side);

uint8_t get_current_rotation();
int16_t get_height_marker_offset();

void viewport_set_saved_view();

#endif
