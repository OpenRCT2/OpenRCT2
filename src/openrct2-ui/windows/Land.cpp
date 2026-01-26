/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/LandLowerAction.h>
#include <openrct2/actions/LandRaiseAction.h>
#include <openrct2/actions/LandSmoothAction.h>
#include <openrct2/actions/SurfaceSetStyleAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/Park.h>

using OpenRCT2::GameActions::CommandFlag;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_LAND;
    static constexpr ScreenSize kWindowSize = { 98, 160 };

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

    enum class SelectionMode
    {
        query,
        apply,
    };

    // clang-format off
    static constexpr auto window_land_widgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget     ({19,  19}, {24, 24}, WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_UP), STR_ENABLE_MOUNTAIN_TOOL_TIP), // mountain mode
        makeWidget     ({55,  19}, {24, 24}, WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_PAINTBRUSH),                 STR_DISABLE_ELEVATION),        // paint mode
        makeWidget     ({27,  48}, {44, 32}, WidgetType::imgBtn,  WindowColour::primary  , ImageId(SPR_LAND_TOOL_SIZE_0),           kStringIdNone),                // preview box
        makeRemapWidget({28,  49}, {16, 16}, WidgetType::trnBtn,  WindowColour::secondary, SPR_LAND_TOOL_DECREASE,                  STR_ADJUST_SMALLER_LAND_TIP),  // decrement size
        makeRemapWidget({54,  63}, {16, 16}, WidgetType::trnBtn,  WindowColour::secondary, SPR_LAND_TOOL_INCREASE,                  STR_ADJUST_LARGER_LAND_TIP),   // increment size
        makeWidget     ({ 2, 106}, {47, 36}, WidgetType::flatBtn, WindowColour::secondary, 0xFFFFFFFF,                              STR_CHANGE_BASE_LAND_TIP),     // floor texture
        makeWidget     ({49, 106}, {47, 36}, WidgetType::flatBtn, WindowColour::secondary, 0xFFFFFFFF,                              STR_CHANGE_VERTICAL_LAND_TIP)  // wall texture
    );
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
        void onOpen() override
        {
            setWidgets(window_land_widgets);

            holdDownWidgets = (1uLL << WIDX_DECREMENT) | (1uLL << WIDX_INCREMENT);
            WindowInitScrollWidgets(*this);
            WindowPushOthersBelow(*this);

            gLandToolSize = 1;
            gLandToolTerrainSurface = kObjectEntryIndexNull;
            gLandToolTerrainEdge = kObjectEntryIndexNull;

            _selectedFloorTexture = LandTool::GetSurfaceStyleFromDropdownIndex(0);
            _selectedWallTexture = LandTool::GetEdgeStyleFromDropdownIndex(0);
        }

        void onClose() override
        {
            // If the tool wasn't changed, turn tool off
            if (isToolActive(WindowClass::land, WIDX_BACKGROUND))
                ToolCancel();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_MOUNTAINMODE:
                    _landToolMountainMode ^= 1;
                    _landToolPaintMode = false;
                    invalidate();
                    break;
                case WIDX_PAINTMODE:
                    _landToolMountainMode = false;
                    _landToolPaintMode ^= 1;
                    invalidate();
                    break;
                case WIDX_PREVIEW:
                    InputSize();
                    break;
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
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

        void onDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            int32_t type;

            switch (widgetIndex)
            {
                case WIDX_FLOOR:
                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdown.highlightedIndex;

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
                    invalidate();
                    break;
                case WIDX_WALL:
                    if (dropdownIndex == -1)
                        dropdownIndex = gDropdown.highlightedIndex;

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
                    invalidate();
                    break;
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
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

                invalidate();
            }
        }

        void onUpdate() override
        {
            if (!isToolActive(WindowClass::land, WIDX_BACKGROUND))
                close();
        }

        void onPrepareDraw() override
        {
            pressedWidgets = 0;
            setWidgetPressed(WIDX_PREVIEW, true);
            if (gLandToolTerrainSurface != kObjectEntryIndexNull)
                setWidgetPressed(WIDX_FLOOR, true);
            if (gLandToolTerrainEdge != kObjectEntryIndexNull)
                setWidgetPressed(WIDX_WALL, true);
            if (_landToolMountainMode)
                setWidgetPressed(WIDX_MOUNTAINMODE, true);
            if (_landToolPaintMode)
                setWidgetPressed(WIDX_PAINTMODE, true);

            // Update the preview image (for tool sizes up to 7)
            widgets[WIDX_PREVIEW].image = ImageId(LandTool::SizeToSpriteIndex(gLandToolSize));
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            ScreenCoordsXY screenCoords;
            int32_t numTiles;
            money64 price;
            Widget* previewWidget = &widgets[WIDX_PREVIEW];

            drawWidgets(rt);
            DrawDropdownButtons(rt);

            // Draw number for tool sizes bigger than 7
            if (gLandToolSize > kLandToolMaximumSizeWithSprite)
            {
                auto ft = Formatter();
                ft.Add<uint16_t>(gLandToolSize);
                screenCoords = { windowPos.x + previewWidget->midX(), windowPos.y + previewWidget->midY() };
                DrawTextBasic(
                    rt, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::centre });
            }
            else if (_landToolMountainMode)
            {
                screenCoords = { windowPos.x + previewWidget->left, windowPos.y + previewWidget->top };
                auto sprite = ImageId(gLandToolSize % 2 == 0 ? SPR_G2_MOUNTAIN_TOOL_EVEN : SPR_G2_MOUNTAIN_TOOL_ODD);
                GfxDrawSprite(rt, sprite, screenCoords);
                widgetDraw(rt, *this, WIDX_DECREMENT);
                widgetDraw(rt, *this, WIDX_INCREMENT);
            }

            screenCoords = { windowPos.x + previewWidget->midX(), windowPos.y + previewWidget->bottom + 5 };

            if (!(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
            {
                // Draw raise cost amount
                if (_landToolRaiseCost != kMoney64Undefined && _landToolRaiseCost != 0)
                {
                    auto ft = Formatter();
                    ft.Add<money64>(_landToolRaiseCost);
                    DrawTextBasic(rt, screenCoords, STR_RAISE_COST_AMOUNT, ft, { TextAlignment::centre });
                }
                screenCoords.y += 10;

                // Draw lower cost amount
                if (_landToolLowerCost != kMoney64Undefined && _landToolLowerCost != 0)
                {
                    auto ft = Formatter();
                    ft.Add<money64>(_landToolLowerCost);
                    DrawTextBasic(rt, screenCoords, STR_LOWER_COST_AMOUNT, ft, { TextAlignment::centre });
                }
                screenCoords.y += 50;

                // Draw paint price
                numTiles = gLandToolSize * gLandToolSize;
                price = 0;
                if (gLandToolTerrainSurface != kObjectEntryIndexNull)
                {
                    auto& objManager = GetContext()->GetObjectManager();
                    const auto* surfaceObj = objManager.GetLoadedObject<TerrainSurfaceObject>(gLandToolTerrainSurface);
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
                    DrawTextBasic(rt, screenCoords, STR_COST_AMOUNT, ft, { TextAlignment::centre });
                }
            }
        }

    private:
        static money64 executeGameAction(GameState_t& gameState, GameActions::GameAction* action, SelectionMode mode)
        {
            GameActions::Result res;
            if (mode == SelectionMode::apply)
            {
                res = GameActions::Execute(action, gameState);
            }
            else
            {
                action->SetFlags({ CommandFlag::allowDuringPaused });
                res = GameActions::Query(action, gameState);
            }
            return res.error == GameActions::Status::ok ? res.cost : kMoney64Undefined;
        }

        /**
         *
         *  rct2: 0x006644DD
         */
        money64 SelectionRaiseLand(const SelectionMode mode)
        {
            int32_t centreX = (gMapSelectPositionA.x + gMapSelectPositionB.x) / 2;
            int32_t centreY = (gMapSelectPositionA.y + gMapSelectPositionB.y) / 2;
            centreX += 16;
            centreY += 16;

            auto& gameState = getGameState();

            if (_landToolMountainMode)
            {
                auto landSmoothAction = GameActions::LandSmoothAction(
                    { centreX, centreY },
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                    gMapSelectType, false);
                return executeGameAction(gameState, &landSmoothAction, mode);
            }

            auto landRaiseAction = GameActions::LandRaiseAction(
                { centreX, centreY },
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y }, gMapSelectType);

            return executeGameAction(gameState, &landRaiseAction, mode);
        }

        /**
         *
         *  rct2: 0x006645B3
         */
        money64 SelectionLowerLand(const SelectionMode mode)
        {
            int32_t centreX = (gMapSelectPositionA.x + gMapSelectPositionB.x) / 2;
            int32_t centreY = (gMapSelectPositionA.y + gMapSelectPositionB.y) / 2;
            centreX += 16;
            centreY += 16;

            auto& gameState = getGameState();

            if (_landToolMountainMode)
            {
                auto landSmoothAction = GameActions::LandSmoothAction(
                    { centreX, centreY },
                    { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                    gMapSelectType, true);
                return executeGameAction(gameState, &landSmoothAction, mode);
            }

            auto landLowerAction = GameActions::LandLowerAction(
                { centreX, centreY },
                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y }, gMapSelectType);

            return executeGameAction(gameState, &landLowerAction, mode);
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
            if (widget.type != WidgetType::viewport)
                return;

            const auto* selectedViewport = window->viewport;
            if (selectedViewport == nullptr)
                return;

            int16_t tile_height = selectedViewport->zoom.ApplyInversedTo(-16);

            int32_t y_diff = screenPos.y - gInputDragLast.y;

            if (y_diff <= tile_height)
            {
                gInputDragLast.y += tile_height;

                SelectionRaiseLand(SelectionMode::apply);

                _landToolRaiseCost = kMoney64Undefined;
                _landToolLowerCost = kMoney64Undefined;
            }
            else if (y_diff >= -tile_height)
            {
                gInputDragLast.y -= tile_height;

                SelectionLowerLand(SelectionMode::apply);

                _landToolRaiseCost = kMoney64Undefined;
                _landToolLowerCost = kMoney64Undefined;
            }
        }

        int8_t ToolUpdateLandPaint(const ScreenCoordsXY& screenPos)
        {
            uint8_t state_changed = 0;

            gMapSelectFlags.unset(MapSelectFlag::enable);

            auto mapTile = ScreenGetMapXY(screenPos, nullptr);

            if (!mapTile.has_value())
            {
                return state_changed;
            }

            if (!gMapSelectFlags.has(MapSelectFlag::enable))
            {
                gMapSelectFlags.set(MapSelectFlag::enable);
                state_changed++;
            }

            if (gMapSelectType != MapSelectType::full)
            {
                gMapSelectType = MapSelectType::full;
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

            return state_changed;
        }

    public:
        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
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

        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    if (gMapSelectFlags.has(MapSelectFlag::enable))
                    {
                        auto surfaceSetStyleAction = GameActions::SurfaceSetStyleAction(
                            { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                            gLandToolTerrainSurface, gLandToolTerrainEdge);

                        GameActions::Execute(&surfaceSetStyleAction, getGameState());

                        gCurrentToolId = Tool::upDownArrow;
                    }
                    else
                    {
                        _landToolBlocked = true;
                    }
                    break;
            }
        }

        void onToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                {
                    // Custom setting to only change land style instead of raising or lowering land
                    if (_landToolPaintMode)
                    {
                        if (gMapSelectFlags.has(MapSelectFlag::enable))
                        {
                            auto surfaceSetStyleAction = GameActions::SurfaceSetStyleAction(
                                { gMapSelectPositionA.x, gMapSelectPositionA.y, gMapSelectPositionB.x, gMapSelectPositionB.y },
                                gLandToolTerrainSurface, gLandToolTerrainEdge);

                            GameActions::Execute(&surfaceSetStyleAction, getGameState());

                            // The tool is set to 12 here instead of 3 so that the dragging cursor is not the elevation change
                            // cursor
                            gCurrentToolId = Tool::crosshair;
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

        void onToolUp(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            _landToolBlocked = false;
            switch (widgetIndex)
            {
                case WIDX_BACKGROUND:
                    gMapSelectFlags.unset(MapSelectFlag::enable);
                    gCurrentToolId = Tool::digDown;
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
         *
         *  rct2: 0x00664280
         */
        void ToolUpdateLand(const ScreenCoordsXY& screenPos)
        {
            const bool mapCtrlPressed = GetInputManager().isModifierKeyPressed(ModifierKey::ctrl);
            auto* windowMgr = GetWindowManager();

            if (gCurrentToolId == Tool::upDownArrow)
            {
                if (!gMapSelectFlags.has(MapSelectFlag::enable))
                    return;

                money64 lower_cost = SelectionLowerLand(SelectionMode::query);
                money64 raise_cost = SelectionRaiseLand(SelectionMode::query);

                if (_landToolRaiseCost != raise_cost || _landToolLowerCost != lower_cost)
                {
                    _landToolRaiseCost = raise_cost;
                    _landToolLowerCost = lower_cost;
                    windowMgr->InvalidateByClass(WindowClass::land);
                }
                return;
            }

            int16_t tool_size = gLandToolSize;
            std::optional<CoordsXY> mapTile;
            uint8_t side{};

            gMapSelectFlags.unset(MapSelectFlag::enable);
            if (tool_size == 1)
            {
                int32_t direction;
                // Get selection type and map coordinates from mouse x,y position
                ScreenPosToMapPos(screenPos, &direction);
                MapSelectType selectionType = static_cast<MapSelectType>(direction);
                mapTile = ScreenGetMapXYSide(screenPos, &side);

                if (!mapTile.has_value())
                {
                    money64 lower_cost = kMoney64Undefined;
                    money64 raise_cost = kMoney64Undefined;

                    if (_landToolRaiseCost != raise_cost || _landToolLowerCost != lower_cost)
                    {
                        _landToolRaiseCost = raise_cost;
                        _landToolLowerCost = lower_cost;
                        windowMgr->InvalidateByClass(WindowClass::land);
                    }
                    return;
                }

                uint8_t state_changed = 0;

                if (!gMapSelectFlags.has(MapSelectFlag::enable))
                {
                    gMapSelectFlags.set(MapSelectFlag::enable);
                    state_changed++;
                }

                if (gMapSelectType != selectionType)
                {
                    gMapSelectType = selectionType;
                    state_changed++;
                }

                MapSelectType selectedSide = getMapSelectEdge(side & 0xFF);
                if (gMapSelectType != selectedSide && mapCtrlPressed)
                {
                    gMapSelectType = selectedSide;
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

                if (!state_changed)
                    return;

                money64 lower_cost = SelectionLowerLand(SelectionMode::query);
                money64 raise_cost = SelectionRaiseLand(SelectionMode::query);

                if (_landToolRaiseCost != raise_cost || _landToolLowerCost != lower_cost)
                {
                    _landToolRaiseCost = raise_cost;
                    _landToolLowerCost = lower_cost;
                    windowMgr->InvalidateByClass(WindowClass::land);
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
                    windowMgr->InvalidateByClass(WindowClass::land);
                }
                return;
            }

            uint8_t state_changed = 0;

            if (!gMapSelectFlags.has(MapSelectFlag::enable))
            {
                gMapSelectFlags.set(MapSelectFlag::enable);
                state_changed++;
            }

            if (gMapSelectType != MapSelectType::full)
            {
                gMapSelectType = MapSelectType::full;
                state_changed++;
            }

            MapSelectType selectedSide = getMapSelectEdge(side & 0xFF);
            if (gMapSelectType != selectedSide && mapCtrlPressed)
            {
                gMapSelectType = selectedSide;
                state_changed++;
            }

            if (tool_size == 0)
                tool_size = 1;

            int16_t tool_length = (tool_size - 1) * 32;

            // Decide on shape of the brush for bigger selection size
            switch (gMapSelectType)
            {
                case MapSelectType::edge0:
                case MapSelectType::edge2:
                    // Line
                    mapTile->y -= (tool_size - 1) * 16;
                    mapTile->y = mapTile->ToTileStart().y;
                    break;
                case MapSelectType::edge1:
                case MapSelectType::edge3:
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
                case MapSelectType::edge0:
                case MapSelectType::edge2:
                    // Line
                    mapTile->y += tool_length;
                    gMapSelectType = MapSelectType::full;
                    break;
                case MapSelectType::edge1:
                case MapSelectType::edge3:
                    // Line
                    mapTile->x += tool_length;
                    gMapSelectType = MapSelectType::full;
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

            if (!state_changed)
                return;

            money64 lower_cost = SelectionLowerLand(SelectionMode::query);
            money64 raise_cost = SelectionRaiseLand(SelectionMode::query);

            if (_landToolRaiseCost != raise_cost || _landToolLowerCost != lower_cost)
            {
                _landToolRaiseCost = raise_cost;
                _landToolLowerCost = lower_cost;
                windowMgr->InvalidateByClass(WindowClass::land);
            }
        }

        void DrawDropdownButtons(Drawing::RenderTarget& rt)
        {
            auto& objManager = GetContext()->GetObjectManager();
            const auto* surfaceObj = objManager.GetLoadedObject<TerrainSurfaceObject>(_selectedFloorTexture);
            ImageId surfaceImage;
            if (surfaceObj != nullptr)
            {
                surfaceImage = ImageId(surfaceObj->IconImageId);
                if (surfaceObj->Colour != Drawing::kColourNull)
                    surfaceImage = surfaceImage.WithPrimary(surfaceObj->Colour);
            }

            const auto edgeObj = objManager.GetLoadedObject<TerrainEdgeObject>(_selectedWallTexture);
            ImageId edgeImage;
            if (edgeObj != nullptr)
            {
                edgeImage = ImageId(edgeObj->IconImageId);
            }

            DrawDropdownButton(rt, WIDX_FLOOR, surfaceImage);
            DrawDropdownButton(rt, WIDX_WALL, edgeImage);
        }

        void DrawDropdownButton(Drawing::RenderTarget& rt, WidgetIndex widgetIndex, ImageId image)
        {
            const auto& widget = widgets[widgetIndex];
            GfxDrawSprite(rt, image, { windowPos.x + widget.left, windowPos.y + widget.top });
        }
    };

    WindowBase* LandOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<LandWindow>(
            WindowClass::land, ScreenCoordsXY(ContextGetWidth() - kWindowSize.width, 29), kWindowSize, {});
    }

    /**
     *
     *  rct2: 0x0066CD54
     */
    void ToggleLandWindow()
    {
        if (isToolActive(WindowClass::land, WIDX_BACKGROUND))
        {
            ToolCancel();
        }
        else
        {
            ShowGridlines();
            auto* toolWindow = ContextOpenWindow(WindowClass::land);
            ToolSet(*toolWindow, WIDX_BACKGROUND, Tool::digDown);
            gInputFlags.set(InputFlag::allowRightMouseRemoval);
        }
    }
} // namespace OpenRCT2::Ui::Windows
