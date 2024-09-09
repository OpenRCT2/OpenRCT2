/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/MapGen.h>
#include <openrct2/world/Surface.h>

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WINDOW_MAPGEN_PAGE_BASE,
        WINDOW_MAPGEN_PAGE_RANDOM,
        WINDOW_MAPGEN_PAGE_SIMPLEX,
        WINDOW_MAPGEN_PAGE_HEIGHTMAP,
        WINDOW_MAPGEN_PAGE_TERRAIN,
        WINDOW_MAPGEN_PAGE_WATER,
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
        WIDX_TAB_5,
        WIDX_TAB_6,

        TAB_BEGIN,

        WIDX_MAP_GENERATE = TAB_BEGIN,
        WIDX_MAP_SIZE_Y,
        WIDX_MAP_SIZE_Y_UP,
        WIDX_MAP_SIZE_Y_DOWN,
        WIDX_MAP_SIZE_LINK,
        WIDX_MAP_SIZE_X,
        WIDX_MAP_SIZE_X_UP,
        WIDX_MAP_SIZE_X_DOWN,

        WIDX_RANDOM_GENERATE = TAB_BEGIN,
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

        WIDX_TERRAIN_GENERATE = TAB_BEGIN,
        WIDX_BASE_HEIGHT,
        WIDX_BASE_HEIGHT_UP,
        WIDX_BASE_HEIGHT_DOWN,
        WIDX_RANDOM_TERRAIN,
        WIDX_FLOOR_TEXTURE,
        WIDX_WALL_TEXTURE,

        WIDX_WATER_GENERATE = TAB_BEGIN,
        WIDX_WATER_LEVEL,
        WIDX_WATER_LEVEL_UP,
        WIDX_WATER_LEVEL_DOWN,
    };

#pragma region Widgets

    static constexpr StringId WINDOW_TITLE = STR_MAPGEN_WINDOW_TITLE;
    static constexpr int32_t WW = 250;
    static constexpr int32_t WH = 273;

