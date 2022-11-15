/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EditorObjectSelectionSession.h"

#include "Context.h"
#include "Editor.h"
#include "Game.h"
#include "OpenRCT2.h"
#include "drawing/Drawing.h"
#include "localisation/Formatter.h"
#include "localisation/Localisation.h"
#include "management/Research.h"
#include "object/DefaultObjects.h"
#include "object/ObjectList.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "ride/RideData.h"
#include "ride/TrainManager.h"
#include "ride/Vehicle.h"
#include "scenario/Scenario.h"
#include "windows/Intent.h"
#include "world/Footpath.h"
#include "world/LargeScenery.h"
#include "world/Scenery.h"

#include <iterator>
#include <vector>

std::optional<StringId> _gSceneryGroupPartialSelectError;
std::vector<uint8_t> _objectSelectionFlags;
int32_t _numSelectedObjectsForType[EnumValue(ObjectType::Count)];
static int32_t _numAvailableObjectsForType[EnumValue(ObjectType::Count)];

static void setup_in_use_selection_flags();
static void setup_track_designer_objects();
static void setup_track_manager_objects();
static void window_editor_object_selection_select_default_objects();
static void SelectDesignerObjects();
static void ReplaceSelectedWaterPalette(const ObjectRepositoryItem* item);

/**
 * Master objects are objects that are not
 * optional / required dependants of an
 * object.
 */
static constexpr ResultWithMessage ObjectSelectionError(bool isMasterObject, StringId message)
{
    if (!isMasterObject)
        reset_selected_object_count_and_size();

    return { false, message };
}

/**
 *
 *  rct2: 0x006ABCD1
 */
