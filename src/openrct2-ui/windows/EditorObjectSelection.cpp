/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2/core/String.hpp>
#include <openrct2/Editor.h>
#include <openrct2/EditorObjectSelectionSession.h>
#include <openrct2/Game.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/object/RideObject.h>
#include <openrct2/object/StexObject.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/platform/platform.h>
#include <openrct2/ride/RideGroupManager.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>

// clang-format off
enum
{
    FILTER_RCT1 = (1 << 0),
    FILTER_AA = (1 << 1),
    FILTER_LL = (1 << 2),
    FILTER_RCT2 = (1 << 3),
    FILTER_WW = (1 << 4),
    FILTER_TT = (1 << 5),
    FILTER_OO = (1 << 6),
    FILTER_CUSTOM = (1 << 7),

    FILTER_RIDE_TRANSPORT = (1 << 8),
    FILTER_RIDE_GENTLE = (1 << 9),
    FILTER_RIDE_COASTER = (1 << 10),
    FILTER_RIDE_THRILL = (1 << 11),
    FILTER_RIDE_WATER = (1 << 12),
    FILTER_RIDE_STALL = (1 << 13),

    FILTER_SELECTED = (1 << 14),
    FILTER_NONSELECTED = (1 << 15),

    FILTER_RIDES = FILTER_RIDE_TRANSPORT | FILTER_RIDE_GENTLE | FILTER_RIDE_COASTER | FILTER_RIDE_THRILL | FILTER_RIDE_WATER | FILTER_RIDE_STALL,
    FILTER_ALL = FILTER_RIDES | FILTER_RCT1 | FILTER_AA | FILTER_LL | FILTER_RCT2 | FILTER_WW | FILTER_TT | FILTER_OO | FILTER_CUSTOM | FILTER_SELECTED | FILTER_NONSELECTED,
};

static constexpr uint8_t _numSourceGameItems = 8;

static uint32_t _filter_flags;
static uint16_t _filter_object_counts[OBJECT_TYPE_COUNT];

static char _filter_string[MAX_PATH];

#define _FILTER_ALL ((_filter_flags & FILTER_ALL) == FILTER_ALL)
#define _FILTER_RCT1 (_filter_flags & FILTER_RCT1)
#define _FILTER_AA (_filter_flags & FILTER_AA)
#define _FILTER_LL (_filter_flags & FILTER_LL)
#define _FILTER_RCT2 (_filter_flags & FILTER_RCT2)
#define _FILTER_WW (_filter_flags & FILTER_WW)
#define _FILTER_TT (_filter_flags & FILTER_TT)
#define _FILTER_OO (_filter_flags & FILTER_OO)
#define _FILTER_CUSTOM (_filter_flags & FILTER_CUSTOM)
#define _FILTER_SELECTED (_filter_flags & FILTER_SELECTED)
#define _FILTER_NONSELECTED (_filter_flags & FILTER_NONSELECTED)

struct ObjectPageDesc
{
    rct_string_id Caption;
    uint32_t Image;
    bool IsAdvanced;
};

static constexpr const ObjectPageDesc ObjectSelectionPages[OBJECT_TYPE_COUNT] = {
    { STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS,   SPR_TAB_RIDE_16,            false },
    { STR_OBJECT_SELECTION_SMALL_SCENERY,               SPR_TAB_SCENERY_TREES,      true  },
    { STR_OBJECT_SELECTION_LARGE_SCENERY,               SPR_TAB_SCENERY_URBAN,      true  },
    { STR_OBJECT_SELECTION_WALLS_FENCES,                SPR_TAB_SCENERY_WALLS,      true  },
    { STR_OBJECT_SELECTION_PATH_SIGNS,                  SPR_TAB_SCENERY_SIGNAGE,    true  },
    { STR_OBJECT_SELECTION_FOOTPATHS,                   SPR_TAB_SCENERY_PATHS,      false },
    { STR_OBJECT_SELECTION_PATH_EXTRAS,                 SPR_TAB_SCENERY_PATH_ITEMS, false },
    { STR_OBJECT_SELECTION_SCENERY_GROUPS,              SPR_TAB_SCENERY_STATUES,    false },
    { STR_OBJECT_SELECTION_PARK_ENTRANCE,               SPR_TAB_PARK,               false },
    { STR_OBJECT_SELECTION_WATER,                       SPR_TAB_WATER,              false },
    { STR_OBJECT_SELECTION_SCENARIO_DESCRIPTION,        SPR_TAB_STATS,              false },
};

#pragma region Widgets

enum WINDOW_STAFF_LIST_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_TAB_CONTENT_PANEL,
    WIDX_ADVANCED,
    WIDX_LIST,
    WIDX_PREVIEW,
    WIDX_INSTALL_TRACK,
    WIDX_FILTER_DROPDOWN,
    WIDX_FILTER_STRING_BUTTON,
    WIDX_FILTER_CLEAR_BUTTON,
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
    WIDX_TAB_1,
};

validate_global_widx(WC_EDITOR_OBJECT_SELECTION, WIDX_TAB_1);

