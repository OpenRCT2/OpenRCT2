/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <iterator>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/ChangeMapSizeAction.h>
#include <openrct2/actions/LandSetRightsAction.h>
#include <openrct2/actions/PlaceParkEntranceAction.h>
#include <openrct2/actions/PlacePeepSpawnAction.h>
#include <openrct2/actions/SurfaceSetStyleAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/entity/EntityList.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrainManager.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/world/Entrance.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/Surface.h>
#include <vector>

static constexpr uint16_t MapColour2(uint8_t colourA, uint8_t colourB)
{
    return (colourA << 8) | colourB;
}
static constexpr uint16_t MapColour(uint8_t colour)
{
    return MapColour2(colour, colour);
}
static constexpr uint16_t MapColourUnowned(uint16_t colour)
{
    return MapColour2((colour & 0xFF00) >> 8, PALETTE_INDEX_10);
}

constexpr int32_t MAP_WINDOW_MAP_SIZE = MAXIMUM_MAP_SIZE_TECHNICAL * 2;

static constexpr const rct_string_id WINDOW_TITLE = STR_MAP_LABEL;
static constexpr const int32_t WH = 259;
static constexpr const int32_t WW = 245;

constexpr uint8_t DefaultPeepMapColour = PALETTE_INDEX_20;
constexpr uint8_t GuestMapColour = PALETTE_INDEX_172;
constexpr uint8_t GuestMapColourAlternate = PALETTE_INDEX_21;
constexpr uint8_t StaffMapColour = PALETTE_INDEX_138;
constexpr uint8_t StaffMapColourAlternate = PALETTE_INDEX_10;

// Some functions manipulate coordinates on the map. These are the coordinates of the pixels in the
// minimap. In order to distinguish those from actual coordinates, we use a separate name.
using MapCoordsXY = TileCoordsXY;

enum
{
    PAGE_PEEPS,
    PAGE_RIDES
};

enum WindowMapWidgetIdx
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_RESIZE = 3,
    WIDX_PEOPLE_TAB = 4,
    WIDX_RIDES_TAB = 5,
    WIDX_MAP = 6,
    WIDX_MAP_SIZE_SPINNER = 7,
    WIDX_MAP_SIZE_SPINNER_UP = 8,
    WIDX_MAP_SIZE_SPINNER_DOWN = 9,
    WIDX_SET_LAND_RIGHTS = 10,
    WIDX_BUILD_PARK_ENTRANCE = 11,
    WIDX_PEOPLE_STARTING_POSITION = 12,
    WIDX_LAND_TOOL = 13,
    WIDX_LAND_TOOL_SMALLER = 14,
    WIDX_LAND_TOOL_LARGER = 15,
    WIDX_LAND_OWNED_CHECKBOX = 16,
    WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX = 17,
    WIDX_LAND_SALE_CHECKBOX = 18,
    WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX = 19,
    WIDX_ROTATE_90 = 20,
    WIDX_MAP_GENERATOR = 21
};

validate_global_widx(WC_MAP, WIDX_ROTATE_90);

// clang-format off
static rct_widget window_map_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget        ({  0,  43}, {245, 215}, WindowWidgetType::Resize,    WindowColour::Secondary                                                                                  ),
    MakeRemapWidget   ({  3,  17}, { 31,  27}, WindowWidgetType::ColourBtn, WindowColour::Secondary, SPR_TAB,                         STR_SHOW_PEOPLE_ON_MAP_TIP                     ),
    MakeRemapWidget   ({ 34,  17}, { 31,  27}, WindowWidgetType::ColourBtn, WindowColour::Secondary, SPR_TAB,                         STR_SHOW_RIDES_STALLS_ON_MAP_TIP               ),
    MakeWidget        ({  3,  46}, {239, 180}, WindowWidgetType::Scroll,    WindowColour::Secondary, SCROLL_BOTH                                                                     ),
    MakeSpinnerWidgets({104, 229}, { 95,  12}, WindowWidgetType::Spinner,   WindowColour::Secondary, STR_MAP_SIZE_VALUE                                                              ), // NB: 3 widgets
    MakeWidget        ({  4,   1}, { 24,  24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, SPR_BUY_LAND_RIGHTS,             STR_SELECT_PARK_OWNED_LAND_TIP                 ),
    MakeWidget        ({  4,   1}, { 24,  24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, SPR_PARK_ENTRANCE,               STR_BUILD_PARK_ENTRANCE_TIP                    ),
    MakeWidget        ({ 28,   1}, { 24,  24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, 0xFFFFFFFF,                      STR_SET_STARTING_POSITIONS_TIP                 ),
    MakeWidget        ({  4,  17}, { 44,  32}, WindowWidgetType::ImgBtn,    WindowColour::Secondary, SPR_LAND_TOOL_SIZE_0                                                            ),
    MakeRemapWidget   ({  5,  18}, { 16,  16}, WindowWidgetType::TrnBtn,    WindowColour::Secondary, SPR_LAND_TOOL_DECREASE,          STR_ADJUST_SMALLER_LAND_TIP                    ),
    MakeRemapWidget   ({ 31,  32}, { 16,  16}, WindowWidgetType::TrnBtn,    WindowColour::Secondary, SPR_LAND_TOOL_INCREASE,          STR_ADJUST_LARGER_LAND_TIP                     ),
    MakeWidget        ({ 58, 197}, {184,  12}, WindowWidgetType::Checkbox,  WindowColour::Secondary, STR_LAND_OWNED,                  STR_SET_LAND_TO_BE_OWNED_TIP                   ),
    MakeWidget        ({ 58, 197}, {184,  12}, WindowWidgetType::Checkbox,  WindowColour::Secondary, STR_CONSTRUCTION_RIGHTS_OWNED,   STR_SET_CONSTRUCTION_RIGHTS_TO_BE_OWNED_TIP    ),
    MakeWidget        ({ 58, 197}, {184,  12}, WindowWidgetType::Checkbox,  WindowColour::Secondary, STR_LAND_SALE,                   STR_SET_LAND_TO_BE_AVAILABLE_TIP               ),
    MakeWidget        ({ 58, 197}, {174,  12}, WindowWidgetType::Checkbox,  WindowColour::Secondary, STR_CONSTRUCTION_RIGHTS_SALE,    STR_SET_CONSTRUCTION_RIGHTS_TO_BE_AVAILABLE_TIP),
    MakeWidget        ({218,  45}, { 24,  24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, SPR_ROTATE_ARROW,                STR_ROTATE_OBJECTS_90                          ),
    MakeWidget        ({110, 189}, {131,  14}, WindowWidgetType::Button,    WindowColour::Secondary, STR_MAPGEN_WINDOW_TITLE,         STR_MAP_GENERATOR_TIP                          ),
    WIDGETS_END,
};

// used in transforming viewport view coordinates to minimap coordinates
// rct2: 0x00981BBC
static constexpr const ScreenCoordsXY MiniMapOffsets[] = {
    {     MAXIMUM_MAP_SIZE_TECHNICAL - 8,                              0 },
    { 2 * MAXIMUM_MAP_SIZE_TECHNICAL - 8,     MAXIMUM_MAP_SIZE_TECHNICAL },
    {     MAXIMUM_MAP_SIZE_TECHNICAL - 8, 2 * MAXIMUM_MAP_SIZE_TECHNICAL },
    {                              0 - 8,     MAXIMUM_MAP_SIZE_TECHNICAL },
};
// clang-format on

/** rct2: 0x00981BCC */
static constexpr const uint16_t RideKeyColours[] = {
    MapColour(PALETTE_INDEX_61),  // COLOUR_KEY_RIDE
    MapColour(PALETTE_INDEX_42),  // COLOUR_KEY_FOOD
    MapColour(PALETTE_INDEX_20),  // COLOUR_KEY_DRINK
    MapColour(PALETTE_INDEX_209), // COLOUR_KEY_SOUVENIR
    MapColour(PALETTE_INDEX_136), // COLOUR_KEY_KIOSK
    MapColour(PALETTE_INDEX_102), // COLOUR_KEY_FIRST_AID
    MapColour(PALETTE_INDEX_55),  // COLOUR_KEY_CASH_MACHINE
    MapColour(PALETTE_INDEX_161), // COLOUR_KEY_TOILETS
};

static void WindowMapClose(rct_window* w);
static void WindowMapResize(rct_window* w);
static void WindowMapMouseup(rct_window* w, rct_widgetindex widgetIndex);
static void WindowMapMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowMapUpdate(rct_window* w);
static void WindowMapToolupdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowMapTooldown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowMapTooldrag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void WindowMapToolabort(rct_window* w, rct_widgetindex widgetIndex);
static void WindowMapScrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height);
static void WindowMapScrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowMapTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text);
static void WindowMapInvalidate(rct_window* w);
static void WindowMapPaint(rct_window* w, rct_drawpixelinfo* dpi);
static void WindowMapScrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex);

