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
#include <openrct2/SpriteIds.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenSize kWindowSize = { 232, 136 };

    enum
    {
        WIDX_LOGO
    };

    static constexpr auto _titleLogoWidgets = makeWidgets(
        makeWidget({ 0, 0 }, kWindowSize, WidgetType::imgBtn, WindowColour::primary));

    class TitleLogoWindow final : public Window
    {
    public:
        /**
         * Creates the window containing the logo and the expansion packs on the title screen.
         *  rct2: 0x0066B679 (part of 0x0066B3E8)
         */
        void OnOpen() override
        {
            SetWidgets(_titleLogoWidgets);
            WindowInitScrollWidgets(*this);
            colours[0] = ColourWithFlags{ COLOUR_GREY }.withFlag(ColourFlag::translucent, true);
            colours[1] = ColourWithFlags{ COLOUR_GREY }.withFlag(ColourFlag::translucent, true);
            colours[2] = ColourWithFlags{ COLOUR_GREY }.withFlag(ColourFlag::translucent, true);
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_LOGO:
                    AboutOpen();
                    break;
            }
        }

        /**
         *
         *  rct2: 0x0066B872
         */
        void OnDraw(RenderTarget& rt) override
        {
            auto screenCoords = windowPos + ScreenCoordsXY{ 2, 2 };
            GfxDrawSprite(rt, ImageId(SPR_G2_LOGO), screenCoords);
            GfxDrawSprite(rt, ImageId(SPR_G2_TITLE), screenCoords + ScreenCoordsXY{ 104, 18 });
        }
    };

    WindowBase* TitleLogoOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::TitleLogo);
        if (window == nullptr)
        {
            window = windowMgr->Create<TitleLogoWindow>(
                WindowClass::TitleLogo, ScreenCoordsXY(0, 0), kWindowSize, WF_STICK_TO_BACK | WF_TRANSPARENT | WF_NO_TITLE_BAR);
        }
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
