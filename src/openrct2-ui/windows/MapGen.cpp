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

enum class ResizeDirection
{
    Both,
    X,
    Y,
};

static void HeightmapLoadsaveCallback(int32_t result, const utf8* path);

class MapGenWindow final : public Window
{
private:
    TileCoordsXY _mapSize{ 150, 150 };
    ResizeDirection _resizeDirection{ ResizeDirection::Both };
    bool _mapWidthAndHeightLinked{ true };
    int32_t _baseHeight = 12;
    int32_t _waterLevel = 6;
    int32_t _floorTexture = 0;
    int32_t _wallTexture = 0;
    bool _randomTerrain = true;
    int32_t _placeTrees = 1;

    int32_t _simplex_low = 6;
    int32_t _simplex_high = 10;
    int32_t _simplex_base_freq = 60;
    int32_t _simplex_octaves = 4;

    bool _heightmapLoaded = false;
    bool _heightmapSmoothMap = false;
    int32_t _heightmapSmoothStrength = 1;
    bool _heightmapNormalize = false;
    bool _heightmapSmoothTiles = true;
    int32_t _heightmapLow = 2;
    int32_t _heightmapHigh = 70;

    void SetPage(int32_t newPage)
    {
        page = newPage;
        frame_no = 0;
        RemoveViewport();

        hold_down_widgets = HoldDownWidgets[newPage];
        widgets = PageWidgets[newPage];
        disabled_widgets = PageDisabledWidgets[newPage];
        pressed_widgets = PressedWidgets[newPage];

        // Enable heightmap widgets if one is loaded
        if (newPage == WINDOW_MAPGEN_PAGE_HEIGHTMAP && _heightmapLoaded)
        {
            SetWidgetEnabled(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, true);
            SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH, _heightmapSmoothMap);
            SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_UP, _heightmapSmoothMap);
            SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_DOWN, _heightmapSmoothMap);
            SetWidgetEnabled(WIDX_HEIGHTMAP_NORMALIZE, true);
            SetWidgetEnabled(WIDX_HEIGHTMAP_SMOOTH_TILES, true);
            SetWidgetEnabled(WIDX_HEIGHTMAP_HIGH, true);
            SetWidgetEnabled(WIDX_HEIGHTMAP_HIGH_UP, true);
            SetWidgetEnabled(WIDX_HEIGHTMAP_HIGH_DOWN, true);
            SetWidgetEnabled(WIDX_HEIGHTMAP_LOW, true);
            SetWidgetEnabled(WIDX_HEIGHTMAP_LOW_UP, true);
            SetWidgetEnabled(WIDX_HEIGHTMAP_LOW_DOWN, true);
            SetWidgetEnabled(WIDX_HEIGHTMAP_WATER_LEVEL, true);
            SetWidgetEnabled(WIDX_HEIGHTMAP_WATER_LEVEL_UP, true);
            SetWidgetEnabled(WIDX_HEIGHTMAP_WATER_LEVEL_DOWN, true);
        }

        InitScrollWidgets();
        Invalidate();
    }

    void SetPressedTab()
    {
        int32_t i;
        for (i = 0; i < WINDOW_MAPGEN_PAGE_COUNT; i++)
            pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
        pressed_widgets |= 1LL << (WIDX_TAB_1 + page);
    }

    void DrawTabImage(DrawPixelInfo& dpi, int32_t newPage, int32_t spriteIndex)
    {
        WidgetIndex widgetIndex = WIDX_TAB_1 + newPage;

        if (!WidgetIsDisabled(*this, widgetIndex))
        {
            if (page == newPage)
            {
                int32_t frame = frame_no / TabAnimationDivisor[page];
                spriteIndex += (frame % TabAnimationFrames[page]);
            }

            GfxDrawSprite(
                dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
        }
    }

    void DrawTabImages(DrawPixelInfo& dpi)
    {
        DrawTabImage(dpi, WINDOW_MAPGEN_PAGE_BASE, SPR_G2_TAB_LAND);
        DrawTabImage(dpi, WINDOW_MAPGEN_PAGE_RANDOM, SPR_G2_TAB_TREE);
        DrawTabImage(dpi, WINDOW_MAPGEN_PAGE_SIMPLEX, SPR_G2_TAB_PENCIL);
        DrawTabImage(dpi, WINDOW_MAPGEN_PAGE_HEIGHTMAP, SPR_TAB_GRAPH_0);
    }

    void ChangeMapSize(int32_t sizeOffset)
    {
        if (_mapWidthAndHeightLinked)
            _resizeDirection = ResizeDirection::Both;

        if (_resizeDirection != ResizeDirection::X)
            _mapSize.y = std::clamp(_mapSize.y + sizeOffset, MINIMUM_MAP_SIZE_TECHNICAL, MAXIMUM_MAP_SIZE_TECHNICAL);
        if (_resizeDirection != ResizeDirection::Y)
            _mapSize.x = std::clamp(_mapSize.x + sizeOffset, MINIMUM_MAP_SIZE_TECHNICAL, MAXIMUM_MAP_SIZE_TECHNICAL);
    }

    void InputMapSize(WidgetIndex callingWidget, int32_t currentValue)
    {
        Formatter ft;
        ft.Add<int16_t>(MINIMUM_MAP_SIZE_PRACTICAL);
        ft.Add<int16_t>(MAXIMUM_MAP_SIZE_PRACTICAL);

        // Practical map size is 2 lower than the technical map size
        currentValue -= 2;
        WindowTextInputOpen(this, callingWidget, STR_MAP_SIZE_2, STR_ENTER_MAP_SIZE, ft, STR_FORMAT_INTEGER, currentValue, 4);
    }

    void SharedMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
        }
    }

