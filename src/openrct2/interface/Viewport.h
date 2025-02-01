/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
struct PaintStruct;
struct DrawPixelInfo;
struct TileElement;
struct EntityBase;
struct Guest;
struct Staff;
struct PaintEntry;

namespace OpenRCT2
{
    struct WindowBase;

    struct Viewport
    {
        int32_t width{};
        int32_t height{};
        ScreenCoordsXY pos{};
        ScreenCoordsXY viewPos{};
        uint32_t flags{};
        ZoomLevel zoom{};
        uint8_t rotation{};
        VisibilityCache visibility{};

        [[nodiscard]] constexpr int32_t ViewWidth() const
        {
            return zoom.ApplyTo(width);
        }

        [[nodiscard]] constexpr int32_t ViewHeight() const
        {
            return zoom.ApplyTo(height);
        }

        // Use this function on coordinates that are relative to the viewport zoom i.e. a peeps x, y position after transforming
        // from its x, y, z
        [[nodiscard]] constexpr bool Contains(const ScreenCoordsXY& vpos) const
        {
            return (
                vpos.y >= viewPos.y && vpos.y < viewPos.y + ViewHeight() && vpos.x >= viewPos.x
                && vpos.x < viewPos.x + ViewWidth());
        }

        // Use this function on coordinates that are relative to the screen that is been drawn i.e. the cursor position
        [[nodiscard]] constexpr bool ContainsScreen(const ScreenCoordsXY& sPos) const
        {
            return (sPos.x >= pos.x && sPos.x < pos.x + width && sPos.y >= pos.y && sPos.y < pos.y + height);
        }

        [[nodiscard]] ScreenCoordsXY ScreenToViewportCoord(const ScreenCoordsXY& screenCoord) const;

        void Invalidate() const;
    };

    struct Focus;

    // Flags must currenly retain their values to avoid breaking plugins.
    // Values can be changed when plugins move to using named constants.
    enum : uint32_t
    {
        VIEWPORT_FLAG_NONE = 0u,

        VIEWPORT_FLAG_GRIDLINES = (1u << 7),
        VIEWPORT_FLAG_UNDERGROUND_INSIDE = (1u << 0),
        VIEWPORT_FLAG_HIDE_BASE = (1u << 12),
        VIEWPORT_FLAG_HIDE_VERTICAL = (1u << 13),

        VIEWPORT_FLAG_SOUND_ON = (1u << 10),
        VIEWPORT_FLAG_LAND_OWNERSHIP = (1u << 8),
        VIEWPORT_FLAG_CONSTRUCTION_RIGHTS = (1u << 9),
        VIEWPORT_FLAG_HIDE_ENTITIES = (1u << 14),
        VIEWPORT_FLAG_CLIP_VIEW = (1u << 17),
        VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES = (1u << 18),
        VIEWPORT_FLAG_TRANSPARENT_BACKGROUND = (1u << 19),

        VIEWPORT_FLAG_LAND_HEIGHTS = (1u << 4),
        VIEWPORT_FLAG_TRACK_HEIGHTS = (1u << 5),
        VIEWPORT_FLAG_PATH_HEIGHTS = (1u << 6),

        VIEWPORT_FLAG_HIDE_RIDES = (1u << 1),
        VIEWPORT_FLAG_HIDE_VEHICLES = (1u << 20),
        VIEWPORT_FLAG_HIDE_VEGETATION = (1u << 21),
        VIEWPORT_FLAG_HIDE_SCENERY = (1u << 2),
        VIEWPORT_FLAG_HIDE_PATHS = (1u << 16),
        VIEWPORT_FLAG_HIDE_SUPPORTS = (1u << 3),
        VIEWPORT_FLAG_HIDE_GUESTS = (1u << 11),
        VIEWPORT_FLAG_HIDE_STAFF = (1u << 23),

        VIEWPORT_FLAG_INVISIBLE_RIDES = (1u << 24),
        VIEWPORT_FLAG_INVISIBLE_VEHICLES = (1u << 25),
        VIEWPORT_FLAG_INVISIBLE_VEGETATION = (1u << 26),
        VIEWPORT_FLAG_INVISIBLE_SCENERY = (1u << 27),
        VIEWPORT_FLAG_INVISIBLE_PATHS = (1u << 28),
        VIEWPORT_FLAG_INVISIBLE_SUPPORTS = (1u << 29),

        VIEWPORT_FLAG_INDEPEDENT_ROTATION = (1u << 30),
        VIEWPORT_FLAG_RENDERING_INHIBITED = (1u << 31),
    };
} // namespace OpenRCT2

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
    PathAddition,
    ParkEntrance,
    Wall,
    LargeScenery,
    Label,
    Banner
};

