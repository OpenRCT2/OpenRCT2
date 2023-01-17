/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/MapGen.h>
#include <openrct2/world/Surface.h>

using namespace OpenRCT2;

enum
{
    WINDOW_MAPGEN_PAGE_BASE,
    WINDOW_MAPGEN_PAGE_RANDOM,
    WINDOW_MAPGEN_PAGE_SIMPLEX,
    WINDOW_MAPGEN_PAGE_HEIGHTMAP,
    WINDOW_MAPGEN_PAGE_COUNT
};

enum
{
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
    WIDX_MAP_SIZE_Y,
    WIDX_MAP_SIZE_Y_UP,
    WIDX_MAP_SIZE_Y_DOWN,
    WIDX_MAP_SIZE_LINK,
    WIDX_MAP_SIZE_X,
    WIDX_MAP_SIZE_X_UP,
    WIDX_MAP_SIZE_X_DOWN,
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
    WIDX_SIMPLEX_MAP_SIZE_Y,
    WIDX_SIMPLEX_MAP_SIZE_Y_UP,
    WIDX_SIMPLEX_MAP_SIZE_Y_DOWN,
    WIDX_SIMPLEX_MAP_SIZE_LINK,
    WIDX_SIMPLEX_MAP_SIZE_X,
    WIDX_SIMPLEX_MAP_SIZE_X_UP,
    WIDX_SIMPLEX_MAP_SIZE_X_DOWN,
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

static constexpr const StringId WINDOW_TITLE = STR_MAPGEN_WINDOW_TITLE;
static constexpr const int32_t WW = 250;
static constexpr const int32_t WH = 273;

// clang-format off
#define SHARED_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), /* WIDX_BACKGROUND, WIDX_TITLE, WIDX_CLOSE */ \
    MakeWidget({ 0, 43}, {WW, 229}, WindowWidgetType::Resize, WindowColour::Secondary), /* WIDX_PAGE_BACKGROUND */ \
    MakeTab   ({ 3, 17}                                                ), /* WIDX_TAB_1 */ \
    MakeTab   ({34, 17}                                                ), /* WIDX_TAB_2 */ \
    MakeTab   ({65, 17}                                                ), /* WIDX_TAB_3 */ \
    MakeTab   ({96, 17}                                                )  /* WIDX_TAB_4 */

static Widget MapWidgets[] = {
    SHARED_WIDGETS,
    MakeWidget        ({155, 255}, {90, 14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_MAPGEN_ACTION_GENERATE                                 ),
    MakeSpinnerWidgets({104,  52}, {50, 12}, WindowWidgetType::Spinner, WindowColour::Secondary, STR_COMMA16                                                ), // NB: 3 widgets
    MakeWidget        ({155,  52}, {21, 12}, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_G2_LINK_CHAIN),          STR_MAINTAIN_SQUARE_MAP_TOOLTIP),
    MakeSpinnerWidgets({177,  52}, {50, 12}, WindowWidgetType::Spinner, WindowColour::Secondary, STR_POP16_COMMA16                                          ), // NB: 3 widgets
    MakeSpinnerWidgets({104,  70}, {95, 12}, WindowWidgetType::Spinner, WindowColour::Secondary                                                             ), // NB: 3 widgets
    MakeSpinnerWidgets({104,  88}, {95, 12}, WindowWidgetType::Spinner, WindowColour::Secondary                                                             ), // NB: 3 widgets
    MakeWidget        ({104, 106}, {47, 36}, WindowWidgetType::FlatBtn, WindowColour::Secondary, 0xFFFFFFFF,                 STR_CHANGE_BASE_LAND_TIP       ),
    MakeWidget        ({151, 106}, {47, 36}, WindowWidgetType::FlatBtn, WindowColour::Secondary, 0xFFFFFFFF,                 STR_CHANGE_VERTICAL_LAND_TIP   ),
    WIDGETS_END,
};

static Widget RandomWidgets[] = {
    SHARED_WIDGETS,
    MakeWidget({155, 255}, { 90, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAPGEN_ACTION_GENERATE      ),
    MakeWidget({  4,  52}, {195, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAPGEN_OPTION_RANDOM_TERRAIN),
    MakeWidget({  4,  70}, {195, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAPGEN_OPTION_PLACE_TREES   ),
    WIDGETS_END,
};

static Widget SimplexWidgets[] = {
    SHARED_WIDGETS,
    MakeWidget        ({155, 255}, { 90, 14}, WindowWidgetType::Button,        WindowColour::Secondary, STR_MAPGEN_ACTION_GENERATE                                       ), // WIDX_SIMPLEX_GENERATE
    MakeWidget        ({  4,  52}, {195, 12}, WindowWidgetType::LabelCentred,  WindowColour::Secondary, STR_MAPGEN_SIMPLEX_NOISE                                         ), // WIDX_SIMPLEX_LABEL
    MakeSpinnerWidgets({104,  70}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                                                                   ), // WIDX_SIMPLEX_LOW{,_UP,_DOWN}
    MakeSpinnerWidgets({104,  88}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                                                                   ), // WIDX_SIMPLEX_HIGH{,_UP,_DOWN}
    MakeSpinnerWidgets({104, 106}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                                                                   ), // WIDX_SIMPLEX_BASE_FREQ{,_UP,_DOWN}
    MakeSpinnerWidgets({104, 124}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                                                                   ), // WIDX_SIMPLEX_OCTAVES{,_UP,_DOWN}
    MakeSpinnerWidgets({104, 148}, { 50, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary, STR_COMMA16                                                      ), // WIDX_SIMPLEX_MAP_SIZE_Y{,_UP,_DOWN}
    MakeWidget        ({155, 148}, { 21, 12}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, ImageId(SPR_G2_LINK_CHAIN),                STR_MAINTAIN_SQUARE_MAP_TOOLTIP), // WIDX_SIMPLEX_MAP_SIZE_LINK
    MakeSpinnerWidgets({177, 148}, { 50, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary, STR_POP16_COMMA16                                                ), // WIDX_SIMPLEX_MAP_SIZE_X{,_UP,_DOWN}
    MakeSpinnerWidgets({104, 166}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                                                                   ), // WIDX_SIMPLEX_WATER_LEVEL{,_UP,_DOWN}
    MakeWidget        ({104, 190}, { 95, 12}, WindowWidgetType::Checkbox,      WindowColour::Secondary, STR_MAPGEN_OPTION_RANDOM_TERRAIN                                 ), // WIDX_SIMPLEX_RANDOM_TERRAIN_CHECKBOX
    MakeWidget        ({102, 202}, { 47, 36}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, 0xFFFFFFFF,                       STR_CHANGE_BASE_LAND_TIP       ), // WIDX_SIMPLEX_FLOOR_TEXTURE
    MakeWidget        ({150, 202}, { 47, 36}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, 0xFFFFFFFF,                       STR_CHANGE_VERTICAL_LAND_TIP   ), // WIDX_SIMPLEX_WALL_TEXTURE
    MakeWidget        ({104, 239}, { 95, 12}, WindowWidgetType::Checkbox,      WindowColour::Secondary                                                                   ), // WIDX_SIMPLEX_PLACE_TREES_CHECKBOX
    WIDGETS_END,
};

static Widget HeightmapWidgets[] = {
    SHARED_WIDGETS,
    MakeWidget        ({ 95, 255}, {150, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAPGEN_SELECT_HEIGHTMAP), // WIDX_HEIGHTMAP_SELECT
    MakeWidget        ({  4,  52}, {100, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAPGEN_SMOOTH_HEIGHTMAP), // WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP
    MakeSpinnerWidgets({104,  70}, { 95, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                             ), // WIDX_HEIGHTMAP_STRENGTH{,_UP,_DOWN}
    MakeWidget        ({  4,  88}, {100, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAPGEN_NORMALIZE       ), // WIDX_HEIGHTMAP_NORMALIZE
    MakeWidget        ({  4, 106}, {100, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAPGEN_SMOOTH_TILE     ), // WIDX_HEIGHTMAP_SMOOTH_TILES
    MakeSpinnerWidgets({104, 124}, { 95, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                             ), // WIDX_HEIGHTMAP_LOW{,_UP,_DOWN}
    MakeSpinnerWidgets({104, 142}, { 95, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                             ), // WIDX_HEIGHTMAP_HIGH{,_UP,_DOWN}
    MakeSpinnerWidgets({104, 160}, { 95, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                             ), // WIDX_HEIGHTMAP_WATER_LEVEL{,_UP,_DOWN}
    WIDGETS_END,
};
// clang-format on

static Widget* PageWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
    MapWidgets,
    RandomWidgets,
    SimplexWidgets,
    HeightmapWidgets,
};

#pragma endregion

#pragma region Events

static void WindowMapgenSharedClose(rct_window* w);
static void WindowMapgenSharedMouseup(rct_window* w, WidgetIndex widgetIndex);

static void WindowMapgenBaseMouseup(rct_window* w, WidgetIndex widgetIndex);
static void WindowMapgenBaseMousedown(rct_window* w, WidgetIndex widgetIndex, Widget* widget);
static void WindowMapgenBaseDropdown(rct_window* w, WidgetIndex widgetIndex, int32_t dropdownIndex);
static void WindowMapgenBaseUpdate(rct_window* w);
static void WindowMapgenTextinput(rct_window* w, WidgetIndex widgetIndex, char* text);
static void WindowMapgenBaseInvalidate(rct_window* w);
static void WindowMapgenBasePaint(rct_window* w, rct_drawpixelinfo* dpi);

static void WindowMapgenRandomMouseup(rct_window* w, WidgetIndex widgetIndex);
static void WindowMapgenRandomMousedown(rct_window* w, WidgetIndex widgetIndex, Widget* widget);
static void WindowMapgenRandomUpdate(rct_window* w);
static void WindowMapgenRandomInvalidate(rct_window* w);
static void WindowMapgenRandomPaint(rct_window* w, rct_drawpixelinfo* dpi);

static void WindowMapgenSimplexMouseup(rct_window* w, WidgetIndex widgetIndex);
static void WindowMapgenSimplexMousedown(rct_window* w, WidgetIndex widgetIndex, Widget* widget);
static void WindowMapgenSimplexDropdown(rct_window* w, WidgetIndex widgetIndex, int32_t dropdownIndex);
static void WindowMapgenSimplexUpdate(rct_window* w);
static void WindowMapgenSimplexInvalidate(rct_window* w);
static void WindowMapgenSimplexPaint(rct_window* w, rct_drawpixelinfo* dpi);

static void WindowMapgenHeightmapMouseup(rct_window* w, WidgetIndex widgetIndex);
static void WindowMapgenHeightmapMousedown(rct_window* w, WidgetIndex widgetIndex, Widget* widget);
static void WindowMapgenHeightmapInvalidate(rct_window* w);
static void WindowMapgenHeightmapPaint(rct_window* w, rct_drawpixelinfo* dpi);

static WindowEventList BaseEvents([](auto& events) {
    events.close = &WindowMapgenSharedClose;
    events.mouse_up = &WindowMapgenBaseMouseup;
    events.mouse_down = &WindowMapgenBaseMousedown;
    events.dropdown = &WindowMapgenBaseDropdown;
    events.update = &WindowMapgenBaseUpdate;
    events.text_input = &WindowMapgenTextinput;
    events.invalidate = &WindowMapgenBaseInvalidate;
    events.paint = &WindowMapgenBasePaint;
});

static WindowEventList RandomEvents([](auto& events) {
    events.close = &WindowMapgenSharedClose;
    events.mouse_up = &WindowMapgenRandomMouseup;
    events.mouse_down = &WindowMapgenRandomMousedown;
    events.update = &WindowMapgenRandomUpdate;
    events.invalidate = &WindowMapgenRandomInvalidate;
    events.paint = &WindowMapgenRandomPaint;
});

static WindowEventList SimplexEvents([](auto& events) {
    events.close = &WindowMapgenSharedClose;
    events.mouse_up = &WindowMapgenSimplexMouseup;
    events.mouse_down = &WindowMapgenSimplexMousedown;
    events.dropdown = &WindowMapgenSimplexDropdown;
    events.update = &WindowMapgenSimplexUpdate;
    events.text_input = &WindowMapgenTextinput;
    events.invalidate = &WindowMapgenSimplexInvalidate;
    events.paint = &WindowMapgenSimplexPaint;
});

static WindowEventList HeightmapEvents([](auto& events) {
    events.close = &WindowMapgenSharedClose;
    events.mouse_up = &WindowMapgenHeightmapMouseup;
    events.mouse_down = &WindowMapgenHeightmapMousedown;
    events.invalidate = &WindowMapgenHeightmapInvalidate;
    events.paint = &WindowMapgenHeightmapPaint;
});

static WindowEventList* PageEvents[] = {
    &BaseEvents,
    &RandomEvents,
    &SimplexEvents,
    &HeightmapEvents,
};

#pragma endregion

#pragma region Widget flags

// clang-format off
static uint64_t PageDisabledWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
    0,

    0,

    0,

    (1uLL << WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP) |
    (1uLL << WIDX_HEIGHTMAP_STRENGTH) |
    (1uLL << WIDX_HEIGHTMAP_STRENGTH_UP) |
    (1uLL << WIDX_HEIGHTMAP_STRENGTH_DOWN) |
    (1uLL << WIDX_HEIGHTMAP_NORMALIZE) |
    (1uLL << WIDX_HEIGHTMAP_SMOOTH_TILES) |
    (1uLL << WIDX_HEIGHTMAP_HIGH) |
    (1uLL << WIDX_HEIGHTMAP_HIGH_UP) |
    (1uLL << WIDX_HEIGHTMAP_HIGH_DOWN) |
    (1uLL << WIDX_HEIGHTMAP_LOW) |
    (1uLL << WIDX_HEIGHTMAP_LOW_UP) |
    (1uLL << WIDX_HEIGHTMAP_LOW_DOWN) |
    (1uLL << WIDX_HEIGHTMAP_WATER_LEVEL) |
    (1uLL << WIDX_HEIGHTMAP_WATER_LEVEL_UP) |
    (1uLL << WIDX_HEIGHTMAP_WATER_LEVEL_DOWN)
};

static uint64_t HoldDownWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
    (1uLL << WIDX_MAP_SIZE_Y_UP) |
    (1uLL << WIDX_MAP_SIZE_Y_DOWN) |
    (1uLL << WIDX_MAP_SIZE_X_UP) |
    (1uLL << WIDX_MAP_SIZE_X_DOWN) |
    (1uLL << WIDX_BASE_HEIGHT_UP) |
    (1uLL << WIDX_BASE_HEIGHT_DOWN) |
    (1uLL << WIDX_WATER_LEVEL_UP) |
    (1uLL << WIDX_WATER_LEVEL_DOWN),

    0,

    (1uLL << WIDX_SIMPLEX_LOW_UP) |
    (1uLL << WIDX_SIMPLEX_LOW_DOWN) |
    (1uLL << WIDX_SIMPLEX_HIGH_UP) |
    (1uLL << WIDX_SIMPLEX_HIGH_DOWN) |
    (1uLL << WIDX_SIMPLEX_BASE_FREQ_UP) |
    (1uLL << WIDX_SIMPLEX_BASE_FREQ_DOWN) |
    (1uLL << WIDX_SIMPLEX_OCTAVES_UP) |
    (1uLL << WIDX_SIMPLEX_OCTAVES_DOWN) |
    (1uLL << WIDX_SIMPLEX_MAP_SIZE_Y_UP) |
    (1uLL << WIDX_SIMPLEX_MAP_SIZE_Y_DOWN) |
    (1uLL << WIDX_SIMPLEX_MAP_SIZE_X_UP) |
    (1uLL << WIDX_SIMPLEX_MAP_SIZE_X_DOWN) |
    (1uLL << WIDX_SIMPLEX_WATER_LEVEL_UP) |
    (1uLL << WIDX_SIMPLEX_WATER_LEVEL_DOWN),

    (1uLL << WIDX_HEIGHTMAP_STRENGTH_UP) |
    (1uLL << WIDX_HEIGHTMAP_STRENGTH_DOWN) |
    (1uLL << WIDX_HEIGHTMAP_LOW_UP) |
    (1uLL << WIDX_HEIGHTMAP_LOW_DOWN) |
    (1uLL << WIDX_HEIGHTMAP_HIGH_UP) |
    (1uLL << WIDX_HEIGHTMAP_HIGH_DOWN) |
    (1uLL << WIDX_HEIGHTMAP_WATER_LEVEL_UP) |
    (1uLL << WIDX_HEIGHTMAP_WATER_LEVEL_DOWN)
};

static uint64_t PressedWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
    0,
    0,
    0,
    (1uLL << WIDX_HEIGHTMAP_SMOOTH_TILES)
};
// clang-format on

#pragma endregion

static constexpr const int32_t TabAnimationDivisor[WINDOW_MAPGEN_PAGE_COUNT] = {
    1,
    1,
    1,
    1,
};
static constexpr const int32_t TabAnimationFrames[WINDOW_MAPGEN_PAGE_COUNT] = {
    1,
    1,
    1,
    1,
};
static constexpr const int32_t TabAnimationLoops[WINDOW_MAPGEN_PAGE_COUNT] = {
    16,
    16,
    16,
    0,
};
// clang-format on

constexpr int32_t BASESIZE_MIN = 0;
constexpr int32_t BASESIZE_MAX = 60;
constexpr int32_t WATERLEVEL_MIN = 0;
constexpr int32_t WATERLEVEL_MAX = 54;
constexpr int32_t MAX_SMOOTH_ITERATIONS = 20;

static void WindowMapgenSetPage(rct_window* w, int32_t page);
static void WindowMapgenSetPressedTab(rct_window* w);
static void WindowMapgenDrawTabImages(rct_drawpixelinfo* dpi, rct_window* w);

enum class ResizeDirection
{
    Both,
    X,
    Y,
};

static TileCoordsXY _mapSize{ 150, 150 };
static ResizeDirection _resizeDirection{ ResizeDirection::Both };
static bool _mapWidthAndHeightLinked{ true };
static int32_t _baseHeight = 12;
static int32_t _waterLevel = 6;
static int32_t _floorTexture = 0;
static int32_t _wallTexture = 0;
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

static void WindowMapgenChangeMapSize(int32_t sizeOffset)
{
    if (_mapWidthAndHeightLinked)
        _resizeDirection = ResizeDirection::Both;

    if (_resizeDirection != ResizeDirection::X)
        _mapSize.y = std::clamp(_mapSize.y + sizeOffset, MINIMUM_MAP_SIZE_TECHNICAL, MAXIMUM_MAP_SIZE_TECHNICAL);
    if (_resizeDirection != ResizeDirection::Y)
        _mapSize.x = std::clamp(_mapSize.x + sizeOffset, MINIMUM_MAP_SIZE_TECHNICAL, MAXIMUM_MAP_SIZE_TECHNICAL);
}

rct_window* WindowMapgenOpen()
{
    rct_window* w = WindowBringToFrontByClass(WindowClass::Mapgen);
    if (w != nullptr)
    {
        return w;
    }

    w = WindowCreateCentred(WW, WH, PageEvents[WINDOW_MAPGEN_PAGE_BASE], WindowClass::Mapgen, WF_10);
    w->number = 0;
    w->frame_no = 0;

    w->page = WINDOW_MAPGEN_PAGE_BASE;
    w->Invalidate();
    w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_BASE];
    w->hold_down_widgets = HoldDownWidgets[WINDOW_MAPGEN_PAGE_BASE];
    w->event_handlers = PageEvents[WINDOW_MAPGEN_PAGE_BASE];
    w->pressed_widgets = PressedWidgets[WINDOW_MAPGEN_PAGE_BASE];
    w->disabled_widgets = PageDisabledWidgets[WINDOW_MAPGEN_PAGE_BASE];
    WindowInitScrollWidgets(*w);

    _heightmapLoaded = false;

    return w;
}

static void WindowMapgenSharedClose(rct_window* w)
{
    MapGenUnloadHeightmap();
}

static void WindowMapgenSharedMouseup(rct_window* w, WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            WindowClose(*w);
            break;
        case WIDX_TAB_1:
        case WIDX_TAB_2:
        case WIDX_TAB_3:
        case WIDX_TAB_4:
            WindowMapgenSetPage(w, widgetIndex - WIDX_TAB_1);
            break;
    }
}

static void WindowMapgenInputMapSize(rct_window* w, WidgetIndex callingWidget, int32_t currentValue)
{
    Formatter ft;
    ft.Add<int16_t>(MINIMUM_MAP_SIZE_PRACTICAL);
    ft.Add<int16_t>(MAXIMUM_MAP_SIZE_PRACTICAL);

    // Practical map size is 2 lower than the technical map size
    currentValue -= 2;
    WindowTextInputOpen(w, callingWidget, STR_MAP_SIZE_2, STR_ENTER_MAP_SIZE, ft, STR_FORMAT_INTEGER, currentValue, 4);
}

#pragma region Base page

static void WindowMapgenBaseMouseup(rct_window* w, WidgetIndex widgetIndex)
{
    WindowMapgenSharedMouseup(w, widgetIndex);

    mapgen_settings mapgenSettings;
    Formatter ft;
    switch (widgetIndex)
    {
        case WIDX_MAP_GENERATE:
            mapgenSettings.mapSize = _mapSize;
            mapgenSettings.height = _baseHeight + 2;
            mapgenSettings.water_level = _waterLevel + 2;
            mapgenSettings.floor = _floorTexture;
            mapgenSettings.wall = _wallTexture;

            MapGenGenerateBlank(&mapgenSettings);
            GfxInvalidateScreen();
            break;
        case WIDX_MAP_SIZE_Y:
            _resizeDirection = ResizeDirection::Y;
            WindowMapgenInputMapSize(w, WIDX_MAP_SIZE_Y, _mapSize.y);
            break;
        case WIDX_MAP_SIZE_X:
            _resizeDirection = ResizeDirection::X;
            WindowMapgenInputMapSize(w, WIDX_MAP_SIZE_X, _mapSize.x);
            break;
        case WIDX_MAP_SIZE_LINK:
            _mapWidthAndHeightLinked = !_mapWidthAndHeightLinked;
            break;
        case WIDX_BASE_HEIGHT:
            ft.Add<int16_t>((BASESIZE_MIN - 12) / 2);
            ft.Add<int16_t>((BASESIZE_MAX - 12) / 2);
            WindowTextInputOpen(
                w, WIDX_BASE_HEIGHT, STR_BASE_HEIGHT, STR_ENTER_BASE_HEIGHT, ft, STR_FORMAT_INTEGER, (_baseHeight - 12) / 2, 3);
            break;
        case WIDX_WATER_LEVEL:
            ft.Add<int16_t>((WATERLEVEL_MIN - 12) / 2);
            ft.Add<int16_t>((WATERLEVEL_MAX - 12) / 2);
            WindowTextInputOpen(
                w, WIDX_WATER_LEVEL, STR_WATER_LEVEL, STR_ENTER_WATER_LEVEL, ft, STR_FORMAT_INTEGER, (_waterLevel - 12) / 2, 3);
            break;
    }
}

static void WindowMapgenBaseMousedown(rct_window* w, WidgetIndex widgetIndex, Widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_MAP_SIZE_Y_UP:
            _resizeDirection = ResizeDirection::Y;
            WindowMapgenChangeMapSize(+1);
            w->Invalidate();
            break;
        case WIDX_MAP_SIZE_Y_DOWN:
            _resizeDirection = ResizeDirection::Y;
            WindowMapgenChangeMapSize(-1);
            w->Invalidate();
            break;
        case WIDX_MAP_SIZE_X_UP:
            _resizeDirection = ResizeDirection::X;
            WindowMapgenChangeMapSize(+1);
            w->Invalidate();
            break;
        case WIDX_MAP_SIZE_X_DOWN:
            _resizeDirection = ResizeDirection::X;
            WindowMapgenChangeMapSize(-1);
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

static void WindowMapgenBaseDropdown(rct_window* w, WidgetIndex widgetIndex, int32_t dropdownIndex)
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

static void WindowMapgenBaseUpdate(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= TabAnimationLoops[w->page])
        w->frame_no = 0;
    WidgetInvalidate(*w, WIDX_TAB_1);
}

static void WindowMapgenTextinput(rct_window* w, WidgetIndex widgetIndex, char* text)
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
        case WIDX_MAP_SIZE_Y:
        case WIDX_MAP_SIZE_X:
        case WIDX_SIMPLEX_MAP_SIZE_Y:
        case WIDX_SIMPLEX_MAP_SIZE_X:
            // The practical size is 2 lower than the technical size
            value += 2;
            if (_resizeDirection == ResizeDirection::Y || _mapWidthAndHeightLinked)
                _mapSize.y = value;
            if (_resizeDirection == ResizeDirection::X || _mapWidthAndHeightLinked)
                _mapSize.x = value;
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

static void WindowMapgenBaseInvalidate(rct_window* w)
{
    if (w->widgets != PageWidgets[WINDOW_MAPGEN_PAGE_BASE])
    {
        w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_BASE];
        WindowInitScrollWidgets(*w);
    }

    // Only allow linking the map size when X and Y are the same
    WidgetSetPressed(*w, WIDX_MAP_SIZE_LINK, _mapWidthAndHeightLinked);
    WidgetSetDisabled(*w, WIDX_MAP_SIZE_LINK, _mapSize.x != _mapSize.y);

    WindowMapgenSetPressedTab(w);

    // Push width (Y) and height (X) to the common formatter arguments for the map size spinners to use
    auto ft = Formatter::Common();
    ft.Add<uint16_t>(_mapSize.y - 2);
    ft.Add<uint16_t>(_mapSize.x - 2);
}

static void WindowMapgenDrawDropdownButton(rct_window* w, rct_drawpixelinfo* dpi, WidgetIndex widgetIndex, ImageId image)
{
    const auto& widget = w->widgets[widgetIndex];
    ScreenCoordsXY pos = { w->windowPos.x + widget.left, w->windowPos.y + widget.top };
    if (WidgetIsDisabled(*w, widgetIndex))
    {
        // Draw greyed out (light border bottom right shadow)
        auto colour = w->colours[widget.colour];
        colour = ColourMapA[NOT_TRANSLUCENT(colour)].lighter;
        GfxDrawSpriteSolid(dpi, image, pos + ScreenCoordsXY{ 1, 1 }, colour);

        // Draw greyed out (dark)
        colour = w->colours[widget.colour];
        colour = ColourMapA[NOT_TRANSLUCENT(colour)].mid_light;
        GfxDrawSpriteSolid(dpi, image, pos, colour);
    }
    else
    {
        GfxDrawSprite(dpi, image, pos);
    }
}

static void WindowMapgenDrawDropdownButtons(
    rct_window* w, rct_drawpixelinfo* dpi, WidgetIndex floorWidgetIndex, WidgetIndex edgeWidgetIndex)
{
    auto& objManager = GetContext()->GetObjectManager();
    const auto surfaceObj = static_cast<TerrainSurfaceObject*>(
        objManager.GetLoadedObject(ObjectType::TerrainSurface, _floorTexture));
    ImageId surfaceImage;
    if (surfaceObj != nullptr)
    {
        surfaceImage = ImageId(surfaceObj->IconImageId);
        if (surfaceObj->Colour != 255)
        {
            surfaceImage = surfaceImage.WithPrimary(surfaceObj->Colour);
        }
    }

    ImageId edgeImage;
    const auto edgeObj = static_cast<TerrainEdgeObject*>(objManager.GetLoadedObject(ObjectType::TerrainEdge, _wallTexture));
    if (edgeObj != nullptr)
    {
        edgeImage = ImageId(edgeObj->IconImageId);
    }

    WindowMapgenDrawDropdownButton(w, dpi, floorWidgetIndex, surfaceImage);
    WindowMapgenDrawDropdownButton(w, dpi, edgeWidgetIndex, edgeImage);
}

static void WindowMapgenBasePaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);
    WindowMapgenDrawTabImages(dpi, w);
    WindowMapgenDrawDropdownButtons(w, dpi, WIDX_FLOOR_TEXTURE, WIDX_WALL_TEXTURE);

    const auto textColour = w->colours[1];

    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 4, w->widgets[WIDX_MAP_SIZE_Y].top + 1 }, STR_MAP_SIZE, {}, { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 4, w->widgets[WIDX_BASE_HEIGHT].top + 1 }, STR_BASE_HEIGHT_LABEL, {},
        { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 4, w->widgets[WIDX_WATER_LEVEL].top + 1 }, STR_WATER_LEVEL_LABEL, {},
        { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 4, w->widgets[WIDX_FLOOR_TEXTURE].top + 1 }, STR_TERRAIN_LABEL, {}, { textColour });

    auto ft = Formatter();
    ft.Add<uint16_t>((_baseHeight - 12) / 2);
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_BASE_HEIGHT].left + 1, w->widgets[WIDX_BASE_HEIGHT].top + 1 },
        STR_COMMA16, ft, { w->colours[1] });

