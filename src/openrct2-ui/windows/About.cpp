/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include <SDL_clipboard.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/Version.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/UiContext.h>

using namespace OpenRCT2;

static constexpr const int32_t WW = 400;
static constexpr const int32_t WH = 352;
static constexpr const StringId WINDOW_TITLE = STR_ABOUT;
static constexpr const int32_t TABHEIGHT = 50;

// clang-format off
enum
{
    WINDOW_ABOUT_PAGE_OPENRCT2,
    WINDOW_ABOUT_PAGE_RCT2,
};

enum WindowAboutWidgetIdx {
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
    WIDX_CONTRIBUTORS,
    WIDX_COPYRIGHT,
};

#define WIDGETS_MAIN \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget     ({ 0, TABHEIGHT}, {WW, WH - TABHEIGHT}, WindowWidgetType::ImgBtn, WindowColour::Secondary               ), /* page background */       \
    MakeRemapWidget({ 3,        17}, {91, TABHEIGHT - 16}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE), /* about OpenRCT2 button */ \
    MakeRemapWidget({94,        17}, {91, TABHEIGHT - 16}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE)  /* about RCT2 button */

static rct_widget _windowAboutOpenRCT2Widgets[] = {
    WIDGETS_MAIN,
    MakeWidget({10, 60},        {WW - 20, 20}, WindowWidgetType::LabelCentred, WindowColour::Secondary, STR_ABOUT_OPENRCT2_DESCRIPTION), // Introduction
    MakeWidget({30, 90},        {128, 128},    WindowWidgetType::Placeholder,  WindowColour::Secondary, STR_NONE), // OpenRCT2 Logo
    MakeWidget({168, 100},      {173, 24},     WindowWidgetType::Placeholder,  WindowColour::Secondary, STR_NONE), // Build version
    MakeWidget({344, 100 },     {24, 24},      WindowWidgetType::ImgBtn,       WindowColour::Secondary, SPR_G2_COPY, STR_COPY_BUILD_HASH   ), // "Copy build info" button
    MakeWidget({168, 115 + 24}, {200, 14},     WindowWidgetType::Placeholder,  WindowColour::Secondary, STR_UPDATE_AVAILABLE  ), // "new version" button
    MakeWidget({168, 115 + 48}, {200, 14},     WindowWidgetType::Button,       WindowColour::Secondary, STR_CHANGELOG_ELLIPSIS), // changelog button
    MakeWidget({168, 115 + 72}, {200, 14},     WindowWidgetType::Button,       WindowColour::Secondary, STR_JOIN_DISCORD      ), // "join discord" button
    MakeWidget({10, 250},       {WW - 20, 50}, WindowWidgetType::LabelCentred, WindowColour::Secondary, STR_ABOUT_OPENRCT2_DESCRIPTION_2), // Contributors
    MakeWidget({10, 300},       {WW - 20, 50}, WindowWidgetType::LabelCentred, WindowColour::Secondary, STR_ABOUT_OPENRCT2_DESCRIPTION_3), // Copyright
    WIDGETS_END,
};

static rct_widget _windowAboutRCT2Widgets[] = {
    WIDGETS_MAIN,
    WIDGETS_END,
};

static rct_widget *_windowAboutPageWidgets[] = {
    _windowAboutOpenRCT2Widgets,
    _windowAboutRCT2Widgets,
};

// clang-format on

class AboutWindow final : public Window
{
public:
    void OnOpen() override
    {
        widgets = _windowAboutOpenRCT2Widgets;

        WindowInitScrollWidgets(*this);
        SetPage(WINDOW_ABOUT_PAGE_OPENRCT2);
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_TAB_ABOUT_OPENRCT2:
            case WIDX_TAB_ABOUT_RCT2:
                SetPage(widgetIndex - WIDX_TAB_ABOUT_OPENRCT2);
                break;
            case WIDX_JOIN_DISCORD:
                OpenRCT2::GetContext()->GetUiContext()->OpenURL("https://discord.gg/ZXZd8D8");
                break;
            case WIDX_CHANGELOG:
                context_open_window(WC_CHANGELOG);
                break;
            case WIDX_NEW_VERSION:
                context_open_window_view(WV_NEW_VERSION_INFO);
                break;
            case WIDX_COPY_BUILD_INFO:
                SDL_SetClipboardText(gVersionInfoFull);
                break;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);

        const auto& aboutOpenRCT2 = widgets[WIDX_TAB_ABOUT_OPENRCT2];
        const auto& aboutRCT2 = widgets[WIDX_TAB_ABOUT_RCT2];

        int32_t y = windowPos.y + aboutOpenRCT2.midY() - 3;
        ScreenCoordsXY aboutOpenRCT2Coords(windowPos.x + aboutOpenRCT2.left + 45, y);
        ScreenCoordsXY aboutRCT2Coords(windowPos.x + aboutRCT2.left + 45, y);

        // Draw tab names
        {
            auto ft = Formatter();
            ft.Add<StringId>(STR_TITLE_SEQUENCE_OPENRCT2);
            DrawTextWrapped(
                &dpi, aboutOpenRCT2Coords, 87, STR_WINDOW_COLOUR_2_STRINGID, ft, { COLOUR_AQUAMARINE, TextAlignment::CENTRE });
        }
        {
            auto ft = Formatter();
            ft.Add<StringId>(STR_TITLE_SEQUENCE_RCT2);
            DrawTextWrapped(
                &dpi, aboutRCT2Coords, 87, STR_WINDOW_COLOUR_2_STRINGID, ft, { COLOUR_AQUAMARINE, TextAlignment::CENTRE });
        }

