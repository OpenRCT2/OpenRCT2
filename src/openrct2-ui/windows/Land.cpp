/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/input/MouseInput.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/actions/LandLowerAction.h>
#include <openrct2/actions/LandRaiseAction.h>
#include <openrct2/actions/LandSmoothAction.h>
#include <openrct2/actions/SurfaceSetStyleAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = STR_LAND;
    static constexpr int32_t WH = 160;
    static constexpr int32_t WW = 98;

    enum WindowLandWidgetIdx : WidgetIndex
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_MOUNTAINMODE,
        WIDX_PAINTMODE,
        WIDX_PREVIEW,
        WIDX_DECREMENT,
        WIDX_INCREMENT,
        WIDX_FLOOR,
        WIDX_WALL,
    };

    // clang-format off
    static constexpr Widget window_land_widgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget     ({19,  19}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_UP), STR_ENABLE_MOUNTAIN_TOOL_TIP), // mountain mode
        MakeWidget     ({55,  19}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_PAINTBRUSH),                 STR_DISABLE_ELEVATION),        // paint mode
        MakeWidget     ({27,  48}, {44, 32}, WindowWidgetType::ImgBtn,  WindowColour::Primary  , ImageId(SPR_LAND_TOOL_SIZE_0),           kStringIdNone),                     // preview box
        MakeRemapWidget({28,  49}, {16, 16}, WindowWidgetType::TrnBtn,  WindowColour::Secondary, SPR_LAND_TOOL_DECREASE,         STR_ADJUST_SMALLER_LAND_TIP),  // decrement size
        MakeRemapWidget({54,  63}, {16, 16}, WindowWidgetType::TrnBtn,  WindowColour::Secondary, SPR_LAND_TOOL_INCREASE,         STR_ADJUST_LARGER_LAND_TIP),   // increment size
        MakeWidget     ({ 2, 106}, {47, 36}, WindowWidgetType::FlatBtn, WindowColour::Secondary, 0xFFFFFFFF,                     STR_CHANGE_BASE_LAND_TIP),     // floor texture
        MakeWidget     ({49, 106}, {47, 36}, WindowWidgetType::FlatBtn, WindowColour::Secondary, 0xFFFFFFFF,                     STR_CHANGE_VERTICAL_LAND_TIP), // wall texture
    };
    // clang-format on

    class LandWindow final : public Window
    {
    private:
        bool _landToolBlocked = false;
        bool _landToolMountainMode = false;
        bool _landToolPaintMode = false;

        money64 _landToolRaiseCost = kMoney64Undefined;
        money64 _landToolLowerCost = kMoney64Undefined;

        ObjectEntryIndex _selectedFloorTexture = 0;
        ObjectEntryIndex _selectedWallTexture = 0;

        void InputSize()
        {
            Formatter ft;
            ft.Add<uint16_t>(kLandToolMinimumSize);
            ft.Add<uint16_t>(kLandToolMaximumSize);
            WindowTextInputOpen(
                this, WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, ft, kStringIdNone, kStringIdNone, 3);
        }

    public:
        void OnOpen() override
        {
            SetWidgets(window_land_widgets);
            hold_down_widgets = (1uLL << WIDX_DECREMENT) | (1uLL << WIDX_INCREMENT);
            WindowInitScrollWidgets(*this);
            WindowPushOthersBelow(*this);

            gLandToolSize = 1;
            gLandToolTerrainSurface = kObjectEntryIndexNull;
            gLandToolTerrainEdge = kObjectEntryIndexNull;

            _selectedFloorTexture = LandTool::GetSurfaceStyleFromDropdownIndex(0);
            _selectedWallTexture = LandTool::GetEdgeStyleFromDropdownIndex(0);
        }

        void OnClose() override
        {
            // If the tool wasn't changed, turn tool off
            if (isToolActive(WindowClass::Land, WIDX_BACKGROUND))
                ToolCancel();
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_MOUNTAINMODE:
                    _landToolMountainMode ^= 1;
                    _landToolPaintMode = 0;
                    Invalidate();
                    break;
                case WIDX_PAINTMODE:
                    _landToolMountainMode = 0;
                    _landToolPaintMode ^= 1;
                    Invalidate();
                    break;
                case WIDX_PREVIEW:
                    InputSize();
                    break;
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            Widget* widget = &widgets[widgetIndex];
            switch (widgetIndex)
            {
                case WIDX_FLOOR:
                    LandTool::ShowSurfaceStyleDropdown(this, widget, _selectedFloorTexture);
                    break;
                case WIDX_WALL:
                    LandTool::ShowEdgeStyleDropdown(this, widget, _selectedWallTexture);
                    break;
                case WIDX_PREVIEW:
                    InputSize();
                    break;
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

        void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            int32_t type;

            switch (widgetIndex)
            {
                case WIDX_FLOOR:
                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdownHighlightedIndex;

                    type = (dropdownIndex == -1)
                        ? _selectedFloorTexture
                        : LandTool::GetSurfaceStyleFromDropdownIndex(static_cast<size_t>(dropdownIndex));

                    if (gLandToolTerrainSurface == type)
                    {
                        gLandToolTerrainSurface = kObjectEntryIndexNull;
                    }
                    else
                    {
                        gLandToolTerrainSurface = type;
                        _selectedFloorTexture = type;
                    }
                    Invalidate();
                    break;
                case WIDX_WALL:
                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdownHighlightedIndex;

                    type = (dropdownIndex == -1) ? _selectedWallTexture
                                                 : LandTool::GetEdgeStyleFromDropdownIndex(static_cast<size_t>(dropdownIndex));

                    if (gLandToolTerrainEdge == type)
                    {
                        gLandToolTerrainEdge = kObjectEntryIndexNull;
                    }
                    else
                    {
                        gLandToolTerrainEdge = type;
                        _selectedWallTexture = type;
                    }
                    Invalidate();
                    break;
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (widgetIndex != WIDX_PREVIEW)
                return;

            char* end;
            std::string textStr = std::string(text);
            int32_t size = strtol(textStr.c_str(), &end, 10);
            if (*end == '\0')
            {
                size = std::max<uint16_t>(kLandToolMinimumSize, size);
                size = std::min<uint16_t>(kLandToolMaximumSize, size);
                gLandToolSize = size;

                Invalidate();
            }
        }

        void OnUpdate() override
        {
            if (!isToolActive(WindowClass::Land, WIDX_BACKGROUND))
                Close();
        }

        void OnPrepareDraw() override
        {
            pressed_widgets = 0;
            SetWidgetPressed(WIDX_PREVIEW, true);
            if (gLandToolTerrainSurface != kObjectEntryIndexNull)
                SetWidgetPressed(WIDX_FLOOR, true);
            if (gLandToolTerrainEdge != kObjectEntryIndexNull)
                SetWidgetPressed(WIDX_WALL, true);
            if (_landToolMountainMode)
                SetWidgetPressed(WIDX_MOUNTAINMODE, true);
            if (_landToolPaintMode)
                SetWidgetPressed(WIDX_PAINTMODE, true);

            // Update the preview image (for tool sizes up to 7)
            widgets[WIDX_PREVIEW].image = ImageId(LandTool::SizeToSpriteIndex(gLandToolSize));
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            ScreenCoordsXY screenCoords;
            int32_t numTiles;
            money64 price;
            Widget* previewWidget = &widgets[WIDX_PREVIEW];

            DrawWidgets(dpi);
            DrawDropdownButtons(dpi);

            // Draw number for tool sizes bigger than 7
            if (gLandToolSize > kLandToolMaximumSizeWithSprite)
            {
                auto ft = Formatter();
                ft.Add<uint16_t>(gLandToolSize);
                screenCoords = { windowPos.x + previewWidget->midX(), windowPos.y + previewWidget->midY() };
                DrawTextBasic(
                    dpi, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::CENTRE });
            }
            else if (_landToolMountainMode)
            {
                screenCoords = { windowPos.x + previewWidget->left, windowPos.y + previewWidget->top };
                auto sprite = ImageId(gLandToolSize % 2 == 0 ? SPR_G2_MOUNTAIN_TOOL_EVEN : SPR_G2_MOUNTAIN_TOOL_ODD);
                GfxDrawSprite(dpi, sprite, screenCoords);
                WidgetDraw(dpi, *this, WIDX_DECREMENT);
                WidgetDraw(dpi, *this, WIDX_INCREMENT);
            }

            screenCoords = { windowPos.x + previewWidget->midX(), windowPos.y + previewWidget->bottom + 5 };

            if (!(GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY))
            {
                // Draw raise cost amount
                if (_landToolRaiseCost != kMoney64Undefined && _landToolRaiseCost != 0)
                {
                    auto ft = Formatter();
                    ft.Add<money64>(_landToolRaiseCost);
                    DrawTextBasic(dpi, screenCoords, STR_RAISE_COST_AMOUNT, ft, { TextAlignment::CENTRE });
                }
                screenCoords.y += 10;

                // Draw lower cost amount
                if (_landToolLowerCost != kMoney64Undefined && _landToolLowerCost != 0)
                {
                    auto ft = Formatter();
                    ft.Add<money64>(_landToolLowerCost);
                    DrawTextBasic(dpi, screenCoords, STR_LOWER_COST_AMOUNT, ft, { TextAlignment::CENTRE });
                }
                screenCoords.y += 50;

                // Draw paint price
                numTiles = gLandToolSize * gLandToolSize;
                price = 0;
                if (gLandToolTerrainSurface != kObjectEntryIndexNull)
                {
                    auto& objManager = GetContext()->GetObjectManager();
                    const auto surfaceObj = static_cast<TerrainSurfaceObject*>(
                        objManager.GetLoadedObject(ObjectType::TerrainSurface, gLandToolTerrainSurface));
                    if (surfaceObj != nullptr)
                    {
                        price += numTiles * static_cast<money64>(surfaceObj->Price);
                    }
                }

                if (gLandToolTerrainEdge != kObjectEntryIndexNull)
                    price += numTiles * 100LL;

                if (price != 0)
                {
                    auto ft = Formatter();
                    ft.Add<money64>(price);
                    DrawTextBasic(dpi, screenCoords, STR_COST_AMOUNT, ft, { TextAlignment::CENTRE });
                }
            }
        }

        void OnResize() override
        {
            ResizeFrame();
        }

    private:
        /**
         *
         *  rct2: 0x006644DD
         */
        money64 SelectionRaiseLand(uint8_t flag)
        {
            int32_t centreX = (gMapSelectPositionA.x + gMapSelectPositionB.x) / 2;
            int32_t centreY = (gMapSelectPositionA.y + gMapSelectPositionB.y) / 2;
            centreX += 16;
            centreY += 16;

            if (_landToolMountainMode)
            {
                auto landSmoothAction = LandSmoothAction(
                    { centreX, centreY },
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                    gMapSelectType, false);
                auto res = (flag & GAME_COMMAND_FLAG_APPLY) ? GameActions::Execute(&landSmoothAction)
                                                            : GameActions::Query(&landSmoothAction);
                return res.Error == GameActions::Status::Ok ? res.Cost : kMoney64Undefined;
            }

            auto landRaiseAction = LandRaiseAction(
                { centreX, centreY },
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y }, gMapSelectType);
            auto res = (flag & GAME_COMMAND_FLAG_APPLY) ? GameActions::Execute(&landRaiseAction)
                                                        : GameActions::Query(&landRaiseAction);

            return res.Error == GameActions::Status::Ok ? res.Cost : kMoney64Undefined;
        }

        /**
         *
         *  rct2: 0x006645B3
         */
        money64 SelectionLowerLand(uint8_t flag)
        {
            int32_t centreX = (gMapSelectPositionA.x + gMapSelectPositionB.x) / 2;
            int32_t centreY = (gMapSelectPositionA.y + gMapSelectPositionB.y) / 2;
            centreX += 16;
            centreY += 16;

            if (_landToolMountainMode)
            {
                auto landSmoothAction = LandSmoothAction(
                    { centreX, centreY },
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                    gMapSelectType, true);
                auto res = (flag & GAME_COMMAND_FLAG_APPLY) ? GameActions::Execute(&landSmoothAction)
                                                            : GameActions::Query(&landSmoothAction);
                return res.Error == GameActions::Status::Ok ? res.Cost : kMoney64Undefined;
            }

            auto landLowerAction = LandLowerAction(
                { centreX, centreY },
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y }, gMapSelectType);
            auto res = (flag & GAME_COMMAND_FLAG_APPLY) ? GameActions::Execute(&landLowerAction)
                                                        : GameActions::Query(&landLowerAction);

            return res.Error == GameActions::Status::Ok ? res.Cost : kMoney64Undefined;
        }

        /**
         *  part of window_top_toolbar_tool_drag(0x0066CB4E)
         *  rct2: 0x00664454
         */
        void LandToolDrag(const ScreenCoordsXY& screenPos)
        {
            auto* windowMgr = GetWindowManager();
            auto* window = windowMgr->FindFromPoint(screenPos);
            if (window == nullptr)
                return;

            WidgetIndex widget_index = windowMgr->FindWidgetFromPoint(*window, screenPos);
            if (widget_index == kWidgetIndexNull)
                return;

            const auto& widget = window->widgets[widget_index];
            if (widget.type != WindowWidgetType::Viewport)
                return;

            const auto* selectedViewport = window->viewport;
            if (selectedViewport == nullptr)
                return;

            int16_t tile_height = selectedViewport->zoom.ApplyInversedTo(-16);

            int32_t y_diff = screenPos.y - gInputDragLast.y;

            if (y_diff <= tile_height)
            {
                gInputDragLast.y += tile_height;

                SelectionRaiseLand(GAME_COMMAND_FLAG_APPLY);

                _landToolRaiseCost = kMoney64Undefined;
                _landToolLowerCost = kMoney64Undefined;
            }
            else if (y_diff >= -tile_height)
            {
                gInputDragLast.y -= tile_height;

                SelectionLowerLand(GAME_COMMAND_FLAG_APPLY);

                _landToolRaiseCost = kMoney64Undefined;
                _landToolLowerCost = kMoney64Undefined;
            }
        }

        int8_t ToolUpdateLandPaint(const ScreenCoordsXY& screenPos)
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

    public:
        void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    if (_landToolPaintMode)
                        ToolUpdateLandPaint(screenCoords);
                    else
                        ToolUpdateLand(screenCoords);
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
                        auto surfaceSetStyleAction = SurfaceSetStyleAction(
                            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                            gLandToolTerrainSurface, gLandToolTerrainEdge);

                        GameActions::Execute(&surfaceSetStyleAction);

                        gCurrentToolId = Tool::UpDownArrow;
                    }
                    else
                    {
                        _landToolBlocked = true;
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
                    // Custom setting to only change land style instead of raising or lowering land
                    if (_landToolPaintMode)
                    {
                        if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
                        {
                            auto surfaceSetStyleAction = SurfaceSetStyleAction(
                                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                                gLandToolTerrainSurface, gLandToolTerrainEdge);

                            GameActions::Execute(&surfaceSetStyleAction);

                            // The tool is set to 12 here instead of 3 so that the dragging cursor is not the elevation change
                            // cursor
                            gCurrentToolId = Tool::Crosshair;
                        }
                    }
                    else
                    {
                        if (!_landToolBlocked)
                        {
                            LandToolDrag(screenCoords);
                        }
                    }
                    break;
                }
            }
        }

        void OnToolUp(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            _landToolBlocked = false;
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    MapInvalidateSelectionRect();
                    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
                    gCurrentToolId = Tool::DigDown;
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
         *
         *  rct2: 0x00664280
         */
        void ToolUpdateLand(const ScreenCoordsXY& screenPos)
        {
            const bool mapCtrlPressed = GetInputManager().IsModifierKeyPressed(ModifierKey::ctrl);
            auto* windowMgr = Ui::GetWindowManager();

            MapInvalidateSelectionRect();

            if (gCurrentToolId == Tool::UpDownArrow)
            {
                if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
                    return;

                money64 lower_cost = SelectionLowerLand(0);
                money64 raise_cost = SelectionRaiseLand(0);

                if (_landToolRaiseCost != raise_cost || _landToolLowerCost != lower_cost)
                {
                    _landToolRaiseCost = raise_cost;
                    _landToolLowerCost = lower_cost;
                    windowMgr->InvalidateByClass(WindowClass::Land);
                }
                return;
            }

            int16_t tool_size = gLandToolSize;
            std::optional<CoordsXY> mapTile;
            uint8_t side{};

            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            if (tool_size == 1)
            {
                int32_t selectionType;
                // Get selection type and map coordinates from mouse x,y position
                ScreenPosToMapPos(screenPos, &selectionType);
                mapTile = ScreenGetMapXYSide(screenPos, &side);

                if (!mapTile.has_value())
                {
                    money64 lower_cost = kMoney64Undefined;
                    money64 raise_cost = kMoney64Undefined;

                    if (_landToolRaiseCost != raise_cost || _landToolLowerCost != lower_cost)
                    {
                        _landToolRaiseCost = raise_cost;
                        _landToolLowerCost = lower_cost;
                        windowMgr->InvalidateByClass(WindowClass::Land);
                    }
                    return;
                }

                uint8_t state_changed = 0;

                if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
                {
                    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
                    state_changed++;
                }

                if (gMapSelectType != selectionType)
                {
                    gMapSelectType = selectionType;
                    state_changed++;
                }

                if ((gMapSelectType != MAP_SELECT_TYPE_EDGE_0 + (side & 0xFF)) && mapCtrlPressed)
                {
                    gMapSelectType = MAP_SELECT_TYPE_EDGE_0 + (side & 0xFF);
                    state_changed++;
                }

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
                if (!state_changed)
                    return;

                money64 lower_cost = SelectionLowerLand(0);
                money64 raise_cost = SelectionRaiseLand(0);

                if (_landToolRaiseCost != raise_cost || _landToolLowerCost != lower_cost)
                {
                    _landToolRaiseCost = raise_cost;
                    _landToolLowerCost = lower_cost;
                    windowMgr->InvalidateByClass(WindowClass::Land);
                }
                return;
            }

            // Get map coordinates and the side of the tile that is being hovered over
            mapTile = ScreenGetMapXYSide(screenPos, &side);

            if (!mapTile.has_value())
            {
                money64 lower_cost = kMoney64Undefined;
                money64 raise_cost = kMoney64Undefined;

                if (_landToolRaiseCost != raise_cost || _landToolLowerCost != lower_cost)
                {
                    _landToolRaiseCost = raise_cost;
                    _landToolLowerCost = lower_cost;
                    windowMgr->InvalidateByClass(WindowClass::Land);
                }
                return;
            }

            uint8_t state_changed = 0;

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

            if ((gMapSelectType != MAP_SELECT_TYPE_EDGE_0 + (side & 0xFF)) && mapCtrlPressed)
            {
                gMapSelectType = MAP_SELECT_TYPE_EDGE_0 + (side & 0xFF);
                state_changed++;
            }

            if (tool_size == 0)
                tool_size = 1;

            int16_t tool_length = (tool_size - 1) * 32;

            // Decide on shape of the brush for bigger selection size
            switch (gMapSelectType)
            {
                case MAP_SELECT_TYPE_EDGE_0:
                case MAP_SELECT_TYPE_EDGE_2:
                    // Line
                    mapTile->y -= (tool_size - 1) * 16;
                    mapTile->y = mapTile->ToTileStart().y;
                    break;
                case MAP_SELECT_TYPE_EDGE_1:
                case MAP_SELECT_TYPE_EDGE_3:
                    // Line
                    mapTile->x -= (tool_size - 1) * 16;
                    mapTile->x = mapTile->ToTileStart().x;
                    break;
                default:
                    // Move to tool bottom left
                    mapTile->x -= (tool_size - 1) * 16;
                    mapTile->y -= (tool_size - 1) * 16;
                    mapTile = mapTile->ToTileStart();
                    break;
            }

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

            // Go to other side
            switch (gMapSelectType)
            {
                case MAP_SELECT_TYPE_EDGE_0:
                case MAP_SELECT_TYPE_EDGE_2:
                    // Line
                    mapTile->y += tool_length;
                    gMapSelectType = MAP_SELECT_TYPE_FULL;
                    break;
                case MAP_SELECT_TYPE_EDGE_1:
                case MAP_SELECT_TYPE_EDGE_3:
                    // Line
                    mapTile->x += tool_length;
                    gMapSelectType = MAP_SELECT_TYPE_FULL;
                    break;
                default:
                    mapTile->x += tool_length;
                    mapTile->y += tool_length;
                    break;
            }

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
            if (!state_changed)
                return;

            money64 lower_cost = SelectionLowerLand(0);
            money64 raise_cost = SelectionRaiseLand(0);

            if (_landToolRaiseCost != raise_cost || _landToolLowerCost != lower_cost)
            {
                _landToolRaiseCost = raise_cost;
                _landToolLowerCost = lower_cost;
                windowMgr->InvalidateByClass(WindowClass::Land);
            }
        }

        void DrawDropdownButtons(DrawPixelInfo& dpi)
        {
            auto& objManager = GetContext()->GetObjectManager();
            const auto surfaceObj = static_cast<TerrainSurfaceObject*>(
                objManager.GetLoadedObject(ObjectType::TerrainSurface, _selectedFloorTexture));
            ImageId surfaceImage;
            if (surfaceObj != nullptr)
            {
                surfaceImage = ImageId(surfaceObj->IconImageId);
                if (surfaceObj->Colour != TerrainSurfaceObject::kNoValue)
                    surfaceImage = surfaceImage.WithPrimary(surfaceObj->Colour);
            }

            const auto edgeObj = static_cast<TerrainEdgeObject*>(
                objManager.GetLoadedObject(ObjectType::TerrainEdge, _selectedWallTexture));
            ImageId edgeImage;
            if (edgeObj != nullptr)
            {
                edgeImage = ImageId(edgeObj->IconImageId);
            }

            DrawDropdownButton(dpi, WIDX_FLOOR, surfaceImage);
            DrawDropdownButton(dpi, WIDX_WALL, edgeImage);
        }

        void DrawDropdownButton(DrawPixelInfo& dpi, WidgetIndex widgetIndex, ImageId image)
        {
            const auto& widget = widgets[widgetIndex];
            GfxDrawSprite(dpi, image, { windowPos.x + widget.left, windowPos.y + widget.top });
        }
    };

    WindowBase* LandOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<LandWindow>(WindowClass::Land, ScreenCoordsXY(ContextGetWidth() - WW, 29), WW, WH, 0);
    }

    /**
     *
     *  rct2: 0x0066CD54
     */
    void ToggleLandWindow()
    {
        if (isToolActive(WindowClass::Land, WIDX_BACKGROUND))
        {
            ToolCancel();
        }
        else
        {
            ShowGridlines();
            auto* toolWindow = ContextOpenWindow(WindowClass::Land);
            ToolSet(*toolWindow, WIDX_BACKGROUND, Tool::DigDown);
            InputSetFlag(INPUT_FLAG_6, true);
        }
    }
} // namespace OpenRCT2::Ui::Windows