static bool _window_editor_object_selection_widgets_initialised;
static std::vector<rct_widget> _window_editor_object_selection_widgets = {
    { WWT_FRAME,            0,  0,      599,    0,      399,    0xFFFFFFFF,                     STR_NONE },
    { WWT_CAPTION,          0,  1,      598,    1,      14,     STR_OBJECT_SELECTION,           STR_WINDOW_TITLE_TIP },
    { WWT_CLOSEBOX,         0,  587,    597,    2,      13,     STR_CLOSE_X,                    STR_CLOSE_WINDOW_TIP },
    { WWT_RESIZE,           1,  0,      599,    43,     399,    0xFFFFFFFF,                     STR_NONE },
    { WWT_BUTTON,           0,  470,    591,    23,     34,     STR_OBJECT_SELECTION_ADVANCED,  STR_OBJECT_SELECTION_ADVANCED_TIP },
    { WWT_SCROLL,           1,  4,      291,    60,     386,    SCROLL_VERTICAL,                STR_NONE },
    { WWT_FLATBTN,          1,  391,    504,    46,     159,    0xFFFFFFFF,                     STR_NONE },
    { WWT_BUTTON,           0,  470,    591,    23,     34,     STR_INSTALL_NEW_TRACK_DESIGN,   STR_INSTALL_NEW_TRACK_DESIGN_TIP },
    { WWT_BUTTON,           0,  350,    463,    23,     34,     STR_OBJECT_FILTER,              STR_OBJECT_FILTER_TIP },
    { WWT_TEXT_BOX,         1,  4,      214,    46,     57,     STR_NONE,                       STR_NONE },
    { WWT_BUTTON,           1,  218,    287,    46,     57,     STR_OBJECT_SEARCH_CLEAR,        STR_NONE },
    { WWT_IMGBTN,           1,  3,      287,    73,     76,     0xFFFFFFFF,                     STR_NONE },
    { WWT_TAB,              1,  3,      33,     47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_OBJECT_FILTER_ALL_RIDES_TIP },
    { WWT_TAB,              1,  34,     64,     47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_TRANSPORT_RIDES_TIP },
    { WWT_TAB,              1,  65,     95,     47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_GENTLE_RIDES_TIP },
    { WWT_TAB,              1,  96,     126,    47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_ROLLER_COASTERS_TIP },
    { WWT_TAB,              1,  127,    157,    47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_THRILL_RIDES_TIP },
    { WWT_TAB,              1,  158,    188,    47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_WATER_RIDES_TIP },
    { WWT_TAB,              1,  189,    219,    47,     73,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_SHOPS_STALLS_TIP },
    { WWT_TABLE_HEADER,     1,  4,      148,    80,     93,     STR_NONE,                       STR_NONE },
    { WWT_TABLE_HEADER,     1,  149,    291,    80,     93,     STR_NONE,                       STR_NONE },

    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_STRING_DEFINED_TOOLTIP },
    // Copied object type times...

    { WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_editor_object_selection_close(rct_window *w);
static void window_editor_object_selection_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_editor_object_selection_resize(rct_window *w);
static void window_editor_object_selection_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_editor_object_selection_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_editor_object_selection_update(rct_window *w);
static void window_editor_object_selection_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_editor_object_selection_scroll_mousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_editor_object_selection_scroll_mouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_editor_object_selection_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_editor_object_selection_invalidate(rct_window *w);
static void window_editor_object_selection_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_editor_object_selection_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);
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
// clang-format on

#pragma endregion

static constexpr const int32_t window_editor_object_selection_animation_loops[] = { 20, 32, 10, 72, 24, 28, 16 };
static constexpr const int32_t window_editor_object_selection_animation_divisor[] = { 4, 8, 2, 4, 4, 4, 2 };

static void window_editor_object_set_page(rct_window *w, int32_t page);
static void window_editor_object_selection_set_pressed_tab(rct_window *w);
static int32_t get_object_from_object_selection(uint8_t object_type, int32_t y);
static void window_editor_object_selection_manage_tracks();
static void editor_load_selected_objects();
static bool filter_selected(uint8_t objectFlags);
static bool filter_string(const ObjectRepositoryItem * item);
static bool filter_source(const ObjectRepositoryItem * item);
static bool filter_chunks(const ObjectRepositoryItem * item);
static void filter_update_counts();

static std::string object_get_description(const void * object);
static int32_t get_selected_object_type(rct_window * w);

enum {
    RIDE_SORT_TYPE,
    RIDE_SORT_RIDE
};

enum {
    DDIX_FILTER_RCT1,
    DDIX_FILTER_AA,
    DDIX_FILTER_LL,
    DDIX_FILTER_RCT2,
    DDIX_FILTER_WW,
    DDIX_FILTER_TT,
    DDIX_FILTER_OO,
    DDIX_FILTER_CUSTOM,
    DDIX_FILTER_SEPARATOR,
    DDIX_FILTER_SELECTED,
    DDIX_FILTER_NONSELECTED,
};

struct list_item {
    const ObjectRepositoryItem * repositoryItem;
    rct_object_entry *entry;
    rct_object_filters *filter;
    uint8_t *flags;
};

static rct_string_id get_ride_type_string_id(const ObjectRepositoryItem * item);

using sortFunc_t = bool (*)(const list_item &, const list_item &);

static std::vector<list_item> _listItems;
static int32_t _listSortType = RIDE_SORT_TYPE;
static bool _listSortDescending = false;
static void * _loadedObject = nullptr;

static void visible_list_dispose()
{
    _listItems.clear();
    _listItems.shrink_to_fit();
}

static bool visible_list_sort_ride_name(const list_item &a, const list_item &b)
{
    auto nameA = a.repositoryItem->Name.c_str();
    auto nameB = b.repositoryItem->Name.c_str();
    return strcmp(nameA, nameB) < 0;
}

static bool visible_list_sort_ride_type(const list_item &a, const list_item &b)
{
    auto rideTypeA = language_get_string(get_ride_type_string_id(a.repositoryItem));
    auto rideTypeB = language_get_string(get_ride_type_string_id(b.repositoryItem));
    int32_t result = String::Compare(rideTypeA, rideTypeB);
    return result != 0 ?
        result < 0 :
        visible_list_sort_ride_name(a, b);
}