        if (page == WINDOW_ABOUT_PAGE_OPENRCT2)
        {
            DrawOpenRCT2Info(dpi);
        }
        else if (page == WINDOW_ABOUT_PAGE_RCT2)
        {
            DrawRCT2Info(dpi);
        }
    }

private:
    /**
     * @brief Set which tab to show
     */
    void SetPage(int32_t p)
    {
        page = p;
        frame_no = 0;
        pressed_widgets = 0;
        widgets = _windowAboutPageWidgets[p];

        pressed_widgets |= (p == WINDOW_ABOUT_PAGE_RCT2) ? (1ULL << WIDX_TAB_ABOUT_RCT2) : (1ULL << WIDX_TAB_ABOUT_OPENRCT2);
        WindowInitScrollWidgets(*this);
        Invalidate();
    }

    /**
     * @brief Draw OpenRCT2 info on open tab
     */
    void DrawOpenRCT2Info(rct_drawpixelinfo& dpi)
    {
        // Draw logo on placeholder widget
        ScreenCoordsXY logoCoords = windowPos
            + ScreenCoordsXY(widgets[WIDX_OPENRCT2_LOGO].left, widgets[WIDX_OPENRCT2_LOGO].top);
        gfx_draw_sprite(&dpi, ImageId(SPR_G2_LOGO), logoCoords);
        // Version info
        utf8 buffer[256];
        utf8* ch = buffer;
        openrct2_write_full_version_info(ch, sizeof(buffer) - (ch - buffer));
        auto ft = Formatter();
        ft.Add<const char*>(buffer);

        auto const& versionPlaceholder = widgets[WIDX_VERSION];
        auto versionPlaceHolderWidth = versionPlaceholder.right - versionPlaceholder.left;
        auto centreX = versionPlaceholder.left + versionPlaceHolderWidth / 2;
        auto centreY = (versionPlaceholder.top + versionPlaceholder.bottom - font_get_line_height(FontSpriteBase::MEDIUM)) / 2;
        auto centrePos = windowPos + ScreenCoordsXY(centreX, centreY);
        DrawTextWrapped(&dpi, centrePos, versionPlaceHolderWidth, STR_STRING, ft, { colours[1], TextAlignment::CENTRE });

        // Shows the update available button
        if (OpenRCT2::GetContext()->HasNewVersionInfo())
        {
            widgets[WIDX_NEW_VERSION].type = WindowWidgetType::Button;
            _windowAboutOpenRCT2Widgets[WIDX_NEW_VERSION].type = WindowWidgetType::Button;
        }
    }

    /**
     * @brief Draw RCT2 info on open tab
     */
    void DrawRCT2Info(rct_drawpixelinfo& dpi)
    {
        int32_t yPage = windowPos.y + widgets[WIDX_PAGE_BACKGROUND].top + 5;

        auto screenCoords = ScreenCoordsXY{ windowPos.x + 200, yPage + 5 };

        int32_t lineHeight = font_get_line_height(FontSpriteBase::MEDIUM);

        // Credits
        DrawTextBasic(&dpi, screenCoords, STR_COPYRIGHT_CS, {}, { TextAlignment::CENTRE });
        screenCoords.y += lineHeight + 74;
        DrawTextBasic(&dpi, screenCoords, STR_DESIGNED_AND_PROGRAMMED_BY_CS, {}, { TextAlignment::CENTRE });
        screenCoords.y += lineHeight;
        DrawTextBasic(&dpi, screenCoords, STR_GRAPHICS_BY_SF, {}, { TextAlignment::CENTRE });
        screenCoords.y += lineHeight;
        DrawTextBasic(&dpi, screenCoords, STR_SOUND_AND_MUSIC_BY_AB, {}, { TextAlignment::CENTRE });
        screenCoords.y += lineHeight;
        DrawTextBasic(&dpi, screenCoords, STR_ADDITIONAL_SOUNDS_RECORDED_BY_DE, {}, { TextAlignment::CENTRE });
        screenCoords.y += lineHeight + 3;
        DrawTextBasic(&dpi, screenCoords, STR_REPRESENTATION_BY_JL, {}, { TextAlignment::CENTRE });
        screenCoords.y += 2 * lineHeight + 5;
        DrawTextBasic(&dpi, screenCoords, STR_THANKS_TO, {}, { TextAlignment::CENTRE });
        screenCoords.y += lineHeight;
        DrawTextBasic(&dpi, screenCoords, STR_THANKS_TO_PEOPLE, {}, { TextAlignment::CENTRE });
        screenCoords.y += 2 * lineHeight + 5;
        DrawTextBasic(&dpi, screenCoords, STR_LICENSED_TO_INFOGRAMES_INTERACTIVE_INC, {}, { TextAlignment::CENTRE });

        // Images
        gfx_draw_sprite(&dpi, ImageId(SPR_CREDITS_CHRIS_SAWYER_SMALL), { windowPos.x + 92, yPage + 24 });

        // Licence
    }
};

/**
 *
 *  rct2: 0x0066D2AC
 */
rct_window* WindowAboutOpen()
{
    return WindowFocusOrCreate<AboutWindow>(WC_ABOUT, WW, WH, WF_CENTRE_SCREEN);
}
