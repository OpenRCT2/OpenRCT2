/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/MapGen.h>
#include <openrct2/world/Surface.h>

using namespace OpenRCT2;

// clang-format off
enum {
    WINDOW_MAPGEN_PAGE_BASE,
    WINDOW_MAPGEN_PAGE_RANDOM,
    WINDOW_MAPGEN_PAGE_SIMPLEX,
    WINDOW_MAPGEN_PAGE_HEIGHTMAP,
    WINDOW_MAPGEN_PAGE_COUNT
};

enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_TAB_3,
    WIDX_TAB_4,


    TAB_BEGIN,

    WIDX_MAP_GENERATE = TAB_BEGIN,
    WIDX_MAP_SIZE,
    WIDX_MAP_SIZE_UP,
    WIDX_MAP_SIZE_DOWN,
    WIDX_BASE_HEIGHT,
    WIDX_BASE_HEIGHT_UP,
    WIDX_BASE_HEIGHT_DOWN,
    WIDX_WATER_LEVEL,
    WIDX_WATER_LEVEL_UP,
    WIDX_WATER_LEVEL_DOWN,
    WIDX_FLOOR_TEXTURE,
    WIDX_WALL_TEXTURE,

    WIDX_RANDOM_GENERATE = TAB_BEGIN,
    WIDX_RANDOM_TERRAIN,
    WIDX_RANDOM_PLACE_TREES,

    WIDX_SIMPLEX_GENERATE = TAB_BEGIN,
    WIDX_SIMPLEX_LABEL,
    WIDX_SIMPLEX_LOW,
    WIDX_SIMPLEX_LOW_UP,
    WIDX_SIMPLEX_LOW_DOWN,
    WIDX_SIMPLEX_HIGH,
    WIDX_SIMPLEX_HIGH_UP,
    WIDX_SIMPLEX_HIGH_DOWN,
    WIDX_SIMPLEX_BASE_FREQ,
    WIDX_SIMPLEX_BASE_FREQ_UP,
    WIDX_SIMPLEX_BASE_FREQ_DOWN,
    WIDX_SIMPLEX_OCTAVES,
    WIDX_SIMPLEX_OCTAVES_UP,
    WIDX_SIMPLEX_OCTAVES_DOWN,
    WIDX_SIMPLEX_MAP_SIZE,
    WIDX_SIMPLEX_MAP_SIZE_UP,
    WIDX_SIMPLEX_MAP_SIZE_DOWN,
    WIDX_SIMPLEX_WATER_LEVEL,
    WIDX_SIMPLEX_WATER_LEVEL_UP,
    WIDX_SIMPLEX_WATER_LEVEL_DOWN,
    WIDX_SIMPLEX_RANDOM_TERRAIN_CHECKBOX,
    WIDX_SIMPLEX_FLOOR_TEXTURE,
    WIDX_SIMPLEX_WALL_TEXTURE,
    WIDX_SIMPLEX_PLACE_TREES_CHECKBOX,

    WIDX_HEIGHTMAP_SELECT = TAB_BEGIN,
    WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP,
    WIDX_HEIGHTMAP_STRENGTH,
    WIDX_HEIGHTMAP_STRENGTH_UP,
    WIDX_HEIGHTMAP_STRENGTH_DOWN,
    WIDX_HEIGHTMAP_NORMALIZE,
    WIDX_HEIGHTMAP_SMOOTH_TILES,
    WIDX_HEIGHTMAP_LOW,
    WIDX_HEIGHTMAP_LOW_UP,
    WIDX_HEIGHTMAP_LOW_DOWN,
    WIDX_HEIGHTMAP_HIGH,
    WIDX_HEIGHTMAP_HIGH_UP,
    WIDX_HEIGHTMAP_HIGH_DOWN,
    WIDX_HEIGHTMAP_WATER_LEVEL,
    WIDX_HEIGHTMAP_WATER_LEVEL_UP,
    WIDX_HEIGHTMAP_WATER_LEVEL_DOWN,
};

#pragma region Widgets

static constexpr const rct_string_id WINDOW_TITLE = STR_MAPGEN_WINDOW_TITLE;
static constexpr const int32_t WW = 250;
static constexpr const int32_t WH = 273;

#define SHARED_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), /* WIDX_BACKGROUND, WIDX_TITLE, WIDX_CLOSE */ \
    MakeWidget({ 0, 43}, {WW, 229}, WindowWidgetType::Resize, WindowColour::Secondary), /* WIDX_PAGE_BACKGROUND */ \
    MakeTab   ({ 3, 17}                                                ), /* WIDX_TAB_1 */ \
    MakeTab   ({34, 17}                                                ), /* WIDX_TAB_2 */ \
    MakeTab   ({65, 17}                                                ), /* WIDX_TAB_3 */ \
    MakeTab   ({96, 17}                                                )  /* WIDX_TAB_4 */

