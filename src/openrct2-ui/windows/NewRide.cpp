/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <iterator>
#include <limits>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/ride/Construction.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/management/Research.h>
#include <openrct2/network/network.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/object/RideObject.h>
#include <openrct2/rct1/RCT1.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>

using namespace OpenRCT2::TrackMetaData;

static constexpr const StringId WindowTitle = STR_NONE;
static constexpr const int32_t WindowHeight = 382;
static constexpr const int32_t WindowWidth = 601;
static constexpr const int32_t RideListItemsMax = 384;
static constexpr const int32_t RideTabCount = 6;
static constexpr const int32_t GroupByTrackTypeWidth = 172;

#pragma region Ride type view order

/**
 * The order of ride types shown in the new ride window so that the order stays consistent across games and rides of the same
 * type are kept together.
 */
static constexpr const ride_type_t RideTypeViewOrder[] = {
    // Transport rides
    RIDE_TYPE_MINIATURE_RAILWAY,
    RIDE_TYPE_MONORAIL,
    RIDE_TYPE_SUSPENDED_MONORAIL,
    RIDE_TYPE_CHAIRLIFT,
    RIDE_TYPE_LIFT,

    // Roller Coasters
    RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER,
    RIDE_TYPE_VIRGINIA_REEL,
    RIDE_TYPE_REVERSER_ROLLER_COASTER,
    RIDE_TYPE_CLASSIC_WOODEN_ROLLER_COASTER,
    RIDE_TYPE_WOODEN_ROLLER_COASTER,
    RIDE_TYPE_WOODEN_WILD_MOUSE,
    RIDE_TYPE_STEEL_WILD_MOUSE,
    RIDE_TYPE_SPINNING_WILD_MOUSE,
    RIDE_TYPE_INVERTED_HAIRPIN_COASTER,
    RIDE_TYPE_JUNIOR_ROLLER_COASTER,
    RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER,
    RIDE_TYPE_MINI_ROLLER_COASTER,
    RIDE_TYPE_SPIRAL_ROLLER_COASTER,
    RIDE_TYPE_MINE_TRAIN_COASTER,
    RIDE_TYPE_LOOPING_ROLLER_COASTER,
    RIDE_TYPE_STAND_UP_ROLLER_COASTER,
    RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
    RIDE_TYPE_HYPERCOASTER,
    RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER,
    RIDE_TYPE_TWISTER_ROLLER_COASTER,
    RIDE_TYPE_HYPER_TWISTER,
    RIDE_TYPE_GIGA_COASTER,
    RIDE_TYPE_SUSPENDED_SWINGING_COASTER,
    RIDE_TYPE_COMPACT_INVERTED_COASTER,
    RIDE_TYPE_INVERTED_ROLLER_COASTER,
    RIDE_TYPE_INVERTED_IMPULSE_COASTER,
    RIDE_TYPE_MINI_SUSPENDED_COASTER,
    RIDE_TYPE_STEEPLECHASE,
    RIDE_TYPE_BOBSLEIGH_COASTER,
    RIDE_TYPE_MINE_RIDE,
    RIDE_TYPE_HEARTLINE_TWISTER_COASTER,
    RIDE_TYPE_LAY_DOWN_ROLLER_COASTER,
    RIDE_TYPE_FLYING_ROLLER_COASTER,
    RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER,
    RIDE_TYPE_REVERSE_FREEFALL_COASTER,
    RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER,
    RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER,
    RIDE_TYPE_HYBRID_COASTER,
    RIDE_TYPE_SINGLE_RAIL_ROLLER_COASTER,
    RIDE_TYPE_ALPINE_COASTER,

    // Gentle rides
    RIDE_TYPE_MONORAIL_CYCLES,
    RIDE_TYPE_CROOKED_HOUSE,
    RIDE_TYPE_HAUNTED_HOUSE,
    RIDE_TYPE_FERRIS_WHEEL,
    RIDE_TYPE_MAZE,
    RIDE_TYPE_MERRY_GO_ROUND,
    RIDE_TYPE_MINI_GOLF,
    RIDE_TYPE_OBSERVATION_TOWER,
    RIDE_TYPE_CAR_RIDE,
    RIDE_TYPE_MONSTER_TRUCKS,
    RIDE_TYPE_MINI_HELICOPTERS,
    RIDE_TYPE_SPIRAL_SLIDE,
    RIDE_TYPE_DODGEMS,
    RIDE_TYPE_SPACE_RINGS,
    RIDE_TYPE_CIRCUS,
    RIDE_TYPE_GHOST_TRAIN,
    RIDE_TYPE_FLYING_SAUCERS,

    // Thrill rides
    RIDE_TYPE_TWIST,
    RIDE_TYPE_MAGIC_CARPET,
    RIDE_TYPE_LAUNCHED_FREEFALL,
    RIDE_TYPE_SWINGING_SHIP,
    RIDE_TYPE_GO_KARTS,
    RIDE_TYPE_SWINGING_INVERTER_SHIP,
    RIDE_TYPE_MOTION_SIMULATOR,
    RIDE_TYPE_3D_CINEMA,
    RIDE_TYPE_TOP_SPIN,
    RIDE_TYPE_ROTO_DROP,
    RIDE_TYPE_ENTERPRISE,

    // Water rides
    RIDE_TYPE_DINGHY_SLIDE,
    RIDE_TYPE_LOG_FLUME,
    RIDE_TYPE_RIVER_RAPIDS,
    RIDE_TYPE_SPLASH_BOATS,
    RIDE_TYPE_SUBMARINE_RIDE,
    RIDE_TYPE_BOAT_HIRE,
    RIDE_TYPE_RIVER_RAFTS,
    RIDE_TYPE_WATER_COASTER,

    // Shops / stalls
    RIDE_TYPE_FOOD_STALL,
    RIDE_TYPE_1D,
    RIDE_TYPE_DRINK_STALL,
    RIDE_TYPE_1F,
    RIDE_TYPE_SHOP,
    RIDE_TYPE_22,
    RIDE_TYPE_INFORMATION_KIOSK,
    RIDE_TYPE_FIRST_AID,
    RIDE_TYPE_CASH_MACHINE,
    RIDE_TYPE_TOILETS,
};