#define SHARED_WIDGETS                                                                                                         \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), /* WIDX_BACKGROUND, WIDX_TITLE, WIDX_CLOSE */                                           \
        MakeWidget({ 0, 43 }, { WW, 229 }, WindowWidgetType::Resize, WindowColour::Secondary), /* WIDX_PAGE_BACKGROUND */      \
        MakeTab({ 3, 17 }),                                                                    /* WIDX_TAB_1 */                \
        MakeTab({ 34, 17 }),                                                                   /* WIDX_TAB_2 */                \
        MakeTab({ 65, 17 }),                                                                   /* WIDX_TAB_3 */                \
        MakeTab({ 96, 17 }),                                                                   /* WIDX_TAB_4 */                \
        MakeTab({ 127, 17 }),                                                                  /* WIDX_TAB_5 */                \
        MakeTab({ 158, 17 })                                                                   /* WIDX_TAB_6 */

    // clang-format off
    static Widget BaseWidgets[] = {
        SHARED_WIDGETS,
        MakeWidget        ({155, 255}, {90, 14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_MAPGEN_ACTION_GENERATE                                 ),
        MakeSpinnerWidgets({104,  52}, {50, 12}, WindowWidgetType::Spinner, WindowColour::Secondary, STR_COMMA16                                                ), // NB: 3 widgets
        MakeWidget        ({155,  52}, {21, 12}, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_G2_LINK_CHAIN), STR_MAINTAIN_SQUARE_MAP_TOOLTIP),
        MakeSpinnerWidgets({177,  52}, {50, 12}, WindowWidgetType::Spinner, WindowColour::Secondary, STR_POP16_COMMA16                                          ), // NB: 3 widgets
        kWidgetsEnd,
    };

    static Widget RandomWidgets[] = {
        SHARED_WIDGETS,
        MakeWidget({155, 255}, { 90, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAPGEN_ACTION_GENERATE      ),
        MakeWidget({  4,  52}, {195, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAPGEN_OPTION_PLACE_TREES   ),
        kWidgetsEnd,
    };

    static Widget SimplexWidgets[] = {
        SHARED_WIDGETS,
        MakeWidget        ({155, 255}, { 90, 14}, WindowWidgetType::Button,        WindowColour::Secondary, STR_MAPGEN_ACTION_GENERATE), // WIDX_SIMPLEX_GENERATE
        MakeWidget        ({  4,  52}, {195, 12}, WindowWidgetType::LabelCentred,  WindowColour::Secondary, STR_MAPGEN_SIMPLEX_NOISE  ), // WIDX_SIMPLEX_LABEL
        MakeSpinnerWidgets({104,  70}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                            ), // WIDX_SIMPLEX_LOW{,_UP,_DOWN}
        MakeSpinnerWidgets({104,  88}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                            ), // WIDX_SIMPLEX_HIGH{,_UP,_DOWN}
        MakeSpinnerWidgets({104, 106}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                            ), // WIDX_SIMPLEX_BASE_FREQ{,_UP,_DOWN}
        MakeSpinnerWidgets({104, 124}, { 95, 12}, WindowWidgetType::Spinner,       WindowColour::Secondary                            ), // WIDX_SIMPLEX_OCTAVES{,_UP,_DOWN}
        kWidgetsEnd,
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
        kWidgetsEnd,
    };

    static Widget TerrainWidgets[] = {
        SHARED_WIDGETS,
        MakeWidget        ({155, 255}, {90, 14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_MAPGEN_ACTION_GENERATE              ),
        MakeSpinnerWidgets({104,  52}, {95, 12}, WindowWidgetType::Spinner,  WindowColour::Secondary                                          ), // NB: 3 widgets
        MakeWidget        ({104,  70}, {95, 12}, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_MAPGEN_OPTION_RANDOM_TERRAIN        ),
        MakeWidget        ({104,  82}, {47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF, STR_CHANGE_BASE_LAND_TIP    ),
        MakeWidget        ({151,  82}, {47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF, STR_CHANGE_VERTICAL_LAND_TIP),
        kWidgetsEnd,
    };

    static Widget WaterWidgets[] = {
        SHARED_WIDGETS,
        MakeWidget        ({155, 255}, {90, 14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_MAPGEN_ACTION_GENERATE),
        MakeSpinnerWidgets({104,  52}, {95, 12}, WindowWidgetType::Spinner, WindowColour::Secondary                            ), // NB: 3 widgets
        kWidgetsEnd,
    };

    static Widget* PageWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
        BaseWidgets,
        RandomWidgets,
        SimplexWidgets,
        HeightmapWidgets,
        TerrainWidgets,
        WaterWidgets,
    };
    // clang-format on

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
        (1uLL << WIDX_HEIGHTMAP_LOW_DOWN)
    };

    static uint64_t HoldDownWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
        (1uLL << WIDX_MAP_SIZE_Y_UP) |
        (1uLL << WIDX_MAP_SIZE_Y_DOWN) |
        (1uLL << WIDX_MAP_SIZE_X_UP) |
        (1uLL << WIDX_MAP_SIZE_X_DOWN),

        0,

        (1uLL << WIDX_SIMPLEX_LOW_UP) |
        (1uLL << WIDX_SIMPLEX_LOW_DOWN) |
        (1uLL << WIDX_SIMPLEX_HIGH_UP) |
        (1uLL << WIDX_SIMPLEX_HIGH_DOWN) |
        (1uLL << WIDX_SIMPLEX_BASE_FREQ_UP) |
        (1uLL << WIDX_SIMPLEX_BASE_FREQ_DOWN) |
        (1uLL << WIDX_SIMPLEX_OCTAVES_UP) |
        (1uLL << WIDX_SIMPLEX_OCTAVES_DOWN),

        (1uLL << WIDX_HEIGHTMAP_STRENGTH_UP) |
        (1uLL << WIDX_HEIGHTMAP_STRENGTH_DOWN) |
        (1uLL << WIDX_HEIGHTMAP_LOW_UP) |
        (1uLL << WIDX_HEIGHTMAP_LOW_DOWN) |
        (1uLL << WIDX_HEIGHTMAP_HIGH_UP) |
        (1uLL << WIDX_HEIGHTMAP_HIGH_DOWN),

        (1uLL << WIDX_BASE_HEIGHT_UP) |
        (1uLL << WIDX_BASE_HEIGHT_DOWN),

        (1uLL << WIDX_WATER_LEVEL_UP) |
        (1uLL << WIDX_WATER_LEVEL_DOWN),
    };

    static uint64_t PressedWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
        0,
        0,
        0,
        (1uLL << WIDX_HEIGHTMAP_SMOOTH_TILES),
        0,
        0,
    };
    // clang-format on

#pragma endregion

    // clang-format off
    static constexpr int32_t TabAnimationDivisor[WINDOW_MAPGEN_PAGE_COUNT] = {
        2,
        1,
        1,
        1,
        1,
        1,
    };
    static constexpr int32_t TabAnimationFrames[WINDOW_MAPGEN_PAGE_COUNT] = {
        4,
        1,
        1,
        1,
        1,
        1,
    };
    static constexpr int32_t TabAnimationLoops[WINDOW_MAPGEN_PAGE_COUNT] = {
        16,
        16,
        16,
        0,
        1,
        1,
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
        ResizeDirection _resizeDirection{ ResizeDirection::Both };
        bool _mapWidthAndHeightLinked{ true };

        MapGenSettings _settings{
            // Base
            .mapSize{ 150, 150 },
            .height = 12,
            .water_level = 6,
            .floor = 0,
            .wall = 0,

            // Features (e.g. tree, rivers, lakes etc.)
            .trees = 1,

            // Simplex Noise Parameters
            .simplex_low = 6,
            .simplex_high = 10,
            .simplex_base_freq = 60,
            .simplex_octaves = 4,

            // Height map _settings
            .smooth = true,
            .smooth_height_map = false,
            .smooth_strength = 1,
            .normalize_height = false,
        };

        bool _randomTerrain = true;
        bool _heightmapLoaded = false;
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
                SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH, _settings.smooth_height_map);
                SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_UP, _settings.smooth_height_map);
                SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_DOWN, _settings.smooth_height_map);
                SetWidgetEnabled(WIDX_HEIGHTMAP_NORMALIZE, true);
                SetWidgetEnabled(WIDX_HEIGHTMAP_SMOOTH_TILES, true);
                SetWidgetEnabled(WIDX_HEIGHTMAP_HIGH, true);
                SetWidgetEnabled(WIDX_HEIGHTMAP_HIGH_UP, true);
                SetWidgetEnabled(WIDX_HEIGHTMAP_HIGH_DOWN, true);
                SetWidgetEnabled(WIDX_HEIGHTMAP_LOW, true);
                SetWidgetEnabled(WIDX_HEIGHTMAP_LOW_UP, true);
                SetWidgetEnabled(WIDX_HEIGHTMAP_LOW_DOWN, true);
            }

            InitScrollWidgets();
            Invalidate();
        }

        void SetPressedTab()
        {
            for (auto i = 0; i < WINDOW_MAPGEN_PAGE_COUNT; i++)
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
                    dpi, ImageId(spriteIndex),
                    windowPos + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
            }
        }

        void DrawTabImages(DrawPixelInfo& dpi)
        {
            DrawTabImage(dpi, WINDOW_MAPGEN_PAGE_BASE, SPR_TAB_GEARS_0);
            DrawTabImage(dpi, WINDOW_MAPGEN_PAGE_RANDOM, SPR_G2_TAB_TREE);
            DrawTabImage(dpi, WINDOW_MAPGEN_PAGE_SIMPLEX, SPR_G2_TAB_PENCIL);
            DrawTabImage(dpi, WINDOW_MAPGEN_PAGE_HEIGHTMAP, SPR_TAB_GRAPH_0);
            DrawTabImage(dpi, WINDOW_MAPGEN_PAGE_TERRAIN, SPR_G2_TAB_LAND);
            DrawTabImage(dpi, WINDOW_MAPGEN_PAGE_WATER, SPR_TAB_WATER);
        }

        void ChangeMapSize(int32_t sizeOffset)
        {
            if (_mapWidthAndHeightLinked)
                _resizeDirection = ResizeDirection::Both;

            if (_resizeDirection != ResizeDirection::X)
            {
                _settings.mapSize.y = std::clamp<int32_t>(
                    _settings.mapSize.y + sizeOffset, kMinimumMapSizeTechnical, kMaximumMapSizeTechnical);
            }

            if (_resizeDirection != ResizeDirection::Y)
            {
                _settings.mapSize.x = std::clamp<int32_t>(
                    _settings.mapSize.x + sizeOffset, kMinimumMapSizeTechnical, kMaximumMapSizeTechnical);
            }
        }

        void InputMapSize(WidgetIndex callingWidget, int32_t currentValue)
        {
            Formatter ft;
            ft.Add<int16_t>(kMinimumMapSizePractical);
            ft.Add<int16_t>(kMaximumMapSizePractical);

            // Practical map size is 2 lower than the technical map size
            currentValue -= 2;
            WindowTextInputOpen(
                this, callingWidget, STR_MAP_SIZE_2, STR_ENTER_MAP_SIZE, ft, STR_FORMAT_INTEGER, currentValue, 4);
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
                case WIDX_TAB_5:
                case WIDX_TAB_6:
                    SetPage(widgetIndex - WIDX_TAB_1);
                    break;
            }
        }

