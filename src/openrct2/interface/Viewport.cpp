/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Viewport.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../Input.h"
#include "../OpenRCT2.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../core/JobPool.h"
#include "../core/Numerics.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/IDrawingEngine.h"
#include "../entity/EntityList.h"
#include "../entity/Guest.h"
#include "../entity/PatrolArea.h"
#include "../entity/Staff.h"
#include "../interface/Cursors.h"
#include "../object/LargeSceneryEntry.h"
#include "../object/SmallSceneryEntry.h"
#include "../object/WallSceneryEntry.h"
#include "../paint/Paint.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/TrackDesign.h"
#include "../ride/Vehicle.h"
#include "../ui/WindowManager.h"
#include "../world/Climate.h"
#include "../world/Map.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/WallElement.h"
#include "Colour.h"
#include "Window.h"
#include "Window_internal.h"

#include <cstring>
#include <list>
#include <unordered_map>

namespace OpenRCT2
{
    using namespace OpenRCT2::Numerics;

    enum : uint8_t
    {
        IMAGE_TYPE_DEFAULT = 0,
        IMAGE_TYPE_REMAP = (1 << 1),
        IMAGE_TYPE_TRANSPARENT = (1 << 2),
    };

    uint8_t gShowGridLinesRefCount;
    uint8_t gShowLandRightsRefCount;
    uint8_t gShowConstructionRightsRefCount;

    static std::list<Viewport> _viewports;
    Viewport* g_music_tracking_viewport;

    static std::unique_ptr<JobPool> _paintJobs;
    static std::vector<PaintSession*> _paintColumns;

    InteractionInfo::InteractionInfo(const PaintStruct* ps)
        : Loc(ps->MapPos)
        , Element(ps->Element)
        , Entity(ps->Entity)
        , interactionType(ps->InteractionItem)
    {
    }

