/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../Diagnostic.h"
#include "../drawing/Drawing.h"
#include "../interface/Viewport.h"
#include "../localisation/Formatter.h"
#include "../localisation/StringIds.h"
#include "../object/FootpathObject.h"
#include "../object/FootpathRailingsObject.h"
#include "../object/FootpathSurfaceObject.h"
#include "../object/LargeSceneryObject.h"
#include "../object/ObjectManager.h"
#include "../rct2/RCT2.h"
#include "../world/Map.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "../world/tile_element/WallElement.h"
#include "TrackDesign.h"

#include <cassert>

using namespace OpenRCT2;

constexpr int32_t TRACK_NEARBY_SCENERY_DISTANCE = 1;

bool gTrackDesignSaveMode = false;
RideId gTrackDesignSaveRideIndex = RideId::GetNull();

static std::vector<const TileElement*> _trackSavedTileElements;
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
    switch (tileElement->getType())
    {
        case TileElementType::path:
        case TileElementType::smallScenery:
        case TileElementType::wall:
            return 1;

        case TileElementType::largeScenery:
        {
            auto* sceneryEntry = tileElement->asLargeScenery()->getEntry();
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

    return true;
}

/**
 *
 *  rct2: 0x006D2F4C
 */
static void TrackDesignSavePushTileElement(const CoordsXY& loc, TileElement* tileElement)
{
    _trackSavedTileElements.push_back(tileElement);
    MapInvalidateTileFull(loc);
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
    auto item = TrackDesignCreateTileElementDesc(object, { loc, smallSceneryElement.getBaseZ() });
    item.setRotation(smallSceneryElement.getDirection());
    item.setQuadrant(smallSceneryElement.getSceneryQuadrant());
    item.primaryColour = smallSceneryElement.getPrimaryColour();
    item.secondaryColour = smallSceneryElement.getSecondaryColour();
    item.tertiaryColour = smallSceneryElement.getTertiaryColour();

    return item;
}

static TrackDesignAddStatus TrackDesignSaveAddSmallScenery(const CoordsXY& loc, SmallSceneryElement* sceneryElement)
{
    auto entryIndex = sceneryElement->getEntryIndex();
    auto obj = ObjectEntryGetObject(ObjectType::smallScenery, entryIndex);
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
    item.setRotation(largeSceneryElement.getDirection());
    item.primaryColour = largeSceneryElement.getPrimaryColour();
    item.secondaryColour = largeSceneryElement.getSecondaryColour();
    item.tertiaryColour = largeSceneryElement.getTertiaryColour();

    return item;
}

static TrackDesignAddStatus TrackDesignSaveAddLargeScenery(const CoordsXY& loc, LargeSceneryElement* tileElement)
{
    auto entryIndex = tileElement->getEntryIndex();
    auto& objectMgr = GetContext()->GetObjectManager();
    auto obj = objectMgr.GetLoadedObject<LargeSceneryObject>(entryIndex);
    if (obj != nullptr && TrackDesignSaveIsSupportedObject(obj))
    {
        auto sceneryEntry = reinterpret_cast<const LargeSceneryEntry*>(obj->GetLegacyData());
        auto& sceneryTiles = sceneryEntry->tiles;

        int32_t z = tileElement->baseHeight;
        auto direction = tileElement->getDirection();
        auto sequence = tileElement->getSequenceIndex();

        auto sceneryOrigin = MapLargeSceneryGetOrigin({ loc.x, loc.y, z << 3, direction }, sequence, nullptr);
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
            auto largeElement = MapGetLargeScenerySegment({ tileLoc, direction }, tile.index);
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
    auto item = TrackDesignCreateTileElementDesc(object, { loc, wallElement.getBaseZ() });
    item.setRotation(wallElement.getDirection());
    item.primaryColour = wallElement.getPrimaryColour();
    item.secondaryColour = wallElement.getSecondaryColour();
    item.tertiaryColour = wallElement.getTertiaryColour();

    return item;
}

static TrackDesignAddStatus TrackDesignSaveAddWall(const CoordsXY& loc, WallElement* wallElement)
{
    auto entryIndex = wallElement->getEntryIndex();
    auto obj = ObjectEntryGetObject(ObjectType::walls, entryIndex);
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
    auto legacyPathObj = pathElement.getLegacyPathEntry();
    if (legacyPathObj != nullptr)
    {
        RCTObjectEntry pathEntry = legacyPathObj->GetObjectEntry();
        if (!pathEntry.IsEmpty())
        {
            return pathEntry;
        }
    }
    else
    {
        auto surfaceEntry = pathElement.getSurfaceEntry();
        if (surfaceEntry != nullptr)
        {
            auto surfaceId = surfaceEntry->GetIdentifier();
            auto railingsEntry = pathElement.getRailingsEntry();
            auto railingsId = railingsEntry == nullptr ? "" : railingsEntry->GetIdentifier();
            return RCT2::GetBestObjectEntryForSurface(surfaceId, railingsId);
        }
    }
    return std::nullopt;
}

static TrackDesignSceneryElement TrackDesignSaveCreateFootpathDesc(
    const RCTObjectEntry& pathEntry, const CoordsXY& loc, const PathElement& pathElement)
{
    auto item = TrackDesignCreateTileElementDesc(pathEntry, { loc, pathElement.getBaseZ() });
    item.setEdges(pathElement.getEdges());
    item.setHasSlope(pathElement.isSloped());
    item.setSlopeDirection(pathElement.getSlopeDirection());
    item.setIsQueue(pathElement.isQueue());

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

    switch (tileElement->getType())
    {
        case TileElementType::smallScenery:
            return TrackDesignSaveAddSmallScenery(loc, tileElement->asSmallScenery());
        case TileElementType::largeScenery:
            return TrackDesignSaveAddLargeScenery(loc, tileElement->asLargeScenery());
        case TileElementType::wall:
            return TrackDesignSaveAddWall(loc, tileElement->asWall());
        case TileElementType::path:
            return TrackDesignSaveAddFootpath(loc, tileElement->asPath());
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
    auto entryIndex = sceneryElement->getEntryIndex();
    auto obj = ObjectEntryGetObject(ObjectType::smallScenery, entryIndex);
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

    auto entryIndex = tileElement->getEntryIndex();
    auto& objectMgr = GetContext()->GetObjectManager();
    auto obj = objectMgr.GetLoadedObject<LargeSceneryObject>(entryIndex);
    if (obj != nullptr)
    {
        auto sceneryEntry = reinterpret_cast<const LargeSceneryEntry*>(obj->GetLegacyData());
        auto sceneryTiles = sceneryEntry->tiles;

        int32_t z = tileElement->baseHeight;
        auto direction = tileElement->getDirection();
        auto sequence = tileElement->getSequenceIndex();

        auto sceneryOrigin = MapLargeSceneryGetOrigin({ loc.x, loc.y, z << 3, direction }, sequence, nullptr);
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
            auto largeElement = MapGetLargeScenerySegment({ tileLoc, direction }, tile.index);
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
    auto entryIndex = wallElement->getEntryIndex();
    auto obj = ObjectEntryGetObject(ObjectType::walls, entryIndex);
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
    switch (tileElement->getType())
    {
        case TileElementType::smallScenery:
            TrackDesignSaveRemoveSmallScenery(loc, tileElement->asSmallScenery());
            break;
        case TileElementType::largeScenery:
            TrackDesignSaveRemoveLargeScenery(loc, tileElement->asLargeScenery());
            break;
        case TileElementType::wall:
            TrackDesignSaveRemoveWall(loc, tileElement->asWall());
            break;
        case TileElementType::path:
            TrackDesignSaveRemoveFootpath(loc, tileElement->asPath());
            break;
        default:
            break;
    }
}

static bool TrackDesignSaveShouldSelectSceneryAround(RideId rideIndex, TileElement* tileElement)
{
    switch (tileElement->getType())
    {
        case TileElementType::path:
            if (tileElement->asPath()->isQueue() && tileElement->asPath()->getRideIndex() == rideIndex)
                return true;
            break;
        case TileElementType::track:
            if (tileElement->asTrack()->getRideIndex() == rideIndex)
                return true;
            break;
        case TileElementType::entrance:
            // FIXME: This will always break and return false!
            if (tileElement->asEntrance()->getEntranceType() != EntranceType::rideEntrance)
                break;
            if (tileElement->asEntrance()->getEntranceType() != EntranceType::rideExit)
                break;
            if (tileElement->asEntrance()->getRideIndex() == rideIndex)
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
                ViewportInteractionItem interactionType = ViewportInteractionItem::none;
                switch (tileElement->getType())
                {
                    case TileElementType::path:
                        if (!tileElement->asPath()->isQueue())
                            interactionType = ViewportInteractionItem::footpath;
                        else if (tileElement->asPath()->getRideIndex() == rideIndex)
                            interactionType = ViewportInteractionItem::footpath;
                        break;
                    case TileElementType::smallScenery:
                        interactionType = ViewportInteractionItem::scenery;
                        break;
                    case TileElementType::wall:
                        interactionType = ViewportInteractionItem::wall;
                        break;
                    case TileElementType::largeScenery:
                        interactionType = ViewportInteractionItem::largeScenery;
                        break;
                    default:
                        break;
                }

                if (interactionType != ViewportInteractionItem::none)
                {
                    if (!TrackDesignSaveContainsTileElement(tileElement))
                    {
                        TrackDesignSaveAddTileElement(TileCoordsXY(x, y).ToCoordsXY(), tileElement);
                    }
                }
            } while (!(tileElement++)->isLastForTile());
        }
    }
}