#pragma region Base page

    void BaseMouseUp(WidgetIndex widgetIndex)
    {
        SharedMouseUp(widgetIndex);

        MapGenSettings mapgenSettings;
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
                InputMapSize(WIDX_MAP_SIZE_Y, _mapSize.y);
                break;
            case WIDX_MAP_SIZE_X:
                _resizeDirection = ResizeDirection::X;
                InputMapSize(WIDX_MAP_SIZE_X, _mapSize.x);
                break;
            case WIDX_MAP_SIZE_LINK:
                _mapWidthAndHeightLinked = !_mapWidthAndHeightLinked;
                break;
            case WIDX_BASE_HEIGHT:
                ft.Add<int16_t>((BASESIZE_MIN - 12) / 2);
                ft.Add<int16_t>((BASESIZE_MAX - 12) / 2);
                WindowTextInputOpen(
                    this, WIDX_BASE_HEIGHT, STR_BASE_HEIGHT, STR_ENTER_BASE_HEIGHT, ft, STR_FORMAT_INTEGER,
                    (_baseHeight - 12) / 2, 3);
                break;
            case WIDX_WATER_LEVEL:
                ft.Add<int16_t>((WATERLEVEL_MIN - 12) / 2);
                ft.Add<int16_t>((WATERLEVEL_MAX - 12) / 2);
                WindowTextInputOpen(
                    this, WIDX_WATER_LEVEL, STR_WATER_LEVEL, STR_ENTER_WATER_LEVEL, ft, STR_FORMAT_INTEGER,
                    (_waterLevel - 12) / 2, 3);
                break;
        }
    }

    void BaseMouseDown(WidgetIndex widgetIndex, Widget* widget)
    {
        switch (widgetIndex)
        {
            case WIDX_MAP_SIZE_Y_UP:
                _resizeDirection = ResizeDirection::Y;
                ChangeMapSize(+1);
                Invalidate();
                break;
            case WIDX_MAP_SIZE_Y_DOWN:
                _resizeDirection = ResizeDirection::Y;
                ChangeMapSize(-1);
                Invalidate();
                break;
            case WIDX_MAP_SIZE_X_UP:
                _resizeDirection = ResizeDirection::X;
                ChangeMapSize(+1);
                Invalidate();
                break;
            case WIDX_MAP_SIZE_X_DOWN:
                _resizeDirection = ResizeDirection::X;
                ChangeMapSize(-1);
                Invalidate();
                break;
            case WIDX_BASE_HEIGHT_UP:
                _baseHeight = std::min(_baseHeight + 2, BASESIZE_MAX);
                Invalidate();
                break;
            case WIDX_BASE_HEIGHT_DOWN:
                _baseHeight = std::max(_baseHeight - 2, BASESIZE_MIN);
                Invalidate();
                break;
            case WIDX_WATER_LEVEL_UP:
                _waterLevel = std::min(_waterLevel + 2, WATERLEVEL_MAX);
                Invalidate();
                break;
            case WIDX_WATER_LEVEL_DOWN:
                _waterLevel = std::max(_waterLevel - 2, WATERLEVEL_MIN);
                Invalidate();
                break;
            case WIDX_FLOOR_TEXTURE:
                LandTool::ShowSurfaceStyleDropdown(this, widget, _floorTexture);
                break;
            case WIDX_WALL_TEXTURE:
                LandTool::ShowEdgeStyleDropdown(this, widget, _wallTexture);
                break;
        }
    }

    void BaseDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
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
                Invalidate();
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
                Invalidate();
                break;
        }
    }

    void BaseUpdate()
    {
        // Tab animation
        if (++frame_no >= TabAnimationLoops[page])
            frame_no = 0;
        InvalidateWidget(WIDX_TAB_1);
    }

    void TextInput(WidgetIndex widgetIndex, std::string_view text)
    {
        int32_t value;
        char* end;

        const auto strText = u8string(text);
        value = strtol(strText.c_str(), &end, 10);

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

        Invalidate();
    }

    void BasePrepareDraw()
    {
        if (widgets != PageWidgets[WINDOW_MAPGEN_PAGE_BASE])
        {
            widgets = PageWidgets[WINDOW_MAPGEN_PAGE_BASE];
            InitScrollWidgets();
        }

        // Only allow linking the map size when X and Y are the same
        SetWidgetPressed(WIDX_MAP_SIZE_LINK, _mapWidthAndHeightLinked);
        SetWidgetDisabled(WIDX_MAP_SIZE_LINK, _mapSize.x != _mapSize.y);

        SetPressedTab();

        // Push width (Y) and height (X) to the common formatter arguments for the map size spinners to use
        auto ft = Formatter::Common();
        ft.Add<uint16_t>(_mapSize.y - 2);
        ft.Add<uint16_t>(_mapSize.x - 2);
    }

    void DrawDropdownButton(DrawPixelInfo& dpi, WidgetIndex widgetIndex, ImageId image)
    {
        const auto& widget = widgets[widgetIndex];
        ScreenCoordsXY pos = { windowPos.x + widget.left, windowPos.y + widget.top };
        if (IsWidgetDisabled(widgetIndex))
        {
            // Draw greyed out (light border bottom right shadow)
            auto colour = colours[widget.colour];
            colour = ColourMapA[NOT_TRANSLUCENT(colour)].lighter;
            GfxDrawSpriteSolid(&dpi, image, pos + ScreenCoordsXY{ 1, 1 }, colour);

            // Draw greyed out (dark)
            colour = colours[widget.colour];
            colour = ColourMapA[NOT_TRANSLUCENT(colour)].mid_light;
            GfxDrawSpriteSolid(&dpi, image, pos, colour);
        }
        else
        {
            GfxDrawSprite(dpi, image, pos);
        }
    }

    void DrawDropdownButtons(DrawPixelInfo& dpi, WidgetIndex floorWidgetIndex, WidgetIndex edgeWidgetIndex)
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

        DrawDropdownButton(dpi, floorWidgetIndex, surfaceImage);
        DrawDropdownButton(dpi, edgeWidgetIndex, edgeImage);
    }

    void BaseDraw(DrawPixelInfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);
        DrawDropdownButtons(dpi, WIDX_FLOOR_TEXTURE, WIDX_WALL_TEXTURE);

        const auto textColour = colours[1];

        DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_MAP_SIZE_Y].top + 1 }, STR_MAP_SIZE, {}, { textColour });
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_BASE_HEIGHT].top + 1 }, STR_BASE_HEIGHT_LABEL, {}, { textColour });
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_WATER_LEVEL].top + 1 }, STR_WATER_LEVEL_LABEL, {}, { textColour });
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_FLOOR_TEXTURE].top + 1 }, STR_TERRAIN_LABEL, {}, { textColour });

        auto ft = Formatter();
        ft.Add<uint16_t>((_baseHeight - 12) / 2);
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_BASE_HEIGHT].left + 1, widgets[WIDX_BASE_HEIGHT].top + 1 },
            STR_COMMA16, ft, { colours[1] });

        ft = Formatter();
        ft.Add<uint16_t>((_waterLevel - 12) / 2);
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_WATER_LEVEL].left + 1, widgets[WIDX_WATER_LEVEL].top + 1 },
            STR_COMMA16, ft, { colours[1] });
    }

