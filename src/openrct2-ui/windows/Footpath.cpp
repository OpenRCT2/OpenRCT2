/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/FootpathPlaceAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/FootpathObject.h>
#include <openrct2/object/FootpathRailingsObject.h>
#include <openrct2/object/FootpathSurfaceObject.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/platform/platform.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Surface.h>

using namespace OpenRCT2;

static constexpr const rct_string_id WINDOW_TITLE = STR_FOOTPATHS;
static constexpr const int32_t WH = 421;
static constexpr const int32_t WW = 106;
static constexpr const uint16_t ARROW_PULSE_DURATION = 200;

static uint8_t _footpathConstructDirection;
static uint8_t _footpathConstructValidDirections;
static uint8_t _footpathConstructionMode;

static std::vector<std::pair<ObjectType, ObjectEntryIndex>> _dropdownEntries;

// clang-format off
enum
{
    PATH_CONSTRUCTION_MODE_LAND,
    PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL,
    PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL
};

enum WindowFootpathWidgetIdx
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
};

static rct_widget _windowFootpathWidgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),

    // Type group
    MakeWidget({ 3,  17}, {100, 95}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_TYPE                                                                          ),
    MakeWidget({ 6,  30}, { 47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_FOOTPATH_TIP                               ),
    MakeWidget({53,  30}, { 47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_QUEUE_LINE_PATH_TIP                        ),
    MakeWidget({29,  69}, { 47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_FOOTPATH_TIP                               ),

    // Direction group
    MakeWidget({ 3, 115}, {100, 77}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_DIRECTION                                                                     ),
    MakeWidget({53, 127}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_NE,     STR_DIRECTION_TIP                              ),
    MakeWidget({53, 156}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_SE,     STR_DIRECTION_TIP                              ),
    MakeWidget({ 8, 156}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_SW,     STR_DIRECTION_TIP                              ),
    MakeWidget({ 8, 127}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_NW,     STR_DIRECTION_TIP                              ),

    // Slope group
    MakeWidget({ 3, 195}, {100, 41}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_SLOPE                                                                         ),
    MakeWidget({17, 207}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_DOWN,  STR_SLOPE_DOWN_TIP                             ),
    MakeWidget({41, 207}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_LEVEL, STR_LEVEL_TIP                                  ),
    MakeWidget({65, 207}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_UP,    STR_SLOPE_UP_TIP                               ),
    MakeWidget({ 8, 242}, { 90, 90}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_CONSTRUCT_THE_SELECTED_FOOTPATH_SECTION_TIP),
    MakeWidget({30, 335}, { 46, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_DEMOLISH_CURRENT_SECTION,      STR_REMOVE_PREVIOUS_FOOTPATH_SECTION_TIP       ),

    // Mode group
    MakeWidget({ 3, 361}, {100, 54}, WindowWidgetType::Groupbox, WindowColour::Primary                                                                                      ),
    MakeWidget({13, 372}, { 36, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_FOOTPATH_LAND,    STR_CONSTRUCT_FOOTPATH_ON_LAND_TIP             ),
    MakeWidget({57, 372}, { 36, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_FOOTPATH_BRIDGE,  STR_CONSTRUCT_BRIDGE_OR_TUNNEL_FOOTPATH_TIP    ),
    WIDGETS_END,
};

// clang-format on

static money32 _windowFootpathCost;
static uint32_t _footpathConstructionNextArrowPulse = 0;
static uint8_t _lastUpdatedCameraRotation = UINT8_MAX;
static bool _footpathErrorOccured;

static constexpr const uint8_t DefaultPathSlope[] = {
    0,
    SLOPE_IS_IRREGULAR_FLAG,
    SLOPE_IS_IRREGULAR_FLAG,
    FOOTPATH_PROPERTIES_FLAG_IS_SLOPED | 2,
    SLOPE_IS_IRREGULAR_FLAG,
    SLOPE_IS_IRREGULAR_FLAG,
    FOOTPATH_PROPERTIES_FLAG_IS_SLOPED | 3,
    RAISE_FOOTPATH_FLAG,
    SLOPE_IS_IRREGULAR_FLAG,
    FOOTPATH_PROPERTIES_FLAG_IS_SLOPED | 1,
    SLOPE_IS_IRREGULAR_FLAG,
    RAISE_FOOTPATH_FLAG,
    FOOTPATH_PROPERTIES_FLAG_IS_SLOPED | 0,
    RAISE_FOOTPATH_FLAG,
    RAISE_FOOTPATH_FLAG,
    SLOPE_IS_IRREGULAR_FLAG,
};

static constexpr const uint8_t ConstructionPreviewImages[][4] = {
    { 5, 10, 5, 10 },   // Flat
    { 16, 17, 18, 19 }, // Upwards
    { 18, 19, 16, 17 }, // Downwards
};

class FootpathWindow final : public Window
{
public:
    void OnOpen() override
    {
        if (!SelectDefault())
        {
            // No path objects to select from, don't open window
            return;
        }

        widgets = _windowFootpathWidgets;
        enabled_widgets = (1ULL << WIDX_CLOSE) | (1ULL << WIDX_FOOTPATH_TYPE) | (1ULL << WIDX_QUEUELINE_TYPE)
            | (1ULL << WIDX_RAILINGS_TYPE) | (1ULL << WIDX_DIRECTION_NW) | (1ULL << WIDX_DIRECTION_NE)
            | (1ULL << WIDX_DIRECTION_SW) | (1ULL << WIDX_DIRECTION_SE) | (1ULL << WIDX_SLOPEDOWN) | (1ULL << WIDX_LEVEL)
            | (1ULL << WIDX_SLOPEUP) | (1ULL << WIDX_CONSTRUCT) | (1ULL << WIDX_REMOVE) | (1ULL << WIDX_CONSTRUCT_ON_LAND)
            | (1ULL << WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL);
        WindowInitScrollWidgets(this);
        window_push_others_right(this);
        show_gridlines();

        tool_cancel();
        _footpathConstructionMode = PATH_CONSTRUCTION_MODE_LAND;
        tool_set(this, WIDX_CONSTRUCT_ON_LAND, Tool::PathDown);
        input_set_flag(INPUT_FLAG_6, true);
        _footpathErrorOccured = false;
        SetEnabledAndPressedWidgets();
    }

    void OnClose() override
    {
        footpath_provisional_update();
        viewport_set_visibility(0);
        map_invalidate_map_selection_tiles();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        window_invalidate_by_class(WC_TOP_TOOLBAR);
        hide_gridlines();
    }

    void OnUpdate() override
    {
        widget_invalidate(this, WIDX_CONSTRUCT);
        UpdateProvisionalPathForBridgeMode();

        // #2502: The camera might have changed rotation, so we need to update which directional buttons are pressed
        uint8_t currentRotation = get_current_rotation();
        if (_lastUpdatedCameraRotation != currentRotation)
        {
            _lastUpdatedCameraRotation = currentRotation;
            SetEnabledAndPressedWidgets();
        }

        // Check tool
        if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_LAND)
        {
            if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
            {
                window_close(this);
            }
            else if (gCurrentToolWidget.window_classification != WC_FOOTPATH)
            {
                window_close(this);
            }
            else if (gCurrentToolWidget.widget_index != WIDX_CONSTRUCT_ON_LAND)
            {
                window_close(this);
            }
        }
        else if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL)
        {
            if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
            {
                window_close(this);
            }
            else if (gCurrentToolWidget.window_classification != WC_FOOTPATH)
            {
                window_close(this);
            }
            else if (gCurrentToolWidget.widget_index != WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
            {
                window_close(this);
            }
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        ScreenCoordsXY screenCoords;
        DrawWidgets(dpi);
        DrawDropdownButtons(dpi);

        if (!(disabled_widgets & (1ULL << WIDX_CONSTRUCT)))
        {
            // Get construction image
            uint8_t direction = (_footpathConstructDirection + get_current_rotation()) % 4;
            uint8_t slope = 0;
            if (gFootpathConstructSlope == 2)
            {
                slope = TILE_ELEMENT_SLOPE_N_CORNER_UP;
            }
            else if (gFootpathConstructSlope == 6)
            {
                slope = TILE_ELEMENT_SLOPE_E_CORNER_UP;
            }

            std::optional<uint32_t> baseImage;
            if (gFootpathSelection.LegacyPath == OBJECT_ENTRY_INDEX_NULL)
            {
                auto selectedPath = gFootpathSelection.GetSelectedSurface();
                const auto* pathType = GetPathSurfaceEntry(selectedPath);
                if (pathType != nullptr)
                {
                    baseImage = pathType->BaseImageId;
                }
            }
            else
            {
                auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
                auto* pathObj = static_cast<FootpathObject*>(
                    objManager.GetLoadedObject(ObjectType::Paths, gFootpathSelection.LegacyPath));
                if (pathObj != nullptr)
                {
                    auto pathEntry = reinterpret_cast<const rct_footpath_entry*>(pathObj->GetLegacyData());
                    if (gFootpathSelection.IsQueueSelected)
                        baseImage = pathEntry->GetQueueImage();
                    else
                        baseImage = pathEntry->image;
                }
            }

            if (baseImage)
            {
                auto image = *baseImage + ConstructionPreviewImages[slope][direction];

                // Draw construction image
                screenCoords = windowPos
                    + ScreenCoordsXY{ _windowFootpathWidgets[WIDX_CONSTRUCT].midX(),
                                      _windowFootpathWidgets[WIDX_CONSTRUCT].bottom - 60 };
                gfx_draw_sprite(&dpi, ImageId(image), screenCoords);
            }

            // Draw build this... label
            screenCoords = windowPos
                + ScreenCoordsXY{ _windowFootpathWidgets[WIDX_CONSTRUCT].midX(),
                                  _windowFootpathWidgets[WIDX_CONSTRUCT].bottom - 23 };
            DrawTextBasic(&dpi, screenCoords, STR_BUILD_THIS, {}, { TextAlignment::CENTRE });
        }

        // Draw cost
        screenCoords = windowPos
            + ScreenCoordsXY{ _windowFootpathWidgets[WIDX_CONSTRUCT].midX(),
                              _windowFootpathWidgets[WIDX_CONSTRUCT].bottom - 12 };
        if (_windowFootpathCost != MONEY32_UNDEFINED)
        {
            if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
            {
                auto ft = Formatter();
                ft.Add<money64>(_windowFootpathCost);
                DrawTextBasic(&dpi, screenCoords, STR_COST_LABEL, ft, { TextAlignment::CENTRE });
            }
        }
    }

    void OnMouseDown(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_FOOTPATH_TYPE:
                ShowFootpathTypesDialog(widgetIndex, false);
                break;
            case WIDX_QUEUELINE_TYPE:
                ShowFootpathTypesDialog(widgetIndex, true);
                break;
            case WIDX_RAILINGS_TYPE:
                ShowRailingsTypesDialog(widgetIndex);
                break;
            case WIDX_DIRECTION_NW:
                MousedownDirection(0);
                break;
            case WIDX_DIRECTION_NE:
                MousedownDirection(1);
                break;
            case WIDX_DIRECTION_SW:
                MousedownDirection(2);
                break;
            case WIDX_DIRECTION_SE:
                MousedownDirection(3);
                break;
            case WIDX_SLOPEDOWN:
                MousedownSlope(6);
                break;
            case WIDX_LEVEL:
                MousedownSlope(0);
                break;
            case WIDX_SLOPEUP:
                MousedownSlope(2);
                break;
        }
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_CONSTRUCT:
                Construct();
                break;
            case WIDX_REMOVE:
                Remove();
                break;
            case WIDX_CONSTRUCT_ON_LAND:
                if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_LAND)
                {
                    break;
                }

                _windowFootpathCost = MONEY32_UNDEFINED;
                tool_cancel();
                footpath_provisional_update();
                map_invalidate_map_selection_tiles();
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
                _footpathConstructionMode = PATH_CONSTRUCTION_MODE_LAND;
                tool_set(this, WIDX_CONSTRUCT_ON_LAND, Tool::PathDown);
                input_set_flag(INPUT_FLAG_6, true);
                _footpathErrorOccured = false;
                SetEnabledAndPressedWidgets();
                break;
            case WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL:
                if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL)
                {
                    break;
                }

                _windowFootpathCost = MONEY32_UNDEFINED;
                tool_cancel();
                footpath_provisional_update();
                map_invalidate_map_selection_tiles();
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
                _footpathConstructionMode = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL;
                tool_set(this, WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL, Tool::Crosshair);
                input_set_flag(INPUT_FLAG_6, true);
                _footpathErrorOccured = false;
                SetEnabledAndPressedWidgets();
                break;
        }
    }

    void OnDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex) override
    {
        if (dropdownIndex < 0 || static_cast<size_t>(dropdownIndex) >= _dropdownEntries.size())
            return;

        auto entryIndex = _dropdownEntries[dropdownIndex];
        if (widgetIndex == WIDX_FOOTPATH_TYPE)
        {
            gFootpathSelection.IsQueueSelected = false;
            if (entryIndex.first == ObjectType::Paths)
            {
                gFootpathSelection.LegacyPath = entryIndex.second;
            }
            else
            {
                gFootpathSelection.LegacyPath = OBJECT_ENTRY_INDEX_NULL;
                gFootpathSelection.NormalSurface = entryIndex.second;
            }
        }
        else if (widgetIndex == WIDX_QUEUELINE_TYPE)
        {
            gFootpathSelection.IsQueueSelected = true;
            if (entryIndex.first == ObjectType::Paths)
            {
                gFootpathSelection.LegacyPath = entryIndex.second;
            }
            else
            {
                gFootpathSelection.LegacyPath = OBJECT_ENTRY_INDEX_NULL;
                gFootpathSelection.QueueSurface = entryIndex.second;
            }
        }
        else if (widgetIndex == WIDX_RAILINGS_TYPE)
        {
            gFootpathSelection.Railings = entryIndex.second;
        }
        else
        {
            return;
        }

        footpath_provisional_update();
        _windowFootpathCost = MONEY32_UNDEFINED;
        Invalidate();
    }

    void OnToolUpdate(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
        {
            SetProvisionalPathAtPoint(screenCoords);
        }
        else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
        {
            SetSelectionStartBridgeAtPoint(screenCoords);
        }
    }

    void OnToolDown(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
        {
            PlacePathAtPoint(screenCoords);
        }
        else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
        {
            StartBridgeAtPoint(screenCoords);
        }
    }

    void OnToolDrag(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
        {
            PlacePathAtPoint(screenCoords);
        }
    }

    void OnToolUp(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
        {
            _footpathErrorOccured = false;
        }
    }

    void KeyboardShortcutTurnLeft()
    {
        if (WidgetIsDisabled(this, WIDX_DIRECTION_NW) || WidgetIsDisabled(this, WIDX_DIRECTION_NE)
            || WidgetIsDisabled(this, WIDX_DIRECTION_SW) || WidgetIsDisabled(this, WIDX_DIRECTION_SE)
            || _footpathConstructionMode != 2)
        {
            return;
        }
        int32_t currentRotation = get_current_rotation();
        int32_t turnedRotation = _footpathConstructDirection - currentRotation + (currentRotation % 2 == 1 ? 1 : -1);
        MousedownDirection(turnedRotation);
    }

    void KeyboardShortcutTurnRight()
    {
        if (WidgetIsDisabled(this, WIDX_DIRECTION_NW) || WidgetIsDisabled(this, WIDX_DIRECTION_NE)
            || WidgetIsDisabled(this, WIDX_DIRECTION_SW) || WidgetIsDisabled(this, WIDX_DIRECTION_SE)
            || _footpathConstructionMode != 2)
        {
            return;
        }
        int32_t currentRotation = get_current_rotation();
        int32_t turnedRotation = _footpathConstructDirection - currentRotation + (currentRotation % 2 == 1 ? -1 : 1);
        MousedownDirection(turnedRotation);
    }

    void KeyboardShortcutSlopeDown()
    {
        if (WidgetIsDisabled(this, WIDX_SLOPEDOWN) || WidgetIsDisabled(this, WIDX_LEVEL)
            || WidgetIsDisabled(this, WIDX_SLOPEUP) || this->widgets[WIDX_LEVEL].type == WindowWidgetType::Empty)
        {
            return;
        }

        switch (gFootpathConstructSlope)
        {
            case 0:
                window_event_mouse_down_call(this, WIDX_SLOPEDOWN);
                break;
            case 2:
                window_event_mouse_down_call(this, WIDX_LEVEL);
                break;
            default:
            case 6:
                return;
        }
    }

    void KeyboardShortcutSlopeUp()
    {
        if (WidgetIsDisabled(this, WIDX_SLOPEDOWN) || WidgetIsDisabled(this, WIDX_LEVEL)
            || WidgetIsDisabled(this, WIDX_SLOPEUP) || this->widgets[WIDX_LEVEL].type == WindowWidgetType::Empty)
        {
            return;
        }

        switch (gFootpathConstructSlope)
        {
            case 6:
                window_event_mouse_down_call(this, WIDX_LEVEL);
                break;
            case 0:
                window_event_mouse_down_call(this, WIDX_SLOPEUP);
                break;
            default:
            case 2:
                return;
        }
    }

    void KeyboardShortcutDemolishCurrent()
    {
        if (WidgetIsDisabled(this, WIDX_REMOVE) || this->widgets[WIDX_REMOVE].type == WindowWidgetType::Empty
            || (!gCheatsBuildInPauseMode && game_is_paused()))
        {
            return;
        }

        Remove();
    }

    void KeyboardShortcutBuildCurrent()
    {
        if (WidgetIsDisabled(this, WIDX_CONSTRUCT)
            || this->widgets[WIDX_CONSTRUCT].type == WindowWidgetType::Empty)
        {
            return;
        }

        window_event_mouse_up_call(this, WIDX_CONSTRUCT);
    }

    void ResetSelectedPath()
    {
        gFootpathSelection = {};
    }

private:
    void UpdateProvisionalPathForBridgeMode()
    {
        if (_footpathConstructionMode != PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
        {
            return;
        }

        // Recheck area for construction. Set by ride_construction window
        if (gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_2)
        {
            footpath_provisional_remove();
            gProvisionalFootpath.Flags &= ~PROVISIONAL_PATH_FLAG_2;
        }

        // Update provisional bridge mode path
        if (!(gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_1))
        {
            ObjectEntryIndex type;
            ObjectEntryIndex railings = gFootpathSelection.Railings;

            CoordsXYZ footpathLoc;
            int32_t slope;
            GetNextPathInfo(&type, footpathLoc, &slope);
            auto pathConstructFlags = CreateConstructFlags(type);

            _windowFootpathCost = footpath_provisional_set(type, railings, footpathLoc, slope, pathConstructFlags);
            widget_invalidate(this, WIDX_CONSTRUCT);
        }

        auto curTime = platform_get_ticks();

        // Update little directional arrow on provisional bridge mode path
        if (_footpathConstructionNextArrowPulse < curTime)
        {
            _footpathConstructionNextArrowPulse = curTime + ARROW_PULSE_DURATION;

            gProvisionalFootpath.Flags ^= PROVISIONAL_PATH_FLAG_SHOW_ARROW;
            CoordsXYZ footpathLoc;
            int32_t slope;
            GetNextPathInfo(nullptr, footpathLoc, &slope);
            gMapSelectArrowPosition = footpathLoc;
            gMapSelectArrowDirection = _footpathConstructDirection;
            if (gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_SHOW_ARROW)
            {
                gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
            }
            else
            {
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
            }
            map_invalidate_tile_full(footpathLoc);
        }
    }

    void Invalidate()
    {
        // Press / unpress footpath and queue type buttons
        pressed_widgets &= ~(1ULL << WIDX_FOOTPATH_TYPE);
        pressed_widgets &= ~(1ULL << WIDX_QUEUELINE_TYPE);
        pressed_widgets |= gFootpathSelection.IsQueueSelected ? (1ULL << WIDX_QUEUELINE_TYPE) : (1ULL << WIDX_FOOTPATH_TYPE);

        // Enable / disable construct button
        _windowFootpathWidgets[WIDX_CONSTRUCT].type = _footpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL
            ? WindowWidgetType::ImgBtn
            : WindowWidgetType::Empty;

        if (gFootpathSelection.LegacyPath == OBJECT_ENTRY_INDEX_NULL)
        {
            _windowFootpathWidgets[WIDX_RAILINGS_TYPE].type = WindowWidgetType::FlatBtn;
        }
        else
        {
            _windowFootpathWidgets[WIDX_RAILINGS_TYPE].type = WindowWidgetType::Empty;
        }
    }

    void DrawDropdownButton(rct_drawpixelinfo& dpi, rct_widgetindex widgetIndex, ImageIndex image)
    {
        const auto& widget = widgets[widgetIndex];
        gfx_draw_sprite(&dpi, ImageId(image), { windowPos.x + widget.left, windowPos.y + widget.top });
    }

    void DrawDropdownButtons(rct_drawpixelinfo& dpi)
    {
        if (gFootpathSelection.LegacyPath == OBJECT_ENTRY_INDEX_NULL)
        {
            // Set footpath and queue type button images
            auto pathImage = static_cast<uint32_t>(SPR_NONE);
            auto queueImage = static_cast<uint32_t>(SPR_NONE);
            auto pathEntry = GetPathSurfaceEntry(gFootpathSelection.NormalSurface);
            if (pathEntry != nullptr)
            {
                pathImage = pathEntry->PreviewImageId;
            }

            pathEntry = GetPathSurfaceEntry(gFootpathSelection.QueueSurface);
            if (pathEntry != nullptr)
            {
                queueImage = pathEntry->PreviewImageId;
            }

            DrawDropdownButton(dpi, WIDX_FOOTPATH_TYPE, pathImage);
            DrawDropdownButton(dpi, WIDX_QUEUELINE_TYPE, queueImage);

            // Set railing
            auto railingsImage = static_cast<uint32_t>(SPR_NONE);
            auto railingsEntry = GetPathRailingsEntry(gFootpathSelection.Railings);
            if (railingsEntry != nullptr)
            {
                railingsImage = railingsEntry->PreviewImageId;
            }
            DrawDropdownButton(dpi, WIDX_RAILINGS_TYPE, railingsImage);
        }
        else
        {
            auto& objManager = OpenRCT2::GetContext()->GetObjectManager();

            // Set footpath and queue type button images
            auto pathImage = static_cast<uint32_t>(SPR_NONE);
            auto queueImage = static_cast<uint32_t>(SPR_NONE);
            auto pathObj = static_cast<FootpathObject*>(
                objManager.GetLoadedObject(ObjectType::Paths, gFootpathSelection.LegacyPath));
            if (pathObj != nullptr)
            {
                auto pathEntry = reinterpret_cast<rct_footpath_entry*>(pathObj->GetLegacyData());
                pathImage = pathEntry->GetPreviewImage();
                queueImage = pathEntry->GetQueuePreviewImage();
            }

            DrawDropdownButton(dpi, WIDX_FOOTPATH_TYPE, pathImage);
            DrawDropdownButton(dpi, WIDX_QUEUELINE_TYPE, queueImage);
        }
    }

    void ShowFootpathTypesDialog(rct_widgetindex widgetIndex, bool showQueues)
    {
        auto& objManager = OpenRCT2::GetContext()->GetObjectManager();

        uint32_t numPathTypes = 0;
        // If the game is in sandbox mode, also show paths that are normally restricted to the scenario editor
        bool showEditorPaths = ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode);

        _dropdownEntries.clear();
        std::optional<size_t> defaultIndex;
        for (ObjectEntryIndex i = 0; i < MAX_FOOTPATH_SURFACE_OBJECTS; i++)
        {
            const auto* pathType = static_cast<FootpathSurfaceObject*>(
                objManager.GetLoadedObject(ObjectType::FootpathSurface, i));
            if (pathType == nullptr)
            {
                continue;
            }
            if ((pathType->Flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) && !showEditorPaths)
            {
                continue;
            }
            if (showQueues != ((pathType->Flags & FOOTPATH_ENTRY_FLAG_IS_QUEUE) != 0))
            {
                continue;
            }
            if (gFootpathSelection.LegacyPath == OBJECT_ENTRY_INDEX_NULL
                && i == (showQueues ? gFootpathSelection.QueueSurface : gFootpathSelection.NormalSurface))
            {
                defaultIndex = numPathTypes;
            }

            gDropdownItemsFormat[numPathTypes] = STR_NONE;
            Dropdown::SetImage(numPathTypes, ImageId(pathType->PreviewImageId));
            _dropdownEntries.push_back({ ObjectType::FootpathSurface, i });
            numPathTypes++;
        }

        for (ObjectEntryIndex i = 0; i < MAX_PATH_OBJECTS; i++)
        {
            auto* pathObj = static_cast<FootpathObject*>(objManager.GetLoadedObject(ObjectType::Paths, i));
            if (pathObj == nullptr)
            {
                continue;
            }

            auto pathEntry = reinterpret_cast<const rct_footpath_entry*>(pathObj->GetLegacyData());
            if ((pathEntry->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) && !showEditorPaths)
            {
                continue;
            }

            if (gFootpathSelection.LegacyPath != OBJECT_ENTRY_INDEX_NULL && gFootpathSelection.LegacyPath == i)
            {
                defaultIndex = numPathTypes;
            }

            gDropdownItemsFormat[numPathTypes] = STR_NONE;
            Dropdown::SetImage(
                numPathTypes, ImageId(showQueues ? pathEntry->GetQueuePreviewImage() : pathEntry->GetPreviewImage()));
            _dropdownEntries.push_back({ ObjectType::Paths, i });
            numPathTypes++;
        }
        rct_widget* widget = &this->widgets[widgetIndex];

        auto itemsPerRow = DropdownGetAppropriateImageDropdownItemsPerRow(numPathTypes);
        WindowDropdownShowImage(
            windowPos.x + widget->left, windowPos.y + widget->top, widget->height() + 1, colours[1], 0, numPathTypes, 47, 36,
            itemsPerRow);
        if (defaultIndex)
            gDropdownDefaultIndex = static_cast<int32_t>(*defaultIndex);
    }

    void ShowRailingsTypesDialog(rct_widgetindex widgetIndex)
    {
        uint32_t numRailingsTypes = 0;
        // If the game is in sandbox mode, also show paths that are normally restricted to the scenario editor

        _dropdownEntries.clear();
        std::optional<size_t> defaultIndex;
        for (int32_t i = 0; i < MAX_FOOTPATH_RAILINGS_OBJECTS; i++)
        {
            const auto* railingsEntry = GetPathRailingsEntry(i);
            if (railingsEntry == nullptr)
            {
                continue;
            }
            if (i == gFootpathSelection.Railings)
            {
                defaultIndex = numRailingsTypes;
            }

            gDropdownItemsFormat[numRailingsTypes] = STR_NONE;
            Dropdown::SetImage(numRailingsTypes, ImageId(railingsEntry->PreviewImageId));
            _dropdownEntries.push_back({ ObjectType::FootpathRailings, i });
            numRailingsTypes++;
        }
        rct_widget* widget = &this->widgets[widgetIndex];

        auto itemsPerRow = DropdownGetAppropriateImageDropdownItemsPerRow(numRailingsTypes);
        WindowDropdownShowImage(
            windowPos.x + widget->left, windowPos.y + widget->top, widget->height() + 1, colours[1], 0, numRailingsTypes, 47,
            36, itemsPerRow);
        if (defaultIndex)
            gDropdownDefaultIndex = static_cast<int32_t>(*defaultIndex);
    }

    void MousedownDirection(int32_t direction)
    {
        footpath_provisional_update();
        _footpathConstructDirection = (direction - get_current_rotation()) & 3;
        _windowFootpathCost = MONEY32_UNDEFINED;
        SetEnabledAndPressedWidgets();
    }

    void MousedownSlope(int32_t slope)
    {
        footpath_provisional_update();
        gFootpathConstructSlope = slope;
        _windowFootpathCost = MONEY32_UNDEFINED;
        SetEnabledAndPressedWidgets();
    }

    void SetProvisionalPathAtPoint(const ScreenCoordsXY& screenCoords)
    {
        map_invalidate_selection_rect();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

        auto info = get_map_coordinates_from_pos(
            screenCoords, EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Footpath));

        if (info.SpriteType == ViewportInteractionItem::None || info.Element == nullptr)
        {
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            footpath_provisional_update();
        }
        else
        {
            // Check for change
            if ((gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_1)
                && gProvisionalFootpath.Position == CoordsXYZ{ info.Loc, info.Element->GetBaseZ() })
            {
                return;
            }

            // Set map selection
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
            gMapSelectType = MAP_SELECT_TYPE_FULL;
            gMapSelectPositionA = info.Loc;
            gMapSelectPositionB = info.Loc;

            footpath_provisional_update();

            // Set provisional path
            int32_t slope = 0;
            switch (info.SpriteType)
            {
                case ViewportInteractionItem::Terrain:
                {
                    auto surfaceElement = info.Element->AsSurface();
                    if (surfaceElement != nullptr)
                    {
                        slope = DefaultPathSlope[surfaceElement->GetSlope() & TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK];
                    }
                    break;
                }
                case ViewportInteractionItem::Footpath:
                {
                    auto pathElement = info.Element->AsPath();
                    if (pathElement != nullptr)
                    {
                        slope = pathElement->GetSlopeDirection();
                        if (pathElement->IsSloped())
                        {
                            slope |= FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            auto z = info.Element->GetBaseZ();
            if (slope & RAISE_FOOTPATH_FLAG)
            {
                slope &= ~RAISE_FOOTPATH_FLAG;
                z += PATH_HEIGHT_STEP;
            }

            auto pathType = gFootpathSelection.GetSelectedSurface();
            auto constructFlags = CreateConstructFlags(pathType);
            _windowFootpathCost = footpath_provisional_set(
                pathType, gFootpathSelection.Railings, { info.Loc, z }, slope, constructFlags);
            window_invalidate_by_class(WC_FOOTPATH);
        }
    }

    void SetSelectionStartBridgeAtPoint(const ScreenCoordsXY& screenCoords)
    {
        int32_t direction;
        TileElement* tileElement;

        map_invalidate_selection_rect();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

        auto mapCoords = footpath_bridge_get_info_from_pos(screenCoords, &direction, &tileElement);
        if (mapCoords.IsNull())
        {
            return;
        }

        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        gMapSelectPositionA = mapCoords;
        gMapSelectPositionB = mapCoords;

        int32_t z = tileElement->GetBaseZ();

        if (tileElement->GetType() == TileElementType::Surface)
        {
            uint8_t slope = tileElement->AsSurface()->GetSlope();
            if (slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
            {
                z += PATH_HEIGHT_STEP;
            } // Add 2 for a slope
            if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
                z += PATH_HEIGHT_STEP; // Add another 2 for a steep slope
        }

        gMapSelectArrowPosition = CoordsXYZ{ mapCoords, z };
        gMapSelectArrowDirection = direction;

        map_invalidate_selection_rect();
    }

    void PlacePathAtPoint(const ScreenCoordsXY& screenCoords)
    {
        if (_footpathErrorOccured)
        {
            return;
        }

        footpath_provisional_update();

        const auto info = get_map_coordinates_from_pos(
            screenCoords, EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Footpath));

        if (info.SpriteType == ViewportInteractionItem::None)
        {
            return;
        }

        // Set path
        auto slope = 0;
        switch (info.SpriteType)
        {
            case ViewportInteractionItem::Terrain:
                slope = DefaultPathSlope[info.Element->AsSurface()->GetSlope() & TILE_ELEMENT_SURFACE_RAISED_CORNERS_MASK];
                break;
            case ViewportInteractionItem::Footpath:
                slope = info.Element->AsPath()->GetSlopeDirection();
                if (info.Element->AsPath()->IsSloped())
                {
                    slope |= FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
                }
                break;
            default:
                break;
        }
        auto z = info.Element->GetBaseZ();
        if (slope & RAISE_FOOTPATH_FLAG)
        {
            slope &= ~RAISE_FOOTPATH_FLAG;
            z += PATH_HEIGHT_STEP;
        }

        // Try and place path
        gGameCommandErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
        auto selectedType = gFootpathSelection.GetSelectedSurface();
        PathConstructFlags constructFlags = CreateConstructFlags(selectedType);

        auto footpathPlaceAction = FootpathPlaceAction(
            { info.Loc, z }, slope, selectedType, gFootpathSelection.Railings, INVALID_DIRECTION, constructFlags);
        footpathPlaceAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
            if (result->Error == GameActions::Status::Ok)
            {
                // Don't play sound if it is no cost to prevent multiple sounds. TODO: make this work in no money scenarios
                if (result->Cost != 0)
                {
                    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);
                }
            }
            else
            {
                _footpathErrorOccured = true;
            }
        });
        GameActions::Execute(&footpathPlaceAction);
    }

    void StartBridgeAtPoint(const ScreenCoordsXY& screenCoords)
    {
        int32_t z, direction;
        TileElement* tileElement;

        auto mapCoords = footpath_bridge_get_info_from_pos(screenCoords, &direction, &tileElement);
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
            if (slope & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
            {
                // Steep diagonal slope
                z += 2 * PATH_HEIGHT_STEP;
            }
            else if (slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
            {
                // Normal slope
                z += PATH_HEIGHT_STEP;
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
                        z += PATH_HEIGHT_STEP;
                    }
                }
            }
        }

        tool_cancel();
        gFootpathConstructFromPosition = { mapCoords, z };
        _footpathConstructDirection = direction;
        gProvisionalFootpath.Flags = 0;
        gFootpathConstructSlope = 0;
        _footpathConstructionMode = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL;
        _footpathConstructValidDirections = INVALID_DIRECTION;
        SetEnabledAndPressedWidgets();
    }

    void Construct()
    {
        _windowFootpathCost = MONEY32_UNDEFINED;
        footpath_provisional_update();

        ObjectEntryIndex type;
        int32_t slope;
        CoordsXYZ footpathLoc;
        GetNextPathInfo(&type, footpathLoc, &slope);

        gGameCommandErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
        PathConstructFlags constructFlags = CreateConstructFlags(type);

        auto footpathPlaceAction = FootpathPlaceAction(
            footpathLoc, slope, type, gFootpathSelection.Railings, _footpathConstructDirection, constructFlags);
        footpathPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
            if (result->Error == GameActions::Status::Ok)
            {
                OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);

                if (gFootpathConstructSlope == 0)
                {
                    _footpathConstructValidDirections = INVALID_DIRECTION;
                }
                else
                {
                    _footpathConstructValidDirections = _footpathConstructDirection;
                }

                if (gFootpathGroundFlags & ELEMENT_IS_UNDERGROUND)
                {
                    viewport_set_visibility(1);
                }

                gFootpathConstructFromPosition = footpathLoc;
                // If we have just built an upwards slope, the next path to construct is
                // a bit higher. Note that the z returned by footpath_get_next_path_info
                // already is lowered if we are building a downwards slope.
                if (gFootpathConstructSlope == 2)
                {
                    gFootpathConstructFromPosition.z += PATH_HEIGHT_STEP;
                }
            }
            SetEnabledAndPressedWidgets();
        });
        GameActions::Execute(&footpathPlaceAction);
    }

    void RemoveTileElement(TileElement* tileElement)
    {
        auto z = tileElement->GetBaseZ();
        if (tileElement->AsPath()->IsSloped())
        {
            uint8_t slopeDirection = tileElement->AsPath()->GetSlopeDirection();
            slopeDirection = direction_reverse(slopeDirection);
            if (slopeDirection == _footpathConstructDirection)
            {
                z += PATH_HEIGHT_STEP;
            }
        }

        // Find a connected edge
        int32_t edge = direction_reverse(_footpathConstructDirection);
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
                        edge = direction_reverse(edge);
                    }
                }
            }
        }

        gFootpathConstructFromPosition.z = tileElement->GetBaseZ();
        // Remove path
        footpath_remove(gFootpathConstructFromPosition, GAME_COMMAND_FLAG_APPLY);

        // Move selection
        edge = direction_reverse(edge);
        gFootpathConstructFromPosition.x -= CoordsDirectionDelta[edge].x;
        gFootpathConstructFromPosition.y -= CoordsDirectionDelta[edge].y;
        gFootpathConstructFromPosition.z = z;
        _footpathConstructDirection = edge;
        _footpathConstructValidDirections = INVALID_DIRECTION;
    }

    TileElement* GetTileElementToRemove()
    {
        TileElement* tileElement;
        int32_t z, zLow;

        if (!map_is_location_valid(gFootpathConstructFromPosition))
        {
            return nullptr;
        }

        z = std::min(255 * COORDS_Z_STEP, gFootpathConstructFromPosition.z);
        zLow = z - PATH_HEIGHT_STEP;

        tileElement = map_get_first_element_at(gFootpathConstructFromPosition);
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
                        if (direction_reverse(tileElement->AsPath()->GetSlopeDirection()) != _footpathConstructDirection)
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

    void Remove()
    {
        TileElement* tileElement;

        _windowFootpathCost = MONEY32_UNDEFINED;
        footpath_provisional_update();

        tileElement = GetTileElementToRemove();
        if (tileElement != nullptr)
        {
            RemoveTileElement(tileElement);
        }

        SetEnabledAndPressedWidgets();
    }

    void SetEnabledAndPressedWidgets()
    {
        if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
        {
            map_invalidate_map_selection_tiles();
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            gMapSelectFlags |= MAP_SELECT_FLAG_GREEN;

            int32_t direction = _footpathConstructDirection;
            gMapSelectionTiles.clear();
            gMapSelectionTiles.push_back({ gFootpathConstructFromPosition.x + CoordsDirectionDelta[direction].x,
                                           gFootpathConstructFromPosition.y + CoordsDirectionDelta[direction].y });
            map_invalidate_map_selection_tiles();
        }

        uint64_t pressedWidgets = pressed_widgets
            & ~((1LL << WIDX_DIRECTION_NW) | (1LL << WIDX_DIRECTION_NE) | (1LL << WIDX_DIRECTION_SW)
                | (1LL << WIDX_DIRECTION_SE) | (1LL << WIDX_SLOPEDOWN) | (1LL << WIDX_LEVEL) | (1LL << WIDX_SLOPEUP));
        uint64_t disabledWidgets = 0;
        int32_t currentRotation = get_current_rotation();
        if (_footpathConstructionMode >= PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
        {
            // Set pressed directional widget
            int32_t direction = (_footpathConstructDirection + currentRotation) & 3;
            pressedWidgets |= (1LL << (WIDX_DIRECTION_NW + direction));

            // Set pressed slope widget
            int32_t slope = gFootpathConstructSlope;
            if (slope == TILE_ELEMENT_SLOPE_SE_SIDE_UP)
            {
                pressedWidgets |= (1ULL << WIDX_SLOPEDOWN);
            }
            else if (slope == TILE_ELEMENT_SLOPE_FLAT)
            {
                pressedWidgets |= (1ULL << WIDX_LEVEL);
            }
            else
            {
                pressedWidgets |= (1ULL << WIDX_SLOPEUP);
            }

            // Enable / disable directional widgets
            direction = _footpathConstructValidDirections;
            if (direction != INVALID_DIRECTION)
            {
                disabledWidgets |= (1ULL << WIDX_DIRECTION_NW) | (1ULL << WIDX_DIRECTION_NE) | (1ULL << WIDX_DIRECTION_SW)
                    | (1ULL << WIDX_DIRECTION_SE);

                direction = (direction + currentRotation) & 3;
                disabledWidgets &= ~(1 << (WIDX_DIRECTION_NW + direction));
            }
        }
        else
        {
            // Disable all bridge mode widgets
            disabledWidgets |= (1ULL << WIDX_DIRECTION_GROUP) | (1ULL << WIDX_DIRECTION_NW) | (1ULL << WIDX_DIRECTION_NE)
                | (1ULL << WIDX_DIRECTION_SW) | (1ULL << WIDX_DIRECTION_SE) | (1ULL << WIDX_SLOPE_GROUP)
                | (1ULL << WIDX_SLOPEDOWN) | (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPEUP) | (1ULL << WIDX_CONSTRUCT)
                | (1ULL << WIDX_REMOVE);
        }

        pressed_widgets = pressedWidgets;
        disabled_widgets = disabledWidgets;
        Invalidate();
    }

    void GetNextPathInfo(ObjectEntryIndex* type, CoordsXYZ& footpathLoc, int32_t* slope)
    {
        auto direction = _footpathConstructDirection;
        footpathLoc.x = gFootpathConstructFromPosition.x + CoordsDirectionDelta[direction].x;
        footpathLoc.y = gFootpathConstructFromPosition.y + CoordsDirectionDelta[direction].y;
        footpathLoc.z = gFootpathConstructFromPosition.z;
        if (type != nullptr)
        {
            *type = gFootpathSelection.GetSelectedSurface();
        }
        *slope = TILE_ELEMENT_SLOPE_FLAT;
        if (gFootpathConstructSlope != 0)
        {
            *slope = _footpathConstructDirection | TILE_ELEMENT_SLOPE_S_CORNER_UP;
            if (gFootpathConstructSlope != 2)
            {
                footpathLoc.z -= PATH_HEIGHT_STEP;
                *slope ^= TILE_ELEMENT_SLOPE_E_CORNER_UP;
            }
        }
    }

    ObjectEntryIndex GetDefaultSurface(bool queue)
    {
        bool showEditorPaths = ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode);
        for (ObjectEntryIndex i = 0; i < MAX_FOOTPATH_SURFACE_OBJECTS; i++)
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
        return OBJECT_ENTRY_INDEX_NULL;
    }

    ObjectEntryIndex GetDefaultRailings()
    {
        for (ObjectEntryIndex i = 0; i < MAX_FOOTPATH_RAILINGS_OBJECTS; i++)
        {
            const auto* railingEntry = GetPathRailingsEntry(i);
            if (railingEntry != nullptr)
            {
                return i;
            }
        }
        return OBJECT_ENTRY_INDEX_NULL;
    }

    bool IsSurfaceEntryOkay(ObjectEntryIndex index, bool queue)
    {
        auto pathEntry = GetPathSurfaceEntry(index);
        if (pathEntry != nullptr)
        {
            bool showEditorPaths = ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode);
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

    bool IsLegacyPathEntryOkay(ObjectEntryIndex index)
    {
        bool showEditorPaths = ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode);
        auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
        auto footpathObj = static_cast<FootpathObject*>(objManager.GetLoadedObject(ObjectType::Paths, index));
        if (footpathObj != nullptr)
        {
            auto pathEntry = reinterpret_cast<rct_footpath_entry*>(footpathObj->GetLegacyData());
            return showEditorPaths || !(pathEntry->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR);
        }
        return false;
    }

    ObjectEntryIndex GetDefaultLegacyPath()
    {
        for (ObjectEntryIndex i = 0; i < MAX_PATH_OBJECTS; i++)
        {
            if (IsLegacyPathEntryOkay(i))
            {
                return i;
            }
        }
        return OBJECT_ENTRY_INDEX_NULL;
    }

    bool SelectDefault()
    {
        // Select default footpath
        auto surfaceIndex = GetDefaultSurface(false);
        if (IsSurfaceEntryOkay(gFootpathSelection.NormalSurface, false))
        {
            surfaceIndex = gFootpathSelection.NormalSurface;
        }

        // Select default queue
        auto queueIndex = GetDefaultSurface(true);
        if (IsSurfaceEntryOkay(gFootpathSelection.QueueSurface, true))
        {
            queueIndex = gFootpathSelection.QueueSurface;
        }

        // Select default railing
        auto railingIndex = GetDefaultRailings();
        const auto* railingEntry = GetPathRailingsEntry(gFootpathSelection.Railings);
        if (railingEntry != nullptr)
        {
            railingIndex = gFootpathSelection.Railings;
        }

        // Select default legacy path
        auto legacyPathIndex = GetDefaultLegacyPath();
        if (gFootpathSelection.LegacyPath != OBJECT_ENTRY_INDEX_NULL)
        {
            if (IsLegacyPathEntryOkay(gFootpathSelection.LegacyPath))
            {
                // Keep legacy path selected
                legacyPathIndex = gFootpathSelection.LegacyPath;
            }
            else
            {
                // Reset legacy path, we default to a surface (if there are any)
                gFootpathSelection.LegacyPath = OBJECT_ENTRY_INDEX_NULL;
            }
        }

        if (surfaceIndex == OBJECT_ENTRY_INDEX_NULL)
        {
            if (legacyPathIndex == OBJECT_ENTRY_INDEX_NULL)
            {
                // No surfaces or legacy paths available
                return false;
            }

            // No surfaces available, so default to legacy path
            gFootpathSelection.LegacyPath = legacyPathIndex;
        }

        gFootpathSelection.NormalSurface = surfaceIndex;
        gFootpathSelection.QueueSurface = queueIndex;
        gFootpathSelection.Railings = railingIndex;
        return true;
    }

    PathConstructFlags CreateConstructFlags(ObjectEntryIndex& type)
    {
        PathConstructFlags pathConstructFlags = 0;
        if (gFootpathSelection.IsQueueSelected)
            pathConstructFlags |= PathConstructFlag::IsQueue;
        if (gFootpathSelection.LegacyPath != OBJECT_ENTRY_INDEX_NULL)
        {
            pathConstructFlags |= PathConstructFlag::IsLegacyPathObject;
            type = gFootpathSelection.LegacyPath;
        }
        return pathConstructFlags;
    }
};

