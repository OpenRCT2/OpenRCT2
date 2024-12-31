/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WallPlaceAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../object/LargeSceneryEntry.h"
#include "../object/ObjectEntryManager.h"
#include "../object/SmallSceneryEntry.h"
#include "../object/WallSceneryEntry.h"
#include "../ride/Track.h"
#include "../ride/TrackDesign.h"
#include "../world/Banner.h"
#include "../world/ConstructionClearance.h"
#include "../world/MapAnimation.h"
#include "../world/Wall.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"
#include "../world/tile_element/WallElement.h"

using namespace OpenRCT2;
using namespace OpenRCT2::TrackMetaData;

WallPlaceAction::WallPlaceAction(
    ObjectEntryIndex wallType, const CoordsXYZ& loc, uint8_t edge, int32_t primaryColour, int32_t secondaryColour,
    int32_t tertiaryColour)
    : _wallType(wallType)
    , _loc(loc)
    , _edge(edge)
    , _primaryColour(primaryColour)
    , _secondaryColour(secondaryColour)
    , _tertiaryColour(tertiaryColour)
{
}

void WallPlaceAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("object", _wallType);
    visitor.Visit("edge", _edge);
    visitor.Visit("primaryColour", _primaryColour);
    visitor.Visit("secondaryColour", _secondaryColour);
    visitor.Visit("tertiaryColour", _tertiaryColour);
}

uint16_t WallPlaceAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void WallPlaceAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_wallType) << DS_TAG(_loc) << DS_TAG(_edge) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour)
           << DS_TAG(_tertiaryColour);
}

