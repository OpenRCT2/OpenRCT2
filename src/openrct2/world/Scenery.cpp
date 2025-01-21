/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Scenery.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../actions/BannerRemoveAction.h"
#include "../actions/FootpathAdditionRemoveAction.h"
#include "../actions/LargeSceneryRemoveAction.h"
#include "../actions/SmallSceneryRemoveAction.h"
#include "../actions/WallRemoveAction.h"
#include "../core/CodepointView.hpp"
#include "../entity/Fountain.h"
#include "../network/network.h"
#include "../object/BannerSceneryEntry.h"
#include "../object/LargeSceneryEntry.h"
#include "../object/ObjectEntryManager.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/PathAdditionEntry.h"
#include "../object/SceneryGroupEntry.h"
#include "../object/SceneryGroupObject.h"
#include "../object/SmallSceneryEntry.h"
#include "../object/WallSceneryEntry.h"
#include "../scenario/Scenario.h"
#include "Footpath.h"
#include "Map.h"
#include "Park.h"
#include "tile_element/PathElement.h"
#include "tile_element/SmallSceneryElement.h"

uint8_t gSceneryQuadrant;

money64 gSceneryPlaceCost;
ScenerySelection gSceneryPlaceObject;
int16_t gSceneryPlaceZ;
uint8_t gSceneryPlaceRotation;

uint8_t gSceneryGhostType;
CoordsXYZ gSceneryGhostPosition;
uint8_t gSceneryGhostWallRotation;

int16_t gSceneryShiftPressed;
int16_t gSceneryShiftPressX;
int16_t gSceneryShiftPressY;
int16_t gSceneryShiftPressZOffset;

int16_t gSceneryCtrlPressed;
int16_t gSceneryCtrlPressZ;

using namespace OpenRCT2;

// rct2: 0x009A3E74
const CoordsXY SceneryQuadrantOffsets[] = {
    { 8, 8 },
    { 8, 24 },
    { 24, 24 },
    { 24, 8 },
};

LargeSceneryText::LargeSceneryText(const RCTLargeSceneryText& original)
{
    for (size_t i = 0; i < std::size(original.offset); i++)
    {
        offset[i].x = original.offset[i].x;
        offset[i].y = original.offset[i].y;
    }
    max_width = original.max_width;
    flags = original.flags;
    num_images = original.num_images;
    for (size_t i = 0; i < std::size(original.glyphs); i++)
    {
        glyphs[i] = original.glyphs[i];
    }
}

const LargeSceneryTextGlyph* LargeSceneryText::GetGlyph(char32_t codepoint) const
{
    if (codepoint >= std::size(glyphs))
    {
        return nullptr;
    }
    return &glyphs[codepoint];
}

const LargeSceneryTextGlyph& LargeSceneryText::GetGlyph(char32_t codepoint, char32_t defaultCodepoint) const
{
    auto glyph = GetGlyph(codepoint);
    if (glyph == nullptr)
    {
        glyph = GetGlyph(defaultCodepoint);
    }
    return *glyph;
}

int32_t LargeSceneryText::MeasureWidth(std::string_view text) const
{
    auto result = 0;
    for (auto codepoint : CodepointView(text))
    {
        result += GetGlyph(codepoint, ' ').width;
    }
    return result;
}

int32_t LargeSceneryText::MeasureHeight(std::string_view text) const
{
    auto result = 0;
    for (auto codepoint : CodepointView(text))
    {
        result += GetGlyph(codepoint, ' ').height;
    }
    return result;
}

void SceneryUpdateTile(const CoordsXY& sceneryPos)
{
    TileElement* tileElement;

    tileElement = MapGetFirstElementAt(sceneryPos);
    if (tileElement == nullptr)
        return;
    do
    {
        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (NetworkGetMode() != NETWORK_MODE_NONE)
        {
            if (tileElement->IsGhost())
                continue;
        }

        if (tileElement->GetType() == TileElementType::SmallScenery)
        {
            tileElement->AsSmallScenery()->UpdateAge(sceneryPos);
        }
        else if (tileElement->GetType() == TileElementType::Path)
        {
            if (tileElement->AsPath()->HasAddition() && !tileElement->AsPath()->AdditionIsGhost())
            {
                auto* pathAddEntry = tileElement->AsPath()->GetAdditionEntry();
                if (pathAddEntry != nullptr)
                {
                    if (pathAddEntry->flags & PATH_ADDITION_FLAG_JUMPING_FOUNTAIN_WATER)
                    {
                        JumpingFountain::StartAnimation(JumpingFountainType::Water, sceneryPos, tileElement);
                    }
                    else if (pathAddEntry->flags & PATH_ADDITION_FLAG_JUMPING_FOUNTAIN_SNOW)
                    {
                        JumpingFountain::StartAnimation(JumpingFountainType::Snow, sceneryPos, tileElement);
                    }
                }
            }
        }
    } while (!(tileElement++)->IsLastForTile());
}

