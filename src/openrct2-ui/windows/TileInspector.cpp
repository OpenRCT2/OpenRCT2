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
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/actions/TileModifyAction.h>
#include <openrct2/common.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/tile_inspector.h>
#include <openrct2/world/Banner.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/LargeScenery.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/SmallScenery.h>
#include <openrct2/world/Surface.h>
#include <openrct2/world/TileInspector.h>

static constexpr const rct_string_id EntranceTypeStringIds[] = {
    STR_TILE_INSPECTOR_ENTRANCE_TYPE_RIDE_ENTRANCE,
    STR_TILE_INSPECTOR_ENTRANCE_TYPE_RIDE_EXIT,
    STR_TILE_INSPECTOR_ENTRANCE_TYPE_PARK_ENTRANC,
};

static constexpr const rct_string_id ParkEntrancePartStringIds[] = {
    STR_TILE_INSPECTOR_ENTRANCE_MIDDLE,
    STR_TILE_INSPECTOR_ENTRANCE_LEFT,
    STR_TILE_INSPECTOR_ENTRANCE_RIGHT,
};

static constexpr const rct_string_id WallSlopeStringIds[] = {
    STR_TILE_INSPECTOR_WALL_FLAT,
    STR_TILE_INSPECTOR_WALL_SLOPED_LEFT,
    STR_TILE_INSPECTOR_WALL_SLOPED_RIGHT,
    STR_TILE_INSPECTOR_WALL_ANIMATION_FRAME,
};

enum WINDOW_TILE_INSPECTOR_WIDGET_IDX
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_LIST,
    WIDX_SPINNER_X,
    WIDX_SPINNER_X_INCREASE,
    WIDX_SPINNER_X_DECREASE,
    WIDX_SPINNER_Y,
    WIDX_SPINNER_Y_INCREASE,
    WIDX_SPINNER_Y_DECREASE,
    WIDX_BUTTON_CORRUPT,
    WIDX_BUTTON_REMOVE,
    WIDX_BUTTON_MOVE_UP,
    WIDX_BUTTON_MOVE_DOWN,
    WIDX_BUTTON_ROTATE,
    WIDX_BUTTON_SORT,
    WIDX_BUTTON_PASTE,
    WIDX_BUTTON_COPY,
    WIDX_COLUMN_TYPE,
    WIDX_COLUMN_BASEHEIGHT,
    WIDX_COLUMN_CLEARANCEHEIGHT,
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
    WIDX_TRACK_CHECK_BLOCK_BRAKE_CLOSED,
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

    // Corrupt
    WIDX_CORRUPT_SPINNER_HEIGHT = PAGE_WIDGETS,
    WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE,
    WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE,
    WIDX_CORRUPT_BUTTON_CLAMP,
};

static constexpr const rct_string_id WINDOW_TITLE = STR_TILE_INSPECTOR_TITLE;

// Window sizes
static constexpr const int32_t WW = 400;
static constexpr const int32_t WH = 170;
constexpr int32_t MIN_WW = WW;
constexpr int32_t MAX_WW = WW;
constexpr int32_t MIN_WH = 130;
constexpr int32_t MAX_WH = 800;

// Button space for top buttons
constexpr auto ToolbarButtonAnchor = ScreenCoordsXY{ WW - 27, 17 };
constexpr auto ToolbarButtonSize = ScreenSize{ 24, 24 };
constexpr auto ToolbarButtonHalfSize = ScreenSize{ 24, 12 };
constexpr auto ToolbarButtonOffsetX = ScreenSize{ -24, 0 };

// List's column offsets
constexpr auto TypeColumnXY = ScreenCoordsXY{ 3, 42 };
constexpr auto TypeColumnSize = ScreenSize{ 312, 14 };
constexpr auto BaseHeightColumnXY = TypeColumnXY + ScreenSize{ TypeColumnSize.width, 0 };
constexpr auto BaseHeightColumnSize = ScreenSize{ 20, 14 };
constexpr auto ClearanceHeightColumnXY = BaseHeightColumnXY + ScreenCoordsXY{ BaseHeightColumnSize.width, 0 };
constexpr auto ClearanceHeightColumnSize = ScreenSize{ 20, 14 };
constexpr auto GhostFlagColumnXY = ClearanceHeightColumnXY + ScreenCoordsXY{ ClearanceHeightColumnSize.width, 0 };
constexpr auto GhostFlagColumnSize = ScreenSize{ 12, 14 };
constexpr auto LastFlagColumnXY = GhostFlagColumnXY + ScreenCoordsXY{ GhostFlagColumnSize.width, 0 };
constexpr auto LastFlagColumnSize = ScreenSize{ 30, 14 };

constexpr int32_t PADDING_BOTTOM = 15;
constexpr int32_t GROUPBOX_PADDING = 6;
constexpr int32_t HORIZONTAL_GROUPBOX_PADDING = 5;
constexpr int32_t VERTICAL_GROUPBOX_PADDING = 4;
constexpr int32_t BUTTONW = 130;
constexpr int32_t BUTTONH = 17;
constexpr auto PropertyButtonSize = ScreenSize{ BUTTONW + 1, BUTTONH + 1 };
constexpr auto PropertyFullWidth = ScreenSize{ 370, BUTTONH + 1 };

constexpr ScreenCoordsXY PropertyRowCol(ScreenCoordsXY anchor, int32_t row, int32_t column)
{
    return anchor
        + ScreenCoordsXY{ column * (PropertyButtonSize.width + HORIZONTAL_GROUPBOX_PADDING),
                          row * (PropertyButtonSize.height + VERTICAL_GROUPBOX_PADDING) };
}

// clang-format off
// Calculates the .left, .right, .top and .bottom for buttons in a group box.
// Buttons are used as reference points for all other widgets in the group boxes.
#define GBBL(col)               (12 + col * (BUTTONW + HORIZONTAL_GROUPBOX_PADDING))
#define GBBR(col)               (GBBL(col) + BUTTONW)
#define GBBT(GROUPTOP, row)     ((GROUPTOP) + 14 + row * (BUTTONH + VERTICAL_GROUPBOX_PADDING))
#define GBBB(GROUPTOP, row)     (GBBT((GROUPTOP), row) + BUTTONH)
#define GBB(GROUPTOP, col, row) GBBL(col), GBBR(col), GBBT((GROUPTOP), row), GBBB((GROUPTOP), row)
#define GBBF(GROUPTOP, col, row)GBBL(col), WW - 10, GBBT((GROUPTOP), row), GBBB((GROUPTOP), row) // Full width

// Dropdowns use the buttons as a reference
#define GBD(GBT, col, row)  GBBL(col), GBBR(col), GBBT(GBT, row) + 3, GBBB(GBT, row) - 3 // Group box dropdown field
#define GBDB(GBT, col, row) GBBR(col) - 11, GBBR(col) - 1, GBBT(GBT, row) + 4, GBBB(GBT, row) - 4 // Group box dropdown button

// Checkbox - given topleft corner
#define CHK(x, y) x, x + 13, y + 2, y + 15

#define MAIN_TILE_INSPECTOR_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({3, 57}, {WW - 6, WH - PADDING_BOTTOM - 58}, WindowWidgetType::Scroll, WindowColour::Secondary), /* Element list */ \
    /* X and Y spinners */ \
    MakeSpinnerWidgets({20, 23}, {51, 12}, WindowWidgetType::Spinner, WindowColour::Secondary), /* Spinner X (3 widgets) */ \
    MakeSpinnerWidgets({90, 23}, {51, 12}, WindowWidgetType::Spinner, WindowColour::Secondary), /* Spinner Y (3 widgets) */ \
    /* Top buttons */ \
    MakeWidget(ToolbarButtonAnchor,                                                ToolbarButtonSize,     WindowWidgetType::FlatBtn ,    WindowColour::Secondary, SPR_MAP,          STR_INSERT_CORRUPT_TIP),              /* Insert corrupt button */ \
    MakeWidget(ToolbarButtonAnchor + ToolbarButtonOffsetX * 1,                     ToolbarButtonSize,     WindowWidgetType::FlatBtn,     WindowColour::Secondary, SPR_DEMOLISH,     STR_REMOVE_SELECTED_ELEMENT_TIP ),    /* Remove button */         \
    MakeWidget(ToolbarButtonAnchor + ToolbarButtonOffsetX * 2,                     ToolbarButtonHalfSize, WindowWidgetType::Button,      WindowColour::Secondary, STR_UP,           STR_MOVE_SELECTED_ELEMENT_UP_TIP),    /* Move up */               \
    MakeWidget(ToolbarButtonAnchor + ToolbarButtonOffsetX * 2 + ScreenSize{0, 12}, ToolbarButtonHalfSize, WindowWidgetType::Button,      WindowColour::Secondary, STR_DOWN,         STR_MOVE_SELECTED_ELEMENT_DOWN_TIP),  /* Move down */             \
    MakeWidget(ToolbarButtonAnchor + ToolbarButtonOffsetX * 3,                     ToolbarButtonSize,     WindowWidgetType::FlatBtn,     WindowColour::Secondary, SPR_ROTATE_ARROW, STR_ROTATE_SELECTED_ELEMENT_TIP),     /* Rotate button */         \
    MakeWidget(ToolbarButtonAnchor + ToolbarButtonOffsetX * 4,                     ToolbarButtonSize,     WindowWidgetType::FlatBtn,     WindowColour::Secondary, SPR_G2_SORT,      STR_TILE_INSPECTOR_SORT_TIP),         /* Sort button */           \
    MakeWidget(ToolbarButtonAnchor + ToolbarButtonOffsetX * 5,                     ToolbarButtonSize,     WindowWidgetType::FlatBtn,     WindowColour::Secondary, SPR_G2_PASTE,     STR_TILE_INSPECTOR_PASTE_TIP),        /* Paste button */          \
    MakeWidget(ToolbarButtonAnchor + ToolbarButtonOffsetX * 6,                     ToolbarButtonSize,     WindowWidgetType::FlatBtn,     WindowColour::Secondary, SPR_G2_COPY,      STR_TILE_INSPECTOR_COPY_TIP),         /* Copy button */           \
    /* Column headers */ \
    MakeWidget(TypeColumnXY,            TypeColumnSize,            WindowWidgetType::TableHeader, WindowColour::Secondary, STR_TILE_INSPECTOR_ELEMENT_TYPE),                                                /* Type */              \
    MakeWidget(BaseHeightColumnXY,      BaseHeightColumnSize,      WindowWidgetType::TableHeader, WindowColour::Secondary, STR_TILE_INSPECTOR_BASE_HEIGHT_SHORT,      STR_TILE_INSPECTOR_BASE_HEIGHT),      /* Base height */       \
    MakeWidget(ClearanceHeightColumnXY, ClearanceHeightColumnSize, WindowWidgetType::TableHeader, WindowColour::Secondary, STR_TILE_INSPECTOR_CLEARANGE_HEIGHT_SHORT, STR_TILE_INSPECTOR_CLEARANCE_HEIGHT), /* Clearance height */  \
    MakeWidget(GhostFlagColumnXY,       GhostFlagColumnSize,       WindowWidgetType::TableHeader, WindowColour::Secondary, STR_TILE_INSPECTOR_FLAG_GHOST_SHORT,       STR_TILE_INSPECTOR_FLAG_GHOST),       /* Ghost flag */        \
    MakeWidget(LastFlagColumnXY,        LastFlagColumnSize,        WindowWidgetType::TableHeader, WindowColour::Secondary, STR_TILE_INSPECTOR_FLAG_LAST_SHORT,        STR_TILE_INSPECTOR_FLAG_LAST),        /* Last of tile flag */ \
    /* Group boxes */ \
    MakeWidget({6, 0},             {WW - 12, 0}, WindowWidgetType::Groupbox,    WindowColour::Secondary, STR_NONE,                               STR_NONE ), /* Details group box */     \
    MakeWidget({6, 0},             {WW - 12, 0}, WindowWidgetType::Groupbox,    WindowColour::Secondary, STR_TILE_INSPECTOR_GROUPBOX_PROPERTIES, STR_NONE )  /* Properties group box */

static rct_widget DefaultWidgets[] = {
    MAIN_TILE_INSPECTOR_WIDGETS,
    WIDGETS_END,
};

