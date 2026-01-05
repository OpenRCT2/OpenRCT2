/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/ViewportQuery.h"

#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/StaffSetPatrolAreaAction.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/PatrolArea.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_SET_PATROL_AREA;
    static constexpr ScreenSize kWindowSize = { 104, 54 };

    enum WindowPatrolAreaWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_PREVIEW,
        WIDX_DECREMENT,
        WIDX_INCREMENT,
    };

    // clang-format off
    static constexpr auto PatrolAreaWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget     ({27, 17}, {44, 32}, WidgetType::imgBtn,  WindowColour::primary , ImageId(SPR_LAND_TOOL_SIZE_0)                                  ), // preview box
        makeRemapWidget({28, 18}, {16, 16}, WidgetType::trnBtn,  WindowColour::tertiary, SPR_LAND_TOOL_DECREASE,      STR_ADJUST_SMALLER_PATROL_AREA_TIP), // decrement size
        makeRemapWidget({54, 32}, {16, 16}, WidgetType::trnBtn,  WindowColour::tertiary, SPR_LAND_TOOL_INCREASE,      STR_ADJUST_LARGER_PATROL_AREA_TIP )  // increment size
    );
    // clang-format on

    class PatrolAreaWindow final : public Window
    {
    public:
        void onOpen() override
        {
            setWidgets(PatrolAreaWidgets);

            holdDownWidgets = (1uLL << WIDX_INCREMENT) | (1uLL << WIDX_DECREMENT);
            WindowInitScrollWidgets(*this);
            WindowPushOthersBelow(*this);
            gLandToolSize = 4;
        }

        void onClose() override
        {
            // If the tool wasn't changed, turn tool off
            if (PatrolAreaToolIsActive())
                ToolCancel();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_PREVIEW:
                    InputSize();
                    break;
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_DECREMENT:
                    gLandToolSize = std::max<uint16_t>(kLandToolMinimumSize, gLandToolSize - 1);
                    invalidate();
                    break;
                case WIDX_INCREMENT:
                    gLandToolSize = std::min<uint16_t>(kLandToolMaximumSize, gLandToolSize + 1);
                    invalidate();
                    break;
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (text.empty())
                return;

            if (widgetIndex != WIDX_PREVIEW)
                return;

            const auto res = String::tryParse<int32_t>(text);
            if (res.has_value())
            {
                int32_t size;
                size = res.value();
                size = std::max<uint16_t>(kLandToolMinimumSize, size);
                size = std::min<uint16_t>(kLandToolMaximumSize, size);
                gLandToolSize = size;
                invalidate();
            }
        }

        void onUpdate() override
        {
            // Close window if another tool is open or staff window gets closed
            if (!PatrolAreaToolIsActive() || !IsStaffWindowOpen())
            {
                close();
            }
        }

        void onPrepareDraw() override
        {
            setWidgetPressed(WIDX_PREVIEW, true);
            widgets[WIDX_PREVIEW].image = ImageId(LandTool::SizeToSpriteIndex(gLandToolSize));
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);

            // Draw number for tool sizes bigger than 7
            if (gLandToolSize > kLandToolMaximumSizeWithSprite)
            {
                auto screenCoords = ScreenCoordsXY{ windowPos.x + widgets[WIDX_PREVIEW].midX(),
                                                    windowPos.y + widgets[WIDX_PREVIEW].midY() };
                auto ft = Formatter();
                ft.Add<uint16_t>(gLandToolSize);
                DrawTextBasic(
                    rt, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::centre });
            }
        }

        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto mapTile = GetBestCoordsFromPos(screenCoords);
            if (!mapTile)
                return;

            auto stateChanged = false;
            if (!gMapSelectFlags.has(MapSelectFlag::enable))
                stateChanged = true;

            if (gMapSelectType != MapSelectType::full)
                stateChanged = true;

            auto toolSize = std::max<uint16_t>(1, gLandToolSize);
            auto toolLength = (toolSize - 1) * 32;

            // Move to tool bottom left
            mapTile->x -= (toolSize - 1) * 16;
            mapTile->y -= (toolSize - 1) * 16;
            mapTile = mapTile->ToTileStart();
            auto posA = *mapTile;
            mapTile->x += toolLength;
            mapTile->y += toolLength;
            auto posB = *mapTile;
            if (gMapSelectPositionA != posA || gMapSelectPositionB != posB)
                stateChanged = true;

            if (stateChanged)
            {
                gMapSelectFlags.set(MapSelectFlag::enable);
                gMapSelectType = MapSelectType::full;
                gMapSelectPositionA = posA;
                gMapSelectPositionB = posB;
            }
        }

        void onToolAbort(WidgetIndex widgetIndex) override
        {
            HideGridlines();
            ClearPatrolAreaToRender();
            GfxInvalidateScreen();
        }

        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto mapTile = GetBestCoordsFromPos(screenCoords);
            if (mapTile)
            {
                auto staff = getGameState().entities.GetEntity<Staff>(_staffId);
                if (staff != nullptr)
                {
                    _mode = staff->IsPatrolAreaSet(*mapTile) ? GameActions::StaffSetPatrolAreaMode::Unset
                                                             : GameActions::StaffSetPatrolAreaMode::Set;
                }
            }

            onToolDrag(widgetIndex, screenCoords);
        }

        void onToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto staff = getGameState().entities.GetEntity<Staff>(_staffId);
            if (staff != nullptr)
            {
                MapRange range(gMapSelectPositionA, gMapSelectPositionB);
                auto staffSetPatrolAreaAction = GameActions::StaffSetPatrolAreaAction(_staffId, range, _mode);
                GameActions::Execute(&staffSetPatrolAreaAction, getGameState());
            }
        }

        EntityId GetStaffId() const
        {
            return _staffId;
        }

        void setStaffId(EntityId staffId)
        {
            _staffId = staffId;
            EnableTool();
        }

    private:
        EntityId _staffId;
        GameActions::StaffSetPatrolAreaMode _mode;

        void EnableTool()
        {
            if (PatrolAreaToolIsActive())
            {
                SetPatrolAreaToRender(_staffId);
                GfxInvalidateScreen();
            }
            else
            {
                if (!ToolSet(*this, 0, Tool::walkDown))
                {
                    ShowGridlines();
                    SetPatrolAreaToRender(_staffId);
                    GfxInvalidateScreen();
                }
            }
        }

        void InputSize()
        {
            Formatter ft;
            ft.Add<uint16_t>(kLandToolMinimumSize);
            ft.Add<uint16_t>(kLandToolMaximumSize);
            WindowTextInputOpen(
                this, WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, ft, kStringIdNone, kStringIdNone, 3);
        }

        bool PatrolAreaToolIsActive()
        {
            return isToolActive(WindowClass::patrolArea);
        }

        bool IsStaffWindowOpen()
        {
            // If staff window for this patrol area was closed, tool is no longer active
            auto* windowMgr = GetWindowManager();
            auto staffWindow = windowMgr->FindByNumber(WindowClass::peep, _staffId);
            return staffWindow != nullptr;
        }

        std::optional<CoordsXY> GetBestCoordsFromPos(const ScreenCoordsXY& pos)
        {
            auto coords = FootpathGetCoordinatesFromPos(pos, nullptr, nullptr);
            return coords.IsNull() ? std::nullopt : std::make_optional(coords);
        }
    };

    WindowBase* PatrolAreaOpen(EntityId staffId)
    {
        auto* windowMgr = GetWindowManager();
        auto* w = windowMgr->FocusOrCreate<PatrolAreaWindow>(
            WindowClass::patrolArea, ScreenCoordsXY(ContextGetWidth() - kWindowSize.width, 29), kWindowSize, {});
        if (w != nullptr)
        {
            w->setStaffId(staffId);
        }
        return w;
    }

    EntityId WindowPatrolAreaGetCurrentStaffId()
    {
        auto* windowMgr = GetWindowManager();
        auto current = reinterpret_cast<PatrolAreaWindow*>(windowMgr->FindByClass(WindowClass::patrolArea));
        return current != nullptr ? current->GetStaffId() : EntityId::GetNull();
    }
} // namespace OpenRCT2::Ui::Windows
