/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <bit>
#include <cctype>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/Editor.h>
#include <openrct2/EditorObjectSelectionSession.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/LoadOrQuitAction.h>
#include <openrct2/actions/ResultWithMessage.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/object/ClimateObject.h>
#include <openrct2/object/MusicObject.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/object/RideObject.h>
#include <openrct2/object/SceneryGroupObject.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenes/title/TitleScene.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <span>
#include <string>
#include <vector>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    enum : uint16_t
    {
        FILTER_NONE = 0,

        FILTER_RCT1 = (1 << 0),
        FILTER_AA = (1 << 1),
        FILTER_LL = (1 << 2),
        FILTER_RCT2 = (1 << 3),
        FILTER_WW = (1 << 4),
        FILTER_TT = (1 << 5),
        FILTER_OO = (1 << 6),
        FILTER_CUSTOM = (1 << 7),

        FILTER_RIDE_TRANSPORT = (1 << 8),
        FILTER_RIDE_GENTLE = (1 << 9),
        FILTER_RIDE_COASTER = (1 << 10),
        FILTER_RIDE_THRILL = (1 << 11),
        FILTER_RIDE_WATER = (1 << 12),
        FILTER_RIDE_STALL = (1 << 13),

        FILTER_SELECTED = (1 << 14),
        FILTER_NONSELECTED = (1 << 15),

        FILTER_RIDES_ALL = FILTER_RIDE_TRANSPORT | FILTER_RIDE_GENTLE | FILTER_RIDE_COASTER | FILTER_RIDE_THRILL
            | FILTER_RIDE_WATER | FILTER_RIDE_STALL,
        FILTER_SOURCES_ALL = FILTER_RCT1 | FILTER_AA | FILTER_LL | FILTER_RCT2 | FILTER_WW | FILTER_TT | FILTER_OO
            | FILTER_CUSTOM,
        FILTER_ALL = FILTER_RIDES_ALL | FILTER_SOURCES_ALL | FILTER_SELECTED | FILTER_NONSELECTED,
    };

    enum
    {
        RIDE_SORT_TYPE,
        RIDE_SORT_RIDE
    };

    enum : uint8_t
    {
        DDIX_FILTER_RCT1,
        DDIX_FILTER_AA,
        DDIX_FILTER_LL,
        DDIX_FILTER_RCT2,
        DDIX_FILTER_WW,
        DDIX_FILTER_TT,
        DDIX_FILTER_OO,
        DDIX_FILTER_CUSTOM,
        DDIX_FILTER_SEPARATOR,
        DDIX_FILTER_SELECTED,
        DDIX_FILTER_NONSELECTED,
    };

    struct ObjectListItem
    {
        const ObjectRepositoryItem* repositoryItem;
        std::unique_ptr<RideFilters> filter;
        uint8_t* flags;
    };

    static constexpr uint8_t _numSourceGameItems = 8;

    static constexpr StringId kWindowTitle = STR_OBJECT_SELECTION;
    static constexpr ScreenSize kWindowSize = { 755, 400 };
    static constexpr ScreenSize kMinimumWindowSize = { 600, 400 };
    static constexpr ScreenSize kMaximumWindowSize = { 1200, 1000 };
    static constexpr auto kFilterWidth = 150;
    static constexpr auto kPreviewSize = 113;

    struct ObjectSubTab
    {
        StringId tooltip;
        ObjectType subObjectType;
        uint16_t flagFilter;
        uint32_t baseImage;
        uint8_t animationLength;
        uint8_t animationDivisor;
    };

    struct ObjectPageDesc
    {
        StringId Caption;
        ObjectType mainObjectType;
        uint32_t Image;
        std::span<ObjectSubTab> subTabs;
    };

    // clang-format off
    static ObjectSubTab kRideObjectSubTabs[] = {
        { STR_OBJECT_FILTER_ALL_RIDES_TIP, ObjectType::ride, FILTER_RIDES_ALL,      SPR_G2_INFINITY,                  1, 1 },
        { STR_TRANSPORT_RIDES_TIP,         ObjectType::ride, FILTER_RIDE_TRANSPORT, SPR_TAB_RIDES_TRANSPORT_0,       20, 4 },
        { STR_GENTLE_RIDES_TIP,            ObjectType::ride, FILTER_RIDE_GENTLE,    SPR_TAB_RIDES_GENTLE_0,          32, 8 },
        { STR_ROLLER_COASTERS_TIP,         ObjectType::ride, FILTER_RIDE_COASTER,   SPR_TAB_RIDES_ROLLER_COASTERS_0, 10, 2 },
        { STR_THRILL_RIDES_TIP,            ObjectType::ride, FILTER_RIDE_THRILL,    SPR_TAB_RIDES_THRILL_0,          72, 4 },
        { STR_WATER_RIDES_TIP,             ObjectType::ride, FILTER_RIDE_WATER,     SPR_TAB_RIDES_WATER_0,           24, 4 },
        { STR_SHOPS_STALLS_TIP,            ObjectType::ride, FILTER_RIDE_STALL,     SPR_TAB_RIDES_SHOP_0,            28, 4 },
    };

    static ObjectSubTab kSceneryObjectSubTabs[] = {
        { STR_OBJECT_SELECTION_SCENERY_GROUPS,    ObjectType::sceneryGroup,     FILTER_NONE, SPR_TAB_SCENERY_STATUES,    1, 1 },
        { STR_OBJECT_SELECTION_SMALL_SCENERY,     ObjectType::smallScenery,     FILTER_NONE, SPR_TAB_SCENERY_TREES,      1, 1 },
        { STR_OBJECT_SELECTION_LARGE_SCENERY,     ObjectType::largeScenery,     FILTER_NONE, SPR_TAB_SCENERY_URBAN,      1, 1 },
        { STR_OBJECT_SELECTION_WALLS_FENCES,      ObjectType::walls,            FILTER_NONE, SPR_TAB_SCENERY_WALLS,      1, 1 },
    };

    static ObjectSubTab kPathObjectSubTabs[] = {
        { STR_OBJECT_SELECTION_FOOTPATH_SURFACES, ObjectType::footpathSurface,  FILTER_NONE, SPR_G2_PATH_SURFACE_TAB,    1, 1 },
        { STR_OBJECT_SELECTION_FOOTPATH_RAILINGS, ObjectType::footpathRailings, FILTER_NONE, SPR_G2_PATH_RAILINGS_TAB,   1, 1 },
        { STR_OBJECT_SELECTION_FOOTPATHS,         ObjectType::paths,            FILTER_NONE, SPR_G2_LEGACY_PATH_TAB,     1, 1 },
        { STR_OBJECT_SELECTION_PATH_EXTRAS,       ObjectType::pathAdditions,    FILTER_NONE, SPR_TAB_SCENERY_PATH_ITEMS, 1, 1 },
        { STR_OBJECT_SELECTION_PATH_SIGNS,        ObjectType::banners,          FILTER_NONE, SPR_TAB_SCENERY_SIGNAGE,    1, 1 },
    };

    static ObjectSubTab kEntrancesObjectSubTabs[] = {
        { STR_OBJECT_SELECTION_PARK_ENTRANCE,     ObjectType::parkEntrance,     FILTER_NONE, SPR_TAB_PARK,            1, 1 },
        { STR_OBJECT_SELECTION_STATIONS,          ObjectType::station,          FILTER_NONE, SPR_G2_RIDE_STATION_TAB, 1, 1 },
    };

    static ObjectSubTab kTerrainObjectSubTabs[] = {
        { STR_OBJECT_SELECTION_TERRAIN_SURFACES,  ObjectType::terrainSurface,   FILTER_NONE, SPR_G2_TAB_LAND,         1, 1 },
        { STR_OBJECT_SELECTION_TERRAIN_EDGES,     ObjectType::terrainEdge,      FILTER_NONE, SPR_G2_TERRAIN_EDGE_TAB, 1, 1 },
        { STR_OBJECT_SELECTION_WATER,             ObjectType::water,            FILTER_NONE, SPR_TAB_WATER,           1, 1 },
        { STR_OBJECT_SELECTION_CLIMATE,           ObjectType::climate,          FILTER_NONE, SPR_WEATHER_SUN_CLOUD,   1, 1 },
    };

    static ObjectSubTab kPeepObjectSubTabs[] = {
        { STR_OBJECT_SELECTION_PEEP_ANIMATIONS,   ObjectType::peepAnimations,   FILTER_NONE, SPR_G2_PEEP_ANIMATIONS, 1, 1 },
        { STR_OBJECT_SELECTION_PEEP_NAMES,        ObjectType::peepNames,        FILTER_NONE, SPR_TAB_GUESTS_0,       1, 1 },
    };

    static constexpr ObjectPageDesc ObjectSelectionPages[] = {
        { STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS, ObjectType::ride,            SPR_TAB_RIDE_16,            kRideObjectSubTabs },
        { STR_OBJECT_SELECTION_SCENERY_AND_THEMES,        ObjectType::sceneryGroup,    SPR_TAB_SCENERY_STATUES,    kSceneryObjectSubTabs },
        { STR_OBJECT_SELECTION_FOOTPATH_SURFACES,         ObjectType::footpathSurface, SPR_G2_LEGACY_PATH_TAB,     kPathObjectSubTabs },
        { STR_OBJECT_SELECTION_ENTRANCE_BUILDINGS,        ObjectType::parkEntrance,    SPR_TAB_PARK,               kEntrancesObjectSubTabs },
        { STR_OBJECT_SELECTION_TERRAIN_SURFACES,          ObjectType::terrainSurface,  SPR_G2_MAP_GEN_TERRAIN_TAB, kTerrainObjectSubTabs },
        { STR_OBJECT_SELECTION_MUSIC,                     ObjectType::music,           SPR_TAB_MUSIC_0,            {} },
        { STR_OBJECT_SELECTION_GUESTS_AND_STAFF,          ObjectType::peepNames,       SPR_TAB_GUESTS_0,           kPeepObjectSubTabs },
    };
    // clang-format on