    static void ViewportPaintWeatherGloom(DrawPixelInfo& dpi);
    static void ViewportPaint(const Viewport* viewport, DrawPixelInfo& dpi);
    static void ViewportUpdateFollowSprite(WindowBase* window);
    static void ViewportUpdateSmartFollowEntity(WindowBase* window);
    static void ViewportUpdateSmartFollowStaff(WindowBase* window, const Staff& peep);
    static void ViewportUpdateSmartFollowVehicle(WindowBase* window);
    static void ViewportInvalidate(const Viewport* viewport, const ScreenRect& screenRect);

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
    std::optional<ScreenCoordsXY> centre_2d_coordinates(const CoordsXYZ& loc, Viewport* viewport)
    {
        // If the start location was invalid
        // propagate the invalid location to the output.
        // This fixes a bug that caused the game to enter an infinite loop.
        if (loc.IsNull())
        {
            return std::nullopt;
        }

        auto screenCoord = Translate3DTo2DWithZ(viewport->rotation, loc);
        screenCoord.x -= viewport->ViewWidth() / 2;
        screenCoord.y -= viewport->ViewHeight() / 2;
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
    void ViewportCreate(WindowBase* w, const ScreenCoordsXY& screenCoords, int32_t width, int32_t height, const Focus& focus)
    {
        Viewport* viewport = nullptr;
        if (_viewports.size() >= kMaxViewportCount)
        {
            LOG_ERROR("No more viewport slots left to allocate.");
            return;
        }

        auto itViewport = _viewports.insert(_viewports.end(), Viewport{});

        viewport = &*itViewport;
        viewport->pos = screenCoords;
        viewport->width = width;
        viewport->height = height;

        const auto zoom = focus.zoom;
        viewport->zoom = zoom;
        viewport->flags = 0;
        viewport->rotation = GetCurrentRotation();

        if (Config::Get().general.AlwaysShowGridlines)
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

    void ViewportRemove(Viewport* viewport)
    {
        auto it = std::find_if(_viewports.begin(), _viewports.end(), [viewport](const auto& vp) { return &vp == viewport; });
        if (it == _viewports.end())
        {
            LOG_ERROR("Unable to remove viewport: %p", viewport);
            return;
        }
        _viewports.erase(it);
    }

    static Viewport* ViewportGetMain()
    {
        auto mainWindow = WindowGetMain();
        if (mainWindow == nullptr)
        {
            return nullptr;
        }
        return mainWindow->viewport;
    }

    void ViewportsInvalidate(int32_t x, int32_t y, int32_t z0, int32_t z1, ZoomLevel maxZoom)
    {
        for (auto& vp : _viewports)
        {
            if (maxZoom == ZoomLevel{ -1 } || vp.zoom <= ZoomLevel{ maxZoom })
            {
                int32_t x1, y1, x2, y2;

                x += 16;
                y += 16;
                auto screenCoord = Translate3DTo2DWithZ(vp.rotation, CoordsXYZ{ x, y, 0 });

                x1 = screenCoord.x - 32;
                y1 = screenCoord.y - 32 - z1;
                x2 = screenCoord.x + 32;
                y2 = screenCoord.y + 32 - z0;

                ViewportInvalidate(&vp, ScreenRect{ { x1, y1 }, { x2, y2 } });
            }
        }
    }

    void ViewportsInvalidate(const CoordsXYZ& pos, int32_t width, int32_t minHeight, int32_t maxHeight, ZoomLevel maxZoom)
    {
        for (auto& vp : _viewports)
        {
            if (maxZoom == ZoomLevel{ -1 } || vp.zoom <= ZoomLevel{ maxZoom })
            {
                auto screenCoords = Translate3DTo2DWithZ(vp.rotation, pos);
                auto screenPos = ScreenRect(
                    screenCoords - ScreenCoordsXY{ width, minHeight }, screenCoords + ScreenCoordsXY{ width, maxHeight });

                ViewportInvalidate(&vp, screenPos);
            }
        }
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
    CoordsXYZ ViewportAdjustForMapHeight(const ScreenCoordsXY& startCoords, uint8_t rotation)
    {
        int32_t height = 0;

        CoordsXY pos{};
        for (int32_t i = 0; i < 6; i++)
        {
            pos = ViewportPosToMapPos(startCoords, height, rotation);
            height = TileElementHeight(pos);

            // HACK: This is to prevent the x and y values being set to values outside
            // of the map. This can happen when the height is larger than the map size.
            auto mapSizeMinus2 = GetMapSizeMinus2();
            if (pos.x > mapSizeMinus2.x && pos.y > mapSizeMinus2.y)
            {
                static constexpr CoordsXY kCorr[] = {
                    { -1, -1 },
                    { 1, -1 },
                    { 1, 1 },
                    { -1, 1 },
                };
                pos.x += kCorr[rotation].x * height;
                pos.y += kCorr[rotation].y * height;
            }
        }

        return { pos, height };
    }

    /*
     *  rct2: 0x006E7FF3
     */
    static void ViewportRedrawAfterShift(
        DrawPixelInfo& dpi, WindowBase* window, const WindowBase* originalWindow, const ScreenCoordsXY shift,
        const ScreenRect& drawRect)
    {
        // sub-divide by intersecting windows
        if (window != nullptr)
        {
            // skip current window and non-intersecting windows
            if (window == originalWindow || drawRect.GetRight() <= window->windowPos.x
                || drawRect.GetLeft() >= window->windowPos.x + window->width || drawRect.GetBottom() <= window->windowPos.y
                || drawRect.GetTop() >= window->windowPos.y + window->height)
            {
                auto itWindowPos = WindowGetIterator(window);
                auto itNextWindow = itWindowPos != g_window_list.end() ? std::next(itWindowPos) : g_window_list.end();
                ViewportRedrawAfterShift(
                    dpi, itNextWindow == g_window_list.end() ? nullptr : itNextWindow->get(), originalWindow, shift, drawRect);
                return;
            }

            if (drawRect.GetLeft() < window->windowPos.x)
            {
                ScreenRect leftRect = { drawRect.Point1, { window->windowPos.x, drawRect.GetBottom() } };
                ViewportRedrawAfterShift(dpi, window, originalWindow, shift, leftRect);

                ScreenRect rightRect = { { window->windowPos.x, drawRect.GetTop() }, drawRect.Point2 };
                ViewportRedrawAfterShift(dpi, window, originalWindow, shift, rightRect);
            }
            else if (drawRect.GetRight() > window->windowPos.x + window->width)
            {
                ScreenRect leftRect = { drawRect.Point1, { window->windowPos.x + window->width, drawRect.GetBottom() } };
                ViewportRedrawAfterShift(dpi, window, originalWindow, shift, leftRect);

                ScreenRect rightRect = { { window->windowPos.x + window->width, drawRect.GetTop() }, drawRect.Point2 };
                ViewportRedrawAfterShift(dpi, window, originalWindow, shift, rightRect);
            }
            else if (drawRect.GetTop() < window->windowPos.y)
            {
                ScreenRect topRect = { drawRect.Point1, { drawRect.GetRight(), window->windowPos.y } };
                ViewportRedrawAfterShift(dpi, window, originalWindow, shift, topRect);

                ScreenRect bottomRect = { { drawRect.GetLeft(), window->windowPos.y }, drawRect.Point2 };
                ViewportRedrawAfterShift(dpi, window, originalWindow, shift, bottomRect);
            }
            else if (drawRect.GetBottom() > window->windowPos.y + window->height)
            {
                ScreenRect topRect = { drawRect.Point1, { drawRect.GetRight(), window->windowPos.y + window->height } };
                ViewportRedrawAfterShift(dpi, window, originalWindow, shift, topRect);

                ScreenRect bottomRect = { { drawRect.GetLeft(), window->windowPos.y + window->height }, drawRect.Point2 };
                ViewportRedrawAfterShift(dpi, window, originalWindow, shift, bottomRect);
            }
        }
        else
        {
            auto left = drawRect.GetLeft();
            auto right = drawRect.GetRight();
            auto top = drawRect.GetTop();
            auto bottom = drawRect.GetBottom();

            // if moved more than the draw rectangle size
            if (abs(shift.x) < drawRect.GetWidth() && abs(shift.y) < drawRect.GetHeight())
            {
                // update whole block ?
                DrawingEngineCopyRect(
                    drawRect.GetLeft(), drawRect.GetTop(), drawRect.GetWidth(), drawRect.GetHeight(), shift.x, shift.y);

                if (shift.x > 0)
                {
                    // draw left
                    auto _right = left + shift.x;
                    WindowDrawAll(dpi, left, top, _right, bottom);
                    left += shift.x;
                }
                else if (shift.x < 0)
                {
                    // draw right
                    auto _left = right + shift.x;
                    WindowDrawAll(dpi, _left, top, right, bottom);
                    right += shift.x;
                }

                if (shift.y > 0)
                {
                    // draw top
                    bottom = top + shift.y;
                    WindowDrawAll(dpi, left, top, right, bottom);
                }
                else if (shift.y < 0)
                {
                    // draw bottom
                    top = bottom + shift.y;
                    WindowDrawAll(dpi, left, top, right, bottom);
                }
            }
            else
            {
                // redraw whole draw rectangle
                WindowDrawAll(dpi, left, top, right, bottom);
            }
        }
    }

    static void ViewportShiftPixels(DrawPixelInfo& dpi, WindowBase* window, Viewport* viewport, int32_t x_diff, int32_t y_diff)
    {
        // This loop redraws all parts covered by transparent windows.
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

        ViewportRedrawAfterShift(
            dpi, window, window, { x_diff, y_diff },
            { viewport->pos, { viewport->pos.x + viewport->width, viewport->pos.y + viewport->height } });
    }

    static void ViewportMove(const ScreenCoordsXY& coords, WindowBase* w, Viewport* viewport)
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
                DrawPixelInfo& dpi = DrawingEngineGetDpi();
                WindowDrawAll(dpi, left, top, right, bottom);
                return;
            }
        }

        Viewport view_copy = *viewport;

        if (viewport->pos.x < 0)
        {
            viewport->width += viewport->pos.x;
            viewport->viewPos.x -= zoom.ApplyTo(viewport->pos.x);
            viewport->pos.x = 0;
        }

        int32_t eax = viewport->pos.x + viewport->width - ContextGetWidth();
        if (eax > 0)
        {
            viewport->width -= eax;
        }

        if (viewport->width <= 0)
        {
            *viewport = view_copy;
            return;
        }

        if (viewport->pos.y < 0)
        {
            viewport->height += viewport->pos.y;
            viewport->viewPos.y -= zoom.ApplyTo(viewport->pos.y);
            viewport->pos.y = 0;
        }

        eax = viewport->pos.y + viewport->height - ContextGetHeight();
        if (eax > 0)
        {
            viewport->height -= eax;
        }

        if (viewport->height <= 0)
        {
            *viewport = view_copy;
            return;
        }

        if (DrawingEngineHasDirtyOptimisations())
        {
            DrawPixelInfo& dpi = DrawingEngineGetDpi();
            ViewportShiftPixels(dpi, w, viewport, x_diff, y_diff);
        }

        *viewport = view_copy;
    }

