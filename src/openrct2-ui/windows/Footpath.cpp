/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Cheats.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/FootpathPlaceAction.hpp>
#include <openrct2/audio/audio.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Surface.h>

// clang-format off
enum
{
    PATH_CONSTRUCTION_MODE_LAND,
    PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL,
    PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL
};

enum
{
    SELECTED_PATH_TYPE_NORMAL,
    SELECTED_PATH_TYPE_QUEUE
};

enum WINDOW_FOOTPATH_WIDGET_IDX
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,

    WIDX_TYPE_GROUP,
    WIDX_FOOTPATH_TYPE,
    WIDX_QUEUELINE_TYPE,

    WIDX_DIRECTION_GROUP,
    WIDX_DIRECTION_NW,
    WIDX_DIRECTION_NE,
    WIDX_DIRECTION_SW,
    WIDX_DIRECTION_SE,

    WIDX_SLOPE_GROUP,
    WIDX_SLOPEDOWN,
    WIDX_LEVEL,
    WIDX_SLOPEUP,
    WIDX_CONSTRUCT,
    WIDX_REMOVE,

    WIDX_MODE_GROUP,
    WIDX_CONSTRUCT_ON_LAND,
    WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL,
};

static rct_widget window_footpath_widgets[] = {
    {WWT_FRAME,    0, 0,  105, 0,   380, 0xFFFFFFFF,                        STR_NONE},
    {WWT_CAPTION,  0, 1,  104, 1,   14,  STR_FOOTPATHS,                     STR_WINDOW_TITLE_TIP},
    {WWT_CLOSEBOX, 0, 93, 103, 2,   13,  STR_CLOSE_X,                       STR_CLOSE_WINDOW_TIP},
    // Type group
    {WWT_GROUPBOX, 0, 3,  102, 17,  71,  STR_TYPE,                          STR_NONE},
    {WWT_FLATBTN,  1, 6,  52,  30,  65,  0xFFFFFFFF,                        STR_FOOTPATH_TIP},
    {WWT_FLATBTN,  1, 53, 99,  30,  65,  0xFFFFFFFF,                        STR_QUEUE_LINE_PATH_TIP},

    // Direction group
    {WWT_GROUPBOX, 0, 3,  102, 75,  151, STR_DIRECTION,                     STR_NONE},
    {WWT_FLATBTN,  1, 53, 97,  87,  115, SPR_CONSTRUCTION_DIRECTION_NE,     STR_DIRECTION_TIP},
    {WWT_FLATBTN,  1, 53, 97,  116, 144, SPR_CONSTRUCTION_DIRECTION_SE,     STR_DIRECTION_TIP},
    {WWT_FLATBTN,  1, 8,  52,  116, 144, SPR_CONSTRUCTION_DIRECTION_SW,     STR_DIRECTION_TIP},
    {WWT_FLATBTN,  1, 8,  52,  87,  115, SPR_CONSTRUCTION_DIRECTION_NW,     STR_DIRECTION_TIP},

    // Slope group
    {WWT_GROUPBOX, 0, 3,  102, 155, 195, STR_SLOPE,                         STR_NONE},
    {WWT_FLATBTN,  1, 17, 40,  167, 190, SPR_RIDE_CONSTRUCTION_SLOPE_DOWN,  STR_SLOPE_DOWN_TIP},
    {WWT_FLATBTN,  1, 41, 64,  167, 190, SPR_RIDE_CONSTRUCTION_SLOPE_LEVEL, STR_LEVEL_TIP},
    {WWT_FLATBTN,  1, 65, 88,  167, 190, SPR_RIDE_CONSTRUCTION_SLOPE_UP,    STR_SLOPE_UP_TIP},
    {WWT_FLATBTN,  1, 8,  97,  202, 291, 0xFFFFFFFF,                        STR_CONSTRUCT_THE_SELECTED_FOOTPATH_SECTION_TIP},
    {WWT_FLATBTN,  1, 30, 75,  295, 318, SPR_DEMOLISH_CURRENT_SECTION,      STR_REMOVE_PREVIOUS_FOOTPATH_SECTION_TIP},

    // Mode group
    {WWT_GROUPBOX, 0, 3,  102, 321, 374, 0xFFFFFFFF,                        STR_NONE},
    {WWT_FLATBTN,  1, 13, 48,  332, 367, SPR_CONSTRUCTION_FOOTPATH_LAND,    STR_CONSTRUCT_FOOTPATH_ON_LAND_TIP},
    {WWT_FLATBTN,  1, 57, 92,  332, 367, SPR_CONSTRUCTION_FOOTPATH_BRIDGE,  STR_CONSTRUCT_BRIDGE_OR_TUNNEL_FOOTPATH_TIP},
    {WIDGETS_END},
};