/**
 *
 *  rct2: 0x006E33D9
 */
void SmallSceneryElement::UpdateAge(const CoordsXY& sceneryPos)
{
    auto* sceneryEntry = GetEntry();
    if (sceneryEntry == nullptr)
    {
        return;
    }

    auto& gameState = GetGameState();
    if (gameState.Cheats.disablePlantAging && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_CAN_BE_WATERED))
    {
        return;
    }

    if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_CAN_BE_WATERED) || WeatherIsDry(gameState.ClimateCurrent.Weather)
        || GetAge() < 5)
    {
        IncreaseAge(sceneryPos);
        return;
    }

    // Check map elements above, presumably to see if map element is blocked from weather
    TileElement* tileElementAbove = reinterpret_cast<TileElement*>(this);
    // Change from original: RCT2 only checked for the first three quadrants, which was very likely to be a bug.
    while (!(tileElementAbove->GetOccupiedQuadrants()))
    {
        tileElementAbove++;

        // Ghosts are purely this-client-side and should not cause any interaction,
        // as that may lead to a desync.
        if (tileElementAbove->IsGhost())
            continue;

        switch (tileElementAbove->GetType())
        {
            case TileElementType::LargeScenery:
            case TileElementType::Entrance:
            case TileElementType::Path:
                MapInvalidateTileZoom1({ sceneryPos, tileElementAbove->GetBaseZ(), tileElementAbove->GetClearanceZ() });
                IncreaseAge(sceneryPos);
                return;
            case TileElementType::SmallScenery:
                sceneryEntry = tileElementAbove->AsSmallScenery()->GetEntry();
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
                {
                    IncreaseAge(sceneryPos);
                    return;
                }
                break;
            default:
                break;
        }
    }

    // Reset age / water plant
    SetAge(0);
    MapInvalidateTileZoom1({ sceneryPos, GetBaseZ(), GetClearanceZ() });
}

/**
 *
 *  rct2: 0x006E2712
 */
void SceneryRemoveGhostToolPlacement()
{
    if (gSceneryGhostType & SCENERY_GHOST_FLAG_0)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_0;

        auto removeSceneryAction = SmallSceneryRemoveAction(
            gSceneryGhostPosition, gSceneryQuadrant, gSceneryPlaceObject.EntryIndex);
        removeSceneryAction.SetFlags(
            GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
        removeSceneryAction.Execute();
    }

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_1)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_1;
        TileElement* tileElement = MapGetFirstElementAt(gSceneryGhostPosition);

        do
        {
            if (tileElement == nullptr)
                break;

            if (tileElement->GetType() != TileElementType::Path)
                continue;

            if (tileElement->GetBaseZ() != gSceneryGhostPosition.z)
                continue;

            auto footpathAdditionRemoveAction = FootpathAdditionRemoveAction(gSceneryGhostPosition);
            footpathAdditionRemoveAction.SetFlags(
                GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
            GameActions::Execute(&footpathAdditionRemoveAction);
            break;
        } while (!(tileElement++)->IsLastForTile());
    }

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_2)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_2;

        CoordsXYZD wallLocation = { gSceneryGhostPosition, gSceneryGhostWallRotation };
        auto wallRemoveAction = WallRemoveAction(wallLocation);
        wallRemoveAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
        wallRemoveAction.Execute();
    }

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_3)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_3;

        auto removeSceneryAction = LargeSceneryRemoveAction({ gSceneryGhostPosition, gSceneryPlaceRotation }, 0);
        removeSceneryAction.SetFlags(
            GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
        removeSceneryAction.Execute();
    }

    if (gSceneryGhostType & SCENERY_GHOST_FLAG_4)
    {
        gSceneryGhostType &= ~SCENERY_GHOST_FLAG_4;

        auto removeSceneryAction = BannerRemoveAction({ gSceneryGhostPosition, gSceneryPlaceRotation });
        removeSceneryAction.SetFlags(
            GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
        GameActions::Execute(&removeSceneryAction);
    }
}

