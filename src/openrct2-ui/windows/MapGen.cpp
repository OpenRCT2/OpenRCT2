/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/FileSystem.hpp>
#include <openrct2/core/UnitConversion.h>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/map_generator/MapGen.h>
#include <openrct2/world/map_generator/PngTerrainGenerator.h>

using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::World;

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WINDOW_MAPGEN_PAGE_BASE,
        WINDOW_MAPGEN_PAGE_TERRAIN,
        WINDOW_MAPGEN_PAGE_WATER,
        WINDOW_MAPGEN_PAGE_FORESTS,
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
        WIDX_MAP_GENERATE,

        TAB_BEGIN,

        WIDX_MAP_SIZE_Y = TAB_BEGIN,
        WIDX_MAP_SIZE_Y_UP,
        WIDX_MAP_SIZE_Y_DOWN,
        WIDX_MAP_SIZE_LINK,
        WIDX_MAP_SIZE_X,
        WIDX_MAP_SIZE_X_UP,
        WIDX_MAP_SIZE_X_DOWN,
        WIDX_HEIGHTMAP_SOURCE,
        WIDX_HEIGHTMAP_SOURCE_DROPDOWN,

        WIDX_SIMPLEX_GROUP,
        WIDX_SIMPLEX_BASE_FREQ,
        WIDX_SIMPLEX_BASE_FREQ_UP,
        WIDX_SIMPLEX_BASE_FREQ_DOWN,
        WIDX_SIMPLEX_OCTAVES,
        WIDX_SIMPLEX_OCTAVES_UP,
        WIDX_SIMPLEX_OCTAVES_DOWN,

        WIDX_HEIGHTMAP_GROUP,
        WIDX_HEIGHTMAP_BROWSE,
        WIDX_HEIGHTMAP_NORMALIZE,
        WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP,
        WIDX_HEIGHTMAP_STRENGTH,
        WIDX_HEIGHTMAP_STRENGTH_UP,
        WIDX_HEIGHTMAP_STRENGTH_DOWN,

        WIDX_HEIGHTMAP_LOW = TAB_BEGIN,
        WIDX_HEIGHTMAP_LOW_UP,
        WIDX_HEIGHTMAP_LOW_DOWN,
        WIDX_HEIGHTMAP_HIGH,
        WIDX_HEIGHTMAP_HIGH_UP,
        WIDX_HEIGHTMAP_HIGH_DOWN,
        WIDX_FLOOR_TEXTURE,
        WIDX_WALL_TEXTURE,
        WIDX_RANDOM_TERRAIN,
        WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES,

        WIDX_WATER_LEVEL = TAB_BEGIN,
        WIDX_WATER_LEVEL_UP,
        WIDX_WATER_LEVEL_DOWN,
        WIDX_ADD_BEACHES,

        WIDX_FORESTS_PLACE_TREES = TAB_BEGIN,
        WIDX_TREE_LAND_RATIO,
        WIDX_TREE_LAND_RATIO_UP,
        WIDX_TREE_LAND_RATIO_DOWN,
        WIDX_TREE_ALTITUDE_MIN,
        WIDX_TREE_ALTITUDE_MIN_UP,
        WIDX_TREE_ALTITUDE_MIN_DOWN,
        WIDX_TREE_ALTITUDE_MAX,
        WIDX_TREE_ALTITUDE_MAX_UP,
        WIDX_TREE_ALTITUDE_MAX_DOWN,
    };

#pragma region Widgets

    static constexpr ScreenSize kWindowSize = { 300, 220 };

    // clang-format off
    static constexpr auto makeMapGenWidgets = [](StringId title) {
        return makeWidgets(
            makeWindowShim(title, kWindowSize),
            makeWidget({   0, 43 }, { kWindowSize.width, 177 }, WidgetType::resize, WindowColour::secondary),
            makeTab   ({   3, 17 }),
            makeTab   ({  34, 17 }),
            makeTab   ({  65, 17 }),
            makeTab   ({  96, 17 }),
            makeWidget({ 185, 200 }, { 109, 14 }, WidgetType::button, WindowColour::secondary, STR_MAPGEN_ACTION_GENERATE)
        );
    };

    static constexpr auto BaseWidgets = makeWidgets(
        makeMapGenWidgets(STR_MAPGEN_CAPTION_GENERATOR),
        makeSpinnerWidgets ({165, 52}, { 50, 12}, WidgetType::spinner,      WindowColour::secondary, STR_COMMA16                                                ), // NB: 3 widgets
        makeWidget         ({216, 52}, { 21, 12}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_G2_LINK_CHAIN), STR_MAINTAIN_SQUARE_MAP_TOOLTIP),
        makeSpinnerWidgets ({238, 52}, { 50, 12}, WidgetType::spinner,      WindowColour::secondary, STR_POP16_COMMA16                                          ), // NB: 3 widgets
        makeDropdownWidgets({155, 70}, {133, 14}, WidgetType::dropdownMenu, WindowColour::secondary, STR_HEIGHTMAP_FLATLAND                                     ),

        makeWidget        ({  5,  90}, {290, 55}, WidgetType::groupbox, WindowColour::secondary, STR_MAPGEN_SIMPLEX_NOISE), // WIDX_SIMPLEX_GROUP
        makeSpinnerWidgets({179, 107}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                          ), // WIDX_SIMPLEX_BASE_FREQ{,_UP,_DOWN}
        makeSpinnerWidgets({179, 125}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                          ), // WIDX_SIMPLEX_OCTAVES{,_UP,_DOWN}

        makeWidget        ({  5,  90}, {290, 86}, WidgetType::groupbox, WindowColour::secondary, STR_MAPGEN_SELECT_HEIGHTMAP), // WIDX_HEIGHTMAP_GROUP
        makeWidget        ({223, 107}, { 65, 14}, WidgetType::button,   WindowColour::secondary, STR_BROWSE                 ), // WIDX_HEIGHTMAP_BROWSE
        makeWidget        ({ 10, 125}, {150, 12}, WidgetType::checkbox, WindowColour::secondary, STR_MAPGEN_NORMALIZE       ), // WIDX_HEIGHTMAP_NORMALIZE
        makeWidget        ({ 10, 141}, {150, 12}, WidgetType::checkbox, WindowColour::secondary, STR_MAPGEN_SMOOTH_HEIGHTMAP), // WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP
        makeSpinnerWidgets({179, 157}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                             )  // WIDX_HEIGHTMAP_STRENGTH{,_UP,_DOWN}
    );

    static constexpr auto TerrainWidgets = makeWidgets(
        makeMapGenWidgets(STR_MAPGEN_CAPTION_TERRAIN),
        makeSpinnerWidgets({179,  52}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                                          ), // WIDX_HEIGHTMAP_LOW{,_UP,_DOWN}
        makeSpinnerWidgets({179,  70}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                                          ), // WIDX_HEIGHTMAP_HIGH{,_UP,_DOWN}
        makeWidget        ({179,  88}, { 47, 36}, WidgetType::flatBtn,  WindowColour::secondary, 0xFFFFFFFF, STR_CHANGE_BASE_LAND_TIP    ),
        makeWidget        ({236,  88}, { 47, 36}, WidgetType::flatBtn,  WindowColour::secondary, 0xFFFFFFFF, STR_CHANGE_VERTICAL_LAND_TIP),
        makeWidget        ({ 10, 106}, {150, 12}, WidgetType::checkbox, WindowColour::secondary, STR_MAPGEN_OPTION_RANDOM_TERRAIN        ),
        makeWidget        ({ 10, 122}, {150, 12}, WidgetType::checkbox, WindowColour::secondary, STR_MAPGEN_SMOOTH_TILE                  )  // WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES
    );

    static constexpr auto WaterWidgets = makeWidgets(
        makeMapGenWidgets(STR_MAPGEN_CAPTION_WATER),
        makeSpinnerWidgets({179,  52}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                          ), // NB: 3 widgets
        makeWidget        ({ 10,  70}, {255, 12}, WidgetType::checkbox, WindowColour::secondary, STR_BEACHES_WATER_BODIES)
    );

    static constexpr auto ForestsWidgets = makeWidgets(
        makeMapGenWidgets(STR_MAPGEN_CAPTION_FORESTS),
        makeWidget        ({ 10,  52}, {255, 12}, WidgetType::checkbox, WindowColour::secondary, STR_MAPGEN_OPTION_PLACE_TREES),
        makeSpinnerWidgets({179,  70}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                               ), // WIDX_TREE_LAND_RATIO{,_UP,_DOWN}
        makeSpinnerWidgets({179,  88}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                               ), // WIDX_TREE_ALTITUDE_MIN{,_UP,_DOWN}
        makeSpinnerWidgets({179, 106}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                               )  // WIDX_TREE_ALTITUDE_MAX{,_UP,_DOWN}
    );

    static std::span<const Widget> PageWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
        BaseWidgets,
        TerrainWidgets,
        WaterWidgets,
        ForestsWidgets,
    };
    // clang-format on

