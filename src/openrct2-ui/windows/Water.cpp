/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/WaterLowerAction.h>
#include <openrct2/actions/WaterRaiseAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_WATER;
    static constexpr ScreenSize kWindowSize = { 76, 77 };

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
    static constexpr auto _waterWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget     ({16, 17}, {44, 32}, WidgetType::imgBtn, WindowColour::primary , ImageId(SPR_LAND_TOOL_SIZE_0),   kStringIdNone),            // preview box
        makeRemapWidget({17, 18}, {16, 16}, WidgetType::trnBtn, WindowColour::tertiary, SPR_LAND_TOOL_DECREASE, STR_ADJUST_SMALLER_WATER_TIP), // decrement size
        makeRemapWidget({43, 32}, {16, 16}, WidgetType::trnBtn, WindowColour::tertiary, SPR_LAND_TOOL_INCREASE, STR_ADJUST_LARGER_WATER_TIP) // increment size
    );
    // clang-format on

    class WaterWindow final : public Window
    {
    private:
        money64 _waterToolRaiseCost = kMoney64Undefined;
        money64 _waterToolLowerCost = kMoney64Undefined;

    public:
        void onOpen() override
        {
            setWidgets(_waterWidgets);

            holdDownWidgets = (1uLL << WIDX_INCREMENT) | (1uLL << WIDX_DECREMENT);
            WindowInitScrollWidgets(*this);
            WindowPushOthersBelow(*this);

            gLandToolSize = 1;
        }

        void onClose() override
        {
            // If the tool wasn't changed, turn tool off
            if (isToolActive(WindowClass::water, WIDX_BACKGROUND))
            {
                ToolCancel();
            }
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
                    // Decrement land tool size
                    gLandToolSize = std::max<uint16_t>(kLandToolMinimumSize, gLandToolSize - 1);

                    // Invalidate the window
                    invalidate();
                    break;
                case WIDX_INCREMENT:
                    // Increment land tool size
                    gLandToolSize = std::min<uint16_t>(kLandToolMaximumSize, gLandToolSize + 1);

                    // Invalidate the window
                    invalidate();
                    break;
            }
        }

        void onUpdate() override
        {
            // Close window if another tool is open
            if (!isToolActive(WindowClass::water, WIDX_BACKGROUND))
            {
                close();
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
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

                invalidate();
            }
        }

        void onPrepareDraw() override
        {
            // Set the preview image button to be pressed down
            setWidgetPressed(WIDX_PREVIEW, true);

            // Update the preview image
            widgets[WIDX_PREVIEW].image = ImageId(LandTool::SizeToSpriteIndex(gLandToolSize));
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            auto screenCoords = ScreenCoordsXY{ windowPos.x + widgets[WIDX_PREVIEW].midX(),
                                                windowPos.y + widgets[WIDX_PREVIEW].midY() };

            drawWidgets(rt);
            // Draw number for tool sizes bigger than 7
            if (gLandToolSize > kLandToolMaximumSizeWithSprite)
            {
                auto ft = Formatter();
                ft.Add<uint16_t>(gLandToolSize);
                DrawTextBasic(
                    rt, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::centre });
            }

            if (!(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
            {
                // Draw raise cost amount
                screenCoords = { widgets[WIDX_PREVIEW].midX() + windowPos.x, widgets[WIDX_PREVIEW].bottom + windowPos.y + 5 };
                if (_waterToolRaiseCost != kMoney64Undefined && _waterToolRaiseCost != 0)
                {
                    auto ft = Formatter();
                    ft.Add<money64>(_waterToolRaiseCost);
                    DrawTextBasic(rt, screenCoords, STR_RAISE_COST_AMOUNT, ft, { TextAlignment::centre });
                }
                screenCoords.y += 10;

                // Draw lower cost amount
                if (_waterToolLowerCost != kMoney64Undefined && _waterToolLowerCost != 0)
                {
                    auto ft = Formatter();
                    ft.Add<money64>(_waterToolLowerCost);
                    DrawTextBasic(rt, screenCoords, STR_LOWER_COST_AMOUNT, ft, { TextAlignment::centre });
                }
            }
        }

        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    ToolUpdateWater(screenCoords);
                    break;
            }
        }

        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    if (gMapSelectFlags.has(MapSelectFlag::enable))
                    {
                        gCurrentToolId = Tool::upDownArrow;
                    }
                    break;
            }
        }

        void onToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    WaterToolDrag(screenCoords);
                    break;
            }
        }

        void onToolUp(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    gMapSelectFlags.unset(MapSelectFlag::enable);
                    gCurrentToolId = Tool::waterDown;
                    break;
            }
        }

        void onToolAbort(WidgetIndex widgetIndex) override
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

            auto& gameState = getGameState();

            if (offsetPos.y <= dx)
            {
                gInputDragLast.y += dx;

                auto waterRaiseAction = GameActions::WaterRaiseAction(
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
                GameActions::Execute(&waterRaiseAction, gameState);

                _waterToolRaiseCost = kMoney64Undefined;
                _waterToolLowerCost = kMoney64Undefined;

                return;
            }

            dx = -dx;

            if (offsetPos.y >= dx)
            {
                gInputDragLast.y += dx;

                auto waterLowerAction = GameActions::WaterLowerAction(
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
                GameActions::Execute(&waterLowerAction, gameState);
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
            auto* windowMgr = GetWindowManager();
            auto& gameState = getGameState();

            if (gCurrentToolId == Tool::upDownArrow)
            {
                if (!gMapSelectFlags.has(MapSelectFlag::enable))
                    return;

                auto waterLowerAction = GameActions::WaterLowerAction(
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
                waterLowerAction.SetFlags({ GameActions::CommandFlag::allowDuringPaused });
                auto waterRaiseAction = GameActions::WaterRaiseAction(
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
                waterRaiseAction.SetFlags({ GameActions::CommandFlag::allowDuringPaused });

                auto res = GameActions::Query(&waterLowerAction, gameState);
                money64 lowerCost = res.error == GameActions::Status::ok ? res.cost : kMoney64Undefined;

                res = GameActions::Query(&waterRaiseAction, gameState);
                money64 raiseCost = res.error == GameActions::Status::ok ? res.cost : kMoney64Undefined;

                if (_waterToolRaiseCost != raiseCost || _waterToolLowerCost != lowerCost)
                {
                    _waterToolRaiseCost = raiseCost;
                    _waterToolLowerCost = lowerCost;
                    windowMgr->InvalidateByClass(WindowClass::water);
                }
                return;
            }

            gMapSelectFlags.unset(MapSelectFlag::enable);

            auto info = GetMapCoordinatesFromPos(
                screenPos, EnumsToFlags(ViewportInteractionItem::terrain, ViewportInteractionItem::water));

            if (info.interactionType == ViewportInteractionItem::none)
            {
                if (_waterToolRaiseCost != kMoney64Undefined || _waterToolLowerCost != kMoney64Undefined)
                {
                    _waterToolRaiseCost = kMoney64Undefined;
                    _waterToolLowerCost = kMoney64Undefined;
                    windowMgr->InvalidateByClass(WindowClass::water);
                }
                return;
            }

            auto mapTile = info.Loc;

            uint8_t state_changed = 0;

            if (!gMapSelectFlags.has(MapSelectFlag::enable))
            {
                gMapSelectFlags.set(MapSelectFlag::enable);
                state_changed++;
            }

            if (gMapSelectType != MapSelectType::fullWater)
            {
                gMapSelectType = MapSelectType::fullWater;
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

            if (!state_changed)
                return;

            auto waterLowerAction = GameActions::WaterLowerAction(
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
            waterLowerAction.SetFlags({ GameActions::CommandFlag::allowDuringPaused });
            auto waterRaiseAction = GameActions::WaterRaiseAction(
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y });
            waterRaiseAction.SetFlags({ GameActions::CommandFlag::allowDuringPaused });

            auto res = GameActions::Query(&waterLowerAction, gameState);
            money64 lowerCost = res.error == GameActions::Status::ok ? res.cost : kMoney64Undefined;

            res = GameActions::Query(&waterRaiseAction, gameState);
            money64 raiseCost = res.error == GameActions::Status::ok ? res.cost : kMoney64Undefined;

            if (_waterToolRaiseCost != raiseCost || _waterToolLowerCost != lowerCost)
            {
                _waterToolRaiseCost = raiseCost;
                _waterToolLowerCost = lowerCost;
                windowMgr->InvalidateByClass(WindowClass::water);
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
        return windowMgr->FocusOrCreate<WaterWindow>(
            WindowClass::water, ScreenCoordsXY(ContextGetWidth() - kWindowSize.width, 29), kWindowSize, {});
    }

    /**
     *
     *  rct2: 0x0066CD9C
     */
    void ToggleWaterWindow()
    {
        if (isToolActive(WindowClass::water, WIDX_BACKGROUND))
        {
            ToolCancel();
        }
        else
        {
            ShowGridlines();
            auto* toolWindow = ContextOpenWindow(WindowClass::water);
            ToolSet(*toolWindow, WIDX_BACKGROUND, Tool::waterDown);
            gInputFlags.set(InputFlag::allowRightMouseRemoval);
        }
    }
} // namespace OpenRCT2::Ui::Windows
