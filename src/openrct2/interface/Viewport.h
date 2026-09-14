/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"
#include "../interface/ScreenCoords.hpp"
#include "../interface/ZoomLevel.h"
#include "../world/Location.hpp"
#include "Window.h"

#include <list>
#include <optional>
#include <sfl/static_vector.hpp>

struct PaintSession;
struct PaintStruct;

namespace OpenRCT2::Drawing
{
    struct RenderTarget;
}

namespace OpenRCT2
{
    struct EntityBase;
    struct Guest;
    struct TileElement;
    struct WindowBase;

    enum class ViewportFlag : uint32_t;
    using ViewportFlags = FlagHolder<uint32_t, ViewportFlag>;

    struct Viewport
    {
        int32_t width{};
        int32_t height{};
        ScreenCoordsXY pos{};
        ScreenCoordsXY viewPos{};
        ViewportFlags flags{};
        ZoomLevel zoom{};
        uint8_t rotation{};
        bool isVisible = false;

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

        [[nodiscard]] bool ContainsTile(TileCoordsXY coords) const noexcept;

        [[nodiscard]] ScreenCoordsXY ScreenToViewportCoord(const ScreenCoordsXY& screenCoord) const;

        void Invalidate() const;

        void Invalidate(int32_t x, int32_t y, int32_t z0, int32_t z1, ZoomLevel maxZoom) const;
    };

    struct Focus;

    // Flags must currenly retain their values to avoid breaking plugins.
    // Values can be changed when plugins move to using named constants.
    enum class ViewportFlag : uint32_t
    {
        NONE = 0u,

        GRIDLINES = (1u << 7),
        UNDERGROUND_INSIDE = (1u << 0),
        HIDE_BASE = (1u << 12),
        HIDE_VERTICAL = (1u << 13),

        SOUND_ON = (1u << 10),
        LAND_OWNERSHIP = (1u << 8),
        CONSTRUCTION_RIGHTS = (1u << 9),
        HIDE_ENTITIES = (1u << 14),
        CLIP_VIEW = (1u << 15),
        CLIP_VIEW_SEE_THROUGH = (1u << 17),
        HIGHLIGHT_PATH_ISSUES = (1u << 18),
        TRANSPARENT_BACKGROUND = (1u << 19),

        LAND_HEIGHTS = (1u << 4),
        TRACK_HEIGHTS = (1u << 5),
        PATH_HEIGHTS = (1u << 6),

        HIDE_RIDES = (1u << 1),
        HIDE_VEHICLES = (1u << 20),
        HIDE_VEGETATION = (1u << 21),
        HIDE_SCENERY = (1u << 2),
        HIDE_PATHS = (1u << 16),
        HIDE_SUPPORTS = (1u << 3),
        HIDE_GUESTS = (1u << 11),
        HIDE_STAFF = (1u << 23),

        INVISIBLE_RIDES = (1u << 24),
        INVISIBLE_VEHICLES = (1u << 25),
        INVISIBLE_VEGETATION = (1u << 26),
        INVISIBLE_SCENERY = (1u << 27),
        INVISIBLE_PATHS = (1u << 28),
        INVISIBLE_SUPPORTS = (1u << 29),

        INDEPENDENT_ROTATION = (1u << 30),
        RENDERING_INHIBITED = (1u << 31),
    };
} // namespace OpenRCT2

enum class VisibilityKind
{
    visible,
    partial,
    hidden
};

enum class ViewportInteractionItem : uint8_t
{
    none,
    terrain,
    entity,
    ride,
    water,
    scenery,
    footpath,
    pathAddition,
    parkEntrance,
    wall,
    largeScenery,
    label,
    banner
};
using ViewportInteractionItems = FlagHolder<uint16_t, ViewportInteractionItem>;

enum class ViewportVisibility : uint8_t
{
    standard = 0,
    undergroundViewOn = 1,
    trackHeights = 2,
    undergroundViewOff = 3,
    undergroundViewGhostOn = 4,
    undergroundViewGhostOff = 5,
};