    ft = Formatter();
    ft.Add<uint16_t>((_waterLevel - 12) / 2);
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_WATER_LEVEL].left + 1, w->widgets[WIDX_WATER_LEVEL].top + 1 },
        STR_COMMA16, ft, { w->colours[1] });
}

#pragma endregion

#pragma region Random page

static void WindowMapgenRandomMouseup(rct_window* w, WidgetIndex widgetIndex)
{
    WindowMapgenSharedMouseup(w, widgetIndex);

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

            MapGenGenerate(&mapgenSettings);
            GfxInvalidateScreen();
            break;
        case WIDX_RANDOM_TERRAIN:
            _randomTerrain = !_randomTerrain;
            break;
        case WIDX_RANDOM_PLACE_TREES:
            _placeTrees ^= 1;
            break;
    }
}

static void WindowMapgenRandomMousedown(rct_window* w, WidgetIndex widgetIndex, Widget* widget)
{
}

static void WindowMapgenRandomUpdate(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= TabAnimationLoops[w->page])
        w->frame_no = 0;
    WidgetInvalidate(*w, WIDX_TAB_2);
}

static void WindowMapgenRandomInvalidate(rct_window* w)
{
    if (w->widgets != PageWidgets[WINDOW_MAPGEN_PAGE_RANDOM])
    {
        w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_RANDOM];
        WindowInitScrollWidgets(*w);
    }

    w->pressed_widgets = 0;
    if (_randomTerrain)
        w->pressed_widgets |= 1uLL << WIDX_RANDOM_TERRAIN;
    if (_placeTrees)
        w->pressed_widgets |= 1uLL << WIDX_RANDOM_PLACE_TREES;

    WindowMapgenSetPressedTab(w);
}

