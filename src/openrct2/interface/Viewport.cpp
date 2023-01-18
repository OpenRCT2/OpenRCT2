/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Viewport.h"

#include "../Context.h"
#include "../Game.h"
#include "../Input.h"
#include "../OpenRCT2.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../core/JobPool.h"
#include "../drawing/Drawing.h"
#include "../drawing/IDrawingEngine.h"
#include "../entity/EntityList.h"
#include "../entity/Guest.h"
#include "../entity/PatrolArea.h"
#include "../entity/Staff.h"
#include "../paint/Paint.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/TrackDesign.h"
#include "../ride/Vehicle.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../util/Math.hpp"
#include "../world/Climate.h"
#include "../world/Map.h"
#include "../world/SmallScenery.h"
#include "Colour.h"
#include "Window.h"
#include "Window_internal.h"

#include <algorithm>
#include <cstring>
#include <list>
#include <unordered_map>

using namespace OpenRCT2;

enum : uint32_t
{
    IMAGE_TYPE_DEFAULT = 0,
    IMAGE_TYPE_REMAP = (1 << 29),
    IMAGE_TYPE_TRANSPARENT = (1 << 30),
};

uint8_t gShowGridLinesRefCount;
uint8_t gShowLandRightsRefCount;
uint8_t gShowConstructionRightsRefCount;

static std::list<rct_viewport> _viewports;
rct_viewport* g_music_tracking_viewport;

static std::unique_ptr<JobPool> _paintJobs;
static std::vector<PaintSession*> _paintColumns;

ScreenCoordsXY gSavedView;
ZoomLevel gSavedViewZoom;
uint8_t gSavedViewRotation;

PaintEntry* gNextFreePaintStruct;
uint8_t gCurrentRotation;

static uint32_t _currentImageType;
InteractionInfo::InteractionInfo(const PaintStruct* ps)
    : Loc(ps->map_x, ps->map_y)
    , Element(ps->tileElement)
    , Entity(ps->entity)
    , SpriteType(ps->sprite_type)
{
}
static void ViewportPaintWeatherGloom(DrawPixelInfo* dpi);

/**
 * This is not a viewport function. It is used to setup many variables for
 * multiple things.
 *  rct2: 0x006E6EAC
 */
void ViewportInitAll()
{
    if (!gOpenRCT2NoGraphics)
    {
        ColoursInitMaps();
    }

    WindowInitAll();

    // ?
    InputResetFlags();
    InputSetState(InputState::Reset);
    gPressedWidget.window_classification = WindowClass::Null;
    gPickupPeepImage = ImageId();
    ResetTooltipNotShown();
    gMapSelectFlags = 0;
    ClearPatrolAreaToRender();
    TextinputCancel();
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
std::optional<ScreenCoordsXY> centre_2d_coordinates(const CoordsXYZ& loc, rct_viewport* viewport)
{
    // If the start location was invalid
    // propagate the invalid location to the output.
    // This fixes a bug that caused the game to enter an infinite loop.
    if (loc.IsNull())
    {
        return std::nullopt;
    }

    auto screenCoord = Translate3DTo2DWithZ(GetCurrentRotation(), loc);
    screenCoord.x -= viewport->view_width / 2;
    screenCoord.y -= viewport->view_height / 2;
    return { screenCoord };
}

CoordsXYZ Focus::GetPos() const
{
    return std::visit(
        [](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Focus::CoordinateFocus>)
                return arg;
            else if constexpr (std::is_same_v<T, Focus::EntityFocus>)
            {
                auto* centreEntity = GetEntity(arg);
                if (centreEntity != nullptr)
                {
                    return CoordsXYZ{ centreEntity->x, centreEntity->y, centreEntity->z };
                }
                else
                {
                    LOG_ERROR("Invalid entity for focus.");
                    return CoordsXYZ{};
                }
            }
        },
        data);
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
void ViewportCreate(rct_window* w, const ScreenCoordsXY& screenCoords, int32_t width, int32_t height, const Focus& focus)
{
    rct_viewport* viewport = nullptr;
    if (_viewports.size() >= MAX_VIEWPORT_COUNT)
    {
        LOG_ERROR("No more viewport slots left to allocate.");
        return;
    }

    auto itViewport = _viewports.insert(_viewports.end(), rct_viewport{});

    viewport = &*itViewport;
    viewport->pos = screenCoords;
    viewport->width = width;
    viewport->height = height;

    const auto zoom = focus.zoom;
    viewport->view_width = zoom.ApplyTo(width);
    viewport->view_height = zoom.ApplyTo(height);
    viewport->zoom = zoom;
    viewport->flags = 0;

    if (gConfigGeneral.AlwaysShowGridlines)
        viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
    w->viewport = viewport;

    CoordsXYZ centrePos = focus.GetPos();
    w->viewport_target_sprite = std::visit(
        [](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Focus::CoordinateFocus>)
                return EntityId::GetNull();
            else if constexpr (std::is_same_v<T, Focus::EntityFocus>)
                return arg;
        },
        focus.data);

    auto centreLoc = centre_2d_coordinates(centrePos, viewport);
    if (!centreLoc.has_value())
    {
        LOG_ERROR("Invalid location for viewport.");
        return;
    }
    w->savedViewPos = *centreLoc;
    viewport->viewPos = *centreLoc;
}

void ViewportRemove(rct_viewport* viewport)
{
    auto it = std::find_if(_viewports.begin(), _viewports.end(), [viewport](const auto& vp) { return &vp == viewport; });
    if (it == _viewports.end())
    {
        LOG_ERROR("Unable to remove viewport: %p", viewport);
        return;
    }
    _viewports.erase(it);
}

void ViewportsInvalidate(const ScreenRect& screenRect, ZoomLevel maxZoom)
{
    for (auto& vp : _viewports)
    {
        if (maxZoom == ZoomLevel{ -1 } || vp.zoom <= ZoomLevel{ maxZoom })
        {
            ViewportInvalidate(&vp, screenRect);
        }
    }
}

/**
 *
 *  rct2: 0x00689174
 * edx is assumed to be (and always is) the current rotation, so it is not
 * needed as parameter.
 */
CoordsXYZ ViewportAdjustForMapHeight(const ScreenCoordsXY& startCoords)
{
    int32_t height = 0;

    uint32_t rotation = GetCurrentRotation();
    CoordsXY pos{};
    for (int32_t i = 0; i < 6; i++)
    {
        pos = ViewportPosToMapPos(startCoords, height);
        height = TileElementHeight(pos);

        // HACK: This is to prevent the x and y values being set to values outside
        // of the map. This can happen when the height is larger than the map size.
        auto mapSizeMinus2 = GetMapSizeMinus2();
        if (pos.x > mapSizeMinus2.x && pos.y > mapSizeMinus2.y)
        {
            static constexpr CoordsXY corr[] = {
                { -1, -1 },
                { 1, -1 },
                { 1, 1 },
                { -1, 1 },
            };
            pos.x += corr[rotation].x * height;
            pos.y += corr[rotation].y * height;
        }
    }

    return { pos, height };
}

/*
 *  rct2: 0x006E7FF3
 */