static rct_widget MapWidgets[] = {
    SHARED_WIDGETS,
    MakeWidget        ({155, 255}, {90, 14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_MAPGEN_ACTION_GENERATE                              ),
    MakeSpinnerWidgets({104,  52}, {95, 12}, WindowWidgetType::Spinner, WindowColour::Secondary                                                          ), // NB: 3 widgets
    MakeSpinnerWidgets({104,  70}, {95, 12}, WindowWidgetType::Spinner, WindowColour::Secondary                                                          ), // NB: 3 widgets
    MakeSpinnerWidgets({104,  88}, {95, 12}, WindowWidgetType::Spinner, WindowColour::Secondary                                                          ), // NB: 3 widgets
    MakeWidget        ({104, 106}, {47, 36}, WindowWidgetType::FlatBtn, WindowColour::Secondary, 0xFFFFFFFF,                 STR_CHANGE_BASE_LAND_TIP    ),
    MakeWidget        ({151, 106}, {47, 36}, WindowWidgetType::FlatBtn, WindowColour::Secondary, 0xFFFFFFFF,                 STR_CHANGE_VERTICAL_LAND_TIP),
    { WIDGETS_END },
};

static rct_widget RandomWidgets[] = {
    SHARED_WIDGETS,
    MakeWidget({155, 255}, { 90, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAPGEN_ACTION_GENERATE      ),
    MakeWidget({  4,  52}, {195, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAPGEN_OPTION_RANDOM_TERRAIN),
    MakeWidget({  4,  70}, {195, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAPGEN_OPTION_PLACE_TREES   ),
    { WIDGETS_END },
};

static rct_widget SimplexWidgets[] = {
    SHARED_WIDGETS,
    MakeWidget        ({155, 255}, { 90, 14}, WindowWidgetType::Button,        WindowColour::Secondary, STR_MAPGEN_ACTION_GENERATE                                    ), // WIDX_SIMPLEX_GENERATE
    MakeWidget        ({  4,  52}, {195, 12}, WindowWidgetType::LabelCentred, WindowColour::Secondary, STR_MAPGEN_SIMPLEX_NOISE                                      ), // WIDX_SIMPLEX_LABEL
    MakeSpinnerWidgets({104,  70}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                                                                ), // WIDX_SIMPLEX_LOW{,_UP,_DOWN}
    MakeSpinnerWidgets({104,  88}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                                                                ), // WIDX_SIMPLEX_HIGH{,_UP,_DOWN}
    MakeSpinnerWidgets({104, 106}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                                                                ), // WIDX_SIMPLEX_BASE_FREQ{,_UP,_DOWN}
    MakeSpinnerWidgets({104, 124}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                                                                ), // WIDX_SIMPLEX_OCTAVES{,_UP,_DOWN}
    MakeSpinnerWidgets({104, 148}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                                                                ), // WIDX_SIMPLEX_MAP_SIZE{,_UP,_DOWN}
    MakeSpinnerWidgets({104, 166}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                                                                ), // WIDX_SIMPLEX_WATER_LEVEL{,_UP,_DOWN}
    MakeWidget        ({104, 190}, { 95, 12}, WindowWidgetType::Checkbox,      WindowColour::Secondary, STR_MAPGEN_OPTION_RANDOM_TERRAIN                              ), // WIDX_SIMPLEX_RANDOM_TERRAIN_CHECKBOX
    MakeWidget        ({102, 202}, { 47, 36}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, 0xFFFFFFFF,                       STR_CHANGE_BASE_LAND_TIP    ), // WIDX_SIMPLEX_FLOOR_TEXTURE
    MakeWidget        ({150, 202}, { 47, 36}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, 0xFFFFFFFF,                       STR_CHANGE_VERTICAL_LAND_TIP), // WIDX_SIMPLEX_WALL_TEXTURE
    MakeWidget        ({104, 239}, { 95, 12}, WindowWidgetType::Checkbox,      WindowColour::Secondary                                                                ), // WIDX_SIMPLEX_PLACE_TREES_CHECKBOX
    { WIDGETS_END },
};

static rct_widget HeightmapWidgets[] = {
    SHARED_WIDGETS,
    MakeWidget        ({ 95, 255}, {150, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAPGEN_SELECT_HEIGHTMAP), // WIDX_HEIGHTMAP_SELECT
    MakeWidget        ({  4,  52}, {100, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAPGEN_SMOOTH_HEIGHTMAP), // WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP
    MakeSpinnerWidgets({104,  70}, { 95, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                             ), // WIDX_HEIGHTMAP_STRENGTH{,_UP,_DOWN}
    MakeWidget        ({  4,  88}, {100, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAPGEN_NORMALIZE       ), // WIDX_HEIGHTMAP_NORMALIZE
    MakeWidget        ({  4, 106}, {100, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAPGEN_SMOOTH_TILE     ), // WIDX_HEIGHTMAP_SMOOTH_TILES
    MakeSpinnerWidgets({104, 124}, { 95, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                             ), // WIDX_HEIGHTMAP_LOW{,_UP,_DOWN}
    MakeSpinnerWidgets({104, 142}, { 95, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                             ), // WIDX_HEIGHTMAP_HIGH{,_UP,_DOWN}
    MakeSpinnerWidgets({104, 160}, { 95, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                             ), // WIDX_HEIGHTMAP_WATER_LEVEL{,_UP,_DOWN}
    { WIDGETS_END },
};

static rct_widget *PageWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
    MapWidgets,
    RandomWidgets,
    SimplexWidgets,
    HeightmapWidgets
};

#pragma endregion

#pragma region Events

static void window_mapgen_shared_close(rct_window *w);
static void window_mapgen_shared_mouseup(rct_window *w, rct_widgetindex widgetIndex);

static void window_mapgen_base_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_mapgen_base_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_mapgen_base_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_mapgen_base_update(rct_window *w);
static void window_mapgen_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_mapgen_base_invalidate(rct_window *w);
static void window_mapgen_base_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_mapgen_random_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_mapgen_random_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_mapgen_random_update(rct_window *w);
static void window_mapgen_random_invalidate(rct_window *w);
static void window_mapgen_random_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_mapgen_simplex_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_mapgen_simplex_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_mapgen_simplex_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_mapgen_simplex_update(rct_window *w);
static void window_mapgen_simplex_invalidate(rct_window *w);
static void window_mapgen_simplex_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_mapgen_heightmap_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_mapgen_heightmap_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_mapgen_heightmap_invalidate(rct_window *w);
static void window_mapgen_heightmap_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list BaseEvents([](auto& events)
{
    events.close = &window_mapgen_shared_close;
    events.mouse_up = &window_mapgen_base_mouseup;
    events.mouse_down = &window_mapgen_base_mousedown;
    events.dropdown = &window_mapgen_base_dropdown;
    events.update = &window_mapgen_base_update;
    events.text_input = &window_mapgen_textinput;
    events.invalidate = &window_mapgen_base_invalidate;
    events.paint = &window_mapgen_base_paint;
});

static rct_window_event_list RandomEvents([](auto& events)
{
    events.close = &window_mapgen_shared_close;
    events.mouse_up = &window_mapgen_random_mouseup;
    events.mouse_down = &window_mapgen_random_mousedown;
    events.update = &window_mapgen_random_update;
    events.invalidate = &window_mapgen_random_invalidate;
    events.paint = &window_mapgen_random_paint;
});

static rct_window_event_list SimplexEvents([](auto& events)
{
    events.close = &window_mapgen_shared_close;
    events.mouse_up = &window_mapgen_simplex_mouseup;
    events.mouse_down = &window_mapgen_simplex_mousedown;
    events.dropdown = &window_mapgen_simplex_dropdown;
    events.update = &window_mapgen_simplex_update;
    events.text_input = &window_mapgen_textinput;
    events.invalidate = &window_mapgen_simplex_invalidate;
    events.paint = &window_mapgen_simplex_paint;
});

static rct_window_event_list HeightmapEvents([](auto& events)
{
    events.close = &window_mapgen_shared_close;
    events.mouse_up = &window_mapgen_heightmap_mouseup;
    events.mouse_down = &window_mapgen_heightmap_mousedown;
    events.invalidate = &window_mapgen_heightmap_invalidate;
    events.paint = &window_mapgen_heightmap_paint;
});

static rct_window_event_list *PageEvents[] = {
    &BaseEvents,
    &RandomEvents,
    &SimplexEvents,
    &HeightmapEvents
};

#pragma endregion

#pragma region Enabled widgets

static uint64_t PageEnabledWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
    (1ULL << WIDX_CLOSE) |
    (1ULL << WIDX_TAB_1) |
    (1ULL << WIDX_TAB_2) |
    (1ULL << WIDX_TAB_3) |
    (1ULL << WIDX_TAB_4) |
    (1ULL << WIDX_MAP_GENERATE) |
    (1ULL << WIDX_MAP_SIZE) |
    (1ULL << WIDX_MAP_SIZE_UP) |
    (1ULL << WIDX_MAP_SIZE_DOWN) |
    (1ULL << WIDX_BASE_HEIGHT) |
    (1ULL << WIDX_BASE_HEIGHT_UP) |
    (1ULL << WIDX_BASE_HEIGHT_DOWN) |
    (1ULL << WIDX_WATER_LEVEL) |
    (1ULL << WIDX_WATER_LEVEL_UP) |
    (1ULL << WIDX_WATER_LEVEL_DOWN) |
    (1ULL << WIDX_FLOOR_TEXTURE) |
    (1ULL << WIDX_WALL_TEXTURE),

    (1ULL << WIDX_CLOSE) |
    (1ULL << WIDX_TAB_1) |
    (1ULL << WIDX_TAB_2) |
    (1ULL << WIDX_TAB_3) |
    (1ULL << WIDX_TAB_4) |
    (1ULL << WIDX_RANDOM_GENERATE) |
    (1ULL << WIDX_RANDOM_TERRAIN) |
    (1ULL << WIDX_RANDOM_PLACE_TREES),

    (1ULL << WIDX_CLOSE) |
    (1ULL << WIDX_TAB_1) |
    (1ULL << WIDX_TAB_2) |
    (1ULL << WIDX_TAB_3) |
    (1ULL << WIDX_TAB_4) |
    (1ULL << WIDX_SIMPLEX_GENERATE) |
    (1ULL << WIDX_SIMPLEX_LABEL) |
    (1ULL << WIDX_SIMPLEX_LOW) |
    (1ULL << WIDX_SIMPLEX_LOW_UP) |
    (1ULL << WIDX_SIMPLEX_LOW_DOWN) |
    (1ULL << WIDX_SIMPLEX_HIGH) |
    (1ULL << WIDX_SIMPLEX_HIGH_UP) |
    (1ULL << WIDX_SIMPLEX_HIGH_DOWN) |
    (1ULL << WIDX_SIMPLEX_BASE_FREQ) |
    (1ULL << WIDX_SIMPLEX_BASE_FREQ_UP) |
    (1ULL << WIDX_SIMPLEX_BASE_FREQ_DOWN) |
    (1ULL << WIDX_SIMPLEX_OCTAVES) |
    (1ULL << WIDX_SIMPLEX_OCTAVES_UP) |
    (1ULL << WIDX_SIMPLEX_OCTAVES_DOWN) |
    (1ULL << WIDX_SIMPLEX_MAP_SIZE) |
    (1ULL << WIDX_SIMPLEX_MAP_SIZE_UP) |
    (1ULL << WIDX_SIMPLEX_MAP_SIZE_DOWN) |
    (1ULL << WIDX_SIMPLEX_WATER_LEVEL) |
    (1ULL << WIDX_SIMPLEX_WATER_LEVEL_UP) |
    (1ULL << WIDX_SIMPLEX_WATER_LEVEL_DOWN) |
    (1ULL << WIDX_SIMPLEX_RANDOM_TERRAIN_CHECKBOX) |
    (1ULL << WIDX_SIMPLEX_FLOOR_TEXTURE) |
    (1ULL << WIDX_SIMPLEX_WALL_TEXTURE) |
    (1ULL << WIDX_SIMPLEX_PLACE_TREES_CHECKBOX),

    (1ULL << WIDX_CLOSE) |
    (1ULL << WIDX_TAB_1) |
    (1ULL << WIDX_TAB_2) |
    (1ULL << WIDX_TAB_3) |
    (1ULL << WIDX_TAB_4) |
    (1ULL << WIDX_HEIGHTMAP_SELECT)
};

static uint64_t PageDisabledWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
    0,

    0,

    0,

    (1ULL << WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP) |
    (1ULL << WIDX_HEIGHTMAP_STRENGTH) |
    (1ULL << WIDX_HEIGHTMAP_STRENGTH_UP) |
    (1ULL << WIDX_HEIGHTMAP_STRENGTH_DOWN) |
    (1ULL << WIDX_HEIGHTMAP_NORMALIZE) |
    (1ULL << WIDX_HEIGHTMAP_SMOOTH_TILES) |
    (1ULL << WIDX_HEIGHTMAP_HIGH) |
    (1ULL << WIDX_HEIGHTMAP_HIGH_UP) |
    (1ULL << WIDX_HEIGHTMAP_HIGH_DOWN) |
    (1ULL << WIDX_HEIGHTMAP_LOW) |
    (1ULL << WIDX_HEIGHTMAP_LOW_UP) |
    (1ULL << WIDX_HEIGHTMAP_LOW_DOWN) |
    (1ULL << WIDX_HEIGHTMAP_WATER_LEVEL) |
    (1ULL << WIDX_HEIGHTMAP_WATER_LEVEL_UP) |
    (1ULL << WIDX_HEIGHTMAP_WATER_LEVEL_DOWN)
};

static uint64_t HoldDownWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
    (1ULL << WIDX_MAP_SIZE_UP) |
    (1ULL << WIDX_MAP_SIZE_DOWN) |
    (1ULL << WIDX_BASE_HEIGHT_UP) |
    (1ULL << WIDX_BASE_HEIGHT_DOWN) |
    (1ULL << WIDX_WATER_LEVEL_UP) |
    (1ULL << WIDX_WATER_LEVEL_DOWN),

    0,

    (1ULL << WIDX_SIMPLEX_LOW_UP) |
    (1ULL << WIDX_SIMPLEX_LOW_DOWN) |
    (1ULL << WIDX_SIMPLEX_HIGH_UP) |
    (1ULL << WIDX_SIMPLEX_HIGH_DOWN) |
    (1ULL << WIDX_SIMPLEX_BASE_FREQ_UP) |
    (1ULL << WIDX_SIMPLEX_BASE_FREQ_DOWN) |
    (1ULL << WIDX_SIMPLEX_OCTAVES_UP) |
    (1ULL << WIDX_SIMPLEX_OCTAVES_DOWN) |
    (1ULL << WIDX_SIMPLEX_MAP_SIZE_UP) |
    (1ULL << WIDX_SIMPLEX_MAP_SIZE_DOWN) |
    (1ULL << WIDX_SIMPLEX_WATER_LEVEL_UP) |
    (1ULL << WIDX_SIMPLEX_WATER_LEVEL_DOWN),

    (1ULL << WIDX_HEIGHTMAP_STRENGTH_UP) |
    (1ULL << WIDX_HEIGHTMAP_STRENGTH_DOWN) |
    (1ULL << WIDX_HEIGHTMAP_LOW_UP) |
    (1ULL << WIDX_HEIGHTMAP_LOW_DOWN) |
    (1ULL << WIDX_HEIGHTMAP_HIGH_UP) |
    (1ULL << WIDX_HEIGHTMAP_HIGH_DOWN) |
    (1ULL << WIDX_HEIGHTMAP_WATER_LEVEL_UP) |
    (1ULL << WIDX_HEIGHTMAP_WATER_LEVEL_DOWN)
};

static uint64_t PressedWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
    0,
    0,
    0,
    (1ULL << WIDX_HEIGHTMAP_SMOOTH_TILES)
};

#pragma endregion

static constexpr const int32_t TabAnimationDivisor[WINDOW_MAPGEN_PAGE_COUNT] = {
    1, 1, 1, 1
};
static constexpr const int32_t TabAnimationFrames[WINDOW_MAPGEN_PAGE_COUNT] = {
    1, 1, 1, 1
};
static constexpr const int32_t TabAnimationLoops[WINDOW_MAPGEN_PAGE_COUNT] = {
    16, 16, 16, 0
};
// clang-format on

constexpr int32_t BASESIZE_MIN = 0;
constexpr int32_t BASESIZE_MAX = 60;
constexpr int32_t WATERLEVEL_MIN = 0;
constexpr int32_t WATERLEVEL_MAX = 54;
constexpr int32_t MAX_SMOOTH_ITERATIONS = 20;

static void window_mapgen_set_page(rct_window* w, int32_t page);
static void window_mapgen_set_pressed_tab(rct_window* w);
static void window_mapgen_draw_tab_images(rct_drawpixelinfo* dpi, rct_window* w);

static int32_t _mapSize = 150;
static int32_t _baseHeight = 12;
static int32_t _waterLevel = 6;
static int32_t _floorTexture = TERRAIN_GRASS;
static int32_t _wallTexture = TERRAIN_EDGE_ROCK;
static bool _randomTerrain = true;
static int32_t _placeTrees = 1;

static int32_t _simplex_low = 6;
static int32_t _simplex_high = 10;
static int32_t _simplex_base_freq = 60;
static int32_t _simplex_octaves = 4;

static bool _heightmapLoaded = false;
static bool _heightmapSmoothMap = false;
static int32_t _heightmapSmoothStrength = 1;
static bool _heightmapNormalize = false;
static bool _heightmapSmoothTiles = true;
static int32_t _heightmapLow = 2;
static int32_t _heightmapHigh = 70;

rct_window* window_mapgen_open()
{
    rct_window* w = window_bring_to_front_by_class(WC_MAPGEN);
    if (w != nullptr)
    {
        return w;
    }

    w = WindowCreateCentred(WW, WH, PageEvents[WINDOW_MAPGEN_PAGE_BASE], WC_MAPGEN, WF_10);
    w->number = 0;
    w->frame_no = 0;

    w->page = WINDOW_MAPGEN_PAGE_BASE;
    w->Invalidate();
    w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_BASE];
    w->enabled_widgets = PageEnabledWidgets[WINDOW_MAPGEN_PAGE_BASE];
    w->hold_down_widgets = HoldDownWidgets[WINDOW_MAPGEN_PAGE_BASE];
    w->event_handlers = PageEvents[WINDOW_MAPGEN_PAGE_BASE];
    w->pressed_widgets = PressedWidgets[WINDOW_MAPGEN_PAGE_BASE];
    w->disabled_widgets = PageDisabledWidgets[WINDOW_MAPGEN_PAGE_BASE];
    WindowInitScrollWidgets(w);

    _heightmapLoaded = false;

    return w;
}

static void window_mapgen_shared_close(rct_window* w)
{
    mapgen_unload_heightmap();
}

static void window_mapgen_shared_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
        case WIDX_TAB_3:
        case WIDX_TAB_4:
            window_mapgen_set_page(w, widgetIndex - WIDX_TAB_1);
            break;
    }
}

#pragma region Base page

static void window_mapgen_base_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    window_mapgen_shared_mouseup(w, widgetIndex);

    mapgen_settings mapgenSettings;

    switch (widgetIndex)
    {
        case WIDX_MAP_GENERATE:
            mapgenSettings.mapSize = _mapSize;
            mapgenSettings.height = _baseHeight + 2;
            mapgenSettings.water_level = _waterLevel + 2;
            mapgenSettings.floor = _floorTexture;
            mapgenSettings.wall = _wallTexture;

            mapgen_generate_blank(&mapgenSettings);
            gfx_invalidate_screen();
            break;
        case WIDX_MAP_SIZE:
            TextInputDescriptionArgs[0] = MINIMUM_MAP_SIZE_PRACTICAL;
            TextInputDescriptionArgs[1] = MAXIMUM_MAP_SIZE_PRACTICAL;
            // Practical map size is 2 lower than the technical map size
            window_text_input_open(w, WIDX_MAP_SIZE, STR_MAP_SIZE_2, STR_ENTER_MAP_SIZE, STR_FORMAT_INTEGER, _mapSize - 2, 4);
            break;
        case WIDX_BASE_HEIGHT:
            TextInputDescriptionArgs[0] = static_cast<uint16_t>((BASESIZE_MIN - 12) / 2);
            TextInputDescriptionArgs[1] = static_cast<uint16_t>((BASESIZE_MAX - 12) / 2);
            window_text_input_open(
                w, WIDX_BASE_HEIGHT, STR_BASE_HEIGHT, STR_ENTER_BASE_HEIGHT, STR_FORMAT_INTEGER, (_baseHeight - 12) / 2, 3);
            break;
        case WIDX_WATER_LEVEL:
            TextInputDescriptionArgs[0] = static_cast<uint16_t>((WATERLEVEL_MIN - 12) / 2);
            TextInputDescriptionArgs[1] = static_cast<uint16_t>((WATERLEVEL_MAX - 12) / 2);
            window_text_input_open(
                w, WIDX_WATER_LEVEL, STR_WATER_LEVEL, STR_ENTER_WATER_LEVEL, STR_FORMAT_INTEGER, (_waterLevel - 12) / 2, 3);
            break;
    }
}

static void window_mapgen_base_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_MAP_SIZE_UP:
            _mapSize = std::min(_mapSize + 1, MAXIMUM_MAP_SIZE_TECHNICAL);
            w->Invalidate();
            break;
        case WIDX_MAP_SIZE_DOWN:
            _mapSize = std::max(_mapSize - 1, MINIMUM_MAP_SIZE_TECHNICAL);
            w->Invalidate();
            break;
        case WIDX_BASE_HEIGHT_UP:
            _baseHeight = std::min(_baseHeight + 2, BASESIZE_MAX);
            w->Invalidate();
            break;
        case WIDX_BASE_HEIGHT_DOWN:
            _baseHeight = std::max(_baseHeight - 2, BASESIZE_MIN);
            w->Invalidate();
            break;
        case WIDX_WATER_LEVEL_UP:
            _waterLevel = std::min(_waterLevel + 2, WATERLEVEL_MAX);
            w->Invalidate();
            break;
        case WIDX_WATER_LEVEL_DOWN:
            _waterLevel = std::max(_waterLevel - 2, WATERLEVEL_MIN);
            w->Invalidate();
            break;
        case WIDX_FLOOR_TEXTURE:
            LandTool::ShowSurfaceStyleDropdown(w, widget, _floorTexture);
            break;
        case WIDX_WALL_TEXTURE:
            LandTool::ShowEdgeStyleDropdown(w, widget, _wallTexture);
            break;
    }
}

static void window_mapgen_base_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    int32_t type;

    switch (widgetIndex)
    {
        case WIDX_FLOOR_TEXTURE:
            if (dropdownIndex == -1)
                dropdownIndex = gDropdownHighlightedIndex;

            type = (dropdownIndex == -1) ? _floorTexture : dropdownIndex;

            if (gLandToolTerrainSurface == type)
            {
                gLandToolTerrainSurface = OBJECT_ENTRY_INDEX_NULL;
            }
            else
            {
                gLandToolTerrainSurface = type;
                _floorTexture = type;
            }
            w->Invalidate();
            break;
        case WIDX_WALL_TEXTURE:
            if (dropdownIndex == -1)
                dropdownIndex = gDropdownHighlightedIndex;

            type = (dropdownIndex == -1) ? _wallTexture : dropdownIndex;

            if (gLandToolTerrainEdge == type)
            {
                gLandToolTerrainEdge = OBJECT_ENTRY_INDEX_NULL;
            }
            else
            {
                gLandToolTerrainEdge = type;
                _wallTexture = type;
            }
            w->Invalidate();
            break;
    }
}

