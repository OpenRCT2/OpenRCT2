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
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenSize kWindowSize = { 80, 15 };

    enum WindowTitleOptionsWidgetIdx
    {
        WIDX_OPTIONS,
    };

    static constexpr auto _windowTitleOptionsWidgets = makeWidgets(
        makeWidget({ 0, 0 }, kWindowSize, WidgetType::button, WindowColour::tertiary, STR_OPTIONS, STR_OPTIONS_TIP));

    class TitleOptionsWindow final : public Window
    {
    public:
        void OnOpen() override
        {
            SetWidgets(_windowTitleOptionsWidgets);
            WindowInitScrollWidgets(*this);
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_OPTIONS:
                    ContextOpenWindow(WindowClass::Options);
                    break;
            }
        }

        void OnDraw(RenderTarget& rt) override
        {
            DrawWidgets(rt);
        }
    };

    /**
     * Creates the window containing the options button on the title screen.
     */
    WindowBase* TitleOptionsOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::TitleOptions);
        if (window == nullptr)
        {
            window = windowMgr->Create<TitleOptionsWindow>(
                WindowClass::TitleOptions, ScreenCoordsXY(ContextGetWidth() - 80, 0), kWindowSize,
                WF_STICK_TO_BACK | WF_TRANSPARENT | WF_NO_TITLE_BAR);
        }

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