static void window_footpath_close(rct_window * w);
static void window_footpath_mouseup(rct_window * w, rct_widgetindex widgetIndex);
static void window_footpath_mousedown(rct_window * w, rct_widgetindex widgetIndex, rct_widget * widget);
static void window_footpath_dropdown(rct_window * w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_footpath_update(rct_window * w);
static void window_footpath_toolupdate(rct_window * w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_footpath_tooldown(rct_window * w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_footpath_tooldrag(rct_window * w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_footpath_toolup(rct_window * w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_footpath_invalidate(rct_window * w);
static void window_footpath_paint(rct_window * w, rct_drawpixelinfo * dpi);

static rct_window_event_list window_footpath_events = {
    window_footpath_close,
    window_footpath_mouseup,
    nullptr,
    window_footpath_mousedown,
    window_footpath_dropdown,
    nullptr,
    window_footpath_update,
    nullptr,
    nullptr,
    window_footpath_toolupdate,
    window_footpath_tooldown,
    window_footpath_tooldrag,
    window_footpath_toolup,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_footpath_invalidate,
    window_footpath_paint,
    nullptr
};
// clang-format on

static money32 _window_footpath_cost;
static int8_t _window_footpath_provisional_path_arrow_timer;
static uint8_t _lastUpdatedCameraRotation = UINT8_MAX;
static bool _footpathErrorOccured;

/** rct2: 0x0098D8B4 */
static constexpr const uint8_t DefaultPathSlope[] = {
    0,
    SLOPE_IS_IRREGULAR_FLAG,
    SLOPE_IS_IRREGULAR_FLAG,
    FOOTPATH_PROPERTIES_FLAG_IS_SLOPED | 2,
    SLOPE_IS_IRREGULAR_FLAG,
    SLOPE_IS_IRREGULAR_FLAG,
    FOOTPATH_PROPERTIES_FLAG_IS_SLOPED | 3,
    RAISE_FOOTPATH_FLAG,
    SLOPE_IS_IRREGULAR_FLAG,
    FOOTPATH_PROPERTIES_FLAG_IS_SLOPED | 1,
    SLOPE_IS_IRREGULAR_FLAG,
    RAISE_FOOTPATH_FLAG,
    FOOTPATH_PROPERTIES_FLAG_IS_SLOPED | 0,
    RAISE_FOOTPATH_FLAG,
    RAISE_FOOTPATH_FLAG,
    SLOPE_IS_IRREGULAR_FLAG,
};

/** rct2: 0x0098D7E0 */
static constexpr const uint8_t ConstructionPreviewImages[][4] = {
    { 5, 10, 5, 10 },   // Flat
    { 16, 17, 18, 19 }, // Upwards
    { 18, 19, 16, 17 }, // Downwards
};

static void window_footpath_mousedown_direction(int32_t direction);
static void window_footpath_mousedown_slope(int32_t slope);
static void window_footpath_show_footpath_types_dialog(rct_window* w, rct_widget* widget, bool showQueues);
static void window_footpath_set_provisional_path_at_point(int32_t x, int32_t y);
static void window_footpath_set_selection_start_bridge_at_point(int32_t screenX, int32_t screenY);
static void window_footpath_place_path_at_point(int32_t x, int32_t y);
static void window_footpath_start_bridge_at_point(int32_t screenX, int32_t screenY);
static void window_footpath_construct();
static void window_footpath_remove();
static void window_footpath_set_enabled_and_pressed_widgets();
static void footpath_get_next_path_info(int32_t* type, int32_t* x, int32_t* y, int32_t* z, int32_t* slope);
static bool footpath_select_default();

/**
 *
 *  rct2: 0x006A7C43
 */
rct_window* window_footpath_open()
{
    // If a restricted path was selected when the game is no longer in Sandbox mode, reset it
    PathSurfaceEntry* pathEntry = get_path_surface_entry(gFootpathSelectedId);
    if (pathEntry != nullptr && (pathEntry->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) && !gCheatsSandboxMode)
    {
        pathEntry = nullptr;
    }

    // Select the default path if we don't have one
    if (pathEntry == nullptr)
    {
        if (!footpath_select_default())
        {
            // No path objects to select from, don't open window
            return nullptr;
        }
    }

    // Check if window is already open
    rct_window* window = window_bring_to_front_by_class(WC_FOOTPATH);
    if (window != nullptr)
    {
        return window;
    }

    window = window_create(0, 29, 106, 381, &window_footpath_events, WC_FOOTPATH, 0);
    window->widgets = window_footpath_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_FOOTPATH_TYPE) | (1 << WIDX_QUEUELINE_TYPE)
        | (1 << WIDX_DIRECTION_NW) | (1 << WIDX_DIRECTION_NE) | (1 << WIDX_DIRECTION_SW) | (1 << WIDX_DIRECTION_SE)
        | (1 << WIDX_SLOPEDOWN) | (1 << WIDX_LEVEL) | (1 << WIDX_SLOPEUP) | (1 << WIDX_CONSTRUCT) | (1 << WIDX_REMOVE)
        | (1 << WIDX_CONSTRUCT_ON_LAND) | (1 << WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL);

    window_init_scroll_widgets(window);
    window_push_others_right(window);
    show_gridlines();

    tool_cancel();
    gFootpathConstructionMode = PATH_CONSTRUCTION_MODE_LAND;
    tool_set(window, WIDX_CONSTRUCT_ON_LAND, TOOL_PATH_DOWN);
    input_set_flag(INPUT_FLAG_6, true);
    _footpathErrorOccured = false;
    window_footpath_set_enabled_and_pressed_widgets();

    return window;
}

/**
 *
 *  rct2: 0x006A852F
 */
static void window_footpath_close(rct_window* w)
{
    footpath_provisional_update();
    viewport_set_visibility(0);
    map_invalidate_map_selection_tiles();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    window_invalidate_by_class(WC_TOP_TOOLBAR);
    hide_gridlines();
}

/**
 *
 *  rct2: 0x006A7E92
 */
static void window_footpath_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_CONSTRUCT:
            window_footpath_construct();
            break;
        case WIDX_REMOVE:
            window_footpath_remove();
            break;
        case WIDX_CONSTRUCT_ON_LAND:
            if (gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_LAND)
            {
                break;
            }

            _window_footpath_cost = MONEY32_UNDEFINED;
            tool_cancel();
            footpath_provisional_update();
            map_invalidate_map_selection_tiles();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            gFootpathConstructionMode = PATH_CONSTRUCTION_MODE_LAND;
            tool_set(w, WIDX_CONSTRUCT_ON_LAND, TOOL_PATH_DOWN);
            input_set_flag(INPUT_FLAG_6, true);
            _footpathErrorOccured = false;
            window_footpath_set_enabled_and_pressed_widgets();
            break;
        case WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL:
            if (gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL)
            {
                break;
            }

            _window_footpath_cost = MONEY32_UNDEFINED;
            tool_cancel();
            footpath_provisional_update();
            map_invalidate_map_selection_tiles();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            gFootpathConstructionMode = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL;
            tool_set(w, WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL, TOOL_CROSSHAIR);
            input_set_flag(INPUT_FLAG_6, true);
            _footpathErrorOccured = false;
            window_footpath_set_enabled_and_pressed_widgets();
            break;
    }
}

/**
 *
 *  rct2: 0x006A7EC5
 */
static void window_footpath_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_FOOTPATH_TYPE:
            window_footpath_show_footpath_types_dialog(w, widget, false);
            break;
        case WIDX_QUEUELINE_TYPE:
            window_footpath_show_footpath_types_dialog(w, widget, true);
            break;
        case WIDX_DIRECTION_NW:
            window_footpath_mousedown_direction(0);
            break;
        case WIDX_DIRECTION_NE:
            window_footpath_mousedown_direction(1);
            break;
        case WIDX_DIRECTION_SW:
            window_footpath_mousedown_direction(2);
            break;
        case WIDX_DIRECTION_SE:
            window_footpath_mousedown_direction(3);
            break;
        case WIDX_SLOPEDOWN:
            window_footpath_mousedown_slope(6);
            break;
        case WIDX_LEVEL:
            window_footpath_mousedown_slope(0);
            break;
        case WIDX_SLOPEUP:
            window_footpath_mousedown_slope(2);
            break;
    }
}

