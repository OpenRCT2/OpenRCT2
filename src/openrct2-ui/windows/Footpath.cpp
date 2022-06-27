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
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/FootpathPlaceAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/FootpathObject.h>
#include <openrct2/object/FootpathRailingsObject.h>
#include <openrct2/object/FootpathSurfaceObject.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Surface.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_FOOTPATHS;
static constexpr const int32_t WH = 421;
static constexpr const int32_t WW = 106;
static constexpr const uint16_t ARROW_PULSE_DURATION = 200;

static uint8_t _footpathConstructDirection;
static uint8_t _footpathConstructValidDirections;
static uint8_t _footpathConstructionMode;

static std::vector<std::pair<ObjectType, ObjectEntryIndex>> _dropdownEntries;

static PathConstructFlags FootpathCreateConstructFlags(ObjectEntryIndex& type);

// clang-format off
enum
{
    PATH_CONSTRUCTION_MODE_LAND,
    PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL,
    PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL
};

enum WindowFootpathWidgetIdx
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,

    WIDX_TYPE_GROUP,
    WIDX_FOOTPATH_TYPE,
    WIDX_QUEUELINE_TYPE,
    WIDX_RAILINGS_TYPE,

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
    MakeWidget({ 3,  17}, {100, 95}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_TYPE                                                                          ),
    MakeWidget({ 6,  30}, { 47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_FOOTPATH_TIP                               ),
    MakeWidget({53,  30}, { 47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_QUEUE_LINE_PATH_TIP                        ),
    MakeWidget({29,  69}, { 47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_OBJECT_SELECTION_FOOTPATH_RAILINGS                               ),

    // Direction group
    MakeWidget({ 3, 115}, {100, 77}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_DIRECTION                                                                     ),
    MakeWidget({53, 127}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_NE,     STR_DIRECTION_TIP                              ),
    MakeWidget({53, 156}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_SE,     STR_DIRECTION_TIP                              ),
    MakeWidget({ 8, 156}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_SW,     STR_DIRECTION_TIP                              ),
    MakeWidget({ 8, 127}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_NW,     STR_DIRECTION_TIP                              ),

    // Slope group
    MakeWidget({ 3, 195}, {100, 41}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_SLOPE                                                                         ),
    MakeWidget({17, 207}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_DOWN,  STR_SLOPE_DOWN_TIP                             ),
    MakeWidget({41, 207}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_LEVEL, STR_LEVEL_TIP                                  ),
    MakeWidget({65, 207}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_UP,    STR_SLOPE_UP_TIP                               ),
    MakeWidget({ 8, 242}, { 90, 90}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_CONSTRUCT_THE_SELECTED_FOOTPATH_SECTION_TIP),
    MakeWidget({30, 335}, { 46, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_DEMOLISH_CURRENT_SECTION,      STR_REMOVE_PREVIOUS_FOOTPATH_SECTION_TIP       ),

    // Mode group
    MakeWidget({ 3, 361}, {100, 54}, WindowWidgetType::Groupbox, WindowColour::Primary                                                                                      ),
    MakeWidget({13, 372}, { 36, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_FOOTPATH_LAND,    STR_CONSTRUCT_FOOTPATH_ON_LAND_TIP             ),
    MakeWidget({57, 372}, { 36, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_FOOTPATH_BRIDGE,  STR_CONSTRUCT_BRIDGE_OR_TUNNEL_FOOTPATH_TIP    ),
    WIDGETS_END,
};

static void WindowFootpathClose(rct_window * w);
static void WindowFootpathMouseup(rct_window * w, rct_widgetindex widgetIndex);
static void WindowFootpathMousedown(rct_window * w, rct_widgetindex widgetIndex, rct_widget * widget);
static void WindowFootpathDropdown(rct_window * w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void WindowFootpathUpdate(rct_window * w);
static void WindowFootpathToolupdate(rct_window * w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowFootpathTooldown(rct_window * w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowFootpathTooldrag(rct_window * w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowFootpathToolup(rct_window * w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowFootpathInvalidate(rct_window * w);
static void WindowFootpathPaint(rct_window * w, rct_drawpixelinfo * dpi);

static rct_window_event_list window_footpath_events([](auto& events)
{
    events.close = &WindowFootpathClose;
    events.mouse_up = &WindowFootpathMouseup;
    events.mouse_down = &WindowFootpathMousedown;
    events.dropdown = &WindowFootpathDropdown;
    events.update = &WindowFootpathUpdate;
    events.tool_update = &WindowFootpathToolupdate;
    events.tool_down = &WindowFootpathTooldown;
    events.tool_drag = &WindowFootpathTooldrag;
    events.tool_up = &WindowFootpathToolup;
    events.invalidate = &WindowFootpathInvalidate;
    events.paint = &WindowFootpathPaint;
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

static void WindowFootpathMousedownDirection(int32_t direction);
static void WindowFootpathMousedownSlope(int32_t slope);
static void WindowFootpathShowFootpathTypesDialog(rct_window* w, rct_widget* widget, bool showQueues);
static void WindowFootpathShowRailingsTypesDialog(rct_window* w, rct_widget* widget);
static void WindowFootpathSetProvisionalPathAtPoint(const ScreenCoordsXY& screenCoords);
static void WindowFootpathSetSelectionStartBridgeAtPoint(const ScreenCoordsXY& screenCoords);
static void WindowFootpathPlacePathAtPoint(const ScreenCoordsXY& screenCoords);
static void WindowFootpathStartBridgeAtPoint(const ScreenCoordsXY& screenCoords);
static void WindowFootpathConstruct();
static void WindowFootpathRemove();
static void WindowFootpathSetEnabledAndPressedWidgets();
static void FootpathGetNextPathInfo(ObjectEntryIndex* type, CoordsXYZ& footpathLoc, int32_t* slope);
static bool FootpathSelectDefault();

/**
 *
 *  rct2: 0x006A7C43
 */
rct_window* WindowFootpathOpen()
{
    if (!FootpathSelectDefault())
    {
        // No path objects to select from, don't open window
        return nullptr;
    }

    // Check if window is already open
    rct_window* window = window_bring_to_front_by_class(WC_FOOTPATH);
    if (window != nullptr)
    {
        return window;
    }

    window = WindowCreate(ScreenCoordsXY(0, 29), WW, WH, &window_footpath_events, WC_FOOTPATH, 0);
    window->widgets = window_footpath_widgets;

    WindowInitScrollWidgets(window);
    window_push_others_right(window);
    show_gridlines();

    tool_cancel();
    _footpathConstructionMode = PATH_CONSTRUCTION_MODE_LAND;
    tool_set(window, WIDX_CONSTRUCT_ON_LAND, Tool::PathDown);
    input_set_flag(INPUT_FLAG_6, true);
    _footpathErrorOccured = false;
    WindowFootpathSetEnabledAndPressedWidgets();

    return window;
}

/**
 *
 *  rct2: 0x006A852F
 */
static void WindowFootpathClose(rct_window* w)
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
static void WindowFootpathMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_CONSTRUCT:
            WindowFootpathConstruct();
            break;
        case WIDX_REMOVE:
            WindowFootpathRemove();
            break;
        case WIDX_CONSTRUCT_ON_LAND:
            if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_LAND)
            {
                break;
            }

            _window_footpath_cost = MONEY32_UNDEFINED;
            tool_cancel();
            footpath_provisional_update();
            map_invalidate_map_selection_tiles();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            _footpathConstructionMode = PATH_CONSTRUCTION_MODE_LAND;
            tool_set(w, WIDX_CONSTRUCT_ON_LAND, Tool::PathDown);
            input_set_flag(INPUT_FLAG_6, true);
            _footpathErrorOccured = false;
            WindowFootpathSetEnabledAndPressedWidgets();
            break;
        case WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL:
            if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL)
            {
                break;
            }

            _window_footpath_cost = MONEY32_UNDEFINED;
            tool_cancel();
            footpath_provisional_update();
            map_invalidate_map_selection_tiles();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            _footpathConstructionMode = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL;
            tool_set(w, WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL, Tool::Crosshair);
            input_set_flag(INPUT_FLAG_6, true);
            _footpathErrorOccured = false;
            WindowFootpathSetEnabledAndPressedWidgets();
            break;
    }
}

/**
 *
 *  rct2: 0x006A7EC5
 */
static void WindowFootpathMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_FOOTPATH_TYPE:
            WindowFootpathShowFootpathTypesDialog(w, widget, false);
            break;
        case WIDX_QUEUELINE_TYPE:
            WindowFootpathShowFootpathTypesDialog(w, widget, true);
            break;
        case WIDX_RAILINGS_TYPE:
            WindowFootpathShowRailingsTypesDialog(w, widget);
            break;
        case WIDX_DIRECTION_NW:
            WindowFootpathMousedownDirection(0);
            break;
        case WIDX_DIRECTION_NE:
            WindowFootpathMousedownDirection(1);
            break;
        case WIDX_DIRECTION_SW:
            WindowFootpathMousedownDirection(2);
            break;
        case WIDX_DIRECTION_SE:
            WindowFootpathMousedownDirection(3);
            break;
        case WIDX_SLOPEDOWN:
            WindowFootpathMousedownSlope(6);
            break;
        case WIDX_LEVEL:
            WindowFootpathMousedownSlope(0);
            break;
        case WIDX_SLOPEUP:
            WindowFootpathMousedownSlope(2);
            break;
    }
}

/**
 *
 *  rct2: 0x006A7F18
 */
static void WindowFootpathDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex < 0 || static_cast<size_t>(dropdownIndex) >= _dropdownEntries.size())
        return;

    auto entryIndex = _dropdownEntries[dropdownIndex];
    if (widgetIndex == WIDX_FOOTPATH_TYPE)
    {
        gFootpathSelection.IsQueueSelected = false;
        if (entryIndex.first == ObjectType::Paths)
        {
            gFootpathSelection.LegacyPath = entryIndex.second;
        }
        else
        {
            gFootpathSelection.LegacyPath = OBJECT_ENTRY_INDEX_NULL;
            gFootpathSelection.NormalSurface = entryIndex.second;
        }
    }
    else if (widgetIndex == WIDX_QUEUELINE_TYPE)
    {
        gFootpathSelection.IsQueueSelected = true;
        if (entryIndex.first == ObjectType::Paths)
        {
            gFootpathSelection.LegacyPath = entryIndex.second;
        }
        else
        {
            gFootpathSelection.LegacyPath = OBJECT_ENTRY_INDEX_NULL;
            gFootpathSelection.QueueSurface = entryIndex.second;
        }
    }
    else if (widgetIndex == WIDX_RAILINGS_TYPE)
    {
        gFootpathSelection.Railings = entryIndex.second;
    }
    else
    {
        return;
    }

    footpath_provisional_update();
    _window_footpath_cost = MONEY32_UNDEFINED;
    w->Invalidate();
}

/**
 *
 *  rct2: 0x006A8032
 */
static void WindowFootpathToolupdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
    {
        WindowFootpathSetProvisionalPathAtPoint(screenCoords);
    }
    else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
    {
        WindowFootpathSetSelectionStartBridgeAtPoint(screenCoords);
    }
}

/**
 *
 *  rct2: 0x006A8047
 */
static void WindowFootpathTooldown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
    {
        WindowFootpathPlacePathAtPoint(screenCoords);
    }
    else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
    {
        WindowFootpathStartBridgeAtPoint(screenCoords);
    }
}

/**
 *
 *  rct2: 0x006A8067
 */
static void WindowFootpathTooldrag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
    {
        WindowFootpathPlacePathAtPoint(screenCoords);
    }
}

/**
 *
 *  rct2: 0x006A8066
 */
static void WindowFootpathToolup(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
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
static void WindowFootpathUpdateProvisionalPathForBridgeMode(rct_window* w)
{
    if (_footpathConstructionMode != PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
    {
        return;
    }

    // Recheck area for construction. Set by ride_construction window
    if (gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_2)
    {
        footpath_provisional_remove();
        gProvisionalFootpath.Flags &= ~PROVISIONAL_PATH_FLAG_2;
    }

    // Update provisional bridge mode path
    if (!(gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_1))
    {
        ObjectEntryIndex type;
        ObjectEntryIndex railings = gFootpathSelection.Railings;

        CoordsXYZ footpathLoc;
        int32_t slope;
        FootpathGetNextPathInfo(&type, footpathLoc, &slope);
        auto pathConstructFlags = FootpathCreateConstructFlags(type);

        _window_footpath_cost = footpath_provisional_set(type, railings, footpathLoc, slope, pathConstructFlags);
        widget_invalidate(w, WIDX_CONSTRUCT);
    }

    auto curTime = Platform::GetTicks();

    // Update little directional arrow on provisional bridge mode path
    if (_footpathConstructionNextArrowPulse < curTime)
    {
        _footpathConstructionNextArrowPulse = curTime + ARROW_PULSE_DURATION;

        gProvisionalFootpath.Flags ^= PROVISIONAL_PATH_FLAG_SHOW_ARROW;
        CoordsXYZ footpathLoc;
        int32_t slope;
        FootpathGetNextPathInfo(nullptr, footpathLoc, &slope);
        gMapSelectArrowPosition = footpathLoc;
        gMapSelectArrowDirection = _footpathConstructDirection;
        if (gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_SHOW_ARROW)
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
static void WindowFootpathUpdate(rct_window* w)
{
    widget_invalidate(w, WIDX_CONSTRUCT);
    WindowFootpathUpdateProvisionalPathForBridgeMode(w);

    // #2502: The camera might have changed rotation, so we need to update which directional buttons are pressed
    uint8_t currentRotation = get_current_rotation();
    if (_lastUpdatedCameraRotation != currentRotation)
    {
        _lastUpdatedCameraRotation = currentRotation;
        WindowFootpathSetEnabledAndPressedWidgets();
    }

    // Check tool
    if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_LAND)
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
    else if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL)
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
static void WindowFootpathInvalidate(rct_window* w)
{
    // Press / unpress footpath and queue type buttons
    w->pressed_widgets &= ~(1ULL << WIDX_FOOTPATH_TYPE);
    w->pressed_widgets &= ~(1ULL << WIDX_QUEUELINE_TYPE);
    w->pressed_widgets |= gFootpathSelection.IsQueueSelected ? (1ULL << WIDX_QUEUELINE_TYPE) : (1ULL << WIDX_FOOTPATH_TYPE);

    // Enable / disable construct button
    window_footpath_widgets[WIDX_CONSTRUCT].type = _footpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL
        ? WindowWidgetType::ImgBtn
        : WindowWidgetType::Empty;

    if (gFootpathSelection.LegacyPath == OBJECT_ENTRY_INDEX_NULL)
    {
        window_footpath_widgets[WIDX_RAILINGS_TYPE].type = WindowWidgetType::FlatBtn;
    }
    else
    {
        window_footpath_widgets[WIDX_RAILINGS_TYPE].type = WindowWidgetType::Empty;
    }
}

static void WindowFootpathDrawDropdownButton(
    rct_window* w, rct_drawpixelinfo* dpi, rct_widgetindex widgetIndex, ImageIndex image)
{
    const auto& widget = w->widgets[widgetIndex];
    gfx_draw_sprite(dpi, ImageId(image), { w->windowPos.x + widget.left, w->windowPos.y + widget.top });
}

static void WindowFootpathDrawDropdownButtons(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (gFootpathSelection.LegacyPath == OBJECT_ENTRY_INDEX_NULL)
    {
        // Set footpath and queue type button images
        auto pathImage = static_cast<uint32_t>(SPR_NONE);
        auto queueImage = static_cast<uint32_t>(SPR_NONE);
        auto pathEntry = GetPathSurfaceEntry(gFootpathSelection.NormalSurface);
        if (pathEntry != nullptr)
        {
            pathImage = pathEntry->PreviewImageId;
        }

        pathEntry = GetPathSurfaceEntry(gFootpathSelection.QueueSurface);
        if (pathEntry != nullptr)
        {
            queueImage = pathEntry->PreviewImageId;
        }

        WindowFootpathDrawDropdownButton(w, dpi, WIDX_FOOTPATH_TYPE, pathImage);
        WindowFootpathDrawDropdownButton(w, dpi, WIDX_QUEUELINE_TYPE, queueImage);

        // Set railing
        auto railingsImage = static_cast<uint32_t>(SPR_NONE);
        auto railingsEntry = GetPathRailingsEntry(gFootpathSelection.Railings);
        if (railingsEntry != nullptr)
        {
            railingsImage = railingsEntry->PreviewImageId;
        }
        WindowFootpathDrawDropdownButton(w, dpi, WIDX_RAILINGS_TYPE, railingsImage);
    }
    else
    {
        auto& objManager = OpenRCT2::GetContext()->GetObjectManager();

        // Set footpath and queue type button images
        auto pathImage = static_cast<uint32_t>(SPR_NONE);
        auto queueImage = static_cast<uint32_t>(SPR_NONE);
        auto pathObj = static_cast<FootpathObject*>(
            objManager.GetLoadedObject(ObjectType::Paths, gFootpathSelection.LegacyPath));
        if (pathObj != nullptr)
        {
            auto pathEntry = reinterpret_cast<rct_footpath_entry*>(pathObj->GetLegacyData());
            pathImage = pathEntry->GetPreviewImage();
            queueImage = pathEntry->GetQueuePreviewImage();
        }

        WindowFootpathDrawDropdownButton(w, dpi, WIDX_FOOTPATH_TYPE, pathImage);
        WindowFootpathDrawDropdownButton(w, dpi, WIDX_QUEUELINE_TYPE, queueImage);
    }
}

/**
 *
 *  rct2: 0x006A7D8B
 */
static void WindowFootpathPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    ScreenCoordsXY screenCoords;
    WindowDrawWidgets(w, dpi);
    WindowFootpathDrawDropdownButtons(w, dpi);

    if (!WidgetIsDisabled(w, WIDX_CONSTRUCT))
    {
        // Get construction image
        uint8_t direction = (_footpathConstructDirection + get_current_rotation()) % 4;
        uint8_t slope = 0;
        if (gFootpathConstructSlope == 2)
        {
            slope = TILE_ELEMENT_SLOPE_N_CORNER_UP;
        }
        else if (gFootpathConstructSlope == 6)
        {
            slope = TILE_ELEMENT_SLOPE_E_CORNER_UP;
        }

        std::optional<uint32_t> baseImage;
        if (gFootpathSelection.LegacyPath == OBJECT_ENTRY_INDEX_NULL)
        {
            auto selectedPath = gFootpathSelection.GetSelectedSurface();
            const auto* pathType = GetPathSurfaceEntry(selectedPath);
            if (pathType != nullptr)
            {
                baseImage = pathType->BaseImageId;
            }
        }
        else
        {
            auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
            auto* pathObj = static_cast<FootpathObject*>(
                objManager.GetLoadedObject(ObjectType::Paths, gFootpathSelection.LegacyPath));
            if (pathObj != nullptr)
            {
                auto pathEntry = reinterpret_cast<const rct_footpath_entry*>(pathObj->GetLegacyData());
                if (gFootpathSelection.IsQueueSelected)
                    baseImage = pathEntry->GetQueueImage();
                else
                    baseImage = pathEntry->image;
            }
        }

        if (baseImage)
        {
            auto image = *baseImage + ConstructionPreviewImages[slope][direction];

            // Draw construction image
            screenCoords = w->windowPos
                + ScreenCoordsXY{ window_footpath_widgets[WIDX_CONSTRUCT].midX(),
                                  window_footpath_widgets[WIDX_CONSTRUCT].bottom - 60 };
            gfx_draw_sprite(dpi, ImageId(image), screenCoords);
        }

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
            auto ft = Formatter();
            ft.Add<money64>(_window_footpath_cost);
            DrawTextBasic(dpi, screenCoords, STR_COST_LABEL, ft, { TextAlignment::CENTRE });
        }
    }
}

/**
 *
 *  rct2: 0x006A7F88
 */
static void WindowFootpathShowFootpathTypesDialog(rct_window* w, rct_widget* widget, bool showQueues)
{
    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();

    uint32_t numPathTypes = 0;
    // If the game is in sandbox mode, also show paths that are normally restricted to the scenario editor
    bool showEditorPaths = ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode);

    _dropdownEntries.clear();
    std::optional<size_t> defaultIndex;
    for (ObjectEntryIndex i = 0; i < MAX_FOOTPATH_SURFACE_OBJECTS; i++)
    {
        const auto* pathType = static_cast<FootpathSurfaceObject*>(objManager.GetLoadedObject(ObjectType::FootpathSurface, i));
        if (pathType == nullptr)
        {
            continue;
        }
        if ((pathType->Flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) && !showEditorPaths)
        {
            continue;
        }
        if (showQueues != ((pathType->Flags & FOOTPATH_ENTRY_FLAG_IS_QUEUE) != 0))
        {
            continue;
        }
        if (gFootpathSelection.LegacyPath == OBJECT_ENTRY_INDEX_NULL
            && i == (showQueues ? gFootpathSelection.QueueSurface : gFootpathSelection.NormalSurface))
        {
            defaultIndex = numPathTypes;
        }

        gDropdownItems[numPathTypes].Format = STR_NONE;
        Dropdown::SetImage(numPathTypes, ImageId(pathType->PreviewImageId));
        _dropdownEntries.push_back({ ObjectType::FootpathSurface, i });
        numPathTypes++;
    }

    for (ObjectEntryIndex i = 0; i < MAX_PATH_OBJECTS; i++)
    {
        auto* pathObj = static_cast<FootpathObject*>(objManager.GetLoadedObject(ObjectType::Paths, i));
        if (pathObj == nullptr)
        {
            continue;
        }

        auto pathEntry = reinterpret_cast<const rct_footpath_entry*>(pathObj->GetLegacyData());
        if ((pathEntry->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) && !showEditorPaths)
        {
            continue;
        }

        if (gFootpathSelection.LegacyPath != OBJECT_ENTRY_INDEX_NULL && gFootpathSelection.LegacyPath == i)
        {
            defaultIndex = numPathTypes;
        }

        gDropdownItems[numPathTypes].Format = STR_NONE;
        Dropdown::SetImage(
            numPathTypes, ImageId(showQueues ? pathEntry->GetQueuePreviewImage() : pathEntry->GetPreviewImage()));
        _dropdownEntries.push_back({ ObjectType::Paths, i });
        numPathTypes++;
    }

    auto itemsPerRow = DropdownGetAppropriateImageDropdownItemsPerRow(numPathTypes);
    WindowDropdownShowImage(
        w->windowPos.x + widget->left, w->windowPos.y + widget->top, widget->height() + 1, w->colours[1], 0, numPathTypes, 47,
        36, itemsPerRow);
    if (defaultIndex)
        gDropdownDefaultIndex = static_cast<int32_t>(*defaultIndex);
}

static void WindowFootpathShowRailingsTypesDialog(rct_window* w, rct_widget* widget)
{
    uint32_t numRailingsTypes = 0;
    // If the game is in sandbox mode, also show paths that are normally restricted to the scenario editor

    _dropdownEntries.clear();
    std::optional<size_t> defaultIndex;
    for (int32_t i = 0; i < MAX_FOOTPATH_RAILINGS_OBJECTS; i++)
    {
        const auto* railingsEntry = GetPathRailingsEntry(i);
        if (railingsEntry == nullptr)
        {
            continue;
        }
        if (i == gFootpathSelection.Railings)
        {
            defaultIndex = numRailingsTypes;
        }

        gDropdownItems[numRailingsTypes].Format = STR_NONE;
        Dropdown::SetImage(numRailingsTypes, ImageId(railingsEntry->PreviewImageId));
        _dropdownEntries.push_back({ ObjectType::FootpathRailings, i });
        numRailingsTypes++;
    }

    auto itemsPerRow = DropdownGetAppropriateImageDropdownItemsPerRow(numRailingsTypes);
    WindowDropdownShowImage(
        w->windowPos.x + widget->left, w->windowPos.y + widget->top, widget->height() + 1, w->colours[1], 0, numRailingsTypes,
        47, 36, itemsPerRow);
    if (defaultIndex)
        gDropdownDefaultIndex = static_cast<int32_t>(*defaultIndex);
}

/**
 *
 *  rct2: 0x006A8111 0x006A8135 0x006A815C 0x006A8183
 */
static void WindowFootpathMousedownDirection(int32_t direction)
{
    footpath_provisional_update();
    _footpathConstructDirection = (direction - get_current_rotation()) & 3;
    _window_footpath_cost = MONEY32_UNDEFINED;
    WindowFootpathSetEnabledAndPressedWidgets();
}

/**
 *
 *  rct2: 0x006A81AA 0x006A81C5 0x006A81E0
 */
static void WindowFootpathMousedownSlope(int32_t slope)
{
    footpath_provisional_update();
    gFootpathConstructSlope = slope;
    _window_footpath_cost = MONEY32_UNDEFINED;
    WindowFootpathSetEnabledAndPressedWidgets();
}

/**
 *
 *  rct2: 0x006A81FB
 */
static void WindowFootpathSetProvisionalPathAtPoint(const ScreenCoordsXY& screenCoords)
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
        if ((gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_1)
            && gProvisionalFootpath.Position == CoordsXYZ{ info.Loc, info.Element->GetBaseZ() })
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

        auto pathType = gFootpathSelection.GetSelectedSurface();
        auto constructFlags = FootpathCreateConstructFlags(pathType);
        _window_footpath_cost = footpath_provisional_set(
            pathType, gFootpathSelection.Railings, { info.Loc, z }, slope, constructFlags);
        window_invalidate_by_class(WC_FOOTPATH);
    }
}

