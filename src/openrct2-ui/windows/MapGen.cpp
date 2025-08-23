/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/object/ObjectEntryManager.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/SmallSceneryEntry.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/map_generator/Erosion.h>
#include <openrct2/world/map_generator/MapGen.h>
#include <openrct2/world/map_generator/PngTerrainGenerator.h>
#include <random>

using namespace OpenRCT2::World;

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WINDOW_MAPGEN_PAGE_BASE,
        WINDOW_MAPGEN_PAGE_TERRAIN,
        WINDOW_MAPGEN_PAGE_WATER,
        WINDOW_MAPGEN_PAGE_TEXTURE,
        WINDOW_MAPGEN_PAGE_SCENERY,
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
        WIDX_MAP_GENERATE,

        TAB_BEGIN,

        WIDX_MAP_SIZE_Y = TAB_BEGIN,
        WIDX_MAP_SIZE_Y_UP,
        WIDX_MAP_SIZE_Y_DOWN,
        WIDX_MAP_SIZE_LINK,
        WIDX_MAP_SIZE_X,
        WIDX_MAP_SIZE_X_UP,
        WIDX_MAP_SIZE_X_DOWN,
        WIDX_MAP_SEED_GROUP,
        WIDX_MAP_SEED_RANDOM,
        WIDX_MAP_SEED,
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

        WIDX_BIAS_GROUP,
        WIDX_BIAS_TYPE,
        WIDX_BIAS_TYPE_DROPDOWN,
        WIDX_BIAS_STRENGTH,
        WIDX_BIAS_STRENGTH_UP,
        WIDX_BIAS_STRENGTH_DOWN,

        WIDX_HEIGHTMAP_LOW = TAB_BEGIN,
        WIDX_HEIGHTMAP_LOW_UP,
        WIDX_HEIGHTMAP_LOW_DOWN,
        WIDX_HEIGHTMAP_HIGH,
        WIDX_HEIGHTMAP_HIGH_UP,
        WIDX_HEIGHTMAP_HIGH_DOWN,
        WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES,
        WIDX_HEIGHTMAP_EROSION,
        WIDX_HEIGHTMAP_EROSION_PPT,
        WIDX_HEIGHTMAP_EROSION_PPT_UP,
        WIDX_HEIGHTMAP_EROSION_PPT_DOWN,

        WIDX_WATER_LEVEL = TAB_BEGIN,
        WIDX_WATER_LEVEL_UP,
        WIDX_WATER_LEVEL_DOWN,

        WIDX_RULE_TX_NEW = TAB_BEGIN,
        WIDX_RULE_TX_NEW_PRESET,
        WIDX_RULE_TX_RENAME,
        WIDX_RULE_TX_REMOVE,
        WIDX_RULE_TX_MOVE_UP,
        WIDX_RULE_TX_MOVE_DOWN,
        WIDX_RULE_TX_HEADER_ENABLED,
        WIDX_RULE_TX_HEADER_NAME,
        WIDX_RULE_TX_HEADER_SURFACE,
        WIDX_RULE_TX_HEADER_EDGE,
        WIDX_RULE_TX_SCROLL,
        WIDX_RULE_TX_CONDITION_GROUP,
        WIDX_RULE_TX_CONDITION_SCROLL,
        WIDX_RULE_TX_CONDITION_REMOVE,
        WIDX_RULE_TX_CONDITION_EDIT,
        WIDX_RULE_TX_CONDITION_ADD,
        WIDX_RULE_TX_OUTCOME_GROUP,
        WIDX_RULE_TX_FLOOR_TEXTURE_CHECK,
        WIDX_RULE_TX_FLOOR_TEXTURE,
        WIDX_RULE_TX_WALL_TEXTURE_CHECK,
        WIDX_RULE_TX_WALL_TEXTURE,

        WIDX_RULE_SC_NEW = TAB_BEGIN,
        WIDX_RULE_SC_NEW_PRESET,
        WIDX_RULE_SC_RENAME,
        WIDX_RULE_SC_REMOVE,
        WIDX_RULE_SC_MOVE_UP,
        WIDX_RULE_SC_MOVE_DOWN,
        WIDX_RULE_SC_HEADER_ENABLED,
        WIDX_RULE_SC_HEADER_NAME,
        WIDX_RULE_SC_HEADER_ITEMS,
        WIDX_RULE_SC_SCROLL,
        WIDX_RULE_SC_CONDITION_GROUP,
        WIDX_RULE_SC_CONDITION_SCROLL,
        WIDX_RULE_SC_CONDITION_REMOVE,
        WIDX_RULE_SC_CONDITION_EDIT,
        WIDX_RULE_SC_CONDITION_ADD,
        WIDX_RULE_SC_OUTCOME_GROUP,
        WIDX_RULE_SC_OUTCOME_SELECT,
    };

