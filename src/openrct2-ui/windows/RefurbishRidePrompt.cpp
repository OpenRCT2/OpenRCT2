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
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/actions/RideDemolishAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenSize kWindowSize = { 200, 100 };

    enum WindowRideRefurbishWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_REFURBISH,
        WIDX_CANCEL
    };

    // clang-format off
    static constexpr auto window_ride_refurbish_widgets = makeWidgets(
        makeWindowShim(STR_REFURBISH_RIDE, kWindowSize                                                                                                   ),
        makeWidget    ({                     10, kWindowSize.height - 22 }, { 85, 14 }, WidgetType::button, WindowColour::primary, STR_REFURBISH         ),
        makeWidget    ({ kWindowSize.width - 95, kWindowSize.height - 22 }, { 85, 14 }, WidgetType::button, WindowColour::primary, STR_SAVE_PROMPT_CANCEL)
    );
    // clang-format on

    class RefurbishRidePromptWindow final : public Window
    {
        money64 _demolishRideCost;

    public:
        void SetRide(const Ride& currentRide)
        {
            rideId = currentRide.id;
            _demolishRideCost = -RideGetRefundPrice(currentRide);
        }

        void onOpen() override
        {
            setWidgets(window_ride_refurbish_widgets);
            WindowInitScrollWidgets(*this);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_REFURBISH:
                {
                    auto gameAction = GameActions::RideDemolishAction(rideId, GameActions::RideModifyType::renew);
                    GameActions::Execute(&gameAction, getGameState());
                    break;
                }
                case WIDX_CANCEL:
                case WIDX_CLOSE:
                    close();
                    break;
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            WindowDrawWidgets(*this, rt);

            auto currentRide = GetRide(rideId);
            if (currentRide != nullptr)
            {
                auto stringId = (getGameState().park.flags & PARK_FLAGS_NO_MONEY) ? STR_REFURBISH_RIDE_ID_NO_MONEY
                                                                                  : STR_REFURBISH_RIDE_ID_MONEY;
                auto ft = Formatter();
                currentRide->formatNameTo(ft);
                ft.Add<money64>(_demolishRideCost / 2);

                ScreenCoordsXY stringCoords(windowPos.x + kWindowSize.width / 2, windowPos.y + (kWindowSize.height / 2) - 3);
                DrawTextWrapped(rt, stringCoords, kWindowSize.width - 4, stringId, ft, { TextAlignment::centre });
            }
        }
    };

    WindowBase* RideRefurbishPromptOpen(const Ride& ride)
    {
        RefurbishRidePromptWindow* newWindow;

        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::demolishRidePrompt);
        if (w != nullptr)
        {
            auto windowPos = w->windowPos;
            windowMgr->Close(*w);

            newWindow = windowMgr->Create<RefurbishRidePromptWindow>(
                WindowClass::demolishRidePrompt, windowPos, kWindowSize, WindowFlag::transparent);
        }
        else
        {
            newWindow = windowMgr->Create<RefurbishRidePromptWindow>(
                WindowClass::demolishRidePrompt, kWindowSize, { WindowFlag::centreScreen, WindowFlag::transparent });
        }

        newWindow->SetRide(ride);

        return newWindow;
    }
} // namespace OpenRCT2::Ui::Windows