#pragma endregion

#pragma region Random page

    void RandomMouseUp(WidgetIndex widgetIndex)
    {
        SharedMouseUp(widgetIndex);

        MapGenSettings mapgenSettings;

        switch (widgetIndex)
        {
            case WIDX_RANDOM_GENERATE:
                mapgenSettings.mapSize = _mapSize;
                mapgenSettings.height = _baseHeight + 2;
                mapgenSettings.water_level = _waterLevel + 2;
                mapgenSettings.floor = _randomTerrain ? -1 : _floorTexture;
                mapgenSettings.wall = _randomTerrain ? -1 : _wallTexture;
                mapgenSettings.trees = _placeTrees;

                mapgenSettings.simplex_low = UtilRand() % 4;
                mapgenSettings.simplex_high = 12 + (UtilRand() % (32 - 12));
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

    void RandomUpdate()
    {
        // Tab animation
        if (++frame_no >= TabAnimationLoops[page])
            frame_no = 0;
        InvalidateWidget(WIDX_TAB_2);
    }

    void RandomPrepareDraw()
    {
        if (widgets != PageWidgets[WINDOW_MAPGEN_PAGE_RANDOM])
        {
            widgets = PageWidgets[WINDOW_MAPGEN_PAGE_RANDOM];
            InitScrollWidgets();
        }

        pressed_widgets = 0;
        if (_randomTerrain)
            pressed_widgets |= 1uLL << WIDX_RANDOM_TERRAIN;
        if (_placeTrees)
            pressed_widgets |= 1uLL << WIDX_RANDOM_PLACE_TREES;

        SetPressedTab();
    }

    void RandomDraw(DrawPixelInfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);
    }

#pragma endregion

#pragma region Simplex page

    void SimplexMouseUp(WidgetIndex widgetIndex)
    {
        SharedMouseUp(widgetIndex);

        MapGenSettings mapgenSettings;

        switch (widgetIndex)
        {
            case WIDX_SIMPLEX_MAP_SIZE_Y:
                _resizeDirection = ResizeDirection::Y;
                InputMapSize(WIDX_SIMPLEX_MAP_SIZE_Y, _mapSize.y);
                break;
            case WIDX_SIMPLEX_MAP_SIZE_X:
                _resizeDirection = ResizeDirection::X;
                InputMapSize(WIDX_SIMPLEX_MAP_SIZE_X, _mapSize.x);
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

    void SimplexMouseDown(WidgetIndex widgetIndex, Widget* widget)
    {
        switch (widgetIndex)
        {
            case WIDX_SIMPLEX_LOW_UP:
                _simplex_low = std::min(_simplex_low + 1, 24);
                Invalidate();
                break;
            case WIDX_SIMPLEX_LOW_DOWN:
                _simplex_low = std::max(_simplex_low - 1, 0);
                Invalidate();
                break;
            case WIDX_SIMPLEX_HIGH_UP:
                _simplex_high = std::min(_simplex_high + 1, 36);
                Invalidate();
                break;
            case WIDX_SIMPLEX_HIGH_DOWN:
                _simplex_high = std::max(_simplex_high - 1, 0);
                Invalidate();
                break;
            case WIDX_SIMPLEX_BASE_FREQ_UP:
                _simplex_base_freq = std::min(_simplex_base_freq + 5, 1000);
                Invalidate();
                break;
            case WIDX_SIMPLEX_BASE_FREQ_DOWN:
                _simplex_base_freq = std::max(_simplex_base_freq - 5, 0);
                Invalidate();
                break;
            case WIDX_SIMPLEX_OCTAVES_UP:
                _simplex_octaves = std::min(_simplex_octaves + 1, 10);
                Invalidate();
                break;
            case WIDX_SIMPLEX_OCTAVES_DOWN:
                _simplex_octaves = std::max(_simplex_octaves - 1, 1);
                Invalidate();
                break;
            case WIDX_SIMPLEX_MAP_SIZE_Y_UP:
                _resizeDirection = ResizeDirection::Y;
                ChangeMapSize(+1);
                Invalidate();
                break;
            case WIDX_SIMPLEX_MAP_SIZE_Y_DOWN:
                _resizeDirection = ResizeDirection::Y;
                ChangeMapSize(-1);
                Invalidate();
                break;
            case WIDX_SIMPLEX_MAP_SIZE_X_UP:
                _resizeDirection = ResizeDirection::X;
                ChangeMapSize(+1);
                Invalidate();
                break;
            case WIDX_SIMPLEX_MAP_SIZE_X_DOWN:
                _resizeDirection = ResizeDirection::X;
                ChangeMapSize(-1);
                Invalidate();
                break;
            case WIDX_SIMPLEX_WATER_LEVEL_UP:
                _waterLevel = std::min(_waterLevel + MINIMUM_WATER_HEIGHT, MINIMUM_WATER_HEIGHT + MAXIMUM_WATER_HEIGHT);
                Invalidate();
                break;
            case WIDX_SIMPLEX_WATER_LEVEL_DOWN:
                _waterLevel = std::max(_waterLevel - MINIMUM_WATER_HEIGHT, 0);
                Invalidate();
                break;
            case WIDX_SIMPLEX_RANDOM_TERRAIN_CHECKBOX:
                _randomTerrain = !_randomTerrain;
                Invalidate();
                break;
            case WIDX_SIMPLEX_FLOOR_TEXTURE:
                LandTool::ShowSurfaceStyleDropdown(this, widget, _floorTexture);
                break;
            case WIDX_SIMPLEX_WALL_TEXTURE:
                LandTool::ShowEdgeStyleDropdown(this, widget, _wallTexture);
                break;
            case WIDX_SIMPLEX_PLACE_TREES_CHECKBOX:
                _placeTrees ^= 1;
                Invalidate();
                break;
        }
    }

    void SimplexDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
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
                Invalidate();
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
                Invalidate();
                break;
        }
    }

    void SimplexUpdate()
    {
        // Tab animation
        if (++frame_no >= TabAnimationLoops[page])
            frame_no = 0;
        InvalidateWidget(WIDX_TAB_3);
    }

    void SimplexPrepareDraw()
    {
        if (widgets != PageWidgets[WINDOW_MAPGEN_PAGE_SIMPLEX])
        {
            widgets = PageWidgets[WINDOW_MAPGEN_PAGE_SIMPLEX];
            InitScrollWidgets();
        }

        // Only allow linking the map size when X and Y are the same
        SetWidgetPressed(WIDX_SIMPLEX_MAP_SIZE_LINK, _mapWidthAndHeightLinked);
        SetWidgetDisabled(WIDX_SIMPLEX_MAP_SIZE_LINK, _mapSize.x != _mapSize.y);

        SetCheckboxValue(WIDX_SIMPLEX_RANDOM_TERRAIN_CHECKBOX, _randomTerrain != 0);
        SetCheckboxValue(WIDX_SIMPLEX_PLACE_TREES_CHECKBOX, _placeTrees != 0);

        // Only allow floor and wall texture options if random terrain is disabled
        if (!_randomTerrain)
        {
            SetWidgetEnabled(WIDX_SIMPLEX_FLOOR_TEXTURE, true);
            SetWidgetEnabled(WIDX_SIMPLEX_WALL_TEXTURE, true);
        }
        else
        {
            SetWidgetEnabled(WIDX_SIMPLEX_FLOOR_TEXTURE, false);
            SetWidgetEnabled(WIDX_SIMPLEX_WALL_TEXTURE, false);
        }

        SetPressedTab();

        // Push width (Y) and height (X) to the common formatter arguments for the map size spinners to use
        auto ft = Formatter::Common();
        ft.Add<uint16_t>(_mapSize.y - 2);
        ft.Add<uint16_t>(_mapSize.x - 2);
    }

    void SimplexDraw(DrawPixelInfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);
        DrawDropdownButtons(dpi, WIDX_SIMPLEX_FLOOR_TEXTURE, WIDX_SIMPLEX_WALL_TEXTURE);

        const uint8_t textColour = colours[1];

        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_SIMPLEX_LOW].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_LOW_, {},
            { textColour });
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_SIMPLEX_HIGH].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_HIGH, {},
            { textColour });
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1 },
            STR_MAPGEN_SIMPLEX_NOISE_BASE_FREQUENCY, {}, { textColour });
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_SIMPLEX_OCTAVES].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_OCTAVES, {},
            { textColour });
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_SIMPLEX_MAP_SIZE_Y].top + 1 }, STR_MAP_SIZE, {}, { textColour });
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_SIMPLEX_WATER_LEVEL].top + 1 }, STR_WATER_LEVEL_LABEL, {},
            { textColour });

        auto ft = Formatter();
        ft.Add<uint16_t>(_simplex_low);
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_LOW].left + 1, widgets[WIDX_SIMPLEX_LOW].top + 1 },
            STR_COMMA16, ft, { textColour });
        ft = Formatter();
        ft.Add<uint16_t>(_simplex_high);
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_HIGH].left + 1, widgets[WIDX_SIMPLEX_HIGH].top + 1 },
            STR_COMMA16, ft, { textColour });
        ft = Formatter();
        ft.Add<uint16_t>(_simplex_base_freq);
        DrawTextBasic(
            dpi,
            windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_BASE_FREQ].left + 1, widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1 },
            STR_WINDOW_COLOUR_2_COMMA2DP32, ft, { textColour });
        ft = Formatter();
        ft.Add<uint16_t>(_simplex_octaves);
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_OCTAVES].left + 1, widgets[WIDX_SIMPLEX_OCTAVES].top + 1 },
            STR_COMMA16, ft, { textColour });
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_SIMPLEX_RANDOM_TERRAIN_CHECKBOX].top + 1 }, STR_TERRAIN_LABEL, {},
            { textColour });
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_SIMPLEX_PLACE_TREES_CHECKBOX].top + 1 },
            STR_MAPGEN_OPTION_PLACE_TREES, {}, { textColour });

        ft = Formatter();
        ft.Add<uint16_t>((_waterLevel - 12) / 2);
        DrawTextBasic(
            dpi,
            windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_WATER_LEVEL].left + 1, widgets[WIDX_SIMPLEX_WATER_LEVEL].top + 1 },
            STR_COMMA16, ft, { textColour });
    }