GameActions::Result WallPlaceAction::Query() const
{
    auto res = GameActions::Result();
    res.ErrorTitle = STR_CANT_BUILD_THIS_HERE;
    res.Position = _loc;

    res.Expenditure = ExpenditureType::Landscaping;
    res.Position.x += 16;
    res.Position.y += 16;

    if (_loc.z == 0)
    {
        res.Position.z = TileElementHeight(res.Position);
    }

    if (!LocationValid(_loc))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_BUILD_THIS_HERE, STR_OFF_EDGE_OF_MAP);
    }

    auto& gameState = GetGameState();
    auto mapSizeMax = GetMapSizeMaxXY();
    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !(GetFlags() & GAME_COMMAND_FLAG_TRACK_DESIGN)
        && !gameState.Cheats.sandboxMode)
    {
        if (_loc.z == 0)
        {
            if (!MapIsLocationInPark(_loc))
            {
                return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_BUILD_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
            }
        }
        else if (!MapIsLocationOwned(_loc))
        {
            return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_BUILD_THIS_HERE, STR_LAND_NOT_OWNED_BY_PARK);
        }
    }
    else if (!_trackDesignDrawingPreview && (_loc.x > mapSizeMax.x || _loc.y > mapSizeMax.y))
    {
        LOG_ERROR("Invalid x/y coordinates. x = %d y = %d", _loc.x, _loc.y);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_BUILD_THIS_HERE, STR_OFF_EDGE_OF_MAP);
    }

    if (_edge > 3)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_BUILD_THIS_HERE, STR_NONE);
    }

    uint8_t edgeSlope = 0;
    auto targetHeight = _loc.z;
    if (targetHeight == 0)
    {
        auto* surfaceElement = MapGetSurfaceElementAt(_loc);
        if (surfaceElement == nullptr)
        {
            LOG_ERROR("Surface element not found at %d, %d.", _loc.x, _loc.y);
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_BUILD_THIS_HERE, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
        }
        targetHeight = surfaceElement->GetBaseZ();

        uint8_t slope = surfaceElement->GetSlope();
        edgeSlope = GetWallSlopeFromEdgeSlope(slope, _edge & 3);
        if (edgeSlope & EDGE_SLOPE_ELEVATED)
        {
            targetHeight += 16;
            edgeSlope &= ~EDGE_SLOPE_ELEVATED;
        }
    }

    auto* surfaceElement = MapGetSurfaceElementAt(_loc);
    if (surfaceElement == nullptr)
    {
        LOG_ERROR("Surface element not found at %d, %d.", _loc.x, _loc.y);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_BUILD_THIS_HERE, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
    }

    if (surfaceElement->GetWaterHeight() > 0)
    {
        uint16_t waterHeight = surfaceElement->GetWaterHeight();

        if (targetHeight < waterHeight && !gameState.Cheats.disableClearanceChecks)
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_CANT_BUILD_THIS_HERE, STR_CANT_BUILD_THIS_UNDERWATER);
        }
    }

    if (targetHeight < surfaceElement->GetBaseZ() && !gameState.Cheats.disableClearanceChecks)
    {
        return GameActions::Result(
            GameActions::Status::Disallowed, STR_CANT_BUILD_THIS_HERE, STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
    }

    if (!(edgeSlope & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS)))
    {
        uint8_t newEdge = (_edge + 2) & 3;
        uint8_t newBaseHeight = surfaceElement->BaseHeight;
        newBaseHeight += 2;
        if (surfaceElement->GetSlope() & (1 << newEdge))
        {
            if (targetHeight / 8 < newBaseHeight && !gameState.Cheats.disableClearanceChecks)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_CANT_BUILD_THIS_HERE, STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
            }

            if (surfaceElement->GetSlope() & kTileSlopeDiagonalFlag)
            {
                newEdge = (newEdge - 1) & 3;

                if (surfaceElement->GetSlope() & (1 << newEdge))
                {
                    newEdge = (newEdge + 2) & 3;
                    if (surfaceElement->GetSlope() & (1 << newEdge))
                    {
                        newBaseHeight += 2;
                        if (targetHeight / 8 < newBaseHeight && !gameState.Cheats.disableClearanceChecks)
                        {
                            return GameActions::Result(
                                GameActions::Status::Disallowed, STR_CANT_BUILD_THIS_HERE,
                                STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
                        }
                        newBaseHeight -= 2;
                    }
                }
            }
        }

        newEdge = (_edge + 3) & 3;
        if (surfaceElement->GetSlope() & (1 << newEdge))
        {
            if (targetHeight / 8 < newBaseHeight && !gameState.Cheats.disableClearanceChecks)
            {
                return GameActions::Result(
                    GameActions::Status::Disallowed, STR_CANT_BUILD_THIS_HERE, STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
            }

            if (surfaceElement->GetSlope() & kTileSlopeDiagonalFlag)
            {
                newEdge = (newEdge - 1) & 3;

                if (surfaceElement->GetSlope() & (1 << newEdge))
                {
                    newEdge = (newEdge + 2) & 3;
                    if (surfaceElement->GetSlope() & (1 << newEdge))
                    {
                        newBaseHeight += 2;
                        if (targetHeight / 8 < newBaseHeight && !gameState.Cheats.disableClearanceChecks)
                        {
                            return GameActions::Result(
                                GameActions::Status::Disallowed, STR_CANT_BUILD_THIS_HERE,
                                STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND);
                        }
                    }
                }
            }
        }
    }

    auto* wallEntry = ObjectManager::GetObjectEntry<WallSceneryEntry>(_wallType);

    if (wallEntry == nullptr)
    {
        LOG_ERROR("Wall Type not found %d", _wallType);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_BUILD_THIS_HERE, STR_UNKNOWN_OBJECT_TYPE);
    }

    if (wallEntry->scrolling_mode != SCROLLING_MODE_NONE)
    {
        if (HasReachedBannerLimit())
        {
            LOG_ERROR("No free banners available");
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_BUILD_THIS_HERE, STR_TOO_MANY_BANNERS_IN_GAME);
        }
    }

    uint8_t clearanceHeight = targetHeight / 8;
    if (edgeSlope & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS))
    {
        if (wallEntry->flags & WALL_SCENERY_CANT_BUILD_ON_SLOPE)
        {
            return GameActions::Result(
                GameActions::Status::Disallowed, STR_CANT_BUILD_THIS_HERE, STR_ERR_UNABLE_TO_BUILD_THIS_ON_SLOPE);
        }
        clearanceHeight += 2;
    }
    clearanceHeight += wallEntry->height;

    bool wallAcrossTrack = false;
    if (!(GetFlags() & GAME_COMMAND_FLAG_TRACK_DESIGN) && !gameState.Cheats.disableClearanceChecks)
    {
        auto result = WallCheckObstruction(wallEntry, targetHeight / 8, clearanceHeight, &wallAcrossTrack);
        if (result.Error != GameActions::Status::Ok)
        {
            return result;
        }
    }

    if (!MapCheckCapacityAndReorganise(_loc))
    {
        return GameActions::Result(
            GameActions::Status::NoFreeElements, STR_CANT_BUILD_THIS_HERE, STR_TILE_ELEMENT_LIMIT_REACHED);
    }

    res.Cost = wallEntry->price;

    res.SetData(WallPlaceActionResult{});

    return res;
}

