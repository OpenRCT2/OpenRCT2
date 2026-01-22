/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/ProvisionalElements.h>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/input/ShortcutIds.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/ViewportInteraction.h>
#include <openrct2-ui/interface/ViewportQuery.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/FootpathPlaceAction.h>
#include <openrct2/actions/FootpathRemoveAction.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/core/FlagHolder.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/network/NetworkAction.h>
#include <openrct2/object/FootpathObject.h>
#include <openrct2/object/FootpathRailingsObject.h>
#include <openrct2/object/FootpathSurfaceObject.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/paint/VirtualFloor.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/ConstructionClearance.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/tile_element/PathElement.h>
#include <openrct2/world/tile_element/Slope.h>
#include <openrct2/world/tile_element/SurfaceElement.h>

using namespace OpenRCT2::Numerics;
using OpenRCT2::GameActions::CommandFlag;

namespace OpenRCT2::Ui::Windows
{
    struct ProvisionalTile
    {
        CoordsXYZ position;
        FootpathSlope slope;

        constexpr bool operator==(const ProvisionalTile& rhs) const
        {
            return position == rhs.position && slope == rhs.slope;
        }
    };

    static money64 FootpathProvisionalSet(
        ObjectEntryIndex type, ObjectEntryIndex railingsType, const CoordsXY& footpathLocA, const CoordsXY& footpathLocB,
        int32_t startZ, std::span<const ProvisionalTile> tiles, PathConstructFlags constructFlags);

    enum class PathConstructionMode : uint8_t
    {
        onLand,
        dragArea,
        /**
         * When picking a location to start the bridge or tunnel
         */
        bridgeOrTunnelPick,
        /**
         * When actually building a bridge or tunnel (enables the appropriate buttons)
         */
        bridgeOrTunnel,
    };

    enum class ProvisionalPathFlag : uint8_t
    {
        showArrow = 0,
        /**
         * Set when any provisional path is present.
         */
        placed = 1,
        forceRecheck = 2,
    };

    enum class SlopePitch : uint8_t
    {
        flat,
        upwards,
        downwards,
    };

    using ProvisionalPathFlags = FlagHolder<uint8_t, ProvisionalPathFlag>;

    struct ProvisionalFootpath
    {
        ObjectEntryIndex type{};
        CoordsXY positionA{};
        CoordsXY positionB{};
        /**
         * Z of the first tile. Used for checking if the provisional path needs updating.
         */
        int32_t startZ{};
        ProvisionalPathFlags flags{};
        ObjectEntryIndex surfaceIndex{};
        ObjectEntryIndex railingsIndex{};
        PathConstructFlags constructFlags{};
        std::vector<ProvisionalTile> tiles{};
    };

    static ProvisionalFootpath _provisionalFootpath;
    static CoordsXYZ _footpathConstructFromPosition;
    static SlopePitch _footpathConstructSlope;

#pragma region Measurements

    static constexpr StringId kWindowTitle = STR_FOOTPATHS;
    static constexpr ScreenSize kWindowSize = { 106, 461 };

#pragma endregion

#pragma region Widgets

    enum WindowFootpathWidgetIdx : WidgetIndex
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,

        WIDX_TYPE_GROUP,
        WIDX_FOOTPATH_TYPE,
        WIDX_QUEUELINE_TYPE,
        WIDX_RAILINGS_TYPE,

        WIDX_DIRECTION_GROUP,
        WIDX_DIRECTION_NW,
        WIDX_DIRECTION_NE,
        WIDX_DIRECTION_SW,
        WIDX_DIRECTION_SE,

        WIDX_SLOPE_GROUP,
        WIDX_SLOPEDOWN,
        WIDX_LEVEL,
        WIDX_SLOPEUP,
        WIDX_CONSTRUCT,
        WIDX_REMOVE,

        WIDX_MODE_GROUP,
        WIDX_CONSTRUCT_ON_LAND,
        WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL,
        WIDX_CONSTRUCT_DRAG_AREA,
    };

    // clang-format off
    static constexpr auto kWindowFootpathWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),

        // Type group
        makeWidget({ 3,  17}, {100, 95}, WidgetType::groupbox, WindowColour::primary  , STR_TYPE                                                                                   ),
        makeWidget({ 6,  30}, { 47, 36}, WidgetType::flatBtn,  WindowColour::secondary, 0xFFFFFFFF,                             STR_FOOTPATH_TIP                                   ),
        makeWidget({53,  30}, { 47, 36}, WidgetType::flatBtn,  WindowColour::secondary, 0xFFFFFFFF,                             STR_QUEUE_LINE_PATH_TIP                            ),
        makeWidget({29,  69}, { 47, 36}, WidgetType::flatBtn,  WindowColour::secondary, 0xFFFFFFFF,                             STR_OBJECT_SELECTION_FOOTPATH_RAILINGS             ),

        // Direction group
        makeWidget({ 3, 115}, {100, 77}, WidgetType::groupbox, WindowColour::primary  , STR_DIRECTION                                                                              ),
        makeWidget({53, 127}, { 45, 29}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_CONSTRUCTION_DIRECTION_NE),     STR_DIRECTION_TIP                              ),
        makeWidget({53, 156}, { 45, 29}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_CONSTRUCTION_DIRECTION_SE),     STR_DIRECTION_TIP                              ),
        makeWidget({ 8, 156}, { 45, 29}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_CONSTRUCTION_DIRECTION_SW),     STR_DIRECTION_TIP                              ),
        makeWidget({ 8, 127}, { 45, 29}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_CONSTRUCTION_DIRECTION_NW),     STR_DIRECTION_TIP                              ),

        // Slope group
        makeWidget({ 3, 195}, {100, 41}, WidgetType::groupbox, WindowColour::primary  , STR_SLOPE                                                                                  ),
        makeWidget({17, 207}, { 24, 24}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_DOWN),  STR_SLOPE_DOWN_TIP                             ),
        makeWidget({41, 207}, { 24, 24}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_LEVEL), STR_LEVEL_TIP                                  ),
        makeWidget({65, 207}, { 24, 24}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_UP),    STR_SLOPE_UP_TIP                               ),
        makeWidget({ 8, 242}, { 90, 90}, WidgetType::flatBtn,  WindowColour::secondary, 0xFFFFFFFF,                                 STR_CONSTRUCT_THE_SELECTED_FOOTPATH_SECTION_TIP),
        makeWidget({30, 335}, { 46, 24}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_DEMOLISH_CURRENT_SECTION),      STR_REMOVE_PREVIOUS_FOOTPATH_SECTION_TIP       ),

        // Mode group
        makeWidget({ 3, 361}, {100, 99}, WidgetType::groupbox, WindowColour::primary                                                                                               ),
        makeWidget({13, 372}, { 36, 36}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_CONSTRUCTION_FOOTPATH_LAND),    STR_CONSTRUCT_FOOTPATH_ON_LAND_TIP             ),
        makeWidget({57, 372}, { 36, 36}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_CONSTRUCTION_FOOTPATH_BRIDGE),  STR_CONSTRUCT_BRIDGE_OR_TUNNEL_FOOTPATH_TIP    ),
        makeWidget({35, 416}, { 36, 36}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_G2_ICON_PATH_DRAG_TOOL),        STR_DRAG_AREA_OF_FOOTPATH_TIP                  )
    );