#pragma endregion

#pragma region Heightmap page

    void HeightmapMouseDown(WidgetIndex widgetIndex, Widget* widget)
    {
        switch (widgetIndex)
        {
            case WIDX_HEIGHTMAP_STRENGTH_UP:
                _heightmapSmoothStrength = std::min(_heightmapSmoothStrength + 1, MAX_SMOOTH_ITERATIONS);
                InvalidateWidget(WIDX_HEIGHTMAP_STRENGTH);
                break;
            case WIDX_HEIGHTMAP_STRENGTH_DOWN:
                _heightmapSmoothStrength = std::max(_heightmapSmoothStrength - 1, 1);
                InvalidateWidget(WIDX_HEIGHTMAP_STRENGTH);
                break;
            case WIDX_HEIGHTMAP_LOW_UP:
                _heightmapLow = std::min(_heightmapLow + 1, 142 - 1);
                _heightmapHigh = std::max(_heightmapHigh, _heightmapLow + 1);
                InvalidateWidget(WIDX_HEIGHTMAP_LOW);
                break;
            case WIDX_HEIGHTMAP_LOW_DOWN:
                _heightmapLow = std::max(_heightmapLow - 1, 2);
                InvalidateWidget(WIDX_HEIGHTMAP_LOW);
                break;
            case WIDX_HEIGHTMAP_HIGH_UP:
                _heightmapHigh = std::min(_heightmapHigh + 1, 142);
                InvalidateWidget(WIDX_HEIGHTMAP_HIGH);
                break;
            case WIDX_HEIGHTMAP_HIGH_DOWN:
                _heightmapHigh = std::max(_heightmapHigh - 1, 2 + 1);
                _heightmapLow = std::min(_heightmapLow, _heightmapHigh - 1);
                InvalidateWidget(WIDX_HEIGHTMAP_HIGH);
                break;
            case WIDX_HEIGHTMAP_WATER_LEVEL_UP:
                _waterLevel = std::min(_waterLevel + MINIMUM_WATER_HEIGHT, MINIMUM_WATER_HEIGHT + MAXIMUM_WATER_HEIGHT);
                InvalidateWidget(WIDX_HEIGHTMAP_WATER_LEVEL);
                break;
            case WIDX_HEIGHTMAP_WATER_LEVEL_DOWN:
                _waterLevel = std::max(_waterLevel - MINIMUM_WATER_HEIGHT, 0);
                InvalidateWidget(WIDX_HEIGHTMAP_WATER_LEVEL);
                break;
        }
    }

    void HeightmapGenerateMap()
    {
        MapGenSettings mapgenSettings;
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

    void HeightmapMouseUp(WidgetIndex widgetIndex)
    {
        SharedMouseUp(widgetIndex);

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
                intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<void*>(HeightmapLoadsaveCallback));
                ContextOpenIntent(&intent);
                return;
            }
            case WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP:
                _heightmapSmoothMap = !_heightmapSmoothMap;
                SetCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _heightmapSmoothMap);
                SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH, _heightmapSmoothMap);
                SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_UP, _heightmapSmoothMap);
                SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_DOWN, _heightmapSmoothMap);
                InvalidateWidget(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP);
                InvalidateWidget(WIDX_HEIGHTMAP_STRENGTH);
                break;
            case WIDX_HEIGHTMAP_NORMALIZE:
                _heightmapNormalize = !_heightmapNormalize;
                SetCheckboxValue(WIDX_HEIGHTMAP_NORMALIZE, _heightmapNormalize);
                InvalidateWidget(WIDX_HEIGHTMAP_NORMALIZE);
                break;
            case WIDX_HEIGHTMAP_SMOOTH_TILES:
                _heightmapSmoothTiles = !_heightmapSmoothTiles;
                SetCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_TILES, _heightmapSmoothTiles);
                InvalidateWidget(WIDX_HEIGHTMAP_SMOOTH_TILES);
                break;
        }

        // Always regenerate the map after one of the page widgets has been changed
        HeightmapGenerateMap();
    }

    void HeightmapPrepareDraw()
    {
        if (widgets != PageWidgets[WINDOW_MAPGEN_PAGE_HEIGHTMAP])
        {
            widgets = PageWidgets[WINDOW_MAPGEN_PAGE_HEIGHTMAP];
            InitScrollWidgets();
        }

        SetCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _heightmapSmoothMap);
        SetCheckboxValue(WIDX_HEIGHTMAP_NORMALIZE, _heightmapNormalize);
        SetCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_TILES, _heightmapSmoothTiles);

        SetPressedTab();
    }

    void HeightmapDraw(DrawPixelInfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        const colour_t enabledColour = colours[1];
        const colour_t disabledColour = enabledColour | COLOUR_FLAG_INSET;

        // Smooth strength label and value
        const colour_t strengthColour = _heightmapSmoothMap ? enabledColour : disabledColour;
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_HEIGHTMAP_STRENGTH].top + 1 }, STR_MAPGEN_SMOOTH_STRENGTH, {},
            { strengthColour });

        auto ft = Formatter();
        ft.Add<uint16_t>(_heightmapSmoothStrength);
        DrawTextBasic(
            dpi,
            windowPos + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_STRENGTH].left + 1, widgets[WIDX_HEIGHTMAP_STRENGTH].top + 1 },
            STR_COMMA16, ft, { strengthColour });

        // Low label and value
        const colour_t labelColour = _heightmapLoaded ? enabledColour : disabledColour;
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_HEIGHTMAP_LOW].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_LOW_, {},
            { labelColour });

        ft = Formatter();
        ft.Add<uint16_t>(_heightmapLow);
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_LOW].left + 1, widgets[WIDX_HEIGHTMAP_LOW].top + 1 },
            STR_COMMA16, ft, { labelColour });

        // High label and value
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_HEIGHTMAP_HIGH].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_HIGH, {},
            { labelColour });

        ft = Formatter();
        ft.Add<uint16_t>(_heightmapHigh);
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_HIGH].left + 1, widgets[WIDX_HEIGHTMAP_HIGH].top + 1 },
            STR_COMMA16, ft, { labelColour });

        // Water level label and value
        DrawTextBasic(
            dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_HEIGHTMAP_WATER_LEVEL].top + 1 }, STR_WATER_LEVEL_LABEL, {},
            { labelColour });

        ft = Formatter();
        ft.Add<uint16_t>(_waterLevel);
        DrawTextBasic(
            dpi,
            windowPos
                + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_WATER_LEVEL].left + 1, widgets[WIDX_HEIGHTMAP_WATER_LEVEL].top + 1 },
            STR_COMMA16, ft, { labelColour });
    }