/**
 *
 *  rct2: 0x006A7F18
 */
static void window_footpath_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex == WIDX_FOOTPATH_TYPE)
    {
        gFootpathSelectedType = SELECTED_PATH_TYPE_NORMAL;
    }
    else if (widgetIndex == WIDX_QUEUELINE_TYPE)
    {
        gFootpathSelectedType = SELECTED_PATH_TYPE_QUEUE;
    }
    else
    {
        return;
    }

    // Get path id
    int32_t pathId = dropdownIndex;
    if (pathId == -1)
    {
        pathId = gFootpathSelectedId;
    }
    else
    {
        bool showEditorPaths = ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode);

        int32_t i = 0, j = 0;
        for (; i < MAX_PATH_OBJECTS; i++)
        {
            PathSurfaceEntry* pathType = get_path_surface_entry(i);
            if (pathType == nullptr)
            {
                continue;
            }
            if ((pathType->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) && !showEditorPaths)
            {
                continue;
            }

            if (j == pathId)
            {
                break;
            }
            j++;
        }
        pathId = i;
    }

    // Set selected path id
    gFootpathSelectedId = pathId;
    footpath_provisional_update();
    _window_footpath_cost = MONEY32_UNDEFINED;
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006A8032
 */
static void window_footpath_toolupdate(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
    {
        window_footpath_set_provisional_path_at_point(x, y);
    }
    else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
    {
        window_footpath_set_selection_start_bridge_at_point(x, y);
    }
}

/**
 *
 *  rct2: 0x006A8047
 */
static void window_footpath_tooldown(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
    {
        window_footpath_place_path_at_point(x, y);
    }
    else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
    {
        window_footpath_start_bridge_at_point(x, y);
    }
}

/**
 *
 *  rct2: 0x006A8067
 */
static void window_footpath_tooldrag(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
    {
        window_footpath_place_path_at_point(x, y);
    }
}

/**
 *
 *  rct2: 0x006A8066
 */
static void window_footpath_toolup(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
    {
        _footpathErrorOccured = false;
    }
}

/**
 *
 *  rct2: 0x006A7760
 */
