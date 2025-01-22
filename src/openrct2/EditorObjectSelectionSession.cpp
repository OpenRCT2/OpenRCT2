/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "drawing/Drawing.h"
#include "entity/Guest.h"
#include "entity/Staff.h"
#include "localisation/Formatter.h"
#include "management/Research.h"
#include "object/DefaultObjects.h"
#include "object/FootpathEntry.h"
#include "object/ObjectList.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "ride/RideData.h"
#include "ride/RideManager.hpp"
#include "ride/TrainManager.h"
#include "ride/Vehicle.h"
#include "scenario/Scenario.h"
#include "windows/Intent.h"
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
uint32_t _numSelectedObjectsForType[EnumValue(ObjectType::Count)];
static int32_t _numAvailableObjectsForType[EnumValue(ObjectType::Count)];

static void SetupInUseSelectionFlags();
static void SetupTrackDesignerObjects();
static void SetupTrackManagerObjects();
static void WindowEditorObjectSelectionSelectDefaultObjects();
static void SelectDesignerObjects();
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
        if (item->Type == ObjectType::Ride)
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
    SelectDesignerObjects();
    for (int32_t i = 0; i < numObjects; i++)
    {
        uint8_t* selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem* item = &items[i];
        if (item->Type == ObjectType::Ride)
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
    auto& objectMgr = OpenRCT2::GetContext()->GetObjectManager();

    for (auto objectType : getTransientObjectTypes())
    {
        for (auto i = 0u; i < getObjectEntryGroupCount(objectType); i++)
        {
            Editor::ClearSelectedObject(static_cast<ObjectType>(objectType), i, ObjectSelectionFlags::AllFlags);

            auto loadedObj = objectMgr.GetLoadedObject(static_cast<ObjectType>(objectType), i);
            if (loadedObj != nullptr)
            {
                Editor::SetSelectedObject(static_cast<ObjectType>(objectType), i, ObjectSelectionFlags::Selected);
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

                Editor::SetSelectedObject(ObjectType::TerrainSurface, surfaceIndex, ObjectSelectionFlags::InUse);
                Editor::SetSelectedObject(ObjectType::TerrainEdge, edgeIndex, ObjectSelectionFlags::InUse);
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
                    Editor::SetSelectedObject(ObjectType::FootpathSurface, surfaceEntryIndex, ObjectSelectionFlags::InUse);
                    Editor::SetSelectedObject(ObjectType::FootpathRailings, railingEntryIndex, ObjectSelectionFlags::InUse);
                }
                else
                {
                    Editor::SetSelectedObject(ObjectType::Paths, legacyPathEntryIndex, ObjectSelectionFlags::InUse);
                }
                if (footpathEl->HasAddition())
                {
                    auto pathAdditionEntryIndex = footpathEl->GetAdditionEntryIndex();
                    Editor::SetSelectedObject(ObjectType::PathAdditions, pathAdditionEntryIndex, ObjectSelectionFlags::InUse);
                }
                break;
            }
            case TileElementType::SmallScenery:
                type = iter.element->AsSmallScenery()->GetEntryIndex();
                Editor::SetSelectedObject(ObjectType::SmallScenery, type, ObjectSelectionFlags::InUse);
                break;
            case TileElementType::Entrance:
            {
                auto parkEntranceEl = iter.element->AsEntrance();
                if (parkEntranceEl->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                    break;

                type = iter.element->AsEntrance()->getEntryIndex();
                Editor::SetSelectedObject(ObjectType::ParkEntrance, type, ObjectSelectionFlags::InUse);

                // Skip if not the middle part
                if (parkEntranceEl->GetSequenceIndex() != 0)
                    break;

                auto legacyPathEntryIndex = parkEntranceEl->GetLegacyPathEntryIndex();
                if (legacyPathEntryIndex == kObjectEntryIndexNull)
                {
                    auto surfaceEntryIndex = parkEntranceEl->GetSurfaceEntryIndex();
                    Editor::SetSelectedObject(ObjectType::FootpathSurface, surfaceEntryIndex, ObjectSelectionFlags::InUse);
                }
                else
                {
                    Editor::SetSelectedObject(ObjectType::Paths, legacyPathEntryIndex, ObjectSelectionFlags::InUse);
                }
                break;
            }
            case TileElementType::Wall:
                type = iter.element->AsWall()->GetEntryIndex();
                Editor::SetSelectedObject(ObjectType::Walls, type, ObjectSelectionFlags::InUse);
                break;
            case TileElementType::LargeScenery:
                type = iter.element->AsLargeScenery()->GetEntryIndex();
                Editor::SetSelectedObject(ObjectType::LargeScenery, type, ObjectSelectionFlags::InUse);
                break;
            case TileElementType::Banner:
            {
                auto banner = iter.element->AsBanner()->GetBanner();
                if (banner != nullptr)
                {
                    type = banner->type;
                    Editor::SetSelectedObject(ObjectType::Banners, type, ObjectSelectionFlags::InUse);
                }
                break;
            }
        }
    } while (TileElementIteratorNext(&iter));

    for (auto& ride : GetRideManager())
    {
        Editor::SetSelectedObject(ObjectType::Ride, ride.subtype, ObjectSelectionFlags::InUse);
        Editor::SetSelectedObject(ObjectType::Station, ride.entrance_style, ObjectSelectionFlags::InUse);
        Editor::SetSelectedObject(ObjectType::Music, ride.music, ObjectSelectionFlags::InUse);
    }

    ObjectEntryIndex lastIndex = kObjectEntryIndexNull;
    for (auto* peep : EntityList<Guest>())
    {
        if (peep->AnimationObjectIndex == lastIndex)
            continue;

        lastIndex = peep->AnimationObjectIndex;
        Editor::SetSelectedObject(ObjectType::PeepAnimations, lastIndex, ObjectSelectionFlags::InUse);
    }
    for (auto* peep : EntityList<Staff>())
    {
        if (peep->AnimationObjectIndex == lastIndex)
            continue;

        lastIndex = peep->AnimationObjectIndex;
        Editor::SetSelectedObject(ObjectType::PeepAnimations, lastIndex, ObjectSelectionFlags::InUse);
    }

    // Apply selected object status for hacked vehicles that may not have an associated ride
    for (auto* vehicle : TrainManager::View())
    {
        ObjectEntryIndex type = vehicle->ride_subtype;
        if (type != kObjectEntryIndexNull) // cable lifts use index null. Ignore them
        {
            Editor::SetSelectedObject(ObjectType::Ride, type, ObjectSelectionFlags::InUse);
        }
    }
    for (auto vehicle : EntityList<Vehicle>())
    {
        ObjectEntryIndex type = vehicle->ride_subtype;
        if (type != kObjectEntryIndexNull) // cable lifts use index null. Ignore them
        {
            Editor::SetSelectedObject(ObjectType::Ride, type, ObjectSelectionFlags::InUse);
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

    for (uint8_t objectType = 0; objectType < EnumValue(ObjectType::Count); objectType++)
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

    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
    {
        SetupTrackDesignerObjects();
    }

    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
    {
        SetupTrackManagerObjects();
    }

    SetupInUseSelectionFlags();
    ResetSelectedObjectCountAndSize();

    if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
    {
        // To prevent it breaking in scenario mode.
        if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        {
            WindowEditorObjectSelectionSelectDefaultObjects();
        }
    }

    ResetSelectedObjectCountAndSize();
}

/**
 *
 *  rct2: 0x006AB316
 */
void EditorObjectFlagsFree()
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
    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
    auto obj = objManager.GetLoadedObject(descriptor);
    if (obj != nullptr)
    {
        auto entryIndex = objManager.GetLoadedObjectEntryIndex(obj);
        switch (obj->GetObjectType())
        {
            case ObjectType::Ride:
            {
                auto rideEntry = GetRideEntryByIndex(entryIndex);
                for (auto rideType : rideEntry->ride_type)
                {
                    ResearchItem tmp = {};
                    tmp.type = Research::EntryType::Ride;
                    tmp.entryIndex = entryIndex;
                    tmp.baseRideType = rideType;
                    ResearchRemove(tmp);
                }
                break;
            }
            case ObjectType::SceneryGroup:
            {
                ResearchItem tmp = {};
                tmp.type = Research::EntryType::Scenery;
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
static void WindowEditorObjectSelectionSelectDefaultObjects()
{
    if (_numSelectedObjectsForType[0] == 0)
    {
        for (auto defaultSelectedObject : DefaultSelectedObjects)
        {
            WindowEditorObjectSelectionSelectObject(
                0,
                INPUT_FLAG_EDITOR_OBJECT_SELECT | INPUT_FLAG_EDITOR_OBJECT_1
                    | INPUT_FLAG_EDITOR_OBJECT_SELECT_OBJECTS_IN_SCENERY_GROUP,
                ObjectEntryDescriptor(defaultSelectedObject));
        }
    }
}

static void SelectDesignerObjects()
{
    if (_numSelectedObjectsForType[0] == 0)
    {
        for (auto designerSelectedObject : DesignerSelectedObjects)
        {
            WindowEditorObjectSelectionSelectObject(
                0,
                INPUT_FLAG_EDITOR_OBJECT_SELECT | INPUT_FLAG_EDITOR_OBJECT_1
                    | INPUT_FLAG_EDITOR_OBJECT_SELECT_OBJECTS_IN_SCENERY_GROUP,
                ObjectEntryDescriptor(designerSelectedObject));
        }
    }
}

/**
 * Replaces the previously selected water palette with the palette in the specified item immediately.
 */
static void ReplaceSelectedWaterPalette(const ObjectRepositoryItem* item)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    auto* oldPalette = objectManager.GetLoadedObject(ObjectType::Water, 0);

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
    auto& gameState = GetGameState();
    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
    {
        SetEveryRideTypeInvented();
        SetEveryRideEntryInvented();

        auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
        gameState.LastEntranceStyle = objManager.GetLoadedObjectEntryIndex("rct2.station.plain");
        if (gameState.LastEntranceStyle == kObjectEntryIndexNull)
        {
            gameState.LastEntranceStyle = 0;
        }

        gameState.EditorStep = EditorStep::RollercoasterDesigner;
        GfxInvalidateScreen();
    }
    else
    {
        SetAllSceneryItemsInvented();

        auto intent = Intent(INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG);
        ContextBroadcastIntent(&intent);

        gameState.EditorStep = EditorStep::LandscapeEditor;
        GfxInvalidateScreen();
    }
}

/**
 *
 *  rct2: 0x006AB54F
 */
ResultWithMessage WindowEditorObjectSelectionSelectObject(
    uint8_t isMasterObject, int32_t flags, const ObjectRepositoryItem* item)
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
    if (!(flags & INPUT_FLAG_EDITOR_OBJECT_SELECT))
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
        if (objectType == ObjectType::SceneryGroup && (flags & INPUT_FLAG_EDITOR_OBJECT_SELECT_OBJECTS_IN_SCENERY_GROUP))
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
        if (flags & INPUT_FLAG_EDITOR_OBJECT_ALWAYS_REQUIRED)
        {
            *selectionFlags |= ObjectSelectionFlags::AlwaysRequired;
        }
    }

    if (*selectionFlags & ObjectSelectionFlags::Selected)
    {
        return { true };
    }

    if (item->Flags & ObjectItemFlags::IsCompatibilityObject)
    {
        return ObjectSelectionError(isMasterObject, STR_OBJECT_SELECTION_ERR_COMPAT_OBJECT);
    }

    ObjectType objectType = item->Type;
    auto maxObjects = getObjectEntryGroupCount(objectType);

    if (maxObjects <= _numSelectedObjectsForType[EnumValue(objectType)])
    {
        return ObjectSelectionError(isMasterObject, STR_OBJECT_SELECTION_ERR_TOO_MANY_OF_TYPE_SELECTED);
    }

    if (objectType == ObjectType::SceneryGroup && (flags & INPUT_FLAG_EDITOR_OBJECT_SELECT_OBJECTS_IN_SCENERY_GROUP))
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
    else if (objectType == ObjectType::Water)
    {
        // Replace old palette with newly selected palette immediately.
        ReplaceSelectedWaterPalette(item);
    }
    else if (objectType == ObjectType::PeepNames)
    {
        PeepUpdateNames();
    }

    if (isMasterObject != 0 && !(flags & INPUT_FLAG_EDITOR_OBJECT_1))
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
    uint8_t isMasterObject, int32_t flags, const ObjectEntryDescriptor& descriptor)
{
    auto& objectRepository = OpenRCT2::GetContext()->GetObjectRepository();
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
        const auto isAnimObjectType = items[i].Type == ObjectType::PeepAnimations;
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
        if (ori.Type == ObjectType::FootpathSurface && (_objectSelectionFlags[i] & ObjectSelectionFlags::Selected)
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
                if (objectType == ObjectType::Water)
                    continue;

                // Avoid the used peep names object being deleted as no in-use checks are performed.
                if (objectType == ObjectType::PeepNames)
                    continue;

                // Avoid deleting peep animation objects, as it ensures we don't delete the last ones for a kind of peep.
                if (objectType == ObjectType::PeepAnimations)
                    continue;

                // It’s hard to determine exactly if a scenery group is used, so do not remove these automatically.
                if (objectType == ObjectType::SceneryGroup)
                    continue;

                _numSelectedObjectsForType[EnumValue(objectType)]--;
                _objectSelectionFlags[i] &= ~ObjectSelectionFlags::Selected;
                numUnselectedObjects++;
            }
        }
    }
    UnloadUnselectedObjects();
    EditorObjectFlagsFree();

    auto intent = Intent(INTENT_ACTION_REFRESH_SCENERY);
    ContextBroadcastIntent(&intent);

    return numUnselectedObjects;
}