static void window_mapgen_base_update(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= TabAnimationLoops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_1);
}

static void window_mapgen_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    int32_t value;
    char* end;

    if (text == nullptr)
        return;

    value = strtol(text, &end, 10);

    if (*end != '\0')
    {
        return;
    }

    switch (widgetIndex)
    {
        case WIDX_MAP_SIZE:
        case WIDX_SIMPLEX_MAP_SIZE:
            // The practical size is 2 lower than the technical size
            value += 2;
            _mapSize = std::clamp(value, MINIMUM_MAP_SIZE_TECHNICAL, MAXIMUM_MAP_SIZE_TECHNICAL);
            break;
        case WIDX_BASE_HEIGHT:
            _baseHeight = std::clamp((value * 2) + 12, BASESIZE_MIN, BASESIZE_MAX);
            break;
        case WIDX_WATER_LEVEL:
            _waterLevel = std::clamp((value * 2) + 12, WATERLEVEL_MIN, WATERLEVEL_MAX);
            break;
    }

    w->Invalidate();
}

static void window_mapgen_base_invalidate(rct_window* w)
{
    auto surfaceImage = static_cast<uint32_t>(SPR_NONE);
    auto edgeImage = static_cast<uint32_t>(SPR_NONE);

    auto& objManager = GetContext()->GetObjectManager();
    const auto surfaceObj = static_cast<TerrainSurfaceObject*>(
        objManager.GetLoadedObject(ObjectType::TerrainSurface, _floorTexture));
    if (surfaceObj != nullptr)
    {
        surfaceImage = surfaceObj->IconImageId;
        if (surfaceObj->Colour != 255)
        {
            surfaceImage |= SPRITE_ID_PALETTE_COLOUR_1(surfaceObj->Colour);
        }
    }
    const auto edgeObj = static_cast<TerrainEdgeObject*>(objManager.GetLoadedObject(ObjectType::TerrainEdge, _wallTexture));
    if (edgeObj != nullptr)
    {
        edgeImage = edgeObj->IconImageId;
    }

    if (w->widgets != PageWidgets[WINDOW_MAPGEN_PAGE_BASE])
    {
        w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_BASE];
        WindowInitScrollWidgets(w);
    }

    w->widgets[WIDX_FLOOR_TEXTURE].image = surfaceImage;
    w->widgets[WIDX_WALL_TEXTURE].image = edgeImage;

    window_mapgen_set_pressed_tab(w);
}

