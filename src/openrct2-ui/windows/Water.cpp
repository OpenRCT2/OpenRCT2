/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/input/MouseInput.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/actions/WaterLowerAction.h>
#include <openrct2/actions/WaterRaiseAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = STR_WATER;
    static constexpr int32_t WH = 77;
    static constexpr int32_t WW = 76;

    enum WindowWaterWidgetIdx : WidgetIndex
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_PREVIEW,
        WIDX_DECREMENT,
        WIDX_INCREMENT
    };

    // clang-format off
    static constexpr Widget _waterWidgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget     ({16, 17}, {44, 32}, WindowWidgetType::ImgBtn, WindowColour::Primary , ImageId(SPR_LAND_TOOL_SIZE_0),   kStringIdNone),            // preview box
        MakeRemapWidget({17, 18}, {16, 16}, WindowWidgetType::TrnBtn, WindowColour::Tertiary, SPR_LAND_TOOL_DECREASE, STR_ADJUST_SMALLER_WATER_TIP), // decrement size
        MakeRemapWidget({43, 32}, {16, 16}, WindowWidgetType::TrnBtn, WindowColour::Tertiary, SPR_LAND_TOOL_INCREASE, STR_ADJUST_LARGER_WATER_TIP),  // increment size
    };
    // clang-format on

    class WaterWindow final : public Window
    {
    private:
        money64 _waterToolRaiseCost = kMoney64Undefined;
        money64 _waterToolLowerCost = kMoney64Undefined;

    public:
        void OnOpen() override
        {
            SetWidgets(_waterWidgets);
            hold_down_widgets = (1uLL << WIDX_INCREMENT) | (1uLL << WIDX_DECREMENT);
            WindowInitScrollWidgets(*this);
            WindowPushOthersBelow(*this);

            gLandToolSize = 1;
        }

        void OnClose() override
        {
            // If the tool wasn't changed, turn tool off
            if (isToolActive(WindowClass::Water, WIDX_BACKGROUND))
            {
                ToolCancel();
            }
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
                    // Decrement land tool size
                    gLandToolSize = std::max<uint16_t>(kLandToolMinimumSize, gLandToolSize - 1);

                    // Invalidate the window
                    Invalidate();
                    break;
                case WIDX_INCREMENT:
                    // Increment land tool size
                    gLandToolSize = std::min<uint16_t>(kLandToolMaximumSize, gLandToolSize + 1);

                    // Invalidate the window
                    Invalidate();
                    break;
            }
        }

        void OnUpdate() override
        {
            // Close window if another tool is open
            if (!isToolActive(WindowClass::Water, WIDX_BACKGROUND))
            {
                Close();
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            int32_t size;
            char* end;

            if (widgetIndex != WIDX_PREVIEW)
            {
                return;
            }

            std::string textStr = std::string(text);
            size = strtol(textStr.c_str(), &end, 10);
            if (*end == '\0')
            {
                size = std::max<uint16_t>(kLandToolMinimumSize, size);
                size = std::min<uint16_t>(kLandToolMaximumSize, size);
                gLandToolSize = size;

                Invalidate();
            }
        }

        void OnPrepareDraw() override
        {
            // Set the preview image button to be pressed down
            SetWidgetPressed(WIDX_PREVIEW, true);

            // Update the preview image
            widgets[WIDX_PREVIEW].image = ImageId(LandTool::SizeToSpriteIndex(gLandToolSize));
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            auto screenCoords = ScreenCoordsXY{ windowPos.x + widgets[WIDX_PREVIEW].midX(),
                                                windowPos.y + widgets[WIDX_PREVIEW].midY() };

            DrawWidgets(dpi);
            // Draw number for tool sizes bigger than 7
            if (gLandToolSize > kLandToolMaximumSizeWithSprite)
            {
                auto ft = Formatter();
                ft.Add<uint16_t>(gLandToolSize);
                DrawTextBasic(
                    dpi, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::CENTRE });
            }

            if (!(GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY))
            {
                // Draw raise cost amount
                screenCoords = { widgets[WIDX_PREVIEW].midX() + windowPos.x, widgets[WIDX_PREVIEW].bottom + windowPos.y + 5 };
                if (_waterToolRaiseCost != kMoney64Undefined && _waterToolRaiseCost != 0)
                {
                    auto ft = Formatter();
                    ft.Add<money64>(_waterToolRaiseCost);
                    DrawTextBasic(dpi, screenCoords, STR_RAISE_COST_AMOUNT, ft, { TextAlignment::CENTRE });
                }
                screenCoords.y += 10;

                // Draw lower cost amount
                if (_waterToolLowerCost != kMoney64Undefined && _waterToolLowerCost != 0)
                {
                    auto ft = Formatter();
                    ft.Add<money64>(_waterToolLowerCost);
                    DrawTextBasic(dpi, screenCoords, STR_LOWER_COST_AMOUNT, ft, { TextAlignment::CENTRE });
                }
            }
        }

        void OnResize() override
        {
            ResizeFrame();
        }

        void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    ToolUpdateWater(screenCoords);
                    break;
            }
        }

        void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
                    {
                        gCurrentToolId = Tool::UpDownArrow;
                    }
                    break;
            }
        }

        void OnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    WaterToolDrag(screenCoords);
                    break;
            }
        }

        void OnToolUp(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    MapInvalidateSelectionRect();
                    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
                    gCurrentToolId = Tool::WaterDown;
                    break;
            }
        }

        void OnToolAbort(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    HideGridlines();
                    break;
            }
        }

    private:
        /**
         *  part of window_top_toolbar_tool_drag(0x0066CB4E)
         *  rct2: 0x006E6D4B
         */
        void WaterToolDrag(const ScreenCoordsXY& screenPos)
        {
            auto* windowMgr = GetWindowManager();
            auto* window = windowMgr->FindFromPoint(screenPos);
            if (window == nullptr || window->viewport == nullptr)
                return;

            int16_t dx = window->viewport->zoom.ApplyInversedTo(-16);

            auto offsetPos = screenPos - ScreenCoordsXY{ 0, gInputDragLast.y };

            if (offsetPos.y <= dx)
            {
                gInputDragLast.y += dx;

                auto waterRaiseAction = WaterRaiseAction(
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
                GameActions::Execute(&waterRaiseAction);

                _waterToolRaiseCost = kMoney64Undefined;
                _waterToolLowerCost = kMoney64Undefined;

                return;
            }

            dx = -dx;

            if (offsetPos.y >= dx)
            {
                gInputDragLast.y += dx;

                auto waterLowerAction = WaterLowerAction(
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
                GameActions::Execute(&waterLowerAction);
                _waterToolRaiseCost = kMoney64Undefined;
                _waterToolLowerCost = kMoney64Undefined;

                return;
            }
        }

        /**
         *
         *  rct2: 0x006E6BDC
         */
        void ToolUpdateWater(const ScreenCoordsXY& screenPos)
        {
            MapInvalidateSelectionRect();

            auto* windowMgr = Ui::GetWindowManager();

            if (gCurrentToolId == Tool::UpDownArrow)
            {
                if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
                    return;

                auto waterLowerAction = WaterLowerAction(
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
                auto waterRaiseAction = WaterRaiseAction(
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });

                auto res = GameActions::Query(&waterLowerAction);
                money64 lowerCost = res.Error == GameActions::Status::Ok ? res.Cost : kMoney64Undefined;

                res = GameActions::Query(&waterRaiseAction);
                money64 raiseCost = res.Error == GameActions::Status::Ok ? res.Cost : kMoney64Undefined;

                if (_waterToolRaiseCost != raiseCost || _waterToolLowerCost != lowerCost)
                {
                    _waterToolRaiseCost = raiseCost;
                    _waterToolLowerCost = lowerCost;
                    windowMgr->InvalidateByClass(WindowClass::Water);
                }
                return;
            }

            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;

            auto info = GetMapCoordinatesFromPos(
                screenPos, EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Water));

            if (info.interactionType == ViewportInteractionItem::None)
            {
                if (_waterToolRaiseCost != kMoney64Undefined || _waterToolLowerCost != kMoney64Undefined)
                {
                    _waterToolRaiseCost = kMoney64Undefined;
                    _waterToolLowerCost = kMoney64Undefined;
                    windowMgr->InvalidateByClass(WindowClass::Water);
                }
                return;
            }

            auto mapTile = info.Loc;

            uint8_t state_changed = 0;

            if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
            {
                gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
                state_changed++;
            }

            if (gMapSelectType != MAP_SELECT_TYPE_FULL_WATER)
            {
                gMapSelectType = MAP_SELECT_TYPE_FULL_WATER;
                state_changed++;
            }

            uint16_t tool_size = std::max<uint16_t>(1, gLandToolSize);
            uint16_t tool_length = (tool_size - 1) * kCoordsXYStep;

            // Move to tool bottom left
            mapTile.x -= tool_length / 2;
            mapTile.y -= tool_length / 2;
            mapTile = mapTile.ToTileStart();

            if (gMapSelectPositionA.x != mapTile.x)
            {
                gMapSelectPositionA.x = mapTile.x;
                state_changed++;
            }

            if (gMapSelectPositionA.y != mapTile.y)
            {
                gMapSelectPositionA.y = mapTile.y;
                state_changed++;
            }

            mapTile.x += tool_length;
            mapTile.y += tool_length;

            if (gMapSelectPositionB.x != mapTile.x)
            {
                gMapSelectPositionB.x = mapTile.x;
                state_changed++;
            }

            if (gMapSelectPositionB.y != mapTile.y)
            {
                gMapSelectPositionB.y = mapTile.y;
                state_changed++;
            }

            MapInvalidateSelectionRect();
            if (!state_changed)
                return;

            auto waterLowerAction = WaterLowerAction(
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
            auto waterRaiseAction = WaterRaiseAction(
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });

            auto res = GameActions::Query(&waterLowerAction);
            money64 lowerCost = res.Error == GameActions::Status::Ok ? res.Cost : kMoney64Undefined;

            res = GameActions::Query(&waterRaiseAction);
            money64 raiseCost = res.Error == GameActions::Status::Ok ? res.Cost : kMoney64Undefined;

            if (_waterToolRaiseCost != raiseCost || _waterToolLowerCost != lowerCost)
            {
                _waterToolRaiseCost = raiseCost;
                _waterToolLowerCost = lowerCost;
                windowMgr->InvalidateByClass(WindowClass::Water);
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
    };

    WindowBase* WaterOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<WaterWindow>(WindowClass::Water, ScreenCoordsXY(ContextGetWidth() - WW, 29), WW, WH, 0);
    }

    /**
     *
     *  rct2: 0x0066CD9C
     */
    void ToggleWaterWindow()
    {
        if (isToolActive(WindowClass::Water, WIDX_BACKGROUND))
        {
            ToolCancel();
        }
        else
        {
            ShowGridlines();
            auto* toolWindow = ContextOpenWindow(WindowClass::Water);
            ToolSet(*toolWindow, WIDX_BACKGROUND, Tool::WaterDown);
            InputSetFlag(INPUT_FLAG_6, true);
        }
    }
} // namespace OpenRCT2::Ui::Windows