bool IsSceneryAvailableToBuild(const ScenerySelection& item)
{
    // All scenery can be built when in the scenario editor
    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
    {
        return true;
    }

    auto& gameState = GetGameState();
    if (!gameState.Cheats.ignoreResearchStatus)
    {
        if (!SceneryIsInvented(item))
        {
            return false;
        }
    }

    if (!gameState.Cheats.sandboxMode && !(gScreenFlags & SCREEN_FLAGS_EDITOR))
    {
        if (IsSceneryItemRestricted(item))
        {
            return false;
        }
    }

    return true;
}

static size_t GetMaxObjectsForSceneryType(const uint8_t sceneryType)
{
    switch (sceneryType)
    {
        case SCENERY_TYPE_SMALL:
            return kMaxSmallSceneryObjects;
        case SCENERY_TYPE_PATH_ITEM:
            return kMaxPathAdditionObjects;
        case SCENERY_TYPE_WALL:
            return kMaxWallSceneryObjects;
        case SCENERY_TYPE_LARGE:
            return kMaxLargeSceneryObjects;
        case SCENERY_TYPE_BANNER:
            return kMaxBannerObjects;
        default:
            return 0;
    }
}

static bool IsSceneryEntryValid(const ScenerySelection& item)
{
    switch (item.SceneryType)
    {
        case SCENERY_TYPE_SMALL:
            return OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(item.EntryIndex) != nullptr;
        case SCENERY_TYPE_PATH_ITEM:
            return OpenRCT2::ObjectManager::GetObjectEntry<PathAdditionEntry>(item.EntryIndex) != nullptr;
        case SCENERY_TYPE_WALL:
            return OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(item.EntryIndex) != nullptr;
        case SCENERY_TYPE_LARGE:
            return OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(item.EntryIndex) != nullptr;
        case SCENERY_TYPE_BANNER:
            return OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(item.EntryIndex) != nullptr;
        default:
            return false;
    }
}

bool IsSceneryItemRestricted(const ScenerySelection& item)
{
    auto& gameState = GetGameState();
    return std::find(std::begin(gameState.RestrictedScenery), std::end(gameState.RestrictedScenery), item)
        != std::end(gameState.RestrictedScenery);
}

void ClearRestrictedScenery()
{
    GetGameState().RestrictedScenery.clear();
}

std::vector<ScenerySelection>& GetRestrictedScenery()
{
    return GetGameState().RestrictedScenery;
}

void SetSceneryItemRestricted(const ScenerySelection& item, bool on)
{
    auto& gameState = GetGameState();
    auto existingItem = std::find(std::begin(gameState.RestrictedScenery), std::end(gameState.RestrictedScenery), item);
    const bool existingItemIsPresent = existingItem != std::end(gameState.RestrictedScenery);
    if (on)
    {
        if (!existingItemIsPresent)
        {
            gameState.RestrictedScenery.push_back(item);
        }
    }
    else
    {
        if (existingItemIsPresent)
        {
            gameState.RestrictedScenery.erase(existingItem);
        }
    }
}

bool ObjectTypeCanBeRestricted(ObjectType objectType)
{
    switch (objectType)
    {
        case ObjectType::SmallScenery:
        case ObjectType::LargeScenery:
        case ObjectType::Walls:
        case ObjectType::Banners:
        case ObjectType::PathAdditions:
            return true;
        default:
            return false;
    }
}

struct MiscScenery
{
    // Scenery that will end up on the ‘?’ tab
    std::vector<ScenerySelection> miscScenery{};
    // Scenery that has attached itself to an existing group, but is not referenced directly.
    std::vector<ScenerySelection> additionalGroupScenery{};
};