#pragma endregion

#pragma region Widget flags

    // clang-format off
    static uint64_t PageDisabledWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
        (1uLL << WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP) |
        (1uLL << WIDX_HEIGHTMAP_STRENGTH) |
        (1uLL << WIDX_HEIGHTMAP_STRENGTH_UP) |
        (1uLL << WIDX_HEIGHTMAP_STRENGTH_DOWN) |
        (1uLL << WIDX_HEIGHTMAP_NORMALIZE),

        0,

        0,

        0
    };

    static uint64_t HoldDownWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
        (1uLL << WIDX_MAP_SIZE_Y_UP) |
        (1uLL << WIDX_MAP_SIZE_Y_DOWN) |
        (1uLL << WIDX_MAP_SIZE_X_UP) |
        (1uLL << WIDX_MAP_SIZE_X_DOWN) |
        (1uLL << WIDX_SIMPLEX_BASE_FREQ_UP) |
        (1uLL << WIDX_SIMPLEX_BASE_FREQ_DOWN) |
        (1uLL << WIDX_SIMPLEX_OCTAVES_UP) |
        (1uLL << WIDX_SIMPLEX_OCTAVES_DOWN) |
        (1uLL << WIDX_HEIGHTMAP_STRENGTH_UP) |
        (1uLL << WIDX_HEIGHTMAP_STRENGTH_DOWN),

        (1uLL << WIDX_HEIGHTMAP_LOW_UP) |
        (1uLL << WIDX_HEIGHTMAP_LOW_DOWN) |
        (1uLL << WIDX_HEIGHTMAP_HIGH_UP) |
        (1uLL << WIDX_HEIGHTMAP_HIGH_DOWN),

        (1uLL << WIDX_WATER_LEVEL_UP) |
        (1uLL << WIDX_WATER_LEVEL_DOWN),

        (1uLL << WIDX_TREE_LAND_RATIO_UP) |
        (1uLL << WIDX_TREE_LAND_RATIO_DOWN) |
        (1uLL << WIDX_TREE_ALTITUDE_MIN_UP) |
        (1uLL << WIDX_TREE_ALTITUDE_MIN_DOWN) |
        (1uLL << WIDX_TREE_ALTITUDE_MAX_UP) |
        (1uLL << WIDX_TREE_ALTITUDE_MAX_DOWN),
    };

    static uint64_t PressedWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
        0,
        (1uLL << WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES),
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
    };
    static constexpr int32_t TabAnimationFrames[WINDOW_MAPGEN_PAGE_COUNT] = {
        4,
        1,
        1,
        1,
    };
    static constexpr int32_t TabAnimationLoops[WINDOW_MAPGEN_PAGE_COUNT] = {
        16,
        1,
        1,
        1,
    };
    // clang-format on

    enum class ResizeDirection
    {
        Both,
        X,
        Y,
    };

    static void HeightmapLoadsaveCallback(ModalResult result, const utf8* path);

    class MapGenWindow final : public Window
    {
    private:
        ResizeDirection _resizeDirection{ ResizeDirection::Both };
        bool _mapWidthAndHeightLinked{ true };
        MapGenerator::Settings _settings{};
        bool _randomTerrain = true;
        bool _heightmapLoaded = false;
        std::string _heightmapFilename{};

        void setPage(int32_t newPage)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == newPage && !widgets.empty())
                return;

            page = newPage;
            currentFrame = 0;
            removeViewport();

            setWidgets(PageWidgets[newPage]);
            holdDownWidgets = HoldDownWidgets[newPage];
            disabledWidgets = PageDisabledWidgets[newPage];
            pressedWidgets = PressedWidgets[newPage];

            initScrollWidgets();
            invalidate();
        }

        void SetPressedTab()
        {
            for (auto i = 0; i < WINDOW_MAPGEN_PAGE_COUNT; i++)
                pressedWidgets &= ~(1 << (WIDX_TAB_1 + i));
            pressedWidgets |= 1LL << (WIDX_TAB_1 + page);
        }

        void DrawTabImage(RenderTarget& rt, int32_t newPage, int32_t spriteIndex)
        {
            WidgetIndex widgetIndex = WIDX_TAB_1 + newPage;

            if (!widgetIsDisabled(*this, widgetIndex))
            {
                if (page == newPage)
                {
                    int32_t frame = currentFrame / TabAnimationDivisor[page];
                    spriteIndex += (frame % TabAnimationFrames[page]);
                }

                GfxDrawSprite(
                    rt, ImageId(spriteIndex),
                    windowPos + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
            }
        }

        void DrawTabImages(RenderTarget& rt)
        {
            DrawTabImage(rt, WINDOW_MAPGEN_PAGE_BASE, SPR_TAB_GEARS_0);
            DrawTabImage(rt, WINDOW_MAPGEN_PAGE_TERRAIN, SPR_G2_MAP_GEN_TERRAIN_TAB);
            DrawTabImage(rt, WINDOW_MAPGEN_PAGE_WATER, SPR_TAB_WATER);
            DrawTabImage(rt, WINDOW_MAPGEN_PAGE_FORESTS, SPR_TAB_SCENERY_TREES);
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
                    close();
                    break;
                case WIDX_TAB_1:
                case WIDX_TAB_2:
                case WIDX_TAB_3:
                case WIDX_TAB_4:
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
                case WIDX_MAP_GENERATE:
                    GenerateMap();
                    break;
            }
        }

        void GenerateMap()
        {
            if (_settings.algorithm == MapGenerator::Algorithm::heightmapImage && !_heightmapLoaded)
                return;

            MapGenerator::Settings mapgenSettings = _settings;
            if (_randomTerrain)
            {
                mapgenSettings.landTexture = -1;
                mapgenSettings.edgeTexture = -1;
            }

            MapGenerator::generate(&mapgenSettings);
            GfxInvalidateScreen();
        }

