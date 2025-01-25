/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "ViewportQuery.h"

#include "Viewport.h"
#include "Window.h"

#include <algorithm>
#include <openrct2/Context.h>
#include <openrct2/core/Numerics.hpp>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/tile_element/EntranceElement.h>
#include <openrct2/world/tile_element/PathElement.h>
#include <openrct2/world/tile_element/TileElement.h>

namespace OpenRCT2::Ui
{
    /**
     * Determines the location of the footpath at which we point with the cursor. If no footpath is underneath the cursor,
     * then return the location of the ground tile. Besides the location it also computes the direction of the yellow arrow
     * when we are going to build a footpath bridge/tunnel.
     *  rct2: 0x00689726
     *  In:
     *      screenX: eax
     *      screenY: ebx
     *  Out:
     *      x: ax
     *      y: bx
     *      direction: ecx
     *      tileElement: edx
     */
    CoordsXY FootpathGetCoordinatesFromPos(const ScreenCoordsXY& screenCoords, int32_t* direction, TileElement** tileElement)
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* window = windowMgr->FindFromPoint(screenCoords);
        if (window == nullptr || window->viewport == nullptr)
        {
            CoordsXY position{};
            position.SetNull();
            return position;
        }
        auto viewport = window->viewport;
        auto info = GetMapCoordinatesFromPosWindow(window, screenCoords, EnumsToFlags(ViewportInteractionItem::Footpath));
        if (info.interactionType != ViewportInteractionItem::Footpath
            || !(viewport->flags & (VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_VERTICAL)))
        {
            info = GetMapCoordinatesFromPosWindow(
                window, screenCoords, EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Footpath));
            if (info.interactionType == ViewportInteractionItem::None)
            {
                auto position = info.Loc;
                position.SetNull();
                return position;
            }
        }

        auto minPosition = info.Loc;
        auto maxPosition = info.Loc + CoordsXY{ 31, 31 };
        auto myTileElement = info.Element;
        auto position = info.Loc.ToTileCentre();
        auto z = 0;
        if (info.interactionType == ViewportInteractionItem::Footpath)
        {
            z = myTileElement->GetBaseZ();
            if (myTileElement->AsPath()->IsSloped())
            {
                z += 8;
            }
        }

        auto start_vp_pos = viewport->ScreenToViewportCoord(screenCoords);

        for (int32_t i = 0; i < 5; i++)
        {
            if (info.interactionType != ViewportInteractionItem::Footpath)
            {
                z = TileElementHeight(position);
            }
            position = ViewportPosToMapPos(start_vp_pos, z, viewport->rotation);
            position.x = std::clamp(position.x, minPosition.x, maxPosition.x);
            position.y = std::clamp(position.y, minPosition.y, maxPosition.y);
        }

        // Determine to which edge the cursor is closest
        uint32_t myDirection;
        int32_t mod_x = position.x & 0x1F, mod_y = position.y & 0x1F;
        if (mod_x < mod_y)
        {
            if (mod_x + mod_y < 32)
            {
                myDirection = 0;
            }
            else
            {
                myDirection = 1;
            }
        }
        else
        {
            if (mod_x + mod_y < 32)
            {
                myDirection = 3;
            }
            else
            {
                myDirection = 2;
            }
        }

        position = position.ToTileStart();

        if (direction != nullptr)
            *direction = myDirection;
        if (tileElement != nullptr)
            *tileElement = myTileElement;

        return position;
    }

    /**
     *
     *  rct2: 0x0068A0C9
     * screenX: eax
     * screenY: ebx
     * x: ax
     * y: bx
     * direction: cl
     * tileElement: edx
     */
    CoordsXY FootpathBridgeGetInfoFromPos(const ScreenCoordsXY& screenCoords, int32_t* direction, TileElement** tileElement)
    {
        // First check if we point at an entrance or exit. In that case, we would want the path coming from the entrance/exit.
        auto* windowMgr = GetWindowManager();
        WindowBase* window = windowMgr->FindFromPoint(screenCoords);
        if (window == nullptr || window->viewport == nullptr)
        {
            CoordsXY ret{};
            ret.SetNull();
            return ret;
        }
        auto viewport = window->viewport;
        auto info = GetMapCoordinatesFromPosWindow(window, screenCoords, EnumsToFlags(ViewportInteractionItem::Ride));
        *tileElement = info.Element;
        if (info.interactionType == ViewportInteractionItem::Ride
            && viewport->flags & (VIEWPORT_FLAG_UNDERGROUND_INSIDE | VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_HIDE_VERTICAL)
            && (*tileElement)->GetType() == TileElementType::Entrance)
        {
            uint32_t directions = (*tileElement)->AsEntrance()->GetDirections();
            if (directions & 0x0F)
            {
                int32_t bx = Numerics::bitScanForward(directions);
                bx += (*tileElement)->AsEntrance()->GetDirection();
                bx &= 3;
                if (direction != nullptr)
                    *direction = bx;
                return info.Loc;
            }
        }

        info = GetMapCoordinatesFromPosWindow(
            window, screenCoords,
            EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Footpath, ViewportInteractionItem::Ride));
        if (info.interactionType == ViewportInteractionItem::Ride && (*tileElement)->GetType() == TileElementType::Entrance)
        {
            uint32_t directions = (*tileElement)->AsEntrance()->GetDirections();
            if (directions & 0x0F)
            {
                int32_t bx = (*tileElement)->GetDirectionWithOffset(Numerics::bitScanForward(directions));
                if (direction != nullptr)
                    *direction = bx;
                return info.Loc;
            }
        }

        // We point at something else
        return FootpathGetCoordinatesFromPos(screenCoords, direction, tileElement);
    }
} // namespace OpenRCT2::Ui
