/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "../interface/Viewport.h"

#include <iterator>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Theme.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/CheatSetAction.h>
#include <openrct2/actions/ParkSetDateAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/network/Network.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Climate.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    enum WindowTransparencyWidgetIndex
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,

        WIDX_HIDE_VEGETATION,
        WIDX_HIDE_SCENERY,
        WIDX_HIDE_PATHS,
        WIDX_HIDE_RIDES,
        WIDX_HIDE_VEHICLES,
        WIDX_HIDE_SUPPORTS,
        WIDX_HIDE_GUESTS,
        WIDX_HIDE_STAFF,
        WIDX_INVISIBLE_VEGETATION,
        WIDX_INVISIBLE_SCENERY,
        WIDX_INVISIBLE_PATHS,
        WIDX_INVISIBLE_RIDES,
        WIDX_INVISIBLE_VEHICLES,
        WIDX_INVISIBLE_SUPPORTS,
    };

#pragma region MEASUREMENTS

    static constexpr StringId kWindowTitle = STR_TRANSPARENCY_OPTIONS_TITLE;
    static constexpr ScreenSize kWindowSize = { 204, 57 };

    static constexpr ScreenSize kHideSize = { 24, 24 };
    static constexpr ScreenSize kInvisibleSize = { 24, 12 };