static void WindowMapgenRandomPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);
    WindowMapgenDrawTabImages(dpi, w);
}

#pragma endregion

#pragma region Simplex page

static void WindowMapgenSimplexMouseup(rct_window* w, WidgetIndex widgetIndex)
{
    WindowMapgenSharedMouseup(w, widgetIndex);

    mapgen_settings mapgenSettings;

    switch (widgetIndex)
    {
        case WIDX_SIMPLEX_MAP_SIZE_Y:
            _resizeDirection = ResizeDirection::Y;
            WindowMapgenInputMapSize(w, WIDX_SIMPLEX_MAP_SIZE_Y, _mapSize.y);
            break;
        case WIDX_SIMPLEX_MAP_SIZE_X:
            _resizeDirection = ResizeDirection::X;
            WindowMapgenInputMapSize(w, WIDX_SIMPLEX_MAP_SIZE_X, _mapSize.x);
            break;
        case WIDX_SIMPLEX_MAP_SIZE_LINK:
            _mapWidthAndHeightLinked = !_mapWidthAndHeightLinked;
            break;
        case WIDX_SIMPLEX_GENERATE:
            mapgenSettings.mapSize = _mapSize;

            mapgenSettings.height = _baseHeight;
            mapgenSettings.water_level = _waterLevel + MINIMUM_WATER_HEIGHT;
            mapgenSettings.floor = _randomTerrain ? -1 : _floorTexture;
            mapgenSettings.wall = _randomTerrain ? -1 : _wallTexture;
            mapgenSettings.trees = _placeTrees;

            mapgenSettings.simplex_low = _simplex_low;
            mapgenSettings.simplex_high = _simplex_high;
            mapgenSettings.simplex_base_freq = (static_cast<float>(_simplex_base_freq)) / 100.00f;
            mapgenSettings.simplex_octaves = _simplex_octaves;

            MapGenGenerate(&mapgenSettings);
            GfxInvalidateScreen();
            break;
    }
}