static void window_footpath_update_provisional_path_for_bridge_mode(rct_window* w)
{
    int32_t type, x, y, z, slope;

    if (gFootpathConstructionMode != PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
    {
        return;
    }

    // Recheck area for construction. Set by ride_construction window
    if (gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_2)
    {
        footpath_provisional_remove();
        gFootpathProvisionalFlags &= ~PROVISIONAL_PATH_FLAG_2;
    }

    // Update provisional bridge mode path
    if (!(gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_1))
    {
        footpath_get_next_path_info(&type, &x, &y, &z, &slope);
        _window_footpath_cost = footpath_provisional_set(type, x, y, z, slope);
        widget_invalidate(w, WIDX_CONSTRUCT);
    }

    // Update little directional arrow on provisional bridge mode path
    if (--_window_footpath_provisional_path_arrow_timer < 0)
    {
        _window_footpath_provisional_path_arrow_timer = 5;
        gFootpathProvisionalFlags ^= PROVISIONAL_PATH_FLAG_SHOW_ARROW;
        footpath_get_next_path_info(&type, &x, &y, &z, &slope);
        gMapSelectArrowPosition.x = x;
        gMapSelectArrowPosition.y = y;
        gMapSelectArrowPosition.z = z * 8;
        gMapSelectArrowDirection = gFootpathConstructDirection;
        if (gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_SHOW_ARROW)
        {
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
        }
        else
        {
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
        }
        map_invalidate_tile_full(x, y);
    }
}

/**
 *
 *  rct2: 0x006A84BB
 */
static void window_footpath_update(rct_window* w)
{
    widget_invalidate(w, WIDX_CONSTRUCT);
    window_footpath_update_provisional_path_for_bridge_mode(w);

    // #2502: The camera might have changed rotation, so we need to update which directional buttons are pressed
    uint8_t currentRotation = get_current_rotation();
    if (_lastUpdatedCameraRotation != currentRotation)
    {
        _lastUpdatedCameraRotation = currentRotation;
        window_footpath_set_enabled_and_pressed_widgets();
    }

    // Check tool
    if (gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_LAND)
    {
        if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
        {
            window_close(w);
        }
        else if (gCurrentToolWidget.window_classification != WC_FOOTPATH)
        {
            window_close(w);
        }
        else if (gCurrentToolWidget.widget_index != WIDX_CONSTRUCT_ON_LAND)
        {
            window_close(w);
        }
    }
    else if (gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL)
    {
        if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
        {
            window_close(w);
        }
        else if (gCurrentToolWidget.window_classification != WC_FOOTPATH)
        {
            window_close(w);
        }
        else if (gCurrentToolWidget.widget_index != WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
        {
            window_close(w);
        }
    }
}

/**
 *
 *  rct2: 0x006A7D1C
 */
static void window_footpath_invalidate(rct_window* w)
{
    int32_t selectedPath;
    PathSurfaceEntry* pathType;

    // Press / unpress footpath and queue type buttons
    w->pressed_widgets &= ~(1 << WIDX_FOOTPATH_TYPE);
    w->pressed_widgets &= ~(1 << WIDX_QUEUELINE_TYPE);
    w->pressed_widgets |= gFootpathSelectedType == SELECTED_PATH_TYPE_NORMAL ? (1 << WIDX_FOOTPATH_TYPE)
                                                                             : (1 << WIDX_QUEUELINE_TYPE);

    // Enable / disable construct button
    window_footpath_widgets[WIDX_CONSTRUCT].type = gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL
        ? WWT_IMGBTN
        : WWT_EMPTY;

    // Set footpath and queue type button images
    selectedPath = gFootpathSelectedId;
    pathType = get_path_surface_entry(selectedPath);

    int32_t pathImage = pathType->preview;
    // Editor-only paths might lack a queue image
    int32_t queueImage = (pathType->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) ? pathImage : pathImage + 1;
    window_footpath_widgets[WIDX_FOOTPATH_TYPE].image = pathImage;
    window_footpath_widgets[WIDX_QUEUELINE_TYPE].image = queueImage;
}

/**
 *
 *  rct2: 0x006A7D8B
 */
static void window_footpath_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);

    if (!(w->disabled_widgets & (1 << WIDX_CONSTRUCT)))
    {
        // Get construction image
        uint8_t direction = (gFootpathConstructDirection + get_current_rotation()) % 4;
        uint8_t slope = 0;
        if (gFootpathConstructSlope == 2)
        {
            slope = TILE_ELEMENT_SLOPE_N_CORNER_UP;
        }
        else if (gFootpathConstructSlope == 6)
        {
            slope = TILE_ELEMENT_SLOPE_E_CORNER_UP;
        }
        int32_t image = ConstructionPreviewImages[slope][direction];

        int32_t selectedPath = gFootpathSelectedId;
        PathSurfaceEntry* pathType = get_path_surface_entry(selectedPath);
        if (gFootpathSelectedType == SELECTED_PATH_TYPE_NORMAL)
        {
            image += pathType->image;
        }
        else
        {
            image += pathType->queue_image;
        }

        // Draw construction image
        int32_t x = w->x + (window_footpath_widgets[WIDX_CONSTRUCT].left + window_footpath_widgets[WIDX_CONSTRUCT].right) / 2;
        int32_t y = w->y + window_footpath_widgets[WIDX_CONSTRUCT].bottom - 60;
        gfx_draw_sprite(dpi, image, x, y, 0);

        // Draw build this... label
        x = w->x + (window_footpath_widgets[WIDX_CONSTRUCT].left + window_footpath_widgets[WIDX_CONSTRUCT].right) / 2;
        y = w->y + window_footpath_widgets[WIDX_CONSTRUCT].bottom - 23;
        gfx_draw_string_centred(dpi, STR_BUILD_THIS, x, y, COLOUR_BLACK, nullptr);
    }

    // Draw cost
    int32_t x = w->x + (window_footpath_widgets[WIDX_CONSTRUCT].left + window_footpath_widgets[WIDX_CONSTRUCT].right) / 2;
    int32_t y = w->y + window_footpath_widgets[WIDX_CONSTRUCT].bottom - 12;
    if (_window_footpath_cost != MONEY32_UNDEFINED)
    {
        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            gfx_draw_string_centred(dpi, STR_COST_LABEL, x, y, COLOUR_BLACK, &_window_footpath_cost);
        }
    }
}