static void window_mapgen_base_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    uint16_t arg;

    WindowDrawWidgets(w, dpi);
    window_mapgen_draw_tab_images(dpi, w);

    const auto textColour = w->colours[1];

    DrawTextBasic(dpi, w->windowPos + ScreenCoordsXY{ 4, w->widgets[WIDX_MAP_SIZE].top + 1 }, STR_MAP_SIZE, {}, { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 4, w->widgets[WIDX_BASE_HEIGHT].top + 1 }, STR_BASE_HEIGHT_LABEL, {},
        { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 4, w->widgets[WIDX_WATER_LEVEL].top + 1 }, STR_WATER_LEVEL_LABEL, {},
        { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 4, w->widgets[WIDX_FLOOR_TEXTURE].top + 1 }, STR_TERRAIN_LABEL, {}, { textColour });

    // The practical map size is 2 lower than the technical map size
    // This needs to be cast down to a uint16_t because that's what the STR_RESOLUTION_X_BY_Y string takes.
    uint16_t mapSizeArgs[] = { static_cast<uint16_t>(_mapSize - 2), static_cast<uint16_t>(_mapSize - 2) };
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_MAP_SIZE].left + 1, w->widgets[WIDX_MAP_SIZE].top + 1 },
        STR_RESOLUTION_X_BY_Y, &mapSizeArgs, { w->colours[1] });

    arg = (_baseHeight - 12) / 2;
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_BASE_HEIGHT].left + 1, w->widgets[WIDX_BASE_HEIGHT].top + 1 },
        STR_COMMA16, &arg, { w->colours[1] });

    arg = (_waterLevel - 12) / 2;
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_WATER_LEVEL].left + 1, w->widgets[WIDX_WATER_LEVEL].top + 1 },
        STR_COMMA16, &arg, { w->colours[1] });
}

