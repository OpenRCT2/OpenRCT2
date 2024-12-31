/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../audio/audio.h"
#include "../core/SawyerCoding.h"
#include "../interface/Viewport.h"
#include "../localisation/StringIds.h"
#include "../object/FootpathObject.h"
#include "../object/FootpathRailingsObject.h"
#include "../object/FootpathSurfaceObject.h"
#include "../object/LargeSceneryObject.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../rct2/RCT2.h"
#include "../windows/Intent.h"
#include "../world/Footpath.h"
#include "../world/Scenery.h"
#include "../world/tile_element/BannerElement.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "../world/tile_element/WallElement.h"
#include "RideData.h"
#include "Station.h"
#include "Track.h"
#include "TrackData.h"
#include "TrackDesign.h"
#include "TrackDesignRepository.h"

#include <cassert>

using namespace OpenRCT2;

constexpr size_t TRACK_MAX_SAVED_TILE_ELEMENTS = 1500;
constexpr int32_t TRACK_NEARBY_SCENERY_DISTANCE = 1;

bool gTrackDesignSaveMode = false;
RideId gTrackDesignSaveRideIndex = RideId::GetNull();

std::vector<const TileElement*> _trackSavedTileElements;
std::vector<TrackDesignSceneryElement> _trackSavedTileElementsDesc;

struct TrackDesignAddStatus
{
    bool IsSuccess{};
    StringId Message{};

    static TrackDesignAddStatus Success()
    {
        return { true, StringId() };
    }

    static TrackDesignAddStatus Fail(StringId message)
    {
        return { false, message };
    }
};

static bool TrackDesignSaveShouldSelectSceneryAround(RideId rideIndex, TileElement* tileElement);
static void TrackDesignSaveShouldSelectNearbySceneryForTile(RideId rideIndex, int32_t cx, int32_t cy);
static TrackDesignAddStatus TrackDesignSaveAddTileElement(const CoordsXY& loc, TileElement* tileElement);
static void TrackDesignSaveRemoveTileElement(const CoordsXY& loc, TileElement* tileElement);

void TrackDesignSaveInit()
{
    _trackSavedTileElements.clear();
    _trackSavedTileElementsDesc.clear();
}

/**
 *
 *  rct2: 0x006D2B07
 */
void TrackDesignSaveSelectTileElement(
    ViewportInteractionItem interactionType, const CoordsXY& loc, TileElement* tileElement, bool collect)
{
    if (TrackDesignSaveContainsTileElement(tileElement))
    {
        if (!collect)
        {
            TrackDesignSaveRemoveTileElement(loc, tileElement);
        }
    }
    else
    {
        if (collect)
        {
            auto result = TrackDesignSaveAddTileElement(loc, tileElement);
            if (!result.IsSuccess)
            {
                ContextShowError(STR_SAVE_TRACK_SCENERY_UNABLE_TO_SELECT_ADDITIONAL_ITEM_OF_SCENERY, result.Message, {});
            }
        }
    }
}

/**
 *
 *  rct2: 0x006D303D
 */
void TrackDesignSaveSelectNearbyScenery(RideId rideIndex)
{
    TileElementIterator it;
    TileElementIteratorBegin(&it);
    do
    {
        if (TrackDesignSaveShouldSelectSceneryAround(rideIndex, it.element))
        {
            TrackDesignSaveShouldSelectNearbySceneryForTile(rideIndex, it.x, it.y);
        }
    } while (TileElementIteratorNext(&it));

    GfxInvalidateScreen();
}

/**
 *
 *  rct2: 0x006D3026
 */
void TrackDesignSaveResetScenery()
{
    TrackDesignSaveInit();
    GfxInvalidateScreen();
}

bool TrackDesignSaveContainsTileElement(const TileElement* tileElement)
{
    for (auto& tile : _trackSavedTileElements)
    {
        if (tile == tileElement)
        {
            return true;
        }
    }
    return false;
}