#pragma region Base page

        void BaseMouseUp(WidgetIndex widgetIndex)
        {
            SharedMouseUp(widgetIndex);

            if (_settings.algorithm == MapGenerator::Algorithm::simplexNoise)
                SimplexMouseUp(widgetIndex);
            else if (_settings.algorithm == MapGenerator::Algorithm::heightmapImage)
                HeightmapMouseUp(widgetIndex);

            switch (widgetIndex)
            {
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
            if (_settings.algorithm == MapGenerator::Algorithm::simplexNoise)
                SimplexMouseDown(widgetIndex, widget);

            else if (_settings.algorithm == MapGenerator::Algorithm::heightmapImage)
                HeightmapMouseDown(widgetIndex, widget);

            switch (widgetIndex)
            {
                case WIDX_MAP_SIZE_Y_UP:
                    _resizeDirection = ResizeDirection::Y;
                    ChangeMapSize(+1);
                    invalidate();
                    break;
                case WIDX_MAP_SIZE_Y_DOWN:
                    _resizeDirection = ResizeDirection::Y;
                    ChangeMapSize(-1);
                    invalidate();
                    break;
                case WIDX_MAP_SIZE_X_UP:
                    _resizeDirection = ResizeDirection::X;
                    ChangeMapSize(+1);
                    invalidate();
                    break;
                case WIDX_MAP_SIZE_X_DOWN:
                    _resizeDirection = ResizeDirection::X;
                    ChangeMapSize(-1);
                    invalidate();
                    break;
                case WIDX_HEIGHTMAP_SOURCE_DROPDOWN:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        ToggleOption(0, STR_HEIGHTMAP_FLATLAND),
                        ToggleOption(1, STR_HEIGHTMAP_SIMPLEX_NOISE),
                        ToggleOption(2, STR_HEIGHTMAP_FILE),
                    };

                    SetItems(items);

                    Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height(), colours[1], 0,
                        Dropdown::Flag::StayOpen, std::size(items), ddWidget->width() - 3);

                    gDropdown.items[EnumValue(_settings.algorithm)].setChecked(true);
                    break;
                }
            }
        }

        void BaseUpdate()
        {
            // Tab animation
            if (++currentFrame >= TabAnimationLoops[page])
                currentFrame = 0;
            invalidateWidget(WIDX_TAB_1);
        }

        void BaseDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (dropdownIndex == -1)
                return;

            switch (widgetIndex)
            {
                case WIDX_HEIGHTMAP_SOURCE_DROPDOWN:
                    _settings.algorithm = MapGenerator::Algorithm(dropdownIndex);
                    invalidate();
                    break;
            }
        }

        void BaseTextInput(WidgetIndex widgetIndex, int32_t value)
        {
            if (_settings.algorithm == MapGenerator::Algorithm::simplexNoise)
                SimplexTextInput(widgetIndex, value);
            else if (_settings.algorithm == MapGenerator::Algorithm::heightmapImage)
                HeightmapTextInput(widgetIndex, value);

            switch (widgetIndex)
            {
                case WIDX_MAP_SIZE_Y:
                case WIDX_MAP_SIZE_X:
                    // The practical size is 2 lower than the technical size
                    auto technicalSize = std::clamp<uint16_t>(value + 2, kMinimumMapSizeTechnical, kMaximumMapSizeTechnical);
                    if (_resizeDirection == ResizeDirection::Y || _mapWidthAndHeightLinked)
                        _settings.mapSize.y = technicalSize;
                    if (_resizeDirection == ResizeDirection::X || _mapWidthAndHeightLinked)
                        _settings.mapSize.x = technicalSize;
                    break;
            }

            invalidate();
        }

        void BasePrepareDraw()
        {
            // only allow linking the map size when X and Y are the same
            setWidgetPressed(WIDX_MAP_SIZE_LINK, _mapWidthAndHeightLinked);
            setWidgetDisabled(WIDX_MAP_SIZE_LINK, _settings.mapSize.x != _settings.mapSize.y);

            bool isHeightMapImage = _settings.algorithm == MapGenerator::Algorithm::heightmapImage;
            setWidgetDisabled(WIDX_MAP_SIZE_Y, isHeightMapImage);
            setWidgetDisabled(WIDX_MAP_SIZE_Y_UP, isHeightMapImage);
            setWidgetDisabled(WIDX_MAP_SIZE_Y_DOWN, isHeightMapImage);
            setWidgetDisabled(WIDX_MAP_SIZE_LINK, isHeightMapImage);
            setWidgetDisabled(WIDX_MAP_SIZE_X, isHeightMapImage);
            setWidgetDisabled(WIDX_MAP_SIZE_X_UP, isHeightMapImage);
            setWidgetDisabled(WIDX_MAP_SIZE_X_DOWN, isHeightMapImage);

            // Enable heightmap widgets if one is loaded
            if (isHeightMapImage)
            {
                setWidgetEnabled(WIDX_HEIGHTMAP_NORMALIZE, _heightmapLoaded);
                setWidgetEnabled(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _heightmapLoaded);
                setWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH, _heightmapLoaded && _settings.smooth_height_map);
                setWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_UP, _heightmapLoaded && _settings.smooth_height_map);
                setWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_DOWN, _heightmapLoaded && _settings.smooth_height_map);
            }

            SetPressedTab();

            // Push width (Y) and height (X) to the common formatter arguments for the map size spinners to use
            auto ft = Formatter::Common();
            ft.Add<uint16_t>(_settings.mapSize.y - 2);
            ft.Add<uint16_t>(_settings.mapSize.x - 2);

            auto& sourceWidget = widgets[WIDX_HEIGHTMAP_SOURCE];
            switch (_settings.algorithm)
            {
                case MapGenerator::Algorithm::blank:
                    sourceWidget.text = STR_HEIGHTMAP_FLATLAND;
                    ToggleSimplexWidgets(false);
                    ToggleHeightmapWidgets(false);
                    break;

                case MapGenerator::Algorithm::simplexNoise:
                    sourceWidget.text = STR_HEIGHTMAP_SIMPLEX_NOISE;
                    ToggleSimplexWidgets(true);
                    ToggleHeightmapWidgets(false);
                    break;

                case MapGenerator::Algorithm::heightmapImage:
                    sourceWidget.text = STR_HEIGHTMAP_FILE;
                    ToggleSimplexWidgets(false);
                    ToggleHeightmapWidgets(true);
                    HeightmapPrepareDraw();
                    break;
            }
        }

        void ToggleSimplexWidgets(bool state)
        {
            // clang-format off
            widgets[WIDX_SIMPLEX_GROUP].type          = state ? WidgetType::groupbox : WidgetType::empty;
            widgets[WIDX_SIMPLEX_BASE_FREQ].type      = state ? WidgetType::spinner  : WidgetType::empty;
            widgets[WIDX_SIMPLEX_BASE_FREQ_UP].type   = state ? WidgetType::button   : WidgetType::empty;
            widgets[WIDX_SIMPLEX_BASE_FREQ_DOWN].type = state ? WidgetType::button   : WidgetType::empty;
            widgets[WIDX_SIMPLEX_OCTAVES].type        = state ? WidgetType::spinner  : WidgetType::empty;
            widgets[WIDX_SIMPLEX_OCTAVES_UP].type     = state ? WidgetType::button   : WidgetType::empty;
            widgets[WIDX_SIMPLEX_OCTAVES_DOWN].type   = state ? WidgetType::button   : WidgetType::empty;
            // clang-format on
        }

        void ToggleHeightmapWidgets(bool state)
        {
            // clang-format off
            widgets[WIDX_HEIGHTMAP_GROUP].type            = state ? WidgetType::groupbox : WidgetType::empty;
            widgets[WIDX_HEIGHTMAP_BROWSE].type           = state ? WidgetType::button   : WidgetType::empty;
            widgets[WIDX_HEIGHTMAP_NORMALIZE].type        = state ? WidgetType::checkbox : WidgetType::empty;
            widgets[WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP].type = state ? WidgetType::checkbox : WidgetType::empty;
            widgets[WIDX_HEIGHTMAP_STRENGTH].type         = state ? WidgetType::spinner  : WidgetType::empty;
            widgets[WIDX_HEIGHTMAP_STRENGTH_UP].type      = state ? WidgetType::button   : WidgetType::empty;
            widgets[WIDX_HEIGHTMAP_STRENGTH_DOWN].type    = state ? WidgetType::button   : WidgetType::empty;
            // clang-format on
        }

        void BaseDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            if (_settings.algorithm == MapGenerator::Algorithm::simplexNoise)
                SimplexDraw(rt);

            else if (_settings.algorithm == MapGenerator::Algorithm::heightmapImage)
                HeightmapDraw(rt);

            const auto enabledColour = colours[1];
            const auto disabledColour = enabledColour.withFlag(ColourFlag::inset, true);

            {
                auto textColour = isWidgetDisabled(WIDX_MAP_SIZE_Y) ? disabledColour : enabledColour;
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_MAP_SIZE_Y].top + 1 }, STR_MAP_SIZE, {}, { textColour });
            }

            {
                auto textColour = enabledColour;
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_SOURCE].top + 1 }, STR_HEIGHTMAP_SOURCE, {},
                    { textColour });
            }
        }

