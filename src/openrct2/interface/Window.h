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

#include <functional>
#include <memory>

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

    struct Viewport;
    struct WidgetRef;
    struct WindowBase;
    struct WindowCloseModifier;

    enum class Tool;
    enum class WindowClass : uint8_t;

    extern WindowCloseModifier gLastCloseModifier;
    extern Tool gCurrentToolId;
    extern WidgetRef gCurrentToolWidget;

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

    void WindowResizeGui(int32_t width, int32_t height);

    void TextinputCancel();

    void WindowInitAll();

    void WindowFollowSprite(WindowBase& w, EntityId spriteIndex);
    void WindowUnfollowSprite(WindowBase& w);
} // namespace OpenRCT2