#pragma region Base page

        void BaseMouseUp(WidgetIndex widgetIndex)
        {
            SharedMouseUp(widgetIndex);

            switch (widgetIndex)
            {
                case WIDX_MAP_GENERATE:
                {
                    MapGenSettings mapgenSettings = _settings;
                    mapgenSettings.height += 2;
                    mapgenSettings.water_level += 2;

                    MapGenGenerateBlank(&mapgenSettings);
                    GfxInvalidateScreen();
                    break;
                }
                case WIDX_MAP_SIZE_Y:
                    _resizeDirection = ResizeDirection::Y;
                    InputMapSize(WIDX_MAP_SIZE_Y, _settings.mapSize.y);
                    break;
                case WIDX_MAP_SIZE_X:
                    _resizeDirection = ResizeDirection::X;
                    InputMapSize(WIDX_MAP_SIZE_X, _settings.mapSize.x);
                    break;
                case WIDX_MAP_SIZE_LINK:
                    _mapWidthAndHeightLinked = !_mapWidthAndHeightLinked;
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
            }
        }

        void BaseUpdate()
        {
            // Tab animation
            if (++frame_no >= TabAnimationLoops[page])
                frame_no = 0;
            InvalidateWidget(WIDX_TAB_1);
        }

        void BaseTextInput(WidgetIndex widgetIndex, std::string_view text)
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
                    // The practical size is 2 lower than the technical size
                    value += 2;
                    if (_resizeDirection == ResizeDirection::Y || _mapWidthAndHeightLinked)
                        _settings.mapSize.y = value;
                    if (_resizeDirection == ResizeDirection::X || _mapWidthAndHeightLinked)
                        _settings.mapSize.x = value;
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
            SetWidgetDisabled(WIDX_MAP_SIZE_LINK, _settings.mapSize.x != _settings.mapSize.y);

            SetPressedTab();

            // Push width (Y) and height (X) to the common formatter arguments for the map size spinners to use
            auto ft = Formatter::Common();
            ft.Add<uint16_t>(_settings.mapSize.y - 2);
            ft.Add<uint16_t>(_settings.mapSize.x - 2);
        }

        void BaseDraw(DrawPixelInfo& dpi)
        {
            DrawWidgets(dpi);
            DrawTabImages(dpi);

            const auto textColour = colours[1];

            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_MAP_SIZE_Y].top + 1 }, STR_MAP_SIZE, {}, { textColour });
        }

