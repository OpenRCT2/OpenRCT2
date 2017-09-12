#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../config/Config.h"
#include "../Context.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../OpenRCT2.h"
#include "../core/Util.hpp"
#include "../core/Memory.hpp"

extern "C"
{
    #include <ctype.h>
    #include "../audio/audio.h"
    #include "../game.h"
    #include "../editor.h"
    #include "../interface/widget.h"
    #include "../localisation/localisation.h"
    #include "../object_list.h"
    #include "../rct1.h"
    #include "../ride/ride_data.h"
    #include "../util/util.h"
    #include "dropdown.h"
    #include "error.h"
    #include "../sprites.h"
    #include "_legacy.h"
}

enum {
    FILTER_RCT2 = (1 << 0),
    FILTER_WW = (1 << 1),
    FILTER_TT = (1 << 2),
    FILTER_CUSTOM = (1 << 3),

    FILTER_RIDE_TRANSPORT = (1 << 5),
    FILTER_RIDE_GENTLE = (1 << 6),
    FILTER_RIDE_COASTER = (1 << 7),
    FILTER_RIDE_THRILL = (1 << 8),
    FILTER_RIDE_WATER = (1 << 9),
    FILTER_RIDE_STALL = (1 << 10),

    FILTER_SELECTED = (1 << 12),
    FILTER_NONSELECTED = (1 << 13),

    FILTER_RIDES = FILTER_RIDE_TRANSPORT | FILTER_RIDE_GENTLE | FILTER_RIDE_COASTER | FILTER_RIDE_THRILL | FILTER_RIDE_WATER | FILTER_RIDE_STALL,
    FILTER_ALL = FILTER_RIDES | FILTER_RCT2 | FILTER_WW | FILTER_TT | FILTER_CUSTOM | FILTER_SELECTED | FILTER_NONSELECTED,
};

static uint32 _filter_flags;
static uint16 _filter_object_counts[11];

static char _filter_string[41];

#define _FILTER_ALL ((_filter_flags & FILTER_ALL) == FILTER_ALL)
#define _FILTER_RCT2 (_filter_flags & FILTER_RCT2)
#define _FILTER_WW (_filter_flags & FILTER_WW)
#define _FILTER_TT (_filter_flags & FILTER_TT)
#define _FILTER_CUSTOM (_filter_flags & FILTER_CUSTOM)
#define _FILTER_SELECTED (_filter_flags & FILTER_SELECTED)
#define _FILTER_NONSELECTED (_filter_flags & FILTER_NONSELECTED)

enum {
    WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS,
    WINDOW_OBJECT_SELECTION_PAGE_SMALL_SCENERY,
    WINDOW_OBJECT_SELECTION_PAGE_LARGE_SCENERY,
    WINDOW_OBJECT_SELECTION_PAGE_WALLS_FENCES,
    WINDOW_OBJECT_SELECTION_PAGE_PATH_SIGNS,
    WINDOW_OBJECT_SELECTION_PAGE_FOOTPATHS,
    WINDOW_OBJECT_SELECTION_PAGE_PATH_EXTRAS,
    WINDOW_OBJECT_SELECTION_PAGE_SCENERY_GROUPS,
    WINDOW_OBJECT_SELECTION_PAGE_PARK_ENTRANCE,
    WINDOW_OBJECT_SELECTION_PAGE_WATER,
    WINDOW_OBJECT_SELECTION_PAGE_SCENARIO_DESCRIPTION,
    WINDOW_OBJECT_SELECTION_PAGE_COUNT
};

static const rct_string_id ObjectSelectionPageNames[WINDOW_OBJECT_SELECTION_PAGE_COUNT] = {
    STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS,
    STR_OBJECT_SELECTION_SMALL_SCENERY,
    STR_OBJECT_SELECTION_LARGE_SCENERY,
    STR_OBJECT_SELECTION_WALLS_FENCES,
    STR_OBJECT_SELECTION_PATH_SIGNS,
    STR_OBJECT_SELECTION_FOOTPATHS,
    STR_OBJECT_SELECTION_PATH_EXTRAS,
    STR_OBJECT_SELECTION_SCENERY_GROUPS,
    STR_OBJECT_SELECTION_PARK_ENTRANCE,
    STR_OBJECT_SELECTION_WATER,
    STR_OBJECT_SELECTION_SCENARIO_DESCRIPTION,
};

#pragma region Widgets

enum WINDOW_STAFF_LIST_WIDGET_IDX {
    WIDX_BACKGROUND,            // 0, 1
    WIDX_TITLE,                 // 1, 2
    WIDX_CLOSE,                 // 2, 4
    WIDX_TAB_CONTENT_PANEL,     // 3, 8
    WIDX_TAB_1,                 // 4, 10
    WIDX_TAB_2,                 // 5, 20
    WIDX_TAB_3,                 // 6, 40
    WIDX_TAB_4,                 // 7, 80
    WIDX_TAB_5,                 // 8, 100
    WIDX_TAB_6,                 // 9, 200
    WIDX_TAB_7,                 // 10, 400
    WIDX_TAB_8,                 // 11, 800
    WIDX_TAB_9,                 // 12, 1000
    WIDX_TAB_10,                // 13, 2000
    WIDX_TAB_11,                // 14, 4000
    WIDX_ADVANCED,              // 15, 8000
    WIDX_LIST,                  // 16, 10000
    WIDX_PREVIEW,               // 17, 20000
    WIDX_INSTALL_TRACK,         // 18, 40000
    WIDX_FILTER_DROPDOWN,       // 19, 80000
    WIDX_FILTER_STRING_BUTTON,  // 20, 100000
    WIDX_FILTER_CLEAR_BUTTON,   // 21, 200000
    WIDX_FILTER_RIDE_TAB_FRAME,
    WIDX_FILTER_RIDE_TAB_ALL,
    WIDX_FILTER_RIDE_TAB_TRANSPORT,
    WIDX_FILTER_RIDE_TAB_GENTLE,
    WIDX_FILTER_RIDE_TAB_COASTER,
    WIDX_FILTER_RIDE_TAB_THRILL,
    WIDX_FILTER_RIDE_TAB_WATER,
    WIDX_FILTER_RIDE_TAB_STALL,
    WIDX_LIST_SORT_TYPE,
    WIDX_LIST_SORT_RIDE,
};

validate_global_widx(WC_EDITOR_OBJECT_SELECTION, WIDX_TAB_1);

static rct_widget window_editor_object_selection_widgets[] = {
    { WWT_FRAME,            0,  0,      599,    0,      399,    0xFFFFFFFF,                     STR_NONE },
    { WWT_CAPTION,          0,  1,      598,    1,      14,     STR_OBJECT_SELECTION,           STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  587,    597,    2,      13,     STR_CLOSE_X,                    STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      599,    43,     399,    0xFFFFFFFF,                     STR_NONE },
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_STRING_DEFINED_TOOLTIP },
    { WWT_TAB,              1,  34,     64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_STRING_DEFINED_TOOLTIP },
    { WWT_TAB,              1,  65,     95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_STRING_DEFINED_TOOLTIP },
    { WWT_TAB,              1,  96,     126,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_STRING_DEFINED_TOOLTIP },
    { WWT_TAB,              1,  127,    157,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_STRING_DEFINED_TOOLTIP },
    { WWT_TAB,              1,  158,    188,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_STRING_DEFINED_TOOLTIP },
    { WWT_TAB,              1,  189,    219,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_STRING_DEFINED_TOOLTIP },
    { WWT_TAB,              1,  220,    250,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_STRING_DEFINED_TOOLTIP },
    { WWT_TAB,              1,  251,    281,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_STRING_DEFINED_TOOLTIP },
    { WWT_TAB,              1,  282,    312,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_STRING_DEFINED_TOOLTIP },
    { WWT_TAB,              1,  313,    343,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_STRING_DEFINED_TOOLTIP },
    { WWT_DROPDOWN_BUTTON,  0,  470,    591,    23,     34,     STR_OBJECT_SELECTION_ADVANCED,  STR_OBJECT_SELECTION_ADVANCED_TIP },
    { WWT_SCROLL,           1,  4,      291,    60,     386,    SCROLL_VERTICAL,                STR_NONE },
    { WWT_FLATBTN,          1,  391,    504,    46,     159,    0xFFFFFFFF,                     STR_NONE },
    { WWT_DROPDOWN_BUTTON,  0,  470,    591,    23,     34,     STR_INSTALL_NEW_TRACK_DESIGN,   STR_INSTALL_NEW_TRACK_DESIGN_TIP },
    { WWT_DROPDOWN_BUTTON,  0,  350,    463,    23,     34,     STR_OBJECT_FILTER,              STR_OBJECT_FILTER_TIP },
    { WWT_TEXT_BOX,         1,  4,      214,    46,     57,     STR_NONE,                       STR_NONE },
    { WWT_DROPDOWN_BUTTON,  1,  218,    287,    46,     57,     STR_OBJECT_SEARCH_CLEAR,        STR_NONE },
    { WWT_IMGBTN,           1,  3,      287,    73,     76,     0xFFFFFFFF,                     STR_NONE },
    { WWT_TAB,              1,  3,      33,     47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_OBJECT_FILTER_ALL_RIDES_TIP },
    { WWT_TAB,              1,  34,     64,     47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_TRANSPORT_RIDES_TIP },
    { WWT_TAB,              1,  65,     95,     47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_GENTLE_RIDES_TIP },
    { WWT_TAB,              1,  96,     126,    47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_ROLLER_COASTERS_TIP },
    { WWT_TAB,              1,  127,    157,    47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_THRILL_RIDES_TIP },
    { WWT_TAB,              1,  158,    188,    47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_WATER_RIDES_TIP },
    { WWT_TAB,              1,  189,    219,    47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_SHOPS_STALLS_TIP },
    { WWT_13,               1,  4,      204,    80,     93,     STR_NONE,                       STR_NONE },
    { WWT_13,               1,  205,    291,    80,     93,     STR_NONE,                       STR_NONE },
    { WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_editor_object_selection_close(rct_window *w);
static void window_editor_object_selection_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_editor_object_selection_resize(rct_window *w);
static void window_editor_object_selection_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_editor_object_selection_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_editor_object_selection_update(rct_window *w);
static void window_editor_object_selection_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_editor_object_selection_scroll_mousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_editor_object_selection_scroll_mouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_editor_object_selection_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_editor_object_selection_invalidate(rct_window *w);
static void window_editor_object_selection_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_editor_object_selection_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);
static void window_editor_object_selection_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);