static void visible_list_refresh(rct_window *w)
{
    int32_t numObjects = (int32_t)object_repository_get_items_count();

    visible_list_dispose();
    w->selected_list_item = -1;

    const ObjectRepositoryItem *items = object_repository_get_items();
    for (int32_t i = 0; i < numObjects; i++) {
        uint8_t selectionFlags = _objectSelectionFlags[i];
        const ObjectRepositoryItem * item = &items[i];
        uint8_t objectType = item->ObjectEntry.flags & 0x0F;
        if (objectType == get_selected_object_type(w) && !(selectionFlags & OBJECT_SELECTION_FLAG_6) &&
            filter_source(item) &&
            filter_string(item) &&
            filter_chunks(item) &&
            filter_selected(selectionFlags)
        ) {
            rct_object_filters * filter = new rct_object_filters;
            filter->ride.category[0] = 0;
            filter->ride.category[1] = 0;
            filter->ride.ride_type = 0;

            list_item currentListItem;
            currentListItem.repositoryItem = item;
            currentListItem.entry = (rct_object_entry *)&item->ObjectEntry;
            currentListItem.filter = filter;
            currentListItem.flags = &_objectSelectionFlags[i];
            _listItems.push_back(std::move(currentListItem));
        }
    }

    if (_listItems.size() == 0)
    {
        visible_list_dispose();
    }
    else
    {
        sortFunc_t sortFunc = nullptr;
        switch (_listSortType)
        {
        case RIDE_SORT_TYPE:
            sortFunc = visible_list_sort_ride_type;
            break;
        case RIDE_SORT_RIDE:
            sortFunc = visible_list_sort_ride_name;
            break;
        default:
            log_warning("Wrong sort type %d, leaving list as-is.", _listSortType);
            break;
        }
        if (sortFunc != nullptr)
        {
            std::sort(_listItems.begin(), _listItems.end(), sortFunc);
            if (_listSortDescending)
            {
                std::reverse(_listItems.begin(), _listItems.end());
            }
        }
    }
    window_invalidate(w);
}

static void window_editor_object_selection_init_widgets()
{
    auto &widgets = _window_editor_object_selection_widgets;
    if (!_window_editor_object_selection_widgets_initialised)
    {
        _window_editor_object_selection_widgets_initialised = true;
        auto tabWidget = widgets[widgets.size() - 2];
        for (int32_t i = 1; i < OBJECT_TYPE_COUNT; i++)
        {
            widgets.insert(widgets.end() - 1, tabWidget);
        }
    }
}

/**
 *
 *  rct2: 0x006AA64E
 */
rct_window * window_editor_object_selection_open()
{
    window_editor_object_selection_init_widgets();

    auto window = window_bring_to_front_by_class(WC_EDITOR_OBJECT_SELECTION);
    if (window != nullptr)
        return window;

    sub_6AB211();
    reset_selected_object_count_and_size();

    window = window_create_centred(
        600,
        400,
        &window_editor_object_selection_events,
        WC_EDITOR_OBJECT_SELECTION,
        WF_10 | WF_RESIZABLE
    );
    window->widgets = _window_editor_object_selection_widgets.data();
    window->widgets[WIDX_FILTER_STRING_BUTTON].string = _filter_string;

    window->enabled_widgets =
        (1 << WIDX_ADVANCED) |
        (1 << WIDX_INSTALL_TRACK) |
        (1 << WIDX_FILTER_DROPDOWN) |
        (1 << WIDX_FILTER_STRING_BUTTON) |
        (1 << WIDX_FILTER_CLEAR_BUTTON) |
        (1 << WIDX_CLOSE) |
        (1 << WIDX_LIST_SORT_TYPE) |
        (((uint32_t)1) << WIDX_LIST_SORT_RIDE);

    _filter_flags = gConfigInterface.object_selection_filter_flags;
    memset(_filter_string, 0, sizeof(_filter_string));

    for (int32_t i = WIDX_TAB_1; i < WIDX_TAB_1 + OBJECT_TYPE_COUNT; i++)
    {
        window->enabled_widgets |= (1LL << i);
    }
    window_init_scroll_widgets(window);

    window->var_4AE = 0;
    window->selected_tab = 0;
    window->selected_list_item = -1;
    window->object_entry = nullptr;
    window->min_width = 600;
    window->min_height = 400;
    window->max_width = 1200;
    window->max_height = 1000;

    visible_list_refresh(window);

    return window;
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

    auto intent = Intent(INTENT_ACTION_REFRESH_NEW_RIDES);
    context_broadcast_intent(&intent);

    visible_list_dispose();

    intent = Intent(INTENT_ACTION_REFRESH_SCENERY);
    context_broadcast_intent(&intent);
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
    case WIDX_FILTER_RIDE_TAB_ALL:
        _filter_flags |= FILTER_RIDES;
        gConfigInterface.object_selection_filter_flags = _filter_flags;
        config_save_default();

        filter_update_counts();
        visible_list_refresh(w);

        w->selected_list_item = -1;
        w->object_entry = nullptr;
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
        _filter_flags |= (1 << (widgetIndex - WIDX_FILTER_RIDE_TAB_TRANSPORT + _numSourceGameItems));
        gConfigInterface.object_selection_filter_flags = _filter_flags;
        config_save_default();

        filter_update_counts();
        visible_list_refresh(w);

        w->selected_list_item = -1;
        w->object_entry = nullptr;
        w->scrolls[0].v_top = 0;
        w->frame_no = 0;
        window_invalidate(w);
        break;

    case WIDX_ADVANCED:
        w->list_information_type ^= 1;
        window_invalidate(w);
        break;

    case WIDX_INSTALL_TRACK:
    {
        if (w->selected_list_item != -1)
        {
            w->selected_list_item = -1;
        }
        window_invalidate(w);

        auto intent = Intent(WC_LOADSAVE);
        intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_TRACK);
        context_open_intent(&intent);
        break;
    }
    case WIDX_FILTER_STRING_BUTTON:
        window_start_textbox(w, widgetIndex, STR_STRING, _filter_string, sizeof(_filter_string));
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
    default:
        if (widgetIndex >= WIDX_TAB_1 && widgetIndex < WIDX_TAB_1 + OBJECT_TYPE_COUNT)
        {
            window_editor_object_set_page(w, widgetIndex - WIDX_TAB_1);
        }
        break;
    }
}

static void window_editor_object_selection_resize(rct_window *w)
{
    window_set_resize(w, 600, 400, 1200, 1000);
}