#pragma endregion

#pragma region Random page

        void RandomMouseUp(WidgetIndex widgetIndex)
        {
            SharedMouseUp(widgetIndex);

            switch (widgetIndex)
            {
                case WIDX_RANDOM_GENERATE:
                {
                    MapGenSettings mapgenSettings = _settings;
                    mapgenSettings.height += 2;
                    mapgenSettings.water_level += 2;
                    if (_randomTerrain)
                    {
                        mapgenSettings.floor = -1;
                        mapgenSettings.wall = -1;
                    }

                    mapgenSettings.simplex_low = UtilRand() % 4;
                    mapgenSettings.simplex_high = 12 + (UtilRand() % (32 - 12));
                    mapgenSettings.simplex_base_freq = 1.75f;
                    mapgenSettings.simplex_octaves = 6;

                    MapGenGenerate(&mapgenSettings);
                    GfxInvalidateScreen();
                    break;
                }
                case WIDX_RANDOM_PLACE_TREES:
                    _settings.trees ^= 1;
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
            if (_settings.trees)
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

            switch (widgetIndex)
            {
                case WIDX_SIMPLEX_GENERATE:
                {
                    MapGenSettings mapgenSettings = _settings;
                    mapgenSettings.water_level += kMinimumWaterHeight;
                    mapgenSettings.simplex_base_freq /= 100.00f;
                    if (_randomTerrain)
                    {
                        mapgenSettings.floor = -1;
                        mapgenSettings.wall = -1;
                    }

                    MapGenGenerate(&mapgenSettings);
                    GfxInvalidateScreen();
                    break;
                }
            }
        }

        void SimplexMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_SIMPLEX_LOW_UP:
                    _settings.simplex_low = std::min(_settings.simplex_low + 1, 24);
                    Invalidate();
                    break;
                case WIDX_SIMPLEX_LOW_DOWN:
                    _settings.simplex_low = std::max(_settings.simplex_low - 1, 0);
                    Invalidate();
                    break;
                case WIDX_SIMPLEX_HIGH_UP:
                    _settings.simplex_high = std::min(_settings.simplex_high + 1, 36);
                    Invalidate();
                    break;
                case WIDX_SIMPLEX_HIGH_DOWN:
                    _settings.simplex_high = std::max(_settings.simplex_high - 1, 0);
                    Invalidate();
                    break;
                case WIDX_SIMPLEX_BASE_FREQ_UP:
                    _settings.simplex_base_freq = std::min<int32_t>(_settings.simplex_base_freq + 5, 1000);
                    Invalidate();
                    break;
                case WIDX_SIMPLEX_BASE_FREQ_DOWN:
                    _settings.simplex_base_freq = std::max<int32_t>(_settings.simplex_base_freq - 5, 0);
                    Invalidate();
                    break;
                case WIDX_SIMPLEX_OCTAVES_UP:
                    _settings.simplex_octaves = std::min(_settings.simplex_octaves + 1, 10);
                    Invalidate();
                    break;
                case WIDX_SIMPLEX_OCTAVES_DOWN:
                    _settings.simplex_octaves = std::max(_settings.simplex_octaves - 1, 1);
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

            SetPressedTab();
        }

        void SimplexDraw(DrawPixelInfo& dpi)
        {
            DrawWidgets(dpi);
            DrawTabImages(dpi);

            const auto textColour = colours[1];

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
                dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_SIMPLEX_OCTAVES].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_OCTAVES,
                {}, { textColour });

            auto ft = Formatter();
            ft.Add<uint16_t>(_settings.simplex_low);
            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_LOW].left + 1, widgets[WIDX_SIMPLEX_LOW].top + 1 },
                STR_COMMA16, ft, { textColour });
            ft = Formatter();
            ft.Add<uint16_t>(_settings.simplex_high);
            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_HIGH].left + 1, widgets[WIDX_SIMPLEX_HIGH].top + 1 },
                STR_COMMA16, ft, { textColour });
            ft = Formatter();
            ft.Add<uint16_t>(_settings.simplex_base_freq);
            DrawTextBasic(
                dpi,
                windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_BASE_FREQ].left + 1, widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1 },
                STR_WINDOW_COLOUR_2_COMMA2DP32, ft, { textColour });
            ft = Formatter();
            ft.Add<uint16_t>(_settings.simplex_octaves);
            DrawTextBasic(
                dpi,
                windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_OCTAVES].left + 1, widgets[WIDX_SIMPLEX_OCTAVES].top + 1 },
                STR_COMMA16, ft, { textColour });
        }

