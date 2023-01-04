/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "../OpenRCT2.h"
#include "../actions/BannerRemoveAction.h"
#include "../actions/FootpathAdditionRemoveAction.h"
#include "../actions/LargeSceneryRemoveAction.h"
#include "../actions/SmallSceneryRemoveAction.h"
#include "../actions/WallRemoveAction.h"
#include "../common.h"
#include "../core/String.hpp"
#include "../entity/Fountain.h"
#include "../localisation/Localisation.h"
#include "../network/network.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../scenario/Scenario.h"
#include "Climate.h"
#include "Footpath.h"
#include "LargeScenery.h"
#include "Map.h"
#include "Park.h"
#include "SmallScenery.h"
#include "Wall.h"

uint8_t gSceneryQuadrant;

money32 gSceneryPlaceCost;
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

money64 gClearSceneryCost;

static std::vector<ScenerySelection> _restrictedScenery;

// rct2: 0x009A3E74
const CoordsXY SceneryQuadrantOffsets[] = {
    { 8, 8 },
    { 8, 24 },
    { 24, 24 },
    { 24, 8 },
};

LargeSceneryText::LargeSceneryText(const rct_large_scenery_text& original)
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

const rct_large_scenery_text_glyph* LargeSceneryText::GetGlyph(char32_t codepoint) const
{
    if (codepoint >= std::size(glyphs))
    {
        return nullptr;
    }
    return &glyphs[codepoint];
}

const rct_large_scenery_text_glyph& LargeSceneryText::GetGlyph(char32_t codepoint, char32_t defaultCodepoint) const
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
        if (network_get_mode() != NETWORK_MODE_NONE)
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
                    if (pathAddEntry->flags & PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER)
                    {
                        JumpingFountain::StartAnimation(JumpingFountainType::Water, sceneryPos, tileElement);
                    }
                    else if (pathAddEntry->flags & PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW)
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

    if (gCheatsDisablePlantAging && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_CAN_BE_WATERED))
    {
        return;
    }

    if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_CAN_BE_WATERED) || WeatherIsDry(gClimateCurrent.Weather) || GetAge() < 5)
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

WallSceneryEntry* GetWallEntry(ObjectEntryIndex entryIndex)
{
    WallSceneryEntry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::Walls, entryIndex);
    if (obj != nullptr)
    {
        result = static_cast<WallSceneryEntry*>(obj->GetLegacyData());
    }
    return result;
}

BannerSceneryEntry* GetBannerEntry(ObjectEntryIndex entryIndex)
{
    BannerSceneryEntry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::Banners, entryIndex);
    if (obj != nullptr)
    {
        result = static_cast<BannerSceneryEntry*>(obj->GetLegacyData());
    }
    return result;
}

PathBitEntry* GetFootpathItemEntry(ObjectEntryIndex entryIndex)
{
    PathBitEntry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::PathBits, entryIndex);
    if (obj != nullptr)
    {
        result = static_cast<PathBitEntry*>(obj->GetLegacyData());
    }
    return result;
}

rct_scenery_group_entry* GetSceneryGroupEntry(ObjectEntryIndex entryIndex)
{
    rct_scenery_group_entry* result = nullptr;
    auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objMgr.GetLoadedObject(ObjectType::SceneryGroup, entryIndex);
    if (obj != nullptr)
    {
        result = static_cast<rct_scenery_group_entry*>(obj->GetLegacyData());
    }
    return result;
}

int32_t WallEntryGetDoorSound(const WallSceneryEntry* wallEntry)
{
    return (wallEntry->flags2 & WALL_SCENERY_2_DOOR_SOUND_MASK) >> WALL_SCENERY_2_DOOR_SOUND_SHIFT;
}

