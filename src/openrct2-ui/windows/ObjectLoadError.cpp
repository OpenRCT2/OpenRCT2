/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <string>
#include <vector>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/platform/platform.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/object/ObjectList.h>

// clang-format off
enum WINDOW_OBJECT_LOAD_ERROR_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_COLUMN_OBJECT_NAME,
    WIDX_COLUMN_OBJECT_SOURCE,
    WIDX_COLUMN_OBJECT_TYPE,
    WIDX_SCROLL,
    WIDX_COPY_CURRENT,
    WIDX_COPY_ALL
};

#define WW 450
#define WH 400
#define WW_LESS_PADDING (WW - 5)
#define NAME_COL_LEFT 4
#define SOURCE_COL_LEFT ((WW_LESS_PADDING / 4) + 1)
#define TYPE_COL_LEFT (5 * WW_LESS_PADDING / 8 + 1)
#define LIST_ITEM_HEIGHT 10

static rct_widget window_object_load_error_widgets[] = {
    { WWT_FRAME,             0, 0,               WW - 1,                0,          WH - 1,     STR_NONE,                       STR_NONE },                // Background
    { WWT_CAPTION,           0, 1,               WW - 2,                1,          14,         STR_OBJECT_LOAD_ERROR_TITLE,    STR_WINDOW_TITLE_TIP },    // Title bar
    { WWT_CLOSEBOX,          0, WW - 13,         WW - 3,                2,          13,         STR_CLOSE_X,                    STR_CLOSE_WINDOW_TIP },    // Close button
    { WWT_TABLE_HEADER,      0, NAME_COL_LEFT,   SOURCE_COL_LEFT - 1,   57,         70,         STR_OBJECT_NAME,                STR_NONE },                // 'Object name' header
    { WWT_TABLE_HEADER,      0, SOURCE_COL_LEFT, TYPE_COL_LEFT - 1,     57,         70,         STR_OBJECT_SOURCE,              STR_NONE },                // 'Object source' header
    { WWT_TABLE_HEADER,      0, TYPE_COL_LEFT,   WW_LESS_PADDING - 1,   57,         70,         STR_OBJECT_TYPE,                STR_NONE },                // 'Object type' header
    { WWT_SCROLL,            0, 4,               WW_LESS_PADDING,       70,         WH - 33,    SCROLL_VERTICAL,                STR_NONE },                // Scrollable list area
    { WWT_BUTTON,            0, 45,              220,                   WH - 23,    WH - 10,    STR_COPY_SELECTED,              STR_NONE },                // Copy selected btn
    { WWT_BUTTON,            0, 230,             400,                   WH - 23,    WH - 10,    STR_COPY_ALL,                   STR_NONE },                // Copy all btn
    { WIDGETS_END },
};

static rct_string_id get_object_type_string(const rct_object_entry *entry);
static void window_object_load_error_close(rct_window *w);
static void window_object_load_error_update(rct_window *w);
static void window_object_load_error_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_object_load_error_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_object_load_error_scrollmouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_object_load_error_scrollmousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_object_load_error_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_object_load_error_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_object_load_error_events = {
    window_object_load_error_close,
    window_object_load_error_mouseup,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_object_load_error_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_object_load_error_scrollgetsize,
    window_object_load_error_scrollmousedown,
    nullptr,
    window_object_load_error_scrollmouseover,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_object_load_error_paint,
    window_object_load_error_scrollpaint
};
// clang-format on

static std::vector<rct_object_entry> _invalid_entries;
static int32_t highlighted_index = -1;
static std::string file_path;