static size_t TrackDesignSaveGetTotalElementCount(TileElement* tileElement)
{
    switch (tileElement->GetType())
    {
        case TileElementType::Path:
        case TileElementType::SmallScenery:
        case TileElementType::Wall:
            return 1;

        case TileElementType::LargeScenery:
        {
            auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            return sceneryEntry->tiles.size();
        }
        default:
            return 0;
    }
}

/**
 *
 *  rct2: 0x006D2ED2
 */
static bool TrackDesignSaveCanAddTileElement(TileElement* tileElement)
{
    size_t newElementCount = TrackDesignSaveGetTotalElementCount(tileElement);
    if (newElementCount == 0)
    {
        return false;
    }

    // Get number of spare elements left
    size_t spareSavedElements = TRACK_MAX_SAVED_TILE_ELEMENTS - _trackSavedTileElements.size();
    if (newElementCount > spareSavedElements)
    {
        // No more spare saved elements left
        return false;
    }

    return true;
}

/**
 *
 *  rct2: 0x006D2F4C
 */
static void TrackDesignSavePushTileElement(const CoordsXY& loc, TileElement* tileElement)
{
    if (_trackSavedTileElements.size() < TRACK_MAX_SAVED_TILE_ELEMENTS)
    {
        _trackSavedTileElements.push_back(tileElement);
        MapInvalidateTileFull(loc);
    }
}

static bool TrackDesignSaveIsSupportedObject(const Object* obj)
{
    const auto& entry = obj->GetObjectEntry();
    return !entry.IsEmpty();
}

static TrackDesignSceneryElement TrackDesignCreateTileElementDesc(const RCTObjectEntry& entry, const CoordsXYZ& loc)
{
    TrackDesignSceneryElement item{};
    item.sceneryObject = ObjectEntryDescriptor(entry);
    item.loc = loc;
    return item;
}

static TrackDesignSceneryElement TrackDesignCreateTileElementDesc(const Object& obj, const CoordsXYZ& loc)
{
    const auto& entry = obj.GetObjectEntry();
    if (entry.IsEmpty())
    {
        // Unsupported, should have been blocked earlier
        assert(false);
    }

    return TrackDesignCreateTileElementDesc(entry, loc);
}

static TrackDesignSceneryElement TrackDesignSaveCreateSmallSceneryDesc(
    const Object& object, const CoordsXY& loc, const SmallSceneryElement& smallSceneryElement)
{
    auto item = TrackDesignCreateTileElementDesc(object, { loc, smallSceneryElement.GetBaseZ() });
    item.setRotation(smallSceneryElement.GetDirection());
    item.setQuadrant(smallSceneryElement.GetSceneryQuadrant());
    item.primaryColour = smallSceneryElement.GetPrimaryColour();
    item.secondaryColour = smallSceneryElement.GetSecondaryColour();
    item.tertiaryColour = smallSceneryElement.GetTertiaryColour();

    return item;
}

static TrackDesignAddStatus TrackDesignSaveAddSmallScenery(const CoordsXY& loc, SmallSceneryElement* sceneryElement)
{
    auto entryIndex = sceneryElement->GetEntryIndex();
    auto obj = ObjectEntryGetObject(ObjectType::SmallScenery, entryIndex);
    if (obj != nullptr && TrackDesignSaveIsSupportedObject(obj))
    {
        auto item = TrackDesignSaveCreateSmallSceneryDesc(*obj, loc, *sceneryElement);
        TrackDesignSavePushTileElement(loc, reinterpret_cast<TileElement*>(sceneryElement));
        _trackSavedTileElementsDesc.push_back(std::move(item));
        return TrackDesignAddStatus::Success();
    }

    return TrackDesignAddStatus::Fail(STR_UNSUPPORTED_OBJECT_FORMAT);
}

