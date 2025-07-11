/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Viewport.h"

#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/ClearAction.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>

namespace OpenRCT2::Ui::Windows
{
    enum WindowClearSceneryWidgetIdx : WidgetIndex
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_PREVIEW,
        WIDX_DECREMENT,
        WIDX_INCREMENT,
        WIDX_SMALL_SCENERY,
        WIDX_LARGE_SCENERY,
        WIDX_FOOTPATH
    };

    static constexpr StringId kWindowTitle = STR_CLEAR_SCENERY;
    static constexpr ScreenSize kWindowSize = { 98, 94 };

    static constexpr ScreenSize kClearSceneryButtonSize = { 24, 24 };

    // clang-format off
    static constexpr auto window_clear_scenery_widgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget     ({ 27, 17 }, { 44, 32 },              WidgetType::imgBtn,  WindowColour::primary,   SPR_LAND_TOOL_SIZE_0,        kStringIdNone                             ), // preview box
        makeRemapWidget({ 28, 18 }, { 16, 16 },              WidgetType::trnBtn,  WindowColour::secondary, SPR_LAND_TOOL_DECREASE,      STR_ADJUST_SMALLER_LAND_TIP               ), // decrement size
        makeRemapWidget({ 54, 32 }, { 16, 16 },              WidgetType::trnBtn,  WindowColour::secondary, SPR_LAND_TOOL_INCREASE,      STR_ADJUST_LARGER_LAND_TIP                ), // increment size
        makeRemapWidget({  7, 53 }, kClearSceneryButtonSize, WidgetType::flatBtn, WindowColour::secondary, SPR_G2_BUTTON_TREES,         STR_CLEAR_SCENERY_REMOVE_SMALL_SCENERY_TIP), // small scenery
        makeRemapWidget({ 37, 53 }, kClearSceneryButtonSize, WidgetType::flatBtn, WindowColour::secondary, SPR_G2_BUTTON_LARGE_SCENERY, STR_CLEAR_SCENERY_REMOVE_LARGE_SCENERY_TIP), // large scenery
        makeRemapWidget({ 67, 53 }, kClearSceneryButtonSize, WidgetType::flatBtn, WindowColour::secondary, SPR_G2_BUTTON_FOOTPATH,      STR_CLEAR_SCENERY_REMOVE_FOOTPATHS_TIP    )  // footpaths
    );
    // clang-format on

    class CleanSceneryWindow final : public Window
    {
    private:
        bool _clearSmallScenery = true;
        bool _clearLargeScenery = false;
        bool _clearFootpath = false;
        money64 _clearSceneryCost = kMoney64Undefined;

    public:
        void OnOpen() override
        {
            SetWidgets(window_clear_scenery_widgets);

            hold_down_widgets = (1uLL << WIDX_INCREMENT) | (1uLL << WIDX_DECREMENT);
            WindowInitScrollWidgets(*this);
            WindowPushOthersBelow(*this);

            gLandToolSize = 2;

            Invalidate();
        }

        void OnClose() override
        {
            if (isToolActive(WindowClass::ClearScenery, WIDX_BACKGROUND))
                ToolCancel();
        }

        void OnMouseUp(const WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_PREVIEW:
                {
                    Formatter ft;
                    ft.Add<uint16_t>(kLandToolMinimumSize);
                    ft.Add<uint16_t>(kLandToolMaximumSize);
                    TextInputOpen(
                        WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, ft, kStringIdNone, kStringIdNone, 3);
                    break;
                }
                case WIDX_SMALL_SCENERY:
                    _clearSmallScenery ^= 1;
                    Invalidate();
                    break;
                case WIDX_LARGE_SCENERY:
                    _clearLargeScenery ^= 1;
                    Invalidate();
                    break;
                case WIDX_FOOTPATH:
                    _clearFootpath ^= 1;
                    Invalidate();
                    break;
            }
        }

        void OnMouseDown(const WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_DECREMENT:
                    // Decrement land tool size, if it stays within the limit
                    gLandToolSize = std::max<uint16_t>(kLandToolMinimumSize, gLandToolSize - 1);

                    // Invalidate the window
                    Invalidate();
                    break;
                case WIDX_INCREMENT:
                    // Increment land tool size, if it stays within the limit
                    gLandToolSize = std::min<uint16_t>(kLandToolMaximumSize, gLandToolSize + 1);

                    // Invalidate the window
                    Invalidate();
                    break;
            }
        }

        void OnTextInput(const WidgetIndex widgetIndex, const std::string_view text) override
        {
            if (widgetIndex != WIDX_PREVIEW || text.empty())
                return;

            try
            {
                int32_t size = std::stol(std::string(text));
                size = std::clamp<uint16_t>(size, kLandToolMinimumSize, kLandToolMaximumSize);
                gLandToolSize = size;
                Invalidate();
            }
            catch (const std::logic_error&)
            {
                // std::stol can throw std::out_of_range or std::invalid_argument
            }
        }

        void OnUpdate() override
        {
            frame_no++;
            // Close window if another tool is open
            if (!isToolActive(WindowClass::ClearScenery, WIDX_BACKGROUND))
                Close();
        }

        void OnPrepareDraw() override
        {
            // Set the preview image button to be pressed down
            pressed_widgets = (1uLL << WIDX_PREVIEW) | (_clearSmallScenery ? (1uLL << WIDX_SMALL_SCENERY) : 0)
                | (_clearLargeScenery ? (1uLL << WIDX_LARGE_SCENERY) : 0) | (_clearFootpath ? (1uLL << WIDX_FOOTPATH) : 0);

            // Update the preview image (for tool sizes up to 7)
            widgets[WIDX_PREVIEW].image = ImageId(LandTool::SizeToSpriteIndex(gLandToolSize));
        }

        void OnDraw(RenderTarget& rt) override
        {
            DrawWidgets(rt);

            // Draw number for tool sizes bigger than 7
            ScreenCoordsXY screenCoords = { windowPos.x + widgets[WIDX_PREVIEW].midX(),
                                            windowPos.y + widgets[WIDX_PREVIEW].midY() };
            if (gLandToolSize > kLandToolMaximumSizeWithSprite)
            {
                auto ft = Formatter();
                ft.Add<uint16_t>(gLandToolSize);
                DrawTextBasic(
                    rt, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::CENTRE });
            }

            // Draw cost amount
            if (_clearSceneryCost != kMoney64Undefined && _clearSceneryCost != 0
                && !(getGameState().park.Flags & PARK_FLAGS_NO_MONEY))
            {
                auto ft = Formatter();
                ft.Add<money64>(_clearSceneryCost);
                screenCoords.x = widgets[WIDX_PREVIEW].midX() + windowPos.x;
                screenCoords.y = widgets[WIDX_PREVIEW].bottom + windowPos.y + 5 + 27;
                DrawTextBasic(rt, screenCoords, STR_COST_AMOUNT, ft, { TextAlignment::CENTRE });
            }
        }

        ClearAction GetClearAction()
        {
            auto range = MapRange(gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y);

            ClearableItems itemsToClear = 0;

            if (_clearSmallScenery)
                itemsToClear |= CLEARABLE_ITEMS::kScenerySmall;
            if (_clearLargeScenery)
                itemsToClear |= CLEARABLE_ITEMS::kSceneryLarge;
            if (_clearFootpath)
                itemsToClear |= CLEARABLE_ITEMS::kSceneryFootpath;

            return ClearAction(range, itemsToClear);
        }

        int8_t ToolUpdateClearLandPaint(const ScreenCoordsXY& screenPos)
        {
            uint8_t state_changed = 0;

            MapInvalidateSelectionRect();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;

            auto mapTile = ScreenGetMapXY(screenPos, nullptr);

            if (!mapTile.has_value())
            {
                return state_changed;
            }

            if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
            {
                gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
                state_changed++;
            }

            if (gMapSelectType != MAP_SELECT_TYPE_FULL)
            {
                gMapSelectType = MAP_SELECT_TYPE_FULL;
                state_changed++;
            }

            int16_t tool_size = std::max<uint16_t>(1, gLandToolSize);
            int16_t tool_length = (tool_size - 1) * 32;

            // Move to tool bottom left
            mapTile->x -= (tool_size - 1) * 16;
            mapTile->y -= (tool_size - 1) * 16;
            mapTile = mapTile->ToTileStart();

            if (gMapSelectPositionA.x != mapTile->x)
            {
                gMapSelectPositionA.x = mapTile->x;
                state_changed++;
            }

            if (gMapSelectPositionA.y != mapTile->y)
            {
                gMapSelectPositionA.y = mapTile->y;
                state_changed++;
            }

            mapTile->x += tool_length;
            mapTile->y += tool_length;

            if (gMapSelectPositionB.x != mapTile->x)
            {
                gMapSelectPositionB.x = mapTile->x;
                state_changed++;
            }

            if (gMapSelectPositionB.y != mapTile->y)
            {
                gMapSelectPositionB.y = mapTile->y;
                state_changed++;
            }

            MapInvalidateSelectionRect();
            return state_changed;
        }

        /**
         *
         *  rct2: 0x0068E213
         */
        void ToolUpdateSceneryClear(const ScreenCoordsXY& screenPos)
        {
            if (!ToolUpdateClearLandPaint(screenPos))
                return;

            auto action = GetClearAction();
            auto result = GameActions::Query(&action);
            auto cost = (result.Error == GameActions::Status::Ok ? result.Cost : kMoney64Undefined);
            if (_clearSceneryCost != cost)
            {
                _clearSceneryCost = cost;
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->InvalidateByClass(WindowClass::ClearScenery);
            }
        }

        void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    ToolUpdateSceneryClear(screenCoords);
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
                        auto action = GetClearAction();
                        GameActions::Execute(&action);
                        gCurrentToolId = Tool::bulldozer;
                    }
                    break;
            }
        }

        void OnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                {
                    auto* windowMgr = GetWindowManager();
                    if (windowMgr->FindByClass(WindowClass::Error) == nullptr && (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
                    {
                        auto action = GetClearAction();
                        GameActions::Execute(&action);
                        gCurrentToolId = Tool::bulldozer;
                    }
                    break;
                }
            }
        }

        void OnToolUp(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    MapInvalidateSelectionRect();
                    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
                    gCurrentToolId = Tool::bulldozer;
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
    };

    WindowBase* ClearSceneryOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<CleanSceneryWindow>(
            WindowClass::ClearScenery, ScreenCoordsXY(ContextGetWidth() - kWindowSize.width, 29), kWindowSize, 0);
    }

    /**
     *
     *  rct2: 0x0066CD0C
     */
    void ToggleClearSceneryWindow()
    {
        if (isToolActive(WindowClass::ClearScenery, WIDX_BACKGROUND))
        {
            ToolCancel();
        }
        else
        {
            ShowGridlines();
            auto* toolWindow = ContextOpenWindow(WindowClass::ClearScenery);
            ToolSet(*toolWindow, WIDX_BACKGROUND, Tool::bulldozer);
            gInputFlags.set(InputFlag::unk6);
        }
    }
} // namespace OpenRCT2::Ui::Windows