static void WindowMapgenSimplexMousedown(rct_window* w, WidgetIndex widgetIndex, Widget* widget)
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
        case WIDX_SIMPLEX_MAP_SIZE_Y_UP:
            _resizeDirection = ResizeDirection::Y;
            WindowMapgenChangeMapSize(+1);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_MAP_SIZE_Y_DOWN:
            _resizeDirection = ResizeDirection::Y;
            WindowMapgenChangeMapSize(-1);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_MAP_SIZE_X_UP:
            _resizeDirection = ResizeDirection::X;
            WindowMapgenChangeMapSize(+1);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_MAP_SIZE_X_DOWN:
            _resizeDirection = ResizeDirection::X;
            WindowMapgenChangeMapSize(-1);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_WATER_LEVEL_UP:
            _waterLevel = std::min(_waterLevel + MINIMUM_WATER_HEIGHT, MINIMUM_WATER_HEIGHT + MAXIMUM_WATER_HEIGHT);
            w->Invalidate();
            break;
        case WIDX_SIMPLEX_WATER_LEVEL_DOWN:
            _waterLevel = std::max(_waterLevel - MINIMUM_WATER_HEIGHT, 0);
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

static void WindowMapgenSimplexDropdown(rct_window* w, WidgetIndex widgetIndex, int32_t dropdownIndex)
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