constexpr int32_t NumSurfaceProperties = 4;
constexpr int32_t NumSurfaceDetails = 4;
constexpr int32_t SurfacePropertiesHeight = 16 + NumSurfaceProperties * 21;
constexpr int32_t SurfaceDetailsHeight = 20 + NumSurfaceDetails * 11;
static rct_widget SurfaceWidgets[] = {
    MAIN_TILE_INSPECTOR_WIDGETS,
    MakeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), PropertyButtonSize, WindowWidgetType::Spinner, WindowColour::Secondary), // WIDX_SURFACE_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 0),         PropertyButtonSize, WindowWidgetType::Button,  WindowColour::Secondary, STR_TILE_INSPECTOR_SURFACE_REMOVE_FENCES), // WIDX_SURFACE_BUTTON_REMOVE_FENCES
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 1),         PropertyButtonSize, WindowWidgetType::Button,  WindowColour::Secondary, STR_TILE_INSPECTOR_SURFACE_RESTORE_FENCES), // WIDX_SURFACE_BUTTON_RESTORE_FENCES
    MakeWidget(PropertyRowCol({ 12, 0 }, 3, 1) + ScreenCoordsXY{ 14 * 1, 7 * 0}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_SURFACE_CHECK_CORNER_N
    MakeWidget(PropertyRowCol({ 12, 0 }, 3, 1) + ScreenCoordsXY{ 14 * 2, 7 * 1}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_SURFACE_CHECK_CORNER_E
    MakeWidget(PropertyRowCol({ 12, 0 }, 3, 1) + ScreenCoordsXY{ 14 * 1, 7 * 2}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_SURFACE_CHECK_CORNER_S
    MakeWidget(PropertyRowCol({ 12, 0 }, 3, 1) + ScreenCoordsXY{ 14 * 0, 7 * 1}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_SURFACE_CHECK_CORNER_W
    MakeWidget(PropertyRowCol({ 12, 0 }, 4, 0), PropertyFullWidth, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_TILE_INSPECTOR_SURFACE_DIAGONAL), // WIDX_SURFACE_CHECK_DIAGONAL
    WIDGETS_END,
};

constexpr int32_t NumPathProperties = 5;
constexpr int32_t NumPathDetails = 2;
constexpr int32_t PathPropertiesHeight = 16 + NumPathProperties * 21;
constexpr int32_t PathDetailsHeight = 20 + NumPathDetails * 11;
static rct_widget PathWidgets[] = {
    MAIN_TILE_INSPECTOR_WIDGETS,
    MakeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), PropertyButtonSize, WindowWidgetType::Spinner, WindowColour::Secondary), // WIDX_PATH_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 0), PropertyFullWidth, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_TILE_INSPECTOR_PATH_BROKEN), // WIDX_PATH_CHECK_BROKEN
    MakeWidget(PropertyRowCol({ 12, 0 }, 2, 0), PropertyFullWidth, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_TILE_INSPECTOR_PATH_SLOPED), // WIDX_PATH_CHECK_SLOPED
    MakeWidget(PropertyRowCol({ 12, 0 }, 3, 1) + ScreenCoordsXY{ 14 * 3, 7 * 1}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_PATH_CHECK_EDGE_NE
    MakeWidget(PropertyRowCol({ 12, 0 }, 3, 1) + ScreenCoordsXY{ 14 * 4, 7 * 2}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_PATH_CHECK_EDGE_E
    MakeWidget(PropertyRowCol({ 12, 0 }, 3, 1) + ScreenCoordsXY{ 14 * 3, 7 * 3}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_PATH_CHECK_EDGE_SE
    MakeWidget(PropertyRowCol({ 12, 0 }, 3, 1) + ScreenCoordsXY{ 14 * 2, 7 * 4}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_PATH_CHECK_EDGE_S
    MakeWidget(PropertyRowCol({ 12, 0 }, 3, 1) + ScreenCoordsXY{ 14 * 1, 7 * 3}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_PATH_CHECK_EDGE_SW
    MakeWidget(PropertyRowCol({ 12, 0 }, 3, 1) + ScreenCoordsXY{ 14 * 0, 7 * 2}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_PATH_CHECK_EDGE_W
    MakeWidget(PropertyRowCol({ 12, 0 }, 3, 1) + ScreenCoordsXY{ 14 * 1, 7 * 1}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_PATH_CHECK_EDGE_NW
    MakeWidget(PropertyRowCol({ 12, 0 }, 3, 1) + ScreenCoordsXY{ 14 * 2, 7 * 0}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_PATH_CHECK_EDGE_N
    WIDGETS_END,
};

constexpr int32_t NumTrackProperties = 5;
constexpr int32_t NumTrackDetails = 7;
constexpr int32_t TrackPropertiesHeight = 16 + NumTrackProperties * 21;
constexpr int32_t TrackDetailsHeight = 20 + NumTrackDetails * 11;
static rct_widget TrackWidgets[] = {
    MAIN_TILE_INSPECTOR_WIDGETS,
    MakeWidget(PropertyRowCol({ 12, 0}, 0, 0), PropertyFullWidth, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_TILE_INSPECTOR_TRACK_ENTIRE_TRACK_PIECE), // WIDX_TRACK_CHECK_APPLY_TO_ALL
    MakeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 1, 1), PropertyButtonSize, WindowWidgetType::Spinner, WindowColour::Secondary), // WIDX_TRACK_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
    MakeWidget(PropertyRowCol({ 12, 0}, 2, 0), PropertyFullWidth, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_TILE_INSPECTOR_TRACK_CHAIN_LIFT), // WIDX_TRACK_CHECK_CHAIN_LIFT
    MakeWidget(PropertyRowCol({ 12, 0}, 3, 0), PropertyFullWidth, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_TILE_INSPECTOR_TRACK_BLOCK_BRAKE), // WIDX_TRACK_CHECK_BLOCK_BRAKE_CLOSED
    MakeWidget(PropertyRowCol({ 12, 0}, 4, 0), PropertyFullWidth, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_TILE_INSPECTOR_TRACK_IS_INDESTRUCTIBLE), // WIDX_TRACK_CHECK_IS_INDESTRUCTIBLE
    WIDGETS_END,
};

constexpr int32_t NumSceneryProperties = 4; // The checkbox groups both count for 2 rows
constexpr int32_t NumSceneryDetails = 4;
constexpr int32_t SceneryPropertiesHeight = 16 + NumSceneryProperties * 21;
constexpr int32_t SceneryDetailsHeight = 20 + NumSceneryDetails * 11;
static rct_widget SceneryWidgets[] = {
    MAIN_TILE_INSPECTOR_WIDGETS,
    MakeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), PropertyButtonSize, WindowWidgetType::Spinner, WindowColour::Secondary), // WIDX_SCENERY_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 1) + ScreenCoordsXY{ 14 * 1, 7 * 0}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_SCENERY_CHECK_QUARTER_N
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 1) + ScreenCoordsXY{ 14 * 2, 7 * 1}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_SCENERY_CHECK_QUARTER_E
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 1) + ScreenCoordsXY{ 14 * 1, 7 * 2}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_SCENERY_CHECK_QUARTER_S
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 1) + ScreenCoordsXY{ 14 * 0, 7 * 1}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_SCENERY_CHECK_QUARTER_W
    MakeWidget(PropertyRowCol({ 12, 0 }, 2, 1) + ScreenCoordsXY{ 14 * 1, 7 * 0}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_SCENERY_CHECK_COLLISION_N
    MakeWidget(PropertyRowCol({ 12, 0 }, 2, 1) + ScreenCoordsXY{ 14 * 2, 7 * 1}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_SCENERY_CHECK_COLLISION_E
    MakeWidget(PropertyRowCol({ 12, 0 }, 2, 1) + ScreenCoordsXY{ 14 * 1, 7 * 2}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_SCENERY_CHECK_COLLISION_S
    MakeWidget(PropertyRowCol({ 12, 0 }, 2, 1) + ScreenCoordsXY{ 14 * 0, 7 * 1}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_SCENERY_CHECK_COLLISION_W
    WIDGETS_END,
};

constexpr int32_t NumEntranceProperties = 2;
constexpr int32_t NumEntranceDetails = 4;
constexpr int32_t EntrancePropertiesHeight = 16 + NumEntranceProperties * 21;
constexpr int32_t EntranceDetailsHeight = 20 + NumEntranceDetails * 11;
static rct_widget EntranceWidgets[] = {
    MAIN_TILE_INSPECTOR_WIDGETS,
    MakeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), PropertyButtonSize, WindowWidgetType::Spinner, WindowColour::Secondary), // WIDX_ENTRANCE_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 0),         PropertyButtonSize, WindowWidgetType::Button,  WindowColour::Secondary, STR_TILE_INSPECTOR_ENTRANCE_MAKE_USABLE, STR_TILE_INSPECTOR_ENTRANCE_MAKE_USABLE_TIP), // WIDX_ENTRANCE_BUTTON_MAKE_USABLE
    WIDGETS_END,
};

constexpr int32_t NumWallProperties = 3;
constexpr int32_t NumWallDetails = 2;
constexpr int32_t WallPropertiesHeight = 16 + NumWallProperties * 21;
constexpr int32_t WallDetailsHeight = 20 + NumWallDetails * 11;
static rct_widget WallWidgets[] = {
    MAIN_TILE_INSPECTOR_WIDGETS,
    MakeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1),                 PropertyButtonSize, WindowWidgetType::Spinner,      WindowColour::Secondary), // WIDX_WALL_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 1),                         PropertyButtonSize, WindowWidgetType::DropdownMenu, WindowColour::Secondary), // WIDX_WALL_DROPDOWN_SLOPE
    MakeWidget(PropertyRowCol({ 12 + PropertyButtonSize.width - 12, 0 }, 1, 1), { 11,  12}, WindowWidgetType::Button,       WindowColour::Secondary, STR_DROPDOWN_GLYPH), // WIDX_WALL_DROPDOWN_SLOPE_BUTTON
    MakeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 2, 1),                 PropertyButtonSize, WindowWidgetType::Spinner,      WindowColour::Secondary), // WIDX_WALL_SPINNER_ANIMATION_FRAME{,_INCREASE,_DECREASE}
    WIDGETS_END,
};

constexpr int32_t NumLargeSceneryProperties = 1;
constexpr int32_t NumLargeSceneryDetails = 3;
constexpr int32_t LargeSceneryPropertiesHeight = 16 + NumLargeSceneryProperties * 21;
constexpr int32_t LargeSceneryDetailsHeight = 20 + NumLargeSceneryDetails * 11;
static rct_widget LargeSceneryWidgets[] = {
    MAIN_TILE_INSPECTOR_WIDGETS,
    MakeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), PropertyButtonSize, WindowWidgetType::Spinner, WindowColour::Secondary), // WIDX_LARGE_SCENERY_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
    WIDGETS_END,
};

constexpr int32_t NumBannerProperties = 3;
constexpr int32_t NumBannerDetails = 1;
constexpr int32_t BannerPropertiesHeight = 16 + NumBannerProperties * 21;
constexpr int32_t BannerDetailsHeight = 20 + NumBannerDetails * 11;
static rct_widget BannerWidgets[] = {
    MAIN_TILE_INSPECTOR_WIDGETS,
    MakeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), PropertyButtonSize, WindowWidgetType::Spinner, WindowColour::Secondary), // WIDX_BANNER_SPINNER_HEIGHT{,_INCREASE,_DECREASE}
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 1) + ScreenCoordsXY{ 14 * 3, 7 * 1}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_BANNER_CHECK_BLOCK_NE
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 1) + ScreenCoordsXY{ 14 * 3, 7 * 3}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_BANNER_CHECK_BLOCK_SE
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 1) + ScreenCoordsXY{ 14 * 1, 7 * 3}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_BANNER_CHECK_BLOCK_SW
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 1) + ScreenCoordsXY{ 14 * 1, 7 * 1}, { 12, 12 }, WindowWidgetType::Checkbox, WindowColour::Secondary), // WIDX_BANNER_CHECK_BLOCK_NW

    WIDGETS_END,
};

constexpr int32_t NumCorruptProperties = 2;
constexpr int32_t NumCorruptDetails = 0;
constexpr int32_t CorruptPropertiesHeight = 16 + NumCorruptProperties * 21;
constexpr int32_t CorruptDetailsHeight = 20 + NumCorruptDetails * 11;
static rct_widget CorruptWidgets[] = {
    MAIN_TILE_INSPECTOR_WIDGETS,
    MakeSpinnerWidgets(PropertyRowCol({ 12, 0 }, 0, 1), PropertyButtonSize, WindowWidgetType::Spinner, WindowColour::Secondary), // WIDX_CORRUPT_SPINNER_HEIGHT
    MakeWidget(PropertyRowCol({ 12, 0 }, 1, 0), PropertyButtonSize, WindowWidgetType::Button, WindowColour::Secondary,STR_TILE_INSPECTOR_CLAMP_TO_NEXT, STR_TILE_INSPECTOR_CLAMP_TO_NEXT_TIP ), // WIDX_CORRUPT_BUTTON_CLAMP
    WIDGETS_END,
};

static rct_widget *PageWidgets[] = {
    DefaultWidgets,
    SurfaceWidgets,
    PathWidgets,
    TrackWidgets,
    SceneryWidgets,
    EntranceWidgets,
    WallWidgets,
    LargeSceneryWidgets,
    BannerWidgets,
    CorruptWidgets,
};
// clang-format on

struct TileInspectorGroupboxSettings
{
    // Offsets from the bottom of the window
    int16_t details_top_offset, details_bottom_offset;
    int16_t properties_top_offset, properties_bottom_offset;
    // String to be displayed in the details groupbox
    rct_string_id string_id;
};

static constexpr TileInspectorGroupboxSettings MakeGroupboxSettings(
    int16_t detailsHeight, int16_t propertiesHeight, rct_string_id stringId)
{
    TileInspectorGroupboxSettings settings{};
    decltype(settings.properties_bottom_offset) offsetSum = 0;
    settings.properties_bottom_offset = (offsetSum += PADDING_BOTTOM);
    settings.properties_top_offset = (offsetSum += propertiesHeight);
    settings.details_bottom_offset = (offsetSum += GROUPBOX_PADDING);
    settings.details_top_offset = (offsetSum += detailsHeight);
    settings.string_id = stringId;
    return settings;
}

static constexpr TileInspectorGroupboxSettings PageGroupBoxSettings[] = {
    MakeGroupboxSettings(SurfaceDetailsHeight, SurfacePropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_SURFACE_INFO),
    MakeGroupboxSettings(PathDetailsHeight, PathPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_PATH_INFO),
    MakeGroupboxSettings(TrackDetailsHeight, TrackPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_TRACK_INFO),
    MakeGroupboxSettings(SceneryDetailsHeight, SceneryPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_SCENERY_INFO),
    MakeGroupboxSettings(EntranceDetailsHeight, EntrancePropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_ENTRANCE_INFO),
    MakeGroupboxSettings(WallDetailsHeight, WallPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_WALL_INFO),
    MakeGroupboxSettings(LargeSceneryDetailsHeight, LargeSceneryPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_BANNER_INFO),
    MakeGroupboxSettings(BannerDetailsHeight, BannerPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_BANNER_INFO),
    MakeGroupboxSettings(CorruptDetailsHeight, CorruptPropertiesHeight, STR_TILE_INSPECTOR_GROUPBOX_CORRUPT_INFO),
};

static constexpr int32_t ViewportInteractionFlags = EnumsToFlags(
    ViewportInteractionItem::Terrain, ViewportInteractionItem::Entity, ViewportInteractionItem::Ride,
    ViewportInteractionItem::Scenery, ViewportInteractionItem::Footpath, ViewportInteractionItem::FootpathItem,
    ViewportInteractionItem::ParkEntrance, ViewportInteractionItem::Wall, ViewportInteractionItem::LargeScenery,
    ViewportInteractionItem::Banner);

static int16_t windowTileInspectorHighlightedIndex = -1;
static bool windowTileInspectorTileSelected = false;
static int32_t windowTileInspectorToolMouseX = 0;
static int32_t windowTileInspectorToolMouseY = 0;
static bool windowTileInspectorToolCtrlDown = false;
static CoordsXY windowTileInspectorToolMap = {};
static bool windowTileInspectorApplyToAll = false;
static bool windowTileInspectorElementCopied = false;
static TileElement tileInspectorCopiedElement;

static void window_tile_inspector_mouseup(rct_window* w, rct_widgetindex widgetIndex);
static void window_tile_inspector_resize(rct_window* w);
static void window_tile_inspector_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_tile_inspector_update(rct_window* w);
static void window_tile_inspector_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_tile_inspector_tool_update(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_tile_inspector_update_selected_tile(rct_window* w, const ScreenCoordsXY& screenCoords);
static void window_tile_inspector_tool_down(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_tile_inspector_tool_drag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_tile_inspector_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height);
static void window_tile_inspector_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_tile_inspector_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_tile_inspector_invalidate(rct_window* w);
static void window_tile_inspector_paint(rct_window* w, rct_drawpixelinfo* dpi);
static void window_tile_inspector_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex);
static void window_tile_inspector_set_page(rct_window* w, const TileInspectorPage page);
static void window_tile_inspector_close(rct_window* w);

static rct_window_event_list TileInspectorWindowEvents([](auto& events) {
    events.mouse_up = &window_tile_inspector_mouseup;
    events.resize = &window_tile_inspector_resize;
    events.mouse_down = &window_tile_inspector_mousedown;
    events.dropdown = &window_tile_inspector_dropdown;
    events.update = &window_tile_inspector_update;
    events.tool_update = &window_tile_inspector_tool_update;
    events.tool_down = &window_tile_inspector_tool_down;
    events.tool_drag = &window_tile_inspector_tool_drag;
    events.get_scroll_size = &window_tile_inspector_scrollgetsize;
    events.scroll_mousedown = &window_tile_inspector_scrollmousedown;
    events.scroll_mouseover = &window_tile_inspector_scrollmouseover;
    events.invalidate = &window_tile_inspector_invalidate;
    events.paint = &window_tile_inspector_paint;
    events.scroll_paint = &window_tile_inspector_scrollpaint;
    events.close = &window_tile_inspector_close;
});

