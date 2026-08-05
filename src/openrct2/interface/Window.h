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

#include <functional>
#include <memory>

enum class CloseWindowModifier : uint8_t;
enum class WindowClass : uint8_t;

struct CoordsXYZ;
struct ZoomLevel;

namespace OpenRCT2::Drawing
{
    struct RenderTarget;
}

namespace OpenRCT2
{
    using WindowNumber = int16_t;
    using WidgetIndex = uint16_t;

    struct WindowBase;
    struct WindowCloseModifier;

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