/**
*  Returns an rct_string_id that represents an rct_object_entry's type.
*
*  Could possibly be moved out of the window file if other
*  uses exist and a suitable location is found.
*/
static rct_string_id get_object_type_string(const rct_object_entry *entry)
{
    rct_string_id result;
    uint8_t objectType = object_entry_get_type(entry);
    switch (objectType) {
        case OBJECT_TYPE_RIDE:
            result = STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS;
            break;
        case OBJECT_TYPE_SMALL_SCENERY:
            result = STR_OBJECT_SELECTION_SMALL_SCENERY;
            break;
        case OBJECT_TYPE_LARGE_SCENERY:
            result = STR_OBJECT_SELECTION_LARGE_SCENERY;
            break;
        case OBJECT_TYPE_WALLS:
            result = STR_OBJECT_SELECTION_WALLS_FENCES;
            break;
        case OBJECT_TYPE_BANNERS:
            result = STR_OBJECT_SELECTION_PATH_SIGNS;
            break;
        case OBJECT_TYPE_PATHS:
            result = STR_OBJECT_SELECTION_FOOTPATHS;
            break;
        case OBJECT_TYPE_PATH_BITS:
            result = STR_OBJECT_SELECTION_PATH_EXTRAS;
            break;
        case OBJECT_TYPE_SCENERY_GROUP:
            result = STR_OBJECT_SELECTION_SCENERY_GROUPS;
            break;
        case OBJECT_TYPE_PARK_ENTRANCE:
            result = STR_OBJECT_SELECTION_PARK_ENTRANCE;
            break;
        case OBJECT_TYPE_WATER:
            result = STR_OBJECT_SELECTION_WATER;
            break;
        case OBJECT_TYPE_SCENARIO_TEXT:
            result = STR_OBJECT_SELECTION_SCENARIO_DESCRIPTION;
            break;
        default:
            result = STR_UNKNOWN_OBJECT_TYPE;
    }
    return result;
}

/**
*  Returns a newline-separated string listing all object names.
*  Used for placing all names on the clipboard.
*/
static void copy_object_names_to_clipboard(rct_window *w)
{
    // Something has gone wrong, this shouldn't happen.
    // We don't want to allocate stupidly large amounts of memory for no reason
    assert(w->no_list_items > 0 && w->no_list_items <= OBJECT_ENTRY_COUNT);

    // No system has a newline over 2 characters
    size_t line_sep_len = strnlen(PLATFORM_NEWLINE, 2);
    size_t buffer_len   = (w->no_list_items * (8 + line_sep_len)) + 1;
    utf8 * buffer       = new utf8[buffer_len]{};

    size_t cur_len = 0;
    for (uint16_t i = 0; i < w->no_list_items; i++) {
        cur_len += (8 + line_sep_len);
        assert(cur_len < buffer_len);

        uint16_t nameLength = 8;
        for (; nameLength > 0; nameLength--)
        {
            if (_invalid_entries[i].name[nameLength - 1] != ' ')
                break;
        }

        strncat(buffer, _invalid_entries[i].name, nameLength);
        strncat(buffer, PLATFORM_NEWLINE, line_sep_len);
    }

    platform_place_string_on_clipboard(buffer);
    delete[] buffer;
}

rct_window * window_object_load_error_open(utf8 * path, size_t numMissingObjects, const rct_object_entry * missingObjects)
{
    _invalid_entries = std::vector<rct_object_entry>(missingObjects, missingObjects + numMissingObjects);

    // Check if window is already open
    rct_window * window = window_bring_to_front_by_class(WC_OBJECT_LOAD_ERROR);
    if (window == nullptr) {
        window = window_create_centred(
            WW,
            WH,
            &window_object_load_error_events,
            WC_OBJECT_LOAD_ERROR,
            0
        );

        window->widgets = window_object_load_error_widgets;
        window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_COPY_CURRENT) | (1 << WIDX_COPY_ALL);

        window_init_scroll_widgets(window);
        window->colours[0] = COLOUR_LIGHT_BLUE;
        window->colours[1] = COLOUR_LIGHT_BLUE;
        window->colours[2] = COLOUR_LIGHT_BLUE;
    }

    // Refresh list items and path
    window->no_list_items = (uint16_t)numMissingObjects;
    file_path = path;

    window_invalidate(window);
    return window;
}

static void window_object_load_error_close(rct_window *w)
{
    _invalid_entries.clear();
    _invalid_entries.shrink_to_fit();
}

static void window_object_load_error_update(rct_window *w)
{
    // Check if the mouse is hovering over the list
    if (!widget_is_highlighted(w, WIDX_SCROLL)) {
        highlighted_index = -1;
        widget_invalidate(w, WIDX_SCROLL);
    }
}