static void ViewportRedrawAfterShift(
    DrawPixelInfo* dpi, rct_window* window, rct_viewport* viewport, const ScreenCoordsXY& coords)
{
    // sub-divide by intersecting windows
    if (window != nullptr)
    {
        // skip current window and non-intersecting windows
        if (viewport == window->viewport || viewport->pos.x + viewport->width <= window->windowPos.x
            || viewport->pos.x >= window->windowPos.x + window->width
            || viewport->pos.y + viewport->height <= window->windowPos.y
            || viewport->pos.y >= window->windowPos.y + window->height)
        {
            auto itWindowPos = WindowGetIterator(window);
            auto itNextWindow = itWindowPos != g_window_list.end() ? std::next(itWindowPos) : g_window_list.end();
            ViewportRedrawAfterShift(
                dpi, itNextWindow == g_window_list.end() ? nullptr : itNextWindow->get(), viewport, coords);
            return;
        }

        // save viewport
        rct_viewport view_copy = *viewport;

        if (viewport->pos.x < window->windowPos.x)
        {
            viewport->width = window->windowPos.x - viewport->pos.x;
            viewport->view_width = viewport->zoom.ApplyTo(viewport->width);
            ViewportRedrawAfterShift(dpi, window, viewport, coords);

            viewport->pos.x += viewport->width;
            viewport->viewPos.x += viewport->zoom.ApplyTo(viewport->width);
            viewport->width = view_copy.width - viewport->width;
            viewport->view_width = viewport->zoom.ApplyTo(viewport->width);
            ViewportRedrawAfterShift(dpi, window, viewport, coords);
        }
        else if (viewport->pos.x + viewport->width > window->windowPos.x + window->width)
        {
            viewport->width = window->windowPos.x + window->width - viewport->pos.x;
            viewport->view_width = viewport->zoom.ApplyTo(viewport->width);
            ViewportRedrawAfterShift(dpi, window, viewport, coords);

            viewport->pos.x += viewport->width;
            viewport->viewPos.x += viewport->zoom.ApplyTo(viewport->width);
            viewport->width = view_copy.width - viewport->width;
            viewport->view_width = viewport->zoom.ApplyTo(viewport->width);
            ViewportRedrawAfterShift(dpi, window, viewport, coords);
        }
        else if (viewport->pos.y < window->windowPos.y)
        {
            viewport->height = window->windowPos.y - viewport->pos.y;
            viewport->view_width = viewport->zoom.ApplyTo(viewport->width);
            ViewportRedrawAfterShift(dpi, window, viewport, coords);

            viewport->pos.y += viewport->height;
            viewport->viewPos.y += viewport->zoom.ApplyTo(viewport->height);
            viewport->height = view_copy.height - viewport->height;
            viewport->view_width = viewport->zoom.ApplyTo(viewport->width);
            ViewportRedrawAfterShift(dpi, window, viewport, coords);
        }
        else if (viewport->pos.y + viewport->height > window->windowPos.y + window->height)
        {
            viewport->height = window->windowPos.y + window->height - viewport->pos.y;
            viewport->view_width = viewport->zoom.ApplyTo(viewport->width);
            ViewportRedrawAfterShift(dpi, window, viewport, coords);

            viewport->pos.y += viewport->height;
            viewport->viewPos.y += viewport->zoom.ApplyTo(viewport->height);
            viewport->height = view_copy.height - viewport->height;
            viewport->view_width = viewport->zoom.ApplyTo(viewport->width);
            ViewportRedrawAfterShift(dpi, window, viewport, coords);
        }

        // restore viewport
        *viewport = view_copy;
    }
    else
    {
        auto left = viewport->pos.x;
        auto right = viewport->pos.x + viewport->width;
        auto top = viewport->pos.y;
        auto bottom = viewport->pos.y + viewport->height;

        // if moved more than the viewport size
        if (abs(coords.x) < viewport->width && abs(coords.y) < viewport->height)
        {
            // update whole block ?
            DrawingEngineCopyRect(viewport->pos.x, viewport->pos.y, viewport->width, viewport->height, coords.x, coords.y);

            if (coords.x > 0)
            {
                // draw left
                auto _right = viewport->pos.x + coords.x;
                WindowDrawAll(dpi, left, top, _right, bottom);
                left += coords.x;
            }
            else if (coords.x < 0)
            {
                // draw right
                auto _left = viewport->pos.x + viewport->width + coords.x;
                WindowDrawAll(dpi, _left, top, right, bottom);
                right += coords.x;
            }

            if (coords.y > 0)
            {
                // draw top
                bottom = viewport->pos.y + coords.y;
                WindowDrawAll(dpi, left, top, right, bottom);
            }
            else if (coords.y < 0)
            {
                // draw bottom
                top = viewport->pos.y + viewport->height + coords.y;
                WindowDrawAll(dpi, left, top, right, bottom);
            }
        }
        else
        {
            // redraw whole viewport
            WindowDrawAll(dpi, left, top, right, bottom);
        }
    }
}

static void ViewportShiftPixels(
    DrawPixelInfo* dpi, rct_window* window, rct_viewport* viewport, int32_t x_diff, int32_t y_diff)
{
    auto it = WindowGetIterator(window);
    for (; it != g_window_list.end(); it++)
    {
        auto w = it->get();
        if (!(w->flags & WF_TRANSPARENT))
            continue;
        if (w->viewport == viewport)
            continue;

        if (viewport->pos.x + viewport->width <= w->windowPos.x)
            continue;
        if (w->windowPos.x + w->width <= viewport->pos.x)
            continue;

        if (viewport->pos.y + viewport->height <= w->windowPos.y)
            continue;
        if (w->windowPos.y + w->height <= viewport->pos.y)
            continue;

        auto left = w->windowPos.x;
        auto right = w->windowPos.x + w->width;
        auto top = w->windowPos.y;
        auto bottom = w->windowPos.y + w->height;

        if (left < viewport->pos.x)
            left = viewport->pos.x;
        if (right > viewport->pos.x + viewport->width)
            right = viewport->pos.x + viewport->width;

        if (top < viewport->pos.y)
            top = viewport->pos.y;
        if (bottom > viewport->pos.y + viewport->height)
            bottom = viewport->pos.y + viewport->height;

        if (left >= right)
            continue;
        if (top >= bottom)
            continue;

        WindowDrawAll(dpi, left, top, right, bottom);
    }

    ViewportRedrawAfterShift(dpi, window, viewport, { x_diff, y_diff });
}

static void ViewportMove(const ScreenCoordsXY& coords, rct_window* w, rct_viewport* viewport)
{
    auto zoom = viewport->zoom;

    // Note: do not do the subtraction and then divide!
    // Note: Due to arithmetic shift != /zoom a shift will have to be used
    // hopefully when 0x006E7FF3 is finished this can be converted to /zoom.
    auto x_diff = viewport->zoom.ApplyInversedTo(viewport->viewPos.x) - viewport->zoom.ApplyInversedTo(coords.x);
    auto y_diff = viewport->zoom.ApplyInversedTo(viewport->viewPos.y) - viewport->zoom.ApplyInversedTo(coords.y);

    viewport->viewPos = coords;

    // If no change in viewing area
    if ((!x_diff) && (!y_diff))
        return;

    if (w->flags & WF_7)
    {
        int32_t left = std::max<int32_t>(viewport->pos.x, 0);
        int32_t top = std::max<int32_t>(viewport->pos.y, 0);
        int32_t right = std::min<int32_t>(viewport->pos.x + viewport->width, ContextGetWidth());
        int32_t bottom = std::min<int32_t>(viewport->pos.y + viewport->height, ContextGetHeight());

        if (left >= right)
            return;
        if (top >= bottom)
            return;

        if (DrawingEngineHasDirtyOptimisations())
        {
            DrawPixelInfo* dpi = DrawingEngineGetDpi();
            WindowDrawAll(dpi, left, top, right, bottom);
            return;
        }
    }

    rct_viewport view_copy = *viewport;

    if (viewport->pos.x < 0)
    {
        viewport->width += viewport->pos.x;
        viewport->view_width += zoom.ApplyTo(viewport->pos.x);
        viewport->viewPos.x -= zoom.ApplyTo(viewport->pos.x);
        viewport->pos.x = 0;
    }

    int32_t eax = viewport->pos.x + viewport->width - ContextGetWidth();
    if (eax > 0)
    {
        viewport->width -= eax;
        viewport->view_width -= zoom.ApplyTo(eax);
    }

    if (viewport->width <= 0)
    {
        *viewport = view_copy;
        return;
    }

    if (viewport->pos.y < 0)
    {
        viewport->height += viewport->pos.y;
        viewport->view_height += zoom.ApplyTo(viewport->pos.y);
        viewport->viewPos.y -= zoom.ApplyTo(viewport->pos.y);
        viewport->pos.y = 0;
    }

    eax = viewport->pos.y + viewport->height - ContextGetHeight();
    if (eax > 0)
    {
        viewport->height -= eax;
        viewport->view_height -= zoom.ApplyTo(eax);
    }

    if (viewport->height <= 0)
    {
        *viewport = view_copy;
        return;
    }

    if (DrawingEngineHasDirtyOptimisations())
    {
        DrawPixelInfo* dpi = DrawingEngineGetDpi();
        ViewportShiftPixels(dpi, w, viewport, x_diff, y_diff);
    }

    *viewport = view_copy;
}

