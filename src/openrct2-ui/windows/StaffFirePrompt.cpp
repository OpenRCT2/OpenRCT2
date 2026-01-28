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
#include <openrct2/actions/StaffFireAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_SACK_STAFF;
    static constexpr ScreenSize kWindowSize = { 200, 100 };

    enum WindowStaffFireWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_YES,
        WIDX_CANCEL
    };

    // clang-format off
    // 0x9AFB4C
    static constexpr auto _staffFireWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({                    10, kWindowSize.height - 20}, {85, 14}, WidgetType::button, WindowColour::primary, STR_YES               ),
        makeWidget({kWindowSize.width - 95, kWindowSize.height - 20}, {85, 14}, WidgetType::button, WindowColour::primary, STR_SAVE_PROMPT_CANCEL)
    );
    // clang-format on

    class StaffFirePromptWindow final : public Window
    {
    public:
        void setWindowNumber(WindowNumber windownumber)
        {
            number = windownumber;
        }

        void onOpen() override
        {
            setWidgets(_staffFireWidgets);
            WindowInitScrollWidgets(*this);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_YES:
                {
                    auto staffFireAction = GameActions::StaffFireAction(EntityId::FromUnderlying(number));
                    GameActions::Execute(&staffFireAction, getGameState());
                    break;
                }
                case WIDX_CLOSE:
                case WIDX_CANCEL:
                    close();
                    break;
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);

            Peep* peep = getGameState().entities.GetEntity<Staff>(EntityId::FromUnderlying(number));
            // The staff member may have been fired in the meantime.
            if (peep == nullptr)
            {
                return;
            }
            auto ft = Formatter();
            peep->FormatNameTo(ft);

            ScreenCoordsXY textCoords(windowPos + ScreenCoordsXY{ kWindowSize.width / 2, (kWindowSize.height / 2) - 3 });
            DrawTextWrapped(rt, textCoords, kWindowSize.width - 4, STR_FIRE_STAFF_ID, ft, { TextAlignment::centre });
        }
    };

    WindowBase* StaffFirePromptOpen(Peep* peep)
    {
        // Check if the confirm window already exists
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->FocusOrCreate<StaffFirePromptWindow>(
            WindowClass::firePrompt, kWindowSize, { WindowFlag::centreScreen, WindowFlag::transparent });
        window->setWindowNumber(peep->Id.ToUnderlying());
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