namespace OpenRCT2
{
    constexpr ViewportInteractionItems kViewportInteractionItemAll{
        ViewportInteractionItem::terrain,      ViewportInteractionItem::entity,       ViewportInteractionItem::ride,
        ViewportInteractionItem::water,        ViewportInteractionItem::scenery,      ViewportInteractionItem::footpath,
        ViewportInteractionItem::pathAddition, ViewportInteractionItem::parkEntrance, ViewportInteractionItem::wall,
        ViewportInteractionItem::largeScenery, ViewportInteractionItem::label,        ViewportInteractionItem::banner
    };

    struct InteractionInfo
    {
        InteractionInfo() = default;
        InteractionInfo(const PaintStruct* ps);
        CoordsXY Loc;
        TileElement* Element{};
        EntityBase* Entity{};
        ViewportInteractionItem interactionType = ViewportInteractionItem::none;
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
    extern Viewport* gMusicTrackingViewport;

    std::optional<ScreenCoordsXY> centre2dCoordinates(const CoordsXYZ& loc, Viewport* viewport);
    void ViewportCreate(WindowBase& w, const ScreenCoordsXY& screenCoords, int32_t width, int32_t height, const Focus& focus);
    void ViewportRemove(Viewport* viewport);

    const std::list<Viewport>& GetAllViewports();

    void ViewportsInvalidate(int32_t x, int32_t y, int32_t z0, int32_t z1, ZoomLevel maxZoom);
    void ViewportsInvalidate(const CoordsXYZ& pos, int32_t width, int32_t minHeight, int32_t maxHeight, ZoomLevel maxZoom);
    void ViewportsInvalidate(const ScreenRect& screenRect, ZoomLevel maxZoom = ZoomLevel{ -1 });
    void ViewportUpdatePosition(WindowBase* window);
    void ViewportUpdateSmartFollowGuest(WindowBase* window, const Guest& peep);
    void ViewportRotateSingle(WindowBase* window, int32_t direction);
    void ViewportRotateAll(int32_t direction);
    void ViewportRender(Drawing::RenderTarget& rt, const Viewport* viewport);

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

    InteractionInfo GetMapCoordinatesFromPos(const ScreenCoordsXY& screenCoords, ViewportInteractionItems flags);
    InteractionInfo GetMapCoordinatesFromPosWindow(
        WindowBase* window, const ScreenCoordsXY& screenCoords, ViewportInteractionItems flags);

    InteractionInfo SetInteractionInfoFromPaintSession(
        PaintSession* session, ViewportFlags viewFlags, ViewportInteractionItems filter);

    std::optional<CoordsXY> ScreenGetMapXY(const ScreenCoordsXY& screenCoords, Viewport** viewport);
    std::optional<CoordsXY> ScreenGetMapXYWithZ(const ScreenCoordsXY& screenCoords, int32_t z);
    std::optional<CoordsXY> ScreenGetMapXYQuadrant(const ScreenCoordsXY& screenCoords, uint8_t* quadrant);
    std::optional<CoordsXY> ScreenGetMapXYQuadrantWithZ(const ScreenCoordsXY& screenCoords, int32_t z, uint8_t* quadrant);
    std::optional<CoordsXY> ScreenGetMapXYSide(const ScreenCoordsXY& screenCoords, uint8_t* side);
    std::optional<CoordsXY> ScreenGetMapXYSideWithZ(const ScreenCoordsXY& screenCoords, int32_t z, uint8_t* side);
    Viewport* ViewportFindFromPoint(const ScreenCoordsXY& screenCoords);

    ScreenCoordsXY Translate3DTo2DWithZ(int32_t rotation, const CoordsXYZ& pos);

    uint8_t GetCurrentRotation();
    int32_t GetHeightMarkerOffset();

    void ViewportSetSavedView();

    VisibilityKind GetPaintStructVisibility(const PaintStruct* ps, ViewportFlags viewFlags);

    using ViewportList = sfl::static_vector<Viewport*, kMaxViewportCount>;

    ViewportList GetVisibleViewports() noexcept;
} // namespace OpenRCT2
