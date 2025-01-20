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
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    enum WindowTitleExitWidgetIdx
    {
        WIDX_EXIT,
    };

    static constexpr Widget _titleExitWidgets[] = {
        MakeWidget({ 0, 0 }, { 40, 64 }, WindowWidgetType::ImgBtn, WindowColour::Tertiary, ImageId(SPR_MENU_EXIT), STR_EXIT),
    };

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

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);
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
            WindowClass::TitleExit, ScreenCoordsXY(ContextGetWidth() - 40, ContextGetHeight() - 64), 40, 64,
            WF_STICK_TO_BACK | WF_TRANSPARENT);
    }
} // namespace OpenRCT2::Ui::Windows