    // rct2: 0x006E7A15
    static void ViewportSetUndergroundFlag(int32_t underground, WindowBase* window, Viewport* viewport)
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
    void ViewportUpdatePosition(WindowBase* window)
    {
        window->OnResize();

        Viewport* viewport = window->viewport;
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

        auto viewportMidPoint = ScreenCoordsXY{ window->savedViewPos.x + viewport->ViewWidth() / 2,
                                                window->savedViewPos.y + viewport->ViewHeight() / 2 };

        auto mapCoord = ViewportPosToMapPos(viewportMidPoint, 0, viewport->rotation);

        // Clamp to the map minimum value
        int32_t at_map_edge = 0;
        if (mapCoord.x < kMapMinimumXY)
        {
            mapCoord.x = kMapMinimumXY;
            at_map_edge = 1;
        }
        if (mapCoord.y < kMapMinimumXY)
        {
            mapCoord.y = kMapMinimumXY;
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

    void ViewportUpdateFollowSprite(WindowBase* window)
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
                int32_t height = (TileElementHeight({ sprite->x, sprite->y }))-16;
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

    void ViewportUpdateSmartFollowEntity(WindowBase* window)
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
            {
                auto* guest = entity->As<Guest>();
                if (guest == nullptr)
                {
                    return;
                }
                ViewportUpdateSmartFollowGuest(window, *guest);
                break;
            }
            case EntityType::Staff:
            {
                auto* staff = entity->As<Staff>();
                if (staff == nullptr)
                {
                    return;
                }
                ViewportUpdateSmartFollowStaff(window, *staff);
                break;
            }
            default: // All other types don't need any "smart" following; steam particle, duck, money effect, etc.
                window->focus = Focus(window->viewport_smart_follow_sprite);
                window->viewport_target_sprite = window->viewport_smart_follow_sprite;
                break;
        }
    }