#pragma region Widgets

    static constexpr ScreenSize kWindowSize = { 300, 276 };

    // clang-format off
    static constexpr auto makeMapGenWidgets = [](StringId title) {
        return makeWidgets(
            makeWindowShim(title, kWindowSize),
            makeWidget({   0,  43 }, { kWindowSize.width, 177 }, WidgetType::resize, WindowColour::secondary),
            makeTab   ({   3,  17 }),
            makeTab   ({  34,  17 }),
            makeTab   ({  65,  17 }),
            makeTab   ({  96,  17 }),
            makeTab   ({ 127,  17 }),
            makeWidget({ 185, 256 }, { 109, 14 }, WidgetType::button, WindowColour::secondary, STR_MAPGEN_ACTION_GENERATE)
        );
    };

    static constexpr auto BaseWidgets = makeWidgets(
        makeMapGenWidgets(STR_MAPGEN_CAPTION_GENERATOR),
        makeSpinnerWidgets ({165,  52}, { 50, 12}, WidgetType::spinner,      WindowColour::secondary, STR_COMMA16                                                ), // NB: 3 widgets
        makeWidget         ({216,  52}, { 21, 12}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_G2_LINK_CHAIN), STR_MAINTAIN_SQUARE_MAP_TOOLTIP),
        makeSpinnerWidgets ({238,  52}, { 50, 12}, WidgetType::spinner,      WindowColour::secondary, STR_POP16_COMMA16                                          ), // NB: 3 widgets

        makeWidget         ({  5,  70}, {290, 41}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_SEED       ), // WIDX_SEED_GROUP
        makeWidget         ({ 10,  90}, {150, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_MAPGEN_SEED_RANDOM), // WIDX_SEED_RANDOM
        makeWidget         ({179,  90}, {109, 14}, WidgetType::textBox,      WindowColour::secondary                        ), // WIDX_SEED

        makeDropdownWidgets({179, 117}, {109, 14}, WidgetType::dropdownMenu, WindowColour::secondary, STR_HEIGHTMAP_FLATLAND),

        makeWidget         ({  5, 134}, {290, 56}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_SIMPLEX_NOISE  ), // WIDX_SIMPLEX_GROUP
        makeSpinnerWidgets ({179, 151}, {109, 12}, WidgetType::spinner,      WindowColour::secondary                            ), // WIDX_SIMPLEX_BASE_FREQ{,_UP,_DOWN}
        makeSpinnerWidgets ({179, 169}, {109, 12}, WidgetType::spinner,      WindowColour::secondary                            ), // WIDX_SIMPLEX_OCTAVES{,_UP,_DOWN}

        makeWidget         ({  5, 134}, {290, 86}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_SELECT_HEIGHTMAP), // WIDX_HEIGHTMAP_GROUP
        makeWidget         ({223, 151}, { 65, 14}, WidgetType::button,       WindowColour::secondary, STR_BROWSE                 ), // WIDX_HEIGHTMAP_BROWSE
        makeWidget         ({ 10, 169}, {150, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_MAPGEN_NORMALIZE       ), // WIDX_HEIGHTMAP_NORMALIZE
        makeWidget         ({ 10, 185}, {150, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_MAPGEN_SMOOTH_HEIGHTMAP), // WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP
        makeSpinnerWidgets ({179, 201}, {109, 12}, WidgetType::spinner,      WindowColour::secondary                             ),  // WIDX_HEIGHTMAP_STRENGTH{,_UP,_DOWN}

        makeWidget         ({  5, 194}, {290, 56}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_BIAS     ), // WIDX_BIAS_GROUP
        makeDropdownWidgets({179, 210}, {109, 14}, WidgetType::dropdownMenu, WindowColour::secondary, STR_MAPGEN_BIAS_TYPE_NONE), // WIDX_BIAS_TYPE(_DROPDOWN)
        makeSpinnerWidgets ({179, 230}, {109, 12}, WidgetType::spinner,      WindowColour::secondary                            )  // WIDX_BIAS_STRENGTH{,_UP,_DOWN}
    );

    static constexpr auto TerrainWidgets = makeWidgets(
        makeMapGenWidgets(STR_MAPGEN_CAPTION_TERRAIN),
        makeSpinnerWidgets({179,  52}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                                          ), // WIDX_HEIGHTMAP_LOW{,_UP,_DOWN}
        makeSpinnerWidgets({179,  70}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                                          ), // WIDX_HEIGHTMAP_HIGH{,_UP,_DOWN}
        makeWidget        ({ 10, 122}, {150, 12}, WidgetType::checkbox, WindowColour::secondary, STR_MAPGEN_SMOOTH_TILE                  ), // WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES
        makeWidget        ({ 10, 138}, {255, 12}, WidgetType::checkbox, WindowColour::secondary, STR_EROSION                             ), // WIDX_HEIGHTMAP_EROSION
        makeSpinnerWidgets({179, 154}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                                          )  // WIDX_HEIGHTMAP_EROSION_PARTICLES
    );

    static constexpr auto WaterWidgets = makeWidgets(
        makeMapGenWidgets(STR_MAPGEN_CAPTION_WATER),
        makeSpinnerWidgets({179,  52}, {109, 12}, WidgetType::spinner,  WindowColour::secondary                          ) // NB: 3 widgets
    );

    static constexpr auto TextureWidgets = makeWidgets(
        makeMapGenWidgets(STR_MAPGEN_RULE_TITLE),
        makeWidget({226,  52}, { 55,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_NEW                     ),
        makeWidget({281,  52}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_DOWN                                ),
        makeWidget({152,  52}, { 69,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_RENAME                  ),
        makeWidget({ 78,  52}, { 69,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_REMOVE                  ),
        makeWidget({  5,  52}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_UP                                  ),
        makeWidget({ 22,  52}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_DOWN                                ),
        makeWidget({  5,  70}, { 20,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_ENABLED          ),
        makeWidget({ 25,  70}, {190,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_NAME             ),
        makeWidget({215,  70}, { 40,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_SURFACE          ),
        makeWidget({255,  70}, { 40,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_EDGE             ),
        makeWidget({  5,  83}, {290,  72}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL                         ),

        makeWidget({  5, 158}, {202,  94}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_RULE_GROUP_IF                ),
        makeWidget({ 10, 170}, {191,  59}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL                         ),
        makeWidget({ 10, 233}, { 59,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_COND_REMOVE             ),
        makeWidget({ 76, 233}, { 59,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_COND_EDIT               ),
        makeWidget({142, 233}, { 59,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_COND_ADD                ),

        makeWidget({212, 158}, { 83,  94}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_RULE_GROUP_THEN              ),
        makeWidget({222, 180}, { 12,  12}, WidgetType::checkbox,     WindowColour::secondary                                          ),
        makeWidget({241, 168}, { 47,  36}, WidgetType::flatBtn,      WindowColour::secondary, 0xFFFFFFFF, STR_CHANGE_BASE_LAND_TIP    ),
        makeWidget({222, 222}, { 12,  12}, WidgetType::checkbox,     WindowColour::secondary                                          ),
        makeWidget({241, 210}, { 47,  36}, WidgetType::flatBtn,      WindowColour::secondary, 0xFFFFFFFF, STR_CHANGE_VERTICAL_LAND_TIP)
    );

    static constexpr auto ForestsWidgets = makeWidgets(
        makeMapGenWidgets(STR_MAPGEN_CAPTION_FORESTS),
        makeWidget({226,  52}, { 55,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_NEW                     ),
        makeWidget({281,  52}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_DOWN                                ),
        makeWidget({152,  52}, { 69,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_RENAME                  ),
        makeWidget({ 78,  52}, { 69,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_REMOVE                  ),
        makeWidget({  5,  52}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_UP                                  ),
        makeWidget({ 22,  52}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_DOWN                                ),
        makeWidget({  5,  70}, { 20,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_ENABLED          ),
        makeWidget({ 25,  70}, {190,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_NAME             ),
        makeWidget({215,  70}, { 80,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_ITEMS            ),
        makeWidget({  5,  83}, {290,  72}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL                         ),

        makeWidget({  5, 158}, {202,  94}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_RULE_GROUP_IF                ),
        makeWidget({ 10, 170}, {191,  59}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL                         ),
        makeWidget({ 10, 233}, { 59,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_COND_REMOVE             ),
        makeWidget({ 76, 233}, { 59,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_COND_EDIT               ),
        makeWidget({142, 233}, { 59,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_COND_ADD                ),

        makeWidget({212, 158}, { 83,  94}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_RULE_GROUP_THEN              ),
        makeWidget({217, 233}, { 73,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_SELECT                  )
    );

    static std::span<const Widget> PageWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
        BaseWidgets,
        TerrainWidgets,
        WaterWidgets,
        TextureWidgets,
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
        (1uLL << WIDX_BIAS_STRENGTH_UP) |
        (1uLL << WIDX_BIAS_STRENGTH_DOWN) |
        (1uLL << WIDX_HEIGHTMAP_STRENGTH_UP) |
        (1uLL << WIDX_HEIGHTMAP_STRENGTH_DOWN),

        (1uLL << WIDX_HEIGHTMAP_LOW_UP) |
        (1uLL << WIDX_HEIGHTMAP_LOW_DOWN) |
        (1uLL << WIDX_HEIGHTMAP_HIGH_UP) |
        (1uLL << WIDX_HEIGHTMAP_HIGH_DOWN) |
        (1uLL << WIDX_HEIGHTMAP_EROSION_PPT_UP) |
        (1uLL << WIDX_HEIGHTMAP_EROSION_PPT_DOWN) ,

        (1uLL << WIDX_WATER_LEVEL_UP) |
        (1uLL << WIDX_WATER_LEVEL_DOWN),

        0,

        0,
    };

    static uint64_t PressedWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
        (1uLL << WIDX_MAP_SEED_RANDOM),

        (1uLL << WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES),

        0,

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
    };
    static constexpr int32_t TabAnimationFrames[WINDOW_MAPGEN_PAGE_COUNT] = {
        4,
        1,
        1,
        1,
        1,
    };
    static constexpr int32_t TabAnimationLoops[WINDOW_MAPGEN_PAGE_COUNT] = {
        16,
        1,
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
        u8string _seed = std::to_string(std::random_device{}());
        bool _random_seed = true;
        MapGenerator::Settings _settings{};
        bool _heightmapLoaded = false;
        std::string _heightmapFilename{};

        int32_t _selectedTxRule = -1;
        int32_t _highlightedTxRule = -1;

        int32_t _selectedTxCondition = -1;
        int32_t _highlightedTxCondition = -1;

        int32_t _selectedScRule = -1;
        int32_t _highlightedScRule = -1;

        int32_t _selectedScCondition = -1;
        int32_t _highlightedScCondition = -1;

        void SetPage(int32_t newPage)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == newPage && !widgets.empty())
                return;

            page = newPage;
            frame_no = 0;
            RemoveViewport();

            SetWidgets(PageWidgets[newPage]);
            hold_down_widgets = HoldDownWidgets[newPage];
            disabled_widgets = PageDisabledWidgets[newPage];
            pressed_widgets = PressedWidgets[newPage];

            // prevent window height growing 1px on every page change...
            OnResize();

            InitScrollWidgets();
            Invalidate();
        }

        void SetPressedTab()
        {
            for (auto i = 0; i < WINDOW_MAPGEN_PAGE_COUNT; i++)
                pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
            pressed_widgets |= 1LL << (WIDX_TAB_1 + page);
        }

        void DrawTabImage(RenderTarget& rt, int32_t newPage, int32_t spriteIndex)
        {
            WidgetIndex widgetIndex = WIDX_TAB_1 + newPage;

            if (!widgetIsDisabled(*this, widgetIndex))
            {
                if (page == newPage)
                {
                    int32_t frame = frame_no / TabAnimationDivisor[page];
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
            DrawTabImage(rt, WINDOW_MAPGEN_PAGE_TEXTURE, SPR_G2_TERRAIN_EDGE_TAB);
            DrawTabImage(rt, WINDOW_MAPGEN_PAGE_SCENERY, SPR_TAB_SCENERY_TREES);
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
                    SetPage(widgetIndex - WIDX_TAB_1);
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

            if (_random_seed)
            {
                _seed = std::to_string(std::random_device{}());
            }
            mapgenSettings.seed = static_cast<uint32_t>(std::hash<u8string>{}(_seed));

            MapGenerator::generate(&mapgenSettings);
            GfxInvalidateScreen();
        }

#pragma region Base page

        void BaseMouseUp(WidgetIndex widgetIndex)
        {
            SharedMouseUp(widgetIndex);

            if (_settings.algorithm == MapGenerator::Algorithm::simplexNoise
                || _settings.algorithm == MapGenerator::Algorithm::warpedNoise)
            {
                SimplexMouseUp(widgetIndex);
                BiasMouseUp(widgetIndex);
            }
            else if (_settings.algorithm == MapGenerator::Algorithm::heightmapImage)
            {
                HeightmapMouseUp(widgetIndex);
            }

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
                case WIDX_MAP_SEED:
                    if (!_random_seed)
                    {
                        WindowStartTextbox(*this, widgetIndex, _seed, 16);
                    }
                    break;
                case WIDX_MAP_SEED_RANDOM:
                    _random_seed = !_random_seed;
                    SetCheckboxValue(WIDX_MAP_SEED_RANDOM, _random_seed);
                    widgets[WIDX_MAP_SEED].type = _random_seed ? WidgetType::empty : WidgetType::textBox;
                    InvalidateWidget(WIDX_MAP_SEED_RANDOM);
                    InvalidateWidget(WIDX_MAP_SEED);
                    break;
            }
        }

        void BaseMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            if (_settings.algorithm == MapGenerator::Algorithm::simplexNoise
                || _settings.algorithm == MapGenerator::Algorithm::warpedNoise)
            {
                SimplexMouseDown(widgetIndex, widget);
                BiasMouseDown(widgetIndex, widget);
            }
            else if (_settings.algorithm == MapGenerator::Algorithm::heightmapImage)
            {
                HeightmapMouseDown(widgetIndex, widget);
            }

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
                case WIDX_HEIGHTMAP_SOURCE_DROPDOWN:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        ToggleOption(0, STR_HEIGHTMAP_FLATLAND),
                        ToggleOption(1, STR_HEIGHTMAP_SIMPLEX_NOISE),
                        ToggleOption(2, STR_HEIGHTMAP_WARPED_NOISE),
                        ToggleOption(3, STR_HEIGHTMAP_FILE),
                    };

                    SetItems(items);

                    Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1], 0,
                        Dropdown::Flag::StayOpen, std::size(items), ddWidget->width() - 2);

                    SetChecked(EnumValue(_settings.algorithm), true);
                    break;
                }
            }
        }

        void BaseUpdate()
        {
            // Tab animation
            if (++frame_no >= TabAnimationLoops[page])
                frame_no = 0;
            InvalidateWidget(WIDX_TAB_1);
        }

        void BaseDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (dropdownIndex == -1)
                return;

            switch (widgetIndex)
            {
                case WIDX_HEIGHTMAP_SOURCE_DROPDOWN:
                    _settings.algorithm = static_cast<MapGenerator::Algorithm>(dropdownIndex);
                    Invalidate();
                    break;
                case WIDX_BIAS_TYPE_DROPDOWN:
                    _settings.bias = static_cast<MapGenerator::Bias>(dropdownIndex);
                    Invalidate();
                    break;
            }
        }

        void BaseTextInput(WidgetIndex widgetIndex, int32_t value)
        {
            if (_settings.algorithm == MapGenerator::Algorithm::simplexNoise
                || _settings.algorithm == MapGenerator::Algorithm::warpedNoise)
            {
                SimplexTextInput(widgetIndex, value);
                BiasTextInput(widgetIndex, value);
            }
            else if (_settings.algorithm == MapGenerator::Algorithm::heightmapImage)
            {
                HeightmapTextInput(widgetIndex, value);
            }

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

            Invalidate();
        }

        void BasePrepareDraw()
        {
            // Only allow linking the map size when X and Y are the same
            SetWidgetPressed(WIDX_MAP_SIZE_LINK, _mapWidthAndHeightLinked);
            SetWidgetDisabled(WIDX_MAP_SIZE_LINK, _settings.mapSize.x != _settings.mapSize.y);

            SetCheckboxValue(WIDX_MAP_SEED_RANDOM, _random_seed);
            widgets[WIDX_MAP_SEED].string = _seed.data();
            widgets[WIDX_MAP_SEED].type = _random_seed ? WidgetType::empty : WidgetType::textBox;

            bool isHeightMapImage = _settings.algorithm == MapGenerator::Algorithm::heightmapImage;
            SetWidgetDisabled(WIDX_MAP_SIZE_Y, isHeightMapImage);
            SetWidgetDisabled(WIDX_MAP_SIZE_Y_UP, isHeightMapImage);
            SetWidgetDisabled(WIDX_MAP_SIZE_Y_DOWN, isHeightMapImage);
            SetWidgetDisabled(WIDX_MAP_SIZE_LINK, isHeightMapImage);
            SetWidgetDisabled(WIDX_MAP_SIZE_X, isHeightMapImage);
            SetWidgetDisabled(WIDX_MAP_SIZE_X_UP, isHeightMapImage);
            SetWidgetDisabled(WIDX_MAP_SIZE_X_DOWN, isHeightMapImage);

            // Enable heightmap widgets if one is loaded
            if (isHeightMapImage)
            {
                SetWidgetEnabled(WIDX_HEIGHTMAP_NORMALIZE, _heightmapLoaded);
                SetWidgetEnabled(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _heightmapLoaded);
                SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH, _heightmapLoaded && _settings.smooth_height_map);
                SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_UP, _heightmapLoaded && _settings.smooth_height_map);
                SetWidgetEnabled(WIDX_HEIGHTMAP_STRENGTH_DOWN, _heightmapLoaded && _settings.smooth_height_map);
            }

            auto isNoneBias = _settings.bias == MapGenerator::Bias::none;
            SetWidgetDisabled(WIDX_BIAS_STRENGTH, isNoneBias);
            SetWidgetDisabled(WIDX_BIAS_STRENGTH_UP, isNoneBias);
            SetWidgetDisabled(WIDX_BIAS_STRENGTH_DOWN, isNoneBias);

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
                    ToggleBiasWidgets(false);
                    break;

                case MapGenerator::Algorithm::simplexNoise:
                    sourceWidget.text = STR_HEIGHTMAP_SIMPLEX_NOISE;
                    ToggleSimplexWidgets(true);
                    ToggleHeightmapWidgets(false);
                    ToggleBiasWidgets(true);
                    break;

                case MapGenerator::Algorithm::warpedNoise:
                    sourceWidget.text = STR_HEIGHTMAP_WARPED_NOISE;
                    ToggleSimplexWidgets(true);
                    ToggleHeightmapWidgets(false);
                    ToggleBiasWidgets(true);
                    break;

                case MapGenerator::Algorithm::heightmapImage:
                    sourceWidget.text = STR_HEIGHTMAP_FILE;
                    ToggleSimplexWidgets(false);
                    ToggleHeightmapWidgets(true);
                    ToggleBiasWidgets(false);
                    HeightmapPrepareDraw();
                    break;
            }

            switch (_settings.bias)
            {
                case MapGenerator::Bias::none:
                    widgets[WIDX_BIAS_TYPE].text = STR_MAPGEN_BIAS_TYPE_NONE;
                    break;
                case MapGenerator::Bias::island:
                    widgets[WIDX_BIAS_TYPE].text = STR_MAPGEN_BIAS_TYPE_ISLAND;
                    break;
                case MapGenerator::Bias::valley:
                    widgets[WIDX_BIAS_TYPE].text = STR_MAPGEN_BIAS_TYPE_VALLEY;
                    break;
                case MapGenerator::Bias::coastal:
                    widgets[WIDX_BIAS_TYPE].text = STR_MAPGEN_BIAS_TYPE_COASTAL;
                    break;
                case MapGenerator::Bias::river:
                    widgets[WIDX_BIAS_TYPE].text = STR_MAPGEN_BIAS_TYPE_RIVER;
                    break;
                case MapGenerator::Bias::canyon:
                    widgets[WIDX_BIAS_TYPE].text = STR_MAPGEN_BIAS_TYPE_CANYON;
                    break;
                case MapGenerator::Bias::mountain:
                    widgets[WIDX_BIAS_TYPE].text = STR_MAPGEN_BIAS_TYPE_MOUNTAIN;
                    break;
                case MapGenerator::Bias::cliff:
                    widgets[WIDX_BIAS_TYPE].text = STR_MAPGEN_BIAS_TYPE_CLIFF;
                    break;
            }
        }

        void ToggleSimplexWidgets(bool state)
        {
            // clang-format off
            widgets[WIDX_SIMPLEX_GROUP].type          = state ? WidgetType::groupbox     : WidgetType::empty;
            widgets[WIDX_SIMPLEX_BASE_FREQ].type      = state ? WidgetType::spinner      : WidgetType::empty;
            widgets[WIDX_SIMPLEX_BASE_FREQ_UP].type   = state ? WidgetType::button       : WidgetType::empty;
            widgets[WIDX_SIMPLEX_BASE_FREQ_DOWN].type = state ? WidgetType::button       : WidgetType::empty;
            widgets[WIDX_SIMPLEX_OCTAVES].type        = state ? WidgetType::spinner      : WidgetType::empty;
            widgets[WIDX_SIMPLEX_OCTAVES_UP].type     = state ? WidgetType::button       : WidgetType::empty;
            widgets[WIDX_SIMPLEX_OCTAVES_DOWN].type   = state ? WidgetType::button       : WidgetType::empty;
            // clang-format on
        }

        void ToggleBiasWidgets(bool state)
        {
            // clang-format off
            widgets[WIDX_BIAS_GROUP].type         = state ? WidgetType::groupbox     : WidgetType::empty;
            widgets[WIDX_BIAS_TYPE].type          = state ? WidgetType::dropdownMenu : WidgetType::empty;
            widgets[WIDX_BIAS_TYPE_DROPDOWN].type = state ? WidgetType::button       : WidgetType::empty;
            widgets[WIDX_BIAS_STRENGTH].type      = state ? WidgetType::spinner      : WidgetType::empty;
            widgets[WIDX_BIAS_STRENGTH_UP].type   = state ? WidgetType::button       : WidgetType::empty;
            widgets[WIDX_BIAS_STRENGTH_DOWN].type = state ? WidgetType::button       : WidgetType::empty;
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
            DrawWidgets(rt);
            DrawTabImages(rt);

            if (_settings.algorithm == MapGenerator::Algorithm::simplexNoise
                || _settings.algorithm == MapGenerator::Algorithm::warpedNoise)
            {
                SimplexDraw(rt);
                BiasDraw(rt);
            }
            else if (_settings.algorithm == MapGenerator::Algorithm::heightmapImage)
            {
                HeightmapDraw(rt);
            }

            const auto enabledColour = colours[1];
            const auto disabledColour = enabledColour.withFlag(ColourFlag::inset, true);

            {
                auto textColour = IsWidgetDisabled(WIDX_MAP_SIZE_Y) ? disabledColour : enabledColour;
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_MAP_SIZE_Y].top + 1 }, STR_MAP_SIZE, {}, { textColour });
            }

            {
                auto textColour = enabledColour;
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_SOURCE].top + 1 }, STR_HEIGHTMAP_SOURCE, {},
                    { textColour });
            }

            if (_random_seed)
            {
                DrawText(
                    rt, windowPos + ScreenCoordsXY{ 179, widgets[WIDX_MAP_SEED].top + 1 }, { disabledColour }, _seed.data());
            }
        }

#pragma endregion

#pragma region Scenery page

        void SetSelectedSceneryRule(int32_t roleIdx)
        {
            if (roleIdx != _selectedScRule)
            {
                WindowTextInputCloseByCalling(this, WIDX_RULE_SC_RENAME);
                MapGenRuleScenerySelectCloseByCalling(this, WIDX_RULE_SC_OUTCOME_SELECT);
                SetSelectedSceneryCondition(-1);
                scrolls[1].contentOffsetY = 0;
                Invalidate();
            }
            _selectedScRule = roleIdx;
        }

        void SetSelectedSceneryCondition(int32_t conditionIdx)
        {
            if (conditionIdx != _selectedScCondition)
            {
                MapGenRuleConditionCloseByCalling(this, WIDX_RULE_SC_CONDITION_ADD);
                MapGenRuleConditionCloseByCalling(this, WIDX_RULE_SC_CONDITION_EDIT);
            }
            _selectedScCondition = conditionIdx;
        }

        void SceneryMouseUp(WidgetIndex widgetIndex)
        {
            SharedMouseUp(widgetIndex);

            switch (widgetIndex)
            {
                case WIDX_RULE_SC_NEW:
                {
                    MapGenerator::Rule::createNewSceneryRule(_settings);
                    SetSelectedSceneryRule(static_cast<int32_t>(_settings.sceneryRules.size() - 1));
                    Invalidate();
                    break;
                }
                case WIDX_RULE_SC_REMOVE:
                {
                    if (_selectedScRule != -1)
                    {
                        _settings.sceneryRules.erase(_settings.sceneryRules.begin() + _selectedScRule);
                        SetSelectedSceneryRule(_selectedScRule - 1);
                        Invalidate();
                    }
                    break;
                }
                case WIDX_RULE_SC_RENAME:
                {
                    if (_selectedScRule != -1)
                    {
                        auto& selectedRule = _settings.sceneryRules[_selectedScRule];
                        auto* qqq = selectedRule.name.c_str();
                        WindowTextInputOpen(
                            this, widgetIndex, STR_MAPGEN_RULE_RENAME_TITLE, STR_MAPGEN_RULE_ENTER_NAME, {}, STR_STRING,
                            reinterpret_cast<uintptr_t>(qqq), 32);
                    }
                    break;
                }
                case WIDX_RULE_SC_MOVE_UP:
                {
                    if (_selectedScRule != -1 && _selectedScRule < static_cast<int32_t>(_settings.sceneryRules.size() - 1))
                    {
                        std::swap(_settings.sceneryRules[_selectedScRule], _settings.sceneryRules[_selectedScRule + 1]);
                        SetSelectedSceneryRule(_selectedScRule + 1);
                        Invalidate();
                    }
                    break;
                }
                case WIDX_RULE_SC_MOVE_DOWN:
                {
                    if (_selectedScRule != -1 && _selectedScRule > 0)
                    {
                        std::swap(_settings.sceneryRules[_selectedScRule], _settings.sceneryRules[_selectedScRule - 1]);
                        SetSelectedSceneryRule(_selectedScRule - 1);
                        Invalidate();
                    }
                    break;
                }
                case WIDX_RULE_SC_CONDITION_REMOVE:
                {
                    if (_selectedScRule != -1 && _selectedScCondition != -1)
                    {
                        auto& selectedRule = _settings.sceneryRules[_selectedScRule];
                        selectedRule.conditions.erase(selectedRule.conditions.begin() + _selectedScCondition);
                        auto nextIdx = _selectedScCondition - 1;
                        if (nextIdx == -1 && !selectedRule.conditions.empty())
                        {
                            nextIdx = 0;
                        }
                        SetSelectedSceneryCondition(nextIdx);
                        Invalidate();
                    }
                    break;
                }
                case WIDX_RULE_SC_CONDITION_EDIT:
                {
                    if (_selectedScRule != -1 && _selectedScCondition != -1)
                    {
                        auto& selectedRule = _settings.sceneryRules[_selectedScRule];
                        auto& selectedCondition = selectedRule.conditions[_selectedScCondition];

                        auto callback = [this](MapGenerator::Rule::Condition& condition) {
                            if (_selectedScRule == -1 || _selectedScCondition == -1)
                            {
                                return;
                            }
                            _settings.sceneryRules[_selectedScRule].conditions[_selectedScCondition] = condition;
                            Invalidate();
                        };

                        MapGenRuleConditionOpen(this, WIDX_RULE_SC_CONDITION_ADD, selectedCondition, callback);
                    }
                    break;
                }
                case WIDX_RULE_SC_OUTCOME_SELECT:
                {
                    if (_selectedScRule != -1)
                    {
                        auto& selectedRule = _settings.sceneryRules[_selectedScRule];

                        auto callback = [this](MapGenerator::Rule::SceneryEffect& effect) {
                            if (_selectedScRule == -1)
                            {
                                return;
                            }
                            _settings.sceneryRules[_selectedScRule].effect = effect;
                            Invalidate();
                        };

                        MapGenRuleScenerySelectOpen(this, WIDX_RULE_SC_OUTCOME_SELECT, selectedRule.effect, callback);
                    }
                    break;
                }
            }
        }

        void SceneryMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_RULE_SC_NEW_PRESET:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        ItemExt(0, STR_STRINGID, STR_MAPGEN_RULE_PRESET_SC_MEDITERRANEAN),
                        ItemExt(1, STR_STRINGID, STR_MAPGEN_RULE_PRESET_SC_COLD),
                        ItemExt(2, STR_STRINGID, STR_MAPGEN_RULE_PRESET_SC_ARID),
                        ItemExt(3, STR_STRINGID, STR_MAPGEN_RULE_PRESET_SC_RAINFOREST),
                    };

                    SetItems(items);

                    Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        Dropdown::Flag::StayOpen, std::size(items));
                    break;
                }
                case WIDX_RULE_SC_CONDITION_ADD:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        ItemExt(0, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_ELEVATION_ABSOLUTE),
                        ItemExt(1, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_ELEVATION_RELATIVE_TO_WATER),
                        ItemExt(2, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_WATER),
                        ItemExt(3, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_NOISE),
                        ItemExt(4, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_NORMAL_ANGLE),
                        ItemExt(5, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_PRNG),
                        ItemExt(6, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_BLEND_HEIGHT),
                        ItemExt(7, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_BLEND_NOISE),
                        ItemExt(8, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_LAND_STYLE),
                    };

                    SetItems(items);

                    Widget* ddWidget = &widgets[widgetIndex];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        Dropdown::Flag::StayOpen, std::size(items));

                    // TODO implement
                    SetDisabled(4, true);

                    break;
                }
            }
        }

        void SceneryUpdate()
        {
            // Tab animation
            if (++frame_no >= TabAnimationLoops[page])
                frame_no = 0;
            InvalidateWidget(WIDX_TAB_4);

            // Check if the mouse is hovering over the list
            if (!widgetIsHighlighted(*this, WIDX_RULE_SC_SCROLL))
            {
                if (_highlightedScRule != -1)
                    InvalidateWidget(WIDX_RULE_SC_SCROLL);
                _highlightedScRule = -1;
            }

            // Check if the mouse is hovering over the list
            if (!widgetIsHighlighted(*this, WIDX_RULE_SC_CONDITION_SCROLL))
            {
                if (_highlightedScCondition != -1)
                    InvalidateWidget(WIDX_RULE_SC_CONDITION_SCROLL);
                _highlightedScCondition = -1;
            }
        }

        void SceneryTextInput(WidgetIndex widgetIndex, std::string_view& value)
        {
            switch (widgetIndex)
            {
                case WIDX_RULE_SC_RENAME:
                {
                    if (_selectedScRule == -1)
                    {
                        return;
                    }

                    // TODO trim + check not empty?

                    auto& selectedRule = _settings.sceneryRules[_selectedScRule];
                    selectedRule.name = std::string(value);
                    break;
                }
            }

            Invalidate();
        }

        void SceneryPrepareDraw()
        {
            bool ruleSelected = _selectedScRule != -1;
            bool condSelected = ruleSelected && _selectedScCondition != -1;

            widgets[WIDX_RULE_SC_MOVE_UP].type = ruleSelected ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_RULE_SC_MOVE_DOWN].type = ruleSelected ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_RULE_SC_REMOVE].type = ruleSelected ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_RULE_SC_RENAME].type = ruleSelected ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_RULE_SC_CONDITION_GROUP].type = ruleSelected ? WidgetType::groupbox : WidgetType::empty;
            widgets[WIDX_RULE_SC_CONDITION_SCROLL].type = ruleSelected ? WidgetType::scroll : WidgetType::empty;
            widgets[WIDX_RULE_SC_CONDITION_REMOVE].type = condSelected ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_RULE_SC_CONDITION_EDIT].type = condSelected ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_RULE_SC_CONDITION_ADD].type = ruleSelected ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_RULE_SC_OUTCOME_GROUP].type = ruleSelected ? WidgetType::groupbox : WidgetType::empty;
            widgets[WIDX_RULE_SC_OUTCOME_SELECT].type = ruleSelected ? WidgetType::button : WidgetType::empty;

            if (ruleSelected)
            {
                SetWidgetEnabled(
                    WIDX_RULE_SC_MOVE_UP, _selectedScRule < static_cast<int32_t>(_settings.sceneryRules.size() - 1));
                SetWidgetEnabled(WIDX_RULE_SC_MOVE_DOWN, _selectedScRule > 0);

                SetWidgetEnabled(WIDX_RULE_SC_CONDITION_REMOVE, condSelected);
                SetWidgetEnabled(WIDX_RULE_SC_CONDITION_EDIT, condSelected);
            }

            SetPressedTab();
        }

        void SceneryDraw(RenderTarget& rt)
        {
            DrawWidgets(rt);
            DrawTabImages(rt);

            if (_selectedScRule != -1)
            {
                auto& selectedRule = _settings.sceneryRules[_selectedScRule];

                ScreenCoordsXY sceneryPreviewPos = { windowPos.x + widgets[WIDX_RULE_SC_OUTCOME_GROUP].left + 4,
                                                     windowPos.y + widgets[WIDX_RULE_SC_OUTCOME_GROUP].top + 12 };
                ScreenCoordsXY sceneryPreviewOutcomeSize = { 73, 59 };
                SceneryDrawItems(rt, selectedRule, sceneryPreviewPos, sceneryPreviewOutcomeSize);
            }
            else
            {
                auto& ruleScroll = widgets[WIDX_RULE_SC_SCROLL];
                auto centrePos = windowPos + ScreenCoordsXY{ ruleScroll.left, ruleScroll.bottom + 5 };
                auto textPaint = TextPaint{ colours[1] };
                DrawTextWrapped(rt, centrePos, ruleScroll.width(), STR_MAPGEN_RULE_SC_HINT, {}, textPaint);
            }
        }

        void SceneryDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_RULE_SC_NEW_PRESET:
                {
                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdownHighlightedIndex;

                    if (dropdownIndex != -1)
                    {
                        auto preset = static_cast<MapGenerator::Rule::SceneryRulePreset>(dropdownIndex);
                        MapGenerator::Rule::createNewSceneryRuleFromPreset(_settings, preset);
                        SetSelectedSceneryRule(static_cast<int32_t>(_settings.sceneryRules.size() - 1));
                    }

                    Invalidate();
                    break;
                }
                case WIDX_RULE_SC_CONDITION_ADD:
                {
                    if (_selectedScRule == -1)
                    {
                        return;
                    }

                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdownHighlightedIndex;

                    if (dropdownIndex == -1)
                    {
                        return;
                    }

                    auto conditionType = static_cast<MapGenerator::Rule::Type>(dropdownIndex);
                    auto newCondition = createNewCondition(conditionType);
                    auto callback = [this](MapGenerator::Rule::Condition& condition) {
                        if (_selectedScRule == -1)
                        {
                            return;
                        }
                        auto& selectedRule = _settings.sceneryRules[_selectedScRule];
                        selectedRule.conditions.push_back(std::move(condition));
                        SetSelectedSceneryCondition(static_cast<int32_t>(selectedRule.conditions.size() - 1));
                        Invalidate();
                    };

                    MapGenRuleConditionOpen(this, WIDX_RULE_SC_CONDITION_ADD, newCondition, callback);
                    break;
                }
            }
        }

        ScreenSize SceneryScrollGetSize(int32_t scrollIndex)
        {
            switch (scrollIndex)
            {
                case 0:
                {
                    return ScreenSize(
                        kWindowSize.width - 10, static_cast<int32_t>(_settings.sceneryRules.size() * kScrollableRowHeight));
                }
                case 1:
                {
                    if (_selectedScRule == -1)
                    {
                        return {};
                    }
                    auto& selectedRule = _settings.sceneryRules[_selectedScRule];

                    return ScreenSize(
                        widgets[WIDX_RULE_SC_CONDITION_SCROLL].width(),
                        static_cast<int32_t>(selectedRule.conditions.size() * kScrollableRowHeight));
                }
            }
            return {};
        }

        void SceneryDrawItem(
            RenderTarget& rt, const MapGenerator::Rule::SceneryEffectItem& item, const ScreenCoordsXY& targetSize, size_t idx,
            size_t count)
        {
            uint8_t direction = item.direction.value_or(0);

            auto sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(item.index);
            auto imageId = ImageId(sceneryEntry->image + direction);

            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
            {
                imageId = imageId.WithPrimary(item.colours[0]);
            }
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
            {
                imageId = imageId.WithSecondary(item.colours[1]);
            }
            if (sceneryEntry->flags & SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
            {
                imageId = imageId.WithTertiary(item.colours[2]);
            }

            auto spriteTop = (targetSize.y / 2) + (sceneryEntry->height / 2);
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE) && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
            {
                spriteTop -= 12;
            }

            if (item.weight == 0)
            {
                imageId = imageId.WithRemap(FilterPaletteID::PaletteGhost);
            }

            auto spriteOffsetX = static_cast<int32_t>(targetSize.x / (count + 1) * (idx + 1));
            auto spritePosition = ScreenCoordsXY{ spriteOffsetX, spriteTop };

            GfxDrawSprite(rt, imageId, spritePosition);

            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_GLASS))
            {
                imageId = ImageId(sceneryEntry->image + 4 + direction).WithTransparency(item.colours[0]);
                GfxDrawSprite(rt, imageId, spritePosition);
            }

            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ANIMATED_FG))
            {
                imageId = ImageId(sceneryEntry->image + 4 + direction);
                GfxDrawSprite(rt, imageId, spritePosition);
            }
        }

        void SceneryDrawItems(
            RenderTarget& rt, MapGenerator::Rule::SceneryRule& rule, ScreenCoordsXY& pos, ScreenCoordsXY& size)
        {
            RenderTarget clippedRT;
            if (ClipDrawPixelInfo(clippedRT, rt, pos, size.x, size.y))
            {
                size_t limitedItemCount = std::min<size_t>(4, rule.effect.objects.size());
                for (size_t i = 0; i < limitedItemCount; ++i)
                {
                    auto& item = rule.effect.objects[i];
                    SceneryDrawItem(clippedRT, item, size, i, limitedItemCount);
                }
            }
        }

        void SceneryScrollDraw(int32_t scrollIndex, RenderTarget& rt)
        {
            switch (scrollIndex)
            {
                case 0:
                    return SceneryScrollDrawRule(rt);
                case 1:
                    return SceneryScrollDrawCondition(rt);
            }
        }

        void SceneryScrollDrawCondition(RenderTarget& rt)
        {
            if (_selectedScRule == -1)
            {
                return;
            }
            auto& selectedRule = _settings.sceneryRules[_selectedScRule];

            const int32_t listWidth = widgets[WIDX_RULE_SC_CONDITION_SCROLL].width();
            GfxFillRect(
                rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } },
                ColourMapA[colours[1].colour].mid_light | 0x1000000);

            ScreenCoordsXY screenCoords{};
            screenCoords.y = static_cast<int32_t>(kScrollableRowHeight * (selectedRule.conditions.size() - 1));

            for (int32_t i = 0; i < static_cast<int32_t>(selectedRule.conditions.size()); i++)
            {
                auto& condition = selectedRule.conditions[i];

                // Draw row background colour
                auto fillRectangle = ScreenRect{ { 0, screenCoords.y },
                                                 { listWidth, screenCoords.y + kScrollableRowHeight - 1 } };

                StringId stringFormat = STR_WINDOW_COLOUR_2_STRINGID;
                if (i == _selectedScCondition)
                {
                    GfxFillRect(rt, fillRectangle, ColourMapA[colours[1].colour].mid_dark);
                    stringFormat = STR_WHITE_STRING;
                }
                else if (i == _highlightedScCondition)
                {
                    GfxFillRect(rt, fillRectangle, ColourMapA[colours[1].colour].mid_dark | 0x1000000);
                    stringFormat = STR_WHITE_STRING;
                }

                auto checkboxFormatter = Formatter();
                checkboxFormatter.Add<StringId>(STR_STRING);
                checkboxFormatter.Add<char*>(kCheckMarkString);

                // Draw enabled checkbox and check
                GfxFillRectInset(rt, { { 2, screenCoords.y }, { 15, screenCoords.y + 11 } }, colours[1], INSET_RECT_F_60);
                if (condition.enabled)
                {
                    DrawTextBasic(rt, screenCoords + ScreenCoordsXY{ 4, 1 }, stringFormat, checkboxFormatter);
                }

                // Draw land styles
                if (condition.type == MapGenerator::Rule::Type::LandStyle)
                {
                    auto& landStyleData = std::get<MapGenerator::Rule::LandStyleData>(condition.data);
                    std::vector<ObjectEntryIndex> orderedStyles;
                    std::ranges::copy(landStyleData.styles, std::back_inserter(orderedStyles));
                    std::ranges::sort(orderedStyles);

                    ImageId maskImage(SPR_G2_MASK_MAPGEN_TEXTURE_SCROLL);
                    auto styleCount = std::min<size_t>(4, orderedStyles.size());
                    auto styleWidth = 24;
                    auto baseOffset = listWidth - kScrollBarWidth - styleCount * styleWidth;

                    for (size_t s = 0; s < styleCount; ++s)
                    {
                        auto offset = static_cast<int32_t>(baseOffset + s * styleWidth);
                        auto imgCoords = screenCoords + ScreenCoordsXY{ offset, 0 };
                        auto surfaceImage = LookupSurfaceImage(orderedStyles[s]);
                        GfxDrawSpriteRawMasked(rt, imgCoords, maskImage, surfaceImage);
                    }
                }

                StringId predRepr = kStringIdNone;
                switch (condition.predicate)
                {
                    case MapGenerator::Rule::Predicate::Equal:
                        predRepr = STR_MAPGEN_RULE_PREDICATE_EQUAL;
                        break;
                    case MapGenerator::Rule::Predicate::NotEqual:
                        predRepr = STR_MAPGEN_RULE_PREDICATE_NOT_EQUAL;
                        break;
                    case MapGenerator::Rule::Predicate::LessThan:
                        predRepr = STR_MAPGEN_RULE_PREDICATE_LESS_THAN;
                        break;
                    case MapGenerator::Rule::Predicate::GreaterThan:
                        predRepr = STR_MAPGEN_RULE_PREDICATE_GREATER_THAN;
                        break;
                    case MapGenerator::Rule::Predicate::LessThanOrEqual:
                        predRepr = STR_MAPGEN_RULE_PREDICATE_LESS_THAN_OR_EQUAL;
                        break;
                    case MapGenerator::Rule::Predicate::GreaterThanOrEqual:
                        predRepr = STR_MAPGEN_RULE_PREDICATE_GREATER_THAN_OR_EQUAL;
                        break;
                }

                auto ft = Formatter();
                switch (condition.type)
                {
                    case MapGenerator::Rule::Type::HeightAbsolute:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_ELEVATION_ABSOLUTE);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int16_t>(static_cast<int16_t>(
                            BaseZToMetres(std::get<MapGenerator::Rule::HeightData>(condition.data).height)));
                        break;

                    case MapGenerator::Rule::Type::HeightRelativeToWater:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_ELEVATION_RELATIVE_TO_WATER);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int16_t>(static_cast<int16_t>(
                            HeightUnitsToMetres(std::get<MapGenerator::Rule::HeightData>(condition.data).height)));
                        break;

                    case MapGenerator::Rule::Type::DistanceToWater:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_WATER);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int16_t>(
                            static_cast<int16_t>(std::get<MapGenerator::Rule::DistanceData>(condition.data).distance));
                        break;

                    case MapGenerator::Rule::Type::Noise:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_NOISE);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int32_t>(
                            static_cast<int32_t>(std::get<MapGenerator::Rule::NoiseData>(condition.data).value * 100));
                        break;

                    case MapGenerator::Rule::Type::NormalAngle:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_NORMAL_ANGLE);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int32_t>(
                            static_cast<int32_t>(std::get<MapGenerator::Rule::NormalAngleData>(condition.data).angle * 100));
                        break;

                    case MapGenerator::Rule::Type::Random:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int32_t>(
                            static_cast<int32_t>(std::get<MapGenerator::Rule::RandomData>(condition.data).value * 100));
                        break;

                    case MapGenerator::Rule::Type::BlendHeight:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_BLEND_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_BLEND_HEIGHT);
                        ft.Add<int16_t>(static_cast<int16_t>(
                            BaseZToMetres(std::get<MapGenerator::Rule::BlendHeightData>(condition.data).edgeLow)));
                        ft.Add<int16_t>(static_cast<int16_t>(
                            BaseZToMetres(std::get<MapGenerator::Rule::BlendHeightData>(condition.data).edgeHigh)));
                        ft.Add<StringId>(predRepr);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        break;

                    case MapGenerator::Rule::Type::BlendNoise:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_BLEND_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_BLEND_NOISE);
                        ft.Add<int32_t>(
                            static_cast<int32_t>(std::get<MapGenerator::Rule::BlendNoiseData>(condition.data).edgeLow * 100));
                        ft.Add<int32_t>(
                            static_cast<int32_t>(std::get<MapGenerator::Rule::BlendNoiseData>(condition.data).edgeHigh * 100));
                        ft.Add<StringId>(predRepr);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        break;

                    case MapGenerator::Rule::Type::LandStyle:
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_LAND_STYLE_FMT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_LAND_STYLE);
                        ft.Add<StringId>(
                            condition.predicate == MapGenerator::Rule::Predicate::Equal ? STR_MAPGEN_RULE_PREDICATE_IN
                                                                                        : STR_MAPGEN_RULE_PREDICATE_NOT_IN);
                        break;
                }

                // Draw description
                DrawTextEllipsised(rt, screenCoords + ScreenCoordsXY{ 20, 0 }, 190, stringFormat, ft);

                screenCoords.y -= kScrollableRowHeight;
            }
        }

        void SceneryScrollDrawRule(RenderTarget& rt)
        {
            const int32_t listWidth = widgets[WIDX_RULE_SC_SCROLL].width();
            GfxFillRect(
                rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } }, ColourMapA[colours[1].colour].mid_light);

            ScreenCoordsXY screenCoords{};
            screenCoords.y = static_cast<int32_t>(kScrollableRowHeight * (_settings.sceneryRules.size() - 1));

            for (int32_t i = 0; i < static_cast<int32_t>(_settings.sceneryRules.size()); i++)
            {
                auto& rule = _settings.sceneryRules[i];

                // Draw row background colour
                auto fillRectangle = ScreenRect{ { 0, screenCoords.y },
                                                 { listWidth, screenCoords.y + kScrollableRowHeight - 1 } };

                StringId stringFormat = STR_WINDOW_COLOUR_2_STRINGID;
                if (i == _selectedScRule)
                {
                    GfxFillRect(rt, fillRectangle, ColourMapA[colours[1].colour].mid_dark);
                    stringFormat = STR_WHITE_STRING;
                }
                else if (i == _highlightedScRule)
                {
                    GfxFillRect(rt, fillRectangle, ColourMapA[colours[1].colour].mid_dark | 0x1000000);
                    stringFormat = STR_WHITE_STRING;
                }

                auto checkboxFormatter = Formatter();
                checkboxFormatter.Add<StringId>(STR_STRING);
                checkboxFormatter.Add<char*>(kCheckMarkString);

                // Draw enabled checkbox and check
                GfxFillRectInset(rt, { { 2, screenCoords.y }, { 15, screenCoords.y + 11 } }, colours[1], INSET_RECT_F_E0);
                if (rule.enabled)
                {
                    DrawTextBasic(rt, screenCoords + ScreenCoordsXY{ 4, 1 }, stringFormat, checkboxFormatter);
                }

                // Rule name
                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<char*>(rule.name.c_str());
                DrawTextEllipsised(rt, screenCoords + ScreenCoordsXY{ 20, 0 }, 190, stringFormat, ft);

                // Preview
                ScreenCoordsXY sceneryPreviewPos = screenCoords + ScreenCoordsXY{ 208, 0 };
                ScreenCoordsXY sceneryPreviewOutcomeSize = { 80, kScrollableRowHeight };
                SceneryDrawItems(rt, rule, sceneryPreviewPos, sceneryPreviewOutcomeSize);

                screenCoords.y -= kScrollableRowHeight;
            }
        }

        void SceneryScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
            if (scrollIndex == 0)
            {
                int32_t index = static_cast<int32_t>(
                    _settings.sceneryRules.size() - (screenCoords.y - 1) / kScrollableRowHeight - 1);
                if (index < 0 || index >= static_cast<int32_t>(_settings.sceneryRules.size()))
                {
                    _highlightedScRule = -1;
                }
                else
                {
                    _highlightedScRule = index;
                }
                InvalidateWidget(WIDX_RULE_SC_SCROLL);
            }
            else if (scrollIndex == 1)
            {
                if (_selectedScRule == -1)
                {
                    _highlightedScCondition = -1;
                }
                else
                {
                    auto& selectedRule = _settings.sceneryRules[_selectedScRule];

                    int32_t index = static_cast<int32_t>(
                        selectedRule.conditions.size() - (screenCoords.y - 1) / kScrollableRowHeight - 1);
                    if (index < 0 || index >= static_cast<int32_t>(selectedRule.conditions.size()))
                    {
                        _highlightedScCondition = -1;
                    }
                    else
                    {
                        _highlightedScCondition = index;
                    }
                }
                InvalidateWidget(WIDX_RULE_SC_CONDITION_SCROLL);
            }
        }

        void SceneryScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
            if (_settings.sceneryRules.empty())
                return;

            if (scrollIndex == 0)
            {
                // Because the list items are displayed in reverse order, subtract the calculated index from size
                const int32_t index = static_cast<int32_t>(
                    _settings.sceneryRules.size() - (screenCoords.y - 1) / kScrollableRowHeight - 1);
                const ScreenRect checkboxColumnRect{ { 2, 0 }, { 15, screenCoords.y } };
                if (index >= 0 && checkboxColumnRect.Contains(screenCoords))
                { // Checkbox was clicked
                    _settings.sceneryRules[index].enabled = !_settings.sceneryRules[index].enabled;
                }
                else if (index >= 0 && index < static_cast<int32_t>(_settings.sceneryRules.size()))
                {
                    SetSelectedSceneryRule(index);
                }
                else
                {
                    SetSelectedSceneryRule(-1);
                }
            }
            else if (scrollIndex == 1)
            {
                if (_selectedScRule == -1)
                {
                    SetSelectedSceneryCondition(-1);
                }
                else
                {
                    auto& selectedRule = _settings.sceneryRules[_selectedScRule];
                    // Because the list items are displayed in reverse order, subtract the calculated index from size
                    const int32_t index = static_cast<int32_t>(
                        selectedRule.conditions.size() - (screenCoords.y - 1) / kScrollableRowHeight - 1);
                    const ScreenRect checkboxColumnRect{ { 2, 0 }, { 15, screenCoords.y } };
                    if (index >= 0 && checkboxColumnRect.Contains(screenCoords))
                    { // Checkbox was clicked
                        selectedRule.conditions[index].enabled = !selectedRule.conditions[index].enabled;
                    }
                    else if (index >= 0 && index < static_cast<int32_t>(selectedRule.conditions.size()))
                    {
                        SetSelectedSceneryCondition(index);
                    }
                    else
                    {
                        SetSelectedSceneryCondition(-1);
                    }
                }
            }
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

        void SimplexDraw(RenderTarget& rt)
        {
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

#pragma region bias settings, part of generator tab

        void BiasMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_BIAS_STRENGTH:
                {
                    Formatter ft;
                    ft.Add<int32_t>(0);
                    ft.Add<int32_t>(100);
                    WindowTextInputOpen(
                        this, widgetIndex, STR_MAPGEN_BIAS_STRENGTH, STR_ENTER_BIAS_STRENGTH, ft, STR_FORMAT_COMMA2DP32,
                        _settings.bias_strength, 4);
                    break;
                }
            }
        }

        void BiasMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_BIAS_STRENGTH_UP:
                    _settings.bias_strength = std::min(_settings.bias_strength + 5, 100);
                    Invalidate();
                    break;
                case WIDX_BIAS_STRENGTH_DOWN:
                    _settings.bias_strength = std::max(_settings.bias_strength - 5, 0);
                    Invalidate();
                    break;
                case WIDX_BIAS_TYPE_DROPDOWN:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        // clang-format off
                        ToggleOption(0, STR_MAPGEN_BIAS_TYPE_NONE),
                        ToggleOption(1, STR_MAPGEN_BIAS_TYPE_ISLAND),
                        ToggleOption(2, STR_MAPGEN_BIAS_TYPE_VALLEY),
                        ToggleOption(3, STR_MAPGEN_BIAS_TYPE_COASTAL),
                        ToggleOption(4, STR_MAPGEN_BIAS_TYPE_RIVER),
                        ToggleOption(5, STR_MAPGEN_BIAS_TYPE_CANYON),
                        ToggleOption(7, STR_MAPGEN_BIAS_TYPE_MOUNTAIN),
                        ToggleOption(8, STR_MAPGEN_BIAS_TYPE_CLIFF),
                        //clang-format on
                    };

                    SetItems(items);

                    Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1], 0,
                        Flag::StayOpen, std::size(items), ddWidget->width() - 2);

                    SetChecked(EnumValue(_settings.bias), true);
                    break;
                }
            }
        }

        void BiasDraw(RenderTarget& rt)
        {
            const auto enabledColour = colours[1];
            const auto disabledColour = enabledColour.withFlag(ColourFlag::inset, true);
            const bool strengthDisabled = _settings.bias == MapGenerator::Bias::none;
            const auto strengthColour = strengthDisabled ? disabledColour : enabledColour;

            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_BIAS_TYPE].top + 1 }, STR_MAPGEN_BIAS_TYPE, {},
                { enabledColour });

            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_BIAS_STRENGTH].top + 1 }, STR_MAPGEN_BIAS_STRENGTH, {},
                { strengthColour });

            auto ft = Formatter();
            ft.Add<uint16_t>(_settings.bias_strength);
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_BIAS_STRENGTH].left + 1, widgets[WIDX_BIAS_STRENGTH].top + 1 },
                STR_COMMA2DP32, ft, { strengthColour });
        }

        void BiasTextInput(WidgetIndex widgetIndex, int32_t value)
        {
            switch (widgetIndex)
            {
                case WIDX_BIAS_STRENGTH:
                    _settings.bias_strength = std::clamp(value, 0, 100);
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
                    InvalidateWidget(WIDX_HEIGHTMAP_STRENGTH);
                    break;
                case WIDX_HEIGHTMAP_STRENGTH_DOWN:
                    _settings.smooth_strength = std::max<uint32_t>(_settings.smooth_strength - 1, 1);
                    InvalidateWidget(WIDX_HEIGHTMAP_STRENGTH);
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
                    auto intent = Intent(WindowClass::Loadsave);
                    intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::load);
                    intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::heightmap);
                    intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(HeightmapLoadsaveCallback));
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
            SetCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP, _settings.smooth_height_map);
            SetCheckboxValue(WIDX_HEIGHTMAP_NORMALIZE, _settings.normalize_height);
        }

        void HeightmapDraw(RenderTarget& rt)
        {
            const auto enabledColour = colours[1];
            const auto disabledColour = enabledColour.withFlag(ColourFlag::inset, true);

            // Smooth strength label and value
            const bool strengthDisabled = IsWidgetDisabled(WIDX_HEIGHTMAP_STRENGTH) || !_settings.smooth_height_map;
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
                    SetCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES, _settings.smoothTileEdges);
                    InvalidateWidget(WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES);
                    break;

                case WIDX_HEIGHTMAP_EROSION:
                    _settings.simulate_erosion = !_settings.simulate_erosion;
                    SetCheckboxValue(WIDX_HEIGHTMAP_EROSION, _settings.simulate_erosion);
                    InvalidateWidget(WIDX_HEIGHTMAP_EROSION);
                    break;

                case WIDX_HEIGHTMAP_EROSION_PPT:
                    Formatter ft;
                    ft.Add<int32_t>(MapGenerator::kMinParticlesPerTile);
                    ft.Add<int32_t>(MapGenerator::kMaxParticlesPerTile);
                    WindowTextInputOpen(
                        this, widgetIndex, STR_EROSION_PPT, STR_ENTER_EROSION_PPT, ft, STR_FORMAT_COMMA2DP32,
                        _settings.particles_per_tile, 4);
                    break;
            }
        }

        void TerrainMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_HEIGHTMAP_LOW_UP:
                    _settings.heightmapLow = std::min(_settings.heightmapLow + 2, kMaximumLandHeight / 2 - 1);
                    _settings.heightmapHigh = std::max(_settings.heightmapHigh, _settings.heightmapLow + 2);
                    InvalidateWidget(WIDX_HEIGHTMAP_LOW);
                    break;
                case WIDX_HEIGHTMAP_LOW_DOWN:
                    _settings.heightmapLow = std::max<int32_t>(_settings.heightmapLow - 2, kMinimumLandHeight);
                    InvalidateWidget(WIDX_HEIGHTMAP_LOW);
                    break;
                case WIDX_HEIGHTMAP_HIGH_UP:
                    _settings.heightmapHigh = std::min<int32_t>(_settings.heightmapHigh + 2, kMaximumLandHeight - 1);
                    InvalidateWidget(WIDX_HEIGHTMAP_HIGH);
                    break;
                case WIDX_HEIGHTMAP_HIGH_DOWN:
                    _settings.heightmapHigh = std::max<int32_t>(_settings.heightmapHigh - 2, kMinimumLandHeight);
                    _settings.heightmapLow = std::min(_settings.heightmapLow, _settings.heightmapHigh - 2);
                    InvalidateWidget(WIDX_HEIGHTMAP_HIGH);
                    break;
                case WIDX_HEIGHTMAP_EROSION_PPT_DOWN:
                    _settings.particles_per_tile = std::max<int32_t>(
                        _settings.particles_per_tile - 5, MapGenerator::kMinParticlesPerTile);
                    InvalidateWidget(WIDX_HEIGHTMAP_EROSION_PPT);
                    break;
                case WIDX_HEIGHTMAP_EROSION_PPT_UP:
                    _settings.particles_per_tile = std::min<int32_t>(
                        _settings.particles_per_tile + 5, MapGenerator::kMaxParticlesPerTile);
                    InvalidateWidget(WIDX_HEIGHTMAP_EROSION_PPT);
                    break;
            }
        }

        void TerrainUpdate()
        {
            // Tab animation
            if (++frame_no >= TabAnimationLoops[page])
                frame_no = 0;
            InvalidateWidget(WIDX_TAB_2);
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
                case WIDX_HEIGHTMAP_EROSION_PPT:
                    _settings.particles_per_tile = std::clamp(
                        value, MapGenerator::kMinParticlesPerTile, MapGenerator::kMaxParticlesPerTile);
                    break;
            }

            Invalidate();
        }

        void TerrainDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
        }

        void DrawDropdownButton(RenderTarget& rt, WidgetIndex widgetIndex, ImageId image)
        {
            const auto& widget = widgets[widgetIndex];
            ScreenCoordsXY pos = { windowPos.x + widget.left, windowPos.y + widget.top };
            if (IsWidgetDisabled(widgetIndex))
            {
                // Draw greyed out (light border bottom right shadow)
                auto colour = colours[widget.colour].colour;
                colour = ColourMapA[colour].lighter;
                GfxDrawSpriteSolid(rt, image, pos + ScreenCoordsXY{ 1, 1 }, colour);

                // Draw greyed out (dark)
                colour = colours[widget.colour].colour;
                colour = ColourMapA[colour].mid_light;
                GfxDrawSpriteSolid(rt, image, pos, colour);
            }
            else
            {
                GfxDrawSprite(rt, image, pos);
            }
        }

        ImageId LookupSurfaceImage(uint32_t surfaceTexture)
        {
            auto& objManager = GetContext()->GetObjectManager();
            const auto* surfaceObj = objManager.GetLoadedObject<TerrainSurfaceObject>(surfaceTexture);
            ImageId surfaceImage;
            if (surfaceObj != nullptr)
            {
                surfaceImage = ImageId(surfaceObj->IconImageId);
                if (surfaceObj->Colour != TerrainSurfaceObject::kNoValue)
                {
                    surfaceImage = surfaceImage.WithPrimary(surfaceObj->Colour);
                }
            }
            return surfaceImage;
        }

        ImageId LookupEdgeImage(uint32_t edgeTexture)
        {
            auto& objManager = GetContext()->GetObjectManager();
            ImageId edgeImage;
            const auto* edgeObj = objManager.GetLoadedObject<TerrainEdgeObject>(edgeTexture);
            if (edgeObj != nullptr)
            {
                edgeImage = ImageId(edgeObj->IconImageId);
            }
            return edgeImage;
        }

        void TerrainPrepareDraw()
        {
            bool isNotFlatland = _settings.algorithm != MapGenerator::Algorithm::blank;

            SetCheckboxValue(WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES, _settings.smoothTileEdges);
            SetCheckboxValue(WIDX_HEIGHTMAP_EROSION, _settings.simulate_erosion);

            // Max land height option is irrelevant for flatland
            SetWidgetEnabled(WIDX_HEIGHTMAP_HIGH, isNotFlatland);

            // Only offer terrain edge smoothing if we don't use flatland terrain
            SetWidgetEnabled(WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES, isNotFlatland);

            // Erosion can't be used with flatland
            SetWidgetEnabled(WIDX_HEIGHTMAP_EROSION, isNotFlatland);
            SetWidgetEnabled(WIDX_HEIGHTMAP_EROSION_PPT, _settings.simulate_erosion && isNotFlatland);

            SetPressedTab();
        }

        void TerrainDraw(RenderTarget& rt)
        {
            DrawWidgets(rt);
            DrawTabImages(rt);

            const auto enabledColour = colours[1];
            const auto disabledColour = enabledColour.withFlag(ColourFlag::inset, true);

            // Minimum land height label and value
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_LOW].top + 1 }, STR_MAPGEN_MIN_LAND_HEIGHT, {},
                { enabledColour });

            auto ft = Formatter();
            ft.Add<int32_t>(BaseZToMetres(_settings.heightmapLow));
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_LOW].left + 1, widgets[WIDX_HEIGHTMAP_LOW].top + 1 },
                STR_RIDE_LENGTH_ENTRY, ft, { enabledColour });

            const auto maxLandColour = IsWidgetDisabled(WIDX_HEIGHTMAP_HIGH) ? disabledColour : enabledColour;

            // Maximum land height label and value
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_HIGH].top + 1 }, STR_MAPGEN_MAX_LAND_HEIGHT, {},
                { maxLandColour });

            ft = Formatter();
            ft.Add<int32_t>(BaseZToMetres(_settings.heightmapHigh));
            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_HIGH].left + 1, widgets[WIDX_HEIGHTMAP_HIGH].top + 1 },
                STR_RIDE_LENGTH_ENTRY, ft, { maxLandColour });

            // Erosion particle count label and value
            const auto particleColour = IsWidgetDisabled(WIDX_HEIGHTMAP_EROSION) || !_settings.simulate_erosion ? disabledColour
                                                                                                                : enabledColour;

            DrawTextBasic(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_EROSION_PPT].top + 1 }, STR_EROSION_PPT, {},
                { particleColour });

            ft = Formatter();
            ft.Add<int32_t>(_settings.particles_per_tile);
            DrawTextBasic(
                rt,
                windowPos
                    + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_EROSION_PPT].left + 1,
                                      widgets[WIDX_HEIGHTMAP_EROSION_PPT].top + 1 },
                STR_FORMAT_COMMA2DP32, ft, { particleColour });
        }