// clang-format off
static uint64_t PageEnabledWidgets[] = {
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT),
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_SURFACE_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_SURFACE_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_SURFACE_BUTTON_REMOVE_FENCES) | (1ULL << WIDX_SURFACE_BUTTON_RESTORE_FENCES) | (1ULL << WIDX_SURFACE_CHECK_CORNER_N) | (1ULL << WIDX_SURFACE_CHECK_CORNER_E) | (1ULL << WIDX_SURFACE_CHECK_CORNER_S) | (1ULL << WIDX_SURFACE_CHECK_CORNER_W) | (1ULL << WIDX_SURFACE_CHECK_DIAGONAL),
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_PATH_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_PATH_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_PATH_CHECK_SLOPED) | (1ULL << WIDX_PATH_CHECK_BROKEN) | (1ULL << WIDX_PATH_CHECK_EDGE_N) | (1ULL << WIDX_PATH_CHECK_EDGE_NE) | (1ULL << WIDX_PATH_CHECK_EDGE_E) | (1ULL << WIDX_PATH_CHECK_EDGE_SE) | (1ULL << WIDX_PATH_CHECK_EDGE_S) | (1ULL << WIDX_PATH_CHECK_EDGE_SW) | (1ULL << WIDX_PATH_CHECK_EDGE_W) | (1ULL << WIDX_PATH_CHECK_EDGE_NW),
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_TRACK_CHECK_APPLY_TO_ALL) | (1ULL << WIDX_TRACK_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_TRACK_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_TRACK_CHECK_CHAIN_LIFT) | (1ULL << WIDX_TRACK_CHECK_BLOCK_BRAKE_CLOSED) | (1ULL << WIDX_TRACK_CHECK_IS_INDESTRUCTIBLE),
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_SCENERY_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_SCENERY_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_SCENERY_CHECK_QUARTER_N) | (1ULL << WIDX_SCENERY_CHECK_QUARTER_E) | (1ULL << WIDX_SCENERY_CHECK_QUARTER_S) | (1ULL << WIDX_SCENERY_CHECK_QUARTER_W) | (1ULL << WIDX_SCENERY_CHECK_COLLISION_N) | (1ULL << WIDX_SCENERY_CHECK_COLLISION_E) | (1ULL << WIDX_SCENERY_CHECK_COLLISION_S) | (1ULL << WIDX_SCENERY_CHECK_COLLISION_W),
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_ENTRANCE_BUTTON_MAKE_USABLE),
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_WALL_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_WALL_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_WALL_DROPDOWN_SLOPE) | (1ULL << WIDX_WALL_DROPDOWN_SLOPE_BUTTON) | (1ULL << WIDX_WALL_SPINNER_ANIMATION_FRAME_INCREASE) | (1ULL << WIDX_WALL_SPINNER_ANIMATION_FRAME_DECREASE),
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE),
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_BANNER_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_BANNER_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_BANNER_CHECK_BLOCK_NE) | (1ULL << WIDX_BANNER_CHECK_BLOCK_SE) | (1ULL << WIDX_BANNER_CHECK_BLOCK_SW) | (1ULL << WIDX_BANNER_CHECK_BLOCK_NW),
    (1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_CORRUPT_BUTTON_CLAMP),
};

static uint64_t PageHoldDownWidgets[] = {
    (1ULL << WIDX_SPINNER_X_INCREASE) | (1ULL << WIDX_SPINNER_X_DECREASE) | (1ULL << WIDX_SPINNER_Y_INCREASE) | (1ULL << WIDX_SPINNER_Y_DECREASE),
    (1ULL << WIDX_SPINNER_X_INCREASE) | (1ULL << WIDX_SPINNER_X_DECREASE) | (1ULL << WIDX_SPINNER_Y_INCREASE) | (1ULL << WIDX_SPINNER_Y_DECREASE) | (1ULL << WIDX_SURFACE_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_SURFACE_SPINNER_HEIGHT_DECREASE),
    (1ULL << WIDX_SPINNER_X_INCREASE) | (1ULL << WIDX_SPINNER_X_DECREASE) | (1ULL << WIDX_SPINNER_Y_INCREASE) | (1ULL << WIDX_SPINNER_Y_DECREASE) | (1ULL << WIDX_PATH_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_PATH_SPINNER_HEIGHT_DECREASE),
    (1ULL << WIDX_SPINNER_X_INCREASE) | (1ULL << WIDX_SPINNER_X_DECREASE) | (1ULL << WIDX_SPINNER_Y_INCREASE) | (1ULL << WIDX_SPINNER_Y_DECREASE) | (1ULL << WIDX_TRACK_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_TRACK_SPINNER_HEIGHT_DECREASE),
    (1ULL << WIDX_SPINNER_X_INCREASE) | (1ULL << WIDX_SPINNER_X_DECREASE) | (1ULL << WIDX_SPINNER_Y_INCREASE) | (1ULL << WIDX_SPINNER_Y_DECREASE) | (1ULL << WIDX_SCENERY_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_SCENERY_SPINNER_HEIGHT_DECREASE),
    (1ULL << WIDX_SPINNER_X_INCREASE) | (1ULL << WIDX_SPINNER_X_DECREASE) | (1ULL << WIDX_SPINNER_Y_INCREASE) | (1ULL << WIDX_SPINNER_Y_DECREASE) | (1ULL << WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE),
    (1ULL << WIDX_SPINNER_X_INCREASE) | (1ULL << WIDX_SPINNER_X_DECREASE) | (1ULL << WIDX_SPINNER_Y_INCREASE) | (1ULL << WIDX_SPINNER_Y_DECREASE) | (1ULL << WIDX_WALL_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_WALL_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_WALL_SPINNER_ANIMATION_FRAME_INCREASE) | (1ULL << WIDX_WALL_SPINNER_ANIMATION_FRAME_DECREASE),
    (1ULL << WIDX_SPINNER_X_INCREASE) | (1ULL << WIDX_SPINNER_X_DECREASE) | (1ULL << WIDX_SPINNER_Y_INCREASE) | (1ULL << WIDX_SPINNER_Y_DECREASE) | (1ULL << WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE),
    (1ULL << WIDX_SPINNER_X_INCREASE) | (1ULL << WIDX_SPINNER_X_DECREASE) | (1ULL << WIDX_SPINNER_Y_INCREASE) | (1ULL << WIDX_SPINNER_Y_DECREASE) | (1ULL << WIDX_BANNER_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_BANNER_SPINNER_HEIGHT_DECREASE),
    (1ULL << WIDX_SPINNER_X_INCREASE) | (1ULL << WIDX_SPINNER_X_DECREASE) | (1ULL << WIDX_SPINNER_Y_INCREASE) | (1ULL << WIDX_SPINNER_Y_DECREASE) | (1ULL << WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE),
};

static uint64_t PageDisabledWidgets[] = {
    (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_MOVE_UP) | (1ULL << WIDX_BUTTON_MOVE_DOWN) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY),
    0,
    0,
    0,
    0,
    0,
    0,
    (1ULL << WIDX_BUTTON_ROTATE),
    0,
    (1ULL << WIDX_BUTTON_ROTATE),
};
// clang-format on

rct_window* window_tile_inspector_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_TILE_INSPECTOR);
    if (window != nullptr)
        return window;

    window = WindowCreate(ScreenCoordsXY(0, 29), WW, WH, &TileInspectorWindowEvents, WC_TILE_INSPECTOR, WF_RESIZABLE);

    window_tile_inspector_set_page(window, TileInspectorPage::Default);
    window->min_width = MIN_WW;
    window->min_height = MIN_WH;
    window->max_width = MAX_WW;
    window->max_height = MAX_WH;
    windowTileInspectorSelectedIndex = -1;
    WindowInitScrollWidgets(window);

    windowTileInspectorTileSelected = false;

    tool_set(window, WIDX_BACKGROUND, Tool::Crosshair);

    return window;
}

void window_tile_inspector_clear_clipboard()
{
    windowTileInspectorElementCopied = false;
}

static TileElement* window_tile_inspector_get_selected_element(rct_window* w)
{
    openrct2_assert(
        windowTileInspectorSelectedIndex >= 0 && windowTileInspectorSelectedIndex < windowTileInspectorElementCount,
        "Selected list item out of range");
    return map_get_first_element_at(windowTileInspectorToolMap) + windowTileInspectorSelectedIndex;
}

static void window_tile_inspector_select_element_from_list(rct_window* w, int32_t index)
{
    if (index < 0 || index >= windowTileInspectorElementCount)
    {
        windowTileInspectorSelectedIndex = -1;
        OpenRCT2::TileInspector::SetSelectedElement(nullptr);
    }
    else
    {
        windowTileInspectorSelectedIndex = index;

        const TileElement* const tileElement = window_tile_inspector_get_selected_element(w);
        OpenRCT2::TileInspector::SetSelectedElement(tileElement);
    }

    w->Invalidate();
}

static void window_tile_inspector_load_tile(rct_window* w, TileElement* elementToSelect)
{
    windowTileInspectorSelectedIndex = -1;
    w->scrolls[0].v_top = 0;

    TileElement* element = map_get_first_element_at(windowTileInspectorToolMap);
    int16_t numItems = 0;
    do
    {
        if (element == nullptr)
            break;
        if (element == elementToSelect)
        {
            windowTileInspectorSelectedIndex = numItems;
        }

        numItems++;
    } while (!(element++)->IsLastForTile());

    windowTileInspectorElementCount = numItems;

    w->Invalidate();
}

static void window_tile_inspector_insert_corrupt_element(int32_t elementIndex)
{
    openrct2_assert(elementIndex >= 0 && elementIndex < windowTileInspectorElementCount, "elementIndex out of range");
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::AnyInsertCorrupt, elementIndex);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_remove_element(int32_t elementIndex)
{
    openrct2_assert(elementIndex >= 0 && elementIndex < windowTileInspectorElementCount, "elementIndex out of range");
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::AnyRemove, elementIndex);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_rotate_element(int32_t elementIndex)
{
    openrct2_assert(elementIndex >= 0 && elementIndex < windowTileInspectorElementCount, "elementIndex out of range");
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::AnyRotate, elementIndex);
    GameActions::Execute(&modifyTile);
}

// Swap element with its parent
static void window_tile_inspector_swap_elements(int16_t first, int16_t second)
{
    bool firstInRange = first >= 0 && first < windowTileInspectorElementCount;
    bool secondInRange = second >= 0 && second < windowTileInspectorElementCount;
    // This might happen if two people are modifying the same tile.
    if (!firstInRange || !secondInRange)
        return;

    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::AnySwap, first, second);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_sort_elements()
{
    openrct2_assert(windowTileInspectorTileSelected, "No tile selected");
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::AnySort);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_copy_element(rct_window* w)
{
    // Copy value, in case the element gets moved
    tileInspectorCopiedElement = *window_tile_inspector_get_selected_element(w);
    windowTileInspectorElementCopied = true;
    w->Invalidate();
}