#pragma endregion

#pragma region Heightmap page

        void HeightmapMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_HEIGHTMAP_STRENGTH_UP:
                    _settings.smooth_strength = std::min<uint32_t>(_settings.smooth_strength + 1, MAX_SMOOTH_ITERATIONS);
                    InvalidateWidget(WIDX_HEIGHTMAP_STRENGTH);
                    break;
                case WIDX_HEIGHTMAP_STRENGTH_DOWN:
                    _settings.smooth_strength = std::max<uint32_t>(_settings.smooth_strength - 1, 1);
                    InvalidateWidget(WIDX_HEIGHTMAP_STRENGTH);
                    break;
                case WIDX_HEIGHTMAP_LOW_UP:
                    _heightmapLow = std::min(_heightmapLow + 1, kMaximumWaterHeight - 1);
                    _heightmapHigh = std::max(_heightmapHigh, _heightmapLow + 1);
                    InvalidateWidget(WIDX_HEIGHTMAP_LOW);
                    break;
                case WIDX_HEIGHTMAP_LOW_DOWN:
                    _heightmapLow = std::max(_heightmapLow - 1, 2);
                    InvalidateWidget(WIDX_HEIGHTMAP_LOW);
                    break;
                case WIDX_HEIGHTMAP_HIGH_UP:
                    _heightmapHigh = std::min<int32_t>(_heightmapHigh + 1, kMaximumWaterHeight);
                    InvalidateWidget(WIDX_HEIGHTMAP_HIGH);
                    break;
                case WIDX_HEIGHTMAP_HIGH_DOWN:
                    _heightmapHigh = std::max(_heightmapHigh - 1, 2 + 1);
                    _heightmapLow = std::min(_heightmapLow, _heightmapHigh - 1);
                    InvalidateWidget(WIDX_HEIGHTMAP_HIGH);
                    break;
            }
        }

        void HeightmapGenerateMap()
        {
            MapGenSettings mapgenSettings = _settings;
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
                case WIDX_TAB_5:
                case WIDX_TAB_6:
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
                    _settings.smooth_height_map = !_settings.smooth_height_map;
                    SetCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _settings.smooth_height_map);
                    SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH, _settings.smooth_height_map);
                    SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_UP, _settings.smooth_height_map);
                    SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_DOWN, _settings.smooth_height_map);
                    InvalidateWidget(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP);
                    InvalidateWidget(WIDX_HEIGHTMAP_STRENGTH);
                    break;
                case WIDX_HEIGHTMAP_NORMALIZE:
                    _settings.normalize_height = !_settings.normalize_height;
                    SetCheckboxValue(WIDX_HEIGHTMAP_NORMALIZE, _settings.normalize_height);
                    InvalidateWidget(WIDX_HEIGHTMAP_NORMALIZE);
                    break;
                case WIDX_HEIGHTMAP_SMOOTH_TILES:
                    _settings.smooth = !_settings.smooth;
                    SetCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_TILES, _settings.smooth);
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

            SetCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _settings.smooth_height_map);
            SetCheckboxValue(WIDX_HEIGHTMAP_NORMALIZE, _settings.normalize_height);
            SetCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_TILES, _settings.smooth);

            SetPressedTab();
        }

        void HeightmapDraw(DrawPixelInfo& dpi)
        {
            DrawWidgets(dpi);
            DrawTabImages(dpi);

            const auto enabledColour = colours[1];
            const auto disabledColour = enabledColour.withFlag(ColourFlag::inset, true);

            // Smooth strength label and value
            const auto strengthColour = _settings.smooth_height_map ? enabledColour : disabledColour;
            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ 5, widgets[WIDX_HEIGHTMAP_STRENGTH].top + 1 }, STR_MAPGEN_SMOOTH_STRENGTH, {},
                { strengthColour });

            auto ft = Formatter();
            ft.Add<uint16_t>(_settings.smooth_strength);
            DrawTextBasic(
                dpi,
                windowPos
                    + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_STRENGTH].left + 1, widgets[WIDX_HEIGHTMAP_STRENGTH].top + 1 },
                STR_COMMA16, ft, { strengthColour });

            // Low label and value
            const auto labelColour = _heightmapLoaded ? enabledColour : disabledColour;
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
        }