#pragma endregion

#pragma region Random page

static void window_mapgen_random_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    window_mapgen_shared_mouseup(w, widgetIndex);

    mapgen_settings mapgenSettings;

    switch (widgetIndex)
    {
        case WIDX_RANDOM_GENERATE:
            mapgenSettings.mapSize = _mapSize;
            mapgenSettings.height = _baseHeight + 2;
            mapgenSettings.water_level = _waterLevel + 2;
            mapgenSettings.floor = _randomTerrain ? -1 : _floorTexture;
            mapgenSettings.wall = _randomTerrain ? -1 : _wallTexture;
            mapgenSettings.trees = _placeTrees;

            mapgenSettings.simplex_low = util_rand() % 4;
            mapgenSettings.simplex_high = 12 + (util_rand() % (32 - 12));
            mapgenSettings.simplex_base_freq = 1.75f;
            mapgenSettings.simplex_octaves = 6;

            mapgen_generate(&mapgenSettings);
            gfx_invalidate_screen();
            break;
        case WIDX_RANDOM_TERRAIN:
            _randomTerrain = !_randomTerrain;
            break;
        case WIDX_RANDOM_PLACE_TREES:
            _placeTrees ^= 1;
            break;
    }
}

static void window_mapgen_random_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
}

static void window_mapgen_random_update(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= TabAnimationLoops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_2);
}

static void window_mapgen_random_invalidate(rct_window* w)
{
    if (w->widgets != PageWidgets[WINDOW_MAPGEN_PAGE_RANDOM])
    {
        w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_RANDOM];
        WindowInitScrollWidgets(w);
    }

    w->pressed_widgets = 0;
    if (_randomTerrain)
        w->pressed_widgets |= 1 << WIDX_RANDOM_TERRAIN;
    if (_placeTrees)
        w->pressed_widgets |= 1 << WIDX_RANDOM_PLACE_TREES;

    window_mapgen_set_pressed_tab(w);
}

static void window_mapgen_random_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_mapgen_draw_tab_images(dpi, w);
}

#pragma endregion

#pragma region Simplex page