#pragma endregion

enum NewRideTabId
{
    TRANSPORT_TAB,
    GENTLE_TAB,
    ROLLER_COASTER_TAB,
    THRILL_TAB,
    WATER_TAB,
    SHOP_TAB,
    RESEARCH_TAB,
    TAB_COUNT,
};

#pragma region Widgets

enum
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_TAB_3,
    WIDX_TAB_4,
    WIDX_TAB_5,
    WIDX_TAB_6,
    WIDX_TAB_7,
    WIDX_RIDE_LIST,

    WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP,
    WIDX_LAST_DEVELOPMENT_GROUP,
    WIDX_LAST_DEVELOPMENT_BUTTON,
    WIDX_RESEARCH_FUNDING_BUTTON,

    WIDX_FILTER_TEXT_BOX,
    WIDX_FILTER_CLEAR_BUTTON,
    WIDX_GROUP_BY_TRACK_TYPE,
};

static constexpr const ScreenCoordsXY GroupByTrackTypeOrigin{ WindowWidth - 8 - GroupByTrackTypeWidth, 47 };
static constexpr const ScreenSize GroupTrackTypeSize{ GroupByTrackTypeWidth, 14 };

// clang-format off
static Widget window_new_ride_widgets[] = {
    WINDOW_SHIM(WindowTitle, WindowWidth, WindowHeight),
    MakeWidget({  0,  43},             {601, 339},         WindowWidgetType::Resize,   WindowColour::Secondary                                                                ),
    MakeTab   ({  3,  17},                                                                                      STR_TRANSPORT_RIDES_TIP                                       ),
    MakeTab   ({ 34,  17},                                                                                      STR_GENTLE_RIDES_TIP                                          ),
    MakeTab   ({ 65,  17},                                                                                      STR_ROLLER_COASTERS_TIP                                       ),
    MakeTab   ({ 96,  17},                                                                                      STR_THRILL_RIDES_TIP                                          ),
    MakeTab   ({127,  17},                                                                                      STR_WATER_RIDES_TIP                                           ),
    MakeTab   ({158,  17},                                                                                      STR_SHOPS_STALLS_TIP                                          ),
    MakeTab   ({189,  17},                                                                                      STR_RESEARCH_AND_DEVELOPMENT_TIP                              ),
    MakeWidget({  3,  62},             {595, 256},         WindowWidgetType::Scroll,   WindowColour::Secondary, SCROLL_VERTICAL                                               ),
    MakeWidget({  3,  47},             {290,  70},         WindowWidgetType::Groupbox, WindowColour::Tertiary,  STR_CURRENTLY_IN_DEVELOPMENT                                  ),
    MakeWidget({  3, 124},             {290,  65},         WindowWidgetType::Groupbox, WindowColour::Tertiary,  STR_LAST_DEVELOPMENT                                          ),
    MakeWidget({265, 161},             { 24,  24},         WindowWidgetType::FlatBtn,  WindowColour::Tertiary,  0xFFFFFFFF,                      STR_RESEARCH_SHOW_DETAILS_TIP),
    MakeWidget({265,  68},             { 24,  24},         WindowWidgetType::FlatBtn,  WindowColour::Tertiary,  ImageId(SPR_FINANCE),                     STR_FINANCES_RESEARCH_TIP    ),
    MakeWidget({  4,  46},             {211, 14},          WindowWidgetType::TextBox,  WindowColour::Secondary                          ),
    MakeWidget({218,  46},             { 70, 14},          WindowWidgetType::Button,   WindowColour::Secondary, STR_OBJECT_SEARCH_CLEAR ),
    MakeWidget(GroupByTrackTypeOrigin, GroupTrackTypeSize, WindowWidgetType::Checkbox, WindowColour::Secondary, STR_GROUP_BY_TRACK_TYPE,         STR_GROUP_BY_TRACK_TYPE_TIP  ),
    WIDGETS_END,
};
// clang-format on