static rct_window_event_list window_map_events([](auto& events) {
    events.close = &WindowMapClose;
    events.mouse_up = &WindowMapMouseup;
    events.resize = &WindowMapResize;
    events.mouse_down = &WindowMapMousedown;
    events.update = &WindowMapUpdate;
    events.tool_update = &WindowMapToolupdate;
    events.tool_down = &WindowMapTooldown;
    events.tool_drag = &WindowMapTooldrag;
    events.tool_abort = &WindowMapToolabort;
    events.get_scroll_size = &WindowMapScrollgetsize;
    events.scroll_mousedown = &WindowMapScrollmousedown;
    events.scroll_mousedrag = &WindowMapScrollmousedown;
    events.text_input = &WindowMapTextinput;
    events.invalidate = &WindowMapInvalidate;
    events.paint = &WindowMapPaint;
    events.scroll_paint = &WindowMapScrollpaint;
});

/** rct2: 0x00F1AD61 */
static uint8_t _activeTool;

/** rct2: 0x00F1AD6C */
static uint32_t _currentLine;

/** rct2: 0x00F1AD68 */
static std::vector<uint8_t> _mapImageData;

static uint16_t _landRightsToolSize;

static void WindowMapInitMap();
static void WindowMapCentreOnViewPoint();
static void WindowMapShowDefaultScenarioEditorButtons(rct_window* w);
static void WindowMapDrawTabImages(rct_window* w, rct_drawpixelinfo* dpi);
static void WindowMapPaintPeepOverlay(rct_drawpixelinfo* dpi);
static void WindowMapPaintTrainOverlay(rct_drawpixelinfo* dpi);
static void WindowMapPaintHudRectangle(rct_drawpixelinfo* dpi);
static void WindowMapInputsizeLand(rct_window* w);
static void WindowMapInputsizeMap(rct_window* w);

static void WindowMapSetLandRightsToolUpdate(const ScreenCoordsXY& screenCoords);
static void WindowMapPlaceParkEntranceToolUpdate(const ScreenCoordsXY& screenCoords);
static void WindowMapSetPeepSpawnToolUpdate(const ScreenCoordsXY& screenCoords);
static void WindowMapPlaceParkEntranceToolDown(const ScreenCoordsXY& screenCoords);
static void WindowMapSetPeepSpawnToolDown(const ScreenCoordsXY& screenCoords);
static void MapWindowIncreaseMapSize();
static void MapWindowDecreaseMapSize();
static void MapWindowSetPixels(rct_window* w);

static CoordsXY MapWindowScreenToMap(ScreenCoordsXY screenCoords);

/**
 *
 *  rct2: 0x0068C88A
 */
rct_window* WindowMapOpen()
{
    rct_window* w;

    // Check if window is already open
    w = window_bring_to_front_by_class(WC_MAP);
    if (w != nullptr)
    {
        w->selected_tab = 0;
        w->list_information_type = 0;
        return w;
    }

    try
    {
        _mapImageData.resize(MAP_WINDOW_MAP_SIZE * MAP_WINDOW_MAP_SIZE);
    }
    catch (const std::bad_alloc&)
    {
        return nullptr;
    }

    w = WindowCreateAutoPos(245, 259, &window_map_events, WC_MAP, WF_10);
    w->widgets = window_map_widgets;

    w->hold_down_widgets = (1ULL << WIDX_MAP_SIZE_SPINNER_UP) | (1ULL << WIDX_MAP_SIZE_SPINNER_DOWN)
        | (1ULL << WIDX_LAND_TOOL_LARGER) | (1ULL << WIDX_LAND_TOOL_SMALLER);

    WindowInitScrollWidgets(w);

    w->map.rotation = get_current_rotation();

    WindowMapInitMap();
    gWindowSceneryRotation = 0;
    WindowMapCentreOnViewPoint();

    // Reset land rights tool size
    _landRightsToolSize = 1;

    return w;
}

void WindowMapReset()
{
    rct_window* w;

    // Check if window is even opened
    w = window_bring_to_front_by_class(WC_MAP);
    if (w == nullptr)
    {
        return;
    }

    WindowMapInitMap();
    WindowMapCentreOnViewPoint();
}

/**
 *
 *  rct2: 0x0068D0F1
 */
static void WindowMapClose(rct_window* w)
{
    _mapImageData.clear();
    _mapImageData.shrink_to_fit();
    if ((input_test_flag(INPUT_FLAG_TOOL_ACTIVE)) && gCurrentToolWidget.window_classification == w->classification
        && gCurrentToolWidget.window_number == w->number)
    {
        tool_cancel();
    }
}

/**
 *
 *  rct2: 0x0068CFC1
 */
static void WindowMapMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_SET_LAND_RIGHTS:
            w->Invalidate();
            if (tool_set(w, widgetIndex, Tool::UpArrow))
                break;
            _activeTool = 2;
            // Prevent mountain tool size.
            _landRightsToolSize = std::max<uint16_t>(MINIMUM_TOOL_SIZE, _landRightsToolSize);
            show_gridlines();
            show_land_rights();
            show_construction_rights();
            break;
        case WIDX_LAND_OWNED_CHECKBOX:
            _activeTool ^= 2;

            if (_activeTool & 2)
                _activeTool &= 0xF2;

            w->Invalidate();
            break;
        case WIDX_LAND_SALE_CHECKBOX:
            _activeTool ^= 8;

            if (_activeTool & 8)
                _activeTool &= 0xF8;

            w->Invalidate();
            break;
        case WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX:
            _activeTool ^= 1;

            if (_activeTool & 1)
                _activeTool &= 0xF1;

            w->Invalidate();
            break;
        case WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX:
            _activeTool ^= 4;

            if (_activeTool & 4)
                _activeTool &= 0xF4;

            w->Invalidate();
            break;
        case WIDX_BUILD_PARK_ENTRANCE:
            w->Invalidate();
            if (tool_set(w, widgetIndex, Tool::UpArrow))
                break;

            gParkEntranceGhostExists = false;
            input_set_flag(INPUT_FLAG_6, true);

            show_gridlines();
            show_land_rights();
            show_construction_rights();
            break;
        case WIDX_ROTATE_90:
            gWindowSceneryRotation = (gWindowSceneryRotation + 1) & 3;
            break;
        case WIDX_PEOPLE_STARTING_POSITION:
            if (tool_set(w, widgetIndex, Tool::UpArrow))
                break;

            show_gridlines();
            show_land_rights();
            show_construction_rights();
            break;
        case WIDX_LAND_TOOL:
            WindowMapInputsizeLand(w);
            break;
        case WIDX_MAP_SIZE_SPINNER:
            WindowMapInputsizeMap(w);
            break;
        case WIDX_MAP_GENERATOR:
            context_open_window(WC_MAPGEN);
            break;
        default:
            if (widgetIndex >= WIDX_PEOPLE_TAB && widgetIndex <= WIDX_RIDES_TAB)
            {
                widgetIndex -= WIDX_PEOPLE_TAB;
                if (widgetIndex == w->selected_tab)
                    break;

                w->selected_tab = widgetIndex;
                w->list_information_type = 0;
            }
    }
}

