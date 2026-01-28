/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <deque>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/ViewportInteraction.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/BannerPlaceAction.h>
#include <openrct2/actions/BannerSetColourAction.h>
#include <openrct2/actions/FootpathAdditionPlaceAction.h>
#include <openrct2/actions/LargeSceneryPlaceAction.h>
#include <openrct2/actions/LargeScenerySetColourAction.h>
#include <openrct2/actions/ScenerySetRestrictedAction.h>
#include <openrct2/actions/SmallSceneryPlaceAction.h>
#include <openrct2/actions/SmallScenerySetColourAction.h>
#include <openrct2/actions/WallPlaceAction.h>
#include <openrct2/actions/WallRemoveAction.h>
#include <openrct2/actions/WallSetColourAction.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/management/Research.h>
#include <openrct2/network/Network.h>
#include <openrct2/object/BannerSceneryEntry.h>
#include <openrct2/object/LargeSceneryEntry.h>
#include <openrct2/object/ObjectEntryManager.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/object/PathAdditionEntry.h>
#include <openrct2/object/SceneryGroupEntry.h>
#include <openrct2/object/SmallSceneryEntry.h>
#include <openrct2/object/WallSceneryEntry.h>
#include <openrct2/paint/VirtualFloor.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/ConstructionClearance.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/tile_element/BannerElement.h>
#include <openrct2/world/tile_element/LargeSceneryElement.h>
#include <openrct2/world/tile_element/PathElement.h>
#include <openrct2/world/tile_element/SmallSceneryElement.h>
#include <openrct2/world/tile_element/SurfaceElement.h>
#include <openrct2/world/tile_element/WallElement.h>

