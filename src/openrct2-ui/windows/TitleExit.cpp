/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenSize kWindowSize = { 40, 64 };

    enum WindowTitleExitWidgetIdx
    {
        WIDX_EXIT,
    };

    static constexpr auto _titleExitWidgets = makeWidgets(
        makeWidget({ 0, 0 }, kWindowSize, WidgetType::imgBtn, WindowColour::tertiary, ImageId(SPR_MENU_EXIT), STR_EXIT));

    class TitleExitWindow final : public Window
    {
        void OnOpen() override
        {
            SetWidgets(_titleExitWidgets);
            InitScrollWidgets();
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_EXIT:
                    ContextQuit();
                    // game_do_command(0, 1, 0, 0, 5, 3, 0);
                    break;
            };
        }

        void OnDraw(RenderTarget& rt) override
        {
            DrawWidgets(rt);
        }
    };

    /**
     * Creates the window containing the exit button on the title screen.
     *  rct2: 0x0066B624 (part of 0x0066B3E8)
     */
    WindowBase* TitleExitOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->Create<TitleExitWindow>(
            WindowClass::TitleExit, ScreenCoordsXY(ContextGetWidth() - 40, ContextGetHeight() - 64), kWindowSize,
            WF_STICK_TO_BACK | WF_TRANSPARENT | WF_NO_TITLE_BAR);
    }
} // namespace OpenRCT2::Ui::Windows
