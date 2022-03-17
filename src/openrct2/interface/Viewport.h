/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Location.hpp"
#include "Window.h"

#include <limits>
#include <optional>
#include <vector>

struct paint_session;
struct RecordedPaintSession;
struct paint_struct;
struct rct_drawpixelinfo;
struct TileElement;
struct rct_window;
struct EntityBase;
struct Guest;
struct Staff;
struct paint_entry;

// Flags must currenly retain their values to avoid breaking plugins.
// Values can be changed when plugins move to using named constants.
enum
{
    VIEWPORT_FLAG_GRIDLINES = (1 << 7),
    VIEWPORT_FLAG_UNDERGROUND_INSIDE = (1 << 0),
    VIEWPORT_FLAG_HIDE_BASE = (1 << 12),
    VIEWPORT_FLAG_HIDE_VERTICAL = (1 << 13),

    VIEWPORT_FLAG_SOUND_ON = (1 << 10),
    VIEWPORT_FLAG_LAND_OWNERSHIP = (1 << 8),
    VIEWPORT_FLAG_CONSTRUCTION_RIGHTS = (1 << 9),
    VIEWPORT_FLAG_HIDE_ENTITIES = (1 << 14),
    VIEWPORT_FLAG_CLIP_VIEW = (1 << 17),
    VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES = (1 << 18),
    VIEWPORT_FLAG_TRANSPARENT_BACKGROUND = (1 << 19),

    VIEWPORT_FLAG_LAND_HEIGHTS = (1 << 4),
    VIEWPORT_FLAG_TRACK_HEIGHTS = (1 << 5),
    VIEWPORT_FLAG_PATH_HEIGHTS = (1 << 6),

    VIEWPORT_FLAG_HIDE_RIDES = (1 << 1),
    VIEWPORT_FLAG_HIDE_VEHICLES = (1 << 20),
    VIEWPORT_FLAG_HIDE_VEGETATION = (1 << 21),
    VIEWPORT_FLAG_HIDE_SCENERY = (1 << 2),
    VIEWPORT_FLAG_HIDE_PATHS = (1 << 16),
    VIEWPORT_FLAG_HIDE_SUPPORTS = (1 << 3),
    VIEWPORT_FLAG_HIDE_GUESTS = (1 << 11),
    VIEWPORT_FLAG_HIDE_STAFF = (1 << 23),

    VIEWPORT_FLAG_INVISIBLE_RIDES = (1 << 24),
    VIEWPORT_FLAG_INVISIBLE_VEHICLES = (1 << 25),
    VIEWPORT_FLAG_INVISIBLE_VEGETATION = (1 << 26),
    VIEWPORT_FLAG_INVISIBLE_SCENERY = (1 << 27),
    VIEWPORT_FLAG_INVISIBLE_PATHS = (1 << 28),
    VIEWPORT_FLAG_INVISIBLE_SUPPORTS = (1 << 29),
};

enum class VisibilityKind
{
    Visible,
    Partial,
    Hidden
};

enum class ViewportInteractionItem : uint8_t
{
    None,
    Terrain,
    Entity,
    Ride,
    Water,
    Scenery,
    Footpath,
    FootpathItem,
    ParkEntrance,
    Wall,
    LargeScenery,
    Label,
    Banner
};

constexpr uint16_t ViewportInteractionItemAll = std::numeric_limits<uint16_t>::max();

struct InteractionInfo
{
    InteractionInfo() = default;
    InteractionInfo(const paint_struct* ps);
    CoordsXY Loc;
    TileElement* Element{};
    EntityBase* Entity{};
    ViewportInteractionItem SpriteType = ViewportInteractionItem::None;
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
extern rct_viewport* g_music_tracking_viewport;
extern ScreenCoordsXY gSavedView;
extern ZoomLevel gSavedViewZoom;
extern uint8_t gSavedViewRotation;

extern paint_entry* gNextFreePaintStruct;
extern uint8_t gCurrentRotation;

void viewport_init_all();
std::optional<ScreenCoordsXY> centre_2d_coordinates(const CoordsXYZ& loc, rct_viewport* viewport);
void viewport_create(rct_window* w, const ScreenCoordsXY& screenCoords, int32_t width, int32_t height, const Focus& focus);
void viewport_remove(rct_viewport* viewport);
void viewports_invalidate(const ScreenRect& screenRect, ZoomLevel maxZoom = ZoomLevel{ -1 });
void viewport_update_position(rct_window* window);
void viewport_update_sprite_follow(rct_window* window);
void viewport_update_smart_sprite_follow(rct_window* window);
void viewport_update_smart_guest_follow(rct_window* window, const Guest* peep);
void viewport_update_smart_staff_follow(rct_window* window, const Staff* peep);
void viewport_update_smart_vehicle_follow(rct_window* window);
void viewport_render(
    rct_drawpixelinfo* dpi, const rct_viewport* viewport, const ScreenRect& screenRect,
    std::vector<RecordedPaintSession>* sessions = nullptr);
void viewport_paint(
    const rct_viewport* viewport, rct_drawpixelinfo* dpi, const ScreenRect& screenRect,
    std::vector<RecordedPaintSession>* sessions = nullptr);

CoordsXYZ viewport_adjust_for_map_height(const ScreenCoordsXY& startCoords);

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

InteractionInfo set_interaction_info_from_paint_session(paint_session* session, uint32_t viewFlags, uint16_t filter);
InteractionInfo ViewportInteractionGetItemLeft(const ScreenCoordsXY& screenCoords);
bool ViewportInteractionLeftOver(const ScreenCoordsXY& screenCoords);
bool ViewportInteractionLeftClick(const ScreenCoordsXY& screenCoords);
InteractionInfo ViewportInteractionGetItemRight(const ScreenCoordsXY& screenCoords);
bool ViewportInteractionRightOver(const ScreenCoordsXY& screenCoords);
bool ViewportInteractionRightClick(const ScreenCoordsXY& screenCoords);

CoordsXY ViewportInteractionGetTileStartAtCursor(const ScreenCoordsXY& screenCoords);

void viewport_invalidate(const rct_viewport* viewport, const ScreenRect& screenRect);

std::optional<CoordsXY> screen_get_map_xy(const ScreenCoordsXY& screenCoords, rct_viewport** viewport);
std::optional<CoordsXY> screen_get_map_xy_with_z(const ScreenCoordsXY& screenCoords, int32_t z);
std::optional<CoordsXY> screen_get_map_xy_quadrant(const ScreenCoordsXY& screenCoords, uint8_t* quadrant);
std::optional<CoordsXY> screen_get_map_xy_quadrant_with_z(const ScreenCoordsXY& screenCoords, int32_t z, uint8_t* quadrant);
std::optional<CoordsXY> screen_get_map_xy_side(const ScreenCoordsXY& screenCoords, uint8_t* side);
std::optional<CoordsXY> screen_get_map_xy_side_with_z(const ScreenCoordsXY& screenCoords, int32_t z, uint8_t* side);

uint8_t get_current_rotation();
int32_t get_height_marker_offset();

void viewport_set_saved_view();

VisibilityKind GetPaintStructVisibility(const paint_struct* ps, uint32_t viewFlags);