void window_editor_object_selection_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    int32_t numSelectionItems = 0;

    switch (widgetIndex) {
    case WIDX_FILTER_DROPDOWN:

        gDropdownItemsFormat[DDIX_FILTER_RCT1] = STR_TOGGLE_OPTION;
        gDropdownItemsFormat[DDIX_FILTER_AA] = STR_TOGGLE_OPTION;
        gDropdownItemsFormat[DDIX_FILTER_LL] = STR_TOGGLE_OPTION;
        gDropdownItemsFormat[DDIX_FILTER_RCT2] = STR_TOGGLE_OPTION;
        gDropdownItemsFormat[DDIX_FILTER_WW] = STR_TOGGLE_OPTION;
        gDropdownItemsFormat[DDIX_FILTER_TT] = STR_TOGGLE_OPTION;
        gDropdownItemsFormat[DDIX_FILTER_OO] = STR_TOGGLE_OPTION;
        gDropdownItemsFormat[DDIX_FILTER_CUSTOM] = STR_TOGGLE_OPTION;

        gDropdownItemsArgs[DDIX_FILTER_RCT1] = STR_SCENARIO_CATEGORY_RCT1;
        gDropdownItemsArgs[DDIX_FILTER_AA] = STR_SCENARIO_CATEGORY_RCT1_AA;
        gDropdownItemsArgs[DDIX_FILTER_LL] = STR_SCENARIO_CATEGORY_RCT1_LL;
        gDropdownItemsArgs[DDIX_FILTER_RCT2] = STR_ROLLERCOASTER_TYCOON_2_DROPDOWN;
        gDropdownItemsArgs[DDIX_FILTER_WW] = STR_OBJECT_FILTER_WW;
        gDropdownItemsArgs[DDIX_FILTER_TT] = STR_OBJECT_FILTER_TT;
        gDropdownItemsArgs[DDIX_FILTER_OO] = STR_OBJECT_FILTER_OPENRCT2_OFFICIAL;
        gDropdownItemsArgs[DDIX_FILTER_CUSTOM] = STR_OBJECT_FILTER_CUSTOM;

        // Track manager cannot select multiple, so only show selection filters if not in track manager
        if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            numSelectionItems = 3;
            gDropdownItemsFormat[DDIX_FILTER_SEPARATOR] = 0;
            gDropdownItemsFormat[DDIX_FILTER_SELECTED] = STR_TOGGLE_OPTION;
            gDropdownItemsFormat[DDIX_FILTER_NONSELECTED] = STR_TOGGLE_OPTION;
            gDropdownItemsArgs[DDIX_FILTER_SEPARATOR] = STR_NONE;
            gDropdownItemsArgs[DDIX_FILTER_SELECTED] = STR_SELECTED_ONLY;
            gDropdownItemsArgs[DDIX_FILTER_NONSELECTED] = STR_NON_SELECTED_ONLY;
        }

        window_dropdown_show_text(
            w->x + widget->left,
            w->y + widget->top,
            widget->bottom - widget->top + 1,
            w->colours[widget->colour],
            DROPDOWN_FLAG_STAY_OPEN,
            _numSourceGameItems + numSelectionItems
            );

        for (int32_t i = 0; i < _numSourceGameItems; i++)
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

static void window_editor_object_selection_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
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
static void window_editor_object_selection_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height)
{
    *height = (int32_t)(_listItems.size() * 12);
}

/**
 *
 *  rct2: 0x006AB0B6
 */
static void window_editor_object_selection_scroll_mousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    // Used for in-game object selection cheat to prevent crashing the game
    // when windows attempt to draw objects that don't exist any more
    window_close_all_except_class(WC_EDITOR_OBJECT_SELECTION);

    int32_t selected_object = get_object_from_object_selection(get_selected_object_type(w), y);
    if (selected_object == -1)
        return;

    list_item * listItem = &_listItems[selected_object];
    uint8_t object_selection_flags = *listItem->flags;
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

    int32_t ebx = 6;
    // If already selected
    if (!(object_selection_flags & OBJECT_SELECTION_FLAG_SELECTED))
        ebx = 7;

    _maxObjectsWasHit = false;
    if (!window_editor_object_selection_select_object(0, ebx, listItem->entry)) {
        rct_string_id error_title = (ebx & 1) ?
            STR_UNABLE_TO_SELECT_THIS_OBJECT :
            STR_UNABLE_TO_DE_SELECT_THIS_OBJECT;

        context_show_error(error_title, gGameCommandErrorText);
        return;
    }

    if (_FILTER_SELECTED || _FILTER_NONSELECTED) {
        filter_update_counts();
        visible_list_refresh(w);
        window_invalidate(w);
    }

    if (_maxObjectsWasHit) {
        context_show_error(STR_WARNING_TOO_MANY_OBJECTS_SELECTED, STR_NOT_ALL_OBJECTS_IN_THIS_SCENERY_GROUP_COULD_BE_SELECTED);
    }
}

/**
 *
 *  rct2: 0x006AB079
 */