#pragma endregion

#pragma region Texture page

        void SetSelectedTextureRule(int32_t roleIdx)
        {
            if (roleIdx != _selectedTxRule)
            {
                WindowTextInputCloseByCalling(this, WIDX_RULE_TX_RENAME);
                SetSelectedTextureCondition(-1);
                scrolls[1].contentOffsetY = 0;
                Invalidate();
            }
            _selectedTxRule = roleIdx;
        }

        void SetSelectedTextureCondition(int32_t conditionIdx)
        {
            if (conditionIdx != _selectedTxCondition)
            {
                MapGenRuleConditionCloseByCalling(this, WIDX_RULE_TX_CONDITION_ADD);
                MapGenRuleConditionCloseByCalling(this, WIDX_RULE_TX_CONDITION_EDIT);
            }
            _selectedTxCondition = conditionIdx;
        }

        void TextureMouseUp(WidgetIndex widgetIndex)
        {
            SharedMouseUp(widgetIndex);

            switch (widgetIndex)
            {
                case WIDX_RULE_TX_FLOOR_TEXTURE_CHECK:
                {
                    if (_selectedTxRule != -1)
                    {
                        auto& selectedRule = _settings.textureRules[_selectedTxRule];
                        selectedRule.effect.applyLandTexture = !selectedRule.effect.applyLandTexture;
                        Invalidate();
                    }
                    break;
                }
                case WIDX_RULE_TX_WALL_TEXTURE_CHECK:
                {
                    if (_selectedTxRule != -1)
                    {
                        auto& selectedRule = _settings.textureRules[_selectedTxRule];
                        selectedRule.effect.applyEdgeTexture = !selectedRule.effect.applyEdgeTexture;
                        Invalidate();
                    }
                    break;
                }
                case WIDX_RULE_TX_NEW:
                {
                    MapGenerator::Rule::createNewTextureRule(_settings);
                    SetSelectedTextureRule(static_cast<int32_t>(_settings.textureRules.size() - 1));
                    Invalidate();
                    break;
                }
                case WIDX_RULE_TX_REMOVE:
                {
                    if (_selectedTxRule != -1 && !_settings.textureRules[_selectedTxRule].isDefault)
                    {
                        _settings.textureRules.erase(_settings.textureRules.begin() + _selectedTxRule);
                        SetSelectedTextureRule(_selectedTxRule - 1);
                        Invalidate();
                    }
                    break;
                }
                case WIDX_RULE_TX_RENAME:
                {
                    if (_selectedTxRule != -1 && !_settings.textureRules[_selectedTxRule].isDefault)
                    {
                        auto& selectedRule = _settings.textureRules[_selectedTxRule];
                        auto* qqq = selectedRule.name.c_str();
                        WindowTextInputOpen(
                            this, widgetIndex, STR_MAPGEN_RULE_RENAME_TITLE, STR_MAPGEN_RULE_ENTER_NAME, {}, STR_STRING,
                            reinterpret_cast<uintptr_t>(qqq), 32);
                    }
                    break;
                }
                case WIDX_RULE_TX_MOVE_UP:
                {
                    if (_selectedTxRule != -1 && !_settings.textureRules[_selectedTxRule].isDefault
                        && _selectedTxRule < static_cast<int32_t>(_settings.textureRules.size() - 1))
                    {
                        std::swap(_settings.textureRules[_selectedTxRule], _settings.textureRules[_selectedTxRule + 1]);
                        SetSelectedTextureRule(_selectedTxRule + 1);
                        Invalidate();
                    }
                    break;
                }
                case WIDX_RULE_TX_MOVE_DOWN:
                {
                    if (_selectedTxRule != -1 && !_settings.textureRules[_selectedTxRule].isDefault && _selectedTxRule > 1)
                    {
                        std::swap(_settings.textureRules[_selectedTxRule], _settings.textureRules[_selectedTxRule - 1]);
                        SetSelectedTextureRule(_selectedTxRule - 1);
                        Invalidate();
                    }
                    break;
                }
                case WIDX_RULE_TX_CONDITION_REMOVE:
                {
                    if (_selectedTxRule != -1 && !_settings.textureRules[_selectedTxRule].isDefault && _selectedTxCondition != -1)
                    {
                        auto& selectedRule = _settings.textureRules[_selectedTxRule];
                        selectedRule.conditions.erase(selectedRule.conditions.begin() + _selectedTxCondition);
                        auto nextIdx = _selectedTxCondition - 1;
                        if (nextIdx == -1 && !selectedRule.conditions.empty())
                        {
                            nextIdx = 0;
                        }
                        SetSelectedTextureCondition(nextIdx);
                        Invalidate();
                    }
                    break;
                }
                case WIDX_RULE_TX_CONDITION_EDIT:
                {
                    if (_selectedTxRule != -1 && !_settings.textureRules[_selectedTxRule].isDefault && _selectedTxCondition != -1)
                    {
                        auto& selectedRule = _settings.textureRules[_selectedTxRule];
                        auto& selectedCondition = selectedRule.conditions[_selectedTxCondition];

                        auto callback = [this](MapGenerator::Rule::Condition& condition) {
                            if (_selectedTxRule == -1 || _settings.textureRules[_selectedTxRule].isDefault
                                || _selectedTxCondition == -1)
                            {
                                return;
                            }
                            _settings.textureRules[_selectedTxRule].conditions[_selectedTxCondition] = condition;
                            Invalidate();
                        };

                        MapGenRuleConditionOpen(this, WIDX_RULE_TX_CONDITION_ADD, selectedCondition, callback);
                    }
                    break;
                }
            }
        }

        void TextureMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_RULE_TX_FLOOR_TEXTURE:
                {
                    if (_selectedTxRule == -1)
                    {
                        return;
                    }
                    auto& selectedRule = _settings.textureRules[_selectedTxRule];
                    LandTool::ShowSurfaceStyleDropdown(this, widget, selectedRule.effect.landTexture);
                    break;
                }
                case WIDX_RULE_TX_WALL_TEXTURE:
                {
                    if (_selectedTxRule == -1)
                    {
                        return;
                    }
                    auto& selectedRule = _settings.textureRules[_selectedTxRule];
                    LandTool::ShowEdgeStyleDropdown(this, widget, selectedRule.effect.edgeTexture);
                    break;
                }
                case WIDX_RULE_TX_NEW_PRESET:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        ItemExt(0, STR_STRINGID, STR_MAPGEN_RULE_PRESET_TX_SMALL_ROCK_PATCHES),
                        ItemExt(1, STR_STRINGID, STR_MAPGEN_RULE_PRESET_TX_MEDIUM_DIRT_PATCHES),
                        ItemExt(2, STR_STRINGID, STR_MAPGEN_RULE_PRESET_TX_LARGE_GRASS_CLUMP_PATCHES),
                        ItemExt(3, STR_STRINGID, STR_MAPGEN_RULE_PRESET_TX_MOUNTAIN_PEAKS),
                    };

                    SetItems(items);

                    Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        Dropdown::Flag::StayOpen, std::size(items));
                    break;
                }
                case WIDX_RULE_TX_CONDITION_ADD:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        ItemExt(0, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_ELEVATION_ABSOLUTE),
                        ItemExt(1, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_ELEVATION_RELATIVE_TO_WATER),
                        ItemExt(2, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_WATER),
                        ItemExt(3, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_NOISE),
                        ItemExt(4, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_NORMAL_ANGLE),
                        ItemExt(5, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_PRNG),
                        ItemExt(6, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_BLEND_HEIGHT),
                        ItemExt(7, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_BLEND_NOISE),
                        ItemExt(8, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_LAND_STYLE),
                    };

                    SetItems(items);

                    Widget* ddWidget = &widgets[widgetIndex];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        Dropdown::Flag::StayOpen, std::size(items));

                    // TODO implement
                    SetDisabled(4, true);
                    // not available here
                    SetDisabled(8, true);

                    break;
                }
            }
        }

        void TextureUpdate()
        {
            // Tab animation
            if (++frame_no >= TabAnimationLoops[page])
                frame_no = 0;
            InvalidateWidget(WIDX_TAB_4);

            // Check if the mouse is hovering over the list
            if (!widgetIsHighlighted(*this, WIDX_RULE_TX_SCROLL))
            {
                if (_highlightedTxRule != -1)
                    InvalidateWidget(WIDX_RULE_TX_SCROLL);
                _highlightedTxRule = -1;
            }

            // Check if the mouse is hovering over the list
            if (!widgetIsHighlighted(*this, WIDX_RULE_TX_CONDITION_SCROLL))
            {
                if (_highlightedTxCondition != -1)
                    InvalidateWidget(WIDX_RULE_TX_CONDITION_SCROLL);
                _highlightedTxCondition = -1;
            }
        }

        void TextureTextInput(WidgetIndex widgetIndex, std::string_view& value)
        {
            switch (widgetIndex)
            {
                case WIDX_RULE_TX_RENAME:
                {
                    if (_selectedTxRule == -1 || _settings.textureRules[_selectedTxRule].isDefault)
                    {
                        return;
                    }

                    // TODO trim + check not empty?

                    auto& selectedRule = _settings.textureRules[_selectedTxRule];
                    selectedRule.name = std::string(value);
                    break;
                }
            }

            Invalidate();
        }

        void TexturePrepareDraw()
        {

            bool ruleSelected = _selectedTxRule != -1;
            bool condSelected = ruleSelected && _selectedTxCondition != -1;

            widgets[WIDX_RULE_TX_MOVE_UP].type = ruleSelected ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_RULE_TX_MOVE_DOWN].type = ruleSelected ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_RULE_TX_REMOVE].type = ruleSelected ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_RULE_TX_RENAME].type = ruleSelected ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_RULE_TX_CONDITION_GROUP].type = ruleSelected ? WidgetType::groupbox : WidgetType::empty;
            widgets[WIDX_RULE_TX_CONDITION_SCROLL].type = ruleSelected ? WidgetType::scroll : WidgetType::empty;
            widgets[WIDX_RULE_TX_CONDITION_REMOVE].type = condSelected ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_RULE_TX_CONDITION_EDIT].type = condSelected ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_RULE_TX_CONDITION_ADD].type = ruleSelected ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_RULE_TX_OUTCOME_GROUP].type = ruleSelected ? WidgetType::groupbox : WidgetType::empty;
            widgets[WIDX_RULE_TX_FLOOR_TEXTURE_CHECK].type = ruleSelected ? WidgetType::checkbox : WidgetType::empty;
            widgets[WIDX_RULE_TX_FLOOR_TEXTURE].type = ruleSelected ? WidgetType::flatBtn : WidgetType::empty;
            widgets[WIDX_RULE_TX_WALL_TEXTURE_CHECK].type = ruleSelected ? WidgetType::checkbox : WidgetType::empty;
            widgets[WIDX_RULE_TX_WALL_TEXTURE].type = ruleSelected ? WidgetType::flatBtn : WidgetType::empty;

            if (ruleSelected)
            {
                auto& selectedRule = _settings.textureRules[_selectedTxRule];
                SetCheckboxValue(WIDX_RULE_TX_FLOOR_TEXTURE_CHECK, selectedRule.effect.applyLandTexture);
                SetCheckboxValue(WIDX_RULE_TX_WALL_TEXTURE_CHECK, selectedRule.effect.applyEdgeTexture);

                SetWidgetEnabled(WIDX_RULE_TX_FLOOR_TEXTURE, selectedRule.effect.applyLandTexture);
                SetWidgetEnabled(WIDX_RULE_TX_WALL_TEXTURE, selectedRule.effect.applyEdgeTexture);

                SetWidgetEnabled(WIDX_RULE_TX_FLOOR_TEXTURE_CHECK, !selectedRule.isDefault);
                SetWidgetEnabled(WIDX_RULE_TX_WALL_TEXTURE_CHECK, !selectedRule.isDefault);

                SetWidgetEnabled(WIDX_RULE_TX_REMOVE, !selectedRule.isDefault);
                SetWidgetEnabled(WIDX_RULE_TX_RENAME, !selectedRule.isDefault);

                SetWidgetEnabled(WIDX_RULE_TX_MOVE_UP, !selectedRule.isDefault
                    && _selectedTxRule < static_cast<int32_t>(_settings.textureRules.size() - 1));
                SetWidgetEnabled(WIDX_RULE_TX_MOVE_DOWN, !selectedRule.isDefault &&  _selectedTxRule > 1);

                SetWidgetEnabled(WIDX_RULE_TX_CONDITION_REMOVE, !selectedRule.isDefault && condSelected);
                SetWidgetEnabled(WIDX_RULE_TX_CONDITION_EDIT, !selectedRule.isDefault && condSelected);
                SetWidgetEnabled(WIDX_RULE_TX_CONDITION_ADD, !selectedRule.isDefault);
            }

            SetPressedTab();
        }

        void TextureDraw(RenderTarget& rt)
        {
            DrawWidgets(rt);
            DrawTabImages(rt);

            if (_selectedTxRule != -1)
            {
                auto& selectedRule = _settings.textureRules[_selectedTxRule];
                DrawDropdownButton(rt, WIDX_RULE_TX_FLOOR_TEXTURE, LookupSurfaceImage(selectedRule.effect.landTexture));
                DrawDropdownButton(rt, WIDX_RULE_TX_WALL_TEXTURE, LookupEdgeImage(selectedRule.effect.edgeTexture));
            }
            else
            {
                auto& ruleScroll = widgets[WIDX_RULE_TX_SCROLL];
                auto centrePos = windowPos + ScreenCoordsXY{ ruleScroll.left, ruleScroll.bottom + 5 };
                auto textPaint = TextPaint{ colours[1] };
                DrawTextWrapped(rt, centrePos, ruleScroll.width(), STR_MAPGEN_RULE_TX_HINT, {}, textPaint);
            }
        }

        void TextureDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {

            switch (widgetIndex)
            {
                case WIDX_RULE_TX_FLOOR_TEXTURE:
                {
                    if (_selectedTxRule == -1)
                    {
                        return;
                    }
                    auto& selectedRule = _settings.textureRules[_selectedTxRule];

                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdownHighlightedIndex;

                    if (dropdownIndex != -1)
                        selectedRule.effect.landTexture = dropdownIndex;

                    Invalidate();
                    break;
                }
                case WIDX_RULE_TX_WALL_TEXTURE:
                {
                    if (_selectedTxRule == -1)
                    {
                        return;
                    }
                    auto& selectedRule = _settings.textureRules[_selectedTxRule];

                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdownHighlightedIndex;

                    if (dropdownIndex != -1)
                        selectedRule.effect.edgeTexture = dropdownIndex;

                    Invalidate();
                    break;
                }
                case WIDX_RULE_TX_NEW_PRESET:
                {
                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdownHighlightedIndex;

                    if (dropdownIndex != -1)
                    {
                        auto preset = static_cast<MapGenerator::Rule::TextureRulePreset>(dropdownIndex);
                        MapGenerator::Rule::createNewTextureRuleFromPreset(_settings, preset);
                        SetSelectedTextureRule(static_cast<int32_t>(_settings.textureRules.size() - 1));
                    }

                    Invalidate();
                    break;
                }
                case WIDX_RULE_TX_CONDITION_ADD:
                {
                    if (_selectedTxRule == -1)
                    {
                        return;
                    }

                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdownHighlightedIndex;

                    if (dropdownIndex == -1)
                    {
                        return;
                    }

                    auto conditionType = static_cast<MapGenerator::Rule::Type>(dropdownIndex);
                    auto newCondition = createNewCondition(conditionType);
                    auto callback = [this](MapGenerator::Rule::Condition& condition) {
                        if (_selectedTxRule == -1 || _settings.textureRules[_selectedTxRule].isDefault)
                        {
                            return;
                        }
                        auto& selectedRule = _settings.textureRules[_selectedTxRule];
                        selectedRule.conditions.push_back(std::move(condition));
                        SetSelectedTextureCondition(static_cast<int32_t>(selectedRule.conditions.size() - 1));
                        Invalidate();
                    };

                    MapGenRuleConditionOpen(this, WIDX_RULE_TX_CONDITION_ADD, newCondition, callback);
                    break;
                }
            }
        }

        ScreenSize TextureScrollGetSize(int32_t scrollIndex)
        {
            switch (scrollIndex)
            {
                case 0:
                {
                    return ScreenSize(
                        kWindowSize.width - 10, static_cast<int32_t>(_settings.textureRules.size() * kScrollableRowHeight));
                }
                case 1:
                {
                    if (_selectedTxRule == -1)
                    {
                        return {};
                    }
                    auto& selectedRule = _settings.textureRules[_selectedTxRule];

                    return ScreenSize(
                        widgets[WIDX_RULE_TX_CONDITION_SCROLL].width(),
                        static_cast<int32_t>(selectedRule.conditions.size() * kScrollableRowHeight));
                }

            }
            return {};
        }

        void TextureScrollDraw(int32_t scrollIndex, RenderTarget& rt)
        {
            switch (scrollIndex)
            {
                case 0:
                    return TextureScrollDrawRule(rt);
                case 1:
                    return TextureScrollDrawCondition(rt);
            }
        }

        void TextureScrollDrawCondition(RenderTarget& rt)
        {
            if (_selectedTxRule == -1)
            {
                return;
            }
            auto& selectedRule = _settings.textureRules[_selectedTxRule];

            const int32_t listWidth = widgets[WIDX_RULE_TX_CONDITION_SCROLL].width();
            GfxFillRect(
                rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } },
                ColourMapA[colours[1].colour].mid_light |  0x1000000);

            ScreenCoordsXY screenCoords{};
            screenCoords.y = static_cast<int32_t>(kScrollableRowHeight * (selectedRule.conditions.size() - 1));

            for (int32_t i = 0; i < static_cast<int32_t>(selectedRule.conditions.size()); i++)
            {
                auto& condition = selectedRule.conditions[i];

                // Draw row background colour
                auto fillRectangle = ScreenRect{ { 0, screenCoords.y },
                                                 { listWidth, screenCoords.y + kScrollableRowHeight - 1 } };

                StringId stringFormat = STR_WINDOW_COLOUR_2_STRINGID;
                if (i == _selectedTxCondition)
                {
                    GfxFillRect(rt, fillRectangle, ColourMapA[colours[1].colour].mid_dark);
                    stringFormat = STR_WHITE_STRING;
                }
                else if (i == _highlightedTxCondition)
                {
                    GfxFillRect(rt, fillRectangle, ColourMapA[colours[1].colour].mid_dark | 0x1000000);
                    stringFormat = STR_WHITE_STRING;
                }

                auto checkboxFormatter = Formatter();
                checkboxFormatter.Add<StringId>(STR_STRING);
                checkboxFormatter.Add<char*>(kCheckMarkString);

                // Draw enabled checkbox and check
                GfxFillRectInset(rt, { { 2, screenCoords.y }, { 15, screenCoords.y + 11 } }, colours[1], INSET_RECT_F_60);
                if (condition.enabled)
                {
                    DrawTextBasic(rt, screenCoords + ScreenCoordsXY{ 4, 1 }, stringFormat, checkboxFormatter);
                }

                StringId predRepr = kStringIdNone;
                switch (condition.predicate)
                {
                    case MapGenerator::Rule::Predicate::Equal:
                        predRepr = STR_MAPGEN_RULE_PREDICATE_EQUAL;
                        break;
                    case MapGenerator::Rule::Predicate::NotEqual:
                        predRepr = STR_MAPGEN_RULE_PREDICATE_NOT_EQUAL;
                        break;
                    case MapGenerator::Rule::Predicate::LessThan:
                        predRepr = STR_MAPGEN_RULE_PREDICATE_LESS_THAN;
                        break;
                    case MapGenerator::Rule::Predicate::GreaterThan:
                        predRepr = STR_MAPGEN_RULE_PREDICATE_GREATER_THAN;
                        break;
                    case MapGenerator::Rule::Predicate::LessThanOrEqual:
                        predRepr = STR_MAPGEN_RULE_PREDICATE_LESS_THAN_OR_EQUAL;
                        break;
                    case MapGenerator::Rule::Predicate::GreaterThanOrEqual:
                        predRepr = STR_MAPGEN_RULE_PREDICATE_GREATER_THAN_OR_EQUAL;
                        break;
                }

                auto ft = Formatter();
                switch (condition.type)
                {
                    case MapGenerator::Rule::Type::HeightAbsolute:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_ELEVATION_ABSOLUTE);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int16_t>(static_cast<int16_t>(
                            BaseZToMetres(std::get<MapGenerator::Rule::HeightData>(condition.data).height)));
                        break;

                    case MapGenerator::Rule::Type::HeightRelativeToWater:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_ELEVATION_RELATIVE_TO_WATER);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int16_t>(static_cast<int16_t>(
                            HeightUnitsToMetres(std::get<MapGenerator::Rule::HeightData>(condition.data).height)));
                        break;

                    case MapGenerator::Rule::Type::DistanceToWater:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_WATER);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int16_t>(static_cast<int16_t>(
                            std::get<MapGenerator::Rule::DistanceData>(condition.data).distance));
                        break;

                    case MapGenerator::Rule::Type::Noise:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_NOISE);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int32_t>(static_cast<int32_t>(
                            std::get<MapGenerator::Rule::NoiseData>(condition.data).value * 100));
                       break;

                    case MapGenerator::Rule::Type::NormalAngle:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_NORMAL_ANGLE);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int32_t>(static_cast<int32_t>(
                            std::get<MapGenerator::Rule::NormalAngleData>(condition.data).angle * 100));
                        break;

                    case MapGenerator::Rule::Type::Random:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int32_t>(static_cast<int32_t>(
                            std::get<MapGenerator::Rule::RandomData>(condition.data).value * 100));
                        break;

                    case MapGenerator::Rule::Type::BlendHeight:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_BLEND_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_BLEND_HEIGHT);
                        ft.Add<int16_t>(static_cast<int16_t>(
                            BaseZToMetres(std::get<MapGenerator::Rule::BlendHeightData>(condition.data).edgeLow)));
                        ft.Add<int16_t>(static_cast<int16_t>(
                            BaseZToMetres(std::get<MapGenerator::Rule::BlendHeightData>(condition.data).edgeHigh)));
                        ft.Add<StringId>(predRepr);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        break;

                    case MapGenerator::Rule::Type::BlendNoise:
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_BLEND_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_BLEND_NOISE);
                        ft.Add<int32_t>(static_cast<int32_t>(
                            std::get<MapGenerator::Rule::BlendNoiseData>(condition.data).edgeLow * 100));
                        ft.Add<int32_t>(static_cast<int32_t>(
                            std::get<MapGenerator::Rule::BlendNoiseData>(condition.data).edgeHigh * 100));
                        ft.Add<StringId>(predRepr);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        break;
                    case MapGenerator::Rule::Type::LandStyle:
                        // should not be reachable
                        break;
                }

                // Draw description
                DrawTextEllipsised(rt, screenCoords + ScreenCoordsXY{ 20, 0 }, 190, stringFormat, ft);

                screenCoords.y -= kScrollableRowHeight;
            }
        }

        void TextureScrollDrawRule(RenderTarget& rt)
        {
            const int32_t listWidth = widgets[WIDX_RULE_TX_SCROLL].width();
            GfxFillRect(
                rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } }, ColourMapA[colours[1].colour].mid_light);

            ScreenCoordsXY screenCoords{};
            screenCoords.y = static_cast<int32_t>(kScrollableRowHeight * (_settings.textureRules.size() - 1));

            for (int32_t i = 0; i < static_cast<int32_t>(_settings.textureRules.size()); i++)
            {
                auto& rule = _settings.textureRules[i];

                // Draw row background colour
                auto fillRectangle = ScreenRect{ { 0, screenCoords.y },
                                                 { listWidth, screenCoords.y + kScrollableRowHeight - 1 } };

                StringId stringFormat = STR_WINDOW_COLOUR_2_STRINGID;
                if (i == _selectedTxRule)
                {
                    GfxFillRect(rt, fillRectangle, ColourMapA[colours[1].colour].mid_dark);
                    stringFormat = STR_WHITE_STRING;
                }
                else if (i == _highlightedTxRule)
                {
                    GfxFillRect(rt, fillRectangle, ColourMapA[colours[1].colour].mid_dark | 0x1000000);
                    stringFormat = STR_WHITE_STRING;
                }

                auto checkboxFormatter = Formatter();
                checkboxFormatter.Add<StringId>(STR_STRING);
                checkboxFormatter.Add<char*>(kCheckMarkString);

                if (!rule.isDefault)
                {
                    // Draw enabled checkbox and check
                    GfxFillRectInset(rt, { { 2, screenCoords.y }, { 15, screenCoords.y + 11 } }, colours[1], INSET_RECT_F_E0);
                    if (rule.enabled)
                    {
                        DrawTextBasic(rt, screenCoords + ScreenCoordsXY{ 4, 1 }, stringFormat, checkboxFormatter);
                    }
                }

                // Rule name
                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<char*>(rule.name.c_str());
                DrawTextEllipsised(
                    rt, screenCoords + ScreenCoordsXY{ 20, 0 }, 190, stringFormat, ft);

                ImageId maskImage(SPR_G2_MASK_MAPGEN_TEXTURE_SCROLL);
                // Surface
                if (rule.effect.applyLandTexture)
                {
                    auto surfaceImage = LookupSurfaceImage(rule.effect.landTexture);
                    GfxDrawSpriteRawMasked(rt, screenCoords + ScreenCoordsXY{ 208, 0 }, maskImage, surfaceImage);
                }

                // Edge
                if (rule.effect.applyEdgeTexture)
                {
                    auto edgeImage = LookupEdgeImage(rule.effect.edgeTexture);
                    GfxDrawSpriteRawMasked(rt, screenCoords + ScreenCoordsXY{ 248, 0 }, maskImage, edgeImage);
                }

                screenCoords.y -= kScrollableRowHeight;
            }
        }

        void TextureScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
            if(scrollIndex == 0)
            {
                int32_t index = static_cast<int32_t>(
                    _settings.textureRules.size() - (screenCoords.y - 1) / kScrollableRowHeight - 1);
                if (index < 0 || index >= static_cast<int32_t>(_settings.textureRules.size()))
                {
                    _highlightedTxRule = -1;
                }
                else
                {
                    _highlightedTxRule = index;
                }
                InvalidateWidget(WIDX_RULE_TX_SCROLL);
            }
            else if (scrollIndex == 1)
            {
                if (_selectedTxRule == -1)
                {
                    _highlightedTxCondition = -1;
                }
                else
                {
                    auto& selectedRule = _settings.textureRules[_selectedTxRule];

                    int32_t index = static_cast<int32_t>(
                        selectedRule.conditions.size() - (screenCoords.y - 1) / kScrollableRowHeight - 1);
                    if (index < 0 || index >= static_cast<int32_t>(selectedRule.conditions.size()))
                    {
                        _highlightedTxCondition = -1;
                    }
                    else
                    {
                        _highlightedTxCondition = index;
                    }
                }
                InvalidateWidget(WIDX_RULE_TX_CONDITION_SCROLL);
            }


        }

        void TextureScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
            if (_settings.textureRules.empty())
                return;

            if (scrollIndex == 0)
            {
                // Because the list items are displayed in reverse order, subtract the calculated index from size
                const int32_t index = static_cast<int32_t>(
                    _settings.textureRules.size() - (screenCoords.y - 1) / kScrollableRowHeight - 1);
                const ScreenRect checkboxColumnRect{ { 2, 0 }, { 15, screenCoords.y } };
                if (index >= 0 && checkboxColumnRect.Contains(screenCoords) && !_settings.textureRules[index].isDefault)
                { // Checkbox was clicked
                    _settings.textureRules[index].enabled = !_settings.textureRules[index].enabled;
                }
                else if (index >= 0 && index < static_cast<int32_t>(_settings.textureRules.size()))
                {
                    SetSelectedTextureRule(index);
                }
                else
                {
                    SetSelectedTextureRule(-1);
                }
            } else if (scrollIndex == 1)
            {
                if (_selectedTxRule == -1)
                {
                    SetSelectedTextureCondition(-1);
                } else
                {
                    auto& selectedRule = _settings.textureRules[_selectedTxRule];
                    // Because the list items are displayed in reverse order, subtract the calculated index from size
                    const int32_t index = static_cast<int32_t>(
                        selectedRule.conditions.size() - (screenCoords.y - 1) / kScrollableRowHeight - 1);
                    const ScreenRect checkboxColumnRect{ { 2, 0 }, { 15, screenCoords.y } };
                    if (index >= 0 && checkboxColumnRect.Contains(screenCoords))
                    { // Checkbox was clicked
                        selectedRule.conditions[index].enabled = !selectedRule.conditions[index].enabled;
                    }
                    else if (index >= 0 && index < static_cast<int32_t>(selectedRule.conditions.size()))
                    {
                        SetSelectedTextureCondition(index);
                    }
                    else
                    {
                        SetSelectedTextureCondition(-1);
                    }
                }
            }
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
            }
        }

        void WaterMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_WATER_LEVEL_UP:
                    _settings.waterLevel = std::min<int32_t>(_settings.waterLevel + 2, kMaximumWaterHeight);
                    Invalidate();
                    break;
                case WIDX_WATER_LEVEL_DOWN:
                    _settings.waterLevel = std::max<int32_t>(_settings.waterLevel - 2, kMinimumWaterHeight);
                    Invalidate();
                    break;
            }
        }

        void WaterUpdate()
        {
            // Tab animation
            if (++frame_no >= TabAnimationLoops[page])
                frame_no = 0;
            InvalidateWidget(WIDX_TAB_3);
        }

        void WaterTextInput(WidgetIndex widgetIndex, int32_t value)
        {
            switch (widgetIndex)
            {
                case WIDX_WATER_LEVEL:
                    _settings.waterLevel = value;
                    break;
            }

            Invalidate();
        }

        void WaterPrepareDraw()
        {
            SetPressedTab();
        }

        void WaterDraw(RenderTarget& rt)
        {
            DrawWidgets(rt);
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
        void OnOpen() override
        {
            number = 0;

            MapGenerator::Rule::createDefaultTextureRules(_settings);
            MapGenerator::Rule::createDefaultSceneryRules(_settings);

            SetPage(WINDOW_MAPGEN_PAGE_BASE);
            Invalidate();
            hold_down_widgets = HoldDownWidgets[WINDOW_MAPGEN_PAGE_BASE];
            pressed_widgets = PressedWidgets[WINDOW_MAPGEN_PAGE_BASE];
            disabled_widgets = PageDisabledWidgets[WINDOW_MAPGEN_PAGE_BASE];
            InitScrollWidgets();

            _heightmapLoaded = false;
        }

        void OnClose() override
        {
            MapGenerator::UnloadHeightmapImage();
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BaseMouseUp(widgetIndex);
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryMouseUp(widgetIndex);
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainMouseUp(widgetIndex);
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterMouseUp(widgetIndex);
                case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TextureMouseUp(widgetIndex);
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BaseMouseDown(widgetIndex, &widgets[widgetIndex]);
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainMouseDown(widgetIndex, &widgets[widgetIndex]);
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterMouseDown(widgetIndex, &widgets[widgetIndex]);
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryMouseDown(widgetIndex, &widgets[widgetIndex]);
                case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TextureMouseDown(widgetIndex, &widgets[widgetIndex]);
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BaseDropdown(widgetIndex, selectedIndex);
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainDropdown(widgetIndex, selectedIndex);
                case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TextureDropdown(widgetIndex, selectedIndex);
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryDropdown(widgetIndex, selectedIndex);
            }
        }

        void OnUpdate() override
        {
            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                InvalidateWidget(WIDX_MAP_SEED);
            }

            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BaseUpdate();
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainUpdate();
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryUpdate();
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterUpdate();
                case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TextureUpdate();
            }
        }

        void OnPrepareDraw() override
        {
            bool isHeightMapImage = _settings.algorithm == MapGenerator::Algorithm::heightmapImage;
            SetWidgetDisabled(WIDX_MAP_GENERATE, isHeightMapImage && !_heightmapLoaded);

            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BasePrepareDraw();
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryPrepareDraw();
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainPrepareDraw();
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterPrepareDraw();
               case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TexturePrepareDraw();
            }
        }

        void OnDraw(RenderTarget& rt) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_BASE:
                    return BaseDraw(rt);
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryDraw(rt);
                case WINDOW_MAPGEN_PAGE_TERRAIN:
                    return TerrainDraw(rt);
                case WINDOW_MAPGEN_PAGE_WATER:
                    return WaterDraw(rt);
                case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TextureDraw(rt);
            }
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TextureScrollGetSize(scrollIndex);
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryScrollGetSize(scrollIndex);
            }
            return {};
        }

        void OnScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TextureScrollDraw(scrollIndex, rt);
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryScrollDraw(scrollIndex, rt);
            }
        }

        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TextureScrollMouseOver(scrollIndex, screenCoords);
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryScrollMouseOver(scrollIndex, screenCoords);
            }
        }

        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TextureScrollMouseDown(scrollIndex, screenCoords);
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryScrollMouseDown(scrollIndex, screenCoords);
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            auto strText = std::string(text);
            char* end;

            if (page == WINDOW_MAPGEN_PAGE_BASE && widgetIndex == WIDX_MAP_SEED && !_random_seed)
            {
                _seed.assign(text);
                return;
            }
            if (page == WINDOW_MAPGEN_PAGE_TEXTURE)
            {
                return TextureTextInput(widgetIndex, text);
            }
            if (page == WINDOW_MAPGEN_PAGE_SCENERY)
            {
                return SceneryTextInput(widgetIndex, text);
            }

            // Convert text to integer value
            int32_t value{};
            if ((page == WINDOW_MAPGEN_PAGE_BASE && widgetIndex == WIDX_SIMPLEX_BASE_FREQ)
                || (page == WINDOW_MAPGEN_PAGE_BASE && widgetIndex == WIDX_BIAS_STRENGTH)
                || (page == WINDOW_MAPGEN_PAGE_TERRAIN && widgetIndex == WIDX_HEIGHTMAP_EROSION_PPT))
                value = 100 * strtof(strText.c_str(), &end);
            else
                value = strtol(strText.c_str(), &end, 10);

            if (*end != '\0')
                return;

            // Take care of unit conversion
            if (page != WINDOW_MAPGEN_PAGE_BASE && widgetIndex != WIDX_HEIGHTMAP_EROSION_PPT)
            {
                switch (Config::Get().general.MeasurementFormat)
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
            }
        }

        void AfterLoadingHeightMap(ModalResult result, const utf8* path)
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
                SetPage(WINDOW_MAPGEN_PAGE_BASE);
            }
        }

        void OnResize() override
        {
            WindowSetResize(*this, kWindowSize, kWindowSize);
        }
    };

    WindowBase* MapGenOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<MapGenWindow>(WindowClass::Mapgen, kWindowSize, WF_10 | WF_AUTO_POSITION);
    }

    static void HeightmapLoadsaveCallback(ModalResult result, const utf8* path)
    {
        auto* w = static_cast<MapGenWindow*>(MapGenOpen());
        w->AfterLoadingHeightMap(result, path);
    }
} // namespace OpenRCT2::Ui::Windows