#pragma endregion

    /** rct2: 0x0098D7E0 */
    static constexpr uint8_t kConstructionPreviewImages[][4] = {
        { 5, 10, 5, 10 },   // SlopePitch::flat
        { 16, 17, 18, 19 }, // SlopePitch::upwards
        { 18, 19, 16, 17 }, // SlopePitch::downwards
    };
    // clang-format on

    class FootpathWindow final : public Window
    {
    private:
        const uint16_t ARROW_PULSE_DURATION = 200;

        uint8_t _footpathConstructDirection;
        uint8_t _footpathConstructValidDirections;
        PathConstructionMode _footpathConstructionMode;

        std::vector<std::pair<ObjectType, ObjectEntryIndex>> _dropdownEntries;

        money64 _windowFootpathCost;
        uint32_t _footpathConstructionNextArrowPulse = 0;
        uint8_t _lastUpdatedCameraRotation = UINT8_MAX;
        bool _footpathErrorOccured = false;

        bool _footpathPlaceCtrlState;
        int32_t _footpathPlaceCtrlZ;

        bool _footpathPlaceShiftState;
        ScreenCoordsXY _footpathPlaceShiftStart;
        int32_t _footpathPlaceShiftZ;

        int32_t _footpathPlaceZ;

        CoordsXY _dragStartPos;

    public:
#pragma region Window Override Events

        void onOpen() override
        {
            setWidgets(kWindowFootpathWidgets);

            WindowInitScrollWidgets(*this);
            WindowPushOthersRight(*this);
            ShowGridlines();

            ToolCancel();
            _footpathConstructionMode = PathConstructionMode::onLand;
            ToolSet(*this, WIDX_CONSTRUCT_ON_LAND, Tool::pathDown);
            gInputFlags.set(InputFlag::allowRightMouseRemoval);
            _footpathErrorOccured = false;
            WindowFootpathSetEnabledAndPressedWidgets();

            _footpathPlaceCtrlState = false;
            _footpathPlaceShiftState = false;

            holdDownWidgets = (1u << WIDX_CONSTRUCT) | (1u << WIDX_REMOVE);
        }

        void onClose() override
        {
            FootpathUpdateProvisional();
            ViewportSetVisibility(ViewportVisibility::standard);
            gMapSelectFlags.unset(MapSelectFlag::enableConstruct);

            auto* windowMgr = GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::topToolbar);
            HideGridlines();
        }

        void onUpdate() override
        {
            WindowFootpathUpdateProvisionalPathForBridgeMode();

            // #2502: The camera might have changed rotation, so we need to update which directional buttons are pressed
            uint8_t currentRotation = GetCurrentRotation();
            if (_lastUpdatedCameraRotation != currentRotation)
            {
                _lastUpdatedCameraRotation = currentRotation;
                WindowFootpathSetEnabledAndPressedWidgets();
            }

            // Check tool
            switch (_footpathConstructionMode)
            {
                case PathConstructionMode::onLand:
                    if (!isToolActive(WindowClass::footpath, WIDX_CONSTRUCT_ON_LAND))
                        close();
                    break;
                case PathConstructionMode::dragArea:
                    // If another window has enabled a tool, close ours.
                    // If the user merely pressed Escape, we cancel the tool but don’t close the window.
                    if (gInputFlags.has(InputFlag::toolActive)
                        && gCurrentToolWidget.windowClassification != WindowClass::footpath)
                        close();
                    break;
                case PathConstructionMode::bridgeOrTunnelPick:
                    if (!isToolActive(WindowClass::footpath, WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL))
                        close();
                    break;
                case PathConstructionMode::bridgeOrTunnel:
                    break;
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_FOOTPATH_TYPE:
                    WindowFootpathShowFootpathTypesDialog(&widgets[widgetIndex], false);
                    break;
                case WIDX_QUEUELINE_TYPE:
                    WindowFootpathShowFootpathTypesDialog(&widgets[widgetIndex], true);
                    break;
                case WIDX_RAILINGS_TYPE:
                    WindowFootpathShowRailingsTypesDialog(&widgets[widgetIndex]);
                    break;
                case WIDX_DIRECTION_NW:
                    WindowFootpathMousedownDirection(0);
                    break;
                case WIDX_DIRECTION_NE:
                    WindowFootpathMousedownDirection(1);
                    break;
                case WIDX_DIRECTION_SW:
                    WindowFootpathMousedownDirection(2);
                    break;
                case WIDX_DIRECTION_SE:
                    WindowFootpathMousedownDirection(3);
                    break;
                case WIDX_SLOPEDOWN:
                    WindowFootpathMousedownSlope(SlopePitch::downwards);
                    break;
                case WIDX_LEVEL:
                    WindowFootpathMousedownSlope(SlopePitch::flat);
                    break;
                case WIDX_SLOPEUP:
                    WindowFootpathMousedownSlope(SlopePitch::upwards);
                    break;
                case WIDX_CONSTRUCT:
                    WindowFootpathConstruct();
                    break;
                case WIDX_REMOVE:
                    WindowFootpathRemove();
                    break;
            }
        }

        void enableDragAreaMode()
        {
            _windowFootpathCost = kMoney64Undefined;
            ToolCancel();
            FootpathUpdateProvisional();
            gMapSelectFlags.unset(MapSelectFlag::enableConstruct);
            _footpathConstructionMode = PathConstructionMode::dragArea;
            ToolSet(*this, WIDX_CONSTRUCT_DRAG_AREA, Tool::pathDown);
            gInputFlags.set(InputFlag::allowRightMouseRemoval);
            _footpathErrorOccured = false;
            WindowFootpathSetEnabledAndPressedWidgets();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_CONSTRUCT_ON_LAND:
                    if (_footpathConstructionMode == PathConstructionMode::onLand)
                    {
                        break;
                    }

                    _windowFootpathCost = kMoney64Undefined;
                    ToolCancel();
                    FootpathUpdateProvisional();
                    gMapSelectFlags.unset(MapSelectFlag::enableConstruct);
                    _footpathConstructionMode = PathConstructionMode::onLand;
                    ToolSet(*this, WIDX_CONSTRUCT_ON_LAND, Tool::pathDown);
                    gInputFlags.set(InputFlag::allowRightMouseRemoval);
                    _footpathErrorOccured = false;
                    WindowFootpathSetEnabledAndPressedWidgets();
                    break;
                case WIDX_CONSTRUCT_DRAG_AREA:
                    if (_footpathConstructionMode == PathConstructionMode::dragArea)
                    {
                        break;
                    }

                    enableDragAreaMode();
                    break;
                case WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL:
                    if (_footpathConstructionMode == PathConstructionMode::bridgeOrTunnelPick)
                    {
                        break;
                    }

                    _windowFootpathCost = kMoney64Undefined;
                    ToolCancel();
                    FootpathUpdateProvisional();
                    gMapSelectFlags.unset(MapSelectFlag::enableConstruct);
                    _footpathConstructionMode = PathConstructionMode::bridgeOrTunnelPick;
                    ToolSet(*this, WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL, Tool::crosshair);
                    gInputFlags.set(InputFlag::allowRightMouseRemoval);
                    _footpathErrorOccured = false;
                    WindowFootpathSetEnabledAndPressedWidgets();
                    break;
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (selectedIndex < 0 || static_cast<size_t>(selectedIndex) >= _dropdownEntries.size())
                return;

            auto entryIndex = _dropdownEntries[selectedIndex];
            if (widgetIndex == WIDX_FOOTPATH_TYPE)
            {
                gFootpathSelection.isQueueSelected = false;
                if (entryIndex.first == ObjectType::paths)
                {
                    gFootpathSelection.legacyPath = entryIndex.second;
                }
                else
                {
                    gFootpathSelection.legacyPath = kObjectEntryIndexNull;
                    gFootpathSelection.normalSurface = entryIndex.second;
                }
            }
            else if (widgetIndex == WIDX_QUEUELINE_TYPE)
            {
                gFootpathSelection.isQueueSelected = true;
                if (entryIndex.first == ObjectType::paths)
                {
                    gFootpathSelection.legacyPath = entryIndex.second;
                }
                else
                {
                    gFootpathSelection.legacyPath = kObjectEntryIndexNull;
                    gFootpathSelection.queueSurface = entryIndex.second;
                }
            }
            else if (widgetIndex == WIDX_RAILINGS_TYPE)
            {
                gFootpathSelection.railings = entryIndex.second;
            }
            else
            {
                return;
            }

            FootpathUpdateProvisional();
            _windowFootpathCost = kMoney64Undefined;
            invalidate();
        }

        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
            {
                WindowFootpathSetProvisionalPathAtPoint(screenCoords);
            }
            else if (widgetIndex == WIDX_CONSTRUCT_DRAG_AREA)
            {
                WindowFootpathPlaceDragAreaHover(screenCoords);
            }
            else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
            {
                WindowFootpathSetSelectionStartBridgeAtPoint(screenCoords);
            }
        }

        void onToolUp(WidgetIndex widgetIndex, const ScreenCoordsXY&) override
        {
            if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
            {
                _footpathErrorOccured = false;
            }
            else if (widgetIndex == WIDX_CONSTRUCT_DRAG_AREA)
            {
                WindowFootpathPlacePath();
                _footpathErrorOccured = false;
            }
        }

        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
            {
                WindowFootpathPlacePathAtPoint(screenCoords);
            }
            else if (widgetIndex == WIDX_CONSTRUCT_DRAG_AREA)
            {
                WindowFootpathPlaceDragAreaSetStart(screenCoords);
            }
            else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
            {
                WindowFootpathStartBridgeAtPoint(screenCoords);
            }
        }

        void onToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
            {
                WindowFootpathPlacePathAtPoint(screenCoords);
            }
            else if (widgetIndex == WIDX_CONSTRUCT_DRAG_AREA)
            {
                WindowFootpathPlaceDragAreaSetEnd(screenCoords);
            }
        }

        void onToolAbort(WidgetIndex widgetIndex) override
        {
            // Needs both checks as this will otherwise be triggered when switching from the dragArea tool
            // to another.
            if (widgetIndex == WIDX_CONSTRUCT_DRAG_AREA && _footpathConstructionMode == PathConstructionMode::dragArea)
            {
                FootpathUpdateProvisional();
                enableDragAreaMode();
                // When this tool is aborted, the mouse button is still down.
                // Set this to prevent a stray piece of path from being placed under the mouse pointer.
                _footpathErrorOccured = true;
            }
        }

        void onPrepareDraw() override
        {
            // Press / unpress footpath and queue type buttons
            pressedWidgets &= ~(1uLL << WIDX_FOOTPATH_TYPE);
            pressedWidgets &= ~(1uLL << WIDX_QUEUELINE_TYPE);
            pressedWidgets |= gFootpathSelection.isQueueSelected ? (1uLL << WIDX_QUEUELINE_TYPE) : (1uLL << WIDX_FOOTPATH_TYPE);

            // Enable / disable construct button
            widgets[WIDX_CONSTRUCT].type = _footpathConstructionMode == PathConstructionMode::bridgeOrTunnel
                ? WidgetType::imgBtn
                : WidgetType::empty;

#ifndef DISABLE_NETWORK
            bool canDrag = true;
            if (Network::GetMode() == Network::Mode::client)
            {
                canDrag = Network::CanPerformAction(Network::GetCurrentPlayerGroupIndex(), Network::Permission::dragPathArea);
            }
            if (canDrag)
                disabledWidgets &= ~(1uLL << WIDX_CONSTRUCT_DRAG_AREA);
            else
                disabledWidgets |= (1uLL << WIDX_CONSTRUCT_DRAG_AREA);
#endif

            if (gFootpathSelection.legacyPath == kObjectEntryIndexNull)
            {
                widgets[WIDX_RAILINGS_TYPE].type = WidgetType::flatBtn;
            }
            else
            {
                widgets[WIDX_RAILINGS_TYPE].type = WidgetType::empty;
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            ScreenCoordsXY screenCoords;
            WindowDrawWidgets(*this, rt);
            WindowFootpathDrawDropdownButtons(rt);

            if (!isWidgetDisabled(WIDX_CONSTRUCT))
            {
                // Get construction image
                uint8_t direction = (_footpathConstructDirection + GetCurrentRotation()) % kNumOrthogonalDirections;
                auto slopeOffset = EnumValue(_footpathConstructSlope);

                std::optional<uint32_t> baseImage;
                if (gFootpathSelection.legacyPath == kObjectEntryIndexNull)
                {
                    auto selectedPath = gFootpathSelection.getSelectedSurface();
                    const auto* pathType = GetPathSurfaceEntry(selectedPath);
                    if (pathType != nullptr)
                    {
                        baseImage = pathType->BaseImageId;
                    }
                }
                else
                {
                    auto& objManager = GetContext()->GetObjectManager();
                    const auto* pathObj = objManager.GetLoadedObject<FootpathObject>(gFootpathSelection.legacyPath);
                    if (pathObj != nullptr)
                    {
                        auto pathEntry = reinterpret_cast<const FootpathEntry*>(pathObj->GetLegacyData());
                        if (gFootpathSelection.isQueueSelected)
                            baseImage = pathEntry->GetQueueImage();
                        else
                            baseImage = pathEntry->image;
                    }
                }

                if (baseImage)
                {
                    auto image = *baseImage + kConstructionPreviewImages[slopeOffset][direction];

                    // Draw construction image
                    screenCoords = this->windowPos
                        + ScreenCoordsXY{ widgets[WIDX_CONSTRUCT].midX(), widgets[WIDX_CONSTRUCT].bottom - 60 };
                    GfxDrawSprite(rt, ImageId(image), screenCoords);
                }

                // Draw build this... label
                screenCoords = this->windowPos
                    + ScreenCoordsXY{ widgets[WIDX_CONSTRUCT].midX(), widgets[WIDX_CONSTRUCT].bottom - 23 };
                DrawTextBasic(rt, screenCoords, STR_BUILD_THIS, {}, { TextAlignment::centre });
            }

            // Draw cost
            screenCoords = this->windowPos
                + ScreenCoordsXY{ widgets[WIDX_CONSTRUCT].midX(), widgets[WIDX_CONSTRUCT].bottom - 12 };
            if (_windowFootpathCost != kMoney64Undefined)
            {
                if (!(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
                {
                    auto ft = Formatter();
                    ft.Add<money64>(_windowFootpathCost);
                    DrawTextBasic(rt, screenCoords, STR_COST_LABEL, ft, { TextAlignment::centre });
                }
            }
        }

#pragma endregion

    private:
        /**
         *
         *  rct2: 0x006A7760
         */
        void WindowFootpathUpdateProvisionalPathForBridgeMode()
        {
            if (_footpathConstructionMode != PathConstructionMode::bridgeOrTunnel)
            {
                return;
            }

            // Recheck area for construction. Set by ride_construction window
            if (_provisionalFootpath.flags.has(ProvisionalPathFlag::forceRecheck))
            {
                FootpathRemoveProvisional();
                _provisionalFootpath.flags.unset(ProvisionalPathFlag::forceRecheck);
            }

            // Update provisional bridge mode path
            if (!_provisionalFootpath.flags.has(ProvisionalPathFlag::placed))
            {
                ObjectEntryIndex type;
                ObjectEntryIndex railings = gFootpathSelection.railings;

                CoordsXYZ footpathLoc;
                FootpathSlope slope;
                FootpathGetNextPathInfo(&type, footpathLoc, &slope);
                auto pathConstructFlags = FootpathCreateConstructFlags(type);
                auto tiles = std::array<ProvisionalTile, 1>({ footpathLoc, slope });

                _windowFootpathCost = FootpathProvisionalSet(
                    type, railings, footpathLoc, footpathLoc, footpathLoc.z, tiles, pathConstructFlags);
                invalidateWidget(WIDX_CONSTRUCT);
            }

            auto curTime = Platform::GetTicks();

            // Update little directional arrow on provisional bridge mode path
            if (_footpathConstructionNextArrowPulse < curTime)
            {
                _footpathConstructionNextArrowPulse = curTime + ARROW_PULSE_DURATION;

                _provisionalFootpath.flags.flip(ProvisionalPathFlag::showArrow);
                CoordsXYZ footpathLoc;
                FootpathSlope slope;
                FootpathGetNextPathInfo(nullptr, footpathLoc, &slope);
                gMapSelectArrowPosition = footpathLoc;
                gMapSelectArrowDirection = _footpathConstructDirection;
                if (_provisionalFootpath.flags.has(ProvisionalPathFlag::showArrow))
                {
                    gMapSelectFlags.set(MapSelectFlag::enableArrow);
                }
                else
                {
                    gMapSelectFlags.unset(MapSelectFlag::enableArrow);
                }
            }
        }

        void WindowFootpathDrawDropdownButtons(Drawing::RenderTarget& rt)
        {
            if (gFootpathSelection.legacyPath == kObjectEntryIndexNull)
            {
                // Set footpath and queue type button images
                auto pathImage = kImageIndexUndefined;
                auto queueImage = kImageIndexUndefined;
                auto pathEntry = GetPathSurfaceEntry(gFootpathSelection.normalSurface);
                if (pathEntry != nullptr)
                {
                    pathImage = pathEntry->PreviewImageId;
                }

                pathEntry = GetPathSurfaceEntry(gFootpathSelection.queueSurface);
                if (pathEntry != nullptr)
                {
                    queueImage = pathEntry->PreviewImageId;
                }

                WindowFootpathDrawDropdownButton(rt, WIDX_FOOTPATH_TYPE, pathImage);
                WindowFootpathDrawDropdownButton(rt, WIDX_QUEUELINE_TYPE, queueImage);

                // Set railing
                auto railingsImage = kImageIndexUndefined;
                auto railingsEntry = GetPathRailingsEntry(gFootpathSelection.railings);
                if (railingsEntry != nullptr)
                {
                    railingsImage = railingsEntry->PreviewImageId;
                }
                WindowFootpathDrawDropdownButton(rt, WIDX_RAILINGS_TYPE, railingsImage);
            }
            else
            {
                auto& objManager = GetContext()->GetObjectManager();

                // Set footpath and queue type button images
                auto pathImage = kImageIndexUndefined;
                auto queueImage = kImageIndexUndefined;
                const auto* pathObj = objManager.GetLoadedObject<FootpathObject>(gFootpathSelection.legacyPath);
                if (pathObj != nullptr)
                {
                    auto pathEntry = reinterpret_cast<const FootpathEntry*>(pathObj->GetLegacyData());
                    pathImage = pathEntry->GetPreviewImage();
                    queueImage = pathEntry->GetQueuePreviewImage();
                }

                WindowFootpathDrawDropdownButton(rt, WIDX_FOOTPATH_TYPE, pathImage);
                WindowFootpathDrawDropdownButton(rt, WIDX_QUEUELINE_TYPE, queueImage);
            }
        }

        void WindowFootpathDrawDropdownButton(Drawing::RenderTarget& rt, WidgetIndex widgetIndex, ImageIndex image)
        {
            const auto& widget = widgets[widgetIndex];
            GfxDrawSprite(rt, ImageId(image), { windowPos.x + widget.left, windowPos.y + widget.top });
        }

        /**
         *
         *  rct2: 0x006A7F88
         */
        void WindowFootpathShowFootpathTypesDialog(Widget* widget, bool showQueues)
        {
            auto& objManager = GetContext()->GetObjectManager();

            uint32_t numPathTypes = 0;
            // If the game is in sandbox mode, also show paths that are normally restricted to the scenario editor
            bool showEditorPaths = (gLegacyScene == LegacyScene::scenarioEditor || getGameState().cheats.sandboxMode);

            _dropdownEntries.clear();
            std::optional<size_t> defaultIndex;
            for (ObjectEntryIndex i = 0; i < kMaxFootpathSurfaceObjects; i++)
            {
                const auto* pathType = objManager.GetLoadedObject<FootpathSurfaceObject>(i);
                if (pathType == nullptr)
                {
                    continue;
                }
                if ((pathType->Flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) && !showEditorPaths)
                {
                    continue;
                }
                // If regular paths can be used as queue, only hide the path if we’re _not_ looking at a queue,
                // but the path surface is one.
                if (getGameState().cheats.allowRegularPathAsQueue && !showQueues
                    && ((pathType->Flags & FOOTPATH_ENTRY_FLAG_IS_QUEUE) != 0))
                {
                    continue;
                }
                // If the cheat is disabled, hide queues from the regular path view and vice versa.
                else if (
                    !getGameState().cheats.allowRegularPathAsQueue
                    && showQueues != ((pathType->Flags & FOOTPATH_ENTRY_FLAG_IS_QUEUE) != 0))
                {
                    continue;
                }

                if (gFootpathSelection.legacyPath == kObjectEntryIndexNull
                    && i == (showQueues ? gFootpathSelection.queueSurface : gFootpathSelection.normalSurface))
                {
                    defaultIndex = numPathTypes;
                }

                gDropdown.items[numPathTypes] = Dropdown::ImageItem(ImageId(pathType->PreviewImageId), pathType->NameStringId);
                _dropdownEntries.push_back({ ObjectType::footpathSurface, i });
                numPathTypes++;
            }

            for (ObjectEntryIndex i = 0; i < kMaxPathObjects; i++)
            {
                auto* pathObj = objManager.GetLoadedObject<FootpathObject>(i);
                if (pathObj == nullptr)
                {
                    continue;
                }

                auto pathEntry = reinterpret_cast<const FootpathEntry*>(pathObj->GetLegacyData());
                if ((pathEntry->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) && !showEditorPaths)
                {
                    continue;
                }

                if (gFootpathSelection.legacyPath != kObjectEntryIndexNull && gFootpathSelection.legacyPath == i)
                {
                    defaultIndex = numPathTypes;
                }

                auto image = ImageId(showQueues ? pathEntry->GetQueuePreviewImage() : pathEntry->GetPreviewImage());
                gDropdown.items[numPathTypes] = Dropdown::ImageItem(image, pathEntry->string_idx);
                _dropdownEntries.push_back({ ObjectType::paths, i });
                numPathTypes++;
            }

            auto itemsPerRow = DropdownGetAppropriateImageDropdownItemsPerRow(numPathTypes);
            WindowDropdownShowImage(
                windowPos.x + widget->left, windowPos.y + widget->top, widget->height(), colours[1], 0, numPathTypes, 47, 36,
                itemsPerRow);

            gDropdown.hasTooltips = true;

            if (defaultIndex)
                gDropdown.defaultIndex = static_cast<int32_t>(*defaultIndex);
        }

        void WindowFootpathShowRailingsTypesDialog(Widget* widget)
        {
            uint32_t numRailingsTypes = 0;
            // If the game is in sandbox mode, also show paths that are normally restricted to the scenario editor

            _dropdownEntries.clear();
            std::optional<size_t> defaultIndex;
            for (int32_t i = 0; i < kMaxFootpathRailingsObjects; i++)
            {
                const auto* railingsEntry = GetPathRailingsEntry(i);
                if (railingsEntry == nullptr)
                {
                    continue;
                }
                if (i == gFootpathSelection.railings)
                {
                    defaultIndex = numRailingsTypes;
                }

                gDropdown.items[numRailingsTypes] = Dropdown::ImageItem(
                    ImageId(railingsEntry->PreviewImageId), railingsEntry->NameStringId);
                _dropdownEntries.push_back({ ObjectType::footpathRailings, i });
                numRailingsTypes++;
            }

            auto itemsPerRow = DropdownGetAppropriateImageDropdownItemsPerRow(numRailingsTypes);
            WindowDropdownShowImage(
                windowPos.x + widget->left, windowPos.y + widget->top, widget->height(), colours[1], 0, numRailingsTypes, 47,
                36, itemsPerRow);

            gDropdown.hasTooltips = true;

            if (defaultIndex)
                gDropdown.defaultIndex = static_cast<int32_t>(*defaultIndex);
        }

        /**
         *
         *  rct2: 0x006A8111 0x006A8135 0x006A815C 0x006A8183
         */
        void WindowFootpathMousedownDirection(int32_t direction)
        {
            FootpathUpdateProvisional();
            _footpathConstructDirection = (direction - GetCurrentRotation()) & 3;
            _windowFootpathCost = kMoney64Undefined;
            WindowFootpathSetEnabledAndPressedWidgets();
        }

        /**
         *
         *  rct2: 0x006A81AA 0x006A81C5 0x006A81E0
         */
        void WindowFootpathMousedownSlope(SlopePitch pitch)
        {
            FootpathUpdateProvisional();
            _footpathConstructSlope = pitch;
            _windowFootpathCost = kMoney64Undefined;
            WindowFootpathSetEnabledAndPressedWidgets();
        }

        void WindowFootpathUpdateModifierKeyState(ScreenCoordsXY& screenCoords)
        {
            auto& im = GetInputManager();

            // First, store the initial copy/ctrl state
            if (!_footpathPlaceCtrlState && im.isModifierKeyPressed(ModifierKey::ctrl))
            {
                constexpr auto interactionFlags = EnumsToFlags(
                    ViewportInteractionItem::terrain, ViewportInteractionItem::ride, ViewportInteractionItem::scenery,
                    ViewportInteractionItem::footpath, ViewportInteractionItem::wall, ViewportInteractionItem::largeScenery);

                auto info = GetMapCoordinatesFromPos(screenCoords, interactionFlags);
                if (info.interactionType != ViewportInteractionItem::none)
                {
                    const bool allowInvalidHeights = getGameState().cheats.allowTrackPlaceInvalidHeights;
                    const auto heightStep = kCoordsZStep * (!allowInvalidHeights ? 2 : 1);

                    _footpathPlaceCtrlZ = floor2(info.Element->GetBaseZ(), heightStep);
                    _footpathPlaceCtrlState = true;
                }
            }
            else if (!im.isModifierKeyPressed(ModifierKey::ctrl))
            {
                _footpathPlaceCtrlState = false;
                _footpathPlaceCtrlZ = 0;
            }

            // In addition, vertical shifting on top of the base (copy) placement?
            if (!_footpathPlaceShiftState && im.isModifierKeyPressed(ModifierKey::shift))
            {
                _footpathPlaceShiftState = true;
                _footpathPlaceShiftStart = screenCoords;
                _footpathPlaceShiftZ = 0;
            }
            else if (im.isModifierKeyPressed(ModifierKey::shift))
            {
                uint16_t maxPathHeight = ZoomLevel::max().ApplyTo(
                    std::numeric_limits<decltype(TileElement::BaseHeight)>::max() - 32);

                _footpathPlaceShiftZ = _footpathPlaceShiftStart.y - screenCoords.y + 4;
                // Scale delta by zoom to match mouse position.
                auto* mainWnd = WindowGetMain();
                if (mainWnd != nullptr && mainWnd->viewport != nullptr)
                {
                    _footpathPlaceShiftZ = mainWnd->viewport->zoom.ApplyTo(_footpathPlaceShiftZ);
                }

                const bool allowInvalidHeights = getGameState().cheats.allowTrackPlaceInvalidHeights;
                const auto heightStep = kCoordsZStep * (!allowInvalidHeights ? 2 : 1);
                _footpathPlaceShiftZ = floor2(_footpathPlaceShiftZ, heightStep);

                // Clamp to maximum possible value of BaseHeight can offer.
                _footpathPlaceShiftZ = std::min<int16_t>(_footpathPlaceShiftZ, maxPathHeight);

                screenCoords = _footpathPlaceShiftStart;
            }
            else if (_footpathPlaceShiftState)
            {
                _footpathPlaceShiftState = false;
                _footpathPlaceShiftZ = 0;
            }
        }

        std::optional<CoordsXY> FootpathGetPlacePositionFromScreenPosition(ScreenCoordsXY screenCoords)
        {
            WindowFootpathUpdateModifierKeyState(screenCoords);

            CoordsXY mapCoords;

            if (!_footpathPlaceCtrlState)
            {
                auto info = GetMapCoordinatesFromPos(
                    screenCoords, EnumsToFlags(ViewportInteractionItem::terrain, ViewportInteractionItem::footpath));

                if (info.interactionType == ViewportInteractionItem::none)
                    return std::nullopt;

                mapCoords = info.Loc;
                _footpathPlaceZ = 0;

                if (_footpathPlaceShiftState)
                {
                    auto res = FootpathGetPlacementFromInfo(info);
                    auto mapZ = res.baseZ + _footpathPlaceShiftZ;
                    mapZ = std::max<int16_t>(mapZ, 16);
                    _footpathPlaceZ = mapZ;
                }
            }
            else
            {
                auto mapZ = _footpathPlaceCtrlZ;
                auto mapXYCoords = ScreenGetMapXYWithZ(screenCoords, mapZ);
                if (mapXYCoords.has_value())
                {
                    mapCoords = mapXYCoords.value();
                }
                else
                {
                    return std::nullopt;
                }

                if (_footpathPlaceShiftState != 0)
                {
                    mapZ += _footpathPlaceShiftZ;
                }
                _footpathPlaceZ = std::max<int32_t>(mapZ, 16);
            }

            if (mapCoords.x == kLocationNull)
                return std::nullopt;

            return mapCoords.ToTileStart();
        }

        FootpathPlacementResult FootpathGetPlacementFromInfo(const InteractionInfo& info)
        {
            if (info.interactionType == ViewportInteractionItem::none || info.Element == nullptr)
            {
                gMapSelectFlags.unset(MapSelectFlag::enable);
                FootpathUpdateProvisional();
                return {};
            }

            switch (info.interactionType)
            {
                case ViewportInteractionItem::terrain:
                {
                    auto surfaceElement = info.Element->AsSurface();
                    if (surfaceElement != nullptr)
                    {
                        return FootpathGetOnTerrainPlacement(*surfaceElement);
                    }
                    break;
                }
                case ViewportInteractionItem::footpath:
                {
                    auto pathElement = info.Element->AsPath();
                    if (pathElement != nullptr)
                    {
                        auto slopeDirection = pathElement->GetSlopeDirection();
                        FootpathSlope slope = { FootpathSlopeType::flat, slopeDirection };
                        if (pathElement->IsSloped())
                        {
                            slope.type = FootpathSlopeType::sloped;
                        }
                        return { pathElement->GetBaseZ(), slope };
                    }
                    break;
                }
                default:
                    break;
            }

            return {};
        }

        /**
         *
         *  rct2: 0x006A81FB
         */
        void WindowFootpathSetProvisionalPathAtPoint(const ScreenCoordsXY& screenCoords)
        {
            gMapSelectFlags.unset(MapSelectFlag::enableArrow);

            // Get current map pos and handle key modifier state
            auto mapPos = FootpathGetPlacePositionFromScreenPosition(screenCoords);
            if (!mapPos)
                return;

            // Check for change
            if (_provisionalFootpath.flags.has(ProvisionalPathFlag::placed) && _provisionalFootpath.positionA == mapPos
                && _provisionalFootpath.startZ == _footpathPlaceZ)
            {
                return;
            }

            // Set map selection
            gMapSelectFlags.set(MapSelectFlag::enable);
            gMapSelectType = MapSelectType::full;

            setMapSelectRange(*mapPos);
            FootpathUpdateProvisional();

            // Figure out what slope and height to use
            auto placement = WindowFootpathGetPlacementFromScreenCoords(screenCoords);
            if (!placement.isValid())
            {
                gMapSelectFlags.unset(MapSelectFlag::enable);
                FootpathUpdateProvisional();
                return;
            }

            // Set provisional path
            auto pathType = gFootpathSelection.getSelectedSurface();
            auto constructFlags = FootpathCreateConstructFlags(pathType);
            auto footpathLoc = CoordsXYZ(*mapPos, placement.baseZ);
            auto tiles = std::array<ProvisionalTile, 1>({ footpathLoc, placement.slope });
            const auto footpathCost = FootpathProvisionalSet(
                pathType, gFootpathSelection.railings, *mapPos, *mapPos, placement.baseZ, tiles, constructFlags);

            if (_windowFootpathCost != footpathCost)
            {
                _windowFootpathCost = footpathCost;
                invalidateWidget(WIDX_CONSTRUCT);
            }
        }

        static std::vector<ProvisionalTile> buildTileVector(MapRange range, int32_t baseZ)
        {
            std::vector<ProvisionalTile> tiles{};
            for (auto y = range.GetY1(); y <= range.GetY2(); y += kCoordsXYStep)
            {
                for (auto x = range.GetX1(); x <= range.GetX2(); x += kCoordsXYStep)
                {
                    FootpathPlacementResult placement = { baseZ, {} };
                    if (baseZ == 0)
                        placement = FootpathGetOnTerrainPlacement(TileCoordsXY(CoordsXY(x, y)));

                    auto calculatedLocation = CoordsXYZ(x, y, placement.baseZ);
                    tiles.push_back({ calculatedLocation, placement.slope });
                }
            }

            return tiles;
        }

        void WindowFootpathSetProvisionalPathDragArea(MapRange range, int32_t baseZ)
        {
            gMapSelectFlags.unset(MapSelectFlag::enableArrow);

            // Check for change
            if (_provisionalFootpath.flags.has(ProvisionalPathFlag::placed) && range.Point1 == _provisionalFootpath.positionA
                && range.Point2 == _provisionalFootpath.positionB && baseZ == _provisionalFootpath.startZ)
            {
                return;
            }

            // Set map selection
            gMapSelectFlags.set(MapSelectFlag::enable);
            gMapSelectType = MapSelectType::full;

            FootpathUpdateProvisional();

            // Set provisional path
            auto tiles = buildTileVector(range, baseZ);
            auto pathType = gFootpathSelection.getSelectedSurface();
            auto constructFlags = FootpathCreateConstructFlags(pathType);
            const auto footpathCost = FootpathProvisionalSet(
                pathType, gFootpathSelection.railings, range.Point1, range.Point2, baseZ, tiles, constructFlags);

            if (_windowFootpathCost != footpathCost)
            {
                _windowFootpathCost = footpathCost;
                invalidateWidget(WIDX_CONSTRUCT);
            }
        }

        /**
         *
         *  rct2: 0x006A8388
         */
        void WindowFootpathSetSelectionStartBridgeAtPoint(const ScreenCoordsXY& screenCoords)
        {
            int32_t direction;
            TileElement* tileElement;

            gMapSelectFlags.unset(MapSelectFlag::enable, MapSelectFlag::enableArrow);

            auto mapCoords = FootpathBridgeGetInfoFromPos(screenCoords, &direction, &tileElement);
            if (mapCoords.IsNull())
            {
                return;
            }

            gMapSelectFlags.set(MapSelectFlag::enable, MapSelectFlag::enableArrow);
            gMapSelectType = MapSelectType::full;
            setMapSelectRange(mapCoords);

            int32_t z = tileElement->GetBaseZ();

            if (tileElement->GetType() == TileElementType::Surface)
            {
                uint8_t slope = tileElement->AsSurface()->GetSlope();
                if (slope & kTileSlopeRaisedCornersMask)
                {
                    z += kPathHeightStep;
                } // Add 2 for a slope
                if (slope & kTileSlopeDiagonalFlag)
                    z += kPathHeightStep; // Add another 2 for a steep slope
            }

            gMapSelectArrowPosition = CoordsXYZ{ mapCoords, z };
            gMapSelectArrowDirection = direction;
        }

        FootpathPlacementResult WindowFootpathGetPlacementFromScreenCoords(const ScreenCoordsXY& screenCoords)
        {
            if (_footpathPlaceZ != 0)
                return { _footpathPlaceZ, { FootpathSlopeType::flat } };

            const auto info = GetMapCoordinatesFromPos(
                screenCoords, EnumsToFlags(ViewportInteractionItem::terrain, ViewportInteractionItem::footpath));
            return FootpathGetPlacementFromInfo(info);
        }

        void WindowFootpathPlaceDragAreaSetStart(const ScreenCoordsXY& screenCoords)
        {
            if (_footpathErrorOccured)
            {
                return;
            }
            _provisionalFootpath.tiles.clear();

            auto mapPos = FootpathGetPlacePositionFromScreenPosition(screenCoords);
            if (!mapPos)
                return;

            auto placement = WindowFootpathGetPlacementFromScreenCoords(screenCoords);
            if (placement.baseZ > 0 && placement.slope.type == FootpathSlopeType::flat)
                _footpathPlaceZ = placement.baseZ;

            gMapSelectFlags.set(MapSelectFlag::enable);
            gMapSelectType = MapSelectType::full;

            auto correctedPosition = mapPos->ToTileStart();
            setMapSelectRange(correctedPosition);
            _dragStartPos = correctedPosition;
            WindowFootpathSetProvisionalPathDragArea(getMapSelectRange(), placement.baseZ);
        }

        void WindowFootpathPlaceDragAreaSetEnd(const ScreenCoordsXY& screenCoords)
        {
            if (_footpathErrorOccured)
            {
                return;
            }

            std::optional<CoordsXY> mapPos;
            if (_footpathPlaceShiftState)
                mapPos = ScreenGetMapXYWithZ(screenCoords, _footpathPlaceZ);
            else
                mapPos = FootpathGetPlacePositionFromScreenPosition(screenCoords);

            if (!mapPos)
            {
                return;
            }

            auto correctedPos = mapPos->ToTileStart();

            // For queues, only allow selecting a single line.
            if (gFootpathSelection.isQueueSelected)
            {
                auto xDiff = correctedPos.x - _dragStartPos.x;
                auto yDiff = correctedPos.y - _dragStartPos.y;
                if (std::abs(xDiff) > std::abs(yDiff))
                {
                    correctedPos.y = _dragStartPos.y;
                }
                else
                {
                    correctedPos.x = _dragStartPos.x;
                }
            }

            setMapSelectRange({ _dragStartPos, correctedPos });
            WindowFootpathSetProvisionalPathDragArea(getMapSelectRange(), _footpathPlaceZ);
        }

        void WindowFootpathPlaceDragAreaHover(const ScreenCoordsXY& screenCoords)
        {
            bool isLeftMousePressed = gInputFlags.has(InputFlag::leftMousePressed);
            if (isLeftMousePressed)
                return;

            WindowFootpathSetProvisionalPathAtPoint(screenCoords);
        }

        void WindowFootpathPlacePath()
        {
            if (_footpathErrorOccured)
            {
                return;
            }

            FootpathUpdateProvisional();
            if (_provisionalFootpath.tiles.size() == 0)
                return;

            // Try and place path
            auto selectedType = gFootpathSelection.getSelectedSurface();
            PathConstructFlags constructFlags = FootpathCreateConstructFlags(selectedType);

            bool anySuccess = false;
            money64 cost = 0;
            CoordsXYZ lastLocation;

            for (const auto& tile : _provisionalFootpath.tiles)
            {
                auto footpathPlaceAction = GameActions::FootpathPlaceAction(
                    tile.position, tile.slope, selectedType, gFootpathSelection.railings, kInvalidDirection, constructFlags);
                auto result = GameActions::Execute(&footpathPlaceAction, getGameState());
                if (result.error == GameActions::Status::ok)
                {
                    anySuccess = true;
                    cost += result.cost;
                }
                lastLocation = tile.position;
            }

            if (anySuccess)
            {
                // Don't play sound if it is no cost to prevent multiple sounds. TODO: make this work in no
                // money scenarios
                if (cost != 0)
                {
                    Audio::Play3D(Audio::SoundId::placeItem, lastLocation);
                }
            }
            else
            {
                _footpathErrorOccured = true;
            }
        }

        /**
         *
         *  rct2: 0x006A82C5
         */
        void WindowFootpathPlacePathAtPoint(const ScreenCoordsXY& screenCoords)
        {
            if (_footpathErrorOccured)
            {
                return;
            }

            FootpathUpdateProvisional();

            auto mapPos = FootpathGetPlacePositionFromScreenPosition(screenCoords);
            if (!mapPos)
                return;

            auto placement = WindowFootpathGetPlacementFromScreenCoords(screenCoords);

            // Try and place path
            auto selectedType = gFootpathSelection.getSelectedSurface();
            PathConstructFlags constructFlags = FootpathCreateConstructFlags(selectedType);

            auto footpathPlaceAction = GameActions::FootpathPlaceAction(
                { *mapPos, placement.baseZ }, placement.slope, selectedType, gFootpathSelection.railings, kInvalidDirection,
                constructFlags);
            footpathPlaceAction.SetCallback([this](const GameActions::GameAction* ga, const GameActions::Result* result) {
                if (result->error == GameActions::Status::ok)
                {
                    // Don't play sound if it is no cost to prevent multiple sounds. TODO: make this work in no money scenarios
                    if (result->cost != 0)
                    {
                        Audio::Play3D(Audio::SoundId::placeItem, result->position);
                    }
                }
                else
                {
                    _footpathErrorOccured = true;
                }
            });
            GameActions::Execute(&footpathPlaceAction, getGameState());
        }

        /**
         *
         *  rct2: 0x006A840F
         */
        void WindowFootpathStartBridgeAtPoint(const ScreenCoordsXY& screenCoords)
        {
            int32_t z, direction;
            TileElement* tileElement;

            auto mapCoords = FootpathBridgeGetInfoFromPos(screenCoords, &direction, &tileElement);
            if (mapCoords.IsNull())
            {
                return;
            }

            if (tileElement->GetType() == TileElementType::Surface)
            {
                // If we start the path on a slope, the arrow is slightly raised, so we
                // expect the path to be slightly raised as well.
                uint8_t slope = tileElement->AsSurface()->GetSlope();
                z = tileElement->GetBaseZ();
                if (slope & kTileSlopeDiagonalFlag)
                {
                    // Steep diagonal slope
                    z += 2 * kPathHeightStep;
                }
                else if (slope & kTileSlopeRaisedCornersMask)
                {
                    // Normal slope
                    z += kPathHeightStep;
                }
            }
            else
            {
                z = tileElement->GetBaseZ();
                if (tileElement->GetType() == TileElementType::Path)
                {
                    if (tileElement->AsPath()->IsSloped())
                    {
                        if (direction == (tileElement->AsPath()->GetSlopeDirection()))
                        {
                            z += kPathHeightStep;
                        }
                    }
                }
            }

            ToolCancel();
            _footpathConstructFromPosition = { mapCoords, z };
            _footpathConstructDirection = direction;
            _provisionalFootpath.flags.clearAll();
            _footpathConstructSlope = SlopePitch::flat;
            _footpathConstructionMode = PathConstructionMode::bridgeOrTunnel;
            _footpathConstructValidDirections = kInvalidDirection;
            WindowFootpathSetEnabledAndPressedWidgets();
        }

        /**
         * Construct a piece of footpath while in bridge building mode.
         *  rct2: 0x006A79B7
         */
        void WindowFootpathConstruct()
        {
            _windowFootpathCost = kMoney64Undefined;
            FootpathUpdateProvisional();

            ObjectEntryIndex type;
            FootpathSlope slope;
            CoordsXYZ footpathLoc;
            FootpathGetNextPathInfo(&type, footpathLoc, &slope);

            PathConstructFlags constructFlags = FootpathCreateConstructFlags(type);

            auto footpathPlaceAction = GameActions::FootpathPlaceAction(
                footpathLoc, slope, type, gFootpathSelection.railings, _footpathConstructDirection, constructFlags);

            footpathPlaceAction.SetCallback(
                [footpathLoc](const GameActions::GameAction* ga, const GameActions::Result* result) {
                    if (result->error == GameActions::Status::ok)
                    {
                        Audio::Play3D(Audio::SoundId::placeItem, result->position);
                    }

                    auto* windowMgr = GetWindowManager();
                    auto* self = static_cast<FootpathWindow*>(windowMgr->FindByClass(WindowClass::footpath));
                    if (self == nullptr)
                    {
                        return;
                    }

                    if (result->error == GameActions::Status::ok)
                    {
                        if (_footpathConstructSlope == SlopePitch::flat)
                        {
                            self->_footpathConstructValidDirections = kInvalidDirection;
                        }
                        else
                        {
                            self->_footpathConstructValidDirections = self->_footpathConstructDirection;
                        }

                        if (gFootpathGroundFlags & ELEMENT_IS_UNDERGROUND)
                        {
                            ViewportSetVisibility(ViewportVisibility::undergroundViewOn);
                        }

                        _footpathConstructFromPosition = footpathLoc;
                        // If we have just built an upwards slope, the next path to construct is
                        // a bit higher. Note that the z returned by footpath_get_next_path_info
                        // already is lowered if we are building a downwards slope.
                        if (_footpathConstructSlope == SlopePitch::upwards)
                        {
                            _footpathConstructFromPosition.z += kPathHeightStep;
                        }
                    }
                    self->WindowFootpathSetEnabledAndPressedWidgets();
                });
            GameActions::Execute(&footpathPlaceAction, getGameState());
        }

        /**
         *
         *  rct2: 0x006A78EF
         */
        void FootpathRemoveTileElement(TileElement* tileElement)
        {
            auto z = tileElement->GetBaseZ();
            if (tileElement->AsPath()->IsSloped())
            {
                uint8_t slopeDirection = tileElement->AsPath()->GetSlopeDirection();
                slopeDirection = DirectionReverse(slopeDirection);
                if (slopeDirection == _footpathConstructDirection)
                {
                    z += kPathHeightStep;
                }
            }

            // Find a connected edge
            int32_t edge = DirectionReverse(_footpathConstructDirection);
            if (!(tileElement->AsPath()->GetEdges() & (1 << edge)))
            {
                edge = (edge + 1) & 3;
                if (!(tileElement->AsPath()->GetEdges() & (1 << edge)))
                {
                    edge = (edge + 2) & 3;
                    if (!(tileElement->AsPath()->GetEdges() & (1 << edge)))
                    {
                        edge = (edge - 1) & 3;
                        if (!(tileElement->AsPath()->GetEdges() & (1 << edge)))
                        {
                            edge = DirectionReverse(edge);
                        }
                    }
                }
            }

            _footpathConstructFromPosition.z = tileElement->GetBaseZ();
            auto action = GameActions::FootpathRemoveAction(_footpathConstructFromPosition);
            GameActions::Execute(&action, getGameState());

            // Move selection
            edge = DirectionReverse(edge);
            _footpathConstructFromPosition.x -= CoordsDirectionDelta[edge].x;
            _footpathConstructFromPosition.y -= CoordsDirectionDelta[edge].y;
            _footpathConstructFromPosition.z = z;
            _footpathConstructDirection = edge;
            _footpathConstructValidDirections = kInvalidDirection;
        }

        /**
         *
         *  rct2: 0x006A7873
         */
        TileElement* FootpathGetTileElementToRemove()
        {
            TileElement* tileElement;
            int32_t z, zLow;

            if (!MapIsLocationValid(_footpathConstructFromPosition))
            {
                return nullptr;
            }

            z = std::min(255 * kCoordsZStep, _footpathConstructFromPosition.z);
            zLow = z - kPathHeightStep;

            tileElement = MapGetFirstElementAt(_footpathConstructFromPosition);
            do
            {
                if (tileElement == nullptr)
                    break;
                if (tileElement->GetType() == TileElementType::Path)
                {
                    if (tileElement->GetBaseZ() == z)
                    {
                        if (tileElement->AsPath()->IsSloped())
                        {
                            if (DirectionReverse(tileElement->AsPath()->GetSlopeDirection()) != _footpathConstructDirection)
                            {
                                continue;
                            }
                        }

                        return tileElement;
                    }
                    if (tileElement->GetBaseZ() == zLow)
                    {
                        if (!tileElement->AsPath()->IsSloped())
                        {
                            if ((tileElement->AsPath()->GetSlopeDirection()) == _footpathConstructDirection)
                            {
                                continue;
                            }
                        }

                        return tileElement;
                    }
                }
            } while (!(tileElement++)->IsLastForTile());

            return nullptr;
        }

        /**
         *
         *  rct2: 0x006A7863
         */
        void WindowFootpathRemove()
        {
            TileElement* tileElement;

            _windowFootpathCost = kMoney64Undefined;
            FootpathUpdateProvisional();

            tileElement = FootpathGetTileElementToRemove();
            if (tileElement != nullptr)
            {
                FootpathRemoveTileElement(tileElement);
            }

            WindowFootpathSetEnabledAndPressedWidgets();
        }

        /**
         *
         *  rct2: 0x006A855C
         */
        void WindowFootpathSetEnabledAndPressedWidgets()
        {
            if (_footpathConstructionMode == PathConstructionMode::bridgeOrTunnel)
            {
                gMapSelectFlags.set(MapSelectFlag::enableConstruct, MapSelectFlag::green);

                int32_t direction = _footpathConstructDirection;
                MapSelection::clearSelectedTiles();
                MapSelection::addSelectedTile(
                    { _footpathConstructFromPosition.x + CoordsDirectionDelta[direction].x,
                      _footpathConstructFromPosition.y + CoordsDirectionDelta[direction].y });
            }

            uint64_t newPressedWidgets = pressedWidgets
                & ~((1LL << WIDX_DIRECTION_NW) | (1LL << WIDX_DIRECTION_NE) | (1LL << WIDX_DIRECTION_SW)
                    | (1LL << WIDX_DIRECTION_SE) | (1LL << WIDX_SLOPEDOWN) | (1LL << WIDX_LEVEL) | (1LL << WIDX_SLOPEUP));
            uint64_t newDisabledWidgets = 0;
            int32_t currentRotation = GetCurrentRotation();
            if (_footpathConstructionMode == PathConstructionMode::bridgeOrTunnel)
            {
                // Set pressed directional widget
                int32_t direction = (_footpathConstructDirection + currentRotation) & 3;
                newPressedWidgets |= (1LL << (WIDX_DIRECTION_NW + direction));

                // Set pressed slope widget
                switch (_footpathConstructSlope)
                {
                    case SlopePitch::flat:
                        newPressedWidgets |= (1uLL << WIDX_LEVEL);
                        break;
                    case SlopePitch::upwards:
                        newPressedWidgets |= (1uLL << WIDX_SLOPEUP);
                        break;
                    case SlopePitch::downwards:
                        newPressedWidgets |= (1uLL << WIDX_SLOPEDOWN);
                        break;
                }

                // Enable / disable directional widgets
                direction = _footpathConstructValidDirections;
                if (direction != kInvalidDirection)
                {
                    newDisabledWidgets |= (1uLL << WIDX_DIRECTION_NW) | (1uLL << WIDX_DIRECTION_NE)
                        | (1uLL << WIDX_DIRECTION_SW) | (1uLL << WIDX_DIRECTION_SE);

                    direction = (direction + currentRotation) & 3;
                    newDisabledWidgets &= ~(1 << (WIDX_DIRECTION_NW + direction));
                }
            }
            else
            {
                // Disable all bridge mode widgets
                newDisabledWidgets |= (1uLL << WIDX_DIRECTION_GROUP) | (1uLL << WIDX_DIRECTION_NW) | (1uLL << WIDX_DIRECTION_NE)
                    | (1uLL << WIDX_DIRECTION_SW) | (1uLL << WIDX_DIRECTION_SE) | (1uLL << WIDX_SLOPE_GROUP)
                    | (1uLL << WIDX_SLOPEDOWN) | (1uLL << WIDX_LEVEL) | (1uLL << WIDX_SLOPEUP) | (1uLL << WIDX_CONSTRUCT)
                    | (1uLL << WIDX_REMOVE);
            }

            pressedWidgets = newPressedWidgets;
            disabledWidgets = newDisabledWidgets;
            invalidate();
        }

        /**
         *
         *  rct2: 0x006A7B20
         */
        void FootpathGetNextPathInfo(ObjectEntryIndex* type, CoordsXYZ& footpathLoc, FootpathSlope* slope)
        {
            auto direction = _footpathConstructDirection;
            footpathLoc.x = _footpathConstructFromPosition.x + CoordsDirectionDelta[direction].x;
            footpathLoc.y = _footpathConstructFromPosition.y + CoordsDirectionDelta[direction].y;
            footpathLoc.z = _footpathConstructFromPosition.z;
            if (type != nullptr)
            {
                *type = gFootpathSelection.getSelectedSurface();
            }
            *slope = {};
            if (_footpathConstructSlope != SlopePitch::flat)
            {
                *slope = { FootpathSlopeType::sloped, _footpathConstructDirection };
                if (_footpathConstructSlope == SlopePitch::downwards)
                {
                    footpathLoc.z -= kPathHeightStep;
                    slope->direction = DirectionReverse(slope->direction);
                }
            }
        }

        PathConstructFlags FootpathCreateConstructFlags(ObjectEntryIndex& type)
        {
            PathConstructFlags pathConstructFlags = 0;
            if (gFootpathSelection.isQueueSelected)
                pathConstructFlags |= PathConstructFlag::IsQueue;
            if (gFootpathSelection.legacyPath != kObjectEntryIndexNull)
            {
                pathConstructFlags |= PathConstructFlag::IsLegacyPathObject;
                type = gFootpathSelection.legacyPath;
            }
            return pathConstructFlags;
        }

#pragma region Keyboard Shortcuts Events
    public:
        void KeyboardShortcutTurnLeft()
        {
            if (isWidgetDisabled(WIDX_DIRECTION_NW) || isWidgetDisabled(WIDX_DIRECTION_NE)
                || isWidgetDisabled(WIDX_DIRECTION_SW) || isWidgetDisabled(WIDX_DIRECTION_SE)
                || _footpathConstructionMode != PathConstructionMode::bridgeOrTunnel)
            {
                return;
            }
            int32_t currentRotation = GetCurrentRotation();
            int32_t turnedRotation = _footpathConstructDirection - currentRotation + (currentRotation % 2 == 1 ? 1 : -1);
            WindowFootpathMousedownDirection(turnedRotation);
        }

        void KeyboardShortcutTurnRight()
        {
            if (isWidgetDisabled(WIDX_DIRECTION_NW) || isWidgetDisabled(WIDX_DIRECTION_NE)
                || isWidgetDisabled(WIDX_DIRECTION_SW) || isWidgetDisabled(WIDX_DIRECTION_SE)
                || _footpathConstructionMode != PathConstructionMode::bridgeOrTunnel)
            {
                return;
            }
            int32_t currentRotation = GetCurrentRotation();
            int32_t turnedRotation = _footpathConstructDirection - currentRotation + (currentRotation % 2 == 1 ? -1 : 1);
            WindowFootpathMousedownDirection(turnedRotation);
        }

        void KeyboardShortcutShortcutSlopeDown()
        {
            if (isWidgetDisabled(WIDX_SLOPEDOWN) || isWidgetDisabled(WIDX_LEVEL) || isWidgetDisabled(WIDX_SLOPEUP)
                || widgets[WIDX_LEVEL].type == WidgetType::empty)
            {
                return;
            }

            switch (_footpathConstructSlope)
            {
                case SlopePitch::flat:
                    onMouseDown(WIDX_SLOPEDOWN);
                    break;
                case SlopePitch::upwards:
                    onMouseDown(WIDX_LEVEL);
                    break;
                default:
                case SlopePitch::downwards:
                    return;
            }
        }

        void KeyboardShortcutSlopeUp()
        {
            if (isWidgetDisabled(WIDX_SLOPEDOWN) || isWidgetDisabled(WIDX_LEVEL) || isWidgetDisabled(WIDX_SLOPEUP)
                || widgets[WIDX_LEVEL].type == WidgetType::empty)
            {
                return;
            }

            switch (_footpathConstructSlope)
            {
                case SlopePitch::downwards:
                    onMouseDown(WIDX_LEVEL);
                    break;
                case SlopePitch::flat:
                    onMouseDown(WIDX_SLOPEUP);
                    break;
                default:
                case SlopePitch::upwards:
                    return;
            }
        }

        void KeyboardShortcutSlopeLevel()
        {
            if (isWidgetDisabled(WIDX_SLOPEDOWN) || isWidgetDisabled(WIDX_LEVEL) || isWidgetDisabled(WIDX_SLOPEUP)
                || widgets[WIDX_LEVEL].type == WidgetType::empty || _footpathConstructSlope == SlopePitch::flat)
            {
                return;
            }

            onMouseDown(WIDX_LEVEL);
        }

        void KeyboardShortcutDemolishCurrent()
        {
            if (isWidgetDisabled(WIDX_REMOVE) || widgets[WIDX_REMOVE].type == WidgetType::empty
                || (!getGameState().cheats.buildInPauseMode && GameIsPaused()))
            {
                return;
            }

            WindowFootpathRemove();
        }

        void KeyboardShortcutBuildCurrent()
        {
            if (isWidgetDisabled(WIDX_CONSTRUCT) || widgets[WIDX_CONSTRUCT].type == WidgetType::empty)
            {
                return;
            }

            onMouseDown(WIDX_CONSTRUCT);
        }

#pragma endregion
    };

    /**
     *
     *  rct2: 0x006A7C43
     */
    WindowBase* FootpathOpen()
    {
        if (!WindowFootpathSelectDefault())
        {
            // No path objects to select from, don't open window
            return nullptr;
        }

        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<FootpathWindow>(WindowClass::footpath, kWindowSize, {});
    }

    void WindowFootpathResetSelectedPath()
    {
        gFootpathSelection = {};
    }

    void WindowFootpathKeyboardShortcutTurnLeft()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::footpath);
        if (w != nullptr)
        {
            auto* footpathWindow = static_cast<FootpathWindow*>(w);
            if (footpathWindow != nullptr)
            {
                footpathWindow->KeyboardShortcutTurnLeft();
            }
        }
    }

    void WindowFootpathKeyboardShortcutTurnRight()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::footpath);
        if (w != nullptr)
        {
            auto* footpathWindow = static_cast<FootpathWindow*>(w);
            if (footpathWindow != nullptr)
            {
                footpathWindow->KeyboardShortcutTurnRight();
            }
        }
    }

    void WindowFootpathKeyboardShortcutSlopeDown()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::footpath);
        if (w != nullptr)
        {
            auto* footpathWindow = static_cast<FootpathWindow*>(w);
            if (footpathWindow != nullptr)
            {
                footpathWindow->KeyboardShortcutShortcutSlopeDown();
            }
        }
    }

    void WindowFootpathKeyboardShortcutSlopeUp()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::footpath);
        if (w != nullptr)
        {
            auto* footpathWindow = static_cast<FootpathWindow*>(w);
            if (footpathWindow != nullptr)
            {
                footpathWindow->KeyboardShortcutSlopeUp();
            }
        }
    }

    void WindowFootpathKeyboardShortcutDemolishCurrent()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::footpath);
        if (w != nullptr)
        {
            auto* footpathWindow = static_cast<FootpathWindow*>(w);
            if (footpathWindow != nullptr)
            {
                footpathWindow->KeyboardShortcutDemolishCurrent();
            }
        }
    }

    void WindowFootpathKeyboardShortcutBuildCurrent()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::footpath);
        if (w != nullptr)
        {
            auto* footpathWindow = static_cast<FootpathWindow*>(w);
            if (footpathWindow != nullptr)
            {
                footpathWindow->KeyboardShortcutBuildCurrent();
            }
        }
    }

    /**
     *
     *  rct2: 0x0066CCE7
     */
    void ToggleFootpathWindow()
    {
        auto* windowMgr = GetWindowManager();
        if (windowMgr->FindByClass(WindowClass::footpath) == nullptr)
        {
            ContextOpenWindow(WindowClass::footpath);
        }
        else
        {
            ToolCancel();
            windowMgr->CloseByClass(WindowClass::footpath);
        }
    }

    /**
     *
     *  rct2: 0x006A76FF
     */
    static money64 FootpathProvisionalSet(
        ObjectEntryIndex type, ObjectEntryIndex railingsType, const CoordsXY& footpathLocA, const CoordsXY& footpathLocB,
        int32_t startZ, const std::span<const ProvisionalTile> tiles, PathConstructFlags constructFlags)
    {
        FootpathRemoveProvisional();

        GameActions::Result res;
        std::vector<ProvisionalTile> succesfulTiles{};
        money64 cost = 0;
        for (const auto& tile : tiles)
        {
            auto footpathPlaceAction = GameActions::FootpathPlaceAction(
                tile.position, tile.slope, type, railingsType, kInvalidDirection, constructFlags);
            footpathPlaceAction.SetFlags({ CommandFlag::ghost, CommandFlag::allowDuringPaused });
            res = GameActions::Execute(&footpathPlaceAction, getGameState());
            // The latter status will be returned if there is already path at the tile in question, to prevent a ghost
            // from glitching through the existing path.
            if (res.error == GameActions::Status::ok || res.error == GameActions::Status::itemAlreadyPlaced)
            {
                succesfulTiles.emplace_back(tile);
                cost += res.cost;
            }
        }

        bool anySuccesful = succesfulTiles.size() > 0;
        if (anySuccesful)
        {
            _provisionalFootpath.surfaceIndex = type;
            _provisionalFootpath.railingsIndex = railingsType;
            _provisionalFootpath.positionA = footpathLocA;
            _provisionalFootpath.positionB = footpathLocB;
            _provisionalFootpath.startZ = startZ;
            _provisionalFootpath.tiles = succesfulTiles;
            _provisionalFootpath.constructFlags = constructFlags;
            _provisionalFootpath.flags.set(ProvisionalPathFlag::placed);

            if (gFootpathGroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                ViewportSetVisibility(ViewportVisibility::undergroundViewOn);
            }
            else
            {
                ViewportSetVisibility(ViewportVisibility::undergroundViewOff);
            }
        }

        if (!isToolActive(WindowClass::scenery))
        {
            if (succesfulTiles.size() == 0)
            {
                // If we can't build this, don't show a virtual floor.
                VirtualFloorSetHeight(0);
            }
            else if (
                succesfulTiles[0].slope.type == FootpathSlopeType::flat
                || succesfulTiles[0].position.z < _footpathConstructFromPosition.z)
            {
                // Going either straight on, or down.
                VirtualFloorSetHeight(succesfulTiles[0].position.z);
            }
            else
            {
                // Going up in the world!
                VirtualFloorSetHeight(succesfulTiles[0].position.z + kLandHeightStep);
            }
        }

        return anySuccesful ? cost : kMoney64Undefined;
    }

    /**
     *
     *  rct2: 0x006A77FF
     */
    void FootpathRemoveProvisional()
    {
        if (!_provisionalFootpath.flags.has(ProvisionalPathFlag::placed))
            return;

        _provisionalFootpath.flags.unset(ProvisionalPathFlag::placed);

        for (const auto& tile : _provisionalFootpath.tiles)
        {
            auto action = GameActions::FootpathRemoveAction(tile.position);
            action.SetFlags({ CommandFlag::allowDuringPaused, CommandFlag::noSpend, CommandFlag::ghost });
            GameActions::Execute(&action, getGameState());
        }
    }

    /**
     *
     *  rct2: 0x006A7831
     */
    void FootpathUpdateProvisional()
    {
        if (_provisionalFootpath.flags.has(ProvisionalPathFlag::showArrow))
        {
            _provisionalFootpath.flags.unset(ProvisionalPathFlag::showArrow);

            gMapSelectFlags.unset(MapSelectFlag::enableArrow);
        }
        FootpathRemoveProvisional();
    }

    void FootpathRemoveProvisionalTemporarily()
    {
        if (_provisionalFootpath.flags.has(ProvisionalPathFlag::placed))
        {
            FootpathRemoveProvisional();
            _provisionalFootpath.flags.set(ProvisionalPathFlag::placed);
        }
    }

    void FootpathRestoreProvisional()
    {
        if (_provisionalFootpath.flags.has(ProvisionalPathFlag::placed))
        {
            _provisionalFootpath.flags.unset(ProvisionalPathFlag::placed);
            FootpathProvisionalSet(
                _provisionalFootpath.surfaceIndex, _provisionalFootpath.railingsIndex, _provisionalFootpath.positionA,
                _provisionalFootpath.positionB, _provisionalFootpath.startZ, _provisionalFootpath.tiles,
                _provisionalFootpath.constructFlags);
        }
    }

    void FootpathRecheckProvisional()
    {
        _provisionalFootpath.flags.set(ProvisionalPathFlag::forceRecheck);
    }

    static ObjectEntryIndex FootpathGetDefaultSurface(bool queue)
    {
        bool showEditorPaths = (gLegacyScene == LegacyScene::scenarioEditor || getGameState().cheats.sandboxMode);
        for (ObjectEntryIndex i = 0; i < kMaxFootpathSurfaceObjects; i++)
        {
            auto pathEntry = GetPathSurfaceEntry(i);
            if (pathEntry != nullptr)
            {
                if (!showEditorPaths && (pathEntry->Flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR))
                {
                    continue;
                }
                if (queue == ((pathEntry->Flags & FOOTPATH_ENTRY_FLAG_IS_QUEUE) != 0))
                {
                    return i;
                }
            }
        }
        return kObjectEntryIndexNull;
    }

    static bool FootpathIsSurfaceEntryOkay(ObjectEntryIndex index, bool queue)
    {
        auto pathEntry = GetPathSurfaceEntry(index);
        if (pathEntry != nullptr)
        {
            bool showEditorPaths = (gLegacyScene == LegacyScene::scenarioEditor || getGameState().cheats.sandboxMode);
            if (!showEditorPaths && (pathEntry->Flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR))
            {
                return false;
            }
            if (queue == ((pathEntry->Flags & FOOTPATH_ENTRY_FLAG_IS_QUEUE) != 0))
            {
                return true;
            }
        }
        return false;
    }

    static ObjectEntryIndex FootpathGetDefaultRailings()
    {
        for (ObjectEntryIndex i = 0; i < kMaxFootpathRailingsObjects; i++)
        {
            const auto* railingEntry = GetPathRailingsEntry(i);
            if (railingEntry != nullptr)
            {
                return i;
            }
        }
        return kObjectEntryIndexNull;
    }

    static bool FootpathIsLegacyPathEntryOkay(ObjectEntryIndex index)
    {
        bool showEditorPaths = (gLegacyScene == LegacyScene::scenarioEditor || getGameState().cheats.sandboxMode);
        auto& objManager = GetContext()->GetObjectManager();
        auto footpathObj = objManager.GetLoadedObject<FootpathObject>(index);
        if (footpathObj != nullptr)
        {
            auto pathEntry = reinterpret_cast<FootpathEntry*>(footpathObj->GetLegacyData());
            return showEditorPaths || !(pathEntry->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR);
        }
        return false;
    }

    static ObjectEntryIndex FootpathGetDefaultLegacyPath()
    {
        for (ObjectEntryIndex i = 0; i < kMaxPathObjects; i++)
        {
            if (FootpathIsLegacyPathEntryOkay(i))
            {
                return i;
            }
        }
        return kObjectEntryIndexNull;
    }

    bool WindowFootpathSelectDefault()
    {
        // Select default footpath
        auto surfaceIndex = FootpathGetDefaultSurface(false);
        if (FootpathIsSurfaceEntryOkay(gFootpathSelection.normalSurface, false))
        {
            surfaceIndex = gFootpathSelection.normalSurface;
        }

        // Select default queue
        auto queueIndex = FootpathGetDefaultSurface(true);
        if (FootpathIsSurfaceEntryOkay(gFootpathSelection.queueSurface, true))
        {
            queueIndex = gFootpathSelection.queueSurface;
        }

        // Select default railing
        auto railingIndex = FootpathGetDefaultRailings();
        const auto* railingEntry = GetPathRailingsEntry(gFootpathSelection.railings);
        if (railingEntry != nullptr)
        {
            railingIndex = gFootpathSelection.railings;
        }

        // Select default legacy path
        auto legacyPathIndex = FootpathGetDefaultLegacyPath();
        if (gFootpathSelection.legacyPath != kObjectEntryIndexNull)
        {
            if (FootpathIsLegacyPathEntryOkay(gFootpathSelection.legacyPath))
            {
                // Keep legacy path selected
                legacyPathIndex = gFootpathSelection.legacyPath;
            }
            else
            {
                // Reset legacy path, we default to a surface (if there are any)
                gFootpathSelection.legacyPath = kObjectEntryIndexNull;
            }
        }

        if (surfaceIndex == kObjectEntryIndexNull)
        {
            if (legacyPathIndex == kObjectEntryIndexNull)
            {
                // No surfaces or legacy paths available
                return false;
            }

            // No surfaces available, so default to legacy path
            gFootpathSelection.legacyPath = legacyPathIndex;
        }

        gFootpathSelection.normalSurface = surfaceIndex;
        gFootpathSelection.queueSurface = queueIndex;
        gFootpathSelection.railings = railingIndex;
        return true;
    }
} // namespace OpenRCT2::Ui::Windows