// rct2: 0x006E7A15
static void ViewportSetUndergroundFlag(int32_t underground, rct_window* window, rct_viewport* viewport)
{
    if (window->classification != WindowClass::MainWindow
        || (window->classification == WindowClass::MainWindow && !window->viewport_smart_follow_sprite.IsNull()))
    {
        if (!underground)
        {
            int32_t bit = viewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE;
            viewport->flags &= ~VIEWPORT_FLAG_UNDERGROUND_INSIDE;
            if (!bit)
                return;
        }
        else
        {
            int32_t bit = viewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE;
            viewport->flags |= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
            if (bit)
                return;
        }
        window->Invalidate();
    }
}

/**
 *
 *  rct2: 0x006E7A3A
 */
void ViewportUpdatePosition(rct_window* window)
{
    WindowEventResizeCall(window);

    rct_viewport* viewport = window->viewport;
    if (viewport == nullptr)
        return;

    if (!window->viewport_smart_follow_sprite.IsNull())
    {
        ViewportUpdateSmartFollowEntity(window);
    }

    if (!window->viewport_target_sprite.IsNull())
    {
        ViewportUpdateFollowSprite(window);
        return;
    }

    ViewportSetUndergroundFlag(0, window, viewport);

    auto viewportMidPoint = ScreenCoordsXY{ window->savedViewPos.x + viewport->view_width / 2,
                                            window->savedViewPos.y + viewport->view_height / 2 };

    auto mapCoord = ViewportPosToMapPos(viewportMidPoint, 0);

    // Clamp to the map minimum value
    int32_t at_map_edge = 0;
    if (mapCoord.x < MAP_MINIMUM_X_Y)
    {
        mapCoord.x = MAP_MINIMUM_X_Y;
        at_map_edge = 1;
    }
    if (mapCoord.y < MAP_MINIMUM_X_Y)
    {
        mapCoord.y = MAP_MINIMUM_X_Y;
        at_map_edge = 1;
    }

    // Clamp to the map maximum value (scenario specific)
    auto mapSizeMinus2 = GetMapSizeMinus2();
    if (mapCoord.x > mapSizeMinus2.x)
    {
        mapCoord.x = mapSizeMinus2.x;
        at_map_edge = 1;
    }
    if (mapCoord.y > mapSizeMinus2.y)
    {
        mapCoord.y = mapSizeMinus2.y;
        at_map_edge = 1;
    }

    if (at_map_edge)
    {
        auto centreLoc = centre_2d_coordinates({ mapCoord, 0 }, viewport);
        if (centreLoc.has_value())
        {
            window->savedViewPos = centreLoc.value();
        }
    }

    auto windowCoords = window->savedViewPos;
    if (window->flags & WF_SCROLLING_TO_LOCATION)
    {
        // Moves the viewport if focusing in on an item
        uint8_t flags = 0;
        windowCoords.x -= viewport->viewPos.x;
        if (windowCoords.x < 0)
        {
            windowCoords.x = -windowCoords.x;
            flags |= 1;
        }
        windowCoords.y -= viewport->viewPos.y;
        if (windowCoords.y < 0)
        {
            windowCoords.y = -windowCoords.y;
            flags |= 2;
        }
        windowCoords.x = (windowCoords.x + 7) / 8;
        windowCoords.y = (windowCoords.y + 7) / 8;

        // If we are at the final zoom position
        if (!windowCoords.x && !windowCoords.y)
        {
            window->flags &= ~WF_SCROLLING_TO_LOCATION;
        }
        if (flags & 1)
        {
            windowCoords.x = -windowCoords.x;
        }
        if (flags & 2)
        {
            windowCoords.y = -windowCoords.y;
        }
        windowCoords.x += viewport->viewPos.x;
        windowCoords.y += viewport->viewPos.y;
    }

    ViewportMove(windowCoords, window, viewport);
}

void ViewportUpdateFollowSprite(rct_window* window)
{
    if (!window->viewport_target_sprite.IsNull() && window->viewport != nullptr)
    {
        auto* sprite = GetEntity(window->viewport_target_sprite);
        if (sprite == nullptr)
        {
            return;
        }

        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            int32_t height = (TileElementHeight({ sprite->x, sprite->y })) - 16;
            int32_t underground = sprite->z < height;
            ViewportSetUndergroundFlag(underground, window, window->viewport);
        }

        auto centreLoc = centre_2d_coordinates(sprite->GetLocation(), window->viewport);
        if (centreLoc.has_value())
        {
            window->savedViewPos = *centreLoc;
            ViewportMove(*centreLoc, window, window->viewport);
        }
    }
}

void ViewportUpdateSmartFollowEntity(rct_window* window)
{
    auto entity = TryGetEntity(window->viewport_smart_follow_sprite);
    if (entity == nullptr || entity->Type == EntityType::Null)
    {
        window->viewport_smart_follow_sprite = EntityId::GetNull();
        window->viewport_target_sprite = EntityId::GetNull();
        return;
    }

    switch (entity->Type)
    {
        case EntityType::Vehicle:
            ViewportUpdateSmartFollowVehicle(window);
            break;

        case EntityType::Guest:
            ViewportUpdateSmartFollowGuest(window, entity->As<Guest>());
            break;

        case EntityType::Staff:
            ViewportUpdateSmartFollowStaff(window, entity->As<Staff>());
            break;

        default: // All other types don't need any "smart" following; steam particle, duck, money effect, etc.
            window->focus = Focus(window->viewport_smart_follow_sprite);
            window->viewport_target_sprite = window->viewport_smart_follow_sprite;
            break;
    }
}