#pragma endregion

static constexpr const StringId RideTitles[] = {
    STR_NEW_TRANSPORT_RIDES,      // TRANSPORT_TAB
    STR_NEW_GENTLE_RIDES,         // GENTLE_TAB
    STR_NEW_ROLLER_COASTERS,      // ROLLER_COASTER_TAB
    STR_NEW_THRILL_RIDES,         // THRILL_TAB
    STR_NEW_WATER_RIDES,          // WATER_TAB
    STR_NEW_SHOPS_STALLS,         // SHOP_TAB
    STR_RESEARCH_AND_DEVELOPMENT, // RESEARCH_TAB
};
static_assert(std::size(RideTitles) == TAB_COUNT);

static constexpr const int32_t TabAnimationLoops[] = {
    20, // TRANSPORT_TAB
    32, // GENTLE_TAB
    10, // ROLLER_COASTER_TAB
    72, // THRILL_TAB
    24, // WATER_TAB
    28, // SHOP_TAB
    16, // RESEARCH_TAB
};
static_assert(std::size(TabAnimationLoops) == TAB_COUNT);

static constexpr const int32_t TabAnimationDivisor[] = {
    4, // TRANSPORT_TAB
    8, // GENTLE_TAB
    2, // ROLLER_COASTER_TAB
    4, // THRILL_TAB
    4, // WATER_TAB
    4, // SHOP_TAB
    2, // RESEARCH_TAB
};
static_assert(std::size(TabAnimationDivisor) == TAB_COUNT);

static constexpr const int32_t ThrillRidesTabAnimationSequence[] = {
    5, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0,
};

class NewRideWindow final : public Window
{
private:
    static NewRideTabId _currentTab;
    static uint_fast16_t _windowNewRideTabScroll[RideTabCount];
    u8string _filter;
    u8string _vehicleAvailability{};
    RideSelection _lastTrackDesignCountRideType{};
    int32_t _lastTrackDesignCount{};
    RideSelection _windowNewRideListItems[RideListItemsMax]{};

public:
    static void SetOpeningPage(NewRideTabId tab)
    {
        _currentTab = tab;
    }

    static void ResetTabScrolls()
    {
        std::fill_n(NewRideWindow::_windowNewRideTabScroll, RideTabCount, 0);
    }

    void OnOpen() override
    {
        widgets = window_new_ride_widgets;
        PopulateRideList();
        InitScrollWidgets();
        _filter.clear();

        frame_no = 0;
        new_ride.SelectedRide = { RIDE_TYPE_NULL, OBJECT_ENTRY_INDEX_NULL };
        _lastTrackDesignCountRideType.Type = RIDE_TYPE_NULL;
        _lastTrackDesignCountRideType.EntryIndex = OBJECT_ENTRY_INDEX_NULL;

        width = 1;
        RefreshWidgetSizing();

        if (_currentTab < RESEARCH_TAB)
        {
            SetPage(_currentTab);
            RestoreScrollPositionForCurrentTab();
        }
    }