static void window_mapgen_simplex_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    window_mapgen_shared_mouseup(w, widgetIndex);

    mapgen_settings mapgenSettings;

    switch (widgetIndex)
    {
        case WIDX_SIMPLEX_MAP_SIZE:
            TextInputDescriptionArgs[0] = MINIMUM_MAP_SIZE_PRACTICAL;
            TextInputDescriptionArgs[1] = MAXIMUM_MAP_SIZE_PRACTICAL;
            // Practical map size is 2 lower than the technical map size
            window_text_input_open(
                w, WIDX_SIMPLEX_MAP_SIZE, STR_MAP_SIZE_2, STR_ENTER_MAP_SIZE, STR_FORMAT_INTEGER, _mapSize - 2, 4);
            break;
        case WIDX_SIMPLEX_GENERATE:
            mapgenSettings.mapSize = _mapSize;

            mapgenSettings.height = _baseHeight;
            mapgenSettings.water_level = _waterLevel + 2;
            mapgenSettings.floor = _randomTerrain ? -1 : _floorTexture;
            mapgenSettings.wall = _randomTerrain ? -1 : _wallTexture;
            mapgenSettings.trees = _placeTrees;

            mapgenSettings.simplex_low = _simplex_low;
            mapgenSettings.simplex_high = _simplex_high;
            mapgenSettings.simplex_base_freq = (static_cast<float>(_simplex_base_freq)) / 100.00f;
            mapgenSettings.simplex_octaves = _simplex_octaves;

            mapgen_generate(&mapgenSettings);
            gfx_invalidate_screen();
            break;
    }
}

static void window_mapgen_simplex_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_SIMPLEX_LOW_UP:
            _simplex_low = std::min(_simplex_low + 1, 24);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_LOW_DOWN:
            _simplex_low = std::max(_simplex_low - 1, 0);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_HIGH_UP:
            _simplex_high = std::min(_simplex_high + 1, 36);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_HIGH_DOWN:
            _simplex_high = std::max(_simplex_high - 1, 0);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_BASE_FREQ_UP:
            _simplex_base_freq = std::min(_simplex_base_freq + 5, 1000);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_BASE_FREQ_DOWN:
            _simplex_base_freq = std::max(_simplex_base_freq - 5, 0);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_OCTAVES_UP:
            _simplex_octaves = std::min(_simplex_octaves + 1, 10);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_OCTAVES_DOWN:
            _simplex_octaves = std::max(_simplex_octaves - 1, 1);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_MAP_SIZE_UP:
            _mapSize = std::min(_mapSize + 1, MAXIMUM_MAP_SIZE_TECHNICAL);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_MAP_SIZE_DOWN:
            _mapSize = std::max(_mapSize - 1, MINIMUM_MAP_SIZE_TECHNICAL);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_WATER_LEVEL_UP:
            _waterLevel = std::min(_waterLevel + 2, 54);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_WATER_LEVEL_DOWN:
            _waterLevel = std::max(_waterLevel - 2, 0);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_RANDOM_TERRAIN_CHECKBOX:
            _randomTerrain = !_randomTerrain;
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_FLOOR_TEXTURE:
            LandTool::ShowSurfaceStyleDropdown(w, widget, _floorTexture);
            break;
        case WIDX_SIMPLEX_WALL_TEXTURE:
            LandTool::ShowEdgeStyleDropdown(w, widget, _wallTexture);
            break;
        case WIDX_SIMPLEX_PLACE_TREES_CHECKBOX:
            _placeTrees ^= 1;
            w->Invalidate();
            break;
    }
}

static void window_mapgen_simplex_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    int32_t type;

    switch (widgetIndex)
    {
        case WIDX_SIMPLEX_FLOOR_TEXTURE:
            if (dropdownIndex == -1)
                dropdownIndex = gDropdownHighlightedIndex;

            type = (dropdownIndex == -1) ? _floorTexture : dropdownIndex;

            if (gLandToolTerrainSurface == type)
            {
                gLandToolTerrainSurface = OBJECT_ENTRY_INDEX_NULL;
            }
            else
            {
                gLandToolTerrainSurface = type;
                _floorTexture = type;
            }
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_WALL_TEXTURE:
            if (dropdownIndex == -1)
                dropdownIndex = gDropdownHighlightedIndex;

            type = (dropdownIndex == -1) ? _wallTexture : dropdownIndex;

            if (gLandToolTerrainEdge == type)
            {
                gLandToolTerrainEdge = OBJECT_ENTRY_INDEX_NULL;
            }
            else
            {
                gLandToolTerrainEdge = type;
                _wallTexture = type;
            }
            w->Invalidate();
            break;
    }
}

static void window_mapgen_simplex_update(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= TabAnimationLoops[w->page])
        w->frame_no = 0;
    widget_invalidate(w, WIDX_TAB_3);
}

static void window_mapgen_simplex_invalidate(rct_window* w)
{
    auto surfaceImage = static_cast<uint32_t>(SPR_NONE);
    auto edgeImage = static_cast<uint32_t>(SPR_NONE);

    auto& objManager = GetContext()->GetObjectManager();
    const auto surfaceObj = static_cast<TerrainSurfaceObject*>(
        objManager.GetLoadedObject(ObjectType::TerrainSurface, _floorTexture));
    if (surfaceObj != nullptr)
    {
        surfaceImage = surfaceObj->IconImageId;
        if (surfaceObj->Colour != 255)
        {
            surfaceImage |= SPRITE_ID_PALETTE_COLOUR_1(surfaceObj->Colour);
        }
    }
    const auto edgeObj = static_cast<TerrainEdgeObject*>(objManager.GetLoadedObject(ObjectType::TerrainEdge, _wallTexture));
    if (edgeObj != nullptr)
    {
        edgeImage = edgeObj->IconImageId;
    }

    if (w->widgets != PageWidgets[WINDOW_MAPGEN_PAGE_SIMPLEX])
    {
        w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_SIMPLEX];
        WindowInitScrollWidgets(w);
    }

    w->widgets[WIDX_SIMPLEX_FLOOR_TEXTURE].image = surfaceImage;
    w->widgets[WIDX_SIMPLEX_WALL_TEXTURE].image = edgeImage;

    WidgetSetCheckboxValue(w, WIDX_SIMPLEX_RANDOM_TERRAIN_CHECKBOX, _randomTerrain != 0);
    WidgetSetCheckboxValue(w, WIDX_SIMPLEX_PLACE_TREES_CHECKBOX, _placeTrees != 0);

    // Only allow floor and wall texture options if random terrain is disabled
    if (!_randomTerrain)
    {
        WidgetSetEnabled(w, WIDX_SIMPLEX_FLOOR_TEXTURE, true);
        WidgetSetEnabled(w, WIDX_SIMPLEX_WALL_TEXTURE, true);
    }
    else
    {
        WidgetSetEnabled(w, WIDX_SIMPLEX_FLOOR_TEXTURE, false);
        WidgetSetEnabled(w, WIDX_SIMPLEX_WALL_TEXTURE, false);
    }

    window_mapgen_set_pressed_tab(w);
}