GameActions::Result WallPlaceAction::Execute() const
{
    auto res = GameActions::Result();
    auto& gameState = GetGameState();

    res.ErrorTitle = STR_CANT_BUILD_THIS_HERE;
    res.Position = _loc;

    res.Expenditure = ExpenditureType::Landscaping;
    res.Position.x += 16;
    res.Position.y += 16;

    if (res.Position.z == 0)
    {
        res.Position.z = TileElementHeight(res.Position);
    }

    uint8_t edgeSlope = 0;
    auto targetHeight = _loc.z;
    if (targetHeight == 0)
    {
        auto* surfaceElement = MapGetSurfaceElementAt(_loc);
        if (surfaceElement == nullptr)
        {
            LOG_ERROR("Surface element not found at %d, %d.", _loc.x, _loc.y);
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_BUILD_THIS_HERE, STR_ERR_SURFACE_ELEMENT_NOT_FOUND);
        }
        targetHeight = surfaceElement->GetBaseZ();

        uint8_t slope = surfaceElement->GetSlope();
        edgeSlope = GetWallSlopeFromEdgeSlope(slope, _edge & 3);
        if (edgeSlope & EDGE_SLOPE_ELEVATED)
        {
            targetHeight += 16;
            edgeSlope &= ~EDGE_SLOPE_ELEVATED;
        }
    }
    auto targetLoc = CoordsXYZ(_loc, targetHeight);

    auto* wallEntry = ObjectManager::GetObjectEntry<WallSceneryEntry>(_wallType);

    if (wallEntry == nullptr)
    {
        LOG_ERROR("Wall Type not found %d", _wallType);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_BUILD_THIS_HERE, STR_UNKNOWN_OBJECT_TYPE);
    }

    uint8_t clearanceHeight = targetHeight / kCoordsZStep;
    if (edgeSlope & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS))
    {
        clearanceHeight += 2;
    }
    clearanceHeight += wallEntry->height;

    bool wallAcrossTrack = false;
    if (!(GetFlags() & GAME_COMMAND_FLAG_TRACK_DESIGN) && !gameState.Cheats.disableClearanceChecks)
    {
        auto result = WallCheckObstruction(wallEntry, targetHeight / kCoordsZStep, clearanceHeight, &wallAcrossTrack);
        if (result.Error != GameActions::Status::Ok)
        {
            return result;
        }
    }

    Banner* banner = nullptr;
    if (wallEntry->scrolling_mode != SCROLLING_MODE_NONE)
    {
        banner = CreateBanner();
        if (banner == nullptr)
        {
            LOG_ERROR("No free banners available");
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_CANT_BUILD_THIS_HERE, STR_TOO_MANY_BANNERS_IN_GAME);
        }

        banner->text = {};
        banner->colour = COLOUR_WHITE;
        banner->text_colour = COLOUR_WHITE;
        banner->flags = BANNER_FLAG_IS_WALL;
        banner->type = 0; // Banner must be deleted after this point in an early return
        banner->position = TileCoordsXY(_loc);

        RideId rideIndex = BannerGetClosestRideIndex(targetLoc);
        if (!rideIndex.IsNull())
        {
            banner->ride_index = rideIndex;
            banner->flags |= BANNER_FLAG_LINKED_TO_RIDE;
        }
    }

    auto* wallElement = TileElementInsert<WallElement>(targetLoc, 0b0000);
    if (wallElement == nullptr)
    {
        return GameActions::Result(
            GameActions::Status::NoFreeElements, STR_CANT_POSITION_THIS_HERE, STR_TILE_ELEMENT_LIMIT_REACHED);
    }

    wallElement->ClearanceHeight = clearanceHeight;
    wallElement->SetDirection(_edge);
    wallElement->SetSlope(edgeSlope);

    wallElement->SetPrimaryColour(_primaryColour);
    wallElement->SetSecondaryColour(_secondaryColour);
    wallElement->SetAcrossTrack(wallAcrossTrack);

    wallElement->SetEntryIndex(_wallType);
    wallElement->SetBannerIndex(banner != nullptr ? banner->id : BannerIndex::GetNull());

    if (wallEntry->flags & WALL_SCENERY_HAS_TERTIARY_COLOUR)
    {
        wallElement->SetTertiaryColour(_tertiaryColour);
    }

    wallElement->SetGhost(GetFlags() & GAME_COMMAND_FLAG_GHOST);

    MapAnimationCreate(MAP_ANIMATION_TYPE_WALL, targetLoc);
    MapInvalidateTileZoom1({ _loc, wallElement->GetBaseZ(), wallElement->GetBaseZ() + 72 });

    res.Cost = wallEntry->price;

    const auto bannerId = banner != nullptr ? banner->id : BannerIndex::GetNull();
    res.SetData(WallPlaceActionResult{ wallElement->GetBaseZ(), bannerId });

    return res;
}