#pragma endregion

#pragma region Forests page

        void ForestsMouseUp(WidgetIndex widgetIndex)
        {
            SharedMouseUp(widgetIndex);

            switch (widgetIndex)
            {
                case WIDX_FORESTS_PLACE_TREES:
                    _settings.trees ^= true;
                    invalidate();
                    break;

                case WIDX_TREE_LAND_RATIO:
                {
                    Formatter ft;
                    ft.Add<int16_t>(1);
                    ft.Add<int16_t>(50);
                    WindowTextInputOpen(
                        this, widgetIndex, STR_TREE_TO_LAND_RATIO, STR_ENTER_TREE_TO_LAND_RATIO, ft, STR_FORMAT_INTEGER,
                        _settings.treeToLandRatio, 2);
                    break;
                }

                case WIDX_TREE_ALTITUDE_MIN:
                {
                    Formatter ft;
                    ft.Add<int16_t>(BaseZToMetres(kMinimumLandHeight));
                    ft.Add<int16_t>(BaseZToMetres(kMaximumLandHeight));
                    WindowTextInputOpen(
                        this, widgetIndex, STR_MIN_TREE_ALTITUDE, STR_ENTER_MIN_TREE_ALTITUDE, ft, STR_FORMAT_INTEGER,
                        BaseZToMetres(_settings.minTreeAltitude), 6);
                    break;
                }

                case WIDX_TREE_ALTITUDE_MAX:
                {
                    Formatter ft;
                    ft.Add<int16_t>(BaseZToMetres(kMinimumLandHeight));
                    ft.Add<int16_t>(BaseZToMetres(kMaximumLandHeight));
                    WindowTextInputOpen(
                        this, widgetIndex, STR_MAX_TREE_ALTITUDE, STR_ENTER_MAX_TREE_ALTITUDE, ft, STR_FORMAT_INTEGER,
                        BaseZToMetres(_settings.maxTreeAltitude), 6);
                    break;
                }
            }
        }

        void ForestsMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_TREE_LAND_RATIO_UP:
                    _settings.treeToLandRatio = std::min(_settings.treeToLandRatio + 1, 50);
                    invalidateWidget(WIDX_TREE_LAND_RATIO);
                    break;
                case WIDX_TREE_LAND_RATIO_DOWN:
                    _settings.treeToLandRatio = std::max(_settings.treeToLandRatio - 1, 1);
                    invalidateWidget(WIDX_TREE_LAND_RATIO);
                    break;
                case WIDX_TREE_ALTITUDE_MIN_UP:
                    _settings.minTreeAltitude = std::min(_settings.minTreeAltitude + 2, kMaximumLandHeight / 2 - 1);
                    _settings.maxTreeAltitude = std::max(_settings.maxTreeAltitude, _settings.minTreeAltitude + 2);
                    invalidateWidget(WIDX_TREE_ALTITUDE_MIN);
                    break;
                case WIDX_TREE_ALTITUDE_MIN_DOWN:
                    _settings.minTreeAltitude = std::max<int32_t>(_settings.minTreeAltitude - 2, kMinimumLandHeight);
                    invalidateWidget(WIDX_TREE_ALTITUDE_MIN);
                    break;
                case WIDX_TREE_ALTITUDE_MAX_UP:
                    _settings.maxTreeAltitude = std::min<int32_t>(_settings.maxTreeAltitude + 2, kMaximumLandHeight - 1);
                    invalidateWidget(WIDX_TREE_ALTITUDE_MAX);
                    break;
                case WIDX_TREE_ALTITUDE_MAX_DOWN:
                    _settings.maxTreeAltitude = std::max<int32_t>(_settings.maxTreeAltitude - 2, kMinimumLandHeight - 1);
                    _settings.minTreeAltitude = std::min(_settings.minTreeAltitude, _settings.maxTreeAltitude - 2);
                    invalidateWidget(WIDX_TREE_ALTITUDE_MAX);
                    break;
            }
        }

        void ForestsUpdate()
        {
            // Tab animation
            if (++currentFrame >= TabAnimationLoops[page])
                currentFrame = 0;
            invalidateWidget(WIDX_TAB_2);
        }

        void ForestsTextInput(WidgetIndex widgetIndex, int32_t rawValue, int32_t value)
        {
            switch (widgetIndex)
            {
                case WIDX_TREE_LAND_RATIO:
                    _settings.treeToLandRatio = std::clamp(rawValue, 1, 50);
                    break;

                case WIDX_TREE_ALTITUDE_MIN:
                    _settings.minTreeAltitude = value;
                    _settings.maxTreeAltitude = std::max(_settings.minTreeAltitude, _settings.maxTreeAltitude);
                    break;

                case WIDX_TREE_ALTITUDE_MAX:
                    _settings.maxTreeAltitude = value;
                    _settings.minTreeAltitude = std::min(_settings.minTreeAltitude, _settings.maxTreeAltitude);
                    break;
            }

            invalidate();
        }

        void ForestsPrepareDraw()
        {
            pressedWidgets = 0;
            if (_settings.trees)
                pressedWidgets |= 1uLL << WIDX_FORESTS_PLACE_TREES;

            SetPressedTab();

            const bool isFlatland = _settings.algorithm == MapGenerator::Algorithm::blank;

            setWidgetDisabled(WIDX_TREE_LAND_RATIO, !_settings.trees);
            setWidgetDisabled(WIDX_TREE_LAND_RATIO_UP, !_settings.trees);
            setWidgetDisabled(WIDX_TREE_LAND_RATIO_DOWN, !_settings.trees);
            setWidgetDisabled(WIDX_TREE_ALTITUDE_MIN, !_settings.trees);
            setWidgetDisabled(WIDX_TREE_ALTITUDE_MIN_UP, !_settings.trees);
            setWidgetDisabled(WIDX_TREE_ALTITUDE_MIN_DOWN, !_settings.trees);
            setWidgetDisabled(WIDX_TREE_ALTITUDE_MAX, !_settings.trees || isFlatland);
            setWidgetDisabled(WIDX_TREE_ALTITUDE_MAX_UP, !_settings.trees || isFlatland);
            setWidgetDisabled(WIDX_TREE_ALTITUDE_MAX_DOWN, !_settings.trees || isFlatland);
        }

        void ForestsDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            const auto enabledColour = colours[1];
            const auto disabledColour = enabledColour.withFlag(ColourFlag::inset, true);

            const auto textColour = _settings.trees ? enabledColour : disabledColour;

            // Tree to land ratio, label and value
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_TREE_LAND_RATIO].top + 1 }, STR_MAPGEN_TREE_TO_LAND_RATIO, {},
                { textColour });

            auto ft = Formatter();
            ft.Add<uint16_t>(_settings.treeToLandRatio);
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_TREE_LAND_RATIO].left + 1, widgets[WIDX_TREE_LAND_RATIO].top + 1 },
                STR_MAPGEN_TREE_TO_LAND_RATIO_PCT, ft, { textColour });

            // Minimum tree altitude, label and value
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_TREE_ALTITUDE_MIN].top + 1 }, STR_MAPGEN_TREE_MIN_ALTITUDE, {},
                { textColour });

            ft = Formatter();
            ft.Add<int16_t>(BaseZToMetres(_settings.minTreeAltitude));
            DrawTextBasic(
                rt,
                windowPos + ScreenCoordsXY{ widgets[WIDX_TREE_ALTITUDE_MIN].left + 1, widgets[WIDX_TREE_ALTITUDE_MIN].top + 1 },
                STR_RIDE_LENGTH_ENTRY, ft, { textColour });

            // Maximum tree altitude, label and value
            const bool isFlatland = _settings.algorithm == MapGenerator::Algorithm::blank;
            const auto maxTreeTextColour = _settings.trees && !isFlatland ? enabledColour : disabledColour;

            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_TREE_ALTITUDE_MAX].top + 1 }, STR_MAPGEN_TREE_MAX_ALTITUDE, {},
                { maxTreeTextColour });

            ft = Formatter();
            ft.Add<int16_t>(BaseZToMetres(_settings.maxTreeAltitude));
            DrawTextBasic(
                rt,
                windowPos + ScreenCoordsXY{ widgets[WIDX_TREE_ALTITUDE_MAX].left + 1, widgets[WIDX_TREE_ALTITUDE_MAX].top + 1 },
                STR_RIDE_LENGTH_ENTRY, ft, { maxTreeTextColour });
        }