static rct_window_event_list window_editor_object_selection_events = {
    window_editor_object_selection_close,
    window_editor_object_selection_mouseup,
    window_editor_object_selection_resize,
    window_editor_object_selection_mousedown,
    window_editor_object_selection_dropdown,
    nullptr,
    window_editor_object_selection_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_editor_object_selection_scrollgetsize,
    window_editor_object_selection_scroll_mousedown,
    nullptr,
    window_editor_object_selection_scroll_mouseover,
    window_editor_object_selection_textinput,
    nullptr,
    nullptr,
    window_editor_object_selection_tooltip,
    nullptr,
    nullptr,
    window_editor_object_selection_invalidate,
    window_editor_object_selection_paint,
    window_editor_object_selection_scrollpaint
};

#pragma endregion

const sint32 window_editor_object_selection_animation_loops[] = { 20, 32, 10, 72, 24, 28, 16 };
const sint32 window_editor_object_selection_animation_divisor[] = { 4, 8, 2, 4, 4, 4, 2 };

static void window_editor_object_set_page(rct_window *w, sint32 page);
static void window_editor_object_selection_set_pressed_tab(rct_window *w);
static void window_editor_object_selection_select_default_objects();
static void window_editor_object_selection_select_required_objects();
static sint32 window_editor_object_selection_select_object(uint8 bh, sint32 flags, const rct_object_entry *entry);
static sint32 get_object_from_object_selection(uint8 object_type, sint32 y);
static void window_editor_object_selection_manage_tracks();
static void editor_load_selected_objects();
static bool filter_selected(uint8 objectFlags);
static bool filter_string(const ObjectRepositoryItem * item);
static bool filter_source(const ObjectRepositoryItem * item);
static bool filter_chunks(const ObjectRepositoryItem * item);
static void filter_update_counts();

void reset_selected_object_count_and_size();
static sint32 sub_6AB211();


enum {
    RIDE_SORT_TYPE,
    RIDE_SORT_RIDE
};

enum {
    DDIX_FILTER_RCT2,
    DDIX_FILTER_WW,
    DDIX_FILTER_TT,
    DDIX_FILTER_CUSTOM,
    DDIX_FILTER_SEPERATOR,
    DDIX_FILTER_SELECTED,
    DDIX_FILTER_NONSELECTED,
};

typedef struct list_item {
    const ObjectRepositoryItem * repositoryItem;
    rct_object_entry *entry;
    rct_object_filters *filter;
    uint8 *flags;
} list_item;

static rct_string_id get_ride_type_string_id(const ObjectRepositoryItem * item);

typedef sint32(*sortFunc_t)(const void *, const void *);

static sint32 _numListItems = 0;
static list_item *_listItems = nullptr;
static sint32 _listSortType = RIDE_SORT_TYPE;
static bool _listSortDescending = false;
static void * _loadedObject = nullptr;
static uint8 * _objectSelectionFlags = nullptr;
static sint32 _numSelectedObjectsForType[11];
static sint32 _numAvailableObjectsForType[11];
static bool _maxObjectsWasHit;

static void visible_list_dispose()
{
    SafeFree(_listItems);
    _numListItems = 0;
}

static sint32 visible_list_sort_ride_name(const void *rawA, const void *rawB)
{
    list_item *a = (list_item*)rawA;
    list_item *b = (list_item*)rawB;

    const char *nameA = a->repositoryItem->Name;
    const char *nameB = b->repositoryItem->Name;
    return strcmp(nameA, nameB);
}

static sint32 visible_list_sort_ride_type(const void *rawA, const void *rawB)
{
    list_item *a = (list_item*)rawA;
    list_item *b = (list_item*)rawB;

    const char *rideTypeA = language_get_string(get_ride_type_string_id(a->repositoryItem));
    const char *rideTypeB = language_get_string(get_ride_type_string_id(b->repositoryItem));
    sint32 result = strcmp(rideTypeA, rideTypeB);
    if (result != 0)
        return result;

    return visible_list_sort_ride_name(rawA, rawB);
}