/**
 *
 *  rct2: 0x0068D7DC
 */
static void WindowMapResize(rct_window* w)
{
    w->flags |= WF_RESIZABLE;
    w->min_width = 245;
    w->max_width = 800;
    w->min_height = 259;
    w->max_height = 560;
}

/**
 *
 *  rct2: 0x0068D040
 */
static void WindowMapMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_MAP_SIZE_SPINNER_UP:
            MapWindowIncreaseMapSize();
            break;
        case WIDX_MAP_SIZE_SPINNER_DOWN:
            MapWindowDecreaseMapSize();
            break;
        case WIDX_LAND_TOOL_SMALLER:
            // Decrement land rights tool size
            _landRightsToolSize = std::max(MINIMUM_TOOL_SIZE, _landRightsToolSize - 1);

            w->Invalidate();
            break;
        case WIDX_LAND_TOOL_LARGER:
            // Increment land rights tool size
            _landRightsToolSize = std::min(MAXIMUM_TOOL_SIZE, _landRightsToolSize + 1);

            w->Invalidate();
            break;
    }
}

/**
 *
 *  rct2: 0x0068D7FB
 */
static void WindowMapUpdate(rct_window* w)
{
    if (get_current_rotation() != w->map.rotation)
    {
        w->map.rotation = get_current_rotation();
        WindowMapInitMap();
        WindowMapCentreOnViewPoint();
    }

    for (int32_t i = 0; i < 16; i++)
        MapWindowSetPixels(w);

    w->Invalidate();

    // Update tab animations
    w->list_information_type++;
    switch (w->selected_tab)
    {
        case PAGE_PEEPS:
            if (w->list_information_type >= 32)
            {
                w->list_information_type = 0;
            }
            break;
        case PAGE_RIDES:
            if (w->list_information_type >= 64)
            {
                w->list_information_type = 0;
            }
            break;
    }
}

/**
 *
 *  rct2: 0x0068D093
 */
static void WindowMapToolupdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_SET_LAND_RIGHTS:
            WindowMapSetLandRightsToolUpdate(screenCoords);
            break;
        case WIDX_BUILD_PARK_ENTRANCE:
            WindowMapPlaceParkEntranceToolUpdate(screenCoords);
            break;
        case WIDX_PEOPLE_STARTING_POSITION:
            WindowMapSetPeepSpawnToolUpdate(screenCoords);
            break;
    }
}

/**
 *
 *  rct2: 0x0068D074
 */
static void WindowMapTooldown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_BUILD_PARK_ENTRANCE:
            WindowMapPlaceParkEntranceToolDown(screenCoords);
            break;
        case WIDX_PEOPLE_STARTING_POSITION:
            WindowMapSetPeepSpawnToolDown(screenCoords);
            break;
    }
}

/**
 *
 *  rct2: 0x0068D088
 */
static void WindowMapTooldrag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_SET_LAND_RIGHTS:
            if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
            {
                auto landSetRightsAction = LandSetRightsAction(
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                    LandSetRightSetting::SetOwnershipWithChecks, _activeTool << 4);
                GameActions::Execute(&landSetRightsAction);
            }
            break;
    }
}

/**
 *
 *  rct2: 0x0068D055
 */
static void WindowMapToolabort(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_SET_LAND_RIGHTS:
            w->Invalidate();
            hide_gridlines();
            hide_land_rights();
            hide_construction_rights();
            break;
        case WIDX_BUILD_PARK_ENTRANCE:
            park_entrance_remove_ghost();
            w->Invalidate();
            hide_gridlines();
            hide_land_rights();
            hide_construction_rights();
            break;
        case WIDX_PEOPLE_STARTING_POSITION:
            w->Invalidate();
            hide_gridlines();
            hide_land_rights();
            hide_construction_rights();
            break;
    }
}

/**
 *
 *  rct2: 0x0068D7CC
 */
static void WindowMapScrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    WindowMapInvalidate(w);

    *width = MAP_WINDOW_MAP_SIZE;
    *height = MAP_WINDOW_MAP_SIZE;
}

/**
 *
 *  rct2: 0x0068D726
 */
static void WindowMapScrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    CoordsXY c = MapWindowScreenToMap(screenCoords);
    auto mapCoords = CoordsXY{ std::clamp(c.x, 0, MAXIMUM_MAP_SIZE_BIG - 1), std::clamp(c.y, 0, MAXIMUM_MAP_SIZE_BIG - 1) };
    auto mapZ = tile_element_height(mapCoords);

    rct_window* mainWindow = window_get_main();
    if (mainWindow != nullptr)
    {
        window_scroll_to_location(mainWindow, { mapCoords, mapZ });
    }

    if (LandToolIsActive())
    {
        // Set land terrain
        int32_t landToolSize = std::max<int32_t>(1, gLandToolSize);
        int32_t size = (landToolSize * 32) - 32;
        int32_t radius = (landToolSize * 16) - 16;

        mapCoords = (mapCoords - CoordsXY{ radius, radius }).ToTileStart();
        map_invalidate_selection_rect();
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        gMapSelectPositionA = mapCoords;
        gMapSelectPositionB = mapCoords + CoordsXY{ size, size };
        map_invalidate_selection_rect();

        auto surfaceSetStyleAction = SurfaceSetStyleAction(
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
            gLandToolTerrainSurface, gLandToolTerrainEdge);
        GameActions::Execute(&surfaceSetStyleAction);
    }
    else if (WidgetIsActiveTool(w, WIDX_SET_LAND_RIGHTS))
    {
        // Set land rights
        int32_t landRightsToolSize = std::max<int32_t>(1, _landRightsToolSize);
        int32_t size = (landRightsToolSize * 32) - 32;
        int32_t radius = (landRightsToolSize * 16) - 16;
        mapCoords = (mapCoords - CoordsXY{ radius, radius }).ToTileStart();

        map_invalidate_selection_rect();
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        gMapSelectPositionA = mapCoords;
        gMapSelectPositionB = mapCoords + CoordsXY{ size, size };
        map_invalidate_selection_rect();

        auto landSetRightsAction = LandSetRightsAction(
            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
            LandSetRightSetting::SetOwnershipWithChecks, _activeTool << 4);
        GameActions::Execute(&landSetRightsAction);
    }
}