static void window_editor_object_selection_scroll_mouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    int32_t selectedObject = get_object_from_object_selection(get_selected_object_type(w), y);
    if (selectedObject != -1) {
        list_item * listItem = &_listItems[selectedObject];
        uint8_t objectSelectionFlags = *listItem->flags;
        if (objectSelectionFlags & OBJECT_SELECTION_FLAG_6) {
            selectedObject = -1;
        }
    }
    if (selectedObject != w->selected_list_item) {
        w->selected_list_item = selectedObject;

        object_delete(_loadedObject);
        _loadedObject = nullptr;

        if (selectedObject == -1) {
            w->object_entry = nullptr;
        } else {
            auto listItem = &_listItems[selectedObject];
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
    if (widgetIndex >= WIDX_TAB_1 && widgetIndex < WIDX_TAB_1 + OBJECT_TYPE_COUNT)
    {
        set_format_arg(0, rct_string_id, ObjectSelectionPages[(widgetIndex - WIDX_TAB_1)].Caption);
    }
    else
    {
        set_format_arg(0, rct_string_id, STR_LIST);
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
    set_format_arg(0, rct_string_id, ObjectSelectionPages[get_selected_object_type(w)].Caption);
    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
        w->widgets[WIDX_TITLE].text = STR_TRACK_DESIGNS_MANAGER_SELECT_RIDE_TYPE;
        w->widgets[WIDX_INSTALL_TRACK].type = WWT_BUTTON;
    } else if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) {
        w->widgets[WIDX_TITLE].text = STR_ROLLER_COASTER_DESIGNER_SELECT_RIDE_TYPES_VEHICLES;
        w->widgets[WIDX_INSTALL_TRACK].type = WWT_EMPTY;
    } else {
        w->widgets[WIDX_TITLE].text = STR_OBJECT_SELECTION;
        w->widgets[WIDX_INSTALL_TRACK].type = WWT_EMPTY;
    }

    // Align tabs, hide advanced ones
    bool advancedMode = (w->list_information_type & 1) != 0;
    int32_t x = 3;
    for (int32_t i = 0; i < OBJECT_TYPE_COUNT; i++)
    {
        auto widget = &w->widgets[WIDX_TAB_1 + i];
        if ((!advancedMode && ObjectSelectionPages[i].IsAdvanced) ||
            i == OBJECT_TYPE_SCENARIO_TEXT)
        {
            widget->type = WWT_EMPTY;
        }
        else
        {
            widget->type = WWT_TAB;
            widget->left = x;
            widget->right = x + 30;
            x += 31;
        }
    }

    if (gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TRACK_DESIGNER)) {
        w->widgets[WIDX_ADVANCED].type = WWT_EMPTY;
        for (int32_t i = 1; i < OBJECT_TYPE_COUNT; i++)
        {
            w->widgets[WIDX_TAB_1 + i].type = WWT_EMPTY;
        }
        x = 150;
    } else {
        w->widgets[WIDX_ADVANCED].type = WWT_BUTTON;
        x = 300;
    }

    w->widgets[WIDX_FILTER_DROPDOWN].type = WWT_BUTTON;
    w->widgets[WIDX_LIST].right = w->width - (600 - 587) - x;
    w->widgets[WIDX_PREVIEW].left = w->width - (600 - 537) - (x / 2);
    w->widgets[WIDX_PREVIEW].right = w->widgets[WIDX_PREVIEW].left + 113;
    w->widgets[WIDX_FILTER_RIDE_TAB_FRAME].right = w->widgets[WIDX_LIST].right;

    bool ridePage = (get_selected_object_type(w) == OBJECT_TYPE_RIDE);
    w->widgets[WIDX_LIST].top = (ridePage ? 118 : 60);
    w->widgets[WIDX_FILTER_STRING_BUTTON].right = w->widgets[WIDX_LIST].right - 77;
    w->widgets[WIDX_FILTER_STRING_BUTTON].top = (ridePage ? 80 : 46);
    w->widgets[WIDX_FILTER_STRING_BUTTON].bottom = (ridePage ? 91 : 57);
    w->widgets[WIDX_FILTER_CLEAR_BUTTON].left = w->widgets[WIDX_LIST].right - 73;
    w->widgets[WIDX_FILTER_CLEAR_BUTTON].right = w->widgets[WIDX_LIST].right;
    w->widgets[WIDX_FILTER_CLEAR_BUTTON].top = (ridePage ? 80 : 46);
    w->widgets[WIDX_FILTER_CLEAR_BUTTON].bottom = (ridePage ? 91 : 57);

    if (ridePage)
    {
        w->enabled_widgets |= (1 << WIDX_FILTER_RIDE_TAB_ALL) | (1 << WIDX_FILTER_RIDE_TAB_TRANSPORT) |
            (1 << WIDX_FILTER_RIDE_TAB_GENTLE) | (1 << WIDX_FILTER_RIDE_TAB_COASTER) | (1 << WIDX_FILTER_RIDE_TAB_THRILL) |
            (1 << WIDX_FILTER_RIDE_TAB_WATER) | (1 << WIDX_FILTER_RIDE_TAB_STALL);

        for (int32_t i = 0; i < 7; i++)
            w->pressed_widgets &= ~(1 << (WIDX_FILTER_RIDE_TAB_ALL + i));

        if ((_filter_flags & FILTER_RIDES) == FILTER_RIDES)
            w->pressed_widgets |= (1 << WIDX_FILTER_RIDE_TAB_ALL);
        else
        {
            for (int32_t i = 0; i < 6; i++)
            {
                if (_filter_flags & (1 << (_numSourceGameItems + i)))
                    w->pressed_widgets |= (uint64_t)(1ULL << (WIDX_FILTER_RIDE_TAB_TRANSPORT + i));
            }
        }

        w->widgets[WIDX_FILTER_RIDE_TAB_FRAME].type = WWT_IMGBTN;
        for (int32_t i = WIDX_FILTER_RIDE_TAB_ALL; i <= WIDX_FILTER_RIDE_TAB_STALL; i++)
            w->widgets[i].type = WWT_TAB;

        int32_t width_limit = (w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left - 15) / 2;

        w->widgets[WIDX_LIST_SORT_TYPE].type = WWT_TABLE_HEADER;
        w->widgets[WIDX_LIST_SORT_TYPE].top = w->widgets[WIDX_FILTER_STRING_BUTTON].bottom + 3;
        w->widgets[WIDX_LIST_SORT_TYPE].bottom = w->widgets[WIDX_LIST_SORT_TYPE].top + 13;
        w->widgets[WIDX_LIST_SORT_TYPE].left = 4;
        w->widgets[WIDX_LIST_SORT_TYPE].right = w->widgets[WIDX_LIST_SORT_TYPE].left + width_limit;

        w->widgets[WIDX_LIST_SORT_RIDE].type = WWT_TABLE_HEADER;
        w->widgets[WIDX_LIST_SORT_RIDE].top = w->widgets[WIDX_LIST_SORT_TYPE].top;
        w->widgets[WIDX_LIST_SORT_RIDE].bottom = w->widgets[WIDX_LIST_SORT_TYPE].bottom;
        w->widgets[WIDX_LIST_SORT_RIDE].left = w->widgets[WIDX_LIST_SORT_TYPE].right + 1;
        w->widgets[WIDX_LIST_SORT_RIDE].right = w->widgets[WIDX_LIST].right;

        w->widgets[WIDX_LIST].top = w->widgets[WIDX_LIST_SORT_TYPE].bottom + 2;
    }
    else
    {
        w->enabled_widgets &= ~((1 << WIDX_FILTER_RIDE_TAB_ALL) | (1 << WIDX_FILTER_RIDE_TAB_TRANSPORT) |
            (1 << WIDX_FILTER_RIDE_TAB_GENTLE) | (1 << WIDX_FILTER_RIDE_TAB_COASTER) | (1 << WIDX_FILTER_RIDE_TAB_THRILL) |
            (1 << WIDX_FILTER_RIDE_TAB_WATER) | (1 << WIDX_FILTER_RIDE_TAB_STALL));
        for (int32_t i = WIDX_FILTER_RIDE_TAB_FRAME; i <= WIDX_FILTER_RIDE_TAB_STALL; i++)
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
    int32_t i, x, y, width;
    rct_widget *widget;
    rct_object_entry *highlightedEntry;
    rct_string_id stringId;

    window_draw_widgets(w, dpi);

    // Draw tabs
    for (i = 0; i < OBJECT_TYPE_COUNT; i++)
    {
        widget = &w->widgets[WIDX_TAB_1 + i];
        if (widget->type != WWT_EMPTY)
        {
            auto image = ObjectSelectionPages[i].Image;
            x = w->x + widget->left;
            y = w->y + widget->top;
            gfx_draw_sprite(dpi, image, x, y, 0);
        }
    }

    const int32_t ride_tabs[] = { SPR_TAB_RIDE_16, IMAGE_TYPE_REMAP | SPR_TAB_RIDES_TRANSPORT_0, SPR_TAB_RIDES_GENTLE_0, IMAGE_TYPE_REMAP | SPR_TAB_RIDES_ROLLER_COASTERS_0, SPR_TAB_RIDES_THRILL_0, SPR_TAB_RIDES_WATER_0, SPR_TAB_RIDES_SHOP_0, SPR_TAB_FINANCES_RESEARCH_0 };
    const int32_t ThrillRidesTabAnimationSequence[] = {
        5, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0
    };

    // Draw ride tabs
    if (get_selected_object_type(w) == OBJECT_TYPE_RIDE)
    {
        for (i = 0; i < 7; i++) {
            widget = &w->widgets[WIDX_FILTER_RIDE_TAB_ALL + i];
            if (widget->type == WWT_EMPTY)
                continue;

            int32_t spriteIndex = ride_tabs[i];
            int32_t frame = 0;
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

        int32_t numSelected = _numSelectedObjectsForType[get_selected_object_type(w)];
        int32_t totalSelectable = object_entry_group_counts[get_selected_object_type(w)];
        if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
            totalSelectable = 4;

        set_format_arg(0, uint16_t, numSelected);
        set_format_arg(2, uint16_t, totalSelectable);
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

    // Draw preview
    widget = &w->widgets[WIDX_PREVIEW];
    {
        rct_drawpixelinfo clipDPI;
        x = w->x + widget->left + 1;
        y = w->y + widget->top + 1;
        width = widget->right - widget->left - 1;
        int32_t height = widget->bottom - widget->top - 1;
        if (clip_drawpixelinfo(&clipDPI, dpi, x, y, width, height)) {
            object_draw_preview(_loadedObject, &clipDPI, width, height);
        }
    }

    // Draw name of object
    x = w->x + (widget->left + widget->right) / 2 + 1;
    y = w->y + widget->bottom + 3;
    width = w->width - w->widgets[WIDX_LIST].right - 6;
    set_format_arg(0, rct_string_id, STR_STRING);
    set_format_arg(2, const char *, listItem->repositoryItem->Name.c_str());
    gfx_draw_string_centred_clipped(dpi, STR_WINDOW_COLOUR_2_STRINGID, gCommonFormatArgs, COLOUR_BLACK, x, y, width);

    // Draw description of object
    auto description = object_get_description(_loadedObject);
    if (!description.empty()) {
        set_format_arg(0, rct_string_id, STR_STRING);
        set_format_arg(2, const char *, description.c_str());

        x = w->x + w->widgets[WIDX_LIST].right + 4;
        y += 15;
        width = w->x + w->width - x - 4;

        gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, x, y + 5, width, STR_WINDOW_COLOUR_2_STRINGID, COLOUR_BLACK);
    }

    y = w->y + w->height - (12 * 4);

    // Draw ride type.
    if (get_selected_object_type(w) == OBJECT_TYPE_RIDE)
    {
        stringId = get_ride_type_string_id(listItem->repositoryItem);
        gfx_draw_string_right(dpi, stringId, nullptr, COLOUR_WHITE, w->x + w->width - 5, y);
    }

    y += 12;

    // Draw object source
    stringId = object_manager_get_source_game_string(highlightedEntry);
    gfx_draw_string_right(dpi, stringId, nullptr, COLOUR_WHITE, w->x + w->width - 5, y);
    y += 12;

    // Draw object dat name
    const char *path = path_get_filename(listItem->repositoryItem->Path.c_str());
    set_format_arg(0, rct_string_id, STR_STRING);
    set_format_arg(2, const char *, path);
    gfx_draw_string_right(dpi, STR_WINDOW_COLOUR_2_STRINGID, gCommonFormatArgs, COLOUR_BLACK, w->x + w->width - 5, y);
}

/**
 *
 *  rct2: 0x006AADA3
 */
static void window_editor_object_selection_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex)
{
    int32_t x, y, colour, colour2;

    bool ridePage = (get_selected_object_type(w) == OBJECT_TYPE_RIDE);

    uint8_t paletteIndex = ColourMapA[w->colours[1]].mid_light;
    gfx_clear(dpi, paletteIndex);

    y = 0;
    for (const auto &listItem : _listItems)
    {
        if (y + 12 >= dpi->y && y <= dpi->y + dpi->height) {
            // Draw checkbox
            if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && !(*listItem.flags & 0x20))
                gfx_fill_rect_inset(dpi, 2, y, 11, y + 10, w->colours[1], INSET_RECT_F_E0);

            // Highlight background
            colour = COLOUR_BRIGHT_GREEN | COLOUR_FLAG_TRANSLUCENT;
            if (listItem.entry == w->object_entry && !(*listItem.flags & OBJECT_SELECTION_FLAG_6)) {
                gfx_filter_rect(dpi, 0, y, w->width, y + 11, PALETTE_DARKEN_1);
                colour = COLOUR_BRIGHT_GREEN;
            }

            // Draw checkmark
            if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && (*listItem.flags & OBJECT_SELECTION_FLAG_SELECTED)) {
                x = 2;
                gCurrentFontSpriteBase = colour == COLOUR_BRIGHT_GREEN ? FONT_SPRITE_BASE_MEDIUM_EXTRA_DARK : FONT_SPRITE_BASE_MEDIUM_DARK;
                colour2 = NOT_TRANSLUCENT(w->colours[1]);
                if (*listItem.flags & (OBJECT_SELECTION_FLAG_IN_USE | OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED))
                    colour2 |= COLOUR_FLAG_INSET;

                gfx_draw_string(dpi, (char*)CheckBoxMarkString, colour2, x, y);
            }

            x = gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER ? 0 : 15;

            char *bufferWithColour = gCommonStringFormatBuffer;
            char *buffer = utf8_write_codepoint(bufferWithColour, colour);
            if (*listItem.flags & OBJECT_SELECTION_FLAG_6) {
                colour = w->colours[1] & 0x7F;
                gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM_DARK;
            }
            else {
                colour = COLOUR_BLACK;
                gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
            }

            int32_t width_limit = (w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left - x) / 2;

            if (ridePage) {
                // Draw ride type
                rct_string_id rideTypeStringId = get_ride_type_string_id(listItem.repositoryItem);
                safe_strcpy(buffer, language_get_string(rideTypeStringId), 256 - (buffer - bufferWithColour));
                gfx_draw_string_left_clipped(dpi, STR_STRING, &bufferWithColour, colour, x, y, width_limit - 15);
                x = w->widgets[WIDX_LIST_SORT_RIDE].left - w->widgets[WIDX_LIST].left;
            }

            // Draw text
            safe_strcpy(buffer, listItem.repositoryItem->Name.c_str(), 256 - (buffer - bufferWithColour));
            if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
                while (*buffer != 0 && *buffer != 9)
                    buffer++;

                *buffer = 0;
            }
            gfx_draw_string_left_clipped(dpi, STR_STRING, &bufferWithColour, colour, x, y, width_limit);
        }
        y += 12;
    }
}