#pragma endregion

#pragma region Terrain page

        void TerrainMouseUp(WidgetIndex widgetIndex)
        {
            SharedMouseUp(widgetIndex);

            switch (widgetIndex)
            {
                case WIDX_MAP_GENERATE:
                {
                    MapGenSettings mapgenSettings = _settings;
                    mapgenSettings.height += 2;
                    mapgenSettings.water_level += 2;

                    MapGenGenerateBlank(&mapgenSettings);
                    GfxInvalidateScreen();
                    break;
                }

                case WIDX_BASE_HEIGHT:
                {
                    Formatter ft;
                    ft.Add<int16_t>((BASESIZE_MIN - 12) / 2);
                    ft.Add<int16_t>((BASESIZE_MAX - 12) / 2);
                    WindowTextInputOpen(
                        this, WIDX_BASE_HEIGHT, STR_BASE_HEIGHT, STR_ENTER_BASE_HEIGHT, ft, STR_FORMAT_INTEGER,
                        (_settings.height - 12) / 2, 3);
                    break;
                }
            }
        }

        void TerrainMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_BASE_HEIGHT_UP:
                    _settings.height = std::min(_settings.height + 2, BASESIZE_MAX);
                    Invalidate();
                    break;
                case WIDX_BASE_HEIGHT_DOWN:
                    _settings.height = std::max(_settings.height - 2, BASESIZE_MIN);
                    Invalidate();
                    break;
                case WIDX_RANDOM_TERRAIN:
                    _randomTerrain = !_randomTerrain;
                    Invalidate();
                    break;
                case WIDX_FLOOR_TEXTURE:
                    LandTool::ShowSurfaceStyleDropdown(this, widget, _settings.floor);
                    break;
                case WIDX_WALL_TEXTURE:
                    LandTool::ShowEdgeStyleDropdown(this, widget, _settings.wall);
                    break;
            }
        }

        void TerrainUpdate()
        {
            // Tab animation
            if (++frame_no >= TabAnimationLoops[page])
                frame_no = 0;
            InvalidateWidget(WIDX_TAB_5);
        }

        void TerrainTextInput(WidgetIndex widgetIndex, std::string_view text)
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
                case WIDX_BASE_HEIGHT:
                    _settings.height = std::clamp((value * 2) + 12, BASESIZE_MIN, BASESIZE_MAX);
                    break;
            }

            Invalidate();
        }

        void TerrainDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            int32_t type;

            switch (widgetIndex)
            {
                case WIDX_FLOOR_TEXTURE:
                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdownHighlightedIndex;

                    type = (dropdownIndex == -1) ? _settings.floor : dropdownIndex;

                    if (gLandToolTerrainSurface == type)
                    {
                        gLandToolTerrainSurface = OBJECT_ENTRY_INDEX_NULL;
                    }
                    else
                    {
                        gLandToolTerrainSurface = type;
                        _settings.floor = type;
                    }
                    Invalidate();
                    break;
                case WIDX_WALL_TEXTURE:
                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdownHighlightedIndex;

                    type = (dropdownIndex == -1) ? _settings.wall : dropdownIndex;

                    if (gLandToolTerrainEdge == type)
                    {
                        gLandToolTerrainEdge = OBJECT_ENTRY_INDEX_NULL;
                    }
                    else
                    {
                        gLandToolTerrainEdge = type;
                        _settings.wall = type;
                    }
                    Invalidate();
                    break;
            }
        }

        void DrawDropdownButton(DrawPixelInfo& dpi, WidgetIndex widgetIndex, ImageId image)
        {
            const auto& widget = widgets[widgetIndex];
            ScreenCoordsXY pos = { windowPos.x + widget.left, windowPos.y + widget.top };
            if (IsWidgetDisabled(widgetIndex))
            {
                // Draw greyed out (light border bottom right shadow)
                auto colour = colours[widget.colour].colour;
                colour = ColourMapA[colour].lighter;
                GfxDrawSpriteSolid(dpi, image, pos + ScreenCoordsXY{ 1, 1 }, colour);

                // Draw greyed out (dark)
                colour = colours[widget.colour].colour;
                colour = ColourMapA[colour].mid_light;
                GfxDrawSpriteSolid(dpi, image, pos, colour);
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
                objManager.GetLoadedObject(ObjectType::TerrainSurface, _settings.floor));
            ImageId surfaceImage;
            if (surfaceObj != nullptr)
            {
                surfaceImage = ImageId(surfaceObj->IconImageId);
                if (surfaceObj->Colour != TerrainSurfaceObject::kNoValue)
                {
                    surfaceImage = surfaceImage.WithPrimary(surfaceObj->Colour);
                }
            }

            ImageId edgeImage;
            const auto edgeObj = static_cast<TerrainEdgeObject*>(
                objManager.GetLoadedObject(ObjectType::TerrainEdge, _settings.wall));
            if (edgeObj != nullptr)
            {
                edgeImage = ImageId(edgeObj->IconImageId);
            }

            DrawDropdownButton(dpi, floorWidgetIndex, surfaceImage);
            DrawDropdownButton(dpi, edgeWidgetIndex, edgeImage);
        }

        void TerrainPrepareDraw()
        {
            if (widgets != PageWidgets[WINDOW_MAPGEN_PAGE_TERRAIN])
            {
                widgets = PageWidgets[WINDOW_MAPGEN_PAGE_TERRAIN];
                InitScrollWidgets();
            }

            SetCheckboxValue(WIDX_RANDOM_TERRAIN, _randomTerrain != 0);

            // Only allow floor and wall texture options if random terrain is disabled
            if (!_randomTerrain)
            {
                SetWidgetEnabled(WIDX_FLOOR_TEXTURE, true);
                SetWidgetEnabled(WIDX_WALL_TEXTURE, true);
            }
            else
            {
                SetWidgetEnabled(WIDX_FLOOR_TEXTURE, false);
                SetWidgetEnabled(WIDX_WALL_TEXTURE, false);
            }

            SetPressedTab();
        }

        void TerrainDraw(DrawPixelInfo& dpi)
        {
            DrawWidgets(dpi);
            DrawTabImages(dpi);
            DrawDropdownButtons(dpi, WIDX_FLOOR_TEXTURE, WIDX_WALL_TEXTURE);

            const auto textColour = colours[1];

            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_BASE_HEIGHT].top + 1 }, STR_BASE_HEIGHT_LABEL, {},
                { textColour });
            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_FLOOR_TEXTURE].top + 1 }, STR_TERRAIN_LABEL, {},
                { textColour });

            auto ft = Formatter();
            ft.Add<uint16_t>((_settings.height - 12) / 2);
            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_BASE_HEIGHT].left + 1, widgets[WIDX_BASE_HEIGHT].top + 1 },
                STR_COMMA16, ft, { colours[1] });
        }