#pragma region Widgets

    enum WindowEditorObjectSelectionWidgetIndex
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_TAB_CONTENT_PANEL,
        WIDX_LIST,
        WIDX_PREVIEW,
        WIDX_INSTALL_TRACK,
        WIDX_FILTER_DROPDOWN,
        WIDX_FILTER_DROPDOWN_BTN,
        WIDX_FILTER_TEXT_BOX,
        WIDX_FILTER_CLEAR_BUTTON,
        WIDX_FILTER_RIDE_TAB_FRAME,
        WIDX_SUB_TAB_0,
        WIDX_SUB_TAB_1,
        WIDX_SUB_TAB_2,
        WIDX_SUB_TAB_3,
        WIDX_SUB_TAB_4,
        WIDX_SUB_TAB_5,
        WIDX_SUB_TAB_6,
        WIDX_LIST_SORT_TYPE,
        WIDX_LIST_SORT_RIDE,
        WIDX_RELOAD_OBJECT,
        WIDX_TAB_1,
    };

    VALIDATE_GLOBAL_WIDX(WC_EDITOR_OBJECT_SELECTION, WIDX_TAB_1);

    // clang-format off
    static constexpr auto _window_editor_object_selection_widgets = makeWidgets(
        makeWindowShim     (kWindowTitle, kWindowSize),
        makeWidget         ({  0, 43}, {kWindowSize.width, 357}, WidgetType::resize,       WindowColour::secondary                                                                 ),
        makeWidget         ({  4, 60}, {              288, 277}, WidgetType::scroll,       WindowColour::secondary, SCROLL_VERTICAL                                                ),
        makeWidget         ({391, 45}, {              114, 114}, WidgetType::flatBtn,      WindowColour::secondary                                                                 ),
        makeWidget         ({340, 22}, {              122,  14}, WidgetType::button,       WindowColour::primary,   STR_INSTALL_NEW_TRACK_DESIGN, STR_INSTALL_NEW_TRACK_DESIGN_TIP ),
        makeDropdownWidgets({470, 22}, {              114,  14}, WidgetType::dropdownMenu, WindowColour::primary,   STR_OBJECT_FILTER,            STR_OBJECT_FILTER_TIP            ),
        makeWidget         ({  4, 45}, {              211,  14}, WidgetType::textBox,      WindowColour::secondary                                                                 ),
        makeWidget         ({218, 45}, {               70,  14}, WidgetType::button,       WindowColour::secondary, STR_OBJECT_SEARCH_CLEAR                                        ),
        makeWidget         ({  3, 73}, {              285,   4}, WidgetType::imgBtn,       WindowColour::secondary                                                                 ),
        makeTab            ({  3, 47}                                                                                                                                              ),
        makeTab            ({ 34, 47}                                                                                                                                              ),
        makeTab            ({ 65, 47}                                                                                                                                              ),
        makeTab            ({ 96, 47}                                                                                                                                              ),
        makeTab            ({127, 47}                                                                                                                                              ),
        makeTab            ({158, 47}                                                                                                                                              ),
        makeTab            ({189, 47}                                                                                                                                              ),
        makeWidget         ({  4, 80}, {              145,  14}, WidgetType::tableHeader, WindowColour::secondary                                                                  ),
        makeWidget         ({149, 80}, {              143,  14}, WidgetType::tableHeader, WindowColour::secondary                                                                  ),
        makeWidget         ({700, 50}, {               24,  24}, WidgetType::imgBtn,      WindowColour::secondary,  SPR_G2_RELOAD,                STR_RELOAD_OBJECT_TIP            ),
        makeTab            ({  3, 17},                                                                                                            STR_STRING_DEFINED_TOOLTIP       )
        // Copied object type times...
    );
    // clang-format on