/**
 *
 *  rct2: 0x006E5CBA
 */
bool WallPlaceAction::WallCheckObstructionWithTrack(
    const WallSceneryEntry* wall, int32_t z0, TrackElement* trackElement, bool* wallAcrossTrack) const
{
    OpenRCT2::TrackElemType trackType = trackElement->GetTrackType();

    using namespace OpenRCT2::TrackMetaData;
    const auto& ted = GetTrackElementDescriptor(trackType);
    int32_t sequence = trackElement->GetSequenceIndex();
    int32_t direction = (_edge - trackElement->GetDirection()) & kTileElementDirectionMask;
    auto ride = GetRide(trackElement->GetRideIndex());
    if (ride == nullptr)
    {
        return false;
    }

    if (TrackIsAllowedWallEdges(ride->type, trackType, sequence, direction))
    {
        return true;
    }

    if (!(wall->flags & WALL_SCENERY_IS_DOOR))
    {
        return false;
    }

    if (!ride->GetRideTypeDescriptor().HasFlag(RtdFlag::allowDoorsOnTrack))
    {
        return false;
    }

    *wallAcrossTrack = true;
    if (z0 & 1)
    {
        return false;
    }

    int32_t z;
    if (sequence == 0)
    {
        if (ted.sequences[0].flags & TRACK_SEQUENCE_FLAG_DISALLOW_DOORS)
        {
            return false;
        }

        if (ted.definition.rollStart == TrackRoll::None)
        {
            if (!(ted.coordinates.rotationBegin & 4))
            {
                direction = DirectionReverse(trackElement->GetDirection());
                if (direction == _edge)
                {
                    z = ted.coordinates.zBegin;
                    z = trackElement->BaseHeight + ((z - ted.sequences[sequence].clearance.z) * 8);
                    if (z == z0)
                    {
                        return true;
                    }
                }
            }
        }
    }

    bool isLastInSequence = (sequence + 1) == ted.numSequences;
    if (!isLastInSequence)
    {
        return false;
    }

    if (ted.definition.rollEnd != TrackRoll::None)
    {
        return false;
    }

    direction = ted.coordinates.rotationEnd;
    if (direction & 4)
    {
        return false;
    }

    direction = (trackElement->GetDirection() + ted.coordinates.rotationEnd) & kTileElementDirectionMask;
    if (direction != _edge)
    {
        return false;
    }

    z = ted.coordinates.zEnd;
    z = trackElement->BaseHeight + ((z - ted.sequences[sequence].clearance.z) * kCoordsZStep);
    return z == z0;
}