static void WindowMapTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    int32_t size;
    char* end;

    if (text == nullptr)
        return;

    switch (widgetIndex)
    {
        case WIDX_LAND_TOOL:
            size = strtol(text, &end, 10);
            if (*end == '\0')
            {
                size = std::clamp(size, MINIMUM_TOOL_SIZE, MAXIMUM_TOOL_SIZE);
                _landRightsToolSize = size;
                w->Invalidate();
            }
            break;
        case WIDX_MAP_SIZE_SPINNER:
            size = strtol(text, &end, 10);
            if (*end == '\0')
            {
                // The practical size is 2 lower than the technical size
                size += 2;
                size = std::clamp(size, MINIMUM_MAP_SIZE_TECHNICAL, MAXIMUM_MAP_SIZE_TECHNICAL);

                auto changeMapSizeAction = ChangeMapSizeAction(size);
                GameActions::Execute(&changeMapSizeAction);
                w->Invalidate();
            }
            break;
    }
}

/**
 *
 *  rct2: 0x0068CA8F
 */
static void WindowMapInvalidate(rct_window* w)
{
    uint64_t pressedWidgets;
    int32_t i, height;

    // Set the pressed widgets
    pressedWidgets = w->pressed_widgets;
    pressedWidgets &= (1ULL << WIDX_PEOPLE_TAB);
    pressedWidgets &= (1ULL << WIDX_RIDES_TAB);
    pressedWidgets &= (1ULL << WIDX_MAP);
    pressedWidgets &= (1ULL << WIDX_LAND_OWNED_CHECKBOX);
    pressedWidgets &= (1ULL << WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX);
    pressedWidgets &= (1ULL << WIDX_LAND_SALE_CHECKBOX);
    pressedWidgets &= (1ULL << WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX);

    pressedWidgets |= (1ULL << (WIDX_PEOPLE_TAB + w->selected_tab));
    pressedWidgets |= (1ULL << WIDX_LAND_TOOL);

    if (_activeTool & (1 << 3))
        pressedWidgets |= (1ULL << WIDX_LAND_SALE_CHECKBOX);

    if (_activeTool & (1 << 2))
        pressedWidgets |= (1ULL << WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX);

    if (_activeTool & (1 << 1))
        pressedWidgets |= (1ULL << WIDX_LAND_OWNED_CHECKBOX);

    if (_activeTool & (1 << 0))
        pressedWidgets |= (1ULL << WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX);

    w->pressed_widgets = pressedWidgets;

    // Resize widgets to window size
    w->widgets[WIDX_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_RESIZE].right = w->width - 1;
    w->widgets[WIDX_RESIZE].bottom = w->height - 1;
    w->widgets[WIDX_TITLE].right = w->width - 2;
    w->widgets[WIDX_CLOSE].left = w->width - 2 - 11;
    w->widgets[WIDX_CLOSE].right = w->width - 2 - 11 + 10;
    w->widgets[WIDX_MAP].right = w->width - 4;

    if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode)
        w->widgets[WIDX_MAP].bottom = w->height - 1 - 72;
    else if (w->selected_tab == PAGE_RIDES)
        w->widgets[WIDX_MAP].bottom = w->height - 1 - (4 * LIST_ROW_HEIGHT + 4);
    else
        w->widgets[WIDX_MAP].bottom = w->height - 1 - 14;

    w->widgets[WIDX_MAP_SIZE_SPINNER].top = w->height - 15;
    w->widgets[WIDX_MAP_SIZE_SPINNER].bottom = w->height - 4;
    w->widgets[WIDX_MAP_SIZE_SPINNER_UP].top = w->height - 14;
    w->widgets[WIDX_MAP_SIZE_SPINNER_UP].bottom = w->height - 5;
    w->widgets[WIDX_MAP_SIZE_SPINNER_DOWN].top = w->height - 14;
    w->widgets[WIDX_MAP_SIZE_SPINNER_DOWN].bottom = w->height - 5;

    w->widgets[WIDX_SET_LAND_RIGHTS].top = w->height - 70;
    w->widgets[WIDX_SET_LAND_RIGHTS].bottom = w->height - 70 + 23;
    w->widgets[WIDX_BUILD_PARK_ENTRANCE].top = w->height - 46;
    w->widgets[WIDX_BUILD_PARK_ENTRANCE].bottom = w->height - 46 + 23;
    w->widgets[WIDX_ROTATE_90].top = w->height - 46;
    w->widgets[WIDX_ROTATE_90].bottom = w->height - 46 + 23;
    w->widgets[WIDX_PEOPLE_STARTING_POSITION].top = w->height - 46;
    w->widgets[WIDX_PEOPLE_STARTING_POSITION].bottom = w->height - 46 + 23;

    w->widgets[WIDX_LAND_TOOL].top = w->height - 42;
    w->widgets[WIDX_LAND_TOOL].bottom = w->height - 42 + 30;
    w->widgets[WIDX_LAND_TOOL_SMALLER].top = w->height - 41;
    w->widgets[WIDX_LAND_TOOL_SMALLER].bottom = w->height - 41 + 15;
    w->widgets[WIDX_LAND_TOOL_LARGER].top = w->height - 27;
    w->widgets[WIDX_LAND_TOOL_LARGER].bottom = w->height - 27 + 15;

    w->widgets[WIDX_MAP_GENERATOR].top = w->height - 69;
    w->widgets[WIDX_MAP_GENERATOR].bottom = w->height - 69 + 13;

    // Land tool mode (4 checkboxes)
    height = w->height - 55;
    for (i = 0; i < 4; i++)
    {
        w->widgets[WIDX_LAND_OWNED_CHECKBOX + i].top = height;
        height += 11;
        w->widgets[WIDX_LAND_OWNED_CHECKBOX + i].bottom = height;
        height += 2;
    }

    // Disable all scenario editor related widgets
    for (i = WIDX_MAP_SIZE_SPINNER; i <= WIDX_MAP_GENERATOR; i++)
    {
        w->widgets[i].type = WindowWidgetType::Empty;
    }

    if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode)
    {
        // scenario editor: build park entrance selected, show rotate button
        if ((input_test_flag(INPUT_FLAG_TOOL_ACTIVE)) && gCurrentToolWidget.window_classification == WC_MAP
            && gCurrentToolWidget.widget_index == WIDX_BUILD_PARK_ENTRANCE)
        {
            w->widgets[WIDX_ROTATE_90].type = WindowWidgetType::FlatBtn;
        }

        // Always show set land rights button
        w->widgets[WIDX_SET_LAND_RIGHTS].type = WindowWidgetType::FlatBtn;

        // If any tool is active
        if ((input_test_flag(INPUT_FLAG_TOOL_ACTIVE)) && gCurrentToolWidget.window_classification == WC_MAP)
        {
            // if not in set land rights mode: show the default scenario editor buttons
            if (gCurrentToolWidget.widget_index != WIDX_SET_LAND_RIGHTS)
            {
                WindowMapShowDefaultScenarioEditorButtons(w);
            }
            else
            { // if in set land rights mode: show land tool buttons + modes
                w->widgets[WIDX_LAND_TOOL].type = WindowWidgetType::ImgBtn;
                w->widgets[WIDX_LAND_TOOL_SMALLER].type = WindowWidgetType::TrnBtn;
                w->widgets[WIDX_LAND_TOOL_LARGER].type = WindowWidgetType::TrnBtn;

                for (i = 0; i < 4; i++)
                    w->widgets[WIDX_LAND_OWNED_CHECKBOX + i].type = WindowWidgetType::Checkbox;

                w->widgets[WIDX_LAND_TOOL].image = LandTool::SizeToSpriteIndex(_landRightsToolSize);
            }
        }
        else
        {
            // if no tool is active: show the default scenario editor buttons
            WindowMapShowDefaultScenarioEditorButtons(w);
        }
    }
}

