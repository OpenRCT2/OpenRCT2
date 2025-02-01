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
#include <openrct2/Version.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr int32_t WW = 500;
    static constexpr int32_t WH = 30;

    static constexpr uint8_t kTextOffset = 8;

    class TitleVersionWindow final : public Window
    {
        void OnDraw(DrawPixelInfo& dpi) override
        {
            // Write name and version information
            const auto whiteOutline = ColourWithFlags{ COLOUR_WHITE }.withFlag(ColourFlag::withOutline, true);
            DrawText(dpi, windowPos, { whiteOutline }, gVersionInfoFull);
            width = GfxGetStringWidth(gVersionInfoFull, FontStyle::Medium);

            // Write platform information
            constexpr const char platformInfo[] = OPENRCT2_PLATFORM " (" OPENRCT2_ARCHITECTURE ")";
            DrawText(dpi, windowPos + ScreenCoordsXY(0, kListRowHeight), { whiteOutline }, platformInfo);
            width = std::max<int16_t>(width, GfxGetStringWidth(platformInfo, FontStyle::Medium)) + kTextOffset;
        }
    };

    WindowBase* TitleVersionOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::TitleVersion);
        if (window == nullptr)
        {
            window = windowMgr->Create<TitleVersionWindow>(
                WindowClass::TitleVersion, ScreenCoordsXY(kTextOffset, ContextGetHeight() - 30), WW, WH,
                WF_STICK_TO_BACK | WF_TRANSPARENT);
        }
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
