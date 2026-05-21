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

constexpr int32_t kWindowMainWidgetIdxViewport = 0;
constexpr int32_t kWindowRideConstructionWidgetIdxConstruct = 27;
constexpr int32_t kWindowRideConstructionWidgetIdxEntrance = 32;
constexpr int32_t kWindowRideConstructionWidgetIdxExit = 33;
constexpr int32_t kWindowRideConstructionWidgetIdxRotate = 34;
constexpr int32_t kWindowScenaryWidgetIdxScenaryRotateObjectsButton = 5;
constexpr int32_t kWindowScenaryWidgetIdxScenaryEyedropperButton = 10;
constexpr int32_t kWindowPeepWidgetIdxPickup = 14;
constexpr int32_t kWindowTrackDesignListWidgetIdxRotate = 8;
constexpr int32_t kWindowTrackDesignPlaceWidgetIdxRotate = 3;
constexpr int32_t kWindowEditorParkEntranceWidgetIdxRotateEntranceButton = 6;
constexpr int32_t kWindowStaffWidgetIdxPickup = 9;
constexpr int32_t kWindowStaffWidgetIdxPatrol = 10;
constexpr int32_t kWindowTileInspectorWidgetIdxButtonRotate = 15;
constexpr int32_t kWindowTileInspectorWidgetIdxButtonCopy = 18;
constexpr int32_t kWindowTileInspectorWidgetIdxButtonPaste = 17;
constexpr int32_t kWindowTileInspectorWidgetIdxButtonSort = 16;
constexpr int32_t kWindowTileInspectorWidgetIdxButtonRemove = 12;
constexpr int32_t kWindowTileInspectorWidgetIdxButtonMoveUp = 13;
constexpr int32_t kWindowTileInspectorWidgetIdxButtonMoveDown = 14;
constexpr int32_t kWindowTileInspectorWidgetIdxSpinnerXIncrease = 6;
constexpr int32_t kWindowTileInspectorWidgetIdxSpinnerXDecrease = 7;
constexpr int32_t kWindowTileInspectorWidgetIdxSpinnerYIncrease = 10;
constexpr int32_t kWindowTileInspectorWidgetIdxSpinnerYDecrease = 11;
constexpr int32_t kWindowTileInspectorWidgetIdxSurfaceSpinnerHeightIncrease = 29;
constexpr int32_t kWindowTileInspectorWidgetIdxSurfaceSpinnerHeightDecrease = 30;
constexpr int32_t kWindowTileInspectorWidgetIdxPathSpinnerHeightIncrease = 29;
constexpr int32_t kWindowTileInspectorWidgetIdxPathSpinnerHeightDecrease = 30;
constexpr int32_t kWindowTileInspectorWidgetIdxTrackSpinnerHeightIncrease = 30;
constexpr int32_t kWindowTileInspectorWidgetIdxTrackSpinnerHeightDecrease = 31;
constexpr int32_t kWindowTileInspectorWidgetIdxScenarySpinnerHeightIncrease = 29;
constexpr int32_t kWindowTileInspectorWidgetIdxScenarySpinnerHeightDecrease = 30;
constexpr int32_t kWindowTileInspectorWidgetIdxEntranceSpinnerHeightIncrease = 29;
constexpr int32_t kWindowTileInspectorWidgetIdxEntranceSpinnerHeightDecrease = 30;
constexpr int32_t kWindowTileInspectorWidgetIdxWallSpinnerHeightIncrease = 29;
constexpr int32_t kWindowTileInspectorWidgetIdxWallSpinnerHeightDecrease = 30;
constexpr int32_t kWindowTileInspectorWidgetIdxLargeScenarySpinnerHeightIncrease = 29;
constexpr int32_t kWindowTileInspectorWidgetIdxLargeScenarySpinnerHeightDecrease = 30;
constexpr int32_t kWindowTileInspectorWidgetIdxBannerSpinnerHeightIncrease = 29;
constexpr int32_t kWindowTileInspectorWidgetIdxBannerSpinnerHeightDecrease = 30;
constexpr int32_t kTileInspectorPageSurface = EnumValue(TileInspectorPage::Surface);
constexpr int32_t kTileInspectorPagePath = EnumValue(TileInspectorPage::Path);
constexpr int32_t kTileInspectorPageTrack = EnumValue(TileInspectorPage::Track);
constexpr int32_t kTileInspectorPageScenary = EnumValue(TileInspectorPage::Scenery);
constexpr int32_t kTileInspectorPageEntrance = EnumValue(TileInspectorPage::Entrance);
constexpr int32_t kTileInspectorPageWall = EnumValue(TileInspectorPage::Wall);
constexpr int32_t kTileInspectorPageLargeScenary = EnumValue(TileInspectorPage::LargeScenery);
constexpr int32_t kTileInspectorPageBanner = EnumValue(TileInspectorPage::Banner);

enum class PromptMode : uint8_t
{
    saveBeforeLoad = 0,
    saveBeforeQuit,
    saveBeforeQuit2,
    saveBeforeNewGame,
    quit
};

enum BottomToolbarDirtyFlags
{
    bottomToolbarDirtyFlagMoney = (1 << 0),
    bottomToolbarDirtyFlagDate = (1 << 1),
    bottomToolbarDirtyFlagPeepCount = (1 << 2),
    bottomToolbarDirtyFlagClimate = (1 << 3),
    bottomToolbarDirtyFlagParkRating = (1 << 4)
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
    // Used to reserve room for the main viewport, toolbars, etc.
    constexpr int8_t kWindowLimitReserved = 4;

    extern WindowBase* gWindowAudioExclusive;

    extern uint32_t gWindowUpdateTicks;

    extern Drawing::Colour gCurrentWindowColours[3];

    std::vector<std::unique_ptr<WindowBase>>::iterator WindowGetIterator(const WindowBase* w);
    void WindowVisitEach(std::function<void(WindowBase*)> func);

    void WindowSetFlagForAllViewports(uint32_t viewportFlag, bool enabled);

    void WindowDispatchUpdateAll();
    void WindowUpdateAllViewports();
    void WindowCullDead();
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