void ViewportUpdateSmartFollowGuest(rct_window* window, const Guest* peep)
{
    Focus focus = Focus(peep->sprite_index);
    window->viewport_target_sprite = peep->sprite_index;

    if (peep->State == PeepState::Picked)
    {
        window->viewport_smart_follow_sprite = EntityId::GetNull();
        window->viewport_target_sprite = EntityId::GetNull();
        window->focus = std::nullopt; // No focus
        return;
    }

    bool overallFocus = true;
    if (peep->State == PeepState::OnRide || peep->State == PeepState::EnteringRide
        || (peep->State == PeepState::LeavingRide && peep->x == LOCATION_NULL))
    {
        auto ride = GetRide(peep->CurrentRide);
        if (ride != nullptr && (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
        {
            auto train = GetEntity<Vehicle>(ride->vehicles[peep->CurrentTrain]);
            if (train != nullptr)
            {
                const auto car = train->GetCar(peep->CurrentCar);
                if (car != nullptr)
                {
                    focus = Focus(car->sprite_index);
                    overallFocus = false;
                    window->viewport_target_sprite = car->sprite_index;
                }
            }
        }
    }

    if (peep->x == LOCATION_NULL && overallFocus)
    {
        auto ride = GetRide(peep->CurrentRide);
        if (ride != nullptr)
        {
            auto xy = ride->overall_view.ToTileCentre();
            CoordsXYZ coordFocus;
            coordFocus.x = xy.x;
            coordFocus.y = xy.y;
            coordFocus.z = TileElementHeight(xy) + (4 * COORDS_Z_STEP);
            focus = Focus(coordFocus);
            window->viewport_target_sprite = EntityId::GetNull();
        }
    }

    window->focus = focus;
}

void ViewportUpdateSmartFollowStaff(rct_window* window, const Staff* peep)
{
    if (peep->State == PeepState::Picked)
    {
        window->viewport_smart_follow_sprite = EntityId::GetNull();
        window->viewport_target_sprite = EntityId::GetNull();
        window->focus = std::nullopt;
        return;
    }

    window->focus = Focus(window->viewport_smart_follow_sprite);
    window->viewport_target_sprite = window->viewport_smart_follow_sprite;
}

void ViewportUpdateSmartFollowVehicle(rct_window* window)
{
    window->focus = Focus(window->viewport_smart_follow_sprite);
    window->viewport_target_sprite = window->viewport_smart_follow_sprite;
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
void ViewportRender(
    DrawPixelInfo* dpi, const rct_viewport* viewport, const ScreenRect& screenRect,
    std::vector<RecordedPaintSession>* sessions)
{
    auto [topLeft, bottomRight] = screenRect;

    if (bottomRight.x <= viewport->pos.x)
        return;
    if (bottomRight.y <= viewport->pos.y)
        return;
    if (topLeft.x >= viewport->pos.x + viewport->width)
        return;
    if (topLeft.y >= viewport->pos.y + viewport->height)
        return;

#ifdef DEBUG_SHOW_DIRTY_BOX
    const auto dirtyBoxTopLeft = topLeft;
    const auto dirtyBoxTopRight = bottomRight - ScreenCoordsXY{ 1, 1 };
#endif

    topLeft -= viewport->pos;
    topLeft = ScreenCoordsXY{
        viewport->zoom.ApplyTo(std::max(topLeft.x, 0)),
        viewport->zoom.ApplyTo(std::max(topLeft.y, 0)),
    } + viewport->viewPos;

    bottomRight -= viewport->pos;
    bottomRight = ScreenCoordsXY{
        viewport->zoom.ApplyTo(std::min(bottomRight.x, viewport->width)),
        viewport->zoom.ApplyTo(std::min(bottomRight.y, viewport->height)),
    } + viewport->viewPos;

    ViewportPaint(viewport, dpi, { topLeft, bottomRight }, sessions);

#ifdef DEBUG_SHOW_DIRTY_BOX
    // FIXME g_viewport_list doesn't exist anymore
    if (viewport != g_viewport_list)
        GfxFillRectInset(dpi, { dirtyBoxTopLeft, dirtyBoxTopRight }, 0x2, INSET_RECT_F_30);
#endif
}

static void RecordSession(
    const PaintSession& session, std::vector<RecordedPaintSession>* recorded_sessions, size_t record_index)
{
    // Perform a deep copy of the paint session, use relative offsets.
    // This is done to extract the session for benchmark.
    // Place the copied session at provided record_index, so the caller can decide which columns/paint sessions to copy;
    // there is no column information embedded in the session itself.
    auto& recordedSession = recorded_sessions->at(record_index);
    recordedSession.Session = session;
    recordedSession.Entries.resize(session.PaintEntryChain.GetCount());

    // Mind the offset needs to be calculated against the original `session`, not `session_copy`
    std::unordered_map<PaintStruct*, PaintStruct*> entryRemap;

    // Copy all entries
    auto paintIndex = 0;
    auto chain = session.PaintEntryChain.Head;
    while (chain != nullptr)
    {
        for (size_t i = 0; i < chain->Count; i++)
        {
            auto& src = chain->PaintStructs[i];
            auto& dst = recordedSession.Entries[paintIndex++];
            dst = src;
            entryRemap[src.AsBasic()] = reinterpret_cast<PaintStruct*>(i * sizeof(PaintEntry));
        }
        chain = chain->Next;
    }
    entryRemap[nullptr] = reinterpret_cast<PaintStruct*>(-1);

    // Remap all entries
    for (auto& ps : recordedSession.Entries)
    {
        auto& ptr = ps.AsBasic()->next_quadrant_ps;
        auto it = entryRemap.find(ptr);
        if (it == entryRemap.end())
        {
            assert(false);
            ptr = nullptr;
        }
        else
        {
            ptr = it->second;
        }
    }
    for (auto& ptr : recordedSession.Session.Quadrants)
    {
        auto it = entryRemap.find(ptr);
        if (it == entryRemap.end())
        {
            assert(false);
            ptr = nullptr;
        }
        else
        {
            ptr = it->second;
        }
    }
}

static void ViewportFillColumn(PaintSession& session, std::vector<RecordedPaintSession>* recorded_sessions, size_t record_index)
{
    PROFILED_FUNCTION();

    PaintSessionGenerate(session);
    if (recorded_sessions != nullptr)
    {
        RecordSession(session, recorded_sessions, record_index);
    }
    PaintSessionArrange(session);
}

static void ViewportPaintColumn(PaintSession& session)
{
    PROFILED_FUNCTION();

    if (session.ViewFlags
            & (VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_UNDERGROUND_INSIDE
               | VIEWPORT_FLAG_CLIP_VIEW)
        && (~session.ViewFlags & VIEWPORT_FLAG_TRANSPARENT_BACKGROUND))
    {
        uint8_t colour = COLOUR_AQUAMARINE;
        if (session.ViewFlags & VIEWPORT_FLAG_HIDE_ENTITIES)
        {
            colour = COLOUR_BLACK;
        }
        GfxClear(&session.DPI, colour);
    }

    PaintDrawStructs(session);

    if (gConfigGeneral.RenderWeatherGloom && !gTrackDesignSaveMode && !(session.ViewFlags & VIEWPORT_FLAG_HIDE_ENTITIES)
        && !(session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
    {
        ViewportPaintWeatherGloom(&session.DPI);
    }

    if (session.PSStringHead != nullptr)
    {
        PaintDrawMoneyStructs(&session.DPI, session.PSStringHead);
    }
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
void ViewportPaint(
    const rct_viewport* viewport, DrawPixelInfo* dpi, const ScreenRect& screenRect,
    std::vector<RecordedPaintSession>* recorded_sessions)
{
    PROFILED_FUNCTION();

    const uint32_t viewFlags = viewport->flags;
    uint32_t width = screenRect.GetWidth();
    uint32_t height = screenRect.GetHeight();
    const uint32_t bitmask = viewport->zoom >= ZoomLevel{ 0 } ? 0xFFFFFFFF & (viewport->zoom.ApplyTo(0xFFFFFFFF)) : 0xFFFFFFFF;
    ScreenCoordsXY topLeft = screenRect.Point1;

    width &= bitmask;
    height &= bitmask;
    topLeft.x &= bitmask;
    topLeft.y &= bitmask;

    auto x = topLeft.x - static_cast<int32_t>(viewport->viewPos.x & bitmask);
    x = viewport->zoom.ApplyInversedTo(x);
    x += viewport->pos.x;

    auto y = topLeft.y - static_cast<int32_t>(viewport->viewPos.y & bitmask);
    y = viewport->zoom.ApplyInversedTo(y);
    y += viewport->pos.y;

    DrawPixelInfo dpi1;
    dpi1.DrawingEngine = dpi->DrawingEngine;
    dpi1.bits = dpi->bits + (x - dpi->x) + ((y - dpi->y) * (dpi->width + dpi->pitch));
    dpi1.x = topLeft.x;
    dpi1.y = topLeft.y;
    dpi1.width = width;
    dpi1.height = height;
    dpi1.pitch = (dpi->width + dpi->pitch) - viewport->zoom.ApplyInversedTo(width);
    dpi1.zoom_level = viewport->zoom;
    dpi1.remX = std::max(0, dpi->x - x);
    dpi1.remY = std::max(0, dpi->y - y);

    // make sure, the compare operation is done in int32_t to avoid the loop becoming an infinite loop.
    // this as well as the [x += 32] in the loop causes signed integer overflow -> undefined behaviour.
    auto rightBorder = dpi1.x + dpi1.width;
    auto alignedX = floor2(dpi1.x, 32);

    _paintColumns.clear();

    bool useMultithreading = gConfigGeneral.MultiThreading;
    if (useMultithreading && _paintJobs == nullptr)
    {
        _paintJobs = std::make_unique<JobPool>();
    }
    else if (useMultithreading == false && _paintJobs != nullptr)
    {
        _paintJobs.reset();
    }

    bool useParallelDrawing = false;
    if (useMultithreading && (dpi->DrawingEngine->GetFlags() & DEF_PARALLEL_DRAWING))
    {
        useParallelDrawing = true;
    }

    // Create space to record sessions and keep track which index is being drawn
    size_t index = 0;
    if (recorded_sessions != nullptr)
    {
        auto columnSize = rightBorder - alignedX;
        auto columnCount = (columnSize + 31) / 32;
        recorded_sessions->resize(columnCount);
    }

    // Generate and sort columns.
    for (x = alignedX; x < rightBorder; x += 32, index++)
    {
        PaintSession* session = PaintSessionAlloc(&dpi1, viewFlags);
        _paintColumns.push_back(session);

        DrawPixelInfo& dpi2 = session->DPI;
        if (x >= dpi2.x)
        {
            auto leftPitch = x - dpi2.x;
            dpi2.width -= leftPitch;
            dpi2.bits += dpi2.zoom_level.ApplyInversedTo(leftPitch);
            dpi2.pitch += dpi2.zoom_level.ApplyInversedTo(leftPitch);
            dpi2.x = x;
        }

        auto paintRight = dpi2.x + dpi2.width;
        if (paintRight >= x + 32)
        {
            auto rightPitch = paintRight - x - 32;
            paintRight -= rightPitch;
            dpi2.pitch += dpi2.zoom_level.ApplyInversedTo(rightPitch);
        }
        dpi2.width = paintRight - dpi2.x;

        if (useMultithreading)
        {
            _paintJobs->AddTask(
                [session, recorded_sessions, index]() -> void { ViewportFillColumn(*session, recorded_sessions, index); });
        }
        else
        {
            ViewportFillColumn(*session, recorded_sessions, index);
        }
    }

    if (useMultithreading)
    {
        _paintJobs->Join();
    }

    // Paint columns.
    for (auto* session : _paintColumns)
    {
        if (useParallelDrawing)
        {
            _paintJobs->AddTask([session]() -> void { ViewportPaintColumn(*session); });
        }
        else
        {
            ViewportPaintColumn(*session);
        }
    }
    if (useParallelDrawing)
    {
        _paintJobs->Join();
    }

    // Release resources.
    for (auto* session : _paintColumns)
    {
        PaintSessionFree(session);
    }
}

static void ViewportPaintWeatherGloom(DrawPixelInfo* dpi)
{
    auto paletteId = ClimateGetWeatherGloomPaletteId(gClimateCurrent);
    if (paletteId != FilterPaletteID::PaletteNull)
    {
        // Only scale width if zoomed in more than 1:1
        auto zoomLevel = dpi->zoom_level < ZoomLevel{ 0 } ? dpi->zoom_level : ZoomLevel{ 0 };
        auto x = dpi->x;
        auto y = dpi->y;
        auto w = zoomLevel.ApplyInversedTo(dpi->width) - 1;
        auto h = zoomLevel.ApplyInversedTo(dpi->height) - 1;
        GfxFilterRect(dpi, ScreenRect(x, y, x + w, y + h), paletteId);
    }
}

/**
 *
 *  rct2: 0x0068958D
 */
std::optional<CoordsXY> ScreenPosToMapPos(const ScreenCoordsXY& screenCoords, int32_t* direction)
{
    auto mapCoords = ScreenGetMapXY(screenCoords, nullptr);
    if (!mapCoords.has_value())
        return std::nullopt;

    int32_t my_direction;
    int32_t dist_from_centre_x = abs(mapCoords->x % 32);
    int32_t dist_from_centre_y = abs(mapCoords->y % 32);
    if (dist_from_centre_x > 8 && dist_from_centre_x < 24 && dist_from_centre_y > 8 && dist_from_centre_y < 24)
    {
        my_direction = 4;
    }
    else
    {
        auto mod_x = mapCoords->x & 0x1F;
        auto mod_y = mapCoords->y & 0x1F;
        if (mod_x <= 16)
        {
            if (mod_y < 16)
            {
                my_direction = 2;
            }
            else
            {
                my_direction = 3;
            }
        }
        else
        {
            if (mod_y < 16)
            {
                my_direction = 1;
            }
            else
            {
                my_direction = 0;
            }
        }
    }

    if (direction != nullptr)
        *direction = my_direction;
    return { mapCoords->ToTileStart() };
}

[[nodiscard]] ScreenCoordsXY rct_viewport::ScreenToViewportCoord(const ScreenCoordsXY& screenCoords) const
{
    ScreenCoordsXY ret;
    ret.x = (zoom.ApplyTo(screenCoords.x - pos.x)) + viewPos.x;
    ret.y = (zoom.ApplyTo(screenCoords.y - pos.y)) + viewPos.y;
    return ret;
}

void rct_viewport::Invalidate() const
{
    ViewportInvalidate(this, { viewPos, viewPos + ScreenCoordsXY{ view_width, view_height } });
}

CoordsXY ViewportPosToMapPos(const ScreenCoordsXY& coords, int32_t z)
{
    // Reverse of Translate3DTo2DWithZ
    CoordsXY ret = { coords.y - coords.x / 2 + z, coords.y + coords.x / 2 + z };
    auto inverseRotation = DirectionFlipXAxis(GetCurrentRotation());
    return ret.Rotate(inverseRotation);
}

/**
 *
 *  rct2: 0x00664689
 */
void ShowGridlines()
{
    if (gShowGridLinesRefCount == 0)
    {
        rct_window* mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            if (!(mainWindow->viewport->flags & VIEWPORT_FLAG_GRIDLINES))
            {
                mainWindow->viewport->flags |= VIEWPORT_FLAG_GRIDLINES;
                mainWindow->Invalidate();
            }
        }
    }
    gShowGridLinesRefCount++;
}

/**
 *
 *  rct2: 0x006646B4
 */
void HideGridlines()
{
    if (gShowGridLinesRefCount > 0)
        gShowGridLinesRefCount--;

    if (gShowGridLinesRefCount == 0)
    {
        rct_window* mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            if (!gConfigGeneral.AlwaysShowGridlines)
            {
                mainWindow->viewport->flags &= ~VIEWPORT_FLAG_GRIDLINES;
                mainWindow->Invalidate();
            }
        }
    }
}

/**
 *
 *  rct2: 0x00664E8E
 */
void ShowLandRights()
{
    if (gShowLandRightsRefCount == 0)
    {
        rct_window* mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            if (!(mainWindow->viewport->flags & VIEWPORT_FLAG_LAND_OWNERSHIP))
            {
                mainWindow->viewport->flags |= VIEWPORT_FLAG_LAND_OWNERSHIP;
                mainWindow->Invalidate();
            }
        }
    }
    gShowLandRightsRefCount++;
}

/**
 *
 *  rct2: 0x00664EB9
 */
void HideLandRights()
{
    if (gShowLandRightsRefCount > 0)
        gShowLandRightsRefCount--;

    if (gShowLandRightsRefCount == 0)
    {
        rct_window* mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            if (mainWindow->viewport->flags & VIEWPORT_FLAG_LAND_OWNERSHIP)
            {
                mainWindow->viewport->flags &= ~VIEWPORT_FLAG_LAND_OWNERSHIP;
                mainWindow->Invalidate();
            }
        }
    }
}

/**
 *
 *  rct2: 0x00664EDD
 */
void ShowConstructionRights()
{
    if (gShowConstructionRightsRefCount == 0)
    {
        rct_window* mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            if (!(mainWindow->viewport->flags & VIEWPORT_FLAG_CONSTRUCTION_RIGHTS))
            {
                mainWindow->viewport->flags |= VIEWPORT_FLAG_CONSTRUCTION_RIGHTS;
                mainWindow->Invalidate();
            }
        }
    }
    gShowConstructionRightsRefCount++;
}

/**
 *
 *  rct2: 0x00664F08
 */
void HideConstructionRights()
{
    if (gShowConstructionRightsRefCount > 0)
        gShowConstructionRightsRefCount--;

    if (gShowConstructionRightsRefCount == 0)
    {
        rct_window* mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            if (mainWindow->viewport->flags & VIEWPORT_FLAG_CONSTRUCTION_RIGHTS)
            {
                mainWindow->viewport->flags &= ~VIEWPORT_FLAG_CONSTRUCTION_RIGHTS;
                mainWindow->Invalidate();
            }
        }
    }
}