static TrackDesignSceneryElement TrackDesignSaveCreateLargeSceneryDesc(
    const Object& object, const CoordsXYZ& loc, const LargeSceneryElement& largeSceneryElement)
{
    auto item = TrackDesignCreateTileElementDesc(object, loc);
    item.setRotation(largeSceneryElement.GetDirection());
    item.primaryColour = largeSceneryElement.GetPrimaryColour();
    item.secondaryColour = largeSceneryElement.GetSecondaryColour();
    item.tertiaryColour = largeSceneryElement.GetTertiaryColour();

    return item;
}

static TrackDesignAddStatus TrackDesignSaveAddLargeScenery(const CoordsXY& loc, LargeSceneryElement* tileElement)
{
    auto entryIndex = tileElement->GetEntryIndex();
    auto& objectMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objectMgr.GetLoadedObject(ObjectType::LargeScenery, entryIndex);
    if (obj != nullptr && TrackDesignSaveIsSupportedObject(obj))
    {
        auto sceneryEntry = reinterpret_cast<const LargeSceneryEntry*>(obj->GetLegacyData());
        auto& sceneryTiles = sceneryEntry->tiles;

        int32_t z = tileElement->BaseHeight;
        auto direction = tileElement->GetDirection();
        auto sequence = tileElement->GetSequenceIndex();

        auto sceneryOrigin = MapLargeSceneryGetOrigin(
            { loc.x, loc.y, z << 3, static_cast<Direction>(direction) }, sequence, nullptr);
        if (!sceneryOrigin.has_value())
        {
            return TrackDesignAddStatus::Success();
        }

        // Iterate through each tile of the large scenery element
        for (auto& tile : sceneryTiles)
        {
            CoordsXY offsetPos{ tile.offset };
            auto rotatedOffsetPos = offsetPos.Rotate(direction);

            CoordsXYZ tileLoc = { sceneryOrigin->x + rotatedOffsetPos.x, sceneryOrigin->y + rotatedOffsetPos.y,
                                  sceneryOrigin->z + tile.offset.z };
            auto largeElement = MapGetLargeScenerySegment({ tileLoc, static_cast<Direction>(direction) }, tile.index);
            if (largeElement != nullptr)
            {
                if (tile.index == 0)
                {
                    auto item = TrackDesignSaveCreateLargeSceneryDesc(*obj, tileLoc, *largeElement);
                    _trackSavedTileElementsDesc.push_back(std::move(item));
                }
                TrackDesignSavePushTileElement(tileLoc, reinterpret_cast<TileElement*>(largeElement));
            }
        }
        return TrackDesignAddStatus::Success();
    }

    return TrackDesignAddStatus::Fail(STR_UNSUPPORTED_OBJECT_FORMAT);
}

static TrackDesignSceneryElement TrackDesignSaveCreateWallDesc(
    const Object& object, const CoordsXY& loc, const WallElement& wallElement)
{
    auto item = TrackDesignCreateTileElementDesc(object, { loc, wallElement.GetBaseZ() });
    item.setRotation(wallElement.GetDirection());
    item.primaryColour = wallElement.GetPrimaryColour();
    item.secondaryColour = wallElement.GetSecondaryColour();
    item.tertiaryColour = wallElement.GetTertiaryColour();

    return item;
}

static TrackDesignAddStatus TrackDesignSaveAddWall(const CoordsXY& loc, WallElement* wallElement)
{
    auto entryIndex = wallElement->GetEntryIndex();
    auto obj = ObjectEntryGetObject(ObjectType::Walls, entryIndex);
    if (obj != nullptr && TrackDesignSaveIsSupportedObject(obj))
    {
        auto item = TrackDesignSaveCreateWallDesc(*obj, loc, *wallElement);
        TrackDesignSavePushTileElement(loc, reinterpret_cast<TileElement*>(wallElement));
        _trackSavedTileElementsDesc.push_back(std::move(item));
        return TrackDesignAddStatus::Success();
    }

    return TrackDesignAddStatus::Fail(STR_UNSUPPORTED_OBJECT_FORMAT);
}

