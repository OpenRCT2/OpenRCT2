/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/windows/Window.h>

#include <openrct2/audio/audio.h>
#include <openrct2/Context.h>
#include <openrct2/core/Math.hpp>
#include <openrct2/Input.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/sprites.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/SmallScenery.h>
#include <openrct2/world/Park.h>
#include <openrct2/management/Research.h>

#define WINDOW_SCENERY_WIDTH    634
#define WINDOW_SCENERY_HEIGHT   180
#define SCENERY_BUTTON_WIDTH    66
#define SCENERY_BUTTON_HEIGHT   80
#define SCENERY_WINDOW_TABS     (MAX_SCENERY_GROUP_OBJECTS + 1) // The + 1 is for the 'Miscellaneous' tab

// clang-format off
enum {
    WINDOW_SCENERY_TAB_1,
    WINDOW_SCENERY_TAB_2,
    WINDOW_SCENERY_TAB_3,
    WINDOW_SCENERY_TAB_4,
    WINDOW_SCENERY_TAB_5,
    WINDOW_SCENERY_TAB_6,
    WINDOW_SCENERY_TAB_7,
    WINDOW_SCENERY_TAB_8,
    WINDOW_SCENERY_TAB_9,
    WINDOW_SCENERY_TAB_10,
    WINDOW_SCENERY_TAB_11,
    WINDOW_SCENERY_TAB_12,
    WINDOW_SCENERY_TAB_13,
    WINDOW_SCENERY_TAB_14,
    WINDOW_SCENERY_TAB_15,
    WINDOW_SCENERY_TAB_16,
    WINDOW_SCENERY_TAB_17,
    WINDOW_SCENERY_TAB_18,
    WINDOW_SCENERY_TAB_19,
    WINDOW_SCENERY_TAB_20
};

static void window_scenery_close(rct_window *w);
static void window_scenery_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_scenery_resize(rct_window *w);
static void window_scenery_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_scenery_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_scenery_update(rct_window *w);
static void window_scenery_event_07(rct_window *w);
static void window_scenery_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_scenery_scrollmousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_scenery_scrollmouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_scenery_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_scenery_invalidate(rct_window *w);
static void window_scenery_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_scenery_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_scenery_events = {
    window_scenery_close,
    window_scenery_mouseup,
    window_scenery_resize,
    window_scenery_mousedown,
    window_scenery_dropdown,
    nullptr,
    window_scenery_update,
    window_scenery_event_07,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_scenery_scrollgetsize,
    window_scenery_scrollmousedown,
    nullptr,
    window_scenery_scrollmouseover,
    nullptr,
    nullptr,
    nullptr,
    window_scenery_tooltip,
    nullptr,
    nullptr,
    window_scenery_invalidate,
    window_scenery_paint,
    window_scenery_scrollpaint,
};


enum WINDOW_SCENERY_LIST_WIDGET_IDX {
    WIDX_SCENERY_BACKGROUND,                // 1
    WIDX_SCENERY_TITLE,                     // 2
    WIDX_SCENERY_CLOSE,                     // 4
    WIDX_SCENERY_TAB_CONTENT_PANEL,         // 8
    WIDX_SCENERY_TAB_1,                     // 10
    WIDX_SCENERY_TAB_2,                     // 20
    WIDX_SCENERY_TAB_3,                     // 40
    WIDX_SCENERY_TAB_4,                     // 80
    WIDX_SCENERY_TAB_5,                     // 100
    WIDX_SCENERY_TAB_6,                     // 200
    WIDX_SCENERY_TAB_7,                     // 400
    WIDX_SCENERY_TAB_8,                     // 800
    WIDX_SCENERY_TAB_9,                     // 1000
    WIDX_SCENERY_TAB_10,                    // 2000
    WIDX_SCENERY_TAB_11,                    // 4000
    WIDX_SCENERY_TAB_12,                    // 8000
    WIDX_SCENERY_TAB_13,                    // 10000
    WIDX_SCENERY_TAB_14,                    // 20000
    WIDX_SCENERY_TAB_15,                    // 40000
    WIDX_SCENERY_TAB_16,                    // 80000
    WIDX_SCENERY_TAB_17,                    // 100000
    WIDX_SCENERY_TAB_18,                    // 200000
    WIDX_SCENERY_TAB_19,                    // 400000
    WIDX_SCENERY_TAB_20,                    // 800000
    WIDX_SCENERY_LIST,                      // 1000000
    WIDX_SCENERY_ROTATE_OBJECTS_BUTTON,     // 2000000
    WIDX_SCENERY_REPAINT_SCENERY_BUTTON,    // 4000000
    WIDX_SCENERY_PRIMARY_COLOUR_BUTTON,     // 8000000
    WIDX_SCENERY_SECONDARY_COLOUR_BUTTON,   // 10000000
    WIDX_SCENERY_TERTIARY_COLOUR_BUTTON,    // 20000000
    WIDX_SCENERY_EYEDROPPER_BUTTON,         // 40000000
    WIDX_SCENERY_BUILD_CLUSTER_BUTTON,      // 80000000
};

validate_global_widx(WC_SCENERY, WIDX_SCENERY_TAB_1);
validate_global_widx(WC_SCENERY, WIDX_SCENERY_ROTATE_OBJECTS_BUTTON);

