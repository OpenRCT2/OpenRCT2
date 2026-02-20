/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../core/EnumUtils.hpp"
#include "../windows/TileInspectorGlobals.h"
#include "../world/Location.hpp"
#include "Widget.h"
#include "WindowClasses.h"
#include "ZoomLevel.h"

#include <functional>
#include <list>
#include <memory>

struct TrackDesignFileRef;
struct ScenarioIndexEntry;

enum class CursorID : uint8_t;
enum class CloseWindowModifier : uint8_t;

namespace OpenRCT2::Drawing
{
    struct RenderTarget;
}

namespace OpenRCT2
{
    using WindowNumber = int16_t;

    struct WindowBase;
    struct WindowCloseModifier;

    enum class RideConstructionState : uint8_t;

    struct WindowIdentifier
    {
        WindowClass classification;
        WindowNumber number;
    };

    struct WidgetIdentifier
    {
        WindowIdentifier window;
        WidgetIndex widgetIndex;
    };

    extern WindowCloseModifier gLastCloseModifier;

    struct WindowCloseModifier
    {
        WindowIdentifier window;
        CloseWindowModifier modifier;
    };

    struct Viewport;

    enum class WindowFlag : uint8_t
    {
        stickToBack,
        stickToFront,
        /**
         * User is unable to scroll this viewport
         */
        noScrolling,
        scrollingToLocation,
        transparent,
        /**
         * Instead of half transparency, completely remove the window background
         */
        noBackground,
        /**
         * Window is closed and will be deleted in the next update.
         */
        dead,
        resizable,
        /**
         * Don't auto close this window if too many windows are open
         */
        noAutoClose,
        // TODO: investigate why exactly this is used.
        higherContrastOnPress,
        noTitleBar,
        noSnapping,

        // *ONLY* create only flags below
        autoPosition,
        centreScreen,
    };
    using WindowFlags = FlagHolder<uint16_t, WindowFlag>;

    enum class WindowView : uint8_t
    {
        parkAwards,
        parkRating,
        parkObjective,
        parkGuests,
        financesResearch,
        rideResearch,
        mazeConstruction,
        networkPassword,
        editorBottomToolbar,
        changelog,
        newVersionInfo,
        financeMarketing,
        contributors,
    };

    enum class WindowDetail : uint8_t
    {
        banner,
        newCampaign,
        demolishRide,
        refurbishRide,
        sign,
        signSmall,

        player,

        vehicle,
        track,

        null = 255,
    };
} // namespace OpenRCT2