#pragma endregion

#pragma region Simplex settings, part of generator tab

        void SimplexMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_SIMPLEX_BASE_FREQ:
                {
                    Formatter ft;
                    ft.Add<int32_t>(0);
                    ft.Add<int32_t>(1000);
                    WindowTextInputOpen(
                        this, widgetIndex, STR_SIMPLEX_BASE_FREQUENCY, STR_ENTER_BASE_FREQUENCY, ft, STR_FORMAT_COMMA2DP32,
                        _settings.simplex_base_freq, 4);
                    break;
                }

                case WIDX_SIMPLEX_OCTAVES:
                {
                    Formatter ft;
                    ft.Add<int16_t>(1);
                    ft.Add<int16_t>(10);
                    WindowTextInputOpen(
                        this, widgetIndex, STR_SIMPLEX_OCTAVES, STR_ENTER_OCTAVES, ft, STR_FORMAT_INTEGER,
                        _settings.simplex_octaves, 10);
                    break;
                }
            }
        }

        void SimplexMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_SIMPLEX_BASE_FREQ_UP:
                    _settings.simplex_base_freq = std::min<int32_t>(_settings.simplex_base_freq + 5, 1000);
                    invalidate();
                    break;
                case WIDX_SIMPLEX_BASE_FREQ_DOWN:
                    _settings.simplex_base_freq = std::max<int32_t>(_settings.simplex_base_freq - 5, 0);
                    invalidate();
                    break;
                case WIDX_SIMPLEX_OCTAVES_UP:
                    _settings.simplex_octaves = std::min(_settings.simplex_octaves + 1, 10);
                    invalidate();
                    break;
                case WIDX_SIMPLEX_OCTAVES_DOWN:
                    _settings.simplex_octaves = std::max(_settings.simplex_octaves - 1, 1);
                    invalidate();
                    break;
            }
        }

        void SimplexDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            const auto textColour = colours[1];

            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1 },
                STR_MAPGEN_SIMPLEX_NOISE_BASE_FREQUENCY, {}, { textColour });
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_SIMPLEX_OCTAVES].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_OCTAVES,
                {}, { textColour });

            auto ft = Formatter();
            ft.Add<uint16_t>(_settings.simplex_base_freq);
            DrawTextBasic(
                rt,
                windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_BASE_FREQ].left + 1, widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1 },
                STR_WINDOW_COLOUR_2_COMMA2DP32, ft, { textColour });

            ft = Formatter();
            ft.Add<uint16_t>(_settings.simplex_octaves);
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_OCTAVES].left + 1, widgets[WIDX_SIMPLEX_OCTAVES].top + 1 },
                STR_COMMA16, ft, { textColour });
        }

        void SimplexTextInput(WidgetIndex widgetIndex, int32_t value)
        {
            switch (widgetIndex)
            {
                case WIDX_SIMPLEX_BASE_FREQ:
                    _settings.simplex_base_freq = std::clamp(value, 0, 1000);
                    break;

                case WIDX_SIMPLEX_OCTAVES:
                    _settings.simplex_octaves = std::clamp(value, 1, 10);
                    break;
            }
        }