static void visible_list_refresh(rct_window *w)
{
    sint32 numObjects = (sint32)object_repository_get_items_count();

    visible_list_dispose();
    w->selected_list_item = -1;
    _listItems = Memory::AllocateArray<list_item>(numObjects);
    _numListItems = 0;

    list_item *currentListItem = &_listItems[0];
    const ObjectRepositoryItem *items = object_repository_get_items();
    for (sint32 i = 0; i < numObjects; i++) {
        uint8 selectionFlags = _objectSelectionFlags[i];
        const ObjectRepositoryItem * item = &items[i];
        uint8 objectType = item->ObjectEntry.flags & 0x0F;
        if (objectType == w->selected_tab && !(selectionFlags & OBJECT_SELECTION_FLAG_6) &&
            filter_source(item) &&
            filter_string(item) &&
            filter_chunks(item) &&
            filter_selected(selectionFlags)
        ) {
            rct_object_filters * filter = new rct_object_filters;
            filter->ride.category[0] = 0;
            filter->ride.category[1] = 0;
            filter->ride.ride_type = 0;
            currentListItem->repositoryItem = item;
            currentListItem->entry = (rct_object_entry *)&item->ObjectEntry;
            currentListItem->filter = filter;
            currentListItem->flags = &_objectSelectionFlags[i];
            currentListItem++;
            _numListItems++;
        }
    }

    if (_numListItems == 0)
    {
        visible_list_dispose();
        window_invalidate(w);
        return;
    }

    _listItems = Memory::ReallocateArray(_listItems, _numListItems);
    if (_listItems == nullptr) {
        _numListItems = 0;
        log_error("Unable to reallocate list items");
    } else {
        sortFunc_t sortFunc = nullptr;
        switch (_listSortType) {
        case RIDE_SORT_TYPE:
            sortFunc = visible_list_sort_ride_type;
            break;
        case RIDE_SORT_RIDE:
            sortFunc = visible_list_sort_ride_name;
            break;
        default:
            log_warning("Wrong sort type %d, leaving list as-is.", _listSortType);
            window_invalidate(w);
            return;
        }
        qsort(_listItems, _numListItems, sizeof(list_item), sortFunc);

        if (_listSortDescending) {
            for (sint32 i = 0; i < _numListItems / 2; i++) {
                sint32 ri = _numListItems - i - 1;
                list_item temp = _listItems[i];
                _listItems[i] = _listItems[ri];
                _listItems[ri] = temp;
            }
        }
    }
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006AA64E
 */
void window_editor_object_selection_open()
{
    rct_window* window;

    window = window_bring_to_front_by_class(WC_EDITOR_OBJECT_SELECTION);
    if (window != nullptr)
        return;

    if (!sub_6AB211())
        return;
    reset_selected_object_count_and_size();

    window = window_create_centred(
        600,
        400,
        &window_editor_object_selection_events,
        WC_EDITOR_OBJECT_SELECTION,
        WF_10 | WF_RESIZABLE
    );
    window_editor_object_selection_widgets[WIDX_FILTER_STRING_BUTTON].string = _filter_string;
    window->widgets = window_editor_object_selection_widgets;

    window->enabled_widgets =
        (1 << WIDX_ADVANCED) |
        (1 << WIDX_INSTALL_TRACK) |
        (1 << WIDX_FILTER_DROPDOWN) |
        (1 << WIDX_FILTER_STRING_BUTTON) |
        (1 << WIDX_FILTER_CLEAR_BUTTON) |
        (1 << WIDX_CLOSE) |
        (1 << WIDX_LIST_SORT_TYPE) |
        (((uint32)1) << WIDX_LIST_SORT_RIDE);

    _filter_flags = gConfigInterface.object_selection_filter_flags;
    memset(_filter_string, 0, sizeof(_filter_string));

    for (sint32 i = WIDX_TAB_1; i <= WIDX_TAB_11; i++)
        window->enabled_widgets |= (1LL << i);
    window_init_scroll_widgets(window);

    window->var_4AE = 0;
    window->selected_tab = 0;
    window->selected_list_item = -1;
    window->object_entry = (rct_object_entry *)-1;
    window->min_width = 600;
    window->min_height = 400;
    window->max_width = 1200;
    window->max_height = 1000;

    visible_list_refresh(window);
}

/**
 *
 *  rct2: 0x006ABCD1
 */
static void setup_track_manager_objects()
{
    sint32 numObjects = (sint32)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();
    for (sint32 i = 0; i < numObjects; i++) {
        uint8 * selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem * item = &items[i];
        uint8 object_type = item->ObjectEntry.flags & 0xF;
        if (object_type == OBJECT_TYPE_RIDE) {
            *selectionFlags |= OBJECT_SELECTION_FLAG_6;

            for (uint8 j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++) {
                uint8 rideType = item->RideType[j];
                if (rideType != RIDE_TYPE_NULL && ride_type_has_flag(rideType, RIDE_TYPE_FLAG_HAS_TRACK)) {
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
    sint32 numObjects = (sint32)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();
    for (sint32 i = 0; i < numObjects; i++) {
        uint8 * selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem * item = &items[i];
        uint8 objectType = item->ObjectEntry.flags & 0xF;
        if (objectType == OBJECT_TYPE_RIDE){
            *selectionFlags |= OBJECT_SELECTION_FLAG_6;

            for (uint8 j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++) {
                uint8 rideType = item->RideType[j];
                if (rideType != RIDE_TYPE_NULL) {
                    if (RideData4[rideType].flags & RIDE_TYPE_FLAG4_SHOW_IN_TRACK_DESIGNER) {
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
static void setup_in_use_selection_flags()
{
    for (uint8 object_type = 0; object_type < 11; object_type++){
        for (uint16 i = 0; i < object_entry_group_counts[object_type]; i++){
            gEditorSelectedObjects[object_type][i] = 0;
        }
    }

    for (uint8 object_type = 0; object_type < 11; object_type++){
        for (uint16 i = 0; i < object_entry_group_counts[object_type]; i++){
            if (object_entry_groups[object_type].chunks[i] != (uint8*)-1) {
                gEditorSelectedObjects[object_type][i] |= (1 << 1);
            }
        }
    }

    map_element_iterator iter;
    map_element_iterator_begin(&iter);
    do {
        uint16 type;
        rct_banner* banner;

        switch (map_element_get_type(iter.element)) {
        default:
        case MAP_ELEMENT_TYPE_SURFACE:
        case MAP_ELEMENT_TYPE_TRACK:
            break;
        case MAP_ELEMENT_TYPE_PATH:
            type = iter.element->properties.path.type;
            type >>= 4;
            assert(type < object_entry_group_counts[OBJECT_TYPE_PATHS]);
            gEditorSelectedObjects[OBJECT_TYPE_PATHS][type] |= (1 << 0);

            if (footpath_element_has_path_scenery(iter.element)) {
                uint8 path_additions = footpath_element_get_path_scenery_index(iter.element);
                gEditorSelectedObjects[OBJECT_TYPE_PATH_BITS][path_additions] |= 1;
            }
            break;
        case MAP_ELEMENT_TYPE_SCENERY:
            type = iter.element->properties.scenery.type;
            assert(type < object_entry_group_counts[OBJECT_TYPE_SMALL_SCENERY]);
            gEditorSelectedObjects[OBJECT_TYPE_SMALL_SCENERY][type] |= (1 << 0);
            break;
        case MAP_ELEMENT_TYPE_ENTRANCE:
            if (iter.element->properties.entrance.type != ENTRANCE_TYPE_PARK_ENTRANCE)
                break;

            gEditorSelectedObjects[OBJECT_TYPE_PARK_ENTRANCE][0] |= (1 << 0);

            type = iter.element->properties.entrance.path_type;
            assert(type < object_entry_group_counts[OBJECT_TYPE_PATHS]);
            gEditorSelectedObjects[OBJECT_TYPE_PATHS][type] |= (1 << 0);
            break;
        case MAP_ELEMENT_TYPE_WALL:
            type = iter.element->properties.wall.type;
            assert(type < object_entry_group_counts[OBJECT_TYPE_WALLS]);
            gEditorSelectedObjects[OBJECT_TYPE_WALLS][type] |= (1 << 0);
            break;
        case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
            type = iter.element->properties.scenerymultiple.type & 0x3FF;
            assert(type < object_entry_group_counts[OBJECT_TYPE_LARGE_SCENERY]);
            gEditorSelectedObjects[OBJECT_TYPE_LARGE_SCENERY][type] |= (1 << 0);
            break;
        case MAP_ELEMENT_TYPE_BANNER:
            banner = &gBanners[iter.element->properties.banner.index];
            type = banner->type;
            assert(type < object_entry_group_counts[OBJECT_TYPE_BANNERS]);
            gEditorSelectedObjects[OBJECT_TYPE_BANNERS][type] |= (1 << 0);
            break;
        }
    } while (map_element_iterator_next(&iter));

    for (uint8 ride_index = 0; ride_index < 0xFF; ride_index++) {
        Ride* ride = get_ride(ride_index);
        if (ride->type != RIDE_TYPE_NULL) {
            uint8 type = ride->subtype;
            gEditorSelectedObjects[OBJECT_TYPE_RIDE][type] |= (1 << 0);
        }
    }

    sint32 numObjects = (sint32)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();
    for (sint32 i = 0; i < numObjects; i++) {
        uint8 *selectionFlags = &_objectSelectionFlags[i];
        const ObjectRepositoryItem * item = &items[i];
        *selectionFlags &= ~OBJECT_SELECTION_FLAG_IN_USE;

        uint8 entryType, entryIndex;
        if (find_object_in_entry_group(&item->ObjectEntry, &entryType, &entryIndex)) {
            if (gEditorSelectedObjects[entryType][entryIndex] & (1 << 0)) {
                *selectionFlags |=
                    OBJECT_SELECTION_FLAG_IN_USE |
                    OBJECT_SELECTION_FLAG_SELECTED;
            }
            if (gEditorSelectedObjects[entryType][entryIndex] & (1 << 1)) {
                *selectionFlags |= OBJECT_SELECTION_FLAG_SELECTED;
            }
        }
    }
}

/**
 *
 *  rct2: 0x006AB211
 */
static sint32 sub_6AB211()
{
    sint32 numObjects = (sint32)object_repository_get_items_count();
    _objectSelectionFlags = (uint8*)calloc(numObjects, sizeof(uint8));
    if (_objectSelectionFlags == nullptr){
        log_error("Failed to allocate memory for object flag list.");
        return 0;
    }

    for (uint8 objectType = 0; objectType < 11; objectType++) {
        _numSelectedObjectsForType[objectType] = 0;
        _numAvailableObjectsForType[objectType] = 0;
    }

    const ObjectRepositoryItem * items = object_repository_get_items();
    for (sint32 i = 0; i < numObjects; i++) {
        uint8 objectType = items[i].ObjectEntry.flags & 0xF;
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
    return 1;
}

/**
 *
 *  rct2: 0x006AB316
 */
static void editor_object_flags_free()
{
    SafeFree(_objectSelectionFlags);
}

/**
 *
 *  rct2: 0x00685791
 */
static void remove_selected_objects_from_research(const rct_object_entry* installedObject){
    uint8 entry_type, entry_index;
    if (!find_object_in_entry_group(installedObject, &entry_type, &entry_index))
        return;

    if (entry_type == OBJECT_TYPE_RIDE){
        rct_ride_entry* rideEntry = (rct_ride_entry*)object_entry_groups[entry_type].chunks[entry_index];

        for (uint8 j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++) {
            research_remove(entry_index | rideEntry->ride_type[j] << 8 | 0x10000);
        }
    }
    else if (entry_type == OBJECT_TYPE_SCENERY_SETS){
        research_remove(entry_index);
    }
}

/**
 *
 *  rct2: 0x006ABB66
 */
static void unload_unselected_objects()
{
    sint32 numItems = (sint32)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();

    size_t numObjectsToUnload = 0;
    rct_object_entry * objectsToUnload = (rct_object_entry *)malloc(numItems * sizeof(rct_object_entry));

    for (sint32 i = 0; i < numItems; i++) {
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
 *  rct2: 0x006AB199
 */
static void window_editor_object_selection_close(rct_window *w)
{
    //if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
    //  return;

    unload_unselected_objects();
    editor_load_selected_objects();
    editor_object_flags_free();

    object_delete(_loadedObject);
    _loadedObject = nullptr;

    if (gScreenFlags & SCREEN_FLAGS_EDITOR) {
        research_populate_list_random();
    }
    else {
        // Used for in-game object selection cheat
        // This resets the ride selection list and resets research to 0 on current item
        gSilentResearch = true;
        research_reset_current_item();
        gSilentResearch = false;
    }
    research_remove_non_separate_vehicle_types();
    window_new_ride_init_vars();

    visible_list_dispose();
}

/**
 *
 *  rct2: 0x006AAFAB
 */
static void window_editor_object_selection_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        if (gScreenFlags & SCREEN_FLAGS_EDITOR) {
            game_do_command(0, 1, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 1, 0);
        }
        else {
            // Used for in-game object selection cheat
            window_close(w);
        }
        break;

    case WIDX_TAB_1:
    case WIDX_TAB_2:
    case WIDX_TAB_3:
    case WIDX_TAB_4:
    case WIDX_TAB_5:
    case WIDX_TAB_6:
    case WIDX_TAB_7:
    case WIDX_TAB_8:
    case WIDX_TAB_9:
    case WIDX_TAB_10:
    case WIDX_TAB_11:
        window_editor_object_set_page(w, widgetIndex - WIDX_TAB_1);
        break;
    case WIDX_FILTER_RIDE_TAB_ALL:
        _filter_flags |= FILTER_RIDES;
        gConfigInterface.object_selection_filter_flags = _filter_flags;
        config_save_default();

        filter_update_counts();
        visible_list_refresh(w);

        w->selected_list_item = -1;
        w->object_entry = (rct_object_entry *)-1;
        w->scrolls[0].v_top = 0;
        window_invalidate(w);
        break;
    case WIDX_FILTER_RIDE_TAB_TRANSPORT:
    case WIDX_FILTER_RIDE_TAB_GENTLE:
    case WIDX_FILTER_RIDE_TAB_COASTER:
    case WIDX_FILTER_RIDE_TAB_THRILL:
    case WIDX_FILTER_RIDE_TAB_WATER:
    case WIDX_FILTER_RIDE_TAB_STALL:
        _filter_flags &= ~FILTER_RIDES;
        _filter_flags |= (1 << (widgetIndex - WIDX_FILTER_RIDE_TAB_TRANSPORT + 5));
        gConfigInterface.object_selection_filter_flags = _filter_flags;
        config_save_default();

        filter_update_counts();
        visible_list_refresh(w);

        w->selected_list_item = -1;
        w->object_entry = (rct_object_entry *)-1;
        w->scrolls[0].v_top = 0;
        w->frame_no = 0;
        window_invalidate(w);
        break;

    case WIDX_ADVANCED:
        w->list_information_type ^= 1;
        window_invalidate(w);
        break;

    case WIDX_INSTALL_TRACK:
        if (w->selected_list_item != -1) {
            w->selected_list_item = -1;
        }
        window_invalidate(w);

        window_loadsave_open(LOADSAVETYPE_LOAD | LOADSAVETYPE_TRACK, nullptr);
        break;
    case WIDX_FILTER_STRING_BUTTON:
        //window_text_input_open(w, widgetIndex, STR_OBJECT_SEARCH, STR_OBJECT_SEARCH_DESC, STR_STRING, (uint32)_filter_string, 40);
        window_start_textbox(w, widgetIndex, STR_STRING, _filter_string, 40);
        break;
    case WIDX_FILTER_CLEAR_BUTTON:
        memset(_filter_string, 0, sizeof(_filter_string));
        filter_update_counts();
        w->scrolls->v_top = 0;
        visible_list_refresh(w);
        window_invalidate(w);
        break;
    case WIDX_LIST_SORT_TYPE:
        if (_listSortType == RIDE_SORT_TYPE) {
            _listSortDescending = !_listSortDescending;
        } else {
            _listSortType = RIDE_SORT_TYPE;
            _listSortDescending = false;
        }
        visible_list_refresh(w);
        break;
    case WIDX_LIST_SORT_RIDE:
        if (_listSortType == RIDE_SORT_RIDE) {
            _listSortDescending = !_listSortDescending;
        } else {
            _listSortType = RIDE_SORT_RIDE;
            _listSortDescending = false;
        }
        visible_list_refresh(w);
        break;
    }
}

static void window_editor_object_selection_resize(rct_window *w)
{
    window_set_resize(w, 600, 400, 1200, 1000);
}

void window_editor_object_selection_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    sint32 num_items;

    switch (widgetIndex) {
    case WIDX_FILTER_DROPDOWN:

        num_items = 4;
        gDropdownItemsFormat[DDIX_FILTER_RCT2] = STR_TOGGLE_OPTION;
        gDropdownItemsFormat[DDIX_FILTER_WW] = STR_TOGGLE_OPTION;
        gDropdownItemsFormat[DDIX_FILTER_TT] = STR_TOGGLE_OPTION;
        gDropdownItemsFormat[DDIX_FILTER_CUSTOM] = STR_TOGGLE_OPTION;
        gDropdownItemsArgs[DDIX_FILTER_RCT2] = STR_ROLLERCOASTER_TYCOON_2_DROPDOWN;
        gDropdownItemsArgs[DDIX_FILTER_WW] = STR_OBJECT_FILTER_WW;
        gDropdownItemsArgs[DDIX_FILTER_TT] = STR_OBJECT_FILTER_TT;
        gDropdownItemsArgs[DDIX_FILTER_CUSTOM] = STR_OBJECT_FILTER_CUSTOM;
        // Track manager cannot select multiple, so only show selection filters if not in track manager
        if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
            num_items = 7;
            gDropdownItemsFormat[DDIX_FILTER_SEPERATOR] = 0;
            gDropdownItemsFormat[DDIX_FILTER_SELECTED] = STR_TOGGLE_OPTION;
            gDropdownItemsFormat[DDIX_FILTER_NONSELECTED] = STR_TOGGLE_OPTION;
            gDropdownItemsArgs[DDIX_FILTER_SEPERATOR] = STR_NONE;
            gDropdownItemsArgs[DDIX_FILTER_SELECTED] = STR_SELECTED_ONLY;
            gDropdownItemsArgs[DDIX_FILTER_NONSELECTED] = STR_NON_SELECTED_ONLY;
        }

        window_dropdown_show_text(
            w->x + widget->left,
            w->y + widget->top,
            widget->bottom - widget->top + 1,
            w->colours[widget->colour],
            DROPDOWN_FLAG_STAY_OPEN,
            num_items
            );

        for (sint32 i = 0; i < 4; i++)
        {
            if (_filter_flags & (1 << i))
            {
                dropdown_set_checked(i, true);
            }
        }

        if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
            dropdown_set_checked(DDIX_FILTER_SELECTED, _FILTER_SELECTED != 0);
            dropdown_set_checked(DDIX_FILTER_NONSELECTED, _FILTER_NONSELECTED != 0);
        }
        break;

    }
}

static void window_editor_object_selection_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex) {
    case WIDX_FILTER_DROPDOWN:
        if (dropdownIndex == DDIX_FILTER_SELECTED) {
            _filter_flags ^= FILTER_SELECTED;
            _filter_flags &= ~FILTER_NONSELECTED;
        }
        else if (dropdownIndex == DDIX_FILTER_NONSELECTED) {
            _filter_flags ^= FILTER_NONSELECTED;
            _filter_flags &= ~FILTER_SELECTED;
        }
        else {
            _filter_flags ^= (1 << dropdownIndex);
        }
        gConfigInterface.object_selection_filter_flags = _filter_flags;
        config_save_default();

        filter_update_counts();
        w->scrolls->v_top = 0;

        visible_list_refresh(w);
        window_invalidate(w);
        break;
    }
}

/**
 *
 *  rct2: 0x006AB031
 */
static void window_editor_object_selection_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
    *height = _numListItems * 12;
}

/**
 *
 *  rct2: 0x006AB0B6
 */
static void window_editor_object_selection_scroll_mousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    // Used for in-game object selection cheat to prevent crashing the game
    // when windows attempt to draw objects that don't exist any more
    window_close_all_except_class(WC_EDITOR_OBJECT_SELECTION);

    sint32 selected_object = get_object_from_object_selection((w->selected_tab & 0xFF), y);
    if (selected_object == -1)
        return;

    list_item * listItem = &_listItems[selected_object];
    uint8 object_selection_flags = *listItem->flags;
    if (object_selection_flags & OBJECT_SELECTION_FLAG_6)
        return;

    window_invalidate(w);

    const CursorState * state = context_get_cursor_state();
    audio_play_sound(SOUND_CLICK_1, 0, state->x);


    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
        if (!window_editor_object_selection_select_object(0, 1, listItem->entry))
            return;

        // Close any other open windows such as options/colour schemes to prevent a crash.
        window_close_all();
        //window_close(w);

        // This function calls window_track_list_open
        window_editor_object_selection_manage_tracks();
        return;
    }

    sint32 ebx = 6;
    // If already selected
    if (!(object_selection_flags & OBJECT_SELECTION_FLAG_SELECTED))
        ebx = 7;

    _maxObjectsWasHit = false;
    if (!window_editor_object_selection_select_object(0, ebx, listItem->entry)) {
        rct_string_id error_title = (ebx & 1) ?
            STR_UNABLE_TO_SELECT_THIS_OBJECT :
            STR_UNABLE_TO_DE_SELECT_THIS_OBJECT;

        window_error_open(error_title, gGameCommandErrorText);
        return;
    }

    if (_FILTER_SELECTED || _FILTER_NONSELECTED) {
        filter_update_counts();
        visible_list_refresh(w);
        window_invalidate(w);
    }

    if (_maxObjectsWasHit) {
        window_error_open(STR_WARNING_TOO_MANY_OBJECTS_SELECTED, STR_NOT_ALL_OBJECTS_IN_THIS_SCENERY_GROUP_COULD_BE_SELECTED);
    }
}

/**
 *
 *  rct2: 0x006AB079
 */
static void window_editor_object_selection_scroll_mouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    sint32 selectedObject = get_object_from_object_selection(w->selected_tab & 0xFF, y);
    if (selectedObject != -1) {
        list_item * listItem = &_listItems[selectedObject];
        uint8 objectSelectionFlags = *listItem->flags;
        if (objectSelectionFlags & OBJECT_SELECTION_FLAG_6) {
            selectedObject = -1;
        }
    }
    if (selectedObject != w->selected_list_item) {
        w->selected_list_item = selectedObject;

        object_delete(_loadedObject);
        _loadedObject = nullptr;

        list_item * listItem = &_listItems[selectedObject];
        if (selectedObject == -1) {
            w->object_entry = nullptr;
        } else {
            w->object_entry = listItem->entry;
            _loadedObject = object_repository_load_object(listItem->entry);
        }

        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x006AB058
 */
static void window_editor_object_selection_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
    switch (widgetIndex) {
    case WIDX_TAB_1:
    case WIDX_TAB_2:
    case WIDX_TAB_3:
    case WIDX_TAB_4:
    case WIDX_TAB_5:
    case WIDX_TAB_6:
    case WIDX_TAB_7:
    case WIDX_TAB_8:
    case WIDX_TAB_9:
    case WIDX_TAB_10:
    case WIDX_TAB_11:
        set_format_arg(0, rct_string_id, ObjectSelectionPageNames[(widgetIndex - WIDX_TAB_1)]);
        break;
    default:
        set_format_arg(0, rct_string_id, STR_LIST);
        break;
    }
}

/**
 *
 *  rct2: 0x006AA9FD
 */
static void window_editor_object_selection_invalidate(rct_window *w)
{
    // Resize widgets
    w->widgets[WIDX_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_TITLE].right = w->width - 2;
    w->widgets[WIDX_CLOSE].left = w->width - 13;
    w->widgets[WIDX_CLOSE].right = w->width - 3;
    w->widgets[WIDX_TAB_CONTENT_PANEL].right = w->width - 1;
    w->widgets[WIDX_TAB_CONTENT_PANEL].bottom = w->height - 1;
    w->widgets[WIDX_ADVANCED].left = w->width - 130;
    w->widgets[WIDX_ADVANCED].right = w->width - 9;
    w->widgets[WIDX_LIST].right = w->width - 309;
    w->widgets[WIDX_LIST].bottom = w->height - 14;
    w->widgets[WIDX_PREVIEW].left = w->width - 209;
    w->widgets[WIDX_PREVIEW].right = w->width - 96;
    w->widgets[WIDX_INSTALL_TRACK].left = w->width - 130;
    w->widgets[WIDX_INSTALL_TRACK].right = w->width - 9;
    w->widgets[WIDX_FILTER_DROPDOWN].left = w->width - 250;
    w->widgets[WIDX_FILTER_DROPDOWN].right = w->width - 137;

    // Set pressed widgets
    w->pressed_widgets |= 1 << WIDX_PREVIEW;
    window_editor_object_selection_set_pressed_tab(w);
    if (w->list_information_type & 1)
        w->pressed_widgets |= (1 << WIDX_ADVANCED);
    else
        w->pressed_widgets &= ~(1 << WIDX_ADVANCED);

    // Set window title and buttons
    set_format_arg(0, rct_string_id, ObjectSelectionPageNames[w->selected_tab]);
    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
        w->widgets[WIDX_TITLE].text = STR_TRACK_DESIGNS_MANAGER_SELECT_RIDE_TYPE;
        w->widgets[WIDX_INSTALL_TRACK].type = WWT_DROPDOWN_BUTTON;
    } else if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) {
        w->widgets[WIDX_TITLE].text = STR_ROLLER_COASTER_DESIGNER_SELECT_RIDE_TYPES_VEHICLES;
        w->widgets[WIDX_INSTALL_TRACK].type = WWT_EMPTY;
    } else {
        w->widgets[WIDX_TITLE].text = STR_OBJECT_SELECTION;
        w->widgets[WIDX_INSTALL_TRACK].type = WWT_EMPTY;
    }

    // Align tabs, hide advanced ones
    sint32 x = 3;
    for (sint32 i = 0; i < WINDOW_OBJECT_SELECTION_PAGE_COUNT; i++) {
        rct_widget *widget = &w->widgets[WIDX_TAB_1 + i];

        if (!(w->list_information_type & 1) && ((1 << i) & 0x5E)) {
            widget->type = WWT_EMPTY;
        } else {
            widget->type = WWT_TAB;
            widget->left = x;
            widget->right = x + 30;
            x += 31;
        }
    }

    if (gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TRACK_DESIGNER)) {
        w->widgets[WIDX_ADVANCED].type = WWT_EMPTY;
        for (sint32 i = 1; i < WINDOW_OBJECT_SELECTION_PAGE_COUNT; i++)
            w->widgets[WIDX_TAB_1 + i].type = WWT_EMPTY;
        x = 150;
    } else {
        w->widgets[WIDX_ADVANCED].type = WWT_DROPDOWN_BUTTON;
        x = 300;
    }

    w->widgets[WIDX_FILTER_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
    w->widgets[WIDX_LIST].right = w->width - (600 - 587) - x;
    w->widgets[WIDX_PREVIEW].left = w->width - (600 - 537) - (x / 2);
    w->widgets[WIDX_PREVIEW].right = w->widgets[WIDX_PREVIEW].left + 113;
    w->widgets[WIDX_FILTER_RIDE_TAB_FRAME].right = w->widgets[WIDX_LIST].right;

    bool ridePage = (w->selected_tab == WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS);
    w->widgets[WIDX_LIST].top = (ridePage ? 118 : 60);
    w->widgets[WIDX_FILTER_STRING_BUTTON].right = w->widgets[WIDX_LIST].right - 77;
    w->widgets[WIDX_FILTER_STRING_BUTTON].top = (ridePage ? 80 : 46);
    w->widgets[WIDX_FILTER_STRING_BUTTON].bottom = (ridePage ? 91 : 57);
    w->widgets[WIDX_FILTER_CLEAR_BUTTON].left = w->widgets[WIDX_LIST].right - 73;
    w->widgets[WIDX_FILTER_CLEAR_BUTTON].right = w->widgets[WIDX_LIST].right;
    w->widgets[WIDX_FILTER_CLEAR_BUTTON].top = (ridePage ? 80 : 46);
    w->widgets[WIDX_FILTER_CLEAR_BUTTON].bottom = (ridePage ? 91 : 57);

    if (ridePage) {
        w->enabled_widgets |= (1 << WIDX_FILTER_RIDE_TAB_ALL) | (1 << WIDX_FILTER_RIDE_TAB_TRANSPORT) |
            (1 << WIDX_FILTER_RIDE_TAB_GENTLE) | (1 << WIDX_FILTER_RIDE_TAB_COASTER) | (1 << WIDX_FILTER_RIDE_TAB_THRILL) |
            (1 << WIDX_FILTER_RIDE_TAB_WATER) | (1 << WIDX_FILTER_RIDE_TAB_STALL);
        for (sint32 i = 0; i < 7; i++)
            w->pressed_widgets &= ~(1 << (WIDX_FILTER_RIDE_TAB_ALL + i));
        if ((_filter_flags & FILTER_RIDES) == FILTER_RIDES)
            w->pressed_widgets |= (1 << WIDX_FILTER_RIDE_TAB_ALL);
        else {
            for (sint32 i = 0; i < 6; i++) {
                if (_filter_flags & (1 << (5 + i)))
                    w->pressed_widgets |= (uint64)(1ULL << (WIDX_FILTER_RIDE_TAB_TRANSPORT + i));
            }
        }
        w->widgets[WIDX_FILTER_RIDE_TAB_FRAME].type = WWT_IMGBTN;
        for (sint32 i = WIDX_FILTER_RIDE_TAB_ALL; i <= WIDX_FILTER_RIDE_TAB_STALL; i++)
            w->widgets[i].type = WWT_TAB;

        w->widgets[WIDX_LIST_SORT_TYPE].type = WWT_13;
        w->widgets[WIDX_LIST_SORT_TYPE].top = w->widgets[WIDX_FILTER_STRING_BUTTON].bottom + 3;
        w->widgets[WIDX_LIST_SORT_TYPE].bottom = w->widgets[WIDX_LIST_SORT_TYPE].top + 13;
        w->widgets[WIDX_LIST_SORT_RIDE].type = WWT_13;
        w->widgets[WIDX_LIST_SORT_RIDE].top = w->widgets[WIDX_LIST_SORT_TYPE].top;
        w->widgets[WIDX_LIST_SORT_RIDE].bottom = w->widgets[WIDX_LIST_SORT_TYPE].bottom;
        w->widgets[WIDX_LIST_SORT_RIDE].right = w->widgets[WIDX_LIST].right;
        w->widgets[WIDX_LIST].top = w->widgets[WIDX_LIST_SORT_TYPE].bottom + 2;
    } else {
        w->enabled_widgets &= ~((1 << WIDX_FILTER_RIDE_TAB_ALL) | (1 << WIDX_FILTER_RIDE_TAB_TRANSPORT) |
            (1 << WIDX_FILTER_RIDE_TAB_GENTLE) | (1 << WIDX_FILTER_RIDE_TAB_COASTER) | (1 << WIDX_FILTER_RIDE_TAB_THRILL) |
            (1 << WIDX_FILTER_RIDE_TAB_WATER) | (1 << WIDX_FILTER_RIDE_TAB_STALL));
        for (sint32 i = WIDX_FILTER_RIDE_TAB_FRAME; i <= WIDX_FILTER_RIDE_TAB_STALL; i++)
            w->widgets[i].type = WWT_EMPTY;

        w->widgets[WIDX_LIST_SORT_TYPE].type = WWT_EMPTY;
        w->widgets[WIDX_LIST_SORT_RIDE].type = WWT_EMPTY;
    }
}

/**
 *
 *  rct2: 0x006AAB56
 */
static void window_editor_object_selection_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    sint32 i, x, y, width, type;
    rct_widget *widget;
    rct_object_entry *highlightedEntry;
    rct_string_id stringId;
    uint8 source;

    /*if (w->selected_tab == WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS) {
        gfx_fill_rect_inset(dpi,
            w->x + w->widgets[WIDX_FILTER_RIDE_TAB_ALL].left - 1,
            w->y + w->widgets[WIDX_FILTER_RIDE_TAB_ALL].bottom,
            w->x + w->widgets[WIDX_FILTER_RIDE_TAB_STALL].right + 1,
            w->y + w->widgets[WIDX_FILTER_RIDE_TAB_ALL].bottom + 2,
            w->colours[1],
            0x10
            );
    }*/

    window_draw_widgets(w, dpi);

    // Draw tabs
    for (i = 0; i < WINDOW_OBJECT_SELECTION_PAGE_COUNT; i++) {
        widget = &w->widgets[WIDX_TAB_1 + i];
        if (widget->type == WWT_EMPTY)
            continue;

        x = w->x + widget->left;
        y = w->y + widget->top;
        gfx_draw_sprite(dpi, SPR_TAB_RIDE_16 + i, x, y, 0);
    }

    const sint32 ride_tabs[] = { SPR_TAB_RIDE_16, IMAGE_TYPE_REMAP | SPR_TAB_RIDES_TRANSPORT_0, SPR_TAB_RIDES_GENTLE_0, IMAGE_TYPE_REMAP | SPR_TAB_RIDES_ROLLER_COASTERS_0, SPR_TAB_RIDES_THRILL_0, SPR_TAB_RIDES_WATER_0, SPR_TAB_RIDES_SHOP_0, SPR_TAB_FINANCES_RESEARCH_0 };
    const sint32 ThrillRidesTabAnimationSequence[] = {
        5, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0
    };

    // Draw ride tabs
    if (w->selected_tab == WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS) {
        for (i = 0; i < 7; i++) {
            widget = &w->widgets[WIDX_FILTER_RIDE_TAB_ALL + i];
            if (widget->type == WWT_EMPTY)
                continue;

            sint32 spriteIndex = ride_tabs[i];
            sint32 frame = 0;
            if (i != 0 && w->pressed_widgets & (1ULL << (WIDX_FILTER_RIDE_TAB_ALL + i))) {
                frame = w->frame_no / window_editor_object_selection_animation_divisor[i - 1];
            }
            spriteIndex += (i == 4 ? ThrillRidesTabAnimationSequence[frame] : frame);

            x = w->x + widget->left;
            y = w->y + widget->top;
            gfx_draw_sprite(dpi, spriteIndex | (w->colours[1] << 19), x, y, 0);
        }

    }

    // Preview background
    widget = &w->widgets[WIDX_PREVIEW];
    gfx_fill_rect(
        dpi,
        w->x + widget->left + 1,
        w->y + widget->top + 1,
        w->x + widget->right - 1,
        w->y + widget->bottom - 1,
        ColourMapA[w->colours[1]].darkest
    );

    // Draw number of selected items
    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
        x = w->x + 3;
        y = w->y + w->height - 13;

        sint32 numSelected = _numSelectedObjectsForType[w->selected_tab];
        sint32 totalSelectable = object_entry_group_counts[w->selected_tab];
        if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
            totalSelectable = 4;

        set_format_arg(0, uint16, numSelected);
        set_format_arg(2, uint16, totalSelectable);
        gfx_draw_string_left(dpi, STR_OBJECT_SELECTION_SELECTION_SIZE, gCommonFormatArgs, COLOUR_BLACK, x, y);
    }

    // Draw sort button text
    widget = &w->widgets[WIDX_LIST_SORT_TYPE];
    if (widget->type != WWT_EMPTY) {
        stringId = _listSortType == RIDE_SORT_TYPE ? (_listSortDescending ? STR_DOWN : STR_UP) : STR_NONE;
        gfx_draw_string_left_clipped(dpi, STR_OBJECTS_SORT_TYPE, &stringId, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
    }
    widget = &w->widgets[WIDX_LIST_SORT_RIDE];
    if (widget->type != WWT_EMPTY) {
        stringId = _listSortType == RIDE_SORT_RIDE ? (_listSortDescending ? STR_DOWN : STR_UP) : STR_NONE;
        gfx_draw_string_left_clipped(dpi, STR_OBJECTS_SORT_RIDE, &stringId, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
    }

    if (w->selected_list_item == -1 || _loadedObject == nullptr)
        return;

    list_item *listItem = &_listItems[w->selected_list_item];

    highlightedEntry = w->object_entry;
    type = highlightedEntry->flags & 0x0F;

    // Draw preview
    widget = &w->widgets[WIDX_PREVIEW];
    {
        rct_drawpixelinfo clipDPI;
        x = w->x + widget->left + 1;
        y = w->y + widget->top + 1;
        width = widget->right - widget->left - 1;
        sint32 height = widget->bottom - widget->top - 1;
        if (clip_drawpixelinfo(&clipDPI, dpi, x, y, width, height)) {
            object_draw_preview(_loadedObject, &clipDPI, width, height);
        }
    }

    // Draw name of object
    x = w->x + (widget->left + widget->right) / 2 + 1;
    y = w->y + widget->bottom + 3;
    width = w->width - w->widgets[WIDX_LIST].right - 6;
    set_format_arg(0, rct_string_id, STR_STRING);
    set_format_arg(2, const char *, listItem->repositoryItem->Name);
    gfx_draw_string_centred_clipped(dpi, STR_WINDOW_COLOUR_2_STRINGID, gCommonFormatArgs, COLOUR_BLACK, x, y, width);

    // Draw description of object
    const char *description = object_get_description(_loadedObject);
    if (description != nullptr) {
        set_format_arg(0, rct_string_id, STR_STRING);
        set_format_arg(2, const char *, description);

        x = w->x + w->widgets[WIDX_LIST].right + 4;
        y += 15;
        width = w->x + w->width - x - 4;
        if (type == OBJECT_TYPE_SCENARIO_TEXT) {
            gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, x, y, width, STR_OBJECT_SELECTION_DESCRIPTION_SCENARIO_TEXT, COLOUR_BLACK);
        } else {
            gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, x, y + 5, width, STR_WINDOW_COLOUR_2_STRINGID, COLOUR_BLACK);
        }
    }

    // Draw object source
    source = (highlightedEntry->flags & 0xF0) >> 4;
    switch (source) {
    case 8: stringId = STR_ROLLERCOASTER_TYCOON_2_DROPDOWN; break;
    case 1: stringId = STR_OBJECT_FILTER_WW; break;
    case 2: stringId = STR_OBJECT_FILTER_TT; break;
    default: stringId = STR_OBJECT_FILTER_CUSTOM; break;
    }
    gfx_draw_string_right(dpi, stringId, nullptr, COLOUR_WHITE, w->x + w->width - 5, w->y + w->height - 3 - 12 - 14);

    //
    if (w->selected_tab == WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS) {
        y = w->y + w->height - 3 - 12 - 14 - 14;
        stringId = get_ride_type_string_id(listItem->repositoryItem);
        gfx_draw_string_right(dpi, stringId, nullptr, COLOUR_WHITE, w->x + w->width - 5, y);
        y -= 11;
    }

    //stringId = highlightedEntry->checksum
    // gfx_draw_string_right(dpi, stringId, nullptr, 2, w->x + w->width - 5, w->y + w->height - 3 - 12 - 14);

    // Draw object dat name
    const char *path = path_get_filename(listItem->repositoryItem->Path);
    set_format_arg(0, rct_string_id, STR_STRING);
    set_format_arg(2, const char *, path);
    gfx_draw_string_right(dpi, STR_WINDOW_COLOUR_2_STRINGID, gCommonFormatArgs, COLOUR_BLACK, w->x + w->width - 5, w->y + w->height - 3 - 12);
}