/**
 *
 *  rct2: 0x0068CDA9
 */
static void WindowMapPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    WindowMapDrawTabImages(w, dpi);

    auto screenCoords = w->windowPos
        + ScreenCoordsXY{ window_map_widgets[WIDX_LAND_TOOL].midX(), window_map_widgets[WIDX_LAND_TOOL].midY() };

    // Draw land tool size
    if (WidgetIsActiveTool(w, WIDX_SET_LAND_RIGHTS) && _landRightsToolSize > MAX_TOOL_SIZE_WITH_SPRITE)
    {
        auto ft = Formatter();
        ft.Add<uint16_t>(_landRightsToolSize);
        DrawTextBasic(dpi, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::CENTRE });
    }
    screenCoords.y = w->windowPos.y + window_map_widgets[WIDX_LAND_TOOL].bottom + 5;

    // People starting position (scenario editor only)
    if (w->widgets[WIDX_PEOPLE_STARTING_POSITION].type != WindowWidgetType::Empty)
    {
        screenCoords = w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_PEOPLE_STARTING_POSITION].left + 12,
                              w->widgets[WIDX_PEOPLE_STARTING_POSITION].top + 18 };
        gfx_draw_sprite(dpi, ImageId(SPR_6410, COLOUR_BRIGHT_RED, COLOUR_LIGHT_BROWN), screenCoords);
    }

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
    {
        // Render the map legend
        if (w->selected_tab == PAGE_RIDES)
        {
            screenCoords = w->windowPos + ScreenCoordsXY{ 4, w->widgets[WIDX_MAP].bottom + 2 };

            static rct_string_id mapLabels[] = {
                STR_MAP_RIDE,       STR_MAP_FOOD_STALL, STR_MAP_DRINK_STALL,  STR_MAP_SOUVENIR_STALL,
                STR_MAP_INFO_KIOSK, STR_MAP_FIRST_AID,  STR_MAP_CASH_MACHINE, STR_MAP_TOILET,
            };

            for (uint32_t i = 0; i < std::size(RideKeyColours); i++)
            {
                gfx_fill_rect(
                    dpi, { screenCoords + ScreenCoordsXY{ 0, 2 }, screenCoords + ScreenCoordsXY{ 6, 8 } }, RideKeyColours[i]);
                DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ LIST_ROW_HEIGHT, 0 }, mapLabels[i], {});
                screenCoords.y += LIST_ROW_HEIGHT;
                if (i == 3)
                {
                    screenCoords += { 118, -(LIST_ROW_HEIGHT * 4) };
                }
            }
        }
    }
    else if (!WidgetIsActiveTool(w, WIDX_SET_LAND_RIGHTS))
    {
        DrawTextBasic(
            dpi, w->windowPos + ScreenCoordsXY{ 4, w->widgets[WIDX_MAP_SIZE_SPINNER].top + 1 }, STR_MAP_SIZE, {},
            { w->colours[1] });
    }
}

/**
 *
 *  rct2: 0x0068CF23
 */
static void WindowMapScrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    gfx_clear(dpi, PALETTE_INDEX_10);

    rct_g1_element g1temp = {};
    g1temp.offset = _mapImageData.data();
    g1temp.width = MAP_WINDOW_MAP_SIZE;
    g1temp.height = MAP_WINDOW_MAP_SIZE;
    g1temp.x_offset = -8;
    g1temp.y_offset = -8;
    gfx_set_g1_element(SPR_TEMP, &g1temp);
    drawing_engine_invalidate_image(SPR_TEMP);
    gfx_draw_sprite(dpi, ImageId(SPR_TEMP), { 0, 0 });

    if (w->selected_tab == PAGE_PEEPS)
    {
        WindowMapPaintPeepOverlay(dpi);
    }
    else
    {
        WindowMapPaintTrainOverlay(dpi);
    }
    WindowMapPaintHudRectangle(dpi);
}

/**
 *
 *  rct2: 0x0068CA6C
 */
static void WindowMapInitMap()
{
    std::fill(_mapImageData.begin(), _mapImageData.end(), PALETTE_INDEX_10);
    _currentLine = 0;
}

/**
 *
 *  rct2: 0x0068C990
 */
static void WindowMapCentreOnViewPoint()
{
    rct_window* w = window_get_main();
    rct_window* w_map;
    int16_t ax, bx, cx, dx;
    int16_t bp, di;

    if (w == nullptr || w->viewport == nullptr)
        return;

    w_map = window_find_by_class(WC_MAP);
    if (w_map == nullptr)
        return;

    auto offset = MiniMapOffsets[get_current_rotation()];

    // calculate centre view point of viewport and transform it to minimap coordinates

    cx = ((w->viewport->view_width >> 1) + w->viewport->viewPos.x) >> 5;
    dx = ((w->viewport->view_height >> 1) + w->viewport->viewPos.y) >> 4;
    cx += offset.x;
    dx += offset.y;

    // calculate width and height of minimap

    ax = w_map->widgets[WIDX_MAP].width() - 11;
    bx = w_map->widgets[WIDX_MAP].height() - 11;
    bp = ax;
    di = bx;

    ax >>= 1;
    bx >>= 1;
    cx = std::max(cx - ax, 0);
    dx = std::max(dx - bx, 0);

    bp = w_map->scrolls[0].h_right - bp;
    di = w_map->scrolls[0].v_bottom - di;

    if (bp < 0 && (bp - cx) < 0)
        cx = 0;

    if (di < 0 && (di - dx) < 0)
        dx = 0;

    w_map->scrolls[0].h_left = cx;
    w_map->scrolls[0].v_top = dx;
    WidgetScrollUpdateThumbs(w_map, WIDX_MAP);
}

/**
 *
 *  rct2: 0x0068CD35 (part of 0x0068CA8F)
 */
static void WindowMapShowDefaultScenarioEditorButtons(rct_window* w)
{
    w->widgets[WIDX_BUILD_PARK_ENTRANCE].type = WindowWidgetType::FlatBtn;
    w->widgets[WIDX_PEOPLE_STARTING_POSITION].type = WindowWidgetType::FlatBtn;
    w->widgets[WIDX_MAP_SIZE_SPINNER].type = WindowWidgetType::Spinner;
    w->widgets[WIDX_MAP_SIZE_SPINNER_UP].type = WindowWidgetType::Button;
    w->widgets[WIDX_MAP_SIZE_SPINNER_DOWN].type = WindowWidgetType::Button;

    // Only show this in the scenario editor, even when in sandbox mode.
    if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
        w->widgets[WIDX_MAP_GENERATOR].type = WindowWidgetType::Button;

    auto ft = Formatter::Common();
    ft.Increment(2);
    ft.Add<uint16_t>(gMapSize - 2);
}

static void WindowMapInputsizeLand(rct_window* w)
{
    Formatter ft;
    ft.Add<int16_t>(MINIMUM_TOOL_SIZE);
    ft.Add<int16_t>(MAXIMUM_TOOL_SIZE);
    WindowTextInputOpen(w, WIDX_LAND_TOOL, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, ft, STR_NONE, STR_NONE, 3);
}

static void WindowMapInputsizeMap(rct_window* w)
{
    Formatter ft;
    ft.Add<int16_t>(MINIMUM_MAP_SIZE_PRACTICAL);
    ft.Add<int16_t>(MAXIMUM_MAP_SIZE_PRACTICAL);
    WindowTextInputOpen(w, WIDX_MAP_SIZE_SPINNER, STR_MAP_SIZE_2, STR_ENTER_MAP_SIZE, ft, STR_NONE, STR_NONE, 4);
}

