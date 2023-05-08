/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>

static constexpr const int32_t WW = 232;
static constexpr const int32_t WH = 136;

enum
{
    WIDX_LOGO
};

static Widget window_title_logo_widgets[] = {
    MakeWidget({ 0, 0 }, { WW + 1, WH + 1 }, WindowWidgetType::ImgBtn, WindowColour::Primary),
    WIDGETS_END,
};

class TitleLogoWindow final : public Window
{
public:
    /**
     * Creates the window containing the logo and the expansion packs on the title screen.
     *  rct2: 0x0066B679 (part of 0x0066B3E8)
     */
    void OnOpen() override
    {
        widgets = window_title_logo_widgets;
        WindowInitScrollWidgets(*this);
        colours[0] = TRANSLUCENT(COLOUR_GREY);
        colours[1] = TRANSLUCENT(COLOUR_GREY);
        colours[2] = TRANSLUCENT(COLOUR_GREY);
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_LOGO:
                WindowAboutOpen();
                break;
        }
    }

    /**
     *
     *  rct2: 0x0066B872
     */
    void OnDraw(DrawPixelInfo& dpi) override
    {
        auto screenCoords = windowPos + ScreenCoordsXY{ 2, 2 };
        GfxDrawSprite(dpi, ImageId(SPR_G2_LOGO), screenCoords);
        GfxDrawSprite(dpi, ImageId(SPR_G2_TITLE), screenCoords + ScreenCoordsXY{ 104, 18 });
    }
};

WindowBase* WindowTitleLogoOpen()
{
    auto* window = WindowBringToFrontByClass(WindowClass::TitleLogo);
    if (window == nullptr)
    {
        window = WindowCreate<TitleLogoWindow>(
            WindowClass::TitleLogo, ScreenCoordsXY(0, 0), WW, WH, WF_STICK_TO_BACK | WF_TRANSPARENT);
    }
    return window;
}