/**
 *
 *  rct2: 0x006AADA3
 */
static void window_editor_object_selection_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
    sint32 x, y, i, colour, colour2;

    bool ridePage = (w->selected_tab == WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS);

    uint8 paletteIndex = ColourMapA[w->colours[1]].mid_light;
    gfx_clear(dpi, paletteIndex);

    y = 0;
    for (i = 0; i < _numListItems; i++) {
        list_item *listItem = &_listItems[i];

        if (y + 12 >= dpi->y && y <= dpi->y + dpi->height) {
            // Draw checkbox
            if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && !(*listItem->flags & 0x20))
                gfx_fill_rect_inset(dpi, 2, y, 11, y + 10, w->colours[1], INSET_RECT_F_E0);

            // Highlight background
            colour = COLOUR_BRIGHT_GREEN | COLOUR_FLAG_TRANSLUCENT;
            if (listItem->entry == w->object_entry && !(*listItem->flags & OBJECT_SELECTION_FLAG_6)) {
                gfx_filter_rect(dpi, 0, y, w->width, y + 11, PALETTE_DARKEN_1);
                colour = COLOUR_BRIGHT_GREEN;
            }

            // Draw checkmark
            if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && (*listItem->flags & OBJECT_SELECTION_FLAG_SELECTED)) {
                x = 2;
                gCurrentFontSpriteBase = colour == COLOUR_BRIGHT_GREEN ? FONT_SPRITE_BASE_MEDIUM_EXTRA_DARK : FONT_SPRITE_BASE_MEDIUM_DARK;
                colour2 = NOT_TRANSLUCENT(w->colours[1]);
                if (*listItem->flags & (OBJECT_SELECTION_FLAG_IN_USE | OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED))
                    colour2 |= COLOUR_FLAG_INSET;

                gfx_draw_string(dpi, (char*)CheckBoxMarkString, colour2, x, y);
            }

            x = gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER ? 0 : 15;

            char *bufferWithColour = gCommonStringFormatBuffer;
            char *buffer = utf8_write_codepoint(bufferWithColour, colour);
            if (*listItem->flags & OBJECT_SELECTION_FLAG_6) {
                colour = w->colours[1] & 0x7F;
                gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM_DARK;
            }
            else {
                colour = COLOUR_BLACK;
                gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
            }

            if (ridePage) {
                // Draw ride type
                rct_string_id rideTypeStringId = get_ride_type_string_id(listItem->repositoryItem);
                safe_strcpy(buffer, language_get_string(rideTypeStringId), 256 - (buffer - bufferWithColour));
                gfx_draw_string(dpi, bufferWithColour, colour, x, y);
                x = w->widgets[WIDX_LIST_SORT_RIDE].left - w->widgets[WIDX_LIST].left;
            }

            // Draw text
            safe_strcpy(buffer, listItem->repositoryItem->Name, 256 - (buffer - bufferWithColour));
            if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
                while (*buffer != 0 && *buffer != 9)
                    buffer++;

                *buffer = 0;
            }
            gfx_draw_string(dpi, bufferWithColour, colour, x, y);
        }
        y += 12;
    }
}