#pragma endregion

public:
    void OnOpen() override
    {
        number = 0;
        frame_no = 0;

        page = WINDOW_MAPGEN_PAGE_BASE;
        Invalidate();
        widgets = PageWidgets[WINDOW_MAPGEN_PAGE_BASE];
        hold_down_widgets = HoldDownWidgets[WINDOW_MAPGEN_PAGE_BASE];
        pressed_widgets = PressedWidgets[WINDOW_MAPGEN_PAGE_BASE];
        disabled_widgets = PageDisabledWidgets[WINDOW_MAPGEN_PAGE_BASE];
        InitScrollWidgets();

        _heightmapLoaded = false;
    }

    void OnClose() override
    {
        MapGenUnloadHeightmap();
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_MAPGEN_PAGE_BASE:
                return BaseMouseUp(widgetIndex);
            case WINDOW_MAPGEN_PAGE_RANDOM:
                return RandomMouseUp(widgetIndex);
            case WINDOW_MAPGEN_PAGE_SIMPLEX:
                return SimplexMouseUp(widgetIndex);
            case WINDOW_MAPGEN_PAGE_HEIGHTMAP:
                return HeightmapMouseUp(widgetIndex);
        }
    }

    void OnMouseDown(WidgetIndex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_MAPGEN_PAGE_BASE:
                return BaseMouseDown(widgetIndex, &widgets[widgetIndex]);
            case WINDOW_MAPGEN_PAGE_SIMPLEX:
                return SimplexMouseDown(widgetIndex, &widgets[widgetIndex]);
            case WINDOW_MAPGEN_PAGE_HEIGHTMAP:
                return HeightmapMouseDown(widgetIndex, &widgets[widgetIndex]);
        }
    }

    void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
    {
        switch (page)
        {
            case WINDOW_MAPGEN_PAGE_BASE:
                return BaseDropdown(widgetIndex, selectedIndex);
            case WINDOW_MAPGEN_PAGE_SIMPLEX:
                return SimplexDropdown(widgetIndex, selectedIndex);
        }
    }

    void OnUpdate() override
    {
        switch (page)
        {
            case WINDOW_MAPGEN_PAGE_BASE:
                return BaseUpdate();
            case WINDOW_MAPGEN_PAGE_RANDOM:
                return RandomUpdate();
            case WINDOW_MAPGEN_PAGE_SIMPLEX:
                return SimplexUpdate();
        }
    }

    void OnPrepareDraw() override
    {
        switch (page)
        {
            case WINDOW_MAPGEN_PAGE_BASE:
                return BasePrepareDraw();
            case WINDOW_MAPGEN_PAGE_RANDOM:
                return RandomPrepareDraw();
            case WINDOW_MAPGEN_PAGE_SIMPLEX:
                return SimplexPrepareDraw();
            case WINDOW_MAPGEN_PAGE_HEIGHTMAP:
                return HeightmapPrepareDraw();
        }
    }

    void OnDraw(DrawPixelInfo& dpi) override
    {
        switch (page)
        {
            case WINDOW_MAPGEN_PAGE_BASE:
                return BaseDraw(dpi);
            case WINDOW_MAPGEN_PAGE_RANDOM:
                return RandomDraw(dpi);
            case WINDOW_MAPGEN_PAGE_SIMPLEX:
                return SimplexDraw(dpi);
            case WINDOW_MAPGEN_PAGE_HEIGHTMAP:
                return HeightmapDraw(dpi);
        }
    }

    void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
    {
        switch (page)
        {
            case WINDOW_MAPGEN_PAGE_BASE:
                return TextInput(widgetIndex, text);
            case WINDOW_MAPGEN_PAGE_SIMPLEX:
                return TextInput(widgetIndex, text);
        }
    }

    void AfterLoadingHeightMap(int32_t result, const utf8* path)
    {
        if (result == MODAL_RESULT_OK)
        {
            if (!MapGenLoadHeightmap(path))
            {
                // TODO: Display error popup
                return;
            }

            // The window needs to be open while using the map
            _heightmapLoaded = true;
            SetPage(WINDOW_MAPGEN_PAGE_HEIGHTMAP);

            HeightmapGenerateMap();
        }
    }
};

WindowBase* WindowMapgenOpen()
{
    return WindowFocusOrCreate<MapGenWindow>(WindowClass::Mapgen, WW, WH, WF_10 | WF_AUTO_POSITION | WF_CENTRE_SCREEN);
}

static void HeightmapLoadsaveCallback(int32_t result, const utf8* path)
{
    auto* w = static_cast<MapGenWindow*>(WindowMapgenOpen());
    w->AfterLoadingHeightMap(result, path);
}
