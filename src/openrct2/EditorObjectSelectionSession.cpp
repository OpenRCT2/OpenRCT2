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
#include "localisation/Localisation.h"
#include "management/Research.h"
#include "object/DefaultObjects.h"
#include "object/ObjectList.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "ride/RideData.h"
#include "scenario/Scenario.h"
#include "windows/Intent.h"
#include "world/Footpath.h"
#include "world/LargeScenery.h"

#include <iterator>
#include <vector>

bool _maxObjectsWasHit;
std::vector<uint8_t> _objectSelectionFlags;
int32_t _numSelectedObjectsForType[OBJECT_TYPE_COUNT];
static int32_t _numAvailableObjectsForType[OBJECT_TYPE_COUNT];

static void setup_in_use_selection_flags();
static void setup_track_designer_objects();
static void setup_track_manager_objects();
static void window_editor_object_selection_select_default_objects();

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
        uint8_t object_type = item->ObjectEntry.GetType();
        if (object_type == OBJECT_TYPE_RIDE)
        {
            *selectionFlags |= OBJECT_SELECTION_FLAG_6;

            for (auto rideType : item->RideInfo.RideType)
            {
                if (rideType != RIDE_TYPE_NULL && ride_type_has_flag(rideType, RIDE_TYPE_FLAG_HAS_TRACK))
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
    for (int32_t i = 0; i < numObjects; i++)
    {
        uint8_t* selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem* item = &items[i];
        uint8_t objectType = item->ObjectEntry.GetType();
        if (objectType == OBJECT_TYPE_RIDE)
        {
            *selectionFlags |= OBJECT_SELECTION_FLAG_6;

            for (uint8_t rideType : item->RideInfo.RideType)
            {
                if (rideType != RIDE_TYPE_NULL)
                {
                    if (RideTypeDescriptors[rideType].Flags & RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER)
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

    for (uint8_t objectType = 0; objectType < OBJECT_TYPE_COUNT; objectType++)
    {
        for (int32_t i = 0; i < object_entry_group_counts[objectType]; i++)
        {
            Editor::ClearSelectedObject(objectType, i, OBJECT_SELECTION_FLAG_ALL);

            auto loadedObj = objectMgr.GetLoadedObject(objectType, i);
            if (loadedObj != nullptr)
            {
                Editor::SetSelectedObject(objectType, i, OBJECT_SELECTION_FLAG_2);
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
                assert(type < object_entry_group_counts[OBJECT_TYPE_PATHS]);
                Editor::SetSelectedObject(OBJECT_TYPE_PATHS, type, OBJECT_SELECTION_FLAG_SELECTED);

                if (iter.element->AsPath()->HasAddition())
                {
                    uint8_t path_additions = iter.element->AsPath()->GetAdditionEntryIndex();
                    Editor::SetSelectedObject(OBJECT_TYPE_PATH_BITS, path_additions, OBJECT_SELECTION_FLAG_SELECTED);
                }
                break;
            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                type = iter.element->AsSmallScenery()->GetEntryIndex();
                assert(type < object_entry_group_counts[OBJECT_TYPE_SMALL_SCENERY]);
                Editor::SetSelectedObject(OBJECT_TYPE_SMALL_SCENERY, type, OBJECT_SELECTION_FLAG_SELECTED);
                break;
            case TILE_ELEMENT_TYPE_ENTRANCE:
                if (iter.element->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                    break;
                // Skip if not the middle part
                if (iter.element->AsEntrance()->GetSequenceIndex() != 0)
                    break;

                Editor::SetSelectedObject(OBJECT_TYPE_PARK_ENTRANCE, 0, OBJECT_SELECTION_FLAG_SELECTED);

                type = iter.element->AsEntrance()->GetPathType();
                assert(type < object_entry_group_counts[OBJECT_TYPE_PATHS]);
                Editor::SetSelectedObject(OBJECT_TYPE_PATHS, type, OBJECT_SELECTION_FLAG_SELECTED);
                break;
            case TILE_ELEMENT_TYPE_WALL:
                type = iter.element->AsWall()->GetEntryIndex();
                assert(type < object_entry_group_counts[OBJECT_TYPE_WALLS]);
                Editor::SetSelectedObject(OBJECT_TYPE_WALLS, type, OBJECT_SELECTION_FLAG_SELECTED);
                break;
            case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                type = iter.element->AsLargeScenery()->GetEntryIndex();
                assert(type < object_entry_group_counts[OBJECT_TYPE_LARGE_SCENERY]);
                Editor::SetSelectedObject(OBJECT_TYPE_LARGE_SCENERY, type, OBJECT_SELECTION_FLAG_SELECTED);
                break;
            case TILE_ELEMENT_TYPE_BANNER:
            {
                auto banner = iter.element->AsBanner()->GetBanner();
                if (banner != nullptr)
                {
                    type = banner->type;
                    assert(type < object_entry_group_counts[OBJECT_TYPE_BANNERS]);
                    Editor::SetSelectedObject(OBJECT_TYPE_BANNERS, type, OBJECT_SELECTION_FLAG_SELECTED);
                }
                break;
            }
        }
    } while (tile_element_iterator_next(&iter));

    for (uint8_t ride_index = 0; ride_index < 0xFF; ride_index++)
    {
        auto ride = get_ride(ride_index);
        if (ride != nullptr)
        {
            uint8_t type = ride->subtype;
            Editor::SetSelectedObject(OBJECT_TYPE_RIDE, type, OBJECT_SELECTION_FLAG_SELECTED);
        }
    }

    int32_t numObjects = static_cast<int32_t>(object_repository_get_items_count());
    const ObjectRepositoryItem* items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++)
    {
        uint8_t* selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem* item = &items[i];
        *selectionFlags &= ~OBJECT_SELECTION_FLAG_IN_USE;

        uint8_t entryType;
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

    for (uint8_t objectType = 0; objectType < OBJECT_TYPE_COUNT; objectType++)
    {
        _numSelectedObjectsForType[objectType] = 0;
        _numAvailableObjectsForType[objectType] = 0;
    }

    const ObjectRepositoryItem* items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++)
    {
        uint8_t objectType = items[i].ObjectEntry.GetType();
        _numAvailableObjectsForType[objectType]++;
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
    uint8_t entry_type;
    ObjectEntryIndex entry_index;
    if (!find_object_in_entry_group(installedObject, &entry_type, &entry_index))
        return;

    if (entry_type == OBJECT_TYPE_RIDE)
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
    else if (entry_type == OBJECT_TYPE_SCENERY_GROUP)
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
        for (const auto& defaultSelectedObject : DefaultSelectedObjects)
        {
            window_editor_object_selection_select_object(0, 7, &defaultSelectedObject);
        }
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
        uint8_t objectType = items[i].ObjectEntry.GetType();
        if (_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_SELECTED)
        {
            _numSelectedObjectsForType[objectType]++;
        }
    }
}

void finish_object_selection()
{
    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
    {
        set_every_ride_type_invented();
        set_every_ride_entry_invented();
        gS6Info.editor_step = EDITOR_STEP_ROLLERCOASTER_DESIGNER;
        gfx_invalidate_screen();
    }
    else
    {
        set_all_scenery_items_invented();
        scenery_set_default_placement_configuration();
        gS6Info.editor_step = EDITOR_STEP_LANDSCAPE_EDITOR;
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
int32_t window_editor_object_selection_select_object(uint8_t bh, int32_t flags, const rct_object_entry* entry)
{
    int32_t numObjects = static_cast<int32_t>(object_repository_get_items_count());
    const ObjectRepositoryItem* item = object_repository_find_object_by_entry(entry);
    if (item == nullptr)
    {
        set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_OBJECT_DATA_NOT_FOUND);
        return 0;
    }

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
    if (!(flags & 1))
    {
        if (!(*selectionFlags & OBJECT_SELECTION_FLAG_SELECTED))
        {
            return 1;
        }
        else if (*selectionFlags & OBJECT_SELECTION_FLAG_IN_USE)
        {
            set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_CURRENTLY_IN_USE);
            return 0;
        }
        else if (*selectionFlags & OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED)
        {
            set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_ALWAYS_REQUIRED);
            return 0;
        }

        uint8_t objectType = item->ObjectEntry.GetType();
        if (objectType == OBJECT_TYPE_SCENERY_GROUP && (flags & INPUT_FLAG_EDITOR_OBJECT_2))
        {
            for (const auto& sgEntry : item->SceneryGroupInfo.Entries)
            {
                window_editor_object_selection_select_object(++bh, flags, &sgEntry);
            }
        }

        _numSelectedObjectsForType[objectType]--;
        *selectionFlags &= ~OBJECT_SELECTION_FLAG_SELECTED;
        return 1;
    }
    else
    {
        if (bh == 0)
        {
            if (flags & INPUT_FLAG_EDITOR_OBJECT_ALWAYS_REQUIRED)
            {
                *selectionFlags |= OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED;
            }
        }
        if (*selectionFlags & OBJECT_SELECTION_FLAG_SELECTED)
        {
            return 1;
        }

        uint8_t objectType = item->ObjectEntry.GetType();
        uint16_t maxObjects = object_entry_group_counts[objectType];
        if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
        {
            maxObjects = 4;
        }

        if (maxObjects <= _numSelectedObjectsForType[objectType])
        {
            set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_TOO_MANY_OF_TYPE_SELECTED);
            return 0;
        }

        if (objectType == OBJECT_TYPE_SCENERY_GROUP && (flags & INPUT_FLAG_EDITOR_OBJECT_2))
        {
            for (const auto& sgEntry : item->SceneryGroupInfo.Entries)
            {
                if (!window_editor_object_selection_select_object(++bh, flags, &sgEntry))
                {
                    _maxObjectsWasHit = true;
                }
            }
        }

        if (bh != 0 && !(flags & INPUT_FLAG_EDITOR_OBJECT_1))
        {
            char objectName[64];
            object_create_identifier_name(objectName, 64, &item->ObjectEntry);
            auto ft = Formatter::Common();
            ft.Add<const char*>(objectName);
            set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_SHOULD_SELECT_X_FIRST);
            return 0;
        }

        if (maxObjects <= _numSelectedObjectsForType[objectType])
        {
            set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_TOO_MANY_OF_TYPE_SELECTED);
            return 0;
        }

        _numSelectedObjectsForType[objectType]++;

        *selectionFlags |= OBJECT_SELECTION_FLAG_SELECTED;
        return 1;
    }
}

bool editor_check_object_group_at_least_one_selected(int32_t checkObjectType)
{
    int32_t numObjects = static_cast<int32_t>(object_repository_get_items_count());
    const ObjectRepositoryItem* items = object_repository_get_items();

    for (int32_t i = 0; i < numObjects; i++)
    {
        uint8_t objectType = items[i].ObjectEntry.GetType();
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
                uint8_t objectType = item->ObjectEntry.GetType();

                if (objectType >= OBJECT_TYPE_SCENERY_GROUP)
                {
                    continue;
                }

                _numSelectedObjectsForType[objectType]--;
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