static void window_editor_object_set_page(rct_window *w, sint32 page)
{
    if (w->selected_tab == page)
        return;

    w->selected_tab = page;
    w->selected_list_item = -1;
    w->object_entry = (rct_object_entry *)-1;
    w->scrolls[0].v_top = 0;
    w->frame_no = 0;

    if (page == WINDOW_OBJECT_SELECTION_PAGE_RIDE_VEHICLES_ATTRACTIONS) {
        _listSortType = RIDE_SORT_TYPE;
        _listSortDescending = false;
    } else {
        _listSortType = RIDE_SORT_RIDE;
        _listSortDescending = false;
    }

    visible_list_refresh(w);
    window_invalidate(w);
}

static void window_editor_object_selection_set_pressed_tab(rct_window *w)
{
    sint32 i;
    for (i = 0; i < WINDOW_OBJECT_SELECTION_PAGE_COUNT; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->selected_tab);
}

/**
*
*  rct2: 0x006AA805
*/
static void window_editor_object_selection_select_default_objects()
{
    if (_numSelectedObjectsForType[0] == 0) {
        for (sint32 i = 0; i < (sint32)Util::CountOf(DefaultSelectedObjects); i++) {
            window_editor_object_selection_select_object(0, 7, &DefaultSelectedObjects[i]);
        }
    }
}