/**
 *
 *  rct2: 0x006A7F88
 */
static void window_footpath_show_footpath_types_dialog(rct_window* w, rct_widget* widget, bool showQueues)
{
    int32_t i, numPathTypes, image;
    PathSurfaceEntry* pathType;

    numPathTypes = 0;
    // If the game is in sandbox mode, also show paths that are normally restricted to the scenario editor
    bool showEditorPaths = ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode);

    for (i = 0; i < MAX_PATH_OBJECTS; i++)
    {
        pathType = get_path_surface_entry(i);
        if (pathType == nullptr)
        {
            continue;
        }
        if ((pathType->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) && !showEditorPaths)
        {
            continue;
        }

        image = pathType->preview;
        // Editor-only paths usually lack queue images. In this case, use the main path image
        if (showQueues && !(pathType->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR))
        {
            image++;
        }

        gDropdownItemsFormat[numPathTypes] = STR_NONE;
        gDropdownItemsArgs[numPathTypes] = image;
        numPathTypes++;
    }

    window_dropdown_show_image(
        w->x + widget->left, w->y + widget->top, widget->bottom - widget->top + 1, w->colours[1], 0, numPathTypes, 47, 36,
        gAppropriateImageDropdownItemsPerRow[numPathTypes]);
}

/**
 *
 *  rct2: 0x006A8111 0x006A8135 0x006A815C 0x006A8183
 */
static void window_footpath_mousedown_direction(int32_t direction)
{
    footpath_provisional_update();
    gFootpathConstructDirection = (direction - get_current_rotation()) & 3;
    _window_footpath_cost = MONEY32_UNDEFINED;
    window_footpath_set_enabled_and_pressed_widgets();
}

/**
 *
 *  rct2: 0x006A81AA 0x006A81C5 0x006A81E0
 */
static void window_footpath_mousedown_slope(int32_t slope)
{
    footpath_provisional_update();
    gFootpathConstructSlope = slope;
    _window_footpath_cost = MONEY32_UNDEFINED;
    window_footpath_set_enabled_and_pressed_widgets();
}

/**
 *
 *  rct2: 0x006A81FB
 */
static void window_footpath_set_provisional_path_at_point(int32_t x, int32_t y)
{
    map_invalidate_selection_rect();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

    int32_t interactionType{};
    TileElement* tileElement{};
    LocationXY16 mapCoord = {};
    get_map_coordinates_from_pos(
        x, y, VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN, &mapCoord.x, &mapCoord.y,
        &interactionType, &tileElement, nullptr);
    x = mapCoord.x;
    y = mapCoord.y;

    if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE || tileElement == nullptr)
    {
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
        footpath_provisional_update();
    }
    else
    {
        // Check for change
        if ((gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_1) && gFootpathProvisionalPosition.x == x
            && gFootpathProvisionalPosition.y == y && gFootpathProvisionalPosition.z == tileElement->base_height)
        {
            return;
        }

        // Set map selection
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        gMapSelectPositionA.x = x;
        gMapSelectPositionA.y = y;
        gMapSelectPositionB.x = x;
        gMapSelectPositionB.y = y;

        footpath_provisional_update();

        // Set provisional path
        int32_t slope = 0;
        switch (interactionType)
        {
            case VIEWPORT_INTERACTION_ITEM_TERRAIN:
            {
                auto surfaceElement = tileElement->AsSurface();
                if (surfaceElement != nullptr)
                {
                    slope = DefaultPathSlope[surfaceElement->GetSlope() & TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK];
                }
                break;
            }
            case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
            {
                auto pathElement = tileElement->AsPath();
                if (pathElement != nullptr)
                {
                    slope = pathElement->GetSlopeDirection();
                    if (pathElement->IsSloped())
                    {
                        slope |= FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
                    }
                }
                break;
            }
        }
        uint8_t z = tileElement->base_height;
        if (slope & RAISE_FOOTPATH_FLAG)
        {
            slope &= ~RAISE_FOOTPATH_FLAG;
            z += 2;
        }
        int32_t pathType = (gFootpathSelectedType << 7) + (gFootpathSelectedId & 0xFF);

        _window_footpath_cost = footpath_provisional_set(pathType, x, y, z, slope);
        window_invalidate_by_class(WC_FOOTPATH);
    }
}

