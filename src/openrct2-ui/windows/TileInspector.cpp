/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <iterator>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/UiStringIds.h>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/GameActionRunner.h>
#include <openrct2/actions/general/TileModifyAction.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/object/FootpathObject.h>
#include <openrct2/object/FootpathRailingsObject.h>
#include <openrct2/object/FootpathSurfaceObject.h>
#include <openrct2/object/LargeSceneryEntry.h>
#include <openrct2/object/ObjectEntryManager.h>
#include <openrct2/object/PathAdditionEntry.h>
#include <openrct2/object/SmallSceneryEntry.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/object/WallSceneryEntry.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/TileInspectorGlobals.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Entrance.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/TileInspector.h>
#include <openrct2/world/tile_element/BannerElement.h>
#include <openrct2/world/tile_element/EntranceElement.h>
#include <openrct2/world/tile_element/LargeSceneryElement.h>
#include <openrct2/world/tile_element/PathElement.h>
#include <openrct2/world/tile_element/Slope.h>
#include <openrct2/world/tile_element/SmallSceneryElement.h>
#include <openrct2/world/tile_element/SurfaceElement.h>
#include <openrct2/world/tile_element/TrackElement.h>
#include <openrct2/world/tile_element/WallElement.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kEntranceTypeStringIds[] = {
        STR_TILE_INSPECTOR_ENTRANCE_TYPE_RIDE_ENTRANCE,
        STR_TILE_INSPECTOR_ENTRANCE_TYPE_RIDE_EXIT,
        STR_TILE_INSPECTOR_ENTRANCE_TYPE_PARK_ENTRANC,
    };

    static constexpr StringId kParkEntrancePartStringIds[] = {
        STR_TILE_INSPECTOR_ENTRANCE_MIDDLE,
        STR_TILE_INSPECTOR_ENTRANCE_LEFT,
        STR_TILE_INSPECTOR_ENTRANCE_RIGHT,
    };

    static constexpr StringId kWallSlopeStringIds[] = {
        STR_TILE_INSPECTOR_WALL_FLAT,
        STR_TILE_INSPECTOR_WALL_SLOPED_LEFT,
        STR_TILE_INSPECTOR_WALL_SLOPED_RIGHT,
        STR_TILE_INSPECTOR_WALL_ANIMATION_FRAME,
    };

    enum WindowTileInspectorWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_LIST,
        WIDX_SPINNER_X_LABEL,
        WIDX_SPINNER_X,
        WIDX_SPINNER_X_INCREASE,
        WIDX_SPINNER_X_DECREASE,
        WIDX_SPINNER_Y_LABEL,
        WIDX_SPINNER_Y,
        WIDX_SPINNER_Y_INCREASE,
        WIDX_SPINNER_Y_DECREASE,
        WIDX_BUTTON_REMOVE,
        WIDX_BUTTON_MOVE_UP,
        WIDX_BUTTON_MOVE_DOWN,
        WIDX_BUTTON_ROTATE,
        WIDX_BUTTON_SORT,
        WIDX_BUTTON_PASTE,
        WIDX_BUTTON_COPY,
        WIDX_COLUMN_INVISIBLE,
        WIDX_COLUMN_TYPE,
        WIDX_COLUMN_BASEHEIGHT,
        WIDX_COLUMN_CLEARANCEHEIGHT,
        WIDX_COLUMN_DIRECTION,
        WIDX_COLUMN_GHOSTFLAG,
        WIDX_COLUMN_LASTFLAG,
        WIDX_GROUPBOX_DETAILS,
        WIDX_GROUPBOX_PROPERTIES,

        PAGE_WIDGETS,

        // Surface
        WIDX_SURFACE_SPINNER_HEIGHT = PAGE_WIDGETS,
        WIDX_SURFACE_SPINNER_HEIGHT_INCREASE,
        WIDX_SURFACE_SPINNER_HEIGHT_DECREASE,
        WIDX_SURFACE_BUTTON_REMOVE_FENCES,
        WIDX_SURFACE_BUTTON_RESTORE_FENCES,
        WIDX_SURFACE_CHECK_CORNER_N,
        WIDX_SURFACE_CHECK_CORNER_E,
        WIDX_SURFACE_CHECK_CORNER_S,
        WIDX_SURFACE_CHECK_CORNER_W,
        WIDX_SURFACE_CHECK_DIAGONAL,

        // Path
        WIDX_PATH_SPINNER_HEIGHT = PAGE_WIDGETS,
        WIDX_PATH_SPINNER_HEIGHT_INCREASE,
        WIDX_PATH_SPINNER_HEIGHT_DECREASE,
        WIDX_PATH_CHECK_BROKEN,
        WIDX_PATH_CHECK_SLOPED,
        WIDX_PATH_CHECK_JUNCTION_RAILINGS,
        WIDX_PATH_CHECK_EDGE_NE, // Note: This is NOT named after the world orientation, but after the way
        WIDX_PATH_CHECK_EDGE_E,  // it looks in the window (top corner is north). Their order is important,
        WIDX_PATH_CHECK_EDGE_SE, // as this is the same order paths use for their corners / edges.
        WIDX_PATH_CHECK_EDGE_S,  //           N
        WIDX_PATH_CHECK_EDGE_SW, //      NW-------NE
        WIDX_PATH_CHECK_EDGE_W,  //   W ------------- E
        WIDX_PATH_CHECK_EDGE_NW, //      SW-------SE
        WIDX_PATH_CHECK_EDGE_N,  //           S

        // Track
        WIDX_TRACK_CHECK_APPLY_TO_ALL = PAGE_WIDGETS,
        WIDX_TRACK_SPINNER_HEIGHT,
        WIDX_TRACK_SPINNER_HEIGHT_INCREASE,
        WIDX_TRACK_SPINNER_HEIGHT_DECREASE,
        WIDX_TRACK_CHECK_CHAIN_LIFT,
        WIDX_TRACK_CHECK_BRAKE_CLOSED,
        WIDX_TRACK_CHECK_IS_INDESTRUCTIBLE,

        // Scenery
        WIDX_SCENERY_SPINNER_HEIGHT = PAGE_WIDGETS,
        WIDX_SCENERY_SPINNER_HEIGHT_INCREASE,
        WIDX_SCENERY_SPINNER_HEIGHT_DECREASE,
        WIDX_SCENERY_CHECK_QUARTER_N,
        WIDX_SCENERY_CHECK_QUARTER_E,
        WIDX_SCENERY_CHECK_QUARTER_S,
        WIDX_SCENERY_CHECK_QUARTER_W,
        WIDX_SCENERY_CHECK_COLLISION_N,
        WIDX_SCENERY_CHECK_COLLISION_E,
        WIDX_SCENERY_CHECK_COLLISION_S,
        WIDX_SCENERY_CHECK_COLLISION_W,

        // Entrance
        WIDX_ENTRANCE_SPINNER_HEIGHT = PAGE_WIDGETS,
        WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE,
        WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE,
        WIDX_ENTRANCE_BUTTON_MAKE_USABLE,

        // Wall
        WIDX_WALL_SPINNER_HEIGHT = PAGE_WIDGETS,
        WIDX_WALL_SPINNER_HEIGHT_INCREASE,
        WIDX_WALL_SPINNER_HEIGHT_DECREASE,
        WIDX_WALL_DROPDOWN_SLOPE,
        WIDX_WALL_DROPDOWN_SLOPE_BUTTON,
        WIDX_WALL_SPINNER_ANIMATION_FRAME,
        WIDX_WALL_SPINNER_ANIMATION_FRAME_INCREASE,
        WIDX_WALL_SPINNER_ANIMATION_FRAME_DECREASE,
        WIDX_WALL_ANIMATION_IS_BACKWARDS,

        // Large
        WIDX_LARGE_SCENERY_SPINNER_HEIGHT = PAGE_WIDGETS,
        WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE,
        WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE,

        // Banner
        WIDX_BANNER_SPINNER_HEIGHT = PAGE_WIDGETS,
        WIDX_BANNER_SPINNER_HEIGHT_INCREASE,
        WIDX_BANNER_SPINNER_HEIGHT_DECREASE,
        WIDX_BANNER_CHECK_BLOCK_NE,
        WIDX_BANNER_CHECK_BLOCK_SE,
        WIDX_BANNER_CHECK_BLOCK_SW,
        WIDX_BANNER_CHECK_BLOCK_NW,
    };

    static_assert(WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE == WIDX_BUTTON_ROTATE);
    static_assert(WC_TILE_INSPECTOR__WIDX_BUTTON_COPY == WIDX_BUTTON_COPY);
    static_assert(WC_TILE_INSPECTOR__WIDX_BUTTON_PASTE == WIDX_BUTTON_PASTE);
    static_assert(WC_TILE_INSPECTOR__WIDX_BUTTON_SORT == WIDX_BUTTON_SORT);
    static_assert(WC_TILE_INSPECTOR__WIDX_BUTTON_REMOVE == WIDX_BUTTON_REMOVE);
    static_assert(WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_UP == WIDX_BUTTON_MOVE_UP);
    static_assert(WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_DOWN == WIDX_BUTTON_MOVE_DOWN);
    static_assert(WC_TILE_INSPECTOR__WIDX_SPINNER_X_INCREASE == WIDX_SPINNER_X_INCREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_SPINNER_X_DECREASE == WIDX_SPINNER_X_DECREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_SPINNER_Y_INCREASE == WIDX_SPINNER_Y_INCREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_SPINNER_Y_DECREASE == WIDX_SPINNER_Y_DECREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_SURFACE_SPINNER_HEIGHT_INCREASE == WIDX_SURFACE_SPINNER_HEIGHT_INCREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_SURFACE_SPINNER_HEIGHT_DECREASE == WIDX_SURFACE_SPINNER_HEIGHT_DECREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_PATH_SPINNER_HEIGHT_INCREASE == WIDX_PATH_SPINNER_HEIGHT_INCREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_PATH_SPINNER_HEIGHT_DECREASE == WIDX_PATH_SPINNER_HEIGHT_DECREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_TRACK_SPINNER_HEIGHT_INCREASE == WIDX_TRACK_SPINNER_HEIGHT_INCREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_TRACK_SPINNER_HEIGHT_DECREASE == WIDX_TRACK_SPINNER_HEIGHT_DECREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_SCENERY_SPINNER_HEIGHT_INCREASE == WIDX_SCENERY_SPINNER_HEIGHT_INCREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_SCENERY_SPINNER_HEIGHT_DECREASE == WIDX_SCENERY_SPINNER_HEIGHT_DECREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE == WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE == WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_WALL_SPINNER_HEIGHT_INCREASE == WIDX_WALL_SPINNER_HEIGHT_INCREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_WALL_SPINNER_HEIGHT_DECREASE == WIDX_WALL_SPINNER_HEIGHT_DECREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE == WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE == WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_BANNER_SPINNER_HEIGHT_INCREASE == WIDX_BANNER_SPINNER_HEIGHT_INCREASE);
    static_assert(WC_TILE_INSPECTOR__WIDX_BANNER_SPINNER_HEIGHT_DECREASE == WIDX_BANNER_SPINNER_HEIGHT_DECREASE);

#pragma region MEASUREMENTS

    static constexpr StringId kWindowTitle = STR_TILE_INSPECTOR_TITLE;

    // Window sizes
    static constexpr ScreenSize kWindowSize = { 400, 170 };
    static constexpr ScreenSize kMinimumWindowSize = { 400, 130 };
    static constexpr ScreenSize kMaximumWindowSize = { 400, 800 };

    // Button space for top buttons
    constexpr auto kToolbarButtonAnchor = ScreenCoordsXY{ kWindowSize.width - 27, 17 };
    constexpr auto kToolbarButtonSize = ScreenSize{ 24, 24 };
    constexpr auto kToolbarButtonHalfSize = ScreenSize{ 24, 12 };
    constexpr auto kToolbarButtonOffsetX = ScreenSize{ -24, 0 };

    // List's column offsets
    constexpr auto kInvisibleFlagColumnXY = ScreenCoordsXY{ 3, 42 };
    constexpr auto kInvisibleFlagColumnSize = ScreenSize{ 20, 14 };
    constexpr auto kTypeColumnXY = kInvisibleFlagColumnXY + ScreenSize{ kInvisibleFlagColumnSize.width, 0 };
    constexpr auto kTypeColumnSize = ScreenSize{ 252, 14 };
    constexpr auto kBaseHeightColumnXY = kTypeColumnXY + ScreenSize{ kTypeColumnSize.width, 0 };
    constexpr auto kBaseHeightColumnSize = ScreenSize{ 30, 14 };
    constexpr auto kClearanceHeightColumnXY = kBaseHeightColumnXY + ScreenCoordsXY{ kBaseHeightColumnSize.width, 0 };
    constexpr auto kClearanceHeightColumnSize = ScreenSize{ 30, 14 };
    constexpr auto kDirectionColumnXY = kClearanceHeightColumnXY + ScreenCoordsXY{ kClearanceHeightColumnSize.width, 0 };
    constexpr auto kDirectionColumnSize = ScreenSize{ 15, 14 };
    constexpr auto kGhostFlagColumnXY = kDirectionColumnXY + ScreenCoordsXY{ kDirectionColumnSize.width, 0 };
    constexpr auto kGhostFlagColumnSize = ScreenSize{ 15, 14 };
    constexpr auto kLastFlagColumnXY = kGhostFlagColumnXY + ScreenCoordsXY{ kGhostFlagColumnSize.width, 0 };
    constexpr auto kLastFlagColumnSize = ScreenSize{ 32, 14 };

    constexpr int32_t kBottomPadding = 15;
    constexpr int32_t kGroupboxPadding = 6;
    constexpr int32_t kHorizontalGroupboxPadding = 5;
    constexpr int32_t kVerticalGroupboxPadding = 4;
    constexpr auto kPropertySpinnerSize = ScreenSize{ 130, 14 };
    constexpr auto kPropertyButtonSize = ScreenSize{ 130, 18 };
    constexpr auto kPropertyFullWidth = ScreenSize{ 370, 18 };

