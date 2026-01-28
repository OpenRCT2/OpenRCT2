/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiStringIds.h"

#include <SDL_clipboard.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/Version.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenSize kWindowSize = { 400, 450 };
    static constexpr StringId kWindowTitle = STR_ABOUT;
    static constexpr int32_t kTabHeight = 50;

    static constexpr auto kPadding = 10;

    enum
    {
        WINDOW_ABOUT_PAGE_OPENRCT2,
        WINDOW_ABOUT_PAGE_RCT2,
    };

    enum WindowAboutWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_PAGE_BACKGROUND,
        WIDX_TAB_ABOUT_OPENRCT2,
        WIDX_TAB_ABOUT_RCT2,

        WIDX_PAGE_START,

        // About OpenRCT2
        WIDX_INTRO = WIDX_PAGE_START,
        WIDX_OPENRCT2_LOGO,
        WIDX_VERSION,
        WIDX_COPY_BUILD_INFO,
        WIDX_NEW_VERSION,
        WIDX_CHANGELOG,
        WIDX_JOIN_DISCORD,
        WIDX_CONTRIBUTORS_BUTTON,
    };

    // clang-format off
    static constexpr auto kMainWidgets = makeWidgets(
        makeWindowShim (kWindowTitle, kWindowSize),
        makeWidget     ({  0, kTabHeight }, { kWindowSize.width, kWindowSize.height - kTabHeight }, WidgetType::frame, WindowColour::secondary),
        makeRemapWidget({  3, 17         }, { 91,                kTabHeight - 16                 }, WidgetType::tab,   WindowColour::secondary, SPR_TAB_LARGE),
        makeRemapWidget({ 94, 17         }, { 91,                kTabHeight - 16                 }, WidgetType::tab,   WindowColour::secondary, SPR_TAB_LARGE)
    );

    static constexpr auto _windowAboutOpenRCT2Widgets = makeWidgets(
        kMainWidgets,
        makeWidget({10, 60},        {kWindowSize.width - 20, 20}, WidgetType::labelCentred, WindowColour::secondary, STR_ABOUT_OPENRCT2_DESCRIPTION           ), // Introduction
        makeWidget({30, 90},        {128, 128},                   WidgetType::placeholder,  WindowColour::secondary, kStringIdNone                            ), // OpenRCT2 Logo
        makeWidget({168, 100},      {173, 24},                    WidgetType::placeholder,  WindowColour::secondary, kStringIdNone                            ), // Build version
        makeWidget({344, 100 },     {24, 24},                     WidgetType::imgBtn,       WindowColour::secondary, ImageId(SPR_G2_COPY), STR_COPY_BUILD_HASH), // "Copy build info" button
        makeWidget({168, 115 + 20}, {200, 14},                    WidgetType::placeholder,  WindowColour::secondary, STR_UPDATE_AVAILABLE                     ), // "new version" button
        makeWidget({168, 115 + 40}, {200, 14},                    WidgetType::button,       WindowColour::secondary, STR_CHANGELOG_ELLIPSIS                   ), // changelog button
        makeWidget({168, 115 + 60}, {200, 14},                    WidgetType::button,       WindowColour::secondary, STR_JOIN_DISCORD                         ), // "join discord" button
        makeWidget({168, 115 + 80}, {200, 14},                    WidgetType::button,       WindowColour::secondary, STR_CONTRIBUTORS_WINDOW_BUTTON           ) // "contributors" button
    );
    // clang-format on

    static constexpr auto _windowAboutRCT2Widgets = makeWidgets(kMainWidgets);

    static constexpr std::span<const Widget> _windowAboutPageWidgets[] = {
        _windowAboutOpenRCT2Widgets,
        _windowAboutRCT2Widgets,
    };

    // clang-format off
    static const StringId _OpenRCT2InfoStrings[] = {
        STR_ABOUT_OPENRCT2_DESCRIPTION_2,
        STR_ABOUT_OPENRCT2_DESCRIPTION_3,
        STR_ABOUT_OPENRCT2_TITLE,
        STR_ABOUT_FAIRGROUND_ORGAN,
        STR_ABOUT_SPECIAL_THANKS_1,
        STR_ABOUT_SPECIAL_THANKS_2,
    };
    // clang-format on

    static const StringId _RCT2InfoStrings[] = {
        STR_COPYRIGHT_CS,
        STR_DESIGNED_AND_PROGRAMMED_BY_CS,
        STR_GRAPHICS_BY_SF,
        STR_SOUND_AND_MUSIC_BY_AB,
        STR_ADDITIONAL_SOUNDS_RECORDED_BY_DE,
        STR_REPRESENTATION_BY_JL,
        kStringIdEmpty,
        STR_THANKS_TO,
        STR_THANKS_TO_PEOPLE,
        kStringIdEmpty,
        STR_LICENSED_TO_INFOGRAMES_INTERACTIVE_INC,
    };

    class AboutWindow final : public Window
    {
    public:
        void onOpen() override
        {
            setPage(WINDOW_ABOUT_PAGE_OPENRCT2);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_TAB_ABOUT_OPENRCT2:
                case WIDX_TAB_ABOUT_RCT2:
                    setPage(widgetIndex - WIDX_TAB_ABOUT_OPENRCT2);
                    break;
                case WIDX_JOIN_DISCORD:
                    GetContext()->GetUiContext().OpenURL("https://discord.gg/ZXZd8D8");
                    break;
                case WIDX_CHANGELOG:
                    ContextOpenWindow(WindowClass::changelog);
                    break;
                case WIDX_NEW_VERSION:
                    ContextOpenWindowView(WindowView::newVersionInfo);
                    break;
                case WIDX_COPY_BUILD_INFO:
                    GetContext()->GetUiContext().SetClipboardText(gVersionInfoFull);
                    break;
                case WIDX_CONTRIBUTORS_BUTTON:
                    ContextOpenWindowView(WindowView::contributors);
                    break;
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);

            const auto& aboutOpenRCT2 = widgets[WIDX_TAB_ABOUT_OPENRCT2];
            const auto& aboutRCT2 = widgets[WIDX_TAB_ABOUT_RCT2];

            int32_t y = windowPos.y + aboutOpenRCT2.midY() - 3;
            ScreenCoordsXY aboutOpenRCT2Coords(windowPos.x + aboutOpenRCT2.left + 45, y);
            ScreenCoordsXY aboutRCT2Coords(windowPos.x + aboutRCT2.left + 45, y);

            // Draw tab names
            {
                // TODO: this string shouldn't be reused for this tab
                auto ft = Formatter();
                ft.Add<StringId>(STR_TITLE_SEQUENCE_OPENRCT2);
                DrawTextWrapped(
                    rt, aboutOpenRCT2Coords, 87, STR_WINDOW_COLOUR_2_STRINGID, ft,
                    { Drawing::Colour::lightWater, TextAlignment::centre });
            }
            {
                // TODO: this string shouldn't be reused for this tab
                auto ft = Formatter();
                ft.Add<StringId>(STR_TITLE_SEQUENCE_RCT2);
                DrawTextWrapped(
                    rt, aboutRCT2Coords, 87, STR_WINDOW_COLOUR_2_STRINGID, ft,
                    { Drawing::Colour::lightWater, TextAlignment::centre });
            }

            int32_t newHeight = 0;
            if (page == WINDOW_ABOUT_PAGE_OPENRCT2)
            {
                newHeight = DrawOpenRCT2Info(rt) + kPadding;
            }
            else if (page == WINDOW_ABOUT_PAGE_RCT2)
            {
                newHeight = DrawRCT2Info(rt) + kPadding;
            }

            if (newHeight != height)
            {
                invalidate();
                widgets[WIDX_PAGE_BACKGROUND].bottom = newHeight;
                widgets[WIDX_BACKGROUND].bottom = newHeight;
                height = newHeight;
            }
        }

    private:
        void setPage(int32_t p)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == p && !widgets.empty())
                return;

            page = p;
            currentFrame = 0;
            pressedWidgets = 0;

            WindowSetResize(*this, kWindowSize, kWindowSize);
            setWidgets(_windowAboutPageWidgets[p]);

            switch (p)
            {
                case WINDOW_ABOUT_PAGE_OPENRCT2:
                    pressedWidgets |= (1uLL << WIDX_TAB_ABOUT_OPENRCT2);
                    break;
                case WINDOW_ABOUT_PAGE_RCT2:
                    pressedWidgets |= (1uLL << WIDX_TAB_ABOUT_RCT2);
                    break;
            }
        }

        int32_t DrawOpenRCT2Info(Drawing::RenderTarget& rt)
        {
            // Draw logo on placeholder widget
            const auto& logoWidget = widgets[WIDX_OPENRCT2_LOGO];
            auto logoCoords = windowPos + ScreenCoordsXY(logoWidget.left, logoWidget.top);
            GfxDrawSprite(rt, ImageId(SPR_G2_LOGO), logoCoords);

            u8string versionInfo = gVersionInfoFull;
            auto ft = Formatter();
            ft.Add<const char*>(versionInfo.c_str());

            const auto& versionWidget = widgets[WIDX_VERSION];
            auto centreX = versionWidget.midX();
            auto centreY = versionWidget.midY() - FontGetLineHeight(FontStyle::medium) / 2;
            auto centrePos = windowPos + ScreenCoordsXY(centreX, centreY);
            DrawTextWrapped(rt, centrePos, versionWidget.width() - 1, STR_STRING, ft, { colours[1], TextAlignment::centre });

            // Shows the update available button
            if (GetContext()->HasNewVersionInfo())
            {
                widgets[WIDX_NEW_VERSION].type = WidgetType::button;
            }

            // Draw the rest of the text
            TextPaint tp{ colours[1], TextAlignment::centre };
            auto textCoords = windowPos + ScreenCoordsXY((width / 2) - 1, 240);
            auto textWidth = kWindowSize.width - (kPadding * 2);
            for (auto stringId : _OpenRCT2InfoStrings)
                textCoords.y += DrawTextWrapped(rt, textCoords, textWidth, stringId, {}, tp) + 5;

            return textCoords.y - windowPos.y;
        }

        int32_t DrawRCT2Info(Drawing::RenderTarget& rt)
        {
            auto& backgroundWidget = widgets[WIDX_PAGE_BACKGROUND];
            auto textCoords = windowPos + ScreenCoordsXY{ backgroundWidget.midX(), backgroundWidget.top + kPadding };
            auto textWidth = kWindowSize.width - 20;
            TextPaint tp{ colours[1], TextAlignment::centre };

            // Draw credits
            for (auto stringId : _RCT2InfoStrings)
            {
                if (stringId == kStringIdEmpty)
                {
                    textCoords.y += 16;
                    continue;
                }

                textCoords.y += DrawTextWrapped(rt, textCoords, textWidth, stringId, {}, tp);
                if (stringId == STR_COPYRIGHT_CS)
                    textCoords.y += 74;
            }

            // Draw images
            auto imageCoords = windowPos + ScreenCoordsXY{ 92, backgroundWidget.top + 5 + 24 };
            GfxDrawSprite(rt, ImageId(SPR_CREDITS_CHRIS_SAWYER_SMALL), imageCoords);

            return textCoords.y - windowPos.y;
        }
    };

    /**
     *
     *  rct2: 0x0066D2AC
     */
    WindowBase* AboutOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<AboutWindow>(WindowClass::about, kWindowSize, WindowFlag::centreScreen);
    }
} // namespace OpenRCT2::Ui::Windows