/**
 *
 *  rct2: 0x006CB70A
 */
void ViewportSetVisibility(uint8_t mode)
{
    rct_window* window = WindowGetMain();

    if (window != nullptr)
    {
        rct_viewport* vp = window->viewport;
        uint32_t invalidate = 0;

        switch (mode)
        {
            case 0:
            { // Set all these flags to 0, and invalidate if any were active
                uint32_t mask = VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_RIDES | VIEWPORT_FLAG_HIDE_SCENERY
                    | VIEWPORT_FLAG_HIDE_PATHS | VIEWPORT_FLAG_LAND_HEIGHTS | VIEWPORT_FLAG_TRACK_HEIGHTS
                    | VIEWPORT_FLAG_PATH_HEIGHTS | VIEWPORT_FLAG_HIDE_GUESTS | VIEWPORT_FLAG_HIDE_STAFF
                    | VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_VEHICLES
                    | VIEWPORT_FLAG_HIDE_SUPPORTS | VIEWPORT_FLAG_HIDE_VEGETATION;

                invalidate += vp->flags & mask;
                vp->flags &= ~mask;
                break;
            }
            case 1: // 6CB79D
            case 4: // 6CB7C4
                // Set underground on, invalidate if it was off
                invalidate += !(vp->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE);
                vp->flags |= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
                break;
            case 2: // 6CB7EB
                // Set track heights on, invalidate if off
                invalidate += !(vp->flags & VIEWPORT_FLAG_TRACK_HEIGHTS);
                vp->flags |= VIEWPORT_FLAG_TRACK_HEIGHTS;
                break;
            case 3: // 6CB7B1
            case 5: // 6CB7D8
                // Set underground off, invalidate if it was on
                invalidate += vp->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE;
                vp->flags &= ~(static_cast<uint16_t>(VIEWPORT_FLAG_UNDERGROUND_INSIDE));
                break;
        }
        if (invalidate != 0)
            window->Invalidate();
    }
}