static void window_tile_inspector_paste_element(rct_window* w)
{
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::AnyPaste, 0, 0, tileInspectorCopiedElement);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_base_height_offset(int16_t elementIndex, int8_t heightOffset)
{
    auto modifyTile = TileModifyAction(
        windowTileInspectorToolMap, TileModifyType::AnyBaseHeightOffset, elementIndex, heightOffset);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_surface_show_park_fences(bool showFences)
{
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::SurfaceShowParkFences, showFences);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_surface_toggle_corner(int32_t cornerIndex)
{
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::SurfaceToggleCorner, cornerIndex);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_surface_toggle_diagonal()
{
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::SurfaceToggleDiagonal);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_path_set_sloped(int32_t elementIndex, bool sloped)
{
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::PathSetSlope, elementIndex, sloped);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_path_set_broken(int32_t elementIndex, bool broken)
{
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::PathSetBroken, elementIndex, broken);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_path_toggle_edge(int32_t elementIndex, int32_t cornerIndex)
{
    openrct2_assert(elementIndex >= 0 && elementIndex < windowTileInspectorElementCount, "elementIndex out of range");
    openrct2_assert(cornerIndex >= 0 && cornerIndex < 8, "cornerIndex out of range");
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::PathToggleEdge, elementIndex, cornerIndex);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_entrance_make_usable(int32_t elementIndex)
{
    Guard::ArgumentInRange(elementIndex, 0, windowTileInspectorElementCount - 1);
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::EntranceMakeUsable, elementIndex);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_wall_set_slope(int32_t elementIndex, int32_t slopeValue)
{
    // Make sure only the correct bits are set
    openrct2_assert((slopeValue & 3) == slopeValue, "slopeValue doesn't match its mask");
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::WallSetSlope, elementIndex, slopeValue);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_wall_animation_frame_offset(int16_t elementIndex, int8_t animationFrameOffset)
{
    auto modifyTile = TileModifyAction(
        windowTileInspectorToolMap, TileModifyType::WallSetAnimationFrame, elementIndex, animationFrameOffset);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_track_block_height_offset(int32_t elementIndex, int8_t heightOffset)
{
    auto modifyTile = TileModifyAction(
        windowTileInspectorToolMap, TileModifyType::TrackBaseHeightOffset, elementIndex, heightOffset);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_track_block_set_lift(int32_t elementIndex, bool entireTrackBlock, bool chain)
{
    auto modifyTile = TileModifyAction(
        windowTileInspectorToolMap, entireTrackBlock ? TileModifyType::TrackSetChainBlock : TileModifyType::TrackSetChain,
        elementIndex, chain);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_track_set_block_brake(int32_t elementIndex, bool blockBrake)
{
    auto modifyTile = TileModifyAction(
        windowTileInspectorToolMap, TileModifyType::TrackSetBlockBrake, elementIndex, blockBrake);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_track_set_indestructible(int32_t elementIndex, bool isIndestructible)
{
    auto modifyTile = TileModifyAction(
        windowTileInspectorToolMap, TileModifyType::TrackSetIndestructible, elementIndex, isIndestructible);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_quarter_tile_set(int32_t elementIndex, const int32_t quarterIndex)
{
    // quarterIndex is widget index relative to WIDX_SCENERY_CHECK_QUARTER_N, so a value from 0-3
    openrct2_assert(quarterIndex >= 0 && quarterIndex < 4, "quarterIndex out of range");
    auto modifyTile = TileModifyAction(
        windowTileInspectorToolMap, TileModifyType::ScenerySetQuarterLocation, elementIndex,
        (quarterIndex - get_current_rotation()) & 3);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_toggle_quadrant_collosion(int32_t elementIndex, const int32_t quadrantIndex)
{
    auto modifyTile = TileModifyAction(
        windowTileInspectorToolMap, TileModifyType::ScenerySetQuarterCollision, elementIndex,
        (quadrantIndex + 2 - get_current_rotation()) & 3);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_banner_toggle_block(int32_t elementIndex, int32_t edgeIndex)
{
    openrct2_assert(edgeIndex >= 0 && edgeIndex < 4, "edgeIndex out of range");

    // Make edgeIndex abstract
    edgeIndex = (edgeIndex - get_current_rotation()) & 3;
    auto modifyTile = TileModifyAction(
        windowTileInspectorToolMap, TileModifyType::BannerToggleBlockingEdge, elementIndex, edgeIndex);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_clamp_corrupt(int32_t elementIndex)
{
    auto modifyTile = TileModifyAction(windowTileInspectorToolMap, TileModifyType::CorruptClamp, elementIndex);
    GameActions::Execute(&modifyTile);
}

static void window_tile_inspector_close(rct_window* w)
{
    OpenRCT2::TileInspector::SetSelectedElement(nullptr);
}

static void window_tile_inspector_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            tool_cancel();
            window_close(w);
            return;
        case WIDX_BUTTON_CORRUPT:
            window_tile_inspector_insert_corrupt_element(windowTileInspectorSelectedIndex);
            break;
        case WIDX_BUTTON_REMOVE:
        {
            int32_t nextItemToSelect = windowTileInspectorSelectedIndex - 1;
            window_tile_inspector_remove_element(windowTileInspectorSelectedIndex);
            window_tile_inspector_select_element_from_list(w, nextItemToSelect);
            break;
        }
        case WIDX_BUTTON_ROTATE:
            window_tile_inspector_rotate_element(windowTileInspectorSelectedIndex);
            break;
        case WIDX_BUTTON_SORT:
            window_tile_inspector_sort_elements();
            break;
        case WIDX_BUTTON_COPY:
            window_tile_inspector_copy_element(w);
            break;
        case WIDX_BUTTON_PASTE:
            window_tile_inspector_paste_element(w);
            break;
        case WIDX_BUTTON_MOVE_UP:
            window_tile_inspector_swap_elements(windowTileInspectorSelectedIndex, windowTileInspectorSelectedIndex + 1);
            break;
        case WIDX_BUTTON_MOVE_DOWN:
            window_tile_inspector_swap_elements(windowTileInspectorSelectedIndex - 1, windowTileInspectorSelectedIndex);
            break;
    }

    // Only element-specific widgets from now on
    if (w->tileInspectorPage == TileInspectorPage::Default || windowTileInspectorSelectedIndex == -1)
    {
        return;
    }

    TileElement* const tileElement = window_tile_inspector_get_selected_element(w);

    // Update selection, can be nullptr.
    OpenRCT2::TileInspector::SetSelectedElement(tileElement);

    if (tileElement == nullptr)
        return;

    // Page widgets
    switch (tileElement->GetType())
    {
        case TILE_ELEMENT_TYPE_SURFACE:
            switch (widgetIndex)
            {
                case WIDX_SURFACE_BUTTON_REMOVE_FENCES:
                    window_tile_inspector_surface_show_park_fences(false);
                    break;
                case WIDX_SURFACE_BUTTON_RESTORE_FENCES:
                    window_tile_inspector_surface_show_park_fences(true);
                    break;
                case WIDX_SURFACE_CHECK_CORNER_N:
                case WIDX_SURFACE_CHECK_CORNER_E:
                case WIDX_SURFACE_CHECK_CORNER_S:
                case WIDX_SURFACE_CHECK_CORNER_W:
                    window_tile_inspector_surface_toggle_corner(
                        ((widgetIndex - WIDX_SURFACE_CHECK_CORNER_N) + 2 - get_current_rotation()) & 3);
                    break;
                case WIDX_SURFACE_CHECK_DIAGONAL:
                    window_tile_inspector_surface_toggle_diagonal();
                    break;
            } // switch widgetindex
            break;

        case TILE_ELEMENT_TYPE_PATH:
            switch (widgetIndex)
            {
                case WIDX_PATH_CHECK_SLOPED:
                    window_tile_inspector_path_set_sloped(windowTileInspectorSelectedIndex, !tileElement->AsPath()->IsSloped());
                    break;
                case WIDX_PATH_CHECK_BROKEN:
                    window_tile_inspector_path_set_broken(windowTileInspectorSelectedIndex, !tileElement->AsPath()->IsBroken());
                    break;
                case WIDX_PATH_CHECK_EDGE_E:
                case WIDX_PATH_CHECK_EDGE_S:
                case WIDX_PATH_CHECK_EDGE_W:
                case WIDX_PATH_CHECK_EDGE_N:
                {
                    // 0 = east/right, 1 = south/bottom, 2 = west/left, 3 = north/top
                    const int32_t eswn = (widgetIndex - WIDX_PATH_CHECK_EDGE_E) / 2;
                    // Transform to world orientation
                    const int32_t index = (eswn - get_current_rotation()) & 3;
                    window_tile_inspector_path_toggle_edge(
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
                    const int32_t index = (neseswnw - get_current_rotation()) & 3;
                    window_tile_inspector_path_toggle_edge(windowTileInspectorSelectedIndex, index);
                    break;
                }
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_TRACK:
            switch (widgetIndex)
            {
                case WIDX_TRACK_CHECK_APPLY_TO_ALL:
                    windowTileInspectorApplyToAll ^= 1;
                    widget_invalidate(w, widgetIndex);
                    break;
                case WIDX_TRACK_CHECK_CHAIN_LIFT:
                {
                    bool entireTrackBlock = WidgetIsPressed(w, WIDX_TRACK_CHECK_APPLY_TO_ALL);
                    bool newLift = !tileElement->AsTrack()->HasChain();
                    window_tile_inspector_track_block_set_lift(windowTileInspectorSelectedIndex, entireTrackBlock, newLift);
                    break;
                }
                case WIDX_TRACK_CHECK_BLOCK_BRAKE_CLOSED:
                    window_tile_inspector_track_set_block_brake(
                        windowTileInspectorSelectedIndex, !tileElement->AsTrack()->BlockBrakeClosed());
                    break;
                case WIDX_TRACK_CHECK_IS_INDESTRUCTIBLE:
                    window_tile_inspector_track_set_indestructible(
                        windowTileInspectorSelectedIndex, !tileElement->AsTrack()->IsIndestructible());
                    break;
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_SMALL_SCENERY:
            switch (widgetIndex)
            {
                case WIDX_SCENERY_CHECK_QUARTER_N:
                case WIDX_SCENERY_CHECK_QUARTER_E:
                case WIDX_SCENERY_CHECK_QUARTER_S:
                case WIDX_SCENERY_CHECK_QUARTER_W:
                    window_tile_inspector_quarter_tile_set(
                        windowTileInspectorSelectedIndex, widgetIndex - WIDX_SCENERY_CHECK_QUARTER_N);
                    break;
                case WIDX_SCENERY_CHECK_COLLISION_N:
                case WIDX_SCENERY_CHECK_COLLISION_E:
                case WIDX_SCENERY_CHECK_COLLISION_S:
                case WIDX_SCENERY_CHECK_COLLISION_W:
                    window_tile_inspector_toggle_quadrant_collosion(
                        windowTileInspectorSelectedIndex, widgetIndex - WIDX_SCENERY_CHECK_COLLISION_N);
                    break;
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_ENTRANCE:
            switch (widgetIndex)
            {
                case WIDX_ENTRANCE_BUTTON_MAKE_USABLE:
                    window_tile_inspector_entrance_make_usable(windowTileInspectorSelectedIndex);
                    break;
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_BANNER:
            switch (widgetIndex)
            {
                case WIDX_BANNER_CHECK_BLOCK_NE:
                case WIDX_BANNER_CHECK_BLOCK_SE:
                case WIDX_BANNER_CHECK_BLOCK_SW:
                case WIDX_BANNER_CHECK_BLOCK_NW:
                    window_tile_inspector_banner_toggle_block(
                        windowTileInspectorSelectedIndex, widgetIndex - WIDX_BANNER_CHECK_BLOCK_NE);
                    break;
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_CORRUPT:
            switch (widgetIndex)
            {
                case WIDX_CORRUPT_BUTTON_CLAMP:
                    window_tile_inspector_clamp_corrupt(windowTileInspectorSelectedIndex);
                    break;
            } // switch widget index
            break;
        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
        case TILE_ELEMENT_TYPE_WALL:
        default:
            break;
    }
}

static void window_tile_inspector_resize(rct_window* w)
{
    w->min_width = WW;
    w->min_height = MIN_WH;
    if (w->width < w->min_width)
    {
        w->Invalidate();
        w->width = w->min_width;
    }
    if (w->height < w->min_height)
    {
        w->Invalidate();
        w->height = w->min_height;
    }
}

static void window_tile_inspector_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_SPINNER_X_INCREASE:
            windowTileInspectorTile.x = std::min<int32_t>(windowTileInspectorTile.x + 1, MAXIMUM_MAP_SIZE_TECHNICAL - 1);
            windowTileInspectorToolMap.x = std::min<int32_t>(windowTileInspectorToolMap.x + 32, MAXIMUM_TILE_START_XY);
            window_tile_inspector_load_tile(w, nullptr);
            break;
        case WIDX_SPINNER_X_DECREASE:
            windowTileInspectorTile.x = std::max<int32_t>(windowTileInspectorTile.x - 1, 0);
            windowTileInspectorToolMap.x = std::max<int32_t>(windowTileInspectorToolMap.x - 32, 0);
            window_tile_inspector_load_tile(w, nullptr);
            break;
        case WIDX_SPINNER_Y_INCREASE:
            windowTileInspectorTile.y = std::min<int32_t>(windowTileInspectorTile.y + 1, MAXIMUM_MAP_SIZE_TECHNICAL - 1);
            windowTileInspectorToolMap.y = std::min<int32_t>(windowTileInspectorToolMap.y + 32, MAXIMUM_TILE_START_XY);
            window_tile_inspector_load_tile(w, nullptr);
            break;
        case WIDX_SPINNER_Y_DECREASE:
            windowTileInspectorTile.y = std::max<int32_t>(windowTileInspectorTile.y - 1, 0);
            windowTileInspectorToolMap.y = std::max<int32_t>(windowTileInspectorToolMap.y - 32, 0);
            window_tile_inspector_load_tile(w, nullptr);
            break;
    } // switch widget index

    // Only element-specific widgets from now on
    if (w->tileInspectorPage == TileInspectorPage::Default || windowTileInspectorSelectedIndex == -1)
    {
        return;
    }

    const TileElement* tileElement = window_tile_inspector_get_selected_element(w);
    if (tileElement == nullptr)
        return;

    switch (tileElement->GetType())
    {
        case TILE_ELEMENT_TYPE_SURFACE:
            switch (widgetIndex)
            {
                case WIDX_SURFACE_SPINNER_HEIGHT_INCREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, 1);
                    break;
                case WIDX_SURFACE_SPINNER_HEIGHT_DECREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, -1);
                    break;
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_PATH:
            switch (widgetIndex)
            {
                case WIDX_PATH_SPINNER_HEIGHT_INCREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, 1);
                    break;
                case WIDX_PATH_SPINNER_HEIGHT_DECREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, -1);
                    break;
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_TRACK:
            switch (widgetIndex)
            {
                case WIDX_TRACK_SPINNER_HEIGHT_INCREASE:
                    if (WidgetIsPressed(w, WIDX_TRACK_CHECK_APPLY_TO_ALL))
                    {
                        window_tile_inspector_track_block_height_offset(windowTileInspectorSelectedIndex, 1);
                    }
                    else
                    {
                        window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, 1);
                    }
                    break;
                case WIDX_TRACK_SPINNER_HEIGHT_DECREASE:
                    if (WidgetIsPressed(w, WIDX_TRACK_CHECK_APPLY_TO_ALL))
                    {
                        window_tile_inspector_track_block_height_offset(windowTileInspectorSelectedIndex, -1);
                    }
                    else
                    {
                        window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, -1);
                    }
                    break;
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_SMALL_SCENERY:
            switch (widgetIndex)
            {
                case WIDX_SCENERY_SPINNER_HEIGHT_INCREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, 1);
                    break;
                case WIDX_SCENERY_SPINNER_HEIGHT_DECREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, -1);
                    break;
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_ENTRANCE:
            switch (widgetIndex)
            {
                case WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, 1);
                    break;
                case WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, -1);
                    break;
                case WIDX_ENTRANCE_BUTTON_MAKE_USABLE:
                    window_tile_inspector_entrance_make_usable(windowTileInspectorSelectedIndex);
                    break;
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_WALL:
            switch (widgetIndex)
            {
                case WIDX_WALL_SPINNER_HEIGHT_INCREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, 1);
                    break;
                case WIDX_WALL_SPINNER_HEIGHT_DECREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, -1);
                    break;
                case WIDX_WALL_DROPDOWN_SLOPE_BUTTON:
                {
                    // Use dropdown instead of dropdown button
                    widget--;

                    // Fill dropdown list
                    gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItemsFormat[2] = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItemsArgs[0] = STR_TILE_INSPECTOR_WALL_FLAT;
                    gDropdownItemsArgs[1] = STR_TILE_INSPECTOR_WALL_SLOPED_LEFT;
                    gDropdownItemsArgs[2] = STR_TILE_INSPECTOR_WALL_SLOPED_RIGHT;
                    WindowDropdownShowTextCustomWidth(
                        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0,
                        Dropdown::Flag::StayOpen, 3, widget->width() - 3);

                    // Set current value as checked
                    Dropdown::SetChecked(tileElement->AsWall()->GetSlope(), true);
                    break;
                }
                case WIDX_WALL_SPINNER_ANIMATION_FRAME_INCREASE:
                    window_tile_inspector_wall_animation_frame_offset(windowTileInspectorSelectedIndex, 1);
                    break;
                case WIDX_WALL_SPINNER_ANIMATION_FRAME_DECREASE:
                    window_tile_inspector_wall_animation_frame_offset(windowTileInspectorSelectedIndex, -1);
                    break;
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
            switch (widgetIndex)
            {
                case WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, 1);
                    break;
                case WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, -1);
                    break;
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_BANNER:
            switch (widgetIndex)
            {
                case WIDX_BANNER_SPINNER_HEIGHT_INCREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, 1);
                    break;
                case WIDX_BANNER_SPINNER_HEIGHT_DECREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, -1);
                    break;
            } // switch widget index
            break;

        case TILE_ELEMENT_TYPE_CORRUPT:
            switch (widgetIndex)
            {
                case WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, 1);
                    break;
                case WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE:
                    window_tile_inspector_base_height_offset(windowTileInspectorSelectedIndex, -1);
                    break;
            } // switch widget index
        default:
            break;
    }
}

static void window_tile_inspector_update(rct_window* w)
{
    // Check if the mouse is hovering over the list
    if (!WidgetIsHighlighted(w, WIDX_LIST))
    {
        windowTileInspectorHighlightedIndex = -1;
        widget_invalidate(w, WIDX_LIST);
    }

    if (gCurrentToolWidget.window_classification != WC_TILE_INSPECTOR)
        window_close(w);
}

static void window_tile_inspector_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
    {
        return;
    }

    // Get selected element
    TileElement* const tileElement = window_tile_inspector_get_selected_element(w);

    if (w->tileInspectorPage == TileInspectorPage::Wall)
    {
        openrct2_assert(tileElement->GetType() == TILE_ELEMENT_TYPE_WALL, "Element is not a wall");

        if (widgetIndex == WIDX_WALL_DROPDOWN_SLOPE_BUTTON)
        {
            window_tile_inspector_wall_set_slope(windowTileInspectorSelectedIndex, dropdownIndex);
        }
    }
}