#pragma endregion

#pragma region Water page

        void WaterMouseUp(WidgetIndex widgetIndex)
        {
            SharedMouseUp(widgetIndex);

            switch (widgetIndex)
            {
                case WIDX_MAP_GENERATE:
                {
                    MapGenSettings mapgenSettings = _settings;
                    mapgenSettings.height += 2;
                    mapgenSettings.water_level += 2;

                    MapGenGenerateBlank(&mapgenSettings);
                    GfxInvalidateScreen();
                    break;
                }

                case WIDX_WATER_LEVEL:
                {
                    Formatter ft;
                    ft.Add<int16_t>((WATERLEVEL_MIN - 12) / 2);
                    ft.Add<int16_t>((WATERLEVEL_MAX - 12) / 2);
                    WindowTextInputOpen(
                        this, WIDX_WATER_LEVEL, STR_WATER_LEVEL, STR_ENTER_WATER_LEVEL, ft, STR_FORMAT_INTEGER,
                        (_settings.water_level - 12) / 2, 3);
                    break;
                }
            }
        }

        void WaterMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_WATER_LEVEL_UP:
                    _settings.water_level = std::min(_settings.water_level + 2, WATERLEVEL_MAX);
                    Invalidate();
                    break;
                case WIDX_WATER_LEVEL_DOWN:
                    _settings.water_level = std::max(_settings.water_level - 2, WATERLEVEL_MIN);
                    Invalidate();
                    break;
            }
        }

        void WaterUpdate()
        {
            // Tab animation
            if (++frame_no >= TabAnimationLoops[page])
                frame_no = 0;
            InvalidateWidget(WIDX_TAB_6);
        }

        void WaterTextInput(WidgetIndex widgetIndex, std::string_view text)
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
                case WIDX_WATER_LEVEL:
                    _settings.water_level = std::clamp((value * 2) + 12, WATERLEVEL_MIN, WATERLEVEL_MAX);
                    break;
            }

            Invalidate();
        }

        void WaterPrepareDraw()
        {
            if (widgets != PageWidgets[WINDOW_MAPGEN_PAGE_WATER])
            {
                widgets = PageWidgets[WINDOW_MAPGEN_PAGE_WATER];
                InitScrollWidgets();
            }

            SetPressedTab();
        }

        void WaterDraw(DrawPixelInfo& dpi)
        {
            DrawWidgets(dpi);
            DrawTabImages(dpi);

            const auto textColour = colours[1];

            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_WATER_LEVEL].top + 1 }, STR_WATER_LEVEL_LABEL, {},
                { textColour });

            auto ft = Formatter();
            ft.Add<uint16_t>((_settings.water_level - 12) / 2);
            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ widgets[WIDX_WATER_LEVEL].left + 1, widgets[WIDX_WATER_LEVEL].top + 1 },
                STR_COMMA16, ft, { colours[1] });
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
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainMouseUp(widgetIndex);
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterMouseUp(widgetIndex);
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
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainMouseDown(widgetIndex, &widgets[widgetIndex]);
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterMouseDown(widgetIndex, &widgets[widgetIndex]);
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainDropdown(widgetIndex, selectedIndex);
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
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainUpdate();
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterUpdate();
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
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainPrepareDraw();
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterPrepareDraw();
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
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainDraw(dpi);
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterDraw(dpi);
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BaseTextInput(widgetIndex, text);
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainTextInput(widgetIndex, text);
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterTextInput(widgetIndex, text);
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

        void OnResize() override
        {
            ResizeFrameWithPage();
        }
    };

    WindowBase* MapgenOpen()
    {
        return WindowFocusOrCreate<MapGenWindow>(WindowClass::Mapgen, WW, WH, WF_10 | WF_AUTO_POSITION | WF_CENTRE_SCREEN);
    }

    static void HeightmapLoadsaveCallback(int32_t result, const utf8* path)
    {
        auto* w = static_cast<MapGenWindow*>(MapgenOpen());
        w->AfterLoadingHeightMap(result, path);
    }
} // namespace OpenRCT2::Ui::Windows
