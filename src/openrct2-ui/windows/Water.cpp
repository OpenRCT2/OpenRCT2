/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/world/Park.h>

static constexpr const StringId WINDOW_TITLE = STR_WATER;
static constexpr const int32_t WH = 77;
static constexpr const int32_t WW = 76;

// clang-format off
enum WindowWaterWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PREVIEW,
    WIDX_DECREMENT,
    WIDX_INCREMENT
};

static rct_widget window_water_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget     ({16, 17}, {44, 32}, WindowWidgetType::ImgBtn, WindowColour::Primary , SPR_LAND_TOOL_SIZE_0,   STR_NONE),                     // preview box
    MakeRemapWidget({17, 18}, {16, 16}, WindowWidgetType::TrnBtn, WindowColour::Tertiary, SPR_LAND_TOOL_DECREASE, STR_ADJUST_SMALLER_WATER_TIP), // decrement size
    MakeRemapWidget({43, 32}, {16, 16}, WindowWidgetType::TrnBtn, WindowColour::Tertiary, SPR_LAND_TOOL_INCREASE, STR_ADJUST_LARGER_WATER_TIP),  // increment size
    WIDGETS_END,
};
// clang-format on

class WaterWindow final : public Window
{
public:
    void OnOpen() override
    {
        widgets = window_water_widgets;
        hold_down_widgets = (1uLL << WIDX_INCREMENT) | (1uLL << WIDX_DECREMENT);
        WindowInitScrollWidgets(*this);
        window_push_others_below(*this);

        gLandToolSize = 1;
        gWaterToolRaiseCost = MONEY64_UNDEFINED;
        gWaterToolLowerCost = MONEY64_UNDEFINED;
    }

    void OnClose() override
    {
        // If the tool wasn't changed, turn tool off
        if (WaterToolIsActive())
        {
            tool_cancel();
        }
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                if (gInputPlaceObjectModifier & PLACE_OBJECT_MODIFIER_SHIFT_Z)
                {
                    CloseAllWindowsExceptNumberAndClass(number, classification);
                }
                else
                {
                    Close();
                }
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
                gLandToolSize = std::max(MINIMUM_TOOL_SIZE, gLandToolSize - 1);

                // Invalidate the window
                Invalidate();
                break;
            case WIDX_INCREMENT:
                // Increment land tool size
                gLandToolSize = std::min(MAXIMUM_TOOL_SIZE, gLandToolSize + 1);

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
            size = std::max(MINIMUM_TOOL_SIZE, size);
            size = std::min(MAXIMUM_TOOL_SIZE, size);
            gLandToolSize = size;

            Invalidate();
        }
    }

    void OnPrepareDraw() override
    {
        // Set the preview image button to be pressed down
        SetWidgetPressed(WIDX_PREVIEW, true);

        // Update the preview image
        widgets[WIDX_PREVIEW].image = LandTool::SizeToSpriteIndex(gLandToolSize);
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        auto screenCoords = ScreenCoordsXY{ windowPos.x + window_water_widgets[WIDX_PREVIEW].midX(),
                                            windowPos.y + window_water_widgets[WIDX_PREVIEW].midY() };

        DrawWidgets(dpi);
        // Draw number for tool sizes bigger than 7
        if (gLandToolSize > MAX_TOOL_SIZE_WITH_SPRITE)
        {
            auto ft = Formatter();
            ft.Add<uint16_t>(gLandToolSize);
            DrawTextBasic(&dpi, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::CENTRE });
        }

        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            // Draw raise cost amount
            screenCoords = { window_water_widgets[WIDX_PREVIEW].midX() + windowPos.x,
                             window_water_widgets[WIDX_PREVIEW].bottom + windowPos.y + 5 };
            if (gWaterToolRaiseCost != MONEY64_UNDEFINED && gWaterToolRaiseCost != 0)
            {
                auto ft = Formatter();
                ft.Add<money64>(gWaterToolRaiseCost);
                DrawTextBasic(&dpi, screenCoords, STR_RAISE_COST_AMOUNT, ft, { TextAlignment::CENTRE });
            }
            screenCoords.y += 10;

            // Draw lower cost amount
            if (gWaterToolLowerCost != MONEY64_UNDEFINED && gWaterToolLowerCost != 0)
            {
                auto ft = Formatter();
                ft.Add<money64>(gWaterToolLowerCost);
                DrawTextBasic(&dpi, screenCoords, STR_LOWER_COST_AMOUNT, ft, { TextAlignment::CENTRE });
            }
        }
    }

private:
    void InputSize()
    {
        Formatter ft;
        ft.Add<int16_t>(MINIMUM_TOOL_SIZE);
        ft.Add<int16_t>(MAXIMUM_TOOL_SIZE);
        WindowTextInputOpen(this, WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, ft, STR_NONE, STR_NONE, 3);
    }
};

rct_window* WindowWaterOpen()
{
    return WindowFocusOrCreate<WaterWindow>(WindowClass::Water, ScreenCoordsXY(ContextGetWidth() - WW, 29), WW, WH, 0);
}
