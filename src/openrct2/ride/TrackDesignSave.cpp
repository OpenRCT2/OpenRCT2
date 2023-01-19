/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../Game.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../interface/Viewport.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../object/FootpathObject.h"
#include "../object/FootpathRailingsObject.h"
#include "../object/FootpathSurfaceObject.h"
#include "../object/LargeSceneryObject.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Footpath.h"
#include "../world/LargeScenery.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Wall.h"
#include "RideData.h"
#include "Station.h"
#include "Track.h"
#include "TrackData.h"
#include "TrackDesign.h"
#include "TrackDesignRepository.h"

#include <algorithm>

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

static bool track_design_save_should_select_scenery_around(RideId rideIndex, TileElement* tileElement);
static void track_design_save_select_nearby_scenery_for_tile(RideId rideIndex, int32_t cx, int32_t cy);
static TrackDesignAddStatus track_design_save_add_tile_element(
    ViewportInteractionItem interactionType, const CoordsXY& loc, TileElement* tileElement);
static void track_design_save_remove_tile_element(
    ViewportInteractionItem interactionType, const CoordsXY& loc, TileElement* tileElement);

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
            track_design_save_remove_tile_element(interactionType, loc, tileElement);
        }
    }
    else
    {
        if (collect)
        {
            auto result = track_design_save_add_tile_element(interactionType, loc, tileElement);
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
        if (track_design_save_should_select_scenery_around(rideIndex, it.element))
        {
            track_design_save_select_nearby_scenery_for_tile(rideIndex, it.x, it.y);
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

static int32_t tile_element_get_total_element_count(TileElement* tileElement)
{
    int32_t elementCount;
    rct_large_scenery_tile* tile;

    switch (tileElement->GetType())
    {
        case TileElementType::Path:
        case TileElementType::SmallScenery:
        case TileElementType::Wall:
            return 1;

        case TileElementType::LargeScenery:
        {
            auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
            tile = sceneryEntry->tiles;
            elementCount = 0;
            do
            {
                tile++;
                elementCount++;
            } while (tile->x_offset != static_cast<int16_t>(static_cast<uint16_t>(0xFFFF)));
            return elementCount;
        }
        default:
            return 0;
    }
}

/**
 *
 *  rct2: 0x006D2ED2
 */
static bool track_design_save_can_add_tile_element(TileElement* tileElement)
{
    size_t newElementCount = tile_element_get_total_element_count(tileElement);
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
static void track_design_save_push_tile_element(const CoordsXY& loc, TileElement* tileElement)
{
    if (_trackSavedTileElements.size() < TRACK_MAX_SAVED_TILE_ELEMENTS)
    {
        _trackSavedTileElements.push_back(tileElement);
        MapInvalidateTileFull(loc);
    }
}

static bool track_design_is_supported_object(const Object* obj)
{
    const auto& entry = obj->GetObjectEntry();
    return !entry.IsEmpty();
}

static void track_design_save_push_tile_element_desc(
    const RCTObjectEntry& entry, const CoordsXYZ& loc, uint8_t flags, uint8_t primaryColour, uint8_t secondaryColour)
{
    TrackDesignSceneryElement item{};
    item.scenery_object = ObjectEntryDescriptor(entry);
    item.loc = loc;
    item.flags = flags;
    item.primary_colour = primaryColour;
    item.secondary_colour = secondaryColour;

    _trackSavedTileElementsDesc.push_back(std::move(item));
}

static void track_design_save_push_tile_element_desc(
    const Object* obj, const CoordsXYZ& loc, uint8_t flags, uint8_t primaryColour, uint8_t secondaryColour)
{
    const auto& entry = obj->GetObjectEntry();
    if (entry.IsEmpty())
    {
        // Unsupported, should have been blocked earlier
        assert(false);
    }

    track_design_save_push_tile_element_desc(entry, loc, flags, primaryColour, secondaryColour);
}

static TrackDesignAddStatus track_design_save_add_scenery(const CoordsXY& loc, SmallSceneryElement* sceneryElement)
{
    auto entryIndex = sceneryElement->GetEntryIndex();
    auto obj = ObjectEntryGetObject(ObjectType::SmallScenery, entryIndex);
    if (obj != nullptr && track_design_is_supported_object(obj))
    {
        uint8_t flags = 0;
        flags |= sceneryElement->GetDirection();
        flags |= sceneryElement->GetSceneryQuadrant() << 2;

        uint8_t primaryColour = sceneryElement->GetPrimaryColour();
        uint8_t secondaryColour = sceneryElement->GetSecondaryColour();

        track_design_save_push_tile_element(loc, reinterpret_cast<TileElement*>(sceneryElement));
        track_design_save_push_tile_element_desc(
            obj, { loc.x, loc.y, sceneryElement->GetBaseZ() }, flags, primaryColour, secondaryColour);
        return TrackDesignAddStatus::Success();
    }

    return TrackDesignAddStatus::Fail(STR_UNSUPPORTED_OBJECT_FORMAT);
}

static TrackDesignAddStatus track_design_save_add_large_scenery(const CoordsXY& loc, LargeSceneryElement* tileElement)
{
    auto entryIndex = tileElement->GetEntryIndex();
    auto& objectMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objectMgr.GetLoadedObject(ObjectType::LargeScenery, entryIndex);
    if (obj != nullptr && track_design_is_supported_object(obj))
    {
        auto sceneryEntry = reinterpret_cast<const LargeSceneryEntry*>(obj->GetLegacyData());
        auto sceneryTiles = sceneryEntry->tiles;

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
        sequence = 0;
        for (auto tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++)
        {
            CoordsXY offsetPos{ tile->x_offset, tile->y_offset };
            auto rotatedOffsetPos = offsetPos.Rotate(direction);

            CoordsXYZ tileLoc = { sceneryOrigin->x + rotatedOffsetPos.x, sceneryOrigin->y + rotatedOffsetPos.y,
                                  sceneryOrigin->z + tile->z_offset };
            auto largeElement = MapGetLargeScenerySegment({ tileLoc, static_cast<Direction>(direction) }, sequence);
            if (largeElement != nullptr)
            {
                if (sequence == 0)
                {
                    uint8_t flags = largeElement->GetDirection();
                    uint8_t primaryColour = largeElement->GetPrimaryColour();
                    uint8_t secondaryColour = largeElement->GetSecondaryColour();

                    track_design_save_push_tile_element_desc(obj, tileLoc, flags, primaryColour, secondaryColour);
                }
                track_design_save_push_tile_element({ tileLoc.x, tileLoc.y }, reinterpret_cast<TileElement*>(largeElement));
            }
        }
        return TrackDesignAddStatus::Success();
    }

    return TrackDesignAddStatus::Fail(STR_UNSUPPORTED_OBJECT_FORMAT);
}

static TrackDesignAddStatus track_design_save_add_wall(const CoordsXY& loc, WallElement* wallElement)
{
    auto entryIndex = wallElement->GetEntryIndex();
    auto obj = ObjectEntryGetObject(ObjectType::Walls, entryIndex);
    if (obj != nullptr && track_design_is_supported_object(obj))
    {
        uint8_t flags = 0;
        flags |= wallElement->GetDirection();
        flags |= wallElement->GetTertiaryColour() << 2;

        uint8_t secondaryColour = wallElement->GetSecondaryColour();
        uint8_t primaryColour = wallElement->GetPrimaryColour();

        track_design_save_push_tile_element(loc, reinterpret_cast<TileElement*>(wallElement));
        track_design_save_push_tile_element_desc(
            obj, { loc.x, loc.y, wallElement->GetBaseZ() }, flags, primaryColour, secondaryColour);
        return TrackDesignAddStatus::Success();
    }

    return TrackDesignAddStatus::Fail(STR_UNSUPPORTED_OBJECT_FORMAT);
}

static std::optional<RCTObjectEntry> track_design_save_footpath_get_best_entry(PathElement* pathElement)
{
    RCTObjectEntry pathEntry;
    auto legacyPathObj = pathElement->GetLegacyPathEntry();
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
        auto surfaceEntry = pathElement->GetSurfaceEntry();
        if (surfaceEntry != nullptr)
        {
            auto surfaceId = surfaceEntry->GetIdentifier();
            auto railingsEntry = pathElement->GetRailingsEntry();
            auto railingsId = railingsEntry == nullptr ? "" : railingsEntry->GetIdentifier();
            return RCT2::GetBestObjectEntryForSurface(surfaceId, railingsId);
        }
    }
    return {};
}

static TrackDesignAddStatus track_design_save_add_footpath(const CoordsXY& loc, PathElement* pathElement)
{
    auto pathEntry = track_design_save_footpath_get_best_entry(pathElement);
    if (!pathElement)
    {
        return TrackDesignAddStatus::Fail(STR_UNSUPPORTED_OBJECT_FORMAT);
    }

    uint8_t flags = 0;
    flags |= pathElement->GetEdges();
    flags |= (pathElement->GetSlopeDirection()) << 5;
    if (pathElement->IsSloped())
        flags |= 0b00010000;
    if (pathElement->IsQueue())
        flags |= 1 << 7;

    track_design_save_push_tile_element(loc, reinterpret_cast<TileElement*>(pathElement));
    track_design_save_push_tile_element_desc(*pathEntry, { loc.x, loc.y, pathElement->GetBaseZ() }, flags, 0, 0);
    return TrackDesignAddStatus::Success();
}

/**
 *
 *  rct2: 0x006D2B3C
 */
static TrackDesignAddStatus track_design_save_add_tile_element(
    ViewportInteractionItem interactionType, const CoordsXY& loc, TileElement* tileElement)
{
    if (!track_design_save_can_add_tile_element(tileElement))
    {
        return TrackDesignAddStatus::Fail(STR_SAVE_TRACK_SCENERY_TOO_MANY_ITEMS_SELECTED);
    }

    switch (interactionType)
    {
        case ViewportInteractionItem::Scenery:
            return track_design_save_add_scenery(loc, tileElement->AsSmallScenery());
        case ViewportInteractionItem::LargeScenery:
            return track_design_save_add_large_scenery(loc, tileElement->AsLargeScenery());
        case ViewportInteractionItem::Wall:
            return track_design_save_add_wall(loc, tileElement->AsWall());
        case ViewportInteractionItem::Footpath:
            return track_design_save_add_footpath(loc, tileElement->AsPath());
        default:
            return TrackDesignAddStatus::Fail(STR_UNKNOWN_OBJECT_TYPE);
    }
}

/**
 *
 *  rct2: 0x006D2F78
 */
static void track_design_save_pop_tile_element(const CoordsXY& loc, TileElement* tileElement)
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
static void track_design_save_pop_tile_element_desc(const ObjectEntryDescriptor& entry, const CoordsXYZ& loc, uint8_t flags)
{
    size_t removeIndex = SIZE_MAX;
    for (size_t i = 0; i < _trackSavedTileElementsDesc.size(); i++)
    {
        TrackDesignSceneryElement* item = &_trackSavedTileElementsDesc[i];
        if (item->loc != loc)
            continue;
        if (item->flags != flags)
            continue;
        if (item->scenery_object != entry)
            continue;

        removeIndex = i;
    }

    if (removeIndex != SIZE_MAX)
    {
        _trackSavedTileElementsDesc.erase(_trackSavedTileElementsDesc.begin() + removeIndex);
    }
}

static void track_design_save_remove_scenery(const CoordsXY& loc, SmallSceneryElement* sceneryElement)
{
    auto entryIndex = sceneryElement->GetEntryIndex();
    auto obj = ObjectEntryGetObject(ObjectType::SmallScenery, entryIndex);
    if (obj != nullptr)
    {
        uint8_t flags = 0;
        flags |= sceneryElement->GetDirection();
        flags |= sceneryElement->GetSceneryQuadrant() << 2;

        track_design_save_pop_tile_element(loc, reinterpret_cast<TileElement*>(sceneryElement));
        track_design_save_pop_tile_element_desc(obj->GetDescriptor(), { loc.x, loc.y, sceneryElement->GetBaseZ() }, flags);
    }
}

static void track_design_save_remove_large_scenery(const CoordsXY& loc, LargeSceneryElement* tileElement)
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
        sequence = 0;
        for (auto tile = sceneryTiles; tile->x_offset != -1; tile++, sequence++)
        {
            CoordsXY offsetPos{ tile->x_offset, tile->y_offset };
            auto rotatedOffsetPos = offsetPos.Rotate(direction);

            CoordsXYZ tileLoc = { sceneryOrigin->x + rotatedOffsetPos.x, sceneryOrigin->y + rotatedOffsetPos.y,
                                  sceneryOrigin->z + tile->z_offset };
            auto largeElement = MapGetLargeScenerySegment({ tileLoc, static_cast<Direction>(direction) }, sequence);
            if (largeElement != nullptr)
            {
                if (sequence == 0)
                {
                    uint8_t flags = largeElement->GetDirection();
                    track_design_save_pop_tile_element_desc(obj->GetDescriptor(), tileLoc, flags);
                }
                track_design_save_pop_tile_element({ tileLoc.x, tileLoc.y }, reinterpret_cast<TileElement*>(largeElement));
            }
        }
    }
}

static void track_design_save_remove_wall(const CoordsXY& loc, WallElement* wallElement)
{
    auto entryIndex = wallElement->GetEntryIndex();
    auto obj = ObjectEntryGetObject(ObjectType::Walls, entryIndex);
    if (obj != nullptr)
    {
        uint8_t flags = 0;
        flags |= wallElement->GetDirection();
        flags |= wallElement->GetTertiaryColour() << 2;

        track_design_save_pop_tile_element(loc, reinterpret_cast<TileElement*>(wallElement));
        track_design_save_pop_tile_element_desc(obj->GetDescriptor(), { loc.x, loc.y, wallElement->GetBaseZ() }, flags);
    }
}

static void track_design_save_remove_footpath(const CoordsXY& loc, PathElement* pathElement)
{
    auto pathEntry = track_design_save_footpath_get_best_entry(pathElement);
    if (pathElement)
    {
        uint8_t flags = 0;
        flags |= pathElement->GetEdges();
        flags |= (pathElement->GetSlopeDirection()) << 5;
        if (pathElement->IsSloped())
            flags |= 0b00010000;
        if (pathElement->IsQueue())
            flags |= 1 << 7;

        track_design_save_pop_tile_element(loc, reinterpret_cast<TileElement*>(pathElement));
        track_design_save_pop_tile_element_desc(
            ObjectEntryDescriptor(*pathEntry), { loc.x, loc.y, pathElement->GetBaseZ() }, flags);
    }
}

/**
 *
 *  rct2: 0x006D2B3C
 */
static void track_design_save_remove_tile_element(
    ViewportInteractionItem interactionType, const CoordsXY& loc, TileElement* tileElement)
{
    switch (interactionType)
    {
        case ViewportInteractionItem::Scenery:
            track_design_save_remove_scenery(loc, tileElement->AsSmallScenery());
            break;
        case ViewportInteractionItem::LargeScenery:
            track_design_save_remove_large_scenery(loc, tileElement->AsLargeScenery());
            break;
        case ViewportInteractionItem::Wall:
            track_design_save_remove_wall(loc, tileElement->AsWall());
            break;
        case ViewportInteractionItem::Footpath:
            track_design_save_remove_footpath(loc, tileElement->AsPath());
            break;
        default:
            break;
    }
}

static bool track_design_save_should_select_scenery_around(RideId rideIndex, TileElement* tileElement)
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

static void track_design_save_select_nearby_scenery_for_tile(RideId rideIndex, int32_t cx, int32_t cy)
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
                        track_design_save_add_tile_element(interactionType, TileCoordsXY(x, y).ToCoordsXY(), tileElement);
                    }
                }
            } while (!(tileElement++)->IsLastForTile());
        }
    }
}