/**
 *
 *  rct2: 0x006A8388
 */
static void WindowFootpathSetSelectionStartBridgeAtPoint(const ScreenCoordsXY& screenCoords)
{
    int32_t direction;
    TileElement* tileElement;

    map_invalidate_selection_rect();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

    auto mapCoords = footpath_bridge_get_info_from_pos(screenCoords, &direction, &tileElement);
    if (mapCoords.IsNull())
    {
        return;
    }

    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectType = MAP_SELECT_TYPE_FULL;
    gMapSelectPositionA = mapCoords;
    gMapSelectPositionB = mapCoords;

    int32_t z = tileElement->GetBaseZ();

    if (tileElement->GetType() == TileElementType::Surface)
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
static void WindowFootpathPlacePathAtPoint(const ScreenCoordsXY& screenCoords)
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

    // Try and place path
    gGameCommandErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
    auto selectedType = gFootpathSelection.GetSelectedSurface();
    PathConstructFlags constructFlags = FootpathCreateConstructFlags(selectedType);

    auto footpathPlaceAction = FootpathPlaceAction(
        { info.Loc, z }, slope, selectedType, gFootpathSelection.Railings, INVALID_DIRECTION, constructFlags);
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
static void WindowFootpathStartBridgeAtPoint(const ScreenCoordsXY& screenCoords)
{
    int32_t z, direction;
    TileElement* tileElement;

    auto mapCoords = footpath_bridge_get_info_from_pos(screenCoords, &direction, &tileElement);
    if (mapCoords.IsNull())
    {
        return;
    }

    if (tileElement->GetType() == TileElementType::Surface)
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
        if (tileElement->GetType() == TileElementType::Path)
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
    _footpathConstructDirection = direction;
    gProvisionalFootpath.Flags = 0;
    gFootpathConstructSlope = 0;
    _footpathConstructionMode = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL;
    _footpathConstructValidDirections = INVALID_DIRECTION;
    WindowFootpathSetEnabledAndPressedWidgets();
}

/**
 * Construct a piece of footpath while in bridge building mode.
 *  rct2: 0x006A79B7
 */
static void WindowFootpathConstruct()
{
    _window_footpath_cost = MONEY32_UNDEFINED;
    footpath_provisional_update();

    ObjectEntryIndex type;
    int32_t slope;
    CoordsXYZ footpathLoc;
    FootpathGetNextPathInfo(&type, footpathLoc, &slope);

    gGameCommandErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
    PathConstructFlags constructFlags = FootpathCreateConstructFlags(type);

    auto footpathPlaceAction = FootpathPlaceAction(
        footpathLoc, slope, type, gFootpathSelection.Railings, _footpathConstructDirection, constructFlags);
    footpathPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error == GameActions::Status::Ok)
        {
            OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);

            if (gFootpathConstructSlope == 0)
            {
                _footpathConstructValidDirections = INVALID_DIRECTION;
            }
            else
            {
                _footpathConstructValidDirections = _footpathConstructDirection;
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
        WindowFootpathSetEnabledAndPressedWidgets();
    });
    GameActions::Execute(&footpathPlaceAction);
}