#pragma endregion

#pragma region Heightmap settings, part of generator tab

        void HeightmapMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_HEIGHTMAP_STRENGTH_UP:
                    _settings.smooth_strength = std::min<uint32_t>(_settings.smooth_strength + 1, 20);
                    invalidateWidget(WIDX_HEIGHTMAP_STRENGTH);
                    break;
                case WIDX_HEIGHTMAP_STRENGTH_DOWN:
                    _settings.smooth_strength = std::max<uint32_t>(_settings.smooth_strength - 1, 1);
                    invalidateWidget(WIDX_HEIGHTMAP_STRENGTH);
                    break;
            }
        }

        void HeightmapMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                // Page widgets
                case WIDX_HEIGHTMAP_BROWSE:
                {
                    auto intent = Intent(WindowClass::loadsave);
                    intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::load);
                    intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::heightmap);
                    intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(HeightmapLoadsaveCallback));
                    ContextOpenIntent(&intent);
                    return;
                }
                case WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP:
                    _settings.smooth_height_map = !_settings.smooth_height_map;
                    setCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _settings.smooth_height_map);
                    setWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH, _settings.smooth_height_map);
                    setWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_UP, _settings.smooth_height_map);
                    setWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_DOWN, _settings.smooth_height_map);
                    invalidateWidget(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP);
                    invalidateWidget(WIDX_HEIGHTMAP_STRENGTH);
                    break;

                case WIDX_HEIGHTMAP_NORMALIZE:
                    _settings.normalize_height = !_settings.normalize_height;
                    setCheckboxValue(WIDX_HEIGHTMAP_NORMALIZE, _settings.normalize_height);
                    invalidateWidget(WIDX_HEIGHTMAP_NORMALIZE);
                    break;

                case WIDX_HEIGHTMAP_STRENGTH:
                {
                    Formatter ft;
                    ft.Add<int16_t>(1);
                    ft.Add<int16_t>(20);
                    WindowTextInputOpen(
                        this, widgetIndex, STR_SMOOTH_STRENGTH, STR_ENTER_SMOOTH_STRENGTH, ft, STR_FORMAT_INTEGER,
                        _settings.smooth_strength, 2);
                    break;
                }
            }
        }

        void HeightmapPrepareDraw()
        {
            setCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _settings.smooth_height_map);
            setCheckboxValue(WIDX_HEIGHTMAP_NORMALIZE, _settings.normalize_height);
        }

        void HeightmapDraw(RenderTarget& rt)
        {
            const auto enabledColour = colours[1];
            const auto disabledColour = enabledColour.withFlag(ColourFlag::inset, true);

            // Smooth strength label and value
            const bool strengthDisabled = isWidgetDisabled(WIDX_HEIGHTMAP_STRENGTH) || !_settings.smooth_height_map;
            const auto strengthColour = strengthDisabled ? disabledColour : enabledColour;

            // Smooth strength label
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 24, widgets[WIDX_HEIGHTMAP_STRENGTH].top + 1 }, STR_MAPGEN_SMOOTH_STRENGTH, {},
                { strengthColour });

            // Smooth strength value
            auto ft = Formatter();
            ft.Add<uint16_t>(_settings.smooth_strength);
            auto pos = ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_STRENGTH].left + 1, widgets[WIDX_HEIGHTMAP_STRENGTH].top + 1 };
            DrawTextBasic(rt, windowPos + pos, STR_COMMA16, ft, { strengthColour });

            // Current heightmap image filename
            ft = Formatter();
            if (!_heightmapLoaded)
                ft.Add<char*>(LanguageGetString(STR_MAPGEN_NONE_SELECTED));
            else
                ft.Add<char*>(_heightmapFilename.c_str());

            pos = ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_BROWSE].top + 1 };
            auto textWidth = widgets[WIDX_HEIGHTMAP_BROWSE].left - 11;
            DrawTextEllipsised(rt, windowPos + pos, textWidth, STR_MAPGEN_CURRENT_HEIGHTMAP_FILE, ft);
        }

        void HeightmapTextInput(WidgetIndex widgetIndex, int32_t value)
        {
            switch (widgetIndex)
            {
                case WIDX_HEIGHTMAP_STRENGTH:
                    _settings.smooth_strength = std::clamp(value, 1, 20);
                    break;
            }
        }

#pragma endregion