static void window_object_load_error_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_COPY_CURRENT:
            if (w->selected_list_item > -1 && w->selected_list_item < w->no_list_items)
            {
                utf8 * selected_name = strndup(_invalid_entries[w->selected_list_item].name, 8);
                utf8 * strp = strchr(selected_name, ' ');
                if (strp != nullptr)
                    *strp = '\0';

                platform_place_string_on_clipboard(selected_name);
                SafeFree(selected_name);
            }
            break;
        case WIDX_COPY_ALL:
            copy_object_names_to_clipboard(w);
            break;
    }
}

static void window_object_load_error_scrollmouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    // Highlight item that the cursor is over, or remove highlighting if none
    int32_t selected_item;
    selected_item = y / SCROLLABLE_ROW_HEIGHT;
    if (selected_item < 0 || selected_item >= w->no_list_items)
        highlighted_index = -1;
    else
        highlighted_index = selected_item;

    widget_invalidate(w, WIDX_SCROLL);
}

static void window_object_load_error_select_element_from_list(rct_window *w, int32_t index)
{
    if (index < 0 || index > w->no_list_items) {
        w->selected_list_item = -1;
    }
    else {
        w->selected_list_item = index;
    }
    widget_invalidate(w, WIDX_SCROLL);
}

static void window_object_load_error_scrollmousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    int32_t selected_item;
    selected_item = y / SCROLLABLE_ROW_HEIGHT;
    window_object_load_error_select_element_from_list(w, selected_item);
}

static void window_object_load_error_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height)
{
    *height = w->no_list_items * SCROLLABLE_ROW_HEIGHT;
}

static void window_object_load_error_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    // Draw explanatory message
    set_format_arg(0, rct_string_id, STR_OBJECT_ERROR_WINDOW_EXPLANATION);
    gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, w->x + 5, w->y + 18, WW-10, STR_BLACK_STRING, COLOUR_BLACK);

    // Draw file name
    set_format_arg(0, rct_string_id, STR_OBJECT_ERROR_WINDOW_FILE);
    set_format_arg(2, utf8*, file_path.c_str());
    gfx_draw_string_left_clipped(dpi, STR_BLACK_STRING, gCommonFormatArgs, COLOUR_BLACK, w->x + 5, w->y + 43, WW-5);
}

static void window_object_load_error_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex)
{
    gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, ColourMapA[w->colours[1]].mid_light);
    const int32_t list_width = w->widgets[WIDX_SCROLL].right - w->widgets[WIDX_SCROLL].left;

    for (int32_t i = 0; i < w->no_list_items; i++)
    {
        int32_t y = i * SCROLLABLE_ROW_HEIGHT;
        if (y > dpi->y + dpi->height)
            break;

        if (y + SCROLLABLE_ROW_HEIGHT < dpi->y)
            continue;

        // If hovering over item, change the color and fill the backdrop.
        if (i == w->selected_list_item)
            gfx_fill_rect(dpi, 0, y, list_width, y + SCROLLABLE_ROW_HEIGHT - 1, ColourMapA[w->colours[1]].darker);
        else if (i == highlighted_index)
            gfx_fill_rect(dpi, 0, y, list_width, y + SCROLLABLE_ROW_HEIGHT - 1, ColourMapA[w->colours[1]].mid_dark);
        else if ((i & 1) != 0) // odd / even check
            gfx_fill_rect(dpi, 0, y, list_width, y + SCROLLABLE_ROW_HEIGHT - 1, ColourMapA[w->colours[1]].light);

        // Draw the actual object entry's name...
        gfx_draw_string(dpi, strndup(_invalid_entries[i].name, 8), COLOUR_DARK_GREEN, NAME_COL_LEFT - 3, y);

        // ... source game ...
        rct_string_id sourceStringId = object_manager_get_source_game_string(&_invalid_entries[i]);
        gfx_draw_string_left(dpi, sourceStringId, nullptr, COLOUR_DARK_GREEN, SOURCE_COL_LEFT - 3, y);

        // ... and type
        rct_string_id type = get_object_type_string(&_invalid_entries[i]);
        gfx_draw_string_left(dpi, type, nullptr, COLOUR_DARK_GREEN, TYPE_COL_LEFT - 3, y);
    }
}