// Keyboard Shortcut wrapper functions
void window_footpath_keyboard_shortcut_turn_left()
{
    auto* window = (FootpathWindow*)window_find_by_class(WC_FOOTPATH);
    window->KeyboardShortcutTurnLeft();
}

void window_footpath_keyboard_shortcut_turn_right()
{
    auto* window = (FootpathWindow*)window_find_by_class(WC_FOOTPATH);
    window->KeyboardShortcutTurnRight();
}

void window_footpath_keyboard_shortcut_slope_down()
{
    auto* window = dynamic_cast<FootpathWindow*>(window_find_by_class(WC_FOOTPATH));
    window->KeyboardShortcutSlopeDown();
}

void window_footpath_keyboard_shortcut_slope_up()
{
    auto* window = dynamic_cast<FootpathWindow*>(window_find_by_class(WC_FOOTPATH));
    window->KeyboardShortcutSlopeUp();
}

void window_footpath_keyboard_shortcut_demolish_current()
{
    auto* window = dynamic_cast<FootpathWindow*> (window_find_by_class(WC_FOOTPATH));
    window->KeyboardShortcutDemolishCurrent();
}

void window_footpath_keyboard_shortcut_build_current()
{
    auto* window = dynamic_cast<FootpathWindow*> (window_find_by_class(WC_FOOTPATH));
    window->KeyboardShortcutBuildCurrent();
}

void WindowFootpathResetSelectedPath()
{
    auto* window =  dynamic_cast<FootpathWindow*> (window_find_by_class(WC_FOOTPATH));
    window->ResetSelectedPath();
}

rct_window* WindowFootpathOpen()
{
    // Check if Window is already open
    auto* window = window_bring_to_front_by_class(WC_FOOTPATH);
    if (window == nullptr)
    {
        window = WindowCreate<FootpathWindow>(WC_FOOTPATH, ScreenCoordsXY(0, 29), WW, WH, 0);
    }
    return window;
}