static void window_mapgen_simplex_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    uint16_t arg;

    WindowDrawWidgets(w, dpi);
    window_mapgen_draw_tab_images(dpi, w);

    const uint8_t textColour = w->colours[1];

    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_SIMPLEX_LOW].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_LOW_, {},
        { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_SIMPLEX_HIGH].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_HIGH, {},
        { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1 },
        STR_MAPGEN_SIMPLEX_NOISE_BASE_FREQUENCY, {}, { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_SIMPLEX_OCTAVES].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_OCTAVES, {},
        { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_SIMPLEX_MAP_SIZE].top + 1 }, STR_MAP_SIZE, {}, { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_SIMPLEX_WATER_LEVEL].top + 1 }, STR_WATER_LEVEL_LABEL, {},
        { textColour });

    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_SIMPLEX_LOW].left + 1, w->widgets[WIDX_SIMPLEX_LOW].top + 1 },
        STR_COMMA16, &_simplex_low, { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_SIMPLEX_HIGH].left + 1, w->widgets[WIDX_SIMPLEX_HIGH].top + 1 },
        STR_COMMA16, &_simplex_high, { textColour });
    DrawTextBasic(
        dpi,
        w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_SIMPLEX_BASE_FREQ].left + 1, w->widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1 },
        STR_WINDOW_OBJECTIVE_VALUE_RATING, &_simplex_base_freq, { textColour });
    DrawTextBasic(
        dpi,
        w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_SIMPLEX_OCTAVES].left + 1, w->widgets[WIDX_SIMPLEX_OCTAVES].top + 1 },
        STR_COMMA16, &_simplex_octaves, { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_SIMPLEX_RANDOM_TERRAIN_CHECKBOX].top + 1 }, STR_TERRAIN_LABEL,
        {}, { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_SIMPLEX_PLACE_TREES_CHECKBOX].top + 1 },
        STR_MAPGEN_OPTION_PLACE_TREES, {}, { textColour });

    // The practical map size is 2 lower than the technical map size.
    // This needs to be cast down to a uint16_t because that's what the STR_RESOLUTION_X_BY_Y string takes.
    uint16_t mapSizeArgs[] = { static_cast<uint16_t>(_mapSize - 2), static_cast<uint16_t>(_mapSize - 2) };
    DrawTextBasic(
        dpi,
        w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_SIMPLEX_MAP_SIZE].left + 1, w->widgets[WIDX_SIMPLEX_MAP_SIZE].top + 1 },
        STR_RESOLUTION_X_BY_Y, &mapSizeArgs, { textColour });

    arg = (_waterLevel - 12) / 2;
    DrawTextBasic(
        dpi,
        w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_SIMPLEX_WATER_LEVEL].left + 1, w->widgets[WIDX_SIMPLEX_WATER_LEVEL].top + 1 },
        STR_COMMA16, &arg, { textColour });
}

#pragma endregion

#pragma region Heightmap page

static void window_mapgen_heightmap_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_HEIGHTMAP_STRENGTH_UP:
            _heightmapSmoothStrength = std::min(_heightmapSmoothStrength + 1, MAX_SMOOTH_ITERATIONS);
            widget_invalidate(w, WIDX_HEIGHTMAP_STRENGTH);
            break;
        case WIDX_HEIGHTMAP_STRENGTH_DOWN:
            _heightmapSmoothStrength = std::max(_heightmapSmoothStrength - 1, 1);
            widget_invalidate(w, WIDX_HEIGHTMAP_STRENGTH);
            break;
        case WIDX_HEIGHTMAP_LOW_UP:
            _heightmapLow = std::min(_heightmapLow + 1, 142 - 1);
            _heightmapHigh = std::max(_heightmapHigh, _heightmapLow + 1);
            widget_invalidate(w, WIDX_HEIGHTMAP_LOW);
            break;
        case WIDX_HEIGHTMAP_LOW_DOWN:
            _heightmapLow = std::max(_heightmapLow - 1, 2);
            widget_invalidate(w, WIDX_HEIGHTMAP_LOW);
            break;
        case WIDX_HEIGHTMAP_HIGH_UP:
            _heightmapHigh = std::min(_heightmapHigh + 1, 142);
            widget_invalidate(w, WIDX_HEIGHTMAP_HIGH);
            break;
        case WIDX_HEIGHTMAP_HIGH_DOWN:
            _heightmapHigh = std::max(_heightmapHigh - 1, 2 + 1);
            _heightmapLow = std::min(_heightmapLow, _heightmapHigh - 1);
            widget_invalidate(w, WIDX_HEIGHTMAP_HIGH);
            break;
        case WIDX_HEIGHTMAP_WATER_LEVEL_UP:
            _waterLevel = std::min(_waterLevel + 2, 54);
            widget_invalidate(w, WIDX_HEIGHTMAP_WATER_LEVEL);
            break;
        case WIDX_HEIGHTMAP_WATER_LEVEL_DOWN:
            _waterLevel = std::max(_waterLevel - 2, 0);
            widget_invalidate(w, WIDX_HEIGHTMAP_WATER_LEVEL);
            break;
    }
}

static void window_mapgen_heightmap_generate_map()
{
    mapgen_settings mapgenSettings;
    mapgenSettings.water_level = _waterLevel;
    mapgenSettings.smooth = _heightmapSmoothTiles;
    mapgenSettings.smooth_height_map = _heightmapSmoothMap;
    mapgenSettings.smooth_strength = _heightmapSmoothStrength;
    mapgenSettings.normalize_height = _heightmapNormalize;
    mapgenSettings.simplex_low = _heightmapLow;
    mapgenSettings.simplex_high = _heightmapHigh;
    mapgen_generate_from_heightmap(&mapgenSettings);
    gfx_invalidate_screen();
}

static void window_mapgen_heightmap_loadsave_callback(int32_t result, const utf8* path)
{
    if (result == MODAL_RESULT_OK)
    {
        if (!mapgen_load_heightmap(path))
        {
            // TODO: Display error popup
            return;
        }

        // The window needs to be open while using the map
        rct_window* const w = context_open_window(WC_MAPGEN);
        _heightmapLoaded = true;
        window_mapgen_set_page(w, WINDOW_MAPGEN_PAGE_HEIGHTMAP);

        window_mapgen_heightmap_generate_map();
    }
}

