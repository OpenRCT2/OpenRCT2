/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/actions/StaffSetPatrolAreaAction.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/PatrolArea.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = STR_SET_PATROL_AREA;
    static constexpr int32_t WH = 54;
    static constexpr int32_t WW = 104;

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
    static Widget PatrolAreaWidgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget     ({27, 17}, {44, 32}, WindowWidgetType::ImgBtn,  WindowColour::Primary , ImageId(SPR_LAND_TOOL_SIZE_0)                                  ), // preview box
        MakeRemapWidget({28, 18}, {16, 16}, WindowWidgetType::TrnBtn,  WindowColour::Tertiary, SPR_LAND_TOOL_DECREASE,      STR_ADJUST_SMALLER_PATROL_AREA_TIP), // decrement size
        MakeRemapWidget({54, 32}, {16, 16}, WindowWidgetType::TrnBtn,  WindowColour::Tertiary, SPR_LAND_TOOL_INCREASE,      STR_ADJUST_LARGER_PATROL_AREA_TIP ), // increment size
        kWidgetsEnd,
    };
    // clang-format on

    class PatrolAreaWindow final : public Window
    {
    public:
        void OnOpen() override
        {
            widgets = PatrolAreaWidgets;
            hold_down_widgets = (1uLL << WIDX_INCREMENT) | (1uLL << WIDX_DECREMENT);
            WindowInitScrollWidgets(*this);
            WindowPushOthersBelow(*this);
            gLandToolSize = 4;
        }

        void OnClose() override
        {
            // If the tool wasn't changed, turn tool off
            if (PatrolAreaToolIsActive())
                ToolCancel();
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_PREVIEW:
                    InputSize();
                    break;
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_DECREMENT:
                    gLandToolSize = std::max<uint16_t>(kLandToolMinimumSize, gLandToolSize - 1);
                    Invalidate();
                    break;
                case WIDX_INCREMENT:
                    gLandToolSize = std::min<uint16_t>(kLandToolMaximumSize, gLandToolSize + 1);
                    Invalidate();
                    break;
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (text.empty())
                return;

            if (widgetIndex != WIDX_PREVIEW)
                return;

            const auto res = String::Parse<int32_t>(text);
            if (res.has_value())
            {
                int32_t size;
                size = res.value();
                size = std::max<uint16_t>(kLandToolMinimumSize, size);
                size = std::min<uint16_t>(kLandToolMaximumSize, size);
                gLandToolSize = size;
                Invalidate();
            }
        }

        void OnUpdate() override
        {
            // Close window if another tool is open or staff window gets closed
            if (!PatrolAreaToolIsActive() || !IsStaffWindowOpen())
            {
                Close();
            }
        }

        void OnPrepareDraw() override
        {
            SetWidgetPressed(WIDX_PREVIEW, true);
            PatrolAreaWidgets[WIDX_PREVIEW].image = ImageId(LandTool::SizeToSpriteIndex(gLandToolSize));
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);

            // Draw number for tool sizes bigger than 7
            if (gLandToolSize > kLandToolMaximumSizeWithSprite)
            {
                auto screenCoords = ScreenCoordsXY{ windowPos.x + PatrolAreaWidgets[WIDX_PREVIEW].midX(),
                                                    windowPos.y + PatrolAreaWidgets[WIDX_PREVIEW].midY() };
                auto ft = Formatter();
                ft.Add<uint16_t>(gLandToolSize);
                DrawTextBasic(
                    dpi, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::CENTRE });
            }
        }

        void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto mapTile = GetBestCoordsFromPos(screenCoords);
            if (!mapTile)
                return;

            auto stateChanged = false;
            if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
                stateChanged = true;

            if (gMapSelectType != MAP_SELECT_TYPE_FULL)
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
                // Invalidate previous area
                MapInvalidateSelectionRect();

                // Update and invalidate new area
                gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
                gMapSelectType = MAP_SELECT_TYPE_FULL;
                gMapSelectPositionA = posA;
                gMapSelectPositionB = posB;
                MapInvalidateSelectionRect();
            }
        }

        void OnToolAbort(WidgetIndex widgetIndex) override
        {
            HideGridlines();
            ClearPatrolAreaToRender();
            GfxInvalidateScreen();
        }

        void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto mapTile = GetBestCoordsFromPos(screenCoords);
            if (mapTile)
            {
                auto staff = GetEntity<Staff>(_staffId);
                if (staff != nullptr)
                {
                    _mode = staff->IsPatrolAreaSet(*mapTile) ? StaffSetPatrolAreaMode::Unset : StaffSetPatrolAreaMode::Set;
                }
            }

            OnToolDrag(widgetIndex, screenCoords);
        }

        void OnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto staff = GetEntity<Staff>(_staffId);
            if (staff != nullptr)
            {
                MapRange range(gMapSelectPositionA, gMapSelectPositionB);
                auto staffSetPatrolAreaAction = StaffSetPatrolAreaAction(_staffId, range, _mode);
                GameActions::Execute(&staffSetPatrolAreaAction);
            }
        }

        EntityId GetStaffId() const
        {
            return _staffId;
        }

        void SetStaffId(EntityId staffId)
        {
            _staffId = staffId;
            EnableTool();
        }

    private:
        EntityId _staffId;
        StaffSetPatrolAreaMode _mode;

        void EnableTool()
        {
            if (PatrolAreaToolIsActive())
            {
                SetPatrolAreaToRender(_staffId);
                GfxInvalidateScreen();
            }
            else
            {
                if (!ToolSet(*this, 0, Tool::WalkDown))
                {
                    ShowGridlines();
                    InputSetFlag(INPUT_FLAG_6, true);
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
            WindowTextInputOpen(this, WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, ft, STR_NONE, STR_NONE, 3);
        }

        bool PatrolAreaToolIsActive()
        {
            return isToolActive(WindowClass::PatrolArea);
        }

        bool IsStaffWindowOpen()
        {
            // If staff window for this patrol area was closed, tool is no longer active
            auto* windowMgr = GetContext()->GetUiContext()->GetWindowManager();
            auto staffWindow = windowMgr->FindByNumber(WindowClass::Peep, _staffId);
            return staffWindow != nullptr;
        }

        std::optional<CoordsXY> GetBestCoordsFromPos(const ScreenCoordsXY& pos)
        {
            auto coords = FootpathGetCoordinatesFromPos(pos, nullptr, nullptr);
            return coords.IsNull() ? std::nullopt : std::make_optional(coords);
        }

        void OnResize() override
        {
            ResizeFrame();
        }
    };

    WindowBase* PatrolAreaOpen(EntityId staffId)
    {
        auto w = WindowFocusOrCreate<PatrolAreaWindow>(
            WindowClass::PatrolArea, ScreenCoordsXY(ContextGetWidth() - WW, 29), WW, WH, 0);
        if (w != nullptr)
        {
            w->SetStaffId(staffId);
        }
        return w;
    }

    EntityId WindowPatrolAreaGetCurrentStaffId()
    {
        auto* windowMgr = GetContext()->GetUiContext()->GetWindowManager();
        auto current = reinterpret_cast<PatrolAreaWindow*>(windowMgr->FindByClass(WindowClass::PatrolArea));
        return current != nullptr ? current->GetStaffId() : EntityId::GetNull();
    }
} // namespace OpenRCT2::Ui::Windows
