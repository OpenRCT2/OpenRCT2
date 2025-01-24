/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Theme.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/LandBuyRightsAction.h>
#include <openrct2/actions/LandSetRightsAction.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/tile_element/SurfaceElement.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenCoordsXY kInGameSize = { 94, 94 };
    static constexpr ScreenCoordsXY kEditorSize = { 280, 104 };

    static constexpr StringId WINDOW_TITLE = STR_LAND_RIGHTS;
    static constexpr int32_t WW = kInGameSize.x;
    static constexpr int32_t WH = kInGameSize.y;

    enum WindowLandRightsWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_PREVIEW,
        WIDX_DECREMENT,
        WIDX_INCREMENT,

        // In-game widgets
        WIDX_BUY_LAND_RIGHTS,
        WIDX_BUY_CONSTRUCTION_RIGHTS,

        // Editor/sandbox widgets
        WIDX_UNOWNED_LAND_CHECKBOX,
        WIDX_LAND_SALE_CHECKBOX,
        WIDX_LAND_OWNED_CHECKBOX,
        WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX,
        WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX,
    };

    // clang-format off
    static constexpr Widget window_land_rights_widgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget     ({ 27, 17}, { 44, 32}, WindowWidgetType::ImgBtn, WindowColour::Primary, ImageId(SPR_LAND_TOOL_SIZE_0)                                                   ), // preview box
        MakeRemapWidget({ 28, 18}, { 16, 16}, WindowWidgetType::TrnBtn, WindowColour::Primary, SPR_LAND_TOOL_DECREASE,          STR_ADJUST_SMALLER_LAND_RIGHTS_TIP             ), // decrement size
        MakeRemapWidget({ 54, 32}, { 16, 16}, WindowWidgetType::TrnBtn, WindowColour::Primary, SPR_LAND_TOOL_INCREASE,          STR_ADJUST_LARGER_LAND_RIGHTS_TIP              ), // increment size
        MakeRemapWidget({ 22, 53}, { 24, 24}, WindowWidgetType::ImgBtn, WindowColour::Primary, SPR_BUY_LAND_RIGHTS,             STR_BUY_LAND_RIGHTS_TIP                        ), // land rights
        MakeRemapWidget({ 52, 53}, { 24, 24}, WindowWidgetType::ImgBtn, WindowColour::Primary, SPR_BUY_CONSTRUCTION_RIGHTS,     STR_BUY_CONSTRUCTION_RIGHTS_TIP                ), // construction rights
        MakeWidget     ({100, 22}, {170, 12}, WindowWidgetType::Empty,  WindowColour::Primary, STR_LAND_NOT_OWNED,              STR_SET_LAND_TO_BE_NOT_OWNED_TIP               ),
        MakeWidget     ({100, 38}, {170, 12}, WindowWidgetType::Empty,  WindowColour::Primary, STR_LAND_SALE,                   STR_SET_LAND_TO_BE_AVAILABLE_TIP               ),
        MakeWidget     ({100, 54}, {170, 12}, WindowWidgetType::Empty,  WindowColour::Primary, STR_LAND_OWNED,                  STR_SET_LAND_TO_BE_OWNED_TIP                   ),
        MakeWidget     ({100, 70}, {170, 12}, WindowWidgetType::Empty,  WindowColour::Primary, STR_CONSTRUCTION_RIGHTS_SALE,    STR_SET_CONSTRUCTION_RIGHTS_TO_BE_AVAILABLE_TIP),
        MakeWidget     ({100, 86}, {170, 12}, WindowWidgetType::Empty,  WindowColour::Primary, STR_CONSTRUCTION_RIGHTS_OWNED,   STR_SET_CONSTRUCTION_RIGHTS_TO_BE_OWNED_TIP    ),
    };
    // clang-format on

    enum class LandRightsMode : uint8_t
    {
        // In-game
        BuyLand,
        BuyConstructionRights,

        // Sandbox/editor mode
        SetLandUnowned,
        SetLandForSale,
        SetLandOwned,
        SetConstructionRightsForSale,
        SetConstructionRightsOwned,
    };

    static const bool kLandRightsVisibleByMode[] = { true, false, true, true, true, false, false };
    static const bool kConstructionRightsVisibleByMode[] = { false, true, true, false, false, true, true };

    class LandRightsWindow final : public Window
    {
    private:
        LandRightsMode _landRightsMode;
        money64 _landRightsCost;

        bool IsOwnershipMode() const
        {
            return (gScreenFlags & SCREEN_FLAGS_EDITOR) != 0 || GetGameState().Cheats.sandboxMode;
        }

        void SwitchToMode(LandRightsMode mode)
        {
            auto widgetIndex = WIDX_BUY_LAND_RIGHTS + EnumValue(mode);
            pressed_widgets = (1uLL << widgetIndex);
            _landRightsMode = mode;

            ToolSet(*this, widgetIndex, Tool::UpArrow);
            InputSetFlag(INPUT_FLAG_6, true);

            if (kLandRightsVisibleByMode[EnumValue(mode)])
                ShowLandRights();
            else
                HideLandRights();

            if (kConstructionRightsVisibleByMode[EnumValue(mode)])
                ShowConstructionRights();
            else
                HideConstructionRights();

            Invalidate();
        }

    public:
        void OnOpen() override
        {
            SetWidgets(window_land_rights_widgets);
            hold_down_widgets = (1uLL << WIDX_INCREMENT) | (1uLL << WIDX_DECREMENT);
            WindowInitScrollWidgets(*this);
            WindowPushOthersBelow(*this);

            gLandToolSize = kLandToolMinimumSize;
            ShowGridlines();

            if (!IsOwnershipMode())
            {
                if (gLandRemainingOwnershipSales > 0)
                    SwitchToMode(LandRightsMode::BuyLand);
                else
                    SwitchToMode(LandRightsMode::BuyConstructionRights);
            }
            else
            {
                SwitchToMode(LandRightsMode::SetLandUnowned);
            }
        }

        void OnClose() override
        {
            HideGridlines();
            HideConstructionRights();

            if (isToolActive(WindowClass::LandRights))
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

                // In-game mode widgets
                case WIDX_BUY_LAND_RIGHTS:
                    if (_landRightsMode != LandRightsMode::BuyLand)
                        SwitchToMode(LandRightsMode::BuyLand);
                    break;
                case WIDX_BUY_CONSTRUCTION_RIGHTS:
                    if (_landRightsMode != LandRightsMode::BuyConstructionRights)
                        SwitchToMode(LandRightsMode::BuyConstructionRights);
                    break;

                // Sandbox/editor mode widgets
                case WIDX_UNOWNED_LAND_CHECKBOX:
                    if (_landRightsMode != LandRightsMode::SetLandUnowned)
                        SwitchToMode(LandRightsMode::SetLandUnowned);
                    break;

                case WIDX_LAND_SALE_CHECKBOX:
                    if (_landRightsMode != LandRightsMode::SetLandForSale)
                        SwitchToMode(LandRightsMode::SetLandForSale);
                    break;

                case WIDX_LAND_OWNED_CHECKBOX:
                    if (_landRightsMode != LandRightsMode::SetLandOwned)
                        SwitchToMode(LandRightsMode::SetLandOwned);
                    break;

                case WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX:
                    if (_landRightsMode != LandRightsMode::SetConstructionRightsForSale)
                        SwitchToMode(LandRightsMode::SetConstructionRightsForSale);
                    break;

                case WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX:
                    if (_landRightsMode != LandRightsMode::SetConstructionRightsOwned)
                        SwitchToMode(LandRightsMode::SetConstructionRightsOwned);
                    break;
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_DECREMENT:
                    // Decrement land rights tool size
                    gLandToolSize = std::max<uint16_t>(kLandToolMinimumSize, gLandToolSize - 1);

                    // Invalidate the window
                    Invalidate();
                    break;
                case WIDX_INCREMENT:
                    // Decrement land rights tool size
                    gLandToolSize = std::min<uint16_t>(kLandToolMaximumSize, gLandToolSize + 1);

                    // Invalidate the window
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
                uint16_t size;
                size = res.value();
                size = std::max(kLandToolMinimumSize, size);
                size = std::min(kLandToolMaximumSize, size);
                gLandToolSize = size;
                Invalidate();
            }
        }

        void OnUpdate() override
        {
            frame_no++;

            // Close window if another tool is open
            if (!isToolActive(WindowClass::LandRights))
            {
                Close();
                return;
            }

            bool inRightsMode = _landRightsMode == LandRightsMode::BuyLand
                || _landRightsMode == LandRightsMode::BuyConstructionRights;

            if (!IsOwnershipMode() && !inRightsMode)
            {
                if (gLandRemainingOwnershipSales > 0)
                    SwitchToMode(LandRightsMode::BuyLand);
                else
                    SwitchToMode(LandRightsMode::BuyConstructionRights);
            }
            else if (IsOwnershipMode() && inRightsMode)
                SwitchToMode(LandRightsMode::SetLandUnowned);
        }

        void PrepareDrawInGame()
        {
            if (_landRightsMode == LandRightsMode::BuyLand)
            {
                SetWidgetPressed(WIDX_BUY_LAND_RIGHTS, true);
                SetWidgetPressed(WIDX_BUY_CONSTRUCTION_RIGHTS, false);
            }
            else if (_landRightsMode == LandRightsMode::BuyConstructionRights)
            {
                SetWidgetPressed(WIDX_BUY_LAND_RIGHTS, false);
                SetWidgetPressed(WIDX_BUY_CONSTRUCTION_RIGHTS, true);
            }

            if (gLandRemainingOwnershipSales == 0)
            {
                SetWidgetDisabled(WIDX_BUY_LAND_RIGHTS, true);
                widgets[WIDX_BUY_LAND_RIGHTS].tooltip = STR_NO_LAND_RIGHTS_FOR_SALE_TIP;
            }
            else
            {
                SetWidgetDisabled(WIDX_BUY_LAND_RIGHTS, false);
                widgets[WIDX_BUY_LAND_RIGHTS].tooltip = STR_BUY_LAND_RIGHTS_TIP;
            }

            if (gLandRemainingConstructionSales == 0)
            {
                SetWidgetDisabled(WIDX_BUY_CONSTRUCTION_RIGHTS, true);
                widgets[WIDX_BUY_CONSTRUCTION_RIGHTS].tooltip = STR_NO_CONSTRUCTION_RIGHTS_FOR_SALE_TIP;
            }
            else
            {
                SetWidgetDisabled(WIDX_BUY_CONSTRUCTION_RIGHTS, false);
                widgets[WIDX_BUY_CONSTRUCTION_RIGHTS].tooltip = STR_BUY_CONSTRUCTION_RIGHTS_TIP;
            }

            // Position land size tool
            widgets[WIDX_PREVIEW].top = 17;
            widgets[WIDX_PREVIEW].bottom = widgets[WIDX_PREVIEW].top + 32;
            widgets[WIDX_DECREMENT].top = 18;
            widgets[WIDX_DECREMENT].bottom = widgets[WIDX_DECREMENT].top + 16;
            widgets[WIDX_INCREMENT].top = 32;
            widgets[WIDX_INCREMENT].bottom = widgets[WIDX_INCREMENT].top + 16;

            // Show in-game mode widgets
            widgets[WIDX_BUY_LAND_RIGHTS].type = WindowWidgetType::ImgBtn;
            widgets[WIDX_BUY_CONSTRUCTION_RIGHTS].type = WindowWidgetType::ImgBtn;

            // Hide editor/sandbox mode widgets
            widgets[WIDX_UNOWNED_LAND_CHECKBOX].type = WindowWidgetType::Empty;
            widgets[WIDX_LAND_OWNED_CHECKBOX].type = WindowWidgetType::Empty;
            widgets[WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX].type = WindowWidgetType::Empty;
            widgets[WIDX_LAND_SALE_CHECKBOX].type = WindowWidgetType::Empty;
            widgets[WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX].type = WindowWidgetType::Empty;
        }

        void PrepareDrawSandbox()
        {
            // Position land size tool
            widgets[WIDX_PREVIEW].top = 17 + 24;
            widgets[WIDX_PREVIEW].bottom = widgets[WIDX_PREVIEW].top + 32;
            widgets[WIDX_DECREMENT].top = 18 + 24;
            widgets[WIDX_DECREMENT].bottom = widgets[WIDX_DECREMENT].top + 16;
            widgets[WIDX_INCREMENT].top = 32 + 24;
            widgets[WIDX_INCREMENT].bottom = widgets[WIDX_INCREMENT].top + 16;

            // Hide in-game mode widgets
            widgets[WIDX_BUY_LAND_RIGHTS].type = WindowWidgetType::Empty;
            widgets[WIDX_BUY_CONSTRUCTION_RIGHTS].type = WindowWidgetType::Empty;

            // Show editor/sandbox mode widgets
            widgets[WIDX_UNOWNED_LAND_CHECKBOX].type = WindowWidgetType::Checkbox;
            widgets[WIDX_LAND_OWNED_CHECKBOX].type = WindowWidgetType::Checkbox;
            widgets[WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX].type = WindowWidgetType::Checkbox;
            widgets[WIDX_LAND_SALE_CHECKBOX].type = WindowWidgetType::Checkbox;
            widgets[WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX].type = WindowWidgetType::Checkbox;
        }

        ScreenCoordsXY GetModeDimensions() const
        {
            if (IsOwnershipMode())
                return kEditorSize;
            else
                return kInGameSize;
        }

        void OnPrepareDraw() override
        {
            SetWidgetPressed(WIDX_PREVIEW, true);
            widgets[WIDX_PREVIEW].image = ImageId(LandTool::SizeToSpriteIndex(gLandToolSize));

            if (width != GetModeDimensions().x)
                OnResize();

            if (IsOwnershipMode())
            {
                PrepareDrawSandbox();
                ColourSchemeUpdateByClass(this, WindowClass::Map);
            }
            else
            {
                PrepareDrawInGame();
                ColourSchemeUpdateByClass(this, WindowClass::LandRights);
            }
        }

        void OnResize() override
        {
            Invalidate();

            auto dimensions = GetModeDimensions();
            width = dimensions.x;
            height = dimensions.y;

            if (windowPos.x + width > ContextGetWidth())
                windowPos.x = ContextGetWidth() - width;

            ResizeFrame();
            Invalidate();
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

            // Draw cost amount
            if (_landRightsCost != kMoney64Undefined && _landRightsCost != 0
                && !(GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY))
            {
                auto ft = Formatter();
                ft.Add<money64>(_landRightsCost);

                auto offset = widgets[WIDX_BUY_LAND_RIGHTS].type != WindowWidgetType::Empty ? 32 : 8;

                screenCoords = { widgets[WIDX_PREVIEW].midX() + windowPos.x,
                                 widgets[WIDX_PREVIEW].bottom + windowPos.y + offset };
                DrawTextBasic(dpi, screenCoords, STR_COST_AMOUNT, ft, { TextAlignment::CENTRE });
            }
        }

        LandBuyRightsAction GetLandBuyAction()
        {
            auto mode = (_landRightsMode == LandRightsMode::BuyLand) ? LandBuyRightSetting::BuyLand
                                                                     : LandBuyRightSetting::BuyConstructionRights;

            return LandBuyRightsAction(
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y }, mode);
        }

        uint8_t GetDesiredOwnership()
        {
            switch (_landRightsMode)
            {
                case LandRightsMode::SetLandForSale:
                    return OWNERSHIP_AVAILABLE;
                case LandRightsMode::SetLandOwned:
                    return OWNERSHIP_OWNED;
                case LandRightsMode::SetConstructionRightsForSale:
                    return OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE;
                case LandRightsMode::SetConstructionRightsOwned:
                    return OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED;
                case LandRightsMode::SetLandUnowned:
                default:
                    return OWNERSHIP_UNOWNED;
            }
        }

        LandSetRightsAction GetLandSetAction()
        {
            return LandSetRightsAction(
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                LandSetRightSetting::SetOwnershipWithChecks, GetDesiredOwnership());
        }

        void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            MapInvalidateSelectionRect();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;

            auto info = GetMapCoordinatesFromPos(
                screenCoords, EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Water));
            if (info.interactionType == ViewportInteractionItem::None)
            {
                if (_landRightsCost != kMoney64Undefined)
                {
                    _landRightsCost = kMoney64Undefined;

                    auto* windowMgr = Ui::GetWindowManager();
                    windowMgr->InvalidateByClass(WindowClass::LandRights);
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

            if (gMapSelectType != MAP_SELECT_TYPE_FULL_LAND_RIGHTS)
            {
                gMapSelectType = MAP_SELECT_TYPE_FULL_LAND_RIGHTS;
                state_changed++;
            }

            int16_t tool_size = gLandToolSize;
            if (tool_size == 0)
                tool_size = 1;

            int16_t tool_length = (tool_size - 1) * 32;

            // Move to tool bottom left
            mapTile.x -= (tool_size - 1) * 16;
            mapTile.y -= (tool_size - 1) * 16;
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

            if (IsOwnershipMode())
            {
                auto landSetRightsAction = GetLandSetAction();
                auto res = GameActions::Query(&landSetRightsAction);
                _landRightsCost = res.Error == GameActions::Status::Ok ? res.Cost : kMoney64Undefined;
            }
            else
            {
                auto landBuyRightsAction = GetLandBuyAction();
                auto res = GameActions::Query(&landBuyRightsAction);
                _landRightsCost = res.Error == GameActions::Status::Ok ? res.Cost : kMoney64Undefined;
            }
        }

        void OnToolAbort(WidgetIndex widgetIndex) override
        {
            if (kLandRightsVisibleByMode[EnumValue(_landRightsMode)])
                HideLandRights();

            if (kConstructionRightsVisibleByMode[EnumValue(_landRightsMode)])
                HideConstructionRights();
        }

        void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (screenCoords.x == kLocationNull)
                return;

            if (IsOwnershipMode())
            {
                auto landSetRightsAction = GetLandSetAction();
                GameActions::Execute(&landSetRightsAction);
            }
            else
            {
                auto landBuyRightsAction = GetLandBuyAction();
                GameActions::Execute(&landBuyRightsAction);
            }
        }

        void OnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            OnToolDown(widgetIndex, screenCoords);
        }

    private:
        void InputSize()
        {
            Formatter ft;
            ft.Add<uint16_t>(kLandToolMinimumSize);
            ft.Add<uint16_t>(kLandToolMaximumSize);
            WindowTextInputOpen(
                this, WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, ft, kStringIdNone, kStringIdNone, 3);
        }
    };

    WindowBase* LandRightsOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<LandRightsWindow>(
            WindowClass::LandRights, ScreenCoordsXY(ContextGetWidth() - WW, 29), WW, WH, 0);
    }
} // namespace OpenRCT2::Ui::Windows