static void WindowMapDrawTabImages(rct_window* w, rct_drawpixelinfo* dpi)
{
    uint32_t image;

    // Guest tab image (animated)
    image = SPR_TAB_GUESTS_0;
    if (w->selected_tab == PAGE_PEEPS)
        image += w->list_information_type / 4;

    gfx_draw_sprite(
        dpi, ImageId(image),
        w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_PEOPLE_TAB].left, w->widgets[WIDX_PEOPLE_TAB].top });

    // Ride/stall tab image (animated)
    image = SPR_TAB_RIDE_0;
    if (w->selected_tab == PAGE_RIDES)
        image += w->list_information_type / 4;

    gfx_draw_sprite(
        dpi, ImageId(image), w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_RIDES_TAB].left, w->widgets[WIDX_RIDES_TAB].top });
}

/**
 *
 * part of window_map_paint_peep_overlay and window_map_paint_train_overlay
 */
static MapCoordsXY WindowMapTransformToMapCoords(CoordsXY c)
{
    int32_t x = c.x, y = c.y;

    switch (get_current_rotation())
    {
        case 3:
            std::swap(x, y);
            x = MAXIMUM_MAP_SIZE_BIG - 1 - x;
            break;
        case 2:
            x = MAXIMUM_MAP_SIZE_BIG - 1 - x;
            y = MAXIMUM_MAP_SIZE_BIG - 1 - y;
            break;
        case 1:
            std::swap(x, y);
            y = MAXIMUM_MAP_SIZE_BIG - 1 - y;
            break;
        case 0:
            break;
    }
    x /= 32;
    y /= 32;

    return { -x + y + MAXIMUM_MAP_SIZE_TECHNICAL - 8, x + y - 8 };
}

static void DrawMapPeepPixel(Peep* peep, const uint8_t flashColour, rct_drawpixelinfo* dpi)
{
    if (peep->x == LOCATION_NULL)
        return;

    MapCoordsXY c = WindowMapTransformToMapCoords({ peep->x, peep->y });
    auto leftTop = ScreenCoordsXY{ c.x, c.y };
    auto rightBottom = leftTop;
    uint8_t colour = DefaultPeepMapColour;
    if (EntityGetFlashing(peep))
    {
        colour = flashColour;
        // If flashing then map peep pixel size is increased (by moving left top downwards)
        if (flashColour != DefaultPeepMapColour)
        {
            leftTop.x--;
        }
    }

    gfx_fill_rect(dpi, { leftTop, rightBottom }, colour);
}

static uint8_t MapGetGuestFlashColour()
{
    uint8_t colour = DefaultPeepMapColour;
    if ((gWindowMapFlashingFlags & MapFlashingFlags::FlashGuests) != 0)
    {
        colour = GuestMapColour;
        if ((gWindowMapFlashingFlags & MapFlashingFlags::SwitchColour) == 0)
            colour = GuestMapColourAlternate;
    }
    return colour;
}

static uint8_t MapGetStaffFlashColour()
{
    uint8_t colour = DefaultPeepMapColour;
    if ((gWindowMapFlashingFlags & MapFlashingFlags::FlashStaff) != 0)
    {
        colour = StaffMapColour;
        if ((gWindowMapFlashingFlags & MapFlashingFlags::SwitchColour) == 0)
            colour = StaffMapColourAlternate;
    }
    return colour;
}

/**
 *
 *  rct2: 0x0068DADA
 */
static void WindowMapPaintPeepOverlay(rct_drawpixelinfo* dpi)
{
    auto flashColour = MapGetGuestFlashColour();
    for (auto guest : EntityList<Guest>())
    {
        DrawMapPeepPixel(guest, flashColour, dpi);
    }
    flashColour = MapGetStaffFlashColour();
    for (auto staff : EntityList<Staff>())
    {
        DrawMapPeepPixel(staff, flashColour, dpi);
    }
}

/**
 *
 *  rct2: 0x0068DBC1
 */
