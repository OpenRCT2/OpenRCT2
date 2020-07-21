/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TileInspector.h"

#include "../Context.h"
#include "../Game.h"
#include "../actions/GameAction.h"
#include "../common.h"
#include "../core/Guard.hpp"
#include "../interface/Window.h"
#include "../interface/Window_internal.h"
#include "../localisation/Localisation.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../windows/Intent.h"
#include "../windows/tile_inspector.h"
#include "Banner.h"
#include "Footpath.h"
#include "LargeScenery.h"
#include "Map.h"
#include "Park.h"
#include "Scenery.h"
#include "Surface.h"

using namespace OpenRCT2;

uint32_t windowTileInspectorTileX;
uint32_t windowTileInspectorTileY;
int32_t windowTileInspectorElementCount = 0;
int32_t windowTileInspectorSelectedIndex;

static bool map_swap_elements_at(const CoordsXY& loc, int16_t first, int16_t second)
{
    TileElement* const firstElement = map_get_nth_element_at(loc, first);
    TileElement* const secondElement = map_get_nth_element_at(loc, second);

    if (firstElement == nullptr)
    {
        log_error("First element is out of range for the tile");
        return false;
    }
    if (secondElement == nullptr)
    {
        log_error("Second element is out of range for the tile");
        return false;
    }
    if (firstElement == secondElement)
    {
        log_error("Can't swap the element with itself");
        return false;
    }

    // Swap their memory
    TileElement temp = *firstElement;
    *firstElement = *secondElement;
    *secondElement = temp;

    // Swap the 'last map element for tile' flag if either one of them was last
    if ((firstElement)->IsLastForTile() || (secondElement)->IsLastForTile())
    {
        firstElement->SetLastForTile(!firstElement->IsLastForTile());
        secondElement->SetLastForTile(!secondElement->IsLastForTile());
    }

    return true;
}

/**
 * Inserts a corrupt element under a given element on a given tile
 * @param x The x coordinate of the tile
 * @param y The y coordinate of the tile
 * @param elementIndex The nth element on this tile
 * Returns 0 on success, MONEY_UNDEFINED otherwise.
 */
GameActionResult::Ptr tile_inspector_insert_corrupt_at(const CoordsXY& loc, int16_t elementIndex, bool isExecuting)
{
    // Make sure there is enough space for the new element
    if (!map_check_free_elements_and_reorganise(1))
        return std::make_unique<GameActionResult>(GA_ERROR::NO_FREE_ELEMENTS, STR_NONE);

    if (isExecuting)
    {
        // Create new corrupt element
        TileElement* corruptElement = tile_element_insert(
            { loc, (-1 * COORDS_Z_STEP) }, 0b0000); // Ugly hack: -1 guarantees this to be placed first
        if (corruptElement == nullptr)
        {
            log_warning("Failed to insert corrupt element.");
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);
        }
        corruptElement->SetType(TILE_ELEMENT_TYPE_CORRUPT);

        // Set the base height to be the same as the selected element
        TileElement* const selectedElement = map_get_nth_element_at(loc, elementIndex + 1);
        if (selectedElement == nullptr)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);
        }
        corruptElement->base_height = corruptElement->clearance_height = selectedElement->base_height;

        // Move the corrupt element up until the selected list item is reached
        // this way it's placed under the selected element, even when there are multiple elements with the same base height
        for (int16_t i = 0; i < elementIndex; i++)
        {
            if (!map_swap_elements_at(loc, i, i + 1))
            {
                // don't return error here, we've already inserted an element
                // and moved it as far as we could, the only sensible thing left
                // to do is to invalidate the window.
                break;
            }
        }

        map_invalidate_tile_full(loc);

        // Update the tile inspector's list for everyone who has the tile selected
        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            windowTileInspectorElementCount++;

            // Keep other elements (that are not being hidden) selected
            if (windowTileInspectorSelectedIndex > elementIndex)
            {
                windowTileInspectorSelectedIndex++;
            }

            tileInspectorWindow->Invalidate();
        }
    }

    // Nothing went wrong
    return std::make_unique<GameActionResult>();
}