    void OnUpdate() override
    {
        frame_no++;
        if (frame_no >= TabAnimationLoops[_currentTab])
            frame_no = 0;

        WidgetInvalidate(*this, WIDX_TAB_1 + static_cast<int32_t>(_currentTab));

        if (gCurrentTextBox.window.classification == classification && gCurrentTextBox.window.number == number)
        {
            WindowUpdateTextboxCaret();
            WidgetInvalidate(*this, WIDX_FILTER_TEXT_BOX);
        }

        if (new_ride.SelectedRide.Type != RIDE_TYPE_NULL && new_ride.selected_ride_countdown-- == 0)
        {
            RideSelect();
        }

        PopulateRideList();

        if (_currentTab < RESEARCH_TAB)
        {
            _windowNewRideTabScroll[_currentTab] = scrolls[0].v_top;

            // Remove highlight when mouse leaves rides list
            if (!WidgetIsHighlighted(*this, WIDX_RIDE_LIST))
            {
                new_ride.HighlightedRide = { RIDE_TYPE_NULL, OBJECT_ENTRY_INDEX_NULL };
                WidgetInvalidate(*this, WIDX_RIDE_LIST);
            }
        }
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_LAST_DEVELOPMENT_BUTTON:
                News::OpenSubject(News::ItemType::Research, gResearchLastItem->rawValue);
                break;
            case WIDX_RESEARCH_FUNDING_BUTTON:
                ContextOpenWindowView(WV_FINANCES_RESEARCH);
                break;
            case WIDX_GROUP_BY_TRACK_TYPE:
                gConfigInterface.ListRideVehiclesSeparately = !gConfigInterface.ListRideVehiclesSeparately;
                ConfigSaveDefault();
                SetPage(_currentTab);
                break;
            case WIDX_FILTER_TEXT_BOX:
                WindowStartTextbox(*this, widgetIndex, STR_STRING, _filter.data(), MAX_PATH);
                break;
            case WIDX_FILTER_CLEAR_BUTTON:
                _filter.clear();
                scrolls->v_top = 0;
                Invalidate();
                break;
        }
    }

    void OnMouseDown(WidgetIndex widgetIndex) override
    {
        if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
        {
            SetPage(widgetIndex - WIDX_TAB_1);
        }
    }

    void OnPrepareDraw() override
    {
        SetPressedTab();

        if (!gConfigInterface.ListRideVehiclesSeparately)
            pressed_widgets |= (1LL << WIDX_GROUP_BY_TRACK_TYPE);
        else
            pressed_widgets &= ~(1LL << WIDX_GROUP_BY_TRACK_TYPE);

        widgets[WIDX_TITLE].text = RideTitles[_currentTab];
        widgets[WIDX_TAB_7].type = WindowWidgetType::Tab;
        widgets[WIDX_FILTER_TEXT_BOX].string = _filter.data();

        if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
            widgets[WIDX_TAB_7].type = WindowWidgetType::Empty;

        if (_currentTab == RESEARCH_TAB)
        {
            widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WindowWidgetType::Empty;
            if (gResearchLastItem.has_value())
            {
                auto type = gResearchLastItem->type;
                widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WindowWidgetType::FlatBtn;
                const auto index = (type == Research::EntryType::Ride) ? SPR_NEW_RIDE : SPR_NEW_SCENERY;
                widgets[WIDX_LAST_DEVELOPMENT_BUTTON].image = ImageId(index);
            }
        }

        const auto& ls = OpenRCT2::GetContext()->GetLocalisationService();
        auto string = ls.GetString(STR_GROUP_BY_TRACK_TYPE);
        auto strWidth = GfxGetStringWidth(string, FontStyle::Medium);
        auto localizedGroupByTrackTypeWidth = strWidth + 14;
        widgets[WIDX_GROUP_BY_TRACK_TYPE].left = width - 8 - localizedGroupByTrackTypeWidth;
    }

    void OnDraw(DrawPixelInfo& dpi) override
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        if (_currentTab != RESEARCH_TAB)
        {
            RideSelection item = new_ride.HighlightedRide;
            if (item.Type != RIDE_TYPE_NULL || item.EntryIndex != OBJECT_ENTRY_INDEX_NULL)
                DrawRideInformation(dpi, item, windowPos + ScreenCoordsXY{ 3, height - 64 }, width - 6);
        }
        else
        {
            WindowResearchDevelopmentPagePaint(this, dpi, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
        }
    }

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        RideSelection* listItem = _windowNewRideListItems;

        int32_t count = 0;
        while (listItem->Type != RIDE_TYPE_NULL || listItem->EntryIndex != OBJECT_ENTRY_INDEX_NULL)
        {
            count++;
            listItem++;
        }
        return { widgets[WIDX_RIDE_LIST].width(), ((count + 4) / 5) * 116 };
    }

    void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        RideSelection item = ScrollGetRideListItemAt(screenCoords);
        if (new_ride.HighlightedRide == item)
        {
            return;
        }

        new_ride.HighlightedRide = item;
        Invalidate();
    }

    void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        RideSelection item = ScrollGetRideListItemAt(screenCoords);
        if (item.Type == RIDE_TYPE_NULL && item.EntryIndex == OBJECT_ENTRY_INDEX_NULL)
        {
            return;
        }

        new_ride.SelectedRide = item;

        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, windowPos.x + (width / 2));
        new_ride.selected_ride_countdown = 8;
        Invalidate();
    }

    void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
    {
        if (_currentTab == RESEARCH_TAB)
        {
            return;
        }

        GfxClear(&dpi, ColourMapA[colours[1]].mid_light);

        ScreenCoordsXY coords{ 1, 1 };
        RideSelection* listItem = _windowNewRideListItems;
        while (listItem->Type != RIDE_TYPE_NULL || listItem->EntryIndex != OBJECT_ENTRY_INDEX_NULL)
        {
            // Draw flat button rectangle
            int32_t buttonFlags = 0;
            if (new_ride.SelectedRide == *listItem)
                buttonFlags |= INSET_RECT_FLAG_BORDER_INSET;
            if (new_ride.HighlightedRide == *listItem || buttonFlags != 0)
                GfxFillRectInset(
                    dpi, { coords, coords + ScreenCoordsXY{ 115, 115 } }, colours[1],
                    INSET_RECT_FLAG_FILL_MID_LIGHT | buttonFlags);

            // Draw ride image with feathered border
            auto mask = ImageId(SPR_NEW_RIDE_MASK);
            auto rideImage = ImageId(GetRideImage(*listItem));
            GfxDrawSpriteRawMasked(&dpi, coords + ScreenCoordsXY{ 2, 2 }, mask, rideImage);

            // Next position
            coords.x += 116;
            if (coords.x >= 116 * 5 + 1)
            {
                coords.x = 1;
                coords.y += 116;
            }

            // Next item
            listItem++;
        }
    }

    void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
    {
        if (widgetIndex != WIDX_FILTER_TEXT_BOX)
            return;

        if (text == _filter)
            return;

        _filter.assign(text);

        scrolls->v_top = 0;
        Invalidate();
    }

    void SetPage(int tab)
    {
        if (tab >= TRANSPORT_TAB && tab < TAB_COUNT)
        {
            SetPage(static_cast<NewRideTabId>(tab));
        }
    }

    void SetPage(NewRideTabId tab)
    {
        _currentTab = tab;
        frame_no = 0;
        new_ride.HighlightedRide = { RIDE_TYPE_NULL, OBJECT_ENTRY_INDEX_NULL };
        new_ride.selected_ride_countdown = std::numeric_limits<uint16_t>::max();
        PopulateRideList();
        RefreshWidgetSizing();
        Invalidate();

        if (tab < RESEARCH_TAB)
        {
            RestoreScrollPositionForCurrentTab();
        }
    }