static void window_mapgen_heightmap_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    window_mapgen_shared_mouseup(w, widgetIndex);

    switch (widgetIndex)
    {
        case WIDX_CLOSE:
        case WIDX_TAB_1:
        case WIDX_TAB_2:
        case WIDX_TAB_3:
        case WIDX_TAB_4:
            return; // Only widgets that change a setting need to regenerate the map

            // Page widgets
        case WIDX_HEIGHTMAP_SELECT:
        {
            auto intent = Intent(WC_LOADSAVE);
            intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_HEIGHTMAP);
            intent.putExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<void*>(window_mapgen_heightmap_loadsave_callback));
            context_open_intent(&intent);
            return;
        }
        case WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP:
            _heightmapSmoothMap = !_heightmapSmoothMap;
            WidgetSetCheckboxValue(w, WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _heightmapSmoothMap);
            WidgetSetEnabled(w, WIDX_HEIGHTMAP_STRENGTH, _heightmapSmoothMap);
            WidgetSetEnabled(w, WIDX_HEIGHTMAP_STRENGTH_UP, _heightmapSmoothMap);
            WidgetSetEnabled(w, WIDX_HEIGHTMAP_STRENGTH_DOWN, _heightmapSmoothMap);
            widget_invalidate(w, WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP);
            widget_invalidate(w, WIDX_HEIGHTMAP_STRENGTH);
            break;
        case WIDX_HEIGHTMAP_NORMALIZE:
            _heightmapNormalize = !_heightmapNormalize;
            WidgetSetCheckboxValue(w, WIDX_HEIGHTMAP_NORMALIZE, _heightmapNormalize);
            widget_invalidate(w, WIDX_HEIGHTMAP_NORMALIZE);
            break;
        case WIDX_HEIGHTMAP_SMOOTH_TILES:
            _heightmapSmoothTiles = !_heightmapSmoothTiles;
            WidgetSetCheckboxValue(w, WIDX_HEIGHTMAP_SMOOTH_TILES, _heightmapSmoothTiles);
            widget_invalidate(w, WIDX_HEIGHTMAP_SMOOTH_TILES);
            break;
    }

    // Always regenerate the map after one of the page widgets has been changed
    window_mapgen_heightmap_generate_map();
}

static void window_mapgen_heightmap_invalidate(rct_window* w)
{
    if (w->widgets != PageWidgets[WINDOW_MAPGEN_PAGE_HEIGHTMAP])
    {
        w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_HEIGHTMAP];
        WindowInitScrollWidgets(w);
    }

    WidgetSetCheckboxValue(w, WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _heightmapSmoothMap);
    WidgetSetCheckboxValue(w, WIDX_HEIGHTMAP_NORMALIZE, _heightmapNormalize);
    WidgetSetCheckboxValue(w, WIDX_HEIGHTMAP_SMOOTH_TILES, _heightmapSmoothTiles);

    window_mapgen_set_pressed_tab(w);
}

static void window_mapgen_heightmap_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_mapgen_draw_tab_images(dpi, w);

    const colour_t enabledColour = w->colours[1];
    const colour_t disabledColour = enabledColour | COLOUR_FLAG_INSET;

    // Smooth strength label and value
    const colour_t strengthColour = _heightmapSmoothMap ? enabledColour : disabledColour;
    int16_t strength = _heightmapSmoothStrength;
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_HEIGHTMAP_STRENGTH].top + 1 }, STR_MAPGEN_SMOOTH_STRENGTH, {},
        { strengthColour });
    DrawTextBasic(
        dpi,
        w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_HEIGHTMAP_STRENGTH].left + 1, w->widgets[WIDX_HEIGHTMAP_STRENGTH].top + 1 },
        STR_COMMA16, &strength, { strengthColour });

    // Low label and value
    const colour_t labelColour = _heightmapLoaded ? enabledColour : disabledColour;
    int16_t low = _heightmapLow;
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_HEIGHTMAP_LOW].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_LOW_, {},
        { labelColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_HEIGHTMAP_LOW].left + 1, w->widgets[WIDX_HEIGHTMAP_LOW].top + 1 },
        STR_COMMA16, &low, { labelColour });

    // High label and value
    int16_t high = _heightmapHigh;
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_HEIGHTMAP_HIGH].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_HIGH, {},
        { labelColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_HEIGHTMAP_HIGH].left + 1, w->widgets[WIDX_HEIGHTMAP_HIGH].top + 1 },
        STR_COMMA16, &high, { labelColour });

    // Water level label and value
    int16_t waterLevel = _waterLevel;
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_HEIGHTMAP_WATER_LEVEL].top + 1 }, STR_WATER_LEVEL_LABEL, {},
        { labelColour });
    DrawTextBasic(
        dpi,
        w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_HEIGHTMAP_WATER_LEVEL].left + 1, w->widgets[WIDX_HEIGHTMAP_WATER_LEVEL].top + 1 },
        STR_COMMA16, &waterLevel, { labelColour });
}

#pragma endregion

#pragma region Common

static void window_mapgen_set_page(rct_window* w, int32_t page)
{
    w->page = page;
    w->frame_no = 0;
    w->RemoveViewport();

    w->enabled_widgets = PageEnabledWidgets[page];
    w->hold_down_widgets = HoldDownWidgets[page];
    w->event_handlers = PageEvents[page];
    w->widgets = PageWidgets[page];
    w->disabled_widgets = PageDisabledWidgets[page];
    w->pressed_widgets = PressedWidgets[page];

    // Enable heightmap widgets if one is loaded
    if (page == WINDOW_MAPGEN_PAGE_HEIGHTMAP && _heightmapLoaded)
    {
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, true);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_STRENGTH, _heightmapSmoothMap);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_STRENGTH_UP, _heightmapSmoothMap);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_STRENGTH_DOWN, _heightmapSmoothMap);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_NORMALIZE, true);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_SMOOTH_TILES, true);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_HIGH, true);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_HIGH_UP, true);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_HIGH_DOWN, true);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_LOW, true);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_LOW_UP, true);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_LOW_DOWN, true);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_WATER_LEVEL, true);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_WATER_LEVEL_UP, true);
        WidgetSetEnabled(w, WIDX_HEIGHTMAP_WATER_LEVEL_DOWN, true);
    }

    WindowInitScrollWidgets(w);
    w->Invalidate();
}

static void window_mapgen_set_pressed_tab(rct_window* w)
{
    int32_t i;
    for (i = 0; i < WINDOW_MAPGEN_PAGE_COUNT; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_mapgen_draw_tab_image(rct_drawpixelinfo* dpi, rct_window* w, int32_t page, int32_t spriteIndex)
{
    rct_widgetindex widgetIndex = WIDX_TAB_1 + page;

    if (!(w->disabled_widgets & (1LL << widgetIndex)))
    {
        if (w->page == page)
        {
            int32_t frame = w->frame_no / TabAnimationDivisor[w->page];
            spriteIndex += (frame % TabAnimationFrames[w->page]);
        }

        gfx_draw_sprite(
            dpi, spriteIndex, w->windowPos + ScreenCoordsXY{ w->widgets[widgetIndex].left, w->widgets[widgetIndex].top }, 0);
    }
}

static void window_mapgen_draw_tab_images(rct_drawpixelinfo* dpi, rct_window* w)
{
    window_mapgen_draw_tab_image(dpi, w, WINDOW_MAPGEN_PAGE_BASE, SPR_G2_TAB_LAND);
    window_mapgen_draw_tab_image(dpi, w, WINDOW_MAPGEN_PAGE_RANDOM, SPR_G2_TAB_TREE);
    window_mapgen_draw_tab_image(dpi, w, WINDOW_MAPGEN_PAGE_SIMPLEX, SPR_G2_TAB_PENCIL);
    window_mapgen_draw_tab_image(dpi, w, WINDOW_MAPGEN_PAGE_HEIGHTMAP, SPR_TAB_GRAPH_0);
}

#pragma endregion