// Cannot be expressed using a constexpr.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define VALIDATE_GLOBAL_WIDX(wc, widx)                                                                                         \
    static_assert(widx == wc##__##widx, "Global WIDX of " #widx " doesn't match actual value.")

constexpr int32_t WC_MAIN_WINDOW__0 = 0;
constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT = 27;
constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE = 32;
constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_EXIT = 33;
constexpr int32_t WC_RIDE_CONSTRUCTION__WIDX_ROTATE = 34;
constexpr int32_t WC_MAZE_CONSTRUCTION__WIDX_MAZE_DIRECTION_GROUPBOX = WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT;
constexpr int32_t WC_MAZE_CONSTRUCTION__WIDX_MAZE_ENTRANCE = WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE;
constexpr int32_t WC_MAZE_CONSTRUCTION__WIDX_MAZE_EXIT = WC_RIDE_CONSTRUCTION__WIDX_EXIT;
constexpr int32_t WC_SCENERY__WIDX_SCENERY_BACKGROUND = 0;
constexpr int32_t WC_SCENERY__WIDX_SCENERY_TAB_1 = 15;
constexpr int32_t WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON = 5;
constexpr int32_t WC_SCENERY__WIDX_SCENERY_EYEDROPPER_BUTTON = 10;
constexpr int32_t WC_PEEP__WIDX_PATROL = 10;
constexpr int32_t WC_PEEP__WIDX_ACTION_LBL = 13;
constexpr int32_t WC_PEEP__WIDX_PICKUP = 14;
constexpr int32_t WC_TRACK_DESIGN_LIST__WIDX_ROTATE = 8;
constexpr int32_t WC_TRACK_DESIGN_PLACE__WIDX_ROTATE = 3;
constexpr int32_t WC_EDITOR_PARK_ENTRANCE__WIDX_ROTATE_ENTRANCE_BUTTON = 6;
constexpr int32_t WC_EDITOR_OBJECT_SELECTION__WIDX_TAB_1 = 22;
constexpr int32_t WC_STAFF__WIDX_PICKUP = 9;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_ROTATE = 15;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_COPY = 18;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_PASTE = 17;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_SORT = 16;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_REMOVE = 12;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_UP = 13;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BUTTON_MOVE_DOWN = 14;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SPINNER_X_INCREASE = 6;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SPINNER_X_DECREASE = 7;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SPINNER_Y_INCREASE = 10;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SPINNER_Y_DECREASE = 11;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_SURFACE = EnumValue(TileInspectorPage::Surface);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SURFACE_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SURFACE_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_PATH = EnumValue(TileInspectorPage::Path);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_PATH_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_PATH_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_TRACK = EnumValue(TileInspectorPage::Track);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_TRACK_SPINNER_HEIGHT_INCREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_TRACK_SPINNER_HEIGHT_DECREASE = 31;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_SCENERY = EnumValue(TileInspectorPage::Scenery);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SCENERY_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_SCENERY_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_ENTRANCE = EnumValue(TileInspectorPage::Entrance);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_WALL = EnumValue(TileInspectorPage::Wall);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_WALL_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_WALL_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_LARGE_SCENERY = EnumValue(TileInspectorPage::LargeScenery);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE = 30;
constexpr int32_t WC_TILE_INSPECTOR__TILE_INSPECTOR_PAGE_BANNER = EnumValue(TileInspectorPage::Banner);
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BANNER_SPINNER_HEIGHT_INCREASE = 29;
constexpr int32_t WC_TILE_INSPECTOR__WIDX_BANNER_SPINNER_HEIGHT_DECREASE = 30;

enum class PromptMode : uint8_t
{
    saveBeforeLoad = 0,
    saveBeforeQuit,
    saveBeforeQuit2,
    saveBeforeNewGame,
    quit
};

enum BTM_TOOLBAR_DIRTY_FLAGS
{
    BTM_TB_DIRTY_FLAG_MONEY = (1 << 0),
    BTM_TB_DIRTY_FLAG_DATE = (1 << 1),
    BTM_TB_DIRTY_FLAG_PEEP_COUNT = (1 << 2),
    BTM_TB_DIRTY_FLAG_CLIMATE = (1 << 3),
    BTM_TB_DIRTY_FLAG_PARK_RATING = (1 << 4)
};

enum class LoadSaveAction : uint8_t
{
    load,
    save,
};

enum class LoadSaveType : uint8_t
{
    park,
    landscape,
    scenario,
    track,
    heightmap,
};

enum class ModalResult : int8_t
{
    fail = -1,
    cancel,
    ok,
};

enum class CloseWindowModifier : uint8_t
{
    none,
    shift,
    control
};

enum class GuestListFilterType : int32_t
{
    guestsOnRide,
    guestsInQueue,
    guestsThinkingAboutRide,
    guestsThinkingX,
};

enum class Tool
{
    arrow = 0,
    upArrow = 2,
    upDownArrow = 3,
    picker = 7,
    crosshair = 12,
    pathDown = 17,
    digDown = 18,
    waterDown = 19,
    walkDown = 22,
    paintDown = 23,
    entranceDown = 24,
    bulldozer = 27,
};

namespace OpenRCT2
{
    struct WidgetRef
    {
        WindowClass windowClassification;
        WindowNumber windowNumber;
        WidgetIndex widgetIndex;
    };

    extern Tool gCurrentToolId;
    extern WidgetRef gCurrentToolWidget;

    using CloseCallback = void (*)();

    constexpr int8_t kWindowLimitMin = 4;
    constexpr int8_t kWindowLimitMax = 64;
    constexpr int8_t kWindowLimitReserved = 4; // Used to reserve room for the main viewport, toolbars, etc.

    extern WindowBase* gWindowAudioExclusive;

    extern uint32_t gWindowUpdateTicks;

    extern Drawing::Colour gCurrentWindowColours[3];

    std::vector<std::unique_ptr<WindowBase>>::iterator WindowGetIterator(const WindowBase* w);
    void WindowVisitEach(std::function<void(WindowBase*)> func);

    void WindowSetFlagForAllViewports(uint32_t viewportFlag, bool enabled);

    void WindowDispatchUpdateAll();
    void WindowUpdateAllViewports();
    void WindowUpdateAll();
    void WindowNotifyLanguageChange();

    void WindowSetWindowLimit(int32_t value);

    int32_t WindowGetScrollDataIndex(const WindowBase& w, WidgetIndex widget_index);

    void WindowPushOthersRight(WindowBase& w);
    void WindowPushOthersBelow(WindowBase& w1);

    WindowBase* WindowGetMain();

    void WindowScrollToLocation(WindowBase& w, const CoordsXYZ& coords);
    void WindowCheckAllValidZoom();
    void WindowZoomSet(WindowBase& w, ZoomLevel zoomLevel, bool atCursor);

    void WindowDrawAll(Drawing::RenderTarget& rt, int32_t left, int32_t top, int32_t right, int32_t bottom);
    void WindowDraw(Drawing::RenderTarget& rt, WindowBase& w, int32_t left, int32_t top, int32_t right, int32_t bottom);

    bool isToolActive(WindowClass cls);
    bool isToolActive(WindowClass cls, WindowNumber number);
    bool isToolActive(WindowClass cls, WidgetIndex widgetIndex);
    bool isToolActive(WindowClass cls, WidgetIndex widgetIndex, WindowNumber number);
    bool isToolActive(const WindowBase& w, WidgetIndex widgetIndex);
    bool ToolSet(const WindowBase& w, WidgetIndex widgetIndex, Tool tool);
    void ToolCancel();

    void WindowUpdateViewportRideMusic();

    Viewport* WindowGetViewport(WindowBase* window);

    // Open window functions
    void WindowResizeGui(int32_t width, int32_t height);
    void WindowResizeGuiScenarioEditor(int32_t width, int32_t height);

    void TextinputCancel();

    void WindowInitAll();

    void WindowFollowSprite(WindowBase& w, EntityId spriteIndex);
    void WindowUnfollowSprite(WindowBase& w);
} // namespace OpenRCT2
