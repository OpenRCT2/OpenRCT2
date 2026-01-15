/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EditorObjectSelectionSession.h"

#include "Context.h"
#include "Diagnostic.h"
#include "Editor.h"
#include "Game.h"
#include "GameState.h"
#include "OpenRCT2.h"
#include "actions/ResultWithMessage.h"
#include "drawing/Drawing.h"
#include "entity/EntityList.h"
#include "entity/Guest.h"
#include "entity/Staff.h"
#include "localisation/Formatter.h"
#include "management/Research.h"
#include "object/DefaultObjects.h"
#include "object/FootpathEntry.h"
#include "object/ObjectList.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "object/WaterObject.h"
#include "ride/RideData.h"
#include "ride/RideManager.hpp"
#include "ride/TrainManager.h"
#include "ride/Vehicle.h"
#include "windows/Intent.h"
#include "world/Map.h"
#include "world/tile_element/BannerElement.h"
#include "world/tile_element/EntranceElement.h"
#include "world/tile_element/LargeSceneryElement.h"
#include "world/tile_element/PathElement.h"
#include "world/tile_element/SmallSceneryElement.h"
#include "world/tile_element/SurfaceElement.h"
#include "world/tile_element/TileElement.h"
#include "world/tile_element/WallElement.h"

#include <iterator>
#include <vector>

using namespace OpenRCT2;

std::optional<StringId> _gSceneryGroupPartialSelectError;
std::vector<uint8_t> _objectSelectionFlags;
uint32_t _numSelectedObjectsForType[EnumValue(ObjectType::count)];
static int32_t _numAvailableObjectsForType[EnumValue(ObjectType::count)];

static void SetupInUseSelectionFlags();
static void SetupTrackDesignerObjects();
static void SetupTrackManagerObjects();
static void selectScenarioEditorObjects();
static void selectTrackDesignerObjects();
static void ReplaceSelectedWaterPalette(const ObjectRepositoryItem* item);

/**
 * Master objects are objects that are not optional / required dependants of an object.
 */
static constexpr ResultWithMessage ObjectSelectionError(bool isMasterObject, StringId message)
{
    if (!isMasterObject)
        ResetSelectedObjectCountAndSize();

    return { false, message };
}

/**
 *
 *  rct2: 0x006ABCD1
 */
static void SetupTrackManagerObjects()
{
    int32_t numObjects = static_cast<int32_t>(ObjectRepositoryGetItemsCount());
    const ObjectRepositoryItem* items = ObjectRepositoryGetItems();
    for (int32_t i = 0; i < numObjects; i++)
    {
        uint8_t* selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem* item = &items[i];
        if (item->Type == ObjectType::ride)
        {
            *selectionFlags |= ObjectSelectionFlags::Flag6;

            for (auto rideType : item->RideInfo.RideType)
            {
                if (GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::hasTrack))
                {
                    *selectionFlags &= ~ObjectSelectionFlags::Flag6;
                    break;
                }
            }
        }
    }
}

/**
 *
 *  rct2: 0x006ABC1E
 */
static void SetupTrackDesignerObjects()
{
    int32_t numObjects = static_cast<int32_t>(ObjectRepositoryGetItemsCount());
    const ObjectRepositoryItem* items = ObjectRepositoryGetItems();
    selectTrackDesignerObjects();
    for (int32_t i = 0; i < numObjects; i++)
    {
        uint8_t* selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem* item = &items[i];
        if (item->Type == ObjectType::ride)
        {
            *selectionFlags |= ObjectSelectionFlags::Flag6;

            for (auto rideType : item->RideInfo.RideType)
            {
                if (rideType != kRideTypeNull)
                {
                    if (GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::showInTrackDesigner))
                    {
                        *selectionFlags &= ~ObjectSelectionFlags::Flag6;
                        break;
                    }
                }
            }
        }
    }
}

/**
 *
 *  rct2: 0x006AA82B
 */