static rct_widget window_scenery_widgets[] = {
    { WWT_FRAME, 0, 0, 633, 0, 141, 0xFFFFFFFF, STR_NONE },                             // 1                0x009DE298
    { WWT_CAPTION, 0, 1, 632, 1, 14, 0xFFFFFFFF, STR_WINDOW_TITLE_TIP },                // 2                0x009DE2A8
    { WWT_CLOSEBOX, 0, 621, 631, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP },            // 4                0x009DE2B8
    { WWT_RESIZE, 1, 0, 633, 43, 141, 0xFFFFFFFF, STR_NONE },                           // 8                0x009DE2C8
    { WWT_TAB, 1, 3, 33, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                  // 10               0x009DE2D8
    { WWT_TAB, 1, 34, 64, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                 // 20               0x009DE2E8
    { WWT_TAB, 1, 65, 95, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                 // 40               0x009DE2F8
    { WWT_TAB, 1, 96, 126, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                    // 80               0x009DE308
    { WWT_TAB, 1, 127, 157, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 100              0x009DE318
    { WWT_TAB, 1, 158, 188, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 200              0x009DE328
    { WWT_TAB, 1, 189, 219, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 400              0x009DE338
    { WWT_TAB, 1, 220, 250, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 800              0x009DE348
    { WWT_TAB, 1, 251, 281, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 1000             0x009DE358
    { WWT_TAB, 1, 282, 312, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 2000             0x009DE368
    { WWT_TAB, 1, 313, 343, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 4000             0x009DE378
    { WWT_TAB, 1, 344, 374, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 8000             0x009DE388
    { WWT_TAB, 1, 375, 405, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 10000            0x009DE398
    { WWT_TAB, 1, 406, 436, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 20000            0x009DE3A8
    { WWT_TAB, 1, 437, 467, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 40000            0x009DE3B8
    { WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 80000            0x009DE3C8
    { WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 100000           0x009DE3D8
    { WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 200000           0x009DE3E8
    { WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },                                   // 400000           0x009DE3F8
    { WWT_TAB, 1, 468, 498, 17, 43, IMAGE_TYPE_REMAP | SPR_TAB_QUESTION, STR_STRING_DEFINED_TOOLTIP },                                    // 800000           0x009DE408
    { WWT_SCROLL, 1, 2, 608, 47, 126, SCROLL_VERTICAL, STR_NONE },                                  // 1000000          0x009DE418
    { WWT_FLATBTN, 1, 609, 632, 44, 67, SPR_ROTATE_ARROW, STR_ROTATE_OBJECTS_90 },                  // 2000000          0x009DE428
    { WWT_FLATBTN, 1, 609, 632, 68, 91, SPR_PAINTBRUSH, STR_SCENERY_PAINTBRUSH_TIP },                                   // 4000000          0x009DE438
    { WWT_COLOURBTN, 1, 615, 626, 93, 104, 0xFFFFFFFF, STR_SELECT_COLOUR },             // 8000000          0x009DE448
    { WWT_COLOURBTN, 1, 615, 626, 105, 116, 0xFFFFFFFF, STR_SELECT_SECONDARY_COLOUR },  // 10000000         0x009DE458
    { WWT_COLOURBTN, 1, 615, 626, 117, 128, 0xFFFFFFFF, STR_SELECT_TERNARY_COLOUR },        // 20000000         0x009DE468
    { WWT_FLATBTN, 1, 609, 632, 130, 153, SPR_G2_EYEDROPPER, STR_SCENERY_EYEDROPPER_TIP },                                 // 40000000         0x009DE478
    { WWT_FLATBTN, 1, 609, 632, 154, 177, SPR_SCENERY_CLUSTER, STR_SCENERY_CLUSTER_TIP },                                   // 40000000         0x009DE478
    { WIDGETS_END },
};
// clang-format on

void window_scenery_update_scroll(rct_window *w);

// rct2: 0x00F64F2C
static int16_t window_scenery_tab_entries[SCENERY_WINDOW_TABS][SCENERY_ENTRIES_BY_TAB + 1];

/**
 * Was part of 0x006DFA00
 * The same code repeated five times for every scenery entry type
 */
static void init_scenery_entry(rct_scenery_entry *sceneryEntry, int32_t index, uint8_t sceneryTabId)
{
    if (scenery_is_invented(index) || gCheatsIgnoreResearchStatus) {
        if (sceneryTabId != 0xFF) {
            for (int32_t i = 0; i < SCENERY_ENTRIES_BY_TAB; i++) {
                if (window_scenery_tab_entries[sceneryTabId][i] == -1)
                {
                    window_scenery_tab_entries[sceneryTabId][i] = index;
                    window_scenery_tab_entries[sceneryTabId][i + 1] = -1;
                    return;
                }
            }
        }

        for (int32_t i = 0; i < SCENERY_WINDOW_TABS - 1; i++) {
            int32_t counter = 0;

            while (window_scenery_tab_entries[i][counter] != -1)
            {
                if (window_scenery_tab_entries[i][counter] == index) {
                    return;
                }

                counter++;
            }
        }

        for (int32_t i = 0; i < SCENERY_ENTRIES_BY_TAB; i++) {
            if (window_scenery_tab_entries[SCENERY_WINDOW_TABS - 1][i] == -1)
            {
                window_scenery_tab_entries[SCENERY_WINDOW_TABS - 1][i] = index;
                window_scenery_tab_entries[SCENERY_WINDOW_TABS - 1][i + 1] = -1;
                break;
            }
        }
    }
}

/**
 *
 *  rct2: 0x006DFA00
 */
void window_scenery_init()
{
    bool enabledScenerySets[SCENERY_WINDOW_TABS] = { false };

    for (int32_t scenerySetIndex = 0; scenerySetIndex < SCENERY_WINDOW_TABS; scenerySetIndex++) {
        window_scenery_tab_entries[scenerySetIndex][0] = -1;
        if (scenerySetIndex == MAX_SCENERY_GROUP_OBJECTS)
            continue;

        rct_scenery_group_entry* sceneryGroupEntry = get_scenery_group_entry(scenerySetIndex);
        if (sceneryGroupEntry == nullptr)
            continue;

        int32_t sceneryTabEntryCount = 0;
        for (int32_t i = 0; i < sceneryGroupEntry->entry_count; i++) {
            uint16_t sceneryEntryId = sceneryGroupEntry->scenery_entries[i];
            if (scenery_is_invented(sceneryEntryId) || gCheatsIgnoreResearchStatus) {
                window_scenery_tab_entries[scenerySetIndex][sceneryTabEntryCount] = sceneryEntryId;
                window_scenery_tab_entries[scenerySetIndex][++sceneryTabEntryCount] = -1;
            } else {
                enabledScenerySets[scenerySetIndex] = true;
            }
        }
    }

    // small scenery
    for (uint16_t sceneryId = SCENERY_SMALL_SCENERY_ID_MIN; sceneryId < SCENERY_SMALL_SCENERY_ID_MAX; sceneryId++) {
        if (get_small_scenery_entry(sceneryId) == nullptr)
            continue;

        rct_scenery_entry* sceneryEntry = get_small_scenery_entry(sceneryId);
        init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->small_scenery.scenery_tab_id);
    }

    // large scenery
    for (int32_t sceneryId = SCENERY_LARGE_SCENERY_ID_MIN; sceneryId < SCENERY_LARGE_SCENERY_ID_MAX; sceneryId++) {
        int32_t largeSceneryIndex = sceneryId - SCENERY_LARGE_SCENERY_ID_MIN;

        if (get_large_scenery_entry(largeSceneryIndex) == nullptr)
            continue;

        rct_scenery_entry* sceneryEntry = get_large_scenery_entry(largeSceneryIndex);
        init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->large_scenery.scenery_tab_id);
    }

    // walls
    for (int32_t sceneryId = SCENERY_WALLS_ID_MIN; sceneryId < SCENERY_WALLS_ID_MAX; sceneryId++) {
        int32_t wallSceneryIndex = sceneryId - SCENERY_WALLS_ID_MIN;

        if (get_wall_entry(wallSceneryIndex) == nullptr)
            continue;

        rct_scenery_entry* sceneryEntry = get_wall_entry(wallSceneryIndex);
        init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->wall.scenery_tab_id);
    }

    // banners
    for (int32_t sceneryId = SCENERY_BANNERS_ID_MIN; sceneryId < SCENERY_BANNERS_ID_MAX; sceneryId++) {
        int32_t bannerIndex = sceneryId - SCENERY_BANNERS_ID_MIN;

        if (get_banner_entry(bannerIndex) == nullptr)
            continue;

        rct_scenery_entry* sceneryEntry = get_banner_entry(bannerIndex);
        init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->banner.scenery_tab_id);
    }

    // path bits
    for (int32_t sceneryId = SCENERY_PATH_SCENERY_ID_MIN; sceneryId < SCENERY_PATH_SCENERY_ID_MAX; sceneryId++) {
        int32_t pathBitIndex = sceneryId - SCENERY_PATH_SCENERY_ID_MIN;

        if (get_footpath_item_entry(pathBitIndex) == nullptr)
            continue;

        rct_scenery_entry* sceneryEntry = get_footpath_item_entry(pathBitIndex);
        init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->path_bit.scenery_tab_id);
    }

    for (rct_widgetindex widgetIndex = WIDX_SCENERY_TAB_1; widgetIndex < WIDX_SCENERY_LIST; widgetIndex++)
        window_scenery_widgets[widgetIndex].type = WWT_EMPTY;

    uint8_t tabIndexes[SCENERY_WINDOW_TABS];
    uint8_t order[SCENERY_WINDOW_TABS];
    int32_t usedValues = 0;

    for (int32_t scenerySetId = 0; scenerySetId < MAX_SCENERY_GROUP_OBJECTS; scenerySetId++) {
        rct_scenery_group_entry* sceneryEntry = get_scenery_group_entry(scenerySetId);
        if (sceneryEntry == nullptr)
            continue;

        tabIndexes[usedValues] = scenerySetId;
        order[usedValues] = sceneryEntry->priority;

        usedValues++;
    }

    while (true) {
        bool finished = true;
        for (int32_t i = 1; i < usedValues; i++) {
            if (order[i - 1] > order[i]) {
                uint8_t tmp = tabIndexes[i - 1];
                tabIndexes[i - 1] = tabIndexes[i];
                tabIndexes[i] = tmp;
                tmp = order[i - 1];
                order[i - 1] = order[i];
                order[i] = tmp;
                finished = false;
            }
        }

        if (finished)
            break;
    }

    tabIndexes[usedValues] = SCENERY_WINDOW_TABS - 1;
    usedValues++;

    uint16_t left = 3;
    for (int32_t i = 0; i < usedValues; i ++) {
        uint32_t tabIndex = tabIndexes[i];
        rct_widget* tabWidget = &window_scenery_widgets[tabIndex + WIDX_SCENERY_TAB_1];

        if (left != 3 || tabIndex != SCENERY_WINDOW_TABS - 1) {
            if (window_scenery_tab_entries[tabIndex][0] == -1)
                continue;

            if (enabledScenerySets[tabIndex])
                continue;
        }

        tabWidget->type = WWT_TAB;
        tabWidget->left = left;
        tabWidget->right = left + 0x1E;
        left += 0x1F;

        if (tabIndex >= SCENERY_WINDOW_TABS - 1)
            continue;

        tabWidget->image = get_scenery_group_entry(tabIndex)->image | IMAGE_TYPE_REMAP;
    }

    window_invalidate_by_class(WC_SCENERY);
}

/**
 *
 *  rct2: 0x006DFEE4
 */
void window_scenery_set_default_placement_configuration()
{
    gWindowSceneryRotation = 3;
    gWindowSceneryPrimaryColour = COLOUR_BORDEAUX_RED;
    gWindowScenerySecondaryColour = COLOUR_YELLOW;
    gWindowSceneryTertiaryColour = COLOUR_DARK_BROWN;
    window_scenery_init();

    for (int32_t i = 0; i < SCENERY_WINDOW_TABS; i++)
        gWindowSceneryTabSelections[i] = WINDOW_SCENERY_TAB_SELECTION_UNDEFINED;

    for (int32_t i = 0; i < SCENERY_WINDOW_TABS; i++) {
        if (window_scenery_tab_entries[i][0] != -1) {
            gWindowSceneryActiveTabIndex = i;
            return;
        }
    }

    for (int32_t i = 0; i < 16; i++) {
        rct_widget *tabWidget = &window_scenery_widgets[WIDX_SCENERY_TAB_1 + i];
        if (tabWidget->type != WWT_EMPTY) {
            gWindowSceneryActiveTabIndex = i;
            return;
        }
    }
}

/**
 *
 *  rct2: 0x006E0FEF
 */
rct_window * window_scenery_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_SCENERY);
    if (window != nullptr)
        return window;

    window_scenery_init();

    window = window_create(
        context_get_width() - WINDOW_SCENERY_WIDTH,
        0x1D,
        WINDOW_SCENERY_WIDTH,
        WINDOW_SCENERY_HEIGHT,
        &window_scenery_events,
        WC_SCENERY,
        WF_NO_SCROLLING
    );
    window->widgets = window_scenery_widgets;

    window->enabled_widgets =
        (1 << WIDX_SCENERY_CLOSE) |
        (1 << WIDX_SCENERY_ROTATE_OBJECTS_BUTTON) |
        (1 << WIDX_SCENERY_TAB_1) |
        (1 << WIDX_SCENERY_TAB_2) |
        (1 << WIDX_SCENERY_TAB_3) |
        (1 << WIDX_SCENERY_TAB_4) |
        (1 << WIDX_SCENERY_TAB_5) |
        (1 << WIDX_SCENERY_TAB_6) |
        (1 << WIDX_SCENERY_TAB_7) |
        (1 << WIDX_SCENERY_TAB_8) |
        (1 << WIDX_SCENERY_TAB_9) |
        (1 << WIDX_SCENERY_TAB_10) |
        (1 << WIDX_SCENERY_TAB_11) |
        (1 << WIDX_SCENERY_TAB_12) |
        (1 << WIDX_SCENERY_TAB_13) |
        (1 << WIDX_SCENERY_TAB_14) |
        (1 << WIDX_SCENERY_TAB_15) |
        (1 << WIDX_SCENERY_TAB_16) |
        (1 << WIDX_SCENERY_TAB_17) |
        (1 << WIDX_SCENERY_TAB_18) |
        (1 << WIDX_SCENERY_TAB_19) |
        (1 << WIDX_SCENERY_TAB_20) |
        (1 << WIDX_SCENERY_PRIMARY_COLOUR_BUTTON) |
        (1 << WIDX_SCENERY_SECONDARY_COLOUR_BUTTON) |
        (1 << WIDX_SCENERY_REPAINT_SCENERY_BUTTON) |
        (1 << WIDX_SCENERY_TERTIARY_COLOUR_BUTTON) |
        (1 << WIDX_SCENERY_EYEDROPPER_BUTTON) |
        (1ULL << WIDX_SCENERY_BUILD_CLUSTER_BUTTON);

    window_init_scroll_widgets(window);
    window_scenery_update_scroll(window);
    show_gridlines();
    gWindowSceneryRotation = 3;
    gSceneryCtrlPressed = false;
    gSceneryShiftPressed = false;
    window->scenery.selected_scenery_id = -1;
    window->scenery.hover_counter = 0;
    window_push_others_below(window);
    gSceneryGhostType = 0;
    gSceneryPlaceCost = MONEY32_UNDEFINED;
    gSceneryPlaceRotation = 0;
    gWindowSceneryPaintEnabled = 0; // repaint coloured scenery tool state
    gWindowSceneryEyedropperEnabled = false;
    gWindowSceneryClusterEnabled = 0; // build cluster tool state

    window->min_width = WINDOW_SCENERY_WIDTH;
    window->max_width = WINDOW_SCENERY_WIDTH;
    window->min_height = WINDOW_SCENERY_HEIGHT;
    window->max_height = WINDOW_SCENERY_HEIGHT;

    return window;
}

/**
 *
 *  rct2: 0x006E1A73
 */
void window_scenery_close(rct_window *w)
{
    scenery_remove_ghost_tool_placement();
    hide_gridlines();
    viewport_set_visibility(0);

    if (scenery_tool_is_active())
        tool_cancel();
}

static int32_t count_rows(int32_t items){
    int32_t rows = items / 9;

    return rows;
}

struct scenery_item {
    int32_t allRows;
    int32_t selected_item;
    int16_t sceneryId;
};

static scenery_item window_scenery_count_rows_with_selected_item(int32_t tabIndex)
{
    scenery_item sceneryItem = { 0, 0, -1 };
    int32_t totalItems = 0;
    int16_t id = 0;
    int16_t sceneryId = gWindowSceneryTabSelections[tabIndex];

    while ((id = window_scenery_tab_entries[tabIndex][totalItems]) != -1){
        if (id == sceneryId){
            sceneryItem.selected_item = totalItems;
            sceneryItem.sceneryId = sceneryId;
        }
        totalItems++;
    }
    sceneryItem.allRows = count_rows(totalItems + 8);
    return sceneryItem;
}

static int32_t window_scenery_count_rows()
{
    int32_t tabIndex = gWindowSceneryActiveTabIndex;
    int32_t totalItems = 0;

    while (window_scenery_tab_entries[tabIndex][totalItems] != -1){
        totalItems++;
    }

    int32_t rows = count_rows(totalItems + 8);
    return rows;
}

static int32_t window_scenery_rows_height(int32_t rows)
{
    return rows * SCENERY_BUTTON_HEIGHT;
}

/**
 *
 *  rct2: 0x006BD94C
 */
static void window_scenery_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_SCENERY_CLOSE:
        window_close(w);
        break;
    case WIDX_SCENERY_ROTATE_OBJECTS_BUTTON:
        gWindowSceneryRotation++;
        gWindowSceneryRotation = gWindowSceneryRotation % 4;
        scenery_remove_ghost_tool_placement();
        window_invalidate(w);
        break;
    case WIDX_SCENERY_REPAINT_SCENERY_BUTTON:
        gWindowSceneryPaintEnabled ^= 1;
        gWindowSceneryClusterEnabled = 0;
        gWindowSceneryEyedropperEnabled = false;
        window_invalidate(w);
        break;
    case WIDX_SCENERY_EYEDROPPER_BUTTON:
        gWindowSceneryPaintEnabled = 0;
        gWindowSceneryClusterEnabled = 0;
        gWindowSceneryEyedropperEnabled = !gWindowSceneryEyedropperEnabled;
        window_invalidate(w);
        break;
    case WIDX_SCENERY_BUILD_CLUSTER_BUTTON:
        gWindowSceneryPaintEnabled = 0;
        gWindowSceneryEyedropperEnabled = false;
        if (gWindowSceneryClusterEnabled == 1) {
            gWindowSceneryClusterEnabled = 0;
        }
        else if (network_get_mode() != NETWORK_MODE_CLIENT || network_can_perform_command(network_get_current_player_group_index(), -2)) {
            gWindowSceneryClusterEnabled ^= 1;
        }
        else {
            context_show_error(STR_CANT_DO_THIS, STR_PERMISSION_DENIED);
        }
        window_invalidate(w);
        break;
    }
}

/**
 *
 *  rct2: 0x006E1EB4
 */
void window_scenery_update_scroll(rct_window *w)
{
    int32_t tabIndex = gWindowSceneryActiveTabIndex;
    int32_t listHeight = w->height - 14 - window_scenery_widgets[WIDX_SCENERY_LIST].top - 1;

    scenery_item sceneryItem = window_scenery_count_rows_with_selected_item(tabIndex);
    w->scrolls[0].v_bottom = window_scenery_rows_height(sceneryItem.allRows) + 1;

    int32_t maxTop = std::max(0, w->scrolls[0].v_bottom - listHeight);
    int32_t rowSelected = count_rows(sceneryItem.selected_item);
    if (sceneryItem.sceneryId == -1) {
        rowSelected = 0;
        int16_t sceneryId = window_scenery_tab_entries[tabIndex][0];
        if (sceneryId != -1)
            gWindowSceneryTabSelections[tabIndex] = sceneryId;
    }

    w->scrolls[0].v_top = window_scenery_rows_height(rowSelected);
    w->scrolls[0].v_top = std::min<int32_t>(maxTop, w->scrolls[0].v_top);

    widget_scroll_update_thumbs(w, WIDX_SCENERY_LIST);
}


/**
 *
 *  rct2: 0x006E1E48
 */
static void window_scenery_resize(rct_window *w)
{
    if (w->width < w->min_width) {
        window_invalidate(w);
        w->width = w->min_width;
        window_invalidate(w);
    }

    if (w->width > w->max_width) {
        window_invalidate(w);
        w->width = w->max_width;
        window_invalidate(w);
    }

    if (w->height < w->min_height) {
        window_invalidate(w);
        w->height = w->min_height;
        window_invalidate(w);
        // HACK: For some reason invalidate has not been called
        window_event_invalidate_call(w);
        window_scenery_update_scroll(w);
    }

    if (w->height > w->max_height) {
        window_invalidate(w);
        w->height = w->max_height;
        window_invalidate(w);
        // HACK: For some reason invalidate has not been called
        window_event_invalidate_call(w);
        window_scenery_update_scroll(w);
    }
}

/**
 *
 *  rct2: 0x006E1A25
 */
static void window_scenery_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget) {
    switch (widgetIndex) {
    case WIDX_SCENERY_PRIMARY_COLOUR_BUTTON:
        window_dropdown_show_colour(w, widget, w->colours[1], gWindowSceneryPrimaryColour);
        break;
    case WIDX_SCENERY_SECONDARY_COLOUR_BUTTON:
        window_dropdown_show_colour(w, widget, w->colours[1], gWindowScenerySecondaryColour);
        break;
    case WIDX_SCENERY_TERTIARY_COLOUR_BUTTON:
        window_dropdown_show_colour(w, widget, w->colours[1], gWindowSceneryTertiaryColour);
        break;
    }

    if (widgetIndex >= WIDX_SCENERY_TAB_1 && widgetIndex <= WIDX_SCENERY_TAB_20) {
        gWindowSceneryActiveTabIndex = widgetIndex - WIDX_SCENERY_TAB_1;
        window_invalidate(w);
        gSceneryPlaceCost = MONEY32_UNDEFINED;

        //HACK: for 3210 Ensures that window_scenery_update_scroll gets called one time
        w->max_height = 60;
    }
}

/**
 *
 *  rct2: 0x006E1A54
 */
static void window_scenery_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    if (widgetIndex == WIDX_SCENERY_PRIMARY_COLOUR_BUTTON) {
        gWindowSceneryPrimaryColour = (uint8_t)dropdownIndex;
    }
    else if (widgetIndex == WIDX_SCENERY_SECONDARY_COLOUR_BUTTON) {
        gWindowScenerySecondaryColour = (uint8_t)dropdownIndex;
    }
    else if (widgetIndex == WIDX_SCENERY_TERTIARY_COLOUR_BUTTON) {
        gWindowSceneryTertiaryColour = (uint8_t)dropdownIndex;
    }

    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006E1B9F
 */
static void window_scenery_event_07(rct_window *w)
{
    if (w->scenery.selected_scenery_id != -1) {
        w->scenery.selected_scenery_id = -1;
    }
}

/**
 *
 *  rct2: 0x006E1CD3
 */
static void window_scenery_update(rct_window *w)
{
    const CursorState * state = context_get_cursor_state();
    rct_window *other = window_find_from_point(state->x, state->y);
    if (other == w) {
        int32_t window_x = state->x - w->x + 26;
        int32_t window_y = state->y - w->y;

        if (window_y < 44 || window_x <= w->width) {
            rct_widgetindex widgetIndex = window_find_widget_from_point(w, state->x, state->y);
            if (widgetIndex >= WIDX_SCENERY_TAB_CONTENT_PANEL) {
                w->scenery.hover_counter++;
                if (w->scenery.hover_counter < 8) {
                    if (input_get_state() != INPUT_STATE_SCROLL_LEFT) {
                        w->min_width = WINDOW_SCENERY_WIDTH;
                        w->max_width = WINDOW_SCENERY_WIDTH;
                        w->min_height = WINDOW_SCENERY_HEIGHT;
                        w->max_height = WINDOW_SCENERY_HEIGHT;
                    }
                } else {
                    int32_t windowHeight = std::min(463, w->scrolls[0].v_bottom + 62);
                    if (context_get_height() < 600)
                        windowHeight = std::min(374, windowHeight);
                    windowHeight = std::max(WINDOW_SCENERY_HEIGHT, windowHeight);

                    w->min_width = WINDOW_SCENERY_WIDTH;
                    w->max_width = WINDOW_SCENERY_WIDTH;
                    w->min_height = windowHeight;
                    w->max_height = windowHeight;
                }
            }
        }
    } else {
        w->scenery.hover_counter = 0;
        if (input_get_state() != INPUT_STATE_SCROLL_LEFT) {
            w->min_width = WINDOW_SCENERY_WIDTH;
            w->max_width = WINDOW_SCENERY_WIDTH;
            w->min_height = WINDOW_SCENERY_HEIGHT;
            w->max_height = WINDOW_SCENERY_HEIGHT;
        }
    }

    window_invalidate(w);

    if (!scenery_tool_is_active()){
        window_close(w);
        return;
    }

    if (gWindowSceneryEyedropperEnabled) {
        gCurrentToolId = TOOL_CROSSHAIR;
    } else if (gWindowSceneryPaintEnabled == 1) { // the repaint scenery tool is active
        gCurrentToolId = TOOL_PAINT_DOWN;
    } else {
        uint16_t tabIndex = gWindowSceneryActiveTabIndex;
        int16_t tabSelectedSceneryId = gWindowSceneryTabSelections[tabIndex];

        if (tabSelectedSceneryId != -1) {
            if (tabSelectedSceneryId >= 0x400) { // banner
                gCurrentToolId = TOOL_ENTRANCE_DOWN;
            } else if (tabSelectedSceneryId >= 0x300) { // large scenery
                gCurrentToolId = static_cast<TOOL_IDX>(get_large_scenery_entry(tabSelectedSceneryId - 0x300)->large_scenery.tool_id);
            } else if (tabSelectedSceneryId >= 0x200) { // wall
                gCurrentToolId = static_cast<TOOL_IDX>(get_wall_entry(tabSelectedSceneryId - 0x200)->wall.tool_id);
            } else if (tabSelectedSceneryId >= 0x100) { // path bit
                gCurrentToolId = static_cast<TOOL_IDX>(get_footpath_item_entry(tabSelectedSceneryId - 0x100)->path_bit.tool_id);
            } else { // small scenery
                gCurrentToolId = static_cast<TOOL_IDX>(get_small_scenery_entry(tabSelectedSceneryId)->small_scenery.tool_id);
            }
        }
    }
}

/**
 *
 *  rct2: 0x006E1A91
 */
void window_scenery_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height)
{
    int32_t rows = window_scenery_count_rows();
    *height = window_scenery_rows_height(rows);
}

static int16_t get_scenery_id_by_cursor_pos(int16_t x, int16_t y)
{
    int32_t tabSceneryIndex = x / SCENERY_BUTTON_WIDTH + (y / SCENERY_BUTTON_HEIGHT) * 9;
    uint8_t tabIndex = gWindowSceneryActiveTabIndex;

    int32_t itemCounter = 0;
    int16_t sceneryId = 0;
    while (itemCounter <= tabSceneryIndex) {
        sceneryId = window_scenery_tab_entries[tabIndex][itemCounter];
        if (sceneryId == -1)
            return -1;

        itemCounter++;
    }

    return sceneryId;
}

/**
 *
 *  rct2: 0x006E1C4A
 */
void window_scenery_scrollmousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    int16_t sceneryId = get_scenery_id_by_cursor_pos(x, y);
    if (sceneryId == -1)
        return;

    uint8_t tabIndex = gWindowSceneryActiveTabIndex;
    gWindowSceneryTabSelections[tabIndex] = sceneryId;

    gWindowSceneryPaintEnabled &= 0xFE;
    gWindowSceneryEyedropperEnabled = false;
    audio_play_sound(4, 0, w->x + (w->width / 2));
    w->scenery.hover_counter = -16;
    gSceneryPlaceCost = MONEY32_UNDEFINED;
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006E1BB8
 */
void window_scenery_scrollmouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    int16_t sceneryId = get_scenery_id_by_cursor_pos(x, y);
    if (sceneryId != -1) {
        w->scenery.selected_scenery_id = sceneryId;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x006E1C05
 */
void window_scenery_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
    switch (widgetIndex) {
    case WIDX_SCENERY_LIST:
        set_format_arg(0, uint16_t, STR_LIST);
        break;
    case WIDX_SCENERY_TAB_1:
    case WIDX_SCENERY_TAB_2:
    case WIDX_SCENERY_TAB_3:
    case WIDX_SCENERY_TAB_4:
    case WIDX_SCENERY_TAB_5:
    case WIDX_SCENERY_TAB_6:
    case WIDX_SCENERY_TAB_7:
    case WIDX_SCENERY_TAB_8:
    case WIDX_SCENERY_TAB_9:
    case WIDX_SCENERY_TAB_10:
    case WIDX_SCENERY_TAB_11:
    case WIDX_SCENERY_TAB_12:
    case WIDX_SCENERY_TAB_13:
    case WIDX_SCENERY_TAB_14:
    case WIDX_SCENERY_TAB_15:
    case WIDX_SCENERY_TAB_16:
    case WIDX_SCENERY_TAB_17:
    case WIDX_SCENERY_TAB_18:
    case WIDX_SCENERY_TAB_19:
        set_format_arg(0, rct_string_id, get_scenery_group_entry(widgetIndex - WIDX_SCENERY_TAB_1)->name);
        break;
    case WIDX_SCENERY_TAB_20:
        set_format_arg(0, rct_string_id, STR_MISCELLANEOUS);
        break;
    }
}

/**
 *
 *  rct2: 0x006E118B
 */
void window_scenery_invalidate(rct_window *w)
{
    uint16_t tabIndex = gWindowSceneryActiveTabIndex;
    uint32_t titleStringId = STR_MISCELLANEOUS;
    if (tabIndex < SCENERY_WINDOW_TABS - 1) {
        rct_scenery_group_entry * sgEntry = get_scenery_group_entry(tabIndex);
        if (sgEntry != nullptr) {
            titleStringId = sgEntry->name;
        }
    }

    window_scenery_widgets[WIDX_SCENERY_TITLE].text = titleStringId;

    w->pressed_widgets = 0;
    w->pressed_widgets |= 1ULL << (tabIndex + WIDX_SCENERY_TAB_1);
    if (gWindowSceneryPaintEnabled == 1)
        w->pressed_widgets |= (1 << WIDX_SCENERY_REPAINT_SCENERY_BUTTON);
    if (gWindowSceneryEyedropperEnabled)
        w->pressed_widgets |= (1 << WIDX_SCENERY_EYEDROPPER_BUTTON);
    if (gWindowSceneryClusterEnabled == 1)
        w->pressed_widgets |= (1 << WIDX_SCENERY_BUILD_CLUSTER_BUTTON);

    window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_EMPTY;
    window_scenery_widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].type = WWT_EMPTY;
    window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WWT_EMPTY;

    if (!(gWindowSceneryPaintEnabled & 1)) {
        window_scenery_widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].type = WWT_FLATBTN;
    }

    int16_t tabSelectedSceneryId = gWindowSceneryTabSelections[tabIndex];
    if (tabSelectedSceneryId != -1) {
        if (tabSelectedSceneryId < 0x100) {
            if (!(gWindowSceneryPaintEnabled & 1)) {
                window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WWT_FLATBTN;
            }

            rct_scenery_entry* sceneryEntry = get_small_scenery_entry(tabSelectedSceneryId);
            if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_ROTATABLE)) {
                window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_FLATBTN;
            }
        }
        else if (tabSelectedSceneryId >= 0x300) {
            window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_FLATBTN;
        }
    }

    window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].image =
        SPRITE_ID_PALETTE_COLOUR_1(gWindowSceneryPrimaryColour) | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;
    window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].image =
        SPRITE_ID_PALETTE_COLOUR_1(gWindowScenerySecondaryColour) | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;
    window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].image =
        SPRITE_ID_PALETTE_COLOUR_1(gWindowSceneryTertiaryColour) | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;

    window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_EMPTY;
    window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_EMPTY;
    window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WWT_EMPTY;

    if (gWindowSceneryPaintEnabled & 1) { // repaint coloured scenery tool is on
        window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
        window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
        window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
        window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_EMPTY;
    } else if (tabSelectedSceneryId != -1) {
        rct_scenery_entry* sceneryEntry = nullptr;

        if (tabSelectedSceneryId >= 0x400) {
            sceneryEntry = get_banner_entry(tabSelectedSceneryId - 0x400);
            if (sceneryEntry->banner.flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR)
            {
                window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
            }
        } else if (tabSelectedSceneryId >= 0x300) {
            sceneryEntry = get_large_scenery_entry(tabSelectedSceneryId - 0x300);

            if (sceneryEntry->large_scenery.flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR)
                window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
            if (sceneryEntry->large_scenery.flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
                window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
        } else if (tabSelectedSceneryId >= 0x200) {
            sceneryEntry = get_wall_entry(tabSelectedSceneryId - 0x200);
            if (sceneryEntry->wall.flags & (WALL_SCENERY_HAS_PRIMARY_COLOUR | WALL_SCENERY_HAS_GLASS)) {
                window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLOURBTN;

                if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
                    window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLOURBTN;

                    if (sceneryEntry->wall.flags2 & WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR)
                        window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_EMPTY;
                    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR)
                        window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
                }
            }
        } else if (tabSelectedSceneryId < 0x100) {
            sceneryEntry = get_small_scenery_entry(tabSelectedSceneryId);

            if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR | SMALL_SCENERY_FLAG_HAS_GLASS))
            {
                window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLOURBTN;

                if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
                    window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
            }
        }
    }

    window_scenery_widgets[WIDX_SCENERY_BACKGROUND].right = w->width - 1;
    window_scenery_widgets[WIDX_SCENERY_BACKGROUND].bottom = w->height - 1;
    window_scenery_widgets[WIDX_SCENERY_TAB_CONTENT_PANEL].right = w->width - 1;
    window_scenery_widgets[WIDX_SCENERY_TAB_CONTENT_PANEL].bottom = w->height - 1;
    window_scenery_widgets[WIDX_SCENERY_TITLE].right = w->width - 2;
    window_scenery_widgets[WIDX_SCENERY_CLOSE].left = w->width - 13;
    window_scenery_widgets[WIDX_SCENERY_CLOSE].right = window_scenery_widgets[WIDX_SCENERY_CLOSE].left + 10;
    window_scenery_widgets[WIDX_SCENERY_LIST].right = w->width - 26;
    window_scenery_widgets[WIDX_SCENERY_LIST].bottom = w->height - 14;

    window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].left = w->width - 25;
    window_scenery_widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].left = w->width - 25;
    window_scenery_widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].left = w->width - 25;
    window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].left = w->width - 25;
    window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].right = w->width - 2;
    window_scenery_widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].right = w->width - 2;
    window_scenery_widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].right = w->width - 2;
    window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].right = w->width - 2;

    window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].left = w->width - 19;
    window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].left = w->width - 19;
    window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].left = w->width - 19;
    window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].right = w->width - 8;
    window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].right = w->width - 8;
    window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].right = w->width - 8;
}