static void WindowMapPaintTrainOverlay(rct_drawpixelinfo* dpi)
{
    for (auto train : TrainManager::View())
    {
        for (Vehicle* vehicle = train; vehicle != nullptr; vehicle = GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
        {
            if (vehicle->x == LOCATION_NULL)
                continue;

            MapCoordsXY c = WindowMapTransformToMapCoords({ vehicle->x, vehicle->y });

            gfx_fill_rect(dpi, { { c.x, c.y }, { c.x, c.y } }, PALETTE_INDEX_171);
        }
    }
}

/**
 * The call to gfx_fill_rect was originally wrapped in sub_68DABD which made sure that arguments were ordered correctly,
 * but it doesn't look like it's ever necessary here so the call was removed.
 *
 *  rct2: 0x0068D8CE
 */
static void WindowMapPaintHudRectangle(rct_drawpixelinfo* dpi)
{
    rct_window* main_window = window_get_main();
    if (main_window == nullptr)
        return;

    rct_viewport* viewport = main_window->viewport;
    if (viewport == nullptr)
        return;

    auto offset = MiniMapOffsets[get_current_rotation()];
    auto leftTop = ScreenCoordsXY{ (viewport->viewPos.x >> 5) + offset.x, (viewport->viewPos.y >> 4) + offset.y };
    auto rightBottom = ScreenCoordsXY{ ((viewport->viewPos.x + viewport->view_width) >> 5) + offset.x,
                                       ((viewport->viewPos.y + viewport->view_height) >> 4) + offset.y };
    auto rightTop = ScreenCoordsXY{ rightBottom.x, leftTop.y };
    auto leftBottom = ScreenCoordsXY{ leftTop.x, rightBottom.y };

    // top horizontal lines
    gfx_fill_rect(dpi, { leftTop, leftTop + ScreenCoordsXY{ 3, 0 } }, PALETTE_INDEX_56);
    gfx_fill_rect(dpi, { rightTop - ScreenCoordsXY{ 3, 0 }, rightTop }, PALETTE_INDEX_56);

    // left vertical lines
    gfx_fill_rect(dpi, { leftTop, leftTop + ScreenCoordsXY{ 0, 3 } }, PALETTE_INDEX_56);
    gfx_fill_rect(dpi, { leftBottom - ScreenCoordsXY{ 0, 3 }, leftBottom }, PALETTE_INDEX_56);

    // bottom horizontal lines
    gfx_fill_rect(dpi, { leftBottom, leftBottom + ScreenCoordsXY{ 3, 0 } }, PALETTE_INDEX_56);
    gfx_fill_rect(dpi, { rightBottom - ScreenCoordsXY{ 3, 0 }, rightBottom }, PALETTE_INDEX_56);

    // right vertical lines
    gfx_fill_rect(dpi, { rightTop, rightTop + ScreenCoordsXY{ 0, 3 } }, PALETTE_INDEX_56);
    gfx_fill_rect(dpi, { rightBottom - ScreenCoordsXY{ 0, 3 }, rightBottom }, PALETTE_INDEX_56);
}

/**
 *
 *  rct2: 0x0068D24E
 */
static void WindowMapSetLandRightsToolUpdate(const ScreenCoordsXY& screenCoords)
{
    rct_viewport* viewport;

    map_invalidate_selection_rect();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    auto mapCoords = screen_get_map_xy(screenCoords, &viewport);
    if (!mapCoords.has_value())
        return;

    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
    gMapSelectType = MAP_SELECT_TYPE_FULL;

    int32_t landRightsToolSize = _landRightsToolSize;
    if (landRightsToolSize == 0)
        landRightsToolSize = 1;

    int32_t size = (landRightsToolSize * 32) - 32;
    int32_t radius = (landRightsToolSize * 16) - 16;
    mapCoords->x = (mapCoords->x - radius) & 0xFFE0;
    mapCoords->y = (mapCoords->y - radius) & 0xFFE0;
    gMapSelectPositionA = *mapCoords;
    gMapSelectPositionB.x = mapCoords->x + size;
    gMapSelectPositionB.y = mapCoords->y + size;
    map_invalidate_selection_rect();
}

/**
 *
 *  rct2: 0x00666EEF
 */
static CoordsXYZD PlaceParkEntranceGetMapPosition(const ScreenCoordsXY& screenCoords)
{
    CoordsXYZD parkEntranceMapPosition{ 0, 0, 0, INVALID_DIRECTION };
    const CoordsXY mapCoords = ViewportInteractionGetTileStartAtCursor(screenCoords);
    parkEntranceMapPosition = { mapCoords.x, mapCoords.y, 0, INVALID_DIRECTION };
    if (parkEntranceMapPosition.IsNull())
        return parkEntranceMapPosition;

    auto surfaceElement = map_get_surface_element_at(mapCoords);
    if (surfaceElement == nullptr)
    {
        parkEntranceMapPosition.SetNull();
        return parkEntranceMapPosition;
    }

    parkEntranceMapPosition.z = surfaceElement->GetWaterHeight();
    if (parkEntranceMapPosition.z == 0)
    {
        parkEntranceMapPosition.z = surfaceElement->GetBaseZ();
        if ((surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) != 0)
        {
            parkEntranceMapPosition.z += 16;
            if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
            {
                parkEntranceMapPosition.z += 16;
            }
        }
    }
    parkEntranceMapPosition.direction = (gWindowSceneryRotation - get_current_rotation()) & 3;
    return parkEntranceMapPosition;
}

/**
 *
 *  rct2: 0x00666FD0
 */
static void WindowMapPlaceParkEntranceToolUpdate(const ScreenCoordsXY& screenCoords)
{
    int32_t sideDirection;

    map_invalidate_selection_rect();
    map_invalidate_map_selection_tiles();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    CoordsXYZD parkEntrancePosition = PlaceParkEntranceGetMapPosition(screenCoords);
    if (parkEntrancePosition.IsNull())
    {
        park_entrance_remove_ghost();
        return;
    }

    sideDirection = (parkEntrancePosition.direction + 1) & 3;
    gMapSelectionTiles.clear();
    gMapSelectionTiles.push_back({ parkEntrancePosition.x, parkEntrancePosition.y });
    gMapSelectionTiles.push_back({ parkEntrancePosition.x + CoordsDirectionDelta[sideDirection].x,
                                   parkEntrancePosition.y + CoordsDirectionDelta[sideDirection].y });
    gMapSelectionTiles.push_back({ parkEntrancePosition.x - CoordsDirectionDelta[sideDirection].x,
                                   parkEntrancePosition.y - CoordsDirectionDelta[sideDirection].y });

    gMapSelectArrowPosition = parkEntrancePosition;
    gMapSelectArrowDirection = parkEntrancePosition.direction;

    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT | MAP_SELECT_FLAG_ENABLE_ARROW;
    map_invalidate_map_selection_tiles();
    if (gParkEntranceGhostExists && parkEntrancePosition == gParkEntranceGhostPosition)
    {
        return;
    }

    park_entrance_remove_ghost();
    park_entrance_place_ghost(parkEntrancePosition);
}

/**
 *
 *  rct2: 0x0068D4E9
 */
static void WindowMapSetPeepSpawnToolUpdate(const ScreenCoordsXY& screenCoords)
{
    int32_t mapZ, direction;
    TileElement* tileElement;

    map_invalidate_selection_rect();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    auto mapCoords = footpath_bridge_get_info_from_pos(screenCoords, &direction, &tileElement);
    if (mapCoords.IsNull())
        return;

    mapZ = tileElement->GetBaseZ();
    if (tileElement->GetType() == TileElementType::Surface)
    {
        if ((tileElement->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP) != 0)
            mapZ += 16;
        if (tileElement->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
            mapZ += 16;
    }

    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectType = MAP_SELECT_TYPE_FULL;
    gMapSelectPositionA = mapCoords;
    gMapSelectPositionB = mapCoords;
    gMapSelectArrowPosition = CoordsXYZ{ mapCoords, mapZ };
    gMapSelectArrowDirection = direction_reverse(direction);
    map_invalidate_selection_rect();
}

/**
 *
 *  rct2: 0x006670A4
 */
static void WindowMapPlaceParkEntranceToolDown(const ScreenCoordsXY& screenCoords)
{
    park_entrance_remove_ghost();

    CoordsXYZD parkEntrancePosition = PlaceParkEntranceGetMapPosition(screenCoords);
    if (!parkEntrancePosition.IsNull())
    {
        auto gameAction = PlaceParkEntranceAction(parkEntrancePosition, gFootpathSelectedId);
        auto result = GameActions::Execute(&gameAction);
        if (result.Error == GameActions::Status::Ok)
        {
            OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result.Position);
        }
    }
}

/**
 *
 *  rct2: 0x0068D573
 */
static void WindowMapSetPeepSpawnToolDown(const ScreenCoordsXY& screenCoords)
{
    TileElement* tileElement;
    int32_t mapZ, direction;

    // Verify footpath exists at location, and retrieve coordinates
    auto mapCoords = footpath_get_coordinates_from_pos(screenCoords, &direction, &tileElement);
    if (mapCoords.IsNull())
        return;

    mapZ = tileElement->GetBaseZ();

    auto gameAction = PlacePeepSpawnAction({ mapCoords, mapZ, static_cast<Direction>(direction) });
    auto result = GameActions::Execute(&gameAction);
    if (result.Error == GameActions::Status::Ok)
    {
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result.Position);
    }
}

/**
 *
 *  rct2: 0x0068D641
 */
static void MapWindowIncreaseMapSize()
{
    auto increaseMapSizeAction = ChangeMapSizeAction(gMapSize + 1);
    GameActions::Execute(&increaseMapSizeAction);
}

/**
 *
 *  rct2: 0x0068D6B4
 */
static void MapWindowDecreaseMapSize()
{
    auto decreaseMapSizeAction = ChangeMapSizeAction(gMapSize - 1);
    GameActions::Execute(&decreaseMapSizeAction);
}

static constexpr const uint16_t WaterColour = MapColour(PALETTE_INDEX_195);

static constexpr const uint16_t ElementTypeMaskColour[] = {
    0xFFFF, // TILE_ELEMENT_TYPE_SURFACE
    0x0000, // TILE_ELEMENT_TYPE_PATH
    0x00FF, // TILE_ELEMENT_TYPE_TRACK
    0xFF00, // TILE_ELEMENT_TYPE_SMALL_SCENERY
    0x0000, // TILE_ELEMENT_TYPE_ENTRANCE
    0xFFFF, // TILE_ELEMENT_TYPE_WALL
    0x0000, // TILE_ELEMENT_TYPE_LARGE_SCENERY
    0xFFFF, // TILE_ELEMENT_TYPE_BANNER
};