void SetupInUseSelectionFlags()
{
    auto& objectMgr = GetContext()->GetObjectManager();

    for (auto objectType : getTransientObjectTypes())
    {
        for (auto i = 0u; i < getObjectEntryGroupCount(objectType); i++)
        {
            Editor::ClearSelectedObject(objectType, i, ObjectSelectionFlags::AllFlags);

            auto loadedObj = objectMgr.GetLoadedObject(objectType, i);
            if (loadedObj != nullptr)
            {
                Editor::SetSelectedObject(objectType, i, ObjectSelectionFlags::Selected);
            }
        }
    }

    TileElementIterator iter;
    TileElementIteratorBegin(&iter);
    do
    {
        ObjectEntryIndex type;

        switch (iter.element->GetType())
        {
            default:
            case TileElementType::Surface:
            {
                auto surfaceEl = iter.element->AsSurface();
                auto surfaceIndex = surfaceEl->GetSurfaceObjectIndex();
                auto edgeIndex = surfaceEl->GetEdgeObjectIndex();

                Editor::SetSelectedObject(ObjectType::terrainSurface, surfaceIndex, ObjectSelectionFlags::InUse);
                Editor::SetSelectedObject(ObjectType::terrainEdge, edgeIndex, ObjectSelectionFlags::InUse);
                break;
            }
            case TileElementType::Track:
                break;
            case TileElementType::Path:
            {
                auto footpathEl = iter.element->AsPath();
                auto legacyPathEntryIndex = footpathEl->GetLegacyPathEntryIndex();
                if (legacyPathEntryIndex == kObjectEntryIndexNull)
                {
                    auto surfaceEntryIndex = footpathEl->GetSurfaceEntryIndex();
                    auto railingEntryIndex = footpathEl->GetRailingsEntryIndex();
                    Editor::SetSelectedObject(ObjectType::footpathSurface, surfaceEntryIndex, ObjectSelectionFlags::InUse);
                    Editor::SetSelectedObject(ObjectType::footpathRailings, railingEntryIndex, ObjectSelectionFlags::InUse);
                }
                else
                {
                    Editor::SetSelectedObject(ObjectType::paths, legacyPathEntryIndex, ObjectSelectionFlags::InUse);
                }
                if (footpathEl->HasAddition())
                {
                    auto pathAdditionEntryIndex = footpathEl->GetAdditionEntryIndex();
                    Editor::SetSelectedObject(ObjectType::pathAdditions, pathAdditionEntryIndex, ObjectSelectionFlags::InUse);
                }
                break;
            }
            case TileElementType::SmallScenery:
                type = iter.element->AsSmallScenery()->GetEntryIndex();
                Editor::SetSelectedObject(ObjectType::smallScenery, type, ObjectSelectionFlags::InUse);
                break;
            case TileElementType::Entrance:
            {
                auto parkEntranceEl = iter.element->AsEntrance();
                if (parkEntranceEl->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                    break;

                type = iter.element->AsEntrance()->getEntryIndex();
                Editor::SetSelectedObject(ObjectType::parkEntrance, type, ObjectSelectionFlags::InUse);

                // Skip if not the middle part
                if (parkEntranceEl->GetSequenceIndex() != 0)
                    break;

                auto legacyPathEntryIndex = parkEntranceEl->GetLegacyPathEntryIndex();
                if (legacyPathEntryIndex == kObjectEntryIndexNull)
                {
                    auto surfaceEntryIndex = parkEntranceEl->GetSurfaceEntryIndex();
                    Editor::SetSelectedObject(ObjectType::footpathSurface, surfaceEntryIndex, ObjectSelectionFlags::InUse);
                }
                else
                {
                    Editor::SetSelectedObject(ObjectType::paths, legacyPathEntryIndex, ObjectSelectionFlags::InUse);
                }
                break;
            }
            case TileElementType::Wall:
                type = iter.element->AsWall()->GetEntryIndex();
                Editor::SetSelectedObject(ObjectType::walls, type, ObjectSelectionFlags::InUse);
                break;
            case TileElementType::LargeScenery:
                type = iter.element->AsLargeScenery()->GetEntryIndex();
                Editor::SetSelectedObject(ObjectType::largeScenery, type, ObjectSelectionFlags::InUse);
                break;
            case TileElementType::Banner:
            {
                auto banner = iter.element->AsBanner()->GetBanner();
                if (banner != nullptr)
                {
                    type = banner->type;
                    Editor::SetSelectedObject(ObjectType::banners, type, ObjectSelectionFlags::InUse);
                }
                break;
            }
        }
    } while (TileElementIteratorNext(&iter));

    auto& gameState = getGameState();
    for (auto& ride : RideManager(gameState))
    {
        Editor::SetSelectedObject(ObjectType::ride, ride.subtype, ObjectSelectionFlags::InUse);
        Editor::SetSelectedObject(ObjectType::station, ride.entranceStyle, ObjectSelectionFlags::InUse);
        Editor::SetSelectedObject(ObjectType::music, ride.music, ObjectSelectionFlags::InUse);
    }

    ObjectEntryIndex lastIndex = kObjectEntryIndexNull;
    for (auto* peep : EntityList<Guest>())
    {
        if (peep->AnimationObjectIndex == lastIndex)
            continue;

        lastIndex = peep->AnimationObjectIndex;
        Editor::SetSelectedObject(ObjectType::peepAnimations, lastIndex, ObjectSelectionFlags::InUse);
    }
    for (auto* peep : EntityList<Staff>())
    {
        if (peep->AnimationObjectIndex == lastIndex)
            continue;

        lastIndex = peep->AnimationObjectIndex;
        Editor::SetSelectedObject(ObjectType::peepAnimations, lastIndex, ObjectSelectionFlags::InUse);
    }

    // Apply selected object status for hacked vehicles that may not have an associated ride
    for (auto* vehicle : TrainManager::View())
    {
        ObjectEntryIndex type = vehicle->ride_subtype;
        if (type != kObjectEntryIndexNull) // cable lifts use index null. Ignore them
        {
            Editor::SetSelectedObject(ObjectType::ride, type, ObjectSelectionFlags::InUse);
        }
    }
    for (auto vehicle : EntityList<Vehicle>())
    {
        ObjectEntryIndex type = vehicle->ride_subtype;
        if (type != kObjectEntryIndexNull) // cable lifts use index null. Ignore them
        {
            Editor::SetSelectedObject(ObjectType::ride, type, ObjectSelectionFlags::InUse);
        }
    }

    auto numObjects = ObjectRepositoryGetItemsCount();
    const auto* items = ObjectRepositoryGetItems();
    for (size_t i = 0; i < numObjects; i++)
    {
        auto* selectionFlags = &_objectSelectionFlags[i];
        const auto* item = &items[i];
        *selectionFlags &= ~ObjectSelectionFlags::InUse;

        if (item->LoadedObject != nullptr)
        {
            auto objectType = item->LoadedObject->GetObjectType();
            auto entryIndex = objectMgr.GetLoadedObjectEntryIndex(item->LoadedObject.get());
            *selectionFlags |= Editor::GetSelectedObjectFlags(objectType, entryIndex);
        }
    }
}

/**
 *
 *  rct2: 0x006AB211
 */
void Sub6AB211()
{
    int32_t numObjects = static_cast<int32_t>(ObjectRepositoryGetItemsCount());
    _objectSelectionFlags = std::vector<uint8_t>(numObjects);

    for (uint8_t objectType = 0; objectType < EnumValue(ObjectType::count); objectType++)
    {
        _numSelectedObjectsForType[objectType] = 0;
        _numAvailableObjectsForType[objectType] = 0;
    }

    const ObjectRepositoryItem* items = ObjectRepositoryGetItems();
    for (int32_t i = 0; i < numObjects; i++)
    {
        ObjectType objectType = items[i].Type;
        _numAvailableObjectsForType[EnumValue(objectType)]++;
    }

    if (gLegacyScene == LegacyScene::trackDesigner)
    {
        SetupTrackDesignerObjects();
    }

    if (gLegacyScene == LegacyScene::trackDesignsManager)
    {
        SetupTrackManagerObjects();
    }

    SetupInUseSelectionFlags();
    ResetSelectedObjectCountAndSize();

    if (!(isInTrackDesignerOrManager()))
    {
        // To prevent it breaking in scenario mode.
        if (gLegacyScene == LegacyScene::scenarioEditor)
        {
            selectScenarioEditorObjects();
        }
    }

    ResetSelectedObjectCountAndSize();
}

/**
 *
 *  rct2: 0x006AB316
 */
void EditorObjectFlagsClear()
{
    _objectSelectionFlags.clear();
    _objectSelectionFlags.shrink_to_fit();
}

/**
 *
 *  rct2: 0x00685791
 */
static void RemoveSelectedObjectsFromResearch(const ObjectEntryDescriptor& descriptor)
{
    auto& objManager = GetContext()->GetObjectManager();
    auto obj = objManager.GetLoadedObject(descriptor);
    if (obj != nullptr)
    {
        auto entryIndex = objManager.GetLoadedObjectEntryIndex(obj);
        switch (obj->GetObjectType())
        {
            case ObjectType::ride:
            {
                auto rideEntry = GetRideEntryByIndex(entryIndex);
                for (auto rideType : rideEntry->ride_type)
                {
                    ResearchItem tmp = {};
                    tmp.type = Research::EntryType::ride;
                    tmp.entryIndex = entryIndex;
                    tmp.baseRideType = rideType;
                    ResearchRemove(tmp);
                }
                break;
            }
            case ObjectType::sceneryGroup:
            {
                ResearchItem tmp = {};
                tmp.type = Research::EntryType::scenery;
                tmp.entryIndex = entryIndex;
                tmp.baseRideType = 0;
                ResearchRemove(tmp);
                break;
            }
            default:
                break;
        }
    }
}

/**
 *
 *  rct2: 0x006ABB66
 */
void UnloadUnselectedObjects()
{
    auto numItems = static_cast<int32_t>(ObjectRepositoryGetItemsCount());
    const auto* items = ObjectRepositoryGetItems();
    std::vector<ObjectEntryDescriptor> objectsToUnload;

    for (int32_t i = 0; i < numItems; i++)
    {
        if (!(_objectSelectionFlags[i] & ObjectSelectionFlags::Selected))
        {
            auto descriptor = ObjectEntryDescriptor(items[i]);
            if (!IsIntransientObjectType(items[i].Type))
            {
                RemoveSelectedObjectsFromResearch(descriptor);
                objectsToUnload.push_back(descriptor);
            }
        }
    }
    ObjectManagerUnloadObjects(objectsToUnload);
}

/**
 *
 *  rct2: 0x006AA805
 */
static void selectScenarioEditorObjects()
{
    if (_numSelectedObjectsForType[0] == 0)
    {
        for (auto designerSelectedObject : kCommonScenarioAndTrackDesignerObjects)
        {
            WindowEditorObjectSelectionSelectObject(
                0, { EditorInputFlag::select, EditorInputFlag::unk1, EditorInputFlag::selectObjectsInSceneryGroup },
                ObjectEntryDescriptor(designerSelectedObject));
        }
        for (auto defaultSelectedObject : kDefaultScenarioObjects)
        {
            WindowEditorObjectSelectionSelectObject(
                0, { EditorInputFlag::select, EditorInputFlag::unk1, EditorInputFlag::selectObjectsInSceneryGroup },
                ObjectEntryDescriptor(defaultSelectedObject));
        }
    }
}

static void selectTrackDesignerObjects()
{
    if (_numSelectedObjectsForType[0] == 0)
    {
        for (auto designerSelectedObject : kCommonScenarioAndTrackDesignerObjects)
        {
            WindowEditorObjectSelectionSelectObject(
                0, { EditorInputFlag::select, EditorInputFlag::unk1, EditorInputFlag::selectObjectsInSceneryGroup },
                ObjectEntryDescriptor(designerSelectedObject));
        }
    }
}

/**
 * Replaces the previously selected water palette with the palette in the specified item immediately.
 */
static void ReplaceSelectedWaterPalette(const ObjectRepositoryItem* item)
{
    auto& objectManager = GetContext()->GetObjectManager();
    auto* oldPalette = objectManager.GetLoadedObject<WaterObject>(0);

    if (oldPalette != nullptr)
    {
        const std::vector<ObjectEntryDescriptor> oldEntries = { oldPalette->GetDescriptor() };
        objectManager.UnloadObjects(oldEntries);
    }

    auto newPaletteEntry = ObjectEntryDescriptor(*item);
    if (objectManager.GetLoadedObject(newPaletteEntry) != nullptr || objectManager.LoadObject(newPaletteEntry) != nullptr)
    {
        LoadPalette();
    }
    else
    {
        LOG_ERROR("Failed to load selected palette %s", std::string(newPaletteEntry.GetName()).c_str());
    }
}

/**
 *
 *  rct2: 0x006AA770
 */
void ResetSelectedObjectCountAndSize()
{
    for (auto& objectType : _numSelectedObjectsForType)
    {
        objectType = 0;
    }

    int32_t numObjects = static_cast<int32_t>(ObjectRepositoryGetItemsCount());
    const ObjectRepositoryItem* items = ObjectRepositoryGetItems();
    for (int32_t i = 0; i < numObjects; i++)
    {
        ObjectType objectType = items[i].Type;
        if (_objectSelectionFlags[i] & ObjectSelectionFlags::Selected)
        {
            _numSelectedObjectsForType[EnumValue(objectType)]++;
        }
    }
}

void FinishObjectSelection()
{
    auto& gameState = getGameState();
    if (gLegacyScene == LegacyScene::trackDesigner)
    {
        SetEveryRideTypeInvented();
        SetEveryRideEntryInvented();

        auto& objManager = GetContext()->GetObjectManager();
        gameState.lastEntranceStyle = objManager.GetLoadedObjectEntryIndex("rct2.station.plain");
        if (gameState.lastEntranceStyle == kObjectEntryIndexNull)
        {
            gameState.lastEntranceStyle = 0;
        }

        gameState.editorStep = EditorStep::RollercoasterDesigner;
        GfxInvalidateScreen();
    }
    else
    {
        SetAllSceneryItemsInvented();

        auto intent = Intent(INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG);
        ContextBroadcastIntent(&intent);

        gameState.editorStep = EditorStep::LandscapeEditor;
        GfxInvalidateScreen();
    }
}

/**
 *
 *  rct2: 0x006AB54F
 */
ResultWithMessage WindowEditorObjectSelectionSelectObject(
    uint8_t isMasterObject, EditorInputFlags flags, const ObjectRepositoryItem* item)
{
    if (item == nullptr)
    {
        return ObjectSelectionError(isMasterObject, STR_OBJECT_SELECTION_ERR_OBJECT_DATA_NOT_FOUND);
    }

    int32_t numObjects = static_cast<int32_t>(ObjectRepositoryGetItemsCount());
    // Get repository item index
    int32_t index = -1;
    const ObjectRepositoryItem* items = ObjectRepositoryGetItems();
    for (int32_t i = 0; i < numObjects; i++)
    {
        if (&items[i] == item)
        {
            index = i;
        }
    }

    uint8_t* selectionFlags = &_objectSelectionFlags[index];
    if (!flags.has(EditorInputFlag::select))
    {
        if (!(*selectionFlags & ObjectSelectionFlags::Selected))
        {
            return { true };
        }

        if (*selectionFlags & ObjectSelectionFlags::InUse)
        {
            return ObjectSelectionError(isMasterObject, STR_OBJECT_SELECTION_ERR_CURRENTLY_IN_USE);
        }

        if (*selectionFlags & ObjectSelectionFlags::AlwaysRequired)
        {
            return ObjectSelectionError(isMasterObject, STR_OBJECT_SELECTION_ERR_ALWAYS_REQUIRED);
        }

        ObjectType objectType = item->Type;
        if (objectType == ObjectType::sceneryGroup && flags.has(EditorInputFlag::selectObjectsInSceneryGroup))
        {
            for (const auto& sgEntry : item->SceneryGroupInfo.Entries)
            {
                WindowEditorObjectSelectionSelectObject(++isMasterObject, flags, sgEntry);
            }
        }

        _numSelectedObjectsForType[EnumValue(objectType)]--;
        *selectionFlags &= ~ObjectSelectionFlags::Selected;
        return { true };
    }

    if (isMasterObject == 0)
    {
        if (flags.has(EditorInputFlag::objectAlwaysRequired))
        {
            *selectionFlags |= ObjectSelectionFlags::AlwaysRequired;
        }
    }

    if (*selectionFlags & ObjectSelectionFlags::Selected)
    {
        return { true };
    }

    if (item->Flags & IsCompatibilityObject)
    {
        return ObjectSelectionError(isMasterObject, STR_OBJECT_SELECTION_ERR_COMPAT_OBJECT);
    }

    ObjectType objectType = item->Type;
    auto maxObjects = getObjectEntryGroupCount(objectType);

    if (maxObjects <= _numSelectedObjectsForType[EnumValue(objectType)])
    {
        return ObjectSelectionError(isMasterObject, STR_OBJECT_SELECTION_ERR_TOO_MANY_OF_TYPE_SELECTED);
    }

    if (objectType == ObjectType::sceneryGroup && flags.has(EditorInputFlag::selectObjectsInSceneryGroup))
    {
        for (const auto& sgEntry : item->SceneryGroupInfo.Entries)
        {
            const auto selectionResult = WindowEditorObjectSelectionSelectObject(++isMasterObject, flags, sgEntry);
            if (!selectionResult.Successful)
            {
                _gSceneryGroupPartialSelectError = selectionResult.Message;
                LOG_ERROR("Could not find object: %s", std::string(sgEntry.GetName()).c_str());
            }
        }
    }
    else if (objectType == ObjectType::water)
    {
        // Replace old palette with newly selected palette immediately.
        ReplaceSelectedWaterPalette(item);
    }
    else if (objectType == ObjectType::peepNames)
    {
        PeepUpdateNames();
    }

    if (isMasterObject != 0 && !flags.has(EditorInputFlag::unk1))
    {
        char objectName[64];
        ObjectCreateIdentifierName(objectName, 64, &item->ObjectEntry);
        auto ft = Formatter::Common();
        ft.Add<const char*>(objectName);
        return ObjectSelectionError(isMasterObject, STR_OBJECT_SELECTION_ERR_SHOULD_SELECT_X_FIRST);
    }

    if (maxObjects <= _numSelectedObjectsForType[EnumValue(objectType)])
    {
        return ObjectSelectionError(isMasterObject, STR_OBJECT_SELECTION_ERR_TOO_MANY_OF_TYPE_SELECTED);
    }

    _numSelectedObjectsForType[EnumValue(objectType)]++;

    *selectionFlags |= ObjectSelectionFlags::Selected;
    return { true };
}

ResultWithMessage WindowEditorObjectSelectionSelectObject(
    uint8_t isMasterObject, EditorInputFlags flags, const ObjectEntryDescriptor& descriptor)
{
    auto& objectRepository = GetContext()->GetObjectRepository();
    const auto* item = objectRepository.FindObject(descriptor);
    return WindowEditorObjectSelectionSelectObject(isMasterObject, flags, item);
}

bool EditorCheckObjectGroupAtLeastOneSelected(ObjectType checkObjectType)
{
    auto numObjects = std::min(ObjectRepositoryGetItemsCount(), _objectSelectionFlags.size());
    const ObjectRepositoryItem* items = ObjectRepositoryGetItems();

    for (size_t i = 0; i < numObjects; i++)
    {
        auto objectType = items[i].Type;
        if (checkObjectType == objectType && (_objectSelectionFlags[i] & ObjectSelectionFlags::Selected))
        {
            return true;
        }
    }
    return false;
}

bool EditorCheckObjectGroupAtLeastOneOfPeepTypeSelected(uint8_t peepType)
{
    auto numObjects = std::min(ObjectRepositoryGetItemsCount(), _objectSelectionFlags.size());
    const ObjectRepositoryItem* items = ObjectRepositoryGetItems();

    for (size_t i = 0; i < numObjects; i++)
    {
        const auto isAnimObjectType = items[i].Type == ObjectType::peepAnimations;
        const bool isSelected = _objectSelectionFlags[i] & ObjectSelectionFlags::Selected;
        if (isAnimObjectType && isSelected && items[i].PeepAnimationsInfo.PeepType == peepType)
        {
            return true;
        }
    }
    return false;
}

bool EditorCheckObjectGroupAtLeastOneSurfaceSelected(bool queue)
{
    auto numObjects = std::min(ObjectRepositoryGetItemsCount(), _objectSelectionFlags.size());
    const auto* items = ObjectRepositoryGetItems();
    for (size_t i = 0; i < numObjects; i++)
    {
        const auto& ori = items[i];
        auto isQueue = (ori.FootpathSurfaceInfo.Flags & FOOTPATH_ENTRY_FLAG_IS_QUEUE) != 0;
        if (ori.Type == ObjectType::footpathSurface && (_objectSelectionFlags[i] & ObjectSelectionFlags::Selected)
            && queue == isQueue)
        {
            return true;
        }
    }
    return false;
}

int32_t EditorRemoveUnusedObjects()
{
    Sub6AB211();
    SetupInUseSelectionFlags();

    int32_t numObjects = static_cast<int32_t>(ObjectRepositoryGetItemsCount());
    const ObjectRepositoryItem* items = ObjectRepositoryGetItems();

    int32_t numUnselectedObjects = 0;
    for (int32_t i = 0; i < numObjects; i++)
    {
        if (_objectSelectionFlags[i] & ObjectSelectionFlags::Selected)
        {
            if (!(_objectSelectionFlags[i] & ObjectSelectionFlags::InUse)
                && !(_objectSelectionFlags[i] & ObjectSelectionFlags::AlwaysRequired))
            {
                const ObjectRepositoryItem* item = &items[i];
                ObjectType objectType = item->Type;
                if (ObjectTypeIsIntransient(objectType))
                    continue;

                // The water type controls the entire palette. Removing that object can badly break the game state.
                if (objectType == ObjectType::water)
                    continue;

                // Avoid the used peep names object being deleted as no in-use checks are performed.
                if (objectType == ObjectType::peepNames)
                    continue;

                // Avoid deleting peep animation objects, as it ensures we don't delete the last ones for a kind of peep.
                if (objectType == ObjectType::peepAnimations)
                    continue;

                // Avoid deleting climate objects, as they're not bound to entities.
                if (objectType == ObjectType::climate)
                    continue;

                // It’s hard to determine exactly if a scenery group is used, so do not remove these automatically.
                if (objectType == ObjectType::sceneryGroup)
                    continue;

                _numSelectedObjectsForType[EnumValue(objectType)]--;
                _objectSelectionFlags[i] &= ~ObjectSelectionFlags::Selected;
                numUnselectedObjects++;
            }
        }
    }
    UnloadUnselectedObjects();
    EditorObjectFlagsClear();

    auto intent = Intent(INTENT_ACTION_REFRESH_SCENERY);
    ContextBroadcastIntent(&intent);

    return numUnselectedObjects;
}