static void window_editor_object_set_page(rct_window *w, int32_t page)
{
    if (w->selected_tab == page)
        return;

    w->selected_tab = page;
    w->selected_list_item = -1;
    w->object_entry = nullptr;
    w->scrolls[0].v_top = 0;
    w->frame_no = 0;

    if (page == OBJECT_TYPE_RIDE) {
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
    for (int32_t i = 0; i < OBJECT_TYPE_COUNT; i++)
    {
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    }
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->selected_tab);
}

/**
 * Takes the y coordinate of the clicked on scroll list
 * and converts this into an object selection.
 * Returns the position in the list.
 * Object_selection_flags, installed_entry also populated
 *
 *  rct2: 0x006AA703
 */
static int32_t get_object_from_object_selection(uint8_t object_type, int32_t y)
{
    int32_t listItemIndex = y / 12;
    if (listItemIndex < 0 || (size_t)listItemIndex >= _listItems.size())
        return -1;

    return listItemIndex;
}

/**
 *
 *  rct2: 0x006D33E2
 */
static void window_editor_object_selection_manage_tracks()
{
    set_every_ride_type_invented();
    set_every_ride_entry_invented();

    gS6Info.editor_step = EDITOR_STEP_TRACK_DESIGNS_MANAGER;

    int32_t entry_index = 0;
    for (; object_entry_get_chunk(OBJECT_TYPE_RIDE, entry_index) == nullptr; entry_index++);

    rct_ride_entry* ride_entry = get_ride_entry(entry_index);
    uint8_t ride_type = ride_entry_get_first_non_null_ride_type(ride_entry);

    auto intent = Intent(WC_TRACK_DESIGN_LIST);
    intent.putExtra(INTENT_EXTRA_RIDE_TYPE, ride_type);
    intent.putExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX, entry_index);
    context_open_intent(&intent);
}