/**
 *
 *  rct2: 0x006AA7E9
 */
static void window_editor_object_selection_select_required_objects()
{
    sint32 i;

    for (i = 0; i < (sint32)Util::CountOf(RequiredSelectedObjects); i++)
        window_editor_object_selection_select_object(0, 0xF, &RequiredSelectedObjects[i]);
}

/**
 *
 *  rct2: 0x006AA770
 */
void reset_selected_object_count_and_size()
{
    for (uint8 objectType = 0; objectType < 11; objectType++) {
        _numSelectedObjectsForType[objectType] = 0;
    }

    sint32 numObjects = (sint32)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();
    for (sint32 i = 0; i < numObjects; i++) {
        uint8 objectType = items[i].ObjectEntry.flags & 0xF;
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
static void set_object_selection_error(uint8 is_master_object, rct_string_id error_msg){
    gGameCommandErrorText = error_msg;
    if (!is_master_object){
        reset_selected_object_count_and_size();
    }
}

/**
 *
 *  rct2: 0x006AB54F
 */
static sint32 window_editor_object_selection_select_object(uint8 bh, sint32 flags, const rct_object_entry *entry)
{
    sint32 numObjects = (sint32)object_repository_get_items_count();
    const ObjectRepositoryItem * item = object_repository_find_object_by_entry(entry);
    if (item == nullptr) {
        set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_OBJECT_DATA_NOT_FOUND);
        return 0;
    }

    // Get repository item index
    sint32 index = -1;
    const ObjectRepositoryItem * items = object_repository_get_items();
    for (sint32 i = 0; i < numObjects; i++) {
        if (&items[i] == item) {
            index = i;
        }
    }

    uint8 * selectionFlags = &_objectSelectionFlags[index];
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

        uint8 objectType = item->ObjectEntry.flags & 0xF;
        if (objectType == OBJECT_TYPE_SCENERY_SETS && (flags & (1 << 2))) {
            for (sint32 j = 0; j < item->NumThemeObjects; j++) {
                window_editor_object_selection_select_object(++bh, flags, &item->ThemeObjects[j]);
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

        uint8 objectType = item->ObjectEntry.flags & 0xF;
        uint16 maxObjects = object_entry_group_counts[objectType];
        if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) {
            maxObjects = 4;
        }

        if (maxObjects <= _numSelectedObjectsForType[objectType]) {
            set_object_selection_error(bh, STR_OBJECT_SELECTION_ERR_TOO_MANY_OF_TYPE_SELECTED);
            return 0;
        }

        if (objectType == OBJECT_TYPE_SCENERY_SETS && (flags & (1 << 2))) {
            for (uint16 j = 0; j < item->NumThemeObjects; j++) {
                if (!window_editor_object_selection_select_object(++bh, flags, &item->ThemeObjects[j])) {
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

/**
 * Takes the y coordinate of the clicked on scroll list
 * and converts this into an object selection.
 * Returns the position in the list.
 * Object_selection_flags, installed_entry also populated
 *
 *  rct2: 0x006AA703
 */
static sint32 get_object_from_object_selection(uint8 object_type, sint32 y)
{
    sint32 listItemIndex = y / 12;
    if (listItemIndex < 0 || listItemIndex >= _numListItems)
        return -1;

    return listItemIndex;
}

/**
 *
 *  rct2: 0x006D33E2
 */
static void window_editor_object_selection_manage_tracks()
{
    reset_researched_ride_types_and_entries();

    gS6Info.editor_step = EDITOR_STEP_TRACK_DESIGNS_MANAGER;

    sint32 entry_index = 0;
    for (; ((intptr_t)object_entry_groups[0].chunks[entry_index]) == -1; ++entry_index);

    rct_ride_entry* ride_entry = get_ride_entry(entry_index);
    uint8 ride_type = ride_entry_get_first_non_null_ride_type(ride_entry);

    ride_list_item item = { ride_type, (uint8) entry_index };
    // track_load_list(item);
    window_track_list_open(item);
}

/**
 *
 *  rct2: 0x006ABBBE
 */
static void editor_load_selected_objects()
{
    sint32 numItems = (sint32)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();
    for (sint32 i = 0; i < numItems; i++) {
        if (_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_SELECTED) {
            const ObjectRepositoryItem * item = &items[i];
            const rct_object_entry * entry = &item->ObjectEntry;
            void * loadedObject = object_manager_get_loaded_object(entry);
            if (loadedObject == nullptr) {
                loadedObject = object_manager_load_object(entry);
                if (loadedObject == nullptr) {
                    log_error("Failed to load entry %.8s", entry->name);
                }
                else if (!(gScreenFlags & SCREEN_FLAGS_EDITOR)) {
                    // Defaults selected items to researched (if in-game)
                    uint8 objectType = entry->flags & 0x0F;
                    uint8 entryIndex = object_manager_get_loaded_object_entry_index(loadedObject);
                    if (objectType == OBJECT_TYPE_RIDE) {
                        rct_ride_entry *rideEntry = get_ride_entry(entryIndex);
                        uint8 rideType = ride_entry_get_first_non_null_ride_type(rideEntry);
                        research_insert(1, 0x10000 | (rideType << 8) | entryIndex, rideEntry->category[0]);
                    }
                    else if (objectType == OBJECT_TYPE_SCENERY_SETS) {
                        research_insert(1, entryIndex, RESEARCH_CATEGORY_SCENERYSET);
                    }
                }
            }
        }
    }
}

static void window_editor_object_selection_update(rct_window *w)
{
    if (gCurrentTextBox.window.classification == w->classification &&
        gCurrentTextBox.window.number == w->number) {
        window_update_textbox_caret();
        widget_invalidate(w, WIDX_FILTER_STRING_BUTTON);
    }

    for (rct_widgetindex i = WIDX_FILTER_RIDE_TAB_TRANSPORT; i <= WIDX_FILTER_RIDE_TAB_STALL; i++) {
        if (!(w->pressed_widgets & (1ULL << i)))
            continue;

        w->frame_no++;
        if (w->frame_no >= window_editor_object_selection_animation_loops[i - WIDX_FILTER_RIDE_TAB_TRANSPORT])
            w->frame_no = 0;

        widget_invalidate(w, i);
        break;
    }
}

static void window_editor_object_selection_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
    if (widgetIndex != WIDX_FILTER_STRING_BUTTON || text == nullptr)
        return;

    if (strcmp(_filter_string, text) == 0)
        return;

    if (strlen(text) == 0) {
        memset(_filter_string, 0, sizeof(_filter_string));
    }
    else {
        memset(_filter_string, 0, sizeof(_filter_string));
        safe_strcpy(_filter_string, text, sizeof(_filter_string));
    }

    filter_update_counts();

    w->scrolls->v_top = 0;

    visible_list_refresh(w);
    window_invalidate(w);
}

static bool filter_selected(uint8 objectFlag)
{
    if (_FILTER_SELECTED == _FILTER_NONSELECTED) {
        return true;
    }
    if (_FILTER_SELECTED && objectFlag & OBJECT_SELECTION_FLAG_SELECTED) {
        return true;
    }
    else if (_FILTER_NONSELECTED && !(objectFlag & OBJECT_SELECTION_FLAG_SELECTED)) {
        return true;
    }
    else {
        return false;
    }
}

static bool filter_string(const ObjectRepositoryItem * item)
{
    // Nothing to search for
    if (_filter_string[0] == '\0')
        return true;

    // Object doesn't have a name
    utf8 *name = item->Name;
    if (name == nullptr || name[0] == '\0')
        return false;

    // Get ride type
    const char *rideTypeName = language_get_string(get_ride_type_string_id(item));

    // Get object name (ride/vehicle for rides) and type name (rides only)
    char name_lower[MAX_PATH];
    char type_lower[MAX_PATH];
    char filter_lower[sizeof(_filter_string)];
    safe_strcpy(name_lower, name, MAX_PATH);
    safe_strcpy(type_lower, rideTypeName, MAX_PATH);
    safe_strcpy(filter_lower, _filter_string, sizeof(_filter_string));

    // Make use of lowercase characters only
    for (sint32 i = 0; name_lower[i] != '\0'; i++)
        name_lower[i] = (char)tolower(name_lower[i]);
    for (sint32 i = 0; type_lower[i] != '\0'; i++)
        type_lower[i] = (char)tolower(type_lower[i]);
    for (sint32 i = 0; filter_lower[i] != '\0'; i++)
        filter_lower[i] = (char)tolower(filter_lower[i]);

    return strstr(name_lower, filter_lower) != nullptr || (((item->ObjectEntry.flags & 0x0F) == OBJECT_TYPE_RIDE) && strstr(type_lower, filter_lower) != nullptr);
}

static bool filter_source(const ObjectRepositoryItem * item)
{
    if (_FILTER_ALL)
        return true;

    uint8 source = (item->ObjectEntry.flags & 0xF0) >> 4;
    return (_FILTER_RCT2 && source == 8) || (_FILTER_WW && source == 1) || (_FILTER_TT && source == 2) || (_FILTER_CUSTOM && source != 8 && source != 1 && source != 2);
}

static bool filter_chunks(const ObjectRepositoryItem * item)
{
    switch (item->ObjectEntry.flags & 0x0F) {
    case OBJECT_TYPE_RIDE:
        if(!gConfigInterface.select_by_track_type) {
            if (_filter_flags & (1 << (item->RideCategory[0] + 5)))
                return true;
            if (_filter_flags & (1 << (item->RideCategory[1] + 5)))
                return true;
        }
        else {
            uint8 rideType = 0;
            for (sint32 i = 0; i < MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++) {
                if (item->RideType[i] != RIDE_TYPE_NULL) {
                    rideType = item->RideType[i];
                    break;
                }
            }
            if (_filter_flags & (1 << (gRideCategories[rideType] + 5)))
                return true;
        }
        return false;
    }
    return true;
}

static void filter_update_counts()
{
    if (!_FILTER_ALL || strlen(_filter_string) > 0) {
        uint8 *selectionFlags = _objectSelectionFlags;
        for (sint32 i = 0; i < 11; i++) {
            _filter_object_counts[i] = 0;
        }

        size_t numObjects = object_repository_get_items_count();
        const ObjectRepositoryItem * items = object_repository_get_items();
        for (size_t i = 0; i < numObjects; i++) {
            const ObjectRepositoryItem * item = &items[i];
            if (filter_source(item) &&
                filter_string(item) &&
                filter_chunks(item) &&
                filter_selected(*selectionFlags)
            ) {
                uint8 objectType = item->ObjectEntry.flags & 0xF;
                _filter_object_counts[objectType]++;
            }
            selectionFlags++;
        }
    }
}

static rct_string_id get_ride_type_string_id(const ObjectRepositoryItem * item)
{
    rct_string_id result = STR_NONE;
    for (sint32 i = 0; i < MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++) {
        uint8 rideType = item->RideType[i];
        if (rideType != RIDE_TYPE_NULL) {
            result = RideNaming[rideType].name;
            break;
        }
    }
    return result;
}

bool editor_check_object_group_at_least_one_selected(sint32 checkObjectType)
{
    sint32 numObjects = (sint32)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();

    for (sint32 i = 0; i < numObjects; i++) {
        uint8 objectType = items[i].ObjectEntry.flags & 0x0F;
        if (checkObjectType == objectType && (_objectSelectionFlags[i] & OBJECT_SELECTION_FLAG_SELECTED)) {
            return true;
        }
    }
    return false;
}
