/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/UiStringIds.h>
#include <openrct2-ui/input/MouseInput.h>
#include <openrct2-ui/input/ShortcutIds.h>
#include <openrct2-ui/input/ShortcutManager.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/interface/WindowTypes.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    enum WindowTouchActionBarWidgetIndex : WidgetIndex
    {
        WIDX_BACKGROUND,
        WIDX_ROTATE,
        WIDX_CONFIRM,
        WIDX_CANCEL,
    };

#pragma region MEASUREMENTS

    // Sized for a fingertip rather than a cursor. 44dp is the usual minimum touch target; at the
    // enlarged UI scale Android runs at, 40 logical pixels lands there or above.
    static constexpr int32_t kButtonWidth = 90;
    static constexpr int32_t kButtonHeight = 40;
    static constexpr int32_t kGap = 4;
    static constexpr int32_t kPadding = 4;

    static constexpr int32_t kButtonCount = 3;
    static constexpr ScreenSize kWindowSize = { (kButtonWidth * kButtonCount) + (kGap * (kButtonCount - 1)) + (kPadding * 2),
                                                kButtonHeight + (kPadding * 2) };

    // Keeps the bar clear of the park/date panels, which sit along the bottom of the screen.
    static constexpr int32_t kBottomMargin = 40;

#pragma endregion

    // clang-format off
    static constexpr auto kTouchActionBarWidgets = makeWidgets(
        makeWidget({0, 0}, kWindowSize, WidgetType::frame,  WindowColour::primary                                                                      ),
        makeWidget({kPadding + ((kButtonWidth + kGap) * 0), kPadding}, {kButtonWidth, kButtonHeight},
            WidgetType::button, WindowColour::secondary, STR_TOUCH_BAR_ROTATE,  STR_TOUCH_BAR_ROTATE_TIP ),
        makeWidget({kPadding + ((kButtonWidth + kGap) * 1), kPadding}, {kButtonWidth, kButtonHeight},
            WidgetType::button, WindowColour::secondary, STR_TOUCH_BAR_CONFIRM, STR_TOUCH_BAR_CONFIRM_TIP),
        makeWidget({kPadding + ((kButtonWidth + kGap) * 2), kPadding}, {kButtonWidth, kButtonHeight},
            WidgetType::button, WindowColour::secondary, STR_TOUCH_BAR_CANCEL,  STR_TOUCH_BAR_CANCEL_TIP )
    );
    // clang-format on

    /**
     * Confirm/Cancel affordance for a tool that touch has positioned but not yet committed.
     *
     * Without this there is nothing on screen telling the player that lifting their finger is
     * about to build something, and no way to change their mind. The bar exists only while a
     * tool is held, and closes itself the moment it is not.
     */
    class TouchActionBarWindow final : public Window
    {
    public:
        void onOpen() override
        {
            setWidgets(kTouchActionBarWidgets);
            reposition();
        }

        void onPrepareDraw() override
        {
            // Clear Scenery positions a brush and then removes what is inside it, so committing it
            // is destruction, not construction. Labelling that "Build" is worse than saying nothing.
            const auto destructive = IsRemovalTool();
            widgets[WIDX_CONFIRM].text = destructive ? STR_DEMOLISH : STR_TOUCH_BAR_CONFIRM;
            widgets[WIDX_CONFIRM].tooltip = destructive ? STR_TOUCH_BAR_DEMOLISH_TIP : STR_TOUCH_BAR_CONFIRM_TIP;

            // Nothing Clear Scenery removes has an orientation to change.
            widgetSetEnabled(*this, WIDX_ROTATE, !destructive);
        }

        void onResize() override
        {
            reposition();
        }

        void onUpdate() override
        {
            // The tool can be cancelled or replaced from anywhere -- another window opening, the
            // player picking a different tool, the tool's own window closing. Rather than trying
            // to hook every one of those, poll the state that matters.
            if (!InputIsTouchToolHeld())
            {
                close();
                return;
            }

            reposition();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_ROTATE:
                    rotateHeldObject();
                    break;
                case WIDX_CONFIRM:
                    // Close first: committing runs the tool's own onToolDown/onToolUp, which can
                    // open windows and re-enter the input state machine. Closing afterwards leaves
                    // a window between those two states that can be resurrected by a stray release.
                    close();
                    InputCommitHeldTouchTool();
                    break;
                case WIDX_CANCEL:
                    InputReleaseHeldTouchTool();
                    ToolCancel();
                    close();
                    break;
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);
        }

    private:
        void reposition()
        {
            const auto screenWidth = ContextGetWidth();
            const auto screenHeight = ContextGetHeight();

            auto pos = ScreenCoordsXY{ (screenWidth - kWindowSize.width) / 2,
                                       screenHeight - kWindowSize.height - kBottomMargin };
            pos.x = std::max(pos.x, 0);
            pos.y = std::max(pos.y, 0);

            if (pos != windowPos)
            {
                invalidate();
                windowPos = pos;
                invalidate();
            }
        }

        /**
         * Whether the active tool removes rather than places.
         */
        static bool IsRemovalTool()
        {
            return gCurrentToolWidget.windowClassification == WindowClass::clearScenery;
        }

        /**
         * Rotates whatever is being placed, by reusing the existing rotate-construction shortcut
         * rather than duplicating its per-window knowledge of scenery, track and mazes.
         */
        static void rotateHeldObject()
        {
            auto* shortcut = GetShortcutManager().getShortcut(ShortcutId::kInterfaceRotateConstruction);
            if (shortcut != nullptr && shortcut->action != nullptr)
            {
                shortcut->action();
            }
        }
    };

    WindowBase* TouchActionBarOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::touchActionBar);
        if (window == nullptr)
        {
            window = windowMgr->Create<TouchActionBarWindow>(
                WindowClass::touchActionBar, kWindowSize,
                { WindowFlag::stickToFront, WindowFlag::noTitleBar, WindowFlag::noAutoClose });
        }
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