static void WindowMapgenSimplexUpdate(rct_window* w)
{
    // Tab animation
    if (++w->frame_no >= TabAnimationLoops[w->page])
        w->frame_no = 0;
    WidgetInvalidate(*w, WIDX_TAB_3);
}

static void WindowMapgenSimplexInvalidate(rct_window* w)
{
    if (w->widgets != PageWidgets[WINDOW_MAPGEN_PAGE_SIMPLEX])
    {
        w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_SIMPLEX];
        WindowInitScrollWidgets(*w);
    }

    // Only allow linking the map size when X and Y are the same
    WidgetSetPressed(*w, WIDX_SIMPLEX_MAP_SIZE_LINK, _mapWidthAndHeightLinked);
    WidgetSetDisabled(*w, WIDX_SIMPLEX_MAP_SIZE_LINK, _mapSize.x != _mapSize.y);

    WidgetSetCheckboxValue(*w, WIDX_SIMPLEX_RANDOM_TERRAIN_CHECKBOX, _randomTerrain != 0);
    WidgetSetCheckboxValue(*w, WIDX_SIMPLEX_PLACE_TREES_CHECKBOX, _placeTrees != 0);

    // Only allow floor and wall texture options if random terrain is disabled
    if (!_randomTerrain)
    {
        WidgetSetEnabled(*w, WIDX_SIMPLEX_FLOOR_TEXTURE, true);
        WidgetSetEnabled(*w, WIDX_SIMPLEX_WALL_TEXTURE, true);
    }
    else
    {
        WidgetSetEnabled(*w, WIDX_SIMPLEX_FLOOR_TEXTURE, false);
        WidgetSetEnabled(*w, WIDX_SIMPLEX_WALL_TEXTURE, false);
    }

    WindowMapgenSetPressedTab(w);

    // Push width (Y) and height (X) to the common formatter arguments for the map size spinners to use
    auto ft = Formatter::Common();
    ft.Add<uint16_t>(_mapSize.y - 2);
    ft.Add<uint16_t>(_mapSize.x - 2);
}

