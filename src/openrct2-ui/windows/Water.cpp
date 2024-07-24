/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = STR_WATER;
    static constexpr int32_t WH = 77;
    static constexpr int32_t WW = 76;

    // clang-format off
enum WindowWaterWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PREVIEW,
    WIDX_DECREMENT,
    WIDX_INCREMENT
};

static Widget _waterWidgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget     ({16, 17}, {44, 32}, WindowWidgetType::ImgBtn, WindowColour::Primary , ImageId(SPR_LAND_TOOL_SIZE_0),   STR_NONE),                     // preview box
    MakeRemapWidget({17, 18}, {16, 16}, WindowWidgetType::TrnBtn, WindowColour::Tertiary, SPR_LAND_TOOL_DECREASE, STR_ADJUST_SMALLER_WATER_TIP), // decrement size
    MakeRemapWidget({43, 32}, {16, 16}, WindowWidgetType::TrnBtn, WindowColour::Tertiary, SPR_LAND_TOOL_INCREASE, STR_ADJUST_LARGER_WATER_TIP),  // increment size
    kWidgetsEnd,
};
    // clang-format on

    class WaterWindow final : public Window
    {
    public:
        void OnOpen() override
        {
            widgets = _waterWidgets;
            hold_down_widgets = (1uLL << WIDX_INCREMENT) | (1uLL << WIDX_DECREMENT);
            WindowInitScrollWidgets(*this);
            WindowPushOthersBelow(*this);

            gLandToolSize = 1;
            gWaterToolRaiseCost = kMoney64Undefined;
            gWaterToolLowerCost = kMoney64Undefined;
        }

        void OnClose() override
        {
            // If the tool wasn't changed, turn tool off
            if (WaterToolIsActive())
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
            if (!WaterToolIsActive())
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
                if (gWaterToolRaiseCost != kMoney64Undefined && gWaterToolRaiseCost != 0)
                {
                    auto ft = Formatter();
                    ft.Add<money64>(gWaterToolRaiseCost);
                    DrawTextBasic(dpi, screenCoords, STR_RAISE_COST_AMOUNT, ft, { TextAlignment::CENTRE });
                }
                screenCoords.y += 10;

                // Draw lower cost amount
                if (gWaterToolLowerCost != kMoney64Undefined && gWaterToolLowerCost != 0)
                {
                    auto ft = Formatter();
                    ft.Add<money64>(gWaterToolLowerCost);
                    DrawTextBasic(dpi, screenCoords, STR_LOWER_COST_AMOUNT, ft, { TextAlignment::CENTRE });
                }
            }
        }

        void OnResize() override
        {
            ResizeFrame();
        }

    private:
        void InputSize()
        {
            Formatter ft;
            ft.Add<uint16_t>(kLandToolMinimumSize);
            ft.Add<uint16_t>(kLandToolMaximumSize);
            WindowTextInputOpen(this, WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, ft, STR_NONE, STR_NONE, 3);
        }
    };

    WindowBase* WaterOpen()
    {
        return WindowFocusOrCreate<WaterWindow>(WindowClass::Water, ScreenCoordsXY(ContextGetWidth() - WW, 29), WW, WH, 0);
    }
} // namespace OpenRCT2::Ui::Windows