static bool IsCursorIdVegetation(CursorID cursor)
{
    switch (cursor)
    {
        case CursorID::TreeDown:
        case CursorID::FlowerDown:
            return true;
        default:
            return false;
    }
}

static bool IsTileElementVegetation(const TileElement* tileElement)
{
    switch (tileElement->GetType())
    {
        case TileElementType::SmallScenery:
        {
            auto sceneryItem = tileElement->AsSmallScenery();
            auto sceneryEntry = sceneryItem->GetEntry();
            if (sceneryEntry != nullptr
                && (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_IS_TREE) || IsCursorIdVegetation(sceneryEntry->tool_id)))
            {
                return true;
            }
            break;
        }
        case TileElementType::LargeScenery:
        {
            auto sceneryItem = tileElement->AsLargeScenery();
            auto sceneryEntry = sceneryItem->GetEntry();
            if (sceneryEntry != nullptr && IsCursorIdVegetation(sceneryEntry->tool_id))
            {
                return true;
            }
            break;
        }
        case TileElementType::Wall:
        {
            auto sceneryItem = tileElement->AsWall();
            auto sceneryEntry = sceneryItem->GetEntry();
            if (sceneryEntry != nullptr && IsCursorIdVegetation(sceneryEntry->tool_id))
            {
                return true;
            }
            break;
        }
        default:
            break;
    }
    return false;
}