static int32_t numLargeScenerySequences(const CoordsXY& loc, const LargeSceneryElement* const largeScenery)
{
    const rct_scenery_entry* const largeEntry = largeScenery->GetEntry();
    const auto* const tiles = largeEntry->large_scenery.tiles;
    const auto direction = largeScenery->GetDirection();

    const auto rotatedFirstTile = CoordsXYZ{
        CoordsXY{ tiles[largeScenery->GetSequenceIndex()].x_offset, tiles[largeScenery->GetSequenceIndex()].y_offset }.Rotate(
            direction),
        tiles[largeScenery->GetSequenceIndex()].z_offset
    };

    const auto firstTile = CoordsXYZ{ loc, largeScenery->GetBaseZ() } - rotatedFirstTile;
    auto numFoundElements = 0;
    for (int32_t i = 0; tiles[i].x_offset != -1; i++)
    {
        const auto rotatedCurrentTile = CoordsXYZ{ CoordsXY{ tiles[i].x_offset, tiles[i].y_offset }.Rotate(direction),
                                                   tiles[i].z_offset };

        const auto currentTile = firstTile + rotatedCurrentTile;

        const TileElement* tileElement = map_get_first_element_at(currentTile);
        if (tileElement != nullptr)
        {
            do
            {
                if (tileElement->GetType() != TILE_ELEMENT_TYPE_LARGE_SCENERY)
                    continue;

                if (tileElement->GetDirection() != direction)
                    continue;

                if (tileElement->AsLargeScenery()->GetSequenceIndex() != i)
                    continue;

                if (tileElement->GetBaseZ() != currentTile.z)
                    continue;

                numFoundElements++;
                break;
            } while (!(tileElement++)->IsLastForTile());
        }
    }
    return numFoundElements;
}

/**
 * Forcefully removes an element for a given tile
 * @param x The x coordinate of the tile
 * @param y The y coordinate of the tile
 * @param elementIndex The nth element on this tile
 */