/**
 *
 *  rct2: 0x006A78EF
 */
static void FootpathRemoveTileElement(TileElement* tileElement)
{
    auto z = tileElement->GetBaseZ();
    if (tileElement->AsPath()->IsSloped())
    {
        uint8_t slopeDirection = tileElement->AsPath()->GetSlopeDirection();
        slopeDirection = direction_reverse(slopeDirection);
        if (slopeDirection == _footpathConstructDirection)
        {
            z += PATH_HEIGHT_STEP;
        }
    }

    // Find a connected edge
    int32_t edge = direction_reverse(_footpathConstructDirection);
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
    _footpathConstructDirection = edge;
    _footpathConstructValidDirections = INVALID_DIRECTION;
}

/**
 *
 *  rct2: 0x006A7873
 */
static TileElement* FootpathGetTileElementToRemove()
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
        if (tileElement->GetType() == TileElementType::Path)
        {
            if (tileElement->GetBaseZ() == z)
            {
                if (tileElement->AsPath()->IsSloped())
                {
                    if (direction_reverse(tileElement->AsPath()->GetSlopeDirection()) != _footpathConstructDirection)
                    {
                        continue;
                    }
                }

                return tileElement;
            }
            if (tileElement->GetBaseZ() == zLow)
            {
                if (!tileElement->AsPath()->IsSloped())
                {
                    if ((tileElement->AsPath()->GetSlopeDirection()) == _footpathConstructDirection)
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
static void WindowFootpathRemove()
{
    TileElement* tileElement;

    _window_footpath_cost = MONEY32_UNDEFINED;
    footpath_provisional_update();

    tileElement = FootpathGetTileElementToRemove();
    if (tileElement != nullptr)
    {
        FootpathRemoveTileElement(tileElement);
    }

    WindowFootpathSetEnabledAndPressedWidgets();
}

/**
 *
 *  rct2: 0x006A855C
 */
static void WindowFootpathSetEnabledAndPressedWidgets()
{
    rct_window* w = window_find_by_class(WC_FOOTPATH);
    if (w == nullptr)
    {
        return;
    }

    if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
    {
        map_invalidate_map_selection_tiles();
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        gMapSelectFlags |= MAP_SELECT_FLAG_GREEN;

        int32_t direction = _footpathConstructDirection;
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
    if (_footpathConstructionMode >= PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
    {
        // Set pressed directional widget
        int32_t direction = (_footpathConstructDirection + currentRotation) & 3;
        pressedWidgets |= (1LL << (WIDX_DIRECTION_NW + direction));

        // Set pressed slope widget
        int32_t slope = gFootpathConstructSlope;
        if (slope == TILE_ELEMENT_SLOPE_SE_SIDE_UP)
        {
            pressedWidgets |= (1ULL << WIDX_SLOPEDOWN);
        }
        else if (slope == TILE_ELEMENT_SLOPE_FLAT)
        {
            pressedWidgets |= (1ULL << WIDX_LEVEL);
        }
        else
        {
            pressedWidgets |= (1ULL << WIDX_SLOPEUP);
        }

        // Enable / disable directional widgets
        direction = _footpathConstructValidDirections;
        if (direction != INVALID_DIRECTION)
        {
            disabledWidgets |= (1ULL << WIDX_DIRECTION_NW) | (1ULL << WIDX_DIRECTION_NE) | (1ULL << WIDX_DIRECTION_SW)
                | (1ULL << WIDX_DIRECTION_SE);

            direction = (direction + currentRotation) & 3;
            disabledWidgets &= ~(1 << (WIDX_DIRECTION_NW + direction));
        }
    }
    else
    {
        // Disable all bridge mode widgets
        disabledWidgets |= (1ULL << WIDX_DIRECTION_GROUP) | (1ULL << WIDX_DIRECTION_NW) | (1ULL << WIDX_DIRECTION_NE)
            | (1ULL << WIDX_DIRECTION_SW) | (1ULL << WIDX_DIRECTION_SE) | (1ULL << WIDX_SLOPE_GROUP) | (1ULL << WIDX_SLOPEDOWN)
            | (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPEUP) | (1ULL << WIDX_CONSTRUCT) | (1ULL << WIDX_REMOVE);
    }

    w->pressed_widgets = pressedWidgets;
    w->disabled_widgets = disabledWidgets;
    w->Invalidate();
}

/**
 *
 *  rct2: 0x006A7B20
 */
static void FootpathGetNextPathInfo(ObjectEntryIndex* type, CoordsXYZ& footpathLoc, int32_t* slope)
{
    auto direction = _footpathConstructDirection;
    footpathLoc.x = gFootpathConstructFromPosition.x + CoordsDirectionDelta[direction].x;
    footpathLoc.y = gFootpathConstructFromPosition.y + CoordsDirectionDelta[direction].y;
    footpathLoc.z = gFootpathConstructFromPosition.z;
    if (type != nullptr)
    {
        *type = gFootpathSelection.GetSelectedSurface();
    }
    *slope = TILE_ELEMENT_SLOPE_FLAT;
    if (gFootpathConstructSlope != 0)
    {
        *slope = _footpathConstructDirection | TILE_ELEMENT_SLOPE_S_CORNER_UP;
        if (gFootpathConstructSlope != 2)
        {
            footpathLoc.z -= PATH_HEIGHT_STEP;
            *slope ^= TILE_ELEMENT_SLOPE_E_CORNER_UP;
        }
    }
}

static ObjectEntryIndex FootpathGetDefaultSurface(bool queue)
{
    bool showEditorPaths = ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode);
    for (ObjectEntryIndex i = 0; i < MAX_FOOTPATH_SURFACE_OBJECTS; i++)
    {
        auto pathEntry = GetPathSurfaceEntry(i);
        if (pathEntry != nullptr)
        {
            if (!showEditorPaths && (pathEntry->Flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR))
            {
                continue;
            }
            if (queue == ((pathEntry->Flags & FOOTPATH_ENTRY_FLAG_IS_QUEUE) != 0))
            {
                return i;
            }
        }
    }
    return OBJECT_ENTRY_INDEX_NULL;
}

static ObjectEntryIndex FootpathGetDefaultRailings()
{
    for (ObjectEntryIndex i = 0; i < MAX_FOOTPATH_RAILINGS_OBJECTS; i++)
    {
        const auto* railingEntry = GetPathRailingsEntry(i);
        if (railingEntry != nullptr)
        {
            return i;
        }
    }
    return OBJECT_ENTRY_INDEX_NULL;
}

static bool FootpathIsSurfaceEntryOkay(ObjectEntryIndex index, bool queue)
{
    auto pathEntry = GetPathSurfaceEntry(index);
    if (pathEntry != nullptr)
    {
        bool showEditorPaths = ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode);
        if (!showEditorPaths && (pathEntry->Flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR))
        {
            return false;
        }
        if (queue == ((pathEntry->Flags & FOOTPATH_ENTRY_FLAG_IS_QUEUE) != 0))
        {
            return true;
        }
    }
    return false;
}

static bool FootpathIsLegacyPathEntryOkay(ObjectEntryIndex index)
{
    bool showEditorPaths = ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode);
    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
    auto footpathObj = static_cast<FootpathObject*>(objManager.GetLoadedObject(ObjectType::Paths, index));
    if (footpathObj != nullptr)
    {
        auto pathEntry = reinterpret_cast<rct_footpath_entry*>(footpathObj->GetLegacyData());
        return showEditorPaths || !(pathEntry->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR);
    }
    return false;
}

static ObjectEntryIndex FootpathGetDefaultLegacyPath()
{
    for (ObjectEntryIndex i = 0; i < MAX_PATH_OBJECTS; i++)
    {
        if (FootpathIsLegacyPathEntryOkay(i))
        {
            return i;
        }
    }
    return OBJECT_ENTRY_INDEX_NULL;
}

static bool FootpathSelectDefault()
{
    // Select default footpath
    auto surfaceIndex = FootpathGetDefaultSurface(false);
    if (FootpathIsSurfaceEntryOkay(gFootpathSelection.NormalSurface, false))
    {
        surfaceIndex = gFootpathSelection.NormalSurface;
    }

    // Select default queue
    auto queueIndex = FootpathGetDefaultSurface(true);
    if (FootpathIsSurfaceEntryOkay(gFootpathSelection.QueueSurface, true))
    {
        queueIndex = gFootpathSelection.QueueSurface;
    }

    // Select default railing
    auto railingIndex = FootpathGetDefaultRailings();
    const auto* railingEntry = GetPathRailingsEntry(gFootpathSelection.Railings);
    if (railingEntry != nullptr)
    {
        railingIndex = gFootpathSelection.Railings;
    }

    // Select default legacy path
    auto legacyPathIndex = FootpathGetDefaultLegacyPath();
    if (gFootpathSelection.LegacyPath != OBJECT_ENTRY_INDEX_NULL)
    {
        if (FootpathIsLegacyPathEntryOkay(gFootpathSelection.LegacyPath))
        {
            // Keep legacy path selected
            legacyPathIndex = gFootpathSelection.LegacyPath;
        }
        else
        {
            // Reset legacy path, we default to a surface (if there are any)
            gFootpathSelection.LegacyPath = OBJECT_ENTRY_INDEX_NULL;
        }
    }

    if (surfaceIndex == OBJECT_ENTRY_INDEX_NULL)
    {
        if (legacyPathIndex == OBJECT_ENTRY_INDEX_NULL)
        {
            // No surfaces or legacy paths available
            return false;
        }

        // No surfaces available, so default to legacy path
        gFootpathSelection.LegacyPath = legacyPathIndex;
    }

    gFootpathSelection.NormalSurface = surfaceIndex;
    gFootpathSelection.QueueSurface = queueIndex;
    gFootpathSelection.Railings = railingIndex;
    return true;
}

static PathConstructFlags FootpathCreateConstructFlags(ObjectEntryIndex& type)
{
    PathConstructFlags pathConstructFlags = 0;
    if (gFootpathSelection.IsQueueSelected)
        pathConstructFlags |= PathConstructFlag::IsQueue;
    if (gFootpathSelection.LegacyPath != OBJECT_ENTRY_INDEX_NULL)
    {
        pathConstructFlags |= PathConstructFlag::IsLegacyPathObject;
        type = gFootpathSelection.LegacyPath;
    }
    return pathConstructFlags;
}

void window_footpath_keyboard_shortcut_turn_left()
{
    rct_window* w = window_find_by_class(WC_FOOTPATH);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_DIRECTION_NW) || WidgetIsDisabled(w, WIDX_DIRECTION_NE)
        || WidgetIsDisabled(w, WIDX_DIRECTION_SW) || WidgetIsDisabled(w, WIDX_DIRECTION_SE) || _footpathConstructionMode != 2)
    {
        return;
    }
    int32_t currentRotation = get_current_rotation();
    int32_t turnedRotation = _footpathConstructDirection - currentRotation + (currentRotation % 2 == 1 ? 1 : -1);
    WindowFootpathMousedownDirection(turnedRotation);
}

void window_footpath_keyboard_shortcut_turn_right()
{
    rct_window* w = window_find_by_class(WC_FOOTPATH);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_DIRECTION_NW) || WidgetIsDisabled(w, WIDX_DIRECTION_NE)
        || WidgetIsDisabled(w, WIDX_DIRECTION_SW) || WidgetIsDisabled(w, WIDX_DIRECTION_SE) || _footpathConstructionMode != 2)
    {
        return;
    }
    int32_t currentRotation = get_current_rotation();
    int32_t turnedRotation = _footpathConstructDirection - currentRotation + (currentRotation % 2 == 1 ? -1 : 1);
    WindowFootpathMousedownDirection(turnedRotation);
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

    WindowFootpathRemove();
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

void WindowFootpathResetSelectedPath()
{
    gFootpathSelection = {};
}
