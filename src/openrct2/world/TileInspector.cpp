/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TileInspector.h"

#include "../Diagnostic.h"
#include "../actions/GameAction.h"
#include "../object/LargeSceneryEntry.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../ui/WindowManager.h"
#include "../windows/TileInspectorGlobals.h"
#include "Banner.h"
#include "Footpath.h"
#include "Location.hpp"
#include "Map.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Scenery.h"
#include "tile_element/BannerElement.h"
#include "tile_element/EntranceElement.h"
#include "tile_element/LargeSceneryElement.h"
#include "tile_element/PathElement.h"
#include "tile_element/Slope.h"
#include "tile_element/SmallSceneryElement.h"
#include "tile_element/SurfaceElement.h"
#include "tile_element/TrackElement.h"
#include "tile_element/WallElement.h"

#include <optional>

TileCoordsXY windowTileInspectorTile;
int32_t windowTileInspectorElementCount = 0;
int32_t windowTileInspectorSelectedIndex = -1;

using namespace OpenRCT2::TrackMetaData;

namespace OpenRCT2::TileInspector
{
    static GameActions::Result SwapTileElements(const CoordsXY& loc, int16_t first, int16_t second)
    {
        TileElement* const firstElement = MapGetNthElementAt(loc, first);
        TileElement* const secondElement = MapGetNthElementAt(loc, second);

        if (firstElement == nullptr)
        {
            LOG_ERROR("First element is out of range for the tile");
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TILE_ELEMENT_NOT_FOUND);
        }
        if (secondElement == nullptr)
        {
            LOG_ERROR("Second element is out of range for the tile");
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TILE_ELEMENT_NOT_FOUND);
        }
        if (firstElement == secondElement)
        {
            LOG_ERROR("Can't swap the element with itself");
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_CANT_SWAP_TILE_ELEMENT_WITH_ITSELF);
        }

        // Swap their memory
        std::swap(*firstElement, *secondElement);

        // Swap the 'last map element for tile' flag if either one of them was last
        if ((firstElement)->IsLastForTile() || (secondElement)->IsLastForTile())
        {
            firstElement->SetLastForTile(!firstElement->IsLastForTile());
            secondElement->SetLastForTile(!secondElement->IsLastForTile());
        }

        return GameActions::Result();
    }

    static bool IsTileSelected(const CoordsXY& loc)
    {
        // Return true for everyone who has the window open and tile selected
        auto* windowMgr = Ui::GetWindowManager();
        auto* window = windowMgr->FindByClass(WindowClass::TileInspector);
        return window != nullptr && loc == windowTileInspectorTile.ToCoordsXY();
    }

    static int32_t NumLargeScenerySequences(const CoordsXY& loc, const LargeSceneryElement* const largeScenery)
    {
        const auto* const largeEntry = largeScenery->GetEntry();
        const auto direction = largeScenery->GetDirection();
        const auto sequenceIndex = largeScenery->GetSequenceIndex();
        const auto& tiles = largeEntry->tiles;
        const auto& initialTile = tiles[sequenceIndex];
        const auto rotatedFirstTile = CoordsXYZ{
            CoordsXY{ initialTile.offset }.Rotate(direction),
            initialTile.offset.z,
        };

        const auto firstTile = CoordsXYZ{ loc, largeScenery->GetBaseZ() } - rotatedFirstTile;
        auto numFoundElements = 0;
        for (auto& tile : tiles)
        {
            const auto rotatedCurrentTile = CoordsXYZ{ CoordsXY{ tile.offset }.Rotate(direction), tile.offset.z };

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

                    if (tileElement->AsLargeScenery()->GetSequenceIndex() != tile.index)
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
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TILE_ELEMENT_NOT_FOUND);

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

            if (IsTileSelected(loc))
            {
                // Update the window
                windowTileInspectorElementCount--;

                if (windowTileInspectorSelectedIndex >= elementIndex)
                {
                    windowTileInspectorSelectedIndex--;
                }
            }
        }

        return GameActions::Result();
    }

    GameActions::Result SwapElementsAt(const CoordsXY& loc, int16_t first, int16_t second, bool isExecuting)
    {
        if (isExecuting)
        {
            auto res = SwapTileElements(loc, first, second);
            if (res.Error != GameActions::Status::Ok)
            {
                return res;
            }

            if (IsTileSelected(loc))
            {
                // If one of them was selected, update selected list item
                if (windowTileInspectorSelectedIndex == first)
                    windowTileInspectorSelectedIndex = second;
                else if (windowTileInspectorSelectedIndex == second)
                    windowTileInspectorSelectedIndex = first;
            }
        }

        return GameActions::Result();
    }

    GameActions::Result RotateElementAt(const CoordsXY& loc, int32_t elementIndex, bool isExecuting)
    {
        TileElement* const tileElement = MapGetNthElementAt(loc, elementIndex);
        if (tileElement == nullptr)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TILE_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            uint8_t newRotation, pathEdges, pathCorners;

            switch (tileElement->GetType())
            {
                case TileElementType::Path:
                    if (tileElement->AsPath()->IsSloped())
                    {
                        newRotation = (tileElement->AsPath()->GetSlopeDirection() + 1) & kTileElementDirectionMask;
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
                        uint8_t z = tileElement->BaseHeight;

                        // Make sure this is the correct entrance or exit
                        if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE && entrance.x == loc.x / kCoordsXYStep
                            && entrance.y == loc.y / kCoordsXYStep && entrance.z == z)
                        {
                            station.Entrance = { entrance, newRotation };
                        }
                        else if (
                            entranceType == ENTRANCE_TYPE_RIDE_EXIT && exit.x == loc.x / kCoordsXYStep
                            && exit.y == loc.y / kCoordsXYStep && exit.z == z)
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
        }

        return GameActions::Result();
    }

    GameActions::Result ToggleInvisibilityOfElementAt(const CoordsXY& loc, int32_t elementIndex, bool isExecuting)
    {
        TileElement* const tileElement = MapGetNthElementAt(loc, elementIndex);
        if (tileElement == nullptr)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TILE_ELEMENT_NOT_FOUND);

        if (!isExecuting)
        {
            return GameActions::Result();
        }

        bool currentlyInvisible = tileElement->IsInvisible();
        tileElement->SetInvisible(!currentlyInvisible);

        return GameActions::Result();
    }

    GameActions::Result PasteElementAt(const CoordsXY& loc, TileElement element, Banner banner, bool isExecuting)
    {
        // Make sure there is enough space for the new element
        if (!MapCheckCapacityAndReorganise(loc))
        {
            return GameActions::Result(GameActions::Status::NoFreeElements, STR_CANT_PASTE, STR_TILE_ELEMENT_LIMIT_REACHED);
        }

        auto tileLoc = TileCoordsXY(loc);

        if (isExecuting)
        {
            // Check if the element to be pasted has a banner
            if (element.GetBannerIndex() != BannerIndex::GetNull())
            {
                // The element to be pasted has a banner - make a copy of it from the banner provided
                auto newBanner = CreateBanner();
                if (newBanner == nullptr)
                {
                    LOG_ERROR("No free banners available");
                    return GameActions::Result(GameActions::Status::Unknown, STR_TOO_MANY_BANNERS_IN_GAME, kStringIdNone);
                }
                auto newId = newBanner->id;
                // Copy the banners style
                *newBanner = banner;
                // Reset the location to the paste location
                newBanner->position = tileLoc;
                newBanner->id = newId;

                // If the linked ride has been destroyed since copying, unlink the pasted banner
                if (newBanner->flags & BANNER_FLAG_LINKED_TO_RIDE && GetRide(newBanner->ride_index) == nullptr)
                {
                    newBanner->flags &= ~BANNER_FLAG_LINKED_TO_RIDE;
                    newBanner->ride_index = RideId::GetNull();
                }

                // Use the new banner index
                element.SetBannerIndex(newBanner->id);
            }

            // The occupiedQuadrants will be automatically set when the element is copied over, so it's not necessary to set
            // them correctly _here_.
            TileElement* const pastedElement = TileElementInsert({ loc, element.GetBaseZ() }, 0b0000, TileElementType::Surface);

            bool lastForTile = pastedElement->IsLastForTile();
            *pastedElement = element;
            pastedElement->SetLastForTile(lastForTile);

            MapAnimationAutoCreateAtTileElement(tileLoc, pastedElement);

            if (IsTileSelected(loc))
            {
                windowTileInspectorElementCount++;

                // Select new element if there was none selected already
                int16_t newIndex = static_cast<int16_t>((pastedElement - MapGetFirstElementAt(loc)));
                if (windowTileInspectorSelectedIndex == -1)
                    windowTileInspectorSelectedIndex = newIndex;
                else if (windowTileInspectorSelectedIndex >= newIndex)
                    windowTileInspectorSelectedIndex++;
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
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TILE_ELEMENT_NOT_FOUND);

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
                       && (otherElement->BaseHeight > currentElement->BaseHeight
                           || (otherElement->BaseHeight == currentElement->BaseHeight
                               && otherElement->ClearanceHeight > currentElement->ClearanceHeight)))
                {
                    auto res = SwapTileElements(loc, currentId - 1, currentId);
                    if (res.Error != GameActions::Status::Ok)
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

            if (IsTileSelected(loc))
            {
                // Deselect tile for clients who had it selected
                windowTileInspectorSelectedIndex = -1;
            }
        }

        return GameActions::Result();
    }

    static GameActions::Result ValidateTileHeight(TileElement* const tileElement, int8_t heightOffset)
    {
        int16_t newBaseHeight = static_cast<int16_t>(tileElement->BaseHeight + heightOffset);
        int16_t newClearanceHeight = static_cast<int16_t>(tileElement->ClearanceHeight + heightOffset);
        if (newBaseHeight < 0)
        {
            return GameActions::Result(GameActions::Status::TooLow, STR_CANT_LOWER_ELEMENT_HERE, STR_TOO_LOW);
        }
        if (newBaseHeight > kMaxTileElementHeight)
        {
            return GameActions::Result(GameActions::Status::TooHigh, STR_CANT_RAISE_ELEMENT_HERE, STR_TOO_HIGH);
        }
        if (newClearanceHeight < 0)
        {
            return GameActions::Result(GameActions::Status::NoClearance, STR_CANT_LOWER_ELEMENT_HERE, STR_NO_CLEARANCE);
        }
        if (newClearanceHeight > kMaxTileElementHeight)
        {
            return GameActions::Result(GameActions::Status::NoClearance, STR_CANT_RAISE_ELEMENT_HERE, STR_NO_CLEARANCE);
        }
        return GameActions::Result();
    }

    GameActions::Result AnyBaseHeightOffset(const CoordsXY& loc, int16_t elementIndex, int8_t heightOffset, bool isExecuting)
    {
        TileElement* const tileElement = MapGetNthElementAt(loc, elementIndex);
        if (tileElement == nullptr)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TILE_ELEMENT_NOT_FOUND);

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
                        uint8_t z = tileElement->BaseHeight;

                        // Make sure this is the correct entrance or exit
                        if (entranceType == ENTRANCE_TYPE_RIDE_ENTRANCE && entranceLoc == TileCoordsXYZ{ loc, z })
                            station.Entrance = { entranceLoc, z + heightOffset, entranceLoc.direction };
                        else if (entranceType == ENTRANCE_TYPE_RIDE_EXIT && exitLoc == TileCoordsXYZ{ loc, z })
                            station.Exit = { exitLoc, z + heightOffset, exitLoc.direction };
                    }
                }
            }

            tileElement->BaseHeight += heightOffset;
            tileElement->ClearanceHeight += heightOffset;
        }

        return GameActions::Result();
    }

    GameActions::Result SurfaceShowParkFences(const CoordsXY& loc, bool showFences, bool isExecuting)
    {
        auto* const surfaceelement = MapGetSurfaceElementAt(loc);

        // No surface element on tile
        if (surfaceelement == nullptr)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            if (!showFences)
                surfaceelement->SetParkFences(0);
            else
                Park::UpdateFences(loc);
        }

        return GameActions::Result();
    }

    GameActions::Result SurfaceToggleCorner(const CoordsXY& loc, int32_t cornerIndex, bool isExecuting)
    {
        auto* const surfaceElement = MapGetSurfaceElementAt(loc);

        // No surface element on tile
        if (surfaceElement == nullptr)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            const uint8_t originalSlope = surfaceElement->GetSlope();
            uint8_t newSlope = surfaceElement->GetSlope() ^ (1 << cornerIndex);

            // All corners are raised
            if ((newSlope & kTileSlopeRaisedCornersMask) == kTileSlopeRaisedCornersMask)
            {
                newSlope = kTileSlopeFlat;
                if ((originalSlope & kTileSlopeDiagonalFlag) != 0)
                {
                    switch (originalSlope & kTileSlopeRaisedCornersMask)
                    {
                        case kTileSlopeSCornerDown:
                            newSlope |= kTileSlopeNCornerUp;
                            break;
                        case kTileSlopeWCornerDown:
                            newSlope |= kTileSlopeECornerUp;
                            break;
                        case kTileSlopeNCornerDown:
                            newSlope |= kTileSlopeSCornerUp;
                            break;
                        case kTileSlopeECornerDown:
                            newSlope |= kTileSlopeWCornerUp;
                            break;
                    }
                }

                surfaceElement->BaseHeight += 2;
                surfaceElement->ClearanceHeight = surfaceElement->BaseHeight;
            }

            surfaceElement->SetSlope(newSlope);
        }

        return GameActions::Result();
    }

    GameActions::Result SurfaceToggleDiagonal(const CoordsXY& loc, bool isExecuting)
    {
        auto* const surfaceElement = MapGetSurfaceElementAt(loc);

        // No surface element on tile
        if (surfaceElement == nullptr)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            uint8_t newSlope = surfaceElement->GetSlope() ^ kTileSlopeDiagonalFlag;
            surfaceElement->SetSlope(newSlope);
        }

        return GameActions::Result();
    }

    GameActions::Result PathSetSloped(const CoordsXY& loc, int32_t elementIndex, bool sloped, bool isExecuting)
    {
        TileElement* const pathElement = MapGetNthElementAt(loc, elementIndex);
        if (pathElement == nullptr || pathElement->GetType() != TileElementType::Path)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_PATH_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            pathElement->AsPath()->SetSloped(sloped);
        }

        return GameActions::Result();
    }

    GameActions::Result PathSetJunctionRailings(
        const CoordsXY& loc, int32_t elementIndex, bool hasJunctionRailings, bool isExecuting)
    {
        TileElement* const pathElement = MapGetNthElementAt(loc, elementIndex);
        if (pathElement == nullptr || pathElement->GetType() != TileElementType::Path)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_PATH_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            pathElement->AsPath()->SetJunctionRailings(hasJunctionRailings);
        }

        return GameActions::Result();
    }

    GameActions::Result PathSetBroken(const CoordsXY& loc, int32_t elementIndex, bool broken, bool isExecuting)
    {
        TileElement* const pathElement = MapGetNthElementAt(loc, elementIndex);
        if (pathElement == nullptr || pathElement->GetType() != TileElementType::Path)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_PATH_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            pathElement->AsPath()->SetIsBroken(broken);
        }

        return GameActions::Result();
    }

    GameActions::Result PathToggleEdge(const CoordsXY& loc, int32_t elementIndex, int32_t edgeIndex, bool isExecuting)
    {
        TileElement* const pathElement = MapGetNthElementAt(loc, elementIndex);
        if (pathElement == nullptr || pathElement->GetType() != TileElementType::Path)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_PATH_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            uint8_t newEdges = pathElement->AsPath()->GetEdgesAndCorners() ^ (1 << edgeIndex);
            pathElement->AsPath()->SetEdgesAndCorners(newEdges);
        }

        return GameActions::Result();
    }

    GameActions::Result EntranceMakeUsable(const CoordsXY& loc, int32_t elementIndex, bool isExecuting)
    {
        TileElement* const entranceElement = MapGetNthElementAt(loc, elementIndex);
        if (entranceElement == nullptr || entranceElement->GetType() != TileElementType::Entrance)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_ENTRANCE_ELEMENT_NOT_FOUND);

        auto ride = GetRide(entranceElement->AsEntrance()->GetRideIndex());
        if (ride == nullptr)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_NOT_FOUND);

        if (isExecuting)
        {
            auto stationIndex = entranceElement->AsEntrance()->GetStationIndex();
            auto& station = ride->GetStation(stationIndex);

            switch (entranceElement->AsEntrance()->GetEntranceType())
            {
                case ENTRANCE_TYPE_RIDE_ENTRANCE:
                    station.Entrance = { loc, entranceElement->BaseHeight, entranceElement->GetDirection() };
                    break;
                case ENTRANCE_TYPE_RIDE_EXIT:
                    station.Exit = { loc, entranceElement->BaseHeight, entranceElement->GetDirection() };
                    break;
            }
        }

        return GameActions::Result();
    }

    GameActions::Result WallSetSlope(const CoordsXY& loc, int32_t elementIndex, int32_t slopeValue, bool isExecuting)
    {
        TileElement* const wallElement = MapGetNthElementAt(loc, elementIndex);
        if (wallElement == nullptr || wallElement->GetType() != TileElementType::Wall)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_WALL_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            // Set new slope value
            wallElement->AsWall()->SetSlope(slopeValue);
        }

        return GameActions::Result();
    }

    GameActions::Result WallAnimationFrameOffset(
        const CoordsXY& loc, int16_t elementIndex, int8_t animationFrameOffset, bool isExecuting)
    {
        TileElement* const wallElement = MapGetNthElementAt(loc, elementIndex);
        if (wallElement == nullptr || wallElement->GetType() != TileElementType::Wall)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_WALL_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            uint8_t animationFrame = wallElement->AsWall()->GetAnimationFrame();
            wallElement->AsWall()->SetAnimationFrame(animationFrame + animationFrameOffset);
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
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TRACK_ELEMENT_NOT_FOUND);

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
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_NOT_FOUND);

            const auto& ted = GetTrackElementDescriptor(type);
            auto sequenceIndex = trackElement->AsTrack()->GetSequenceIndex();
            if (sequenceIndex >= ted.numSequences)
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TRACK_BLOCK_NOT_FOUND);

            const auto& trackBlock = ted.sequences[sequenceIndex].clearance;
            uint8_t originDirection = trackElement->GetDirection();
            CoordsXY offsets = { trackBlock.x, trackBlock.y };
            CoordsXY coords = { originX, originY };
            coords += offsets.Rotate(DirectionReverse(originDirection));

            originX = static_cast<int16_t>(coords.x);
            originY = static_cast<int16_t>(coords.y);
            originZ -= trackBlock.z;

            for (uint8_t i = 0; i < ted.numSequences; i++)
            {
                const auto& trackBlock2 = ted.sequences[i].clearance;
                CoordsXYZD elem = { originX, originY, originZ + trackBlock2.z, rotation };
                offsets.x = trackBlock2.x;
                offsets.y = trackBlock2.y;
                elem += offsets.Rotate(originDirection);

                TrackElement* nextTrackElement = MapGetTrackElementAtOfTypeSeq(elem, type, i);
                if (nextTrackElement == nullptr)
                {
                    LOG_ERROR("Track map element part not found!");
                    return GameActions::Result(
                        GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TRACK_ELEMENT_NOT_FOUND);
                }

                // track_remove returns here on failure, not sure when this would ever be hit. Only thing I can think of is
                // for when you decrease the map size.
                Guard::Assert(MapGetSurfaceElementAt(elem) != nullptr, "No surface at %d,%d", elem.x >> 5, elem.y >> 5);

                // Keep?
                // invalidate_test_results(ride);

                nextTrackElement->BaseHeight += offset;
                nextTrackElement->ClearanceHeight += offset;
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
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TRACK_ELEMENT_NOT_FOUND);

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
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_NOT_FOUND);

            const auto& ted = GetTrackElementDescriptor(type);
            auto sequenceIndex = trackElement->AsTrack()->GetSequenceIndex();
            if (sequenceIndex >= ted.numSequences)
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TRACK_BLOCK_NOT_FOUND);

            const auto& trackBlock = ted.sequences[sequenceIndex].clearance;
            uint8_t originDirection = trackElement->GetDirection();
            CoordsXY offsets = { trackBlock.x, trackBlock.y };
            CoordsXY coords = { originX, originY };
            coords += offsets.Rotate(DirectionReverse(originDirection));

            originX = static_cast<int16_t>(coords.x);
            originY = static_cast<int16_t>(coords.y);
            originZ -= trackBlock.z;

            for (uint8_t i = 0; i < ted.numSequences; i++)
            {
                const auto& trackBlock2 = ted.sequences[i].clearance;
                CoordsXYZD elem = { originX, originY, originZ + trackBlock2.z, rotation };
                offsets.x = trackBlock2.x;
                offsets.y = trackBlock2.y;
                elem += offsets.Rotate(originDirection);

                TrackElement* nextTrackElement = MapGetTrackElementAtOfTypeSeq(elem, type, i);
                if (nextTrackElement == nullptr)
                {
                    LOG_ERROR("Track map element part not found!");
                    return GameActions::Result(
                        GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TRACK_ELEMENT_NOT_FOUND);
                }

                // track_remove returns here on failure, not sure when this would ever be hit. Only thing I can think of is
                // for when you decrease the map size.
                Guard::Assert(MapGetSurfaceElementAt(elem) != nullptr, "No surface at %d,%d", elem.x >> 5, elem.y >> 5);

                // Keep?
                // invalidate_test_results(ride);

                if (nextTrackElement->AsTrack()->HasChain() != setChain)
                {
                    nextTrackElement->AsTrack()->SetHasChain(setChain);
                }
            }
        }

        return GameActions::Result();
    }

    GameActions::Result TrackSetBrakeClosed(const CoordsXY& loc, int32_t elementIndex, bool isClosed, bool isExecuting)
    {
        TileElement* const trackElement = MapGetNthElementAt(loc, elementIndex);
        if (trackElement == nullptr || trackElement->GetType() != TileElementType::Track)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TRACK_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            trackElement->AsTrack()->SetBrakeClosed(isClosed);
        }

        return GameActions::Result();
    }

    GameActions::Result TrackSetIndestructible(
        const CoordsXY& loc, int32_t elementIndex, bool isIndestructible, bool isExecuting)
    {
        TileElement* const trackElement = MapGetNthElementAt(loc, elementIndex);
        if (trackElement == nullptr || trackElement->GetType() != TileElementType::Track)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TRACK_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            trackElement->AsTrack()->SetIsIndestructible(isIndestructible);
        }

        return GameActions::Result();
    }

    GameActions::Result ScenerySetQuarterLocation(
        const CoordsXY& loc, int32_t elementIndex, int32_t quarterIndex, bool isExecuting)
    {
        TileElement* const tileElement = MapGetNthElementAt(loc, elementIndex);
        if (tileElement == nullptr || tileElement->GetType() != TileElementType::SmallScenery)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TILE_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            // Set quadrant index
            tileElement->AsSmallScenery()->SetSceneryQuadrant(quarterIndex);

            // Update collision
            tileElement->SetOccupiedQuadrants(1 << ((quarterIndex + 2) & 3));
        }

        return GameActions::Result();
    }

    GameActions::Result ScenerySetQuarterCollision(
        const CoordsXY& loc, int32_t elementIndex, int32_t quarterIndex, bool isExecuting)
    {
        TileElement* const tileElement = MapGetNthElementAt(loc, elementIndex);
        if (tileElement == nullptr || tileElement->GetType() != TileElementType::SmallScenery)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_TILE_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            auto occupiedQuadrants = tileElement->GetOccupiedQuadrants();
            occupiedQuadrants ^= 1 << quarterIndex;
            tileElement->SetOccupiedQuadrants(occupiedQuadrants);
        }

        return GameActions::Result();
    }

    GameActions::Result BannerToggleBlockingEdge(const CoordsXY& loc, int32_t elementIndex, int32_t edgeIndex, bool isExecuting)
    {
        TileElement* const bannerElement = MapGetNthElementAt(loc, elementIndex);
        if (bannerElement == nullptr || bannerElement->GetType() != TileElementType::Banner)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_BANNER_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            uint8_t edges = bannerElement->AsBanner()->GetAllowedEdges();
            edges ^= (1 << edgeIndex);
            bannerElement->AsBanner()->SetAllowedEdges(edges);
        }

        return GameActions::Result();
    }

    GameActions::Result WallSetAnimationIsBackwards(const CoordsXY& loc, int32_t elementIndex, bool backwards, bool isExecuting)
    {
        TileElement* const wallElement = MapGetNthElementAt(loc, elementIndex);
        if (wallElement == nullptr || wallElement->GetType() != TileElementType::Wall)
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_WALL_ELEMENT_NOT_FOUND);

        if (isExecuting)
        {
            wallElement->AsWall()->SetAnimationIsBackwards(backwards);
        }

        return GameActions::Result();
    }

    TileElement* GetSelectedElement()
    {
        if (windowTileInspectorSelectedIndex == -1)
        {
            return nullptr;
        }
        Guard::Assert(
            windowTileInspectorSelectedIndex >= 0 && windowTileInspectorSelectedIndex < windowTileInspectorElementCount,
            "Selected list item out of range");
        return MapGetNthElementAt(windowTileInspectorTile.ToCoordsXY(), windowTileInspectorSelectedIndex);
    }
} // namespace OpenRCT2::TileInspector
