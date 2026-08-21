/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "openrct2/Diagnostic.h"
#include "openrct2/object/WallSceneryEntry.h"
#include "openrct2/world/map_generator/MapGenSerDe.hpp"
#include "openrct2/world/map_generator/MapHelpers.h"
#include "openrct2/world/map_generator/rule/RuleMisc.h"

#include <algorithm>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/FileSystem.hpp>
#include <openrct2/core/UnitConversion.h>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/drawing/RenderTarget.h>
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
#include <openrct2/world/map_generator/Erosion.h>
#include <openrct2/world/map_generator/MapGen.h>
#include <openrct2/world/map_generator/PngTerrainGenerator.h>
#include <openrct2/world/map_generator/river/RiverTypes.hpp>
#include <random>

using namespace OpenRCT2::Drawing;
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

    enum WindowMapGenWidgetIdx : WidgetIndex
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
        WIDX_MAP_SERDE,
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
        WIDX_SIMPLEX_TYPE,
        WIDX_SIMPLEX_TYPE_DROPDOWN,
        WIDX_SIMPLEX_BASE_FREQ,
        WIDX_SIMPLEX_BASE_FREQ_UP,
        WIDX_SIMPLEX_BASE_FREQ_DOWN,
        WIDX_SIMPLEX_OCTAVES,
        WIDX_SIMPLEX_OCTAVES_UP,
        WIDX_SIMPLEX_OCTAVES_DOWN,

        WIDX_HEIGHTMAP_GROUP,
        WIDX_HEIGHTMAP_BROWSE,
        WIDX_HEIGHTMAP_NORMALIZE,

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

        WIDX_HEIGHTMAP_TRANSFORM_GROUP,
        WIDX_HEIGHTMAP_TRANSFORM_TYPE,
        WIDX_HEIGHTMAP_TRANSFORM_TYPE_DROPDOWN,
        WIDX_HEIGHTMAP_TRANSFORM_STRENGTH,
        WIDX_HEIGHTMAP_TRANSFORM_STRENGTH_UP,
        WIDX_HEIGHTMAP_TRANSFORM_STRENGTH_DOWN,

        WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES,
        WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES_DROPDOWN,

        WIDX_WATER_LEVEL = TAB_BEGIN,
        WIDX_WATER_LEVEL_UP,
        WIDX_WATER_LEVEL_DOWN,
        WIDX_WATER_RIVERS_GROUP,
        WIDX_WATER_RIVERS_ENABLE,
        WIDX_WATER_RIVERS_CATCHMENT,
        WIDX_WATER_RIVERS_CATCHMENT_UP,
        WIDX_WATER_RIVERS_CATCHMENT_DOWN,
        WIDX_WATER_RIVERS_WIDTH_MAX,
        WIDX_WATER_RIVERS_WIDTH_MAX_UP,
        WIDX_WATER_RIVERS_WIDTH_MAX_DOWN,
        WIDX_WATER_RIVERS_GROWTH_EXPONENT,
        WIDX_WATER_RIVERS_GROWTH_EXPONENT_UP,
        WIDX_WATER_RIVERS_GROWTH_EXPONENT_DOWN,
        WIDX_WATER_RIVERS_PRUNE_THRESHOLD,
        WIDX_WATER_RIVERS_PRUNE_THRESHOLD_UP,
        WIDX_WATER_RIVERS_PRUNE_THRESHOLD_DOWN,
        WIDX_WATER_RIVERS_BREACH_LENGTH,
        WIDX_WATER_RIVERS_BREACH_LENGTH_UP,
        WIDX_WATER_RIVERS_BREACH_LENGTH_DOWN,
        WIDX_WATER_RIVERS_BREACH_DEPTH,
        WIDX_WATER_RIVERS_BREACH_DEPTH_UP,
        WIDX_WATER_RIVERS_BREACH_DEPTH_DOWN,
        WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL,
        WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT,
        WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT_UP,
        WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT_DOWN,

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

    static constexpr ScreenSize kWindowSize = { 310, 294 };

    // clang-format off
    static constexpr auto kMakeSharedWidgets = [](StringId title) {
        return makeWidgets(
            makeWindowShim(title, kWindowSize),
            makeWidget({   0,  43 }, { kWindowSize.width, 177 }, WidgetType::resize, WindowColour::secondary),
            makeTab   ({   3,  17 }),
            makeTab   ({  34,  17 }),
            makeTab   ({  65,  17 }),
            makeTab   ({  96,  17 }),
            makeTab   ({ 127,  17 }),
            makeWidget({  5, 274 }, {  14, 14 }, WidgetType::button, WindowColour::secondary, STR_MAPGEN_SETTINGS_MISC_MENU),
            makeWidget({ 150, 274 }, { 155, 14 }, WidgetType::button, WindowColour::secondary, STR_MAPGEN_ACTION_GENERATE)
        );
    };

    static constexpr auto kBaseWidgets = makeWidgets(
        kMakeSharedWidgets(STR_MAPGEN_CAPTION_GENERATOR),
        makeHoldableSpinnerWidgets({153,  52}, { 62, 12}, WidgetType::spinner,      WindowColour::secondary, kStringIdEmpty                                             ), // NB: 3 widgets
        makeWidget                ({215,  52}, { 21, 12}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_G2_LINK_CHAIN), STR_MAINTAIN_SQUARE_MAP_TOOLTIP),
        makeHoldableSpinnerWidgets({236,  52}, { 62, 12}, WidgetType::spinner,      WindowColour::secondary, kStringIdEmpty                                             ), // NB: 3 widgets

        makeWidget                ({  5,  70}, {300, 41}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_SEED       ), // WIDX_SEED_GROUP
        makeWidget                ({ 10,  90}, {138, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_MAPGEN_SEED_RANDOM), // WIDX_SEED_RANDOM
        makeWidget                ({153,  90}, {145, 14}, WidgetType::textBox,      WindowColour::secondary                        ), // WIDX_SEED

        makeDropdownWidgets       ({153, 117}, {145, 14}, WidgetType::dropdownMenu, WindowColour::secondary, STR_HEIGHTMAP_FLATLAND),

        makeWidget                ({  5, 134}, {300, 74}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_SIMPLEX_NOISE  ), // WIDX_SIMPLEX_GROUP
        makeDropdownWidgets       ({153, 151}, {145, 14}, WidgetType::dropdownMenu, WindowColour::secondary, STR_HEIGHTMAP_NOISE    ), // WIDX_SIMPLEX_TYPE
        makeHoldableSpinnerWidgets({153, 169}, {145, 12}, WidgetType::spinner,      WindowColour::secondary                            ), // WIDX_SIMPLEX_BASE_FREQ{,_UP,_DOWN}
        makeHoldableSpinnerWidgets({153, 187}, {145, 12}, WidgetType::spinner,      WindowColour::secondary                            ), // WIDX_SIMPLEX_OCTAVES{,_UP,_DOWN}

        makeWidget                ({  5, 134}, {300, 56}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_SELECT_HEIGHTMAP), // WIDX_HEIGHTMAP_GROUP
        makeWidget                ({223, 151}, { 65, 14}, WidgetType::button,       WindowColour::secondary, STR_BROWSE                 ), // WIDX_HEIGHTMAP_BROWSE
        makeWidget                ({ 10, 169}, {150, 12}, WidgetType::checkbox,     WindowColour::secondary, STR_MAPGEN_NORMALIZE       ), // WIDX_HEIGHTMAP_NORMALIZE

        makeWidget                ({  5, 212}, {300, 56}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_BIAS          ), // WIDX_BIAS_GROUP
        makeDropdownWidgets       ({153, 228}, {145, 14}, WidgetType::dropdownMenu, WindowColour::secondary, STR_MAPGEN_BIAS_TYPE_NONE), // WIDX_BIAS_TYPE(_DROPDOWN)
        makeHoldableSpinnerWidgets({153, 248}, {145, 12}, WidgetType::spinner,      WindowColour::secondary                           )  // WIDX_BIAS_STRENGTH{,_UP,_DOWN}
    );

    static constexpr auto kTerrainWidgets = makeWidgets(
        kMakeSharedWidgets(STR_MAPGEN_CAPTION_TERRAIN),
        makeHoldableSpinnerWidgets({153,  52}, {145, 12}, WidgetType::spinner,      WindowColour::secondary                               ), // WIDX_HEIGHTMAP_LOW{,_UP,_DOWN}
        makeHoldableSpinnerWidgets({153,  70}, {145, 12}, WidgetType::spinner,      WindowColour::secondary                               ), // WIDX_HEIGHTMAP_HIGH{,_UP,_DOWN}

        makeWidget                ({  5,  88}, {300, 56}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_TRANSFORM         ), // WIDX_HEIGHTMAP_SMOOTH_HEIGHTMAP_GROUP
        makeDropdownWidgets       ({153, 105}, {145, 14}, WidgetType::dropdownMenu, WindowColour::secondary, STR_MAPGEN_TRANSFORM_TYPE_BOX), // WIDX_HEIGHTMAP_SMOOTH_FILTER(_DROPDOWN)
        makeHoldableSpinnerWidgets({153, 125}, {145, 12}, WidgetType::spinner,      WindowColour::secondary                               ), // WIDX_HEIGHTMAP_SMOOTH_STRENGTH

        makeDropdownWidgets       ({153, 149}, {145, 14}, WidgetType::dropdownMenu, WindowColour::secondary, STR_MAPGEN_SMOOTH_EDGE_WEAK  )  // WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES(_DROPDOWN)
    );

    static constexpr auto kWaterWidgets = makeWidgets(
        kMakeSharedWidgets(STR_MAPGEN_CAPTION_WATER),
        makeHoldableSpinnerWidgets({153,  52}, {145,  12}, WidgetType::spinner,  WindowColour::secondary                         ), // NB: 3 widgets
        makeWidget                ({  5,  70}, {300, 162}, WidgetType::groupbox, WindowColour::secondary, STR_WATER_RIVERS       ),
        makeWidget                ({ 10,  86}, {136,  12}, WidgetType::checkbox, WindowColour::secondary, STR_WATER_RIVERS_ENABLE, STR_WATER_RIVERS_ENABLE),
        makeHoldableSpinnerWidgets({153, 104}, {145,  12}, WidgetType::spinner,  WindowColour::secondary                         ), // NB: 3 widgets
        makeHoldableSpinnerWidgets({153, 122}, {145,  12}, WidgetType::spinner,  WindowColour::secondary                         ), // NB: 3 widgets
        makeHoldableSpinnerWidgets({153, 140}, {145,  12}, WidgetType::spinner,  WindowColour::secondary                         ), // NB: 3 widgets
        makeHoldableSpinnerWidgets({153, 158}, {145,  12}, WidgetType::spinner,  WindowColour::secondary                         ), // NB: 3 widgets
        makeHoldableSpinnerWidgets({153, 176}, { 70,  12}, WidgetType::spinner,  WindowColour::secondary                         ), // NB: 3 widgets
        makeHoldableSpinnerWidgets({228, 176}, { 70,  12}, WidgetType::spinner,  WindowColour::secondary                         ), // NB: 3 widgets
        makeWidget                ({ 10, 194}, {300,  12}, WidgetType::checkbox, WindowColour::secondary, STR_WATER_RIVERS_FRACTIONAL_AGGREGATION), // NB: 3 widgets
        makeHoldableSpinnerWidgets({153, 212}, {145,  12}, WidgetType::spinner,  WindowColour::secondary                         )  // NB: 3 widgets
    );

    static constexpr auto kTextureWidgets = makeWidgets(
        kMakeSharedWidgets(STR_MAPGEN_RULE_TITLE),
        makeWidget({236,  52}, { 55,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_NEW                     ),
        makeWidget({291,  52}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_DOWN                                ),
        makeWidget({162,  52}, { 69,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_RENAME                  ),
        makeWidget({ 88,  52}, { 69,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_REMOVE                  ),
        makeWidget({  5,  52}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_UP                                  ),
        makeWidget({ 22,  52}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_DOWN                                ),
        makeWidget({  5,  70}, { 20,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_ENABLED          ),
        makeWidget({ 25,  70}, {200,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_NAME             ),
        makeWidget({225,  70}, { 40,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_SURFACE          ),
        makeWidget({265,  70}, { 40,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_EDGE             ),
        makeWidget({  5,  83}, {300,  90}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL                         ),

        makeWidget({  5, 176}, {212,  94}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_RULE_GROUP_IF                ),
        makeWidget({ 10, 188}, {201,  59}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL                         ),
        makeWidget({ 10, 251}, { 62,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_COND_REMOVE             ),
        makeWidget({ 79, 251}, { 62,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_COND_EDIT               ),
        makeWidget({148, 251}, { 62,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_COND_ADD                ),

        makeWidget({222, 176}, { 83,  94}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_RULE_GROUP_THEN              ),
        makeWidget({232, 198}, { 12,  12}, WidgetType::checkbox,     WindowColour::secondary                                          ),
        makeWidget({251, 186}, { 47,  36}, WidgetType::flatBtn,      WindowColour::secondary, 0xFFFFFFFF, STR_CHANGE_BASE_LAND_TIP    ),
        makeWidget({232, 240}, { 12,  12}, WidgetType::checkbox,     WindowColour::secondary                                          ),
        makeWidget({251, 228}, { 47,  36}, WidgetType::flatBtn,      WindowColour::secondary, 0xFFFFFFFF, STR_CHANGE_VERTICAL_LAND_TIP)
    );

    static constexpr auto kForestsWidgets = makeWidgets(
        kMakeSharedWidgets(STR_MAPGEN_CAPTION_FORESTS),
        makeWidget({236,  52}, { 55,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_NEW                     ),
        makeWidget({291,  52}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_DOWN                                ),
        makeWidget({162,  52}, { 69,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_RENAME                  ),
        makeWidget({ 88,  52}, { 69,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_REMOVE                  ),
        makeWidget({  5,  52}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_UP                                  ),
        makeWidget({ 22,  52}, { 14,  14}, WidgetType::button,       WindowColour::secondary, STR_DOWN                                ),
        makeWidget({  5,  70}, { 20,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_ENABLED          ),
        makeWidget({ 25,  70}, {200,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_NAME             ),
        makeWidget({225,  70}, { 80,  14}, WidgetType::tableHeader,  WindowColour::secondary, STR_MAPGEN_RULE_HEADER_ITEMS            ),
        makeWidget({  5,  83}, {300,  90}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL                         ),

        makeWidget({  5, 176}, {212,  94}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_RULE_GROUP_IF                ),
        makeWidget({ 10, 188}, {201,  59}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL                         ),
        makeWidget({ 10, 251}, { 62,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_COND_REMOVE             ),
        makeWidget({ 79, 251}, { 62,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_COND_EDIT               ),
        makeWidget({148, 251}, { 62,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_COND_ADD                ),

        makeWidget({222, 176}, { 83,  94}, WidgetType::groupbox,     WindowColour::secondary, STR_MAPGEN_RULE_GROUP_THEN              ),
        makeWidget({227, 251}, { 73,  14}, WidgetType::button,       WindowColour::secondary, STR_MAPGEN_RULE_SELECT                  )
    );

    static std::span<const Widget> PageWidgets[WINDOW_MAPGEN_PAGE_COUNT] = {
        kBaseWidgets,
        kTerrainWidgets,
        kWaterWidgets,
        kTextureWidgets,
        kForestsWidgets,
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
        both,
        x,
        y,
    };

    static void HeightmapLoadsaveCallback(ModalResult result, const utf8* path);
    static void MapgenSettingsLoadCallback(ModalResult result, const utf8* path);
    static void MapgenSettingsSaveCallback(ModalResult result, const utf8* path);

    class MapGenWindow final : public Window
    {
    private:
        ResizeDirection _resizeDirection{ ResizeDirection::both };
        bool _mapWidthAndHeightLinked{ true };
        bool _random_seed = true;
        bool _heightmapLoaded = false;
        MapGenerator::Settings& _settings = MapGenerator::getMapGenSettings();
        std::string _heightmapFilename{};

        u8string _xSpinnerCaption{};
        u8string _ySpinnerCaption{};

        int32_t _selectedTxRule = -1;
        int32_t _highlightedTxRule = -1;

        int32_t _selectedTxCondition = -1;
        int32_t _highlightedTxCondition = -1;

        int32_t _selectedScRule = -1;
        int32_t _highlightedScRule = -1;

        int32_t _selectedScCondition = -1;
        int32_t _highlightedScCondition = -1;

        void setPage(int32_t newPage)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == newPage && !widgets.empty())
                return;

            page = newPage;
            currentFrame = 0;
            removeViewport();

            setWidgets(PageWidgets[newPage]);
            widgetSetPressedExclusive(
                *this, { WIDX_TAB_1, WIDX_TAB_2, WIDX_TAB_3, WIDX_TAB_4, WIDX_TAB_5 }, WIDX_TAB_1 + newPage);

            initScrollWidgets();
            invalidate();
            onResize();
        }

        void DrawTabImage(RenderTarget& rt, int32_t newPage, int32_t spriteIndex)
        {
            const WidgetIndex widgetIndex = WIDX_TAB_1 + newPage;

            if (!widgetIsDisabled(*this, widgetIndex))
            {
                if (page == newPage)
                {
                    const int32_t frame = currentFrame / TabAnimationDivisor[page];
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
                _resizeDirection = ResizeDirection::both;

            if (_resizeDirection != ResizeDirection::x)
            {
                _settings.mapSize.y = std::clamp<int32_t>(
                    _settings.mapSize.y + sizeOffset, kMinimumMapSizeTechnical, kMaximumMapSizeTechnical);
            }

            if (_resizeDirection != ResizeDirection::y)
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
                case WIDX_TAB_5:
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
                case WIDX_MAP_GENERATE:
                    GenerateMap();
                    break;
            }
        }

        void SharedMouseDown(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_MAP_SERDE:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        ItemExt(0, STR_STRINGID, STR_MAPGEN_SETTINGS_SERDE_IMPORT),
                        ItemExt(1, STR_STRINGID, STR_MAPGEN_SETTINGS_SERDE_EXPORT),
                        ItemExt(2, STR_STRINGID, STR_MAPGEN_SETTINGS_RESET),
                    };

                    SetItems(items);

                    const Widget* ddWidget = &widgets[widgetIndex];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height(), colours[1],
                        {}, std::size(items));

                    break;
                }
            }
        }

        void SharedDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_MAP_SERDE:
                {
                    if (dropdownIndex == 0)
                    {
                        auto intent = Intent(WindowClass::loadsave);
                        intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::load);
                        intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::mapgenSettings);
                        intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(MapgenSettingsLoadCallback));
                        ContextOpenIntent(&intent);
                    }
                    else if (dropdownIndex == 1)
                    {
                        auto intent = Intent(WindowClass::loadsave);
                        intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::save);
                        intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::mapgenSettings);
                        intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(MapgenSettingsSaveCallback));
                        ContextOpenIntent(&intent);
                    }
                    else if (dropdownIndex == 2)
                    {
                        MapGenerator::resetMapGenSettings();
                        invalidate();
                    }
                    break;
                }
            }
        }

        void GenerateMap()
        {
            if (_settings.generator == MapGenerator::HeightMapGenerator::image && !_heightmapLoaded)
            {
                return;
            }

            if (_random_seed)
            {
                MapGenerator::setRandomSeed(_settings);
                LOG_INFO("seed %s", _settings.seed.c_str());
            }

            MapGenerator::Settings mapgenSettings = _settings;

            // doing this here so all dimensions in the map gen code are consistent
            if (_settings.generator == MapGenerator::HeightMapGenerator::image)
            {
                auto imageDimensions = MapGenerator::queryHeightMapFromImageDimensions();
                mapgenSettings.mapSize = { imageDimensions.x, imageDimensions.y };
            }

            MapGenerator::generate(mapgenSettings);
            GfxInvalidateScreen();
        }

#pragma region Base page

        void BaseMouseUp(WidgetIndex widgetIndex)
        {
            SharedMouseUp(widgetIndex);

            if (_settings.generator == MapGenerator::HeightMapGenerator::noise)
            {
                SimplexMouseUp(widgetIndex);
                BiasMouseUp(widgetIndex);
            }
            else if (_settings.generator == MapGenerator::HeightMapGenerator::image)
            {
                HeightmapMouseUp(widgetIndex);
            }

            switch (widgetIndex)
            {
                case WIDX_MAP_SIZE_Y:
                    _resizeDirection = ResizeDirection::y;
                    InputMapSize(WIDX_MAP_SIZE_Y, _settings.mapSize.y);
                    break;
                case WIDX_MAP_SIZE_X:
                    _resizeDirection = ResizeDirection::x;
                    InputMapSize(WIDX_MAP_SIZE_X, _settings.mapSize.x);
                    break;
                case WIDX_MAP_SIZE_LINK:
                    _mapWidthAndHeightLinked = !_mapWidthAndHeightLinked;
                    break;
                case WIDX_MAP_SEED:
                    if (!_random_seed)
                    {
                        WindowStartTextbox(*this, widgetIndex, _settings.seed, 16);
                    }
                    break;
                case WIDX_MAP_SEED_RANDOM:
                    _random_seed = !_random_seed;
                    setCheckboxValue(WIDX_MAP_SEED_RANDOM, _random_seed);
                    widgets[WIDX_MAP_SEED].type = _random_seed ? WidgetType::empty : WidgetType::textBox;
                    invalidateWidget(WIDX_MAP_SEED_RANDOM);
                    invalidateWidget(WIDX_MAP_SEED);
                    break;
            }
        }

        void BaseMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            if (_settings.generator == MapGenerator::HeightMapGenerator::noise)
            {
                SimplexMouseDown(widgetIndex, widget);
                BiasMouseDown(widgetIndex, widget);
            }
            else if (_settings.generator == MapGenerator::HeightMapGenerator::image)
            {
                HeightmapMouseDown(widgetIndex, widget);
            }

            switch (widgetIndex)
            {
                case WIDX_MAP_SIZE_Y_UP:
                    _resizeDirection = ResizeDirection::y;
                    ChangeMapSize(+1);
                    invalidate();
                    break;
                case WIDX_MAP_SIZE_Y_DOWN:
                    _resizeDirection = ResizeDirection::y;
                    ChangeMapSize(-1);
                    invalidate();
                    break;
                case WIDX_MAP_SIZE_X_UP:
                    _resizeDirection = ResizeDirection::x;
                    ChangeMapSize(+1);
                    invalidate();
                    break;
                case WIDX_MAP_SIZE_X_DOWN:
                    _resizeDirection = ResizeDirection::x;
                    ChangeMapSize(-1);
                    invalidate();
                    break;
                case WIDX_HEIGHTMAP_SOURCE_DROPDOWN:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        ToggleOption(0, STR_HEIGHTMAP_FLATLAND),
                        ToggleOption(1, STR_HEIGHTMAP_NOISE),
                        ToggleOption(2, STR_HEIGHTMAP_FILE),
                    };

                    SetItems(items);

                    const Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height(), colours[1], 0, {},
                        std::size(items), ddWidget->width() - 3);

                    gDropdown.items[EnumValue(_settings.generator)].setChecked(true);
                    break;
                }
                case WIDX_SIMPLEX_TYPE_DROPDOWN:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        ToggleOption(0, STR_HEIGHTMAP_SIMPLEX_NOISE),
                        ToggleOption(1, STR_HEIGHTMAP_WARPED_NOISE),
                        ToggleOption(2, STR_HEIGHTMAP_RIDGED_NOISE),
                        ToggleOption(3, STR_HEIGHTMAP_VORONOI_NOISE),
                    };

                    SetItems(items);

                    const Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height(), colours[1], 0,
                        {}, std::size(items), ddWidget->width() - 3);

                    gDropdown.items[EnumValue(_settings.noise.algorithm)].setChecked(true);
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
                    _settings.generator = static_cast<MapGenerator::HeightMapGenerator>(dropdownIndex);
                    invalidate();
                    break;
                case WIDX_SIMPLEX_TYPE_DROPDOWN:
                    _settings.noise.algorithm = static_cast<MapGenerator::NoiseAlgorithm>(dropdownIndex);
                    invalidate();
                    break;
                case WIDX_BIAS_TYPE_DROPDOWN:
                    _settings.noise.bias.type = static_cast<MapGenerator::Bias>(dropdownIndex);
                    invalidate();
                    break;
            }
        }

        void BaseTextInput(WidgetIndex widgetIndex, int32_t value)
        {
            if (_settings.generator == MapGenerator::HeightMapGenerator::noise)
            {
                SimplexTextInput(widgetIndex, value);
                BiasTextInput(widgetIndex, value);
            }
            else if (_settings.generator == MapGenerator::HeightMapGenerator::image)
            {
                HeightmapTextInput(widgetIndex, value);
            }

            switch (widgetIndex)
            {
                case WIDX_MAP_SIZE_Y:
                case WIDX_MAP_SIZE_X:
                    // The practical size is 2 lower than the technical size
                    const auto technicalSize = std::clamp<uint16_t>(
                        value + 2, kMinimumMapSizeTechnical, kMaximumMapSizeTechnical);
                    if (_resizeDirection == ResizeDirection::y || _mapWidthAndHeightLinked)
                        _settings.mapSize.y = technicalSize;
                    if (_resizeDirection == ResizeDirection::x || _mapWidthAndHeightLinked)
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

            setCheckboxValue(WIDX_MAP_SEED_RANDOM, _random_seed);
            widgets[WIDX_MAP_SEED].string = _settings.seed.data();
            widgets[WIDX_MAP_SEED].type = _random_seed ? WidgetType::empty : WidgetType::textBox;

            const bool isHeightMapImage = _settings.generator == MapGenerator::HeightMapGenerator::image;
            setWidgetDisabled(WIDX_MAP_SIZE_Y, isHeightMapImage);
            setWidgetDisabled(WIDX_MAP_SIZE_Y_UP, isHeightMapImage);
            setWidgetDisabled(WIDX_MAP_SIZE_Y_DOWN, isHeightMapImage);
            setWidgetDisabled(WIDX_MAP_SIZE_LINK, isHeightMapImage);
            setWidgetDisabled(WIDX_MAP_SIZE_X, isHeightMapImage);
            setWidgetDisabled(WIDX_MAP_SIZE_X_UP, isHeightMapImage);
            setWidgetDisabled(WIDX_MAP_SIZE_X_DOWN, isHeightMapImage);

            // Enable heightmap widgets if one is loaded
            const bool heightmapEnabled = isHeightMapImage && _heightmapLoaded;
            setWidgetEnabled(WIDX_HEIGHTMAP_NORMALIZE, heightmapEnabled);

            const auto isNoneBias = _settings.noise.bias.type == MapGenerator::Bias::none;
            setWidgetDisabled(WIDX_BIAS_STRENGTH, isNoneBias);
            setWidgetDisabled(WIDX_BIAS_STRENGTH_UP, isNoneBias);
            setWidgetDisabled(WIDX_BIAS_STRENGTH_DOWN, isNoneBias);

            _xSpinnerCaption = std::to_string(_settings.mapSize.x - 2);
            widgets[WIDX_MAP_SIZE_X].setString(_xSpinnerCaption.c_str());
            _ySpinnerCaption = std::to_string(_settings.mapSize.y - 2);
            widgets[WIDX_MAP_SIZE_Y].setString(_ySpinnerCaption.c_str());

            auto& sourceWidget = widgets[WIDX_HEIGHTMAP_SOURCE];
            switch (_settings.generator)
            {
                case MapGenerator::HeightMapGenerator::flat:
                    sourceWidget.text = STR_HEIGHTMAP_FLATLAND;
                    ToggleSimplexWidgets(false);
                    ToggleHeightmapWidgets(false);
                    ToggleBiasWidgets(false);
                    break;

                case MapGenerator::HeightMapGenerator::noise:
                    sourceWidget.text = STR_HEIGHTMAP_NOISE;
                    ToggleSimplexWidgets(true);
                    ToggleHeightmapWidgets(false);
                    ToggleBiasWidgets(true);
                    break;

                case MapGenerator::HeightMapGenerator::image:
                    sourceWidget.text = STR_HEIGHTMAP_FILE;
                    ToggleSimplexWidgets(false);
                    ToggleHeightmapWidgets(true);
                    ToggleBiasWidgets(false);
                    HeightmapPrepareDraw();
                    break;
            }

            switch (_settings.noise.algorithm)
            {
                case MapGenerator::NoiseAlgorithm::simplex:
                    widgets[WIDX_SIMPLEX_TYPE].text = STR_HEIGHTMAP_SIMPLEX_NOISE;
                    break;
                case MapGenerator::NoiseAlgorithm::warped:
                    widgets[WIDX_SIMPLEX_TYPE].text = STR_HEIGHTMAP_WARPED_NOISE;
                    break;
                case MapGenerator::NoiseAlgorithm::ridged:
                    widgets[WIDX_SIMPLEX_TYPE].text = STR_HEIGHTMAP_RIDGED_NOISE;
                    break;
                case MapGenerator::NoiseAlgorithm::voronoi:
                    widgets[WIDX_SIMPLEX_TYPE].text = STR_HEIGHTMAP_VORONOI_NOISE;
                    break;
            }

            switch (_settings.noise.bias.type)
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
                case MapGenerator::Bias::terrace:
                    widgets[WIDX_BIAS_TYPE].text = STR_MAPGEN_BIAS_TYPE_TERRACE;
                    break;
            }
        }

        void ToggleSimplexWidgets(const bool state)
        {
            widgets[WIDX_SIMPLEX_GROUP].setVisible(state);
            widgets[WIDX_SIMPLEX_TYPE].type           = state ? WidgetType::dropdownMenu : WidgetType::empty;
            widgets[WIDX_SIMPLEX_TYPE_DROPDOWN].type  = state ? WidgetType::button       : WidgetType::empty;
            widgets[WIDX_SIMPLEX_BASE_FREQ].setVisible(state);
            widgets[WIDX_SIMPLEX_BASE_FREQ_UP].setVisible(state);
            widgets[WIDX_SIMPLEX_BASE_FREQ_DOWN].setVisible(state);
            widgets[WIDX_SIMPLEX_OCTAVES].setVisible(state);
            widgets[WIDX_SIMPLEX_OCTAVES_UP].setVisible(state);
            widgets[WIDX_SIMPLEX_OCTAVES_DOWN].setVisible(state);
        }

        void ToggleBiasWidgets(const bool state)
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

        void ToggleHeightmapWidgets(const bool state)
        {
            widgets[WIDX_HEIGHTMAP_GROUP].setVisible(state);
            widgets[WIDX_HEIGHTMAP_BROWSE].setVisible(state);
            widgets[WIDX_HEIGHTMAP_NORMALIZE].setVisible(state);
        }

        void BaseDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            if (_settings.generator == MapGenerator::HeightMapGenerator::noise)
            {
                SimplexDraw(rt);
                BiasDraw(rt);
            }
            else if (_settings.generator == MapGenerator::HeightMapGenerator::image)
            {
                HeightmapDraw(rt);
            }

            const auto enabledColour = colours[1];
            const auto disabledColour = enabledColour.withFlag(ColourFlag::inset, true);

            {
                auto textColour = isWidgetDisabled(WIDX_MAP_SIZE_Y) ? disabledColour : enabledColour;
                drawText(rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_MAP_SIZE_Y].top + 1 }, STR_MAP_SIZE, { textColour });
            }

            {
                auto textColour = enabledColour;
                drawText(
                    rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_SOURCE].top + 1 }, STR_HEIGHTMAP_SOURCE,
                    { textColour });
            }

            if (_random_seed)
            {
                drawText(
                    rt, windowPos + ScreenCoordsXY{ 153, widgets[WIDX_MAP_SEED].top + 1 }, _settings.seed.data(),
                    { disabledColour });
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
                invalidate();
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
                    invalidate();
                    break;
                }
                case WIDX_RULE_SC_REMOVE:
                {
                    if (_selectedScRule != -1)
                    {
                        _settings.sceneryRules.erase(_settings.sceneryRules.begin() + _selectedScRule);
                        SetSelectedSceneryRule(_selectedScRule - 1);
                        invalidate();
                    }
                    break;
                }
                case WIDX_RULE_SC_RENAME:
                {
                    if (_selectedScRule != -1)
                    {
                        const auto& selectedRule = _settings.sceneryRules[_selectedScRule];
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
                        invalidate();
                    }
                    break;
                }
                case WIDX_RULE_SC_MOVE_DOWN:
                {
                    if (_selectedScRule != -1 && _selectedScRule > 0)
                    {
                        std::swap(_settings.sceneryRules[_selectedScRule], _settings.sceneryRules[_selectedScRule - 1]);
                        SetSelectedSceneryRule(_selectedScRule - 1);
                        invalidate();
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
                        invalidate();
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
                            invalidate();
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
                            invalidate();
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

                    const Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        {}, std::size(items));
                    break;
                }
                case WIDX_RULE_SC_CONDITION_ADD:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        ItemExt(0, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_ABSOLUTE),
                        ItemExt(1, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_RELATIVE),
                        ItemExt(2, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO),
                        ItemExt(3, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_NOISE),
                        ItemExt(4, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_NORMAL_ANGLE),
                        ItemExt(5, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_PRNG),
                        ItemExt(6, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_BLEND_HEIGHT),
                        ItemExt(7, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_BLEND_NOISE),
                        ItemExt(8, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_BLEND_DISTANCE_TO),
                        ItemExt(9, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_LAND_STYLE),
                    };

                    SetItems(items);

                    const Widget* ddWidget = &widgets[widgetIndex];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        {}, std::size(items));

                    // TODO implement
                    gDropdown.items[4].setDisabled(true);

                    break;
                }
            }
        }

        void SceneryUpdate()
        {
            // Tab animation
            if (++currentFrame >= TabAnimationLoops[page])
                currentFrame = 0;
            invalidateWidget(WIDX_TAB_5);

            // Check if the mouse is hovering over the list
            if (!widgetIsHighlighted(*this, WIDX_RULE_SC_SCROLL))
            {
                if (_highlightedScRule != -1)
                    invalidateWidget(WIDX_RULE_SC_SCROLL);
                _highlightedScRule = -1;
            }

            // Check if the mouse is hovering over the list
            if (!widgetIsHighlighted(*this, WIDX_RULE_SC_CONDITION_SCROLL))
            {
                if (_highlightedScCondition != -1)
                    invalidateWidget(WIDX_RULE_SC_CONDITION_SCROLL);
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

            invalidate();
        }

        void SceneryPrepareDraw()
        {
            const bool ruleSelected = _selectedScRule != -1;
            const bool condSelected = ruleSelected && _selectedScCondition != -1;

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
                setWidgetEnabled(
                    WIDX_RULE_SC_MOVE_UP, _selectedScRule < static_cast<int32_t>(_settings.sceneryRules.size() - 1));
                setWidgetEnabled(WIDX_RULE_SC_MOVE_DOWN, _selectedScRule > 0);

                setWidgetEnabled(WIDX_RULE_SC_CONDITION_REMOVE, condSelected);
                setWidgetEnabled(WIDX_RULE_SC_CONDITION_EDIT, condSelected);
            }
        }

        void SceneryDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
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
                const auto& ruleScroll = widgets[WIDX_RULE_SC_SCROLL];
                const auto centrePos = windowPos + ScreenCoordsXY{ ruleScroll.left, ruleScroll.bottom + 5 };
                const auto textPaint = TextPaint{ colours[1] };
                drawTextWrapped(rt, centrePos, ruleScroll.width(), STR_MAPGEN_RULE_SC_HINT, {}, textPaint);
            }
        }

        void SceneryDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_RULE_SC_NEW_PRESET:
                {
                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdown.highlightedIndex;

                    if (dropdownIndex != -1)
                    {
                        const auto preset = static_cast<MapGenerator::Rule::SceneryRulePreset>(dropdownIndex);
                        MapGenerator::Rule::createNewSceneryRuleFromPreset(_settings, preset);
                        SetSelectedSceneryRule(static_cast<int32_t>(_settings.sceneryRules.size() - 1));
                    }

                    invalidate();
                    break;
                }
                case WIDX_RULE_SC_CONDITION_ADD:
                {
                    if (_selectedScRule == -1)
                    {
                        return;
                    }

                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdown.highlightedIndex;

                    if (dropdownIndex == -1)
                    {
                        return;
                    }

                    const auto conditionType = static_cast<MapGenerator::Rule::Type>(dropdownIndex);
                    auto newCondition = createNewCondition(conditionType);
                    auto callback = [this](MapGenerator::Rule::Condition& condition) {
                        if (_selectedScRule == -1)
                        {
                            return;
                        }
                        auto& selectedRule = _settings.sceneryRules[_selectedScRule];
                        selectedRule.conditions.push_back(std::move(condition));
                        SetSelectedSceneryCondition(static_cast<int32_t>(selectedRule.conditions.size() - 1));
                        invalidate();
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
                    const auto& selectedRule = _settings.sceneryRules[_selectedScRule];

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
            const uint8_t direction = item.direction.value_or(0);

            switch (item.type)
            {
                case MapGenerator::Rule::Small:
                {
                    const auto sceneryEntry = OpenRCT2::ObjectEntryManager::GetObjectEntry<SmallSceneryEntry>(item.index);
                    auto imageId = ImageId(sceneryEntry->image + direction);

                    if (sceneryEntry->flags.has(SmallSceneryFlag::hasPrimaryColour))
                    {
                        imageId = imageId.WithPrimary(item.colours[0]);
                    }
                    if (sceneryEntry->flags.has(SmallSceneryFlag::hasSecondaryColour))
                    {
                        imageId = imageId.WithSecondary(item.colours[1]);
                    }
                    if (sceneryEntry->flags.has(SmallSceneryFlag::hasTertiaryColour))
                    {
                        imageId = imageId.WithTertiary(item.colours[2]);
                    }

                    auto spriteTop = (targetSize.y / 2) + (sceneryEntry->height / 2);
                    if (sceneryEntry->flags.hasAll(SmallSceneryFlag::occupiesFullTile, SmallSceneryFlag::vOffsetCentre))
                    {
                        spriteTop -= 12;
                    }

                    if (item.weight == 0)
                    {
                        imageId = imageId.WithRemap(FilterPaletteID::paletteGhost);
                    }

                    const auto spriteOffsetX = static_cast<int32_t>(targetSize.x / (count + 1) * (idx + 1));
                    const auto spritePosition = ScreenCoordsXY{ spriteOffsetX, spriteTop };

                    GfxDrawSprite(rt, imageId, spritePosition);

                    if (sceneryEntry->flags.has(SmallSceneryFlag::hasGlass))
                    {
                        imageId = ImageId(sceneryEntry->image + 4 + direction).WithTransparency(item.colours[0]);
                        GfxDrawSprite(rt, imageId, spritePosition);
                    }

                    if (sceneryEntry->flags.has(SmallSceneryFlag::isAnimated))
                    {
                        imageId = ImageId(sceneryEntry->image + 4 + direction);
                        GfxDrawSprite(rt, imageId, spritePosition);
                    }
                    break;
                }
                case MapGenerator::Rule::Large:
                {
                    // TODO
                    break;
                }
                case MapGenerator::Rule::Wall:
                {
                    const auto wallEntry = ObjectEntryManager::GetObjectEntry<WallSceneryEntry>(item.index);
                    if (wallEntry == nullptr)
                        return;

                    auto directionImgOffset = 0;
                    auto directionPosOffset = 0;
                    switch (direction)
                    {
                        case 0 /* NE */:
                            directionImgOffset = 1;
                            directionPosOffset = -16;
                            break;
                        case 1 /* SE */:
                            directionImgOffset = 0;
                            directionPosOffset = 16;
                            break;
                        case 2 /* SW */:
                            directionImgOffset = 1;
                            directionPosOffset = -16;
                            break;
                        case 3 /* NW */:
                            directionImgOffset = 0;
                            directionPosOffset = 16;
                            break;
                    }

                    auto imageId = ImageId(wallEntry->image + directionImgOffset);

                    if (wallEntry->flags.has(WallSceneryFlag::hasPrimaryColour))
                    {
                        imageId = imageId.WithPrimary(item.colours[0]);
                    }
                    if (wallEntry->flags.has(WallSceneryFlag::hasSecondaryColour))
                    {
                        imageId = imageId.WithSecondary(item.colours[1]);
                    }
                    if (wallEntry->flags.has(WallSceneryFlag::hasTertiaryColour))
                    {
                        imageId = imageId.WithTertiary(item.colours[2]);
                    }

                    if (item.weight == 0)
                    {
                        imageId = imageId.WithRemap(FilterPaletteID::paletteGhost);
                    }

                    const auto spriteTop = (targetSize.y / 2) + (wallEntry->height / 2);
                    const auto spriteOffsetX = static_cast<int32_t>(targetSize.x / (count + 1) * (idx + 1));
                    const auto spritePosition = ScreenCoordsXY{ spriteOffsetX + directionPosOffset, spriteTop };

                    GfxDrawSprite(rt, imageId, spritePosition);

                    if (wallEntry->flags.has(WallSceneryFlag::isDoor))
                    {
                        GfxDrawSprite(rt, imageId.WithIndexOffset(1), spritePosition);
                    }
                    if (wallEntry->flags.has(WallSceneryFlag::hasGlass))
                    {
                        const auto glassImageId = ImageId(wallEntry->image + 6).WithTransparency(item.colours[1]);
                        GfxDrawSprite(rt, glassImageId, spritePosition);
                    }

                    break;
                }
            }
        }

        void SceneryDrawItems(
            RenderTarget& rt, MapGenerator::Rule::SceneryRule& rule, ScreenCoordsXY& pos, ScreenCoordsXY& size)
        {
            RenderTarget clippedRT;
            if (ClipRenderTarget(clippedRT, rt, pos, size.x, size.y))
            {
                const size_t limitedItemCount = std::min<size_t>(4, rule.effect.objects.size());
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

        StringId heightSourceToStringId(const MapGenerator::Rule::HeightSource& source)
        {
            switch (source)
            {
                case MapGenerator::Rule::HeightSource::SelfLand:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_SELF_LAND;
                case MapGenerator::Rule::HeightSource::SelfWater:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_SELF_WATER;
                case MapGenerator::Rule::HeightSource::SelfClearance:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_SELF_CLEARANCE;
                case MapGenerator::Rule::HeightSource::NeighbourNWLand:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NW_LAND;
                case MapGenerator::Rule::HeightSource::NeighbourNWWater:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NW_WATER;
                case MapGenerator::Rule::HeightSource::NeighbourNWClearance:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NW_CLEARANCE;
                case MapGenerator::Rule::HeightSource::NeighbourNELand:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NE_LAND;
                case MapGenerator::Rule::HeightSource::NeighbourNEWater:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NE_WATER;
                case MapGenerator::Rule::HeightSource::NeighbourNEClearance:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NE_CLEARANCE;
                case MapGenerator::Rule::HeightSource::NeighbourSELand:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SE_LAND;
                case MapGenerator::Rule::HeightSource::NeighbourSEWater:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SE_WATER;
                case MapGenerator::Rule::HeightSource::NeighbourSEClearance:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SE_CLEARANCE;
                case MapGenerator::Rule::HeightSource::NeighbourSWLand:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SW_LAND;
                case MapGenerator::Rule::HeightSource::NeighbourSWWater:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SW_WATER;
                case MapGenerator::Rule::HeightSource::NeighbourSWClearance:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SW_CLEARANCE;
                case MapGenerator::Rule::HeightSource::GlobalMin:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_MIN;
                case MapGenerator::Rule::HeightSource::GlobalMax:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_MAX;
                case MapGenerator::Rule::HeightSource::GlobalWaterLevel:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_WATER_LEVEL;
                default:
                    throw std::runtime_error("unknown height source");
            }
        }

        StringId featureToStringId(MapGenerator::Rule::Feature& feature)
        {
            switch (feature)
            {
                case MapGenerator::Rule::Feature::Water:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_WATER;
                case MapGenerator::Rule::Feature::River:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_RIVER;
                case MapGenerator::Rule::Feature::Sea:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_SEA;
                case MapGenerator::Rule::Feature::MapBorder:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_BORDER;
                case MapGenerator::Rule::Feature::Fill:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_FILL;
                case MapGenerator::Rule::Feature::Breach:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_BREACH;
                case MapGenerator::Rule::Feature::Land:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_LAND;
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
            Rectangle::fill(
                rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } }, getColourMap(colours[1].colour).midLight,
                true);

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
                    Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).midDark);
                    stringFormat = STR_WHITE_STRING;
                }
                else if (i == _highlightedScCondition)
                {
                    Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).midDark, true);
                    stringFormat = STR_WHITE_STRING;
                }

                auto checkboxFormatter = Formatter();
                checkboxFormatter.Add<StringId>(STR_STRING);
                checkboxFormatter.Add<char*>(kCheckMarkString);

                // Draw enabled checkbox and check
                Rectangle::fillInset(
                    rt, { { 2, screenCoords.y }, { 15, screenCoords.y + 11 } }, colours[1], Rectangle::BorderStyle::inset,
                    Rectangle::FillBrightness::dark, Rectangle::FillMode::dontLightenWhenInset);
                if (condition.enabled)
                {
                    drawText(rt, screenCoords + ScreenCoordsXY{ 4, 1 }, stringFormat, checkboxFormatter);
                }

                // Draw land styles
                if (condition.type == MapGenerator::Rule::Type::LandStyle)
                {
                    auto& landStyleData = std::get<MapGenerator::Rule::LandStyleData>(condition.data);
                    std::vector<ObjectEntryIndex> orderedStyles;
                    std::ranges::copy(landStyleData.styles, std::back_inserter(orderedStyles));
                    std::ranges::sort(orderedStyles);

                    const ImageId maskImage(SPR_G2_MASK_MAPGEN_TEXTURE_SCROLL);
                    const auto styleCount = std::min<size_t>(4, orderedStyles.size());
                    const auto styleWidth = 24;
                    const auto baseOffset = listWidth - kScrollBarWidth - styleCount * styleWidth;

                    for (size_t s = 0; s < styleCount; ++s)
                    {
                        const auto offset = static_cast<int32_t>(baseOffset + s * styleWidth);
                        auto imgCoords = screenCoords + ScreenCoordsXY{ offset, 0 };
                        const auto surfaceImage = LookupSurfaceImage(orderedStyles[s]);
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
                    {
                        auto& heightData = std::get<MapGenerator::Rule::HeightAbsoluteData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FEATURE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_HEIGHT_MODE_ABSOLUTE);
                        ft.Add<StringId>(heightSourceToStringId(heightData.source));
                        ft.Add<StringId>(predRepr);
                        ft.Add<int16_t>(static_cast<int16_t>(BaseZToMetres(heightData.height)));
                        break;
                    }
                    case MapGenerator::Rule::Type::HeightRelative:
                    {
                        auto& heightData = std::get<MapGenerator::Rule::HeightRelativeData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_HEIGHT_RELATIVE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_HEIGHT_RELATIVE);
                        ft.Add<StringId>(heightSourceToStringId(heightData.sourceFirst));
                        ft.Add<StringId>(heightSourceToStringId(heightData.sourceSecond));
                        ft.Add<StringId>(predRepr);
                        ft.Add<int16_t>(static_cast<int16_t>(HeightUnitsToMetres(heightData.height)));
                        break;
                    }
                    case MapGenerator::Rule::Type::Distance:
                    {
                        auto& distanceData = std::get<MapGenerator::Rule::DistanceData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FEATURE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_DISTANCE_TO);
                        ft.Add<StringId>(featureToStringId(distanceData.feature));
                        ft.Add<StringId>(predRepr);
                        ft.Add<int16_t>(static_cast<int16_t>(TileUnitsToMetres(distanceData.distance)));
                        break;
                    }
                    case MapGenerator::Rule::Type::Noise:
                    {
                        auto& noiseData = std::get<MapGenerator::Rule::NoiseData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_NOISE);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int32_t>(static_cast<int32_t>(noiseData.value * 100));
                        break;
                    }
                    case MapGenerator::Rule::Type::NormalAngle:
                    {
                        auto& normalAngleData = std::get<MapGenerator::Rule::NormalAngleData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_NORMAL_ANGLE);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int32_t>(static_cast<int32_t>(normalAngleData.angle * 100));
                        break;
                    }
                    case MapGenerator::Rule::Type::Random:
                    {
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        ft.Add<StringId>(predRepr);
                        auto& randomData = std::get<MapGenerator::Rule::RandomData>(condition.data);
                        ft.Add<int32_t>(static_cast<int32_t>(randomData.value * 100));
                        break;
                    }
                    case MapGenerator::Rule::Type::BlendHeight:
                    {
                        auto& blendHeightData = std::get<MapGenerator::Rule::BlendHeightData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_BLEND_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_BLEND_HEIGHT);
                        ft.Add<int16_t>(static_cast<int16_t>(BaseZToMetres(blendHeightData.edgeLow)));
                        ft.Add<int16_t>(static_cast<int16_t>(BaseZToMetres(blendHeightData.edgeHigh)));
                        ft.Add<StringId>(predRepr);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        break;
                    }
                    case MapGenerator::Rule::Type::BlendNoise:
                    {
                        auto& blendNoiseData = std::get<MapGenerator::Rule::BlendNoiseData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_BLEND_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_BLEND_NOISE);
                        ft.Add<int32_t>(static_cast<int32_t>(blendNoiseData.edgeLow * 100));
                        ft.Add<int32_t>(static_cast<int32_t>(blendNoiseData.edgeHigh * 100));
                        ft.Add<StringId>(predRepr);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        break;
                    }
                    case MapGenerator::Rule::Type::BlendDistance:
                    {
                        auto& blendDistanceData = std::get<MapGenerator::Rule::BlendDistanceData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_BLEND_DISTANCE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_BLEND_DISTANCE_TO);
                        ft.Add<StringId>(featureToStringId(blendDistanceData.feature));
                        ft.Add<int16_t>(TileUnitsToMetres(blendDistanceData.edgeLow));
                        ft.Add<int16_t>(TileUnitsToMetres(blendDistanceData.edgeHigh));
                        ft.Add<StringId>(predRepr);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        break;
                    }
                    case MapGenerator::Rule::Type::LandStyle:
                    {
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_LAND_STYLE_FMT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_LAND_STYLE);
                        ft.Add<StringId>(
                            condition.predicate == MapGenerator::Rule::Predicate::Equal ? STR_MAPGEN_RULE_PREDICATE_IN
                                                                                        : STR_MAPGEN_RULE_PREDICATE_NOT_IN);
                        break;
                    }
                }

                // Draw description
                drawTextEllipsised(rt, screenCoords + ScreenCoordsXY{ 20, 0 }, 200, stringFormat, ft);

                screenCoords.y -= kScrollableRowHeight;
            }
        }

        void SceneryScrollDrawRule(RenderTarget& rt)
        {
            const int32_t listWidth = widgets[WIDX_RULE_SC_SCROLL].width();
            Rectangle::fill(
                rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } },
                getColourMap(colours[1].colour).midLight);

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
                    Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).midDark);
                    stringFormat = STR_WHITE_STRING;
                }
                else if (i == _highlightedScRule)
                {
                    Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).midDark, true);
                    stringFormat = STR_WHITE_STRING;
                }

                auto checkboxFormatter = Formatter();
                checkboxFormatter.Add<StringId>(STR_STRING);
                checkboxFormatter.Add<char*>(kCheckMarkString);

                // Draw enabled checkbox and check
                Rectangle::fillInset(
                    rt, { { 2, screenCoords.y }, { 15, screenCoords.y + 11 } }, colours[1], Rectangle::BorderStyle::inset,
                    Rectangle::FillBrightness::dark, Rectangle::FillMode::dontLightenWhenInset);
                if (rule.enabled)
                {
                    drawText(rt, screenCoords + ScreenCoordsXY{ 4, 1 }, stringFormat, checkboxFormatter);
                }

                // Rule name
                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<char*>(rule.name.c_str());
                drawTextEllipsised(rt, screenCoords + ScreenCoordsXY{ 20, 0 }, 200, stringFormat, ft);

                // Preview
                ScreenCoordsXY sceneryPreviewPos = screenCoords + ScreenCoordsXY{ 218, 0 };
                ScreenCoordsXY sceneryPreviewOutcomeSize = { 80, kScrollableRowHeight };
                SceneryDrawItems(rt, rule, sceneryPreviewPos, sceneryPreviewOutcomeSize);

                screenCoords.y -= kScrollableRowHeight;
            }
        }

        void SceneryScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
            if (scrollIndex == 0)
            {
                const int32_t index = static_cast<int32_t>(
                    _settings.sceneryRules.size() - (screenCoords.y - 1) / kScrollableRowHeight - 1);
                if (index < 0 || index >= static_cast<int32_t>(_settings.sceneryRules.size()))
                {
                    _highlightedScRule = -1;
                }
                else
                {
                    _highlightedScRule = index;
                }
                invalidateWidget(WIDX_RULE_SC_SCROLL);
            }
            else if (scrollIndex == 1)
            {
                if (_selectedScRule == -1)
                {
                    _highlightedScCondition = -1;
                }
                else
                {
                    const auto& selectedRule = _settings.sceneryRules[_selectedScRule];

                    const int32_t index = static_cast<int32_t>(
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
                invalidateWidget(WIDX_RULE_SC_CONDITION_SCROLL);
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
                        _settings.noise.baseFrequency, 4);
                    break;
                }

                case WIDX_SIMPLEX_OCTAVES:
                {
                    Formatter ft;
                    ft.Add<int16_t>(1);
                    ft.Add<int16_t>(10);
                    WindowTextInputOpen(
                        this, widgetIndex, STR_SIMPLEX_OCTAVES, STR_ENTER_OCTAVES, ft, STR_FORMAT_INTEGER,
                        _settings.noise.octaves, 10);
                    break;
                }
            }
        }

        void SimplexMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_SIMPLEX_BASE_FREQ_UP:
                    _settings.noise.baseFrequency.increment();
                    invalidate();
                    break;
                case WIDX_SIMPLEX_BASE_FREQ_DOWN:
                    _settings.noise.baseFrequency.decrement();
                    invalidate();
                    break;
                case WIDX_SIMPLEX_OCTAVES_UP:
                    _settings.noise.octaves.increment();
                    invalidate();
                    break;
                case WIDX_SIMPLEX_OCTAVES_DOWN:
                    _settings.noise.octaves.decrement();
                    invalidate();
                    break;
            }
        }

        void SimplexDraw(RenderTarget& rt)
        {
            const auto textColour = colours[1];

            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_SIMPLEX_TYPE].top + 1 }, STR_HEIGHTMAP_NOISE_TYPE,
                { textColour });

            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1 },
                STR_MAPGEN_SIMPLEX_NOISE_BASE_FREQUENCY, { textColour });
            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_SIMPLEX_OCTAVES].top + 1 }, STR_MAPGEN_SIMPLEX_NOISE_OCTAVES,
                {}, { textColour });

            auto ft = Formatter();
            ft.Add<uint16_t>(_settings.noise.baseFrequency);
            drawText(
                rt,
                windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_BASE_FREQ].left + 1, widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1 },
                STR_WINDOW_COLOUR_2_COMMA2DP32, ft, { textColour });

            ft = Formatter();
            ft.Add<uint16_t>(_settings.noise.octaves);
            drawText(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_SIMPLEX_OCTAVES].left + 1, widgets[WIDX_SIMPLEX_OCTAVES].top + 1 },
                STR_COMMA16, ft, { textColour });
        }

        void SimplexTextInput(WidgetIndex widgetIndex, int32_t value)
        {
            switch (widgetIndex)
            {
                case WIDX_SIMPLEX_BASE_FREQ:
                    _settings.noise.baseFrequency = value;
                    break;

                case WIDX_SIMPLEX_OCTAVES:
                    _settings.noise.octaves = value;
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
                        _settings.noise.bias.strength, 4);
                    break;
                }
            }
        }

        void BiasMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_BIAS_STRENGTH_UP:
                    _settings.noise.bias.strength.increment();
                    invalidate();
                    break;
                case WIDX_BIAS_STRENGTH_DOWN:
                    _settings.noise.bias.strength.decrement();
                    invalidate();
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
                        ToggleOption(9, STR_MAPGEN_BIAS_TYPE_TERRACE),
                        //clang-format on
                    };

                    SetItems(items);

                    const Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1], 0,
                        {}, std::size(items), ddWidget->width() - 2);

                    setCheckboxValue(EnumValue(_settings.noise.bias.type), true);
                    break;
                }
            }
        }

        void BiasDraw(RenderTarget& rt)
        {
            const auto enabledColour = colours[1];
            const auto disabledColour = enabledColour.withFlag(ColourFlag::inset, true);
            const bool strengthDisabled = _settings.noise.bias.type == MapGenerator::Bias::none;
            const auto strengthColour = strengthDisabled ? disabledColour : enabledColour;

            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_BIAS_TYPE].top + 1 }, STR_MAPGEN_BIAS_TYPE, {},
                { enabledColour });

            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_BIAS_STRENGTH].top + 1 }, STR_MAPGEN_BIAS_STRENGTH, {},
                { strengthColour });

            auto ft = Formatter();
            ft.Add<uint16_t>(_settings.noise.bias.strength);
            drawText(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_BIAS_STRENGTH].left + 1, widgets[WIDX_BIAS_STRENGTH].top + 1 },
                STR_COMMA2DP32, ft, { strengthColour });
        }

        void BiasTextInput(WidgetIndex widgetIndex, int32_t value)
        {
            switch (widgetIndex)
            {
                case WIDX_BIAS_STRENGTH:
                    _settings.noise.bias.strength = value;
                    break;
            }
        }