static void setup_track_manager_objects()
{
    int32_t numObjects = static_cast<int32_t>(object_repository_get_items_count());
    const ObjectRepositoryItem* items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++)
    {
        uint8_t* selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem* item = &items[i];
        if (item->Type == ObjectType::Ride)
        {
            *selectionFlags |= ObjectSelectionFlags::Flag6;

            for (auto rideType : item->RideInfo.RideType)
            {
                if (rideType != RIDE_TYPE_NULL)
                {
                    if (GetRideTypeDescriptor(rideType).Category != RIDE_CATEGORY_SHOP)
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
 *  rct2: 0x006ABC1E
 */
static void setup_track_designer_objects()
{
    int32_t numObjects = static_cast<int32_t>(object_repository_get_items_count());
    const ObjectRepositoryItem* items = object_repository_get_items();
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
                if (rideType != RIDE_TYPE_NULL)
                {
                    if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER))
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
void setup_in_use_selection_flags()
{
    auto& objectMgr = OpenRCT2::GetContext()->GetObjectManager();

    for (auto objectType : TransientObjectTypes)
    {
        for (int32_t i = 0; i < object_entry_group_counts[EnumValue(objectType)]; i++)
        {
            Editor::ClearSelectedObject(static_cast<ObjectType>(objectType), i, ObjectSelectionFlags::AllFlags);

            auto loadedObj = objectMgr.GetLoadedObject(static_cast<ObjectType>(objectType), i);
            if (loadedObj != nullptr)
            {
                Editor::SetSelectedObject(static_cast<ObjectType>(objectType), i, ObjectSelectionFlags::Selected);
            }
        }
    }

    tile_element_iterator iter;
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
                auto surfaceIndex = surfaceEl->GetSurfaceStyle();
                auto edgeIndex = surfaceEl->GetEdgeStyle();

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
                if (legacyPathEntryIndex == OBJECT_ENTRY_INDEX_NULL)
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
                    Editor::SetSelectedObject(ObjectType::PathBits, pathAdditionEntryIndex, ObjectSelectionFlags::InUse);
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

                Editor::SetSelectedObject(ObjectType::ParkEntrance, 0, ObjectSelectionFlags::InUse);

                // Skip if not the middle part
                if (parkEntranceEl->GetSequenceIndex() != 0)
                    break;

                auto legacyPathEntryIndex = parkEntranceEl->GetLegacyPathEntryIndex();
                if (legacyPathEntryIndex == OBJECT_ENTRY_INDEX_NULL)
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

    // Apply selected object status for hacked vehicles that may not have an associated ride
    for (auto* vehicle : TrainManager::View())
    {
        ObjectEntryIndex type = vehicle->ride_subtype;
        if (type != OBJECT_ENTRY_INDEX_NULL) // cable lifts use index null. Ignore them
        {
            Editor::SetSelectedObject(ObjectType::Ride, type, ObjectSelectionFlags::InUse);
        }
    }
    for (auto vehicle : EntityList<Vehicle>())
    {
        ObjectEntryIndex type = vehicle->ride_subtype;
        if (type != OBJECT_ENTRY_INDEX_NULL) // cable lifts use index null. Ignore them
        {
            Editor::SetSelectedObject(ObjectType::Ride, type, ObjectSelectionFlags::InUse);
        }
    }

    auto numObjects = object_repository_get_items_count();
    const auto* items = object_repository_get_items();
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
void sub_6AB211()
{
    int32_t numObjects = static_cast<int32_t>(object_repository_get_items_count());
    _objectSelectionFlags = std::vector<uint8_t>(numObjects);

    for (uint8_t objectType = 0; objectType < EnumValue(ObjectType::Count); objectType++)
    {
        _numSelectedObjectsForType[objectType] = 0;
        _numAvailableObjectsForType[objectType] = 0;
    }

    const ObjectRepositoryItem* items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++)
    {
        ObjectType objectType = items[i].Type;
        _numAvailableObjectsForType[EnumValue(objectType)]++;
    }

    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
    {
        setup_track_designer_objects();
    }

    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
    {
        setup_track_manager_objects();
    }

    setup_in_use_selection_flags();
    reset_selected_object_count_and_size();

    if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
    {
        // To prevent it breaking in scenario mode.
        if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        {
            window_editor_object_selection_select_default_objects();
        }
    }

    reset_selected_object_count_and_size();
}

/**
 *
 *  rct2: 0x006AB316
 */
void editor_object_flags_free()
{
    _objectSelectionFlags.clear();
    _objectSelectionFlags.shrink_to_fit();
}

/**
 *
 *  rct2: 0x00685791
 */
static void remove_selected_objects_from_research(const ObjectEntryDescriptor& descriptor)
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
                auto rideEntry = get_ride_entry(entryIndex);
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
void unload_unselected_objects()
{
    auto numItems = static_cast<int32_t>(object_repository_get_items_count());
    const auto* items = object_repository_get_items();
    std::vector<ObjectEntryDescriptor> objectsToUnload;

    for (int32_t i = 0; i < numItems; i++)
    {
        if (!(_objectSelectionFlags[i] & ObjectSelectionFlags::Selected))
        {
            auto descriptor = ObjectEntryDescriptor(items[i]);
            if (!IsIntransientObjectType(items[i].Type))
            {
                remove_selected_objects_from_research(descriptor);
                objectsToUnload.push_back(descriptor);
            }
        }
    }
    object_manager_unload_objects(objectsToUnload);
}

/**
 *
 *  rct2: 0x006AA805
 */
static void window_editor_object_selection_select_default_objects()
{
    if (_numSelectedObjectsForType[0] == 0)
    {
        for (auto defaultSelectedObject : DefaultSelectedObjects)
        {
            window_editor_object_selection_select_object(
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
            window_editor_object_selection_select_object(
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
        load_palette();
    }
    else
    {
        log_error("Failed to load selected palette %s", std::string(newPaletteEntry.GetName()).c_str());
    }
}

/**
 *
 *  rct2: 0x006AA770
 */
void reset_selected_object_count_and_size()
{
    for (auto& objectType : _numSelectedObjectsForType)
    {
        objectType = 0;
    }

    int32_t numObjects = static_cast<int32_t>(object_repository_get_items_count());
    const ObjectRepositoryItem* items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++)
    {
        ObjectType objectType = items[i].Type;
        if (_objectSelectionFlags[i] & ObjectSelectionFlags::Selected)
        {
            _numSelectedObjectsForType[EnumValue(objectType)]++;
        }
    }
}

void finish_object_selection()
{
    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
    {
        set_every_ride_type_invented();
        set_every_ride_entry_invented();
        gEditorStep = EditorStep::RollercoasterDesigner;
        gfx_invalidate_screen();
    }
    else
    {
        set_all_scenery_items_invented();
        ScenerySetDefaultPlacementConfiguration();
        gEditorStep = EditorStep::LandscapeEditor;
        gfx_invalidate_screen();
    }
}

/**
 *
 *  rct2: 0x006AB54F
 */
ResultWithMessage window_editor_object_selection_select_object(
    uint8_t isMasterObject, int32_t flags, const ObjectRepositoryItem* item)
{
    if (item == nullptr)
    {
        return ObjectSelectionError(isMasterObject, STR_OBJECT_SELECTION_ERR_OBJECT_DATA_NOT_FOUND);
    }

    int32_t numObjects = static_cast<int32_t>(object_repository_get_items_count());
    // Get repository item index
    int32_t index = -1;
    const ObjectRepositoryItem* items = object_repository_get_items();
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
                window_editor_object_selection_select_object(++isMasterObject, flags, sgEntry);
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

    ObjectType objectType = item->Type;
    uint16_t maxObjects = object_entry_group_counts[EnumValue(objectType)];

    if (maxObjects <= _numSelectedObjectsForType[EnumValue(objectType)])
    {
        return ObjectSelectionError(isMasterObject, STR_OBJECT_SELECTION_ERR_TOO_MANY_OF_TYPE_SELECTED);
    }

    if (objectType == ObjectType::SceneryGroup && (flags & INPUT_FLAG_EDITOR_OBJECT_SELECT_OBJECTS_IN_SCENERY_GROUP))
    {
        for (const auto& sgEntry : item->SceneryGroupInfo.Entries)
        {
            const auto selectionResult = window_editor_object_selection_select_object(++isMasterObject, flags, sgEntry);
            if (!selectionResult.Successful)
            {
                _gSceneryGroupPartialSelectError = selectionResult.Message;
            }
        }
    }
    else if (objectType == ObjectType::Water)
    {
        // Replace old palette with newly selected palette immediately.
        ReplaceSelectedWaterPalette(item);
    }

    if (isMasterObject != 0 && !(flags & INPUT_FLAG_EDITOR_OBJECT_1))
    {
        char objectName[64];
        object_create_identifier_name(objectName, 64, &item->ObjectEntry);
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

ResultWithMessage window_editor_object_selection_select_object(
    uint8_t isMasterObject, int32_t flags, const ObjectEntryDescriptor& descriptor)
{
    auto& objectRepository = OpenRCT2::GetContext()->GetObjectRepository();
    const auto* item = objectRepository.FindObject(descriptor);
    return window_editor_object_selection_select_object(isMasterObject, flags, item);
}

bool editor_check_object_group_at_least_one_selected(ObjectType checkObjectType)
{
    auto numObjects = std::min(object_repository_get_items_count(), _objectSelectionFlags.size());
    const ObjectRepositoryItem* items = object_repository_get_items();

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

bool editor_check_object_group_at_least_one_surface_selected(bool queue)
{
    auto numObjects = std::min(object_repository_get_items_count(), _objectSelectionFlags.size());
    const auto* items = object_repository_get_items();
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

int32_t editor_remove_unused_objects()
{
    sub_6AB211();
    setup_in_use_selection_flags();

    int32_t numObjects = static_cast<int32_t>(object_repository_get_items_count());
    const ObjectRepositoryItem* items = object_repository_get_items();

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

                if (objectType >= ObjectType::SceneryGroup)
                {
                    continue;
                }

                _numSelectedObjectsForType[EnumValue(objectType)]--;
                _objectSelectionFlags[i] &= ~ObjectSelectionFlags::Selected;
                numUnselectedObjects++;
            }
        }
    }
    unload_unselected_objects();
    editor_object_flags_free();

    auto intent = Intent(INTENT_ACTION_REFRESH_SCENERY);
    ContextBroadcastIntent(&intent);

    return numUnselectedObjects;
}
