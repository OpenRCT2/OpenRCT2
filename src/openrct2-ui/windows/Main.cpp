/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiStringIds.h"

#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/config/Config.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Footpath.h>

namespace OpenRCT2::Ui::Windows
{
    // clang-format off
    static constexpr Widget _mainWidgets[] = {
        makeWidget({0, 0}, {0, 0}, WidgetType::viewport, WindowColour::primary),
    };
    // clang-format on

    class MainWindow final : public Window
    {
    public:
        void onOpen() override
        {
            setWidgets(_mainWidgets);
            widgets[0].right = width;
            widgets[0].bottom = height;

            ViewportCreate(*this, windowPos, width, height, Focus(CoordsXYZ(0x0FFF, 0x0FFF, 0)));
            if (viewport != nullptr)
            {
                SetViewportFlags();
                viewport->rotation = 0;
            }
            gShowGridLinesRefCount = 0;
            gShowLandRightsRefCount = 0;
            gShowConstructionRightsRefCount = 0;
            WindowFootpathResetSelectedPath();
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            ViewportRender(rt, viewport);
        }

    private:
        void SetViewportFlags()
        {
            viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
            if (Config::Get().general.invisibleRides)
                viewport->flags |= VIEWPORT_FLAG_INVISIBLE_RIDES;
            if (Config::Get().general.invisibleVehicles)
                viewport->flags |= VIEWPORT_FLAG_INVISIBLE_VEHICLES;
            if (Config::Get().general.invisibleTrees)
                viewport->flags |= VIEWPORT_FLAG_INVISIBLE_VEGETATION;
            if (Config::Get().general.invisibleScenery)
                viewport->flags |= VIEWPORT_FLAG_INVISIBLE_SCENERY;
            if (Config::Get().general.invisiblePaths)
                viewport->flags |= VIEWPORT_FLAG_INVISIBLE_PATHS;
            if (Config::Get().general.invisibleSupports)
                viewport->flags |= VIEWPORT_FLAG_INVISIBLE_SUPPORTS;
        }
    };

    /**
     * Creates the main window that holds the main viewport.
     *  rct2: 0x0066B3E8
     */
    WindowBase* MainOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->Create<MainWindow>(
            WindowClass::mainWindow, { 0, 0 }, { ContextGetWidth(), ContextGetHeight() },
            { WindowFlag::stickToBack, WindowFlag::noTitleBar });
    }
} // namespace OpenRCT2::Ui::Windows