static void WindowMapgenSimplexPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);
    WindowMapgenDrawTabImages(dpi, w);
    WindowMapgenDrawDropdownButtons(w, dpi, WIDX_SIMPLEX_FLOOR_TEXTURE, WIDX_SIMPLEX_WALL_TEXTURE);

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
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_SIMPLEX_MAP_SIZE_Y].top + 1 }, STR_MAP_SIZE, {}, { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_SIMPLEX_WATER_LEVEL].top + 1 }, STR_WATER_LEVEL_LABEL, {},
        { textColour });

    auto ft = Formatter();
    ft.Add<uint16_t>(_simplex_low);
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_SIMPLEX_LOW].left + 1, w->widgets[WIDX_SIMPLEX_LOW].top + 1 },
        STR_COMMA16, ft, { textColour });
    ft = Formatter();
    ft.Add<uint16_t>(_simplex_high);
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_SIMPLEX_HIGH].left + 1, w->widgets[WIDX_SIMPLEX_HIGH].top + 1 },
        STR_COMMA16, ft, { textColour });
    ft = Formatter();
    ft.Add<uint16_t>(_simplex_base_freq);
    DrawTextBasic(
        dpi,
        w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_SIMPLEX_BASE_FREQ].left + 1, w->widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1 },
        STR_WINDOW_COLOUR_2_COMMA2DP32, ft, { textColour });
    ft = Formatter();
    ft.Add<uint16_t>(_simplex_octaves);
    DrawTextBasic(
        dpi,
        w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_SIMPLEX_OCTAVES].left + 1, w->widgets[WIDX_SIMPLEX_OCTAVES].top + 1 },
        STR_COMMA16, ft, { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_SIMPLEX_RANDOM_TERRAIN_CHECKBOX].top + 1 }, STR_TERRAIN_LABEL,
        {}, { textColour });
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_SIMPLEX_PLACE_TREES_CHECKBOX].top + 1 },
        STR_MAPGEN_OPTION_PLACE_TREES, {}, { textColour });

    ft = Formatter();
    ft.Add<uint16_t>((_waterLevel - 12) / 2);
    DrawTextBasic(
        dpi,
        w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_SIMPLEX_WATER_LEVEL].left + 1, w->widgets[WIDX_SIMPLEX_WATER_LEVEL].top + 1 },
        STR_COMMA16, ft, { textColour });
}