/**
 *
 *  rct2: 0x006ABBBE
 */
static void editor_load_selected_objects()
{
    int32_t numItems = (int32_t)object_repository_get_items_count();
    const ObjectRepositoryItem * items = object_repository_get_items();
    for (int32_t i = 0; i < numItems; i++) {
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
                    uint8_t objectType = object_entry_get_type(entry);
                    uint8_t entryIndex = object_manager_get_loaded_object_entry_index(loadedObject);
                    if (objectType == OBJECT_TYPE_RIDE) {
                        rct_ride_entry *rideEntry = get_ride_entry(entryIndex);
                        uint8_t rideType = ride_entry_get_first_non_null_ride_type(rideEntry);
                        research_insert(1, RESEARCH_ENTRY_RIDE_MASK | (rideType << 8) | entryIndex, rideEntry->category[0]);
                    }
                    else if (objectType == OBJECT_TYPE_SCENERY_GROUP) {
                        research_insert(1, entryIndex, RESEARCH_CATEGORY_SCENERY_GROUP);
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

    safe_strcpy(_filter_string, text, sizeof(_filter_string));

    filter_update_counts();

    w->scrolls->v_top = 0;

    visible_list_refresh(w);
    window_invalidate(w);
}

static bool filter_selected(uint8_t objectFlag)
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
    if (item->Name.empty())
        return false;

    // Get ride type
    const char *rideTypeName = language_get_string(get_ride_type_string_id(item));

    // Get object name (ride/vehicle for rides) and type name (rides only)
    char name_lower[MAX_PATH];
    char type_lower[MAX_PATH];
    char object_path[MAX_PATH];
    char filter_lower[sizeof(_filter_string)];
    safe_strcpy(name_lower, item->Name.c_str(), MAX_PATH);
    safe_strcpy(type_lower, rideTypeName, MAX_PATH);
    safe_strcpy(object_path, item->Path.c_str(), MAX_PATH);
    safe_strcpy(filter_lower, _filter_string, sizeof(_filter_string));

    // Make use of lowercase characters only
    for (int32_t i = 0; name_lower[i] != '\0'; i++)
        name_lower[i] = (char)tolower(name_lower[i]);
    for (int32_t i = 0; type_lower[i] != '\0'; i++)
        type_lower[i] = (char)tolower(type_lower[i]);
    for (int32_t i = 0; object_path[i] != '\0'; i++)
        object_path[i] = (char)tolower(object_path[i]);
    for (int32_t i = 0; filter_lower[i] != '\0'; i++)
        filter_lower[i] = (char)tolower(filter_lower[i]);

    // Check if the searched string exists in the name, ride type, or filename
    bool inName = strstr(name_lower, filter_lower) != nullptr;
    bool inRideType = ((item->ObjectEntry.flags & 0x0F) == OBJECT_TYPE_RIDE) && strstr(type_lower, filter_lower) != nullptr;
    bool inPath = strstr(object_path, filter_lower) != nullptr;

    return inName || inRideType || inPath;
}

static bool filter_source(const ObjectRepositoryItem * item)
{
    if (_FILTER_ALL)
        return true;

    uint8_t source = object_entry_get_source_game(&item->ObjectEntry);
    return (_FILTER_RCT1 && source == OBJECT_SOURCE_RCT1) ||
           (_FILTER_AA && source == OBJECT_SOURCE_ADDED_ATTRACTIONS) ||
           (_FILTER_LL && source == OBJECT_SOURCE_LOOPY_LANDSCAPES) ||
           (_FILTER_RCT2 && source == OBJECT_SOURCE_RCT2) ||
           (_FILTER_WW && source == OBJECT_SOURCE_WACKY_WORLDS) ||
           (_FILTER_TT && source == OBJECT_SOURCE_TIME_TWISTER) ||
           (_FILTER_OO && source == OBJECT_SOURCE_OPENRCT2_OFFICIAL) ||
           (_FILTER_CUSTOM &&
               source != OBJECT_SOURCE_RCT1 &&
               source != OBJECT_SOURCE_ADDED_ATTRACTIONS &&
               source != OBJECT_SOURCE_LOOPY_LANDSCAPES &&
               source != OBJECT_SOURCE_RCT2 &&
               source != OBJECT_SOURCE_WACKY_WORLDS &&
               source != OBJECT_SOURCE_TIME_TWISTER &&
               source != OBJECT_SOURCE_OPENRCT2_OFFICIAL);
}

static bool filter_chunks(const ObjectRepositoryItem * item)
{
    switch (item->ObjectEntry.flags & 0x0F) {
    case OBJECT_TYPE_RIDE:

        uint8_t rideType = 0;
        for (int32_t i = 0; i < MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++)
        {
            if (item->RideInfo.RideType[i] != RIDE_TYPE_NULL)
            {
                rideType = item->RideInfo.RideType[i];
                break;
            }
        }
        if (_filter_flags & (1 << (gRideCategories[rideType] + _numSourceGameItems)))
            return true;

        return false;
    }
    return true;
}

static void filter_update_counts()
{
    if (!_FILTER_ALL || strlen(_filter_string) > 0) {
        const auto &selectionFlags = _objectSelectionFlags;
        for (int32_t i = 0; i < 11; i++) {
            _filter_object_counts[i] = 0;
        }

        size_t numObjects = object_repository_get_items_count();
        const ObjectRepositoryItem * items = object_repository_get_items();
        for (size_t i = 0; i < numObjects; i++) {
            const ObjectRepositoryItem * item = &items[i];
            if (filter_source(item) &&
                filter_string(item) &&
                filter_chunks(item) &&
                filter_selected(selectionFlags[i])
            ) {
                uint8_t objectType = item->ObjectEntry.flags & 0xF;
                _filter_object_counts[objectType]++;
            }
        }
    }
}

static rct_string_id get_ride_type_string_id(const ObjectRepositoryItem * item)
{
    rct_string_id result = STR_NONE;
    for (int32_t i = 0; i < MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++)
    {
        uint8_t rideType = item->RideInfo.RideType[i];
        if (rideType != RIDE_TYPE_NULL)
        {
            if (RideGroupManager::RideTypeHasRideGroups(rideType))
            {
                const RideGroup * rideGroup = RideGroupManager::RideGroupFind(rideType, item->RideInfo.RideGroupIndex);
                result = rideGroup->Naming.name;
            }
            else
            {
                result = RideNaming[rideType].name;
            }

            break;
        }
    }
    return result;
}

static std::string object_get_description(const void * object)
{
    const Object * baseObject = static_cast<const Object *>(object);
    switch (baseObject->GetObjectType()) {
    case OBJECT_TYPE_RIDE:
    {
        const RideObject * rideObject = static_cast<const RideObject *>(baseObject);
        return rideObject->GetDescription();
    }
    default:
        return "";
    }
}

static int32_t get_selected_object_type(rct_window * w)
{
    return w->selected_tab;
}