/**
 *
 *  rct2: 0x006E1462
 */
void window_scenery_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    uint16_t tabIndex = gWindowSceneryActiveTabIndex;
    uint16_t selectedWidgetId = tabIndex + 4;
    uint32_t imageId = ((w->colours[1] << 19) | window_scenery_widgets[selectedWidgetId].image) + 1ul;

    gfx_draw_sprite(dpi, imageId,
        w->x + window_scenery_widgets[selectedWidgetId].left,
        w->y + window_scenery_widgets[selectedWidgetId].top,
        selectedWidgetId);

    int16_t selectedSceneryEntryId = w->scenery.selected_scenery_id;
    if (selectedSceneryEntryId == -1) {
        if (gWindowSceneryPaintEnabled & 1)  // repaint coloured scenery tool is on
            return;
        if (gWindowSceneryEyedropperEnabled)
            return;

        selectedSceneryEntryId = gWindowSceneryTabSelections[tabIndex];

        if (selectedSceneryEntryId == -1)
            return;
    }

    uint32_t price = 0;

    rct_scenery_entry* sceneryEntry = nullptr;
    if (selectedSceneryEntryId >= 0x400) {
        sceneryEntry = get_banner_entry(selectedSceneryEntryId - 0x400);
        price = sceneryEntry->banner.price;
    } else if (selectedSceneryEntryId >= 0x300) {
        sceneryEntry = get_large_scenery_entry(selectedSceneryEntryId - 0x300);
        price = sceneryEntry->large_scenery.price * 10;
    } else if (selectedSceneryEntryId >= 0x200) {
        sceneryEntry = get_wall_entry(selectedSceneryEntryId - 0x200);
        price = sceneryEntry->wall.price;
    } else if (selectedSceneryEntryId >= 0x100) {
        sceneryEntry = get_footpath_item_entry(selectedSceneryEntryId - 0x100);
        price = sceneryEntry->path_bit.price;
    } else {
        sceneryEntry = get_small_scenery_entry(selectedSceneryEntryId);
        price = sceneryEntry->small_scenery.price * 10;
    }

    if (w->scenery.selected_scenery_id == -1 && gSceneryPlaceCost != MONEY32_UNDEFINED) {
        price = gSceneryPlaceCost;
    }

    set_format_arg(0, uint32_t, price);

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY)) {
        // -14
        gfx_draw_string_right(dpi, STR_COST_LABEL, gCommonFormatArgs, COLOUR_BLACK,
            w->x + w->width - 0x1A, w->y + w->height - 13);
    }

    set_format_arg(0, rct_string_id, sceneryEntry->name);
    gfx_draw_string_left_clipped(dpi, STR_BLACK_STRING, gCommonFormatArgs, COLOUR_BLACK,
        w->x + 3, w->y + w->height - 13, w->width - 19);
}

