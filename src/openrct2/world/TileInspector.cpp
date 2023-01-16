/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "../ride/TrackData.h"
#include "../windows/Intent.h"
#include "../windows/tile_inspector.h"
#include "Banner.h"
#include "Footpath.h"
#include "LargeScenery.h"
#include "Location.hpp"
#include "Map.h"
#include "Park.h"
#include "Scenery.h"
#include "Surface.h"

#include <algorithm>
#include <optional>

TileCoordsXY windowTileInspectorTile;
int32_t windowTileInspectorElementCount = 0;
int32_t windowTileInspectorSelectedIndex;

using namespace OpenRCT2::TrackMetaData;
namespace OpenRCT2::TileInspector
{
    static bool SwapTileElements(const CoordsXY& loc, int16_t first, int16_t second)
    {
        TileElement* const firstElement = MapGetNthElementAt(loc, first);
        TileElement* const secondElement = MapGetNthElementAt(loc, second);

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
        std::swap(*firstElement, *secondElement);

        // Swap the 'last map element for tile' flag if either one of them was last
        if ((firstElement)->IsLastForTile() || (secondElement)->IsLastForTile())
        {
            firstElement->SetLastForTile(!firstElement->IsLastForTile());
            secondElement->SetLastForTile(!secondElement->IsLastForTile());
        }

        return true;
    }

    static rct_window* GetTileInspectorWithPos(const CoordsXY& loc)
    {
        // Return the tile inspector window for everyone who has the tile selected
        auto* window = WindowFindByClass(WindowClass::TileInspector);
        if (window != nullptr && loc == windowTileInspectorTile.ToCoordsXY())
        {
            return window;
        }
        return nullptr;
    }