/**
 *
 *  rct2: 0x006A8388
 */
static void window_footpath_set_selection_start_bridge_at_point(int32_t screenX, int32_t screenY)
{
    int32_t x, y, direction;
    TileElement* tileElement;

    map_invalidate_selection_rect();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

    footpath_bridge_get_info_from_pos(screenX, screenY, &x, &y, &direction, &tileElement);
    if (x == LOCATION_NULL)
    {
        return;
    }

    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectType = MAP_SELECT_TYPE_FULL;
    gMapSelectPositionA.x = x;
    gMapSelectPositionB.x = x;
    gMapSelectPositionA.y = y;
    gMapSelectPositionB.y = y;

    gMapSelectArrowDirection = direction;
    gMapSelectArrowPosition.x = x;
    gMapSelectArrowPosition.y = y;

    int32_t z = tileElement->base_height;

    if (tileElement->GetType() == TILE_ELEMENT_TYPE_SURFACE)
    {
        uint8_t slope = tileElement->AsSurface()->GetSlope();
        if (slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
        {
            z += 2;
        } // Add 2 for a slope
        if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
            z += 2; // Add another 2 for a steep slope
    }

    gMapSelectArrowPosition.z = z << 3;

    map_invalidate_selection_rect();
}

/**
 *
 *  rct2: 0x006A82C5
 */
static void window_footpath_place_path_at_point(int32_t x, int32_t y)
{
    int32_t interactionType, currentType, selectedType, z;
    TileElement* tileElement;

    if (_footpathErrorOccured)
    {
        return;
    }

    footpath_provisional_update();

    LocationXY16 mapCoord = {};
    get_map_coordinates_from_pos(
        x, y, VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN, &mapCoord.x, &mapCoord.y,
        &interactionType, &tileElement, nullptr);
    x = mapCoord.x;
    y = mapCoord.y;

    if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE)
    {
        return;
    }

    // Set path
    currentType = 0;
    switch (interactionType)
    {
        case VIEWPORT_INTERACTION_ITEM_TERRAIN:
            currentType = DefaultPathSlope[tileElement->AsSurface()->GetSlope() & TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK];
            break;
        case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
            currentType = tileElement->AsPath()->GetSlopeDirection();
            if (tileElement->AsPath()->IsSloped())
            {
                currentType |= FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
            }
            break;
    }
    z = tileElement->base_height;
    if (currentType & RAISE_FOOTPATH_FLAG)
    {
        currentType &= ~RAISE_FOOTPATH_FLAG;
        z += 2;
    }
    selectedType = (gFootpathSelectedType << 7) + (gFootpathSelectedId & 0xFF);

    // Try and place path
    gGameCommandErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
    auto footpathPlaceAction = FootpathPlaceAction({ x, y, z * 8 }, currentType, selectedType);
    footpathPlaceAction.SetCallback([](const GameAction* ga, const GameActionResult* result) {
        if (result->Error == GA_ERROR::OK)
        {
            // Don't play sound if it is no cost to prevent multiple sounds. TODO: make this work in no money scenarios
            if (result->Cost != 0)
            {
                audio_play_sound_at_location(SOUND_PLACE_ITEM, result->Position.x, result->Position.y, result->Position.z);
            }
        }
        else
        {
            _footpathErrorOccured = true;
        }
    });
    GameActions::Execute(&footpathPlaceAction);
}

/**
 *
 *  rct2: 0x006A840F
 */
static void window_footpath_start_bridge_at_point(int32_t screenX, int32_t screenY)
{
    int32_t x, y, z, direction;
    TileElement* tileElement;

    footpath_bridge_get_info_from_pos(screenX, screenY, &x, &y, &direction, &tileElement);
    if (x == LOCATION_NULL)
    {
        return;
    }

    if (tileElement->GetType() == TILE_ELEMENT_TYPE_SURFACE)
    {
        // If we start the path on a slope, the arrow is slightly raised, so we
        // expect the path to be slightly raised as well.
        uint8_t slope = tileElement->AsSurface()->GetSlope();
        z = tileElement->base_height;
        if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
        {
            // Steep diagonal slope
            z += 4;
        }
        else if (slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
        {
            // Normal slope
            z += 2;
        }
    }
    else
    {
        z = tileElement->base_height;
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            if (tileElement->AsPath()->IsSloped())
            {
                if (direction == (tileElement->AsPath()->GetSlopeDirection()))
                {
                    z += 2;
                }
            }
        }
    }

    tool_cancel();
    gFootpathConstructFromPosition.x = x;
    gFootpathConstructFromPosition.y = y;
    gFootpathConstructFromPosition.z = z * 8;
    gFootpathConstructDirection = direction;
    gFootpathProvisionalFlags = 0;
    _window_footpath_provisional_path_arrow_timer = 0;
    gFootpathConstructSlope = 0;
    gFootpathConstructionMode = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL;
    gFootpathConstructValidDirections = 255;
    window_footpath_set_enabled_and_pressed_widgets();
}