static constexpr const uint16_t ElementTypeAddColour[] = {
    MapColour(PALETTE_INDEX_0),                     // TILE_ELEMENT_TYPE_SURFACE
    MapColour(PALETTE_INDEX_17),                    // TILE_ELEMENT_TYPE_PATH
    MapColour2(PALETTE_INDEX_183, PALETTE_INDEX_0), // TILE_ELEMENT_TYPE_TRACK
    MapColour2(PALETTE_INDEX_0, PALETTE_INDEX_99),  // TILE_ELEMENT_TYPE_SMALL_SCENERY
    MapColour(PALETTE_INDEX_186),                   // TILE_ELEMENT_TYPE_ENTRANCE
    MapColour(PALETTE_INDEX_0),                     // TILE_ELEMENT_TYPE_WALL
    MapColour(PALETTE_INDEX_99),                    // TILE_ELEMENT_TYPE_LARGE_SCENERY
    MapColour(PALETTE_INDEX_0),                     // TILE_ELEMENT_TYPE_BANNER
};

static uint16_t MapWindowGetPixelColourPeep(const CoordsXY& c)
{
    auto* surfaceElement = map_get_surface_element_at(c);
    if (surfaceElement == nullptr)
        return 0;

    uint16_t colour = MapColour(PALETTE_INDEX_0);
    const auto* surfaceObject = surfaceElement->GetSurfaceStyleObject();
    if (surfaceObject != nullptr)
        colour = MapColour2(surfaceObject->MapColours[0], surfaceObject->MapColours[1]);

    if (surfaceElement->GetWaterHeight() > 0)
        colour = WaterColour;

    if (!(surfaceElement->GetOwnership() & OWNERSHIP_OWNED))
        colour = MapColourUnowned(colour);

    const int32_t maxSupportedTileElementType = static_cast<int32_t>(std::size(ElementTypeAddColour));
    auto tileElement = reinterpret_cast<TileElement*>(surfaceElement);
    while (!(tileElement++)->IsLastForTile())
    {
        if (tileElement->IsGhost())
        {
            colour = MapColour(PALETTE_INDEX_21);
            break;
        }

        auto tileElementType = tileElement->GetType();
        if (EnumValue(tileElementType) >= maxSupportedTileElementType)
        {
            tileElementType = TileElementType::Surface;
        }
        colour &= ElementTypeMaskColour[EnumValue(tileElementType)];
        colour |= ElementTypeAddColour[EnumValue(tileElementType)];
    }

    return colour;
}

static uint16_t MapWindowGetPixelColourRide(const CoordsXY& c)
{
    Ride* ride;
    uint16_t colourA = 0;                           // highlight colour
    uint16_t colourB = MapColour(PALETTE_INDEX_13); // surface colour (dark grey)

    // as an improvement we could use first_element to show underground stuff?
    TileElement* tileElement = reinterpret_cast<TileElement*>(map_get_surface_element_at(c));
    do
    {
        if (tileElement == nullptr)
            break;

        if (tileElement->IsGhost())
        {
            colourA = MapColour(PALETTE_INDEX_21);
            break;
        }

        switch (tileElement->GetType())
        {
            case TileElementType::Surface:
                if (tileElement->AsSurface()->GetWaterHeight() > 0)
                    // Why is this a different water colour as above (195)?
                    colourB = MapColour(PALETTE_INDEX_194);
                if (!(tileElement->AsSurface()->GetOwnership() & OWNERSHIP_OWNED))
                    colourB = MapColourUnowned(colourB);
                break;
            case TileElementType::Path:
                colourA = MapColour(PALETTE_INDEX_14); // lighter grey
                break;
            case TileElementType::Entrance:
                if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE)
                    break;
                ride = get_ride(tileElement->AsEntrance()->GetRideIndex());
                if (ride != nullptr)
                {
                    const auto& colourKey = ride->GetRideTypeDescriptor().ColourKey;
                    colourA = RideKeyColours[static_cast<size_t>(colourKey)];
                }
                break;
            case TileElementType::Track:
                ride = get_ride(tileElement->AsTrack()->GetRideIndex());
                if (ride != nullptr)
                {
                    const auto& colourKey = ride->GetRideTypeDescriptor().ColourKey;
                    colourA = RideKeyColours[static_cast<size_t>(colourKey)];
                }

                break;
            default:
                break;
        }
    } while (!(tileElement++)->IsLastForTile());

    if (colourA != 0)
        return colourA;

    return colourB;
}

static void MapWindowSetPixels(rct_window* w)
{
    uint16_t colour = 0;
    int32_t x = 0, y = 0, dx = 0, dy = 0;

    int32_t pos = (_currentLine * (MAP_WINDOW_MAP_SIZE - 1)) + MAXIMUM_MAP_SIZE_TECHNICAL - 1;
    auto destinationPosition = ScreenCoordsXY{ pos % MAP_WINDOW_MAP_SIZE, pos / MAP_WINDOW_MAP_SIZE };
    auto destination = _mapImageData.data() + (destinationPosition.y * MAP_WINDOW_MAP_SIZE) + destinationPosition.x;
    switch (get_current_rotation())
    {
        case 0:
            x = _currentLine * COORDS_XY_STEP;
            y = 0;
            dx = 0;
            dy = COORDS_XY_STEP;
            break;
        case 1:
            x = MAXIMUM_TILE_START_XY;
            y = _currentLine * COORDS_XY_STEP;
            dx = -COORDS_XY_STEP;
            dy = 0;
            break;
        case 2:
            x = MAXIMUM_MAP_SIZE_BIG - ((_currentLine + 1) * COORDS_XY_STEP);
            y = MAXIMUM_TILE_START_XY;
            dx = 0;
            dy = -COORDS_XY_STEP;
            break;
        case 3:
            x = 0;
            y = MAXIMUM_MAP_SIZE_BIG - ((_currentLine + 1) * COORDS_XY_STEP);
            dx = COORDS_XY_STEP;
            dy = 0;
            break;
    }

    for (int32_t i = 0; i < MAXIMUM_MAP_SIZE_TECHNICAL; i++)
    {
        if (!map_is_edge({ x, y }))
        {
            switch (w->selected_tab)
            {
                case PAGE_PEEPS:
                    colour = MapWindowGetPixelColourPeep({ x, y });
                    break;
                case PAGE_RIDES:
                    colour = MapWindowGetPixelColourRide({ x, y });
                    break;
            }
            destination[0] = (colour >> 8) & 0xFF;
            destination[1] = colour;
        }
        x += dx;
        y += dy;

        destinationPosition.x++;
        destinationPosition.y++;
        destination = _mapImageData.data() + (destinationPosition.y * MAP_WINDOW_MAP_SIZE) + destinationPosition.x;
    }
    _currentLine++;
    if (_currentLine >= MAXIMUM_MAP_SIZE_TECHNICAL)
        _currentLine = 0;
}

static CoordsXY MapWindowScreenToMap(ScreenCoordsXY screenCoords)
{
    screenCoords.x = ((screenCoords.x + 8) - MAXIMUM_MAP_SIZE_TECHNICAL) / 2;
    screenCoords.y = ((screenCoords.y + 8)) / 2;
    auto location = TileCoordsXY(screenCoords.y - screenCoords.x, screenCoords.x + screenCoords.y).ToCoordsXY();

    switch (get_current_rotation())
    {
        case 0:
            return location;
        case 1:
            return { MAXIMUM_MAP_SIZE_BIG - 1 - location.y, location.x };
        case 2:
            return { MAXIMUM_MAP_SIZE_BIG - 1 - location.x, MAXIMUM_MAP_SIZE_BIG - 1 - location.y };
        case 3:
            return { location.y, MAXIMUM_MAP_SIZE_BIG - 1 - location.x };
    }

    return { 0, 0 }; // unreachable
}