static void window_tile_inspector_tool_update(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    map_invalidate_selection_rect();

    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;

    CoordsXY mapCoords;
    TileElement* clickedElement = nullptr;
    bool mouseOnViewport = false;
    if (InputTestPlaceObjectModifier(PLACE_OBJECT_MODIFIER_COPY_Z))
    {
        auto info = get_map_coordinates_from_pos(screenCoords, ViewportInteractionFlags);
        clickedElement = info.Element;
        mapCoords = info.Loc;
    }

    // Even if Ctrl was pressed, fall back to normal selection when there was nothing under the cursor
    if (clickedElement == nullptr)
    {
        auto mouseCoords = screen_pos_to_map_pos(screenCoords, nullptr);
        if (mouseCoords.has_value())
        {
            mouseOnViewport = true;
            mapCoords = mouseCoords.value();
        }
    }

    if (mouseOnViewport)
    {
        gMapSelectPositionA = gMapSelectPositionB = mapCoords;
    }
    else if (windowTileInspectorTileSelected)
    {
        gMapSelectPositionA = gMapSelectPositionB = windowTileInspectorToolMap;
    }
    else
    {
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    }

    gMapSelectType = MAP_SELECT_TYPE_FULL;
    map_invalidate_selection_rect();
}

static void window_tile_inspector_update_selected_tile(rct_window* w, const ScreenCoordsXY& screenCoords)
{
    const bool ctrlIsHeldDown = InputTestPlaceObjectModifier(PLACE_OBJECT_MODIFIER_COPY_Z);

    // Mouse hasn't moved
    if (screenCoords.x == windowTileInspectorToolMouseX && screenCoords.y == windowTileInspectorToolMouseY
        && windowTileInspectorToolCtrlDown == ctrlIsHeldDown)
    {
        return;
    }

    windowTileInspectorToolMouseX = screenCoords.x;
    windowTileInspectorToolMouseY = screenCoords.y;
    windowTileInspectorToolCtrlDown = ctrlIsHeldDown;

    CoordsXY mapCoords{};
    TileElement* clickedElement = nullptr;
    if (ctrlIsHeldDown)
    {
        auto info = get_map_coordinates_from_pos(screenCoords, ViewportInteractionFlags);
        clickedElement = info.Element;
        mapCoords = info.Loc;
    }

    // Even if Ctrl was pressed, fall back to normal selection when there was nothing under the cursor
    if (clickedElement == nullptr)
    {
        auto mouseCoords = screen_pos_to_map_pos(screenCoords, nullptr);

        if (!mouseCoords.has_value())
        {
            return;
        }

        mapCoords = mouseCoords.value();
        // Tile is already selected
        if (windowTileInspectorTileSelected && mapCoords.x == windowTileInspectorToolMap.x
            && mapCoords.y == windowTileInspectorToolMap.y)
        {
            return;
        }
    }

    windowTileInspectorTileSelected = true;
    windowTileInspectorToolMap = mapCoords;
    windowTileInspectorTile = TileCoordsXY(mapCoords);

    OpenRCT2::TileInspector::SetSelectedElement(clickedElement);

    window_tile_inspector_load_tile(w, clickedElement);
}

static void window_tile_inspector_tool_down(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    window_tile_inspector_update_selected_tile(w, screenCoords);
}

static void window_tile_inspector_tool_drag(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    window_tile_inspector_update_selected_tile(w, screenCoords);
}

static void window_tile_inspector_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *width = WW - 30;
    *height = windowTileInspectorElementCount * SCROLLABLE_ROW_HEIGHT;
}

static void window_tile_inspector_set_page(rct_window* w, const TileInspectorPage page)
{
    // Invalidate the window already, because the size may change
    w->Invalidate();

    // subtract current page height, then add new page height
    if (w->tileInspectorPage != TileInspectorPage::Default)
    {
        auto index = EnumValue(w->tileInspectorPage) - 1;
        w->height -= PageGroupBoxSettings[index].details_top_offset - GROUPBOX_PADDING - 3;
        w->min_height -= PageGroupBoxSettings[index].details_top_offset - GROUPBOX_PADDING - 3;
    }
    if (page != TileInspectorPage::Default)
    {
        auto index = EnumValue(page) - 1;
        w->height += PageGroupBoxSettings[index].details_top_offset - GROUPBOX_PADDING - 3;
        w->min_height += PageGroupBoxSettings[index].details_top_offset - GROUPBOX_PADDING - 3;
    }
    w->tileInspectorPage = page;
    auto pageIndex = EnumValue(page);
    w->widgets = PageWidgets[pageIndex];
    w->enabled_widgets = PageEnabledWidgets[pageIndex];
    w->hold_down_widgets = PageHoldDownWidgets[pageIndex];
    w->disabled_widgets = PageDisabledWidgets[pageIndex];
    w->pressed_widgets = 0;
}

static void window_tile_inspector_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    // Because the list items are displayed in reverse order, subtract the calculated index from the amount of elements
    const int16_t index = windowTileInspectorElementCount - (screenCoords.y - 1) / SCROLLABLE_ROW_HEIGHT - 1;
    window_tile_inspector_select_element_from_list(w, index);
}

static void window_tile_inspector_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int16_t index = windowTileInspectorElementCount - (screenCoords.y - 1) / SCROLLABLE_ROW_HEIGHT - 1;
    if (index < 0 || index >= windowTileInspectorElementCount)
        windowTileInspectorHighlightedIndex = -1;
    else
        windowTileInspectorHighlightedIndex = index;

    widget_invalidate(w, WIDX_LIST);
}