    static int32_t NumLargeScenerySequences(const CoordsXY& loc, const LargeSceneryElement* const largeScenery)
    {
        const auto* const largeEntry = largeScenery->GetEntry();
        const auto direction = largeScenery->GetDirection();
        const auto sequenceIndex = largeScenery->GetSequenceIndex();
        const auto* tiles = largeEntry->tiles;
        const auto& tile = tiles[sequenceIndex];
        const auto rotatedFirstTile = CoordsXYZ{
            CoordsXY{ tile.x_offset, tile.y_offset }.Rotate(direction),
            tile.z_offset,
        };

        const auto firstTile = CoordsXYZ{ loc, largeScenery->GetBaseZ() } - rotatedFirstTile;
        auto numFoundElements = 0;
        for (int32_t i = 0; tiles[i].x_offset != -1; i++)
        {
            const auto rotatedCurrentTile = CoordsXYZ{ CoordsXY{ tiles[i].x_offset, tiles[i].y_offset }.Rotate(direction),
                                                       tiles[i].z_offset };

            const auto currentTile = firstTile + rotatedCurrentTile;

            const TileElement* tileElement = MapGetFirstElementAt(currentTile);
            if (tileElement != nullptr)
            {
                do
                {
                    if (tileElement->GetType() != TileElementType::LargeScenery)
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
    GameActions::Result RemoveElementAt(const CoordsXY& loc, int16_t elementIndex, bool isExecuting)
    {
        TileElement* const tileElement = MapGetNthElementAt(loc, elementIndex);
        if (tileElement == nullptr)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            // Forcefully remove the element
            auto largeScenery = tileElement->AsLargeScenery();
            if (largeScenery != nullptr)
            {
                // Only delete the banner entry if there are no other parts of the large scenery to delete
                if (NumLargeScenerySequences(loc, largeScenery) == 1)
                {
                    tileElement->RemoveBannerEntry();
                }
            }
            else
            {
                // Removes any potential banners from the entry
                tileElement->RemoveBannerEntry();
            }

            TileElementRemove(tileElement);
            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                // Update the window
                windowTileInspectorElementCount--;

                if (windowTileInspectorSelectedIndex > elementIndex)
                {
                    windowTileInspectorSelectedIndex--;
                }
                else if (windowTileInspectorSelectedIndex == elementIndex)
                {
                    windowTileInspectorSelectedIndex = -1;
                }

                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result SwapElementsAt(const CoordsXY& loc, int16_t first, int16_t second, bool isExecuting)
    {
        if (isExecuting)
        {
            if (!SwapTileElements(loc, first, second))
            {
                return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);
            }
            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                // If one of them was selected, update selected list item
                if (windowTileInspectorSelectedIndex == first)
                    windowTileInspectorSelectedIndex = second;
                else if (windowTileInspectorSelectedIndex == second)
                    windowTileInspectorSelectedIndex = first;

                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result RotateElementAt(const CoordsXY& loc, int32_t elementIndex, bool isExecuting)
    {
        TileElement* const tileElement = MapGetNthElementAt(loc, elementIndex);
        if (tileElement == nullptr)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            uint8_t newRotation, pathEdges, pathCorners;

            switch (tileElement->GetType())
            {
                case TileElementType::Path:
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
                case TileElementType::Entrance:
                {
                    // Update element rotation
                    newRotation = tileElement->GetDirectionWithOffset(1);
                    tileElement->SetDirection(newRotation);

                    // Update ride's known entrance/exit rotation
                    auto ride = GetRide(tileElement->AsEntrance()->GetRideIndex());
                    if (ride != nullptr)
                    {
                        auto stationIndex = tileElement->AsEntrance()->GetStationIndex();
                        auto& station = ride->GetStation(stationIndex);
                        auto entrance = station.Entrance;
                        auto exit = station.Exit;
                        uint8_t entranceType = tileElement->AsEntrance()->GetEntranceType();
                        uint8_t z = tileElement->base_height;

                        // Make sure this is the correct entrance or exit
                        if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE && entrance.x == loc.x / COORDS_XY_STEP
                            && entrance.y == loc.y / COORDS_XY_STEP && entrance.z == z)
                        {
                            station.Entrance = { entrance, newRotation };
                        }
                        else if (
                            entranceType == ENTRANCE_TYPE_RIDE_EXIT && exit.x == loc.x / COORDS_XY_STEP
                            && exit.y == loc.y / COORDS_XY_STEP && exit.z == z)
                        {
                            station.Exit = { exit, newRotation };
                        }
                    }
                    break;
                }
                case TileElementType::Track:
                case TileElementType::SmallScenery:
                case TileElementType::Wall:
                    newRotation = tileElement->GetDirectionWithOffset(1);
                    tileElement->SetDirection(newRotation);
                    break;
                case TileElementType::Banner:
                {
                    uint8_t unblockedEdges = tileElement->AsBanner()->GetAllowedEdges();
                    unblockedEdges = (unblockedEdges << 1 | unblockedEdges >> 3) & 0xF;
                    tileElement->AsBanner()->SetAllowedEdges(unblockedEdges);
                    tileElement->AsBanner()->SetPosition((tileElement->AsBanner()->GetPosition() + 1) & 3);
                    break;
                }
                case TileElementType::Surface:
                case TileElementType::LargeScenery:
                    break;
            }

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result ToggleInvisibilityOfElementAt(const CoordsXY& loc, int32_t elementIndex, bool isExecuting)
    {
        TileElement* const tileElement = MapGetNthElementAt(loc, elementIndex);
        if (tileElement == nullptr)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (!isExecuting)
        {
            return GameActions::Result();
        }

        bool currentlyInvisible = tileElement->IsInvisible();
        tileElement->SetInvisible(!currentlyInvisible);

        MapInvalidateTileFull(loc);
        if (loc == windowTileInspectorTile.ToCoordsXY())
        {
            WindowInvalidateByClass(WindowClass::TileInspector);
        }

        return GameActions::Result();
    }

    GameActions::Result PasteElementAt(const CoordsXY& loc, TileElement element, bool isExecuting)
    {
        // Make sure there is enough space for the new element
        if (!MapCheckCapacityAndReorganise(loc))
        {
            return GameActions::Result(GameActions::Status::NoFreeElements, STR_NONE, STR_NONE);
        }

        auto tileLoc = TileCoordsXY(loc);

        if (isExecuting)
        {
            // Check if the element to be pasted refers to a banner index
            auto bannerIndex = element.GetBannerIndex();
            if (bannerIndex != BannerIndex::GetNull())
            {
                // The element to be pasted refers to a banner index - make a copy of it
                auto newBanner = CreateBanner();
                if (newBanner == nullptr)
                {
                    log_error("No free banners available");
                    return GameActions::Result(GameActions::Status::Unknown, STR_TOO_MANY_BANNERS_IN_GAME, STR_NONE);
                }
                // Copy the banners style
                *newBanner = *GetBanner(bannerIndex);
                // Reset the location to the paste location
                newBanner->position = tileLoc;

                // Use the new banner index
                element.SetBannerIndex(newBanner->id);
            }

            // The occupiedQuadrants will be automatically set when the element is copied over, so it's not necessary to set
            // them correctly _here_.
            TileElement* const pastedElement = TileElementInsert({ loc, element.GetBaseZ() }, 0b0000, TileElementType::Surface);

            bool lastForTile = pastedElement->IsLastForTile();
            *pastedElement = element;
            pastedElement->SetLastForTile(lastForTile);

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                windowTileInspectorElementCount++;

                // Select new element if there was none selected already
                int16_t newIndex = static_cast<int16_t>((pastedElement - MapGetFirstElementAt(loc)));
                if (windowTileInspectorSelectedIndex == -1)
                    windowTileInspectorSelectedIndex = newIndex;
                else if (windowTileInspectorSelectedIndex >= newIndex)
                    windowTileInspectorSelectedIndex++;

                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result SortElementsAt(const CoordsXY& loc, bool isExecuting)
    {
        if (isExecuting)
        {
            const TileElement* const firstElement = MapGetFirstElementAt(loc);
            if (firstElement == nullptr)
                return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

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
                    if (!SwapTileElements(loc, currentId - 1, currentId))
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

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                // Deselect tile for clients who had it selected
                windowTileInspectorSelectedIndex = -1;

                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    static GameActions::Result ValidateTileHeight(TileElement* const tileElement, int8_t heightOffset)
    {
        int16_t newBaseHeight = static_cast<int16_t>(tileElement->base_height + heightOffset);
        int16_t newClearanceHeight = static_cast<int16_t>(tileElement->clearance_height + heightOffset);
        if (newBaseHeight < 0)
        {
            return GameActions::Result(GameActions::Status::TooLow, STR_CANT_LOWER_ELEMENT_HERE, STR_TOO_LOW);
        }
        if (newBaseHeight > MAX_ELEMENT_HEIGHT)
        {
            return GameActions::Result(GameActions::Status::TooHigh, STR_CANT_RAISE_ELEMENT_HERE, STR_TOO_HIGH);
        }
        if (newClearanceHeight < 0)
        {
            return GameActions::Result(GameActions::Status::NoClearance, STR_CANT_LOWER_ELEMENT_HERE, STR_NO_CLEARANCE);
        }
        if (newClearanceHeight > MAX_ELEMENT_HEIGHT)
        {
            return GameActions::Result(GameActions::Status::NoClearance, STR_CANT_RAISE_ELEMENT_HERE, STR_NO_CLEARANCE);
        }
        return GameActions::Result();
    }

    GameActions::Result AnyBaseHeightOffset(const CoordsXY& loc, int16_t elementIndex, int8_t heightOffset, bool isExecuting)
    {
        TileElement* const tileElement = MapGetNthElementAt(loc, elementIndex);
        if (tileElement == nullptr)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        auto heightValidationResult = ValidateTileHeight(tileElement, heightOffset);
        if (heightValidationResult.Error != GameActions::Status::Ok)
            return heightValidationResult;

        if (isExecuting)
        {
            if (tileElement->GetType() == TileElementType::Entrance)
            {
                uint8_t entranceType = tileElement->AsEntrance()->GetEntranceType();
                if (entranceType != ENTRANCE_TYPE_PARK_ENTRANCE)
                {
                    // Update the ride's known entrance or exit height
                    auto ride = GetRide(tileElement->AsEntrance()->GetRideIndex());
                    if (ride != nullptr)
                    {
                        auto entranceIndex = tileElement->AsEntrance()->GetStationIndex();
                        auto& station = ride->GetStation(entranceIndex);
                        const auto& entranceLoc = station.Entrance;
                        const auto& exitLoc = station.Exit;
                        uint8_t z = tileElement->base_height;

                        // Make sure this is the correct entrance or exit
                        if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE && entranceLoc == TileCoordsXYZ{ loc, z })
                            station.Entrance = { entranceLoc, z + heightOffset, entranceLoc.direction };
                        else if (entranceType == ENTRANCE_TYPE_RIDE_EXIT && exitLoc == TileCoordsXYZ{ loc, z })
                            station.Exit = { exitLoc, z + heightOffset, exitLoc.direction };
                    }
                }
            }

            tileElement->base_height += heightOffset;
            tileElement->clearance_height += heightOffset;

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result SurfaceShowParkFences(const CoordsXY& loc, bool showFences, bool isExecuting)
    {
        auto* const surfaceelement = MapGetSurfaceElementAt(loc);

        // No surface element on tile
        if (surfaceelement == nullptr)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            if (!showFences)
                surfaceelement->SetParkFences(0);
            else
                ParkUpdateFences(loc);

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result SurfaceToggleCorner(const CoordsXY& loc, int32_t cornerIndex, bool isExecuting)
    {
        auto* const surfaceElement = MapGetSurfaceElementAt(loc);

        // No surface element on tile
        if (surfaceElement == nullptr)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            const uint8_t originalSlope = surfaceElement->GetSlope();
            uint8_t newSlope = surfaceElement->GetSlope() ^ (1 << cornerIndex);

            // All corners are raised
            if ((newSlope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) == TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
            {
                newSlope = TILE_ELEMENT_SLOPE_FLAT;
                if ((originalSlope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT) != 0)
                {
                    switch (originalSlope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
                    {
                        case TILE_ELEMENT_SLOPE_S_CORNER_DN:
                            newSlope |= TILE_ELEMENT_SLOPE_N_CORNER_UP;
                            break;
                        case TILE_ELEMENT_SLOPE_W_CORNER_DN:
                            newSlope |= TILE_ELEMENT_SLOPE_E_CORNER_UP;
                            break;
                        case TILE_ELEMENT_SLOPE_N_CORNER_DN:
                            newSlope |= TILE_ELEMENT_SLOPE_S_CORNER_UP;
                            break;
                        case TILE_ELEMENT_SLOPE_E_CORNER_DN:
                            newSlope |= TILE_ELEMENT_SLOPE_W_CORNER_UP;
                            break;
                    }
                }

                surfaceElement->base_height += 2;
                surfaceElement->clearance_height = surfaceElement->base_height;
            }

            surfaceElement->SetSlope(newSlope);

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result SurfaceToggleDiagonal(const CoordsXY& loc, bool isExecuting)
    {
        auto* const surfaceElement = MapGetSurfaceElementAt(loc);

        // No surface element on tile
        if (surfaceElement == nullptr)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            uint8_t newSlope = surfaceElement->GetSlope() ^ TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT;
            surfaceElement->SetSlope(newSlope);

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result PathSetSloped(const CoordsXY& loc, int32_t elementIndex, bool sloped, bool isExecuting)
    {
        TileElement* const pathElement = MapGetNthElementAt(loc, elementIndex);
        if (pathElement == nullptr || pathElement->GetType() != TileElementType::Path)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            pathElement->AsPath()->SetSloped(sloped);

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result PathSetBroken(const CoordsXY& loc, int32_t elementIndex, bool broken, bool isExecuting)
    {
        TileElement* const pathElement = MapGetNthElementAt(loc, elementIndex);
        if (pathElement == nullptr || pathElement->GetType() != TileElementType::Path)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            pathElement->AsPath()->SetIsBroken(broken);

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result PathToggleEdge(const CoordsXY& loc, int32_t elementIndex, int32_t edgeIndex, bool isExecuting)
    {
        TileElement* const pathElement = MapGetNthElementAt(loc, elementIndex);
        if (pathElement == nullptr || pathElement->GetType() != TileElementType::Path)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            uint8_t newEdges = pathElement->AsPath()->GetEdgesAndCorners() ^ (1 << edgeIndex);
            pathElement->AsPath()->SetEdgesAndCorners(newEdges);

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result EntranceMakeUsable(const CoordsXY& loc, int32_t elementIndex, bool isExecuting)
    {
        TileElement* const entranceElement = MapGetNthElementAt(loc, elementIndex);
        if (entranceElement == nullptr || entranceElement->GetType() != TileElementType::Entrance)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        auto ride = GetRide(entranceElement->AsEntrance()->GetRideIndex());
        if (ride == nullptr)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            auto stationIndex = entranceElement->AsEntrance()->GetStationIndex();
            auto& station = ride->GetStation(stationIndex);

            switch (entranceElement->AsEntrance()->GetEntranceType())
            {
                case ENTRANCE_TYPE_RIDE_ENTRANCE:
                    station.Entrance = { loc, entranceElement->base_height, entranceElement->GetDirection() };
                    break;
                case ENTRANCE_TYPE_RIDE_EXIT:
                    station.Exit = { loc, entranceElement->base_height, entranceElement->GetDirection() };
                    break;
            }

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result WallSetSlope(const CoordsXY& loc, int32_t elementIndex, int32_t slopeValue, bool isExecuting)
    {
        TileElement* const wallElement = MapGetNthElementAt(loc, elementIndex);
        if (wallElement == nullptr || wallElement->GetType() != TileElementType::Wall)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            // Set new slope value
            wallElement->AsWall()->SetSlope(slopeValue);

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result WallAnimationFrameOffset(
        const CoordsXY& loc, int16_t elementIndex, int8_t animationFrameOffset, bool isExecuting)
    {
        TileElement* const wallElement = MapGetNthElementAt(loc, elementIndex);
        if (wallElement == nullptr || wallElement->GetType() != TileElementType::Wall)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            uint8_t animationFrame = wallElement->AsWall()->GetAnimationFrame();
            wallElement->AsWall()->SetAnimationFrame(animationFrame + animationFrameOffset);

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    // Changes the height of all track elements that belong to the same track piece
    // Broxzier: Copied from track_remove and stripped of unneeded code, but I think this should be smaller
    GameActions::Result TrackBaseHeightOffset(const CoordsXY& loc, int32_t elementIndex, int8_t offset, bool isExecuting)
    {
        if (offset == 0)
            return GameActions::Result();

        TileElement* const trackElement = MapGetNthElementAt(loc, elementIndex);
        if (trackElement == nullptr || trackElement->GetType() != TileElementType::Track)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            auto type = trackElement->AsTrack()->GetTrackType();
            int16_t originX = loc.x;
            int16_t originY = loc.y;
            int16_t originZ = trackElement->GetBaseZ();
            uint8_t rotation = trackElement->GetDirection();
            auto rideIndex = trackElement->AsTrack()->GetRideIndex();
            auto ride = GetRide(rideIndex);
            if (ride == nullptr)
                return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

            const auto& ted = GetTrackElementDescriptor(type);
            const auto* trackBlock = ted.GetBlockForSequence(trackElement->AsTrack()->GetSequenceIndex());
            if (trackBlock == nullptr)
                return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

            uint8_t originDirection = trackElement->GetDirection();
            CoordsXY offsets = { trackBlock->x, trackBlock->y };
            CoordsXY coords = { originX, originY };
            coords += offsets.Rotate(DirectionReverse(originDirection));

            originX = static_cast<int16_t>(coords.x);
            originY = static_cast<int16_t>(coords.y);
            originZ -= trackBlock->z;

            trackBlock = ted.Block;
            for (; trackBlock->index != 255; trackBlock++)
            {
                CoordsXYZD elem = { originX, originY, originZ + trackBlock->z, rotation };
                offsets.x = trackBlock->x;
                offsets.y = trackBlock->y;
                elem += offsets.Rotate(originDirection);

                TrackElement* tileElement = MapGetTrackElementAtOfTypeSeq(elem, type, trackBlock->index);
                if (tileElement == nullptr)
                {
                    log_error("Track map element part not found!");
                    return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);
                }

                // track_remove returns here on failure, not sure when this would ever be hit. Only thing I can think of is
                // for when you decrease the map size.
                openrct2_assert(MapGetSurfaceElementAt(elem) != nullptr, "No surface at %d,%d", elem.x >> 5, elem.y >> 5);

                MapInvalidateTileFull(elem);

                // Keep?
                // invalidate_test_results(ride);

                tileElement->base_height += offset;
                tileElement->clearance_height += offset;
            }

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    // Sets chainlift, optionally for an entire track block
    // Broxzier: Basically a copy of the above function, with just two different lines... should probably be combined
    // somehow
    GameActions::Result TrackSetChain(
        const CoordsXY& loc, int32_t elementIndex, bool entireTrackBlock, bool setChain, bool isExecuting)
    {
        TileElement* const trackElement = MapGetNthElementAt(loc, elementIndex);
        if (trackElement == nullptr || trackElement->GetType() != TileElementType::Track)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            if (!entireTrackBlock)
            {
                // Set chain for only the selected piece
                if (trackElement->AsTrack()->HasChain() != setChain)
                {
                    trackElement->AsTrack()->SetHasChain(setChain);
                }

                return GameActions::Result();
            }

            auto type = trackElement->AsTrack()->GetTrackType();
            int16_t originX = loc.x;
            int16_t originY = loc.y;
            int16_t originZ = trackElement->GetBaseZ();
            uint8_t rotation = trackElement->GetDirection();
            auto rideIndex = trackElement->AsTrack()->GetRideIndex();
            auto ride = GetRide(rideIndex);
            if (ride == nullptr)
                return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

            const auto& ted = GetTrackElementDescriptor(type);
            auto trackBlock = ted.GetBlockForSequence(trackElement->AsTrack()->GetSequenceIndex());
            if (trackBlock == nullptr)
                return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

            uint8_t originDirection = trackElement->GetDirection();
            CoordsXY offsets = { trackBlock->x, trackBlock->y };
            CoordsXY coords = { originX, originY };
            coords += offsets.Rotate(DirectionReverse(originDirection));

            originX = static_cast<int16_t>(coords.x);
            originY = static_cast<int16_t>(coords.y);
            originZ -= trackBlock->z;

            trackBlock = ted.Block;
            for (; trackBlock->index != 255; trackBlock++)
            {
                CoordsXYZD elem = { originX, originY, originZ + trackBlock->z, rotation };
                offsets.x = trackBlock->x;
                offsets.y = trackBlock->y;
                elem += offsets.Rotate(originDirection);

                TrackElement* tileElement = MapGetTrackElementAtOfTypeSeq(elem, type, trackBlock->index);
                if (tileElement == nullptr)
                {
                    log_error("Track map element part not found!");
                    return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);
                }

                // track_remove returns here on failure, not sure when this would ever be hit. Only thing I can think of is
                // for when you decrease the map size.
                openrct2_assert(MapGetSurfaceElementAt(elem) != nullptr, "No surface at %d,%d", elem.x >> 5, elem.y >> 5);

                MapInvalidateTileFull(elem);

                // Keep?
                // invalidate_test_results(ride);

                if (tileElement->AsTrack()->HasChain() != setChain)
                {
                    tileElement->AsTrack()->SetHasChain(setChain);
                }
            }

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result TrackSetBrakeClosed(const CoordsXY& loc, int32_t elementIndex, bool isClosed, bool isExecuting)
    {
        TileElement* const trackElement = MapGetNthElementAt(loc, elementIndex);
        if (trackElement == nullptr || trackElement->GetType() != TileElementType::Track)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            trackElement->AsTrack()->SetBrakeClosed(isClosed);

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result TrackSetIndestructible(
        const CoordsXY& loc, int32_t elementIndex, bool isIndestructible, bool isExecuting)
    {
        TileElement* const trackElement = MapGetNthElementAt(loc, elementIndex);
        if (trackElement == nullptr || trackElement->GetType() != TileElementType::Track)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            trackElement->AsTrack()->SetIsIndestructible(isIndestructible);

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result ScenerySetQuarterLocation(
        const CoordsXY& loc, int32_t elementIndex, int32_t quarterIndex, bool isExecuting)
    {
        TileElement* const tileElement = MapGetNthElementAt(loc, elementIndex);
        if (tileElement == nullptr || tileElement->GetType() != TileElementType::SmallScenery)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            // Set quadrant index
            tileElement->AsSmallScenery()->SetSceneryQuadrant(quarterIndex);

            // Update collision
            tileElement->SetOccupiedQuadrants(1 << ((quarterIndex + 2) & 3));

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result ScenerySetQuarterCollision(
        const CoordsXY& loc, int32_t elementIndex, int32_t quarterIndex, bool isExecuting)
    {
        TileElement* const tileElement = MapGetNthElementAt(loc, elementIndex);
        if (tileElement == nullptr || tileElement->GetType() != TileElementType::SmallScenery)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            auto occupiedQuadrants = tileElement->GetOccupiedQuadrants();
            occupiedQuadrants ^= 1 << quarterIndex;
            tileElement->SetOccupiedQuadrants(occupiedQuadrants);

            MapInvalidateTileFull(loc);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    GameActions::Result BannerToggleBlockingEdge(const CoordsXY& loc, int32_t elementIndex, int32_t edgeIndex, bool isExecuting)
    {
        TileElement* const bannerElement = MapGetNthElementAt(loc, elementIndex);
        if (bannerElement == nullptr || bannerElement->GetType() != TileElementType::Banner)
            return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);

        if (isExecuting)
        {
            uint8_t edges = bannerElement->AsBanner()->GetAllowedEdges();
            edges ^= (1 << edgeIndex);
            bannerElement->AsBanner()->SetAllowedEdges(edges);

            if (auto* inspector = GetTileInspectorWithPos(loc); inspector != nullptr)
            {
                inspector->Invalidate();
            }
        }

        return GameActions::Result();
    }

    // NOTE: The pointer is exclusively used to determine the  current selection,
    // do not access the data, points to potentially invalid memory.
    static const TileElement* _highlightedElement = nullptr;

    void SetSelectedElement(const TileElement* elem)
    {
        _highlightedElement = elem;
    }

    bool IsElementSelected(const TileElement* elem)
    {
        return _highlightedElement == elem;
    }
} // namespace OpenRCT2::TileInspector