static MiscScenery GetAllMiscScenery()
{
    MiscScenery ret;
    std::vector<ScenerySelection> referencedBySceneryGroups;
    std::vector<ObjectEntryIndex> sceneryGroupIds;
    for (ObjectEntryIndex i = 0; i < kMaxSceneryGroupObjects; i++)
    {
        const auto* sgEntry = OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(i);
        if (sgEntry != nullptr)
        {
            referencedBySceneryGroups.insert(
                referencedBySceneryGroups.end(), sgEntry->SceneryEntries.begin(), sgEntry->SceneryEntries.end());
            sceneryGroupIds.emplace_back(i);
        }
    }
    for (uint8_t sceneryType = SCENERY_TYPE_SMALL; sceneryType < SCENERY_TYPE_COUNT; sceneryType++)
    {
        const auto maxObjects = GetMaxObjectsForSceneryType(sceneryType);
        for (ObjectEntryIndex i = 0; i < maxObjects; i++)
        {
            ObjectEntryIndex linkedSceneryGroup = kObjectEntryIndexNull;
            const auto objectType = GetObjectTypeFromSceneryType(sceneryType);
            switch (objectType)
            {
                case ObjectType::SmallScenery:
                {
                    const auto* objectEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(i);
                    if (objectEntry != nullptr)
                        linkedSceneryGroup = objectEntry->scenery_tab_id;
                    break;
                }
                case ObjectType::LargeScenery:
                {
                    const auto* objectEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(i);
                    if (objectEntry != nullptr)
                        linkedSceneryGroup = objectEntry->scenery_tab_id;
                    break;
                }
                case ObjectType::Walls:
                {
                    const auto* objectEntry = OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(i);
                    if (objectEntry != nullptr)
                        linkedSceneryGroup = objectEntry->scenery_tab_id;
                    break;
                }
                case ObjectType::Banners:
                {
                    const auto* objectEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(i);
                    if (objectEntry != nullptr)
                        linkedSceneryGroup = objectEntry->scenery_tab_id;
                    break;
                }
                case ObjectType::PathAdditions:
                {
                    const auto* objectEntry = OpenRCT2::ObjectManager::GetObjectEntry<PathAdditionEntry>(i);
                    if (objectEntry != nullptr)
                        linkedSceneryGroup = objectEntry->scenery_tab_id;
                    break;
                }
                default:
                    break;
            }

            const ScenerySelection sceneryItem = { sceneryType, i };
            if (!IsSceneryEntryValid(sceneryItem))
                continue;

            const bool isReferencedBySceneryGroup = std::find(
                                                        std::begin(referencedBySceneryGroups),
                                                        std::end(referencedBySceneryGroups), sceneryItem)
                != std::end(referencedBySceneryGroups);
            if (isReferencedBySceneryGroup)
                continue;

            // An object may link itself against a scenery group, in which case it should not be marked as miscellaneous.
            bool isLinkedToKnownSceneryGroup = false;
            if (linkedSceneryGroup != kObjectEntryIndexNull)
            {
                if (std::find(std::begin(sceneryGroupIds), std::end(sceneryGroupIds), linkedSceneryGroup)
                    != std::end(sceneryGroupIds))
                {
                    isLinkedToKnownSceneryGroup = true;
                }
            }

            if (isLinkedToKnownSceneryGroup)
                ret.additionalGroupScenery.push_back(sceneryItem);
            else
                ret.miscScenery.push_back(sceneryItem);
        }
    }
    return ret;
}

void RestrictAllMiscScenery()
{
    auto& gameState = GetGameState();
    auto miscScenery = GetAllMiscScenery().miscScenery;
    gameState.RestrictedScenery.insert(gameState.RestrictedScenery.begin(), miscScenery.begin(), miscScenery.end());
}

static void MarkAllUnrestrictedSceneryInVectorInvented(const std::vector<ScenerySelection>& vector)
{
    auto& restrictedScenery = GetGameState().RestrictedScenery;

    for (const auto& sceneryItem : vector)
    {
        const bool isNotRestricted = std::find(restrictedScenery.begin(), restrictedScenery.end(), sceneryItem)
            == restrictedScenery.end();
        if (isNotRestricted)
        {
            ScenerySetInvented(sceneryItem);
        }
    }
}

void MarkAllUnrestrictedSceneryAsInvented()
{
    auto scenery = GetAllMiscScenery();
    MarkAllUnrestrictedSceneryInVectorInvented(scenery.miscScenery);
    MarkAllUnrestrictedSceneryInVectorInvented(scenery.additionalGroupScenery);
}

ObjectType GetObjectTypeFromSceneryType(uint8_t type)
{
    switch (type)
    {
        case SCENERY_TYPE_SMALL:
            return ObjectType::SmallScenery;
        case SCENERY_TYPE_PATH_ITEM:
            return ObjectType::PathAdditions;
        case SCENERY_TYPE_WALL:
            return ObjectType::Walls;
        case SCENERY_TYPE_LARGE:
            return ObjectType::LargeScenery;
        case SCENERY_TYPE_BANNER:
            return ObjectType::Banners;
        default:
            throw std::runtime_error("Invalid scenery type");
    }
}

uint8_t GetSceneryTypeFromObjectType(ObjectType type)
{
    switch (type)
    {
        case ObjectType::SmallScenery:
            return SCENERY_TYPE_SMALL;
        case ObjectType::PathAdditions:
            return SCENERY_TYPE_PATH_ITEM;
        case ObjectType::Walls:
            return SCENERY_TYPE_WALL;
        case ObjectType::LargeScenery:
            return SCENERY_TYPE_LARGE;
        case ObjectType::Banners:
            return SCENERY_TYPE_BANNER;
        default:
            throw std::runtime_error("Invalid object type");
    }
}
