/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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

struct PaintSession;
struct RecordedPaintSession;
struct PaintStruct;
struct DrawPixelInfo;
struct TileElement;
struct rct_window;
struct EntityBase;
struct Guest;
struct Staff;
struct PaintEntry;

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
    InteractionInfo(const PaintStruct* ps);
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
extern uint8_t gShowConstructionRightsRefCount;

// rct2: 0x014234BC
extern rct_viewport* g_music_tracking_viewport;
extern ScreenCoordsXY gSavedView;
extern ZoomLevel gSavedViewZoom;
extern uint8_t gSavedViewRotation;

extern PaintEntry* gNextFreePaintStruct;
extern uint8_t gCurrentRotation;

void ViewportInitAll();
std::optional<ScreenCoordsXY> centre_2d_coordinates(const CoordsXYZ& loc, rct_viewport* viewport);
void ViewportCreate(rct_window* w, const ScreenCoordsXY& screenCoords, int32_t width, int32_t height, const Focus& focus);
void ViewportRemove(rct_viewport* viewport);
void ViewportsInvalidate(const ScreenRect& screenRect, ZoomLevel maxZoom = ZoomLevel{ -1 });
void ViewportUpdatePosition(rct_window* window);
void ViewportUpdateFollowSprite(rct_window* window);
void ViewportUpdateSmartFollowEntity(rct_window* window);
void ViewportUpdateSmartFollowGuest(rct_window* window, const Guest* peep);
void ViewportUpdateSmartFollowStaff(rct_window* window, const Staff* peep);
void ViewportUpdateSmartFollowVehicle(rct_window* window);
void ViewportRender(
    DrawPixelInfo* dpi, const rct_viewport* viewport, const ScreenRect& screenRect,
    std::vector<RecordedPaintSession>* sessions = nullptr);
void ViewportPaint(
    const rct_viewport* viewport, DrawPixelInfo* dpi, const ScreenRect& screenRect,
    std::vector<RecordedPaintSession>* sessions = nullptr);

CoordsXYZ ViewportAdjustForMapHeight(const ScreenCoordsXY& startCoords);

CoordsXY ViewportPosToMapPos(const ScreenCoordsXY& coords, int32_t z);
std::optional<CoordsXY> ScreenPosToMapPos(const ScreenCoordsXY& screenCoords, int32_t* direction);

void ShowGridlines();
void HideGridlines();
void ShowLandRights();
void HideLandRights();
void ShowConstructionRights();
void HideConstructionRights();
void ViewportSetVisibility(uint8_t mode);

InteractionInfo GetMapCoordinatesFromPos(const ScreenCoordsXY& screenCoords, int32_t flags);
InteractionInfo GetMapCoordinatesFromPosWindow(rct_window* window, const ScreenCoordsXY& screenCoords, int32_t flags);

InteractionInfo SetInteractionInfoFromPaintSession(PaintSession* session, uint32_t viewFlags, uint16_t filter);
InteractionInfo ViewportInteractionGetItemLeft(const ScreenCoordsXY& screenCoords);
bool ViewportInteractionLeftOver(const ScreenCoordsXY& screenCoords);
bool ViewportInteractionLeftClick(const ScreenCoordsXY& screenCoords);
InteractionInfo ViewportInteractionGetItemRight(const ScreenCoordsXY& screenCoords);
bool ViewportInteractionRightOver(const ScreenCoordsXY& screenCoords);
bool ViewportInteractionRightClick(const ScreenCoordsXY& screenCoords);

CoordsXY ViewportInteractionGetTileStartAtCursor(const ScreenCoordsXY& screenCoords);

void ViewportInvalidate(const rct_viewport* viewport, const ScreenRect& screenRect);

std::optional<CoordsXY> ScreenGetMapXY(const ScreenCoordsXY& screenCoords, rct_viewport** viewport);
std::optional<CoordsXY> ScreenGetMapXYWithZ(const ScreenCoordsXY& screenCoords, int32_t z);
std::optional<CoordsXY> ScreenGetMapXYQuadrant(const ScreenCoordsXY& screenCoords, uint8_t* quadrant);
std::optional<CoordsXY> ScreenGetMapXYQuadrantWithZ(const ScreenCoordsXY& screenCoords, int32_t z, uint8_t* quadrant);
std::optional<CoordsXY> ScreenGetMapXYSide(const ScreenCoordsXY& screenCoords, uint8_t* side);
std::optional<CoordsXY> ScreenGetMapXYSideWithZ(const ScreenCoordsXY& screenCoords, int32_t z, uint8_t* side);

uint8_t GetCurrentRotation();
int32_t GetHeightMarkerOffset();

void ViewportSetSavedView();

VisibilityKind GetPaintStructVisibility(const PaintStruct* ps, uint32_t viewFlags);