private:
    void RideSelect()
    {
        RideSelection item = new_ride.SelectedRide;
        if (item.Type == RIDE_TYPE_NULL)
        {
            return;
        }

        Close();
        WindowCloseConstructionWindows();

        auto count = GetNumTrackDesigns(item);
        if (count > 0)
        {
            auto intent = Intent(WindowClass::TrackDesignList);
            intent.PutExtra(INTENT_EXTRA_RIDE_TYPE, item.Type);
            intent.PutExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX, item.EntryIndex);
            ContextOpenIntent(&intent);
            return;
        }

        RideConstructNew(item);
    }

    int32_t GetNumTrackDesigns(RideSelection item)
    {
        // Cache the result
        if (item.Type == _lastTrackDesignCountRideType.Type && item.EntryIndex == _lastTrackDesignCountRideType.EntryIndex)
        {
            return _lastTrackDesignCount;
        }

        std::string entryName;

        if (item.Type < 0x80)
        {
            if (GetRideTypeDescriptor(item.Type).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
            {
                entryName = GetRideEntryName(item.EntryIndex);
            }
        }

        auto repo = OpenRCT2::GetContext()->GetTrackDesignRepository();
        _lastTrackDesignCount = static_cast<int32_t>(repo->GetCountForObjectEntry(item.Type, entryName));
        _lastTrackDesignCountRideType = item;
        return _lastTrackDesignCount;
    }

    void UpdateVehicleAvailability(ObjectEntryIndex rideType)
    {
        _vehicleAvailability.clear();
        if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
        {
            return;
        }

        auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
        auto& rideEntries = objManager.GetAllRideEntries(rideType);
        auto isFirst = true;
        for (auto rideEntryIndex : rideEntries)
        {
            auto currentRideEntry = GetRideEntryByIndex(rideEntryIndex);

            // Skip if vehicle type is not invented yet
            if (!RideEntryIsInvented(rideEntryIndex) && !gCheatsIgnoreResearchStatus)
            {
                continue;
            }

            // Append comma if not the first iteration
            if (!isFirst)
            {
                _vehicleAvailability += u8", ";
            }

            // Append vehicle name
            auto vehicleName = LanguageGetString(currentRideEntry->naming.Name);
            _vehicleAvailability += vehicleName;

            isFirst = false;
        }
    }

    ImageIndex GetRideImage(RideSelection rideSelection)
    {
        auto& objMgr = OpenRCT2::GetContext()->GetObjectManager();
        auto obj = static_cast<RideObject*>(objMgr.GetLoadedObject(ObjectType::Ride, rideSelection.EntryIndex));
        return obj == nullptr ? ImageIndexUndefined : obj->GetPreviewImage(rideSelection.Type);
    }

    void PopulateRideList()
    {
        uint8_t currentCategory = _currentTab;
        RideSelection* nextListItem = _windowNewRideListItems;
        RideSelection* listEnd = _windowNewRideListItems + RideListItemsMax;

        // For each ride type in the view order list
        for (int32_t i = 0; i < static_cast<int32_t>(std::size(RideTypeViewOrder)); i++)
        {
            auto rideType = RideTypeViewOrder[i];
            if (rideType == RIDE_TYPE_NULL)
                continue;

            if (GetRideTypeDescriptor(rideType).Category != currentCategory)
                continue;

            nextListItem = IterateOverRideType(rideType, nextListItem, listEnd);
        }

        nextListItem->Type = RIDE_TYPE_NULL;
        nextListItem->EntryIndex = OBJECT_ENTRY_INDEX_NULL;
    }

    RideSelection* IterateOverRideType(ride_type_t rideType, RideSelection* nextListItem, RideSelection* listEnd)
    {
        bool buttonForRideTypeCreated = false;
        bool allowDrawingOverLastButton = false;

        uint8_t highestVehiclePriority = 0;

        // For each ride entry for this ride type
        auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
        auto& rideEntries = objManager.GetAllRideEntries(rideType);
        for (auto rideEntryIndex : rideEntries)
        {
            // Skip if vehicle type is not invented yet
            if (!RideEntryIsInvented(rideEntryIndex) && !gCheatsIgnoreResearchStatus)
                continue;

            // Ride entries
            const auto* rideEntry = GetRideEntryByIndex(rideEntryIndex);

            // Skip if the vehicle isn't the preferred vehicle for this generic track type
            if (!gConfigInterface.ListRideVehiclesSeparately
                && !GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY)
                && highestVehiclePriority > rideEntry->BuildMenuPriority)
            {
                continue;
            }

            if (!IsFiltered(*rideEntry))
            {
                continue;
            }

            highestVehiclePriority = rideEntry->BuildMenuPriority;

            // Determines how and where to draw a button for this ride type/vehicle.
            if (gConfigInterface.ListRideVehiclesSeparately
                || GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
            {
                // Separate, draw apart
                allowDrawingOverLastButton = false;

                if (nextListItem >= listEnd)
                    continue;

                nextListItem->Type = rideType;
                nextListItem->EntryIndex = rideEntryIndex;
                nextListItem++;
            }
            else if (!buttonForRideTypeCreated)
            {
                // Non-separate, draw-apart
                buttonForRideTypeCreated = true;
                allowDrawingOverLastButton = true;

                if (nextListItem >= listEnd)
                    continue;

                nextListItem->Type = rideType;
                nextListItem->EntryIndex = rideEntryIndex;
                nextListItem++;
            }
            else if (allowDrawingOverLastButton)
            {
                // Non-separate, draw over previous
                if (rideType == rideEntry->ride_type[0])
                {
                    nextListItem--;
                    nextListItem->Type = rideType;
                    nextListItem->EntryIndex = rideEntryIndex;
                    nextListItem++;
                }
            }
        }

        return nextListItem;
    }

    bool IsFiltered(const RideObjectEntry& rideEntry)
    {
        if (_filter.empty())
            return true;

        return IsFilterInRideType(rideEntry) || IsFilterInRideName(rideEntry) || IsFilterInIdentifier(rideEntry)
            || IsFilterInAuthors(rideEntry) || IsFilterInFilename(rideEntry);
    }

    bool IsFilterInRideType(const RideObjectEntry& rideEntry)
    {
        auto rideTypeName = GetRideNaming(rideEntry.ride_type[0], rideEntry).Name;
        return String::Contains(u8string_view(LanguageGetString(rideTypeName)), _filter, true);
    }

    bool IsFilterInRideName(const RideObjectEntry& rideEntry)
    {
        auto rideName = rideEntry.naming.Name;
        return String::Contains(u8string_view(LanguageGetString(rideName)), _filter, true);
    }

    bool IsFilterInAuthors(const RideObjectEntry& rideEntry)
    {
        auto rideObject = static_cast<RideObject*>(rideEntry.obj);
        auto authors = rideObject->GetAuthors();

        for (auto author : authors)
            if (String::Contains(author, _filter, true))
                return true;

        return false;
    }

    bool IsFilterInIdentifier(const RideObjectEntry& rideEntry)
    {
        auto rideObject = static_cast<RideObject*>(rideEntry.obj);
        auto objectName = rideObject->GetObjectEntry().GetName();

        return String::Contains(objectName, _filter, true);
    }

    bool IsFilterInFilename(const RideObjectEntry& rideEntry)
    {
        auto rideObject = static_cast<RideObject*>(rideEntry.obj);
        auto repoItem = ObjectRepositoryFindObjectByEntry(&(rideObject->GetObjectEntry()));

        return String::Contains(repoItem->Path, _filter, true);
    }

    void SetPressedTab()
    {
        int32_t i{};
        for (i = 0; i < TAB_COUNT; i++)
        {
            pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
        }
        pressed_widgets |= 1LL << (WIDX_TAB_1 + static_cast<int32_t>(_currentTab));
    }

    void RefreshWidgetSizing()
    {
        int32_t newWidth{}, newHeight{};

        if (_currentTab < SHOP_TAB)
        {
            disabled_widgets &= ~(1 << WIDX_GROUP_BY_TRACK_TYPE);
        }
        else
        {
            disabled_widgets |= 1LL << WIDX_GROUP_BY_TRACK_TYPE;
        }

        // Show or hide unrelated widgets

        if (_currentTab < RESEARCH_TAB)
        {
            widgets[WIDX_GROUP_BY_TRACK_TYPE].type = WindowWidgetType::Checkbox;
        }
        else
        {
            widgets[WIDX_GROUP_BY_TRACK_TYPE].type = WindowWidgetType::Empty;
        }

        if (_currentTab != RESEARCH_TAB)
        {
            widgets[WIDX_RIDE_LIST].type = WindowWidgetType::Scroll;
            widgets[WIDX_FILTER_TEXT_BOX].type = WindowWidgetType::TextBox;
            widgets[WIDX_FILTER_CLEAR_BUTTON].type = WindowWidgetType::Button;
            widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].type = WindowWidgetType::Empty;
            widgets[WIDX_LAST_DEVELOPMENT_GROUP].type = WindowWidgetType::Empty;
            widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WindowWidgetType::Empty;
            widgets[WIDX_RESEARCH_FUNDING_BUTTON].type = WindowWidgetType::Empty;

            newWidth = WindowWidth;
            newHeight = WindowHeight;
        }
        else
        {
            widgets[WIDX_RIDE_LIST].type = WindowWidgetType::Empty;
            widgets[WIDX_FILTER_TEXT_BOX].type = WindowWidgetType::Empty;
            widgets[WIDX_FILTER_CLEAR_BUTTON].type = WindowWidgetType::Empty;
            widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].type = WindowWidgetType::Groupbox;
            widgets[WIDX_LAST_DEVELOPMENT_GROUP].type = WindowWidgetType::Groupbox;
            widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WindowWidgetType::FlatBtn;
            if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
                widgets[WIDX_RESEARCH_FUNDING_BUTTON].type = WindowWidgetType::FlatBtn;

            newWidth = 300;
            newHeight = 196;
        }

        // Handle new window size
        if (width != newWidth || height != newHeight)
        {
            Invalidate();

            // Resize widgets to new window size
            width = newWidth;
            height = newHeight;
            ResizeFrameWithPage();
            widgets[WIDX_GROUP_BY_TRACK_TYPE].left = newWidth - 8 - GroupByTrackTypeWidth;
            widgets[WIDX_GROUP_BY_TRACK_TYPE].right = newWidth - 8;

            Invalidate();
        }

        InitScrollWidgets();
    }

    RideSelection ScrollGetRideListItemAt(const ScreenCoordsXY& screenCoords)
    {
        RideSelection result;
        result.Type = RIDE_TYPE_NULL;
        result.EntryIndex = OBJECT_ENTRY_INDEX_NULL;

        if (screenCoords.x <= 0 || screenCoords.y <= 0)
            return result;

        int32_t column = screenCoords.x / 116;
        int32_t row = screenCoords.y / 116;
        if (column >= 5)
            return result;

        int32_t index = column + (row * 5);

        RideSelection* listItem = _windowNewRideListItems;
        while (listItem->Type != RIDE_TYPE_NULL || listItem->EntryIndex != OBJECT_ENTRY_INDEX_NULL)
        {
            if (index-- == 0)
            {
                return *listItem;
            }
            listItem++;
        }

        return result;
    }

    void RestoreScrollPositionForCurrentTab()
    {
        assert(static_cast<size_t>(_currentTab) < std::size(_windowNewRideTabScroll));
        auto& currentTabScroll = _windowNewRideTabScroll[_currentTab];

        // Get maximum scroll height
        int32_t scrollWidth = 0;
        int32_t scrollHeight = 0;
        WindowGetScrollSize(this, 0, &scrollWidth, &scrollHeight);
        const Widget& listWidget = widgets[WIDX_RIDE_LIST];
        const int32_t listWidgetHeight = listWidget.bottom - listWidget.top - 1;

        // Ensure the current tab scroll is within range
        currentTabScroll = std::min<uint16_t>(currentTabScroll, std::max(0, scrollHeight - listWidgetHeight));

        scrolls[0].v_top = currentTabScroll;
        WidgetScrollUpdateThumbs(*this, WIDX_RIDE_LIST);
    }

    void DrawRideInformation(DrawPixelInfo& dpi, RideSelection item, const ScreenCoordsXY& screenPos, int32_t textWidth)
    {
        const auto* rideEntry = GetRideEntryByIndex(item.EntryIndex);
        RideNaming rideNaming = GetRideNaming(item.Type, *rideEntry);
        auto ft = Formatter();

        UpdateVehicleAvailability(item.Type);

        // Ride name and description
        ft.Add<StringId>(rideNaming.Name);
        ft.Add<StringId>(rideNaming.Description);
        DrawTextWrapped(dpi, screenPos, textWidth, STR_NEW_RIDE_NAME_AND_DESCRIPTION, ft);

        if (!_vehicleAvailability.empty())
        {
            if (gConfigInterface.ListRideVehiclesSeparately)
            {
                ft = Formatter();
                ft.Add<StringId>(rideEntry->naming.Name);
                DrawTextEllipsised(dpi, screenPos + ScreenCoordsXY{ 0, 39 }, WindowWidth - 2, STR_NEW_RIDE_VEHICLE_NAME, ft);
            }
            else
            {
                ft = Formatter();
                ft.Add<const utf8*>(_vehicleAvailability.c_str());
                DrawTextEllipsised(dpi, screenPos + ScreenCoordsXY{ 0, 39 }, WindowWidth - 2, STR_AVAILABLE_VEHICLES, ft);
            }
        }

        auto count = GetNumTrackDesigns(item);
        auto designCountStringId = GetDesignsAvailableStringId(count);
        ft = Formatter();
        ft.Add<int32_t>(count);
        DrawTextBasic(dpi, screenPos + ScreenCoordsXY{ 0, 51 }, designCountStringId, ft);

        // Price
        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            // Get price of ride
            int32_t startPieceId = GetRideTypeDescriptor(item.Type).StartTrackPiece;
            money64 price = GetRideTypeDescriptor(item.Type).BuildCosts.TrackPrice;
            const auto& ted = GetTrackElementDescriptor(startPieceId);
            price *= ted.PriceModifier;
            price = (price >> 16) * GetRideTypeDescriptor(item.Type).BuildCosts.PriceEstimateMultiplier;

            //
            StringId stringId = STR_NEW_RIDE_COST;
            if (!GetRideTypeDescriptor(item.Type).HasFlag(RIDE_TYPE_FLAG_HAS_NO_TRACK))
                stringId = STR_NEW_RIDE_COST_FROM;

            ft = Formatter();
            ft.Add<money64>(price);
            DrawTextBasic(dpi, screenPos + ScreenCoordsXY{ textWidth, 51 }, stringId, ft, { TextAlignment::RIGHT });
        }
    }

    void DrawTabImage(DrawPixelInfo& dpi, NewRideTabId tab, int32_t spriteIndex)
    {
        WidgetIndex widgetIndex = WIDX_TAB_1 + static_cast<int32_t>(tab);

        if (widgets[widgetIndex].type != WindowWidgetType::Empty && !WidgetIsDisabled(*this, widgetIndex))
        {
            int32_t frame = 0;
            if (_currentTab == tab)
                frame = frame_no / TabAnimationDivisor[_currentTab];

            spriteIndex += tab == THRILL_TAB ? ThrillRidesTabAnimationSequence[frame] : frame;

            GfxDrawSprite(
                dpi, ImageId(spriteIndex, colours[1]),
                windowPos + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
        }
    }

    void DrawTabImages(DrawPixelInfo& dpi)
    {
        DrawTabImage(dpi, TRANSPORT_TAB, SPR_TAB_RIDES_TRANSPORT_0);
        DrawTabImage(dpi, GENTLE_TAB, SPR_TAB_RIDES_GENTLE_0);
        DrawTabImage(dpi, ROLLER_COASTER_TAB, SPR_TAB_RIDES_ROLLER_COASTERS_0);
        DrawTabImage(dpi, THRILL_TAB, SPR_TAB_RIDES_THRILL_0);
        DrawTabImage(dpi, WATER_TAB, SPR_TAB_RIDES_WATER_0);
        DrawTabImage(dpi, SHOP_TAB, SPR_TAB_RIDES_SHOP_0);
        DrawTabImage(dpi, RESEARCH_TAB, SPR_TAB_FINANCES_RESEARCH_0);
    }

    StringId GetDesignsAvailableStringId(int32_t count)
    {
        switch (count)
        {
            case 0:
                return STR_CUSTOM_DESIGNED_LAYOUT;
            case 1:
                return STR_1_DESIGN_AVAILABLE;
            default:
                return STR_X_DESIGNS_AVAILABLE;
        }
    }
};

