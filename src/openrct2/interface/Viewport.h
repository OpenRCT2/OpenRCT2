/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include "../world/Location.hpp"
#include "Window.h"

#include <optional>
#include <vector>

struct paint_session;
struct paint_struct;
struct rct_drawpixelinfo;
struct Peep;
struct TileElement;
struct rct_window;
union paint_entry;
struct SpriteBase;

enum
{
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
    VIEWPORT_FLAG_TRANSPARENT_BACKGROUND = (1 << 19),
};

enum ViewportInteractionItem : uint8_t
{
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

enum
{
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

struct InteractionInfo
{
    InteractionInfo() = default;
    InteractionInfo(const paint_struct* ps);
    CoordsXY Loc;
    union
    {
        TileElement* Element = nullptr;
        SpriteBase* Entity;
    };
    ViewportInteractionItem SpriteType = VIEWPORT_INTERACTION_ITEM_NONE;
};

#define MAX_VIEWPORT_COUNT WINDOW_LIMIT_MAX

/**
 * A reference counter for whether something is forcing the grid lines to show. When the counter
 * is decremented to 0, the grid lines are hidden.
 */
extern uint8_t gShowGridLinesRefCount;
extern uint8_t gShowLandRightsRefCount;
extern uint8_t gShowConstuctionRightsRefCount;

// rct2: 0x014234BC
extern rct_viewport g_viewport_list[MAX_VIEWPORT_COUNT];
extern rct_viewport* g_music_tracking_viewport;
extern ScreenCoordsXY gSavedView;
extern ZoomLevel gSavedViewZoom;
extern uint8_t gSavedViewRotation;

extern paint_entry* gNextFreePaintStruct;
extern uint8_t gCurrentRotation;

void viewport_init_all();
std::optional<ScreenCoordsXY> centre_2d_coordinates(const CoordsXYZ& loc, rct_viewport* viewport);
void viewport_create(
    rct_window* w, const ScreenCoordsXY& screenCoords, int32_t width, int32_t height, int32_t zoom, CoordsXYZ centrePos,
    char flags, uint16_t sprite);
void viewport_update_position(rct_window* window);
void viewport_update_sprite_follow(rct_window* window);
void viewport_update_smart_sprite_follow(rct_window* window);
viewport_focus viewport_update_smart_guest_follow(rct_window* window, Peep* peep);
void viewport_update_smart_staff_follow(rct_window* window, Peep* peep);
void viewport_update_smart_vehicle_follow(rct_window* window);
void viewport_render(
    rct_drawpixelinfo* dpi, const rct_viewport* viewport, int32_t left, int32_t top, int32_t right, int32_t bottom,
    std::vector<paint_session>* sessions = nullptr);
void viewport_paint(
    const rct_viewport* viewport, rct_drawpixelinfo* dpi, int16_t left, int16_t top, int16_t right, int16_t bottom,
    std::vector<paint_session>* sessions = nullptr);

CoordsXYZ viewport_adjust_for_map_height(const ScreenCoordsXY& startCoords);

ScreenCoordsXY screen_coord_to_viewport_coord(rct_viewport* viewport, const ScreenCoordsXY& screenCoords);
CoordsXY viewport_coord_to_map_coord(const ScreenCoordsXY& coords, int32_t z);
std::optional<CoordsXY> screen_pos_to_map_pos(const ScreenCoordsXY& screenCoords, int32_t* direction);

void show_gridlines();
void hide_gridlines();
void show_land_rights();
void hide_land_rights();
void show_construction_rights();
void hide_construction_rights();
void viewport_set_visibility(uint8_t mode);

InteractionInfo get_map_coordinates_from_pos(const ScreenCoordsXY& screenCoords, int32_t flags);
InteractionInfo get_map_coordinates_from_pos_window(rct_window* window, const ScreenCoordsXY& screenCoords, int32_t flags);

InteractionInfo set_interaction_info_from_paint_session(paint_session* session, uint16_t filter);
InteractionInfo viewport_interaction_get_item_left(const ScreenCoordsXY& screenCoords);
bool viewport_interaction_left_over(const ScreenCoordsXY& screenCoords);
bool viewport_interaction_left_click(const ScreenCoordsXY& screenCoords);
InteractionInfo viewport_interaction_get_item_right(const ScreenCoordsXY& screenCoords);
bool viewport_interaction_right_over(const ScreenCoordsXY& screenCoords);
bool viewport_interaction_right_click(const ScreenCoordsXY& screenCoords);

CoordsXY sub_68A15E(const ScreenCoordsXY& screenCoords);
void sub_68B2B7(paint_session* session, const CoordsXY& mapCoords);

void viewport_invalidate(rct_viewport* viewport, int32_t left, int32_t top, int32_t right, int32_t bottom);

std::optional<CoordsXY> screen_get_map_xy(const ScreenCoordsXY& screenCoords, rct_viewport** viewport);
std::optional<CoordsXY> screen_get_map_xy_with_z(const ScreenCoordsXY& screenCoords, int16_t z);
std::optional<CoordsXY> screen_get_map_xy_quadrant(const ScreenCoordsXY& screenCoords, uint8_t* quadrant);
std::optional<CoordsXY> screen_get_map_xy_quadrant_with_z(const ScreenCoordsXY& screenCoords, int16_t z, uint8_t* quadrant);
std::optional<CoordsXY> screen_get_map_xy_side(const ScreenCoordsXY& screenCoords, uint8_t* side);
std::optional<CoordsXY> screen_get_map_xy_side_with_z(const ScreenCoordsXY& screenCoords, int16_t z, uint8_t* side);

uint8_t get_current_rotation();
int16_t get_height_marker_offset();

void viewport_set_saved_view();

#endif