static std::optional<RCTObjectEntry> TrackDesignSaveFootpathGetBestEntry(const PathElement& pathElement)
{
    RCTObjectEntry pathEntry;
    auto legacyPathObj = pathElement.GetLegacyPathEntry();
    if (legacyPathObj != nullptr)
    {
        pathEntry = legacyPathObj->GetObjectEntry();
        if (!pathEntry.IsEmpty())
        {
            return pathEntry;
        }
    }
    else
    {
        auto surfaceEntry = pathElement.GetSurfaceEntry();
        if (surfaceEntry != nullptr)
        {
            auto surfaceId = surfaceEntry->GetIdentifier();
            auto railingsEntry = pathElement.GetRailingsEntry();
            auto railingsId = railingsEntry == nullptr ? "" : railingsEntry->GetIdentifier();
            return RCT2::GetBestObjectEntryForSurface(surfaceId, railingsId);
        }
    }
    return std::nullopt;
}

static TrackDesignSceneryElement TrackDesignSaveCreateFootpathDesc(
    const RCTObjectEntry& pathEntry, const CoordsXY& loc, const PathElement& pathElement)
{
    auto item = TrackDesignCreateTileElementDesc(pathEntry, { loc, pathElement.GetBaseZ() });
    item.setEdges(pathElement.GetEdges());
    item.setHasSlope(pathElement.IsSloped());
    item.setSlopeDirection(pathElement.GetSlopeDirection());
    item.setIsQueue(pathElement.IsQueue());

    return item;
}

static TrackDesignAddStatus TrackDesignSaveAddFootpath(const CoordsXY& loc, PathElement* pathElement)
{
    auto pathEntry = TrackDesignSaveFootpathGetBestEntry(*pathElement);
    if (!pathEntry.has_value())
    {
        return TrackDesignAddStatus::Fail(STR_UNSUPPORTED_OBJECT_FORMAT);
    }

    auto item = TrackDesignSaveCreateFootpathDesc(pathEntry.value(), loc, *pathElement);
    TrackDesignSavePushTileElement(loc, reinterpret_cast<TileElement*>(pathElement));

    _trackSavedTileElementsDesc.push_back(std::move(item));
    return TrackDesignAddStatus::Success();
}

/**
 *
 *  rct2: 0x006D2B3C
 */
static TrackDesignAddStatus TrackDesignSaveAddTileElement(const CoordsXY& loc, TileElement* tileElement)
{
    if (!TrackDesignSaveCanAddTileElement(tileElement))
    {
        return TrackDesignAddStatus::Fail(STR_SAVE_TRACK_SCENERY_TOO_MANY_ITEMS_SELECTED);
    }

    switch (tileElement->GetType())
    {
        case TileElementType::SmallScenery:
            return TrackDesignSaveAddSmallScenery(loc, tileElement->AsSmallScenery());
        case TileElementType::LargeScenery:
            return TrackDesignSaveAddLargeScenery(loc, tileElement->AsLargeScenery());
        case TileElementType::Wall:
            return TrackDesignSaveAddWall(loc, tileElement->AsWall());
        case TileElementType::Path:
            return TrackDesignSaveAddFootpath(loc, tileElement->AsPath());
        default:
            return TrackDesignAddStatus::Fail(STR_UNKNOWN_OBJECT_TYPE);
    }
}

/**
 *
 *  rct2: 0x006D2F78
 */
static void TrackDesignSavePopTileElement(const CoordsXY& loc, TileElement* tileElement)
{
    MapInvalidateTileFull(loc);

    // Find index of map element to remove
    size_t removeIndex = SIZE_MAX;
    for (size_t i = 0; i < _trackSavedTileElements.size(); i++)
    {
        if (_trackSavedTileElements[i] == tileElement)
        {
            removeIndex = i;
        }
    }

    if (removeIndex != SIZE_MAX)
    {
        _trackSavedTileElements.erase(_trackSavedTileElements.begin() + removeIndex);
    }
}