NewRideTabId NewRideWindow::_currentTab{};
uint_fast16_t NewRideWindow::_windowNewRideTabScroll[RideTabCount]{};

/**
 *
 *  rct2: 0x006ACA58
 */
void WindowNewRideInitVars()
{
    // If we are in the track designer, default to the Roller Coaster tab
    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
    {
        NewRideWindow::SetOpeningPage(ROLLER_COASTER_TAB);
    }
    else
    {
        NewRideWindow::SetOpeningPage(TRANSPORT_TAB);
    }
    NewRideWindow::ResetTabScrolls();
}

/**
 *
 *  rct2: 0x006B3CFF
 */
WindowBase* WindowNewRideOpen()
{
    WindowBase* window;

    window = WindowBringToFrontByClass(WindowClass::ConstructRide);
    if (window)
    {
        return window;
    }

    WindowCloseByClass(WindowClass::TrackDesignList);
    WindowCloseByClass(WindowClass::TrackDesignPlace);

    window = WindowCreate<NewRideWindow>(WindowClass::ConstructRide, WindowWidth, WindowHeight, WF_10 | WF_AUTO_POSITION);
    return window;
}

WindowBase* WindowNewRideOpenResearch()
{
    auto w = static_cast<NewRideWindow*>(WindowNewRideOpen());
    w->SetPage(RESEARCH_TAB);
    return w;
}

/**
 *
 *  rct2: 0x006B3EBA
 */
void WindowNewRideFocus(RideSelection rideItem)
{
    auto w = static_cast<NewRideWindow*>(WindowFindByClass(WindowClass::ConstructRide));
    if (!w)
    {
        return;
    }

    const auto* rideEntry = GetRideEntryByIndex(rideItem.EntryIndex);
    if (rideEntry == nullptr)
        return;

    auto rideTypeIndex = rideEntry->GetFirstNonNullRideType();
    w->SetPage(GetRideTypeDescriptor(rideTypeIndex).Category);
}