GameActionResult::Ptr tile_inspector_remove_element_at(const CoordsXY& loc, int16_t elementIndex, bool isExecuting)
{
    if (isExecuting)
    {
        // Forcefully remove the element
        TileElement* const tileElement = map_get_nth_element_at(loc, elementIndex);
        if (tileElement == nullptr)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);
        }

        auto largeScenery = tileElement->AsLargeScenery();
        if (largeScenery)
        {
            // Only delete the banner entry if there are no other parts of the large scenery to delete
            if (numLargeScenerySequences(loc, largeScenery) == 1)
            {
                tile_element_remove_banner_entry(tileElement);
            }
        }
        else
        {
            // Removes any potential banners from the entry
            tile_element_remove_banner_entry(tileElement);
        }

        tile_element_remove(tileElement);
        map_invalidate_tile_full(loc);

        // Update the window
        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            windowTileInspectorElementCount--;

            if (windowTileInspectorSelectedIndex > elementIndex)
            {
                windowTileInspectorSelectedIndex--;
            }
            else if (windowTileInspectorSelectedIndex == elementIndex)
            {
                windowTileInspectorSelectedIndex = -1;
            }

            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_swap_elements_at(const CoordsXY& loc, int16_t first, int16_t second, bool isExecuting)
{
    if (isExecuting)
    {
        if (!map_swap_elements_at(loc, first, second))
        {
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);
        }
        map_invalidate_tile_full(loc);

        // Update the window
        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            // If one of them was selected, update selected list item
            if (windowTileInspectorSelectedIndex == first)
                windowTileInspectorSelectedIndex = second;
            else if (windowTileInspectorSelectedIndex == second)
                windowTileInspectorSelectedIndex = first;

            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_rotate_element_at(const CoordsXY& loc, int32_t elementIndex, bool isExecuting)
{
    if (isExecuting)
    {
        uint8_t newRotation, pathEdges, pathCorners;

        TileElement* const tileElement = map_get_nth_element_at(loc, elementIndex);
        if (tileElement == nullptr)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);
        }
        switch (tileElement->GetType())
        {
            case TILE_ELEMENT_TYPE_PATH:
                if (tileElement->AsPath()->IsSloped())
                {
                    newRotation = (tileElement->AsPath()->GetSlopeDirection() + 1) & TILE_ELEMENT_DIRECTION_MASK;
                    tileElement->AsPath()->SetSlopeDirection(newRotation);
                }
                pathEdges = tileElement->AsPath()->GetEdges();
                pathCorners = tileElement->AsPath()->GetCorners();
                tileElement->AsPath()->SetEdges((pathEdges << 1) | (pathEdges >> 3));
                tileElement->AsPath()->SetCorners((pathCorners << 1) | (pathCorners >> 3));
                break;
            case TILE_ELEMENT_TYPE_ENTRANCE:
            {
                // Update element rotation
                newRotation = tileElement->GetDirectionWithOffset(1);
                tileElement->SetDirection(newRotation);

                // Update ride's known entrance/exit rotation
                auto ride = get_ride(tileElement->AsEntrance()->GetRideIndex());
                if (ride != nullptr)
                {
                    auto stationIndex = tileElement->AsEntrance()->GetStationIndex();
                    auto entrance = ride_get_entrance_location(ride, stationIndex);
                    auto exit = ride_get_exit_location(ride, stationIndex);
                    uint8_t entranceType = tileElement->AsEntrance()->GetEntranceType();
                    uint8_t z = tileElement->base_height;

                    // Make sure this is the correct entrance or exit
                    if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE && entrance.x == loc.x / 32 && entrance.y == loc.y / 32
                        && entrance.z == z)
                    {
                        ride_set_entrance_location(ride, stationIndex, { entrance.x, entrance.y, entrance.z, newRotation });
                    }
                    else if (
                        entranceType == ENTRANCE_TYPE_RIDE_EXIT && exit.x == loc.x / 32 && exit.y == loc.y / 32 && exit.z == z)
                    {
                        ride_set_exit_location(ride, stationIndex, { exit.x, exit.y, exit.z, newRotation });
                    }
                }
                break;
            }
            case TILE_ELEMENT_TYPE_TRACK:
            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
            case TILE_ELEMENT_TYPE_WALL:
                newRotation = tileElement->GetDirectionWithOffset(1);
                tileElement->SetDirection(newRotation);
                break;
            case TILE_ELEMENT_TYPE_BANNER:
            {
                uint8_t unblockedEdges = tileElement->AsBanner()->GetAllowedEdges();
                unblockedEdges = (unblockedEdges << 1 | unblockedEdges >> 3) & 0xF;
                tileElement->AsBanner()->SetAllowedEdges(unblockedEdges);
                tileElement->AsBanner()->SetPosition((tileElement->AsBanner()->GetPosition() + 1) & 3);
                break;
            }
        }

        map_invalidate_tile_full(loc);

        if (static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_paste_element_at(const CoordsXY& loc, TileElement element, bool isExecuting)
{
    // Make sure there is enough space for the new element
    if (!map_check_free_elements_and_reorganise(1))
    {
        return std::make_unique<GameActionResult>(GA_ERROR::NO_FREE_ELEMENTS, STR_NONE);
    }

    auto tileLoc = TileCoordsXY(loc);

    if (isExecuting)
    {
        // Check if the element to be pasted refers to a banner index
        auto bannerIndex = tile_element_get_banner_index(&element);
        if (bannerIndex != BANNER_INDEX_NULL)
        {
            // The element to be pasted refers to a banner index - make a copy of it
            auto newBannerIndex = create_new_banner(GAME_COMMAND_FLAG_APPLY);
            if (newBannerIndex == BANNER_INDEX_NULL)
            {
                return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);
            }
            auto& newBanner = *GetBanner(newBannerIndex);
            newBanner = *GetBanner(bannerIndex);
            newBanner.position = tileLoc;

            // Use the new banner index
            tile_element_set_banner_index(&element, newBannerIndex);
        }

        // The occupiedQuadrants will be automatically set when the element is copied over, so it's not necessary to set them
        // correctly _here_.
        TileElement* const pastedElement = tile_element_insert({ loc, element.GetBaseZ() }, 0b0000);

        bool lastForTile = pastedElement->IsLastForTile();
        *pastedElement = element;
        pastedElement->SetLastForTile(lastForTile);

        map_invalidate_tile_full(loc);

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(tileLoc.x) == windowTileInspectorTileX
            && static_cast<uint32_t>(tileLoc.y) == windowTileInspectorTileY)
        {
            windowTileInspectorElementCount++;

            // Select new element if there was none selected already
            int16_t newIndex = static_cast<int16_t>((pastedElement - map_get_first_element_at(loc)));
            if (windowTileInspectorSelectedIndex == -1)
                windowTileInspectorSelectedIndex = newIndex;
            else if (windowTileInspectorSelectedIndex >= newIndex)
                windowTileInspectorSelectedIndex++;

            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_sort_elements_at(const CoordsXY& loc, bool isExecuting)
{
    if (isExecuting)
    {
        const TileElement* const firstElement = map_get_first_element_at(loc);
        if (firstElement == nullptr)
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

        // Count elements on tile
        int32_t numElement = 0;
        const TileElement* elementIterator = firstElement;
        do
        {
            numElement++;
        } while (!(elementIterator++)->IsLastForTile());

        // Bubble sort
        for (int32_t loopStart = 1; loopStart < numElement; loopStart++)
        {
            int32_t currentId = loopStart;
            const TileElement* currentElement = firstElement + currentId;
            const TileElement* otherElement = currentElement - 1;

            // While current element's base height is lower, or (when their baseheight is the same) the other map element's
            // clearance height is lower...
            while (currentId > 0
                   && (otherElement->base_height > currentElement->base_height
                       || (otherElement->base_height == currentElement->base_height
                           && otherElement->clearance_height > currentElement->clearance_height)))
            {
                if (!map_swap_elements_at(loc, currentId - 1, currentId))
                {
                    // don't return error here, we've already ran some actions
                    // and moved things as far as we could, the only sensible
                    // thing left to do is to invalidate the window.
                    loopStart = numElement;
                    break;
                }
                currentId--;
                currentElement--;
                otherElement--;
            }
        }

        map_invalidate_tile_full(loc);

        // Deselect tile for clients who had it selected
        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            windowTileInspectorSelectedIndex = -1;
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_any_base_height_offset(
    const CoordsXY& loc, int16_t elementIndex, int8_t heightOffset, bool isExecuting)
{
    TileElement* const tileElement = map_get_nth_element_at(loc, elementIndex);
    if (tileElement == nullptr)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    int16_t newBaseHeight = static_cast<int16_t>(tileElement->base_height + heightOffset);
    int16_t newClearanceHeight = static_cast<int16_t>(tileElement->clearance_height + heightOffset);
    if (newBaseHeight < 0 || newBaseHeight > MAX_ELEMENT_HEIGHT || newClearanceHeight < 0
        || newClearanceHeight > MAX_ELEMENT_HEIGHT)
    {
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);
    }

    if (isExecuting)
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
        {
            uint8_t entranceType = tileElement->AsEntrance()->GetEntranceType();
            if (entranceType != ENTRANCE_TYPE_PARK_ENTRANCE)
            {
                // Update the ride's known entrance or exit height
                auto ride = get_ride(tileElement->AsEntrance()->GetRideIndex());
                if (ride != nullptr)
                {
                    auto entranceIndex = tileElement->AsEntrance()->GetStationIndex();
                    auto entrance = ride_get_entrance_location(ride, entranceIndex);
                    auto exit = ride_get_exit_location(ride, entranceIndex);
                    uint8_t z = tileElement->base_height;

                    // Make sure this is the correct entrance or exit
                    if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE && entrance.x == loc.x / 32 && entrance.y == loc.y / 32
                        && entrance.z == z)
                        ride_set_entrance_location(
                            ride, entranceIndex, { entrance.x, entrance.y, z + heightOffset, entrance.direction });
                    else if (
                        entranceType == ENTRANCE_TYPE_RIDE_EXIT && exit.x == loc.x / 32 && exit.y == loc.y / 32 && exit.z == z)
                        ride_set_exit_location(ride, entranceIndex, { exit.x, exit.y, z + heightOffset, exit.direction });
                }
            }
        }

        tileElement->base_height += heightOffset;
        tileElement->clearance_height += heightOffset;

        map_invalidate_tile_full(loc);

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_surface_show_park_fences(const CoordsXY& loc, bool showFences, bool isExecuting)
{
    auto* const surfaceelement = map_get_surface_element_at(loc);

    // No surface element on tile
    if (surfaceelement == nullptr)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        if (!showFences)
            surfaceelement->SetParkFences(0);
        else
            update_park_fences(loc);

        map_invalidate_tile_full(loc);

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_surface_toggle_corner(const CoordsXY& loc, int32_t cornerIndex, bool isExecuting)
{
    auto* const surfaceElement = map_get_surface_element_at(loc);

    // No surface element on tile
    if (surfaceElement == nullptr)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        const uint8_t originalSlope = surfaceElement->GetSlope();
        const bool diagonal = (originalSlope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT) >> 4;

        uint8_t newSlope = surfaceElement->GetSlope() ^ (1 << cornerIndex);
        surfaceElement->SetSlope(newSlope);
        if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
        {
            surfaceElement->clearance_height = surfaceElement->base_height + 2;
        }
        else
        {
            surfaceElement->clearance_height = surfaceElement->base_height;
        }

        // All corners are raised
        if ((surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) == TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
        {
            uint8_t slope = TILE_ELEMENT_SLOPE_FLAT;

            if (diagonal)
            {
                switch (originalSlope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
                {
                    case TILE_ELEMENT_SLOPE_S_CORNER_DN:
                        slope |= TILE_ELEMENT_SLOPE_N_CORNER_UP;
                        break;
                    case TILE_ELEMENT_SLOPE_W_CORNER_DN:
                        slope |= TILE_ELEMENT_SLOPE_E_CORNER_UP;
                        break;
                    case TILE_ELEMENT_SLOPE_N_CORNER_DN:
                        slope |= TILE_ELEMENT_SLOPE_S_CORNER_UP;
                        break;
                    case TILE_ELEMENT_SLOPE_E_CORNER_DN:
                        slope |= TILE_ELEMENT_SLOPE_W_CORNER_UP;
                        break;
                }
            }
            surfaceElement->SetSlope(slope);

            // Update base and clearance heights
            surfaceElement->base_height += 2;
            surfaceElement->clearance_height = surfaceElement->base_height + (diagonal ? 2 : 0);
        }

        map_invalidate_tile_full(loc);

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_surface_toggle_diagonal(const CoordsXY& loc, bool isExecuting)
{
    auto* const surfaceElement = map_get_surface_element_at(loc);

    // No surface element on tile
    if (surfaceElement == nullptr)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        uint8_t newSlope = surfaceElement->GetSlope() ^ TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT;
        surfaceElement->SetSlope(newSlope);
        if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
        {
            surfaceElement->clearance_height = surfaceElement->base_height + 4;
        }
        else if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
        {
            surfaceElement->clearance_height = surfaceElement->base_height + 2;
        }
        else
        {
            surfaceElement->clearance_height = surfaceElement->base_height;
        }

        map_invalidate_tile_full(loc);

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_path_set_sloped(const CoordsXY& loc, int32_t elementIndex, bool sloped, bool isExecuting)
{
    TileElement* const pathElement = map_get_nth_element_at(loc, elementIndex);

    if (pathElement == nullptr || pathElement->GetType() != TILE_ELEMENT_TYPE_PATH)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        pathElement->AsPath()->SetSloped(sloped);

        map_invalidate_tile_full(loc);

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_path_set_broken(const CoordsXY& loc, int32_t elementIndex, bool broken, bool isExecuting)
{
    TileElement* const pathElement = map_get_nth_element_at(loc, elementIndex);

    if (pathElement == nullptr || pathElement->GetType() != TILE_ELEMENT_TYPE_PATH)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        pathElement->AsPath()->SetIsBroken(broken);

        map_invalidate_tile_full(loc);

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_path_toggle_edge(
    const CoordsXY& loc, int32_t elementIndex, int32_t edgeIndex, bool isExecuting)
{
    TileElement* const pathElement = map_get_nth_element_at(loc, elementIndex);

    if (pathElement == nullptr || pathElement->GetType() != TILE_ELEMENT_TYPE_PATH)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        uint8_t newEdges = pathElement->AsPath()->GetEdgesAndCorners() ^ (1 << edgeIndex);
        pathElement->AsPath()->SetEdgesAndCorners(newEdges);

        map_invalidate_tile_full(loc);

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_entrance_make_usable(const CoordsXY& loc, int32_t elementIndex, bool isExecuting)
{
    TileElement* const entranceElement = map_get_nth_element_at(loc, elementIndex);

    if (entranceElement == nullptr || entranceElement->GetType() != TILE_ELEMENT_TYPE_ENTRANCE)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    auto ride = get_ride(entranceElement->AsEntrance()->GetRideIndex());
    if (ride == nullptr)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        auto stationIndex = entranceElement->AsEntrance()->GetStationIndex();

        switch (entranceElement->AsEntrance()->GetEntranceType())
        {
            case ENTRANCE_TYPE_RIDE_ENTRANCE:
                ride_set_entrance_location(
                    ride, stationIndex,
                    { loc.x / 32, loc.y / 32, entranceElement->base_height,
                      static_cast<uint8_t>(entranceElement->GetDirection()) });
                break;
            case ENTRANCE_TYPE_RIDE_EXIT:
                ride_set_exit_location(
                    ride, stationIndex,
                    { loc.x / 32, loc.y / 32, entranceElement->base_height,
                      static_cast<uint8_t>(entranceElement->GetDirection()) });
                break;
        }

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_wall_set_slope(
    const CoordsXY& loc, int32_t elementIndex, int32_t slopeValue, bool isExecuting)
{
    TileElement* const wallElement = map_get_nth_element_at(loc, elementIndex);

    if (wallElement == nullptr || wallElement->GetType() != TILE_ELEMENT_TYPE_WALL)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        // Set new slope value
        wallElement->AsWall()->SetSlope(slopeValue);

        map_invalidate_tile_full(loc);

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_wall_animation_frame_offset(
    const CoordsXY& loc, int16_t elementIndex, int8_t animationFrameOffset, bool isExecuting)
{
    TileElement* const wallElement = map_get_nth_element_at(loc, elementIndex);

    if (wallElement == nullptr || wallElement->GetType() != TILE_ELEMENT_TYPE_WALL)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        uint8_t animationFrame = wallElement->AsWall()->GetAnimationFrame();
        wallElement->AsWall()->SetAnimationFrame(animationFrame + animationFrameOffset);

        map_invalidate_tile_full(loc);

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

// Changes the height of all track elements that belong to the same track piece
// Broxzier: Copied from track_remove and stripped of unneeded code, but I think this should be smaller
GameActionResult::Ptr tile_inspector_track_base_height_offset(
    const CoordsXY& loc, int32_t elementIndex, int8_t offset, bool isExecuting)
{
    if (offset == 0)
        return std::make_unique<GameActionResult>();

    TileElement* const trackElement = map_get_nth_element_at(loc, elementIndex);

    if (trackElement == nullptr || trackElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        auto type = trackElement->AsTrack()->GetTrackType();
        int16_t originX = loc.x;
        int16_t originY = loc.y;
        int16_t originZ = trackElement->GetBaseZ();
        uint8_t rotation = trackElement->GetDirection();
        auto rideIndex = trackElement->AsTrack()->GetRideIndex();
        auto ride = get_ride(rideIndex);
        if (ride == nullptr)
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

        auto trackBlock = get_track_def_from_ride(ride, type);
        trackBlock += trackElement->AsTrack()->GetSequenceIndex();

        uint8_t originDirection = trackElement->GetDirection();
        CoordsXY offsets = { trackBlock->x, trackBlock->y };
        CoordsXY coords = { originX, originY };
        coords += offsets.Rotate(direction_reverse(originDirection));

        originX = static_cast<int16_t>(coords.x);
        originY = static_cast<int16_t>(coords.y);
        originZ -= trackBlock->z;

        trackBlock = get_track_def_from_ride(ride, type);
        for (; trackBlock->index != 255; trackBlock++)
        {
            CoordsXY elem = { originX, originY };
            int16_t elemZ = originZ;
            offsets.x = trackBlock->x;
            offsets.y = trackBlock->y;
            elem += offsets.Rotate(originDirection);
            elemZ += trackBlock->z;

            map_invalidate_tile_full(elem);

            bool found = false;
            TileElement* tileElement = map_get_first_element_at({ elem.x, elem.y });
            do
            {
                if (tileElement == nullptr)
                    break;

                if (tileElement->GetBaseZ() != elemZ)
                    continue;

                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;

                if (tileElement->GetDirection() != rotation)
                    continue;

                if (tileElement->AsTrack()->GetSequenceIndex() != trackBlock->index)
                    continue;

                if (tileElement->AsTrack()->GetTrackType() != type)
                    continue;

                found = true;
                break;
            } while (!(tileElement++)->IsLastForTile());

            if (!found)
            {
                log_error("Track map element part not found!");
                return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);
            }

            // track_remove returns here on failure, not sure when this would ever be hit. Only thing I can think of is for when
            // you decrease the map size.
            openrct2_assert(map_get_surface_element_at(elem) != nullptr, "No surface at %d,%d", elem.x >> 5, elem.y >> 5);

            // Keep?
            // invalidate_test_results(ride);

            tileElement->base_height += offset;
            tileElement->clearance_height += offset;
        }
    }

    // TODO: Only invalidate when one of the affected tiles is selected
    window_invalidate_by_class(WC_TILE_INSPECTOR);

    return std::make_unique<GameActionResult>();
}

// Sets chainlift, optionally for an entire track block
// Broxzier: Basically a copy of the above function, with just two different lines... should probably be combined somehow
GameActionResult::Ptr tile_inspector_track_set_chain(
    const CoordsXY& loc, int32_t elementIndex, bool entireTrackBlock, bool setChain, bool isExecuting)
{
    TileElement* const trackElement = map_get_nth_element_at(loc, elementIndex);

    if (trackElement == nullptr || trackElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        if (!entireTrackBlock)
        {
            // Set chain for only the selected piece
            if (trackElement->AsTrack()->HasChain() != setChain)
            {
                trackElement->AsTrack()->SetHasChain(setChain);
            }

            return std::make_unique<GameActionResult>();
        }

        auto type = trackElement->AsTrack()->GetTrackType();
        int16_t originX = loc.x;
        int16_t originY = loc.y;
        int16_t originZ = trackElement->GetBaseZ();
        uint8_t rotation = trackElement->GetDirection();
        auto rideIndex = trackElement->AsTrack()->GetRideIndex();
        auto ride = get_ride(rideIndex);
        if (ride == nullptr)
            return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

        auto trackBlock = get_track_def_from_ride(ride, type);
        trackBlock += trackElement->AsTrack()->GetSequenceIndex();

        uint8_t originDirection = trackElement->GetDirection();
        CoordsXY offsets = { trackBlock->x, trackBlock->y };
        CoordsXY coords = { originX, originY };
        coords += offsets.Rotate(direction_reverse(originDirection));

        originX = static_cast<int16_t>(coords.x);
        originY = static_cast<int16_t>(coords.y);
        originZ -= trackBlock->z;

        trackBlock = get_track_def_from_ride(ride, type);
        for (; trackBlock->index != 255; trackBlock++)
        {
            CoordsXY elem = { originX, originY };
            int16_t elemZ = originZ;
            offsets.x = trackBlock->x;
            offsets.y = trackBlock->y;
            elem += offsets.Rotate(originDirection);
            elemZ += trackBlock->z;

            map_invalidate_tile_full(elem);

            bool found = false;
            TileElement* tileElement = map_get_first_element_at({ elem.x, elem.y });
            do
            {
                if (tileElement == nullptr)
                    break;

                if (tileElement->GetBaseZ() != elemZ)
                    continue;

                if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                    continue;

                if (tileElement->GetDirection() != rotation)
                    continue;

                if (tileElement->AsTrack()->GetSequenceIndex() != trackBlock->index)
                    continue;

                if (tileElement->AsTrack()->GetTrackType() != type)
                    continue;

                found = true;
                break;
            } while (!(tileElement++)->IsLastForTile());

            if (!found)
            {
                log_error("Track map element part not found!");
                return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);
            }

            // track_remove returns here on failure, not sure when this would ever be hit. Only thing I can think of is for when
            // you decrease the map size.
            openrct2_assert(map_get_surface_element_at(elem) != nullptr, "No surface at %d,%d", elem.x >> 5, elem.y >> 5);

            // Keep?
            // invalidate_test_results(ride);

            if (tileElement->AsTrack()->HasChain() != setChain)
            {
                tileElement->AsTrack()->SetHasChain(setChain);
            }
        }
    }

    // TODO: Only invalidate when one of the affected tiles is selected
    window_invalidate_by_class(WC_TILE_INSPECTOR);

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_track_set_block_brake(
    const CoordsXY& loc, int32_t elementIndex, bool blockBrake, bool isExecuting)
{
    TileElement* const trackElement = map_get_nth_element_at(loc, elementIndex);

    if (trackElement == nullptr || trackElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        trackElement->AsTrack()->SetBlockBrakeClosed(blockBrake);

        map_invalidate_tile_full(loc);

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_track_set_indestructible(
    const CoordsXY& loc, int32_t elementIndex, bool isIndestructible, bool isExecuting)
{
    TileElement* const trackElement = map_get_nth_element_at(loc, elementIndex);

    if (trackElement == nullptr || trackElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        trackElement->AsTrack()->SetIsIndestructible(isIndestructible);

        map_invalidate_tile_full(loc);

        rct_window* const tileInspectorWindow = window_find_by_class(WC_TILE_INSPECTOR);
        if (tileInspectorWindow != nullptr && static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            tileInspectorWindow->Invalidate();
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_scenery_set_quarter_location(
    const CoordsXY& loc, int32_t elementIndex, int32_t quarterIndex, bool isExecuting)
{
    TileElement* const tileElement = map_get_nth_element_at(loc, elementIndex);

    if (tileElement == nullptr || tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        // Set quadrant index
        tileElement->AsSmallScenery()->SetSceneryQuadrant(quarterIndex);

        // Update collision
        tileElement->SetOccupiedQuadrants(1 << ((quarterIndex + 2) & 3));

        map_invalidate_tile_full(loc);
        if (static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_scenery_set_quarter_collision(
    const CoordsXY& loc, int32_t elementIndex, int32_t quarterIndex, bool isExecuting)
{
    TileElement* const tileElement = map_get_nth_element_at(loc, elementIndex);

    if (tileElement == nullptr || tileElement->GetType() != TILE_ELEMENT_TYPE_SMALL_SCENERY)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        auto occupiedQuadrants = tileElement->GetOccupiedQuadrants();
        occupiedQuadrants ^= 1 << quarterIndex;
        tileElement->SetOccupiedQuadrants(occupiedQuadrants);

        map_invalidate_tile_full(loc);
        if (static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_banner_toggle_blocking_edge(
    const CoordsXY& loc, int32_t elementIndex, int32_t edgeIndex, bool isExecuting)
{
    TileElement* const bannerElement = map_get_nth_element_at(loc, elementIndex);

    if (bannerElement == nullptr || bannerElement->GetType() != TILE_ELEMENT_TYPE_BANNER)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        uint8_t edges = bannerElement->AsBanner()->GetAllowedEdges();
        edges ^= (1 << edgeIndex);
        bannerElement->AsBanner()->SetAllowedEdges(edges);

        if (static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return std::make_unique<GameActionResult>();
}

GameActionResult::Ptr tile_inspector_corrupt_clamp(const CoordsXY& loc, int32_t elementIndex, bool isExecuting)
{
    TileElement* const corruptElement = map_get_nth_element_at(loc, elementIndex);

    if (corruptElement == nullptr || corruptElement->GetType() != TILE_ELEMENT_TYPE_CORRUPT)
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (corruptElement->IsLastForTile())
        return std::make_unique<GameActionResult>(GA_ERROR::UNKNOWN, STR_NONE);

    if (isExecuting)
    {
        TileElement* const nextElement = corruptElement + 1;
        corruptElement->base_height = corruptElement->clearance_height = nextElement->base_height;

        if (static_cast<uint32_t>(loc.x / 32) == windowTileInspectorTileX
            && static_cast<uint32_t>(loc.y / 32) == windowTileInspectorTileY)
        {
            window_invalidate_by_class(WC_TILE_INSPECTOR);
        }
    }

    return std::make_unique<GameActionResult>();
}