#pragma endregion

#pragma region Heightmap settings, part of generator tab

        void HeightmapMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
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

                case WIDX_HEIGHTMAP_NORMALIZE:
                    _settings.normalizeHeight = !_settings.normalizeHeight;
                    setCheckboxValue(WIDX_HEIGHTMAP_NORMALIZE, _settings.normalizeHeight);
                    invalidateWidget(WIDX_HEIGHTMAP_NORMALIZE);
                    break;
            }
        }

        void HeightmapPrepareDraw()
        {
            setCheckboxValue(WIDX_HEIGHTMAP_NORMALIZE, _settings.normalizeHeight);
        }

        void HeightmapDraw(RenderTarget& rt)
        {
            // Current heightmap image filename
            auto ft = Formatter();
            if (!_heightmapLoaded)
                ft.Add<char*>(LanguageGetString(STR_MAPGEN_NONE_SELECTED));
            else
                ft.Add<char*>(_heightmapFilename.c_str());

            const auto pos = ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_BROWSE].top + 1 };
            const auto textWidth = widgets[WIDX_HEIGHTMAP_BROWSE].left - 11;
            drawTextEllipsised(rt, windowPos + pos, textWidth, STR_MAPGEN_CURRENT_HEIGHTMAP_FILE, ft);
        }

        void HeightmapTextInput(WidgetIndex widgetIndex, int32_t value)
        {
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
            }
        }

        void TerrainMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_HEIGHTMAP_LOW_UP:
                    _settings.heightmapLow.increment();
                    _settings.heightmapHigh = std::max(_settings.heightmapHigh.get(), _settings.heightmapLow.get() + 2);
                    invalidateWidget(WIDX_HEIGHTMAP_LOW);
                    break;
                case WIDX_HEIGHTMAP_LOW_DOWN:
                    _settings.heightmapLow.decrement();
                    invalidateWidget(WIDX_HEIGHTMAP_LOW);
                    break;
                case WIDX_HEIGHTMAP_HIGH_UP:
                    _settings.heightmapHigh.increment();
                    invalidateWidget(WIDX_HEIGHTMAP_HIGH);
                    break;
                case WIDX_HEIGHTMAP_HIGH_DOWN:
                    _settings.heightmapHigh.decrement();
                    _settings.heightmapLow = std::min(_settings.heightmapLow.get(), _settings.heightmapHigh.get() - 2);
                    invalidateWidget(WIDX_HEIGHTMAP_HIGH);
                    break;
                case WIDX_HEIGHTMAP_TRANSFORM_STRENGTH_DOWN:
                    _settings.filter.strength.decrement();
                    invalidateWidget(WIDX_HEIGHTMAP_TRANSFORM_STRENGTH);
                    break;
                case WIDX_HEIGHTMAP_TRANSFORM_STRENGTH_UP:
                    _settings.filter.strength.increment();
                    invalidateWidget(WIDX_HEIGHTMAP_TRANSFORM_STRENGTH);
                    break;
                case WIDX_HEIGHTMAP_TRANSFORM_TYPE_DROPDOWN:
                    {
                        using namespace Dropdown;

                        constexpr ItemExt items[] = {
                            ToggleOption(0, STR_MAPGEN_TRANSFORM_TYPE_NONE),
                            ToggleOption(1, STR_MAPGEN_TRANSFORM_TYPE_BOX),
                            ToggleOption(2, STR_MAPGEN_TRANSFORM_TYPE_GAUSSIAN),
                            ToggleOption(3, STR_MAPGEN_TRANSFORM_TYPE_SHARPEN),
                            ToggleOption(4, STR_MAPGEN_TRANSFORM_TYPE_BILATERAL),
                            ToggleOption(5, STR_MAPGEN_TRANSFORM_TYPE_EROSION),
                        };

                        SetItems(items);

                        const Widget* ddWidget = &widgets[widgetIndex - 1];
                        WindowDropdownShowTextCustomWidth(
                            { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height(), colours[1], 0,
                            {}, std::size(items), ddWidget->width() - 3);

                        gDropdown.items[EnumValue(_settings.filter.type)].setChecked(true);
                        break;
                    }
                case WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES_DROPDOWN:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        ToggleOption(0, STR_MAPGEN_SMOOTH_EDGE_NONE),
                        ToggleOption(1, STR_MAPGEN_SMOOTH_EDGE_WEAK),
                        ToggleOption(2, STR_MAPGEN_SMOOTH_EDGE_STRONG),
                    };

                    SetItems(items);

                    const Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height(), colours[1], 0,
                        {}, std::size(items), ddWidget->width() - 3);

                    gDropdown.items[EnumValue(_settings.slopeSmooth)].setChecked(true);
                    break;
                }
            }
        }

        void TerrainUpdate()
        {
            // Tab animation
            if (++currentFrame >= TabAnimationLoops[page])
                currentFrame = 0;
            invalidateWidget(WIDX_TAB_2);
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
                case WIDX_HEIGHTMAP_TRANSFORM_STRENGTH:
                    _settings.filter.strength = value;
                    break;
            }

            invalidate();
        }

        void TerrainDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (dropdownIndex == -1)
                return;

            switch (widgetIndex)
            {
                case WIDX_HEIGHTMAP_TRANSFORM_TYPE_DROPDOWN:
                    _settings.filter.type = static_cast<MapGenerator::Filter>(dropdownIndex);
                    invalidate();
                    break;
                case WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES_DROPDOWN:
                    _settings.slopeSmooth = static_cast<MapGenerator::SlopeSmooth>(dropdownIndex);
                    invalidate();
                    break;
            }
        }

        void DrawDropdownButton(RenderTarget& rt, WidgetIndex widgetIndex, ImageId image)
        {
            const auto& widget = widgets[widgetIndex];
            const ScreenCoordsXY pos = { windowPos.x + widget.left, windowPos.y + widget.top };
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

        ImageId LookupSurfaceImage(uint32_t surfaceTexture)
        {
            auto& objManager = GetContext()->GetObjectManager();
            const auto* surfaceObj = objManager.GetLoadedObject<TerrainSurfaceObject>(surfaceTexture);
            ImageId surfaceImage;
            if (surfaceObj != nullptr)
            {
                surfaceImage = ImageId(surfaceObj->IconImageId);
                if (surfaceObj->Colour != kColourNull)
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
            const bool isNotFlatland = _settings.generator != MapGenerator::HeightMapGenerator::flat;
            const bool isNoneTransform = _settings.filter.type == MapGenerator::Filter::none;

            // Max land height option is irrelevant for flatland
            setWidgetEnabled(WIDX_HEIGHTMAP_HIGH, isNotFlatland);
            setWidgetEnabled(WIDX_HEIGHTMAP_HIGH_UP, isNotFlatland);
            setWidgetEnabled(WIDX_HEIGHTMAP_HIGH_DOWN, isNotFlatland);

            // only offer terrain edge smoothing if we don't use flatland terrain
            setWidgetEnabled(WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES, isNotFlatland);
            setWidgetEnabled(WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES_DROPDOWN, isNotFlatland);

            // Transform can't be used with flatland
            setWidgetEnabled(WIDX_HEIGHTMAP_TRANSFORM_TYPE, isNotFlatland);
            setWidgetEnabled(WIDX_HEIGHTMAP_TRANSFORM_TYPE_DROPDOWN, isNotFlatland);

            setWidgetEnabled(WIDX_HEIGHTMAP_TRANSFORM_STRENGTH, isNotFlatland && !isNoneTransform);
            setWidgetEnabled(WIDX_HEIGHTMAP_TRANSFORM_STRENGTH_UP, isNotFlatland && !isNoneTransform);
            setWidgetEnabled(WIDX_HEIGHTMAP_TRANSFORM_STRENGTH_DOWN, isNotFlatland && !isNoneTransform);


            switch (_settings.filter.type)
            {
                case MapGenerator::Filter::none:
                    widgets[WIDX_HEIGHTMAP_TRANSFORM_TYPE].text = STR_MAPGEN_TRANSFORM_TYPE_NONE;
                    break;
                case MapGenerator::Filter::box:
                    widgets[WIDX_HEIGHTMAP_TRANSFORM_TYPE].text = STR_MAPGEN_TRANSFORM_TYPE_BOX;
                    break;
                case MapGenerator::Filter::gaussian:
                    widgets[WIDX_HEIGHTMAP_TRANSFORM_TYPE].text = STR_MAPGEN_TRANSFORM_TYPE_GAUSSIAN;
                    break;
                case MapGenerator::Filter::sharpen:
                    widgets[WIDX_HEIGHTMAP_TRANSFORM_TYPE].text = STR_MAPGEN_TRANSFORM_TYPE_SHARPEN;
                    break;
                case MapGenerator::Filter::bilateral:
                    widgets[WIDX_HEIGHTMAP_TRANSFORM_TYPE].text = STR_MAPGEN_TRANSFORM_TYPE_BILATERAL;
                    break;
                case MapGenerator::Filter::erosion:
                    widgets[WIDX_HEIGHTMAP_TRANSFORM_TYPE].text = STR_MAPGEN_TRANSFORM_TYPE_EROSION;
                    break;
            }

            switch (_settings.slopeSmooth)
            {
                case MapGenerator::SlopeSmooth::none:
                    widgets[WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES].text = STR_MAPGEN_SMOOTH_EDGE_NONE;
                    break;
                case MapGenerator::SlopeSmooth::weak:
                    widgets[WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES].text = STR_MAPGEN_SMOOTH_EDGE_WEAK;
                    break;
                case MapGenerator::SlopeSmooth::strong:
                    widgets[WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES].text = STR_MAPGEN_SMOOTH_EDGE_STRONG;
                    break;
            }
        }

        void TerrainDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            const auto enabledColour = colours[1];
            const auto disabledColour = enabledColour.withFlag(ColourFlag::inset, true);

            // Minimum land height label and value
            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_LOW].top + 1 }, STR_MAPGEN_MIN_LAND_HEIGHT,
                { enabledColour });

            auto ft = Formatter();
            ft.Add<int32_t>(BaseZToMetres(_settings.heightmapLow));
            drawText(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_LOW].left + 1, widgets[WIDX_HEIGHTMAP_LOW].top + 1 },
                STR_RIDE_LENGTH_ENTRY, ft, { enabledColour });

            const auto maxLandColour = isWidgetDisabled(WIDX_HEIGHTMAP_HIGH) ? disabledColour : enabledColour;

            // Maximum land height label and value
            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_HIGH].top + 1 }, STR_MAPGEN_MAX_LAND_HEIGHT,
                { maxLandColour });

            ft = Formatter();
            ft.Add<int32_t>(BaseZToMetres(_settings.heightmapHigh));
            drawText(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_HIGH].left + 1, widgets[WIDX_HEIGHTMAP_HIGH].top + 1 },
                STR_RIDE_LENGTH_ENTRY, ft, { maxLandColour });


            const auto transformColour = isWidgetDisabled(WIDX_HEIGHTMAP_TRANSFORM_STRENGTH) ? disabledColour   : enabledColour;
            // Transform type label
            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_TRANSFORM_TYPE].top + 1 }, STR_MAPGEN_TRANSFORM_TYPE, {},
                { transformColour });

            // Transform strength label and value
            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_TRANSFORM_STRENGTH].top + 1 }, STR_MAPGEN_TRANSFORM_STRENGTH, {},
                { transformColour });

            ft = Formatter();
            ft.Add<int32_t>(_settings.filter.strength);
            drawText(
                rt,
                windowPos
                    + ScreenCoordsXY{ widgets[WIDX_HEIGHTMAP_TRANSFORM_STRENGTH].left + 1,
                                      widgets[WIDX_HEIGHTMAP_TRANSFORM_STRENGTH].top + 1 },
                STR_FORMAT_INTEGER, ft, { transformColour });

            // Slope smooth type label
            const auto smoothColour = isWidgetDisabled(WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES) ? disabledColour   : enabledColour;
            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_HEIGHTMAP_SMOOTH_TILE_EDGES].top + 1 }, STR_MAPGEN_SMOOTH_TILE, {},
                { smoothColour });
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
                invalidate();
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
                        invalidate();
                    }
                    break;
                }
                case WIDX_RULE_TX_WALL_TEXTURE_CHECK:
                {
                    if (_selectedTxRule != -1)
                    {
                        auto& selectedRule = _settings.textureRules[_selectedTxRule];
                        selectedRule.effect.applyEdgeTexture = !selectedRule.effect.applyEdgeTexture;
                        invalidate();
                    }
                    break;
                }
                case WIDX_RULE_TX_NEW:
                {
                    MapGenerator::Rule::createNewTextureRule(_settings);
                    SetSelectedTextureRule(static_cast<int32_t>(_settings.textureRules.size() - 1));
                    invalidate();
                    break;
                }
                case WIDX_RULE_TX_REMOVE:
                {
                    if (_selectedTxRule != -1 && !_settings.textureRules[_selectedTxRule].isDefault)
                    {
                        _settings.textureRules.erase(_settings.textureRules.begin() + _selectedTxRule);
                        SetSelectedTextureRule(_selectedTxRule - 1);
                        invalidate();
                    }
                    break;
                }
                case WIDX_RULE_TX_RENAME:
                {
                    if (_selectedTxRule != -1 && !_settings.textureRules[_selectedTxRule].isDefault)
                    {
                        const auto& selectedRule = _settings.textureRules[_selectedTxRule];
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
                        invalidate();
                    }
                    break;
                }
                case WIDX_RULE_TX_MOVE_DOWN:
                {
                    if (_selectedTxRule != -1 && !_settings.textureRules[_selectedTxRule].isDefault && _selectedTxRule > 1)
                    {
                        std::swap(_settings.textureRules[_selectedTxRule], _settings.textureRules[_selectedTxRule - 1]);
                        SetSelectedTextureRule(_selectedTxRule - 1);
                        invalidate();
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
                        invalidate();
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
                            invalidate();
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
                    const auto& selectedRule = _settings.textureRules[_selectedTxRule];
                    LandTool::ShowSurfaceStyleDropdown(this, widget, selectedRule.effect.landTexture);
                    break;
                }
                case WIDX_RULE_TX_WALL_TEXTURE:
                {
                    if (_selectedTxRule == -1)
                    {
                        return;
                    }
                    const auto& selectedRule = _settings.textureRules[_selectedTxRule];
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

                    const Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        {}, std::size(items));
                    break;
                }
                case WIDX_RULE_TX_CONDITION_ADD:
                {
                    using namespace Dropdown;

                    constexpr ItemExt items[] = {
                        ItemExt(0, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_ABSOLUTE),
                        ItemExt(1, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_RELATIVE),
                        ItemExt(2, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO),
                        ItemExt(3, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_NOISE),
                        ItemExt(4, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_NORMAL_ANGLE),
                        ItemExt(5, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_PRNG),
                        ItemExt(6, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_BLEND_HEIGHT),
                        ItemExt(7, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_BLEND_NOISE),
                        ItemExt(8, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_BLEND_DISTANCE_TO),
                        ItemExt(9, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_LAND_STYLE),
                    };


                    SetItems(items);

                    const Widget* ddWidget = &widgets[widgetIndex];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        {}, std::size(items));

                    // TODO implement
                    gDropdown.items[4].setDisabled(true);
                    // not available here
                    gDropdown.items[9].setDisabled(true);

                    break;
                }
            }
        }

        void TextureUpdate()
        {
            // Tab animation
            if (++currentFrame >= TabAnimationLoops[page])
                currentFrame = 0;
            invalidateWidget(WIDX_TAB_4);

            // Check if the mouse is hovering over the list
            if (!widgetIsHighlighted(*this, WIDX_RULE_TX_SCROLL))
            {
                if (_highlightedTxRule != -1)
                    invalidateWidget(WIDX_RULE_TX_SCROLL);
                _highlightedTxRule = -1;
            }

            // Check if the mouse is hovering over the list
            if (!widgetIsHighlighted(*this, WIDX_RULE_TX_CONDITION_SCROLL))
            {
                if (_highlightedTxCondition != -1)
                    invalidateWidget(WIDX_RULE_TX_CONDITION_SCROLL);
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

            invalidate();
        }

        void TexturePrepareDraw()
        {

            const bool ruleSelected = _selectedTxRule != -1;
            const bool condSelected = ruleSelected && _selectedTxCondition != -1;

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
                const auto& selectedRule = _settings.textureRules[_selectedTxRule];
                setCheckboxValue(WIDX_RULE_TX_FLOOR_TEXTURE_CHECK, selectedRule.effect.applyLandTexture);
                setCheckboxValue(WIDX_RULE_TX_WALL_TEXTURE_CHECK, selectedRule.effect.applyEdgeTexture);

                setWidgetEnabled(WIDX_RULE_TX_FLOOR_TEXTURE, selectedRule.effect.applyLandTexture);
                setWidgetEnabled(WIDX_RULE_TX_WALL_TEXTURE, selectedRule.effect.applyEdgeTexture);

                setWidgetEnabled(WIDX_RULE_TX_FLOOR_TEXTURE_CHECK, !selectedRule.isDefault);
                setWidgetEnabled(WIDX_RULE_TX_WALL_TEXTURE_CHECK, !selectedRule.isDefault);

                setWidgetEnabled(WIDX_RULE_TX_REMOVE, !selectedRule.isDefault);
                setWidgetEnabled(WIDX_RULE_TX_RENAME, !selectedRule.isDefault);

                setWidgetEnabled(WIDX_RULE_TX_MOVE_UP, !selectedRule.isDefault
                    && _selectedTxRule < static_cast<int32_t>(_settings.textureRules.size() - 1));
                setWidgetEnabled(WIDX_RULE_TX_MOVE_DOWN, !selectedRule.isDefault &&  _selectedTxRule > 1);

                setWidgetEnabled(WIDX_RULE_TX_CONDITION_REMOVE, !selectedRule.isDefault && condSelected);
                setWidgetEnabled(WIDX_RULE_TX_CONDITION_EDIT, !selectedRule.isDefault && condSelected);
                setWidgetEnabled(WIDX_RULE_TX_CONDITION_ADD, !selectedRule.isDefault);
            }
        }

        void TextureDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            if (_selectedTxRule != -1)
            {
                const auto& selectedRule = _settings.textureRules[_selectedTxRule];
                DrawDropdownButton(rt, WIDX_RULE_TX_FLOOR_TEXTURE, LookupSurfaceImage(selectedRule.effect.landTexture));
                DrawDropdownButton(rt, WIDX_RULE_TX_WALL_TEXTURE, LookupEdgeImage(selectedRule.effect.edgeTexture));
            }
            else
            {
                const auto& ruleScroll = widgets[WIDX_RULE_TX_SCROLL];
                const auto centrePos = windowPos + ScreenCoordsXY{ ruleScroll.left, ruleScroll.bottom + 5 };
                const auto textPaint = TextPaint{ colours[1] };
                drawTextWrapped(rt, centrePos, ruleScroll.width(), STR_MAPGEN_RULE_TX_HINT, {}, textPaint);
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
                        dropdownIndex = gDropdown.highlightedIndex;

                    if (dropdownIndex != -1)
                        selectedRule.effect.landTexture = dropdownIndex;

                    invalidate();
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
                        dropdownIndex = gDropdown.highlightedIndex;

                    if (dropdownIndex != -1)
                        selectedRule.effect.edgeTexture = dropdownIndex;

                    invalidate();
                    break;
                }
                case WIDX_RULE_TX_NEW_PRESET:
                {
                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdown.highlightedIndex;

                    if (dropdownIndex != -1)
                    {
                        const auto preset = static_cast<MapGenerator::Rule::TextureRulePreset>(dropdownIndex);
                        MapGenerator::Rule::createNewTextureRuleFromPreset(_settings, preset);
                        SetSelectedTextureRule(static_cast<int32_t>(_settings.textureRules.size() - 1));
                    }

                    invalidate();
                    break;
                }
                case WIDX_RULE_TX_CONDITION_ADD:
                {
                    if (_selectedTxRule == -1)
                    {
                        return;
                    }

                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdown.highlightedIndex;

                    if (dropdownIndex == -1)
                    {
                        return;
                    }

                    const auto conditionType = static_cast<MapGenerator::Rule::Type>(dropdownIndex);
                    auto newCondition = createNewCondition(conditionType);
                    auto callback = [this](MapGenerator::Rule::Condition& condition) {
                        if (_selectedTxRule == -1 || _settings.textureRules[_selectedTxRule].isDefault)
                        {
                            return;
                        }
                        auto& selectedRule = _settings.textureRules[_selectedTxRule];
                        selectedRule.conditions.push_back(std::move(condition));
                        SetSelectedTextureCondition(static_cast<int32_t>(selectedRule.conditions.size() - 1));
                        invalidate();
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
                    const auto& selectedRule = _settings.textureRules[_selectedTxRule];

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
            Rectangle::fill(
                rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } },
                getColourMap(colours[1].colour).midLight, true);

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
                    Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).midDark);
                    stringFormat = STR_WHITE_STRING;
                }
                else if (i == _highlightedTxCondition)
                {
                    Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).midDark, true);
                    stringFormat = STR_WHITE_STRING;
                }

                auto checkboxFormatter = Formatter();
                checkboxFormatter.Add<StringId>(STR_STRING);
                checkboxFormatter.Add<char*>(kCheckMarkString);

                // Draw enabled checkbox and check
                Rectangle::fillInset(
                    rt, { { 2, screenCoords.y }, { 15, screenCoords.y + 11 } }, colours[1], Rectangle::BorderStyle::inset,
                    Rectangle::FillBrightness::dark, Rectangle::FillMode::dontLightenWhenInset);
                if (condition.enabled)
                {
                    drawText(rt, screenCoords + ScreenCoordsXY{ 4, 1 }, stringFormat, checkboxFormatter);
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
                    {
                        auto& heightData = std::get<MapGenerator::Rule::HeightAbsoluteData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FEATURE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_HEIGHT_MODE_ABSOLUTE);
                        ft.Add<StringId>(heightSourceToStringId(heightData.source));
                        ft.Add<StringId>(predRepr);
                        ft.Add<int16_t>(static_cast<int16_t>( BaseZToMetres(heightData.height)));
                        break;
                    }
                    case MapGenerator::Rule::Type::HeightRelative:
                    {
                        auto& heightData = std::get<MapGenerator::Rule::HeightRelativeData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_HEIGHT_RELATIVE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_HEIGHT_RELATIVE);
                        ft.Add<StringId>(heightSourceToStringId(heightData.sourceFirst));
                        ft.Add<StringId>(heightSourceToStringId(heightData.sourceSecond));
                        ft.Add<StringId>(predRepr);
                        ft.Add<int16_t>(static_cast<int16_t>( HeightUnitsToMetres(heightData.height)));
                        break;
                    }
                    case MapGenerator::Rule::Type::Distance:
                    {
                        auto& distanceData = std::get<MapGenerator::Rule::DistanceData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FEATURE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_DISTANCE_TO);
                        ft.Add<StringId>(featureToStringId(distanceData.feature));
                        ft.Add<StringId>(predRepr);
                        ft.Add<int16_t>(static_cast<int16_t>( TileUnitsToMetres(distanceData.distance)));
                        break;
                    }
                    case MapGenerator::Rule::Type::Noise:
                    {
                        auto& noiseData = std::get<MapGenerator::Rule::NoiseData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_NOISE);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int32_t>(static_cast<int32_t>(   noiseData.value * 100));
                        break;
                    }
                    case MapGenerator::Rule::Type::NormalAngle:
                    {
                        auto& normalAngleData = std::get<MapGenerator::Rule::NormalAngleData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_NORMAL_ANGLE);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int32_t>(static_cast<int32_t>(  normalAngleData.angle * 100));
                        break;
                    }
                    case MapGenerator::Rule::Type::Random:
                    {
                        auto& randomData = std::get<MapGenerator::Rule::RandomData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        ft.Add<StringId>(predRepr);
                        ft.Add<int32_t>(static_cast<int32_t>(  randomData.value * 100));
                        break;
                    }
                    case MapGenerator::Rule::Type::BlendHeight:
                    {
                        auto& blendHeightData = std::get<MapGenerator::Rule::BlendHeightData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_BLEND_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_BLEND_HEIGHT);
                        ft.Add<int16_t>(static_cast<int16_t>(   BaseZToMetres(blendHeightData.edgeLow)));
                        ft.Add<int16_t>(static_cast<int16_t>(  BaseZToMetres(blendHeightData.edgeHigh)));
                        ft.Add<StringId>(predRepr);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        break;
                    }
                    case MapGenerator::Rule::Type::BlendNoise:
                    {
                        auto& blendNoiseData = std::get<MapGenerator::Rule::BlendNoiseData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_BLEND_FLOAT);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_BLEND_NOISE);
                        ft.Add<int32_t>(static_cast<int32_t>(  blendNoiseData.edgeLow * 100));
                        ft.Add<int32_t>(static_cast<int32_t>( blendNoiseData.edgeHigh * 100));
                        ft.Add<StringId>(predRepr);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        break;
                    }
                    case MapGenerator::Rule::Type::BlendDistance:
                    {
                        auto& blendDistanceData = std::get<MapGenerator::Rule::BlendDistanceData>(condition.data);
                        ft.Add<StringId>(STR_MAPGEN_RULE_VALUE_BLEND_DISTANCE_LENGTH);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_BLEND_DISTANCE_TO);
                        ft.Add<StringId>(featureToStringId(blendDistanceData.feature));
                        ft.Add<int16_t>(TileUnitsToMetres(blendDistanceData.edgeLow));
                        ft.Add<int16_t>(TileUnitsToMetres(blendDistanceData.edgeHigh));
                        ft.Add<StringId>(predRepr);
                        ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                        break;
                    }
                    case MapGenerator::Rule::Type::LandStyle:
                    {
                        // should not be reachable
                        break;
                    }
                }

                // Draw description
                drawTextEllipsised(rt, screenCoords + ScreenCoordsXY{ 20, 0 }, 200, stringFormat, ft);

                screenCoords.y -= kScrollableRowHeight;
            }
        }

        void TextureScrollDrawRule(RenderTarget& rt)
        {
            const int32_t listWidth = widgets[WIDX_RULE_TX_SCROLL].width();
            Rectangle::fill(
                rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } }, getColourMap(colours[1].colour).midLight);

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
                    Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).midDark);
                    stringFormat = STR_WHITE_STRING;
                }
                else if (i == _highlightedTxRule)
                {
                    Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).midDark, true);
                    stringFormat = STR_WHITE_STRING;
                }

                auto checkboxFormatter = Formatter();
                checkboxFormatter.Add<StringId>(STR_STRING);
                checkboxFormatter.Add<char*>(kCheckMarkString);

                if (!rule.isDefault)
                {
                    // Draw enabled checkbox and check
                    Rectangle::fillInset(
                        rt, { { 2, screenCoords.y }, { 15, screenCoords.y + 11 } }, colours[1], Rectangle::BorderStyle::inset,
                        Rectangle::FillBrightness::dark, Rectangle::FillMode::dontLightenWhenInset);
                    if (rule.enabled)
                    {
                        drawText(rt, screenCoords + ScreenCoordsXY{ 4, 1 }, stringFormat, checkboxFormatter);
                    }
                }

                // Rule name
                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<char*>(rule.name.c_str());
                drawTextEllipsised(
                    rt, screenCoords + ScreenCoordsXY{ 20, 0 }, 200, stringFormat, ft);

                const ImageId maskImage(SPR_G2_MASK_MAPGEN_TEXTURE_SCROLL);
                // Surface
                if (rule.effect.applyLandTexture)
                {
                    const auto surfaceImage = LookupSurfaceImage(rule.effect.landTexture);
                    GfxDrawSpriteRawMasked(rt, screenCoords + ScreenCoordsXY{ 218, 0 }, maskImage, surfaceImage);
                }

                // Edge
                if (rule.effect.applyEdgeTexture)
                {
                    const auto edgeImage = LookupEdgeImage(rule.effect.edgeTexture);
                    GfxDrawSpriteRawMasked(rt, screenCoords + ScreenCoordsXY{ 258, 0 }, maskImage, edgeImage);
                }

                screenCoords.y -= kScrollableRowHeight;
            }
        }

        void TextureScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
            if(scrollIndex == 0)
            {
                const int32_t index = static_cast<int32_t>(
                    _settings.textureRules.size() - (screenCoords.y - 1) / kScrollableRowHeight - 1);
                if (index < 0 || index >= static_cast<int32_t>(_settings.textureRules.size()))
                {
                    _highlightedTxRule = -1;
                }
                else
                {
                    _highlightedTxRule = index;
                }
                invalidateWidget(WIDX_RULE_TX_SCROLL);
            }
            else if (scrollIndex == 1)
            {
                if (_selectedTxRule == -1)
                {
                    _highlightedTxCondition = -1;
                }
                else
                {
                    const auto& selectedRule = _settings.textureRules[_selectedTxRule];

                    const int32_t index = static_cast<int32_t>(
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
                invalidateWidget(WIDX_RULE_TX_CONDITION_SCROLL);
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
                    ft.Add<int16_t>(baseZToDisplayHeight(kMinimumWaterHeight));
                    ft.Add<int16_t>(baseZToDisplayHeight(kMaximumWaterHeight));
                    WindowTextInputOpen(
                        this, WIDX_WATER_LEVEL, STR_WATER_LEVEL, STR_ENTER_WATER_LEVEL, ft, STR_FORMAT_INTEGER,
                        baseZToDisplayHeight(_settings.waterLevel), 6);
                    break;
                }
                case WIDX_WATER_RIVERS_CATCHMENT:
                {
                    Formatter ft;
                    ft.Add<int32_t>(tileUnitsAreaToDisplayArea(MapGenerator::River::kRiverCatchmentThresholdMin));
                    ft.Add<int32_t>(tileUnitsAreaToDisplayArea(MapGenerator::River::kRiverCatchmentThresholdMax));
                    WindowTextInputOpen(
                        this, WIDX_WATER_RIVERS_CATCHMENT, STR_WATER_RIVERS_CATCHMENT, STR_WATER_RIVERS_CATCHMENT_ENTER, ft,
                        STR_FORMAT_INTEGER, tileUnitsAreaToDisplayArea(_settings.river.catchmentThreshold), 8);
                    break;
                }
                case WIDX_WATER_RIVERS_ENABLE:
                {
                    _settings.river.generate = !_settings.river.generate;
                    setCheckboxValue(WIDX_WATER_RIVERS_ENABLE, _settings.river.generate);
                    invalidate();
                    break;
                }
                case WIDX_WATER_RIVERS_WIDTH_MAX:
                {
                    Formatter ft;
                    ft.Add<int32_t>(tileUnitsToDisplayLength(MapGenerator::River::kRiverWidthMin));
                    ft.Add<int32_t>(tileUnitsToDisplayLength(MapGenerator::River::kRiverWidthMax));
                    WindowTextInputOpen(
                        this, WIDX_WATER_RIVERS_WIDTH_MAX, STR_WATER_RIVERS_WIDTH_MAX, STR_WATER_RIVERS_WIDTH_MAX_ENTER, ft,
                        STR_FORMAT_INTEGER, tileUnitsToDisplayLength(_settings.river.riverWidthMax), 4);
                    break;
                }
                case WIDX_WATER_RIVERS_GROWTH_EXPONENT:
                {
                    Formatter ft;
                    ft.Add<int32_t>(MapGenerator::River::kRiverGrowthExponentMin);
                    ft.Add<int32_t>(MapGenerator::River::kRiverGrowthExponentMax);
                    WindowTextInputOpen(
                        this, WIDX_WATER_RIVERS_GROWTH_EXPONENT, STR_WATER_RIVERS_GROWTH_EXPONENT,
                        STR_WATER_RIVERS_GROWTH_EXPONENT_ENTER, ft, STR_FORMAT_COMMA2DP32, _settings.river.riverGrowthExponent,
                        5);
                    break;
                }
                case WIDX_WATER_RIVERS_PRUNE_THRESHOLD:
                {
                    Formatter ft;
                    ft.Add<int32_t>(tileUnitsToDisplayLength(MapGenerator::River::kRiverPruneLengthThresholdMin));
                    ft.Add<int32_t>(tileUnitsToDisplayLength(MapGenerator::River::kRiverPruneLengthThresholdMin));
                    WindowTextInputOpen(
                        this, WIDX_WATER_RIVERS_PRUNE_THRESHOLD, STR_WATER_RIVERS_PRUNE_THRESHOLD,
                        STR_WATER_RIVERS_PRUNE_THRESHOLD_ENTER, ft, STR_FORMAT_INTEGER,
                        tileUnitsToDisplayLength(_settings.river.pruneThreshold), 3);
                    break;
                }
                case WIDX_WATER_RIVERS_BREACH_LENGTH:
                {
                    Formatter ft;
                    ft.Add<int32_t>(tileUnitsToDisplayLength(MapGenerator::River::kRiverBreachLengthMin));
                    ft.Add<int32_t>(tileUnitsToDisplayLength(MapGenerator::River::kRiverBreachLengthMax));
                    WindowTextInputOpen(
                        this, WIDX_WATER_RIVERS_BREACH_LENGTH, STR_WATER_RIVERS_BREACH_LENGTH,
                        STR_WATER_RIVERS_BREACH_LENGTH_ENTER, ft, STR_FORMAT_INTEGER,
                        tileUnitsToDisplayLength(_settings.river.breachMaxLength), 3);
                    break;
                }
                case WIDX_WATER_RIVERS_BREACH_DEPTH:
                {
                    Formatter ft;
                    ft.Add<int32_t>(heightUnitsToDisplayHeight(MapGenerator::River::kRiverBreachDepthMin));
                    ft.Add<int32_t>(heightUnitsToDisplayHeight(MapGenerator::River::kRiverBreachDepthMax));
                    WindowTextInputOpen(
                        this, WIDX_WATER_RIVERS_BREACH_DEPTH, STR_WATER_RIVERS_BREACH_DEPTH,
                        STR_WATER_RIVERS_BREACH_DEPTH_ENTER, ft, STR_FORMAT_INTEGER,
                        heightUnitsToDisplayHeight(_settings.river.breachMaxDepth), 3);
                    break;
                }
                case WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL:
                {
                    _settings.river.riverFlowAggregationFractional = !_settings.river.riverFlowAggregationFractional;
                    invalidate();
                    break;
                }
                case WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT:
                {
                    Formatter ft;
                    ft.Add<int32_t>(MapGenerator::River::kRiverFlowAggregationSlopeExponentMin);
                    ft.Add<int32_t>(MapGenerator::River::kRiverFlowAggregationSlopeExponentMax);
                    WindowTextInputOpen(
                        this, WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT,
                        STR_WATER_RIVERS_FRACTIONAL_AGGREGATION_EXPONENT,
                        STR_WATER_RIVERS_FRACTIONAL_AGGREGATION_EXPONENT_ENTER, ft, STR_FORMAT_COMMA2DP32,
                        _settings.river.riverFlowAggregationSlopeExponent, 5);
                    break;
                }
            }
        }

        void WaterMouseDown(WidgetIndex widgetIndex, Widget* widget)
        {
            switch (widgetIndex)
            {
                case WIDX_WATER_LEVEL_UP:
                    _settings.waterLevel.increment();
                    invalidate();
                    break;
                case WIDX_WATER_LEVEL_DOWN:
                    _settings.waterLevel .decrement();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_CATCHMENT_UP:
                    _settings.river.catchmentThreshold.increment();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_CATCHMENT_DOWN:
                    _settings.river.catchmentThreshold.decrement();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_WIDTH_MAX_UP:
                    _settings.river.riverWidthMax.increment();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_WIDTH_MAX_DOWN:
                    _settings.river.riverWidthMax.decrement();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_GROWTH_EXPONENT_UP:
                    _settings.river.riverGrowthExponent.increment();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_GROWTH_EXPONENT_DOWN:
                    _settings.river.riverGrowthExponent.decrement();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_PRUNE_THRESHOLD_UP:
                    _settings.river.pruneThreshold.increment();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_PRUNE_THRESHOLD_DOWN:
                    _settings.river.pruneThreshold.decrement();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_BREACH_LENGTH_UP:
                    _settings.river.breachMaxLength.increment();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_BREACH_LENGTH_DOWN:
                    _settings.river.breachMaxLength.decrement();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_BREACH_DEPTH_UP:
                    _settings.river.breachMaxDepth.increment();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_BREACH_DEPTH_DOWN:
                    _settings.river.breachMaxDepth.decrement();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT_UP:
                    _settings.river.riverFlowAggregationSlopeExponent.increment();
                    invalidate();
                    break;
                case WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT_DOWN:
                    _settings.river.riverFlowAggregationSlopeExponent.decrement();
                    invalidate();
                    break;
            }
        }

        void WaterUpdate()
        {
            // Tab animation
            if (++currentFrame >= TabAnimationLoops[page])
                currentFrame = 0;
            invalidateWidget(WIDX_TAB_3);
        }

        int32_t tileUnitsAreaToDisplayArea(int32_t area)
        {
            area = area * 10;

            if (Config::Get().general.measurementFormat == MeasurementFormat::imperial)
            {
                area = SquaredMetresToSquaredFeet(area);
            }
            return area;
        }

        int32_t tileUnitsToDisplayLength(int32_t length)
        {
            length = TileUnitsToMetres(length);
            if (Config::Get().general.measurementFormat == MeasurementFormat::imperial)
            {
                length = MetresToFeet(length);
            }
            return length;
        }

        int32_t baseZToDisplayHeight(int32_t height)
        {
            height = BaseZToMetres(height);
            if (Config::Get().general.measurementFormat == MeasurementFormat::imperial)
            {
                height =MetresToFeet(height);
            }
            return height;
        }

        int32_t heightUnitsToDisplayHeight(int32_t height)
        {
            height = HeightUnitsToMetres(height);
            if (Config::Get().general.measurementFormat == MeasurementFormat::imperial)
            {
                height = MetresToFeet(height);
            }
            return height;
        }

        int32_t displayAreaToTileUnits(int32_t displayArea)
        {
            if (Config::Get().general.measurementFormat == MeasurementFormat::imperial)
            {
                displayArea = SquaredFeetToSquaredMetres(displayArea);
            }
            return displayArea / 10;
        }

        int32_t displayLengthToTileUnits(int32_t displayLength)
        {
            if (Config::Get().general.measurementFormat == MeasurementFormat::imperial)
            {
                displayLength = FeetToMetres(displayLength);
            }
            return MetresToTileUnits(displayLength);
        }

        int32_t displayHeightToBaseZ(int32_t displayHeight)
        {
            if (Config::Get().general.measurementFormat == MeasurementFormat::imperial)
            {
                displayHeight =FeetToMetres(displayHeight);
            }
            return MetresToBaseZ(displayHeight);
        }

        int32_t displayHeightToTileUnits(int32_t displayHeight)
        {
            if (Config::Get().general.measurementFormat == MeasurementFormat::imperial)
            {
                displayHeight =FeetToMetres(displayHeight);
            }
            return MetresToHeightUnits(displayHeight);
        }

        void WaterTextInput(WidgetIndex widgetIndex, int32_t value)
        {
            switch (widgetIndex)
            {
                case WIDX_WATER_LEVEL:
                    _settings.waterLevel = displayHeightToBaseZ(value);
                    break;
                case WIDX_WATER_RIVERS_CATCHMENT:
                    _settings.river.catchmentThreshold = displayAreaToTileUnits(value);
                    break;
                case WIDX_WATER_RIVERS_WIDTH_MAX:
                    _settings.river.riverWidthMax = displayLengthToTileUnits(value);
                    break;
                case WIDX_WATER_RIVERS_GROWTH_EXPONENT:
                    _settings.river.riverGrowthExponent = value;
                    break;
                case WIDX_WATER_RIVERS_PRUNE_THRESHOLD:
                    _settings.river.pruneThreshold = displayLengthToTileUnits(value);
                    break;
                case WIDX_WATER_RIVERS_BREACH_LENGTH:
                    _settings.river.breachMaxLength = displayLengthToTileUnits(value);
                    break;
                case WIDX_WATER_RIVERS_BREACH_DEPTH:
                    _settings.river.breachMaxDepth = displayHeightToTileUnits(value);
                    break;
                case WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT:
                    _settings.river.riverFlowAggregationSlopeExponent = value;
                    break;
            }

            invalidate();
        }

        void WaterPrepareDraw()
        {
            const bool isNotFlatland = _settings.generator != MapGenerator::HeightMapGenerator::flat;

            setWidgetEnabled(WIDX_WATER_RIVERS_ENABLE, isNotFlatland);

            const bool enableRiver = _settings.river.generate;

            setCheckboxValue(WIDX_WATER_RIVERS_ENABLE, enableRiver);

            setWidgetDisabled(WIDX_WATER_RIVERS_CATCHMENT, !enableRiver);
            setWidgetDisabled(WIDX_WATER_RIVERS_CATCHMENT_UP, !enableRiver);
            setWidgetDisabled(WIDX_WATER_RIVERS_CATCHMENT_DOWN, !enableRiver);

            setWidgetDisabled(WIDX_WATER_RIVERS_WIDTH_MAX, !enableRiver);
            setWidgetDisabled(WIDX_WATER_RIVERS_WIDTH_MAX_UP, !enableRiver);
            setWidgetDisabled(WIDX_WATER_RIVERS_WIDTH_MAX_DOWN, !enableRiver);

            setWidgetDisabled(WIDX_WATER_RIVERS_GROWTH_EXPONENT, !enableRiver);
            setWidgetDisabled(WIDX_WATER_RIVERS_GROWTH_EXPONENT_UP, !enableRiver);
            setWidgetDisabled(WIDX_WATER_RIVERS_GROWTH_EXPONENT_DOWN, !enableRiver);

            setWidgetDisabled(WIDX_WATER_RIVERS_PRUNE_THRESHOLD, !enableRiver);
            setWidgetDisabled(WIDX_WATER_RIVERS_PRUNE_THRESHOLD_UP, !enableRiver);
            setWidgetDisabled(WIDX_WATER_RIVERS_PRUNE_THRESHOLD_DOWN, !enableRiver);

            setWidgetDisabled(WIDX_WATER_RIVERS_BREACH_LENGTH, !enableRiver);
            setWidgetDisabled(WIDX_WATER_RIVERS_BREACH_LENGTH_UP, !enableRiver);
            setWidgetDisabled(WIDX_WATER_RIVERS_BREACH_LENGTH_DOWN, !enableRiver);

            setWidgetDisabled(WIDX_WATER_RIVERS_BREACH_DEPTH, !enableRiver);
            setWidgetDisabled(WIDX_WATER_RIVERS_BREACH_DEPTH_UP, !enableRiver);
            setWidgetDisabled(WIDX_WATER_RIVERS_BREACH_DEPTH_DOWN, !enableRiver);

            setWidgetDisabled(WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL, !enableRiver);
            setCheckboxValue(WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL, _settings.river.riverFlowAggregationFractional);

            const bool enableFractionalFlowAgg = enableRiver && _settings.river.riverFlowAggregationFractional;

            setWidgetDisabled(WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT, !enableFractionalFlowAgg);
            setWidgetDisabled(WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT_UP, !enableFractionalFlowAgg);
            setWidgetDisabled(WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT_DOWN, !enableFractionalFlowAgg);
        }

        void WaterDraw(RenderTarget& rt)
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            const auto textColour = colours[1];
            const auto disabledColour = textColour.withFlag(ColourFlag::inset, true);

            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_WATER_LEVEL].top + 1 }, STR_WATER_LEVEL_LABEL, { textColour });

            auto ft = Formatter();
            ft.Add<int32_t>(BaseZToMetres(_settings.waterLevel));
            drawText(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_WATER_LEVEL].left + 1, widgets[WIDX_WATER_LEVEL].top + 1 },
                STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });

            const auto valueColour = isWidgetDisabled(WIDX_WATER_RIVERS_CATCHMENT) ? disabledColour : textColour;

            // catchment
            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_WATER_RIVERS_CATCHMENT].top + 1 }, STR_WATER_RIVERS_CATCHMENT,
                { valueColour });

            StringId areaFmt = STR_MAPGEN_FORMAT_AREA_METRIC;
            auto catchmentFmt = _settings.river.catchmentThreshold * 10;
            if (Config::Get().general.measurementFormat == MeasurementFormat::imperial)
            {
                areaFmt = STR_MAPGEN_FORMAT_AREA_IMPERIAL;
                catchmentFmt = SquaredMetresToSquaredFeet(catchmentFmt);
            }
            ft = Formatter();
            ft.Add<int32_t>(catchmentFmt);
            drawText(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_WATER_RIVERS_CATCHMENT].left + 1,
                widgets[WIDX_WATER_RIVERS_CATCHMENT].top + 1 }, areaFmt, ft, { valueColour });

            // max width
            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_WATER_RIVERS_WIDTH_MAX].top + 1 }, STR_WATER_RIVERS_WIDTH_MAX,
                { valueColour });

            ft = Formatter();
            ft.Add<int32_t>(TileUnitsToMetres(_settings.river.riverWidthMax));
            drawText(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_WATER_RIVERS_WIDTH_MAX].left + 1,
                widgets[WIDX_WATER_RIVERS_WIDTH_MAX].top + 1 }, STR_RIDE_LENGTH_ENTRY, ft, { valueColour });

            // growth exponent
            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_WATER_RIVERS_GROWTH_EXPONENT].top + 1 }, STR_WATER_RIVERS_GROWTH_EXPONENT,
                { valueColour });

            ft = Formatter();
            ft.Add<int32_t>(_settings.river.riverGrowthExponent);
            drawText(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_WATER_RIVERS_GROWTH_EXPONENT].left + 1,
                widgets[WIDX_WATER_RIVERS_GROWTH_EXPONENT].top + 1 }, STR_FORMAT_COMMA2DP32, ft, { valueColour });

            // prune threshold
            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_WATER_RIVERS_PRUNE_THRESHOLD].top + 1 }, STR_WATER_RIVERS_PRUNE_THRESHOLD,
                { valueColour });

            ft = Formatter();
            ft.Add<int32_t>(TileUnitsToMetres(_settings.river.pruneThreshold));
            drawText(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_WATER_RIVERS_PRUNE_THRESHOLD].left + 1,
                widgets[WIDX_WATER_RIVERS_PRUNE_THRESHOLD].top + 1 }, STR_RIDE_LENGTH_ENTRY, ft, { valueColour });

            // breach length/depth
            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_WATER_RIVERS_BREACH_LENGTH].top + 1 }, STR_WATER_RIVERS_BREACH_LABEL,
                { valueColour });

            ft = Formatter();
            ft.Add<int32_t>(TileUnitsToMetres(_settings.river.breachMaxLength));
            drawText(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_WATER_RIVERS_BREACH_LENGTH].left + 1,
                widgets[WIDX_WATER_RIVERS_BREACH_LENGTH].top + 1 }, STR_RIDE_LENGTH_ENTRY, ft, { valueColour });

            ft = Formatter();
            ft.Add<int32_t>(HeightUnitsToMetres(_settings.river.breachMaxDepth));
            drawText(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_WATER_RIVERS_BREACH_DEPTH].left + 1,
                widgets[WIDX_WATER_RIVERS_BREACH_DEPTH].top + 1 }, STR_RIDE_LENGTH_ENTRY, ft, { valueColour });

            // flow agg exponent
            const auto flowAggExpValueColour = isWidgetDisabled(WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT)
                ? disabledColour : textColour;

            drawText(
                rt, windowPos + ScreenCoordsXY{ 10, widgets[WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT].top + 1 },
                STR_WATER_RIVERS_FRACTIONAL_AGGREGATION_EXPONENT, { flowAggExpValueColour });

            ft = Formatter();
            ft.Add<int32_t>(_settings.river.riverFlowAggregationSlopeExponent);
            drawText(
                rt, windowPos + ScreenCoordsXY{ widgets[WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT].left + 1,
                widgets[WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT].top + 1 }, STR_FORMAT_COMMA2DP32, ft,
                { flowAggExpValueColour });
        }

