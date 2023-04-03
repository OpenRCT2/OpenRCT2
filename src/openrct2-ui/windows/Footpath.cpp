/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/input/ShortcutIds.h>
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
#include <openrct2/actions/FootpathRemoveAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/FootpathObject.h>
#include <openrct2/object/FootpathRailingsObject.h>
#include <openrct2/object/FootpathSurfaceObject.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Surface.h>

using namespace OpenRCT2::Ui;

// clang-format off
enum
{
    PATH_CONSTRUCTION_MODE_LAND,
    PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL,
    PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL
};

#pragma region Measurements

static constexpr const StringId WINDOW_TITLE = STR_FOOTPATHS;
static constexpr const int32_t WH_WINDOW = 421;
static constexpr const int32_t WW_WINDOW = 106;

#pragma endregion

#pragma region Widgets

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

static Widget window_footpath_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW_WINDOW, WH_WINDOW),

    // Type group
    MakeWidget({ 3,  17}, {100, 95}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_TYPE                                                                          ),
    MakeWidget({ 6,  30}, { 47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_FOOTPATH_TIP                               ),
    MakeWidget({53,  30}, { 47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_QUEUE_LINE_PATH_TIP                        ),
    MakeWidget({29,  69}, { 47, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_OBJECT_SELECTION_FOOTPATH_RAILINGS         ),

    // Direction group
    MakeWidget({ 3, 115}, {100, 77}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_DIRECTION                                                                     ),
    MakeWidget({53, 127}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_CONSTRUCTION_DIRECTION_NE),     STR_DIRECTION_TIP                              ),
    MakeWidget({53, 156}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_CONSTRUCTION_DIRECTION_SE),     STR_DIRECTION_TIP                              ),
    MakeWidget({ 8, 156}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_CONSTRUCTION_DIRECTION_SW),     STR_DIRECTION_TIP                              ),
    MakeWidget({ 8, 127}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_CONSTRUCTION_DIRECTION_NW),     STR_DIRECTION_TIP                              ),

    // Slope group
    MakeWidget({ 3, 195}, {100, 41}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_SLOPE                                                                         ),
    MakeWidget({17, 207}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_DOWN),  STR_SLOPE_DOWN_TIP                             ),
    MakeWidget({41, 207}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_LEVEL), STR_LEVEL_TIP                                  ),
    MakeWidget({65, 207}, { 24, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_UP),    STR_SLOPE_UP_TIP                               ),
    MakeWidget({ 8, 242}, { 90, 90}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, 0xFFFFFFFF,                        STR_CONSTRUCT_THE_SELECTED_FOOTPATH_SECTION_TIP),
    MakeWidget({30, 335}, { 46, 24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_DEMOLISH_CURRENT_SECTION),      STR_REMOVE_PREVIOUS_FOOTPATH_SECTION_TIP       ),

    // Mode group
    MakeWidget({ 3, 361}, {100, 54}, WindowWidgetType::Groupbox, WindowColour::Primary                                                                                      ),
    MakeWidget({13, 372}, { 36, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_CONSTRUCTION_FOOTPATH_LAND),    STR_CONSTRUCT_FOOTPATH_ON_LAND_TIP             ),
    MakeWidget({57, 372}, { 36, 36}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_CONSTRUCTION_FOOTPATH_BRIDGE),  STR_CONSTRUCT_BRIDGE_OR_TUNNEL_FOOTPATH_TIP    ),
    WIDGETS_END,
};

#pragma endregion

/** rct2: 0x0098D8B4 */
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

/** rct2: 0x0098D7E0 */
static constexpr const uint8_t ConstructionPreviewImages[][4] = {
    { 5, 10, 5, 10 },   // Flat
    { 16, 17, 18, 19 }, // Upwards
    { 18, 19, 16, 17 }, // Downwards
};
// clang-format on

class FootpathWindow final : public Window
{
private:
    const uint16_t ARROW_PULSE_DURATION = 200;

    uint8_t _footpathConstructDirection;
    uint8_t _footpathConstructValidDirections;
    uint8_t _footpathConstructionMode;