static void window_tile_inspector_invalidate(rct_window* w)
{
    // Set the correct page automatically
    TileInspectorPage page = TileInspectorPage::Default;
    if (windowTileInspectorSelectedIndex != -1)
    {
        const auto element = window_tile_inspector_get_selected_element(w);
        auto type = element->GetType();
        switch (type)
        {
            case TILE_ELEMENT_TYPE_SURFACE:
                page = TileInspectorPage::Surface;
                break;
            case TILE_ELEMENT_TYPE_PATH:
                page = TileInspectorPage::Path;
                break;
            case TILE_ELEMENT_TYPE_TRACK:
                page = TileInspectorPage::Track;
                break;
            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                page = TileInspectorPage::Scenery;
                break;
            case TILE_ELEMENT_TYPE_ENTRANCE:
                page = TileInspectorPage::Entrance;
                break;
            case TILE_ELEMENT_TYPE_WALL:
                page = TileInspectorPage::Wall;
                break;
            case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                page = TileInspectorPage::LargeScenery;
                break;
            case TILE_ELEMENT_TYPE_BANNER:
                page = TileInspectorPage::Banner;
                break;
            case TILE_ELEMENT_TYPE_CORRUPT:
            default:
                page = TileInspectorPage::Corrupt;
                break;
        }
    }

    if (w->tileInspectorPage != page)
    {
        window_tile_inspector_set_page(w, page);
        w->Invalidate();
    }

    // X and Y spinners
    WidgetSetEnabled(
        w, WIDX_SPINNER_X_INCREASE,
        (windowTileInspectorTileSelected && ((windowTileInspectorToolMap.x / 32) < MAXIMUM_MAP_SIZE_TECHNICAL - 1)));
    WidgetSetEnabled(
        w, WIDX_SPINNER_X_DECREASE, (windowTileInspectorTileSelected && ((windowTileInspectorToolMap.x / 32) > 0)));
    WidgetSetEnabled(
        w, WIDX_SPINNER_Y_INCREASE,
        (windowTileInspectorTileSelected && ((windowTileInspectorToolMap.y / 32) < MAXIMUM_MAP_SIZE_TECHNICAL - 1)));
    WidgetSetEnabled(
        w, WIDX_SPINNER_Y_DECREASE, (windowTileInspectorTileSelected && ((windowTileInspectorToolMap.y / 32) > 0)));

    // Sort buttons
    WidgetSetEnabled(w, WIDX_BUTTON_SORT, (windowTileInspectorTileSelected && windowTileInspectorElementCount > 1));

    // Move Up button
    WidgetSetEnabled(
        w, WIDX_BUTTON_MOVE_UP,
        (windowTileInspectorSelectedIndex != -1 && windowTileInspectorSelectedIndex < windowTileInspectorElementCount - 1));
    widget_invalidate(w, WIDX_BUTTON_MOVE_UP);

    // Move Down button
    WidgetSetEnabled(w, WIDX_BUTTON_MOVE_DOWN, (windowTileInspectorSelectedIndex > 0));
    widget_invalidate(w, WIDX_BUTTON_MOVE_DOWN);

    // Copy button
    WidgetSetEnabled(w, WIDX_BUTTON_COPY, windowTileInspectorSelectedIndex >= 0);
    widget_invalidate(w, WIDX_BUTTON_COPY);

    // Paste button
    WidgetSetEnabled(w, WIDX_BUTTON_PASTE, windowTileInspectorTileSelected && windowTileInspectorElementCopied);
    widget_invalidate(w, WIDX_BUTTON_PASTE);

    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;

    if (w->tileInspectorPage == TileInspectorPage::Default)
    {
        w->widgets[WIDX_GROUPBOX_DETAILS].type = WindowWidgetType::Empty;
        w->widgets[WIDX_GROUPBOX_PROPERTIES].type = WindowWidgetType::Empty;
        w->widgets[WIDX_LIST].bottom = w->height - PADDING_BOTTOM;
    }
    else
    {
        w->widgets[WIDX_GROUPBOX_DETAILS].type = WindowWidgetType::Groupbox;
        w->widgets[WIDX_GROUPBOX_PROPERTIES].type = WindowWidgetType::Groupbox;
        auto pageIndex = EnumValue(w->tileInspectorPage) - 1;
        w->widgets[WIDX_GROUPBOX_DETAILS].text = PageGroupBoxSettings[pageIndex].string_id;
        w->widgets[WIDX_GROUPBOX_DETAILS].top = w->height - PageGroupBoxSettings[pageIndex].details_top_offset;
        w->widgets[WIDX_GROUPBOX_DETAILS].bottom = w->height - PageGroupBoxSettings[pageIndex].details_bottom_offset;
        w->widgets[WIDX_GROUPBOX_PROPERTIES].top = w->height - PageGroupBoxSettings[pageIndex].properties_top_offset;
        w->widgets[WIDX_GROUPBOX_PROPERTIES].bottom = w->height - PageGroupBoxSettings[pageIndex].properties_bottom_offset;
        w->widgets[WIDX_LIST].bottom = w->widgets[WIDX_GROUPBOX_DETAILS].top - GROUPBOX_PADDING;
    }

    // The default page doesn't need further invalidation
    if (w->tileInspectorPage == TileInspectorPage::Default)
    {
        return;
    }

    // Using a switch, because I don't think giving each page their own callbacks is
    // needed here, as only the mouseup and invalidate functions are different.
    const int32_t propertiesAnchor = w->widgets[WIDX_GROUPBOX_PROPERTIES].top;
    const TileElement* const tileElement = window_tile_inspector_get_selected_element(w);
    if (tileElement == nullptr)
        return;

    switch (tileElement->GetType())
    {
        case TILE_ELEMENT_TYPE_SURFACE:
            w->widgets[WIDX_SURFACE_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
            w->widgets[WIDX_SURFACE_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
            w->widgets[WIDX_SURFACE_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_SURFACE_SPINNER_HEIGHT_INCREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_SURFACE_SPINNER_HEIGHT_DECREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_SURFACE_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_SURFACE_BUTTON_REMOVE_FENCES].top = GBBT(propertiesAnchor, 1);
            w->widgets[WIDX_SURFACE_BUTTON_REMOVE_FENCES].bottom = GBBB(propertiesAnchor, 1);
            w->widgets[WIDX_SURFACE_BUTTON_RESTORE_FENCES].top = GBBT(propertiesAnchor, 1);
            w->widgets[WIDX_SURFACE_BUTTON_RESTORE_FENCES].bottom = GBBB(propertiesAnchor, 1);
            w->widgets[WIDX_SURFACE_CHECK_CORNER_N].top = GBBT(propertiesAnchor, 2) + 7 * 0;
            w->widgets[WIDX_SURFACE_CHECK_CORNER_N].bottom = w->widgets[WIDX_SURFACE_CHECK_CORNER_N].top + 13;
            w->widgets[WIDX_SURFACE_CHECK_CORNER_E].top = GBBT(propertiesAnchor, 2) + 7 * 1;
            w->widgets[WIDX_SURFACE_CHECK_CORNER_E].bottom = w->widgets[WIDX_SURFACE_CHECK_CORNER_E].top + 13;
            w->widgets[WIDX_SURFACE_CHECK_CORNER_S].top = GBBT(propertiesAnchor, 2) + 7 * 2;
            w->widgets[WIDX_SURFACE_CHECK_CORNER_S].bottom = w->widgets[WIDX_SURFACE_CHECK_CORNER_S].top + 13;
            w->widgets[WIDX_SURFACE_CHECK_CORNER_W].top = GBBT(propertiesAnchor, 2) + 7 * 1;
            w->widgets[WIDX_SURFACE_CHECK_CORNER_W].bottom = w->widgets[WIDX_SURFACE_CHECK_CORNER_W].top + 13;
            w->widgets[WIDX_SURFACE_CHECK_DIAGONAL].top = GBBT(propertiesAnchor, 3) + 7 * 1;
            w->widgets[WIDX_SURFACE_CHECK_DIAGONAL].bottom = w->widgets[WIDX_SURFACE_CHECK_DIAGONAL].top + 13;
            WidgetSetCheckboxValue(
                w, WIDX_SURFACE_CHECK_CORNER_N,
                tileElement->AsSurface()->GetSlope() & (1 << ((2 - get_current_rotation()) & 3)));
            WidgetSetCheckboxValue(
                w, WIDX_SURFACE_CHECK_CORNER_E,
                tileElement->AsSurface()->GetSlope() & (1 << ((3 - get_current_rotation()) & 3)));
            WidgetSetCheckboxValue(
                w, WIDX_SURFACE_CHECK_CORNER_S,
                tileElement->AsSurface()->GetSlope() & (1 << ((0 - get_current_rotation()) & 3)));
            WidgetSetCheckboxValue(
                w, WIDX_SURFACE_CHECK_CORNER_W,
                tileElement->AsSurface()->GetSlope() & (1 << ((1 - get_current_rotation()) & 3)));
            WidgetSetCheckboxValue(
                w, WIDX_SURFACE_CHECK_DIAGONAL, tileElement->AsSurface()->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT);
            break;
        case TILE_ELEMENT_TYPE_PATH:
            w->widgets[WIDX_PATH_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
            w->widgets[WIDX_PATH_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
            w->widgets[WIDX_PATH_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_PATH_SPINNER_HEIGHT_INCREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_PATH_SPINNER_HEIGHT_DECREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_PATH_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_PATH_CHECK_BROKEN].top = GBBT(propertiesAnchor, 1);
            w->widgets[WIDX_PATH_CHECK_BROKEN].bottom = GBBB(propertiesAnchor, 1);
            w->widgets[WIDX_PATH_CHECK_SLOPED].top = GBBT(propertiesAnchor, 2);
            w->widgets[WIDX_PATH_CHECK_SLOPED].bottom = GBBB(propertiesAnchor, 2);
            w->widgets[WIDX_PATH_CHECK_EDGE_N].top = GBBT(propertiesAnchor, 3) + 7 * 0;
            w->widgets[WIDX_PATH_CHECK_EDGE_N].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_N].top + 13;
            w->widgets[WIDX_PATH_CHECK_EDGE_NE].top = GBBT(propertiesAnchor, 3) + 7 * 1;
            w->widgets[WIDX_PATH_CHECK_EDGE_NE].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_NE].top + 13;
            w->widgets[WIDX_PATH_CHECK_EDGE_E].top = GBBT(propertiesAnchor, 3) + 7 * 2;
            w->widgets[WIDX_PATH_CHECK_EDGE_E].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_E].top + 13;
            w->widgets[WIDX_PATH_CHECK_EDGE_SE].top = GBBT(propertiesAnchor, 3) + 7 * 3;
            w->widgets[WIDX_PATH_CHECK_EDGE_SE].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_SE].top + 13;
            w->widgets[WIDX_PATH_CHECK_EDGE_S].top = GBBT(propertiesAnchor, 3) + 7 * 4;
            w->widgets[WIDX_PATH_CHECK_EDGE_S].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_S].top + 13;
            w->widgets[WIDX_PATH_CHECK_EDGE_SW].top = GBBT(propertiesAnchor, 3) + 7 * 3;
            w->widgets[WIDX_PATH_CHECK_EDGE_SW].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_SW].top + 13;
            w->widgets[WIDX_PATH_CHECK_EDGE_W].top = GBBT(propertiesAnchor, 3) + 7 * 2;
            w->widgets[WIDX_PATH_CHECK_EDGE_W].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_W].top + 13;
            w->widgets[WIDX_PATH_CHECK_EDGE_NW].top = GBBT(propertiesAnchor, 3) + 7 * 1;
            w->widgets[WIDX_PATH_CHECK_EDGE_NW].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_NW].top + 13;
            WidgetSetCheckboxValue(w, WIDX_PATH_CHECK_SLOPED, tileElement->AsPath()->IsSloped());
            WidgetSetCheckboxValue(w, WIDX_PATH_CHECK_BROKEN, tileElement->AsPath()->IsBroken());
            WidgetSetCheckboxValue(
                w, WIDX_PATH_CHECK_EDGE_NE, tileElement->AsPath()->GetEdges() & (1 << ((0 - get_current_rotation()) & 3)));
            WidgetSetCheckboxValue(
                w, WIDX_PATH_CHECK_EDGE_SE, tileElement->AsPath()->GetEdges() & (1 << ((1 - get_current_rotation()) & 3)));
            WidgetSetCheckboxValue(
                w, WIDX_PATH_CHECK_EDGE_SW, tileElement->AsPath()->GetEdges() & (1 << ((2 - get_current_rotation()) & 3)));
            WidgetSetCheckboxValue(
                w, WIDX_PATH_CHECK_EDGE_NW, tileElement->AsPath()->GetEdges() & (1 << ((3 - get_current_rotation()) & 3)));
            WidgetSetCheckboxValue(
                w, WIDX_PATH_CHECK_EDGE_E, tileElement->AsPath()->GetCorners() & (1 << ((0 - get_current_rotation()) & 3)));
            WidgetSetCheckboxValue(
                w, WIDX_PATH_CHECK_EDGE_S, tileElement->AsPath()->GetCorners() & (1 << ((1 - get_current_rotation()) & 3)));
            WidgetSetCheckboxValue(
                w, WIDX_PATH_CHECK_EDGE_W, tileElement->AsPath()->GetCorners() & (1 << ((2 - get_current_rotation()) & 3)));
            WidgetSetCheckboxValue(
                w, WIDX_PATH_CHECK_EDGE_N, tileElement->AsPath()->GetCorners() & (1 << ((3 - get_current_rotation()) & 3)));
            break;
        case TILE_ELEMENT_TYPE_TRACK:
            w->widgets[WIDX_TRACK_CHECK_APPLY_TO_ALL].top = GBBT(propertiesAnchor, 0);
            w->widgets[WIDX_TRACK_CHECK_APPLY_TO_ALL].bottom = GBBB(propertiesAnchor, 0);
            w->widgets[WIDX_TRACK_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 1) + 3;
            w->widgets[WIDX_TRACK_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 1) - 3;
            w->widgets[WIDX_TRACK_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 1) + 4;
            w->widgets[WIDX_TRACK_SPINNER_HEIGHT_INCREASE].bottom = GBBB(propertiesAnchor, 1) - 4;
            w->widgets[WIDX_TRACK_SPINNER_HEIGHT_DECREASE].top = GBBT(propertiesAnchor, 1) + 4;
            w->widgets[WIDX_TRACK_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 1) - 4;
            w->widgets[WIDX_TRACK_CHECK_CHAIN_LIFT].top = GBBT(propertiesAnchor, 2);
            w->widgets[WIDX_TRACK_CHECK_CHAIN_LIFT].bottom = GBBB(propertiesAnchor, 2);
            w->widgets[WIDX_TRACK_CHECK_BLOCK_BRAKE_CLOSED].top = GBBT(propertiesAnchor, 3);
            w->widgets[WIDX_TRACK_CHECK_BLOCK_BRAKE_CLOSED].bottom = GBBB(propertiesAnchor, 3);
            w->widgets[WIDX_TRACK_CHECK_IS_INDESTRUCTIBLE].top = GBBT(propertiesAnchor, 4);
            w->widgets[WIDX_TRACK_CHECK_IS_INDESTRUCTIBLE].bottom = GBBB(propertiesAnchor, 4);
            WidgetSetCheckboxValue(w, WIDX_TRACK_CHECK_APPLY_TO_ALL, windowTileInspectorApplyToAll);
            WidgetSetCheckboxValue(w, WIDX_TRACK_CHECK_CHAIN_LIFT, tileElement->AsTrack()->HasChain());
            WidgetSetCheckboxValue(w, WIDX_TRACK_CHECK_BLOCK_BRAKE_CLOSED, tileElement->AsTrack()->BlockBrakeClosed());
            WidgetSetCheckboxValue(w, WIDX_TRACK_CHECK_IS_INDESTRUCTIBLE, tileElement->AsTrack()->IsIndestructible());
            break;
        case TILE_ELEMENT_TYPE_SMALL_SCENERY:
        {
            // Raise / Lower
            w->widgets[WIDX_SCENERY_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
            w->widgets[WIDX_SCENERY_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
            w->widgets[WIDX_SCENERY_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_SCENERY_SPINNER_HEIGHT_INCREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_SCENERY_SPINNER_HEIGHT_DECREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_SCENERY_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;

            // Quadrant checkboxes
            w->widgets[WIDX_SCENERY_CHECK_QUARTER_N].top = GBBT(propertiesAnchor, 1) - 5 + 7 * 0;
            w->widgets[WIDX_SCENERY_CHECK_QUARTER_N].bottom = w->widgets[WIDX_SCENERY_CHECK_QUARTER_N].top + 13;
            w->widgets[WIDX_SCENERY_CHECK_QUARTER_E].top = GBBT(propertiesAnchor, 1) - 5 + 7 * 1;
            w->widgets[WIDX_SCENERY_CHECK_QUARTER_E].bottom = w->widgets[WIDX_SCENERY_CHECK_QUARTER_E].top + 13;
            w->widgets[WIDX_SCENERY_CHECK_QUARTER_S].top = GBBT(propertiesAnchor, 1) - 5 + 7 * 2;
            w->widgets[WIDX_SCENERY_CHECK_QUARTER_S].bottom = w->widgets[WIDX_SCENERY_CHECK_QUARTER_S].top + 13;
            w->widgets[WIDX_SCENERY_CHECK_QUARTER_W].top = GBBT(propertiesAnchor, 1) - 5 + 7 * 1;
            w->widgets[WIDX_SCENERY_CHECK_QUARTER_W].bottom = w->widgets[WIDX_SCENERY_CHECK_QUARTER_W].top + 13;
            // This gets the relative rotation, by subtracting the camera's rotation, and wrapping it between 0-3 inclusive
            bool N = tileElement->AsSmallScenery()->GetSceneryQuadrant() == ((0 - get_current_rotation()) & 3);
            bool E = tileElement->AsSmallScenery()->GetSceneryQuadrant() == ((1 - get_current_rotation()) & 3);
            bool S = tileElement->AsSmallScenery()->GetSceneryQuadrant() == ((2 - get_current_rotation()) & 3);
            bool W = tileElement->AsSmallScenery()->GetSceneryQuadrant() == ((3 - get_current_rotation()) & 3);
            WidgetSetCheckboxValue(w, WIDX_SCENERY_CHECK_QUARTER_N, N);
            WidgetSetCheckboxValue(w, WIDX_SCENERY_CHECK_QUARTER_E, E);
            WidgetSetCheckboxValue(w, WIDX_SCENERY_CHECK_QUARTER_S, S);
            WidgetSetCheckboxValue(w, WIDX_SCENERY_CHECK_QUARTER_W, W);

            // Collision checkboxes
            w->widgets[WIDX_SCENERY_CHECK_COLLISION_N].top = GBBT(propertiesAnchor, 2) + 5 + 7 * 0;
            w->widgets[WIDX_SCENERY_CHECK_COLLISION_N].bottom = w->widgets[WIDX_SCENERY_CHECK_COLLISION_N].top + 13;
            w->widgets[WIDX_SCENERY_CHECK_COLLISION_E].top = GBBT(propertiesAnchor, 2) + 5 + 7 * 1;
            w->widgets[WIDX_SCENERY_CHECK_COLLISION_E].bottom = w->widgets[WIDX_SCENERY_CHECK_COLLISION_E].top + 13;
            w->widgets[WIDX_SCENERY_CHECK_COLLISION_S].top = GBBT(propertiesAnchor, 2) + 5 + 7 * 2;
            w->widgets[WIDX_SCENERY_CHECK_COLLISION_S].bottom = w->widgets[WIDX_SCENERY_CHECK_COLLISION_S].top + 13;
            w->widgets[WIDX_SCENERY_CHECK_COLLISION_W].top = GBBT(propertiesAnchor, 2) + 5 + 7 * 1;
            w->widgets[WIDX_SCENERY_CHECK_COLLISION_W].bottom = w->widgets[WIDX_SCENERY_CHECK_COLLISION_W].top + 13;
            auto occupiedQuadrants = tileElement->GetOccupiedQuadrants();
            N = (occupiedQuadrants & (1 << ((2 - get_current_rotation()) & 3))) != 0;
            E = (occupiedQuadrants & (1 << ((3 - get_current_rotation()) & 3))) != 0;
            S = (occupiedQuadrants & (1 << ((0 - get_current_rotation()) & 3))) != 0;
            W = (occupiedQuadrants & (1 << ((1 - get_current_rotation()) & 3))) != 0;
            WidgetSetCheckboxValue(w, WIDX_SCENERY_CHECK_COLLISION_N, N);
            WidgetSetCheckboxValue(w, WIDX_SCENERY_CHECK_COLLISION_E, E);
            WidgetSetCheckboxValue(w, WIDX_SCENERY_CHECK_COLLISION_S, S);
            WidgetSetCheckboxValue(w, WIDX_SCENERY_CHECK_COLLISION_W, W);
            break;
        }
        case TILE_ELEMENT_TYPE_ENTRANCE:
            w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
            w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
            w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_ENTRANCE_BUTTON_MAKE_USABLE].top = GBBT(propertiesAnchor, 1);
            w->widgets[WIDX_ENTRANCE_BUTTON_MAKE_USABLE].bottom = GBBB(propertiesAnchor, 1);
            WidgetSetEnabled(
                w, WIDX_ENTRANCE_BUTTON_MAKE_USABLE,
                tileElement->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE);
            break;
        case TILE_ELEMENT_TYPE_WALL:
        {
            bool canBeSloped = false;
            bool hasAnimation = false;
            const auto wallEntry = tileElement->AsWall()->GetEntry();
            if (wallEntry != nullptr)
            {
                canBeSloped = !(wallEntry->flags & WALL_SCENERY_CANT_BUILD_ON_SLOPE);
                hasAnimation = wallEntry->flags & WALL_SCENERY_IS_DOOR;
            }

            w->widgets[WIDX_WALL_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
            w->widgets[WIDX_WALL_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
            w->widgets[WIDX_WALL_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_WALL_SPINNER_HEIGHT_INCREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_WALL_SPINNER_HEIGHT_DECREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_WALL_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_WALL_DROPDOWN_SLOPE].top = GBBT(propertiesAnchor, 1) + 3;
            w->widgets[WIDX_WALL_DROPDOWN_SLOPE].bottom = GBBB(propertiesAnchor, 1) - 3;
            w->widgets[WIDX_WALL_DROPDOWN_SLOPE].text = WallSlopeStringIds[tileElement->AsWall()->GetSlope()];
            w->widgets[WIDX_WALL_DROPDOWN_SLOPE_BUTTON].top = GBBT(propertiesAnchor, 1) + 4;
            w->widgets[WIDX_WALL_DROPDOWN_SLOPE_BUTTON].bottom = GBBB(propertiesAnchor, 1) - 4;
            w->widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME].top = GBBT(propertiesAnchor, 2) + 3;
            w->widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME].bottom = GBBB(propertiesAnchor, 2) - 3;
            w->widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME_INCREASE].top = GBBT(propertiesAnchor, 2) + 4;
            w->widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME_INCREASE].bottom = GBBB(propertiesAnchor, 2) - 4;
            w->widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME_DECREASE].top = GBBT(propertiesAnchor, 2) + 4;
            w->widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME_DECREASE].bottom = GBBB(propertiesAnchor, 2) - 4;

            // Wall slope dropdown
            WidgetSetEnabled(w, WIDX_WALL_DROPDOWN_SLOPE, canBeSloped);
            widget_invalidate(w, WIDX_WALL_DROPDOWN_SLOPE);
            WidgetSetEnabled(w, WIDX_WALL_DROPDOWN_SLOPE_BUTTON, canBeSloped);
            widget_invalidate(w, WIDX_WALL_DROPDOWN_SLOPE_BUTTON);
            // Wall animation frame spinner
            WidgetSetEnabled(w, WIDX_WALL_SPINNER_ANIMATION_FRAME, hasAnimation);
            WidgetSetEnabled(w, WIDX_WALL_SPINNER_ANIMATION_FRAME_INCREASE, hasAnimation);
            WidgetSetEnabled(w, WIDX_WALL_SPINNER_ANIMATION_FRAME_DECREASE, hasAnimation);
            break;
        }
        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
            w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
            w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
            w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            break;
        case TILE_ELEMENT_TYPE_BANNER:
            w->widgets[WIDX_BANNER_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
            w->widgets[WIDX_BANNER_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
            w->widgets[WIDX_BANNER_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_BANNER_SPINNER_HEIGHT_INCREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_BANNER_SPINNER_HEIGHT_DECREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_BANNER_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_BANNER_CHECK_BLOCK_NE].top = GBBT(propertiesAnchor, 1);
            w->widgets[WIDX_BANNER_CHECK_BLOCK_NE].bottom = GBBB(propertiesAnchor, 1);
            w->widgets[WIDX_BANNER_CHECK_BLOCK_SE].top = GBBT(propertiesAnchor, 2);
            w->widgets[WIDX_BANNER_CHECK_BLOCK_SE].bottom = GBBB(propertiesAnchor, 2);
            w->widgets[WIDX_BANNER_CHECK_BLOCK_SW].top = GBBT(propertiesAnchor, 2);
            w->widgets[WIDX_BANNER_CHECK_BLOCK_SW].bottom = GBBB(propertiesAnchor, 2);
            w->widgets[WIDX_BANNER_CHECK_BLOCK_NW].top = GBBT(propertiesAnchor, 1);
            w->widgets[WIDX_BANNER_CHECK_BLOCK_NW].bottom = GBBB(propertiesAnchor, 1);
            WidgetSetCheckboxValue(
                w, WIDX_BANNER_CHECK_BLOCK_NE,
                !(tileElement->AsBanner()->GetAllowedEdges() & (1 << ((0 - get_current_rotation()) & 3))));
            WidgetSetCheckboxValue(
                w, WIDX_BANNER_CHECK_BLOCK_SE,
                !(tileElement->AsBanner()->GetAllowedEdges() & (1 << ((1 - get_current_rotation()) & 3))));
            WidgetSetCheckboxValue(
                w, WIDX_BANNER_CHECK_BLOCK_SW,
                !(tileElement->AsBanner()->GetAllowedEdges() & (1 << ((2 - get_current_rotation()) & 3))));
            WidgetSetCheckboxValue(
                w, WIDX_BANNER_CHECK_BLOCK_NW,
                !(tileElement->AsBanner()->GetAllowedEdges() & (1 << ((3 - get_current_rotation()) & 3))));
            break;
        case TILE_ELEMENT_TYPE_CORRUPT:
            w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
            w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
            w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE].top = GBBT(propertiesAnchor, 0) + 4;
            w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
            w->widgets[WIDX_CORRUPT_BUTTON_CLAMP].top = GBBT(propertiesAnchor, 1);
            w->widgets[WIDX_CORRUPT_BUTTON_CLAMP].bottom = GBBB(propertiesAnchor, 1);
            break;
        default:
            break; // Nothing.
    }
}