/**
 *
 *  rct2: 0x006D2FDD
 */
static void TrackDesignSavePopTileElementDesc(const TrackDesignSceneryElement& desc)
{
    size_t removeIndex = SIZE_MAX;
    for (size_t i = 0; i < _trackSavedTileElementsDesc.size(); i++)
    {
        TrackDesignSceneryElement& item = _trackSavedTileElementsDesc[i];
        if (item != desc)
            continue;

        removeIndex = i;
    }

    if (removeIndex != SIZE_MAX)
    {
        _trackSavedTileElementsDesc.erase(_trackSavedTileElementsDesc.begin() + removeIndex);
    }
}

static void TrackDesignSaveRemoveSmallScenery(const CoordsXY& loc, SmallSceneryElement* sceneryElement)
{
    auto entryIndex = sceneryElement->GetEntryIndex();
    auto obj = ObjectEntryGetObject(ObjectType::SmallScenery, entryIndex);
    if (obj != nullptr)
    {
        auto item = TrackDesignSaveCreateSmallSceneryDesc(*obj, loc, *sceneryElement);
        TrackDesignSavePopTileElement(loc, reinterpret_cast<TileElement*>(sceneryElement));
        TrackDesignSavePopTileElementDesc(item);
    }
}

static void TrackDesignSaveRemoveLargeScenery(const CoordsXY& loc, LargeSceneryElement* tileElement)
{
    if (tileElement == nullptr)
    {
        LOG_WARNING("Null tile element");
        return;
    }

    auto entryIndex = tileElement->GetEntryIndex();
    auto& objectMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objectMgr.GetLoadedObject(ObjectType::LargeScenery, entryIndex);
    if (obj != nullptr)
    {
        auto sceneryEntry = reinterpret_cast<const LargeSceneryEntry*>(obj->GetLegacyData());
        auto sceneryTiles = sceneryEntry->tiles;

        int32_t z = tileElement->BaseHeight;
        auto direction = tileElement->GetDirection();
        auto sequence = tileElement->GetSequenceIndex();

        auto sceneryOrigin = MapLargeSceneryGetOrigin(
            { loc.x, loc.y, z << 3, static_cast<Direction>(direction) }, sequence, nullptr);
        if (!sceneryOrigin)
        {
            return;
        }

        // Iterate through each tile of the large scenery element
        for (auto& tile : sceneryTiles)
        {
            CoordsXY offsetPos{ tile.offset };
            auto rotatedOffsetPos = offsetPos.Rotate(direction);

            CoordsXYZ tileLoc = { sceneryOrigin->x + rotatedOffsetPos.x, sceneryOrigin->y + rotatedOffsetPos.y,
                                  sceneryOrigin->z + tile.offset.z };
            auto largeElement = MapGetLargeScenerySegment({ tileLoc, static_cast<Direction>(direction) }, tile.index);
            if (largeElement != nullptr)
            {
                if (tile.index == 0)
                {
                    auto item = TrackDesignSaveCreateLargeSceneryDesc(*obj, tileLoc, *largeElement);
                    TrackDesignSavePopTileElementDesc(item);
                }
                TrackDesignSavePopTileElement({ tileLoc.x, tileLoc.y }, reinterpret_cast<TileElement*>(largeElement));
            }
        }
    }
}

static void TrackDesignSaveRemoveWall(const CoordsXY& loc, WallElement* wallElement)
{
    auto entryIndex = wallElement->GetEntryIndex();
    auto obj = ObjectEntryGetObject(ObjectType::Walls, entryIndex);
    if (obj != nullptr)
    {
        auto item = TrackDesignSaveCreateWallDesc(*obj, loc, *wallElement);
        TrackDesignSavePopTileElement(loc, reinterpret_cast<TileElement*>(wallElement));
        TrackDesignSavePopTileElementDesc(item);
    }
}