    std::vector<std::pair<ObjectType, ObjectEntryIndex>> _dropdownEntries;

    money64 _windowFootpathCost;
    uint32_t _footpathConstructionNextArrowPulse = 0;
    uint8_t _lastUpdatedCameraRotation = UINT8_MAX;
    bool _footpathErrorOccured = false;

public:
#pragma region Window Override Events

    void OnOpen() override
    {
        widgets = window_footpath_widgets;

        WindowInitScrollWidgets(*this);
        WindowPushOthersRight(*this);
        ShowGridlines();

        ToolCancel();
        _footpathConstructionMode = PATH_CONSTRUCTION_MODE_LAND;
        ToolSet(*this, WIDX_CONSTRUCT_ON_LAND, Tool::PathDown);
        InputSetFlag(INPUT_FLAG_6, true);
        _footpathErrorOccured = false;
        WindowFootpathSetEnabledAndPressedWidgets();
    }

    void OnClose() override
    {
        FootpathProvisionalUpdate();
        ViewportSetVisibility(0);
        MapInvalidateMapSelectionTiles();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        WindowInvalidateByClass(WindowClass::TopToolbar);
        HideGridlines();
    }

    void OnUpdate() override
    {
        WidgetInvalidate(*this, WIDX_CONSTRUCT);
        WindowFootpathUpdateProvisionalPathForBridgeMode();

        // #2502: The camera might have changed rotation, so we need to update which directional buttons are pressed
        uint8_t currentRotation = GetCurrentRotation();
        if (_lastUpdatedCameraRotation != currentRotation)
        {
            _lastUpdatedCameraRotation = currentRotation;
            WindowFootpathSetEnabledAndPressedWidgets();
        }

        // Check tool
        if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_LAND)
        {
            if (!(InputTestFlag(INPUT_FLAG_TOOL_ACTIVE)))
            {
                Close();
            }
            else if (gCurrentToolWidget.window_classification != WindowClass::Footpath)
            {
                Close();
            }
            else if (gCurrentToolWidget.widget_index != WIDX_CONSTRUCT_ON_LAND)
            {
                Close();
            }
        }
        else if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL)
        {
            if (!(InputTestFlag(INPUT_FLAG_TOOL_ACTIVE)))
            {
                Close();
            }
            else if (gCurrentToolWidget.window_classification != WindowClass::Footpath)
            {
                Close();
            }
            else if (gCurrentToolWidget.widget_index != WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
            {
                Close();
            }
        }
    }

    void OnMouseDown(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_FOOTPATH_TYPE:
                WindowFootpathShowFootpathTypesDialog(&window_footpath_widgets[widgetIndex], false);
                break;
            case WIDX_QUEUELINE_TYPE:
                WindowFootpathShowFootpathTypesDialog(&window_footpath_widgets[widgetIndex], true);
                break;
            case WIDX_RAILINGS_TYPE:
                WindowFootpathShowRailingsTypesDialog(&window_footpath_widgets[widgetIndex]);
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
                WindowFootpathMousedownSlope(6);
                break;
            case WIDX_LEVEL:
                WindowFootpathMousedownSlope(0);
                break;
            case WIDX_SLOPEUP:
                WindowFootpathMousedownSlope(2);
                break;
        }
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_CONSTRUCT:
                WindowFootpathConstruct();
                break;
            case WIDX_REMOVE:
                WindowFootpathRemove();
                break;
            case WIDX_CONSTRUCT_ON_LAND:
                if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_LAND)
                {
                    break;
                }

                _windowFootpathCost = MONEY64_UNDEFINED;
                ToolCancel();
                FootpathProvisionalUpdate();
                MapInvalidateMapSelectionTiles();
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
                _footpathConstructionMode = PATH_CONSTRUCTION_MODE_LAND;
                ToolSet(*this, WIDX_CONSTRUCT_ON_LAND, Tool::PathDown);
                InputSetFlag(INPUT_FLAG_6, true);
                _footpathErrorOccured = false;
                WindowFootpathSetEnabledAndPressedWidgets();
                break;
            case WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL:
                if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL)
                {
                    break;
                }

                _windowFootpathCost = MONEY64_UNDEFINED;
                ToolCancel();
                FootpathProvisionalUpdate();
                MapInvalidateMapSelectionTiles();
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
                _footpathConstructionMode = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL;
                ToolSet(*this, WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL, Tool::Crosshair);
                InputSetFlag(INPUT_FLAG_6, true);
                _footpathErrorOccured = false;
                WindowFootpathSetEnabledAndPressedWidgets();
                break;
        }
    }

    void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
    {
        if (selectedIndex < 0 || static_cast<size_t>(selectedIndex) >= _dropdownEntries.size())
            return;

        auto entryIndex = _dropdownEntries[selectedIndex];
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

        FootpathProvisionalUpdate();
        _windowFootpathCost = MONEY64_UNDEFINED;
        Invalidate();
    }

    void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
        {
            WindowFootpathSetProvisionalPathAtPoint(screenCoords);
        }
        else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
        {
            WindowFootpathSetSelectionStartBridgeAtPoint(screenCoords);
        }
    }

    void OnToolUp(WidgetIndex widgetIndex, const ScreenCoordsXY&) override
    {
        if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
        {
            _footpathErrorOccured = false;
        }
    }

    void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
        {
            WindowFootpathPlacePathAtPoint(screenCoords);
        }
        else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
        {
            WindowFootpathStartBridgeAtPoint(screenCoords);
        }
    }

    void OnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
        {
            WindowFootpathPlacePathAtPoint(screenCoords);
        }
    }

    void OnPrepareDraw() override
    {
        // Press / unpress footpath and queue type buttons
        pressed_widgets &= ~(1uLL << WIDX_FOOTPATH_TYPE);
        pressed_widgets &= ~(1uLL << WIDX_QUEUELINE_TYPE);
        pressed_widgets |= gFootpathSelection.IsQueueSelected ? (1uLL << WIDX_QUEUELINE_TYPE) : (1uLL << WIDX_FOOTPATH_TYPE);

        // Enable / disable construct button
        window_footpath_widgets[WIDX_CONSTRUCT].type = _footpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL
            ? WindowWidgetType::ImgBtn
            : WindowWidgetType::Empty;

        if (gFootpathSelection.LegacyPath == OBJECT_ENTRY_INDEX_NULL)
        {
            window_footpath_widgets[WIDX_RAILINGS_TYPE].type = WindowWidgetType::FlatBtn;
        }
        else
        {
            window_footpath_widgets[WIDX_RAILINGS_TYPE].type = WindowWidgetType::Empty;
        }
    }

    void OnDraw(DrawPixelInfo& dpi) override
    {
        ScreenCoordsXY screenCoords;
        WindowDrawWidgets(*this, dpi);
        WindowFootpathDrawDropdownButtons(dpi);

        if (!IsWidgetDisabled(WIDX_CONSTRUCT))
        {
            // Get construction image
            uint8_t direction = (_footpathConstructDirection + GetCurrentRotation()) % 4;
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
                    auto pathEntry = reinterpret_cast<const FootpathEntry*>(pathObj->GetLegacyData());
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
                screenCoords = this->windowPos
                    + ScreenCoordsXY{ window_footpath_widgets[WIDX_CONSTRUCT].midX(),
                                      window_footpath_widgets[WIDX_CONSTRUCT].bottom - 60 };
                GfxDrawSprite(dpi, ImageId(image), screenCoords);
            }

            // Draw build this... label
            screenCoords = this->windowPos
                + ScreenCoordsXY{ window_footpath_widgets[WIDX_CONSTRUCT].midX(),
                                  window_footpath_widgets[WIDX_CONSTRUCT].bottom - 23 };
            DrawTextBasic(dpi, screenCoords, STR_BUILD_THIS, {}, { TextAlignment::CENTRE });
        }

        // Draw cost
        screenCoords = this->windowPos
            + ScreenCoordsXY{ window_footpath_widgets[WIDX_CONSTRUCT].midX(),
                              window_footpath_widgets[WIDX_CONSTRUCT].bottom - 12 };
        if (_windowFootpathCost != MONEY64_UNDEFINED)
        {
            if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
            {
                auto ft = Formatter();
                ft.Add<money64>(_windowFootpathCost);
                DrawTextBasic(dpi, screenCoords, STR_COST_LABEL, ft, { TextAlignment::CENTRE });
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
        if (_footpathConstructionMode != PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
        {
            return;
        }

        // Recheck area for construction. Set by ride_construction window
        if (gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_2)
        {
            FootpathProvisionalRemove();
            gProvisionalFootpath.Flags &= ~PROVISIONAL_PATH_FLAG_2;
        }

        // Update provisional bridge mode path
        if (!(gProvisionalFootpath.Flags & PROVISIONAL_PATH_FLAG_1))
        {
            ObjectEntryIndex type;
            ObjectEntryIndex railings = gFootpathSelection.Railings;

            CoordsXYZ footpathLoc;
            int32_t slope;
            FootpathGetNextPathInfo(&type, footpathLoc, &slope);
            auto pathConstructFlags = FootpathCreateConstructFlags(type);

            _windowFootpathCost = FootpathProvisionalSet(type, railings, footpathLoc, slope, pathConstructFlags);
            WidgetInvalidate(*this, WIDX_CONSTRUCT);
        }

        auto curTime = Platform::GetTicks();

        // Update little directional arrow on provisional bridge mode path
        if (_footpathConstructionNextArrowPulse < curTime)
        {
            _footpathConstructionNextArrowPulse = curTime + ARROW_PULSE_DURATION;

            gProvisionalFootpath.Flags ^= PROVISIONAL_PATH_FLAG_SHOW_ARROW;
            CoordsXYZ footpathLoc;
            int32_t slope;
            FootpathGetNextPathInfo(nullptr, footpathLoc, &slope);
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
            MapInvalidateTileFull(footpathLoc);
        }
    }

    void WindowFootpathDrawDropdownButtons(DrawPixelInfo& dpi)
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

            WindowFootpathDrawDropdownButton(dpi, WIDX_FOOTPATH_TYPE, pathImage);
            WindowFootpathDrawDropdownButton(dpi, WIDX_QUEUELINE_TYPE, queueImage);

            // Set railing
            auto railingsImage = static_cast<uint32_t>(SPR_NONE);
            auto railingsEntry = GetPathRailingsEntry(gFootpathSelection.Railings);
            if (railingsEntry != nullptr)
            {
                railingsImage = railingsEntry->PreviewImageId;
            }
            WindowFootpathDrawDropdownButton(dpi, WIDX_RAILINGS_TYPE, railingsImage);
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
                auto pathEntry = reinterpret_cast<FootpathEntry*>(pathObj->GetLegacyData());
                pathImage = pathEntry->GetPreviewImage();
                queueImage = pathEntry->GetQueuePreviewImage();
            }

            WindowFootpathDrawDropdownButton(dpi, WIDX_FOOTPATH_TYPE, pathImage);
            WindowFootpathDrawDropdownButton(dpi, WIDX_QUEUELINE_TYPE, queueImage);
        }
    }

    void WindowFootpathDrawDropdownButton(DrawPixelInfo& dpi, WidgetIndex widgetIndex, ImageIndex image)
    {
        const auto& widget = widgets[widgetIndex];
        GfxDrawSprite(dpi, ImageId(image), { windowPos.x + widget.left, windowPos.y + widget.top });
    }

    /**
     *
     *  rct2: 0x006A7F88
     */
    void WindowFootpathShowFootpathTypesDialog(Widget* widget, bool showQueues)
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
            // If regular paths can be used as queue, only hide the path if we’re _not_ looking at a queue,
            // but the path surface is one.
            if (gCheatsAllowRegularPathAsQueue && !showQueues && ((pathType->Flags & FOOTPATH_ENTRY_FLAG_IS_QUEUE) != 0))
            {
                continue;
            }
            // If the cheat is disabled, hide queues from the regular path view and vice versa.
            else if (!gCheatsAllowRegularPathAsQueue && showQueues != ((pathType->Flags & FOOTPATH_ENTRY_FLAG_IS_QUEUE) != 0))
            {
                continue;
            }

            if (gFootpathSelection.LegacyPath == OBJECT_ENTRY_INDEX_NULL
                && i == (showQueues ? gFootpathSelection.QueueSurface : gFootpathSelection.NormalSurface))
            {
                defaultIndex = numPathTypes;
            }

            gDropdownItems[numPathTypes].Format = STR_NONE;
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

            auto pathEntry = reinterpret_cast<const FootpathEntry*>(pathObj->GetLegacyData());
            if ((pathEntry->flags & FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR) && !showEditorPaths)
            {
                continue;
            }

            if (gFootpathSelection.LegacyPath != OBJECT_ENTRY_INDEX_NULL && gFootpathSelection.LegacyPath == i)
            {
                defaultIndex = numPathTypes;
            }

            gDropdownItems[numPathTypes].Format = STR_NONE;
            Dropdown::SetImage(
                numPathTypes, ImageId(showQueues ? pathEntry->GetQueuePreviewImage() : pathEntry->GetPreviewImage()));
            _dropdownEntries.push_back({ ObjectType::Paths, i });
            numPathTypes++;
        }

        auto itemsPerRow = DropdownGetAppropriateImageDropdownItemsPerRow(numPathTypes);
        WindowDropdownShowImage(
            windowPos.x + widget->left, windowPos.y + widget->top, widget->height() + 1, colours[1], 0, numPathTypes, 47, 36,
            itemsPerRow);
        if (defaultIndex)
            gDropdownDefaultIndex = static_cast<int32_t>(*defaultIndex);
    }

    void WindowFootpathShowRailingsTypesDialog(Widget* widget)
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

            gDropdownItems[numRailingsTypes].Format = STR_NONE;
            Dropdown::SetImage(numRailingsTypes, ImageId(railingsEntry->PreviewImageId));
            _dropdownEntries.push_back({ ObjectType::FootpathRailings, i });
            numRailingsTypes++;
        }

        auto itemsPerRow = DropdownGetAppropriateImageDropdownItemsPerRow(numRailingsTypes);
        WindowDropdownShowImage(
            windowPos.x + widget->left, windowPos.y + widget->top, widget->height() + 1, colours[1], 0, numRailingsTypes, 47,
            36, itemsPerRow);
        if (defaultIndex)
            gDropdownDefaultIndex = static_cast<int32_t>(*defaultIndex);
    }

    /**
     *
     *  rct2: 0x006A8111 0x006A8135 0x006A815C 0x006A8183
     */
    void WindowFootpathMousedownDirection(int32_t direction)
    {
        FootpathProvisionalUpdate();
        _footpathConstructDirection = (direction - GetCurrentRotation()) & 3;
        _windowFootpathCost = MONEY64_UNDEFINED;
        WindowFootpathSetEnabledAndPressedWidgets();
    }

    /**
     *
     *  rct2: 0x006A81AA 0x006A81C5 0x006A81E0
     */
    void WindowFootpathMousedownSlope(int32_t slope)
    {
        FootpathProvisionalUpdate();
        gFootpathConstructSlope = slope;
        _windowFootpathCost = MONEY64_UNDEFINED;
        WindowFootpathSetEnabledAndPressedWidgets();
    }

    /**
     *
     *  rct2: 0x006A81FB
     */
    void WindowFootpathSetProvisionalPathAtPoint(const ScreenCoordsXY& screenCoords)
    {
        MapInvalidateSelectionRect();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

        auto info = GetMapCoordinatesFromPos(
            screenCoords, EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Footpath));

        if (info.SpriteType == ViewportInteractionItem::None || info.Element == nullptr)
        {
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            FootpathProvisionalUpdate();
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

            FootpathProvisionalUpdate();

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
            auto constructFlags = FootpathCreateConstructFlags(pathType);
            _windowFootpathCost = FootpathProvisionalSet(
                pathType, gFootpathSelection.Railings, { info.Loc, z }, slope, constructFlags);
            WindowInvalidateByClass(WindowClass::Footpath);
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

        MapInvalidateSelectionRect();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

        auto mapCoords = FootpathBridgeGetInfoFromPos(screenCoords, &direction, &tileElement);
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

        MapInvalidateSelectionRect();
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

        FootpathProvisionalUpdate();

        const auto info = GetMapCoordinatesFromPos(
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
        auto selectedType = gFootpathSelection.GetSelectedSurface();
        PathConstructFlags constructFlags = FootpathCreateConstructFlags(selectedType);

        auto footpathPlaceAction = FootpathPlaceAction(
            { info.Loc, z }, slope, selectedType, gFootpathSelection.Railings, INVALID_DIRECTION, constructFlags);
        footpathPlaceAction.SetCallback([this](const GameAction* ga, const GameActions::Result* result) {
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

        ToolCancel();
        gFootpathConstructFromPosition = { mapCoords, z };
        _footpathConstructDirection = direction;
        gProvisionalFootpath.Flags = 0;
        gFootpathConstructSlope = 0;
        _footpathConstructionMode = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL;
        _footpathConstructValidDirections = INVALID_DIRECTION;
        WindowFootpathSetEnabledAndPressedWidgets();
    }

    /**
     * Construct a piece of footpath while in bridge building mode.
     *  rct2: 0x006A79B7
     */
    void WindowFootpathConstruct()
    {
        _windowFootpathCost = MONEY64_UNDEFINED;
        FootpathProvisionalUpdate();

        ObjectEntryIndex type;
        int32_t slope;
        CoordsXYZ footpathLoc;
        FootpathGetNextPathInfo(&type, footpathLoc, &slope);

        PathConstructFlags constructFlags = FootpathCreateConstructFlags(type);

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
                    ViewportSetVisibility(1);
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
            WindowFootpathSetEnabledAndPressedWidgets();
        });
        GameActions::Execute(&footpathPlaceAction);
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
                z += PATH_HEIGHT_STEP;
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

        gFootpathConstructFromPosition.z = tileElement->GetBaseZ();
        auto action = FootpathRemoveAction(gFootpathConstructFromPosition);
        GameActions::Execute(&action);

        // Move selection
        edge = DirectionReverse(edge);
        gFootpathConstructFromPosition.x -= CoordsDirectionDelta[edge].x;
        gFootpathConstructFromPosition.y -= CoordsDirectionDelta[edge].y;
        gFootpathConstructFromPosition.z = z;
        _footpathConstructDirection = edge;
        _footpathConstructValidDirections = INVALID_DIRECTION;
    }

    /**
     *
     *  rct2: 0x006A7873
     */
    TileElement* FootpathGetTileElementToRemove()
    {
        TileElement* tileElement;
        int32_t z, zLow;

        if (!MapIsLocationValid(gFootpathConstructFromPosition))
        {
            return nullptr;
        }

        z = std::min(255 * COORDS_Z_STEP, gFootpathConstructFromPosition.z);
        zLow = z - PATH_HEIGHT_STEP;

        tileElement = MapGetFirstElementAt(gFootpathConstructFromPosition);
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

        _windowFootpathCost = MONEY64_UNDEFINED;
        FootpathProvisionalUpdate();

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
        if (_footpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
        {
            MapInvalidateMapSelectionTiles();
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            gMapSelectFlags |= MAP_SELECT_FLAG_GREEN;

            int32_t direction = _footpathConstructDirection;
            gMapSelectionTiles.clear();
            gMapSelectionTiles.push_back({ gFootpathConstructFromPosition.x + CoordsDirectionDelta[direction].x,
                                           gFootpathConstructFromPosition.y + CoordsDirectionDelta[direction].y });
            MapInvalidateMapSelectionTiles();
        }

        uint64_t pressedWidgets = pressed_widgets
            & ~((1LL << WIDX_DIRECTION_NW) | (1LL << WIDX_DIRECTION_NE) | (1LL << WIDX_DIRECTION_SW)
                | (1LL << WIDX_DIRECTION_SE) | (1LL << WIDX_SLOPEDOWN) | (1LL << WIDX_LEVEL) | (1LL << WIDX_SLOPEUP));
        uint64_t disabledWidgets = 0;
        int32_t currentRotation = GetCurrentRotation();
        if (_footpathConstructionMode >= PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
        {
            // Set pressed directional widget
            int32_t direction = (_footpathConstructDirection + currentRotation) & 3;
            pressedWidgets |= (1LL << (WIDX_DIRECTION_NW + direction));

            // Set pressed slope widget
            int32_t slope = gFootpathConstructSlope;
            if (slope == TILE_ELEMENT_SLOPE_SE_SIDE_UP)
            {
                pressedWidgets |= (1uLL << WIDX_SLOPEDOWN);
            }
            else if (slope == TILE_ELEMENT_SLOPE_FLAT)
            {
                pressedWidgets |= (1uLL << WIDX_LEVEL);
            }
            else
            {
                pressedWidgets |= (1uLL << WIDX_SLOPEUP);
            }

            // Enable / disable directional widgets
            direction = _footpathConstructValidDirections;
            if (direction != INVALID_DIRECTION)
            {
                disabledWidgets |= (1uLL << WIDX_DIRECTION_NW) | (1uLL << WIDX_DIRECTION_NE) | (1uLL << WIDX_DIRECTION_SW)
                    | (1uLL << WIDX_DIRECTION_SE);

                direction = (direction + currentRotation) & 3;
                disabledWidgets &= ~(1 << (WIDX_DIRECTION_NW + direction));
            }
        }
        else
        {
            // Disable all bridge mode widgets
            disabledWidgets |= (1uLL << WIDX_DIRECTION_GROUP) | (1uLL << WIDX_DIRECTION_NW) | (1uLL << WIDX_DIRECTION_NE)
                | (1uLL << WIDX_DIRECTION_SW) | (1uLL << WIDX_DIRECTION_SE) | (1uLL << WIDX_SLOPE_GROUP)
                | (1uLL << WIDX_SLOPEDOWN) | (1uLL << WIDX_LEVEL) | (1uLL << WIDX_SLOPEUP) | (1uLL << WIDX_CONSTRUCT)
                | (1uLL << WIDX_REMOVE);
        }

        pressed_widgets = pressedWidgets;
        disabled_widgets = disabledWidgets;
        Invalidate();
    }

    /**
     *
     *  rct2: 0x006A7B20
     */
    void FootpathGetNextPathInfo(ObjectEntryIndex* type, CoordsXYZ& footpathLoc, int32_t* slope)
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

    PathConstructFlags FootpathCreateConstructFlags(ObjectEntryIndex& type)
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

#pragma region Keyboard Shortcuts Events
public:
    void KeyboardShortcutTurnLeft()
    {
        if (IsWidgetDisabled(WIDX_DIRECTION_NW) || IsWidgetDisabled(WIDX_DIRECTION_NE) || IsWidgetDisabled(WIDX_DIRECTION_SW)
            || IsWidgetDisabled(WIDX_DIRECTION_SE) || _footpathConstructionMode != 2)
        {
            return;
        }
        int32_t currentRotation = GetCurrentRotation();
        int32_t turnedRotation = _footpathConstructDirection - currentRotation + (currentRotation % 2 == 1 ? 1 : -1);
        WindowFootpathMousedownDirection(turnedRotation);
    }

    void KeyboardShortcutTurnRight()
    {
        if (IsWidgetDisabled(WIDX_DIRECTION_NW) || IsWidgetDisabled(WIDX_DIRECTION_NE) || IsWidgetDisabled(WIDX_DIRECTION_SW)
            || IsWidgetDisabled(WIDX_DIRECTION_SE) || _footpathConstructionMode != 2)
        {
            return;
        }
        int32_t currentRotation = GetCurrentRotation();
        int32_t turnedRotation = _footpathConstructDirection - currentRotation + (currentRotation % 2 == 1 ? -1 : 1);
        WindowFootpathMousedownDirection(turnedRotation);
    }

    void KeyboardShortcutShortcutSlopeDown()
    {
        if (IsWidgetDisabled(WIDX_SLOPEDOWN) || IsWidgetDisabled(WIDX_LEVEL) || IsWidgetDisabled(WIDX_SLOPEUP)
            || widgets[WIDX_LEVEL].type == WindowWidgetType::Empty)
        {
            return;
        }

        switch (gFootpathConstructSlope)
        {
            case 0:
                OnMouseDown(WIDX_SLOPEDOWN);
                break;
            case 2:
                OnMouseDown(WIDX_LEVEL);
                break;
            default:
            case 6:
                return;
        }
    }

    void KeyboardShortcutSlopeUp()
    {
        if (IsWidgetDisabled(WIDX_SLOPEDOWN) || IsWidgetDisabled(WIDX_LEVEL) || IsWidgetDisabled(WIDX_SLOPEUP)
            || widgets[WIDX_LEVEL].type == WindowWidgetType::Empty)
        {
            return;
        }

        switch (gFootpathConstructSlope)
        {
            case 6:
                OnMouseDown(WIDX_LEVEL);
                break;
            case 0:
                OnMouseDown(WIDX_SLOPEUP);
                break;
            default:
            case 2:
                return;
        }
    }

    void KeyboardShortcutSlopeLevel()
    {
        if (IsWidgetDisabled(WIDX_SLOPEDOWN) || IsWidgetDisabled(WIDX_LEVEL) || IsWidgetDisabled(WIDX_SLOPEUP)
            || widgets[WIDX_LEVEL].type == WindowWidgetType::Empty || gFootpathConstructSlope == 0)
        {
            return;
        }

        OnMouseDown(WIDX_LEVEL);
    }

    void KeyboardShortcutDemolishCurrent()
    {
        if (IsWidgetDisabled(WIDX_REMOVE) || widgets[WIDX_REMOVE].type == WindowWidgetType::Empty
            || (!gCheatsBuildInPauseMode && GameIsPaused()))
        {
            return;
        }

        WindowFootpathRemove();
    }

    void KeyboardShortcutBuildCurrent()
    {
        if (IsWidgetDisabled(WIDX_CONSTRUCT) || widgets[WIDX_CONSTRUCT].type == WindowWidgetType::Empty)
        {
            return;
        }

        OnMouseUp(WIDX_CONSTRUCT);
    }

#pragma endregion
};

/**
 *
 *  rct2: 0x006A7C43
 */
WindowBase* WindowFootpathOpen()
{
    if (!FootpathSelectDefault())
    {
        // No path objects to select from, don't open window
        return nullptr;
    }

    return WindowFocusOrCreate<FootpathWindow>(WindowClass::Footpath, WW_WINDOW, WH_WINDOW, 0);
}

void WindowFootpathResetSelectedPath()
{
    gFootpathSelection = {};
}

void WindowFootpathKeyboardShortcutTurnLeft()
{
    WindowBase* w = WindowFindByClass(WindowClass::Footpath);
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
    WindowBase* w = WindowFindByClass(WindowClass::Footpath);
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
    WindowBase* w = WindowFindByClass(WindowClass::Footpath);
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
    WindowBase* w = WindowFindByClass(WindowClass::Footpath);
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
    WindowBase* w = WindowFindByClass(WindowClass::Footpath);
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
    WindowBase* w = WindowFindByClass(WindowClass::Footpath);
    if (w != nullptr)
    {
        auto* footpathWindow = static_cast<FootpathWindow*>(w);
        if (footpathWindow != nullptr)
        {
            footpathWindow->KeyboardShortcutBuildCurrent();
        }
    }
}
