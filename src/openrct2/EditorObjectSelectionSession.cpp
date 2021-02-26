/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include "localisation/Localisation.h"
#include "management/Research.h"
#include "object/DefaultObjects.h"
#include "object/ObjectList.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "ride/RideData.h"
#include "ride/Vehicle.h"
#include "scenario/Scenario.h"
#include "windows/Intent.h"
#include "world/Footpath.h"
#include "world/LargeScenery.h"

#include <iterator>
#include <vector>

bool _maxObjectsWasHit;
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
        ObjectType object_type = item->ObjectEntry.GetType();
        if (object_type == ObjectType::Ride)
        {
            *selectionFlags |= OBJECT_SELECTION_FLAG_6;

            for (auto rideType : item->RideInfo.RideType)
            {
                if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_HAS_TRACK))
                {
                    *selectionFlags &= ~OBJECT_SELECTION_FLAG_6;
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
static void setup_track_designer_objects()
{
    int32_t numObjects = static_cast<int32_t>(object_repository_get_items_count());
    const ObjectRepositoryItem* items = object_repository_get_items();
    SelectDesignerObjects();
    for (int32_t i = 0; i < numObjects; i++)
    {
        uint8_t* selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem* item = &items[i];
        ObjectType objectType = item->ObjectEntry.GetType();
        if (objectType == ObjectType::Ride)
        {
            *selectionFlags |= OBJECT_SELECTION_FLAG_6;

            for (uint8_t rideType : item->RideInfo.RideType)
            {
                if (rideType != RIDE_TYPE_NULL)
                {
                    if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER))
                    {
                        *selectionFlags &= ~OBJECT_SELECTION_FLAG_6;
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

    for (uint8_t objectType = 0; objectType < EnumValue(ObjectType::Count); objectType++)
    {
        for (int32_t i = 0; i < object_entry_group_counts[objectType]; i++)
        {
            Editor::ClearSelectedObject(static_cast<ObjectType>(objectType), i, OBJECT_SELECTION_FLAG_ALL);

            auto loadedObj = objectMgr.GetLoadedObject(static_cast<ObjectType>(objectType), i);
            if (loadedObj != nullptr)
            {
                Editor::SetSelectedObject(static_cast<ObjectType>(objectType), i, OBJECT_SELECTION_FLAG_2);
            }
        }
    }

    tile_element_iterator iter;
    tile_element_iterator_begin(&iter);
    do
    {
        ObjectEntryIndex type;

        switch (iter.element->GetType())
        {
            default:
            case TILE_ELEMENT_TYPE_SURFACE:
            case TILE_ELEMENT_TYPE_TRACK:
                break;
            case TILE_ELEMENT_TYPE_PATH:
                type = iter.element->AsPath()->GetSurfaceEntryIndex();
                assert(type < object_entry_group_counts[EnumValue(ObjectType::Paths)]);
                Editor::SetSelectedObject(ObjectType::Paths, type, OBJECT_SELECTION_FLAG_SELECTED);

                if (iter.element->AsPath()->HasAddition())
                {
                    uint8_t path_additions = iter.element->AsPath()->GetAdditionEntryIndex();
                    Editor::SetSelectedObject(ObjectType::PathBits, path_additions, OBJECT_SELECTION_FLAG_SELECTED);
                }
                break;
            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                type = iter.element->AsSmallScenery()->GetEntryIndex();
                assert(type < object_entry_group_counts[EnumValue(ObjectType::SmallScenery)]);
                Editor::SetSelectedObject(ObjectType::SmallScenery, type, OBJECT_SELECTION_FLAG_SELECTED);
                break;
            case TILE_ELEMENT_TYPE_ENTRANCE:
                if (iter.element->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                    break;
                // Skip if not the middle part
                if (iter.element->AsEntrance()->GetSequenceIndex() != 0)
                    break;

                Editor::SetSelectedObject(ObjectType::ParkEntrance, 0, OBJECT_SELECTION_FLAG_SELECTED);

                type = iter.element->AsEntrance()->GetPathType();
                assert(type < object_entry_group_counts[EnumValue(ObjectType::Paths)]);
                Editor::SetSelectedObject(ObjectType::Paths, type, OBJECT_SELECTION_FLAG_SELECTED);
                break;
            case TILE_ELEMENT_TYPE_WALL:
                type = iter.element->AsWall()->GetEntryIndex();
                assert(type < object_entry_group_counts[EnumValue(ObjectType::Walls)]);
                Editor::SetSelectedObject(ObjectType::Walls, type, OBJECT_SELECTION_FLAG_SELECTED);
                break;
            case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                type = iter.element->AsLargeScenery()->GetEntryIndex();
                assert(type < object_entry_group_counts[EnumValue(ObjectType::LargeScenery)]);
                Editor::SetSelectedObject(ObjectType::LargeScenery, type, OBJECT_SELECTION_FLAG_SELECTED);
                break;
            case TILE_ELEMENT_TYPE_BANNER:
            {
                auto banner = iter.element->AsBanner()->GetBanner();
                if (banner != nullptr)
                {
                    type = banner->type;
                    assert(type < object_entry_group_counts[EnumValue(ObjectType::Banners)]);
                    Editor::SetSelectedObject(ObjectType::Banners, type, OBJECT_SELECTION_FLAG_SELECTED);
                }
                break;
            }
        }
    } while (tile_element_iterator_next(&iter));

    for (ride_id_t ride_index = 0; ride_index < MAX_RIDES; ride_index++)
    {
        auto ride = get_ride(ride_index);
        if (ride != nullptr)
        {
            ObjectEntryIndex type = ride->subtype;
            Editor::SetSelectedObject(ObjectType::Ride, type, OBJECT_SELECTION_FLAG_SELECTED);
        }
    }

    // Apply selected object status for hacked vehicles that may not have an associated ride
    for (auto vehicle : EntityList<Vehicle>(EntityListId::TrainHead))
    {
        ObjectEntryIndex type = vehicle->ride_subtype;
        if (type != RIDE_ENTRY_INDEX_NULL) // cable lifts use index null. Ignore them
        {
            Editor::SetSelectedObject(ObjectType::Ride, type, OBJECT_SELECTION_FLAG_SELECTED);
        }
    }
    for (auto vehicle : EntityList<Vehicle>(EntityListId::Vehicle))
    {
        ObjectEntryIndex type = vehicle->ride_subtype;
        if (type != RIDE_ENTRY_INDEX_NULL) // cable lifts use index null. Ignore them
        {
            Editor::SetSelectedObject(ObjectType::Ride, type, OBJECT_SELECTION_FLAG_SELECTED);
        }
    }

    int32_t numObjects = static_cast<int32_t>(object_repository_get_items_count());
    const ObjectRepositoryItem* items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++)
    {
        uint8_t* selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem* item = &items[i];
        *selectionFlags &= ~OBJECT_SELECTION_FLAG_IN_USE;

        ObjectType entryType;
        ObjectEntryIndex entryIndex;
        if (find_object_in_entry_group(&item->ObjectEntry, &entryType, &entryIndex))
        {
            auto flags = Editor::GetSelectedObjectFlags(entryType, entryIndex);
            if (flags & OBJECT_SELECTION_FLAG_SELECTED)
            {
                *selectionFlags |= OBJECT_SELECTION_FLAG_IN_USE | OBJECT_SELECTION_FLAG_SELECTED;
            }
            if (flags & OBJECT_SELECTION_FLAG_2)
            {
                *selectionFlags |= OBJECT_SELECTION_FLAG_SELECTED;
            }
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
        ObjectType objectType = items[i].ObjectEntry.GetType();
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
static void remove_selected_objects_from_research(const rct_object_entry* installedObject)
{
    ObjectType entry_type;
    ObjectEntryIndex entry_index;
    if (!find_object_in_entry_group(installedObject, &entry_type, &entry_index))
        return;

    if (entry_type == ObjectType::Ride)
    {
        auto rideEntry = get_ride_entry(entry_index);

        for (auto rideType : rideEntry->ride_type)
        {
            ResearchItem tmp = {};
            tmp.type = Research::EntryType::Ride;
            tmp.entryIndex = entry_index;
            tmp.baseRideType = rideType;
            research_remove(&tmp);
        }
    }
    else if (entry_type == ObjectType::SceneryGroup)
    {
        ResearchItem tmp = {};
        tmp.type = Research::EntryType::Scenery;
        tmp.entryIndex = entry_index;
        research_remove(&tmp);
    }
}

/**
 *
 *  rct2: 0x006ABB66
 */
void unload_unselected_objects()
{
    int32_t numItems = static_cast<int32_t>(object_repository_get_items_count());
    const ObjectRepositoryItem* items = object_repository_get_items();
    std::vector<rct_object_entry> objectsToUnload;

    for (int32_t i = 0; i < numItems; i++)
    {
        if (!(_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_SELECTED))
        {
            const rct_object_entry* entry = &items[i].ObjectEntry;

            remove_selected_objects_from_research(entry);
            objectsToUnload.push_back(*entry);
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
                defaultSelectedObject);
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
                designerSelectedObject);
        }
    }
}

/**
 * Replaces the previously selected water palette with the palette in the specified item immediately.
 */
static void ReplaceSelectedWaterPalette(const ObjectRepositoryItem* item)
{
    auto& objectManager = OpenRCT2::GetContext()->GetObjectManager();
    Object* oldPalette = objectManager.GetLoadedObject(ObjectType::Water, 0);

    if (oldPalette != nullptr)
    {
        const std::vector<rct_object_entry> oldEntries = { *(oldPalette->GetObjectEntry()) };
        objectManager.UnloadObjects(oldEntries);
    }

    const rct_object_entry* newPaletteEntry = &item->ObjectEntry;

    if (objectManager.GetLoadedObject(newPaletteEntry) != nullptr || objectManager.LoadObject(newPaletteEntry) != nullptr)
    {
        load_palette();
    }
    else
    {
        log_error("Failed to load selected palette %.8s", newPaletteEntry->name);
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
        ObjectType objectType = items[i].ObjectEntry.GetType();
        if (_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_SELECTED)
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
        gS6Info.editor_step = EditorStep::RollercoasterDesigner;
        gfx_invalidate_screen();
    }
    else
    {
        set_all_scenery_items_invented();
        scenery_set_default_placement_configuration();
        gS6Info.editor_step = EditorStep::LandscapeEditor;
        gfx_invalidate_screen();
    }
}

/**
 * Master objects are objects that are not
 * optional / required dependants of an
 * object.
 */
static void set_object_selection_error(uint8_t is_master_object, rct_string_id error_msg)
{
    gGameCommandErrorText = error_msg;
    if (!is_master_object)
    {
        reset_selected_object_count_and_size();
    }
}

/**
 *
 *  rct2: 0x006AB54F
 */
bool window_editor_object_selection_select_object(uint8_t isMasterObject, int32_t flags, const ObjectRepositoryItem* item)
{
    if (item == nullptr)
    {
        set_object_selection_error(isMasterObject, STR_OBJECT_SELECTION_ERR_OBJECT_DATA_NOT_FOUND);
        return false;
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
        if (!(*selectionFlags & OBJECT_SELECTION_FLAG_SELECTED))
        {
            return true;
        }
        else if (*selectionFlags & OBJECT_SELECTION_FLAG_IN_USE)
        {
            set_object_selection_error(isMasterObject, STR_OBJECT_SELECTION_ERR_CURRENTLY_IN_USE);
            return false;
        }
        else if (*selectionFlags & OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED)
        {
            set_object_selection_error(isMasterObject, STR_OBJECT_SELECTION_ERR_ALWAYS_REQUIRED);
            return false;
        }

        ObjectType objectType = item->ObjectEntry.GetType();
        if (objectType == ObjectType::SceneryGroup && (flags & INPUT_FLAG_EDITOR_OBJECT_SELECT_OBJECTS_IN_SCENERY_GROUP))
        {
            for (const auto& sgEntry : item->SceneryGroupInfo.Entries)
            {
                window_editor_object_selection_select_object(++isMasterObject, flags, sgEntry);
            }
        }

        _numSelectedObjectsForType[EnumValue(objectType)]--;
        *selectionFlags &= ~OBJECT_SELECTION_FLAG_SELECTED;
        return true;
    }
    else
    {
        if (isMasterObject == 0)
        {
            if (flags & INPUT_FLAG_EDITOR_OBJECT_ALWAYS_REQUIRED)
            {
                *selectionFlags |= OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED;
            }
        }
        if (*selectionFlags & OBJECT_SELECTION_FLAG_SELECTED)
        {
            return true;
        }

        ObjectType objectType = item->ObjectEntry.GetType();
        uint16_t maxObjects = object_entry_group_counts[EnumValue(objectType)];

        if (maxObjects <= _numSelectedObjectsForType[EnumValue(objectType)])
        {
            set_object_selection_error(isMasterObject, STR_OBJECT_SELECTION_ERR_TOO_MANY_OF_TYPE_SELECTED);
            return false;
        }

        if (objectType == ObjectType::SceneryGroup && (flags & INPUT_FLAG_EDITOR_OBJECT_SELECT_OBJECTS_IN_SCENERY_GROUP))
        {
            for (const auto& sgEntry : item->SceneryGroupInfo.Entries)
            {
                if (!window_editor_object_selection_select_object(++isMasterObject, flags, sgEntry))
                {
                    _maxObjectsWasHit = true;
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
            set_object_selection_error(isMasterObject, STR_OBJECT_SELECTION_ERR_SHOULD_SELECT_X_FIRST);
            return false;
        }

        if (maxObjects <= _numSelectedObjectsForType[EnumValue(objectType)])
        {
            set_object_selection_error(isMasterObject, STR_OBJECT_SELECTION_ERR_TOO_MANY_OF_TYPE_SELECTED);
            return false;
        }

        _numSelectedObjectsForType[EnumValue(objectType)]++;

        *selectionFlags |= OBJECT_SELECTION_FLAG_SELECTED;
        return true;
    }
}

bool window_editor_object_selection_select_object(uint8_t isMasterObject, int32_t flags, std::string_view identifier)
{
    auto& objectRepository = OpenRCT2::GetContext()->GetObjectRepository();
    const auto* item = objectRepository.FindObject(identifier);
    return window_editor_object_selection_select_object(isMasterObject, flags, item);
}

bool window_editor_object_selection_select_object(uint8_t isMasterObject, int32_t flags, const rct_object_entry* entry)
{
    const ObjectRepositoryItem* item = object_repository_find_object_by_entry(entry);
    return window_editor_object_selection_select_object(isMasterObject, flags, item);
}

bool window_editor_object_selection_select_object(uint8_t isMasterObject, int32_t flags, const ObjectEntryDescriptor& entry)
{
    if (entry.Generation == ObjectGeneration::DAT)
        return window_editor_object_selection_select_object(isMasterObject, flags, &entry.Entry);

    return window_editor_object_selection_select_object(isMasterObject, flags, entry.Identifier);
}

bool editor_check_object_group_at_least_one_selected(ObjectType checkObjectType)
{
    auto numObjects = std::min(object_repository_get_items_count(), _objectSelectionFlags.size());
    const ObjectRepositoryItem* items = object_repository_get_items();

    for (size_t i = 0; i < numObjects; i++)
    {
        ObjectType objectType = items[i].ObjectEntry.GetType();
        if (checkObjectType == objectType && (_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_SELECTED))
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
        if (_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_SELECTED)
        {
            if (!(_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_IN_USE)
                && !(_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED))
            {
                const ObjectRepositoryItem* item = &items[i];
                ObjectType objectType = item->ObjectEntry.GetType();

                if (objectType >= ObjectType::SceneryGroup)
                {
                    continue;
                }

                _numSelectedObjectsForType[EnumValue(objectType)]--;
                _objectSelectionFlags[i] &= ~OBJECT_SELECTION_FLAG_SELECTED;
                numUnselectedObjects++;
            }
        }
    }
    unload_unselected_objects();
    editor_object_flags_free();

    auto intent = Intent(INTENT_ACTION_REFRESH_SCENERY);
    context_broadcast_intent(&intent);

    return numUnselectedObjects;
}
