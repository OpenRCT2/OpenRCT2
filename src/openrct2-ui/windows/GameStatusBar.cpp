/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Theme.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    enum GameStatusBarWidgetIdx : WidgetIndex
    {
        WIDX_PANEL_OUTSET,
        WIDX_PANEL_INSET,
    };

    // clang-format off
    static constexpr Widget kGameStatusBarWidgets[] =
    {
        makeWidget({  0, 0}, {356, 34}, WidgetType::imgBtn,       WindowColour::primary), // Middle outset panel
        makeWidget({  2, 2}, {352, 30}, WidgetType::hiddenButton, WindowColour::primary), // Middle inset panel
    };
    // clang-format on

    class GameStatusBar final : public Window
    {
        void DrawMiddlePanel(RenderTarget& rt)
        {
            Widget* middleOutsetWidget = &widgets[WIDX_PANEL_OUTSET];

            Rectangle::fillInset(
                rt,
                { windowPos + ScreenCoordsXY{ middleOutsetWidget->left + 1, middleOutsetWidget->top + 1 },
                  windowPos + ScreenCoordsXY{ middleOutsetWidget->right - 1, middleOutsetWidget->bottom - 1 } },
                colours[0], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::light, Rectangle::FillMode::none);

            // Figure out how much line height we have to work with.
            uint32_t line_height = FontGetLineHeight(FontStyle::medium);

            ScreenCoordsXY middleWidgetCoords(
                windowPos.x + middleOutsetWidget->midX(), windowPos.y + middleOutsetWidget->top + line_height + 1);
            int32_t panelWidth = middleOutsetWidget->width() - 63;

            // Check if there is a map tooltip to draw
            StringId stringId;
            auto ft = GetMapTooltip();
            std::memcpy(&stringId, ft.Data(), sizeof(StringId));
            if (stringId == kStringIdNone)
            {
                // TODO: this string probably shouldn't be reused for this
                drawTextWrapped(
                    rt, middleWidgetCoords, panelWidth, STR_TITLE_SEQUENCE_OPENRCT2, ft, { colours[0], TextAlignment::centre });
            }
            else
            {
                // Show tooltip in bottom toolbar
                drawTextWrapped(rt, middleWidgetCoords, panelWidth, STR_STRINGID, ft, { colours[0], TextAlignment::centre });
            }
        }

    public:
        GameStatusBar()
        {
            setWidgets(kGameStatusBarWidgets);

            // Reset the middle widget to not show by default.
            // If it is required to be shown news_update will reshow it.
            widgets[WIDX_PANEL_OUTSET].setHidden();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_PANEL_INSET:
                    ContextOpenWindow(WindowClass::recentNews);
                    break;
            }
        }

        void onPrepareDraw() override
        {
            // Figure out how much line height we have to work with.
            uint32_t line_height = FontGetLineHeight(FontStyle::medium);

            // Reset dimensions as appropriate -- in case we're switching languages.
            height = line_height * 2 + 12;
            windowPos.y = ContextGetHeight() - height;

            // Change height of widgets in accordance with line height.
            widgets[WIDX_PANEL_OUTSET].bottom = line_height * 3 + 3;
            widgets[WIDX_PANEL_INSET].bottom = line_height * 3 + 1;

            // Anchor the middle and right panel to the right
            widgets[WIDX_PANEL_OUTSET].right = width - 1;
            widgets[WIDX_PANEL_INSET].right = width - 3;

            bool useFullToolbar = News::IsQueueEmpty() && (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
                && gLegacyScene == LegacyScene::playing;

            widgets[WIDX_PANEL_OUTSET].setVisible(useFullToolbar);
            widgets[WIDX_PANEL_INSET].setVisible(useFullToolbar);
        }

        void onDraw(RenderTarget& rt) override
        {
            const auto& middleWidget = widgets[WIDX_PANEL_OUTSET];
            if (middleWidget.isHidden())
                return;

            if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
            {
                // Draw grey background
                auto leftTop = windowPos + ScreenCoordsXY{ middleWidget.left, middleWidget.top };
                auto rightBottom = windowPos + ScreenCoordsXY{ middleWidget.right, middleWidget.bottom };
                Rectangle::filter(rt, { leftTop, rightBottom }, FilterPaletteID::palette51);
            }

            drawWidgets(rt);

            if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
            {
                DrawMiddlePanel(rt);
            }
        }
    };

    /**
     * Creates the main game bottom toolbar window.
     */
    WindowBase* gameStatusBarOpen()
    {
        // Don't create window when theme does not require it
        // NB: bailing out here as Game.cpp calls this function without access to themes
        if (!(ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR))
            return nullptr;

        // TODO: query ParkInfoPanel, DateInfoPanel
        constexpr auto kPanelWidth = 142;

        // Toolbar width depends on whether we're showing panels besides
        // TODO: read config when applicable
        auto toolbarWidth = ContextGetWidth() - kPanelWidth * 2;

        // Figure out how much line height we have to work with.
        uint32_t lineHeight = FontGetLineHeight(FontStyle::medium);
        int32_t toolbarHeight = lineHeight * 2 + 12;

        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->Create<GameStatusBar>(
            WindowClass::gameStatusBar, ScreenCoordsXY(kPanelWidth, ContextGetHeight() - toolbarHeight),
            { toolbarWidth, toolbarHeight },
            { WindowFlag::stickToFront, WindowFlag::transparent, WindowFlag::noBackground, WindowFlag::noTitleBar });

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