/**
 *
 *  rct2: 0x006E5C1A
 */
GameActions::Result WallPlaceAction::WallCheckObstruction(
    const WallSceneryEntry* wall, int32_t z0, int32_t z1, bool* wallAcrossTrack) const
{
    *wallAcrossTrack = false;
    if (MapIsLocationAtEdge(_loc))
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_BUILD_THIS_HERE, STR_OFF_EDGE_OF_MAP);
    }

    TileElement* tileElement = MapGetFirstElementAt(_loc);
    do
    {
        if (tileElement == nullptr)
            break;
        auto elementType = tileElement->GetType();
        if (elementType == TileElementType::Surface)
            continue;
        if (tileElement->IsGhost())
            continue;
        if (z0 >= tileElement->ClearanceHeight)
            continue;
        if (z1 <= tileElement->BaseHeight)
            continue;
        if (elementType == TileElementType::Wall)
        {
            int32_t direction = tileElement->GetDirection();
            if (_edge == direction)
            {
                auto res = GameActions::Result(GameActions::Status::NoClearance, STR_CANT_BUILD_THIS_HERE, STR_NONE);
                MapGetObstructionErrorText(tileElement, res);
                return res;
            }
            continue;
        }
        if (tileElement->GetOccupiedQuadrants() == 0)
            continue;
        auto res = GameActions::Result(GameActions::Status::NoClearance, STR_CANT_BUILD_THIS_HERE, STR_NONE);
        switch (elementType)
        {
            case TileElementType::Entrance:
                MapGetObstructionErrorText(tileElement, res);
                return res;
            case TileElementType::Path:
                if (tileElement->AsPath()->GetEdges() & (1 << _edge))
                {
                    MapGetObstructionErrorText(tileElement, res);
                    return res;
                }
                break;
            case TileElementType::LargeScenery:
            {
                const auto* largeSceneryElement = tileElement->AsLargeScenery();
                const auto* sceneryEntry = largeSceneryElement->GetEntry();

                // If there is no entry, assume the object is not in the way.
                if (sceneryEntry == nullptr)
                    break;

                auto sequence = largeSceneryElement->GetSequenceIndex();
                const LargeSceneryTile& tile = sceneryEntry->tiles[sequence];

                int32_t direction = ((_edge - tileElement->GetDirection()) & kTileElementDirectionMask);
                if (!(tile.walls & (1 << direction)))
                {
                    MapGetObstructionErrorText(tileElement, res);
                    return res;
                }
                break;
            }
            case TileElementType::SmallScenery:
            {
                auto sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
                if (sceneryEntry != nullptr && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_NO_WALLS))
                {
                    MapGetObstructionErrorText(tileElement, res);
                    return res;
                }
                break;
            }
            case TileElementType::Track:
                if (!WallCheckObstructionWithTrack(wall, z0, tileElement->AsTrack(), wallAcrossTrack))
                {
                    MapGetObstructionErrorText(tileElement, res);
                    return res;
                }
                break;
            default:
                break;
        }
    } while (!(tileElement++)->IsLastForTile());

    return GameActions::Result();
}

bool WallPlaceAction::TrackIsAllowedWallEdges(
    ride_type_t rideType, OpenRCT2::TrackElemType trackType, uint8_t trackSequence, uint8_t direction)
{
    if (!GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::noWallsAroundTrack))
    {
        const auto& ted = GetTrackElementDescriptor(trackType);
        if (ted.sequences[trackSequence].allowedWallEdges & (1 << direction))
        {
            return true;
        }
    }
    return false;
}
