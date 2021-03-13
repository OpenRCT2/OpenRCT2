/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include <openrct2/actions/FootpathPlaceAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/platform/platform.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Surface.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_FOOTPATHS;
static constexpr const int32_t WH = 381;
static constexpr const int32_t WW = 106;
static constexpr const uint16_t ARROW_PULSE_DURATION = 200;

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
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),

    // Type group
    MakeWidget({ 3,  17}, {100, 55}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_TYPE                                                                          ),
    MakeWidget({ 6,  30}, { 47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_FOOTPATH_TIP                               ),
    MakeWidget({53,  30}, { 47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_QUEUE_LINE_PATH_TIP                        ),

    // Direction group
    MakeWidget({ 3,  75}, {100, 77}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_DIRECTION                                                                     ),
    MakeWidget({53,  87}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_NE,     STR_DIRECTION_TIP                              ),
    MakeWidget({53, 116}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_SE,     STR_DIRECTION_TIP                              ),
    MakeWidget({ 8, 116}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_SW,     STR_DIRECTION_TIP                              ),
    MakeWidget({ 8,  87}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_NW,     STR_DIRECTION_TIP                              ),

    // Slope group
    MakeWidget({ 3, 155}, {100, 41}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_SLOPE                                                                         ),
    MakeWidget({17, 167}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_DOWN,  STR_SLOPE_DOWN_TIP                             ),
    MakeWidget({41, 167}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_LEVEL, STR_LEVEL_TIP                                  ),
    MakeWidget({65, 167}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_UP,    STR_SLOPE_UP_TIP                               ),
    MakeWidget({ 8, 202}, { 90, 90}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_CONSTRUCT_THE_SELECTED_FOOTPATH_SECTION_TIP),
    MakeWidget({30, 295}, { 46, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_DEMOLISH_CURRENT_SECTION,      STR_REMOVE_PREVIOUS_FOOTPATH_SECTION_TIP       ),

    // Mode group
    MakeWidget({ 3, 321}, {100, 54}, WindowWidgetType::Groupbox, WindowColour::Primary                                                                                      ),
    MakeWidget({13, 332}, { 36, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_FOOTPATH_LAND,    STR_CONSTRUCT_FOOTPATH_ON_LAND_TIP             ),
    MakeWidget({57, 332}, { 36, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_FOOTPATH_BRIDGE,  STR_CONSTRUCT_BRIDGE_OR_TUNNEL_FOOTPATH_TIP    ),
    {WIDGETS_END},
};

static void window_footpath_close(rct_window * w);
static void window_footpath_mouseup(rct_window * w, rct_widgetindex widgetIndex);
static void window_footpath_mousedown(rct_window * w, rct_widgetindex widgetIndex, rct_widget * widget);
static void window_footpath_dropdown(rct_window * w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_footpath_update(rct_window * w);
static void window_footpath_toolupdate(rct_window * w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_footpath_tooldown(rct_window * w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_footpath_tooldrag(rct_window * w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_footpath_toolup(rct_window * w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_footpath_invalidate(rct_window * w);
static void window_footpath_paint(rct_window * w, rct_drawpixelinfo * dpi);

static rct_window_event_list window_footpath_events([](auto& events)
{
    events.close = &window_footpath_close;
    events.mouse_up = &window_footpath_mouseup;
    events.mouse_down = &window_footpath_mousedown;
    events.dropdown = &window_footpath_dropdown;
    events.update = &window_footpath_update;
    events.tool_update = &window_footpath_toolupdate;
    events.tool_down = &window_footpath_tooldown;
    events.tool_drag = &window_footpath_tooldrag;
    events.tool_up = &window_footpath_toolup;
    events.invalidate = &window_footpath_invalidate;
    events.paint = &window_footpath_paint;
});
// clang-format on

static money32 _window_footpath_cost;
static uint32_t _footpathConstructionNextArrowPulse = 0;
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
static void window_footpath_set_provisional_path_at_point(const ScreenCoordsXY& screenCoords);
static void window_footpath_set_selection_start_bridge_at_point(const ScreenCoordsXY& screenCoords);
static void window_footpath_place_path_at_point(const ScreenCoordsXY& screenCoords);
static void window_footpath_start_bridge_at_point(const ScreenCoordsXY& screenCoords);
static void window_footpath_construct();
static void window_footpath_remove();
static void window_footpath_set_enabled_and_pressed_widgets();
static void footpath_get_next_path_info(int32_t* type, CoordsXYZ& footpathLoc, int32_t* slope);
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

    window = WindowCreate(ScreenCoordsXY(0, 29), 106, 381, &window_footpath_events, WC_FOOTPATH, 0);
    window->widgets = window_footpath_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_FOOTPATH_TYPE) | (1 << WIDX_QUEUELINE_TYPE)
        | (1 << WIDX_DIRECTION_NW) | (1 << WIDX_DIRECTION_NE) | (1 << WIDX_DIRECTION_SW) | (1 << WIDX_DIRECTION_SE)
        | (1 << WIDX_SLOPEDOWN) | (1 << WIDX_LEVEL) | (1 << WIDX_SLOPEUP) | (1 << WIDX_CONSTRUCT) | (1 << WIDX_REMOVE)
        | (1 << WIDX_CONSTRUCT_ON_LAND) | (1 << WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL);

    WindowInitScrollWidgets(window);
    window_push_others_right(window);
    show_gridlines();

    tool_cancel();
    gFootpathConstructionMode = PATH_CONSTRUCTION_MODE_LAND;
    tool_set(window, WIDX_CONSTRUCT_ON_LAND, Tool::PathDown);
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
            tool_set(w, WIDX_CONSTRUCT_ON_LAND, Tool::PathDown);
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
            tool_set(w, WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL, Tool::Crosshair);
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
    w->Invalidate();
}

/**
 *
 *  rct2: 0x006A8032
 */
static void window_footpath_toolupdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
    {
        window_footpath_set_provisional_path_at_point(screenCoords);
    }
    else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
    {
        window_footpath_set_selection_start_bridge_at_point(screenCoords);
    }
}

/**
 *
 *  rct2: 0x006A8047
 */
static void window_footpath_tooldown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
    {
        window_footpath_place_path_at_point(screenCoords);
    }
    else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
    {
        window_footpath_start_bridge_at_point(screenCoords);
    }
}

/**
 *
 *  rct2: 0x006A8067
 */
static void window_footpath_tooldrag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
    {
        window_footpath_place_path_at_point(screenCoords);
    }
}

/**
 *
 *  rct2: 0x006A8066
 */
static void window_footpath_toolup(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
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
    int32_t type, slope;

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
        CoordsXYZ footpathLoc;
        footpath_get_next_path_info(&type, footpathLoc, &slope);
        _window_footpath_cost = footpath_provisional_set(type, footpathLoc, slope);
        widget_invalidate(w, WIDX_CONSTRUCT);
    }

    auto curTime = platform_get_ticks();

    // Update little directional arrow on provisional bridge mode path
    if (_footpathConstructionNextArrowPulse < curTime)
    {
        _footpathConstructionNextArrowPulse = curTime + ARROW_PULSE_DURATION;

        gFootpathProvisionalFlags ^= PROVISIONAL_PATH_FLAG_SHOW_ARROW;
        CoordsXYZ footpathLoc;
        footpath_get_next_path_info(&type, footpathLoc, &slope);
        gMapSelectArrowPosition = footpathLoc;
        gMapSelectArrowDirection = gFootpathConstructDirection;
        if (gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_SHOW_ARROW)
        {
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
        }
        else
        {
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
        }
        map_invalidate_tile_full(footpathLoc);
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
    // Press / unpress footpath and queue type buttons
    w->pressed_widgets &= ~(1 << WIDX_FOOTPATH_TYPE);
    w->pressed_widgets &= ~(1 << WIDX_QUEUELINE_TYPE);
    w->pressed_widgets |= gFootpathSelectedType == SELECTED_PATH_TYPE_NORMAL ? (1 << WIDX_FOOTPATH_TYPE)
                                                                             : (1 << WIDX_QUEUELINE_TYPE);

    // Enable / disable construct button
    window_footpath_widgets[WIDX_CONSTRUCT].type = gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL
        ? WindowWidgetType::ImgBtn
        : WindowWidgetType::Empty;

    // Set footpath and queue type button images
    auto pathImage = static_cast<uint32_t>(SPR_NONE);
    auto queueImage = static_cast<uint32_t>(SPR_NONE);
    auto pathEntry = get_path_surface_entry(gFootpathSelectedId);
    if (pathEntry != nullptr)
    {
        pathImage = pathEntry->preview;
        // Editor-only paths might lack a queue image
        queueImage = (pathEntry->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) ? pathImage : pathImage + 1;
    }
    window_footpath_widgets[WIDX_FOOTPATH_TYPE].image = pathImage;
    window_footpath_widgets[WIDX_QUEUELINE_TYPE].image = queueImage;
}

/**
 *
 *  rct2: 0x006A7D8B
 */
static void window_footpath_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    ScreenCoordsXY screenCoords;
    WindowDrawWidgets(w, dpi);

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

        int32_t selectedPath = gFootpathSelectedId + (MAX_PATH_OBJECTS * gFootpathSelectedType);
        PathSurfaceEntry* pathType = get_path_surface_entry(selectedPath);
        image += pathType->image;

        // Draw construction image
        screenCoords = w->windowPos
            + ScreenCoordsXY{ window_footpath_widgets[WIDX_CONSTRUCT].midX(),
                              window_footpath_widgets[WIDX_CONSTRUCT].bottom - 60 };
        gfx_draw_sprite(dpi, ImageId(image), screenCoords);

        // Draw build this... label
        screenCoords = w->windowPos
            + ScreenCoordsXY{ window_footpath_widgets[WIDX_CONSTRUCT].midX(),
                              window_footpath_widgets[WIDX_CONSTRUCT].bottom - 23 };
        DrawTextBasic(dpi, screenCoords, STR_BUILD_THIS, {}, { TextAlignment::CENTRE });
    }

    // Draw cost
    screenCoords = w->windowPos
        + ScreenCoordsXY{ window_footpath_widgets[WIDX_CONSTRUCT].midX(), window_footpath_widgets[WIDX_CONSTRUCT].bottom - 12 };
    if (_window_footpath_cost != MONEY32_UNDEFINED)
    {
        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            DrawTextBasic(dpi, screenCoords, STR_COST_LABEL, &_window_footpath_cost, { TextAlignment::CENTRE });
        }
    }
}

/**
 *
 *  rct2: 0x006A7F88
 */
static void window_footpath_show_footpath_types_dialog(rct_window* w, rct_widget* widget, bool showQueues)
{
    int32_t i, image;
    PathSurfaceEntry* pathType;

    uint32_t numPathTypes = 0;
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

    auto itemsPerRow = DropdownGetAppropriateImageDropdownItemsPerRow(numPathTypes);
    WindowDropdownShowImage(
        w->windowPos.x + widget->left, w->windowPos.y + widget->top, widget->height() + 1, w->colours[1], 0, numPathTypes, 47,
        36, itemsPerRow);
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
static void window_footpath_set_provisional_path_at_point(const ScreenCoordsXY& screenCoords)
{
    map_invalidate_selection_rect();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

    auto info = get_map_coordinates_from_pos(
        screenCoords, EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Footpath));

    if (info.SpriteType == ViewportInteractionItem::None || info.Element == nullptr)
    {
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
        footpath_provisional_update();
    }
    else
    {
        // Check for change
        if ((gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_1)
            && gFootpathProvisionalPosition == CoordsXYZ{ info.Loc, info.Element->GetBaseZ() })
        {
            return;
        }

        // Set map selection
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        gMapSelectPositionA = info.Loc;
        gMapSelectPositionB = info.Loc;

        footpath_provisional_update();

        // Set provisional path
        int32_t slope = 0;
        switch (info.SpriteType)
        {
            case ViewportInteractionItem::Terrain:
            {
                auto surfaceElement = info.Element->AsSurface();
                if (surfaceElement != nullptr)
                {
                    slope = DefaultPathSlope[surfaceElement->GetSlope() & TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK];
                }
                break;
            }
            case ViewportInteractionItem::Footpath:
            {
                auto pathElement = info.Element->AsPath();
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
            default:
                break;
        }
        auto z = info.Element->GetBaseZ();
        if (slope & RAISE_FOOTPATH_FLAG)
        {
            slope &= ~RAISE_FOOTPATH_FLAG;
            z += PATH_HEIGHT_STEP;
        }
        int32_t pathType = (gFootpathSelectedType << 7) + (gFootpathSelectedId & 0xFF);

        _window_footpath_cost = footpath_provisional_set(pathType, { info.Loc, z }, slope);
        window_invalidate_by_class(WC_FOOTPATH);
    }
}

/**
 *
 *  rct2: 0x006A8388
 */
static void window_footpath_set_selection_start_bridge_at_point(const ScreenCoordsXY& screenCoords)
{
    int32_t direction;
    TileElement* tileElement;

    map_invalidate_selection_rect();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

    auto mapCoords = footpath_bridge_get_info_from_pos(screenCoords, &direction, &tileElement);
    if (mapCoords.isNull())
    {
        return;
    }

    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectType = MAP_SELECT_TYPE_FULL;
    gMapSelectPositionA = mapCoords;
    gMapSelectPositionB = mapCoords;

    int32_t z = tileElement->GetBaseZ();

    if (tileElement->GetType() == TILE_ELEMENT_TYPE_SURFACE)
    {
        uint8_t slope = tileElement->AsSurface()->GetSlope();
        if (slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
        {
            z += PATH_HEIGHT_STEP;
        } // Add 2 for a slope
        if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
            z += PATH_HEIGHT_STEP; // Add another 2 for a steep slope
    }

    gMapSelectArrowPosition = CoordsXYZ{ mapCoords, z };
    gMapSelectArrowDirection = direction;

    map_invalidate_selection_rect();
}

/**
 *
 *  rct2: 0x006A82C5
 */
static void window_footpath_place_path_at_point(const ScreenCoordsXY& screenCoords)
{
    if (_footpathErrorOccured)
    {
        return;
    }

    footpath_provisional_update();

    const auto info = get_map_coordinates_from_pos(
        screenCoords, EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Footpath));

    if (info.SpriteType == ViewportInteractionItem::None)
    {
        return;
    }

    // Set path
    auto slope = 0;
    switch (info.SpriteType)
    {
        case ViewportInteractionItem::Terrain:
            slope = DefaultPathSlope[info.Element->AsSurface()->GetSlope() & TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK];
            break;
        case ViewportInteractionItem::Footpath:
            slope = info.Element->AsPath()->GetSlopeDirection();
            if (info.Element->AsPath()->IsSloped())
            {
                slope |= FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
            }
            break;
        default:
            break;
    }
    auto z = info.Element->GetBaseZ();
    if (slope & RAISE_FOOTPATH_FLAG)
    {
        slope &= ~RAISE_FOOTPATH_FLAG;
        z += PATH_HEIGHT_STEP;
    }
    auto selectedType = (gFootpathSelectedType << 7) + (gFootpathSelectedId & 0xFF);

    // Try and place path
    gGameCommandErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
    auto footpathPlaceAction = FootpathPlaceAction({ info.Loc, z }, slope, selectedType);
    footpathPlaceAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error == GameActions::Status::Ok)
        {
            // Don't play sound if it is no cost to prevent multiple sounds. TODO: make this work in no money scenarios
            if (result->Cost != 0)
            {
                OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);
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
static void window_footpath_start_bridge_at_point(const ScreenCoordsXY& screenCoords)
{
    int32_t z, direction;
    TileElement* tileElement;

    auto mapCoords = footpath_bridge_get_info_from_pos(screenCoords, &direction, &tileElement);
    if (mapCoords.isNull())
    {
        return;
    }

    if (tileElement->GetType() == TILE_ELEMENT_TYPE_SURFACE)
    {
        // If we start the path on a slope, the arrow is slightly raised, so we
        // expect the path to be slightly raised as well.
        uint8_t slope = tileElement->AsSurface()->GetSlope();
        z = tileElement->GetBaseZ();
        if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
        {
            // Steep diagonal slope
            z += 2 * PATH_HEIGHT_STEP;
        }
        else if (slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
        {
            // Normal slope
            z += PATH_HEIGHT_STEP;
        }
    }
    else
    {
        z = tileElement->GetBaseZ();
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            if (tileElement->AsPath()->IsSloped())
            {
                if (direction == (tileElement->AsPath()->GetSlopeDirection()))
                {
                    z += PATH_HEIGHT_STEP;
                }
            }
        }
    }

    tool_cancel();
    gFootpathConstructFromPosition = { mapCoords, z };
    gFootpathConstructDirection = direction;
    gFootpathProvisionalFlags = 0;
    gFootpathConstructSlope = 0;
    gFootpathConstructionMode = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL;
    gFootpathConstructValidDirections = INVALID_DIRECTION;
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

    int32_t type, slope;
    CoordsXYZ footpathLoc;
    footpath_get_next_path_info(&type, footpathLoc, &slope);

    gGameCommandErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
    auto footpathPlaceAction = FootpathPlaceAction(footpathLoc, slope, type, gFootpathConstructDirection);
    footpathPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error == GameActions::Status::Ok)
        {
            OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);

            if (gFootpathConstructSlope == 0)
            {
                gFootpathConstructValidDirections = INVALID_DIRECTION;
            }
            else
            {
                gFootpathConstructValidDirections = gFootpathConstructDirection;
            }

            if (gFootpathGroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                viewport_set_visibility(1);
            }

            gFootpathConstructFromPosition = footpathLoc;
            // If we have just built an upwards slope, the next path to construct is
            // a bit higher. Note that the z returned by footpath_get_next_path_info
            // already is lowered if we are building a downwards slope.
            if (gFootpathConstructSlope == 2)
            {
                gFootpathConstructFromPosition.z += PATH_HEIGHT_STEP;
            }
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
    auto z = tileElement->GetBaseZ();
    if (tileElement->AsPath()->IsSloped())
    {
        uint8_t slopeDirection = tileElement->AsPath()->GetSlopeDirection();
        slopeDirection = direction_reverse(slopeDirection);
        if (slopeDirection == gFootpathConstructDirection)
        {
            z += PATH_HEIGHT_STEP;
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

    gFootpathConstructFromPosition.z = tileElement->GetBaseZ();
    // Remove path
    footpath_remove(gFootpathConstructFromPosition, GAME_COMMAND_FLAG_APPLY);

    // Move selection
    edge = direction_reverse(edge);
    gFootpathConstructFromPosition.x -= CoordsDirectionDelta[edge].x;
    gFootpathConstructFromPosition.y -= CoordsDirectionDelta[edge].y;
    gFootpathConstructFromPosition.z = z;
    gFootpathConstructDirection = edge;
    gFootpathConstructValidDirections = INVALID_DIRECTION;
}

/**
 *
 *  rct2: 0x006A7873
 */
static TileElement* footpath_get_tile_element_to_remove()
{
    TileElement* tileElement;
    int32_t z, zLow;

    if (!map_is_location_valid(gFootpathConstructFromPosition))
    {
        return nullptr;
    }

    z = std::min(255 * COORDS_Z_STEP, gFootpathConstructFromPosition.z);
    zLow = z - PATH_HEIGHT_STEP;

    tileElement = map_get_first_element_at(gFootpathConstructFromPosition);
    do
    {
        if (tileElement == nullptr)
            break;
        if (tileElement->GetType() == TILE_ELEMENT_TYPE_PATH)
        {
            if (tileElement->GetBaseZ() == z)
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
            else if (tileElement->GetBaseZ() == zLow)
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
        gMapSelectionTiles.clear();
        gMapSelectionTiles.push_back({ gFootpathConstructFromPosition.x + CoordsDirectionDelta[direction].x,
                                       gFootpathConstructFromPosition.y + CoordsDirectionDelta[direction].y });
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
        if (direction != INVALID_DIRECTION)
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
    w->Invalidate();
}

/**
 *
 *  rct2: 0x006A7B20
 */
static void footpath_get_next_path_info(int32_t* type, CoordsXYZ& footpathLoc, int32_t* slope)
{
    int32_t direction;

    direction = gFootpathConstructDirection;
    footpathLoc.x = gFootpathConstructFromPosition.x + CoordsDirectionDelta[direction].x;
    footpathLoc.y = gFootpathConstructFromPosition.y + CoordsDirectionDelta[direction].y;
    footpathLoc.z = gFootpathConstructFromPosition.z;
    *type = (gFootpathSelectedType << 7) + (gFootpathSelectedId & 0xFF);
    *slope = TILE_ELEMENT_SLOPE_FLAT;
    if (gFootpathConstructSlope != 0)
    {
        *slope = gFootpathConstructDirection | TILE_ELEMENT_SLOPE_S_CORNER_UP;
        if (gFootpathConstructSlope != 2)
        {
            footpathLoc.z -= PATH_HEIGHT_STEP;
            *slope ^= TILE_ELEMENT_SLOPE_E_CORNER_UP;
        }
    }
}

static bool footpath_select_default()
{
    // Select first available footpath
    int32_t footpathId = -1;
    for (int32_t i = 0; i < object_entry_group_counts[EnumValue(ObjectType::Paths)]; i++)
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

void window_footpath_keyboard_shortcut_turn_left()
{
    rct_window* w = window_find_by_class(WC_FOOTPATH);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_DIRECTION_NW) || WidgetIsDisabled(w, WIDX_DIRECTION_NE)
        || WidgetIsDisabled(w, WIDX_DIRECTION_SW) || WidgetIsDisabled(w, WIDX_DIRECTION_SE) || gFootpathConstructionMode != 2)
    {
        return;
    }
    int32_t currentRotation = get_current_rotation();
    int32_t turnedRotation = gFootpathConstructDirection - currentRotation + (currentRotation % 2 == 1 ? 1 : -1);
    window_footpath_mousedown_direction(turnedRotation);
}

void window_footpath_keyboard_shortcut_turn_right()
{
    rct_window* w = window_find_by_class(WC_FOOTPATH);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_DIRECTION_NW) || WidgetIsDisabled(w, WIDX_DIRECTION_NE)
        || WidgetIsDisabled(w, WIDX_DIRECTION_SW) || WidgetIsDisabled(w, WIDX_DIRECTION_SE) || gFootpathConstructionMode != 2)
    {
        return;
    }
    int32_t currentRotation = get_current_rotation();
    int32_t turnedRotation = gFootpathConstructDirection - currentRotation + (currentRotation % 2 == 1 ? -1 : 1);
    window_footpath_mousedown_direction(turnedRotation);
}

void window_footpath_keyboard_shortcut_slope_down()
{
    rct_window* w = window_find_by_class(WC_FOOTPATH);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_SLOPEDOWN) || WidgetIsDisabled(w, WIDX_LEVEL)
        || WidgetIsDisabled(w, WIDX_SLOPEUP) || w->widgets[WIDX_LEVEL].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (gFootpathConstructSlope)
    {
        case 0:
            window_event_mouse_down_call(w, WIDX_SLOPEDOWN);
            break;
        case 2:
            window_event_mouse_down_call(w, WIDX_LEVEL);
            break;
        default:
        case 6:
            return;
    }
}

void window_footpath_keyboard_shortcut_slope_up()
{
    rct_window* w = window_find_by_class(WC_FOOTPATH);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_SLOPEDOWN) || WidgetIsDisabled(w, WIDX_LEVEL)
        || WidgetIsDisabled(w, WIDX_SLOPEUP) || w->widgets[WIDX_LEVEL].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (gFootpathConstructSlope)
    {
        case 6:
            window_event_mouse_down_call(w, WIDX_LEVEL);
            break;
        case 0:
            window_event_mouse_down_call(w, WIDX_SLOPEUP);
            break;
        default:
        case 2:
            return;
    }
}

void window_footpath_keyboard_shortcut_demolish_current()
{
    rct_window* w = window_find_by_class(WC_FOOTPATH);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_REMOVE) || w->widgets[WIDX_REMOVE].type == WindowWidgetType::Empty
        || (!gCheatsBuildInPauseMode && game_is_paused()))
    {
        return;
    }

    window_footpath_remove();
}

void window_footpath_keyboard_shortcut_build_current()
{
    rct_window* w = window_find_by_class(WC_FOOTPATH);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_CONSTRUCT) || w->widgets[WIDX_CONSTRUCT].type == WindowWidgetType::Empty)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_CONSTRUCT);
}