using namespace OpenRCT2::Drawing;
using OpenRCT2::GameActions::CommandFlag;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = kStringIdNone;
    static constexpr ScreenSize kMinimumWindowSize = { 634, 195 - kTitleHeightNormal };
    constexpr int32_t kSceneryButtonWidth = 66;
    constexpr int32_t kSceneryButtonHeight = 80;
    constexpr int32_t kDescriptionHeight = 24;
    constexpr int32_t kInputMargin = 2;
    constexpr int32_t kMaxWindowHeight = 473;
    constexpr int32_t kTabMargin = 3;
    constexpr int32_t kTabWidth = 31;
    constexpr int32_t kTabHeight = 28;
    constexpr int32_t kReservedTabCount = 2;
    constexpr int32_t kMaxTabs = 257; // 255 selected tabs + misc + all
    constexpr int32_t kMaxTabsPerRow = 20;

    constexpr uint8_t kSceneryContentScrollIndex = 0;

    enum WindowSceneryListWidgetIdx : WidgetIndex
    {
        WIDX_SCENERY_BACKGROUND,
        WIDX_SCENERY_TITLE,
        WIDX_SCENERY_CLOSE,
        WIDX_SCENERY_TAB_CONTENT_PANEL,
        WIDX_SCENERY_LIST,
        WIDX_SCENERY_ROTATE_OBJECTS_BUTTON,
        WIDX_SCENERY_REPAINT_SCENERY_BUTTON,
        WIDX_SCENERY_PRIMARY_COLOUR_BUTTON,
        WIDX_SCENERY_SECONDARY_COLOUR_BUTTON,
        WIDX_SCENERY_TERTIARY_COLOUR_BUTTON,
        WIDX_SCENERY_EYEDROPPER_BUTTON,
        WIDX_SCENERY_BUILD_CLUSTER_BUTTON,
        WIDX_FILTER_TEXT_BOX,
        WIDX_FILTER_CLEAR_BUTTON,
        WIDX_RESTRICT_SCENERY,
        WIDX_SCENERY_TAB_1,
    };

    VALIDATE_GLOBAL_WIDX(WC_SCENERY, WIDX_SCENERY_BACKGROUND);
    VALIDATE_GLOBAL_WIDX(WC_SCENERY, WIDX_SCENERY_TAB_1);
    VALIDATE_GLOBAL_WIDX(WC_SCENERY, WIDX_SCENERY_ROTATE_OBJECTS_BUTTON);
    VALIDATE_GLOBAL_WIDX(WC_SCENERY, WIDX_SCENERY_EYEDROPPER_BUTTON);

    // clang-format off
    static constexpr auto WindowSceneryBaseWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kMinimumWindowSize),
        makeWidget({  0,  43}, {634, 99}, WidgetType::resize,    WindowColour::secondary                                                           ), // 8         0x009DE2C8
        makeWidget({  2,  62}, {607, 80}, WidgetType::scroll,    WindowColour::secondary, SCROLL_VERTICAL                                          ), // 1000000   0x009DE418
        makeWidget({609,  59}, { 24, 24}, WidgetType::flatBtn,   WindowColour::secondary, ImageId(SPR_ROTATE_ARROW),    STR_ROTATE_OBJECTS_90      ), // 2000000   0x009DE428
        makeWidget({609,  83}, { 24, 24}, WidgetType::flatBtn,   WindowColour::secondary, ImageId(SPR_PAINTBRUSH),      STR_SCENERY_PAINTBRUSH_TIP ), // 4000000   0x009DE438
        makeWidget({615, 108}, { 12, 12}, WidgetType::colourBtn, WindowColour::secondary, 0xFFFFFFFF,                   STR_SELECT_COLOUR          ), // 8000000   0x009DE448
        makeWidget({615, 120}, { 12, 12}, WidgetType::colourBtn, WindowColour::secondary, 0xFFFFFFFF,                   STR_SELECT_SECONDARY_COLOUR), // 10000000  0x009DE458
        makeWidget({615, 132}, { 12, 12}, WidgetType::colourBtn, WindowColour::secondary, 0xFFFFFFFF,                   STR_SELECT_TERTIARY_COLOUR ), // 20000000  0x009DE468
        makeWidget({609, 145}, { 24, 24}, WidgetType::flatBtn,   WindowColour::secondary, ImageId(SPR_G2_EYEDROPPER),   STR_SCENERY_EYEDROPPER_TIP ), // 40000000  0x009DE478
        makeWidget({609, 169}, { 24, 24}, WidgetType::flatBtn,   WindowColour::secondary, ImageId(SPR_SCENERY_CLUSTER), STR_SCENERY_CLUSTER_TIP    ), // 40000000  0x009DE478
        makeWidget({  4,  46}, {211, 14}, WidgetType::textBox,   WindowColour::secondary                                                           ),
        makeWidget({218,  46}, { 70, 14}, WidgetType::button,    WindowColour::secondary, STR_OBJECT_SEARCH_CLEAR                                  ),
        makeWidget({539,  46}, { 70, 14}, WidgetType::button,    WindowColour::secondary, STR_RESTRICT_SCENERY,   STR_RESTRICT_SCENERY_TIP         )
    );
    // clang-format on

    // Persistent between window instances
    static size_t _activeTabIndex;
    static std::vector<ScenerySelection> _tabSelections;

    static bool _sceneryPaintEnabled;
    static Colour _sceneryPrimaryColour;
    static Colour _scenerySecondaryColour;
    static Colour _sceneryTertiaryColour;

    uint8_t gWindowSceneryRotation;
    bool gWindowSceneryEyedropperEnabled;

    class SceneryWindow final : public Window
    {
    private:
        enum SceneryTabType
        {
            SCENERY_TAB_TYPE_GROUP,
            SCENERY_TAB_TYPE_MISC,
            SCENERY_TAB_TYPE_ALL,
        };

        struct SceneryItem
        {
            int32_t allRows;
            int32_t selected_item;
            ScenerySelection scenerySelection;
        };

        struct SceneryTabInfo
        {
            SceneryTabType Type = SCENERY_TAB_TYPE_GROUP;
            ObjectEntryIndex SceneryGroupIndex = kObjectEntryIndexNull;
            std::deque<ScenerySelection> Entries{};
            u8string Filter = "";

            bool IsMisc() const
            {
                return Type == SCENERY_TAB_TYPE_MISC;
            }

            bool IsAll() const
            {
                return Type == SCENERY_TAB_TYPE_ALL;
            }

            bool IsSceneryGroup() const
            {
                return Type == SCENERY_TAB_TYPE_GROUP;
            }

            bool Contains(const ScenerySelection& entry) const
            {
                return std::find(std::begin(Entries), std::end(Entries), entry) != std::end(Entries);
            }

            void AddEntryToBack(const ScenerySelection& entry)
            {
                if (!Contains(entry))
                {
                    Entries.push_back(entry);
                }
            }

            void AddEntryToFront(const ScenerySelection& entry)
            {
                if (!Contains(entry))
                {
                    Entries.push_front(entry);
                }
            }

            const SceneryGroupEntry* GetSceneryGroupEntry() const
            {
                return ObjectManager::GetObjectEntry<SceneryGroupEntry>(SceneryGroupIndex);
            }
        };

        struct ProvisionalWallTile
        {
            CoordsXYZD location;
            int32_t calculatedZ;
        };

        std::vector<SceneryTabInfo> _tabEntries;
        int32_t _requiredWidth;
        int32_t _actualMinHeight;
        ScenerySelection _selectedScenery;
        int16_t _hoverCounter;
        SceneryTabInfo _filteredSceneryTab;

        uint8_t _unkF64F0E{ 0 };
        int16_t _unkF64F0A{ 0 };

        CoordsXY _dragStartPos{};
        /**
         * When placing fences in the air using Shift, the user may shift their mouse to the left or right.
         * We save this position to avoid detecting a wall drag where there isn’t one.
         */
        CoordsXY _dragStartHoverPos{};
        CoordsXY _dragEndPos{};
        uint8_t _startEdge{};
        GameActions::Result _lastProvisionalError{};
        bool _inDragMode = false;
        std::vector<ProvisionalWallTile> _provisionalTiles{};

    public:
        void onOpen() override
        {
            init();

            initScrollWidgets();
            ContentUpdateScroll();
            ShowGridlines();
            gWindowSceneryRotation = 3;
            gSceneryCtrlPressed = false;
            gSceneryShiftPressed = false;
            _selectedScenery = {};
            _hoverCounter = 0;
            gSceneryGhostType = 0;
            gSceneryPlaceCost = kMoney64Undefined;
            gSceneryPlaceRotation = 0;
            _sceneryPaintEnabled = false; // repaint coloured scenery tool state
            gWindowSceneryEyedropperEnabled = false;

            _actualMinHeight = GetMinimumHeight();

            width = GetRequiredWidth();
            minWidth = width;
            maxWidth = width;
            height = _actualMinHeight;
            minHeight = height;
            maxHeight = height;
            if (_activeTabIndex > _tabSelections.size())
            {
                _activeTabIndex = 0;
            }

            WindowMovePosition(*this, { ContextGetWidth() - GetRequiredWidth(), 0x1D });
            WindowPushOthersBelow(*this);
        }

        void onClose() override
        {
            SceneryRemoveGhostToolPlacement();
            removeProvisionalTilesFromMap();
            HideGridlines();
            ViewportSetVisibility(ViewportVisibility::standard);

            if (gWindowSceneryScatterEnabled)
            {
                auto* windowMgr = GetWindowManager();
                windowMgr->CloseByClass(WindowClass::sceneryScatter);
            }

            if (isToolActive(WindowClass::scenery))
                ToolCancel();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            auto* windowMgr = GetWindowManager();

            switch (widgetIndex)
            {
                case WIDX_SCENERY_CLOSE:
                    close();
                    break;
                case WIDX_SCENERY_ROTATE_OBJECTS_BUTTON:
                    gWindowSceneryRotation++;
                    gWindowSceneryRotation = gWindowSceneryRotation % 4;
                    SceneryRemoveGhostToolPlacement();
                    invalidate();
                    break;
                case WIDX_SCENERY_REPAINT_SCENERY_BUTTON:
                    _sceneryPaintEnabled ^= true;
                    gWindowSceneryEyedropperEnabled = false;
                    if (gWindowSceneryScatterEnabled)
                        windowMgr->CloseByClass(WindowClass::sceneryScatter);
                    invalidate();
                    break;
                case WIDX_SCENERY_EYEDROPPER_BUTTON:
                    _sceneryPaintEnabled = false;
                    gWindowSceneryEyedropperEnabled = !gWindowSceneryEyedropperEnabled;
                    if (gWindowSceneryScatterEnabled)
                        windowMgr->CloseByClass(WindowClass::sceneryScatter);
                    SceneryRemoveGhostToolPlacement();
                    invalidate();
                    break;
                case WIDX_SCENERY_BUILD_CLUSTER_BUTTON:
                    _sceneryPaintEnabled = false;
                    gWindowSceneryEyedropperEnabled = false;
                    if (gWindowSceneryScatterEnabled)
                        windowMgr->CloseByClass(WindowClass::sceneryScatter);
                    else if (
                        Network::GetMode() != Network::Mode::client
                        || Network::CanPerformCommand(Network::GetCurrentPlayerGroupIndex(), -2))
                    {
                        SceneryScatterOpen();
                    }
                    else
                    {
                        ContextShowError(STR_CANT_DO_THIS, STR_PERMISSION_DENIED, {});
                    }
                    invalidate();
                    break;
                case WIDX_FILTER_TEXT_BOX:
                    WindowStartTextbox(*this, widgetIndex, _filteredSceneryTab.Filter, kTextInputSize);
                    break;
                case WIDX_FILTER_CLEAR_BUTTON:
                    _tabEntries[_activeTabIndex].Filter.clear();
                    ContentUpdateScroll();
                    scrolls->contentOffsetY = 0;
                    invalidate();
                    break;
                case WIDX_RESTRICT_SCENERY:
                {
                    const auto tabIndex = _activeTabIndex;
                    const auto tabSelectedScenery = GetSelectedScenery(tabIndex);
                    if (!tabSelectedScenery.IsUndefined())
                    {
                        const auto newStatus = !IsSceneryItemRestricted(tabSelectedScenery);
                        const auto objectType = GetObjectTypeFromSceneryType(tabSelectedScenery.SceneryType);
                        auto action = GameActions::ScenerySetRestrictedAction(
                            objectType, tabSelectedScenery.EntryIndex, newStatus);
                        GameActions::Execute(&action, getGameState());
                    }
                    break;
                }
            }
        }

        void onResize() override
        {
            if (width < minWidth)
            {
                invalidate();
                width = minWidth;
                invalidate();
            }

            if (width > maxWidth)
            {
                invalidate();
                width = maxWidth;
                invalidate();
            }

            if (height < minHeight)
            {
                invalidate();
                height = minHeight;
                invalidate();
                // HACK: For some reason invalidate has not been called
                onPrepareDraw();
                ContentUpdateScroll();
            }

            if (height > maxHeight)
            {
                invalidate();
                height = maxHeight;
                invalidate();
                // HACK: For some reason invalidate has not been called
                onPrepareDraw();
                ContentUpdateScroll();
            }

            resizeFrame();
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_SCENERY_PRIMARY_COLOUR_BUTTON:
                    WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], _sceneryPrimaryColour);
                    break;
                case WIDX_SCENERY_SECONDARY_COLOUR_BUTTON:
                    WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], _scenerySecondaryColour);
                    break;
                case WIDX_SCENERY_TERTIARY_COLOUR_BUTTON:
                    WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], _sceneryTertiaryColour);
                    break;
            }

            if (widgetIndex >= WIDX_SCENERY_TAB_1)
            {
                _activeTabIndex = widgetIndex - WIDX_SCENERY_TAB_1;
                invalidate();
                gSceneryPlaceCost = kMoney64Undefined;

                ContentUpdateScroll();
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (dropdownIndex == -1)
                return;

            if (widgetIndex == WIDX_SCENERY_PRIMARY_COLOUR_BUTTON)
            {
                _sceneryPrimaryColour = ColourDropDownIndexToColour(dropdownIndex);
            }
            else if (widgetIndex == WIDX_SCENERY_SECONDARY_COLOUR_BUTTON)
            {
                _scenerySecondaryColour = ColourDropDownIndexToColour(dropdownIndex);
            }
            else if (widgetIndex == WIDX_SCENERY_TERTIARY_COLOUR_BUTTON)
            {
                _sceneryTertiaryColour = ColourDropDownIndexToColour(dropdownIndex);
            }

            invalidate();
        }

        void onPeriodicUpdate() override
        {
            if (!_selectedScenery.IsUndefined())
            {
                // Find out what scenery the cursor is over
                const CursorState* state = ContextGetCursorState();

                auto* windowMgr = GetWindowManager();
                WidgetIndex widgetIndex = windowMgr->FindWidgetFromPoint(*this, state->position);
                if (widgetIndex == WIDX_SCENERY_LIST)
                {
                    ScreenCoordsXY scrollPos = {};
                    int32_t scrollArea = 0;
                    int32_t scrollId = 0;
                    widgetScrollGetPart(*this, &widgets[WIDX_SCENERY_LIST], state->position, scrollPos, &scrollArea, &scrollId);
                    if (scrollArea == SCROLL_PART_VIEW)
                    {
                        const ScenerySelection scenery = GetSceneryIdByCursorPos(scrollPos);
                        if (scenery == _selectedScenery)
                        {
                            return;
                        }
                    }
                }

                // Cursor was not over the currently hover selected scenery so reset hover selection.
                // This will happen when the mouse leaves the scroll window and is required so that the cost and description
                // switch to the tool scenery selection.
                _selectedScenery = {};
                invalidate();
            }
        }

        void onUpdate() override
        {
            const CursorState* state = ContextGetCursorState();

            auto* windowMgr = GetWindowManager();
            WindowBase* other = windowMgr->FindFromPoint(state->position);
            if (other == this)
            {
                ScreenCoordsXY window = state->position - ScreenCoordsXY{ windowPos.x - 26, windowPos.y };

                if (window.y < 44 || window.x <= width)
                {
                    WidgetIndex widgetIndex = windowMgr->FindWidgetFromPoint(*this, state->position);
                    if (widgetIndex >= WIDX_SCENERY_TAB_CONTENT_PANEL)
                    {
                        _hoverCounter++;
                        if (_hoverCounter < 8)
                        {
                            if (InputGetState() != InputState::ScrollLeft)
                            {
                                minHeight = _actualMinHeight;
                                maxHeight = _actualMinHeight;
                            }
                        }
                        else
                        {
                            const auto& listWidget = widgets[WIDX_SCENERY_LIST];
                            const auto nonListHeight = height - listWidget.height() - 1 + 2;

                            const auto numRows = static_cast<int32_t>(CountRows());
                            const auto maxContentHeight = numRows * kSceneryButtonHeight;
                            const auto expandedWindowHeight = maxContentHeight + nonListHeight;
                            const auto windowHeight = std::clamp(expandedWindowHeight, _actualMinHeight, kMaxWindowHeight);

                            minHeight = windowHeight;
                            maxHeight = windowHeight;

                            if (height < minHeight)
                            {
                                height = minHeight;
                                onPrepareDraw();
                                ContentUpdateScroll();
                                invalidate();
                            }
                        }
                    }
                }
            }
            else
            {
                _hoverCounter = 0;
                if (InputGetState() != InputState::ScrollLeft)
                {
                    minHeight = _actualMinHeight;
                    maxHeight = _actualMinHeight;
                }
            }

            if (height > maxHeight)
            {
                invalidate();
                height = maxHeight;
                onPrepareDraw();
                ContentUpdateScroll();
            }

            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                invalidateWidget(WIDX_FILTER_TEXT_BOX);
            }

            if (!isToolActive(WindowClass::scenery))
            {
                if (_inDragMode)
                {
                    removeProvisionalTilesFromMap();
                    _provisionalTiles.clear();
                    _inDragMode = false;

                    ToolSet(*this, WIDX_SCENERY_BACKGROUND, Tool::arrow);
                    gInputFlags.set(InputFlag::allowRightMouseRemoval);
                }
                else
                {
                    close();
                    return;
                }
            }

            if (gWindowSceneryEyedropperEnabled)
            {
                gCurrentToolId = Tool::crosshair;
            }
            else if (_sceneryPaintEnabled)
            {
                gCurrentToolId = Tool::paintDown;
            }
            else
            {
                const auto tabIndex = _activeTabIndex;
                const auto tabSelectedScenery = GetSelectedScenery(tabIndex);
                if (!tabSelectedScenery.IsUndefined())
                {
                    if (tabSelectedScenery.SceneryType == SCENERY_TYPE_BANNER)
                    {
                        gCurrentToolId = Tool::entranceDown;
                    }
                    else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_LARGE)
                    {
                        const auto* sceneryEntry = ObjectManager::GetObjectEntry<LargeSceneryEntry>(
                            tabSelectedScenery.EntryIndex);
                        if (sceneryEntry != nullptr)
                        {
                            gCurrentToolId = static_cast<Tool>(sceneryEntry->tool_id);
                        }
                    }
                    else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_WALL)
                    {
                        const auto* sceneryEntry = ObjectManager::GetObjectEntry<WallSceneryEntry>(
                            tabSelectedScenery.EntryIndex);
                        if (sceneryEntry != nullptr)
                        {
                            gCurrentToolId = static_cast<Tool>(sceneryEntry->tool_id);
                        }
                    }
                    else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_PATH_ITEM)
                    {
                        const auto* sceneryEntry = ObjectManager::GetObjectEntry<PathAdditionEntry>(
                            tabSelectedScenery.EntryIndex);
                        if (sceneryEntry != nullptr)
                        {
                            gCurrentToolId = static_cast<Tool>(sceneryEntry->tool_id);
                        }
                    }
                    else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_SMALL)
                    {
                        const auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(
                            tabSelectedScenery.EntryIndex);
                        if (sceneryEntry != nullptr)
                        {
                            gCurrentToolId = static_cast<Tool>(sceneryEntry->tool_id);
                        }
                    }
                }
                else
                {
                    gCurrentToolId = Tool::arrow;
                }
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (widgetIndex != WIDX_FILTER_TEXT_BOX)
                return;

            if (text == _tabEntries[_activeTabIndex].Filter)
                return;

            _tabEntries[_activeTabIndex].Filter.assign(text);
            ContentUpdateScroll();

            scrolls->contentOffsetY = 0;
            invalidate();
        }

        OpenRCT2String onTooltip(const WidgetIndex widgetIndex, const StringId fallback) override
        {
            if (widgetIndex >= WIDX_SCENERY_TAB_1)
            {
                const auto tabIndex = static_cast<size_t>(widgetIndex - WIDX_SCENERY_TAB_1);
                if (_tabEntries.size() > tabIndex)
                {
                    const auto& tabInfo = _tabEntries[tabIndex];
                    if (tabInfo.IsMisc())
                    {
                        auto ft = Formatter();
                        ft.Add<StringId>(STR_MISCELLANEOUS);
                        return { fallback, ft };
                    }

                    if (tabInfo.IsAll())
                    {
                        auto ft = Formatter();
                        ft.Add<StringId>(STR_ALL_SCENERY);
                        return { fallback, ft };
                    }

                    const auto* sceneryEntry = tabInfo.GetSceneryGroupEntry();
                    if (sceneryEntry != nullptr)
                    {
                        auto ft = Formatter();
                        ft.Add<StringId>(sceneryEntry->name);
                        return { fallback, ft };
                    }
                }
            }
            return { kStringIdNone, Formatter() };
        }

        void onPrepareDraw() override
        {
            _actualMinHeight = GetMinimumHeight();

            // Set the window title
            StringId titleStringId = STR_MISCELLANEOUS;
            const auto tabIndex = _activeTabIndex;
            if (tabIndex < _tabEntries.size())
            {
                const auto& tabInfo = _tabEntries[tabIndex];
                if (tabInfo.IsAll())
                {
                    titleStringId = STR_ALL_SCENERY;
                }
                else
                {
                    const auto* sgEntry = tabInfo.GetSceneryGroupEntry();
                    if (sgEntry != nullptr)
                    {
                        titleStringId = sgEntry->name;
                    }
                }
            }
            widgets[WIDX_SCENERY_TITLE].text = titleStringId;
            widgets[WIDX_FILTER_TEXT_BOX].string = _filteredSceneryTab.Filter.data();

            pressedWidgets = 0;
            pressedWidgets |= 1uLL << (tabIndex + WIDX_SCENERY_TAB_1);
            if (_sceneryPaintEnabled)
                pressedWidgets |= (1uLL << WIDX_SCENERY_REPAINT_SCENERY_BUTTON);
            if (gWindowSceneryEyedropperEnabled)
                pressedWidgets |= (1uLL << WIDX_SCENERY_EYEDROPPER_BUTTON);
            if (gWindowSceneryScatterEnabled)
                pressedWidgets |= (1uLL << WIDX_SCENERY_BUILD_CLUSTER_BUTTON);

            widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WidgetType::empty;
            widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WidgetType::empty;
            widgets[WIDX_RESTRICT_SCENERY].type = WidgetType::empty;

            const auto tabSelectedScenery = GetSelectedScenery(tabIndex);
            if (!tabSelectedScenery.IsUndefined())
            {
                if (tabSelectedScenery.SceneryType == SCENERY_TYPE_SMALL)
                {
                    widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WidgetType::flatBtn;

                    auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(tabSelectedScenery.EntryIndex);
                    if (sceneryEntry != nullptr && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ROTATABLE))
                    {
                        widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WidgetType::flatBtn;
                    }
                }
                else if (tabSelectedScenery.SceneryType >= SCENERY_TYPE_LARGE)
                {
                    widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WidgetType::flatBtn;
                }

                if (gLegacyScene == LegacyScene::scenarioEditor || getGameState().cheats.sandboxMode)
                {
                    widgets[WIDX_RESTRICT_SCENERY].type = WidgetType::button;
                    if (IsSceneryItemRestricted(tabSelectedScenery))
                        pressedWidgets |= (1uLL << WIDX_RESTRICT_SCENERY);
                    else
                        pressedWidgets &= ~(1uLL << WIDX_RESTRICT_SCENERY);
                }
            }

            widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].image = getColourButtonImage(_sceneryPrimaryColour);
            widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].image = getColourButtonImage(_scenerySecondaryColour);
            widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].image = getColourButtonImage(_sceneryTertiaryColour);

            widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WidgetType::empty;
            widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WidgetType::empty;
            widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WidgetType::empty;

            if (_sceneryPaintEnabled)
            { // repaint coloured scenery tool is on
                widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WidgetType::colourBtn;
                widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WidgetType::colourBtn;
                widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WidgetType::colourBtn;
            }
            else if (!tabSelectedScenery.IsUndefined())
            {
                if (tabSelectedScenery.SceneryType == SCENERY_TYPE_BANNER)
                {
                    auto* bannerEntry = ObjectManager::GetObjectEntry<BannerSceneryEntry>(tabSelectedScenery.EntryIndex);
                    if (bannerEntry != nullptr && bannerEntry->flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR)
                    {
                        widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WidgetType::colourBtn;
                    }
                }
                else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_LARGE)
                {
                    auto* sceneryEntry = ObjectManager::GetObjectEntry<LargeSceneryEntry>(tabSelectedScenery.EntryIndex);
                    if (sceneryEntry != nullptr)
                    {
                        if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR
                            && !(sceneryEntry->flags & LARGE_SCENERY_FLAG_HIDE_PRIMARY_REMAP_BUTTON))
                            widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WidgetType::colourBtn;
                        if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR
                            && !(sceneryEntry->flags & LARGE_SCENERY_FLAG_HIDE_SECONDARY_REMAP_BUTTON))
                            widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WidgetType::colourBtn;
                        if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
                            widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WidgetType::colourBtn;
                    }
                }
                else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_WALL)
                {
                    auto* wallEntry = ObjectManager::GetObjectEntry<WallSceneryEntry>(tabSelectedScenery.EntryIndex);
                    if (wallEntry != nullptr && wallEntry->flags & (WALL_SCENERY_HAS_PRIMARY_COLOUR | WALL_SCENERY_HAS_GLASS))
                    {
                        widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WidgetType::colourBtn;

                        if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                        {
                            widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WidgetType::colourBtn;

                            if (wallEntry->flags2 & WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR)
                                widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WidgetType::empty;
                            if (wallEntry->flags & WALL_SCENERY_HAS_TERTIARY_COLOUR)
                                widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WidgetType::colourBtn;
                        }
                    }
                }
                else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_SMALL)
                {
                    auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(tabSelectedScenery.EntryIndex);

                    if (sceneryEntry != nullptr
                        && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR | SMALL_SCENERY_FLAG_HAS_GLASS))
                    {
                        widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WidgetType::colourBtn;

                        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
                            widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WidgetType::colourBtn;
                        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR))
                            widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WidgetType::colourBtn;
                    }
                }
            }

            auto windowWidth = width;
            if (_tabEntries.size() > 0)
            {
                const auto lastTabIndex = GetMaxTabCountInARow() == kMaxTabsPerRow ? kMaxTabsPerRow - 1
                                                                                   : _tabEntries.size() - 1;
                const auto lastTabWidget = &widgets[WIDX_SCENERY_TAB_1 + lastTabIndex];
                windowWidth = std::max<int32_t>(windowWidth, lastTabWidget->right + 3);

                auto tabTop = widgets[WIDX_SCENERY_TITLE].bottom + kTabMargin;
                if (GetSceneryTabInfoForMisc() != nullptr)
                {
                    auto miscTabWidget = &widgets[WIDX_SCENERY_TAB_1 + _tabEntries.size() - 2];
                    miscTabWidget->left = windowWidth - 2 * kTabWidth - 6;
                    miscTabWidget->right = windowWidth - kTabWidth - 7;
                    miscTabWidget->top = tabTop;
                    miscTabWidget->bottom = tabTop + kTabHeight;
                }

                if (_tabEntries.back().IsAll())
                {
                    auto allTabWidget = &widgets[WIDX_SCENERY_TAB_1 + _tabEntries.size() - 1];
                    allTabWidget->left = windowWidth - kTabWidth - 6;
                    allTabWidget->right = windowWidth - 7;
                    allTabWidget->top = tabTop;
                    allTabWidget->bottom = tabTop + kTabHeight;
                }
            }

            resizeFrame();
            widgets[WIDX_SCENERY_LIST].right = windowWidth - 26;
            widgets[WIDX_SCENERY_LIST].bottom = height - kDescriptionHeight;

            widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].left = windowWidth - 25;
            widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].left = windowWidth - 25;
            widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].left = windowWidth - 25;
            widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].left = windowWidth - 25;
            widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].right = windowWidth - 2;
            widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].right = windowWidth - 2;
            widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].right = windowWidth - 2;
            widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].right = windowWidth - 2;

            widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].left = windowWidth - 19;
            widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].left = windowWidth - 19;
            widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].left = windowWidth - 19;
            widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].right = windowWidth - 8;
            widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].right = windowWidth - 8;
            widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].right = windowWidth - 8;

            const bool canFit = widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].top < height;
            widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].type = canFit ? WidgetType::flatBtn : WidgetType::empty;
            widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = canFit ? WidgetType::flatBtn : WidgetType::empty;
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
            DrawTabs(rt, windowPos);

            auto selectedSceneryEntry = _selectedScenery;
            if (selectedSceneryEntry.IsUndefined())
            {
                if (_sceneryPaintEnabled) // repaint coloured scenery tool is on
                    return;
                if (gWindowSceneryEyedropperEnabled)
                    return;

                selectedSceneryEntry = GetSelectedScenery(_activeTabIndex);
                if (selectedSceneryEntry.IsUndefined())
                    return;
            }

            auto [name, price] = GetNameAndPrice(selectedSceneryEntry);
            if (price != kMoney64Undefined && !(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
            {
                auto ft = Formatter();
                ft.Add<money64>(price);

                // -14
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ width - 0x1A, height - 13 }, STR_COST_LABEL, ft, { TextAlignment::right });
            }

            auto ft = Formatter();
            ft.Add<StringId>(name);
            DrawTextEllipsised(rt, { windowPos.x + 3, windowPos.y + height - 23 }, width - 19, STR_BLACK_STRING, ft);

            // Draw object author(s) if debugging tools are active
            if (Config::Get().general.debuggingTools)
            {
                auto sceneryObjectType = GetObjectTypeFromSceneryType(selectedSceneryEntry.SceneryType);
                auto& objManager = GetContext()->GetObjectManager();
                auto sceneryObject = objManager.GetLoadedObject(sceneryObjectType, selectedSceneryEntry.EntryIndex);
                if (sceneryObject != nullptr && sceneryObject->GetAuthors().size() > 0)
                {
                    std::string authorsString;
                    const auto& authors = sceneryObject->GetAuthors();
                    for (size_t i = 0; i < authors.size(); ++i)
                    {
                        if (i > 0)
                        {
                            authorsString.append(", ");
                        }
                        authorsString.append(authors[i]);
                    }
                    ft = Formatter();
                    ft.Add<const char*>(authorsString.c_str());
                    DrawTextEllipsised(
                        rt, windowPos + ScreenCoordsXY{ 3, height - 13 }, width - 19,
                        (sceneryObject->GetAuthors().size() == 1 ? STR_SCENERY_AUTHOR : STR_SCENERY_AUTHOR_PLURAL), ft);
                }
            }
        }

        void setSelectedItem(
            const ScenerySelection& scenery, const std::optional<Colour> primary, const std::optional<Colour> secondary,
            const std::optional<Colour> tertiary, const std::optional<uint8_t> rotation)
        {
            auto tabIndex = FindTabWithScenery(scenery);
            if (!tabIndex.has_value())
            {
                return;
            }

            _activeTabIndex = tabIndex.value();
            SetSelectedScenery(tabIndex.value(), scenery);
            if (primary.has_value())
            {
                _sceneryPrimaryColour = primary.value();
            }
            if (secondary.has_value())
            {
                _scenerySecondaryColour = secondary.value();
            }
            if (tertiary.has_value())
            {
                _sceneryTertiaryColour = tertiary.value();
            }
            if (rotation.has_value())
            {
                gWindowSceneryRotation = rotation.value();
            }
            gWindowSceneryEyedropperEnabled = false;
            Audio::Play(Audio::SoundId::click1, 0, ContextGetWidth() / 2);
            _hoverCounter = -16;
            gSceneryPlaceCost = kMoney64Undefined;
            invalidate();
        }

        void setSelectedTab(const ObjectEntryIndex sceneryGroupIndex)
        {
            const auto* tabInfo = GetSceneryTabInfoForGroup(sceneryGroupIndex);
            if (tabInfo == nullptr)
            {
                tabInfo = &_tabEntries.back();
            }
            const auto tabId = std::distance(&*_tabEntries.cbegin(), tabInfo);

            onMouseDown(WIDX_SCENERY_TAB_1 + tabId);
        }

        const ScenerySelection getTabSelection()
        {
            return GetSelectedScenery(_activeTabIndex);
        }

        void init()
        {
            _tabEntries.clear();

            for (ObjectEntryIndex scenerySetIndex = 0; scenerySetIndex < kMaxTabs - kReservedTabCount; scenerySetIndex++)
            {
                const auto* sceneryGroupEntry = ObjectManager::GetObjectEntry<SceneryGroupEntry>(scenerySetIndex);
                if (sceneryGroupEntry != nullptr && SceneryGroupIsInvented(scenerySetIndex))
                {
                    SceneryTabInfo tabInfo;
                    tabInfo.SceneryGroupIndex = scenerySetIndex;
                    for (const auto& sceneryEntry : sceneryGroupEntry->SceneryEntries)
                    {
                        if (IsSceneryAvailableToBuild(sceneryEntry))
                        {
                            tabInfo.AddEntryToBack(sceneryEntry);
                        }
                    }
                    if (tabInfo.Entries.size() > 0)
                    {
                        _tabEntries.push_back(std::move(tabInfo));
                    }
                }
            }

            // Sort scenery group tabs before adding other tabs
            SortTabs();

            // Add misc and all tab
            _tabEntries.emplace_back(SceneryWindow::SceneryTabInfo{ SCENERY_TAB_TYPE_MISC });
            _tabEntries.emplace_back(SceneryWindow::SceneryTabInfo{ SCENERY_TAB_TYPE_ALL });

            // small scenery
            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxSmallSceneryObjects; sceneryId++)
            {
                const auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    initSceneryEntry({ SCENERY_TYPE_SMALL, sceneryId }, sceneryEntry->scenery_tab_id);
                }
            }

            // large scenery
            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxLargeSceneryObjects; sceneryId++)
            {
                const auto* sceneryEntry = ObjectManager::GetObjectEntry<LargeSceneryEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    initSceneryEntry({ SCENERY_TYPE_LARGE, sceneryId }, sceneryEntry->scenery_tab_id);
                }
            }

            // walls
            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxWallSceneryObjects; sceneryId++)
            {
                const auto* sceneryEntry = ObjectManager::GetObjectEntry<WallSceneryEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    initSceneryEntry({ SCENERY_TYPE_WALL, sceneryId }, sceneryEntry->scenery_tab_id);
                }
            }

            // banners
            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxBannerObjects; sceneryId++)
            {
                const auto* sceneryEntry = ObjectManager::GetObjectEntry<BannerSceneryEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    initSceneryEntry({ SCENERY_TYPE_BANNER, sceneryId }, sceneryEntry->scenery_tab_id);
                }
            }

            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxPathAdditionObjects; sceneryId++)
            {
                const auto* sceneryEntry = ObjectManager::GetObjectEntry<PathAdditionEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    initSceneryEntry({ SCENERY_TYPE_PATH_ITEM, sceneryId }, sceneryEntry->scenery_tab_id);
                }
            }

            // Remove empty tabs
            _tabEntries.erase(
                std::remove_if(
                    _tabEntries.begin(), _tabEntries.end(),
                    [](const SceneryTabInfo& tabInfo) { return tabInfo.Entries.empty(); }),
                _tabEntries.end());

            // Set required width
            _requiredWidth = std::min(static_cast<int32_t>(_tabEntries.size()), kMaxTabsPerRow) * kTabWidth + 5;

            PrepareWidgets();

            auto* windowMgr = GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::scenery);
        }

        int32_t GetRequiredWidth() const
        {
            return std::max(_requiredWidth, kMinimumWindowSize.width);
        }

    private:
        int32_t GetMinimumHeight() const
        {
            // Minimum window height: title, status bar, padding
            int32_t newMinHeight = getTitleBarTargetHeight() + kDescriptionHeight + 2 * kTabMargin;

            // Room for the search/filter widget
            newMinHeight += widgets[WIDX_FILTER_TEXT_BOX].height() - 1 + 2 * kInputMargin;

            // Fit *all* rows of tabs
            newMinHeight += static_cast<int32_t>(1 + (_tabEntries.size() / GetMaxTabCountInARow())) * kTabHeight;

            // And at least 1.5 rows of scenery items
            newMinHeight += 1.5 * kSceneryButtonHeight;

            return newMinHeight;
        }

        int32_t GetNumColumns() const
        {
            const auto& listWidget = widgets[WIDX_SCENERY_LIST];
            const auto contentWidth = listWidget.width() - 1 - kScrollBarWidth;
            return contentWidth / kSceneryButtonWidth;
        }

        template<typename T>
        T CountRows(T items) const
        {
            const auto rows = items / GetNumColumns();
            return rows;
        }

        size_t CountRows() const
        {
            const auto tabIndex = _activeTabIndex;
            if (tabIndex >= _tabEntries.size())
            {
                return 0;
            }

            const auto totalItems = _filteredSceneryTab.Entries.size();
            const auto numColumns = GetNumColumns();
            const auto rows = CountRows(totalItems + numColumns - 1);
            return rows;
        }

        int32_t constexpr ContentRowsHeight(const size_t rows) const
        {
            return static_cast<int32_t>(rows * kSceneryButtonHeight);
        }

        void ContentUpdateScroll()
        {
            const auto tabIndex = _activeTabIndex;
            if (tabIndex >= _tabEntries.size())
            {
                return;
            }

            SetFilteredScenery(tabIndex);

            const int32_t listHeight = height - 14 - widgets[WIDX_SCENERY_LIST].top - 1;

            const auto sceneryItem = ContentCountRowsWithSelectedItem(tabIndex);
            scrolls[kSceneryContentScrollIndex].contentHeight = ContentRowsHeight(sceneryItem.allRows) + 1;

            const int32_t maxTop = std::max(0, scrolls[kSceneryContentScrollIndex].contentHeight - listHeight);
            auto rowSelected = CountRows(sceneryItem.selected_item);
            if (sceneryItem.scenerySelection.IsUndefined())
            {
                auto& oldScenery = GetSelectedScenery(tabIndex);
                if (!oldScenery.IsUndefined())
                {
                    // Make sure last selected scenery for tab is always present
                    _filteredSceneryTab.AddEntryToFront(oldScenery);
                }
                else
                {
                    // No last selection was made, use blank ScenerySelection
                    SetSelectedScenery(tabIndex, ScenerySelection());
                    rowSelected = 0;
                    if (!_filteredSceneryTab.Entries.empty())
                    {
                        // Select first item in current filtered tab if available
                        const auto& scenery = _filteredSceneryTab.Entries.front();
                        if (!scenery.IsUndefined())
                        {
                            SetSelectedScenery(tabIndex, scenery);
                        }
                    }
                }
            }

            scrolls[kSceneryContentScrollIndex].contentOffsetY = ContentRowsHeight(rowSelected);
            scrolls[kSceneryContentScrollIndex].contentOffsetY = std::min<int32_t>(
                maxTop, scrolls[kSceneryContentScrollIndex].contentOffsetY);

            widgetScrollUpdateThumbs(*this, WIDX_SCENERY_LIST);
        }

        SceneryItem ContentCountRowsWithSelectedItem(const size_t tabIndex)
        {
            SceneryItem sceneryItem = { 0, 0, ScenerySelection() };
            const auto scenerySelection = GetSelectedScenery(tabIndex);
            for (size_t i = 0; i < _filteredSceneryTab.Entries.size(); i++)
            {
                const auto& currentEntry = _filteredSceneryTab.Entries[i];
                if (currentEntry == scenerySelection)
                {
                    sceneryItem.selected_item = static_cast<int32_t>(i);
                    sceneryItem.scenerySelection = scenerySelection;
                }
            }
            sceneryItem.allRows = static_cast<int32_t>(CountRows(_filteredSceneryTab.Entries.size() + 8));
            return sceneryItem;
        }

        const ScenerySelection GetSelectedScenery(const size_t tabIndex)
        {
            if (_tabSelections.size() > tabIndex)
            {
                return _tabSelections[tabIndex];
            }
            return {};
        }

        void SetSelectedScenery(const size_t tabIndex, const ScenerySelection& value)
        {
            if (_tabSelections.size() <= tabIndex)
            {
                _tabSelections.resize(tabIndex + 1);
            }
            _tabSelections[tabIndex] = value;
        }

        SceneryTabInfo* GetSceneryTabInfoForMisc()
        {
            if (_tabEntries.size() >= 2)
            {
                if (_tabEntries[_tabEntries.size() - 2].IsMisc())
                    return &_tabEntries[_tabEntries.size() - 2];
            }

            return nullptr;
        }

        SceneryTabInfo* GetSceneryTabInfoForAll()
        {
            if (!_tabEntries.empty())
            {
                if (_tabEntries.back().IsAll())
                    return &_tabEntries.back();
            }

            return nullptr;
        }

        SceneryTabInfo* GetSceneryTabInfoForGroup(const ObjectEntryIndex sceneryGroupIndex)
        {
            for (auto& tabEntry : _tabEntries)
            {
                if (tabEntry.SceneryGroupIndex == sceneryGroupIndex)
                    return &tabEntry;
            }

            return nullptr;
        }

        std::optional<size_t> FindTabWithScenery(const ScenerySelection& scenery)
        {
            for (size_t i = 0; i < _tabEntries.size(); i++)
            {
                const auto& tabInfo = _tabEntries[i];
                if (tabInfo.IsAll())
                {
                    // The scenery will be always added here so exclude this one.
                    continue;
                }
                if (tabInfo.Contains(scenery))
                {
                    return i;
                }
            }
            return std::nullopt;
        }

        void initSceneryEntry(const ScenerySelection& selection, const ObjectEntryIndex sceneryGroupIndex)
        {
            Guard::ArgumentInRange<int32_t>(selection.EntryIndex, 0, kObjectEntryIndexNull);

            if (IsSceneryAvailableToBuild(selection))
            {
                // Add scenery to all tab
                auto* allTabInfo = GetSceneryTabInfoForAll();
                if (allTabInfo != nullptr)
                {
                    allTabInfo->AddEntryToBack(selection);
                }

                // Add scenery to primary group (usually trees or path additions)
                if (sceneryGroupIndex != kObjectEntryIndexNull)
                {
                    auto* tabInfo = GetSceneryTabInfoForGroup(sceneryGroupIndex);
                    if (tabInfo != nullptr)
                    {
                        tabInfo->AddEntryToBack(selection);
                        return;
                    }
                }

                // If scenery has no tab, add it to misc
                const auto tabIndex = FindTabWithScenery(selection);
                if (!tabIndex.has_value())
                {
                    auto* tabInfo = GetSceneryTabInfoForMisc();
                    if (tabInfo != nullptr)
                    {
                        tabInfo->AddEntryToBack(selection);
                    }
                }
            }
        }

        void SetFilteredScenery(const size_t tabIndex)
        {
            auto currentTab = _tabEntries[tabIndex];

            _filteredSceneryTab.Entries.clear();
            _filteredSceneryTab.Filter = currentTab.Filter;

            for (auto selection : currentTab.Entries)
            {
                if (MatchFilter(selection))
                    _filteredSceneryTab.AddEntryToBack(selection);
            }
        }

        bool MatchFilter(const ScenerySelection& selection)
        {
            if (_filteredSceneryTab.Filter.empty())
                return true;

            auto& objManager = GetContext()->GetObjectManager();
            auto sceneryObjectType = GetObjectTypeFromSceneryType(selection.SceneryType);
            auto sceneryObject = objManager.GetLoadedObject(sceneryObjectType, selection.EntryIndex);

            return IsFilterInName(*sceneryObject) || IsFilterInAuthors(*sceneryObject) || IsFilterInIdentifier(*sceneryObject)
                || IsFilterInFilename(*sceneryObject);
        }

        bool IsFilterInName(const Object& object)
        {
            return String::contains(object.GetName(), _filteredSceneryTab.Filter, true);
        }

        bool IsFilterInAuthors(const Object& object)
        {
            for (auto author : object.GetAuthors())
                if (String::contains(author, _filteredSceneryTab.Filter, true))
                    return true;

            return false;
        }

        bool IsFilterInIdentifier(const Object& object)
        {
            return String::contains(object.GetIdentifier(), _filteredSceneryTab.Filter, true);
        }

        bool IsFilterInFilename(const Object& object)
        {
            return String::contains(object.GetFileName(), _filteredSceneryTab.Filter, true);
        }

        void SortTabs()
        {
            std::sort(_tabEntries.begin(), _tabEntries.end(), [](const SceneryTabInfo& a, const SceneryTabInfo& b) {
                if (a.SceneryGroupIndex == b.SceneryGroupIndex)
                    return false;

                if (a.SceneryGroupIndex == kObjectEntryIndexNull)
                    return false;
                if (b.SceneryGroupIndex == kObjectEntryIndexNull)
                    return true;

                const auto* entryA = a.GetSceneryGroupEntry();
                const auto* entryB = b.GetSceneryGroupEntry();
                return entryA->priority < entryB->priority;
            });
        }

        int32_t GetTabRowCount()
        {
            return std::max<int32_t>((static_cast<int32_t>(_tabEntries.size()) + kMaxTabsPerRow - 1) / kMaxTabsPerRow, 0);
        }

        int32_t GetMaxTabCountInARow() const
        {
            int32_t tabEntries = static_cast<int32_t>(_tabEntries.size());
            return std::clamp(tabEntries, 1, kMaxTabsPerRow);
        }

        void PrepareWidgets()
        {
            // Add the base widgets
            setWidgets(WindowSceneryBaseWidgets);

            auto hasMisc = GetSceneryTabInfoForMisc() != nullptr;
            auto kMaxTabsInThisRow = kMaxTabsPerRow - 1 - (hasMisc ? 1 : 0);

            // Add tabs
            int32_t tabsInThisRow = 0;
            ScreenCoordsXY pos = { kTabMargin, widgets[WIDX_SCENERY_TITLE].bottom + kTabMargin };
            for (const auto& tabInfo : _tabEntries)
            {
                auto widget = makeTab(pos, STR_STRING_DEFINED_TOOLTIP);
                pos.x += kTabWidth;

                if (tabInfo.IsMisc())
                {
                    widget.image = ImageId(SPR_TAB_QUESTION, FilterPaletteID::paletteNull);
                }
                else if (tabInfo.IsAll())
                {
                    widget.image = ImageId(SPR_TAB, FilterPaletteID::paletteNull);
                }
                else if (tabInfo.IsSceneryGroup())
                {
                    // Default tab image
                    widget.image = ImageId(SPR_TAB_QUESTION, FilterPaletteID::paletteNull);

                    // Scenery Group image
                    auto scgEntry = tabInfo.GetSceneryGroupEntry();
                    if (scgEntry != nullptr)
                    {
                        widget.image = ImageId(scgEntry->image, colours[1].colour);
                    }
                }

                widgets.push_back(widget);

                tabsInThisRow++;
                if (tabsInThisRow >= kMaxTabsInThisRow)
                {
                    pos.x = kTabMargin;
                    pos.y += kTabHeight;
                    tabsInThisRow = 0;
                    _actualMinHeight += kTabHeight;
                    kMaxTabsInThisRow = kMaxTabsPerRow;
                }
            }

            // Shift base widgets based on number of tab rows
            int32_t shiftAmount = (GetTabRowCount() - 1) * kTabHeight;
            if (shiftAmount > 0)
            {
                widgets[WIDX_SCENERY_LIST].top += shiftAmount;
                widgets[WIDX_SCENERY_LIST].bottom += shiftAmount;
                widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].bottom += shiftAmount;
                widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].bottom += shiftAmount;
                widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].bottom += shiftAmount;
                widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].bottom += shiftAmount;
                widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].bottom += shiftAmount;
                widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].bottom += shiftAmount;
                widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].bottom += shiftAmount;
                widgets[WIDX_FILTER_TEXT_BOX].top += shiftAmount;
                widgets[WIDX_FILTER_TEXT_BOX].bottom += shiftAmount;
                widgets[WIDX_FILTER_CLEAR_BUTTON].top += shiftAmount;
                widgets[WIDX_FILTER_CLEAR_BUTTON].bottom += shiftAmount;
            }
        }

        ScenerySelection GetSceneryIdByCursorPos(const ScreenCoordsXY& screenCoords) const
        {
            ScenerySelection scenery{};

            const auto numColumns = GetNumColumns();
            const auto colIndex = screenCoords.x / kSceneryButtonWidth;
            const auto rowIndex = screenCoords.y / kSceneryButtonHeight;
            if (colIndex >= 0 && colIndex < numColumns && rowIndex >= 0)
            {
                const auto tabSceneryIndex = static_cast<size_t>((rowIndex * numColumns) + colIndex);
                if (tabSceneryIndex < _filteredSceneryTab.Entries.size())
                {
                    return _filteredSceneryTab.Entries[tabSceneryIndex];
                }
            }
            return scenery;
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            auto rows = CountRows();
            return { 0, ContentRowsHeight(rows) };
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            const auto scenery = GetSceneryIdByCursorPos(screenCoords);
            if (scenery.IsUndefined())
                return;

            auto lastScenery = GetSelectedScenery(_activeTabIndex);
            if (lastScenery != scenery && !MatchFilter(lastScenery))
            {
                _filteredSceneryTab.Entries.pop_front();
            }
            SetSelectedScenery(_activeTabIndex, scenery);

            _sceneryPaintEnabled = false;
            gWindowSceneryEyedropperEnabled = false;

            Audio::Play(Audio::SoundId::click1, 0, windowPos.x + (width / 2));
            _hoverCounter = -16;
            gSceneryPlaceCost = kMoney64Undefined;
            invalidate();
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            ScenerySelection scenery = GetSceneryIdByCursorPos(screenCoords);
            if (!scenery.IsUndefined() && _selectedScenery != scenery)
            {
                _selectedScenery = scenery;
                invalidate();
            }
        }

        template<typename TObjectType>
        std::pair<StringId, money64> GetNameAndPriceByType(const ScenerySelection& selectedScenery)
        {
            auto* sceneryEntry = ObjectManager::GetObjectEntry<TObjectType>(selectedScenery.EntryIndex);
            if (sceneryEntry != nullptr)
            {
                return { sceneryEntry->name, sceneryEntry->price };
            }
            else
            {
                return { STR_UNKNOWN_OBJECT_TYPE, kMoney64Undefined };
            }
        }

        std::pair<StringId, money64> GetNameAndPrice(const ScenerySelection& selectedScenery)
        {
            if (!selectedScenery.IsUndefined())
            {
                switch (selectedScenery.SceneryType)
                {
                    case SCENERY_TYPE_SMALL:
                    {
                        return GetNameAndPriceByType<SmallSceneryEntry>(selectedScenery);
                    }
                    case SCENERY_TYPE_PATH_ITEM:
                    {
                        return GetNameAndPriceByType<PathAdditionEntry>(selectedScenery);
                    }
                    case SCENERY_TYPE_WALL:
                    {
                        return GetNameAndPriceByType<WallSceneryEntry>(selectedScenery);
                    }
                    case SCENERY_TYPE_LARGE:
                    {
                        return GetNameAndPriceByType<LargeSceneryEntry>(selectedScenery);
                    }
                    case SCENERY_TYPE_BANNER:
                    {
                        return GetNameAndPriceByType<BannerSceneryEntry>(selectedScenery);
                    }
                }
            }

            StringId name = STR_UNKNOWN_OBJECT_TYPE;
            money64 price = gSceneryPlaceCost != kMoney64Undefined ? gSceneryPlaceCost : kMoney64Undefined;
            return { name, price };
        }

        void DrawTabs(RenderTarget& rt, const ScreenCoordsXY& offset)
        {
            for (size_t tabIndex = 0; tabIndex < _tabEntries.size(); tabIndex++)
            {
                auto widgetIndex = static_cast<WidgetIndex>(WIDX_SCENERY_TAB_1 + tabIndex);
                auto widgetCoordsXY = ScreenCoordsXY(widgets[widgetIndex].left, widgets[widgetIndex].top);

                if (_tabEntries[tabIndex].IsAll())
                {
                    auto imageId = ImageId(SPR_G2_INFINITY, FilterPaletteID::paletteNull);
                    GfxDrawSprite(rt, imageId, offset + widgetCoordsXY);
                }
            }
        }

        void DrawSceneryItem(RenderTarget& rt, ScenerySelection scenerySelection)
        {
            if (scenerySelection.SceneryType == SCENERY_TYPE_BANNER)
            {
                auto bannerEntry = ObjectManager::GetObjectEntry<BannerSceneryEntry>(scenerySelection.EntryIndex);
                if (bannerEntry == nullptr)
                    return;

                auto imageId = ImageId(bannerEntry->image + gWindowSceneryRotation * 2, _sceneryPrimaryColour);
                GfxDrawSprite(rt, imageId, { 33, 40 });
                GfxDrawSprite(rt, imageId.WithIndexOffset(1), { 33, 40 });
            }
            else if (scenerySelection.SceneryType == SCENERY_TYPE_LARGE)
            {
                auto sceneryEntry = ObjectManager::GetObjectEntry<LargeSceneryEntry>(scenerySelection.EntryIndex);
                if (sceneryEntry == nullptr)
                    return;

                auto imageId = ImageId(sceneryEntry->image + gWindowSceneryRotation);
                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR)
                    imageId = imageId.WithPrimary(_sceneryPrimaryColour);
                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
                    imageId = imageId.WithSecondary(_scenerySecondaryColour);
                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
                    imageId = imageId.WithTertiary(_sceneryTertiaryColour);
                GfxDrawSprite(rt, imageId, { 33, 0 });
            }
            else if (scenerySelection.SceneryType == SCENERY_TYPE_WALL)
            {
                auto wallEntry = ObjectManager::GetObjectEntry<WallSceneryEntry>(scenerySelection.EntryIndex);
                if (wallEntry == nullptr)
                    return;

                auto imageId = ImageId(wallEntry->image);
                auto spriteTop = (wallEntry->height * 2) + 0x32;
                if (wallEntry->flags & WALL_SCENERY_HAS_GLASS)
                {
                    imageId = imageId.WithPrimary(_sceneryPrimaryColour);
                    if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                    {
                        imageId = imageId.WithSecondary(_scenerySecondaryColour);
                    }
                    GfxDrawSprite(rt, imageId, { 47, spriteTop });

                    auto glassImageId = ImageId(wallEntry->image + 6).WithTransparency(_sceneryPrimaryColour);
                    GfxDrawSprite(rt, glassImageId, { 47, spriteTop });
                }
                else
                {
                    imageId = imageId.WithPrimary(_sceneryPrimaryColour);
                    if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                    {
                        imageId = imageId.WithSecondary(_scenerySecondaryColour);
                        if (wallEntry->flags & WALL_SCENERY_HAS_TERTIARY_COLOUR)
                        {
                            imageId = imageId.WithTertiary(_sceneryTertiaryColour);
                        }
                    }
                    GfxDrawSprite(rt, imageId, { 47, spriteTop });

                    if (wallEntry->flags & WALL_SCENERY_IS_DOOR)
                    {
                        GfxDrawSprite(rt, imageId.WithIndexOffset(1), { 47, spriteTop });
                    }
                }
            }
            else if (scenerySelection.SceneryType == SCENERY_TYPE_PATH_ITEM)
            {
                auto* pathAdditionEntry = ObjectManager::GetObjectEntry<PathAdditionEntry>(scenerySelection.EntryIndex);
                if (pathAdditionEntry == nullptr)
                    return;

                auto imageId = ImageId(pathAdditionEntry->image);
                GfxDrawSprite(rt, imageId, { 11, 16 });
            }
            else
            {
                auto sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(scenerySelection.EntryIndex);
                if (sceneryEntry == nullptr)
                    return;

                auto imageId = ImageId(sceneryEntry->image + gWindowSceneryRotation);
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
                {
                    imageId = imageId.WithPrimary(_sceneryPrimaryColour);
                    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
                    {
                        imageId = imageId.WithSecondary(_scenerySecondaryColour);
                    }
                }
                if (sceneryEntry->flags & SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
                {
                    imageId = imageId.WithTertiary(_sceneryTertiaryColour);
                }

                auto spriteTop = (sceneryEntry->height / 4) + 43;
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE)
                    && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
                {
                    spriteTop -= 12;
                }

                GfxDrawSprite(rt, imageId, { 32, spriteTop });

                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_GLASS))
                {
                    imageId = ImageId(sceneryEntry->image + 4 + gWindowSceneryRotation).WithTransparency(_sceneryPrimaryColour);
                    GfxDrawSprite(rt, imageId, { 32, spriteTop });
                }

                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ANIMATED_FG))
                {
                    imageId = ImageId(sceneryEntry->image + 4 + gWindowSceneryRotation);
                    GfxDrawSprite(rt, imageId, { 32, spriteTop });
                }
            }
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            GfxClear(rt, getColourMap(colours[1].colour).midLight);

            auto numColumns = GetNumColumns();
            auto tabIndex = _activeTabIndex;
            if (tabIndex >= _tabEntries.size())
            {
                return;
            }

            ScreenCoordsXY topLeft{ 0, 0 };

            for (size_t sceneryTabItemIndex = 0; sceneryTabItemIndex < _filteredSceneryTab.Entries.size();
                 sceneryTabItemIndex++)
            {
                const auto& currentSceneryGlobal = _filteredSceneryTab.Entries[sceneryTabItemIndex];
                const auto tabSelectedScenery = GetSelectedScenery(tabIndex);
                if (_sceneryPaintEnabled == 1 || gWindowSceneryEyedropperEnabled)
                {
                    if (_selectedScenery == currentSceneryGlobal)
                    {
                        Rectangle::fillInset(
                            rt, { topLeft, topLeft + ScreenCoordsXY{ kSceneryButtonWidth - 1, kSceneryButtonHeight - 1 } },
                            colours[1], Rectangle::BorderStyle::outset, Rectangle::FillBrightness::dark);
                    }
                }
                else
                {
                    if (tabSelectedScenery == currentSceneryGlobal)
                    {
                        Rectangle::fillInset(
                            rt, { topLeft, topLeft + ScreenCoordsXY{ kSceneryButtonWidth - 1, kSceneryButtonHeight - 1 } },
                            colours[1], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::dark);
                    }
                    else if (_selectedScenery == currentSceneryGlobal)
                    {
                        Rectangle::fillInset(
                            rt, { topLeft, topLeft + ScreenCoordsXY{ kSceneryButtonWidth - 1, kSceneryButtonHeight - 1 } },
                            colours[1], Rectangle::BorderStyle::outset, Rectangle::FillBrightness::dark);
                    }
                }

                RenderTarget clippedRT;
                if (ClipRenderTarget(
                        clippedRT, rt, topLeft + ScreenCoordsXY{ 1, 1 }, kSceneryButtonWidth - 2, kSceneryButtonHeight - 2))
                {
                    DrawSceneryItem(clippedRT, currentSceneryGlobal);
                }

                topLeft.x += kSceneryButtonWidth;
                if (topLeft.x >= numColumns * kSceneryButtonWidth)
                {
                    topLeft.y += kSceneryButtonHeight;
                    topLeft.x = 0;
                }
            }
        }

        void onToolUpdateSmallScenery(WidgetIndex widgetIndex, const ScreenCoordsXY& screenPos, ScenerySelection selection)
        {
            CoordsXY mapTile = {};
            uint8_t quadrant;
            Direction rotation;

            updatePlacementSmallScenery(screenPos, selection.EntryIndex, mapTile, &quadrant, &rotation);

            if (mapTile.IsNull())
            {
                SceneryRemoveGhostToolPlacement();
                return;
            }

            gMapSelectFlags.set(MapSelectFlag::enable);
            if (gWindowSceneryScatterEnabled)
            {
                uint16_t cluster_size = (gWindowSceneryScatterSize - 1) * kCoordsXYStep;
                gMapSelectPositionA.x = mapTile.x - cluster_size / 2;
                gMapSelectPositionA.y = mapTile.y - cluster_size / 2;
                gMapSelectPositionB.x = mapTile.x + cluster_size / 2;
                gMapSelectPositionB.y = mapTile.y + cluster_size / 2;
                if (gWindowSceneryScatterSize % 2 == 0)
                {
                    gMapSelectPositionB.x += kCoordsXYStep;
                    gMapSelectPositionB.y += kCoordsXYStep;
                }
            }
            else
            {
                gMapSelectPositionA.x = mapTile.x;
                gMapSelectPositionA.y = mapTile.y;
                gMapSelectPositionB.x = mapTile.x;
                gMapSelectPositionB.y = mapTile.y;
            }

            auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(selection.EntryIndex);

            gMapSelectType = MapSelectType::full;
            if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE) && !gWindowSceneryScatterEnabled)
            {
                gMapSelectType = getMapSelectQuarter((quadrant ^ 2));
            }

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_0) && mapTile == gSceneryGhostPosition && quadrant == _unkF64F0E
                && gSceneryPlaceZ == _unkF64F0A && gSceneryPlaceObject.SceneryType == SCENERY_TYPE_SMALL
                && gSceneryPlaceObject.EntryIndex == selection.EntryIndex)
            {
                return;
            }

            SceneryRemoveGhostToolPlacement();

            _unkF64F0E = quadrant;
            _unkF64F0A = gSceneryPlaceZ;

            uint8_t attemptsLeft = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                attemptsLeft = 20;
            }

            money64 cost = 0;
            for (; attemptsLeft != 0; attemptsLeft--)
            {
                cost = TryPlaceGhostSmallScenery(
                    { mapTile, gSceneryPlaceZ, rotation }, quadrant, selection.EntryIndex, _sceneryPrimaryColour,
                    _scenerySecondaryColour, _sceneryTertiaryColour);

                if (cost != kMoney64Undefined)
                    break;
                gSceneryPlaceZ += 8;
            }

            gSceneryPlaceCost = cost;
        }

        void onToolUpdatePathItem(WidgetIndex widgetIndex, const ScreenCoordsXY& screenPos, ScenerySelection selection)
        {
            CoordsXY mapTile = {};
            int32_t z;

            updatePlacementPathItem(screenPos, selection.EntryIndex, mapTile, &z);

            if (mapTile.IsNull())
            {
                SceneryRemoveGhostToolPlacement();
                return;
            }

            gMapSelectFlags.set(MapSelectFlag::enable);
            gMapSelectPositionA.x = mapTile.x;
            gMapSelectPositionA.y = mapTile.y;
            gMapSelectPositionB.x = mapTile.x;
            gMapSelectPositionB.y = mapTile.y;
            gMapSelectType = MapSelectType::full;

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_1) && mapTile == gSceneryGhostPosition && z == gSceneryGhostPosition.z)
            {
                return;
            }

            SceneryRemoveGhostToolPlacement();

            money64 cost = TryPlaceGhostPathAddition({ mapTile, z }, selection.EntryIndex);

            gSceneryPlaceCost = cost;
        }

        void onToolUpdateWall(WidgetIndex widgetIndex, const ScreenCoordsXY& screenPos, ScenerySelection selection)
        {
            bool isLeftMousePressed = gInputFlags.has(InputFlag::leftMousePressed);
            if (isLeftMousePressed)
            {
                gMapSelectFlags.set(MapSelectFlag::enable);
                return;
            }

            CoordsXY mapTile = {};
            uint8_t edge;

            updatePlacementWall(screenPos, selection.EntryIndex, mapTile, &edge);

            if (mapTile.IsNull())
            {
                SceneryRemoveGhostToolPlacement();
                return;
            }

            gMapSelectFlags.set(MapSelectFlag::enable);
            setMapSelectRange(mapTile);
            gMapSelectType = getMapSelectEdge(edge);

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_2) && mapTile == gSceneryGhostPosition
                && edge == gSceneryGhostWallRotation && gSceneryPlaceZ == _unkF64F0A)
            {
                return;
            }

            SceneryRemoveGhostToolPlacement();

            gSceneryGhostWallRotation = edge;
            _unkF64F0A = gSceneryPlaceZ;

            uint8_t attemptsLeft = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                attemptsLeft = 20;
            }

            money64 cost = 0;
            for (; attemptsLeft != 0; attemptsLeft--)
            {
                cost = TryPlaceGhostWall(
                    { mapTile, gSceneryPlaceZ }, edge, selection.EntryIndex, _sceneryPrimaryColour, _scenerySecondaryColour,
                    _sceneryTertiaryColour);

                if (cost != kMoney64Undefined)
                    break;
                gSceneryPlaceZ += 8;
            }

            gSceneryPlaceCost = cost;
        }

        void onToolUpdateLargeScenery(WidgetIndex widgetIndex, const ScreenCoordsXY& screenPos, ScenerySelection selection)
        {
            CoordsXY mapTile = {};
            Direction direction;

            updatePlacementLargeScenery(screenPos, selection.EntryIndex, mapTile, &direction);

            if (mapTile.IsNull())
            {
                SceneryRemoveGhostToolPlacement();
                return;
            }

            auto* sceneryEntry = ObjectManager::GetObjectEntry<LargeSceneryEntry>(selection.EntryIndex);

            gMapSelectFlags.set(MapSelectFlag::enableConstruct);

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_3) && mapTile == gSceneryGhostPosition && gSceneryPlaceZ == _unkF64F0A
                && gSceneryPlaceObject.SceneryType == SCENERY_TYPE_LARGE
                && gSceneryPlaceObject.EntryIndex == selection.EntryIndex)
            {
                return;
            }

            MapSelection::clearSelectedTiles();
            for (auto& tile : sceneryEntry->tiles)
            {
                MapSelection::addSelectedTile(mapTile + tile.offset.Rotate(direction));
            }

            SceneryRemoveGhostToolPlacement();

            gSceneryPlaceObject.SceneryType = SCENERY_TYPE_LARGE;
            gSceneryPlaceObject.EntryIndex = selection.EntryIndex;
            _unkF64F0A = gSceneryPlaceZ;

            uint8_t attemptsLeft = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                attemptsLeft = 20;
            }

            money64 cost = 0;
            for (; attemptsLeft != 0; attemptsLeft--)
            {
                cost = TryPlaceGhostLargeScenery(
                    { mapTile, gSceneryPlaceZ, direction }, selection.EntryIndex, _sceneryPrimaryColour,
                    _scenerySecondaryColour, _sceneryTertiaryColour);

                if (cost != kMoney64Undefined)
                    break;
                gSceneryPlaceZ += kCoordsZStep;
            }

            gSceneryPlaceCost = cost;
        }

        void onToolUpdateBanner(WidgetIndex widgetIndex, const ScreenCoordsXY& screenPos, ScenerySelection selection)
        {
            CoordsXY mapTile = {};
            Direction direction;
            int32_t z;

            updatePlacementBanner(screenPos, selection.EntryIndex, mapTile, &z, &direction);

            if (mapTile.IsNull())
            {
                SceneryRemoveGhostToolPlacement();
                return;
            }

            gMapSelectFlags.set(MapSelectFlag::enable);
            gMapSelectPositionA.x = mapTile.x;
            gMapSelectPositionA.y = mapTile.y;
            gMapSelectPositionB.x = mapTile.x;
            gMapSelectPositionB.y = mapTile.y;
            gMapSelectType = MapSelectType::full;

            // If no change in ghost placement
            if ((gSceneryGhostType & SCENERY_GHOST_FLAG_4) && mapTile == gSceneryGhostPosition && z == gSceneryGhostPosition.z
                && direction == gSceneryPlaceRotation)
            {
                return;
            }

            SceneryRemoveGhostToolPlacement();

            money64 cost = TryPlaceGhostBanner({ mapTile, z, direction }, selection.EntryIndex);

            gSceneryPlaceCost = cost;
        }

        /**
         *
         *  rct2: 0x006E287B
         */
        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenPos) override
        {
            gMapSelectFlags.unset(MapSelectFlag::enable);
            gMapSelectFlags.unset(MapSelectFlag::enableConstruct);

            if (_sceneryPaintEnabled)
                return;
            if (gWindowSceneryEyedropperEnabled)
                return;

            const auto selection = WindowSceneryGetTabSelection();
            if (selection.IsUndefined())
            {
                SceneryRemoveGhostToolPlacement();
                return;
            }

            switch (selection.SceneryType)
            {
                case SCENERY_TYPE_SMALL:
                {
                    return onToolUpdateSmallScenery(widgetIndex, screenPos, selection);
                }
                case SCENERY_TYPE_PATH_ITEM:
                {
                    return onToolUpdatePathItem(widgetIndex, screenPos, selection);
                }
                case SCENERY_TYPE_WALL:
                {
                    return onToolUpdateWall(widgetIndex, screenPos, selection);
                }
                case SCENERY_TYPE_LARGE:
                {
                    return onToolUpdateLargeScenery(widgetIndex, screenPos, selection);
                }
                case SCENERY_TYPE_BANNER:
                {
                    return onToolUpdateBanner(widgetIndex, screenPos, selection);
                }
            }
        }

        /**
         *
         *  rct2: 0x006E24F6
         * on failure returns kMoney64Undefined
         * on success places ghost scenery and returns cost to place proper
         */
        money64 TryPlaceGhostSmallScenery(
            CoordsXYZD loc, uint8_t quadrant, ObjectEntryIndex entryIndex, Colour primaryColour, Colour secondaryColour,
            Colour tertiaryColour)
        {
            SceneryRemoveGhostToolPlacement();

            // 6e252b
            auto smallSceneryPlaceAction = GameActions::SmallSceneryPlaceAction(
                loc, quadrant, entryIndex, primaryColour, secondaryColour, tertiaryColour);
            smallSceneryPlaceAction.SetFlags({ CommandFlag::ghost, CommandFlag::allowDuringPaused });
            auto res = GameActions::Execute(&smallSceneryPlaceAction, getGameState());
            if (res.error != GameActions::Status::ok)
                return kMoney64Undefined;

            const auto placementData = res.getData<GameActions::SmallSceneryPlaceActionResult>();

            gSceneryPlaceRotation = loc.direction;
            gSceneryPlaceObject.SceneryType = SCENERY_TYPE_SMALL;
            gSceneryPlaceObject.EntryIndex = entryIndex;

            gSceneryGhostPosition = { loc, placementData.BaseHeight };
            gSceneryQuadrant = placementData.SceneryQuadrant;
            if (placementData.GroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                // Set underground on
                ViewportSetVisibility(ViewportVisibility::undergroundViewGhostOn);
            }
            else
            {
                // Set underground off
                ViewportSetVisibility(ViewportVisibility::undergroundViewGhostOff);
            }

            gSceneryGhostType |= SCENERY_GHOST_FLAG_0;
            return res.cost;
        }

        money64 TryPlaceGhostPathAddition(CoordsXYZ loc, ObjectEntryIndex entryIndex)
        {
            SceneryRemoveGhostToolPlacement();

            // 6e265b
            auto footpathAdditionPlaceAction = GameActions::FootpathAdditionPlaceAction(loc, entryIndex);
            footpathAdditionPlaceAction.SetFlags({ CommandFlag::ghost, CommandFlag::allowDuringPaused });
            footpathAdditionPlaceAction.SetCallback([=](const GameActions::GameAction* ga, const GameActions::Result* result) {
                if (result->error != GameActions::Status::ok)
                {
                    return;
                }
                gSceneryGhostPosition = loc;
                gSceneryGhostType |= SCENERY_GHOST_FLAG_1;
            });
            auto res = GameActions::Execute(&footpathAdditionPlaceAction, getGameState());
            if (res.error != GameActions::Status::ok)
                return kMoney64Undefined;

            return res.cost;
        }

        money64 TryPlaceGhostWall(
            CoordsXYZ loc, uint8_t edge, ObjectEntryIndex entryIndex, Colour primaryColour, Colour secondaryColour,
            Colour tertiaryColour)
        {
            SceneryRemoveGhostToolPlacement();

            // 6e26b0
            auto wallPlaceAction = GameActions::WallPlaceAction(
                entryIndex, loc, edge, primaryColour, secondaryColour, tertiaryColour);
            wallPlaceAction.SetFlags({ CommandFlag::ghost, CommandFlag::allowDuringPaused, CommandFlag::noSpend });
            wallPlaceAction.SetCallback([=](const GameActions::GameAction* ga, const GameActions::Result* result) {
                if (result->error != GameActions::Status::ok)
                    return;

                const auto placementData = result->getData<GameActions::WallPlaceActionResult>();
                gSceneryGhostPosition = { loc, placementData.BaseHeight };
                gSceneryGhostWallRotation = edge;

                gSceneryGhostType |= SCENERY_GHOST_FLAG_2;
            });

            auto res = GameActions::Execute(&wallPlaceAction, getGameState());
            if (res.error != GameActions::Status::ok)
                return kMoney64Undefined;

            return res.cost;
        }

        money64 TryPlaceGhostLargeScenery(
            CoordsXYZD loc, ObjectEntryIndex entryIndex, Colour primaryColour, Colour secondaryColour, Colour tertiaryColour)
        {
            SceneryRemoveGhostToolPlacement();

            // 6e25a7
            auto sceneryPlaceAction = GameActions::LargeSceneryPlaceAction(
                loc, entryIndex, primaryColour, secondaryColour, tertiaryColour);
            sceneryPlaceAction.SetFlags({ CommandFlag::ghost, CommandFlag::allowDuringPaused, CommandFlag::noSpend });
            auto res = GameActions::Execute(&sceneryPlaceAction, getGameState());
            if (res.error != GameActions::Status::ok)
                return kMoney64Undefined;

            const auto placementData = res.getData<GameActions::LargeSceneryPlaceActionResult>();

            gSceneryPlaceRotation = loc.direction;

            gSceneryGhostPosition = { loc, placementData.firstTileHeight };
            if (placementData.GroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                // Set underground on
                ViewportSetVisibility(ViewportVisibility::undergroundViewGhostOn);
            }
            else
            {
                // Set underground off
                ViewportSetVisibility(ViewportVisibility::undergroundViewGhostOff);
            }

            gSceneryGhostType |= SCENERY_GHOST_FLAG_3;
            return res.cost;
        }

        money64 TryPlaceGhostBanner(CoordsXYZD loc, ObjectEntryIndex entryIndex)
        {
            SceneryRemoveGhostToolPlacement();

            // 6e2612
            auto primaryColour = _sceneryPrimaryColour;
            auto bannerPlaceAction = GameActions::BannerPlaceAction(loc, entryIndex, primaryColour);
            bannerPlaceAction.SetFlags({ CommandFlag::ghost, CommandFlag::allowDuringPaused, CommandFlag::noSpend });
            auto res = GameActions::Execute(&bannerPlaceAction, getGameState());
            if (res.error != GameActions::Status::ok)
                return kMoney64Undefined;

            gSceneryGhostPosition = loc;
            gSceneryGhostPosition.z += kPathHeightStep;
            gSceneryPlaceRotation = loc.direction;
            gSceneryGhostType |= SCENERY_GHOST_FLAG_4;
            return res.cost;
        }

        /**
         *
         *  rct2: 0x006E3158
         */
        void RepaintSceneryToolDown(const ScreenCoordsXY& screenCoords, WidgetIndex widgetIndex)
        {
            constexpr auto flag = EnumsToFlags(
                ViewportInteractionItem::scenery, ViewportInteractionItem::wall, ViewportInteractionItem::largeScenery,
                ViewportInteractionItem::banner);
            auto info = GetMapCoordinatesFromPos(screenCoords, flag);
            auto& gameState = getGameState();
            switch (info.interactionType)
            {
                case ViewportInteractionItem::scenery:
                {
                    auto* sceneryEntry = info.Element->AsSmallScenery()->GetEntry();

                    // If can't repaint
                    if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR | SMALL_SCENERY_FLAG_HAS_GLASS))
                        return;

                    uint8_t quadrant = info.Element->AsSmallScenery()->GetSceneryQuadrant();
                    auto repaintScenery = GameActions::SmallScenerySetColourAction(
                        { info.Loc, info.Element->GetBaseZ() }, quadrant, info.Element->AsSmallScenery()->GetEntryIndex(),
                        _sceneryPrimaryColour, _scenerySecondaryColour, _sceneryTertiaryColour);

                    GameActions::Execute(&repaintScenery, gameState);
                    break;
                }
                case ViewportInteractionItem::wall:
                {
                    auto* scenery_entry = info.Element->AsWall()->GetEntry();

                    // If can't repaint
                    if (!(scenery_entry->flags & (WALL_SCENERY_HAS_PRIMARY_COLOUR | WALL_SCENERY_HAS_GLASS)))
                        return;

                    auto repaintScenery = GameActions::WallSetColourAction(
                        { info.Loc, info.Element->GetBaseZ(), info.Element->GetDirection() }, _sceneryPrimaryColour,
                        _scenerySecondaryColour, _sceneryTertiaryColour);

                    GameActions::Execute(&repaintScenery, gameState);
                    break;
                }
                case ViewportInteractionItem::largeScenery:
                {
                    auto* sceneryEntry = info.Element->AsLargeScenery()->GetEntry();

                    // If can't repaint
                    if (!(sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
                        return;

                    auto repaintScenery = GameActions::LargeScenerySetColourAction(
                        { info.Loc, info.Element->GetBaseZ(), info.Element->GetDirection() },
                        info.Element->AsLargeScenery()->GetSequenceIndex(), _sceneryPrimaryColour, _scenerySecondaryColour,
                        _sceneryTertiaryColour);

                    GameActions::Execute(&repaintScenery, gameState);
                    break;
                }
                case ViewportInteractionItem::banner:
                {
                    auto banner = info.Element->AsBanner()->GetBanner();
                    if (banner != nullptr)
                    {
                        auto* bannerEntry = ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);
                        if (bannerEntry->flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR)
                        {
                            auto repaintScenery = GameActions::BannerSetColourAction(
                                { info.Loc, info.Element->GetBaseZ(), info.Element->AsBanner()->GetPosition() },
                                _sceneryPrimaryColour);

                            GameActions::Execute(&repaintScenery, gameState);
                        }
                    }
                    break;
                }
                default:
                    return;
            }
        }

        void SceneryEyedropperToolDown(const ScreenCoordsXY& screenCoords, WidgetIndex widgetIndex)
        {
            constexpr auto flag = EnumsToFlags(
                ViewportInteractionItem::scenery, ViewportInteractionItem::wall, ViewportInteractionItem::largeScenery,
                ViewportInteractionItem::banner, ViewportInteractionItem::pathAddition);
            auto info = GetMapCoordinatesFromPos(screenCoords, flag);
            switch (info.interactionType)
            {
                case ViewportInteractionItem::scenery:
                {
                    SmallSceneryElement* sceneryElement = info.Element->AsSmallScenery();
                    auto entryIndex = sceneryElement->GetEntryIndex();
                    auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(entryIndex);
                    if (sceneryEntry != nullptr)
                    {
                        WindowScenerySetSelectedItem(
                            { SCENERY_TYPE_SMALL, entryIndex }, sceneryElement->GetPrimaryColour(),
                            sceneryElement->GetSecondaryColour(), sceneryElement->GetTertiaryColour(),
                            sceneryElement->GetDirectionWithOffset(GetCurrentRotation()));
                    }
                    break;
                }
                case ViewportInteractionItem::wall:
                {
                    auto entryIndex = info.Element->AsWall()->GetEntryIndex();
                    auto* sceneryEntry = ObjectManager::GetObjectEntry<WallSceneryEntry>(entryIndex);
                    if (sceneryEntry != nullptr)
                    {
                        WindowScenerySetSelectedItem(
                            { SCENERY_TYPE_WALL, entryIndex }, info.Element->AsWall()->GetPrimaryColour(),
                            info.Element->AsWall()->GetSecondaryColour(), info.Element->AsWall()->GetTertiaryColour(),
                            std::nullopt);
                    }
                    break;
                }
                case ViewportInteractionItem::largeScenery:
                {
                    auto entryIndex = info.Element->AsLargeScenery()->GetEntryIndex();
                    auto* sceneryEntry = ObjectManager::GetObjectEntry<LargeSceneryEntry>(entryIndex);
                    if (sceneryEntry != nullptr)
                    {
                        WindowScenerySetSelectedItem(
                            { SCENERY_TYPE_LARGE, entryIndex }, info.Element->AsLargeScenery()->GetPrimaryColour(),
                            info.Element->AsLargeScenery()->GetSecondaryColour(), std::nullopt,
                            (GetCurrentRotation() + info.Element->GetDirection()) & 3);
                    }
                    break;
                }
                case ViewportInteractionItem::banner:
                {
                    auto banner = info.Element->AsBanner()->GetBanner();
                    if (banner != nullptr)
                    {
                        auto sceneryEntry = ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);
                        if (sceneryEntry != nullptr)
                        {
                            WindowScenerySetSelectedItem(
                                { SCENERY_TYPE_BANNER, banner->type }, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
                        }
                    }
                    break;
                }
                case ViewportInteractionItem::pathAddition:
                {
                    auto entryIndex = info.Element->AsPath()->GetAdditionEntryIndex();
                    auto* pathAdditionEntry = ObjectManager::GetObjectEntry<PathAdditionEntry>(entryIndex);
                    if (pathAdditionEntry != nullptr)
                    {
                        WindowScenerySetSelectedItem(
                            { SCENERY_TYPE_PATH_ITEM, entryIndex }, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        void updatePlacementUpdateScreenCoordsAndButtonsPressed(bool canRaiseItem, ScreenCoordsXY& screenPos)
        {
            if (!canRaiseItem && !getGameState().cheats.disableSupportLimits)
            {
                gSceneryCtrlPressed = false;
                gSceneryShiftPressed = false;
            }
            else
            {
                auto& im = GetInputManager();
                if (!gSceneryCtrlPressed)
                {
                    if (im.isModifierKeyPressed(ModifierKey::ctrl))
                    {
                        // CTRL pressed
                        constexpr auto flag = EnumsToFlags(
                            ViewportInteractionItem::terrain, ViewportInteractionItem::ride, ViewportInteractionItem::scenery,
                            ViewportInteractionItem::footpath, ViewportInteractionItem::wall,
                            ViewportInteractionItem::largeScenery);
                        auto info = GetMapCoordinatesFromPos(screenPos, flag);

                        if (info.interactionType != ViewportInteractionItem::none)
                        {
                            gSceneryCtrlPressed = true;
                            gSceneryCtrlPressZ = info.Element->GetBaseZ();
                        }
                    }
                }
                else
                {
                    if (!(im.isModifierKeyPressed(ModifierKey::ctrl)))
                    {
                        // CTRL not pressed
                        gSceneryCtrlPressed = false;
                    }
                }

                if (!gSceneryShiftPressed)
                {
                    if (im.isModifierKeyPressed(ModifierKey::shift))
                    {
                        // SHIFT pressed
                        gSceneryShiftPressed = true;
                        gSceneryShiftPressX = screenPos.x;
                        gSceneryShiftPressY = screenPos.y;
                        gSceneryShiftPressZOffset = 0;
                    }
                }
                else
                {
                    if (im.isModifierKeyPressed(ModifierKey::shift))
                    {
                        // SHIFT pressed
                        gSceneryShiftPressZOffset = (gSceneryShiftPressY - screenPos.y + 4);
                        // Scale delta by zoom to match mouse position.
                        auto* mainWnd = WindowGetMain();
                        if (mainWnd != nullptr && mainWnd->viewport != nullptr)
                        {
                            gSceneryShiftPressZOffset = mainWnd->viewport->zoom.ApplyTo(gSceneryShiftPressZOffset);
                        }
                        gSceneryShiftPressZOffset = Numerics::floor2(gSceneryShiftPressZOffset, 8);

                        screenPos.x = gSceneryShiftPressX;
                        screenPos.y = gSceneryShiftPressY;
                    }
                    else
                    {
                        // SHIFT not pressed
                        gSceneryShiftPressed = false;
                    }
                }
            }
        }

        void updatePlacementSmallScenery(
            const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, uint8_t* outQuadrant,
            Direction* outRotation)
        {
            auto screenPos = sourceScreenPos;
            uint16_t maxPossibleHeight = ZoomLevel::max().ApplyTo(
                std::numeric_limits<decltype(TileElement::BaseHeight)>::max() - 32);
            bool can_raise_item = false;

            const auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(sceneryIndex);
            if (sceneryEntry == nullptr)
            {
                gridPos.SetNull();
                return;
            }

            maxPossibleHeight -= sceneryEntry->height;
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_STACKABLE))
            {
                can_raise_item = true;
            }

            updatePlacementUpdateScreenCoordsAndButtonsPressed(can_raise_item, screenPos);

            // Small scenery
            if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
            {
                uint8_t quadrant = 0;

                // If CTRL not pressed
                if (!gSceneryCtrlPressed)
                {
                    auto gridCoords = ScreenGetMapXYQuadrant(screenPos, &quadrant);
                    if (!gridCoords.has_value())
                    {
                        gridPos.SetNull();
                        return;
                    }
                    gridPos = gridCoords.value();

                    gSceneryPlaceZ = 0;

                    // If SHIFT pressed
                    if (gSceneryShiftPressed)
                    {
                        auto* surfaceElement = MapGetSurfaceElementAt(gridPos);

                        if (surfaceElement == nullptr)
                        {
                            gridPos.SetNull();
                            return;
                        }

                        int16_t z = (surfaceElement->GetBaseZ()) & 0xFFF0;
                        z += gSceneryShiftPressZOffset;

                        z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                        gSceneryPlaceZ = z;
                    }
                }
                else
                {
                    int16_t z = gSceneryCtrlPressZ;

                    auto mapCoords = ScreenGetMapXYQuadrantWithZ(screenPos, z, &quadrant);
                    if (!mapCoords.has_value())
                    {
                        gridPos.SetNull();
                        return;
                    }
                    gridPos = mapCoords.value();

                    // If SHIFT pressed
                    if (gSceneryShiftPressed)
                    {
                        z += gSceneryShiftPressZOffset;
                    }

                    z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                    gSceneryPlaceZ = z;
                }

                if (gridPos.IsNull())
                    return;

                uint8_t rotation = gWindowSceneryRotation;

                if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ROTATABLE))
                {
                    rotation = UtilRand() & 0xFF;
                }

                rotation -= GetCurrentRotation();
                rotation &= 0x3;

                if (Config::Get().general.virtualFloorStyle != VirtualFloorStyles::Off)
                {
                    VirtualFloorSetHeight(gSceneryPlaceZ);
                }

                *outQuadrant = quadrant ^ 2;
                *outRotation = rotation;

                return;
            }

            // If CTRL not pressed
            if (!gSceneryCtrlPressed)
            {
                constexpr auto flag = EnumsToFlags(ViewportInteractionItem::terrain, ViewportInteractionItem::water);

                auto info = GetMapCoordinatesFromPos(screenPos, flag);
                gridPos = info.Loc;

                if (info.interactionType == ViewportInteractionItem::none)
                {
                    gridPos.SetNull();
                    return;
                }

                // If CTRL and SHIFT not pressed
                gSceneryPlaceZ = 0;

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    auto surfaceElement = MapGetSurfaceElementAt(gridPos);

                    if (surfaceElement == nullptr)
                    {
                        gridPos.SetNull();
                        return;
                    }

                    int16_t z = (surfaceElement->GetBaseZ()) & 0xFFF0;
                    z += gSceneryShiftPressZOffset;

                    z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                    gSceneryPlaceZ = z;
                }
            }
            else
            {
                int16_t z = gSceneryCtrlPressZ;
                auto coords = ScreenGetMapXYWithZ(screenPos, z);
                if (coords.has_value())
                {
                    gridPos = *coords;
                }
                else
                {
                    gridPos.SetNull();
                }
                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    z += gSceneryShiftPressZOffset;
                }

                z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                gSceneryPlaceZ = z;
            }

            if (gridPos.IsNull())
                return;

            gridPos = gridPos.ToTileStart();
            uint8_t rotation = gWindowSceneryRotation;

            if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ROTATABLE))
            {
                rotation = UtilRand() & 0xFF;
            }

            rotation -= GetCurrentRotation();
            rotation &= 0x3;

            if (Config::Get().general.virtualFloorStyle != VirtualFloorStyles::Off)
            {
                VirtualFloorSetHeight(gSceneryPlaceZ);
            }

            *outQuadrant = 0;
            *outRotation = rotation;
        }

        void updatePlacementPathItem(
            const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, int32_t* outZ)
        {
            auto screenPos = sourceScreenPos;
            updatePlacementUpdateScreenCoordsAndButtonsPressed(false, screenPos);

            // Path additions
            constexpr auto flag = EnumsToFlags(ViewportInteractionItem::footpath, ViewportInteractionItem::pathAddition);
            auto info = GetMapCoordinatesFromPos(screenPos, flag);
            gridPos = info.Loc;

            if (info.interactionType == ViewportInteractionItem::none)
            {
                gridPos.SetNull();
                return;
            }

            if (Config::Get().general.virtualFloorStyle != VirtualFloorStyles::Off)
            {
                VirtualFloorSetHeight(gSceneryPlaceZ);
            }

            *outZ = info.Element->GetBaseZ();
        }

        void updatePlacementWall(
            const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, uint8_t* outEdges)
        {
            auto screenPos = sourceScreenPos;
            uint16_t maxPossibleHeight = ZoomLevel::max().ApplyTo(
                std::numeric_limits<decltype(TileElement::BaseHeight)>::max() - 32);

            auto* wallEntry = ObjectManager::GetObjectEntry<WallSceneryEntry>(sceneryIndex);
            if (wallEntry != nullptr)
            {
                maxPossibleHeight -= wallEntry->height;
            }

            updatePlacementUpdateScreenCoordsAndButtonsPressed(true, screenPos);

            // Walls
            uint8_t edge;
            // If CTRL not pressed
            if (!gSceneryCtrlPressed)
            {
                auto gridCoords = ScreenGetMapXYSide(screenPos, &edge);
                if (!gridCoords.has_value())
                {
                    gridPos.SetNull();
                    return;
                }
                gridPos = gridCoords.value();

                gSceneryPlaceZ = 0;

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    auto* surfaceElement = MapGetSurfaceElementAt(gridPos);

                    if (surfaceElement == nullptr)
                    {
                        gridPos.SetNull();
                        return;
                    }

                    int16_t z = (surfaceElement->GetBaseZ()) & 0xFFF0;
                    z += gSceneryShiftPressZOffset;

                    z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                    gSceneryPlaceZ = z;
                }
            }
            else
            {
                int16_t z = gSceneryCtrlPressZ;
                auto mapCoords = ScreenGetMapXYSideWithZ(screenPos, z, &edge);
                if (!mapCoords.has_value())
                {
                    gridPos.SetNull();
                    return;
                }
                gridPos = mapCoords.value();

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    z += gSceneryShiftPressZOffset;
                }

                z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                gSceneryPlaceZ = z;
            }

            if (gridPos.IsNull())
                return;

            gridPos = gridPos.ToTileStart();

            if (Config::Get().general.virtualFloorStyle != VirtualFloorStyles::Off)
            {
                VirtualFloorSetHeight(gSceneryPlaceZ);
            }

            *outEdges = edge;
        }

        void updatePlacementLargeScenery(
            const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, Direction* outDirection)
        {
            auto screenPos = sourceScreenPos;
            uint16_t maxPossibleHeight = ZoomLevel::max().ApplyTo(
                std::numeric_limits<decltype(TileElement::BaseHeight)>::max() - 32);

            auto* sceneryEntry = ObjectManager::GetObjectEntry<LargeSceneryEntry>(sceneryIndex);
            if (sceneryEntry)
            {
                int16_t maxClearZ = 0;
                for (auto& tile : sceneryEntry->tiles)
                {
                    maxClearZ = std::max<int16_t>(maxClearZ, tile.zClearance);
                }
                maxPossibleHeight = std::max(0, maxPossibleHeight - maxClearZ);
            }

            updatePlacementUpdateScreenCoordsAndButtonsPressed(true, screenPos);

            // Large scenery
            // If CTRL not pressed
            if (!gSceneryCtrlPressed)
            {
                const CoordsXY mapCoords = ViewportInteractionGetTileStartAtCursor(screenPos);
                gridPos = mapCoords;

                if (gridPos.IsNull())
                    return;

                gSceneryPlaceZ = 0;

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    auto* surfaceElement = MapGetSurfaceElementAt(gridPos);

                    if (surfaceElement == nullptr)
                    {
                        gridPos.SetNull();
                        return;
                    }

                    int16_t z = (surfaceElement->GetBaseZ()) & 0xFFF0;
                    z += gSceneryShiftPressZOffset;

                    z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                    gSceneryPlaceZ = z;
                }
            }
            else
            {
                int16_t z = gSceneryCtrlPressZ;
                auto coords = ScreenGetMapXYWithZ(screenPos, z);
                if (coords.has_value())
                {
                    gridPos = *coords;
                }
                else
                {
                    gridPos.SetNull();
                }

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    z += gSceneryShiftPressZOffset;
                }

                z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                gSceneryPlaceZ = z;
            }

            if (gridPos.IsNull())
                return;

            gridPos = gridPos.ToTileStart();

            Direction rotation = gWindowSceneryRotation;
            rotation -= GetCurrentRotation();
            rotation &= 0x3;

            if (Config::Get().general.virtualFloorStyle != VirtualFloorStyles::Off)
            {
                VirtualFloorSetHeight(gSceneryPlaceZ);
            }

            *outDirection = rotation;
        }

        void updatePlacementBanner(
            const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, int32_t* outZ,
            Direction* outDirection)
        {
            auto screenPos = sourceScreenPos;
            updatePlacementUpdateScreenCoordsAndButtonsPressed(false, screenPos);

            // Banner
            constexpr auto flag = EnumsToFlags(ViewportInteractionItem::footpath, ViewportInteractionItem::pathAddition);
            auto info = GetMapCoordinatesFromPos(screenPos, flag);
            gridPos = info.Loc;

            if (info.interactionType == ViewportInteractionItem::none)
            {
                gridPos.SetNull();
                return;
            }

            uint8_t rotation = gWindowSceneryRotation;
            rotation -= GetCurrentRotation();
            rotation &= 0x3;

            auto z = info.Element->GetBaseZ();

            if (info.Element->AsPath()->IsSloped())
            {
                if (rotation != DirectionReverse(info.Element->AsPath()->GetSlopeDirection()))
                {
                    z += (2 * kCoordsZStep);
                }
            }

            if (Config::Get().general.virtualFloorStyle != VirtualFloorStyles::Off)
            {
                VirtualFloorSetHeight(gSceneryPlaceZ);
            }

            *outDirection = rotation;
            *outZ = z;
        }

        void onToolDownSmallScenery(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords, uint16_t selectedScenery)
        {
            CoordsXY gridPos;
            uint8_t quadrant;
            Direction rotation;

            updatePlacementSmallScenery(screenCoords, selectedScenery, gridPos, &quadrant, &rotation);
            if (gridPos.IsNull())
                return;

            int32_t quantity = 1;
            bool isCluster = gWindowSceneryScatterEnabled
                && (Network::GetMode() != Network::Mode::client
                    || Network::CanPerformCommand(Network::GetCurrentPlayerGroupIndex(), -2));

            if (isCluster)
            {
                switch (gWindowSceneryScatterDensity)
                {
                    case ScatterToolDensity::LowDensity:
                        quantity = gWindowSceneryScatterSize;
                        break;

                    case ScatterToolDensity::MediumDensity:
                        quantity = gWindowSceneryScatterSize * 2;
                        break;

                    case ScatterToolDensity::HighDensity:
                        quantity = gWindowSceneryScatterSize * 3;
                        break;
                }
            }

            bool forceError = true;
            for (int32_t q = 0; q < quantity; q++)
            {
                int32_t zCoordinate = gSceneryPlaceZ;
                auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(selectedScenery);

                int16_t cur_grid_x = gridPos.x;
                int16_t cur_grid_y = gridPos.y;

                if (isCluster)
                {
                    if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
                    {
                        quadrant = UtilRand() & 3;
                    }

                    int16_t grid_x_offset = (UtilRand() % gWindowSceneryScatterSize) - (gWindowSceneryScatterSize / 2);
                    int16_t grid_y_offset = (UtilRand() % gWindowSceneryScatterSize) - (gWindowSceneryScatterSize / 2);
                    if (gWindowSceneryScatterSize % 2 == 0)
                    {
                        grid_x_offset += 1;
                        grid_y_offset += 1;
                    }
                    cur_grid_x += grid_x_offset * kCoordsXYStep;
                    cur_grid_y += grid_y_offset * kCoordsXYStep;

                    if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ROTATABLE))
                    {
                        gSceneryPlaceRotation = (gSceneryPlaceRotation + 1) & 3;
                    }
                }

                uint8_t zAttemptRange = 1;
                if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
                {
                    zAttemptRange = 20;
                }

                auto success = GameActions::Status::unknown;
                // Try find a valid z coordinate
                for (; zAttemptRange != 0; zAttemptRange--)
                {
                    auto smallSceneryPlaceAction = GameActions::SmallSceneryPlaceAction(
                        { cur_grid_x, cur_grid_y, gSceneryPlaceZ, gSceneryPlaceRotation }, quadrant, selectedScenery,
                        _sceneryPrimaryColour, _scenerySecondaryColour, _sceneryTertiaryColour);

                    auto& gameState = getGameState();
                    auto res = GameActions::Query(&smallSceneryPlaceAction, gameState);
                    success = res.error;
                    if (res.error == GameActions::Status::ok)
                    {
                        break;
                    }

                    if (res.error == GameActions::Status::insufficientFunds)
                    {
                        break;
                    }
                    if (zAttemptRange != 1)
                    {
                        gSceneryPlaceZ += 8;
                    }
                }

                // Actually place
                if (success == GameActions::Status::ok || ((q + 1 == quantity) && forceError))
                {
                    auto smallSceneryPlaceAction = GameActions::SmallSceneryPlaceAction(
                        { cur_grid_x, cur_grid_y, gSceneryPlaceZ, gSceneryPlaceRotation }, quadrant, selectedScenery,
                        _sceneryPrimaryColour, _scenerySecondaryColour, _sceneryTertiaryColour);

                    smallSceneryPlaceAction.SetCallback(
                        [=](const GameActions::GameAction* ga, const GameActions::Result* result) {
                            if (result->error == GameActions::Status::ok)
                            {
                                Audio::Play3D(Audio::SoundId::placeItem, result->position);
                            }
                        });

                    auto& gameState = getGameState();
                    auto res = GameActions::Execute(&smallSceneryPlaceAction, gameState);
                    if (res.error == GameActions::Status::ok)
                    {
                        forceError = false;
                    }

                    if (res.error == GameActions::Status::insufficientFunds)
                    {
                        break;
                    }
                }
                gSceneryPlaceZ = zCoordinate;
            }
        }

        void onToolDownPathItem(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords, uint16_t selectedScenery)
        {
            CoordsXY gridPos;
            int32_t z;

            updatePlacementPathItem(screenCoords, selectedScenery, gridPos, &z);
            if (gridPos.IsNull())
                return;

            auto footpathAdditionPlaceAction = GameActions::FootpathAdditionPlaceAction({ gridPos, z }, selectedScenery);

            footpathAdditionPlaceAction.SetCallback([](const GameActions::GameAction* ga, const GameActions::Result* result) {
                if (result->error != GameActions::Status::ok)
                {
                    return;
                }
                Audio::Play3D(Audio::SoundId::placeItem, result->position);
            });

            auto& gameState = getGameState();
            auto res = GameActions::Execute(&footpathAdditionPlaceAction, gameState);
        }

        std::optional<CoordsXY> getMapPosFromScreenPos(const ScreenCoordsXY& screenCoords)
        {
            CoordsXY coords;
            if (gSceneryPlaceZ > 0)
            {
                auto candidate = ScreenGetMapXYWithZ(screenCoords, gSceneryPlaceZ);
                if (!candidate.has_value())
                    return std::nullopt;

                coords = *candidate;
            }
            else
            {
                auto info = GetMapCoordinatesFromPos(screenCoords, EnumsToFlags(ViewportInteractionItem::terrain));

                if (info.interactionType == ViewportInteractionItem::none)
                    return std::nullopt;

                coords = info.Loc;
            }

            coords = coords.ToTileStart();
            return coords;
        }

        void onToolDownWall(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords, uint16_t selectedScenery)
        {
            CoordsXY gridPos;
            uint8_t edges;
            updatePlacementWall(screenCoords, selectedScenery, gridPos, &edges);
            if (gridPos.IsNull())
                return;

            uint8_t zAttemptRange = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                zAttemptRange = 20;
            }

            for (; zAttemptRange != 0; zAttemptRange--)
            {
                auto wallPlaceAction = GameActions::WallPlaceAction(
                    selectedScenery, { gridPos, gSceneryPlaceZ }, edges, _sceneryPrimaryColour, _scenerySecondaryColour,
                    _sceneryTertiaryColour);

                auto& gameState = getGameState();
                auto res = GameActions::Query(&wallPlaceAction, gameState);
                if (res.error == GameActions::Status::ok)
                {
                    break;
                }

                if (const auto* message = std::get_if<StringId>(&res.errorMessage))
                {
                    if (*message == STR_NOT_ENOUGH_CASH_REQUIRES || *message == STR_CAN_ONLY_BUILD_THIS_ON_WATER)
                    {
                        break;
                    }
                }

                if (zAttemptRange != 1)
                {
                    gSceneryPlaceZ += 8;
                }
            }

            _inDragMode = true;
            _dragStartPos = gridPos;
            auto hoverPos = getMapPosFromScreenPos(screenCoords);
            _dragStartHoverPos = hoverPos.has_value() ? *hoverPos : gridPos;
            _dragEndPos = {};
            _startEdge = edges;
            gMapSelectFlags.set(MapSelectFlag::enable);
            gMapSelectType = getMapSelectEdge(_startEdge);
            setMapSelectRange(gridPos);
        }

        void dragWallSetEndPos(const ScreenCoordsXY& screenCoords)
        {
            auto endCoords = getMapPosFromScreenPos(screenCoords);
            if (!endCoords.has_value())
                return;

            if (endCoords == _dragStartHoverPos)
            {
                endCoords = _dragStartPos;
            }

            const bool onXAxis = (_startEdge == 1 || _startEdge == 3);
            if (onXAxis)
            {
                endCoords->y = _dragStartPos.y;
            }
            else
            {
                endCoords->x = _dragStartPos.x;
            }

            if (*endCoords != _dragEndPos)
            {
                setMapSelectRange({ _dragStartPos, *endCoords });
                updateProvisionalTiles();
            }
            _dragEndPos = *endCoords;
        }

        void removeProvisionalTilesFromMap()
        {
            auto& gameState = getGameState();
            for (const auto& tile : _provisionalTiles)
            {
                auto location = tile.location;
                location.z = tile.calculatedZ;
                auto wallRemoveAction = GameActions::WallRemoveAction(location);
                wallRemoveAction.SetFlags({ CommandFlag::allowDuringPaused, CommandFlag::noSpend, CommandFlag::ghost });
                wallRemoveAction.Execute(gameState);
            }
        }

        void updateProvisionalTiles()
        {
            auto& gameState = getGameState();
            auto tabSelection = WindowSceneryGetTabSelection();
            removeProvisionalTilesFromMap();

            _provisionalTiles.clear();
            _lastProvisionalError = {};

            auto mapRange = getMapSelectRange();
            for (auto y = mapRange.GetY1(); y <= mapRange.GetY2(); y += kCoordsXYStep)
            {
                for (auto x = mapRange.GetX1(); x <= mapRange.GetX2(); x += kCoordsXYStep)
                {
                    auto wallPlaceAction = GameActions::WallPlaceAction(
                        tabSelection.EntryIndex, { x, y, gSceneryPlaceZ }, _startEdge, _sceneryPrimaryColour,
                        _scenerySecondaryColour, _sceneryTertiaryColour);
                    wallPlaceAction.SetFlags({ CommandFlag::ghost, CommandFlag::allowDuringPaused });

                    auto result = GameActions::Execute(&wallPlaceAction, gameState);
                    if (result.error == GameActions::Status::ok)
                    {
                        const auto placementData = result.getData<GameActions::WallPlaceActionResult>();
                        _provisionalTiles.push_back({ CoordsXYZD(x, y, gSceneryPlaceZ, _startEdge), placementData.BaseHeight });
                    }
                    else
                    {
                        _lastProvisionalError = result;
                    }
                }
            }
        }

        void onToolDragWall(const ScreenCoordsXY& screenCoords)
        {
            if (!_inDragMode)
                return;

            dragWallSetEndPos(screenCoords);
        }

        void onToolUp(WidgetIndex, const ScreenCoordsXY&) override
        {
            if (_sceneryPaintEnabled || gWindowSceneryEyedropperEnabled)
                return;

            auto tabSelection = WindowSceneryGetTabSelection();
            auto sceneryType = tabSelection.SceneryType;
            if (sceneryType == SCENERY_TYPE_WALL)
            {
                onToolUpWall(tabSelection.EntryIndex);
            }
        }

        void onToolUpWall(uint16_t selectedScenery)
        {
            removeProvisionalTilesFromMap();

            if (!_inDragMode)
            {
                _provisionalTiles.clear();
                return;
            }

            if (_provisionalTiles.empty())
            {
                auto z = gSceneryPlaceZ > 0 ? gSceneryPlaceZ : TileElementHeight(_dragStartPos);
                Audio::Play3D(Audio::SoundId::error, { _dragStartPos, z });

                if (_lastProvisionalError.error != GameActions::Status::ok)
                {
                    auto windowManager = GetWindowManager();
                    windowManager->ShowError(
                        _lastProvisionalError.getErrorTitle(), _lastProvisionalError.getErrorMessage(), true);
                }

                _inDragMode = false;
                return;
            }

            auto mapRange = getMapSelectRange();
            bool anySuccessful = false;
            CoordsXYZ lastLocation = { mapRange.Point2, gSceneryPlaceZ };
            for (const auto& tile : _provisionalTiles)
            {
                auto wallPlaceAction = GameActions::WallPlaceAction(
                    selectedScenery, tile.location, tile.location.direction, _sceneryPrimaryColour, _scenerySecondaryColour,
                    _sceneryTertiaryColour);

                auto& gameState = getGameState();
                auto result = GameActions::Execute(&wallPlaceAction, gameState);
                if (result.error == GameActions::Status::ok)
                {
                    anySuccessful = true;
                    lastLocation = result.position;
                }
            }

            _provisionalTiles.clear();
            _inDragMode = false;

            if (anySuccessful)
            {
                Audio::Play3D(Audio::SoundId::placeItem, lastLocation);
            }
        }

        void onToolDownLargeScenery(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords, uint16_t selectedScenery)
        {
            CoordsXY gridPos;
            Direction direction;
            updatePlacementLargeScenery(screenCoords, selectedScenery, gridPos, &direction);
            if (gridPos.IsNull())
                return;

            uint8_t zAttemptRange = 1;
            if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
            {
                zAttemptRange = 20;
            }

            for (; zAttemptRange != 0; zAttemptRange--)
            {
                CoordsXYZD loc = { gridPos, gSceneryPlaceZ, direction };

                auto sceneryPlaceAction = GameActions::LargeSceneryPlaceAction(
                    loc, selectedScenery, _sceneryPrimaryColour, _scenerySecondaryColour, _sceneryTertiaryColour);

                auto& gameState = getGameState();
                auto res = GameActions::Query(&sceneryPlaceAction, gameState);
                if (res.error == GameActions::Status::ok)
                {
                    break;
                }

                if (const auto* message = std::get_if<StringId>(&res.errorMessage))
                {
                    if (*message == STR_NOT_ENOUGH_CASH_REQUIRES || *message == STR_CAN_ONLY_BUILD_THIS_ON_WATER)
                    {
                        break;
                    }
                }

                if (zAttemptRange != 1)
                {
                    gSceneryPlaceZ += 8;
                }
            }

            CoordsXYZD loc = { gridPos, gSceneryPlaceZ, direction };

            auto sceneryPlaceAction = GameActions::LargeSceneryPlaceAction(
                loc, selectedScenery, _sceneryPrimaryColour, _scenerySecondaryColour, _sceneryTertiaryColour);
            sceneryPlaceAction.SetCallback([=](const GameActions::GameAction* ga, const GameActions::Result* result) {
                if (result->error == GameActions::Status::ok)
                {
                    Audio::Play3D(Audio::SoundId::placeItem, result->position);
                }
                else
                {
                    Audio::Play3D(Audio::SoundId::error, { loc.x, loc.y, gSceneryPlaceZ });
                }
            });

            auto& gameState = getGameState();
            auto res = GameActions::Execute(&sceneryPlaceAction, gameState);
        }

        void onToolDownBanner(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords, uint16_t selectedScenery)
        {
            CoordsXY gridPos;
            int32_t z;
            Direction direction;
            updatePlacementBanner(screenCoords, selectedScenery, gridPos, &z, &direction);
            if (gridPos.IsNull())
                return;

            CoordsXYZD loc{ gridPos, z, direction };
            auto primaryColour = _sceneryPrimaryColour;
            auto bannerPlaceAction = GameActions::BannerPlaceAction(loc, selectedScenery, primaryColour);
            bannerPlaceAction.SetCallback([=](const GameActions::GameAction* ga, const GameActions::Result* result) {
                if (result->error == GameActions::Status::ok)
                {
                    auto data = result->getData<GameActions::BannerPlaceActionResult>();
                    Audio::Play3D(Audio::SoundId::placeItem, result->position);
                    ContextOpenDetailWindow(WindowDetail::banner, data.bannerId.ToUnderlying());
                }
            });

            auto& gameState = getGameState();
            GameActions::Execute(&bannerPlaceAction, gameState);
        }

        /**
         *
         *  rct2: 0x006E2CC6
         */
        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            SceneryRemoveGhostToolPlacement();

            if (_sceneryPaintEnabled)
            {
                RepaintSceneryToolDown(screenCoords, widgetIndex);
                return;
            }

            if (gWindowSceneryEyedropperEnabled)
            {
                SceneryEyedropperToolDown(screenCoords, widgetIndex);
                return;
            }

            auto tabSelection = WindowSceneryGetTabSelection();
            uint8_t sceneryType = tabSelection.SceneryType;
            uint16_t selectedScenery = tabSelection.EntryIndex;

            switch (sceneryType)
            {
                case SCENERY_TYPE_SMALL:
                {
                    return onToolDownSmallScenery(widgetIndex, screenCoords, selectedScenery);
                }

                case SCENERY_TYPE_PATH_ITEM:
                {
                    return onToolDownPathItem(widgetIndex, screenCoords, selectedScenery);
                }
                case SCENERY_TYPE_WALL:
                {
                    return onToolDownWall(widgetIndex, screenCoords, selectedScenery);
                }
                case SCENERY_TYPE_LARGE:
                {
                    return onToolDownLargeScenery(widgetIndex, screenCoords, selectedScenery);
                }
                case SCENERY_TYPE_BANNER:
                {
                    return onToolDownBanner(widgetIndex, screenCoords, selectedScenery);
                }
            }
        }

        void onToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (_sceneryPaintEnabled || gWindowSceneryEyedropperEnabled)
                onToolDown(widgetIndex, screenCoords);

            auto tabSelection = WindowSceneryGetTabSelection();
            auto sceneryType = tabSelection.SceneryType;
            if (sceneryType == SCENERY_TYPE_WALL)
            {
                onToolDragWall(screenCoords);
            }
        }
    };

    WindowBase* SceneryOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<SceneryWindow*>(windowMgr->BringToFrontByClass(WindowClass::scenery));
        if (w == nullptr)
        {
            w = windowMgr->Create<SceneryWindow>(WindowClass::scenery);
        }
        return w;
    }

    void WindowScenerySetSelectedItem(
        const ScenerySelection& scenery, const std::optional<Colour> primary, const std::optional<Colour> secondary,
        const std::optional<Colour> tertiary, const std::optional<uint8_t> rotation)
    {
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<SceneryWindow*>(windowMgr->BringToFrontByClass(WindowClass::scenery));
        if (w != nullptr)
        {
            w->setSelectedItem(scenery, primary, secondary, tertiary, rotation);
        }
    }

    void WindowScenerySetSelectedTab(const ObjectEntryIndex sceneryGroupIndex)
    {
        // Should this bring to front?
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<SceneryWindow*>(windowMgr->FindByClass(WindowClass::scenery));
        if (w != nullptr)
        {
            return w->setSelectedTab(sceneryGroupIndex);
        }
    }

    // Used after removing objects, in order to avoid crashes.
    void WindowSceneryResetSelectedSceneryItems()
    {
        _tabSelections.clear();
        _activeTabIndex = 0;
    }

    void WindowScenerySetDefaultPlacementConfiguration()
    {
        gWindowSceneryRotation = 3;
        _sceneryPrimaryColour = Colour::bordeauxRed;
        _scenerySecondaryColour = Colour::yellow;
        _sceneryTertiaryColour = Colour::darkBrown;

        WindowSceneryResetSelectedSceneryItems();
    }

    const ScenerySelection WindowSceneryGetTabSelection()
    {
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<SceneryWindow*>(windowMgr->FindByClass(WindowClass::scenery));
        if (w != nullptr)
        {
            return w->getTabSelection();
        }
        else
        {
            return {};
        }
    }

    void WindowSceneryInit()
    {
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<SceneryWindow*>(windowMgr->FindByClass(WindowClass::scenery));
        if (w != nullptr)
        {
            w->init();
        }
    }

    void ToggleSceneryWindow()
    {
        if (isToolActive(WindowClass::scenery, WIDX_SCENERY_BACKGROUND))
        {
            ToolCancel();
        }
        else
        {
            auto* toolWindow = ContextOpenWindow(WindowClass::scenery);
            ToolSet(*toolWindow, WIDX_SCENERY_BACKGROUND, Tool::arrow);
            gInputFlags.set(InputFlag::allowRightMouseRemoval);
        }
    }
} // namespace OpenRCT2::Ui::Windows