#pragma endregion

#pragma region Heightmap page

static void WindowMapgenHeightmapMousedown(rct_window* w, WidgetIndex widgetIndex, Widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_HEIGHTMAP_STRENGTH_UP:
            _heightmapSmoothStrength = std::min(_heightmapSmoothStrength + 1, MAX_SMOOTH_ITERATIONS);
            WidgetInvalidate(*w, WIDX_HEIGHTMAP_STRENGTH);
            break;
        case WIDX_HEIGHTMAP_STRENGTH_DOWN:
            _heightmapSmoothStrength = std::max(_heightmapSmoothStrength - 1, 1);
            WidgetInvalidate(*w, WIDX_HEIGHTMAP_STRENGTH);
            break;
        case WIDX_HEIGHTMAP_LOW_UP:
            _heightmapLow = std::min(_heightmapLow + 1, 142 - 1);
            _heightmapHigh = std::max(_heightmapHigh, _heightmapLow + 1);
            WidgetInvalidate(*w, WIDX_HEIGHTMAP_LOW);
            break;
        case WIDX_HEIGHTMAP_LOW_DOWN:
            _heightmapLow = std::max(_heightmapLow - 1, 2);
            WidgetInvalidate(*w, WIDX_HEIGHTMAP_LOW);
            break;
        case WIDX_HEIGHTMAP_HIGH_UP:
            _heightmapHigh = std::min(_heightmapHigh + 1, 142);
            WidgetInvalidate(*w, WIDX_HEIGHTMAP_HIGH);
            break;
        case WIDX_HEIGHTMAP_HIGH_DOWN:
            _heightmapHigh = std::max(_heightmapHigh - 1, 2 + 1);
            _heightmapLow = std::min(_heightmapLow, _heightmapHigh - 1);
            WidgetInvalidate(*w, WIDX_HEIGHTMAP_HIGH);
            break;
        case WIDX_HEIGHTMAP_WATER_LEVEL_UP:
            _waterLevel = std::min(_waterLevel + MINIMUM_WATER_HEIGHT, MINIMUM_WATER_HEIGHT + MAXIMUM_WATER_HEIGHT);
            WidgetInvalidate(*w, WIDX_HEIGHTMAP_WATER_LEVEL);
            break;
        case WIDX_HEIGHTMAP_WATER_LEVEL_DOWN:
            _waterLevel = std::max(_waterLevel - MINIMUM_WATER_HEIGHT, 0);
            WidgetInvalidate(*w, WIDX_HEIGHTMAP_WATER_LEVEL);
            break;
    }
}

static void WindowMapgenHeightmapGenerateMap()
{
    mapgen_settings mapgenSettings;
    mapgenSettings.water_level = _waterLevel;
    mapgenSettings.smooth = _heightmapSmoothTiles;
    mapgenSettings.smooth_height_map = _heightmapSmoothMap;
    mapgenSettings.smooth_strength = _heightmapSmoothStrength;
    mapgenSettings.normalize_height = _heightmapNormalize;
    mapgenSettings.simplex_low = _heightmapLow;
    mapgenSettings.simplex_high = _heightmapHigh;
    MapGenGenerateFromHeightmap(&mapgenSettings);
    GfxInvalidateScreen();
}

static void WindowMapgenHeightmapLoadsaveCallback(int32_t result, const utf8* path)
{
    if (result == MODAL_RESULT_OK)
    {
        if (!MapGenLoadHeightmap(path))
        {
            // TODO: Display error popup
            return;
        }

        // The window needs to be open while using the map
        rct_window* const w = ContextOpenWindow(WindowClass::Mapgen);
        _heightmapLoaded = true;
        WindowMapgenSetPage(w, WINDOW_MAPGEN_PAGE_HEIGHTMAP);

        WindowMapgenHeightmapGenerateMap();
    }
}

static void WindowMapgenHeightmapMouseup(rct_window* w, WidgetIndex widgetIndex)
{
    WindowMapgenSharedMouseup(w, widgetIndex);

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
            auto intent = Intent(WindowClass::Loadsave);
            intent.PutExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_HEIGHTMAP);
            intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<void*>(WindowMapgenHeightmapLoadsaveCallback));
            ContextOpenIntent(&intent);
            return;
        }
        case WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP:
            _heightmapSmoothMap = !_heightmapSmoothMap;
            WidgetSetCheckboxValue(*w, WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _heightmapSmoothMap);
            WidgetSetEnabled(*w, WIDX_HEIGHTMAP_STRENGTH, _heightmapSmoothMap);
            WidgetSetEnabled(*w, WIDX_HEIGHTMAP_STRENGTH_UP, _heightmapSmoothMap);
            WidgetSetEnabled(*w, WIDX_HEIGHTMAP_STRENGTH_DOWN, _heightmapSmoothMap);
            WidgetInvalidate(*w, WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP);
            WidgetInvalidate(*w, WIDX_HEIGHTMAP_STRENGTH);
            break;
        case WIDX_HEIGHTMAP_NORMALIZE:
            _heightmapNormalize = !_heightmapNormalize;
            WidgetSetCheckboxValue(*w, WIDX_HEIGHTMAP_NORMALIZE, _heightmapNormalize);
            WidgetInvalidate(*w, WIDX_HEIGHTMAP_NORMALIZE);
            break;
        case WIDX_HEIGHTMAP_SMOOTH_TILES:
            _heightmapSmoothTiles = !_heightmapSmoothTiles;
            WidgetSetCheckboxValue(*w, WIDX_HEIGHTMAP_SMOOTH_TILES, _heightmapSmoothTiles);
            WidgetInvalidate(*w, WIDX_HEIGHTMAP_SMOOTH_TILES);
            break;
    }

    // Always regenerate the map after one of the page widgets has been changed
    WindowMapgenHeightmapGenerateMap();
}