bool IsSceneryAvailableToBuild(const ScenerySelection& item)
{
    // All scenery can be built when in the scenario editor
    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
    {
        return true;
    }

    if (!gCheatsIgnoreResearchStatus)
    {
        if (!scenery_is_invented(item))
        {
            return false;
        }
    }

    if (!gCheatsSandboxMode && !(gScreenFlags & SCREEN_FLAGS_EDITOR))
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
            return MAX_SMALL_SCENERY_OBJECTS;
        case SCENERY_TYPE_PATH_ITEM:
            return MAX_PATH_ADDITION_OBJECTS;
        case SCENERY_TYPE_WALL:
            return MAX_WALL_SCENERY_OBJECTS;
        case SCENERY_TYPE_LARGE:
            return MAX_LARGE_SCENERY_OBJECTS;
        case SCENERY_TYPE_BANNER:
            return MAX_BANNER_OBJECTS;
        default:
            return 0;
    }
}

static SceneryEntryBase* GetSceneryEntry(const ScenerySelection& item)
{
    switch (item.SceneryType)
    {
        case SCENERY_TYPE_SMALL:
            return GetSmallSceneryEntry(item.EntryIndex);
        case SCENERY_TYPE_PATH_ITEM:
            return GetFootpathItemEntry(item.EntryIndex);
        case SCENERY_TYPE_WALL:
            return GetWallEntry(item.EntryIndex);
        case SCENERY_TYPE_LARGE:
            return GetLargeSceneryEntry(item.EntryIndex);
        case SCENERY_TYPE_BANNER:
            return GetBannerEntry(item.EntryIndex);
        default:
            return nullptr;
    }
}

bool IsSceneryItemRestricted(const ScenerySelection& item)
{
    return std::find(std::begin(_restrictedScenery), std::end(_restrictedScenery), item) != std::end(_restrictedScenery);
}

void ClearRestrictedScenery()
{
    _restrictedScenery.clear();
}

std::vector<ScenerySelection>& GetRestrictedScenery()
{
    return _restrictedScenery;
}

static std::vector<ScenerySelection> GetAllMiscScenery()
{
    std::vector<ScenerySelection> miscScenery;
    std::vector<ScenerySelection> nonMiscScenery;
    for (ObjectEntryIndex i = 0; i < MAX_SCENERY_GROUP_OBJECTS; i++)
    {
        const auto* sgEntry = GetSceneryGroupEntry(i);
        if (sgEntry != nullptr)
        {
            nonMiscScenery.insert(nonMiscScenery.end(), sgEntry->SceneryEntries.begin(), sgEntry->SceneryEntries.end());
        }
    }
    for (uint8_t sceneryType = SCENERY_TYPE_SMALL; sceneryType < SCENERY_TYPE_COUNT; sceneryType++)
    {
        const auto maxObjects = GetMaxObjectsForSceneryType(sceneryType);
        for (ObjectEntryIndex i = 0; i < maxObjects; i++)
        {
            const ScenerySelection sceneryItem = { sceneryType, i };
            const auto* sceneryEntry = GetSceneryEntry(sceneryItem);
            if (sceneryEntry != nullptr)
            {
                if (std::find(std::begin(nonMiscScenery), std::end(nonMiscScenery), sceneryItem) == std::end(nonMiscScenery))
                {
                    miscScenery.push_back(sceneryItem);
                }
            }
        }
    }
    return miscScenery;
}

void RestrictAllMiscScenery()
{
    auto miscScenery = GetAllMiscScenery();
    _restrictedScenery.insert(_restrictedScenery.begin(), miscScenery.begin(), miscScenery.end());
}

void MarkAllUnrestrictedSceneryAsInvented()
{
    auto miscScenery = GetAllMiscScenery();
    for (const auto& sceneryItem : miscScenery)
    {
        if (std::find(_restrictedScenery.begin(), _restrictedScenery.end(), sceneryItem) == _restrictedScenery.end())
        {
            scenery_set_invented(sceneryItem);
        }
    }
}

ObjectType GetObjectTypeFromSceneryType(uint8_t type)
{
    switch (type)
    {
        case SCENERY_TYPE_SMALL:
            return ObjectType::SmallScenery;
        case SCENERY_TYPE_PATH_ITEM:
            return ObjectType::PathBits;
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
        case ObjectType::PathBits:
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