static void window_tile_inspector_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    ScreenCoordsXY screenCoords(w->windowPos.x, w->windowPos.y);

    // Draw coordinates
    gfx_draw_string(dpi, screenCoords + ScreenCoordsXY(5, 24), "X:", { COLOUR_WHITE });
    gfx_draw_string(dpi, screenCoords + ScreenCoordsXY(74, 24), "Y:", { COLOUR_WHITE });
    if (windowTileInspectorTileSelected)
    {
        auto tileCoords = TileCoordsXY{ windowTileInspectorToolMap };
        auto ft = Formatter();
        ft.Add<int32_t>(tileCoords.x);
        DrawTextBasic(
            dpi, screenCoords + ScreenCoordsXY{ 43, 24 }, STR_FORMAT_INTEGER, ft, { COLOUR_WHITE, TextAlignment::RIGHT });
        ft = Formatter();
        ft.Add<int32_t>(tileCoords.y);
        DrawTextBasic(
            dpi, screenCoords + ScreenCoordsXY{ 113, 24 }, STR_FORMAT_INTEGER, ft, { COLOUR_WHITE, TextAlignment::RIGHT });
    }
    else
    {
        gfx_draw_string(dpi, screenCoords + ScreenCoordsXY(43 - 7, 24), "-", { COLOUR_WHITE });
        gfx_draw_string(dpi, screenCoords + ScreenCoordsXY(113 - 7, 24), "-", { COLOUR_WHITE });
    }

    if (windowTileInspectorSelectedIndex != -1)
    {
        // X and Y of first element in detail box
        screenCoords = w->windowPos
            + ScreenCoordsXY{ w->widgets[WIDX_GROUPBOX_DETAILS].left + 7, w->widgets[WIDX_GROUPBOX_DETAILS].top + 14 };

        // Get map element
        TileElement* const tileElement = window_tile_inspector_get_selected_element(w);
        if (tileElement == nullptr)
            return;

        switch (tileElement->GetType())
        {
            case TILE_ELEMENT_TYPE_SURFACE:
            {
                // Details
                // Terrain texture name
                rct_string_id terrainNameId = STR_EMPTY;
                auto surfaceStyle = tileElement->AsSurface()->GetSurfaceStyleObject();
                if (surfaceStyle != nullptr)
                {
                    terrainNameId = surfaceStyle->NameStringId;
                }
                auto ft = Formatter();
                ft.Add<rct_string_id>(terrainNameId);
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_SURFACE_TERAIN, ft, { COLOUR_WHITE });

                // Edge texture name
                rct_string_id terrainEdgeNameId = STR_EMPTY;
                auto edgeStyle = tileElement->AsSurface()->GetEdgeStyleObject();
                if (edgeStyle != nullptr)
                {
                    terrainEdgeNameId = edgeStyle->NameStringId;
                }
                ft = Formatter();
                ft.Add<rct_string_id>(terrainEdgeNameId);
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_SURFACE_EDGE, ft, { COLOUR_WHITE });

                // Land ownership
                rct_string_id landOwnership;
                if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_OWNED)
                    landOwnership = STR_LAND_OWNED;
                else if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_AVAILABLE)
                    landOwnership = STR_LAND_SALE;
                else if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED)
                    landOwnership = STR_CONSTRUCTION_RIGHTS_OWNED;
                else if (tileElement->AsSurface()->GetOwnership() & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE)
                    landOwnership = STR_CONSTRUCTION_RIGHTS_SALE;
                else
                    landOwnership = STR_TILE_INSPECTOR_LAND_NOT_OWNED_AND_NOT_AVAILABLE;
                ft = Formatter();
                ft.Add<rct_string_id>(landOwnership);
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_SURFACE_OWNERSHIP, ft, { COLOUR_WHITE });

                // Water level
                ft = Formatter();
                ft.Add<uint32_t>(tileElement->AsSurface()->GetWaterHeight());
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ 0, 33 }, STR_TILE_INSPECTOR_SURFACE_WATER_LEVEL, ft, { COLOUR_WHITE });

                // Properties
                // Raise / lower label
                screenCoords = w->windowPos
                    + ScreenCoordsXY{ w->widgets[WIDX_GROUPBOX_DETAILS].left + 7, w->widgets[WIDX_SURFACE_SPINNER_HEIGHT].top };
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, {}, { COLOUR_WHITE });

                // Current base height
                screenCoords.x = w->windowPos.x + w->widgets[WIDX_SURFACE_SPINNER_HEIGHT].left + 3;
                ft = Formatter();
                ft.Add<int32_t>(tileElement->base_height);
                DrawTextBasic(dpi, screenCoords, STR_FORMAT_INTEGER, ft, { COLOUR_WHITE });

                // Raised corners
                screenCoords = w->windowPos
                    + ScreenCoordsXY{ w->widgets[WIDX_GROUPBOX_DETAILS].left + 7, w->widgets[WIDX_SURFACE_CHECK_CORNER_E].top };
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_SURFACE_CORNERS, {}, { COLOUR_WHITE });
                break;
            }

            case TILE_ELEMENT_TYPE_PATH:
            {
                // Details
                // Path name
                auto ft = Formatter();
                ft.Add<rct_string_id>(tileElement->AsPath()->GetSurfaceDescriptor()->Name);
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_PATH_NAME, ft, { COLOUR_WHITE });

                // Path addition
                if (tileElement->AsPath()->HasAddition())
                {
                    const auto pathAdditionType = tileElement->AsPath()->GetAdditionEntryIndex();
                    const auto* pathBitEntry = get_footpath_item_entry(pathAdditionType);
                    rct_string_id additionNameId = pathBitEntry != nullptr
                        ? pathBitEntry->name
                        : static_cast<rct_string_id>(STR_UNKNOWN_OBJECT_TYPE);
                    ft = Formatter();
                    ft.Add<rct_string_id>(additionNameId);
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_PATH_ADDITIONS, ft, { COLOUR_WHITE });
                }
                else
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_PATH_ADDITIONS_NONE, {},
                        { COLOUR_WHITE });

                // Properties
                // Raise / lower label
                screenCoords = w->windowPos
                    + ScreenCoordsXY{ w->widgets[WIDX_GROUPBOX_DETAILS].left + 7, w->widgets[WIDX_PATH_SPINNER_HEIGHT].top };
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, {}, { COLOUR_WHITE });

                // Current base height
                screenCoords.x = w->windowPos.x + w->widgets[WIDX_PATH_SPINNER_HEIGHT].left + 3;
                ft = Formatter();
                ft.Add<int32_t>(tileElement->base_height);
                DrawTextBasic(dpi, screenCoords, STR_FORMAT_INTEGER, ft, { COLOUR_WHITE });

                // Path connections
                screenCoords = w->windowPos
                    + ScreenCoordsXY{ w->widgets[WIDX_GROUPBOX_DETAILS].left + 7, w->widgets[WIDX_PATH_CHECK_EDGE_W].top };
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_PATH_CONNECTED_EDGES, {}, { COLOUR_WHITE });
                break;
            }

            case TILE_ELEMENT_TYPE_TRACK:
            {
                auto trackElement = tileElement->AsTrack();
                ride_id_t rideId = trackElement->GetRideIndex();
                auto ride = get_ride(rideId);

                // Ride ID
                auto ft = Formatter();
                ft.Add<int16_t>(rideId);
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_TRACK_RIDE_ID, ft, { COLOUR_WHITE });

                // Ride name
                if (ride != nullptr)
                {
                    ft = Formatter();
                    ride->FormatNameTo(ft);
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_TRACK_RIDE_NAME, ft, { COLOUR_WHITE });
                }

                // Ride type. Individual pieces may be of a different ride type from the ride it belongs to.
                const auto& rtd = GetRideTypeDescriptor(trackElement->GetRideType());
                ft = Formatter();
                ft.Add<rct_string_id>(rtd.Naming.Name);
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_TRACK_RIDE_TYPE, ft, { COLOUR_WHITE });

                // Track
                ft = Formatter();
                ft.Add<track_type_t>(trackElement->GetTrackType());
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ 0, 33 }, STR_TILE_INSPECTOR_TRACK_PIECE_ID, ft, { COLOUR_WHITE });

                ft = Formatter();
                ft.Add<track_type_t>(trackElement->GetSequenceIndex());
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ 0, 44 }, STR_TILE_INSPECTOR_TRACK_SEQUENCE, ft, { COLOUR_WHITE });
                if (trackElement->IsStation())
                {
                    int16_t stationIndex = trackElement->GetStationIndex();
                    ft = Formatter();
                    ft.Add<rct_string_id>(STR_COMMA16);
                    ft.Add<int16_t>(stationIndex);
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 55 }, STR_TILE_INSPECTOR_STATION_INDEX, ft, { COLOUR_WHITE });
                }
                else
                {
                    const char* stationNone = "-";
                    ft = Formatter();
                    ft.Add<rct_string_id>(STR_STRING);
                    ft.Add<char*>(stationNone);
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 55 }, STR_TILE_INSPECTOR_STATION_INDEX, ft, { COLOUR_WHITE });
                }

                ft = Formatter();
                ft.Add<rct_string_id>(ColourSchemeNames[trackElement->GetColourScheme()]);
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ 0, 66 }, STR_TILE_INSPECTOR_COLOUR_SCHEME, ft, { COLOUR_WHITE });

                // Properties
                // Raise / lower label
                screenCoords.y = w->windowPos.y + w->widgets[WIDX_TRACK_SPINNER_HEIGHT].top;
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, {}, { COLOUR_WHITE });

                // Current base height
                screenCoords.x = w->windowPos.x + w->widgets[WIDX_TRACK_SPINNER_HEIGHT].left + 3;
                ft = Formatter();
                ft.Add<int32_t>(tileElement->base_height);
                DrawTextBasic(dpi, screenCoords, STR_FORMAT_INTEGER, ft, { COLOUR_WHITE });
                break;
            }

            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
            {
                // Details
                // Age
                auto ft = Formatter();
                ft.Add<int16_t>(tileElement->AsSmallScenery()->GetAge());
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_SCENERY_AGE, ft, { COLOUR_WHITE });

                // Quadrant value
                const auto* sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
                if (sceneryEntry != nullptr && !(sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE)))
                {
                    int16_t quadrant = tileElement->AsSmallScenery()->GetSceneryQuadrant();
                    static constexpr rct_string_id quadrant_string_idx[] = {
                        STR_TILE_INSPECTOR_SCENERY_QUADRANT_SW,
                        STR_TILE_INSPECTOR_SCENERY_QUADRANT_NW,
                        STR_TILE_INSPECTOR_SCENERY_QUADRANT_NE,
                        STR_TILE_INSPECTOR_SCENERY_QUADRANT_SE,
                    };
                    ft = Formatter();
                    ft.Add<rct_string_id>(quadrant_string_idx[quadrant]);
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_SCENERY_QUADRANT, ft, { COLOUR_WHITE });
                }

                // Scenery ID
                ft = Formatter();
                ft.Add<ObjectEntryIndex>(tileElement->AsSmallScenery()->GetEntryIndex());
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_SCENERY_ENTRY_IDX, ft, { COLOUR_WHITE });

                // Properties
                // Raise / Lower
                screenCoords.y = w->windowPos.y + w->widgets[WIDX_SCENERY_SPINNER_HEIGHT].top;
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, {}, { COLOUR_WHITE });

                // Current base height
                screenCoords.x = w->windowPos.x + w->widgets[WIDX_SCENERY_SPINNER_HEIGHT].left + 3;
                ft = Formatter();
                ft.Add<int32_t>(tileElement->base_height);
                DrawTextBasic(dpi, screenCoords, STR_FORMAT_INTEGER, ft, { COLOUR_WHITE });

                // Quarter tile
                screenCoords = w->windowPos
                    + ScreenCoordsXY{ w->widgets[WIDX_GROUPBOX_DETAILS].left + 7,
                                      w->widgets[WIDX_SCENERY_CHECK_QUARTER_E].top };
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_SCENERY_QUADRANT_LABEL, {}, { COLOUR_WHITE });

                // Collision
                screenCoords.y = w->windowPos.y + w->widgets[WIDX_SCENERY_CHECK_COLLISION_E].top;
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_COLLISSION, {}, { COLOUR_WHITE });
                break;
            }

            case TILE_ELEMENT_TYPE_ENTRANCE:
            {
                // Details
                // Entrance type
                auto ft = Formatter();
                ft.Add<rct_string_id>(EntranceTypeStringIds[tileElement->AsEntrance()->GetEntranceType()]);
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_ENTRANCE_TYPE, ft, { COLOUR_WHITE });

                if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE)
                {
                    // TODO: Make this work with Left/Right park entrance parts
                    ft = Formatter();
                    ft.Add<rct_string_id>(park_entrance_get_index({ windowTileInspectorToolMap, tileElement->GetBaseZ() }));
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_ENTRANCE_ENTRANCE_ID, ft,
                        { COLOUR_WHITE });
                }
                else
                {
                    ft = Formatter();
                    ft.Add<int16_t>(tileElement->AsEntrance()->GetStationIndex());
                    if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_ENTRANCE)
                    {
                        // Ride entrance ID
                        DrawTextBasic(
                            dpi, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_ENTRANCE_ENTRANCE_ID, ft,
                            { COLOUR_WHITE });
                    }
                    else
                    {
                        // Ride exit ID
                        DrawTextBasic(
                            dpi, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_ENTRANCE_EXIT_ID, ft,
                            { COLOUR_WHITE });
                    }
                }

                if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE)
                {
                    // Entrance part
                    ft = Formatter();
                    ft.Add<rct_string_id>(ParkEntrancePartStringIds[tileElement->AsEntrance()->GetSequenceIndex()]);
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_ENTRANCE_PART, ft, { COLOUR_WHITE });
                }
                else
                {
                    // Ride ID
                    ft = Formatter();
                    ft.Add<ride_id_t>(tileElement->AsEntrance()->GetRideIndex());
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_ENTRANCE_RIDE_ID, ft, { COLOUR_WHITE });
                    // Station index
                    int16_t stationIndex = tileElement->AsEntrance()->GetStationIndex();
                    ft = Formatter();
                    ft.Add<rct_string_id>(STR_COMMA16);
                    ft.Add<int16_t>(stationIndex);
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 33 }, STR_TILE_INSPECTOR_STATION_INDEX, ft, { COLOUR_WHITE });
                }

                // Properties
                // Raise / Lower
                screenCoords.y = w->windowPos.y + w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT].top;
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, {}, { COLOUR_WHITE });

                // Current base height
                screenCoords.x = w->windowPos.x + w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT].left + 3;
                ft = Formatter();
                ft.Add<int32_t>(tileElement->base_height);
                DrawTextBasic(dpi, screenCoords, STR_FORMAT_INTEGER, ft, { COLOUR_WHITE });
                break;
            }

            case TILE_ELEMENT_TYPE_WALL:
            {
                // Details
                // Type
                auto ft = Formatter();
                ft.Add<ObjectEntryIndex>(tileElement->AsWall()->GetEntryIndex());
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_WALL_TYPE, ft, { COLOUR_WHITE });

                // Banner info
                auto banner = tileElement->AsWall()->GetBanner();
                if (banner != nullptr)
                {
                    ft = Formatter();
                    banner->FormatTextTo(ft);
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_ENTRY_BANNER_TEXT, ft,
                        { COLOUR_WHITE });
                }
                else
                {
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_ENTRY_BANNER_NONE, {},
                        { COLOUR_WHITE });
                }

                // Properties
                // Raise / lower label
                screenCoords.y = w->windowPos.y + w->widgets[WIDX_WALL_SPINNER_HEIGHT].top;
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, {}, { COLOUR_WHITE });

                // Current base height
                screenCoords.x = w->windowPos.x + w->widgets[WIDX_WALL_SPINNER_HEIGHT].left + 3;
                ft = Formatter();
                ft.Add<int32_t>(tileElement->base_height);
                DrawTextBasic(dpi, screenCoords, STR_FORMAT_INTEGER, ft, { COLOUR_WHITE });

                // Slope label
                screenCoords = w->windowPos
                    + ScreenCoordsXY{ w->widgets[WIDX_GROUPBOX_DETAILS].left + 7, w->widgets[WIDX_WALL_DROPDOWN_SLOPE].top };
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_WALL_SLOPE, {}, { COLOUR_WHITE });

                // Animation frame label
                screenCoords.y = w->windowPos.y + w->widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME].top;
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_WALL_ANIMATION_FRAME, {}, { COLOUR_WHITE });

                // Current animation frame
                colour_t colour = COLOUR_WHITE;
                if (WidgetIsDisabled(w, WIDX_WALL_SPINNER_ANIMATION_FRAME))
                {
                    colour = w->colours[0] | COLOUR_FLAG_INSET;
                }
                screenCoords.x = w->windowPos.x + w->widgets[WIDX_WALL_SPINNER_ANIMATION_FRAME].left + 3;
                ft = Formatter();
                ft.Add<int32_t>(tileElement->AsWall()->GetAnimationFrame());
                DrawTextBasic(dpi, screenCoords, STR_FORMAT_INTEGER, ft, { colour });
                break;
            }

            case TILE_ELEMENT_TYPE_LARGE_SCENERY:
            {
                // Details
                // Type
                auto sceneryElement = tileElement->AsLargeScenery();
                ObjectEntryIndex largeSceneryType = sceneryElement->GetEntryIndex();
                auto ft = Formatter();
                ft.Add<ObjectEntryIndex>(largeSceneryType);
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_LARGE_SCENERY_TYPE, ft, { COLOUR_WHITE });

                // Part ID
                ft = Formatter();
                ft.Add<int16_t>(sceneryElement->GetSequenceIndex());
                DrawTextBasic(
                    dpi, screenCoords + ScreenCoordsXY{ 0, 11 }, STR_TILE_INSPECTOR_LARGE_SCENERY_PIECE_ID, ft,
                    { COLOUR_WHITE });

                // Banner info
                auto* largeSceneryEntry = get_large_scenery_entry(largeSceneryType);
                if (largeSceneryEntry != nullptr && largeSceneryEntry->scrolling_mode != SCROLLING_MODE_NONE)
                {
                    auto banner = sceneryElement->GetBanner();
                    if (banner != nullptr)
                    {
                        ft = Formatter();
                        banner->FormatTextTo(ft);
                        DrawTextBasic(
                            dpi, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_ENTRY_BANNER_TEXT, ft,
                            { COLOUR_WHITE });
                    }
                }
                else
                {
                    DrawTextBasic(
                        dpi, screenCoords + ScreenCoordsXY{ 0, 22 }, STR_TILE_INSPECTOR_ENTRY_BANNER_NONE, {},
                        { COLOUR_WHITE });
                }

                // Properties
                // Raise / lower label
                screenCoords.y = w->windowPos.y + w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT].top;
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, {}, { COLOUR_WHITE });

                // Current base height
                screenCoords.x = w->windowPos.x + w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT].left + 3;
                ft = Formatter();
                ft.Add<int32_t>(tileElement->base_height);
                DrawTextBasic(dpi, screenCoords, STR_FORMAT_INTEGER, ft, { COLOUR_WHITE });
                break;
            }

            case TILE_ELEMENT_TYPE_BANNER:
            {
                // Details
                // Banner info
                auto banner = tileElement->AsBanner()->GetBanner();
                if (banner != nullptr)
                {
                    Formatter ft;
                    banner->FormatTextTo(ft);
                    DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_ENTRY_BANNER_TEXT, ft, { COLOUR_WHITE });
                }

                // Properties
                // Raise / lower label
                screenCoords.y = w->windowPos.y + w->widgets[WIDX_BANNER_SPINNER_HEIGHT].top;
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, {}, { COLOUR_WHITE });

                // Current base height
                screenCoords.x = w->windowPos.x + w->widgets[WIDX_BANNER_SPINNER_HEIGHT].left + 3;
                auto ft = Formatter();
                ft.Add<int32_t>(tileElement->base_height);
                DrawTextBasic(dpi, screenCoords, STR_FORMAT_INTEGER, ft, { COLOUR_WHITE });

                // Blocked paths
                screenCoords.y += 28;
                screenCoords.x = w->windowPos.x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_BANNER_BLOCKED_PATHS, {}, { COLOUR_WHITE });
                break;
            }

            case TILE_ELEMENT_TYPE_CORRUPT:
            {
                // Properties
                // Raise / lower label
                screenCoords.y = w->windowPos.y + w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT].top;
                DrawTextBasic(dpi, screenCoords, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, {}, { COLOUR_WHITE });

                // Current base height
                screenCoords.x = w->windowPos.x + w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT].left + 3;
                auto ft = Formatter();
                ft.Add<int32_t>(tileElement->base_height);
                DrawTextBasic(dpi, screenCoords, STR_FORMAT_INTEGER, ft, { COLOUR_WHITE });
                break;
            }

            default:
            {
                break;
            }
        }
    }
}

