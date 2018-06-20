/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <vector>
#include "Context.h"
#include "core/Util.hpp"
#include "Editor.h"
#include "EditorObjectSelectionSession.h"
#include "Game.h"
#include "localisation/Localisation.h"
#include "management/Research.h"
#include "object/DefaultObjects.h"
#include "object/ObjectList.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "OpenRCT2.h"
#include "ride/RideData.h"
#include "windows/Intent.h"
#include "world/Footpath.h"
#include "world/LargeScenery.h"


bool _maxObjectsWasHit;
std::vector<uint8_t> _objectSelectionFlags;
int32_t _numSelectedObjectsForType[OBJECT_TYPE_COUNT];
static int32_t _numAvailableObjectsForType[OBJECT_TYPE_COUNT];

static void setup_in_use_selection_flags();
static void setup_track_designer_objects();
static void setup_track_manager_objects();
static void window_editor_object_selection_select_required_objects();
static void window_editor_object_selection_select_default_objects();

/**
 *
 *  rct2: 0x006ABCD1
 */
static void setup_track_manager_objects()
{
    int32_t numObjects = (int32_t)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++)
    {
        uint8_t * selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem * item = &items[i];
        uint8_t object_type = object_entry_get_type(&item->ObjectEntry);
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
    int32_t numObjects = (int32_t)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++)
    {
        uint8_t * selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem * item = &items[i];
        uint8_t objectType = object_entry_get_type(&item->ObjectEntry);
        if (objectType == OBJECT_TYPE_RIDE)
        {
            *selectionFlags |= OBJECT_SELECTION_FLAG_6;

            for (uint8_t rideType : item->RideInfo.RideType)
            {
                if (rideType != RIDE_TYPE_NULL)
                {
                    if (RideData4[rideType].flags & RIDE_TYPE_FLAG4_SHOW_IN_TRACK_DESIGNER)
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
    for (uint8_t objectType = 0; objectType < OBJECT_TYPE_COUNT; objectType++)
    {
        for (int32_t i = 0; i < object_entry_group_counts[objectType]; i++)
        {
            Editor::ClearSelectedObject(objectType, i, OBJECT_SELECTION_FLAG_ALL);
            if (object_entry_get_chunk(objectType, i) != nullptr)
            {
                Editor::SetSelectedObject(objectType, i, OBJECT_SELECTION_FLAG_2);
            }
        }
    }

    tile_element_iterator iter;
    tile_element_iterator_begin(&iter);
    do {
        uint16_t type;
        rct_banner* banner;

        switch (iter.element->GetType()) {
        default:
        case TILE_ELEMENT_TYPE_SURFACE:
        case TILE_ELEMENT_TYPE_TRACK:
            break;
        case TILE_ELEMENT_TYPE_PATH:
            type = iter.element->properties.path.type;
            type >>= 4;
            assert(type < object_entry_group_counts[OBJECT_TYPE_PATHS]);
            Editor::SetSelectedObject(OBJECT_TYPE_PATHS, type, OBJECT_SELECTION_FLAG_SELECTED);

            if (footpath_element_has_path_scenery(iter.element)) {
                uint8_t path_additions = footpath_element_get_path_scenery_index(iter.element);
                Editor::SetSelectedObject(OBJECT_TYPE_PATH_BITS, path_additions, OBJECT_SELECTION_FLAG_SELECTED);
            }
            break;
        case TILE_ELEMENT_TYPE_SMALL_SCENERY:
            type = iter.element->properties.scenery.type;
            assert(type < object_entry_group_counts[OBJECT_TYPE_SMALL_SCENERY]);
            Editor::SetSelectedObject(OBJECT_TYPE_SMALL_SCENERY, type, OBJECT_SELECTION_FLAG_SELECTED);
            break;
        case TILE_ELEMENT_TYPE_ENTRANCE:
            if (iter.element->properties.entrance.type != ENTRANCE_TYPE_PARK_ENTRANCE)
                break;
            // Skip if not the middle part
            if (iter.element->properties.entrance.index != 0)
                break;

            Editor::SetSelectedObject(OBJECT_TYPE_PARK_ENTRANCE, 0, OBJECT_SELECTION_FLAG_SELECTED);

            type = iter.element->properties.entrance.path_type;
            assert(type < object_entry_group_counts[OBJECT_TYPE_PATHS]);
            Editor::SetSelectedObject(OBJECT_TYPE_PATHS, type, OBJECT_SELECTION_FLAG_SELECTED);
            break;
        case TILE_ELEMENT_TYPE_WALL:
            type = iter.element->properties.wall.type;
            assert(type < object_entry_group_counts[OBJECT_TYPE_WALLS]);
            Editor::SetSelectedObject(OBJECT_TYPE_WALLS, type, OBJECT_SELECTION_FLAG_SELECTED);
            break;
        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
            type = scenery_large_get_type(iter.element);
            assert(type < object_entry_group_counts[OBJECT_TYPE_LARGE_SCENERY]);
            Editor::SetSelectedObject(OBJECT_TYPE_LARGE_SCENERY, type, OBJECT_SELECTION_FLAG_SELECTED);
            break;
        case TILE_ELEMENT_TYPE_BANNER:
            banner = &gBanners[iter.element->properties.banner.index];
            type = banner->type;
            assert(type < object_entry_group_counts[OBJECT_TYPE_BANNERS]);
            Editor::SetSelectedObject(OBJECT_TYPE_BANNERS, type, OBJECT_SELECTION_FLAG_SELECTED);
            break;
        }
    } while (tile_element_iterator_next(&iter));

    for (uint8_t ride_index = 0; ride_index < 0xFF; ride_index++) {
        Ride* ride = get_ride(ride_index);
        if (ride->type != RIDE_TYPE_NULL) {
            uint8_t type = ride->subtype;
            Editor::SetSelectedObject(OBJECT_TYPE_RIDE, type, OBJECT_SELECTION_FLAG_SELECTED);
        }
    }

    int32_t numObjects = (int32_t)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++) {
        uint8_t *selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem * item = &items[i];
        *selectionFlags &= ~OBJECT_SELECTION_FLAG_IN_USE;

        uint8_t entryType, entryIndex;
        if (find_object_in_entry_group(&item->ObjectEntry, &entryType, &entryIndex)) {
            auto flags = Editor::GetSelectedObjectFlags(entryType, entryIndex);
            if (flags & OBJECT_SELECTION_FLAG_SELECTED)
            {
                *selectionFlags |=
                    OBJECT_SELECTION_FLAG_IN_USE |
                    OBJECT_SELECTION_FLAG_SELECTED;
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
    int32_t numObjects = (int32_t)object_repository_get_items_count();
    _objectSelectionFlags = std::vector<uint8_t>(numObjects);

    for (uint8_t objectType = 0; objectType < OBJECT_TYPE_COUNT; objectType++) {
        _numSelectedObjectsForType[objectType] = 0;
        _numAvailableObjectsForType[objectType] = 0;
    }

    const ObjectRepositoryItem * items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++) {
        uint8_t objectType = object_entry_get_type(&items[i].ObjectEntry);
        _numAvailableObjectsForType[objectType]++;
    }

    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) {
        setup_track_designer_objects();
    }

    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
        setup_track_manager_objects();
    }

    setup_in_use_selection_flags();
    reset_selected_object_count_and_size();

    if (!(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))) {
        window_editor_object_selection_select_required_objects();

        // To prevent it breaking in scenario mode.
        if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) {
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
    uint8_t entry_type, entry_index;
    if (!find_object_in_entry_group(installedObject, &entry_type, &entry_index))
        return;

    if (entry_type == OBJECT_TYPE_RIDE)
    {
        auto rideEntry = get_ride_entry(entry_index);
        
        for (auto rideType : rideEntry->ride_type)
        {
            rct_research_item tmp = {};
            tmp.type = RESEARCH_ENTRY_TYPE_RIDE;
            tmp.entryIndex = entry_index;
            tmp.baseRideType = rideType;
            research_remove(&tmp);
        }
    }
    else if (entry_type == OBJECT_TYPE_SCENERY_GROUP)
    {
        rct_research_item tmp = {};
        tmp.type = RESEARCH_ENTRY_TYPE_SCENERY;
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
    int32_t numItems = (int32_t)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();

    size_t numObjectsToUnload = 0;
    rct_object_entry * objectsToUnload = (rct_object_entry *)malloc(numItems * sizeof(rct_object_entry));

    for (int32_t i = 0; i < numItems; i++) {
        if (!(_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_SELECTED)) {
            const rct_object_entry * entry = &items[i].ObjectEntry;

            remove_selected_objects_from_research(entry);
            objectsToUnload[numObjectsToUnload++] = *entry;
        }
    }

    object_manager_unload_objects(objectsToUnload, numObjectsToUnload);
    free(objectsToUnload);
}

/**
*
*  rct2: 0x006AA805
*/
static void window_editor_object_selection_select_default_objects()
{
    if (_numSelectedObjectsForType[0] == 0)
    {
        for (const auto &defaultSelectedObject : DefaultSelectedObjects)
        {
            window_editor_object_selection_select_object(0, 7, &defaultSelectedObject);
        }
    }
}

/**
 *
 *  rct2: 0x006AA7E9
 */
static void window_editor_object_selection_select_required_objects()
{
    int32_t i;

    for (i = 0; i < (int32_t)Util::CountOf(RequiredSelectedObjects); i++)
        window_editor_object_selection_select_object(0, 0xF, &RequiredSelectedObjects[i]);
}

/**
 *
 *  rct2: 0x006AA770
 */
void reset_selected_object_count_and_size()
{
    for (auto &objectType : _numSelectedObjectsForType)
    {
        objectType = 0;
    }

    int32_t numObjects = (int32_t)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++) {
        uint8_t objectType = object_entry_get_type(&items[i].ObjectEntry);
        if (_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_SELECTED) {
            _numSelectedObjectsForType[objectType]++;
        }
    }
}

/**
 * Master objects are objects that are not
 * optional / required dependants of an
 * object.
 */
static void set_object_selection_error(uint8_t is_master_object, rct_string_id error_msg){
    gGameCommandErrorText = error_msg;
    if (!is_master_object){
        reset_selected_object_count_and_size();
    }
}

/**
 *
 *  rct2: 0x006AB54F
 */
int32_t window_editor_object_selection_select_object(uint8_t bh, int32_t flags, const rct_object_entry *entry)
{
    int32_t numObjects = (int32_t)object_repository_get_items_count();
    const ObjectRepositoryItem * item = object_repository_find_object_by_entry(entry);
    if (item == nullptr) {
        set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_OBJECT_DATA_NOT_FOUND);
        return 0;
    }

    // Get repository item index
    int32_t index = -1;
    const ObjectRepositoryItem * items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++) {
        if (&items[i] == item) {
            index = i;
        }
    }

    uint8_t * selectionFlags = &_objectSelectionFlags[index];
    if (!(flags & 1)) {
        if (!(*selectionFlags & OBJECT_SELECTION_FLAG_SELECTED)) {
            return 1;
        }
        else if (*selectionFlags & OBJECT_SELECTION_FLAG_IN_USE) {
            set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_CURRENTLY_IN_USE);
            return 0;
        }
        else if (*selectionFlags & OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED) {
            set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_ALWAYS_REQUIRED);
            return 0;
        }

        uint8_t objectType = object_entry_get_type(&item->ObjectEntry);
        if (objectType == OBJECT_TYPE_SCENERY_GROUP && (flags & (1 << 2))) {
            for (const auto& sgEntry : item->SceneryGroupInfo.Entries)
            {
                window_editor_object_selection_select_object(++bh, flags, &sgEntry);
            }
        }

        _numSelectedObjectsForType[objectType]--;
        *selectionFlags &= ~OBJECT_SELECTION_FLAG_SELECTED;
        return 1;
    } else {
        if (bh == 0) {
            if (flags & (1 << 3)) {
                *selectionFlags |= OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED;
            }
        }
        if (*selectionFlags & OBJECT_SELECTION_FLAG_SELECTED) {
            return 1;
        }

        uint8_t objectType = object_entry_get_type(&item->ObjectEntry);
        uint16_t maxObjects = object_entry_group_counts[objectType];
        if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) {
            maxObjects = 4;
        }

        if (maxObjects <= _numSelectedObjectsForType[objectType]) {
            set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_TOO_MANY_OF_TYPE_SELECTED);
            return 0;
        }

        if (objectType == OBJECT_TYPE_SCENERY_GROUP && (flags & (1 << 2))) {
            for (const auto& sgEntry : item->SceneryGroupInfo.Entries)
            {
                if (!window_editor_object_selection_select_object(++bh, flags, &sgEntry))
                {
                    _maxObjectsWasHit = true;
                }
            }
        }

        if (bh != 0 && !(flags & (1 << 1))) {
            char objectName[64];
            object_create_identifier_name(objectName, 64, &item->ObjectEntry);
            set_format_arg(0, const char *, objectName);
            set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_SHOULD_SELECT_X_FIRST);
            return 0;
        }

        if (maxObjects <= _numSelectedObjectsForType[objectType]) {
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
    int32_t numObjects = (int32_t)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();

    for (int32_t i = 0; i < numObjects; i++) {
        uint8_t objectType = object_entry_get_type(&items[i].ObjectEntry);
        if (checkObjectType == objectType && (_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_SELECTED)) {
            return true;
        }
    }
    return false;
}

int32_t editor_remove_unused_objects()
{
    sub_6AB211();
    setup_in_use_selection_flags();

    int32_t numObjects = (int32_t)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();

    int32_t numUnselectedObjects = 0;
    for (int32_t i = 0; i < numObjects; i++)
    {
        if (!(_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_IN_USE) && !(_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED))
        {
            const ObjectRepositoryItem * item = &items[i];
            uint8_t objectType = object_entry_get_type(&item->ObjectEntry);

            if (objectType == OBJECT_TYPE_PARK_ENTRANCE || objectType == OBJECT_TYPE_SCENARIO_TEXT || objectType == OBJECT_TYPE_WATER || objectType == OBJECT_TYPE_SCENERY_GROUP)
            {
                continue;
            }

            _numSelectedObjectsForType[objectType]--;
            _objectSelectionFlags[i] &= ~OBJECT_SELECTION_FLAG_SELECTED;
            numUnselectedObjects++;
        }
    }
    unload_unselected_objects();
    editor_object_flags_free();

    auto intent = Intent(INTENT_ACTION_REFRESH_SCENERY);
    context_broadcast_intent(&intent);

    return numUnselectedObjects;
}