#pragma endregion

    // clang-format off
    static constexpr auto _transparancyWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({  2, 17}, kHideSize,      WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_G2_BUTTON_HIDE_VEGETATION), STR_SEE_THROUGH_VEGETATION),
        makeWidget({ 27, 17}, kHideSize,      WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_G2_BUTTON_HIDE_SCENERY),    STR_SEE_THROUGH_SCENERY),
        makeWidget({ 52, 17}, kHideSize,      WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_G2_BUTTON_FOOTPATH),        STR_SEE_THROUGH_PATHS),
        makeWidget({ 77, 17}, kHideSize,      WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_RIDE),                      STR_SEE_THROUGH_RIDES),
        makeWidget({102, 17}, kHideSize,      WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_G2_BUTTON_HIDE_VEHICLES),   STR_SEE_THROUGH_VEHICLES),
        makeWidget({127, 17}, kHideSize,      WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_G2_BUTTON_HIDE_SUPPORTS),   STR_SEE_THROUGH_SUPPORTS),
        makeWidget({152, 17}, kHideSize,      WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_GUESTS),                    STR_SEE_THROUGH_GUESTS),
        makeWidget({177, 17}, kHideSize,      WidgetType::flatBtn, WindowColour::secondary, 0xFFFFFFFF,                             STR_SEE_THROUGH_STAFF),

        makeWidget({  2, 42}, kInvisibleSize, WidgetType::flatBtn, WindowColour::tertiary,  kStringIdNone,                          STR_INVISIBLE_VEGETATION),
        makeWidget({ 27, 42}, kInvisibleSize, WidgetType::flatBtn, WindowColour::tertiary,  kStringIdNone,                          STR_INVISIBLE_SCENERY),
        makeWidget({ 52, 42}, kInvisibleSize, WidgetType::flatBtn, WindowColour::tertiary,  kStringIdNone,                          STR_INVISIBLE_PATHS),
        makeWidget({ 77, 42}, kInvisibleSize, WidgetType::flatBtn, WindowColour::tertiary,  kStringIdNone,                          STR_INVISIBLE_RIDES),
        makeWidget({102, 42}, kInvisibleSize, WidgetType::flatBtn, WindowColour::tertiary,  kStringIdNone,                          STR_INVISIBLE_VEHICLES),
        makeWidget({127, 42}, kInvisibleSize, WidgetType::flatBtn, WindowColour::tertiary,  kStringIdNone,                          STR_INVISIBLE_SUPPORTS)
    );
    // clang-format on

    class TransparencyWindow final : public Window
    {
    private:
    public:
        void onOpen() override
        {
            setWidgets(_transparancyWidgets);
            WindowPushOthersBelow(*this);

            auto* w = WindowGetMain();
            if (w != nullptr)
                windowPos.x = ((w->width / 2) - (width / 2));
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                default:
                    ToggleViewportFlag(widgetIndex);
                    break;
            }
        }

        void onPrepareDraw() override
        {
            uint32_t wflags = 0;
            WindowBase* w = WindowGetMain();

            pressedWidgets = 0;
            disabledWidgets = 0;

            if (w != nullptr)
                wflags = w->viewport->flags;

            setWidgetPressed(WIDX_HIDE_VEGETATION, (wflags & VIEWPORT_FLAG_HIDE_VEGETATION));
            setWidgetPressed(WIDX_HIDE_SCENERY, (wflags & VIEWPORT_FLAG_HIDE_SCENERY));
            setWidgetPressed(WIDX_HIDE_PATHS, (wflags & VIEWPORT_FLAG_HIDE_PATHS));
            setWidgetPressed(WIDX_HIDE_RIDES, (wflags & VIEWPORT_FLAG_HIDE_RIDES));
            setWidgetPressed(WIDX_HIDE_VEHICLES, (wflags & VIEWPORT_FLAG_HIDE_VEHICLES));
            setWidgetPressed(WIDX_HIDE_SUPPORTS, (wflags & VIEWPORT_FLAG_HIDE_SUPPORTS));
            setWidgetPressed(WIDX_HIDE_GUESTS, (wflags & VIEWPORT_FLAG_HIDE_GUESTS));
            setWidgetPressed(WIDX_HIDE_STAFF, (wflags & VIEWPORT_FLAG_HIDE_STAFF));
            setWidgetPressed(WIDX_INVISIBLE_VEGETATION, (wflags & VIEWPORT_FLAG_INVISIBLE_VEGETATION));
            setWidgetPressed(WIDX_INVISIBLE_SCENERY, (wflags & VIEWPORT_FLAG_INVISIBLE_SCENERY));
            setWidgetPressed(WIDX_INVISIBLE_PATHS, (wflags & VIEWPORT_FLAG_INVISIBLE_PATHS));
            setWidgetPressed(WIDX_INVISIBLE_RIDES, (wflags & VIEWPORT_FLAG_INVISIBLE_RIDES));
            setWidgetPressed(WIDX_INVISIBLE_VEHICLES, (wflags & VIEWPORT_FLAG_INVISIBLE_VEHICLES));
            setWidgetPressed(WIDX_INVISIBLE_SUPPORTS, (wflags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS));

            for (WidgetIndex i = WIDX_INVISIBLE_VEGETATION; i <= WIDX_INVISIBLE_SUPPORTS; i++)
            {
                widgets[i].image = ImageId(isWidgetPressed(i) ? SPR_G2_BUTTON_HIDE_FULL : SPR_G2_BUTTON_HIDE_PARTIAL);
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);
            // Locate mechanic button image
            const auto& widget = widgets[WIDX_HIDE_STAFF];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };
            auto image = ImageId(SPR_MECHANIC, Drawing::Colour::black, getGameState().park.staffMechanicColour);
            GfxDrawSprite(rt, image, screenCoords);
        }

    private:
        uint32_t ToggleTransparency(uint32_t wflags, uint32_t transparencyFlag, uint32_t seeThroughFlag)
        {
            wflags ^= transparencyFlag;
            if (wflags & transparencyFlag)
            {
                wflags |= seeThroughFlag;
            }
            SaveInConfig(wflags);
            return wflags;
        }

        void ToggleViewportFlag(WidgetIndex widgetIndex)
        {
            uint32_t wflags = 0;
            WindowBase* w = WindowGetMain();

            if (w == nullptr)
                return;

            wflags = w->viewport->flags;

            switch (widgetIndex)
            {
                case WIDX_HIDE_RIDES:
                    wflags ^= VIEWPORT_FLAG_HIDE_RIDES;
                    break;
                case WIDX_HIDE_VEHICLES:
                    wflags ^= VIEWPORT_FLAG_HIDE_VEHICLES;
                    break;
                case WIDX_HIDE_SCENERY:
                    wflags ^= VIEWPORT_FLAG_HIDE_SCENERY;
                    break;
                case WIDX_HIDE_VEGETATION:
                    wflags ^= VIEWPORT_FLAG_HIDE_VEGETATION;
                    break;
                case WIDX_HIDE_PATHS:
                    wflags ^= VIEWPORT_FLAG_HIDE_PATHS;
                    break;
                case WIDX_HIDE_SUPPORTS:
                    wflags ^= VIEWPORT_FLAG_HIDE_SUPPORTS;
                    break;
                case WIDX_INVISIBLE_RIDES:
                    wflags = ToggleTransparency(wflags, VIEWPORT_FLAG_INVISIBLE_RIDES, VIEWPORT_FLAG_HIDE_RIDES);
                    break;
                case WIDX_INVISIBLE_VEHICLES:
                    wflags = ToggleTransparency(wflags, VIEWPORT_FLAG_INVISIBLE_VEHICLES, VIEWPORT_FLAG_HIDE_VEHICLES);
                    break;
                case WIDX_INVISIBLE_SCENERY:
                    wflags = ToggleTransparency(wflags, VIEWPORT_FLAG_INVISIBLE_SCENERY, VIEWPORT_FLAG_HIDE_SCENERY);
                    break;
                case WIDX_INVISIBLE_VEGETATION:
                    wflags = ToggleTransparency(wflags, VIEWPORT_FLAG_INVISIBLE_VEGETATION, VIEWPORT_FLAG_HIDE_VEGETATION);
                    break;
                case WIDX_INVISIBLE_PATHS:
                    wflags = ToggleTransparency(wflags, VIEWPORT_FLAG_INVISIBLE_PATHS, VIEWPORT_FLAG_HIDE_PATHS);
                    break;
                case WIDX_INVISIBLE_SUPPORTS:
                    wflags = ToggleTransparency(wflags, VIEWPORT_FLAG_INVISIBLE_SUPPORTS, VIEWPORT_FLAG_HIDE_SUPPORTS);
                    break;
                case WIDX_HIDE_GUESTS:
                    wflags ^= VIEWPORT_FLAG_HIDE_GUESTS;
                    break;
                case WIDX_HIDE_STAFF:
                    wflags ^= VIEWPORT_FLAG_HIDE_STAFF;
                    break;
                default:
                    return;
            }

            if (w->viewport->flags == wflags)
                return;

            w->viewport->flags = wflags;
            w->invalidate();
        }

        void SaveInConfig(uint32_t wflags)
        {
            Config::Get().general.invisibleRides = wflags & VIEWPORT_FLAG_INVISIBLE_RIDES;
            Config::Get().general.invisibleVehicles = wflags & VIEWPORT_FLAG_INVISIBLE_VEHICLES;
            Config::Get().general.invisibleScenery = wflags & VIEWPORT_FLAG_INVISIBLE_SCENERY;
            Config::Get().general.invisibleTrees = wflags & VIEWPORT_FLAG_INVISIBLE_VEGETATION;
            Config::Get().general.invisiblePaths = wflags & VIEWPORT_FLAG_INVISIBLE_PATHS;
            Config::Get().general.invisibleSupports = wflags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS;
            Config::Save();
        }
    };

    WindowBase* TransparencyOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::transparency);
        if (window == nullptr)
            window = windowMgr->Create<TransparencyWindow>(WindowClass::transparency, ScreenCoordsXY(32, 32), kWindowSize);

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