#pragma endregion

    public:
        void onOpen() override
        {
            number = 0;

            setPage(WINDOW_MAPGEN_PAGE_BASE);
            invalidate();
            initScrollWidgets();

            _heightmapLoaded = false;
        }

        void onClose() override
        {
            MapGenerator::unloadHeightMapImage();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
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

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            SharedMouseDown(widgetIndex);
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

        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            SharedDropdown(widgetIndex, selectedIndex);

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

        void onUpdate() override
        {
            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                invalidateWidget(WIDX_MAP_SEED);
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

        void onPrepareDraw() override
        {
            const bool isHeightMapImage = _settings.generator == MapGenerator::HeightMapGenerator::image;
            setWidgetDisabled(WIDX_MAP_GENERATE, isHeightMapImage && !_heightmapLoaded);

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

        void onDraw(RenderTarget& rt) override
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

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
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

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TextureScrollDraw(scrollIndex, rt);
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryScrollDraw(scrollIndex, rt);
            }
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TextureScrollMouseOver(scrollIndex, screenCoords);
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryScrollMouseOver(scrollIndex, screenCoords);
            }
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (page)
            {
                case WINDOW_MAPGEN_PAGE_TEXTURE:
                    return TextureScrollMouseDown(scrollIndex, screenCoords);
                case WINDOW_MAPGEN_PAGE_SCENERY:
                    return SceneryScrollMouseDown(scrollIndex, screenCoords);
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            const auto strText = std::string(text);
            char* end;

            if (page == WINDOW_MAPGEN_PAGE_BASE && widgetIndex == WIDX_MAP_SEED && !_random_seed)
            {
                _settings.seed.assign(text);
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
                || (page == WINDOW_MAPGEN_PAGE_WATER && widgetIndex == WIDX_WATER_RIVERS_GROWTH_EXPONENT)
                || (page == WINDOW_MAPGEN_PAGE_WATER && widgetIndex == WIDX_WATER_RIVERS_FLOW_AGGREGATION_FRACTIONAL_EXPONENT))
                value = 100 * strtof(strText.c_str(), &end);
            else
                value = strtol(strText.c_str(), &end, 10);

            if (*end != '\0')
                return;

            // Take care of unit conversion
            if (page != WINDOW_MAPGEN_PAGE_BASE && page != WINDOW_MAPGEN_PAGE_WATER )
            {
                switch (Config::Get().general.measurementFormat)
                {
                    case MeasurementFormat::imperial:
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

        void afterLoadingHeightMap(ModalResult result, const utf8* path)
        {
            if (result == ModalResult::ok)
            {
                if (!MapGenerator::loadHeightMapImage(path))
                {
                    // TODO: Display error popup
                    return;
                }

                // The window needs to be open while using the map
                _heightmapLoaded = true;
                _heightmapFilename = fs::u8path(path).filename().string();
                _settings.generator = MapGenerator::HeightMapGenerator::image;
                setPage(WINDOW_MAPGEN_PAGE_BASE);
            }
        }

        void afterLoadMapgenSettings(ModalResult result, const utf8* path)
        {
            if (result == ModalResult::ok)
            {
                try
                {
                    _settings = MapGenerator::loadMapgenSettingsFromPath(path);
                    _random_seed = false;
                }
                catch (MapGenerator::SettingSerdeException& e)
                {
                    GetWindowManager()->ShowError("Failed to load mapgen settings", e.what());
                }
            }
        }

        void afterSaveMapgenSettings(ModalResult result, const utf8* path)
        {
            if (result == ModalResult::ok)
            {
               try
                {
                   MapGenerator::saveMapgenSettingsToPath(_settings, path);
                }
                catch (MapGenerator::SettingSerdeException& e)
                {
                    GetWindowManager()->ShowError("Failed to save mapgen settings", e.what());
                }
            }
        }

        void onResize() override
        {
            WindowSetResize(*this, kWindowSize, kWindowSize);
        }
    };

    WindowBase* MapGenOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<MapGenWindow>(
            WindowClass::mapgen, kWindowSize, { WindowFlag::higherContrastOnPress, WindowFlag::autoPosition });
    }

    static void HeightmapLoadsaveCallback(ModalResult result, const utf8* path)
    {
        auto* w = static_cast<MapGenWindow*>(MapGenOpen());
        w->afterLoadingHeightMap(result, path);
    }

    static void MapgenSettingsLoadCallback(ModalResult result, const utf8* path)
    {
        auto* w = static_cast<MapGenWindow*>(MapGenOpen());
        w->afterLoadMapgenSettings(result, path);
    }

    static void MapgenSettingsSaveCallback(ModalResult result, const utf8* path)
    {
        auto* w = static_cast<MapGenWindow*>(MapGenOpen());
        w->afterSaveMapgenSettings(result, path);
    }
} // namespace OpenRCT2::Ui::Windows