#pragma region Terrain page

        void TerrainMouseUp(WidgetIndex widgetIndex)
        {
            SharedMouseUp(widgetIndex);

            switch (widgetIndex)
            {
                case WIDX_HEIGHTMAP_LOW:
                {
                    Formatter ft;
                    ft.Add<int16_t>(BaseZToMetres(kMinimumLandHeight));
                    ft.Add<int16_t>(BaseZToMetres(kMaximumLandHeight));
                    WindowTextInputOpen(
                        this, widgetIndex, STR_MIN_LAND_HEIGHT, STR_ENTER_MIN_LAND, ft, STR_FORMAT_INTEGER,
                        BaseZToMetres(_settings.heightmapLow), 6);
                    break;
                }

                case WIDX_HEIGHTMAP_HIGH:
                {
                    Formatter ft;
                    ft.Add<int16_t>(BaseZToMetres(kMinimumLandHeight));
                    ft.Add<int16_t>(BaseZToMetres(kMaximumLandHeight));
                    WindowTextInputOpen(
                        this, widgetIndex, STR_MAX_LAND_HEIGHT, STR_ENTER_MAX_LAND, ft, STR_FORMAT_INTEGER,
                        BaseZToMetres(_settings.heightmapHigh), 6);
                    break;
                }

                case WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES:
                    _settings.smoothTileEdges = !_settings.smoothTileEdges;
                    setCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES, _settings.smoothTileEdges);
                    invalidateWidget(WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES);
                    break;
            }
        }

        void TerrainMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_RANDOM_TERRAIN:
                    _randomTerrain = !_randomTerrain;
                    invalidate();
                    break;
                case WIDX_FLOOR_TEXTURE:
                    LandTool::ShowSurfaceStyleDropdown(this, widget, _settings.landTexture);
                    break;
                case WIDX_WALL_TEXTURE:
                    LandTool::ShowEdgeStyleDropdown(this, widget, _settings.edgeTexture);
                    break;
                case WIDX_HEIGHTMAP_LOW_UP:
                    _settings.heightmapLow = std::min(_settings.heightmapLow + 2, kMaximumLandHeight / 2 - 1);
                    _settings.heightmapHigh = std::max(_settings.heightmapHigh, _settings.heightmapLow + 2);
                    invalidateWidget(WIDX_HEIGHTMAP_LOW);
                    break;
                case WIDX_HEIGHTMAP_LOW_DOWN:
                    _settings.heightmapLow = std::max<int32_t>(_settings.heightmapLow - 2, kMinimumLandHeight);
                    invalidateWidget(WIDX_HEIGHTMAP_LOW);
                    break;
                case WIDX_HEIGHTMAP_HIGH_UP:
                    _settings.heightmapHigh = std::min<int32_t>(_settings.heightmapHigh + 2, kMaximumLandHeight - 1);
                    invalidateWidget(WIDX_HEIGHTMAP_HIGH);
                    break;
                case WIDX_HEIGHTMAP_HIGH_DOWN:
                    _settings.heightmapHigh = std::max<int32_t>(_settings.heightmapHigh - 2, kMinimumLandHeight);
                    _settings.heightmapLow = std::min(_settings.heightmapLow, _settings.heightmapHigh - 2);
                    invalidateWidget(WIDX_HEIGHTMAP_HIGH);
                    break;
            }
        }

        void TerrainUpdate()
        {
            // Tab animation
            if (++currentFrame >= TabAnimationLoops[page])
                currentFrame = 0;
            invalidateWidget(WIDX_TAB_3);
        }

        void TerrainTextInput(WidgetIndex widgetIndex, int32_t value)
        {
            switch (widgetIndex)
            {
                case WIDX_HEIGHTMAP_LOW:
                    _settings.heightmapLow = value;
                    _settings.heightmapHigh = std::max(_settings.heightmapLow, _settings.heightmapHigh);
                    break;

                case WIDX_HEIGHTMAP_HIGH:
                    _settings.heightmapHigh = value;
                    _settings.heightmapLow = std::min(_settings.heightmapLow, _settings.heightmapHigh);
                    break;
            }

            invalidate();
        }

        void TerrainDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            int32_t type;

            switch (widgetIndex)
            {
                case WIDX_FLOOR_TEXTURE:
                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdown.highlightedIndex;

                    type = (dropdownIndex == -1) ? _settings.landTexture : dropdownIndex;

                    if (gLandToolTerrainSurface == type)
                    {
                        gLandToolTerrainSurface = kObjectEntryIndexNull;
                    }
                    else
                    {
                        gLandToolTerrainSurface = type;
                        _settings.landTexture = type;
                    }
                    invalidate();
                    break;
                case WIDX_WALL_TEXTURE:
                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdown.highlightedIndex;

                    type = (dropdownIndex == -1) ? _settings.edgeTexture : dropdownIndex;

                    if (gLandToolTerrainEdge == type)
                    {
                        gLandToolTerrainEdge = kObjectEntryIndexNull;
                    }
                    else
                    {
                        gLandToolTerrainEdge = type;
                        _settings.edgeTexture = type;
                    }
                    invalidate();
                    break;
            }
        }

        void DrawDropdownButton(RenderTarget& rt, WidgetIndex widgetIndex, ImageId image)
        {
            const auto& widget = widgets[widgetIndex];
            ScreenCoordsXY pos = { windowPos.x + widget.left, windowPos.y + widget.top };
            if (isWidgetDisabled(widgetIndex))
            {
                // Draw greyed out (light border bottom right shadow)
                auto colour = colours[widget.colour].colour;
                auto paletteIndex = getColourMap(colour).lighter;
                GfxDrawSpriteSolid(rt, image, pos + ScreenCoordsXY{ 1, 1 }, paletteIndex);

                // Draw greyed out (dark)
                colour = colours[widget.colour].colour;
                paletteIndex = getColourMap(colour).midLight;
                GfxDrawSpriteSolid(rt, image, pos, paletteIndex);
            }
            else
            {
                GfxDrawSprite(rt, image, pos);
            }
        }

        void DrawDropdownButtons(RenderTarget& rt, WidgetIndex floorWidgetIndex, WidgetIndex edgeWidgetIndex)
        {
            auto& objManager = GetContext()->GetObjectManager();
            const auto* surfaceObj = objManager.GetLoadedObject<TerrainSurfaceObject>(_settings.landTexture);
            ImageId surfaceImage;
            if (surfaceObj != nullptr)
            {
                surfaceImage = ImageId(surfaceObj->IconImageId);
                if (surfaceObj->Colour != kColourNull)
                {
                    surfaceImage = surfaceImage.WithPrimary(surfaceObj->Colour);
                }
            }

            ImageId edgeImage;
            const auto* edgeObj = objManager.GetLoadedObject<TerrainEdgeObject>(_settings.edgeTexture);
            if (edgeObj != nullptr)
            {
                edgeImage = ImageId(edgeObj->IconImageId);
            }

            DrawDropdownButton(rt, floorWidgetIndex, surfaceImage);
            DrawDropdownButton(rt, edgeWidgetIndex, edgeImage);
        }

        void TerrainPrepareDraw()
        {
            setCheckboxValue(WIDX_RANDOM_TERRAIN, _randomTerrain != 0);
            setCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES, _settings.smoothTileEdges);

            // only allow floor and wall texture options if random terrain is disabled
            setWidgetEnabled(WIDX_FLOOR_TEXTURE, !_randomTerrain);
            setWidgetEnabled(WIDX_WALL_TEXTURE, !_randomTerrain);

            // Max land height option is irrelevant for flatland
            setWidgetEnabled(WIDX_HEIGHTMAP_HIGH, _settings.algorithm != MapGenerator::Algorithm::blank);

            // only offer terrain edge smoothing if we don't use flatland terrain
            setWidgetEnabled(WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES, _settings.algorithm != MapGenerator::Algorithm::blank);
            SetPressedTab();
        }

        void TerrainDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            DrawTabImages(rt);
            DrawDropdownButtons(rt, WIDX_FLOOR_TEXTURE, WIDX_WALL_TEXTURE);

            const auto enabledColour = colours[1];
            const auto disabledColour = enabledColour.withFlag(ColourFlag::inset, true);

            // Floor texture label
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_FLOOR_TEXTURE].top + 1 }, STR_TERRAIN_LABEL, {},
                { enabledColour });

            // Minimum land height label and value
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_LOW].top + 1 }, STR_MAPGEN_MIN_LAND_HEIGHT, {},
                { enabledColour });

            auto ft = Formatter();
            ft.Add<int32_t>(BaseZToMetres(_settings.heightmapLow));
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_LOW].left + 1, widgets[WIDX_HEIGHTMAP_LOW].top + 1 },
                STR_RIDE_LENGTH_ENTRY, ft, { enabledColour });

            const auto maxLandColour = isWidgetDisabled(WIDX_HEIGHTMAP_HIGH) ? disabledColour : enabledColour;

            // Maximum land height label and value
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_HIGH].top + 1 }, STR_MAPGEN_MAX_LAND_HEIGHT, {},
                { maxLandColour });

            ft = Formatter();
            ft.Add<int32_t>(BaseZToMetres(_settings.heightmapHigh));
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_HIGH].left + 1, widgets[WIDX_HEIGHTMAP_HIGH].top + 1 },
                STR_RIDE_LENGTH_ENTRY, ft, { maxLandColour });
        }