/**
 * Construct a piece of footpath while in bridge building mode.
 *  rct2: 0x006A79B7
 */
static void window_footpath_construct()
{
    _window_footpath_cost = MONEY32_UNDEFINED;
    footpath_provisional_update();

    int32_t type, x, y, z, slope;
    footpath_get_next_path_info(&type, &x, &y, &z, &slope);

    gGameCommandErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
    auto footpathPlaceAction = FootpathPlaceAction({ x, y, z * 8 }, slope, type, gFootpathConstructDirection);
    footpathPlaceAction.SetCallback([=](const GameAction* ga, const GameActionResult* result) {
        if (result->Error == GA_ERROR::OK)
        {
            audio_play_sound_at_location(SOUND_PLACE_ITEM, result->Position.x, result->Position.y, result->Position.z);

            if (gFootpathConstructSlope == 0)
            {
                gFootpathConstructValidDirections = 0xFF;
            }
            else
            {
                gFootpathConstructValidDirections = gFootpathConstructDirection;
            }

            if (gFootpathGroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                viewport_set_visibility(1);
            }

            // If we have just built an upwards slope, the next path to construct is
            // a bit higher. Note that the z returned by footpath_get_next_path_info
            // already is lowered if we are building a downwards slope.
            if (gFootpathConstructSlope == 2)
            {
                gFootpathConstructFromPosition.z = (z + 2) * 8;
            }
            else
            {
                gFootpathConstructFromPosition.z = z * 8;
            }

            gFootpathConstructFromPosition.x = x;
            gFootpathConstructFromPosition.y = y;
        }
        window_footpath_set_enabled_and_pressed_widgets();
    });
    GameActions::Execute(&footpathPlaceAction);
}

/**
 *
 *  rct2: 0x006A78EF
 */
static void footpath_remove_tile_element(TileElement* tileElement)
{
    int32_t x, y, z;

    z = tileElement->base_height;
    if (tileElement->AsPath()->IsSloped())
    {
        uint8_t slopeDirection = tileElement->AsPath()->GetSlopeDirection();
        slopeDirection = direction_reverse(slopeDirection);
        if (slopeDirection == gFootpathConstructDirection)
        {
            z += 2;
        }
    }

    // Find a connected edge
    int32_t edge = direction_reverse(gFootpathConstructDirection);
    if (!(tileElement->AsPath()->GetEdges() & (1 << edge)))
    {
        edge = (edge + 1) & 3;
        if (!(tileElement->AsPath()->GetEdges() & (1 << edge)))
        {
            edge = (edge + 2) & 3;
            if (!(tileElement->AsPath()->GetEdges() & (1 << edge)))
            {
                edge = (edge - 1) & 3;
                if (!(tileElement->AsPath()->GetEdges() & (1 << edge)))
                {
                    edge = direction_reverse(edge);
                }
            }
        }
    }

    // Remove path
    footpath_remove(
        gFootpathConstructFromPosition.x, gFootpathConstructFromPosition.y, tileElement->base_height, GAME_COMMAND_FLAG_APPLY);

    // Move selection
    edge = direction_reverse(edge);
    x = gFootpathConstructFromPosition.x - CoordsDirectionDelta[edge].x;
    y = gFootpathConstructFromPosition.y - CoordsDirectionDelta[edge].y;
    gFootpathConstructFromPosition.x = x;
    gFootpathConstructFromPosition.y = y;
    gFootpathConstructFromPosition.z = z << 3;
    gFootpathConstructDirection = edge;
    gFootpathConstructValidDirections = 255;
}

/**
 *
 *  rct2: 0x006A7873
 */