#pragma endregion

    static StringId GetRideTypeStringId(const ObjectRepositoryItem* item);
    static bool VisibleListSortRideType(const ObjectListItem& a, const ObjectListItem& b);
    static bool VisibleListSortRideName(const ObjectListItem& a, const ObjectListItem& b);
    void EditorLoadSelectedObjects();

    class EditorObjectSelectionWindow final : public Window
    {
        using sortFunc_t = bool (*)(const ObjectListItem&, const ObjectListItem&);

    private:
        std::vector<ObjectListItem> _listItems;
        int32_t _listSortType = RIDE_SORT_TYPE;
        bool _listSortDescending = false;
        std::unique_ptr<Object> _loadedObject;
        u8string _filter;
        uint32_t _filterFlags = FILTER_RIDES_ALL;
        uint8_t _selectedSubTab = 0;
        bool _overrideChecks = false;

    public:
        /**
         *
         * rct2: 0x006AA64E
         */
        void onOpen() override
        {
            initWidgets();

            Sub6AB211();
            ResetSelectedObjectCountAndSize();

            _filterFlags = FILTER_RIDES_ALL | Config::Get().interface.objectSelectionFilterFlags;
            _filter.clear();

            WindowInitScrollWidgets(*this);

            selectedTab = 0;
            selectedListItem = -1;

            WindowSetResize(*this, kMinimumWindowSize, kMaximumWindowSize);

            _listSortType = RIDE_SORT_TYPE;
            _listSortDescending = false;

            disabledWidgets |= 1u << WIDX_FILTER_RIDE_TAB_FRAME;

            VisibleListRefresh();
        }

        void SetOverrideChecks(bool newState)
        {
            _overrideChecks = newState;
        }

        bool canClose() override
        {
            // Prevent window closure when selection is invalid
            return _overrideChecks || EditorObjectSelectionWindowCheck();
        }

        /**
         *
         * rct2: 0x006AB199
         */
        void onClose() override
        {
            UnloadUnselectedObjects();
            EditorLoadSelectedObjects();
            EditorObjectFlagsClear();

            if (_loadedObject != nullptr)
                _loadedObject->Unload();

            if (isInEditorMode())
            {
                ResearchPopulateListRandom();
            }
            else
            {
                // Used for in-game object selection cheat
                // This resets the ride selection list and resets research to 0 on current item
                gSilentResearch = true;
                ResearchResetCurrentItem();
                gSilentResearch = false;
            }

            auto intent = Intent(INTENT_ACTION_REFRESH_NEW_RIDES);
            ContextBroadcastIntent(&intent);

            VisibleListClear();

            intent = Intent(INTENT_ACTION_REFRESH_SCENERY);
            ContextBroadcastIntent(&intent);
        }

        void onUpdate() override
        {
            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                invalidateWidget(WIDX_FILTER_TEXT_BOX);
            }

            auto& currentPage = ObjectSelectionPages[selectedTab];
            if (currentPage.subTabs.empty())
                return;

            if (_selectedSubTab == 0)
                return;

            auto& subTabDef = currentPage.subTabs[_selectedSubTab];

            currentFrame++;
            if (currentFrame >= subTabDef.animationLength)
                currentFrame = 0;

            invalidateWidget(WIDX_SUB_TAB_0 + _selectedSubTab);
        }

        /**
         *
         * rct2: 0x006AAFAB
         */
        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                {
                    if (gLegacyScene != LegacyScene::trackDesignsManager && !EditorObjectSelectionWindowCheck())
                        return;

                    auto* windowMgr = GetWindowManager();
                    windowMgr->CloseByClass(WindowClass::editorObjectSelection);

                    if (isInEditorMode())
                    {
                        FinishObjectSelection();
                    }
                    if (gLegacyScene == LegacyScene::trackDesignsManager)
                    {
                        GameNotifyMapChange();
                        GameUnloadScripts();

                        auto* context = GetContext();
                        context->SetActiveScene(context->GetTitleScene());
                    }
                    break;
                }

                case WIDX_SUB_TAB_0:
                case WIDX_SUB_TAB_1:
                case WIDX_SUB_TAB_2:
                case WIDX_SUB_TAB_3:
                case WIDX_SUB_TAB_4:
                case WIDX_SUB_TAB_5:
                case WIDX_SUB_TAB_6:
                {
                    _selectedSubTab = widgetIndex - WIDX_SUB_TAB_0;

                    auto& currentPage = ObjectSelectionPages[selectedTab];
                    auto& subTabDef = currentPage.subTabs[_selectedSubTab];
                    _filterFlags &= ~FILTER_RIDES_ALL;
                    _filterFlags |= subTabDef.flagFilter;

                    Config::Get().interface.objectSelectionFilterFlags = _filterFlags;
                    Config::Save();

                    VisibleListRefresh();

                    selectedListItem = -1;
                    scrolls[0].contentOffsetY = 0;
                    currentFrame = 0;
                    invalidate();
                    break;
                }

                case WIDX_INSTALL_TRACK:
                {
                    if (selectedListItem != -1)
                    {
                        selectedListItem = -1;
                    }
                    invalidate();

                    auto intent = Intent(WindowClass::loadsave);
                    intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::load);
                    intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::track);
                    ContextOpenIntent(&intent);
                    break;
                }
                case WIDX_FILTER_TEXT_BOX:
                    WindowStartTextbox(*this, widgetIndex, _filter, kTextInputSize);
                    break;
                case WIDX_FILTER_CLEAR_BUTTON:
                    _filter.clear();
                    scrolls->contentOffsetY = 0;
                    VisibleListRefresh();
                    invalidate();
                    break;
                case WIDX_LIST_SORT_TYPE:
                    if (_listSortType == RIDE_SORT_TYPE)
                    {
                        _listSortDescending = !_listSortDescending;
                    }
                    else
                    {
                        _listSortType = RIDE_SORT_TYPE;
                        _listSortDescending = false;
                    }
                    VisibleListRefresh();
                    break;
                case WIDX_LIST_SORT_RIDE:
                    if (_listSortType == RIDE_SORT_RIDE)
                    {
                        _listSortDescending = !_listSortDescending;
                    }
                    else
                    {
                        _listSortType = RIDE_SORT_RIDE;
                        _listSortDescending = false;
                    }
                    VisibleListRefresh();
                    break;
                case WIDX_RELOAD_OBJECT:
                    if (_loadedObject != nullptr)
                    {
                        auto descriptor = _loadedObject->GetDescriptor();
                        auto& objectManager = GetContext()->GetObjectManager();
                        auto entryIndex = objectManager.GetLoadedObjectEntryIndex(descriptor);
                        if (entryIndex != kObjectEntryIndexNull)
                        {
                            objectManager.UnloadObjects({ descriptor });
                            objectManager.LoadObject(descriptor, entryIndex);
                            GfxInvalidateScreen();
                        }
                    }
                    break;
                default:
                    if (widgetIndex >= WIDX_TAB_1
                        && static_cast<size_t>(widgetIndex) < WIDX_TAB_1 + std::size(ObjectSelectionPages))
                    {
                        setPage(widgetIndex - WIDX_TAB_1);
                    }
                    break;
            }
        }

        void onResize() override
        {
            WindowSetResize(*this, kMinimumWindowSize, kMaximumWindowSize);
        }

        static constexpr StringId kSourceStringIds[] = {
            STR_SCENARIO_CATEGORY_RCT1,          STR_SCENARIO_CATEGORY_RCT1_AA, STR_SCENARIO_CATEGORY_RCT1_LL,
            STR_ROLLERCOASTER_TYCOON_2_DROPDOWN, STR_OBJECT_FILTER_WW,          STR_OBJECT_FILTER_TT,
            STR_OBJECT_FILTER_OPENRCT2_OFFICIAL, STR_OBJECT_FILTER_CUSTOM,
        };

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            int32_t numSelectionItems = 0;

            switch (widgetIndex)
            {
                case WIDX_FILTER_DROPDOWN_BTN:
                    for (auto ddIdx = EnumValue(DDIX_FILTER_RCT1); ddIdx <= EnumValue(DDIX_FILTER_CUSTOM); ddIdx++)
                    {
                        gDropdown.items[ddIdx] = Dropdown::ToggleOption(kSourceStringIds[ddIdx]);
                    }

                    // Track designs manager cannot select multiple, so only show selection filters if not in track designs
                    // manager
                    if (!(gLegacyScene == LegacyScene::trackDesignsManager))
                    {
                        numSelectionItems = 3;
                        gDropdown.items[DDIX_FILTER_SEPARATOR] = Dropdown::Separator();

                        gDropdown.items[DDIX_FILTER_SELECTED] = Dropdown::ToggleOption(STR_SELECTED_ONLY);
                        gDropdown.items[DDIX_FILTER_NONSELECTED] = Dropdown::ToggleOption(STR_NON_SELECTED_ONLY);
                    }

                    auto& ddWidget = widgets[WIDX_FILTER_DROPDOWN];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget.left, windowPos.y + ddWidget.top }, ddWidget.height(),
                        colours[ddWidget.colour], Dropdown::Flag::StayOpen, _numSourceGameItems + numSelectionItems);

                    for (int32_t i = 0; i < _numSourceGameItems; i++)
                    {
                        if (_filterFlags & (1 << i))
                        {
                            gDropdown.items[i].setChecked(true);
                        }
                    }

                    if (!(gLegacyScene == LegacyScene::trackDesignsManager))
                    {
                        gDropdown.items[DDIX_FILTER_SELECTED].setChecked(IsFilterActive(FILTER_SELECTED));
                        gDropdown.items[DDIX_FILTER_NONSELECTED].setChecked(IsFilterActive(FILTER_NONSELECTED));
                    }
                    break;
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (dropdownIndex == -1)
                return;

            switch (widgetIndex)
            {
                case WIDX_FILTER_DROPDOWN_BTN:
                    if (dropdownIndex == DDIX_FILTER_SELECTED)
                    {
                        _filterFlags ^= FILTER_SELECTED;
                        _filterFlags &= ~FILTER_NONSELECTED;
                    }
                    else if (dropdownIndex == DDIX_FILTER_NONSELECTED)
                    {
                        _filterFlags ^= FILTER_NONSELECTED;
                        _filterFlags &= ~FILTER_SELECTED;
                    }
                    else
                    {
                        _filterFlags ^= (1 << dropdownIndex);
                    }
                    Config::Get().interface.objectSelectionFilterFlags = _filterFlags;
                    Config::Save();

                    scrolls->contentOffsetY = 0;

                    VisibleListRefresh();
                    invalidate();
                    break;
            }
        }

        /**
         *
         * rct2: 0x006AB031
         */
        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            const auto newHeight = static_cast<int32_t>(_listItems.size() * kScrollableRowHeight);
            return { 0, newHeight };
        }

        /**
         *
         * rct2: 0x006AB0B6
         */
        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            // Used for in-game object selection cheat to prevent crashing the game
            // when windows attempt to draw objects that don't exist any more
            auto* windowMgr = GetWindowManager();
            windowMgr->CloseAllExceptClass(WindowClass::editorObjectSelection);

            int32_t selected_object = GetObjectFromObjectSelection(GetSelectedObjectType(), screenCoords.y);
            if (selected_object == -1)
                return;

            ObjectListItem* listItem = &_listItems[selected_object];
            uint8_t object_selection_flags = *listItem->flags;
            if (object_selection_flags & ObjectSelectionFlags::Flag6)
                return;

            invalidate();

            const CursorState* state = ContextGetCursorState();
            Audio::Play(Audio::SoundId::click1, 0, state->position.x);

            if (gLegacyScene == LegacyScene::trackDesignsManager)
            {
                const auto objectSelectResult = WindowEditorObjectSelectionSelectObject(
                    0, { EditorInputFlag::select }, listItem->repositoryItem);
                if (!objectSelectResult.Successful)
                    return;

                if (_loadedObject != nullptr)
                {
                    _loadedObject->Unload();
                    _loadedObject = nullptr;
                }

                auto& objRepository = GetContext()->GetObjectRepository();
                _loadedObject = objRepository.LoadObject(listItem->repositoryItem);
                auto& objManager = GetContext()->GetObjectManager();
                objManager.LoadObject(_loadedObject.get()->GetIdentifier());

                windowMgr->ForceClose(WindowClass::editorObjectSelection);

                // This function calls window_track_list_open
                ManageTracks();
                return;
            }

            EditorInputFlags inputFlags = { EditorInputFlag::unk1, EditorInputFlag::selectObjectsInSceneryGroup };
            // If already selected
            if (!(object_selection_flags & ObjectSelectionFlags::Selected))
                inputFlags.set(EditorInputFlag::select);

            _gSceneryGroupPartialSelectError = std::nullopt;
            const auto objectSelectResult = WindowEditorObjectSelectionSelectObject(0, inputFlags, listItem->repositoryItem);
            if (!objectSelectResult.Successful)
            {
                StringId error_title = inputFlags.has(EditorInputFlag::select) ? STR_UNABLE_TO_SELECT_THIS_OBJECT
                                                                               : STR_UNABLE_TO_DE_SELECT_THIS_OBJECT;

                ContextShowError(error_title, objectSelectResult.Message, {});
                return;
            }

            if (IsFilterActive(FILTER_SELECTED) || IsFilterActive(FILTER_NONSELECTED))
            {
                VisibleListRefresh();
                invalidate();
            }

            if (_gSceneryGroupPartialSelectError.has_value())
            {
                const auto errorMessage = _gSceneryGroupPartialSelectError.value();
                if (errorMessage == STR_OBJECT_SELECTION_ERR_TOO_MANY_OF_TYPE_SELECTED)
                {
                    ContextShowError(
                        STR_WARNING_TOO_MANY_OBJECTS_SELECTED, STR_NOT_ALL_OBJECTS_IN_THIS_SCENERY_GROUP_COULD_BE_SELECTED, {});
                }
                else
                {
                    ContextShowError(
                        errorMessage, STR_NOT_ALL_OBJECTS_IN_THIS_SCENERY_GROUP_COULD_BE_SELECTED, Formatter::Common());
                }
            }
        }

        /**
         *
         * rct2: 0x006AB079
         */
        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            int32_t selectedObject = GetObjectFromObjectSelection(GetSelectedObjectType(), screenCoords.y);
            if (selectedObject != -1)
            {
                ObjectListItem* listItem = &_listItems[selectedObject];
                uint8_t objectSelectionFlags = *listItem->flags;
                if (objectSelectionFlags & ObjectSelectionFlags::Flag6)
                {
                    selectedObject = -1;
                }
            }
            if (selectedObject != selectedListItem)
            {
                selectedListItem = selectedObject;

                if (_loadedObject != nullptr)
                {
                    _loadedObject->Unload();
                    _loadedObject = nullptr;
                }

                if (selectedObject != -1)
                {
                    auto listItem = &_listItems[selectedObject];
                    auto& objRepository = GetContext()->GetObjectRepository();
                    _loadedObject = objRepository.LoadObject(listItem->repositoryItem);
                    if (_loadedObject != nullptr)
                    {
                        _loadedObject->Load();
                    }
                }

                invalidate();
            }
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            // ScrollPaint
            ScreenCoordsXY screenCoords;
            bool ridePage = (GetSelectedObjectType() == ObjectType::ride);

            auto paletteIndex = getColourMap(colours[1].colour).midLight;
            GfxClear(rt, paletteIndex);

            screenCoords.y = 0;
            for (size_t i = 0; i < _listItems.size(); i++)
            {
                const auto& listItem = _listItems[i];
                if (screenCoords.y + kScrollableRowHeight >= rt.y && screenCoords.y <= rt.y + rt.height)
                {
                    // Draw checkbox
                    if (!(gLegacyScene == LegacyScene::trackDesignsManager) && !(*listItem.flags & 0x20))
                        Rectangle::fillInset(
                            rt, { { 2, screenCoords.y }, { 11, screenCoords.y + 10 } }, colours[1],
                            Rectangle::BorderStyle::inset, Rectangle::FillBrightness::dark,
                            Rectangle::FillMode::dontLightenWhenInset);

                    // Highlight background
                    auto highlighted = i == static_cast<size_t>(selectedListItem)
                        && !(*listItem.flags & ObjectSelectionFlags::Flag6);
                    if (highlighted)
                    {
                        auto bottom = screenCoords.y + (kScrollableRowHeight - 1);
                        Rectangle::filter(rt, { 0, screenCoords.y, width, bottom }, FilterPaletteID::paletteDarken1);
                    }

                    // Draw checkmark
                    if (!(gLegacyScene == LegacyScene::trackDesignsManager)
                        && (*listItem.flags & ObjectSelectionFlags::Selected))
                    {
                        screenCoords.x = 2;
                        auto darkness = highlighted ? TextDarkness::extraDark : TextDarkness::dark;
                        auto colour2 = colours[1].withFlag(ColourFlag::translucent, false);
                        if (*listItem.flags & (ObjectSelectionFlags::InUse | ObjectSelectionFlags::AlwaysRequired))
                            colour2.flags.set(ColourFlag::inset, true);

                        DrawText(rt, screenCoords, { colour2, FontStyle::medium, darkness }, kCheckMarkString);
                    }

                    screenCoords.x = gLegacyScene == LegacyScene::trackDesignsManager ? 0 : 15;

                    utf8 itemBuffer[512]{};
                    auto bufferWithColour = strcpy(itemBuffer, highlighted ? "{WINDOW_COLOUR_2}" : "{BLACK}");
                    auto buffer = strchr(bufferWithColour, '\0');

                    Drawing::Colour colour = Drawing::Colour::black;
                    auto darkness = TextDarkness::regular;
                    if (*listItem.flags & ObjectSelectionFlags::Flag6)
                    {
                        colour = colours[1].colour;
                        darkness = TextDarkness::dark;
                    }

                    int32_t width_limit = widgets[WIDX_LIST].width() - 1 - screenCoords.x;

                    if (ridePage)
                    {
                        width_limit /= 2;
                        // Draw ride type
                        StringId rideTypeStringId = GetRideTypeStringId(listItem.repositoryItem);
                        String::safeUtf8Copy(buffer, LanguageGetString(rideTypeStringId), 256 - (buffer - bufferWithColour));
                        auto ft = Formatter();
                        ft.Add<const char*>(itemBuffer);
                        DrawTextEllipsised(
                            rt, screenCoords, width_limit - 15, STR_STRING, ft, { colour, FontStyle::medium, darkness });
                        screenCoords.x = widgets[WIDX_LIST_SORT_RIDE].left - widgets[WIDX_LIST].left;
                    }

                    // Draw text
                    String::safeUtf8Copy(buffer, listItem.repositoryItem->Name.c_str(), 256 - (buffer - bufferWithColour));
                    if (gLegacyScene == LegacyScene::trackDesignsManager)
                    {
                        while (*buffer != 0 && *buffer != 9)
                            buffer++;

                        *buffer = 0;
                    }
                    auto ft = Formatter();
                    ft.Add<const char*>(itemBuffer);
                    DrawTextEllipsised(rt, screenCoords, width_limit, STR_STRING, ft, { colour, FontStyle::medium, darkness });
                }
                screenCoords.y += kScrollableRowHeight;
            }
        }

        /**
         *
         * rct2: 0x006AB058
         */
        OpenRCT2String onTooltip(const WidgetIndex widgetIndex, const StringId fallback) override
        {
            if (widgetIndex >= WIDX_TAB_1 && static_cast<size_t>(widgetIndex) < WIDX_TAB_1 + std::size(ObjectSelectionPages))
            {
                auto ft = Formatter();
                ft.Add<StringId>(ObjectSelectionPages[(widgetIndex - WIDX_TAB_1)].Caption);
                return { fallback, ft };
            }
            return { fallback, {} };
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (widgetIndex != WIDX_FILTER_TEXT_BOX)
                return;

            if (text == _filter)
                return;

            _filter.assign(text);

            scrolls->contentOffsetY = 0;

            VisibleListRefresh();
            invalidate();
        }

        void onPrepareDraw() override
        {
            // Resize widgets
            widgets[WIDX_LIST].right = width - 309;
            widgets[WIDX_LIST].bottom = height - 14;
            widgets[WIDX_PREVIEW].left = width - 209;
            widgets[WIDX_PREVIEW].right = width - 96;
            widgets[WIDX_RELOAD_OBJECT].left = width - 9 - 24;
            widgets[WIDX_RELOAD_OBJECT].right = width - 9;

            auto& dropdownWidget = widgets[WIDX_FILTER_DROPDOWN];
            resizeDropdown(WIDX_FILTER_DROPDOWN, { width - kFilterWidth - 10, dropdownWidget.top }, { kFilterWidth, 14 });
            auto& installTrackWidget = widgets[WIDX_INSTALL_TRACK];
            installTrackWidget.moveToX(dropdownWidget.left - installTrackWidget.width() - 11);

            // Set pressed widgets
            pressedWidgets |= 1uLL << WIDX_PREVIEW;
            SetPressedTab();

            // Set window title and buttons
            auto& titleWidget = widgets[WIDX_TITLE];
            if (gLegacyScene == LegacyScene::trackDesignsManager)
            {
                titleWidget.text = STR_TRACK_DESIGNS_MANAGER_SELECT_RIDE_TYPE;
                installTrackWidget.type = WidgetType::button;
            }
            else if (gLegacyScene == LegacyScene::trackDesigner)
            {
                titleWidget.text = STR_ROLLER_COASTER_DESIGNER_SELECT_RIDE_TYPES_VEHICLES;
                installTrackWidget.type = WidgetType::empty;
            }
            else
            {
                titleWidget.text = STR_OBJECT_SELECTION;
                installTrackWidget.type = WidgetType::empty;
            }

            // Set title parameters for current page
            const auto& currentPage = ObjectSelectionPages[selectedTab];
            auto ft = Formatter::Common();
            if (!currentPage.subTabs.empty())
                ft.Add<StringId>(currentPage.subTabs[_selectedSubTab].tooltip);
            else
                ft.Add<StringId>(currentPage.Caption);

            // Set filter dropdown caption
            if (!IsFilterActive(FILTER_SOURCES_ALL))
            {
                // only one source active?
                uint32_t sources = _filterFlags & FILTER_SOURCES_ALL;
                auto numSourcesActive = std::popcount(sources);
                if (numSourcesActive == 1)
                {
                    widgets[WIDX_FILTER_DROPDOWN].text = STR_OBJECT_SELECTION_ONLY_STRINGID;
                    auto firstActiveSource = Numerics::bitScanForward(sources);
                    ft.Add<StringId>(kSourceStringIds[firstActiveSource]);
                }
                else
                {
                    widgets[WIDX_FILTER_DROPDOWN].text = STR_OBJECT_SELECTION_SHOWING_N_SOURCES;
                    ft.Add<uint16_t>(numSourcesActive);
                }
            }
            else
                widgets[WIDX_FILTER_DROPDOWN].text = STR_OBJECT_SELECTION_ALL_SOURCES_SHOWN;

            // Align main tabs
            int32_t x = 3;
            for (size_t i = 0; i < std::size(ObjectSelectionPages); i++)
            {
                auto& widget = widgets[WIDX_TAB_1 + i];
                if (ObjectSelectionPages[i].Image != kImageIndexUndefined)
                {
                    widget.type = WidgetType::tab;
                    widget.left = x;
                    widget.right = x + 30;
                    x += 31;
                }
                else
                    widget.type = WidgetType::empty;
            }

            if (Config::Get().general.debuggingTools)
                widgets[WIDX_RELOAD_OBJECT].type = WidgetType::imgBtn;
            else
                widgets[WIDX_RELOAD_OBJECT].type = WidgetType::empty;

            if (gLegacyScene == LegacyScene::trackDesignsManager || gLegacyScene == LegacyScene::trackDesigner)
            {
                for (size_t i = 1; i < std::size(ObjectSelectionPages); i++)
                {
                    widgets[WIDX_TAB_1 + i].type = WidgetType::empty;
                }
            }

            // Do we have any sub-tabs?
            const bool hasSubTabs = !currentPage.subTabs.empty();

            widgets[WIDX_LIST].right = width / 2 - 2;
            widgets[WIDX_FILTER_RIDE_TAB_FRAME].right = widgets[WIDX_LIST].right;

            widgets[WIDX_FILTER_TEXT_BOX].right = widgets[WIDX_LIST].right - 77;
            auto contentStart = widgets[WIDX_TAB_CONTENT_PANEL].top;
            widgets[WIDX_FILTER_TEXT_BOX].moveToY(contentStart + (hasSubTabs ? 36 : 5));
            widgets[WIDX_FILTER_TEXT_BOX].string = _filter.data();

            widgets[WIDX_FILTER_CLEAR_BUTTON].left = widgets[WIDX_LIST].right - 73;
            widgets[WIDX_FILTER_CLEAR_BUTTON].right = widgets[WIDX_LIST].right;
            widgets[WIDX_FILTER_CLEAR_BUTTON].moveToY(contentStart + (hasSubTabs ? 36 : 5));

            // Toggle sub-tab visibility
            const auto numSubTabs = static_cast<int8_t>(currentPage.subTabs.size());
            for (int8_t i = 0; i <= 6; i++)
            {
                widgets[WIDX_SUB_TAB_0 + i].tooltip = i < numSubTabs ? currentPage.subTabs[i].tooltip : kStringIdNone;
                widgets[WIDX_SUB_TAB_0 + i].type = i < numSubTabs ? WidgetType::tab : WidgetType::empty;
                pressedWidgets &= ~(1uLL << (WIDX_SUB_TAB_0 + i));
            }

            // Mark current sub-tab as active, and toggle tab frame
            if (hasSubTabs)
            {
                pressedWidgets |= (1uLL << (WIDX_SUB_TAB_0 + _selectedSubTab));
                widgets[WIDX_FILTER_RIDE_TAB_FRAME].type = WidgetType::imgBtn;
            }
            else
                widgets[WIDX_FILTER_RIDE_TAB_FRAME].type = WidgetType::empty;

            // The ride tab has two headers for the list
            bool isRideTab = GetSelectedObjectType() == ObjectType::ride;
            if (isRideTab)
            {
                int32_t width_limit = (widgets[WIDX_LIST].width() - 16) / 2;

                widgets[WIDX_LIST_SORT_TYPE].type = WidgetType::tableHeader;
                widgets[WIDX_LIST_SORT_TYPE].top = widgets[WIDX_FILTER_TEXT_BOX].bottom + 3;
                widgets[WIDX_LIST_SORT_TYPE].bottom = widgets[WIDX_LIST_SORT_TYPE].top + 13;
                widgets[WIDX_LIST_SORT_TYPE].left = 4;
                widgets[WIDX_LIST_SORT_TYPE].right = widgets[WIDX_LIST_SORT_TYPE].left + width_limit;

                widgets[WIDX_LIST_SORT_RIDE].type = WidgetType::tableHeader;
                widgets[WIDX_LIST_SORT_RIDE].top = widgets[WIDX_LIST_SORT_TYPE].top;
                widgets[WIDX_LIST_SORT_RIDE].bottom = widgets[WIDX_LIST_SORT_TYPE].bottom;
                widgets[WIDX_LIST_SORT_RIDE].left = widgets[WIDX_LIST_SORT_TYPE].right + 1;
                widgets[WIDX_LIST_SORT_RIDE].right = widgets[WIDX_LIST].right;

                widgets[WIDX_LIST].top = widgets[WIDX_LIST_SORT_TYPE].bottom + 2;
            }
            else
            {
                widgets[WIDX_LIST_SORT_TYPE].type = WidgetType::empty;
                widgets[WIDX_LIST_SORT_RIDE].type = WidgetType::empty;

                widgets[WIDX_LIST].top = widgets[WIDX_FILTER_TEXT_BOX].bottom + 2;
            }

            widgets[WIDX_PREVIEW].top = widgets[WIDX_FILTER_TEXT_BOX].top;
            widgets[WIDX_PREVIEW].bottom = widgets[WIDX_PREVIEW].top + kPreviewSize;
            widgets[WIDX_PREVIEW].left = width * 3 / 4 - kPreviewSize / 2;
            widgets[WIDX_PREVIEW].right = widgets[WIDX_PREVIEW].left + kPreviewSize;
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);

            // Draw main tab images
            for (size_t i = 0; i < std::size(ObjectSelectionPages); i++)
            {
                const auto& widget = widgets[WIDX_TAB_1 + i];
                if (widget.type != WidgetType::empty)
                {
                    auto image = ImageId(ObjectSelectionPages[i].Image);
                    auto screenPos = windowPos + ScreenCoordsXY{ widget.left, widget.top };
                    GfxDrawSprite(rt, image, screenPos);
                }
            }

            constexpr int32_t kThrillRidesTabAnimationSequence[] = {
                5, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0,
            };

            // Draw sub-tab images, if applicable
            auto& currentPage = ObjectSelectionPages[selectedTab];
            if (!currentPage.subTabs.empty())
            {
                for (auto i = 0u; i < currentPage.subTabs.size(); i++)
                {
                    const auto& widget = widgets[WIDX_SUB_TAB_0 + i];
                    if (widget.type == WidgetType::empty)
                        continue;

                    auto& subTabDef = currentPage.subTabs[i];
                    int32_t frame = 0;
                    if (subTabDef.animationLength > 1 && _selectedSubTab == i)
                    {
                        frame = currentFrame / subTabDef.animationDivisor;
                    }

                    // TODO: generalise this?
                    int32_t spriteIndex = subTabDef.baseImage;
                    if (currentPage.Caption == STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS && i == 4)
                        spriteIndex += kThrillRidesTabAnimationSequence[frame];
                    else
                        spriteIndex += frame;

                    auto screenPos = windowPos + ScreenCoordsXY{ widget.left, widget.top };
                    if (spriteIndex == SPR_WEATHER_SUN_CLOUD)
                        screenPos += { 2, 4 };

                    GfxDrawSprite(rt, ImageId(spriteIndex, colours[1].colour), screenPos);
                }
            }

            // Preview background
            const auto& previewWidget = widgets[WIDX_PREVIEW];
            Rectangle::fill(
                rt,
                { windowPos + ScreenCoordsXY{ previewWidget.left + 1, previewWidget.top + 1 },
                  windowPos + ScreenCoordsXY{ previewWidget.right - 1, previewWidget.bottom - 1 } },
                getColourMap(colours[1].colour).darkest);

            // Draw number of selected items
            if (!(gLegacyScene == LegacyScene::trackDesignsManager))
            {
                auto screenPos = windowPos + ScreenCoordsXY{ 3, height - 13 };

                auto numSelected = _numSelectedObjectsForType[EnumValue(GetSelectedObjectType())];
                auto totalSelectable = getObjectEntryGroupCount(GetSelectedObjectType());

                auto ft = Formatter();
                ft.Add<uint16_t>(numSelected);
                ft.Add<uint16_t>(totalSelectable);
                DrawTextBasic(rt, screenPos, STR_OBJECT_SELECTION_SELECTION_SIZE, ft);
            }

            // Draw sort button text
            const auto& listSortTypeWidget = widgets[WIDX_LIST_SORT_TYPE];
            if (listSortTypeWidget.type != WidgetType::empty)
            {
                auto ft = Formatter();
                auto stringId = _listSortType == RIDE_SORT_TYPE ? static_cast<StringId>(_listSortDescending ? STR_DOWN : STR_UP)
                                                                : kStringIdNone;
                ft.Add<StringId>(stringId);
                auto screenPos = windowPos + ScreenCoordsXY{ listSortTypeWidget.left + 1, listSortTypeWidget.top + 1 };
                DrawTextEllipsised(rt, screenPos, listSortTypeWidget.width() - 1, STR_OBJECTS_SORT_TYPE, ft, { colours[1] });
            }
            const auto& listSortRideWidget = widgets[WIDX_LIST_SORT_RIDE];
            if (listSortRideWidget.type != WidgetType::empty)
            {
                auto ft = Formatter();
                auto stringId = _listSortType == RIDE_SORT_RIDE ? static_cast<StringId>(_listSortDescending ? STR_DOWN : STR_UP)
                                                                : kStringIdNone;
                ft.Add<StringId>(stringId);
                auto screenPos = windowPos + ScreenCoordsXY{ listSortRideWidget.left + 1, listSortRideWidget.top + 1 };
                DrawTextEllipsised(rt, screenPos, listSortRideWidget.width() - 1, STR_OBJECTS_SORT_RIDE, ft, { colours[1] });
            }

            if (selectedListItem == -1 || _loadedObject == nullptr)
                return;

            // Draw preview
            {
                RenderTarget clipRT;
                auto screenPos = windowPos + ScreenCoordsXY{ previewWidget.left + 1, previewWidget.top + 1 };
                int32_t previewWidth = previewWidget.width() - 2;
                int32_t previewHeight = previewWidget.height() - 2;
                if (ClipRenderTarget(clipRT, rt, screenPos, previewWidth, previewHeight))
                {
                    _loadedObject->DrawPreview(clipRT, previewWidth, previewHeight);
                }
            }

            DrawDescriptions(rt);
            DrawDebugData(rt);
        }

        void GoToTab(ObjectType objectType)
        {
            for (auto offset = 0u; offset < std::size(ObjectSelectionPages); offset++)
            {
                if (ObjectSelectionPages[offset].mainObjectType == objectType)
                {
                    setPage(offset);
                    return;
                }
            }
        }

    private:
        bool tabWidgetsInitialised = false;

        void initWidgets()
        {
            setWidgets(_window_editor_object_selection_widgets);
            if (!tabWidgetsInitialised)
            {
                // Create a new tab widget based on the initial one
                auto tabWidget = widgets[WIDX_TAB_1];
                for (size_t i = 1; i < std::size(ObjectSelectionPages); i++)
                {
                    widgets.insert(widgets.end() - 1, tabWidget);
                }

                tabWidgetsInitialised = true;
            }
        }

        void setPage(int32_t _page)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (selectedTab == _page && !widgets.empty())
                return;

            selectedTab = _page;
            _selectedSubTab = 0;
            _filterFlags |= FILTER_RIDES_ALL;
            selectedListItem = -1;
            scrolls[0].contentOffsetY = 0;
            currentFrame = 0;

            if (_page == EnumValue(ObjectType::ride))
            {
                _listSortType = RIDE_SORT_TYPE;
                _listSortDescending = false;
            }
            else
            {
                _listSortType = RIDE_SORT_RIDE;
                _listSortDescending = false;
            }

            VisibleListRefresh();
            invalidate();
        }

        void VisibleListRefresh()
        {
            int32_t numObjects = static_cast<int32_t>(ObjectRepositoryGetItemsCount());

            VisibleListClear();
            selectedListItem = -1;

            const ObjectRepositoryItem* items = ObjectRepositoryGetItems();
            for (int32_t i = 0; i < numObjects; i++)
            {
                uint8_t selectionFlags = _objectSelectionFlags[i];
                const ObjectRepositoryItem* item = &items[i];
                if (item->Type == GetSelectedObjectType() && !(selectionFlags & ObjectSelectionFlags::Flag6)
                    && FilterSource(item) && FilterString(*item) && FilterChunks(item) && FilterSelected(selectionFlags)
                    && FilterCompatibilityObject(*item, selectionFlags))
                {
                    auto filter = std::make_unique<RideFilters>();
                    filter->category[0] = 0;
                    filter->category[1] = 0;
                    filter->ride_type = 0;

                    ObjectListItem currentListItem;
                    currentListItem.repositoryItem = item;
                    currentListItem.filter = std::move(filter);
                    currentListItem.flags = &_objectSelectionFlags[i];
                    _listItems.push_back(std::move(currentListItem));
                }
            }

            if (_listItems.empty())
            {
                VisibleListClear();
            }
            else
            {
                sortFunc_t sortFunc = nullptr;
                switch (_listSortType)
                {
                    case RIDE_SORT_TYPE:
                        sortFunc = VisibleListSortRideType;
                        break;
                    case RIDE_SORT_RIDE:
                        sortFunc = VisibleListSortRideName;
                        break;
                    default:
                        LOG_WARNING("Wrong sort type %d, leaving list as-is.", _listSortType);
                        break;
                }
                if (sortFunc != nullptr)
                {
                    std::sort(_listItems.begin(), _listItems.end(), sortFunc);
                    if (_listSortDescending)
                    {
                        std::reverse(_listItems.begin(), _listItems.end());
                    }
                }
            }
            invalidate();
        }

        void VisibleListClear()
        {
            _listItems.clear();
            _listItems.shrink_to_fit();
        }

        void DrawDescriptions(RenderTarget& rt)
        {
            auto screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_PREVIEW].midX(), widgets[WIDX_PREVIEW].bottom + 3 };
            auto descriptionWidth = width - widgets[WIDX_LIST].right - 12;

            // Draw name of object
            {
                ObjectListItem* listItem = &_listItems[selectedListItem];

                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(listItem->repositoryItem->Name.c_str());
                screenPos.y += DrawTextWrapped(
                    rt, screenPos, descriptionWidth, STR_WINDOW_COLOUR_2_STRINGID, ft, { TextAlignment::centre });

                // Leave some space between name and description
                screenPos.y += kListRowHeight;
            }

            screenPos.x = windowPos.x + widgets[WIDX_LIST].right + 6;

            // Compatibility object?
            if (_loadedObject->IsCompatibilityObject())
            {
                screenPos.y += DrawTextWrapped(
                                   rt, screenPos, descriptionWidth, STR_OBJECT_SELECTION_COMPAT_OBJECT_DESCRIPTION, {},
                                   { Drawing::Colour::brightRed })
                    + kListRowHeight;
            }

            auto description = ObjectGetDescription(_loadedObject.get());
            if (!description.empty())
            {
                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(description.c_str());

                screenPos.y += DrawTextWrapped(rt, screenPos, descriptionWidth, STR_WINDOW_COLOUR_2_STRINGID, ft);
                screenPos.y += kListRowHeight;
            }

            if (GetSelectedObjectType() == ObjectType::ride)
            {
                auto* rideObject = reinterpret_cast<RideObject*>(_loadedObject.get());
                const auto& rideEntry = rideObject->GetEntry();
                if (rideEntry.shop_item[0] != ShopItem::none)
                {
                    std::string sells = "";
                    for (size_t i = 0; i < std::size(rideEntry.shop_item); i++)
                    {
                        if (rideEntry.shop_item[i] == ShopItem::none)
                            continue;

                        if (!sells.empty())
                            sells += ", ";

                        sells += LanguageGetString(GetShopItemDescriptor(rideEntry.shop_item[i]).Naming.Plural);
                    }
                    auto ft = Formatter();
                    ft.Add<const char*>(sells.c_str());
                    screenPos.y += DrawTextWrapped(rt, screenPos, descriptionWidth, STR_RIDE_OBJECT_SHOP_SELLS, ft) + 2;
                }
            }
            else if (GetSelectedObjectType() == ObjectType::sceneryGroup)
            {
                const auto* sceneryGroupObject = reinterpret_cast<SceneryGroupObject*>(_loadedObject.get());
                auto ft = Formatter();
                ft.Add<uint16_t>(sceneryGroupObject->GetNumIncludedObjects());
                screenPos.y += DrawTextWrapped(rt, screenPos, descriptionWidth, STR_INCLUDES_X_OBJECTS, ft) + 2;
            }
            else if (GetSelectedObjectType() == ObjectType::music)
            {
                screenPos.y += DrawTextWrapped(rt, screenPos, descriptionWidth, STR_MUSIC_OBJECT_TRACK_HEADER) + 2;
                const auto* musicObject = reinterpret_cast<MusicObject*>(_loadedObject.get());
                for (size_t i = 0; i < musicObject->GetTrackCount(); i++)
                {
                    const auto* track = musicObject->GetTrack(i);
                    if (track->Name.empty())
                        continue;

                    auto stringId = track->Composer.empty() ? STR_MUSIC_OBJECT_TRACK_LIST_ITEM
                                                            : STR_MUSIC_OBJECT_TRACK_LIST_ITEM_WITH_COMPOSER;
                    auto ft = Formatter();
                    ft.Add<const char*>(track->Name.c_str());
                    ft.Add<const char*>(track->Composer.c_str());
                    screenPos.y += DrawTextWrapped(rt, screenPos + ScreenCoordsXY{ 10, 0 }, descriptionWidth, stringId, ft);
                }
            }
        }

        StringId GetAnimationPeepTypeStringId(AnimationPeepType type)
        {
            switch (type)
            {
                case AnimationPeepType::handyman:
                    return STR_HANDYMAN_PLURAL;
                case AnimationPeepType::mechanic:
                    return STR_MECHANIC_PLURAL;
                case AnimationPeepType::security:
                    return STR_SECURITY_GUARD_PLURAL;
                case AnimationPeepType::entertainer:
                    return STR_ENTERTAINER_PLURAL;
                case AnimationPeepType::guest:
                default:
                    return STR_GUESTS;
            }
        }

        void DrawDebugData(RenderTarget& rt)
        {
            ObjectListItem* listItem = &_listItems[selectedListItem];
            auto screenPos = windowPos + ScreenCoordsXY{ width - 5, height - (kListRowHeight * 6) };

            // Draw fallback image warning
            if (_loadedObject && _loadedObject->UsesFallbackImages())
            {
                DrawTextBasic(
                    rt, screenPos, STR_OBJECT_USES_FALLBACK_IMAGES, {}, { Drawing::Colour::white, TextAlignment::right });
            }
            screenPos.y += kListRowHeight;

            // Draw ride type.
            if (GetSelectedObjectType() == ObjectType::ride)
            {
                auto stringId = GetRideTypeStringId(listItem->repositoryItem);
                DrawTextBasic(rt, screenPos, stringId, {}, { Drawing::Colour::white, TextAlignment::right });
            }

            // Draw peep animation object type
            if (GetSelectedObjectType() == ObjectType::peepAnimations)
            {
                auto* animObj = reinterpret_cast<PeepAnimationsObject*>(_loadedObject.get());
                auto stringId = GetAnimationPeepTypeStringId(animObj->GetPeepType());
                DrawTextBasic(rt, screenPos, stringId, {}, { Drawing::Colour::white, TextAlignment::right });
            }

            screenPos.y += kListRowHeight;

            // Draw object source
            auto stringId = ObjectManagerGetSourceGameString(listItem->repositoryItem->GetFirstSourceGame());
            DrawTextBasic(rt, screenPos, stringId, {}, { Drawing::Colour::white, TextAlignment::right });
            screenPos.y += kListRowHeight;

            // Draw object filename
            {
                auto path = Path::GetFileName(listItem->repositoryItem->Path);
                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const utf8*>(path.c_str());
                DrawTextBasic(
                    rt, { windowPos.x + this->width - 5, screenPos.y }, STR_WINDOW_COLOUR_2_STRINGID, ft,
                    { Drawing::Colour::black, TextAlignment::right });
                screenPos.y += kListRowHeight;
            }

            // Draw object author (will be blank space if no author in file or a non JSON object)
            {
                auto ft = Formatter();
                std::string authorsString;
                for (size_t i = 0; i < listItem->repositoryItem->Authors.size(); i++)
                {
                    if (i > 0)
                    {
                        authorsString.append(", ");
                    }
                    authorsString.append(listItem->repositoryItem->Authors[i]);
                }
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(authorsString.c_str());
                DrawTextEllipsised(
                    rt, { windowPos.x + width - 5, screenPos.y }, width - widgets[WIDX_LIST].right - 4,
                    STR_WINDOW_COLOUR_2_STRINGID, ft, { TextAlignment::right });
            }
        }

        bool FilterSelected(uint8_t objectFlag)
        {
            // Track Manager has no concept of selection filtering, so always return true
            if (gLegacyScene == LegacyScene::trackDesignsManager)
            {
                return true;
            }
            if (IsFilterActive(FILTER_SELECTED) == IsFilterActive(FILTER_NONSELECTED))
            {
                return true;
            }
            if (IsFilterActive(FILTER_SELECTED) && (objectFlag & ObjectSelectionFlags::Selected))
            {
                return true;
            }
            if (IsFilterActive(FILTER_NONSELECTED) && !(objectFlag & ObjectSelectionFlags::Selected))
            {
                return true;
            }

            return false;
        }

        bool FilterCompatibilityObject(const ObjectRepositoryItem& item, uint8_t objectFlag)
        {
            // only show compat objects if they are selected already.
            return !(item.Flags & ObjectItemFlags::IsCompatibilityObject) || (objectFlag & ObjectSelectionFlags::Selected);
        }

        bool IsFilterActive(const uint16_t filter) const
        {
            return (_filterFlags & filter) == filter;
        }

        static bool IsFilterInName(const ObjectRepositoryItem& item, std::string_view filter)
        {
            return String::contains(item.Name, filter, true);
        }

        static bool IsFilterInRideType(const ObjectRepositoryItem& item, std::string_view filter)
        {
            if (item.Type == ObjectType::ride)
            {
                auto rideTypeName = LanguageGetString(GetRideTypeStringId(&item));
                if (String::contains(rideTypeName, filter, true))
                    return true;
            }
            return false;
        }

        static bool IsFilterInFilename(const ObjectRepositoryItem& item, std::string_view filter)
        {
            return String::contains(item.Path, filter, true);
        }

        static bool IsFilterInAuthor(const ObjectRepositoryItem& item, std::string_view filter)
        {
            for (auto& author : item.Authors)
            {
                if (String::contains(author, filter, true))
                {
                    return true;
                }
            }
            return false;
        }

        bool FilterString(const ObjectRepositoryItem& item)
        {
            // Nothing to search for
            if (_filter.empty())
                return true;

            return IsFilterInName(item, _filter) || IsFilterInRideType(item, _filter) || IsFilterInFilename(item, _filter)
                || IsFilterInAuthor(item, _filter);
        }

        bool SourcesMatch(ObjectSourceGame source)
        {
            // clang-format off
            return (IsFilterActive(FILTER_RCT1) && source == ObjectSourceGame::rct1) ||
                   (IsFilterActive(FILTER_AA)   && source == ObjectSourceGame::addedAttractions) ||
                   (IsFilterActive(FILTER_LL)   && source == ObjectSourceGame::loopyLandscapes) ||
                   (IsFilterActive(FILTER_RCT2) && source == ObjectSourceGame::rct2) ||
                   (IsFilterActive(FILTER_WW)   && source == ObjectSourceGame::wackyWorlds) ||
                   (IsFilterActive(FILTER_TT)   && source == ObjectSourceGame::timeTwister) ||
                   (IsFilterActive(FILTER_OO)   && source == ObjectSourceGame::openRCT2Official) ||
                   (IsFilterActive(FILTER_CUSTOM) &&
                        source != ObjectSourceGame::rct1 &&
                        source != ObjectSourceGame::addedAttractions &&
                        source != ObjectSourceGame::loopyLandscapes &&
                        source != ObjectSourceGame::rct2 &&
                        source != ObjectSourceGame::wackyWorlds &&
                        source != ObjectSourceGame::timeTwister &&
                        source != ObjectSourceGame::openRCT2Official);
            // clang-format on
        }

        bool FilterSource(const ObjectRepositoryItem* item)
        {
            if (IsFilterActive(FILTER_ALL))
                return true;

            for (auto source : item->Sources)
            {
                if (SourcesMatch(source))
                    return true;
            }

            return false;
        }

        bool FilterChunks(const ObjectRepositoryItem* item)
        {
            if (item->Type == ObjectType::ride)
            {
                ride_type_t rideType = 0;
                for (int32_t i = 0; i < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; i++)
                {
                    if (item->RideInfo.RideType[i] != kRideTypeNull)
                    {
                        rideType = item->RideInfo.RideType[i];
                        break;
                    }
                }
                return (_filterFlags & (1 << (EnumValue(GetRideTypeDescriptor(rideType).Category) + _numSourceGameItems))) != 0;
            }
            return true;
        }

        std::string ObjectGetDescription(const Object* object)
        {
            switch (object->GetObjectType())
            {
                case ObjectType::ride:
                {
                    const RideObject* rideObject = static_cast<const RideObject*>(object);
                    return rideObject->GetDescription();
                }
                default:
                    return {};
            }
        }

        ObjectType GetSelectedObjectType()
        {
            auto& currentPage = ObjectSelectionPages[selectedTab];
            auto& subTabs = currentPage.subTabs;
            if (!subTabs.empty())
                return subTabs[_selectedSubTab].subObjectType;
            else
                return currentPage.mainObjectType;
        }

        /**
         * Takes the y coordinate of the clicked on scroll list
         * and converts this into an object selection.
         * Returns the position in the list.
         * Object_selection_flags, installed_entry also populated
         *
         * rct2: 0x006AA703
         */
        int32_t GetObjectFromObjectSelection(ObjectType object_type, int32_t y)
        {
            int32_t listItemIndex = y / kScrollableRowHeight;
            if (listItemIndex < 0 || static_cast<size_t>(listItemIndex) >= _listItems.size())
                return -1;

            return listItemIndex;
        }

        void SetPressedTab()
        {
            for (size_t i = 0; i < std::size(ObjectSelectionPages); i++)
            {
                pressedWidgets &= ~(1ull << (WIDX_TAB_1 + i));
            }
            pressedWidgets |= 1LL << (WIDX_TAB_1 + selectedTab);
        }

        /**
         *
         * rct2: 0x006D33E2
         */
        void ManageTracks()
        {
            SetEveryRideTypeInvented();
            SetEveryRideEntryInvented();

            getGameState().editorStep = EditorStep::DesignsManager;

            int32_t entry_index = 0;
            for (; ObjectEntryGetChunk(ObjectType::ride, entry_index) == nullptr; entry_index++)
                ;

            const auto* rideEntry = GetRideEntryByIndex(entry_index);
            auto rideType = rideEntry->GetFirstNonNullRideType();

            auto intent = Intent(WindowClass::trackDesignList);
            intent.PutExtra(INTENT_EXTRA_RIDE_TYPE, rideType);
            intent.PutExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX, entry_index);
            ContextOpenIntent(&intent);
        }
    };

    /**
     *
     * rct2: 0x006AA64E
     */
    WindowBase* EditorObjectSelectionOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<EditorObjectSelectionWindow>(
            WindowClass::editorObjectSelection, kWindowSize,
            { WindowFlag::higherContrastOnPress, WindowFlag::resizable, WindowFlag::centreScreen });
    }

    // Used for forced closure
    void EditorObjectSelectionClose()
    {
        auto* windowMgr = GetWindowManager();
        auto window = windowMgr->FindByClass(WindowClass::editorObjectSelection);
        if (window == nullptr)
        {
            return;
        }
        auto objSelWindow = static_cast<EditorObjectSelectionWindow*>(window);
        objSelWindow->SetOverrideChecks(true);
        objSelWindow->close();
    }

    static bool VisibleListSortRideName(const ObjectListItem& a, const ObjectListItem& b)
    {
        auto nameA = a.repositoryItem->Name.c_str();
        auto nameB = b.repositoryItem->Name.c_str();
        return strcmp(nameA, nameB) < 0;
    }

    static bool VisibleListSortRideType(const ObjectListItem& a, const ObjectListItem& b)
    {
        auto rideTypeA = LanguageGetString(GetRideTypeStringId(a.repositoryItem));
        auto rideTypeB = LanguageGetString(GetRideTypeStringId(b.repositoryItem));
        int32_t result = String::compare(rideTypeA, rideTypeB);
        return result != 0 ? result < 0 : VisibleListSortRideName(a, b);
    }

    static StringId GetRideTypeStringId(const ObjectRepositoryItem* item)
    {
        StringId result = kStringIdNone;
        for (int32_t i = 0; i < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; i++)
        {
            auto rideType = item->RideInfo.RideType[i];
            if (rideType != kRideTypeNull)
            {
                result = GetRideTypeDescriptor(rideType).Naming.Name;
                break;
            }
        }
        return result;
    }

    /**
     *
     * rct2: 0x006ABBBE
     */
    void EditorLoadSelectedObjects()
    {
        auto& objManager = GetContext()->GetObjectManager();
        int32_t numItems = static_cast<int32_t>(ObjectRepositoryGetItemsCount());
        const ObjectRepositoryItem* items = ObjectRepositoryGetItems();
        bool showFallbackWarning = false;
        for (int32_t i = 0; i < numItems; i++)
        {
            if (_objectSelectionFlags[i] & ObjectSelectionFlags::Selected)
            {
                const auto* item = &items[i];
                auto descriptor = ObjectEntryDescriptor(*item);
                const auto* loadedObject = objManager.GetLoadedObject(descriptor);
                if (loadedObject == nullptr)
                {
                    loadedObject = objManager.LoadObject(descriptor);
                    if (loadedObject == nullptr)
                    {
                        LOG_ERROR("Failed to load entry %s", std::string(descriptor.GetName()).c_str());
                    }
                    else if (!isInEditorMode())
                    {
                        // Defaults selected items to researched (if in-game)
                        auto objectType = loadedObject->GetObjectType();
                        auto entryIndex = ObjectManagerGetLoadedObjectEntryIndex(loadedObject);
                        if (objectType == ObjectType::ride)
                        {
                            const auto* rideEntry = GetRideEntryByIndex(entryIndex);
                            auto rideType = rideEntry->GetFirstNonNullRideType();
                            ResearchCategory category = static_cast<ResearchCategory>(GetRideTypeDescriptor(rideType).Category);
                            ResearchInsertRideEntry(rideType, entryIndex, category, true);
                        }
                        else if (objectType == ObjectType::sceneryGroup)
                        {
                            ResearchInsertSceneryGroupEntry(entryIndex, true);
                        }
                        if (loadedObject->UsesFallbackImages())
                        {
                            showFallbackWarning = true;
                        }
                    }
                }
            }
        }
        if (_numSelectedObjectsForType[EnumValue(ObjectType::water)] == 0)
        {
            // Reloads the default cyan water palette if no palette was selected.
            LoadPalette();
        }
        if (showFallbackWarning)
            ContextShowError(STR_OBJECT_SELECTION_FALLBACK_IMAGES_WARNING, kStringIdEmpty, Formatter::Common());
    }

    bool EditorObjectSelectionWindowCheck()
    {
        auto [missingObjectType, errorString] = Editor::CheckObjectSelection();
        if (missingObjectType == ObjectType::none)
        {
            return true;
        }

        ContextShowError(STR_INVALID_SELECTION_OF_OBJECTS, errorString, {});

        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::editorObjectSelection);
        if (w != nullptr)
        {
            // Click tab with missing object
            static_cast<EditorObjectSelectionWindow*>(w)->GoToTab(missingObjectType);
        }
        return false;
    }
} // namespace OpenRCT2::Ui::Windows