#pragma endregion

    // clang-format off
    constexpr ScreenCoordsXY PropertyRowCol(ScreenCoordsXY anchor, int32_t row, int32_t column)
    {
        return anchor + ScreenCoordsXY{ column * (kPropertyButtonSize.width + kHorizontalGroupboxPadding),
                                        row * (kPropertyButtonSize.height + kVerticalGroupboxPadding) };
    }

    constexpr ScreenCoordsXY CheckboxGroupOffset(
        ScreenCoordsXY anchorPoint, int16_t horizontalMultiplier, int16_t verticalMultiplier)
    {
        return anchorPoint + ScreenCoordsXY{ 14 * horizontalMultiplier, 7 * verticalMultiplier };
    }

    static constexpr auto kMainTileInspectorWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({3, 57}, {kWindowSize.width - 6, kWindowSize.height - kBottomPadding - 58}, WidgetType::scroll, WindowColour::secondary, SCROLL_VERTICAL), /* Element list */
        /* X and Y spinners */
        makeWidget        ({ 4, 24}, {38, 12}, WidgetType::label,   WindowColour::secondary,  STR_TILE_INSPECTOR_X_LABEL),
        makeSpinnerWidgets({20, 23}, {51, 14}, WidgetType::spinner, WindowColour::secondary), /* Spinner X (3 widgets) */
        makeWidget        ({74, 24}, {38, 12}, WidgetType::label,   WindowColour::secondary,  STR_TILE_INSPECTOR_Y_LABEL),
        makeSpinnerWidgets({90, 23}, {51, 14}, WidgetType::spinner, WindowColour::secondary), /* Spinner Y (3 widgets) */
        /* Top buttons */
        makeWidget(kToolbarButtonAnchor + kToolbarButtonOffsetX * 0,                     kToolbarButtonSize,     WidgetType::flatBtn,     WindowColour::secondary, ImageId(SPR_DEMOLISH),     STR_REMOVE_SELECTED_ELEMENT_TIP ),    /* Remove button */
        makeWidget(kToolbarButtonAnchor + kToolbarButtonOffsetX * 1,                     kToolbarButtonHalfSize, WidgetType::button,      WindowColour::secondary, STR_UP,                    STR_MOVE_SELECTED_ELEMENT_UP_TIP),    /* Move up */
        makeWidget(kToolbarButtonAnchor + kToolbarButtonOffsetX * 1 + ScreenSize{0, 12}, kToolbarButtonHalfSize, WidgetType::button,      WindowColour::secondary, STR_DOWN,                  STR_MOVE_SELECTED_ELEMENT_DOWN_TIP),  /* Move down */
        makeWidget(kToolbarButtonAnchor + kToolbarButtonOffsetX * 2,                     kToolbarButtonSize,     WidgetType::flatBtn,     WindowColour::secondary, ImageId(SPR_ROTATE_ARROW), STR_ROTATE_SELECTED_ELEMENT_TIP),     /* Rotate button */
        makeWidget(kToolbarButtonAnchor + kToolbarButtonOffsetX * 3,                     kToolbarButtonSize,     WidgetType::flatBtn,     WindowColour::secondary, ImageId(SPR_G2_SORT),      STR_TILE_INSPECTOR_SORT_TIP),         /* Sort button */
        makeWidget(kToolbarButtonAnchor + kToolbarButtonOffsetX * 4,                     kToolbarButtonSize,     WidgetType::flatBtn,     WindowColour::secondary, ImageId(SPR_G2_PASTE),     STR_TILE_INSPECTOR_PASTE_TIP),        /* Paste button */
        makeWidget(kToolbarButtonAnchor + kToolbarButtonOffsetX * 5,                     kToolbarButtonSize,     WidgetType::flatBtn,     WindowColour::secondary, ImageId(SPR_G2_COPY),      STR_TILE_INSPECTOR_COPY_TIP),         /* Copy button */
        /* Column headers */
        makeWidget(kInvisibleFlagColumnXY,   kInvisibleFlagColumnSize,   WidgetType::tableHeader, WindowColour::secondary, STR_TILE_INSPECTOR_INVISIBLE_SHORT,        STR_TILE_INSPECTOR_FLAG_INVISIBLE),   /* Invisible flag */
        makeWidget(kTypeColumnXY,            kTypeColumnSize,            WidgetType::tableHeader, WindowColour::secondary, STR_TILE_INSPECTOR_ELEMENT_TYPE),                                                /* Type */
        makeWidget(kBaseHeightColumnXY,      kBaseHeightColumnSize,      WidgetType::tableHeader, WindowColour::secondary, STR_TILE_INSPECTOR_BASE_HEIGHT_SHORT,      STR_TILE_INSPECTOR_BASE_HEIGHT),      /* Base height */
        makeWidget(kClearanceHeightColumnXY, kClearanceHeightColumnSize, WidgetType::tableHeader, WindowColour::secondary, STR_TILE_INSPECTOR_CLEARANGE_HEIGHT_SHORT, STR_TILE_INSPECTOR_CLEARANCE_HEIGHT), /* Clearance height */
        makeWidget(kDirectionColumnXY,       kDirectionColumnSize,       WidgetType::tableHeader, WindowColour::secondary, STR_TILE_INSPECTOR_DIRECTION_SHORT,        STR_TILE_INSPECTOR_DIRECTION),        /* Direction */
        makeWidget(kGhostFlagColumnXY,       kGhostFlagColumnSize,       WidgetType::tableHeader, WindowColour::secondary, STR_TILE_INSPECTOR_FLAG_GHOST_SHORT,       STR_TILE_INSPECTOR_FLAG_GHOST),       /* Ghost flag */
        makeWidget(kLastFlagColumnXY,        kLastFlagColumnSize,        WidgetType::tableHeader, WindowColour::secondary, STR_TILE_INSPECTOR_FLAG_LAST_SHORT,        STR_TILE_INSPECTOR_FLAG_LAST),        /* Last of tile flag */
        /* Group boxes */
        makeWidget({6, 0},             {kWindowSize.width - 12, 0}, WidgetType::groupbox,    WindowColour::secondary, kStringIdNone,                          kStringIdNone ), /* Details group box */
        makeWidget({6, 0},             {kWindowSize.width - 12, 0}, WidgetType::groupbox,    WindowColour::secondary, STR_TILE_INSPECTOR_GROUPBOX_PROPERTIES, kStringIdNone )  /* Properties group box */
    );

    static constexpr auto kDefaultWidgets = makeWidgets(
        kMainTileInspectorWidgets
    );

    constexpr int32_t kNumSurfaceProperties = 4;
    constexpr int32_t kNumSurfaceDetails = 4;
    constexpr int32_t kSurfacePropertiesHeight = 20 + kNumSurfaceProperties * 21;
    constexpr int32_t kSurfaceDetailsHeight = 20 + kNumSurfaceDetails * 11;
    static constexpr auto kSurfaceWidgets = makeWidgets(
        kMainTileInspectorWidgets,
        makeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), kPropertySpinnerSize, WidgetType::spinner, WindowColour::secondary), // WIDX_SURFACE_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
        makeWidget(PropertyRowCol({ 12, 0 }, 1, 0),         kPropertyButtonSize, WidgetType::button,  WindowColour::secondary, STR_TILE_INSPECTOR_SURFACE_REMOVE_FENCES), // WIDX_SURFACE_BUTTON_REMOVE_FENCES
        makeWidget(PropertyRowCol({ 12, 0 }, 1, 1),         kPropertyButtonSize, WidgetType::button,  WindowColour::secondary, STR_TILE_INSPECTOR_SURFACE_RESTORE_FENCES), // WIDX_SURFACE_BUTTON_RESTORE_FENCES
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 2, 1), 1, 0), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_SURFACE_CHECK_CORNER_N
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 2, 1), 2, 1), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_SURFACE_CHECK_CORNER_E
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 2, 1), 1, 2), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_SURFACE_CHECK_CORNER_S
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 2, 1), 0, 1), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_SURFACE_CHECK_CORNER_W
        makeWidget(PropertyRowCol({ 12, 0 }, 3, 0), kPropertyFullWidth, WidgetType::checkbox, WindowColour::secondary, STR_TILE_INSPECTOR_SURFACE_DIAGONAL) // WIDX_SURFACE_CHECK_DIAGONAL
    );

    constexpr int32_t kNumPathProperties = 6;
    constexpr int32_t kNumPathDetails = 3;
    constexpr int32_t kPathPropertiesHeight = 20 + kNumPathProperties * 21;
    constexpr int32_t kPathDetailsHeight = 20 + kNumPathDetails * 11;
    static constexpr auto kPathWidgets = makeWidgets(
        kMainTileInspectorWidgets,
        makeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), kPropertySpinnerSize, WidgetType::spinner, WindowColour::secondary), // WIDX_PATH_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
        makeWidget(PropertyRowCol({ 12, 0 }, 1, 0), kPropertyFullWidth, WidgetType::checkbox, WindowColour::secondary, STR_TILE_INSPECTOR_PATH_BROKEN), // WIDX_PATH_CHECK_BROKEN
        makeWidget(PropertyRowCol({ 12, 0 }, 2, 0), kPropertyFullWidth, WidgetType::checkbox, WindowColour::secondary, STR_TILE_INSPECTOR_PATH_SLOPED), // WIDX_PATH_CHECK_SLOPED
        makeWidget(PropertyRowCol({ 12, 0 }, 3, 0), kPropertyFullWidth, WidgetType::checkbox, WindowColour::secondary, STR_TILE_INSPECTOR_PATH_JUNCTION_RAILINGS), // WIDX_PATH_CHECK_JUNCTION_RAILINGS
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 4, 1), 3, 1), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_PATH_CHECK_EDGE_NE
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 4, 1), 4, 2), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_PATH_CHECK_EDGE_E
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 4, 1), 3, 3), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_PATH_CHECK_EDGE_SE
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 4, 1), 2, 4), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_PATH_CHECK_EDGE_S
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 4, 1), 1, 3), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_PATH_CHECK_EDGE_SW
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 4, 1), 0, 2), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_PATH_CHECK_EDGE_W
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 4, 1), 1, 1), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_PATH_CHECK_EDGE_NW
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 4, 1), 2, 0), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary)  // WIDX_PATH_CHECK_EDGE_N
    );

    constexpr int32_t kNumTrackProperties = 5;
    constexpr int32_t kNumTrackDetails = 7;
    constexpr int32_t kTrackPropertiesHeight = 20 + kNumTrackProperties * 21;
    constexpr int32_t kTrackDetailsHeight = 20 + kNumTrackDetails * 11;
    static constexpr auto kTrackWidgets = makeWidgets(
        kMainTileInspectorWidgets,
        makeWidget(PropertyRowCol({ 12, 0}, 0, 0),          kPropertyFullWidth, WidgetType::checkbox, WindowColour::secondary, STR_TILE_INSPECTOR_TRACK_ENTIRE_TRACK_PIECE), // WIDX_TRACK_CHECK_APPLY_TO_ALL
        makeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 1, 1), kPropertySpinnerSize, WidgetType::spinner, WindowColour::secondary), // WIDX_TRACK_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
        makeWidget(PropertyRowCol({ 12, 0}, 2, 0),          kPropertyFullWidth, WidgetType::checkbox, WindowColour::secondary, STR_TILE_INSPECTOR_TRACK_CHAIN_LIFT), // WIDX_TRACK_CHECK_CHAIN_LIFT
        makeWidget(PropertyRowCol({ 12, 0}, 3, 0),          kPropertyFullWidth, WidgetType::checkbox, WindowColour::secondary, STR_TILE_INSPECTOR_TRACK_BRAKE_CLOSED), // WIDX_TRACK_CHECK_BRAKE_CLOSED
        makeWidget(PropertyRowCol({ 12, 0}, 4, 0),          kPropertyFullWidth, WidgetType::checkbox, WindowColour::secondary, STR_TILE_INSPECTOR_TRACK_IS_INDESTRUCTIBLE) // WIDX_TRACK_CHECK_IS_INDESTRUCTIBLE
    );

    constexpr int32_t kNumSceneryProperties = 4; // The checkbox groups both count for 2 rows
    constexpr int32_t kNumSceneryDetails = 3;
    constexpr int32_t kSceneryPropertiesHeight = 20 + kNumSceneryProperties * 21;
    constexpr int32_t kSceneryDetailsHeight = 20 + kNumSceneryDetails * 11;
    static constexpr auto kSceneryWidgets = makeWidgets(
        kMainTileInspectorWidgets,
        makeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), kPropertySpinnerSize, WidgetType::spinner, WindowColour::secondary), // WIDX_SCENERY_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 1, 1), 1, 0 + 0), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_SCENERY_CHECK_QUARTER_N
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 1, 1), 2, 0 + 1), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_SCENERY_CHECK_QUARTER_E
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 1, 1), 1, 0 + 2), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_SCENERY_CHECK_QUARTER_S
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 1, 1), 0, 0 + 1), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_SCENERY_CHECK_QUARTER_W
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 2, 1), 1, 1 + 0), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_SCENERY_CHECK_COLLISION_N
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 2, 1), 2, 1 + 1), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_SCENERY_CHECK_COLLISION_E
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 2, 1), 1, 1 + 2), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_SCENERY_CHECK_COLLISION_S
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 2, 1), 0, 1 + 1), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary)  // WIDX_SCENERY_CHECK_COLLISION_W
    );

    constexpr int32_t kNumEntranceProperties = 2;
    constexpr int32_t kNumEntranceDetails = 4;
    constexpr int32_t kEntrancePropertiesHeight = 20 + kNumEntranceProperties * 21;
    constexpr int32_t kEntranceDetailsHeight = 20 + kNumEntranceDetails * 11;
    static constexpr auto kEntranceWidgets = makeWidgets(
        kMainTileInspectorWidgets,
        makeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), kPropertySpinnerSize, WidgetType::spinner, WindowColour::secondary), // WIDX_ENTRANCE_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
        makeWidget(PropertyRowCol({ 12, 0 }, 1, 0),         kPropertyButtonSize, WidgetType::button,  WindowColour::secondary, STR_TILE_INSPECTOR_ENTRANCE_MAKE_USABLE, STR_TILE_INSPECTOR_ENTRANCE_MAKE_USABLE_TIP) // WIDX_ENTRANCE_BUTTON_MAKE_USABLE
    );

    constexpr int32_t kNumWallProperties = 4;
    constexpr int32_t kNumWallDetails = 2;
    constexpr int32_t kWallPropertiesHeight = 20 + kNumWallProperties * 21;
    constexpr int32_t kWallDetailsHeight = 20 + kNumWallDetails * 11;
    static constexpr auto kWallWidgets = makeWidgets(
        kMainTileInspectorWidgets,
        makeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1),                          kPropertySpinnerSize, WidgetType::spinner,      WindowColour::secondary), // WIDX_WALL_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
        makeWidget(PropertyRowCol({ 12, 0 }, 1, 1),                                  kPropertyButtonSize, WidgetType::dropdownMenu, WindowColour::secondary), // WIDX_WALL_DROPDOWN_SLOPE
        makeWidget(PropertyRowCol({ 12 + kPropertyButtonSize.width - 12, 0 }, 1, 1), { 11,  12},          WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH), // WIDX_WALL_DROPDOWN_SLOPE_BUTTON
        makeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 2, 1),                          kPropertySpinnerSize, WidgetType::spinner,      WindowColour::secondary), // WIDX_WALL_SPINNER_ANIMATION_FRAME{,_INCREASE,_DECREASE}
        makeWidget(PropertyRowCol({ 12, 0 }, 3, 0),                                  kPropertyFullWidth,  WidgetType::checkbox,     WindowColour::secondary, STR_TILE_INSPECTOR_WALL_ANIMATION_IS_BACKWARDS) // WIDX_WALL_ANIMATION_IS_BACKWARDS
    );

    constexpr int32_t kNumLargeSceneryProperties = 1;
    constexpr int32_t kNumLargeSceneryDetails = 3;
    constexpr int32_t kLargeSceneryPropertiesHeight = 20 + kNumLargeSceneryProperties * 21;
    constexpr int32_t kLargeSceneryDetailsHeight = 20 + kNumLargeSceneryDetails * 11;
    static constexpr auto kLargeSceneryWidgets = makeWidgets(
        kMainTileInspectorWidgets,
        makeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), kPropertySpinnerSize, WidgetType::spinner, WindowColour::secondary) // WIDX_LARGE_SCENERY_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
    );

    constexpr int32_t kNumBannerProperties = 3;
    constexpr int32_t kNumBannerDetails = 1;
    constexpr int32_t kBannerPropertiesHeight = 20 + kNumBannerProperties * 21;
    constexpr int32_t kBannerDetailsHeight = 20 + kNumBannerDetails * 11;
    static constexpr auto kBannerWidgets = makeWidgets(
        kMainTileInspectorWidgets,
        makeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), kPropertySpinnerSize, WidgetType::spinner, WindowColour::secondary), // WIDX_BANNER_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 1, 1), 3, 1), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_BANNER_CHECK_BLOCK_NE
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 1, 1), 3, 3), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_BANNER_CHECK_BLOCK_SE
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 1, 1), 1, 3), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary), // WIDX_BANNER_CHECK_BLOCK_SW
        makeWidget(CheckboxGroupOffset(PropertyRowCol({ 12, 0 }, 1, 1), 1, 1), { 12, 12 }, WidgetType::checkbox, WindowColour::secondary)  // WIDX_BANNER_CHECK_BLOCK_NW
    );

    static constexpr std::span<const Widget> kWidgetsByPage[] = {
        kDefaultWidgets,
        kSurfaceWidgets,
        kPathWidgets,
        kTrackWidgets,
        kSceneryWidgets,
        kEntranceWidgets,
        kWallWidgets,
        kLargeSceneryWidgets,
        kBannerWidgets,
    };
    // clang-format on

    struct GroupboxSettings
    {
        // Offsets from the bottom of the window
        int16_t details_top_offset, details_bottom_offset;
        int16_t properties_top_offset, properties_bottom_offset;
        // String to be displayed in the details groupbox
        StringId string_id;
    };

    static constexpr GroupboxSettings MakeGroupboxSettings(int16_t detailsHeight, int16_t propertiesHeight, StringId stringId)
    {
        GroupboxSettings settings{};
        decltype(settings.properties_bottom_offset) offsetSum = 0;
        settings.properties_bottom_offset = (offsetSum += kBottomPadding);
        settings.properties_top_offset = (offsetSum += propertiesHeight);
        settings.details_bottom_offset = (offsetSum += kGroupboxPadding);
        settings.details_top_offset = (offsetSum += detailsHeight);
        settings.string_id = stringId;
        return settings;
    }

    static constexpr GroupboxSettings kPageGroupBoxSettings[] = {
        MakeGroupboxSettings(kSurfaceDetailsHeight, kSurfacePropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_SURFACE_INFO),
        MakeGroupboxSettings(kPathDetailsHeight, kPathPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_PATH_INFO),
        MakeGroupboxSettings(kTrackDetailsHeight, kTrackPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_TRACK_INFO),
        MakeGroupboxSettings(kSceneryDetailsHeight, kSceneryPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_SCENERY_INFO),
        MakeGroupboxSettings(kEntranceDetailsHeight, kEntrancePropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_ENTRANCE_INFO),
        MakeGroupboxSettings(kWallDetailsHeight, kWallPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_WALL_INFO),
        MakeGroupboxSettings(
            kLargeSceneryDetailsHeight, kLargeSceneryPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_LARGE_SCENERY_INFO),
        MakeGroupboxSettings(kBannerDetailsHeight, kBannerPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_BANNER_INFO),
    };

    static constexpr int32_t ViewportInteractionFlags = EnumsToFlags(
        ViewportInteractionItem::terrain, ViewportInteractionItem::ride, ViewportInteractionItem::scenery,
        ViewportInteractionItem::footpath, ViewportInteractionItem::pathAddition, ViewportInteractionItem::parkEntrance,
        ViewportInteractionItem::wall, ViewportInteractionItem::largeScenery, ViewportInteractionItem::banner);

    // clang-format off
    static uint64_t kHoldableWidgetsByPage[] = {
        (1uLL << WIDX_SPINNER_X_INCREASE) | (1uLL << WIDX_SPINNER_X_DECREASE) | (1uLL << WIDX_SPINNER_Y_INCREASE) | (1uLL << WIDX_SPINNER_Y_DECREASE),
        (1uLL << WIDX_SPINNER_X_INCREASE) | (1uLL << WIDX_SPINNER_X_DECREASE) | (1uLL << WIDX_SPINNER_Y_INCREASE) | (1uLL << WIDX_SPINNER_Y_DECREASE) | (1uLL << WIDX_SURFACE_SPINNER_HEIGHT_INCREASE) | (1uLL << WIDX_SURFACE_SPINNER_HEIGHT_DECREASE),
        (1uLL << WIDX_SPINNER_X_INCREASE) | (1uLL << WIDX_SPINNER_X_DECREASE) | (1uLL << WIDX_SPINNER_Y_INCREASE) | (1uLL << WIDX_SPINNER_Y_DECREASE) | (1uLL << WIDX_PATH_SPINNER_HEIGHT_INCREASE) | (1uLL << WIDX_PATH_SPINNER_HEIGHT_DECREASE),
        (1uLL << WIDX_SPINNER_X_INCREASE) | (1uLL << WIDX_SPINNER_X_DECREASE) | (1uLL << WIDX_SPINNER_Y_INCREASE) | (1uLL << WIDX_SPINNER_Y_DECREASE) | (1uLL << WIDX_TRACK_SPINNER_HEIGHT_INCREASE) | (1uLL << WIDX_TRACK_SPINNER_HEIGHT_DECREASE),
        (1uLL << WIDX_SPINNER_X_INCREASE) | (1uLL << WIDX_SPINNER_X_DECREASE) | (1uLL << WIDX_SPINNER_Y_INCREASE) | (1uLL << WIDX_SPINNER_Y_DECREASE) | (1uLL << WIDX_SCENERY_SPINNER_HEIGHT_INCREASE) | (1uLL << WIDX_SCENERY_SPINNER_HEIGHT_DECREASE),
        (1uLL << WIDX_SPINNER_X_INCREASE) | (1uLL << WIDX_SPINNER_X_DECREASE) | (1uLL << WIDX_SPINNER_Y_INCREASE) | (1uLL << WIDX_SPINNER_Y_DECREASE) | (1uLL << WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE) | (1uLL << WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE),
        (1uLL << WIDX_SPINNER_X_INCREASE) | (1uLL << WIDX_SPINNER_X_DECREASE) | (1uLL << WIDX_SPINNER_Y_INCREASE) | (1uLL << WIDX_SPINNER_Y_DECREASE) | (1uLL << WIDX_WALL_SPINNER_HEIGHT_INCREASE) | (1uLL << WIDX_WALL_SPINNER_HEIGHT_DECREASE) | (1uLL << WIDX_WALL_SPINNER_ANIMATION_FRAME_INCREASE) | (1uLL << WIDX_WALL_SPINNER_ANIMATION_FRAME_DECREASE),
        (1uLL << WIDX_SPINNER_X_INCREASE) | (1uLL << WIDX_SPINNER_X_DECREASE) | (1uLL << WIDX_SPINNER_Y_INCREASE) | (1uLL << WIDX_SPINNER_Y_DECREASE) | (1uLL << WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE) | (1uLL << WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE),
        (1uLL << WIDX_SPINNER_X_INCREASE) | (1uLL << WIDX_SPINNER_X_DECREASE) | (1uLL << WIDX_SPINNER_Y_INCREASE) | (1uLL << WIDX_SPINNER_Y_DECREASE) | (1uLL << WIDX_BANNER_SPINNER_HEIGHT_INCREASE) | (1uLL << WIDX_BANNER_SPINNER_HEIGHT_DECREASE),
    };

    static uint64_t kDisabledWidgetsByPage[] = {
        (1uLL << WIDX_BUTTON_MOVE_UP) | (1uLL << WIDX_BUTTON_MOVE_DOWN) | (1uLL << WIDX_BUTTON_REMOVE) | (1uLL << WIDX_BUTTON_ROTATE) | (1uLL << WIDX_BUTTON_COPY),
        0,
        0,
        0,
        0,
        0,
        0,
        (1uLL << WIDX_BUTTON_ROTATE),
        0,
    };
    // clang-format on

    class TileInspector final : public Window
    {
    private:
        int16_t _highlightedIndex = -1;
        bool _tileSelected = false;
        int32_t _toolMouseX = 0;
        int32_t _toolMouseY = 0;
        bool _toolCtrlDown = false;
        CoordsXY _toolMap = {};
        bool _applyToAll = false;
        bool _elementCopied = false;
        TileElement _copiedElement;
        Banner _copiedBanner;

    public:
        void onOpen() override
        {
            WindowSetResize(*this, kMinimumWindowSize, kMaximumWindowSize);

            windowTileInspectorSelectedIndex = -1;
            setPage(TileInspectorPage::Default);
            WindowInitScrollWidgets(*this);
            _tileSelected = false;

            ToolSet(*this, WIDX_BACKGROUND, Tool::crosshair);
        }

        void onUpdate() override
        {
            // Check if the mouse is hovering over the list
            if (!widgetIsHighlighted(*this, WIDX_LIST))
            {
                if (_highlightedIndex != -1)
                    invalidateWidget(WIDX_LIST);
                _highlightedIndex = -1;
            }

            if (!isToolActive(WindowClass::tileInspector))
                close();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    return;

                case WIDX_BUTTON_REMOVE:
                {
                    RemoveElement(windowTileInspectorSelectedIndex);
                    break;
                }

                case WIDX_BUTTON_ROTATE:
                    RotateElement(windowTileInspectorSelectedIndex);
                    break;

                case WIDX_BUTTON_SORT:
                    SortElements();
                    break;

                case WIDX_BUTTON_COPY:
                    CopyElement();
                    break;

                case WIDX_BUTTON_PASTE:
                    PasteElement();
                    break;

                case WIDX_BUTTON_MOVE_UP:
                    SwapElements(windowTileInspectorSelectedIndex, windowTileInspectorSelectedIndex + 1);
                    break;

                case WIDX_BUTTON_MOVE_DOWN:
                    SwapElements(windowTileInspectorSelectedIndex - 1, windowTileInspectorSelectedIndex);
                    break;
            }

            // only element-specific widgets from now on
            if (tileInspectorPage == TileInspectorPage::Default || windowTileInspectorSelectedIndex == -1)
                return;

            const TileElement* const tileElement = OpenRCT2::TileInspector::GetSelectedElement();

            if (tileElement == nullptr)
                return;

            // Page widgets
            switch (tileElement->GetType())
            {
                case TileElementType::Surface:
                    switch (widgetIndex)
                    {
                        case WIDX_SURFACE_BUTTON_REMOVE_FENCES:
                            SurfaceShowParkFences(false);
                            break;

                        case WIDX_SURFACE_BUTTON_RESTORE_FENCES:
                            SurfaceShowParkFences(true);
                            break;

                        case WIDX_SURFACE_CHECK_CORNER_N:
                        case WIDX_SURFACE_CHECK_CORNER_E:
                        case WIDX_SURFACE_CHECK_CORNER_S:
                        case WIDX_SURFACE_CHECK_CORNER_W:
                            SurfaceToggleCorner(((widgetIndex - WIDX_SURFACE_CHECK_CORNER_N) + 2 - GetCurrentRotation()) & 3);
                            break;

                        case WIDX_SURFACE_CHECK_DIAGONAL:
                            SurfaceToggleDiagonal();
                            break;
                    } // switch widgetindex
                    break;
                case TileElementType::Path:
                    switch (widgetIndex)
                    {
                        case WIDX_PATH_CHECK_SLOPED:
                            PathSetSloped(windowTileInspectorSelectedIndex, !tileElement->AsPath()->IsSloped());
                            break;
                        case WIDX_PATH_CHECK_JUNCTION_RAILINGS:
                            PathSetJunctionRailings(
                                windowTileInspectorSelectedIndex, !tileElement->AsPath()->HasJunctionRailings());
                            break;

                        case WIDX_PATH_CHECK_BROKEN:
                            PathSetBroken(windowTileInspectorSelectedIndex, !tileElement->AsPath()->IsBroken());
                            break;

                        case WIDX_PATH_CHECK_EDGE_E:
                        case WIDX_PATH_CHECK_EDGE_S:
                        case WIDX_PATH_CHECK_EDGE_W:
                        case WIDX_PATH_CHECK_EDGE_N:
                        {
                            // 0 = east/right, 1 = south/bottom, 2 = west/left, 3 = north/top
                            const int32_t eswn = (widgetIndex - WIDX_PATH_CHECK_EDGE_E) / 2;
                            // Transform to world orientation
                            const int32_t index = (eswn - GetCurrentRotation()) & 3;
                            PathToggleEdge(
                                windowTileInspectorSelectedIndex,
                                index + 4); // The corners are stored in the 4 most significant bits, hence the + 4
                            break;
                        }

                        case WIDX_PATH_CHECK_EDGE_NE:
                        case WIDX_PATH_CHECK_EDGE_SE:
                        case WIDX_PATH_CHECK_EDGE_SW:
                        case WIDX_PATH_CHECK_EDGE_NW:
                        {
                            // 0 = NE, 1 = SE, 2 = SW, 3 = NW
                            const int32_t neseswnw = (widgetIndex - WIDX_PATH_CHECK_EDGE_NE) / 2;
                            // Transform to world orientation
                            const int32_t index = (neseswnw - GetCurrentRotation()) & 3;
                            PathToggleEdge(windowTileInspectorSelectedIndex, index);
                            break;
                        }
                    } // switch widget index
                    break;

                case TileElementType::Track:
                    switch (widgetIndex)
                    {
                        case WIDX_TRACK_CHECK_APPLY_TO_ALL:
                            _applyToAll ^= 1;
                            invalidateWidget(widgetIndex);
                            break;

                        case WIDX_TRACK_CHECK_CHAIN_LIFT:
                        {
                            bool entireTrackBlock = isWidgetPressed(WIDX_TRACK_CHECK_APPLY_TO_ALL);
                            bool newLift = !tileElement->AsTrack()->HasChain();
                            TrackBlockSetLift(windowTileInspectorSelectedIndex, entireTrackBlock, newLift);
                            break;
                        }

                        case WIDX_TRACK_CHECK_BRAKE_CLOSED:
                            TrackSetBrakeClosed(windowTileInspectorSelectedIndex, !tileElement->AsTrack()->IsBrakeClosed());
                            break;

                        case WIDX_TRACK_CHECK_IS_INDESTRUCTIBLE:
                            TrackSetIndestructible(
                                windowTileInspectorSelectedIndex, !tileElement->AsTrack()->IsIndestructible());
                            break;
                    } // switch widget index
                    break;

                case TileElementType::SmallScenery:
                    switch (widgetIndex)
                    {
                        case WIDX_SCENERY_CHECK_QUARTER_N:
                        case WIDX_SCENERY_CHECK_QUARTER_E:
                        case WIDX_SCENERY_CHECK_QUARTER_S:
                        case WIDX_SCENERY_CHECK_QUARTER_W:
                            QuarterTileSet(windowTileInspectorSelectedIndex, widgetIndex - WIDX_SCENERY_CHECK_QUARTER_N);
                            break;

                        case WIDX_SCENERY_CHECK_COLLISION_N:
                        case WIDX_SCENERY_CHECK_COLLISION_E:
                        case WIDX_SCENERY_CHECK_COLLISION_S:
                        case WIDX_SCENERY_CHECK_COLLISION_W:
                            ToggleQuadrantCollosion(
                                windowTileInspectorSelectedIndex, widgetIndex - WIDX_SCENERY_CHECK_COLLISION_N);
                            break;
                    } // switch widget index
                    break;

                case TileElementType::Entrance:
                    switch (widgetIndex)
                    {
                        case WIDX_ENTRANCE_BUTTON_MAKE_USABLE:
                            EntranceMakeUsable(windowTileInspectorSelectedIndex);
                            break;
                    } // switch widget index
                    break;

                case TileElementType::Banner:
                    switch (widgetIndex)
                    {
                        case WIDX_BANNER_CHECK_BLOCK_NE:
                        case WIDX_BANNER_CHECK_BLOCK_SE:
                        case WIDX_BANNER_CHECK_BLOCK_SW:
                        case WIDX_BANNER_CHECK_BLOCK_NW:
                            BannerToggleBlock(windowTileInspectorSelectedIndex, widgetIndex - WIDX_BANNER_CHECK_BLOCK_NE);
                            break;
                    } // switch widget index
                    break;

                case TileElementType::LargeScenery:
                case TileElementType::Wall:
                    switch (widgetIndex)
                    {
                        case WIDX_WALL_ANIMATION_IS_BACKWARDS:
                            WallSetAnimationIsBackwards(
                                windowTileInspectorSelectedIndex, !tileElement->AsWall()->AnimationIsBackwards());
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        void onClose() override
        {
            if (isToolActive(WindowClass::tileInspector))
                ToolCancel();

            TileElement* const elem = OpenRCT2::TileInspector::GetSelectedElement();
            if (elem != nullptr)
            {
                MapInvalidateElement(_toolMap, elem);
            }
            windowTileInspectorSelectedIndex = -1;
        }

        void onResize() override
        {
            if (width < minWidth)
            {
                invalidate();
                width = minWidth;
            }
            if (height < minHeight)
            {
                invalidate();
                height = minHeight;
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_SPINNER_X_INCREASE:
                    windowTileInspectorTile.x = std::min<int32_t>(windowTileInspectorTile.x + 1, kMaximumMapSizeTechnical - 1);
                    _toolMap.x = std::min<int32_t>(_toolMap.x + 32, kMaximumTileStartXY);
                    LoadTile(nullptr);
                    break;

                case WIDX_SPINNER_X_DECREASE:
                    windowTileInspectorTile.x = std::max<int32_t>(windowTileInspectorTile.x - 1, 0);
                    _toolMap.x = std::max<int32_t>(_toolMap.x - 32, 0);
                    LoadTile(nullptr);
                    break;

                case WIDX_SPINNER_Y_INCREASE:
                    windowTileInspectorTile.y = std::min<int32_t>(windowTileInspectorTile.y + 1, kMaximumMapSizeTechnical - 1);
                    _toolMap.y = std::min<int32_t>(_toolMap.y + 32, kMaximumTileStartXY);
                    LoadTile(nullptr);
                    break;

                case WIDX_SPINNER_Y_DECREASE:
                    windowTileInspectorTile.y = std::max<int32_t>(windowTileInspectorTile.y - 1, 0);
                    _toolMap.y = std::max<int32_t>(_toolMap.y - 32, 0);
                    LoadTile(nullptr);
                    break;
            } // switch widget index

            // only element-specific widgets from now on
            if (tileInspectorPage == TileInspectorPage::Default || windowTileInspectorSelectedIndex == -1)
                return;

            const TileElement* tileElement = OpenRCT2::TileInspector::GetSelectedElement();
            if (tileElement == nullptr)
                return;

            switch (tileElement->GetType())
            {
                case TileElementType::Surface:
                    switch (widgetIndex)
                    {
                        case WIDX_SURFACE_SPINNER_HEIGHT_INCREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, 1);
                            break;

                        case WIDX_SURFACE_SPINNER_HEIGHT_DECREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, -1);
                            break;
                    } // switch widget index
                    break;

                case TileElementType::Path:
                    switch (widgetIndex)
                    {
                        case WIDX_PATH_SPINNER_HEIGHT_INCREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, 1);
                            break;

                        case WIDX_PATH_SPINNER_HEIGHT_DECREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, -1);
                            break;
                    } // switch widget index
                    break;

                case TileElementType::Track:
                    switch (widgetIndex)
                    {
                        case WIDX_TRACK_SPINNER_HEIGHT_INCREASE:
                            if (isWidgetPressed(WIDX_TRACK_CHECK_APPLY_TO_ALL))
                                TrackBlockHeightOffset(windowTileInspectorSelectedIndex, 1);
                            else
                                BaseHeightOffset(windowTileInspectorSelectedIndex, 1);
                            break;

                        case WIDX_TRACK_SPINNER_HEIGHT_DECREASE:
                            if (isWidgetPressed(WIDX_TRACK_CHECK_APPLY_TO_ALL))
                                TrackBlockHeightOffset(windowTileInspectorSelectedIndex, -1);
                            else
                                BaseHeightOffset(windowTileInspectorSelectedIndex, -1);
                            break;
                    } // switch widget index
                    break;

                case TileElementType::SmallScenery:
                    switch (widgetIndex)
                    {
                        case WIDX_SCENERY_SPINNER_HEIGHT_INCREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, 1);
                            break;

                        case WIDX_SCENERY_SPINNER_HEIGHT_DECREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, -1);
                            break;
                    } // switch widget index
                    break;

                case TileElementType::Entrance:
                    switch (widgetIndex)
                    {
                        case WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, 1);
                            break;

                        case WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, -1);
                            break;

                        case WIDX_ENTRANCE_BUTTON_MAKE_USABLE:
                            EntranceMakeUsable(windowTileInspectorSelectedIndex);
                            break;
                    } // switch widget index
                    break;

                case TileElementType::Wall:
                    switch (widgetIndex)
                    {
                        case WIDX_WALL_SPINNER_HEIGHT_INCREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, 1);
                            break;

                        case WIDX_WALL_SPINNER_HEIGHT_DECREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, -1);
                            break;

                        case WIDX_WALL_DROPDOWN_SLOPE_BUTTON:
                        {
                            Widget* widget = &widgets[widgetIndex];
                            // Use dropdown instead of dropdown button
                            widget--;
                            // Fill dropdown list
                            gDropdown.items[0] = Dropdown::MenuLabel(STR_TILE_INSPECTOR_WALL_FLAT);
                            gDropdown.items[1] = Dropdown::MenuLabel(STR_TILE_INSPECTOR_WALL_SLOPED_LEFT);
                            gDropdown.items[2] = Dropdown::MenuLabel(STR_TILE_INSPECTOR_WALL_SLOPED_RIGHT);
                            WindowDropdownShowTextCustomWidth(
                                { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height(), colours[1], 0,
                                Dropdown::Flag::StayOpen, 3, widget->width() - 4);

                            // Set current value as checked
                            gDropdown.items[tileElement->AsWall()->GetSlope()].setChecked(true);
                            break;
                        }

                        case WIDX_WALL_SPINNER_ANIMATION_FRAME_INCREASE:
                            WallAnimationFrameOffset(windowTileInspectorSelectedIndex, 1);
                            break;

                        case WIDX_WALL_SPINNER_ANIMATION_FRAME_DECREASE:
                            WallAnimationFrameOffset(windowTileInspectorSelectedIndex, -1);
                            break;
                    } // switch widget index
                    break;

                case TileElementType::LargeScenery:
                    switch (widgetIndex)
                    {
                        case WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, 1);
                            break;

                        case WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, -1);
                            break;
                    } // switch widget index
                    break;

                case TileElementType::Banner:
                    switch (widgetIndex)
                    {
                        case WIDX_BANNER_SPINNER_HEIGHT_INCREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, 1);
                            break;

                        case WIDX_BANNER_SPINNER_HEIGHT_DECREASE:
                            BaseHeightOffset(windowTileInspectorSelectedIndex, -1);
                            break;
                    } // switch widget index
                    break;

                default:
                    break;
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (dropdownIndex == -1)
                return;
            // Get selected element
            const TileElement* const tileElement = OpenRCT2::TileInspector::GetSelectedElement();
            if (tileInspectorPage == TileInspectorPage::Wall)
            {
                Guard::Assert(tileElement->GetType() == TileElementType::Wall, "Element is not a wall");
                if (widgetIndex == WIDX_WALL_DROPDOWN_SLOPE_BUTTON)
                    WallSetSlope(windowTileInspectorSelectedIndex, dropdownIndex);
            }
        }

        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            gMapSelectFlags.set(MapSelectFlag::enable);

            CoordsXY mapCoords;
            TileElement* clickedElement = nullptr;
            bool mouseOnViewport = false;
            if (GetInputManager().isModifierKeyPressed(ModifierKey::ctrl))
            {
                auto info = GetMapCoordinatesFromPos(screenCoords, ViewportInteractionFlags);
                clickedElement = info.Element;
                mapCoords = info.Loc;
            }
            // Even if Ctrl was pressed, fall back to normal selection when there was nothing under the cursor
            if (clickedElement == nullptr)
            {
                auto mouseCoords = ScreenPosToMapPos(screenCoords, nullptr);
                if (mouseCoords.has_value())
                {
                    mouseOnViewport = true;
                    mapCoords = mouseCoords.value();
                }
            }
            if (mouseOnViewport)
                gMapSelectPositionA = gMapSelectPositionB = mapCoords;
            else if (_tileSelected)
                gMapSelectPositionA = gMapSelectPositionB = _toolMap;
            else
                gMapSelectFlags.unset(MapSelectFlag::enable);

            gMapSelectType = MapSelectType::full;
        }

        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            UpdateSelectedTile(screenCoords);
        }

        void onToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            UpdateSelectedTile(screenCoords);
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            return ScreenSize(kWindowSize.width - 30, windowTileInspectorElementCount * kScrollableRowHeight);
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            // There is nothing to interact with when no tile is selected
            if (!_tileSelected)
                return;

            // Because the list items are displayed in reverse order, subtract the calculated index from the amount of elements
            const int16_t index = windowTileInspectorElementCount - (screenCoords.y - 1) / kScrollableRowHeight - 1;
            const ScreenRect checkboxColumnRect{ { 2, 0 }, { 15, screenCoords.y } };
            if (index >= 0 && checkboxColumnRect.Contains(screenCoords))
            { // Checkbox was clicked
                ToggleInvisibility(index);
            }
            else
            {
                SelectElementFromList(index);
            }
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            int16_t index = windowTileInspectorElementCount - (screenCoords.y - 1) / kScrollableRowHeight - 1;
            if (index < 0 || index >= windowTileInspectorElementCount)
                _highlightedIndex = -1;
            else
                _highlightedIndex = index;
            invalidateWidget(WIDX_LIST);
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
            ScreenCoordsXY screenCoords(windowPos.x, windowPos.y);

            // Draw coordinates
            auto yOffset = widgets[WIDX_SPINNER_X_LABEL].textTop();
            if (_tileSelected)
            {
                auto tileCoords = TileCoordsXY{ _toolMap };
                auto ft = Formatter();
                ft.Add<int32_t>(tileCoords.x);
                drawText(
                    rt, screenCoords + ScreenCoordsXY{ 43, yOffset }, STR_FORMAT_INTEGER, ft,
                    { colours[1], TextAlignment::right });
                ft = Formatter();
                ft.Add<int32_t>(tileCoords.y);
                drawText(
                    rt, screenCoords + ScreenCoordsXY{ 113, yOffset }, STR_FORMAT_INTEGER, ft,
                    { colours[1], TextAlignment::right });
            }
            else
            {
                drawText(rt, screenCoords + ScreenCoordsXY(43 - 7, yOffset), "-", { colours[1] });
                drawText(rt, screenCoords + ScreenCoordsXY(113 - 7, yOffset), "-", { colours[1] });
            }

            if (windowTileInspectorSelectedIndex != -1)
            {
                // X and Y of first element in detail box
                screenCoords = windowPos
                    + ScreenCoordsXY{ widgets[WIDX_GROUPBOX_DETAILS].left + 7, widgets[WIDX_GROUPBOX_DETAILS].top + 14 };

                // Get map element
                const TileElement* const tileElement = OpenRCT2::TileInspector::GetSelectedElement();
                if (tileElement == nullptr)
                    return;

                switch (tileElement->GetType())
                {
                    case TileElementType::Surface:
                    {
                        auto* surfaceEl = tileElement->AsSurface();
                        onDrawSurface(rt, screenCoords, *surfaceEl);
                        break;
                    }
                    case TileElementType::Path:
                    {
                        auto* pathEl = tileElement->AsPath();
                        onDrawPath(rt, screenCoords, *pathEl);
                        break;
                    }

                    case TileElementType::Track:
                    {
                        auto* trackEl = tileElement->AsTrack();
                        onDrawTrack(rt, screenCoords, *trackEl);
                        break;
                    }

                    case TileElementType::SmallScenery:
                    {
                        auto* smallSceneryEl = tileElement->AsSmallScenery();
                        onDrawSmallScenery(rt, screenCoords, *smallSceneryEl);
                        break;
                    }

                    case TileElementType::Entrance:
                    {
                        auto* entranceEl = tileElement->AsEntrance();
                        onDrawEntrance(rt, screenCoords, *entranceEl);
                        break;
                    }

                    case TileElementType::Wall:
                    {
                        auto* wallEl = tileElement->AsWall();
                        onDrawWall(rt, screenCoords, *wallEl);
                        break;
                    }

                    case TileElementType::LargeScenery:
                    {
                        auto* largeSceneryEl = tileElement->AsLargeScenery();
                        onDrawLargeScenery(rt, screenCoords, *largeSceneryEl);
                        break;
                    }

                    case TileElementType::Banner:
                    {
                        auto* bannerEl = tileElement->AsBanner();
                        onDrawBanner(rt, screenCoords, *bannerEl);
                        break;
                    }

                    default:
                        break;
                }
            }
        }

        void onDrawSurface(RenderTarget& rt, ScreenCoordsXY screenCoords, const SurfaceElement& surfaceEl)
        {
            // Details
            // Terrain texture name
            StringId terrainNameId = kStringIdEmpty;
            auto surfaceStyle = surfaceEl.GetSurfaceObject();
            if (surfaceStyle != nullptr)
                terrainNameId = surfaceStyle->NameStringId;
            auto ft = Formatter();
            ft.Add<StringId>(terrainNameId);
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_SURFACE_TERAIN, ft, { colours[1] });

            // Edge texture name
            StringId terrainEdgeNameId = kStringIdEmpty;
            auto edgeStyle = surfaceEl.GetEdgeObject();
            if (edgeStyle != nullptr)
                terrainEdgeNameId = edgeStyle->NameStringId;
            ft = Formatter();
            ft.Add<StringId>(terrainEdgeNameId);
            drawText(rt, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_SURFACE_EDGE, ft, { colours[1] });

            // Land ownership
            StringId landOwnership;
            if (surfaceEl.GetOwnership() & OWNERSHIP_OWNED)
                landOwnership = STR_LAND_OWNED;
            else if (surfaceEl.GetOwnership() & OWNERSHIP_AVAILABLE)
                landOwnership = STR_LAND_SALE;
            else if (surfaceEl.GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
                landOwnership = STR_CONSTRUCTION_RIGHTS_OWNED;
            else if (surfaceEl.GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE)
                landOwnership = STR_CONSTRUCTION_RIGHTS_SALE;
            else
                landOwnership = STR_TILE_INSPECTOR_LAND_NOT_OWNED_AND_NOT_AVAILABLE;

            ft = Formatter();
            ft.Add<StringId>(landOwnership);
            drawText(rt, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_SURFACE_OWNERSHIP, ft, { colours[1] });

            // Water level
            ft = Formatter();
            ft.Add<uint32_t>(surfaceEl.GetWaterHeight());
            drawText(rt, screenCoords + ScreenCoordsXY{ 0, 33 }, STR_TILE_INSPECTOR_SURFACE_WATER_LEVEL, ft, { colours[1] });

            // Properties
            // Raise / lower label
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_GROUPBOX_DETAILS].left + 7, widgets[WIDX_SURFACE_SPINNER_HEIGHT].top + 1 };
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, { colours[1] });

            // Current base height
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_SURFACE_SPINNER_HEIGHT].left + 3,
                                  widgets[WIDX_SURFACE_SPINNER_HEIGHT].textTop() };
            ft = Formatter();
            ft.Add<int32_t>(surfaceEl.BaseHeight);
            drawText(rt, screenCoords, STR_FORMAT_INTEGER, ft, { colours[1] });

            // Raised corners
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_GROUPBOX_DETAILS].left + 7, widgets[WIDX_SURFACE_CHECK_CORNER_E].top };
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_SURFACE_CORNERS, { colours[1] });
        }

        void onDrawPath(RenderTarget& rt, ScreenCoordsXY screenCoords, const PathElement& pathEl)
        {
            // Details
            auto footpathObj = pathEl.GetLegacyPathEntry();
            if (footpathObj == nullptr)
            {
                // Surface name
                auto surfaceObj = pathEl.GetSurfaceEntry();
                if (surfaceObj != nullptr)
                {
                    auto ft = Formatter();
                    ft.Add<StringId>(surfaceObj->NameStringId);
                    drawText(rt, screenCoords, STR_TILE_INSPECTOR_FOOTPATH_SURFACE_NAME, ft, { Drawing::Colour::white });
                }

                // Railings name
                auto railingsObj = pathEl.GetRailingsEntry();
                if (railingsObj != nullptr)
                {
                    auto ft = Formatter();
                    ft.Add<StringId>(railingsObj->NameStringId);
                    drawText(
                        rt, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_FOOTPATH_RAILINGS_NAME, ft,
                        { Drawing::Colour::white });
                }
            }
            else
            {
                // Legacy path name
                auto footpathEntry = reinterpret_cast<const FootpathEntry*>(footpathObj->GetLegacyData());
                auto ft = Formatter();
                ft.Add<StringId>(footpathEntry->string_idx);
                drawText(rt, screenCoords, STR_TILE_INSPECTOR_PATH_NAME, ft, { Drawing::Colour::white });
            }

            // Path addition
            if (pathEl.HasAddition())
            {
                const auto pathAdditionEntry = pathEl.GetAdditionEntry();
                StringId additionNameId = pathAdditionEntry != nullptr ? pathAdditionEntry->name
                                                                       : static_cast<StringId>(STR_UNKNOWN_OBJECT_TYPE);
                auto ft = Formatter();
                ft.Add<StringId>(additionNameId);
                drawText(rt, screenCoords + ScreenCoordsXY{ 0, 2 * 11 }, STR_TILE_INSPECTOR_PATH_ADDITIONS, ft, { colours[1] });
            }
            else
            {
                drawText(
                    rt, screenCoords + ScreenCoordsXY{ 0, 2 * 11 }, STR_TILE_INSPECTOR_PATH_ADDITIONS_NONE, { colours[1] });
            }

            // Properties
            // Raise / lower label
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_GROUPBOX_DETAILS].left + 7, widgets[WIDX_PATH_SPINNER_HEIGHT].top + 1 };
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, { colours[1] });

            // Current base height
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PATH_SPINNER_HEIGHT].left + 3, widgets[WIDX_PATH_SPINNER_HEIGHT].textTop() };
            auto ft = Formatter();
            ft.Add<int32_t>(pathEl.BaseHeight);
            drawText(rt, screenCoords, STR_FORMAT_INTEGER, ft, { colours[1] });

            // Path connections
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_GROUPBOX_DETAILS].left + 7, widgets[WIDX_PATH_CHECK_EDGE_W].top };
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_PATH_CONNECTED_EDGES, { colours[1] });
        }

        void onDrawTrack(RenderTarget& rt, ScreenCoordsXY screenCoords, const TrackElement& trackEl)
        {
            RideId id = trackEl.GetRideIndex();
            auto rideTile = GetRide(id);

            // Ride ID
            auto ft = Formatter();
            ft.Add<int16_t>(id);
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_TRACK_RIDE_ID, ft, { colours[1] });

            // Ride name
            if (rideTile != nullptr)
            {
                ft = Formatter();
                rideTile->formatNameTo(ft);
                drawText(rt, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_TRACK_RIDE_NAME, ft, { colours[1] });
            }

            // Ride type. Individual pieces may be of a different ride type from the ride it belongs to.
            const auto& rtd = GetRideTypeDescriptor(trackEl.GetRideType());
            ft = Formatter();
            ft.Add<StringId>(rtd.Naming.Name);
            drawText(rt, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_TRACK_RIDE_TYPE, ft, { colours[1] });

            // Track
            ft = Formatter();
            ft.Add<uint16_t>(trackEl.GetTrackType());
            drawText(rt, screenCoords + ScreenCoordsXY{ 0, 33 }, STR_TILE_INSPECTOR_TRACK_PIECE_ID, ft, { colours[1] });

            ft = Formatter();
            ft.Add<uint16_t>(trackEl.GetSequenceIndex());
            drawText(rt, screenCoords + ScreenCoordsXY{ 0, 44 }, STR_TILE_INSPECTOR_TRACK_SEQUENCE, ft, { colours[1] });
            if (trackEl.IsStation())
            {
                auto stationIndex = trackEl.GetStationIndex();
                ft = Formatter();
                ft.Add<StringId>(STR_COMMA16);
                ft.Add<int16_t>(stationIndex.ToUnderlying());
                drawText(rt, screenCoords + ScreenCoordsXY{ 0, 55 }, STR_TILE_INSPECTOR_STATION_INDEX, ft, { colours[1] });
            }
            else
            {
                const char* stationNone = "-";
                ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<char*>(stationNone);
                drawText(rt, screenCoords + ScreenCoordsXY{ 0, 55 }, STR_TILE_INSPECTOR_STATION_INDEX, ft, { colours[1] });
            }

            ft = Formatter();
            ft.Add<StringId>(ColourSchemeNames[trackEl.GetColourScheme()]);
            drawText(rt, screenCoords + ScreenCoordsXY{ 0, 66 }, STR_TILE_INSPECTOR_COLOUR_SCHEME, ft, { colours[1] });

            // Properties
            // Raise / lower label
            screenCoords.y = windowPos.y + widgets[WIDX_TRACK_SPINNER_HEIGHT].top + 1;
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, { colours[1] });

            // Current base height
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_TRACK_SPINNER_HEIGHT].left + 3, widgets[WIDX_TRACK_SPINNER_HEIGHT].textTop() };
            ft = Formatter();
            ft.Add<int32_t>(trackEl.BaseHeight);
            drawText(rt, screenCoords, STR_FORMAT_INTEGER, ft, { colours[1] });
        }

        void onDrawSmallScenery(RenderTarget& rt, ScreenCoordsXY screenCoords, const SmallSceneryElement& smallSceneryEl)
        {
            // Details
            // Age
            auto ft = Formatter();
            ft.Add<int16_t>(smallSceneryEl.GetAge());
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_SCENERY_AGE, ft, { colours[1] });

            // Quadrant value
            const auto* sceneryEntry = smallSceneryEl.GetEntry();
            if (sceneryEntry != nullptr && !sceneryEntry->flags.has(SmallSceneryFlag::occupiesFullTile))
            {
                int16_t quadrant = smallSceneryEl.GetSceneryQuadrant();
                static constexpr StringId _quadrantStringIdx[] = {
                    STR_TILE_INSPECTOR_SCENERY_QUADRANT_SW,
                    STR_TILE_INSPECTOR_SCENERY_QUADRANT_NW,
                    STR_TILE_INSPECTOR_SCENERY_QUADRANT_NE,
                    STR_TILE_INSPECTOR_SCENERY_QUADRANT_SE,
                };
                ft = Formatter();
                ft.Add<StringId>(_quadrantStringIdx[quadrant]);
                drawText(rt, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_SCENERY_QUADRANT, ft, { colours[1] });
            }

            // Scenery ID
            ft = Formatter();
            ft.Add<ObjectEntryIndex>(smallSceneryEl.GetEntryIndex());
            drawText(rt, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_SCENERY_ENTRY_IDX, ft, { colours[1] });

            // Properties
            // Raise / Lower
            screenCoords.y = windowPos.y + widgets[WIDX_SCENERY_SPINNER_HEIGHT].top + 1;
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, { colours[1] });

            // Current base height
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_SCENERY_SPINNER_HEIGHT].left + 3,
                                  widgets[WIDX_SCENERY_SPINNER_HEIGHT].textTop() };
            ft = Formatter();
            ft.Add<int32_t>(smallSceneryEl.BaseHeight);
            drawText(rt, screenCoords, STR_FORMAT_INTEGER, ft, { colours[1] });

            // Quarter tile
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_GROUPBOX_DETAILS].left + 7, widgets[WIDX_SCENERY_CHECK_QUARTER_E].top };
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_SCENERY_QUADRANT_LABEL, { colours[1] });

            // Collision
            screenCoords.y = windowPos.y + widgets[WIDX_SCENERY_CHECK_COLLISION_E].top;
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_COLLISSION, { colours[1] });
        }

        void onDrawEntrance(RenderTarget& rt, ScreenCoordsXY screenCoords, const EntranceElement& entranceEl)
        {
            // Details
            // Entrance type
            auto ft = Formatter();
            ft.Add<StringId>(kEntranceTypeStringIds[entranceEl.GetEntranceType()]);
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_ENTRANCE_TYPE, ft, { colours[1] });

            if (entranceEl.GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE)
            {
                // TODO: Make this work with Left/Right park entrance parts
                ft = Formatter();
                ft.Add<uint16_t>(ParkEntranceGetIndex({ _toolMap, entranceEl.GetBaseZ() }));
                drawText(
                    rt, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_ENTRANCE_ENTRANCE_ID, ft, { colours[1] });
            }
            else
            {
                ft = Formatter();
                ft.Add<int16_t>(entranceEl.GetStationIndex().ToUnderlying());
                if (entranceEl.GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
                {
                    // Ride entrance ID
                    drawText(
                        rt, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_ENTRANCE_ENTRANCE_ID, ft,
                        { colours[1] });
                }
                else
                {
                    // Ride exit ID
                    drawText(
                        rt, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_ENTRANCE_EXIT_ID, ft, { colours[1] });
                }
            }

            if (entranceEl.GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE)
            {
                // Entrance part
                ft = Formatter();
                ft.Add<StringId>(kParkEntrancePartStringIds[entranceEl.GetSequenceIndex()]);
                drawText(rt, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_ENTRANCE_PART, ft, { colours[1] });
            }
            else
            {
                // Ride ID
                ft = Formatter();
                ft.Add<RideId>(entranceEl.GetRideIndex());
                drawText(rt, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_ENTRANCE_RIDE_ID, ft, { colours[1] });
                // Station index
                auto stationIndex = entranceEl.GetStationIndex();
                ft = Formatter();
                ft.Add<StringId>(STR_COMMA16);
                ft.Add<int16_t>(stationIndex.ToUnderlying());
                drawText(rt, screenCoords + ScreenCoordsXY{ 0, 33 }, STR_TILE_INSPECTOR_STATION_INDEX, ft, { colours[1] });
            }

            // Properties
            // Raise / Lower
            screenCoords.y = windowPos.y + widgets[WIDX_ENTRANCE_SPINNER_HEIGHT].top + 1;
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, { colours[1] });

            // Current base height
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_ENTRANCE_SPINNER_HEIGHT].left + 3,
                                  widgets[WIDX_ENTRANCE_SPINNER_HEIGHT].textTop() };
            ft = Formatter();
            ft.Add<int32_t>(entranceEl.BaseHeight);
            drawText(rt, screenCoords, STR_FORMAT_INTEGER, ft, { colours[1] });
        }

        void onDrawWall(RenderTarget& rt, ScreenCoordsXY screenCoords, const WallElement& wallEl)
        {
            // Details
            // Type
            auto ft = Formatter();
            ft.Add<ObjectEntryIndex>(wallEl.GetEntryIndex());
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_WALL_TYPE, ft, { colours[1] });

            // Banner info
            auto banner = wallEl.GetBanner();
            if (banner != nullptr)
            {
                ft = Formatter();
                banner->formatTextTo(ft);
                drawText(rt, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_ENTRY_BANNER_TEXT, ft, { colours[1] });
            }
            else
            {
                drawText(rt, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_ENTRY_BANNER_NONE, { colours[1] });
            }

            // Properties
            // Raise / lower label
            screenCoords.y = windowPos.y + widgets[WIDX_WALL_SPINNER_HEIGHT].top + 1;
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, { colours[1] });

            // Current base height
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_WALL_SPINNER_HEIGHT].left + 3, widgets[WIDX_WALL_SPINNER_HEIGHT].textTop() };
            ft = Formatter();
            ft.Add<int32_t>(wallEl.BaseHeight);
            drawText(rt, screenCoords, STR_FORMAT_INTEGER, ft, { colours[1] });

            // Slope label
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_GROUPBOX_DETAILS].left + 7, widgets[WIDX_WALL_DROPDOWN_SLOPE].top };
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_WALL_SLOPE, { colours[1] });

            // Animation frame label
            screenCoords.y = windowPos.y + widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME].top;
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_WALL_ANIMATION_FRAME, { colours[1] });

            // Current animation frame
            auto colour = colours[1];
            if (isWidgetDisabled(WIDX_WALL_SPINNER_ANIMATION_FRAME))
            {
                colour = colours[0].withFlag(ColourFlag::inset, true);
            }
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME].left + 3,
                                  widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME].textTop() };
            ft = Formatter();
            ft.Add<int32_t>(wallEl.GetAnimationFrame());
            drawText(rt, screenCoords, STR_FORMAT_INTEGER, ft, { colour });
        }

        void onDrawLargeScenery(RenderTarget& rt, ScreenCoordsXY screenCoords, const LargeSceneryElement& largeSceneryEl)
        {
            // Details
            // Type
            ObjectEntryIndex largeSceneryType = largeSceneryEl.GetEntryIndex();
            auto ft = Formatter();
            ft.Add<ObjectEntryIndex>(largeSceneryType);
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_LARGE_SCENERY_TYPE, ft, { colours[1] });

            // Part ID
            ft = Formatter();
            ft.Add<int16_t>(largeSceneryEl.GetSequenceIndex());
            drawText(rt, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_LARGE_SCENERY_PIECE_ID, ft, { colours[1] });

            // Banner info
            auto* largeSceneryEntry = OpenRCT2::ObjectEntryManager::GetObjectEntry<LargeSceneryEntry>(largeSceneryType);
            if (largeSceneryEntry != nullptr && largeSceneryEntry->scrolling_mode != kScrollingModeNone)
            {
                auto banner = largeSceneryEl.GetBanner();
                if (banner != nullptr)
                {
                    ft = Formatter();
                    banner->formatTextTo(ft);
                    drawText(
                        rt, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_ENTRY_BANNER_TEXT, ft, { colours[1] });
                }
            }
            else
            {
                drawText(rt, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_ENTRY_BANNER_NONE, { colours[1] });
            }

            // Properties
            // Raise / lower label
            screenCoords.y = windowPos.y + widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT].top + 1;
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, { colours[1] });

            // Current base height
            screenCoords.x = windowPos.x + widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT].left + 3;

            ft = Formatter();
            ft.Add<int32_t>(largeSceneryEl.BaseHeight);
            drawText(rt, screenCoords, STR_FORMAT_INTEGER, ft, { colours[1] });
        }

        void onDrawBanner(RenderTarget& rt, ScreenCoordsXY screenCoords, const BannerElement& bannerEl)
        {
            // Details
            // Banner info
            auto* banner = bannerEl.GetBanner();
            if (banner != nullptr)
            {
                Formatter ft;
                banner->formatTextTo(ft);
                drawText(rt, screenCoords, STR_TILE_INSPECTOR_ENTRY_BANNER_TEXT, ft, { colours[1] });
            }

            // Properties
            // Raise / lower label
            screenCoords.y = windowPos.y + widgets[WIDX_BANNER_SPINNER_HEIGHT].top + 1;
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, { colours[1] });

            // Current base height
            screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_BANNER_SPINNER_HEIGHT].left + 3, widgets[WIDX_BANNER_SPINNER_HEIGHT].textTop() };
            auto ft = Formatter();
            ft.Add<int32_t>(bannerEl.BaseHeight);
            drawText(rt, screenCoords, STR_FORMAT_INTEGER, ft, { colours[1] });

            // Blocked paths
            screenCoords.y += 28;
            screenCoords.x = windowPos.x + widgets[WIDX_GROUPBOX_DETAILS].left + 7;
            drawText(rt, screenCoords, STR_TILE_INSPECTOR_BANNER_BLOCKED_PATHS, { colours[1] });
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            const int32_t listWidth = widgets[WIDX_LIST].width() - 1;
            Rectangle::fill(
                rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } },
                getColourMap(colours[1].colour).midLight);

            // Show usage hint when nothing is selected
            if (!_tileSelected)
            {
                auto& listWidget = widgets[WIDX_LIST];
                auto centrePos = ScreenCoordsXY{ listWidget.width() / 2,
                                                 (listWidget.height() - 1 - FontGetLineHeight(FontStyle::medium)) / 2 };
                auto ft = Formatter{};
                auto textPaint = TextPaint{ colours[1], TextAlignment::centre };
                drawTextWrapped(rt, centrePos, listWidth, STR_TILE_INSPECTOR_SELECT_TILE_HINT, ft, textPaint);
                return;
            }

            ScreenCoordsXY screenCoords{};
            screenCoords.y = kScrollableRowHeight * (windowTileInspectorElementCount - 1);
            int32_t i = 0;
            char buffer[256];

            const TileElement* tileElement = MapGetFirstElementAt(_toolMap);

            do
            {
                if (tileElement == nullptr)
                    break;

                const bool selectedRow = i == windowTileInspectorSelectedIndex;
                const bool hoveredRow = i == _highlightedIndex;
                const char* typeName = "";

                // Draw row background colour
                auto fillRectangle = ScreenRect{ { 0, screenCoords.y },
                                                 { listWidth, screenCoords.y + kScrollableRowHeight - 1 } };
                if (selectedRow)
                    Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).midDark);
                else if (hoveredRow)
                    Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).midDark, true);
                // Zebra stripes
                else if (((windowTileInspectorElementCount - i) & 1) == 0)
                    Rectangle::fill(rt, fillRectangle, getColourMap(colours[1].colour).light, true);

                StringId stringFormat = STR_WINDOW_COLOUR_2_STRINGID;
                if (selectedRow || hoveredRow)
                    stringFormat = STR_WHITE_STRING;

                auto checkboxFormatter = Formatter();
                checkboxFormatter.Add<StringId>(STR_STRING);
                checkboxFormatter.Add<char*>(kCheckMarkString);

                // Draw checkbox and check if visible
                Rectangle::fillInset(
                    rt, { { 2, screenCoords.y }, { 15, screenCoords.y + 11 } }, colours[1], Rectangle::BorderStyle::inset,
                    Rectangle::FillBrightness::dark, Rectangle::FillMode::dontLightenWhenInset);
                if (!tileElement->IsInvisible())
                {
                    auto eyeFormatter = Formatter();
                    eyeFormatter.Add<StringId>(STR_STRING);
                    eyeFormatter.Add<char*>(kEyeString);
                    drawText(rt, screenCoords + ScreenCoordsXY{ 2, 1 }, stringFormat, eyeFormatter);
                }

                const auto type = tileElement->GetType();
                switch (type)
                {
                    case TileElementType::Surface:
                        typeName = LanguageGetString(STR_TILE_INSPECTOR_SURFACE);
                        break;

                    case TileElementType::Path:
                        typeName = tileElement->AsPath()->IsQueue() ? LanguageGetString(STR_QUEUE_LINE_MAP_TIP)
                                                                    : LanguageGetString(STR_FOOTPATH_MAP_TIP);
                        break;

                    case TileElementType::Track:
                        typeName = LanguageGetString(STR_RIDE_COMPONENT_TRACK_CAPITALISED);
                        break;

                    case TileElementType::SmallScenery:
                    {
                        const auto* sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
                        snprintf(
                            buffer, sizeof(buffer), "%s (%s)", LanguageGetString(STR_OBJECT_SELECTION_SMALL_SCENERY),
                            sceneryEntry != nullptr ? LanguageGetString(sceneryEntry->name) : "");
                        typeName = buffer;
                        break;
                    }

                    case TileElementType::Entrance:
                        typeName = LanguageGetString(STR_RIDE_CONSTRUCTION_ENTRANCE);
                        break;

                    case TileElementType::Wall:
                    {
                        const auto* entry = tileElement->AsWall()->GetEntry();
                        snprintf(
                            buffer, sizeof(buffer), "%s (%s)", LanguageGetString(STR_TILE_INSPECTOR_WALL),
                            entry != nullptr ? LanguageGetString(entry->name) : "");
                        typeName = buffer;
                        break;
                    }

                    case TileElementType::LargeScenery:
                        typeName = LanguageGetString(STR_OBJECT_SELECTION_LARGE_SCENERY);
                        break;

                    case TileElementType::Banner:
                        snprintf(
                            buffer, sizeof(buffer), "%s (%u)", LanguageGetString(STR_BANNER_WINDOW_TITLE),
                            tileElement->AsBanner()->GetIndex().ToUnderlying());
                        typeName = buffer;
                        break;

                    default:
                        snprintf(
                            buffer, sizeof(buffer), "%s (%d)", LanguageGetString(STR_UNKNOWN_OBJECT_TYPE), EnumValue(type));
                        typeName = buffer;
                }

                const int32_t clearanceHeight = tileElement->ClearanceHeight;
                const bool ghost = tileElement->IsGhost();
                const bool last = tileElement->IsLastForTile();

                // Element name
                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<char*>(typeName);
                drawTextEllipsised(
                    rt, screenCoords + ScreenCoordsXY{ kTypeColumnXY.x, 0 }, kTypeColumnSize.width, stringFormat, ft);

                // Base height
                ft = Formatter();
                ft.Add<StringId>(STR_FORMAT_INTEGER);
                ft.Add<int32_t>(tileElement->BaseHeight);
                drawText(rt, screenCoords + ScreenCoordsXY{ kBaseHeightColumnXY.x, 0 }, stringFormat, ft);

                // Clearance height
                ft = Formatter();
                ft.Add<StringId>(STR_FORMAT_INTEGER);
                ft.Add<int32_t>(clearanceHeight);
                drawText(rt, screenCoords + ScreenCoordsXY{ kClearanceHeightColumnXY.x, 0 }, stringFormat, ft);

                // Direction
                ft = Formatter();
                ft.Add<StringId>(STR_FORMAT_INTEGER);
                ft.Add<int32_t>(tileElement->GetDirection());
                drawText(rt, screenCoords + ScreenCoordsXY{ kDirectionColumnXY.x, 0 }, stringFormat, ft);

                // Checkmarks for ghost and last for tile
                if (ghost)
                    drawText(rt, screenCoords + ScreenCoordsXY{ kGhostFlagColumnXY.x, 0 }, stringFormat, checkboxFormatter);
                if (last)
                    drawText(rt, screenCoords + ScreenCoordsXY{ kLastFlagColumnXY.x, 0 }, stringFormat, checkboxFormatter);

                screenCoords.y -= kScrollableRowHeight;
                i++;
            } while (!(tileElement++)->IsLastForTile());
        }

        void ClearClipboard()
        {
            _elementCopied = false;
        }

        void ToggleInvisibility()
        {
            if (windowTileInspectorSelectedIndex >= 0 && windowTileInspectorSelectedIndex < windowTileInspectorElementCount)
            {
                ToggleInvisibility(windowTileInspectorSelectedIndex);
            }
        }

    private:
        void setPage(const TileInspectorPage p)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (tileInspectorPage == p && !widgets.empty())
                return;

            invalidate();
            // subtract current page height, then add new page height
            if (tileInspectorPage != TileInspectorPage::Default)
            {
                auto index = EnumValue(tileInspectorPage) - 1;
                height -= kPageGroupBoxSettings[index].details_top_offset - kGroupboxPadding - 3;
                minHeight -= kPageGroupBoxSettings[index].details_top_offset - kGroupboxPadding - 3;
            }
            if (p != TileInspectorPage::Default)
            {
                auto index = EnumValue(p) - 1;
                height += kPageGroupBoxSettings[index].details_top_offset - kGroupboxPadding - 3;
                minHeight += kPageGroupBoxSettings[index].details_top_offset - kGroupboxPadding - 3;
            }

            tileInspectorPage = p;
            auto pageIndex = EnumValue(p);
            setWidgets(kWidgetsByPage[pageIndex]);
            holdDownWidgets = kHoldableWidgetsByPage[pageIndex];
            disabledWidgets = kDisabledWidgetsByPage[pageIndex];
            pressedWidgets = 0;
            invalidate();
        }

        void UpdateSelectedTile(const ScreenCoordsXY& screenCoords)
        {
            const bool ctrlIsHeldDown = GetInputManager().isModifierKeyPressed(ModifierKey::ctrl);
            // Mouse hasn't moved
            if (screenCoords.x == _toolMouseX && screenCoords.y == _toolMouseY && _toolCtrlDown == ctrlIsHeldDown)
                return;

            _toolMouseX = screenCoords.x;
            _toolMouseY = screenCoords.y;
            _toolCtrlDown = ctrlIsHeldDown;

            CoordsXY mapCoords{};
            TileElement* clickedElement = nullptr;
            if (ctrlIsHeldDown)
            {
                auto info = GetMapCoordinatesFromPos(screenCoords, ViewportInteractionFlags);
                clickedElement = info.Element;
                mapCoords = info.Loc;
            }

            // Even if Ctrl was pressed, fall back to normal selection when there was nothing under the cursor
            if (clickedElement == nullptr)
            {
                auto mouseCoords = ScreenPosToMapPos(screenCoords, nullptr);
                if (!mouseCoords.has_value())
                    return;

                mapCoords = mouseCoords.value();
                // Tile is already selected
                if (_tileSelected && mapCoords.x == _toolMap.x && mapCoords.y == _toolMap.y)
                    return;
            }

            // Invalidate the previous selection
            if (auto* elem = OpenRCT2::TileInspector::GetSelectedElement(); elem != nullptr)
            {
                MapInvalidateElement(windowTileInspectorTile.ToCoordsXY(), elem);
            }

            _tileSelected = true;
            _toolMap = mapCoords;
            windowTileInspectorTile = TileCoordsXY(mapCoords);
            LoadTile(clickedElement);
        }

        void SelectElementFromList(int32_t index)
        {
            if (index < 0 || index >= windowTileInspectorElementCount)
            {
                windowTileInspectorSelectedIndex = -1;
            }
            else
            {
                windowTileInspectorSelectedIndex = index;
            }
            invalidate();
            MapInvalidateTileFull(_toolMap);
        }

        void LoadTile(TileElement* elementToSelect)
        {
            windowTileInspectorSelectedIndex = -1;
            scrolls[0].contentOffsetY = 0;

            TileElement* element = MapGetFirstElementAt(_toolMap);
            int16_t numItems = 0;
            do
            {
                if (element == nullptr)
                    break;
                if (element == elementToSelect)
                    windowTileInspectorSelectedIndex = numItems;

                numItems++;
            } while (!(element++)->IsLastForTile());
            windowTileInspectorElementCount = numItems;
            invalidate();
        }

        void RemoveElement(int32_t elementIndex)
        {
            Guard::Assert(elementIndex >= 0 && elementIndex < windowTileInspectorElementCount, "elementIndex out of range");
            auto modifyTile = GameActions::TileModifyAction(_toolMap, GameActions::TileModifyType::AnyRemove, elementIndex);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void RotateElement(int32_t elementIndex)
        {
            Guard::Assert(elementIndex >= 0 && elementIndex < windowTileInspectorElementCount, "elementIndex out of range");
            auto modifyTile = GameActions::TileModifyAction(_toolMap, GameActions::TileModifyType::AnyRotate, elementIndex);
            GameActions::Execute(&modifyTile, getGameState());
        }

        // Swap element with its parent
        void SwapElements(int16_t first, int16_t second)
        {
            bool firstInRange = first >= 0 && first < windowTileInspectorElementCount;
            bool secondInRange = second >= 0 && second < windowTileInspectorElementCount;
            // This might happen if two people are modifying the same tile.
            if (!firstInRange || !secondInRange)
                return;
            auto modifyTile = GameActions::TileModifyAction(_toolMap, GameActions::TileModifyType::AnySwap, first, second);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void SortElements()
        {
            Guard::Assert(_tileSelected, "No tile selected");
            auto modifyTile = GameActions::TileModifyAction(_toolMap, GameActions::TileModifyType::AnySort);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void CopyElement()
        {
            const TileElement* const tileElement = OpenRCT2::TileInspector::GetSelectedElement();
            Guard::Assert(tileElement != nullptr, "Invalid tile element");
            // Copy value, in case the element gets moved
            _copiedElement = *tileElement;
            _copiedBanner = {};
            auto bannerIndex = _copiedElement.GetBannerIndex();
            if (bannerIndex != BannerIndex::GetNull())
            {
                auto banner = GetBanner(bannerIndex);
                if (banner != nullptr)
                    _copiedBanner = *banner;
            }
            _elementCopied = true;
            invalidate();
        }

        void PasteElement()
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::AnyPaste, 0, 0, _copiedElement, _copiedBanner);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void BaseHeightOffset(int16_t elementIndex, int8_t heightOffset)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::AnyBaseHeightOffset, elementIndex, heightOffset);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void SurfaceShowParkFences(bool showFences)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::SurfaceShowParkFences, showFences);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void SurfaceToggleCorner(int32_t cornerIndex)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::SurfaceToggleCorner, cornerIndex);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void SurfaceToggleDiagonal()
        {
            auto modifyTile = GameActions::TileModifyAction(_toolMap, GameActions::TileModifyType::SurfaceToggleDiagonal);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void PathSetSloped(int32_t elementIndex, bool sloped)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::PathSetSlope, elementIndex, sloped);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void PathSetJunctionRailings(int32_t elementIndex, bool hasJunctionRailings)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::PathSetJunctionRailings, elementIndex, hasJunctionRailings);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void PathSetBroken(int32_t elementIndex, bool broken)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::PathSetBroken, elementIndex, broken);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void PathToggleEdge(int32_t elementIndex, int32_t cornerIndex)
        {
            Guard::Assert(elementIndex >= 0 && elementIndex < windowTileInspectorElementCount, "elementIndex out of range");
            Guard::Assert(cornerIndex >= 0 && cornerIndex < 8, "cornerIndex out of range");
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::PathToggleEdge, elementIndex, cornerIndex);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void EntranceMakeUsable(int32_t elementIndex)
        {
            Guard::ArgumentInRange(elementIndex, 0, windowTileInspectorElementCount - 1);
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::EntranceMakeUsable, elementIndex);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void WallSetSlope(int32_t elementIndex, int32_t slopeValue)
        {
            // Make sure only the correct bits are set
            Guard::Assert((slopeValue & 3) == slopeValue, "slopeValue doesn't match its mask");
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::WallSetSlope, elementIndex, slopeValue);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void WallAnimationFrameOffset(int16_t elementIndex, int8_t animationFrameOffset)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::WallSetAnimationFrame, elementIndex, animationFrameOffset);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void TrackBlockHeightOffset(int32_t elementIndex, int8_t heightOffset)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::TrackBaseHeightOffset, elementIndex, heightOffset);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void TrackBlockSetLift(int32_t elementIndex, bool entireTrackBlock, bool chain)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap,
                entireTrackBlock ? GameActions::TileModifyType::TrackSetChainBlock : GameActions::TileModifyType::TrackSetChain,
                elementIndex, chain);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void TrackSetBrakeClosed(int32_t elementIndex, bool isClosed)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::TrackSetBrake, elementIndex, isClosed);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void TrackSetIndestructible(int32_t elementIndex, bool isIndestructible)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::TrackSetIndestructible, elementIndex, isIndestructible);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void QuarterTileSet(int32_t elementIndex, const int32_t quarterIndex)
        {
            // quarterIndex is widget index relative to WIDX_SCENERY_CHECK_QUARTER_N, so a value from 0-3
            Guard::Assert(quarterIndex >= 0 && quarterIndex < 4, "quarterIndex out of range");
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::ScenerySetQuarterLocation, elementIndex,
                (quarterIndex - GetCurrentRotation()) & 3);
            GameActions::Execute(&modifyTile, getGameState());
        }

        // ToggleQuadrantCollision?
        void ToggleQuadrantCollosion(int32_t elementIndex, const int32_t quadrantIndex)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::ScenerySetQuarterCollision, elementIndex,
                (quadrantIndex + 2 - GetCurrentRotation()) & 3);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void BannerToggleBlock(int32_t elementIndex, int32_t edgeIndex)
        {
            Guard::Assert(edgeIndex >= 0 && edgeIndex < 4, "edgeIndex out of range");
            // Make edgeIndex  = 0
            edgeIndex = (edgeIndex - GetCurrentRotation()) & 3;
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::BannerToggleBlockingEdge, elementIndex, edgeIndex);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void ToggleInvisibility(int32_t elementIndex)
        {
            Guard::Assert(elementIndex >= 0 && elementIndex < windowTileInspectorElementCount, "elementIndex out of range");
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::AnyToggleInvisilibity, elementIndex);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void WallSetAnimationIsBackwards(int32_t elementIndex, bool backwards)
        {
            auto modifyTile = GameActions::TileModifyAction(
                _toolMap, GameActions::TileModifyType::WallSetAnimationIsBackwards, elementIndex, backwards);
            GameActions::Execute(&modifyTile, getGameState());
        }

        void onPrepareDraw() override
        {
            const TileElement* const tileElement = OpenRCT2::TileInspector::GetSelectedElement();

            // Set the correct page automatically
            TileInspectorPage p = TileInspectorPage::Default;
            if (tileElement != nullptr)
            {
                switch (tileElement->GetType())
                {
                    case TileElementType::Surface:
                        p = TileInspectorPage::Surface;
                        break;

                    case TileElementType::Path:
                        p = TileInspectorPage::Path;
                        break;

                    case TileElementType::Track:
                        p = TileInspectorPage::Track;
                        break;

                    case TileElementType::SmallScenery:
                        p = TileInspectorPage::Scenery;
                        break;

                    case TileElementType::Entrance:
                        p = TileInspectorPage::Entrance;
                        break;

                    case TileElementType::Wall:
                        p = TileInspectorPage::Wall;
                        break;

                    case TileElementType::LargeScenery:
                        p = TileInspectorPage::LargeScenery;
                        break;

                    case TileElementType::Banner:
                        p = TileInspectorPage::Banner;
                        break;
                }
            }

            if (tileInspectorPage != p)
            {
                setPage(p);
                invalidate();
            }
            // X and Y spinners
            setWidgetDisabledAndInvalidate(
                WIDX_SPINNER_X_INCREASE, !(_tileSelected && ((_toolMap.x / 32) < kMaximumMapSizeTechnical - 1)));
            setWidgetDisabledAndInvalidate(WIDX_SPINNER_X_DECREASE, !(_tileSelected && ((_toolMap.x / 32) > 0)));
            setWidgetDisabledAndInvalidate(
                WIDX_SPINNER_Y_INCREASE, !(_tileSelected && ((_toolMap.y / 32) < kMaximumMapSizeTechnical - 1)));
            setWidgetDisabledAndInvalidate(WIDX_SPINNER_Y_DECREASE, !(_tileSelected && ((_toolMap.y / 32) > 0)));

            // Sort buttons
            setWidgetDisabledAndInvalidate(WIDX_BUTTON_SORT, !(_tileSelected && windowTileInspectorElementCount > 1));

            // Move Up button
            setWidgetDisabledAndInvalidate(
                WIDX_BUTTON_MOVE_UP,
                !(windowTileInspectorSelectedIndex != -1
                  && windowTileInspectorSelectedIndex < windowTileInspectorElementCount - 1));

            // Move Down button
            setWidgetDisabledAndInvalidate(WIDX_BUTTON_MOVE_DOWN, windowTileInspectorSelectedIndex <= 0);

            // Copy button
            setWidgetDisabledAndInvalidate(WIDX_BUTTON_COPY, windowTileInspectorSelectedIndex < 0);

            // Paste button
            setWidgetDisabledAndInvalidate(WIDX_BUTTON_PASTE, !(_tileSelected && _elementCopied));

            widgets[WIDX_BACKGROUND].bottom = height - 1;

            if (tileInspectorPage == TileInspectorPage::Default)
            {
                widgets[WIDX_GROUPBOX_DETAILS].type = WidgetType::empty;
                widgets[WIDX_GROUPBOX_PROPERTIES].type = WidgetType::empty;
                widgets[WIDX_LIST].bottom = height - kBottomPadding;
            }
            else
            {
                widgets[WIDX_GROUPBOX_DETAILS].type = WidgetType::groupbox;
                widgets[WIDX_GROUPBOX_PROPERTIES].type = WidgetType::groupbox;

                auto pageIndex = EnumValue(tileInspectorPage) - 1;
                auto& settings = kPageGroupBoxSettings[pageIndex];
                widgets[WIDX_GROUPBOX_DETAILS].text = settings.string_id;
                widgets[WIDX_GROUPBOX_DETAILS].top = height - settings.details_top_offset;
                widgets[WIDX_GROUPBOX_DETAILS].bottom = height - settings.details_bottom_offset;
                widgets[WIDX_GROUPBOX_PROPERTIES].top = height - settings.properties_top_offset;
                widgets[WIDX_GROUPBOX_PROPERTIES].bottom = height - settings.properties_bottom_offset;
                widgets[WIDX_LIST].bottom = widgets[WIDX_GROUPBOX_DETAILS].top - kGroupboxPadding;
            }

            // The default page doesn't need further invalidation
            if (tileInspectorPage == TileInspectorPage::Default)
                return;

            // Using a switch, because I don't think giving each page their own callbacks is
            // needed here, as only the mouseup and invalidate functions are different.
            const auto propertiesAnchor = ScreenCoordsXY{ widgets[WIDX_GROUPBOX_PROPERTIES].left + 6,
                                                          widgets[WIDX_GROUPBOX_PROPERTIES].top + 17 };

            switch (tileElement->GetType())
            {
                case TileElementType::Surface:
                    widgets[WIDX_SURFACE_SPINNER_HEIGHT].moveTo(PropertyRowCol(propertiesAnchor, 0, 1));
                    widgets[WIDX_SURFACE_SPINNER_HEIGHT_INCREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertySpinnerSize.width - 13, 1 });
                    widgets[WIDX_SURFACE_SPINNER_HEIGHT_DECREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertySpinnerSize.width - 26, 1 });
                    widgets[WIDX_SURFACE_BUTTON_REMOVE_FENCES].moveTo(PropertyRowCol(propertiesAnchor, 1, 0));
                    widgets[WIDX_SURFACE_BUTTON_RESTORE_FENCES].moveTo(PropertyRowCol(propertiesAnchor, 1, 1));
                    widgets[WIDX_SURFACE_CHECK_CORNER_N].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 2, 1), 1, 0));
                    widgets[WIDX_SURFACE_CHECK_CORNER_E].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 2, 1), 2, 1));
                    widgets[WIDX_SURFACE_CHECK_CORNER_S].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 2, 1), 1, 2));
                    widgets[WIDX_SURFACE_CHECK_CORNER_W].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 2, 1), 0, 1));
                    widgets[WIDX_SURFACE_CHECK_DIAGONAL].moveTo(PropertyRowCol(propertiesAnchor, 3, 0));

                    setCheckboxValue(
                        WIDX_SURFACE_CHECK_CORNER_N,
                        tileElement->AsSurface()->GetSlope() & (1 << ((2 - GetCurrentRotation()) & 3)));
                    setCheckboxValue(
                        WIDX_SURFACE_CHECK_CORNER_E,
                        tileElement->AsSurface()->GetSlope() & (1 << ((3 - GetCurrentRotation()) & 3)));
                    setCheckboxValue(
                        WIDX_SURFACE_CHECK_CORNER_S,
                        tileElement->AsSurface()->GetSlope() & (1 << ((0 - GetCurrentRotation()) & 3)));
                    setCheckboxValue(
                        WIDX_SURFACE_CHECK_CORNER_W,
                        tileElement->AsSurface()->GetSlope() & (1 << ((1 - GetCurrentRotation()) & 3)));
                    setCheckboxValue(
                        WIDX_SURFACE_CHECK_DIAGONAL, tileElement->AsSurface()->GetSlope() & kTileSlopeDiagonalFlag);
                    break;

                case TileElementType::Path:
                    widgets[WIDX_PATH_SPINNER_HEIGHT].moveTo(PropertyRowCol(propertiesAnchor, 0, 1));
                    widgets[WIDX_PATH_SPINNER_HEIGHT_INCREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertySpinnerSize.width - 13, 1 });
                    widgets[WIDX_PATH_SPINNER_HEIGHT_DECREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertySpinnerSize.width - 26, 1 });
                    widgets[WIDX_PATH_CHECK_BROKEN].moveTo(PropertyRowCol(propertiesAnchor, 1, 0));
                    widgets[WIDX_PATH_CHECK_SLOPED].moveTo(PropertyRowCol(propertiesAnchor, 2, 0));
                    widgets[WIDX_PATH_CHECK_JUNCTION_RAILINGS].moveTo(PropertyRowCol(propertiesAnchor, 3, 0));
                    widgets[WIDX_PATH_CHECK_EDGE_NE].moveTo(CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 4, 1), 3, 1));
                    widgets[WIDX_PATH_CHECK_EDGE_E].moveTo(CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 4, 1), 4, 2));
                    widgets[WIDX_PATH_CHECK_EDGE_SE].moveTo(CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 4, 1), 3, 3));
                    widgets[WIDX_PATH_CHECK_EDGE_S].moveTo(CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 4, 1), 2, 4));
                    widgets[WIDX_PATH_CHECK_EDGE_SW].moveTo(CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 4, 1), 1, 3));
                    widgets[WIDX_PATH_CHECK_EDGE_W].moveTo(CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 4, 1), 0, 2));
                    widgets[WIDX_PATH_CHECK_EDGE_NW].moveTo(CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 4, 1), 1, 1));
                    widgets[WIDX_PATH_CHECK_EDGE_N].moveTo(CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 4, 1), 2, 0));

                    setCheckboxValue(WIDX_PATH_CHECK_SLOPED, tileElement->AsPath()->IsSloped());
                    setCheckboxValue(WIDX_PATH_CHECK_JUNCTION_RAILINGS, tileElement->AsPath()->HasJunctionRailings());
                    setCheckboxValue(WIDX_PATH_CHECK_BROKEN, tileElement->AsPath()->IsBroken());
                    setCheckboxValue(
                        WIDX_PATH_CHECK_EDGE_NE, tileElement->AsPath()->GetEdges() & (1 << ((0 - GetCurrentRotation()) & 3)));
                    setCheckboxValue(
                        WIDX_PATH_CHECK_EDGE_SE, tileElement->AsPath()->GetEdges() & (1 << ((1 - GetCurrentRotation()) & 3)));
                    setCheckboxValue(
                        WIDX_PATH_CHECK_EDGE_SW, tileElement->AsPath()->GetEdges() & (1 << ((2 - GetCurrentRotation()) & 3)));
                    setCheckboxValue(
                        WIDX_PATH_CHECK_EDGE_NW, tileElement->AsPath()->GetEdges() & (1 << ((3 - GetCurrentRotation()) & 3)));
                    setCheckboxValue(
                        WIDX_PATH_CHECK_EDGE_E, tileElement->AsPath()->GetCorners() & (1 << ((0 - GetCurrentRotation()) & 3)));
                    setCheckboxValue(
                        WIDX_PATH_CHECK_EDGE_S, tileElement->AsPath()->GetCorners() & (1 << ((1 - GetCurrentRotation()) & 3)));
                    setCheckboxValue(
                        WIDX_PATH_CHECK_EDGE_W, tileElement->AsPath()->GetCorners() & (1 << ((2 - GetCurrentRotation()) & 3)));
                    setCheckboxValue(
                        WIDX_PATH_CHECK_EDGE_N, tileElement->AsPath()->GetCorners() & (1 << ((3 - GetCurrentRotation()) & 3)));
                    break;

                case TileElementType::Track:
                    widgets[WIDX_TRACK_CHECK_APPLY_TO_ALL].moveTo(PropertyRowCol(propertiesAnchor, 0, 0));
                    widgets[WIDX_TRACK_SPINNER_HEIGHT].moveTo(PropertyRowCol(propertiesAnchor, 1, 1));
                    widgets[WIDX_TRACK_SPINNER_HEIGHT_INCREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 1, 1) + ScreenCoordsXY{ kPropertySpinnerSize.width - 13, 1 });
                    widgets[WIDX_TRACK_SPINNER_HEIGHT_DECREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 1, 1) + ScreenCoordsXY{ kPropertySpinnerSize.width - 26, 1 });
                    widgets[WIDX_TRACK_CHECK_CHAIN_LIFT].moveTo(PropertyRowCol(propertiesAnchor, 2, 0));
                    widgets[WIDX_TRACK_CHECK_BRAKE_CLOSED].moveTo(PropertyRowCol(propertiesAnchor, 3, 0));
                    widgets[WIDX_TRACK_CHECK_IS_INDESTRUCTIBLE].moveTo(PropertyRowCol(propertiesAnchor, 4, 0));

                    setCheckboxValue(WIDX_TRACK_CHECK_APPLY_TO_ALL, _applyToAll);
                    setCheckboxValue(WIDX_TRACK_CHECK_CHAIN_LIFT, tileElement->AsTrack()->HasChain());
                    setCheckboxValue(WIDX_TRACK_CHECK_BRAKE_CLOSED, tileElement->AsTrack()->IsBrakeClosed());
                    widgets[WIDX_TRACK_CHECK_BRAKE_CLOSED].content = tileElement->AsTrack()->IsBlockStart()
                        ? STR_TILE_INSPECTOR_TRACK_BLOCK_BRAKE
                        : STR_TILE_INSPECTOR_TRACK_BRAKE_CLOSED;
                    setCheckboxValue(WIDX_TRACK_CHECK_IS_INDESTRUCTIBLE, tileElement->AsTrack()->IsIndestructible());
                    break;

                case TileElementType::SmallScenery:
                {
                    // Raise / Lower
                    widgets[WIDX_SCENERY_SPINNER_HEIGHT].moveTo(PropertyRowCol(propertiesAnchor, 0, 1));
                    widgets[WIDX_SCENERY_SPINNER_HEIGHT_INCREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertySpinnerSize.width - 13, 1 });
                    widgets[WIDX_SCENERY_SPINNER_HEIGHT_DECREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertySpinnerSize.width - 26, 1 });

                    // Quadrant checkboxes
                    widgets[WIDX_SCENERY_CHECK_QUARTER_N].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 1, 1), 1, 0 + 0));
                    widgets[WIDX_SCENERY_CHECK_QUARTER_E].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 1, 1), 2, 0 + 1));
                    widgets[WIDX_SCENERY_CHECK_QUARTER_S].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 1, 1), 1, 0 + 2));
                    widgets[WIDX_SCENERY_CHECK_QUARTER_W].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 1, 1), 0, 0 + 1));

                    // This gets the relative rotation, by subtracting the camera's rotation, and wrapping it between 0-3
                    // inclusive
                    bool N = tileElement->AsSmallScenery()->GetSceneryQuadrant() == ((0 - GetCurrentRotation()) & 3);
                    bool E = tileElement->AsSmallScenery()->GetSceneryQuadrant() == ((1 - GetCurrentRotation()) & 3);
                    bool S = tileElement->AsSmallScenery()->GetSceneryQuadrant() == ((2 - GetCurrentRotation()) & 3);
                    bool W = tileElement->AsSmallScenery()->GetSceneryQuadrant() == ((3 - GetCurrentRotation()) & 3);
                    setCheckboxValue(WIDX_SCENERY_CHECK_QUARTER_N, N);
                    setCheckboxValue(WIDX_SCENERY_CHECK_QUARTER_E, E);
                    setCheckboxValue(WIDX_SCENERY_CHECK_QUARTER_S, S);
                    setCheckboxValue(WIDX_SCENERY_CHECK_QUARTER_W, W);

                    // Collision checkboxes
                    widgets[WIDX_SCENERY_CHECK_COLLISION_N].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 2, 1), 1, 1 + 0));
                    widgets[WIDX_SCENERY_CHECK_COLLISION_E].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 2, 1), 2, 1 + 1));
                    widgets[WIDX_SCENERY_CHECK_COLLISION_S].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 2, 1), 1, 1 + 2));
                    widgets[WIDX_SCENERY_CHECK_COLLISION_W].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 2, 1), 0, 1 + 1));

                    auto occupiedQuadrants = tileElement->GetOccupiedQuadrants();
                    N = (occupiedQuadrants & (1 << ((2 - GetCurrentRotation()) & 3))) != 0;
                    E = (occupiedQuadrants & (1 << ((3 - GetCurrentRotation()) & 3))) != 0;
                    S = (occupiedQuadrants & (1 << ((0 - GetCurrentRotation()) & 3))) != 0;
                    W = (occupiedQuadrants & (1 << ((1 - GetCurrentRotation()) & 3))) != 0;
                    setCheckboxValue(WIDX_SCENERY_CHECK_COLLISION_N, N);
                    setCheckboxValue(WIDX_SCENERY_CHECK_COLLISION_E, E);
                    setCheckboxValue(WIDX_SCENERY_CHECK_COLLISION_S, S);
                    setCheckboxValue(WIDX_SCENERY_CHECK_COLLISION_W, W);
                    break;
                }

                case TileElementType::Entrance:
                    widgets[WIDX_ENTRANCE_SPINNER_HEIGHT].moveTo(PropertyRowCol(propertiesAnchor, 0, 1));
                    widgets[WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertyButtonSize.width - 13, 1 });
                    widgets[WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertyButtonSize.width - 26, 1 });
                    widgets[WIDX_ENTRANCE_BUTTON_MAKE_USABLE].moveTo(PropertyRowCol(propertiesAnchor, 1, 0));

                    setWidgetDisabled(
                        WIDX_ENTRANCE_BUTTON_MAKE_USABLE,
                        tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE);
                    break;

                case TileElementType::Wall:
                {
                    bool canBeSloped = false;
                    bool hasAnimation = false;
                    const auto wallEntry = tileElement->AsWall()->GetEntry();
                    if (wallEntry != nullptr)
                    {
                        canBeSloped = !(wallEntry->flags & WALL_SCENERY_CANT_BUILD_ON_SLOPE);
                        hasAnimation = wallEntry->flags & WALL_SCENERY_IS_DOOR;
                    }

                    widgets[WIDX_WALL_SPINNER_HEIGHT].moveTo(PropertyRowCol(propertiesAnchor, 0, 1));
                    widgets[WIDX_WALL_SPINNER_HEIGHT_INCREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertyButtonSize.width - 13, 1 });
                    widgets[WIDX_WALL_SPINNER_HEIGHT_DECREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertyButtonSize.width - 26, 1 });
                    widgets[WIDX_WALL_DROPDOWN_SLOPE].moveTo(PropertyRowCol(propertiesAnchor, 1, 1));
                    widgets[WIDX_WALL_DROPDOWN_SLOPE_BUTTON].moveTo(
                        PropertyRowCol(propertiesAnchor + ScreenCoordsXY{ kPropertyButtonSize.width - 12, 0 }, 1, 1));
                    widgets[WIDX_WALL_DROPDOWN_SLOPE].text = kWallSlopeStringIds[tileElement->AsWall()->GetSlope()];
                    widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME].moveTo(PropertyRowCol(propertiesAnchor, 2, 1));
                    widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME_INCREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 2, 1) + ScreenCoordsXY{ kPropertyButtonSize.width - 13, 1 });
                    widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME_DECREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 2, 1) + ScreenCoordsXY{ kPropertyButtonSize.width - 26, 1 });
                    widgets[WIDX_WALL_ANIMATION_IS_BACKWARDS].moveTo(PropertyRowCol(propertiesAnchor, 3, 0));

                    // Wall slope dropdown
                    setWidgetDisabled(WIDX_WALL_DROPDOWN_SLOPE, !canBeSloped);
                    invalidateWidget(WIDX_WALL_DROPDOWN_SLOPE);
                    setWidgetDisabled(WIDX_WALL_DROPDOWN_SLOPE_BUTTON, !canBeSloped);
                    invalidateWidget(WIDX_WALL_DROPDOWN_SLOPE_BUTTON);
                    // Wall animation frame spinner
                    setWidgetDisabled(WIDX_WALL_SPINNER_ANIMATION_FRAME, !hasAnimation);
                    setWidgetDisabled(WIDX_WALL_SPINNER_ANIMATION_FRAME_INCREASE, !hasAnimation);
                    setWidgetDisabled(WIDX_WALL_SPINNER_ANIMATION_FRAME_DECREASE, !hasAnimation);

                    setCheckboxValue(WIDX_WALL_ANIMATION_IS_BACKWARDS, tileElement->AsWall()->AnimationIsBackwards());
                    setWidgetDisabled(WIDX_WALL_ANIMATION_IS_BACKWARDS, !hasAnimation);
                    break;
                }

                case TileElementType::LargeScenery:
                    widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT].moveTo(PropertyRowCol(propertiesAnchor, 0, 1));
                    widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertyButtonSize.width - 13, 1 });
                    widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertyButtonSize.width - 26, 1 });
                    break;

                case TileElementType::Banner:
                    widgets[WIDX_BANNER_SPINNER_HEIGHT].moveTo(PropertyRowCol(propertiesAnchor, 0, 1));
                    widgets[WIDX_BANNER_SPINNER_HEIGHT_INCREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertyButtonSize.width - 13, 1 });
                    widgets[WIDX_BANNER_SPINNER_HEIGHT_DECREASE].moveTo(
                        PropertyRowCol(propertiesAnchor, 0, 1) + ScreenCoordsXY{ kPropertyButtonSize.width - 26, 1 });
                    widgets[WIDX_BANNER_CHECK_BLOCK_NE].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 1, 1), 3, 1));
                    widgets[WIDX_BANNER_CHECK_BLOCK_SE].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 1, 1), 3, 3));
                    widgets[WIDX_BANNER_CHECK_BLOCK_SW].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 1, 1), 1, 3));
                    widgets[WIDX_BANNER_CHECK_BLOCK_NW].moveTo(
                        CheckboxGroupOffset(PropertyRowCol(propertiesAnchor, 1, 1), 1, 1));

                    setCheckboxValue(
                        WIDX_BANNER_CHECK_BLOCK_NE,
                        (tileElement->AsBanner()->GetAllowedEdges() & (1 << ((0 - GetCurrentRotation()) & 3))));
                    setCheckboxValue(
                        WIDX_BANNER_CHECK_BLOCK_SE,
                        (tileElement->AsBanner()->GetAllowedEdges() & (1 << ((1 - GetCurrentRotation()) & 3))));
                    setCheckboxValue(
                        WIDX_BANNER_CHECK_BLOCK_SW,
                        (tileElement->AsBanner()->GetAllowedEdges() & (1 << ((2 - GetCurrentRotation()) & 3))));
                    setCheckboxValue(
                        WIDX_BANNER_CHECK_BLOCK_NW,
                        (tileElement->AsBanner()->GetAllowedEdges() & (1 << ((3 - GetCurrentRotation()) & 3))));
                    break;

                default:
                    break; // Nothing.
            }
        }
    };

    WindowBase* TileInspectorOpen()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* window = windowMgr->BringToFrontByClass(WindowClass::tileInspector);
        if (window == nullptr)
            window = windowMgr->Create<TileInspector>(
                WindowClass::tileInspector, ScreenCoordsXY(0, 29), kWindowSize, WindowFlag::resizable);
        return window;
    }

    void WindowTileInspectorClearClipboard()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->FindByClass(WindowClass::tileInspector);
        if (window != nullptr)
            static_cast<TileInspector*>(window)->ClearClipboard();
    }

    void WindowTileInspectorKeyboardShortcutToggleInvisibility()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->FindByClass(WindowClass::tileInspector);
        if (window != nullptr)
            static_cast<TileInspector*>(window)->ToggleInvisibility();
    }
} // namespace OpenRCT2::Ui::Windows