VisibilityKind GetPaintStructVisibility(const PaintStruct* ps, uint32_t viewFlags)
{
    switch (ps->sprite_type)
    {
        case ViewportInteractionItem::Entity:
            if (ps->entity != nullptr)
            {
                switch (ps->entity->Type)
                {
                    case EntityType::Vehicle:
                    {
                        if (viewFlags & VIEWPORT_FLAG_HIDE_VEHICLES)
                        {
                            return (viewFlags & VIEWPORT_FLAG_INVISIBLE_VEHICLES) ? VisibilityKind::Hidden
                                                                                  : VisibilityKind::Partial;
                        }
                        // Rides without track can technically have a 'vehicle':
                        // these should be hidden if 'hide rides' is enabled
                        if (viewFlags & VIEWPORT_FLAG_HIDE_RIDES)
                        {
                            auto vehicle = ps->entity->As<Vehicle>();
                            if (vehicle == nullptr)
                                break;

                            auto ride = vehicle->GetRide();
                            if (ride != nullptr && ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_NO_TRACK))
                            {
                                return (viewFlags & VIEWPORT_FLAG_INVISIBLE_RIDES) ? VisibilityKind::Hidden
                                                                                   : VisibilityKind::Partial;
                            }
                        }
                        break;
                    }
                    case EntityType::Guest:
                        if (viewFlags & VIEWPORT_FLAG_HIDE_GUESTS)
                        {
                            return VisibilityKind::Hidden;
                        }
                        break;
                    case EntityType::Staff:
                        if (viewFlags & VIEWPORT_FLAG_HIDE_STAFF)
                        {
                            return VisibilityKind::Hidden;
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        case ViewportInteractionItem::Ride:
            if (viewFlags & VIEWPORT_FLAG_HIDE_RIDES)
            {
                return (viewFlags & VIEWPORT_FLAG_INVISIBLE_RIDES) ? VisibilityKind::Hidden : VisibilityKind::Partial;
            }
            break;
        case ViewportInteractionItem::Footpath:
        case ViewportInteractionItem::FootpathItem:
        case ViewportInteractionItem::Banner:
            if (viewFlags & VIEWPORT_FLAG_HIDE_PATHS)
            {
                return (viewFlags & VIEWPORT_FLAG_INVISIBLE_PATHS) ? VisibilityKind::Hidden : VisibilityKind::Partial;
            }
            break;
        case ViewportInteractionItem::Scenery:
        case ViewportInteractionItem::LargeScenery:
        case ViewportInteractionItem::Wall:
            if (ps->tileElement != nullptr)
            {
                if (IsTileElementVegetation(ps->tileElement))
                {
                    if (viewFlags & VIEWPORT_FLAG_HIDE_VEGETATION)
                    {
                        return (viewFlags & VIEWPORT_FLAG_INVISIBLE_VEGETATION) ? VisibilityKind::Hidden
                                                                                : VisibilityKind::Partial;
                    }
                }
                else
                {
                    if (viewFlags & VIEWPORT_FLAG_HIDE_SCENERY)
                    {
                        return (viewFlags & VIEWPORT_FLAG_INVISIBLE_SCENERY) ? VisibilityKind::Hidden : VisibilityKind::Partial;
                    }
                }
            }
            if (ps->sprite_type == ViewportInteractionItem::Wall && (viewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE))
            {
                return VisibilityKind::Partial;
            }
            break;
        default:
            break;
    }
    return VisibilityKind::Visible;
}

/**
 * Checks if a PaintStruct sprite type is in the filter mask.
 */
static bool PSSpriteTypeIsInFilter(PaintStruct* ps, uint16_t filter)
{
    if (ps->sprite_type != ViewportInteractionItem::None && ps->sprite_type != ViewportInteractionItem::Label
        && ps->sprite_type <= ViewportInteractionItem::Banner)
    {
        auto mask = EnumToFlag(ps->sprite_type);
        if (filter & mask)
        {
            return true;
        }
    }
    return false;
}

/**
 * rct2: 0x00679236, 0x00679662, 0x00679B0D, 0x00679FF1
 */
static bool IsPixelPresentBMP(uint32_t imageType, const G1Element* g1, const uint8_t* index, const PaletteMap& paletteMap)
{
    PROFILED_FUNCTION();

    // Needs investigation as it has no consideration for pure BMP maps.
    if (!(g1->flags & G1_FLAG_HAS_TRANSPARENCY))
    {
        return false;
    }

    if (imageType & IMAGE_TYPE_REMAP)
    {
        return paletteMap[*index] != 0;
    }

    if (imageType & IMAGE_TYPE_TRANSPARENT)
    {
        return false;
    }

    return (*index != 0);
}

/**
 * rct2: 0x0067933B, 0x00679788, 0x00679C4A, 0x0067A117
 */
static bool IsPixelPresentRLE(const uint8_t* esi, int32_t x_start_point, int32_t y_start_point, int32_t round)
{
    PROFILED_FUNCTION();

    uint32_t start_offset = esi[y_start_point * 2] | (esi[y_start_point * 2 + 1] << 8);
    const uint8_t* ebx = esi + start_offset;

    uint8_t last_data_line = 0;
    while (!last_data_line)
    {
        int32_t no_pixels = *ebx++;
        uint8_t gap_size = *ebx++;

        last_data_line = no_pixels & 0x80;

        no_pixels &= 0x7F;

        ebx += no_pixels;

        if (round > 1)
        {
            if (gap_size % 2)
            {
                gap_size++;
                no_pixels--;
                if (no_pixels == 0)
                {
                    continue;
                }
            }
        }

        if (round == 4)
        {
            if (gap_size % 4)
            {
                gap_size += 2;
                no_pixels -= 2;
                if (no_pixels <= 0)
                {
                    continue;
                }
            }
        }

        int32_t x_start = gap_size - x_start_point;
        if (x_start <= 0)
        {
            no_pixels += x_start;
            if (no_pixels <= 0)
            {
                continue;
            }

            x_start = 0;
        }
        else
        {
            // Do nothing?
        }

        x_start += no_pixels;
        x_start--;
        if (x_start > 0)
        {
            no_pixels -= x_start;
            if (no_pixels <= 0)
            {
                continue;
            }
        }

        if (round > 1)
        {
            // This matches the original implementation, but allows empty lines to cause false positives on zoom 0
            if (ceil2(no_pixels, round) == 0)
                continue;
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
static bool IsSpriteInteractedWithPaletteSet(
    DrawPixelInfo* dpi, ImageId imageId, const ScreenCoordsXY& coords, const PaletteMap& paletteMap)
{
    PROFILED_FUNCTION();

    const G1Element* g1 = GfxGetG1Element(imageId);
    if (g1 == nullptr)
    {
        return false;
    }

    if (dpi->zoom_level > ZoomLevel{ 0 })
    {
        if (g1->flags & G1_FLAG_NO_ZOOM_DRAW)
        {
            return false;
        }

        if (g1->flags & G1_FLAG_HAS_ZOOM_SPRITE)
        {
            // TODO: SAR in dpi done with `>> 1`, in coordinates with `/ 2`
            DrawPixelInfo zoomed_dpi = {
                /* .bits = */ dpi->bits,
                /* .x = */ dpi->x >> 1,
                /* .y = */ dpi->y >> 1,
                /* .height = */ dpi->height,
                /* .width = */ dpi->width,
                /* .pitch = */ dpi->pitch,
                /* .zoom_level = */ dpi->zoom_level - 1,
            };

            auto zoomImageId = imageId.WithIndex(imageId.GetIndex() - g1->zoomed_offset);
            return IsSpriteInteractedWithPaletteSet(&zoomed_dpi, zoomImageId, { coords.x / 2, coords.y / 2 }, paletteMap);
        }
    }

    int32_t round = std::max(1, dpi->zoom_level.ApplyTo(1));

    auto origin = coords;
    if (g1->flags & G1_FLAG_RLE_COMPRESSION)
    {
        origin.y -= (round - 1);
    }

    origin.y += g1->y_offset;
    int32_t yStartPoint = 0;
    int32_t height = g1->height;
    if (dpi->zoom_level != ZoomLevel{ 0 })
    {
        if (height % 2)
        {
            height--;
            yStartPoint++;
        }

        if (dpi->zoom_level == ZoomLevel{ 2 })
        {
            if (height % 4)
            {
                height -= 2;
                yStartPoint += 2;
            }
        }

        if (height == 0)
        {
            return false;
        }
    }

    origin.y = floor2(origin.y, round);
    int32_t yEndPoint = height;
    origin.y -= dpi->y;
    if (origin.y < 0)
    {
        yEndPoint += origin.y;
        if (yEndPoint <= 0)
        {
            return false;
        }

        yStartPoint -= origin.y;
        origin.y = 0;
    }

    origin.y += yEndPoint;
    origin.y--;
    if (origin.y > 0)
    {
        yEndPoint -= origin.y;
        if (yEndPoint <= 0)
        {
            return false;
        }
    }

    int32_t xStartPoint = 0;
    int32_t xEndPoint = g1->width;

    origin.x += g1->x_offset;
    origin.x = floor2(origin.x, round);
    origin.x -= dpi->x;
    if (origin.x < 0)
    {
        xEndPoint += origin.x;
        if (xEndPoint <= 0)
        {
            return false;
        }

        xStartPoint -= origin.x;
        origin.x = 0;
    }

    origin.x += xEndPoint;
    origin.x--;
    if (origin.x > 0)
    {
        xEndPoint -= origin.x;
        if (xEndPoint <= 0)
        {
            return false;
        }
    }

    if (g1->flags & G1_FLAG_RLE_COMPRESSION)
    {
        return IsPixelPresentRLE(g1->offset, xStartPoint, yStartPoint, round);
    }

    uint8_t* offset = g1->offset + (yStartPoint * g1->width) + xStartPoint;
    uint32_t imageType = _currentImageType;

    if (!(g1->flags & G1_FLAG_1))
    {
        return IsPixelPresentBMP(imageType, g1, offset, paletteMap);
    }

    Guard::Assert(false, "Invalid image type encountered.");
    return false;
}

/**
 *
 *  rct2: 0x00679023
 */

static bool IsSpriteInteractedWith(DrawPixelInfo* dpi, ImageId imageId, const ScreenCoordsXY& coords)
{
    PROFILED_FUNCTION();

    auto paletteMap = PaletteMap::GetDefault();
    if (imageId.HasPrimary() || imageId.IsRemap())
    {
        _currentImageType = IMAGE_TYPE_REMAP;
        uint8_t paletteIndex;
        if (imageId.HasSecondary())
        {
            paletteIndex = imageId.GetPrimary();
        }
        else
        {
            paletteIndex = imageId.GetRemap();
        }
        if (auto pm = GetPaletteMapForColour(paletteIndex); pm.has_value())
        {
            paletteMap = pm.value();
        }
    }
    else
    {
        _currentImageType = IMAGE_TYPE_DEFAULT;
    }
    return IsSpriteInteractedWithPaletteSet(dpi, imageId, coords, paletteMap);
}

/**
 *
 *  rct2: 0x0068862C
 */
InteractionInfo SetInteractionInfoFromPaintSession(PaintSession* session, uint32_t viewFlags, uint16_t filter)
{
    PROFILED_FUNCTION();

    PaintStruct* ps = &session->PaintHead;
    DrawPixelInfo* dpi = &session->DPI;
    InteractionInfo info{};

    while ((ps = ps->next_quadrant_ps) != nullptr)
    {
        PaintStruct* old_ps = ps;
        PaintStruct* next_ps = ps;
        while (next_ps != nullptr)
        {
            ps = next_ps;
            if (IsSpriteInteractedWith(dpi, ps->image_id, { ps->x, ps->y }))
            {
                if (PSSpriteTypeIsInFilter(ps, filter) && GetPaintStructVisibility(ps, viewFlags) != VisibilityKind::Hidden)
                {
                    info = { ps };
                }
            }
            next_ps = ps->children;
        }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
        for (AttachedPaintStruct* attached_ps = ps->attached_ps; attached_ps != nullptr; attached_ps = attached_ps->next)
        {
            if (IsSpriteInteractedWith(dpi, attached_ps->image_id, { (attached_ps->x + ps->x), (attached_ps->y + ps->y) }))
            {
                if (PSSpriteTypeIsInFilter(ps, filter) && GetPaintStructVisibility(ps, viewFlags) != VisibilityKind::Hidden)
                {
                    info = { ps };
                }
            }
        }
#pragma GCC diagnostic pop

        ps = old_ps;
    }
    return info;
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
 * tileElement: edx
 * viewport: edi
 */
InteractionInfo GetMapCoordinatesFromPos(const ScreenCoordsXY& screenCoords, int32_t flags)
{
    rct_window* window = WindowFindFromPoint(screenCoords);
    return GetMapCoordinatesFromPosWindow(window, screenCoords, flags);
}

InteractionInfo GetMapCoordinatesFromPosWindow(rct_window* window, const ScreenCoordsXY& screenCoords, int32_t flags)
{
    InteractionInfo info{};
    if (window == nullptr || window->viewport == nullptr)
    {
        return info;
    }

    rct_viewport* myviewport = window->viewport;
    auto viewLoc = screenCoords;
    viewLoc -= myviewport->pos;
    if (viewLoc.x >= 0 && viewLoc.x < static_cast<int32_t>(myviewport->width) && viewLoc.y >= 0
        && viewLoc.y < static_cast<int32_t>(myviewport->height))
    {
        viewLoc.x = myviewport->zoom.ApplyTo(viewLoc.x);
        viewLoc.y = myviewport->zoom.ApplyTo(viewLoc.y);
        viewLoc += myviewport->viewPos;
        if (myviewport->zoom > ZoomLevel{ 0 })
        {
            viewLoc.x &= myviewport->zoom.ApplyTo(0xFFFFFFFF) & 0xFFFFFFFF;
            viewLoc.y &= myviewport->zoom.ApplyTo(0xFFFFFFFF) & 0xFFFFFFFF;
        }
        DrawPixelInfo dpi;
        dpi.x = viewLoc.x;
        dpi.y = viewLoc.y;
        dpi.height = 1;
        dpi.zoom_level = myviewport->zoom;
        dpi.width = 1;

        PaintSession* session = PaintSessionAlloc(&dpi, myviewport->flags);
        PaintSessionGenerate(*session);
        PaintSessionArrange(*session);
        info = SetInteractionInfoFromPaintSession(session, myviewport->flags, flags & 0xFFFF);
        PaintSessionFree(session);
    }
    return info;
}

/**
 * screenRect represents 2D map coordinates at zoom 0.
 */
void ViewportInvalidate(const rct_viewport* viewport, const ScreenRect& screenRect)
{
    PROFILED_FUNCTION();

    // if unknown viewport visibility, use the containing window to discover the status
    if (viewport->visibility == VisibilityCache::Unknown)
    {
        auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
        auto owner = windowManager->GetOwner(viewport);
        if (owner != nullptr && owner->classification != WindowClass::MainWindow)
        {
            // note, window_is_visible will update viewport->visibility, so this should have a low hit count
            if (!WindowIsVisible(*owner))
            {
                return;
            }
        }
    }

    if (viewport->visibility == VisibilityCache::Covered)
        return;

    auto [topLeft, bottomRight] = screenRect;
    const auto [viewportRight, viewportBottom] = viewport->viewPos
        + ScreenCoordsXY{ viewport->view_width, viewport->view_height };

    if (bottomRight.x > viewport->viewPos.x && bottomRight.y > viewport->viewPos.y)
    {
        topLeft = { std::max(topLeft.x, viewport->viewPos.x), std::max(topLeft.y, viewport->viewPos.y) };
        topLeft -= viewport->viewPos;
        topLeft = { viewport->zoom.ApplyInversedTo(topLeft.x), viewport->zoom.ApplyInversedTo(topLeft.y) };
        topLeft += viewport->pos;

        bottomRight = { std::min(bottomRight.x, viewportRight), std::min(bottomRight.y, viewportBottom) };
        bottomRight -= viewport->viewPos;
        bottomRight = { viewport->zoom.ApplyInversedTo(bottomRight.x), viewport->zoom.ApplyInversedTo(bottomRight.y) };
        bottomRight += viewport->pos;

        GfxSetDirtyBlocks({ topLeft, bottomRight });
    }
}

static rct_viewport* ViewportFindFromPoint(const ScreenCoordsXY& screenCoords)
{
    rct_window* w = WindowFindFromPoint(screenCoords);
    if (w == nullptr)
        return nullptr;

    rct_viewport* viewport = w->viewport;
    if (viewport == nullptr)
        return nullptr;

    if (viewport->ContainsScreen(screenCoords))
        return viewport;

    return nullptr;
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
 *      tile_element: edx ?
 *      viewport: edi
 */
std::optional<CoordsXY> ScreenGetMapXY(const ScreenCoordsXY& screenCoords, rct_viewport** viewport)
{
    // This will get the tile location but we will need the more accuracy
    rct_window* window = WindowFindFromPoint(screenCoords);
    if (window == nullptr || window->viewport == nullptr)
    {
        return std::nullopt;
    }
    auto myViewport = window->viewport;
    auto info = GetMapCoordinatesFromPosWindow(window, screenCoords, EnumsToFlags(ViewportInteractionItem::Terrain));
    if (info.SpriteType == ViewportInteractionItem::None)
    {
        return std::nullopt;
    }

    auto start_vp_pos = myViewport->ScreenToViewportCoord(screenCoords);
    CoordsXY cursorMapPos = info.Loc.ToTileCentre();

    // Iterates the cursor location to work out exactly where on the tile it is
    for (int32_t i = 0; i < 5; i++)
    {
        int32_t z = TileElementHeight(cursorMapPos);
        cursorMapPos = ViewportPosToMapPos(start_vp_pos, z);
        cursorMapPos.x = std::clamp(cursorMapPos.x, info.Loc.x, info.Loc.x + 31);
        cursorMapPos.y = std::clamp(cursorMapPos.y, info.Loc.y, info.Loc.y + 31);
    }

    if (viewport != nullptr)
        *viewport = myViewport;

    return cursorMapPos;
}

/**
 *
 *  rct2: 0x006894D4
 */
std::optional<CoordsXY> ScreenGetMapXYWithZ(const ScreenCoordsXY& screenCoords, int32_t z)
{
    rct_viewport* viewport = ViewportFindFromPoint(screenCoords);
    if (viewport == nullptr)
    {
        return std::nullopt;
    }

    auto vpCoords = viewport->ScreenToViewportCoord(screenCoords);
    auto mapPosition = ViewportPosToMapPos(vpCoords, z);
    if (!MapIsLocationValid(mapPosition))
    {
        return std::nullopt;
    }

    return mapPosition;
}

/**
 *
 *  rct2: 0x00689604
 */
std::optional<CoordsXY> ScreenGetMapXYQuadrant(const ScreenCoordsXY& screenCoords, uint8_t* quadrant)
{
    auto mapCoords = ScreenGetMapXY(screenCoords, nullptr);
    if (!mapCoords.has_value())
        return std::nullopt;

    *quadrant = MapGetTileQuadrant(*mapCoords);
    return mapCoords->ToTileStart();
}

/**
 *
 *  rct2: 0x0068964B
 */
std::optional<CoordsXY> ScreenGetMapXYQuadrantWithZ(const ScreenCoordsXY& screenCoords, int32_t z, uint8_t* quadrant)
{
    auto mapCoords = ScreenGetMapXYWithZ(screenCoords, z);
    if (!mapCoords.has_value())
        return std::nullopt;

    *quadrant = MapGetTileQuadrant(*mapCoords);
    return mapCoords->ToTileStart();
}

/**
 *
 *  rct2: 0x00689692
 */
std::optional<CoordsXY> ScreenGetMapXYSide(const ScreenCoordsXY& screenCoords, uint8_t* side)
{
    auto mapCoords = ScreenGetMapXY(screenCoords, nullptr);
    if (!mapCoords.has_value())
        return std::nullopt;

    *side = MapGetTileSide(*mapCoords);
    return mapCoords->ToTileStart();
}

/**
 *
 *  rct2: 0x006896DC
 */
std::optional<CoordsXY> ScreenGetMapXYSideWithZ(const ScreenCoordsXY& screenCoords, int32_t z, uint8_t* side)
{
    auto mapCoords = ScreenGetMapXYWithZ(screenCoords, z);
    if (!mapCoords.has_value())
        return std::nullopt;

    *side = MapGetTileSide(*mapCoords);
    return mapCoords->ToTileStart();
}

/**
 * Get current viewport rotation.
 *
 * If an invalid rotation is detected and DEBUG_LEVEL_1 is enabled, an error
 * will be reported.
 *
 * @returns rotation in range 0-3 (inclusive)
 */
uint8_t GetCurrentRotation()
{
    uint8_t rotation = gCurrentRotation;
    uint8_t rotation_masked = rotation & 3;
#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
    if (rotation != rotation_masked)
    {
        LOG_ERROR(
            "Found wrong rotation %d! Will return %d instead.", static_cast<uint32_t>(rotation),
            static_cast<uint32_t>(rotation_masked));
    }
#endif // DEBUG_LEVEL_1
    return rotation_masked;
}

int32_t GetHeightMarkerOffset()
{
    // Height labels in units
    if (gConfigGeneral.ShowHeightAsUnits)
        return 0;

    // Height labels in feet
    if (gConfigGeneral.MeasurementFormat == MeasurementFormat::Imperial)
        return 1 * 256;

    // Height labels in metres
    return 2 * 256;
}

void ViewportSetSavedView()
{
    rct_window* w = WindowGetMain();
    if (w != nullptr)
    {
        rct_viewport* viewport = w->viewport;

        gSavedView = ScreenCoordsXY{ viewport->view_width / 2, viewport->view_height / 2 } + viewport->viewPos;

        gSavedViewZoom = viewport->zoom;
        gSavedViewRotation = GetCurrentRotation();
    }
}

ZoomLevel ZoomLevel::min()
{
#ifndef DISABLE_OPENGL
    if (drawing_engine_get_type() == DrawingEngine::OpenGL)
    {
        return ZoomLevel{ -2 };
    }
#endif

    return ZoomLevel{ 0 };
}
