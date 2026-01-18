/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cassert>
#include <iterator>
#include <limits>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/ride/Construction.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/management/Research.h>
#include <openrct2/network/Network.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/object/RideObject.h>
#include <openrct2/rct1/RCT1.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>

using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::TrackMetaData;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WindowTitle = kStringIdNone;
    static constexpr int32_t kWindowHeightResearch = 194;
    static constexpr int32_t RideListItemsMax = kMaxRideObjects;
    static constexpr int32_t RideTabCount = 6;
    static constexpr int32_t GroupByTrackTypeWidth = 172;
    static constexpr int32_t kScrollItemSize = 116;
    static constexpr int32_t kMinColCount = 5;
    static constexpr int32_t kMaxColCount = 15;
    static constexpr int32_t kMinRowCount = 2;
    static constexpr int32_t kMaxRowCount = 7;
    static constexpr int32_t kHorizontalPadding = 21;
    static constexpr int32_t kVerticalPadding = 150;
    static constexpr ScreenSize kWindowSize = { (kMinColCount * kScrollItemSize) + kHorizontalPadding,
                                                (kMinRowCount * kScrollItemSize) + kVerticalPadding };
    static constexpr ScreenSize kWindowMaxSize = { (kMaxColCount * kScrollItemSize) + kHorizontalPadding,
                                                   (kMaxRowCount * kScrollItemSize) + kVerticalPadding };