static void window_tile_inspector_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    const int32_t listWidth = w->widgets[WIDX_LIST].width();
    gfx_fill_rect(
        dpi, { { dpi->x, dpi->y }, { dpi->x + dpi->width - 1, dpi->y + dpi->height - 1 } },
        ColourMapA[w->colours[1]].mid_light);

    ScreenCoordsXY screenCoords{};
    screenCoords.y = SCROLLABLE_ROW_HEIGHT * (windowTileInspectorElementCount - 1);
    int32_t i = 0;
    char buffer[256];

    if (!windowTileInspectorTileSelected)
        return;

    const TileElement* tileElement = map_get_first_element_at(windowTileInspectorToolMap);

    do
    {
        if (tileElement == nullptr)
            break;
        const bool selectedRow = i == windowTileInspectorSelectedIndex;
        const bool hoveredRow = i == windowTileInspectorHighlightedIndex;
        int32_t type = tileElement->GetType();
        const char* typeName = "";

        auto fillRectangle = ScreenRect{ { 0, screenCoords.y }, { listWidth, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1 } };
        if (selectedRow)
        {
            gfx_fill_rect(dpi, fillRectangle, ColourMapA[w->colours[1]].mid_dark);
        }
        else if (hoveredRow)
        {
            gfx_fill_rect(dpi, fillRectangle, ColourMapA[w->colours[1]].mid_dark | 0x1000000);
        }
        else if (((windowTileInspectorElementCount - i) & 1) == 0)
        {
            // Zebra stripes
            gfx_fill_rect(dpi, fillRectangle, ColourMapA[w->colours[1]].light | 0x1000000);
        }

        switch (type)
        {
            case TILE_ELEMENT_TYPE_SURFACE:
                typeName = language_get_string(STR_TILE_INSPECTOR_SURFACE);
                break;
            case TILE_ELEMENT_TYPE_PATH:
                typeName = tileElement->AsPath()->IsQueue() ? language_get_string(STR_QUEUE_LINE_MAP_TIP)
                                                            : language_get_string(STR_FOOTPATH_MAP_TIP);
                break;
            case TILE_ELEMENT_TYPE_TRACK:
                typeName = language_get_string(STR_RIDE_COMPONENT_TRACK_CAPITALISED);
                break;
            case TILE_ELEMENT_TYPE_SMALL_SCENERY:
            {
                const auto* sceneryEntry = tileElement->AsSmallScenery()->GetEntry();
                snprintf(
                    buffer, sizeof(buffer), "%s (%s)", language_get_string(STR_OBJECT_SELECTION_SMALL_SCENERY),
                    sceneryEntry != nullptr ? language_get_string(sceneryEntry->name) : "");
                typeName = buffer;
                break;
            }
            case TILE_ELEMENT_TYPE_ENTRANCE:
                typeName = language_get_string(STR_RIDE_CONSTRUCTION_ENTRANCE);
                break;
            case TILE_ELEMENT_TYPE_WALL:
            {
                const auto* entry = tileElement->AsWall()->GetEntry();
                snprintf(
                    buffer, sizeof(buffer), "%s (%s)", language_get_string(STR_TILE_INSPECTOR_WALL),
                    entry != nullptr ? language_get_string(entry->name) : "");
                typeName = buffer;
                break;
            }
            case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                typeName = language_get_string(STR_OBJECT_SELECTION_LARGE_SCENERY);
                break;
            case TILE_ELEMENT_TYPE_BANNER:
                snprintf(
                    buffer, sizeof(buffer), "%s (%d)", language_get_string(STR_BANNER_WINDOW_TITLE),
                    tileElement->AsBanner()->GetIndex());
                typeName = buffer;
                break;
            case TILE_ELEMENT_TYPE_CORRUPT:
                // fall-through
            default:
                snprintf(buffer, sizeof(buffer), "%s (%d)", language_get_string(STR_UNKNOWN_OBJECT_TYPE), type);
                typeName = buffer;
        }

        const int32_t clearanceHeight = tileElement->clearance_height;
        const bool ghost = tileElement->IsGhost();
        const bool last = tileElement->IsLastForTile();

        const rct_string_id stringFormat = (selectedRow || hoveredRow) ? STR_WHITE_STRING : STR_WINDOW_COLOUR_2_STRINGID;

        // Element name
        auto ft = Formatter();
        ft.Add<rct_string_id>(STR_STRING);
        ft.Add<char*>(typeName);
        DrawTextEllipsised(dpi, screenCoords + ScreenCoordsXY{ TypeColumnXY.x, 0 }, TypeColumnSize.width, stringFormat, ft);

        // Base height
        ft = Formatter();
        ft.Add<rct_string_id>(STR_FORMAT_INTEGER);
        ft.Add<int32_t>(tileElement->base_height);
        DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ BaseHeightColumnXY.x, 0 }, stringFormat, ft);

        // Clearance height
        ft = Formatter();
        ft.Add<rct_string_id>(STR_FORMAT_INTEGER);
        ft.Add<int32_t>(clearanceHeight);
        DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ ClearanceHeightColumnXY.x, 0 }, stringFormat, ft);

        // Checkmarks for ghost and last for tile
        ft = Formatter();
        ft.Add<rct_string_id>(STR_STRING);
        ft.Add<char*>(CheckBoxMarkString);
        if (ghost)
        {
            DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ GhostFlagColumnXY.x, 0 }, stringFormat, ft);
        }
        if (last)
        {
            DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ LastFlagColumnXY.x, 0 }, stringFormat, ft);
        }

        screenCoords.y -= SCROLLABLE_ROW_HEIGHT;
        i++;
    } while (!(tileElement++)->IsLastForTile());
}