    void ViewportUpdateSmartFollowGuest(WindowBase* window, const Guest& peep)
    {
        Focus focus = Focus(peep.Id);
        window->viewport_target_sprite = peep.Id;

        if (peep.State == PeepState::Picked)
        {
            window->viewport_smart_follow_sprite = EntityId::GetNull();
            window->viewport_target_sprite = EntityId::GetNull();
            window->focus = std::nullopt; // No focus
            return;
        }

        bool overallFocus = true;
        if (peep.State == PeepState::OnRide || peep.State == PeepState::EnteringRide
            || (peep.State == PeepState::LeavingRide && peep.x == kLocationNull))
        {
            auto ride = GetRide(peep.CurrentRide);
            if (ride != nullptr && (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
            {
                auto train = GetEntity<Vehicle>(ride->vehicles[peep.CurrentTrain]);
                if (train != nullptr)
                {
                    const auto car = train->GetCar(peep.CurrentCar);
                    if (car != nullptr)
                    {
                        focus = Focus(car->Id);
                        overallFocus = false;
                        window->viewport_target_sprite = car->Id;
                    }
                }
            }
        }

        if (peep.x == kLocationNull && overallFocus)
        {
            auto ride = GetRide(peep.CurrentRide);
            if (ride != nullptr)
            {
                auto xy = ride->overall_view.ToTileCentre();
                CoordsXYZ coordFocus;
                coordFocus.x = xy.x;
                coordFocus.y = xy.y;
                coordFocus.z = TileElementHeight(xy) + (4 * kCoordsZStep);
                focus = Focus(coordFocus);
                window->viewport_target_sprite = EntityId::GetNull();
            }
        }

        window->focus = focus;
    }

    void ViewportUpdateSmartFollowStaff(WindowBase* window, const Staff& peep)
    {
        if (peep.State == PeepState::Picked)
        {
            window->viewport_smart_follow_sprite = EntityId::GetNull();
            window->viewport_target_sprite = EntityId::GetNull();
            window->focus = std::nullopt;
            return;
        }

        window->focus = Focus(window->viewport_smart_follow_sprite);
        window->viewport_target_sprite = window->viewport_smart_follow_sprite;
    }

    void ViewportUpdateSmartFollowVehicle(WindowBase* window)
    {
        window->focus = Focus(window->viewport_smart_follow_sprite);
        window->viewport_target_sprite = window->viewport_smart_follow_sprite;
    }

    static void ViewportRotateSingleInternal(WindowBase& w, int32_t direction)
    {
        auto* viewport = w.viewport;
        if (viewport == nullptr)
            return;

        auto windowPos = ScreenCoordsXY{ (viewport->width >> 1), (viewport->height >> 1) } + viewport->pos;

        // has something to do with checking if middle of the viewport is obstructed
        Viewport* other;
        auto mapXYCoords = ScreenGetMapXY(windowPos, &other);
        CoordsXYZ coords{};

        // other != viewport probably triggers on viewports in ride or guest window?
        // mapXYCoords is nullopt if middle of viewport is obstructed by another window?
        if (!mapXYCoords.has_value() || other != viewport)
        {
            auto viewPos = ScreenCoordsXY{ (viewport->ViewWidth() >> 1), (viewport->ViewHeight() >> 1) } + viewport->viewPos;

            coords = ViewportAdjustForMapHeight(viewPos, viewport->rotation);
        }
        else
        {
            coords.x = mapXYCoords->x;
            coords.y = mapXYCoords->y;
            coords.z = TileElementHeight(coords);
        }

        viewport->rotation = (viewport->rotation + direction) & 3;

        auto centreLoc = centre_2d_coordinates(coords, viewport);

        if (centreLoc.has_value())
        {
            w.savedViewPos = centreLoc.value();
            viewport->viewPos = *centreLoc;
        }

        w.Invalidate();
        w.OnViewportRotate();
    }

    void ViewportRotateSingle(WindowBase* window, int32_t direction)
    {
        ViewportRotateSingleInternal(*window, direction);
    }

    void ViewportRotateAll(int32_t direction)
    {
        WindowVisitEach([direction](WindowBase* w) {
            auto* viewport = w->viewport;
            if (viewport == nullptr)
                return;
            if (viewport->flags & VIEWPORT_FLAG_INDEPEDENT_ROTATION)
                return;
            ViewportRotateSingleInternal(*w, direction);
        });
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
    void ViewportRender(DrawPixelInfo& dpi, const Viewport* viewport)
    {
        if (viewport->flags & VIEWPORT_FLAG_RENDERING_INHIBITED)
            return;

        if (dpi.x + dpi.width <= viewport->pos.x)
            return;
        if (dpi.y + dpi.height <= viewport->pos.y)
            return;
        if (dpi.x >= viewport->pos.x + viewport->width)
            return;
        if (dpi.y >= viewport->pos.y + viewport->height)
            return;

        ViewportPaint(viewport, dpi);
    }

    static void ViewportFillColumn(PaintSession& session)
    {
        PROFILED_FUNCTION();

        PaintSessionGenerate(session);
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
            GfxClear(session.DPI, colour);
        }

        PaintDrawStructs(session);

        if (Config::Get().general.RenderWeatherGloom && !gTrackDesignSaveMode
            && !(session.ViewFlags & VIEWPORT_FLAG_HIDE_ENTITIES) && !(session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
        {
            ViewportPaintWeatherGloom(session.DPI);
        }

        if (session.PSStringHead != nullptr)
        {
            PaintDrawMoneyStructs(session.DPI, session.PSStringHead);
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
    static void ViewportPaint(const Viewport* viewport, DrawPixelInfo& dpi)
    {
        PROFILED_FUNCTION();

        const int32_t offsetX = dpi.x - viewport->pos.x;
        const int32_t offsetY = dpi.y - viewport->pos.y;
        const int32_t worldX = viewport->zoom.ApplyInversedTo(viewport->viewPos.x) + std::max(0, offsetX);
        const int32_t worldY = viewport->zoom.ApplyInversedTo(viewport->viewPos.y) + std::max(0, offsetY);
        const int32_t width = std::min(viewport->pos.x + viewport->width, dpi.x + dpi.width) - std::max(viewport->pos.x, dpi.x);
        const int32_t height = std::min(viewport->pos.y + viewport->height, dpi.y + dpi.height)
            - std::max(viewport->pos.y, dpi.y);

        DrawPixelInfo worldDpi;
        worldDpi.DrawingEngine = dpi.DrawingEngine;
        worldDpi.bits = dpi.bits + std::max(0, -offsetX) + std::max(0, -offsetY) * dpi.LineStride();
        worldDpi.x = worldX;
        worldDpi.y = worldY;
        worldDpi.width = width;
        worldDpi.height = height;
        worldDpi.pitch = dpi.LineStride() - worldDpi.width;
        worldDpi.zoom_level = viewport->zoom;

        _paintColumns.clear();

        bool useMultithreading = Config::Get().general.MultiThreading;
        if (useMultithreading && _paintJobs == nullptr)
        {
            _paintJobs = std::make_unique<JobPool>();
        }
        else if (useMultithreading == false && _paintJobs != nullptr)
        {
            _paintJobs.reset();
        }

        bool useParallelDrawing = false;
        if (useMultithreading && (dpi.DrawingEngine->GetFlags() & DEF_PARALLEL_DRAWING))
        {
            useParallelDrawing = true;
        }

        const int32_t columnWidth = worldDpi.zoom_level.ApplyInversedTo(kCoordsXYStep);
        const int32_t rightBorder = worldDpi.x + worldDpi.width;
        const int32_t alignedX = floor2(worldDpi.x, columnWidth);

        // Generate and sort columns.
        for (int32_t x = alignedX; x < rightBorder; x += columnWidth)
        {
            PaintSession* session = PaintSessionAlloc(worldDpi, viewport->flags, viewport->rotation);
            _paintColumns.push_back(session);

            DrawPixelInfo& columnDpi = session->DPI;
            if (x >= columnDpi.x)
            {
                const int32_t leftPitch = x - columnDpi.x;
                columnDpi.width = columnDpi.width - leftPitch;
                columnDpi.bits += leftPitch;
                columnDpi.pitch += leftPitch;
                columnDpi.x = x;
            }

            int32_t paintRight = columnDpi.x + columnDpi.width;
            if (paintRight >= x + columnWidth)
            {
                const int32_t rightPitch = paintRight - x - columnWidth;
                paintRight -= rightPitch;
                columnDpi.pitch += rightPitch;
            }
            columnDpi.width = paintRight - columnDpi.x;

            if (useMultithreading)
            {
                _paintJobs->AddTask([session]() -> void { ViewportFillColumn(*session); });
            }
            else
            {
                ViewportFillColumn(*session);
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

    static void ViewportPaintWeatherGloom(DrawPixelInfo& dpi)
    {
        auto paletteId = ClimateGetWeatherGloomPaletteId(GetGameState().ClimateCurrent);
        if (paletteId != FilterPaletteID::PaletteNull)
        {
            auto x = dpi.x;
            auto y = dpi.y;
            auto w = dpi.width;
            auto h = dpi.height;
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

    [[nodiscard]] ScreenCoordsXY Viewport::ScreenToViewportCoord(const ScreenCoordsXY& screenCoords) const
    {
        ScreenCoordsXY ret;
        ret.x = (zoom.ApplyTo(screenCoords.x - pos.x)) + viewPos.x;
        ret.y = (zoom.ApplyTo(screenCoords.y - pos.y)) + viewPos.y;
        return ret;
    }

    void Viewport::Invalidate() const
    {
        ViewportInvalidate(this, { viewPos, viewPos + ScreenCoordsXY{ ViewWidth(), ViewHeight() } });
    }

    CoordsXY ViewportPosToMapPos(const ScreenCoordsXY& coords, int32_t z, uint8_t rotation)
    {
        // Reverse of Translate3DTo2DWithZ
        CoordsXY ret = { coords.y - coords.x / 2 + z, coords.y + coords.x / 2 + z };
        auto inverseRotation = DirectionFlipXAxis(rotation);
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
            WindowBase* mainWindow = WindowGetMain();
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
            WindowBase* mainWindow = WindowGetMain();
            if (mainWindow != nullptr)
            {
                if (!Config::Get().general.AlwaysShowGridlines)
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
            WindowBase* mainWindow = WindowGetMain();
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
            WindowBase* mainWindow = WindowGetMain();
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
            WindowBase* mainWindow = WindowGetMain();
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
            WindowBase* mainWindow = WindowGetMain();
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
    void ViewportSetVisibility(ViewportVisibility mode)
    {
        WindowBase* window = WindowGetMain();

        if (window != nullptr)
        {
            Viewport* vp = window->viewport;
            uint32_t invalidate = 0;

            switch (mode)
            {
                case ViewportVisibility::Default:
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
                case ViewportVisibility::UndergroundViewOn:      // 6CB79D
                case ViewportVisibility::UndergroundViewGhostOn: // 6CB7C4
                    // Set underground on, invalidate if it was off
                    invalidate += !(vp->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE);
                    vp->flags |= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
                    break;
                case ViewportVisibility::TrackHeights: // 6CB7EB
                    // Set track heights on, invalidate if off
                    invalidate += !(vp->flags & VIEWPORT_FLAG_TRACK_HEIGHTS);
                    vp->flags |= VIEWPORT_FLAG_TRACK_HEIGHTS;
                    break;
                case ViewportVisibility::UndergroundViewOff:      // 6CB7B1
                case ViewportVisibility::UndergroundViewGhostOff: // 6CB7D8
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
        switch (ps->InteractionItem)
        {
            case ViewportInteractionItem::Entity:
                if (ps->Entity != nullptr)
                {
                    switch (ps->Entity->Type)
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
                                auto vehicle = ps->Entity->As<Vehicle>();
                                if (vehicle == nullptr)
                                    break;

                                auto ride = vehicle->GetRide();
                                if (ride != nullptr && !ride->GetRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
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
            case ViewportInteractionItem::PathAddition:
            case ViewportInteractionItem::Banner:
                if (viewFlags & VIEWPORT_FLAG_HIDE_PATHS)
                {
                    return (viewFlags & VIEWPORT_FLAG_INVISIBLE_PATHS) ? VisibilityKind::Hidden : VisibilityKind::Partial;
                }
                break;
            case ViewportInteractionItem::Scenery:
            case ViewportInteractionItem::LargeScenery:
            case ViewportInteractionItem::Wall:
                if (ps->Element != nullptr)
                {
                    if (IsTileElementVegetation(ps->Element))
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
                            return (viewFlags & VIEWPORT_FLAG_INVISIBLE_SCENERY) ? VisibilityKind::Hidden
                                                                                 : VisibilityKind::Partial;
                        }
                    }
                }
                if (ps->InteractionItem == ViewportInteractionItem::Wall && (viewFlags & VIEWPORT_FLAG_UNDERGROUND_INSIDE))
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
    static bool PSInteractionTypeIsInFilter(PaintStruct* ps, uint16_t filter)
    {
        if (ps->InteractionItem != ViewportInteractionItem::None && ps->InteractionItem != ViewportInteractionItem::Label
            && ps->InteractionItem <= ViewportInteractionItem::Banner)
        {
            auto mask = EnumToFlag(ps->InteractionItem);
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
    static bool IsPixelPresentBMP(
        const uint32_t imageType, const G1Element* g1, const int32_t x, const int32_t y, const PaletteMap& paletteMap)
    {
        uint8_t* index = g1->offset + (y * g1->width) + x;

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
    static bool IsPixelPresentRLE(const uint8_t* imgData, const int32_t x, const int32_t y)
    {
        uint16_t lineOffset;
        std::memcpy(&lineOffset, &imgData[y * sizeof(uint16_t)], sizeof(uint16_t));
        const uint8_t* data8 = imgData + lineOffset;

        bool lastDataLine = false;
        while (!lastDataLine)
        {
            int32_t numPixels = *data8++;
            uint8_t pixelRunStart = *data8++;
            lastDataLine = numPixels & 0x80;
            numPixels &= 0x7F;
            data8 += numPixels;

            if (pixelRunStart <= x && x < pixelRunStart + numPixels)
                return true;
        }
        return false;
    }

    /**
     * rct2: 0x00679074
     */
    static bool IsSpriteInteractedWithPaletteSet(
        DrawPixelInfo& dpi, ImageId imageId, const ScreenCoordsXY& coords, const PaletteMap& paletteMap,
        const uint8_t imageType)
    {
        PROFILED_FUNCTION();

        const G1Element* g1 = GfxGetG1Element(imageId);
        if (g1 == nullptr)
        {
            return false;
        }

        ZoomLevel zoomLevel = dpi.zoom_level;
        ScreenCoordsXY interactionPoint{ dpi.WorldX(), dpi.WorldY() };
        ScreenCoordsXY origin = coords;

        if (dpi.zoom_level > ZoomLevel{ 0 })
        {
            if (g1->flags & G1_FLAG_NO_ZOOM_DRAW)
            {
                return false;
            }

            while (g1->flags & G1_FLAG_HAS_ZOOM_SPRITE && zoomLevel > ZoomLevel{ 0 })
            {
                imageId = imageId.WithIndex(imageId.GetIndex() - g1->zoomed_offset);
                g1 = GfxGetG1Element(imageId);
                if (g1 == nullptr || g1->flags & G1_FLAG_NO_ZOOM_DRAW)
                {
                    return false;
                }
                zoomLevel = zoomLevel - 1;
                interactionPoint.x >>= 1;
                interactionPoint.y >>= 1;
                origin.x >>= 1;
                origin.y >>= 1;
            }
        }

        origin.x += g1->x_offset;
        origin.y += g1->y_offset;
        interactionPoint -= origin;

        if (interactionPoint.x < 0 || interactionPoint.y < 0 || interactionPoint.x >= g1->width
            || interactionPoint.y >= g1->height)
        {
            return false;
        }

        if (g1->flags & G1_FLAG_RLE_COMPRESSION)
        {
            return IsPixelPresentRLE(g1->offset, interactionPoint.x, interactionPoint.y);
        }

        if (!(g1->flags & G1_FLAG_1))
        {
            return IsPixelPresentBMP(imageType, g1, interactionPoint.x, interactionPoint.y, paletteMap);
        }

        Guard::Assert(false, "Invalid image type encountered.");
        return false;
    }

    /**
     *
     *  rct2: 0x00679023
     */

    static bool IsSpriteInteractedWith(DrawPixelInfo& dpi, ImageId imageId, const ScreenCoordsXY& coords)
    {
        PROFILED_FUNCTION();

        auto paletteMap = PaletteMap::GetDefault();
        uint8_t imageType;
        if (imageId.HasPrimary() || imageId.IsRemap())
        {
            imageType = IMAGE_TYPE_REMAP;
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
            imageType = IMAGE_TYPE_DEFAULT;
        }
        return IsSpriteInteractedWithPaletteSet(dpi, imageId, coords, paletteMap, imageType);
    }

    /**
     *
     *  rct2: 0x0068862C
     */
    InteractionInfo SetInteractionInfoFromPaintSession(PaintSession* session, uint32_t viewFlags, uint16_t filter)
    {
        PROFILED_FUNCTION();

        InteractionInfo info{};

        PaintStruct* ps = session->PaintHead;
        while (ps != nullptr)
        {
            PaintStruct* old_ps = ps;
            PaintStruct* next_ps = ps;
            while (next_ps != nullptr)
            {
                ps = next_ps;
                if (IsSpriteInteractedWith(session->DPI, ps->image_id, ps->ScreenPos))
                {
                    if (PSInteractionTypeIsInFilter(ps, filter)
                        && GetPaintStructVisibility(ps, viewFlags) == VisibilityKind::Visible)
                    {
                        info = { ps };
                    }
                }
                next_ps = ps->Children;
            }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
            for (AttachedPaintStruct* attached_ps = ps->Attached; attached_ps != nullptr; attached_ps = attached_ps->NextEntry)
            {
                if (IsSpriteInteractedWith(session->DPI, attached_ps->image_id, ps->ScreenPos + attached_ps->RelativePos))
                {
                    if (PSInteractionTypeIsInFilter(ps, filter)
                        && GetPaintStructVisibility(ps, viewFlags) == VisibilityKind::Visible)
                    {
                        info = { ps };
                    }
                }
            }
#pragma GCC diagnostic pop

            ps = old_ps->NextQuadrantEntry;
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
        auto* windowMgr = Ui::GetWindowManager();
        WindowBase* window = windowMgr->FindFromPoint(screenCoords);
        return GetMapCoordinatesFromPosWindow(window, screenCoords, flags);
    }

    InteractionInfo GetMapCoordinatesFromPosWindow(WindowBase* window, const ScreenCoordsXY& screenCoords, int32_t flags)
    {
        InteractionInfo info{};
        if (window == nullptr || window->viewport == nullptr)
        {
            return info;
        }

        Viewport* viewport = window->viewport;
        auto viewLoc = screenCoords;
        viewLoc -= viewport->pos;
        if (viewLoc.x >= 0 && viewLoc.x < static_cast<int32_t>(viewport->width) && viewLoc.y >= 0
            && viewLoc.y < static_cast<int32_t>(viewport->height))
        {
            viewLoc.x = viewport->zoom.ApplyTo(viewLoc.x);
            viewLoc.y = viewport->zoom.ApplyTo(viewLoc.y);
            viewLoc += viewport->viewPos;
            if (viewport->zoom > ZoomLevel{ 0 })
            {
                viewLoc.x &= viewport->zoom.ApplyTo(0xFFFFFFFF) & 0xFFFFFFFF;
                viewLoc.y &= viewport->zoom.ApplyTo(0xFFFFFFFF) & 0xFFFFFFFF;
            }
            DrawPixelInfo dpi;
            dpi.zoom_level = viewport->zoom;
            dpi.x = viewport->zoom.ApplyInversedTo(viewLoc.x);
            dpi.y = viewport->zoom.ApplyInversedTo(viewLoc.y);
            dpi.height = 1;
            dpi.width = 1;

            PaintSession* session = PaintSessionAlloc(dpi, viewport->flags, viewport->rotation);
            PaintSessionGenerate(*session);
            PaintSessionArrange(*session);
            info = SetInteractionInfoFromPaintSession(session, viewport->flags, flags & 0xFFFF);
            PaintSessionFree(session);
        }
        return info;
    }

    /**
     * screenRect represents 2D map coordinates at zoom 0.
     */
    void ViewportInvalidate(const Viewport* viewport, const ScreenRect& screenRect)
    {
        PROFILED_FUNCTION();

        // if unknown viewport visibility, use the containing window to discover the status
        if (viewport->visibility == VisibilityCache::Unknown)
        {
            auto windowManager = Ui::GetWindowManager();
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

        auto zoom = viewport->zoom;
        auto viewPos = viewport->viewPos;
        auto viewportScreenPos = viewport->pos;

        ScreenRect invalidRect = { { zoom.ApplyInversedTo(screenRect.GetLeft() - viewPos.x),
                                     zoom.ApplyInversedTo(screenRect.GetTop() - viewPos.y) },
                                   { zoom.ApplyInversedTo(screenRect.GetRight() - viewPos.x),
                                     zoom.ApplyInversedTo(screenRect.GetBottom() - viewPos.y) } };

        if (invalidRect.GetTop() >= viewport->height || invalidRect.GetBottom() <= 0 || invalidRect.GetLeft() >= viewport->width
            || invalidRect.GetRight() <= 0)
        {
            return;
        }
        invalidRect.Point1 += viewportScreenPos;
        invalidRect.Point2 += viewportScreenPos;
        GfxSetDirtyBlocks(invalidRect);
    }

    static Viewport* ViewportFindFromPoint(const ScreenCoordsXY& screenCoords)
    {
        auto* windowMgr = Ui::GetWindowManager();
        WindowBase* w = windowMgr->FindFromPoint(screenCoords);
        if (w == nullptr)
            return nullptr;

        Viewport* viewport = w->viewport;
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
    std::optional<CoordsXY> ScreenGetMapXY(const ScreenCoordsXY& screenCoords, Viewport** viewport)
    {
        auto* windowMgr = Ui::GetWindowManager();

        // This will get the tile location but we will need the more accuracy
        WindowBase* window = windowMgr->FindFromPoint(screenCoords);
        if (window == nullptr || window->viewport == nullptr)
        {
            return std::nullopt;
        }
        auto myViewport = window->viewport;
        auto info = GetMapCoordinatesFromPosWindow(window, screenCoords, EnumsToFlags(ViewportInteractionItem::Terrain));
        if (info.interactionType == ViewportInteractionItem::None)
        {
            return std::nullopt;
        }

        auto start_vp_pos = myViewport->ScreenToViewportCoord(screenCoords);
        CoordsXY cursorMapPos = info.Loc.ToTileCentre();

        // Iterates the cursor location to work out exactly where on the tile it is
        for (int32_t i = 0; i < 5; i++)
        {
            int32_t z = TileElementHeight(cursorMapPos);
            cursorMapPos = ViewportPosToMapPos(start_vp_pos, z, myViewport->rotation);
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
        Viewport* viewport = ViewportFindFromPoint(screenCoords);
        if (viewport == nullptr)
        {
            return std::nullopt;
        }

        auto vpCoords = viewport->ScreenToViewportCoord(screenCoords);
        auto mapPosition = ViewportPosToMapPos(vpCoords, z, viewport->rotation);
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

    ScreenCoordsXY Translate3DTo2DWithZ(int32_t rotation, const CoordsXYZ& pos)
    {
        auto rotated = pos.Rotate(rotation);
        // Use right shift to avoid issues like #9301
        return ScreenCoordsXY{ rotated.y - rotated.x, ((rotated.x + rotated.y) >> 1) - pos.z };
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
        auto* viewport = ViewportGetMain();
        if (viewport == nullptr)
        {
            LOG_VERBOSE("No viewport found! Will return 0.");
            return 0;
        }
        uint8_t rotation = viewport->rotation;
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
        if (Config::Get().general.ShowHeightAsUnits)
            return 0;

        // Height labels in feet
        if (Config::Get().general.MeasurementFormat == MeasurementFormat::Imperial)
            return 1 * 256;

        // Height labels in metres
        return 2 * 256;
    }

    void ViewportSetSavedView()
    {
        WindowBase* w = WindowGetMain();
        if (w != nullptr)
        {
            Viewport* viewport = w->viewport;
            auto& gameState = GetGameState();

            gameState.SavedView = ScreenCoordsXY{ viewport->ViewWidth() / 2, viewport->ViewHeight() / 2 } + viewport->viewPos;

            gameState.SavedViewZoom = viewport->zoom;
            gameState.SavedViewRotation = viewport->rotation;
        }
    }
} // namespace OpenRCT2

ZoomLevel ZoomLevel::min()
{
    return ZoomLevel{ -2 };
}
