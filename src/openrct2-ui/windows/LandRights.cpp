/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/LandBuyRightsAction.h>
#include <openrct2/actions/LandSetRightsAction.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/tile_element/SurfaceElement.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenSize kInGameSize = { 94, 94 };
    static constexpr ScreenSize kEditorSize = { 280, 104 };

    static constexpr ScreenSize kWindowSize = kInGameSize;
    static constexpr StringId kWindowTitle = STR_LAND_RIGHTS;

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
        WIDX_LAND_OWNED_CHECKBOX,
        WIDX_LAND_SALE_CHECKBOX,
        WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX,
        WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX,
        WIDX_UNOWNED_LAND_CHECKBOX,
    };

    // clang-format off
    static constexpr auto window_land_rights_widgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget     ({ 27, 17}, { 44, 32}, WidgetType::imgBtn, WindowColour::primary, ImageId(SPR_LAND_TOOL_SIZE_0)                                                   ), // preview box
        makeRemapWidget({ 28, 18}, { 16, 16}, WidgetType::trnBtn, WindowColour::primary, SPR_LAND_TOOL_DECREASE,          STR_ADJUST_SMALLER_LAND_RIGHTS_TIP             ), // decrement size
        makeRemapWidget({ 54, 32}, { 16, 16}, WidgetType::trnBtn, WindowColour::primary, SPR_LAND_TOOL_INCREASE,          STR_ADJUST_LARGER_LAND_RIGHTS_TIP              ), // increment size
        makeRemapWidget({ 22, 53}, { 24, 24}, WidgetType::imgBtn, WindowColour::primary, SPR_BUY_LAND_RIGHTS,             STR_BUY_LAND_RIGHTS_TIP                        ), // land rights
        makeRemapWidget({ 52, 53}, { 24, 24}, WidgetType::imgBtn, WindowColour::primary, SPR_BUY_CONSTRUCTION_RIGHTS,     STR_BUY_CONSTRUCTION_RIGHTS_TIP                ), // construction rights
        makeWidget     ({100, 22}, {170, 12}, WidgetType::empty,  WindowColour::primary, STR_LAND_OWNED,                  STR_SET_LAND_TO_BE_OWNED_TIP                   ),
        makeWidget     ({100, 38}, {170, 12}, WidgetType::empty,  WindowColour::primary, STR_LAND_SALE,                   STR_SET_LAND_TO_BE_AVAILABLE_TIP               ),
        makeWidget     ({100, 54}, {170, 12}, WidgetType::empty,  WindowColour::primary, STR_CONSTRUCTION_RIGHTS_OWNED,   STR_SET_CONSTRUCTION_RIGHTS_TO_BE_OWNED_TIP    ),
        makeWidget     ({100, 70}, {170, 12}, WidgetType::empty,  WindowColour::primary, STR_CONSTRUCTION_RIGHTS_SALE,    STR_SET_CONSTRUCTION_RIGHTS_TO_BE_AVAILABLE_TIP),
        makeWidget     ({100, 86}, {170, 12}, WidgetType::empty,  WindowColour::primary, STR_LAND_NOT_OWNED,              STR_SET_LAND_TO_BE_NOT_OWNED_TIP               )
    );
    // clang-format on

    enum class LandRightsMode : uint8_t
    {
        // In-game
        BuyLand,
        BuyConstructionRights,

        // Sandbox/editor mode
        SetLandOwned,
        SetLandForSale,
        SetConstructionRightsOwned,
        SetConstructionRightsForSale,
        SetLandUnowned,
    };

    static const bool kLandRightsVisibleByMode[] = { true, false, true, true, false, false, true };
    static const bool kConstructionRightsVisibleByMode[] = { false, true, false, false, true, true, true };

    class LandRightsWindow final : public Window
    {
    private:
        LandRightsMode _landRightsMode;
        money64 _landRightsCost;

        bool IsOwnershipMode() const
        {
            return isInEditorMode() != 0 || getGameState().cheats.sandboxMode;
        }

        void SwitchToMode(LandRightsMode mode)
        {
            auto widgetIndex = WIDX_BUY_LAND_RIGHTS + EnumValue(mode);
            pressedWidgets = (1uLL << widgetIndex);
            _landRightsMode = mode;

            ToolSet(*this, widgetIndex, Tool::upArrow);

            if (kLandRightsVisibleByMode[EnumValue(mode)])
                ShowLandRights();
            else
                HideLandRights();

            if (kConstructionRightsVisibleByMode[EnumValue(mode)])
                ShowConstructionRights();
            else
                HideConstructionRights();

            invalidate();
        }

    public:
        void onOpen() override
        {
            setWidgets(window_land_rights_widgets);

            holdDownWidgets = (1uLL << WIDX_INCREMENT) | (1uLL << WIDX_DECREMENT);
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
                SwitchToMode(LandRightsMode::SetLandOwned);
            }
        }

        void onClose() override
        {
            HideGridlines();
            HideConstructionRights();

            if (isToolActive(WindowClass::landRights))
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

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_DECREMENT:
                    // Decrement land rights tool size
                    gLandToolSize = std::max<uint16_t>(kLandToolMinimumSize, gLandToolSize - 1);

                    // Invalidate the window
                    invalidate();
                    break;
                case WIDX_INCREMENT:
                    // Decrement land rights tool size
                    gLandToolSize = std::min<uint16_t>(kLandToolMaximumSize, gLandToolSize + 1);

                    // Invalidate the window
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
                uint16_t size;
                size = res.value();
                size = std::max(kLandToolMinimumSize, size);
                size = std::min(kLandToolMaximumSize, size);
                gLandToolSize = size;
                invalidate();
            }
        }

        void onUpdate() override
        {
            currentFrame++;

            // Close window if another tool is open
            if (!isToolActive(WindowClass::landRights))
            {
                close();
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
                setWidgetPressed(WIDX_BUY_LAND_RIGHTS, true);
                setWidgetPressed(WIDX_BUY_CONSTRUCTION_RIGHTS, false);
            }
            else if (_landRightsMode == LandRightsMode::BuyConstructionRights)
            {
                setWidgetPressed(WIDX_BUY_LAND_RIGHTS, false);
                setWidgetPressed(WIDX_BUY_CONSTRUCTION_RIGHTS, true);
            }

            if (gLandRemainingOwnershipSales == 0)
            {
                setWidgetDisabled(WIDX_BUY_LAND_RIGHTS, true);
                widgets[WIDX_BUY_LAND_RIGHTS].tooltip = STR_NO_LAND_RIGHTS_FOR_SALE_TIP;
            }
            else
            {
                setWidgetDisabled(WIDX_BUY_LAND_RIGHTS, false);
                widgets[WIDX_BUY_LAND_RIGHTS].tooltip = STR_BUY_LAND_RIGHTS_TIP;
            }

            if (gLandRemainingConstructionSales == 0)
            {
                setWidgetDisabled(WIDX_BUY_CONSTRUCTION_RIGHTS, true);
                widgets[WIDX_BUY_CONSTRUCTION_RIGHTS].tooltip = STR_NO_CONSTRUCTION_RIGHTS_FOR_SALE_TIP;
            }
            else
            {
                setWidgetDisabled(WIDX_BUY_CONSTRUCTION_RIGHTS, false);
                widgets[WIDX_BUY_CONSTRUCTION_RIGHTS].tooltip = STR_BUY_CONSTRUCTION_RIGHTS_TIP;
            }

            // Position land size tool
            auto contentTop = widgets[WIDX_TITLE].bottom + 3;
            widgets[WIDX_PREVIEW].top = contentTop;
            widgets[WIDX_PREVIEW].bottom = widgets[WIDX_PREVIEW].top + 32;
            widgets[WIDX_DECREMENT].top = contentTop + 1;
            widgets[WIDX_DECREMENT].bottom = widgets[WIDX_DECREMENT].top + 16;
            widgets[WIDX_INCREMENT].top = contentTop + 16;
            widgets[WIDX_INCREMENT].bottom = widgets[WIDX_INCREMENT].top + 16;

            // Show in-game mode widgets
            widgets[WIDX_BUY_LAND_RIGHTS].type = WidgetType::imgBtn;
            widgets[WIDX_BUY_CONSTRUCTION_RIGHTS].type = WidgetType::imgBtn;

            // Hide editor/sandbox mode widgets
            widgets[WIDX_UNOWNED_LAND_CHECKBOX].type = WidgetType::empty;
            widgets[WIDX_LAND_OWNED_CHECKBOX].type = WidgetType::empty;
            widgets[WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX].type = WidgetType::empty;
            widgets[WIDX_LAND_SALE_CHECKBOX].type = WidgetType::empty;
            widgets[WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX].type = WidgetType::empty;
        }

        void PrepareDrawSandbox()
        {
            // Position land size tool
            auto contentTop = widgets[WIDX_TITLE].bottom + 27;
            widgets[WIDX_PREVIEW].top = contentTop;
            widgets[WIDX_PREVIEW].bottom = widgets[WIDX_PREVIEW].top + 32;
            widgets[WIDX_DECREMENT].top = contentTop + 1;
            widgets[WIDX_DECREMENT].bottom = widgets[WIDX_DECREMENT].top + 16;
            widgets[WIDX_INCREMENT].top = contentTop + 16;
            widgets[WIDX_INCREMENT].bottom = widgets[WIDX_INCREMENT].top + 16;

            // Hide in-game mode widgets
            widgets[WIDX_BUY_LAND_RIGHTS].type = WidgetType::empty;
            widgets[WIDX_BUY_CONSTRUCTION_RIGHTS].type = WidgetType::empty;

            // Show editor/sandbox mode widgets
            widgets[WIDX_UNOWNED_LAND_CHECKBOX].type = WidgetType::checkbox;
            widgets[WIDX_LAND_OWNED_CHECKBOX].type = WidgetType::checkbox;
            widgets[WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX].type = WidgetType::checkbox;
            widgets[WIDX_LAND_SALE_CHECKBOX].type = WidgetType::checkbox;
            widgets[WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX].type = WidgetType::checkbox;
        }

        ScreenSize GetModeDimensions() const
        {
            if (IsOwnershipMode())
                return kEditorSize;
            else
                return kInGameSize;
        }

        void onPrepareDraw() override
        {
            setWidgetPressed(WIDX_PREVIEW, true);
            widgets[WIDX_PREVIEW].image = ImageId(LandTool::SizeToSpriteIndex(gLandToolSize));

            if (width != GetModeDimensions().width)
                onResize();

            if (IsOwnershipMode())
            {
                PrepareDrawSandbox();
                ColourSchemeUpdateByClass(this, WindowClass::map);
            }
            else
            {
                PrepareDrawInGame();
                ColourSchemeUpdateByClass(this, WindowClass::landRights);
            }
        }

        void onResize() override
        {
            invalidate();

            auto dimensions = GetModeDimensions();
            WindowSetResize(*this, dimensions, dimensions);

            if (windowPos.x + width > ContextGetWidth())
                windowPos.x = ContextGetWidth() - width;

            invalidate();
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

            // Draw cost amount
            if (_landRightsCost != kMoney64Undefined && _landRightsCost != 0
                && !(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
            {
                auto ft = Formatter();
                ft.Add<money64>(_landRightsCost);

                auto offset = widgets[WIDX_BUY_LAND_RIGHTS].type != WidgetType::empty ? 32 : 8;

                screenCoords = { widgets[WIDX_PREVIEW].midX() + windowPos.x,
                                 widgets[WIDX_PREVIEW].bottom + windowPos.y + offset };
                DrawTextBasic(rt, screenCoords, STR_COST_AMOUNT, ft, { TextAlignment::centre });
            }
        }

        GameActions::LandBuyRightsAction GetLandBuyAction()
        {
            auto mode = (_landRightsMode == LandRightsMode::BuyLand) ? GameActions::LandBuyRightSetting::BuyLand
                                                                     : GameActions::LandBuyRightSetting::BuyConstructionRights;

            return GameActions::LandBuyRightsAction(
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

        GameActions::LandSetRightsAction GetLandSetAction()
        {
            return GameActions::LandSetRightsAction(
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                GameActions::LandSetRightSetting::SetOwnershipWithChecks, GetDesiredOwnership());
        }

        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            gMapSelectFlags.unset(MapSelectFlag::enable);

            auto info = GetMapCoordinatesFromPos(
                screenCoords, EnumsToFlags(ViewportInteractionItem::terrain, ViewportInteractionItem::water));
            if (info.interactionType == ViewportInteractionItem::none)
            {
                if (_landRightsCost != kMoney64Undefined)
                {
                    _landRightsCost = kMoney64Undefined;

                    auto* windowMgr = GetWindowManager();
                    windowMgr->InvalidateByClass(WindowClass::landRights);
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

            if (gMapSelectType != MapSelectType::fullLandRights)
            {
                gMapSelectType = MapSelectType::fullLandRights;
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

            if (!state_changed)
                return;

            auto& gameState = getGameState();
            if (IsOwnershipMode())
            {
                auto landSetRightsAction = GetLandSetAction();
                auto res = GameActions::Query(&landSetRightsAction, gameState);
                _landRightsCost = res.error == GameActions::Status::ok ? res.cost : kMoney64Undefined;
            }
            else
            {
                auto landBuyRightsAction = GetLandBuyAction();
                auto res = GameActions::Query(&landBuyRightsAction, gameState);
                _landRightsCost = res.error == GameActions::Status::ok ? res.cost : kMoney64Undefined;
            }
        }

        void onToolAbort(WidgetIndex widgetIndex) override
        {
            if (kLandRightsVisibleByMode[EnumValue(_landRightsMode)])
                HideLandRights();

            if (kConstructionRightsVisibleByMode[EnumValue(_landRightsMode)])
                HideConstructionRights();
        }

        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (screenCoords.x == kLocationNull)
                return;

            auto& gameState = getGameState();
            if (IsOwnershipMode())
            {
                auto landSetRightsAction = GetLandSetAction();
                GameActions::Execute(&landSetRightsAction, gameState);
            }
            else
            {
                auto landBuyRightsAction = GetLandBuyAction();
                GameActions::Execute(&landBuyRightsAction, gameState);
            }
        }

        void onToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            onToolDown(widgetIndex, screenCoords);
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
            WindowClass::landRights, ScreenCoordsXY(ContextGetWidth() - kWindowSize.width, 29), kWindowSize, {});
    }
} // namespace OpenRCT2::Ui::Windows