enum class ViewportVisibility : uint8_t
{
    Default = 0,
    UndergroundViewOn = 1,
    TrackHeights = 2,
    UndergroundViewOff = 3,
    UndergroundViewGhostOn = 4,
    UndergroundViewGhostOff = 5,
};

namespace OpenRCT2
{
    constexpr uint16_t kViewportInteractionItemAll = std::numeric_limits<uint16_t>::max();

    struct InteractionInfo
    {
        InteractionInfo() = default;
        InteractionInfo(const PaintStruct* ps);
        CoordsXY Loc;
        TileElement* Element{};
        EntityBase* Entity{};
        ViewportInteractionItem interactionType = ViewportInteractionItem::None;
    };

    constexpr int32_t kMaxViewportCount = kWindowLimitMax;

    /**
     * A reference counter for whether something is forcing the grid lines to show. When the counter
     * is decremented to 0, the grid lines are hidden.
     */
    extern uint8_t gShowGridLinesRefCount;
    extern uint8_t gShowLandRightsRefCount;
    extern uint8_t gShowConstructionRightsRefCount;

    // rct2: 0x014234BC
    extern Viewport* g_music_tracking_viewport;

    void ViewportInitAll();
    std::optional<ScreenCoordsXY> centre_2d_coordinates(const CoordsXYZ& loc, Viewport* viewport);
    void ViewportCreate(WindowBase* w, const ScreenCoordsXY& screenCoords, int32_t width, int32_t height, const Focus& focus);
    void ViewportRemove(Viewport* viewport);

    void ViewportsInvalidate(int32_t x, int32_t y, int32_t z0, int32_t z1, ZoomLevel maxZoom);
    void ViewportsInvalidate(const CoordsXYZ& pos, int32_t width, int32_t minHeight, int32_t maxHeight, ZoomLevel maxZoom);
    void ViewportsInvalidate(const ScreenRect& screenRect, ZoomLevel maxZoom = ZoomLevel{ -1 });
    void ViewportUpdatePosition(WindowBase* window);
    void ViewportUpdateSmartFollowGuest(WindowBase* window, const Guest& peep);
    void ViewportRotateSingle(WindowBase* window, int32_t direction);
    void ViewportRotateAll(int32_t direction);
    void ViewportRender(DrawPixelInfo& dpi, const Viewport* viewport);

    CoordsXYZ ViewportAdjustForMapHeight(const ScreenCoordsXY& startCoords, uint8_t rotation);

    CoordsXY ViewportPosToMapPos(const ScreenCoordsXY& coords, int32_t z, uint8_t rotation);
    std::optional<CoordsXY> ScreenPosToMapPos(const ScreenCoordsXY& screenCoords, int32_t* direction);

    void ShowGridlines();
    void HideGridlines();
    void ShowLandRights();
    void HideLandRights();
    void ShowConstructionRights();
    void HideConstructionRights();
    void ViewportSetVisibility(ViewportVisibility mode);

    InteractionInfo GetMapCoordinatesFromPos(const ScreenCoordsXY& screenCoords, int32_t flags);
    InteractionInfo GetMapCoordinatesFromPosWindow(WindowBase* window, const ScreenCoordsXY& screenCoords, int32_t flags);

    InteractionInfo SetInteractionInfoFromPaintSession(PaintSession* session, uint32_t viewFlags, uint16_t filter);

    std::optional<CoordsXY> ScreenGetMapXY(const ScreenCoordsXY& screenCoords, Viewport** viewport);
    std::optional<CoordsXY> ScreenGetMapXYWithZ(const ScreenCoordsXY& screenCoords, int32_t z);
    std::optional<CoordsXY> ScreenGetMapXYQuadrant(const ScreenCoordsXY& screenCoords, uint8_t* quadrant);
    std::optional<CoordsXY> ScreenGetMapXYQuadrantWithZ(const ScreenCoordsXY& screenCoords, int32_t z, uint8_t* quadrant);
    std::optional<CoordsXY> ScreenGetMapXYSide(const ScreenCoordsXY& screenCoords, uint8_t* side);
    std::optional<CoordsXY> ScreenGetMapXYSideWithZ(const ScreenCoordsXY& screenCoords, int32_t z, uint8_t* side);

    ScreenCoordsXY Translate3DTo2DWithZ(int32_t rotation, const CoordsXYZ& pos);

    uint8_t GetCurrentRotation();
    int32_t GetHeightMarkerOffset();

    void ViewportSetSavedView();

    VisibilityKind GetPaintStructVisibility(const PaintStruct* ps, uint32_t viewFlags);
} // namespace OpenRCT2