/**
*
*  rct2: 0x006E15ED
*/
void window_scenery_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex)
{
    gfx_clear(dpi, ColourMapA[w->colours[1]].mid_light);

    uint8_t tabIndex = gWindowSceneryActiveTabIndex;

    int32_t sceneryTabItemIndex = 0;
    int16_t currentSceneryGlobalId = -1;
    int16_t left = 0, top = 0;

    while ((currentSceneryGlobalId = window_scenery_tab_entries[tabIndex][sceneryTabItemIndex]) != -1) {
        uint16_t tabSelectedSceneryId = gWindowSceneryTabSelections[tabIndex];

        if (gWindowSceneryPaintEnabled == 1 || gWindowSceneryEyedropperEnabled) {
            if (w->scenery.selected_scenery_id == currentSceneryGlobalId) {
                gfx_fill_rect_inset(dpi, left, top, left + SCENERY_BUTTON_WIDTH - 1,
                    top + SCENERY_BUTTON_HEIGHT - 1, w->colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
            }
        }
        else {
            if (tabSelectedSceneryId == currentSceneryGlobalId) {
                gfx_fill_rect_inset(dpi, left, top, left + SCENERY_BUTTON_WIDTH - 1,
                    top + SCENERY_BUTTON_HEIGHT - 1, w->colours[1], (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_MID_LIGHT));
            }
            else if (w->scenery.selected_scenery_id == currentSceneryGlobalId) {
                gfx_fill_rect_inset(dpi, left, top, left + SCENERY_BUTTON_WIDTH - 1,
                    top + SCENERY_BUTTON_HEIGHT - 1, w->colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
            }
        }

        rct_scenery_entry* sceneryEntry;
        rct_drawpixelinfo clipdpi;
        if (clip_drawpixelinfo(&clipdpi, dpi, left + 1, top + 1, SCENERY_BUTTON_WIDTH - 2, SCENERY_BUTTON_HEIGHT - 2)) {
            if (currentSceneryGlobalId >= SCENERY_BANNERS_ID_MIN) {
                sceneryEntry = get_banner_entry(currentSceneryGlobalId - SCENERY_BANNERS_ID_MIN);
                uint32_t imageId = sceneryEntry->image + gWindowSceneryRotation * 2;
                imageId |= (gWindowSceneryPrimaryColour << 19) | IMAGE_TYPE_REMAP;

                gfx_draw_sprite(&clipdpi, imageId, 0x21, 0x28, w->colours[1]);
                gfx_draw_sprite(&clipdpi, imageId + 1, 0x21, 0x28, w->colours[1]);
            }
            else if (currentSceneryGlobalId >= SCENERY_LARGE_SCENERY_ID_MIN) {
                sceneryEntry = get_large_scenery_entry(currentSceneryGlobalId - SCENERY_LARGE_SCENERY_ID_MIN);
                uint32_t imageId = sceneryEntry->image + gWindowSceneryRotation;
                imageId |= (gWindowSceneryPrimaryColour << 19) | IMAGE_TYPE_REMAP;
                imageId |= (gWindowScenerySecondaryColour << 24) | IMAGE_TYPE_REMAP_2_PLUS;

                gfx_draw_sprite(&clipdpi, imageId, 0x21, 0, w->colours[1]);
            }
            else if (currentSceneryGlobalId >= SCENERY_WALLS_ID_MIN) {
                sceneryEntry = get_wall_entry(currentSceneryGlobalId - SCENERY_WALLS_ID_MIN);
                uint32_t imageId = sceneryEntry->image;
                uint8_t tertiaryColour = w->colours[1];
                uint16_t spriteTop = (sceneryEntry->wall.height * 2) + 0x32;

                if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_GLASS) {
                    imageId |= (gWindowSceneryPrimaryColour << 19) | IMAGE_TYPE_REMAP;

                    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
                        imageId |= (gWindowScenerySecondaryColour << 24) | IMAGE_TYPE_REMAP_2_PLUS;
                    }
                    gfx_draw_sprite(&clipdpi, imageId, 0x2F, spriteTop, tertiaryColour);

                    imageId = (sceneryEntry->image + 0x40000006) | (GlassPaletteIds[gWindowSceneryPrimaryColour] << 19);
                    gfx_draw_sprite(&clipdpi, imageId, 0x2F, spriteTop, tertiaryColour);
                }
                else {
                    imageId |= (gWindowSceneryPrimaryColour << 19) | IMAGE_TYPE_REMAP;

                    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
                        imageId |= (gWindowScenerySecondaryColour << 24) | IMAGE_TYPE_REMAP_2_PLUS;

                        if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR) {
                            imageId &= 0xDFFFFFFF;
                            tertiaryColour = gWindowSceneryTertiaryColour;
                        }
                    }
                    gfx_draw_sprite(&clipdpi, imageId, 0x2F, spriteTop, tertiaryColour);

                    if (sceneryEntry->wall.flags & WALL_SCENERY_IS_DOOR) {
                        gfx_draw_sprite(&clipdpi, imageId + 1, 0x2F, spriteTop, tertiaryColour);
                    }
                }
            }
            else if (currentSceneryGlobalId >= SCENERY_PATH_SCENERY_ID_MIN) {
                sceneryEntry = get_footpath_item_entry(currentSceneryGlobalId - SCENERY_PATH_SCENERY_ID_MIN);
                uint32_t imageId = sceneryEntry->image;

                gfx_draw_sprite(&clipdpi, imageId, 0x0B, 0x10, w->colours[1]);
            }
            else {
                sceneryEntry = get_small_scenery_entry(currentSceneryGlobalId);
                uint32_t imageId = sceneryEntry->image + gWindowSceneryRotation;

                if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR)) {
                    imageId |= (gWindowSceneryPrimaryColour << 19) | IMAGE_TYPE_REMAP;

                    if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR)) {
                        imageId |= (gWindowScenerySecondaryColour << 24) | IMAGE_TYPE_REMAP_2_PLUS;
                    }
                }

                uint16_t spriteTop = (sceneryEntry->small_scenery.height / 4) + 0x2B;

                if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE) &&
                    scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_VOFFSET_CENTRE)) {
                    spriteTop -= 0x0C;
                }

                gfx_draw_sprite(&clipdpi, imageId, 0x20, spriteTop, w->colours[1]);

                if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_HAS_GLASS)) {
                    imageId = ((sceneryEntry->image + gWindowSceneryRotation) + 0x40000004) +
                        (GlassPaletteIds[gWindowSceneryPrimaryColour] << 19);

                    gfx_draw_sprite(&clipdpi, imageId, 0x20, spriteTop, w->colours[1]);
                }

                if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_ANIMATED_FG)) {
                    imageId = (sceneryEntry->image + gWindowSceneryRotation) + 4;
                    gfx_draw_sprite(&clipdpi, imageId, 0x20, spriteTop, w->colours[1]);
                }
            }
        }

        left += SCENERY_BUTTON_WIDTH;
        if (left >= 594) {
            top += SCENERY_BUTTON_HEIGHT;
            left = 0;
        }
        sceneryTabItemIndex++;
    }
}

