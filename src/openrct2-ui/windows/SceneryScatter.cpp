/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Scenery.h>

namespace OpenRCT2::Ui::Windows
{
    enum WindowSceneryScatterWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_PREVIEW,
        WIDX_DECREMENT,
        WIDX_INCREMENT,
        WIDX_DENSITY,
        WIDX_DENSITY_LOW,
        WIDX_DENSITY_MEDIUM,
        WIDX_DENSITY_HIGH
    };

    bool gWindowSceneryScatterEnabled = false;
    uint16_t gWindowSceneryScatterSize;
    ScatterToolDensity gWindowSceneryScatterDensity;

    static constexpr ScreenSize kWindowSize = { 86, 100 };

    // clang-format off
    static constexpr auto _sceneryScatterWidgets = makeWidgets(
        makeWindowShim(STR_SCENERY_SCATTER, kWindowSize),

        makeWidget     ({20, 17}, {44,  32}, WidgetType::imgBtn,   WindowColour::secondary, ImageId(SPR_LAND_TOOL_SIZE_0)                                 ), // preview box
        makeRemapWidget({21, 18}, {16,  16}, WidgetType::trnBtn,   WindowColour::secondary, SPR_LAND_TOOL_DECREASE,        STR_ADJUST_SMALLER_LAND_TIP    ), // decrement size
        makeRemapWidget({47, 32}, {16,  16}, WidgetType::trnBtn,   WindowColour::secondary, SPR_LAND_TOOL_INCREASE,        STR_ADJUST_LARGER_LAND_TIP     ), // increment size

        makeWidget     ({ 3, 55}, {80,  42}, WidgetType::groupbox, WindowColour::secondary, STR_SCATTER_TOOL_DENSITY                                      ),
        makeRemapWidget({ 7, 68}, {24,  24}, WidgetType::flatBtn,  WindowColour::secondary, SPR_G2_SCENERY_SCATTER_LOW,    STR_SCATTER_TOOL_DENSITY_LOW   ), // low amount
        makeRemapWidget({31, 68}, {24,  24}, WidgetType::flatBtn,  WindowColour::secondary, SPR_G2_SCENERY_SCATTER_MEDIUM, STR_SCATTER_TOOL_DENSITY_MEDIUM), // medium amount
        makeRemapWidget({55, 68}, {24,  24}, WidgetType::flatBtn,  WindowColour::secondary, SPR_G2_SCENERY_SCATTER_HIGH,   STR_SCATTER_TOOL_DENSITY_HIGH  )  // high amount
    );
    // clang-format on

    class SceneryScatterWindow final : public Window
    {
    public:
        void onOpen() override
        {
            setWidgets(_sceneryScatterWidgets);

            holdDownWidgets = (1uLL << WIDX_INCREMENT) | (1uLL << WIDX_DECREMENT);
            WindowInitScrollWidgets(*this);
            WindowPushOthersBelow(*this);

            gWindowSceneryScatterEnabled = true;
            gWindowSceneryScatterSize = 16;
            gWindowSceneryScatterDensity = ScatterToolDensity::MediumDensity;
        }

        void onClose() override
        {
            gWindowSceneryScatterEnabled = false;
        }

        void InputSize(const WidgetIndex widgetIndex)
        {
            uint8_t maxLength = 0;
            Formatter ft;

            switch (widgetIndex)
            {
                case WIDX_PREVIEW:
                    ft.Add<uint16_t>(kLandToolMinimumSize);
                    ft.Add<uint16_t>(kLandToolMaximumSize);
                    maxLength = 3;
                    break;
            }
            WindowTextInputOpen(
                this, widgetIndex, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, ft, kStringIdNone, kStringIdNone, maxLength);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;

                case WIDX_PREVIEW:
                    InputSize(widgetIndex);
                    break;

                case WIDX_DENSITY_LOW:
                    gWindowSceneryScatterDensity = ScatterToolDensity::LowDensity;
                    break;

                case WIDX_DENSITY_MEDIUM:
                    gWindowSceneryScatterDensity = ScatterToolDensity::MediumDensity;
                    break;

                case WIDX_DENSITY_HIGH:
                    gWindowSceneryScatterDensity = ScatterToolDensity::HighDensity;
                    break;
            }
        }

        void onMouseDown(const WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_DECREMENT:
                    // Decrement land tool size, if it stays within the limit
                    gWindowSceneryScatterSize = std::max<uint16_t>(kLandToolMinimumSize, gWindowSceneryScatterSize - 1);
                    invalidate();
                    break;

                case WIDX_INCREMENT:
                    // Increment land tool size, if it stays within the limit
                    gWindowSceneryScatterSize = std::min<uint16_t>(kLandToolMaximumSize, gWindowSceneryScatterSize + 1);
                    invalidate();
                    break;
            }
        }

        void onTextInput(const WidgetIndex widgetIndex, const std::string_view text) override
        {
            if (widgetIndex != WIDX_PREVIEW || text.empty())
                return;

            const auto res = String::tryParse<int32_t>(text);

            if (res.has_value())
            {
                switch (widgetIndex)
                {
                    case WIDX_PREVIEW:
                        gWindowSceneryScatterSize = std::clamp<int32_t>(
                            res.value(), kLandToolMinimumSize, kLandToolMaximumSize);
                        break;
                }
                invalidate();
            }
        }

        void onPrepareDraw() override
        {
            // Set the preview image button to be pressed down
            pressedWidgets = (1uLL << WIDX_PREVIEW);

            // Set density buttons' pressed state.
            switch (gWindowSceneryScatterDensity)
            {
                case ScatterToolDensity::LowDensity:
                    pressedWidgets |= (1uLL << WIDX_DENSITY_LOW);
                    break;

                case ScatterToolDensity::MediumDensity:
                    pressedWidgets |= (1uLL << WIDX_DENSITY_MEDIUM);
                    break;

                case ScatterToolDensity::HighDensity:
                    pressedWidgets |= (1uLL << WIDX_DENSITY_HIGH);
                    break;
            }

            // Update the preview image (for tool sizes up to 7)
            widgets[WIDX_PREVIEW].image = ImageId(LandTool::SizeToSpriteIndex(gWindowSceneryScatterSize));
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            WindowDrawWidgets(*this, rt);

            // Draw area as a number for tool sizes bigger than 7
            if (gWindowSceneryScatterSize > kLandToolMaximumSizeWithSprite)
            {
                const auto& preview = widgets[WIDX_PREVIEW];
                const auto screenCoords = ScreenCoordsXY{ windowPos.x + preview.midX(), windowPos.y + preview.midY() };
                auto ft = Formatter();
                ft.Add<uint16_t>(gWindowSceneryScatterSize);
                DrawTextBasic(
                    rt, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::centre });
            }
        }
    };

    WindowBase* SceneryScatterOpen()
    {
        // Check if window is already open
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->FindByClass(WindowClass::sceneryScatter);
        if (window == nullptr)
        {
            window = windowMgr->Create<SceneryScatterWindow>(WindowClass::sceneryScatter, kWindowSize, {});
        }

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