static void WindowMapgenHeightmapInvalidate(rct_window* w)
{
    if (w->widgets != PageWidgets[WINDOW_MAPGEN_PAGE_HEIGHTMAP])
    {
        w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_HEIGHTMAP];
        WindowInitScrollWidgets(*w);
    }

    WidgetSetCheckboxValue(*w, WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _heightmapSmoothMap);
    WidgetSetCheckboxValue(*w, WIDX_HEIGHTMAP_NORMALIZE, _heightmapNormalize);
    WidgetSetCheckboxValue(*w, WIDX_HEIGHTMAP_SMOOTH_TILES, _heightmapSmoothTiles);

    WindowMapgenSetPressedTab(w);
}

static void WindowMapgenHeightmapPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);
    WindowMapgenDrawTabImages(dpi, w);

    const colour_t enabledColour = w->colours[1];
    const colour_t disabledColour = enabledColour | COLOUR_FLAG_INSET;

    // Smooth strength label and value
    const colour_t strengthColour = _heightmapSmoothMap ? enabledColour : disabledColour;
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_HEIGHTMAP_STRENGTH].top + 1 }, STR_MAPGEN_SMOOTH_STRENGTH, {},
        { strengthColour });

    auto ft = Formatter();
    ft.Add<uint16_t>(_heightmapSmoothStrength);
    DrawTextBasic(
        dpi,
        w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_HEIGHTMAP_STRENGTH].left + 1, w->widgets[WIDX_HEIGHTMAP_STRENGTH].top + 1 },
        STR_COMMA16, ft, { strengthColour });

    // Low label and value
    const colour_t labelColour = _heightmapLoaded ? enabledColour : disabledColour;
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_HEIGHTMAP_LOW].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_LOW_, {},
        { labelColour });

    ft = Formatter();
    ft.Add<uint16_t>(_heightmapLow);
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_HEIGHTMAP_LOW].left + 1, w->widgets[WIDX_HEIGHTMAP_LOW].top + 1 },
        STR_COMMA16, ft, { labelColour });

    // High label and value
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_HEIGHTMAP_HIGH].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_HIGH, {},
        { labelColour });

    ft = Formatter();
    ft.Add<uint16_t>(_heightmapHigh);
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_HEIGHTMAP_HIGH].left + 1, w->widgets[WIDX_HEIGHTMAP_HIGH].top + 1 },
        STR_COMMA16, ft, { labelColour });

    // Water level label and value
    DrawTextBasic(
        dpi, w->windowPos + ScreenCoordsXY{ 5, w->widgets[WIDX_HEIGHTMAP_WATER_LEVEL].top + 1 }, STR_WATER_LEVEL_LABEL, {},
        { labelColour });

    ft = Formatter();
    ft.Add<uint16_t>(_waterLevel);
    DrawTextBasic(
        dpi,
        w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_HEIGHTMAP_WATER_LEVEL].left + 1, w->widgets[WIDX_HEIGHTMAP_WATER_LEVEL].top + 1 },
        STR_COMMA16, ft, { labelColour });
}

#pragma endregion

#pragma region Common

static void WindowMapgenSetPage(rct_window* w, int32_t page)
{
    w->page = page;
    w->frame_no = 0;
    w->RemoveViewport();

    w->hold_down_widgets = HoldDownWidgets[page];
    w->event_handlers = PageEvents[page];
    w->widgets = PageWidgets[page];
    w->disabled_widgets = PageDisabledWidgets[page];
    w->pressed_widgets = PressedWidgets[page];

    // Enable heightmap widgets if one is loaded
    if (page == WINDOW_MAPGEN_PAGE_HEIGHTMAP && _heightmapLoaded)
    {
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, true);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_STRENGTH, _heightmapSmoothMap);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_STRENGTH_UP, _heightmapSmoothMap);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_STRENGTH_DOWN, _heightmapSmoothMap);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_NORMALIZE, true);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_SMOOTH_TILES, true);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_HIGH, true);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_HIGH_UP, true);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_HIGH_DOWN, true);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_LOW, true);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_LOW_UP, true);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_LOW_DOWN, true);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_WATER_LEVEL, true);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_WATER_LEVEL_UP, true);
        WidgetSetEnabled(*w, WIDX_HEIGHTMAP_WATER_LEVEL_DOWN, true);
    }

    WindowInitScrollWidgets(*w);
    w->Invalidate();
}

static void WindowMapgenSetPressedTab(rct_window* w)
{
    int32_t i;
    for (i = 0; i < WINDOW_MAPGEN_PAGE_COUNT; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void WindowMapgenDrawTabImage(rct_drawpixelinfo* dpi, rct_window* w, int32_t page, int32_t spriteIndex)
{
    WidgetIndex widgetIndex = WIDX_TAB_1 + page;

    if (!WidgetIsDisabled(*w, widgetIndex))
    {
        if (w->page == page)
        {
            int32_t frame = w->frame_no / TabAnimationDivisor[w->page];
            spriteIndex += (frame % TabAnimationFrames[w->page]);
        }

        GfxDrawSprite(
            dpi, ImageId(spriteIndex),
            w->windowPos + ScreenCoordsXY{ w->widgets[widgetIndex].left, w->widgets[widgetIndex].top });
    }
}

static void WindowMapgenDrawTabImages(rct_drawpixelinfo* dpi, rct_window* w)
{
    WindowMapgenDrawTabImage(dpi, w, WINDOW_MAPGEN_PAGE_BASE, SPR_G2_TAB_LAND);
    WindowMapgenDrawTabImage(dpi, w, WINDOW_MAPGEN_PAGE_RANDOM, SPR_G2_TAB_TREE);
    WindowMapgenDrawTabImage(dpi, w, WINDOW_MAPGEN_PAGE_SIMPLEX, SPR_G2_TAB_PENCIL);
    WindowMapgenDrawTabImage(dpi, w, WINDOW_MAPGEN_PAGE_HEIGHTMAP, SPR_TAB_GRAPH_0);
}

#pragma endregion