static TileElement* footpath_get_tile_element_to_remove()
{
    TileElement* tileElement;
    int32_t x, y, z, zLow;

    x = gFootpathConstructFromPosition.x / 32;
    y = gFootpathConstructFromPosition.y / 32;
    if (x >= 256 || y >= 256)
    {
        return nullptr;
    }

    z = (gFootpathConstructFromPosition.z >> 3) & 0xFF;
    zLow = z - 2;

    tileElement = map_get_first_element_at(x, y);
    do
    {
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            if (tileElement->base_height == z)
            {
                if (tileElement->AsPath()->IsSloped())
                {
                    if (direction_reverse(tileElement->AsPath()->GetSlopeDirection()) != gFootpathConstructDirection)
                    {
                        continue;
                    }
                }

                return tileElement;
            }
            else if (tileElement->base_height == zLow)
            {
                if (!tileElement->AsPath()->IsSloped())
                {
                    if ((tileElement->AsPath()->GetSlopeDirection()) == gFootpathConstructDirection)
                    {
                        continue;
                    }
                }

                return tileElement;
            }
        }
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

/**
 *
 *  rct2: 0x006A7863
 */
static void window_footpath_remove()
{
    TileElement* tileElement;

    _window_footpath_cost = MONEY32_UNDEFINED;
    footpath_provisional_update();

    tileElement = footpath_get_tile_element_to_remove();
    if (tileElement != nullptr)
    {
        footpath_remove_tile_element(tileElement);
    }

    window_footpath_set_enabled_and_pressed_widgets();
}

/**
 *
 *  rct2: 0x006A855C
 */
static void window_footpath_set_enabled_and_pressed_widgets()
{
    rct_window* w = window_find_by_class(WC_FOOTPATH);
    if (w == nullptr)
    {
        return;
    }

    if (gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
    {
        map_invalidate_map_selection_tiles();
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        gMapSelectFlags |= MAP_SELECT_FLAG_GREEN;

        int32_t direction = gFootpathConstructDirection;
        gMapSelectionTiles[0].x = gFootpathConstructFromPosition.x + CoordsDirectionDelta[direction].x;
        gMapSelectionTiles[0].y = gFootpathConstructFromPosition.y + CoordsDirectionDelta[direction].y;
        gMapSelectionTiles[1].x = -1;
        map_invalidate_map_selection_tiles();
    }

    uint64_t pressedWidgets = w->pressed_widgets
        & ~((1LL << WIDX_DIRECTION_NW) | (1LL << WIDX_DIRECTION_NE) | (1LL << WIDX_DIRECTION_SW) | (1LL << WIDX_DIRECTION_SE)
            | (1LL << WIDX_SLOPEDOWN) | (1LL << WIDX_LEVEL) | (1LL << WIDX_SLOPEUP));
    uint64_t disabledWidgets = 0;
    int32_t currentRotation = get_current_rotation();
    if (gFootpathConstructionMode >= PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
    {
        // Set pressed directional widget
        int32_t direction = (gFootpathConstructDirection + currentRotation) & 3;
        pressedWidgets |= (1LL << (WIDX_DIRECTION_NW + direction));

        // Set pressed slope widget
        int32_t slope = gFootpathConstructSlope;
        if (slope == TILE_ELEMENT_SLOPE_SE_SIDE_UP)
        {
            pressedWidgets |= (1 << WIDX_SLOPEDOWN);
        }
        else if (slope == TILE_ELEMENT_SLOPE_FLAT)
        {
            pressedWidgets |= (1 << WIDX_LEVEL);
        }
        else
        {
            pressedWidgets |= (1 << WIDX_SLOPEUP);
        }

        // Enable / disable directional widgets
        direction = gFootpathConstructValidDirections;
        if (direction != 255)
        {
            disabledWidgets |= (1 << WIDX_DIRECTION_NW) | (1 << WIDX_DIRECTION_NE) | (1 << WIDX_DIRECTION_SW)
                | (1 << WIDX_DIRECTION_SE);

            direction = (direction + currentRotation) & 3;
            disabledWidgets &= ~(1 << (WIDX_DIRECTION_NW + direction));
        }
    }
    else
    {
        // Disable all bridge mode widgets
        disabledWidgets |= (1 << WIDX_DIRECTION_GROUP) | (1 << WIDX_DIRECTION_NW) | (1 << WIDX_DIRECTION_NE)
            | (1 << WIDX_DIRECTION_SW) | (1 << WIDX_DIRECTION_SE) | (1 << WIDX_SLOPE_GROUP) | (1 << WIDX_SLOPEDOWN)
            | (1 << WIDX_LEVEL) | (1 << WIDX_SLOPEUP) | (1 << WIDX_CONSTRUCT) | (1 << WIDX_REMOVE);
    }

    w->pressed_widgets = pressedWidgets;
    w->disabled_widgets = disabledWidgets;
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006A7B20
 */
static void footpath_get_next_path_info(int32_t* type, int32_t* x, int32_t* y, int32_t* z, int32_t* slope)
{
    int32_t direction;

    direction = gFootpathConstructDirection;
    *x = gFootpathConstructFromPosition.x + CoordsDirectionDelta[direction].x;
    *y = gFootpathConstructFromPosition.y + CoordsDirectionDelta[direction].y;
    *z = gFootpathConstructFromPosition.z / 8;
    *type = (gFootpathSelectedType << 7) + (gFootpathSelectedId & 0xFF);
    *slope = TILE_ELEMENT_SLOPE_FLAT;
    if (gFootpathConstructSlope != 0)
    {
        *slope = gFootpathConstructDirection | TILE_ELEMENT_SLOPE_S_CORNER_UP;
        if (gFootpathConstructSlope != 2)
        {
            *z -= 2;
            *slope ^= TILE_ELEMENT_SLOPE_E_CORNER_UP;
        }
    }
}

static bool footpath_select_default()
{
    // Select first available footpath
    int32_t footpathId = -1;
    for (int32_t i = 0; i < object_entry_group_counts[OBJECT_TYPE_PATHS]; i++)
    {
        PathSurfaceEntry* pathEntry = get_path_surface_entry(i);
        if (pathEntry != nullptr)
        {
            footpathId = i;

            // Prioritise non-restricted path
            if (!(pathEntry->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR))
            {
                break;
            }
        }
    }
    if (footpathId == -1)
    {
        return false;
    }
    else
    {
        gFootpathSelectedId = footpathId;
        return true;
    }
}