static void TrackDesignSaveRemoveFootpath(const CoordsXY& loc, PathElement* pathElement)
{
    auto pathEntry = TrackDesignSaveFootpathGetBestEntry(*pathElement);
    if (pathEntry.has_value())
    {
        auto item = TrackDesignSaveCreateFootpathDesc(pathEntry.value(), loc, *pathElement);
        TrackDesignSavePopTileElement(loc, reinterpret_cast<TileElement*>(pathElement));
        TrackDesignSavePopTileElementDesc(item);
    }
}

/**
 *
 *  rct2: 0x006D2B3C
 */
static void TrackDesignSaveRemoveTileElement(const CoordsXY& loc, TileElement* tileElement)
{
    switch (tileElement->GetType())
    {
        case TileElementType::SmallScenery:
            TrackDesignSaveRemoveSmallScenery(loc, tileElement->AsSmallScenery());
            break;
        case TileElementType::LargeScenery:
            TrackDesignSaveRemoveLargeScenery(loc, tileElement->AsLargeScenery());
            break;
        case TileElementType::Wall:
            TrackDesignSaveRemoveWall(loc, tileElement->AsWall());
            break;
        case TileElementType::Path:
            TrackDesignSaveRemoveFootpath(loc, tileElement->AsPath());
            break;
        default:
            break;
    }
}

static bool TrackDesignSaveShouldSelectSceneryAround(RideId rideIndex, TileElement* tileElement)
{
    switch (tileElement->GetType())
    {
        case TileElementType::Path:
            if (tileElement->AsPath()->IsQueue() && tileElement->AsPath()->GetRideIndex() == rideIndex)
                return true;
            break;
        case TileElementType::Track:
            if (tileElement->AsTrack()->GetRideIndex() == rideIndex)
                return true;
            break;
        case TileElementType::Entrance:
            // FIXME: This will always break and return false!
            if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_ENTRANCE)
                break;
            if (tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_RIDE_EXIT)
                break;
            if (tileElement->AsEntrance()->GetRideIndex() == rideIndex)
                return true;
            break;
        default:
            break;
    }
    return false;
}

static void TrackDesignSaveShouldSelectNearbySceneryForTile(RideId rideIndex, int32_t cx, int32_t cy)
{
    TileElement* tileElement;

    for (int32_t y = cy - TRACK_NEARBY_SCENERY_DISTANCE; y <= cy + TRACK_NEARBY_SCENERY_DISTANCE; y++)
    {
        for (int32_t x = cx - TRACK_NEARBY_SCENERY_DISTANCE; x <= cx + TRACK_NEARBY_SCENERY_DISTANCE; x++)
        {
            tileElement = MapGetFirstElementAt(TileCoordsXY{ x, y });
            if (tileElement == nullptr)
                continue;
            do
            {
                ViewportInteractionItem interactionType = ViewportInteractionItem::None;
                switch (tileElement->GetType())
                {
                    case TileElementType::Path:
                        if (!tileElement->AsPath()->IsQueue())
                            interactionType = ViewportInteractionItem::Footpath;
                        else if (tileElement->AsPath()->GetRideIndex() == rideIndex)
                            interactionType = ViewportInteractionItem::Footpath;
                        break;
                    case TileElementType::SmallScenery:
                        interactionType = ViewportInteractionItem::Scenery;
                        break;
                    case TileElementType::Wall:
                        interactionType = ViewportInteractionItem::Wall;
                        break;
                    case TileElementType::LargeScenery:
                        interactionType = ViewportInteractionItem::LargeScenery;
                        break;
                    default:
                        break;
                }

                if (interactionType != ViewportInteractionItem::None)
                {
                    if (!TrackDesignSaveContainsTileElement(tileElement))
                    {
                        TrackDesignSaveAddTileElement(TileCoordsXY(x, y).ToCoordsXY(), tileElement);
                    }
                }
            } while (!(tileElement++)->IsLastForTile());
        }
    }
}