static int32_t window_scenery_find_tab_with_scenery_id(int32_t sceneryId)
{
    for (int32_t i = 0; i < SCENERY_WINDOW_TABS; i++) {
        for (int32_t j = 0; j < SCENERY_ENTRIES_BY_TAB; j++) {
            int16_t entry = window_scenery_tab_entries[i][j];
            if (entry == -1) break;
            if (entry == sceneryId) return i;
        }
    }
    return -1;
}

bool window_scenery_set_selected_item(int32_t sceneryId)
{
    bool result = false;
    rct_window * w = window_bring_to_front_by_class(WC_SCENERY);
    if (w != nullptr) {
        int32_t tabIndex = window_scenery_find_tab_with_scenery_id(sceneryId);
        if (tabIndex != -1) {
            gWindowSceneryActiveTabIndex = tabIndex;
            gWindowSceneryTabSelections[tabIndex] = sceneryId;

            audio_play_sound(SOUND_CLICK_1, 0, context_get_width() / 2);
            w->scenery.hover_counter = -16;
            gSceneryPlaceCost = MONEY32_UNDEFINED;
            window_invalidate(w);
            result = true;
        }
    }
    return result;
}

// Used after removing objects, in order to avoid crashes.
void window_scenery_reset_selected_scenery_items()
{
    for (size_t i = 0; i < SCENERY_WINDOW_TABS; i++)
    {
        gWindowSceneryTabSelections[i] = WINDOW_SCENERY_TAB_SELECTION_UNDEFINED;
    }
}
