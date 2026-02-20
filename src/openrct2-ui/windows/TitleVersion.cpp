/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Version.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/ColourWithFlags.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenSize kWindowSize = { 500, 30 };

    static constexpr uint8_t kTextOffset = 8;

    class TitleVersionWindow final : public Window
    {
        void onDraw(Drawing::RenderTarget& rt) override
        {
            // Write name and version information
            const auto whiteOutline = ColourWithFlags{ Drawing::Colour::white }.withFlag(ColourFlag::withOutline, true);
            DrawText(rt, windowPos, { whiteOutline }, gVersionInfoFull);
            width = GfxGetStringWidth(gVersionInfoFull, FontStyle::medium);

            // Write platform information
            constexpr const char platformInfo[] = OPENRCT2_PLATFORM " (" OPENRCT2_ARCHITECTURE ")";
            DrawText(rt, windowPos + ScreenCoordsXY(0, kListRowHeight), { whiteOutline }, platformInfo);
            width = std::max<int16_t>(width, GfxGetStringWidth(platformInfo, FontStyle::medium)) + kTextOffset;
        }
    };

    WindowBase* TitleVersionOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::titleVersion);
        if (window == nullptr)
        {
            window = windowMgr->Create<TitleVersionWindow>(
                WindowClass::titleVersion, ScreenCoordsXY(kTextOffset, ContextGetHeight() - 30), kWindowSize,
                { WindowFlag::stickToBack, WindowFlag::transparent, WindowFlag::noTitleBar });
        }
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