#pragma region Ride type view order

    /**
     * The order of ride types shown in the new ride window so that the order stays consistent across games and rides of the
     * same type are kept together.
     */
    static constexpr ride_type_t RideTypeViewOrder[] = {
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
        RIDE_TYPE_CLASSIC_WOODEN_TWISTER_ROLLER_COASTER,
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
        RIDE_TYPE_CLASSIC_STAND_UP_ROLLER_COASTER,
        RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
        RIDE_TYPE_HYPERCOASTER,
        RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER,
        RIDE_TYPE_TWISTER_ROLLER_COASTER,
        RIDE_TYPE_HYPER_TWISTER,
        RIDE_TYPE_LSM_LAUNCHED_ROLLER_COASTER,
        RIDE_TYPE_GIGA_COASTER,
        RIDE_TYPE_SUSPENDED_SWINGING_COASTER,
        RIDE_TYPE_COMPACT_INVERTED_COASTER,
        RIDE_TYPE_INVERTED_ROLLER_COASTER,
        RIDE_TYPE_INVERTED_IMPULSE_COASTER,
        RIDE_TYPE_LAY_DOWN_ROLLER_COASTER,
        RIDE_TYPE_FLYING_ROLLER_COASTER,
        RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER,
        RIDE_TYPE_MINI_SUSPENDED_COASTER,
        RIDE_TYPE_STEEPLECHASE,
        RIDE_TYPE_BOBSLEIGH_COASTER,
        RIDE_TYPE_MINE_RIDE,
        RIDE_TYPE_HEARTLINE_TWISTER_COASTER,
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

    static constexpr ScreenCoordsXY GroupByTrackTypeOrigin{ kWindowSize.width - 8 - GroupByTrackTypeWidth, 47 };
    static constexpr ScreenSize GroupTrackTypeSize{ GroupByTrackTypeWidth, 14 };

    // clang-format off
    static constexpr auto window_new_ride_widgets = makeWidgets(
        makeWindowShim(WindowTitle, kWindowSize),
        makeWidget({  0,  43},             {601, 339},         WidgetType::resize,   WindowColour::secondary                                                                 ),
        makeTab   ({  3,  17},                                                                                STR_TRANSPORT_RIDES_TIP                                        ),
        makeTab   ({ 34,  17},                                                                                STR_GENTLE_RIDES_TIP                                           ),
        makeTab   ({ 65,  17},                                                                                STR_ROLLER_COASTERS_TIP                                        ),
        makeTab   ({ 96,  17},                                                                                STR_THRILL_RIDES_TIP                                           ),
        makeTab   ({127,  17},                                                                                STR_WATER_RIDES_TIP                                            ),
        makeTab   ({158,  17},                                                                                STR_SHOPS_STALLS_TIP                                           ),
        makeTab   ({189,  17},                                                                                STR_RESEARCH_AND_DEVELOPMENT_TIP                               ),
        makeWidget({  3,  62},             {595, 256},         WidgetType::scroll,   WindowColour::secondary, SCROLL_VERTICAL                                                ),
        makeWidget({  3,  47},             {290,  70},         WidgetType::groupbox, WindowColour::tertiary,  STR_CURRENTLY_IN_DEVELOPMENT                                   ),
        makeWidget({  3, 124},             {290,  65},         WidgetType::groupbox, WindowColour::tertiary,  STR_LAST_DEVELOPMENT                                           ),
        makeWidget({265, 161},             { 24,  24},         WidgetType::flatBtn,  WindowColour::tertiary,  0xFFFFFFFF,                       STR_RESEARCH_SHOW_DETAILS_TIP),
        makeWidget({265,  68},             { 24,  24},         WidgetType::flatBtn,  WindowColour::tertiary,  ImageId(SPR_FINANCE),             STR_FINANCES_RESEARCH_TIP    ),
        makeWidget({  4,  46},             {211, 14},          WidgetType::textBox,  WindowColour::secondary                                                                 ),
        makeWidget({218,  46},             { 70, 14},          WidgetType::button,   WindowColour::secondary, STR_OBJECT_SEARCH_CLEAR                                        ),
        makeWidget(GroupByTrackTypeOrigin, GroupTrackTypeSize, WidgetType::checkbox, WindowColour::secondary, STR_GROUP_BY_TRACK_TYPE,          STR_GROUP_BY_TRACK_TYPE_TIP  )
    );
    // clang-format on

#pragma endregion

    static constexpr StringId RideTitles[] = {
        STR_NEW_TRANSPORT_RIDES,      // TRANSPORT_TAB
        STR_NEW_GENTLE_RIDES,         // GENTLE_TAB
        STR_NEW_ROLLER_COASTERS,      // ROLLER_COASTER_TAB
        STR_NEW_THRILL_RIDES,         // THRILL_TAB
        STR_NEW_WATER_RIDES,          // WATER_TAB
        STR_NEW_SHOPS_STALLS,         // SHOP_TAB
        STR_RESEARCH_AND_DEVELOPMENT, // RESEARCH_TAB
    };
    static_assert(std::size(RideTitles) == TAB_COUNT);

    static constexpr int32_t TabAnimationLoops[] = {
        20, // TRANSPORT_TAB
        32, // GENTLE_TAB
        10, // ROLLER_COASTER_TAB
        72, // THRILL_TAB
        24, // WATER_TAB
        28, // SHOP_TAB
        16, // RESEARCH_TAB
    };
    static_assert(std::size(TabAnimationLoops) == TAB_COUNT);

    static constexpr int32_t TabAnimationDivisor[] = {
        4, // TRANSPORT_TAB
        8, // GENTLE_TAB
        2, // ROLLER_COASTER_TAB
        4, // THRILL_TAB
        4, // WATER_TAB
        4, // SHOP_TAB
        2, // RESEARCH_TAB
    };
    static_assert(std::size(TabAnimationDivisor) == TAB_COUNT);

    static constexpr int32_t ThrillRidesTabAnimationSequence[] = {
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
        struct NewRideVariables
        {
            RideSelection SelectedRide{};
            RideSelection HighlightedRide{};
            uint16_t SelectedRideCountdown{};
        } _newRideVars{};

    public:
        static void SetOpeningPage(NewRideTabId tab)
        {
            _currentTab = tab;
        }

        static void ResetTabScrolls()
        {
            std::fill_n(_windowNewRideTabScroll, RideTabCount, 0);
        }

        void onOpen() override
        {
            setWidgets(window_new_ride_widgets);
            PopulateRideList();
            initScrollWidgets();
            _filter.clear();

            currentFrame = 0;
            _newRideVars.SelectedRide = { kRideTypeNull, kObjectEntryIndexNull };
            _lastTrackDesignCountRideType.Type = kRideTypeNull;
            _lastTrackDesignCountRideType.EntryIndex = kObjectEntryIndexNull;

            width = 1;
            RefreshWidgetSizing();

            if (_currentTab < RESEARCH_TAB)
            {
                setPage(_currentTab);
                RestoreScrollPositionForCurrentTab();
            }
        }

        void onUpdate() override
        {
            currentFrame++;
            if (currentFrame >= TabAnimationLoops[_currentTab])
                currentFrame = 0;

            invalidateWidget(WIDX_TAB_1 + static_cast<int32_t>(_currentTab));

            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                invalidateWidget(WIDX_FILTER_TEXT_BOX);
            }

            if (_newRideVars.SelectedRide.Type != kRideTypeNull && _newRideVars.SelectedRideCountdown-- == 0)
            {
                RideSelect();
            }

            PopulateRideList();

            if (_currentTab < RESEARCH_TAB)
            {
                _windowNewRideTabScroll[_currentTab] = scrolls[0].contentOffsetY;

                // Remove highlight when mouse leaves rides list
                if (!widgetIsHighlighted(*this, WIDX_RIDE_LIST) && _newRideVars.HighlightedRide.Type != kRideTypeNull)
                {
                    _newRideVars.HighlightedRide = { kRideTypeNull, kObjectEntryIndexNull };
                    invalidateWidget(WIDX_RIDE_LIST);
                }
            }
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_LAST_DEVELOPMENT_BUTTON:
                    WindowResearchDevelopmentMouseUp(widgetIndex, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
                    break;
                case WIDX_RESEARCH_FUNDING_BUTTON:
                    ContextOpenWindowView(WindowView::financesResearch);
                    break;
                case WIDX_GROUP_BY_TRACK_TYPE:
                    Config::Get().interface.listRideVehiclesSeparately = !Config::Get().interface.listRideVehiclesSeparately;
                    Config::Save();
                    PopulateRideList();
                    invalidate();
                    break;
                case WIDX_FILTER_TEXT_BOX:
                    WindowStartTextbox(*this, widgetIndex, _filter, kTextInputSize);
                    break;
                case WIDX_FILTER_CLEAR_BUTTON:
                    _filter.clear();
                    scrolls->contentOffsetY = 0;
                    invalidate();
                    break;
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            if (widgetIndex >= WIDX_TAB_1 && widgetIndex <= WIDX_TAB_7)
            {
                setPage(widgetIndex - WIDX_TAB_1);
            }
        }

        void onPrepareDraw() override
        {
            SetPressedTab();

            if (!Config::Get().interface.listRideVehiclesSeparately)
                pressedWidgets |= (1LL << WIDX_GROUP_BY_TRACK_TYPE);
            else
                pressedWidgets &= ~(1LL << WIDX_GROUP_BY_TRACK_TYPE);

            widgets[WIDX_TITLE].text = RideTitles[_currentTab];
            widgets[WIDX_TAB_7].type = WidgetType::tab;
            widgets[WIDX_FILTER_TEXT_BOX].string = _filter.data();

            if (gLegacyScene == LegacyScene::trackDesigner)
                widgets[WIDX_TAB_7].type = WidgetType::empty;

            if (_currentTab == RESEARCH_TAB)
            {
                WindowResearchDevelopmentPrepareDraw(this, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
            }

            const auto& ls = GetContext()->GetLocalisationService();
            auto string = ls.GetString(STR_GROUP_BY_TRACK_TYPE);
            auto strWidth = GfxGetStringWidth(string, FontStyle::medium);
            auto localizedGroupByTrackTypeWidth = strWidth + 14;
            widgets[WIDX_GROUP_BY_TRACK_TYPE].left = width - 8 - localizedGroupByTrackTypeWidth;
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            if (_currentTab != RESEARCH_TAB)
            {
                RideSelection item = _newRideVars.HighlightedRide;
                if (item.Type != kRideTypeNull || item.EntryIndex != kObjectEntryIndexNull)
                    DrawRideInformation(rt, item, windowPos + ScreenCoordsXY{ 3, height - 64 }, width - 6);
            }
            else
            {
                WindowResearchDevelopmentDraw(this, rt, WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP);
            }
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            RideSelection* listItem = _windowNewRideListItems;

            int32_t count = 0;
            while (listItem->Type != kRideTypeNull || listItem->EntryIndex != kObjectEntryIndexNull)
            {
                count++;
                listItem++;
            }
            auto itemsPerRow = getNumImagesPerRow();
            return { widgets[WIDX_RIDE_LIST].width() - 1, ((count + (itemsPerRow - 1)) / itemsPerRow) * kScrollItemSize };
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            RideSelection item = ScrollGetRideListItemAt(screenCoords);
            if (_newRideVars.HighlightedRide == item)
            {
                return;
            }

            _newRideVars.HighlightedRide = item;
            invalidate();
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            RideSelection item = ScrollGetRideListItemAt(screenCoords);
            if (item.Type == kRideTypeNull && item.EntryIndex == kObjectEntryIndexNull)
            {
                return;
            }

            _newRideVars.SelectedRide = item;

            Audio::Play(Audio::SoundId::click1, 0, windowPos.x + (width / 2));
            _newRideVars.SelectedRideCountdown = 8;
            invalidate();
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            if (_currentTab == RESEARCH_TAB)
            {
                return;
            }

            GfxClear(rt, getColourMap(colours[1].colour).midLight);

            ScreenCoordsXY coords{ 1, 1 };
            RideSelection* listItem = _windowNewRideListItems;
            while (listItem->Type != kRideTypeNull || listItem->EntryIndex != kObjectEntryIndexNull)
            {
                // Draw flat button rectangle
                const bool isSelected = _newRideVars.SelectedRide == *listItem;
                if (_newRideVars.HighlightedRide == *listItem || isSelected)
                {
                    const auto borderStyle = isSelected ? Rectangle::BorderStyle::inset : Rectangle::BorderStyle::outset;
                    Rectangle::fillInset(
                        rt, { coords, coords + ScreenCoordsXY{ kScrollItemSize - 1, kScrollItemSize - 1 } }, colours[1],
                        borderStyle, Rectangle::FillBrightness::dark);
                }

                // Draw ride image with feathered border
                auto mask = ImageId(SPR_NEW_RIDE_MASK);
                auto rideImage = ImageId(GetRideImage(*listItem));
                GfxDrawSpriteRawMasked(rt, coords + ScreenCoordsXY{ 2, 2 }, mask, rideImage);

                // Next position
                coords.x += kScrollItemSize;
                if (coords.x >= kScrollItemSize * getNumImagesPerRow() + 1)
                {
                    coords.x = 1;
                    coords.y += kScrollItemSize;
                }

                // Next item
                listItem++;
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (widgetIndex != WIDX_FILTER_TEXT_BOX)
                return;

            if (text == _filter)
                return;

            _filter.assign(text);

            scrolls->contentOffsetY = 0;
            PopulateRideList();
            invalidate();
        }

        void setPage(int tab)
        {
            if (tab >= TRANSPORT_TAB && tab < TAB_COUNT)
            {
                setPage(static_cast<NewRideTabId>(tab));
            }
        }

        void setPage(NewRideTabId tab)
        {
            // Skip setting page if we're already on this page, unless we're initialising the window
            if (_currentTab == tab && !widgets.empty())
                return;

            _currentTab = tab;
            currentFrame = 0;
            _newRideVars.HighlightedRide = { kRideTypeNull, kObjectEntryIndexNull };
            _newRideVars.SelectedRideCountdown = std::numeric_limits<uint16_t>::max();
            PopulateRideList();
            RefreshWidgetSizing();
            invalidate();

            if (tab < RESEARCH_TAB)
            {
                RestoreScrollPositionForCurrentTab();
            }
        }

    private:
        void RideSelect()
        {
            RideSelection item = _newRideVars.SelectedRide;
            if (item.Type == kRideTypeNull)
            {
                return;
            }

            close();

            auto* windowMgr = GetWindowManager();
            windowMgr->CloseConstructionWindows();

            auto count = GetNumTrackDesigns(item);
            if (count > 0)
            {
                auto intent = Intent(WindowClass::trackDesignList);
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
                if (GetRideTypeDescriptor(item.Type).HasFlag(RtdFlag::listVehiclesSeparately))
                {
                    entryName = GetRideEntryName(item.EntryIndex);
                }
            }

            auto repo = GetContext()->GetTrackDesignRepository();
            _lastTrackDesignCount = static_cast<int32_t>(repo->GetCountForObjectEntry(item.Type, entryName));
            _lastTrackDesignCountRideType = item;
            return _lastTrackDesignCount;
        }

        void UpdateVehicleAvailability(ride_type_t rideType)
        {
            _vehicleAvailability.clear();
            if (GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::listVehiclesSeparately))
            {
                return;
            }

            auto& objManager = GetContext()->GetObjectManager();
            auto& rideEntries = objManager.GetAllRideEntries(rideType);
            auto isFirst = true;
            for (auto rideEntryIndex : rideEntries)
            {
                auto currentRideEntry = GetRideEntryByIndex(rideEntryIndex);

                // Skip if vehicle type is not invented yet
                if (!RideEntryIsInvented(rideEntryIndex) && !getGameState().cheats.ignoreResearchStatus)
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
            auto& objMgr = GetContext()->GetObjectManager();
            auto obj = objMgr.GetLoadedObject<RideObject>(rideSelection.EntryIndex);
            return obj == nullptr ? kImageIndexUndefined : obj->GetPreviewImage(rideSelection.Type);
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
                if (rideType == kRideTypeNull)
                    continue;

                if (EnumValue(GetRideTypeDescriptor(rideType).Category) != currentCategory)
                    continue;

                nextListItem = IterateOverRideType(rideType, nextListItem, listEnd);
            }

            nextListItem->Type = kRideTypeNull;
            nextListItem->EntryIndex = kObjectEntryIndexNull;
        }

        RideSelection* IterateOverRideType(ride_type_t rideType, RideSelection* nextListItem, RideSelection* listEnd)
        {
            bool buttonForRideTypeCreated = false;
            bool allowDrawingOverLastButton = false;

            uint8_t highestVehiclePriority = 0;

            // For each ride entry for this ride type
            auto& objManager = GetContext()->GetObjectManager();
            auto& rideEntries = objManager.GetAllRideEntries(rideType);
            for (auto rideEntryIndex : rideEntries)
            {
                // Skip if vehicle type is not invented yet
                if (!RideEntryIsInvented(rideEntryIndex) && !getGameState().cheats.ignoreResearchStatus)
                    continue;

                // Ride entries
                auto& objMgr = GetContext()->GetObjectManager();
                auto* rideObj = objMgr.GetLoadedObject<RideObject>(rideEntryIndex);

                // Skip if the vehicle isn't the preferred vehicle for this generic track type
                if (!Config::Get().interface.listRideVehiclesSeparately
                    && !GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::listVehiclesSeparately)
                    && highestVehiclePriority > rideObj->GetEntry().BuildMenuPriority)
                {
                    continue;
                }

                if (!IsFiltered(*rideObj))
                {
                    continue;
                }

                highestVehiclePriority = rideObj->GetEntry().BuildMenuPriority;

                // Determines how and where to draw a button for this ride type/vehicle.
                if (Config::Get().interface.listRideVehiclesSeparately
                    || GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::listVehiclesSeparately))
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
                    if (rideType == rideObj->GetEntry().ride_type[0])
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

        bool IsFiltered(const RideObject& rideObject)
        {
            if (_filter.empty())
                return true;

            return IsFilterInRideType(rideObject.GetEntry()) || IsFilterInRideName(rideObject.GetEntry())
                || IsFilterInIdentifier(rideObject) || IsFilterInAuthors(rideObject) || IsFilterInFilename(rideObject);
        }

        bool IsFilterInRideType(const RideObjectEntry& rideEntry)
        {
            auto rideTypeName = GetRideNaming(rideEntry.ride_type[0], &rideEntry).Name;
            return String::contains(u8string_view(LanguageGetString(rideTypeName)), _filter, true);
        }

        bool IsFilterInRideName(const RideObjectEntry& rideEntry)
        {
            auto rideName = rideEntry.naming.Name;
            return String::contains(u8string_view(LanguageGetString(rideName)), _filter, true);
        }

        bool IsFilterInAuthors(const RideObject& rideObject)
        {
            auto& authors = rideObject.GetAuthors();

            for (auto& author : authors)
                if (String::contains(author, _filter, true))
                    return true;

            return false;
        }

        bool IsFilterInIdentifier(const RideObject& rideObject)
        {
            return String::contains(rideObject.GetIdentifier(), _filter, true);
        }

        bool IsFilterInFilename(const RideObject& rideObject)
        {
            return String::contains(rideObject.GetFileName(), _filter, true);
        }

        void SetPressedTab()
        {
            int32_t i{};
            for (i = 0; i < TAB_COUNT; i++)
            {
                pressedWidgets &= ~(1 << (WIDX_TAB_1 + i));
            }
            pressedWidgets |= 1LL << (WIDX_TAB_1 + static_cast<int32_t>(_currentTab));
        }

        void RefreshWidgetSizing()
        {
            if (_currentTab < SHOP_TAB)
            {
                disabledWidgets &= ~(1 << WIDX_GROUP_BY_TRACK_TYPE);
            }
            else
            {
                disabledWidgets |= 1LL << WIDX_GROUP_BY_TRACK_TYPE;
            }

            // Show or hide unrelated widgets

            if (_currentTab < RESEARCH_TAB)
            {
                widgets[WIDX_GROUP_BY_TRACK_TYPE].type = WidgetType::checkbox;
            }
            else
            {
                widgets[WIDX_GROUP_BY_TRACK_TYPE].type = WidgetType::empty;
            }

            ScreenSize newMinSize{}, newMaxSize{};
            if (_currentTab != RESEARCH_TAB)
            {
                widgets[WIDX_RIDE_LIST].type = WidgetType::scroll;
                widgets[WIDX_FILTER_TEXT_BOX].type = WidgetType::textBox;
                widgets[WIDX_FILTER_CLEAR_BUTTON].type = WidgetType::button;
                widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].type = WidgetType::empty;
                widgets[WIDX_LAST_DEVELOPMENT_GROUP].type = WidgetType::empty;
                widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WidgetType::empty;
                widgets[WIDX_RESEARCH_FUNDING_BUTTON].type = WidgetType::empty;

                newMinSize = kWindowSize;
                newMaxSize = kWindowMaxSize;
            }
            else
            {
                widgets[WIDX_RIDE_LIST].type = WidgetType::empty;
                widgets[WIDX_FILTER_TEXT_BOX].type = WidgetType::empty;
                widgets[WIDX_FILTER_CLEAR_BUTTON].type = WidgetType::empty;
                widgets[WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP].type = WidgetType::groupbox;
                widgets[WIDX_LAST_DEVELOPMENT_GROUP].type = WidgetType::groupbox;
                widgets[WIDX_LAST_DEVELOPMENT_BUTTON].type = WidgetType::flatBtn;
                if (!(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
                    widgets[WIDX_RESEARCH_FUNDING_BUTTON].type = WidgetType::flatBtn;

                newMinSize = { 300, kWindowHeightResearch };
                newMaxSize = newMinSize;
            }

            // Handle new window size
            if (width != newMinSize.width || height != newMinSize.height)
            {
                WindowSetResize(*this, newMinSize, newMaxSize);
                onResize();
            }

            initScrollWidgets();
        }

        void onResize() override
        {
            widgets[WIDX_GROUP_BY_TRACK_TYPE].left = width - 8 - GroupByTrackTypeWidth;
            widgets[WIDX_GROUP_BY_TRACK_TYPE].right = width - 8;
            widgets[WIDX_RIDE_LIST].right = width - 3 - 1;
            widgets[WIDX_RIDE_LIST].bottom = height - 65;
        }

        uint8_t getNumImagesPerRow()
        {
            const Widget& listWidget = widgets[WIDX_RIDE_LIST];
            auto scrollWidth = listWidget.width();
            return scrollWidth / kScrollItemSize;
        }

        RideSelection ScrollGetRideListItemAt(const ScreenCoordsXY& screenCoords)
        {
            RideSelection result;
            result.Type = kRideTypeNull;
            result.EntryIndex = kObjectEntryIndexNull;

            if (screenCoords.x <= 0 || screenCoords.y <= 0)
                return result;

            auto itemsPerRow = getNumImagesPerRow();
            int32_t column = screenCoords.x / kScrollItemSize;
            int32_t row = screenCoords.y / kScrollItemSize;
            if (column >= itemsPerRow)
                return result;

            int32_t index = column + (row * itemsPerRow);

            RideSelection* listItem = _windowNewRideListItems;
            while (listItem->Type != kRideTypeNull || listItem->EntryIndex != kObjectEntryIndexNull)
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
            ScreenSize scrollSize = onScrollGetSize(0);
            const Widget& listWidget = widgets[WIDX_RIDE_LIST];
            const int32_t listWidgetHeight = listWidget.bottom - listWidget.top - 1;

            // Ensure the current tab scroll is within range
            currentTabScroll = std::min<uint16_t>(currentTabScroll, std::max(0, scrollSize.height - listWidgetHeight));

            scrolls[0].contentOffsetY = currentTabScroll;
            widgetScrollUpdateThumbs(*this, WIDX_RIDE_LIST);
        }

        void DrawRideInformation(RenderTarget& rt, RideSelection item, const ScreenCoordsXY& screenPos, int32_t textWidth)
        {
            auto& objMgr = GetContext()->GetObjectManager();
            const auto* rideObj = objMgr.GetLoadedObject<RideObject>(item.EntryIndex);
            const auto& rideEntry = rideObj->GetEntry();
            RideNaming rideNaming = GetRideNaming(item.Type, &rideEntry);
            auto ft = Formatter();

            UpdateVehicleAvailability(item.Type);

            // Ride name and description
            ft.Add<StringId>(rideNaming.Name);
            ft.Add<StringId>(rideNaming.Description);
            DrawTextWrapped(rt, screenPos, textWidth, STR_NEW_RIDE_NAME_AND_DESCRIPTION, ft);

            if (!_vehicleAvailability.empty())
            {
                if (Config::Get().interface.listRideVehiclesSeparately)
                {
                    ft = Formatter();
                    ft.Add<StringId>(rideEntry.naming.Name);
                    DrawTextEllipsised(
                        rt, screenPos + ScreenCoordsXY{ 0, 39 }, kWindowSize.width - 2, STR_NEW_RIDE_VEHICLE_NAME, ft);
                }
                else
                {
                    ft = Formatter();
                    ft.Add<const utf8*>(_vehicleAvailability.c_str());
                    DrawTextEllipsised(
                        rt, screenPos + ScreenCoordsXY{ 0, 39 }, kWindowSize.width - 2, STR_AVAILABLE_VEHICLES, ft);
                }
            }

            if (_currentTab != SHOP_TAB)
            {
                auto count = GetNumTrackDesigns(item);
                auto designCountStringId = GetDesignsAvailableStringId(count);
                ft = Formatter();
                ft.Add<int32_t>(count);
                DrawTextBasic(rt, screenPos + ScreenCoordsXY{ 0, 51 }, designCountStringId, ft);
            }

            // Price
            if (!(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
            {
                // Get price of ride
                auto startPieceId = GetRideTypeDescriptor(item.Type).StartTrackPiece;
                money64 price = GetRideTypeDescriptor(item.Type).BuildCosts.TrackPrice;
                const auto& ted = GetTrackElementDescriptor(startPieceId);
                price *= ted.priceModifier;
                price = (price >> 16) * GetRideTypeDescriptor(item.Type).BuildCosts.PriceEstimateMultiplier;

                //
                StringId stringId = STR_NEW_RIDE_COST;
                if (GetRideTypeDescriptor(item.Type).HasFlag(RtdFlag::hasTrack))
                    stringId = STR_NEW_RIDE_COST_FROM;

                ft = Formatter();
                ft.Add<money64>(price);
                DrawTextBasic(rt, screenPos + ScreenCoordsXY{ textWidth - 14, 51 }, stringId, ft, { TextAlignment::right });
            }

            // Draw object author(s) if debugging tools are active
            if (Config::Get().general.debuggingTools && !rideObj->GetAuthors().empty())
            {
                const auto& authors = rideObj->GetAuthors();

                std::string authorsString;
                for (auto& author : authors)
                {
                    if (!authorsString.empty())
                        authorsString.append(", ");

                    authorsString.append(author);
                }

                ft = Formatter();
                ft.Add<StringId>(authors.size() > 1 ? STR_AUTHORS_STRING : STR_AUTHOR_STRING);
                ft.Add<const char*>(authorsString.c_str());

                DrawTextEllipsised(
                    rt, screenPos + ScreenCoordsXY{ textWidth, 0 }, kWindowSize.width - 2, STR_WINDOW_COLOUR_2_STRINGID, ft,
                    { TextAlignment::right });
            }
        }

        void DrawTabImage(RenderTarget& rt, NewRideTabId tab, int32_t spriteIndex)
        {
            WidgetIndex widgetIndex = WIDX_TAB_1 + static_cast<int32_t>(tab);

            if (widgets[widgetIndex].type != WidgetType::empty && !widgetIsDisabled(*this, widgetIndex))
            {
                int32_t frame = 0;
                if (_currentTab == tab)
                    frame = currentFrame / TabAnimationDivisor[_currentTab];

                spriteIndex += tab == THRILL_TAB ? ThrillRidesTabAnimationSequence[frame] : frame;

                GfxDrawSprite(
                    rt, ImageId(spriteIndex, colours[1].colour),
                    windowPos + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
            }
        }

        void DrawTabImages(RenderTarget& rt)
        {
            DrawTabImage(rt, TRANSPORT_TAB, SPR_TAB_RIDES_TRANSPORT_0);
            DrawTabImage(rt, GENTLE_TAB, SPR_TAB_RIDES_GENTLE_0);
            DrawTabImage(rt, ROLLER_COASTER_TAB, SPR_TAB_RIDES_ROLLER_COASTERS_0);
            DrawTabImage(rt, THRILL_TAB, SPR_TAB_RIDES_THRILL_0);
            DrawTabImage(rt, WATER_TAB, SPR_TAB_RIDES_WATER_0);
            DrawTabImage(rt, SHOP_TAB, SPR_TAB_RIDES_SHOP_0);
            DrawTabImage(rt, RESEARCH_TAB, SPR_TAB_FINANCES_RESEARCH_0);
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
        if (gLegacyScene == LegacyScene::trackDesigner)
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
    WindowBase* NewRideOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::constructRide);
        if (window)
        {
            return window;
        }

        windowMgr->CloseByClass(WindowClass::trackDesignList);
        windowMgr->CloseByClass(WindowClass::trackDesignPlace);

        window = windowMgr->Create<NewRideWindow>(
            WindowClass::constructRide, kWindowSize,
            { WindowFlag::higherContrastOnPress, WindowFlag::autoPosition, WindowFlag::resizable });
        return window;
    }

    WindowBase* NewRideOpenResearch()
    {
        auto w = static_cast<NewRideWindow*>(NewRideOpen());
        w->setPage(RESEARCH_TAB);
        return w;
    }

    /**
     *
     *  rct2: 0x006B3EBA
     */
    void WindowNewRideFocus(RideSelection rideItem)
    {
        auto* windowMgr = GetWindowManager();
        auto w = static_cast<NewRideWindow*>(windowMgr->FindByClass(WindowClass::constructRide));
        if (!w)
        {
            return;
        }

        const auto* rideEntry = GetRideEntryByIndex(rideItem.EntryIndex);
        if (rideEntry == nullptr)
            return;

        auto rideTypeIndex = rideEntry->GetFirstNonNullRideType();
        w->setPage(EnumValue(GetRideTypeDescriptor(rideTypeIndex).Category));
    }
} // namespace OpenRCT2::Ui::Windows