#pragma endregion

#pragma region Water page

        void WaterMouseUp(WidgetIndex widgetIndex)
        {
            SharedMouseUp(widgetIndex);

            switch (widgetIndex)
            {
                case WIDX_WATER_LEVEL:
                {
                    Formatter ft;
                    ft.Add<int16_t>(kMinimumWaterHeight);
                    ft.Add<int16_t>(kMaximumWaterHeight);
                    WindowTextInputOpen(
                        this, WIDX_WATER_LEVEL, STR_WATER_LEVEL, STR_ENTER_WATER_LEVEL, ft, STR_FORMAT_INTEGER,
                        _settings.waterLevel, 6);
                    break;
                }

                case WIDX_ADD_BEACHES:
                {
                    _settings.beaches ^= true;
                    invalidate();
                    break;
                }
            }
        }

        void WaterMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_WATER_LEVEL_UP:
                    _settings.waterLevel = std::min<int32_t>(_settings.waterLevel + 2, kMaximumWaterHeight);
                    invalidate();
                    break;
                case WIDX_WATER_LEVEL_DOWN:
                    _settings.waterLevel = std::max<int32_t>(_settings.waterLevel - 2, kMinimumWaterHeight);
                    invalidate();
                    break;
            }
        }

        void WaterUpdate()
        {
            // Tab animation
            if (++currentFrame >= TabAnimationLoops[page])
                currentFrame = 0;
            invalidateWidget(WIDX_TAB_4);
        }

        void WaterTextInput(WidgetIndex widgetIndex, int32_t value)
        {
            switch (widgetIndex)
            {
                case WIDX_WATER_LEVEL:
                    _settings.waterLevel = value;
                    break;
            }

            invalidate();
        }

        void WaterPrepareDraw()
        {
            setCheckboxValue(WIDX_ADD_BEACHES, _settings.beaches != 0);

            SetPressedTab();
        }

        void WaterDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            const auto textColour = colours[1];

            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_WATER_LEVEL].top + 1 }, STR_WATER_LEVEL_LABEL, {},
                { textColour });

            auto ft = Formatter();
            ft.Add<int32_t>(BaseZToMetres(_settings.waterLevel));
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_WATER_LEVEL].left + 1, widgets[WIDX_WATER_LEVEL].top + 1 },
                STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });
        }

#pragma endregion

    public:
        void onOpen() override
        {
            number = 0;

            setPage(WINDOW_MAPGEN_PAGE_BASE);
            invalidate();
            holdDownWidgets = HoldDownWidgets[WINDOW_MAPGEN_PAGE_BASE];
            pressedWidgets = PressedWidgets[WINDOW_MAPGEN_PAGE_BASE];
            disabledWidgets = PageDisabledWidgets[WINDOW_MAPGEN_PAGE_BASE];
            initScrollWidgets();

            _heightmapLoaded = false;
        }

        void onClose() override
        {
            MapGenerator::UnloadHeightmapImage();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BaseMouseUp(widgetIndex);
                case WINDOW_MAPGEN_PAGE_FORESTS:
                    return ForestsMouseUp(widgetIndex);
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainMouseUp(widgetIndex);
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterMouseUp(widgetIndex);
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BaseMouseDown(widgetIndex, &widgets[widgetIndex]);
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainMouseDown(widgetIndex, &widgets[widgetIndex]);
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterMouseDown(widgetIndex, &widgets[widgetIndex]);
                case WINDOW_MAPGEN_PAGE_FORESTS:
                    return ForestsMouseDown(widgetIndex, &widgets[widgetIndex]);
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BaseDropdown(widgetIndex, selectedIndex);
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainDropdown(widgetIndex, selectedIndex);
            }
        }

        void onUpdate() override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BaseUpdate();
                case WINDOW_MAPGEN_PAGE_FORESTS:
                    return ForestsUpdate();
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterUpdate();
            }
        }

        void onPrepareDraw() override
        {
            bool isHeightMapImage = _settings.algorithm == MapGenerator::Algorithm::heightmapImage;
            setWidgetDisabled(WIDX_MAP_GENERATE, isHeightMapImage && !_heightmapLoaded);

            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BasePrepareDraw();
                case WINDOW_MAPGEN_PAGE_FORESTS:
                    return ForestsPrepareDraw();
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainPrepareDraw();
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterPrepareDraw();
            }
        }

        void onDraw(RenderTarget& rt) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BaseDraw(rt);
                case WINDOW_MAPGEN_PAGE_FORESTS:
                    return ForestsDraw(rt);
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainDraw(rt);
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterDraw(rt);
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            auto strText = std::string(text);
            char* end;

            // Convert text to integer value
            int32_t value{};
            if (page == WINDOW_MAPGEN_PAGE_BASE && widgetIndex == WIDX_SIMPLEX_BASE_FREQ)
                value = 100 * strtof(strText.c_str(), &end);
            else
                value = strtol(strText.c_str(), &end, 10);

            if (*end != '\0')
                return;

            // Take care of unit conversion
            int32_t rawValue = value;
            if (page != WINDOW_MAPGEN_PAGE_BASE)
            {
                switch (Config::Get().general.measurementFormat)
                {
                    case MeasurementFormat::Imperial:
                        value = FeetToMetres(value);
                        [[fallthrough]];

                    default:
                        value = std::clamp(MetresToBaseZ(value), kMinimumLandHeight, kMaximumLandHeight);
                        break;
                }
            }

            // Pass on to the actual properties
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BaseTextInput(widgetIndex, value);
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainTextInput(widgetIndex, value);
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterTextInput(widgetIndex, value);
                case WINDOW_MAPGEN_PAGE_FORESTS:
                    return ForestsTextInput(widgetIndex, rawValue, value);
            }
        }

        void afterLoadingHeightMap(ModalResult result, const utf8* path)
        {
            if (result == ModalResult::ok)
            {
                if (!MapGenerator::LoadHeightmapImage(path))
                {
                    // TODO: Display error popup
                    return;
                }

                // The window needs to be open while using the map
                _heightmapLoaded = true;
                _heightmapFilename = fs::u8path(path).filename().string();
                _settings.algorithm = MapGenerator::Algorithm::heightmapImage;
                setPage(WINDOW_MAPGEN_PAGE_BASE);
            }
        }

        void onResize() override
        {
            WindowSetResize(*this, kWindowSize, kWindowSize);
        }
    };

    WindowBase* MapgenOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<MapGenWindow>(
            WindowClass::mapgen, kWindowSize,
            { WindowFlag::higherContrastOnPress, WindowFlag::autoPosition, WindowFlag::centreScreen });
    }

    static void HeightmapLoadsaveCallback(ModalResult result, const utf8* path)
    {
        auto* w = static_cast<MapGenWindow*>(MapgenOpen());
        w->afterLoadingHeightMap(result, path);
    }
} // namespace OpenRCT2::Ui::Windows
