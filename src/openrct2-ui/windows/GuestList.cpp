/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cmath>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/core/Numerics.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Guest.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/peep/PeepThoughts.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>
#include <vector>

using namespace OpenRCT2::Numerics;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = STR_GUESTS;
    static constexpr int32_t WH = 330;
    static constexpr int32_t WW = 350;

    enum WindowGuestListWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_TAB_CONTENT_PANEL,
        WIDX_PAGE_DROPDOWN,
        WIDX_PAGE_DROPDOWN_BUTTON,
        WIDX_INFO_TYPE_DROPDOWN,
        WIDX_INFO_TYPE_DROPDOWN_BUTTON,
        WIDX_MAP,
        WIDX_FILTER_BY_NAME,
        WIDX_TRACKING,
        WIDX_TAB_1,
        WIDX_TAB_2,
        WIDX_GUEST_LIST
    };

    // clang-format off
    static constexpr Widget window_guest_list_widgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget({  0, 43}, {350, 287}, WindowWidgetType::Resize,   WindowColour::Secondary                                                   ), // tab content panel
        MakeWidget({  5, 59}, { 80,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_ARG_4_PAGE_X                                 ), // page dropdown
        MakeWidget({ 73, 60}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH                               ), // page dropdown button
        MakeWidget({120, 59}, {142,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, 0xFFFFFFFF,         STR_INFORMATION_TYPE_TIP     ), // information type dropdown
        MakeWidget({250, 60}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_INFORMATION_TYPE_TIP     ), // information type dropdown button
        MakeWidget({273, 46}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_MAP),            STR_SHOW_GUESTS_ON_MAP_TIP   ), // map
        MakeWidget({297, 46}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_G2_SEARCH),      STR_GUESTS_FILTER_BY_NAME_TIP), // filter by name
        MakeWidget({321, 46}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_TRACK_PEEP),     STR_TRACKED_GUESTS_ONLY_TIP  ), // tracking
        MakeTab   ({  3, 17},                                                                        STR_INDIVIDUAL_GUESTS_TIP    ), // tab 1
        MakeTab   ({ 34, 17},                                                                        STR_SUMMARISED_GUESTS_TIP    ), // tab 2
        MakeWidget({  3, 72}, {344, 255}, WindowWidgetType::Scroll,   WindowColour::Secondary, SCROLL_BOTH                                      ), // guest list
    };
    // clang-format on

    class GuestListWindow final : public Window
    {
    private:
        enum class TabId
        {
            Individual,
            Summarised
        };

        enum class GuestViewType
        {
            Actions,
            Thoughts,
        };

        enum class GuestFilterType
        {
            Guests,
            GuestsThinking,
            GuestsThinkingAbout,
        };

        struct FilterArguments
        {
            uint8_t args[12]{};

            StringId GetFirstStringId()
            {
                StringId firstStrId{};
                std::memcpy(&firstStrId, args, sizeof(firstStrId));
                return firstStrId;
            }

            bool operator==(const FilterArguments& other) const
            {
                return std::memcmp(args, other.args, sizeof(args)) == 0;
            }
            bool operator!=(const FilterArguments& other) const
            {
                return !(*this == other);
            }
        };

        struct GuestGroup
        {
            size_t NumGuests{};
            FilterArguments Arguments;
            uint8_t Faces[58]{};
        };

        struct GuestItem
        {
            using CompareFunc = bool (*)(const GuestItem&, const GuestItem&);

            EntityId Id;
            char Name[256];
        };

        static constexpr uint8_t SUMMARISED_GUEST_ROW_HEIGHT = kScrollableRowHeight + 11;
        static constexpr auto GUESTS_PER_PAGE = 2000;
        static constexpr const auto GUEST_PAGE_HEIGHT = GUESTS_PER_PAGE * kScrollableRowHeight;
        static constexpr size_t MaxGroups = 240;

        TabId _selectedTab{};
        GuestViewType _selectedView{};
        bool _trackingOnly{};
        std::optional<GuestFilterType> _selectedFilter;
        FilterArguments _filterArguments;
        std::string _filterName;

        size_t _numPages{};
        size_t _selectedPage{};

        GuestViewType _lastFindGroupsSelectedView{};
        uint32_t _lastFindGroupsTick{};
        uint32_t _lastFindGroupsWait{};
        std::vector<GuestGroup> _groups;

        std::vector<GuestItem> _guestList;
        std::optional<size_t> _highlightedIndex;

        uint32_t _tabAnimationIndex{};

    public:
        void OnOpen() override
        {
            SetWidgets(window_guest_list_widgets);
            WindowInitScrollWidgets(*this);

            _selectedTab = TabId::Summarised;
            _selectedView = GuestViewType::Thoughts;
            _numPages = 1;
            widgets[WIDX_TRACKING].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_FILTER_BY_NAME].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_PAGE_DROPDOWN].type = WindowWidgetType::Empty;
            widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WindowWidgetType::Empty;
            min_width = 350;
            min_height = 330;
            max_width = 500;
            max_height = 450;
            RefreshList();
        }

        void SetFilter(GuestListFilterType type, int32_t index)
        {
            _selectedPage = 0;
            _numPages = 1;
            _trackingOnly = false;
            _filterArguments = {};

            Formatter ft(_filterArguments.args);

            switch (type)
            {
                case GuestListFilterType::GuestsOnRide:
                {
                    auto guestRide = GetRide(RideId::FromUnderlying(index));
                    if (guestRide != nullptr)
                    {
                        ft.Add<StringId>(
                            guestRide->GetRideTypeDescriptor().HasFlag(RtdFlag::describeAsInside) ? STR_IN_RIDE : STR_ON_RIDE);
                        guestRide->FormatNameTo(ft);

                        _selectedFilter = GuestFilterType::Guests;
                        _highlightedIndex = {};
                        _selectedTab = TabId::Individual;
                        _selectedView = GuestViewType::Thoughts;
                    }
                    break;
                }
                case GuestListFilterType::GuestsInQueue:
                {
                    auto guestRide = GetRide(RideId::FromUnderlying(index));
                    if (guestRide != nullptr)
                    {
                        ft.Add<StringId>(STR_QUEUING_FOR);
                        guestRide->FormatNameTo(ft);

                        _selectedFilter = GuestFilterType::Guests;
                        _highlightedIndex = {};
                        _selectedTab = TabId::Individual;
                        _selectedView = GuestViewType::Thoughts;
                    }
                    break;
                }
                case GuestListFilterType::GuestsThinkingAboutRide:
                {
                    auto guestRide = GetRide(RideId::FromUnderlying(index));
                    if (guestRide != nullptr)
                    {
                        ft.Add<StringId>(kStringIdNone);
                        guestRide->FormatNameTo(ft);

                        _selectedFilter = GuestFilterType::GuestsThinking;
                        _highlightedIndex = {};
                        _selectedTab = TabId::Individual;
                        _selectedView = GuestViewType::Thoughts;
                    }
                    break;
                }
                case GuestListFilterType::GuestsThinkingX:
                {
                    ft.Add<StringId>(kPeepThoughtIds[index & 0xFF]);

                    _selectedFilter = GuestFilterType::GuestsThinking;
                    _highlightedIndex = {};
                    _selectedTab = TabId::Individual;
                    _selectedView = GuestViewType::Thoughts;
                    break;
                }
            }

            RefreshList();
        }

        void OnResize() override
        {
            min_width = 350;
            min_height = 330;
            if (width < min_width)
            {
                Invalidate();
                width = min_width;
            }
            if (height < min_height)
            {
                Invalidate();
                height = min_height;
            }
        }

        void OnUpdate() override
        {
            if (_lastFindGroupsWait != 0)
            {
                _lastFindGroupsWait--;
            }

            // Current tab image animation
            _tabAnimationIndex++;
            if (_tabAnimationIndex >= (_selectedTab == TabId::Individual ? 24uL : 32uL))
                _tabAnimationIndex = 0;
            InvalidateWidget(WIDX_TAB_1 + static_cast<int32_t>(_selectedTab));
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_MAP:
                    ContextOpenWindow(WindowClass::Map);
                    break;
                case WIDX_TRACKING:
                    _trackingOnly = !_trackingOnly;
                    SetWidgetPressed(WIDX_TRACKING, _trackingOnly);
                    Invalidate();
                    scrolls[0].contentOffsetY = 0;
                    RefreshList();
                    break;
                case WIDX_FILTER_BY_NAME:
                    if (!_filterName.empty())
                    {
                        // Unset the search filter.
                        _filterName.clear();
                        SetWidgetPressed(WIDX_FILTER_BY_NAME, false);
                        RefreshList();
                    }
                    else
                    {
                        WindowTextInputRawOpen(
                            this, WIDX_FILTER_BY_NAME, STR_GUESTS_FILTER_BY_NAME, STR_GUESTS_ENTER_NAME_TO_SEARCH, {},
                            _filterName.c_str(), 32);
                    }
                    break;
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_TAB_1:
                case WIDX_TAB_2:
                {
                    if (_selectedFilter && _selectedTab == static_cast<TabId>(widgetIndex - WIDX_TAB_1))
                        break;
                    _selectedTab = static_cast<TabId>(widgetIndex - WIDX_TAB_1);
                    _selectedPage = 0;
                    _numPages = 1;
                    widgets[WIDX_TRACKING].type = WindowWidgetType::Empty;
                    if (_selectedTab == TabId::Summarised)
                    {
                        widgets[WIDX_FILTER_BY_NAME].type = WindowWidgetType::Empty;
                        SetWidgetPressed(WIDX_FILTER_BY_NAME, false);
                        _filterName.clear();
                    }
                    else if (_selectedTab == TabId::Individual)
                    {
                        widgets[WIDX_TRACKING].type = WindowWidgetType::FlatBtn;
                        widgets[WIDX_FILTER_BY_NAME].type = WindowWidgetType::FlatBtn;
                    }
                    widgets[WIDX_PAGE_DROPDOWN].type = WindowWidgetType::Empty;
                    widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WindowWidgetType::Empty;
                    _tabAnimationIndex = 0;
                    _selectedFilter = {};
                    Invalidate();
                    scrolls[0].contentOffsetY = 0;
                    RefreshList();
                    break;
                }
                case WIDX_PAGE_DROPDOWN_BUTTON:
                {
                    auto* widget = &widgets[widgetIndex - 1];

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0,
                        Dropdown::Flag::StayOpen, _numPages, widget->width() - 3);

                    for (size_t i = 0; i < _numPages; i++)
                    {
                        gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
                        uint16_t* args = reinterpret_cast<uint16_t*>(&gDropdownItems[i].Args);
                        args[0] = STR_PAGE_X;
                        args[1] = static_cast<uint16_t>(i + 1);
                    }
                    Dropdown::SetChecked(static_cast<int32_t>(_selectedPage), true);
                    break;
                }
                case WIDX_INFO_TYPE_DROPDOWN_BUTTON:
                {
                    gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[0].Args = GetViewName(GuestViewType::Actions);
                    gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItems[1].Args = GetViewName(GuestViewType::Thoughts);

                    auto* widget = &widgets[widgetIndex - 1];
                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0,
                        Dropdown::Flag::StayOpen, 2, widget->width() - 3);

                    Dropdown::SetChecked(static_cast<int32_t>(_selectedView), true);
                    break;
                }
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (dropdownIndex == -1)
            {
                return;
            }
            switch (widgetIndex)
            {
                case WIDX_PAGE_DROPDOWN_BUTTON:
                    _selectedPage = dropdownIndex;
                    Invalidate();
                    break;
                case WIDX_INFO_TYPE_DROPDOWN_BUTTON:
                    _selectedView = static_cast<GuestViewType>(dropdownIndex);
                    Invalidate();
                    break;
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (!text.empty())
            {
                _filterName = text;
                SetWidgetPressed(WIDX_FILTER_BY_NAME, true);
                RefreshList();
            }
        }

        void OnPrepareDraw() override
        {
            SetWidgetPressed(WIDX_TAB_1, false);
            SetWidgetPressed(WIDX_TAB_2, false);
            SetWidgetPressed(WIDX_TAB_1 + static_cast<int32_t>(_selectedTab), true);

            widgets[WIDX_INFO_TYPE_DROPDOWN].text = GetViewName(_selectedView);
            widgets[WIDX_MAP].type = WindowWidgetType::Empty;
            if (_selectedTab == TabId::Individual && _selectedFilter)
                widgets[WIDX_MAP].type = WindowWidgetType::FlatBtn;

            ResizeFrameWithPage();
            widgets[WIDX_GUEST_LIST].right = width - 4;
            widgets[WIDX_GUEST_LIST].bottom = height - 15;
            widgets[WIDX_MAP].left = 273 - 350 + width;
            widgets[WIDX_MAP].right = 296 - 350 + width;
            widgets[WIDX_FILTER_BY_NAME].left = 297 - 350 + width;
            widgets[WIDX_FILTER_BY_NAME].right = 320 - 350 + width;
            widgets[WIDX_TRACKING].left = 321 - 350 + width;
            widgets[WIDX_TRACKING].right = 344 - 350 + width;

            if (_numPages > 1)
            {
                widgets[WIDX_PAGE_DROPDOWN].type = WindowWidgetType::DropdownMenu;
                widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WindowWidgetType::Button;
                auto ft = Formatter::Common();
                ft.Increment(4);
                ft.Add<uint16_t>(_selectedPage + 1);
            }
            else
            {
                widgets[WIDX_PAGE_DROPDOWN].type = WindowWidgetType::Empty;
                widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WindowWidgetType::Empty;
            }
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);
            DrawTabImages(dpi);

            // Filter description
            StringId format;
            auto screenCoords = windowPos + ScreenCoordsXY{ 6, widgets[WIDX_TAB_CONTENT_PANEL].top + 3 };
            if (_selectedTab == TabId::Individual)
            {
                if (_selectedFilter)
                {
                    if (_filterArguments.GetFirstStringId() != kStringIdNone)
                    {
                        format = GetFilterString(*_selectedFilter);
                    }
                    else
                    {
                        format = STR_GUESTS_FILTER_THINKING_ABOUT;
                    }
                }
                else
                {
                    format = STR_ALL_GUESTS;
                }
            }
            else
            {
                format = STR_ALL_GUESTS_SUMMARISED;
            }

            {
                Formatter ft(_filterArguments.args);
                DrawTextEllipsised(dpi, screenCoords, 310, format, ft);
            }

            // Number of guests (list items)
            if (_selectedTab == TabId::Individual)
            {
                screenCoords = windowPos + ScreenCoordsXY{ 4, widgets[WIDX_GUEST_LIST].bottom + 2 };
                auto ft = Formatter();
                ft.Add<int32_t>(static_cast<int32_t>(_guestList.size()));
                DrawTextBasic(
                    dpi, screenCoords, (_guestList.size() == 1 ? STR_FORMAT_NUM_GUESTS_SINGULAR : STR_FORMAT_NUM_GUESTS_PLURAL),
                    ft);
            }
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            int32_t y = 0;
            switch (_selectedTab)
            {
                case TabId::Individual:
                    // Count the number of guests
                    y = static_cast<int32_t>(_guestList.size()) * kScrollableRowHeight;
                    _numPages = (_guestList.size() + GUESTS_PER_PAGE - 1) / GUESTS_PER_PAGE;
                    if (_numPages == 0)
                        _selectedPage = 0;
                    else if (_selectedPage >= _numPages)
                        _selectedPage = _numPages - 1;
                    break;
                case TabId::Summarised:
                default:
                    // Find the groups
                    if (IsRefreshOfGroupsRequired())
                    {
                        RefreshGroups();
                    }
                    y = static_cast<int32_t>(_groups.size() * SUMMARISED_GUEST_ROW_HEIGHT);
                    break;
            }

            y -= static_cast<int32_t>(GUEST_PAGE_HEIGHT * _selectedPage);
            y = std::max(0, std::min(y, GUEST_PAGE_HEIGHT));

            if (_highlightedIndex)
            {
                _highlightedIndex = {};
                Invalidate();
            }

            auto i = std::max(0, y - widgets[WIDX_GUEST_LIST].bottom + widgets[WIDX_GUEST_LIST].top + 21);
            if (i < scrolls[0].contentOffsetY)
            {
                scrolls[0].contentOffsetY = i;
                Invalidate();
            }

            return { 447, y };
        }

        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto i = screenCoords.y / (_selectedTab == TabId::Individual ? kScrollableRowHeight : SUMMARISED_GUEST_ROW_HEIGHT);
            i += static_cast<int32_t>(_selectedPage * GUESTS_PER_PAGE);
            if (static_cast<size_t>(i) != _highlightedIndex)
            {
                _highlightedIndex = i;
                Invalidate();
            }
        }

        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (_selectedTab)
            {
                case TabId::Individual:
                {
                    auto i = screenCoords.y / kScrollableRowHeight;
                    i += static_cast<int32_t>(_selectedPage * GUESTS_PER_PAGE);
                    for (const auto& guestItem : _guestList)
                    {
                        if (i == 0)
                        {
                            auto guest = GetEntity<Guest>(guestItem.Id);
                            if (guest != nullptr)
                            {
                                GuestOpen(guest);
                            }
                            break;
                        }
                        i--;
                    }
                    break;
                }
                case TabId::Summarised:
                {
                    auto i = static_cast<size_t>(screenCoords.y / SUMMARISED_GUEST_ROW_HEIGHT);
                    if (i < _groups.size())
                    {
                        _filterArguments = _groups[i].Arguments;
                        _selectedFilter = _selectedView == GuestViewType::Actions ? GuestFilterType::Guests
                                                                                  : GuestFilterType::GuestsThinking;
                        _selectedTab = TabId::Individual;
                        widgets[WIDX_TRACKING].type = WindowWidgetType::FlatBtn;
                        Invalidate();
                        widgets[WIDX_FILTER_BY_NAME].type = WindowWidgetType::FlatBtn;
                        scrolls[0].contentOffsetY = 0;
                        RefreshList();
                    }
                    break;
                }
            }
        }

        void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
        {
            GfxFillRect(
                dpi, { { dpi.x, dpi.y }, { dpi.x + dpi.width - 1, dpi.y + dpi.height - 1 } },
                ColourMapA[colours[1].colour].mid_light);
            switch (_selectedTab)
            {
                case TabId::Individual:
                    DrawScrollIndividual(dpi);
                    break;
                case TabId::Summarised:
                    DrawScrollSummarised(dpi);
                    break;
            }
        }

        void RefreshList()
        {
            // Only the individual tab uses the GuestList so no point calculating it
            if (_selectedTab != TabId::Individual)
            {
                RefreshGroups();
            }
            else
            {
                _guestList.clear();

                for (auto peep : EntityList<Guest>())
                {
                    EntitySetFlashing(peep, false);
                    if (peep->OutsideOfPark)
                        continue;
                    if (_selectedFilter)
                    {
                        if (!IsPeepInFilter(*peep))
                            continue;
                        EntitySetFlashing(peep, true);
                    }
                    if (!GuestShouldBeVisible(*peep))
                        continue;

                    auto& item = _guestList.emplace_back();
                    item.Id = peep->Id;

                    Formatter ft;
                    peep->FormatNameTo(ft);
                    OpenRCT2::FormatStringLegacy(item.Name, sizeof(item.Name), STR_STRINGID, ft.Data());
                }

                std::sort(_guestList.begin(), _guestList.end(), GetGuestCompareFunc());
            }
        }

    private:
        void DrawTabImages(DrawPixelInfo& dpi)
        {
            // Tab 1 image
            auto i = (_selectedTab == TabId::Individual ? _tabAnimationIndex & ~3 : 0);
            auto* animObj = findPeepAnimationsObjectForType(AnimationPeepType::Guest);
            i += animObj->GetPeepAnimation(PeepAnimationGroup::Normal).base_image + 1;
            GfxDrawSprite(
                dpi, ImageId(i, COLOUR_GREY, COLOUR_DARK_OLIVE_GREEN),
                windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].midX(), widgets[WIDX_TAB_1].bottom - 6 });

            // Tab 2 image
            i = (_selectedTab == TabId::Summarised ? _tabAnimationIndex / 4 : 0);
            GfxDrawSprite(
                dpi, ImageId(SPR_TAB_GUESTS_0 + i),
                windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left, widgets[WIDX_TAB_2].top });
        }

        void DrawScrollIndividual(DrawPixelInfo& dpi)
        {
            size_t index = 0;
            auto y = static_cast<int32_t>(_selectedPage) * -GUEST_PAGE_HEIGHT;
            for (const auto& guestItem : _guestList)
            {
                // Check if y is beyond the scroll control
                if (y + kScrollableRowHeight + 1 >= -0x7FFF && y + kScrollableRowHeight + 1 > dpi.y && y < 0x7FFF
                    && y < dpi.y + dpi.height)
                {
                    // Highlight backcolour and text colour (format)
                    StringId format = STR_BLACK_STRING;
                    if (index == _highlightedIndex)
                    {
                        GfxFilterRect(dpi, { 0, y, 800, y + kScrollableRowHeight - 1 }, FilterPaletteID::PaletteDarken1);
                        format = STR_WINDOW_COLOUR_2_STRINGID;
                    }

                    // Guest name
                    auto peep = GetEntity<Guest>(guestItem.Id);
                    if (peep == nullptr)
                    {
                        continue;
                    }
                    auto ft = Formatter();
                    peep->FormatNameTo(ft);
                    DrawTextEllipsised(dpi, { 0, y }, 113, format, ft);

                    switch (_selectedView)
                    {
                        case GuestViewType::Actions:
                            // Guest face
                            GfxDrawSprite(dpi, ImageId(GetPeepFaceSpriteSmall(peep)), { 118, y + 1 });

                            // Tracking icon
                            if (peep->PeepFlags & PEEP_FLAGS_TRACKING)
                                GfxDrawSprite(dpi, ImageId(STR_ENTER_SELECTION_SIZE), { 112, y + 1 });

                            // Action
                            ft = Formatter();
                            peep->FormatActionTo(ft);
                            DrawTextEllipsised(dpi, { 133, y }, 314, format, ft);
                            break;
                        case GuestViewType::Thoughts:
                            // For each thought
                            for (const auto& thought : peep->Thoughts)
                            {
                                if (thought.type == PeepThoughtType::None)
                                    break;
                                if (thought.freshness == 0)
                                    continue;
                                if (thought.freshness > 5)
                                    break;

                                ft = Formatter();
                                PeepThoughtSetFormatArgs(&thought, ft);
                                DrawTextEllipsised(dpi, { 118, y }, 329, format, ft, { FontStyle::Small });
                                break;
                            }
                            break;
                    }
                }
                y += kScrollableRowHeight;
                index++;
            }
        }

        void DrawScrollSummarised(DrawPixelInfo& dpi)
        {
            size_t index = 0;
            auto y = 0;
            for (auto& group : _groups)
            {
                // Check if y is beyond the scroll control
                if (y + SUMMARISED_GUEST_ROW_HEIGHT + 1 >= dpi.y)
                {
                    // Check if y is beyond the scroll control
                    if (y >= dpi.y + dpi.height)
                        break;

                    // Highlight backcolour and text colour (format)
                    StringId format = STR_BLACK_STRING;
                    if (index == _highlightedIndex)
                    {
                        GfxFilterRect(dpi, { 0, y, 800, y + SUMMARISED_GUEST_ROW_HEIGHT }, FilterPaletteID::PaletteDarken1);
                        format = STR_WINDOW_COLOUR_2_STRINGID;
                    }

                    // Draw guest faces
                    for (uint32_t j = 0; j < std::size(group.Faces) && j < group.NumGuests; j++)
                    {
                        GfxDrawSprite(
                            dpi, ImageId(group.Faces[j] + SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY),
                            { static_cast<int32_t>(j) * 8, y + 12 });
                    }

                    // Draw action/thoughts
                    Formatter ft(group.Arguments.args);
                    // Draw small font if displaying guests
                    if (_selectedView == GuestViewType::Thoughts)
                    {
                        DrawTextEllipsised(dpi, { 0, y }, 414, format, ft, { FontStyle::Small });
                    }
                    else
                    {
                        DrawTextEllipsised(dpi, { 0, y }, 414, format, ft);
                    }

                    // Draw guest count
                    ft = Formatter();
                    ft.Add<StringId>(STR_GUESTS_COUNT_COMMA_SEP);
                    ft.Add<uint32_t>(group.NumGuests);
                    DrawTextBasic(dpi, { 326, y }, format, ft, { TextAlignment::RIGHT });
                }
                y += SUMMARISED_GUEST_ROW_HEIGHT;
                index++;
            }
        }

        bool GuestShouldBeVisible(const Guest& peep)
        {
            if (_trackingOnly && !(peep.PeepFlags & PEEP_FLAGS_TRACKING))
                return false;

            if (!_filterName.empty())
            {
                char name[256]{};

                Formatter ft;
                peep.FormatNameTo(ft);
                OpenRCT2::FormatStringLegacy(name, sizeof(name), STR_STRINGID, ft.Data());
                if (!String::contains(name, _filterName.c_str(), true))
                {
                    return false;
                }
            }

            return true;
        }

        bool IsPeepInFilter(const Guest& peep)
        {
            auto guestViewType = _selectedFilter == GuestFilterType::Guests ? GuestViewType::Actions : GuestViewType::Thoughts;
            auto peepArgs = GetArgumentsFromPeep(peep, guestViewType);
            if (_filterArguments.GetFirstStringId() == kStringIdNone && _selectedFilter == GuestFilterType::GuestsThinking)
            {
                Formatter(peepArgs.args).Add<StringId>(kStringIdNone);
            }
            return _filterArguments == peepArgs;
        }

        bool IsRefreshOfGroupsRequired()
        {
            uint32_t tick256 = floor2(GetGameState().CurrentTicks, 256);
            if (_selectedView == _lastFindGroupsSelectedView)
            {
                if (_lastFindGroupsWait != 0 || _lastFindGroupsTick == tick256)
                {
                    return false;
                }
            }

            return true;
        }

        GuestGroup& FindOrAddGroup(FilterArguments&& arguments)
        {
            auto foundGroup = std::find_if(std::begin(_groups), std::end(_groups), [&arguments](GuestGroup& group) {
                return group.Arguments == arguments;
            });
            if (foundGroup != std::end(_groups))
            {
                return *foundGroup;
            }
            auto& newGroup = _groups.emplace_back();
            newGroup.Arguments = arguments;
            return newGroup;
        }

        void RefreshGroups()
        {
            _lastFindGroupsTick = floor2(GetGameState().CurrentTicks, 256);
            _lastFindGroupsSelectedView = _selectedView;
            _lastFindGroupsWait = 320;
            _groups.clear();

            for (auto peep : EntityList<Guest>())
            {
                if (peep->OutsideOfPark)
                    continue;

                auto& group = FindOrAddGroup(GetArgumentsFromPeep(*peep, _selectedView));
                if (group.NumGuests < std::size(group.Faces))
                {
                    group.Faces[group.NumGuests] = GetPeepFaceSpriteSmall(peep) - SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY;
                }
                group.NumGuests++;
            }

            // Remove empty group (basically guests with no thoughts)
            auto foundGroup = std::find_if(std::begin(_groups), std::end(_groups), [](GuestGroup& group) {
                return group.Arguments.GetFirstStringId() == kStringIdEmpty;
            });
            if (foundGroup != std::end(_groups))
            {
                _groups.erase(foundGroup);
            }

            // Sort groups by number of guests
            std::sort(_groups.begin(), _groups.end(), [](const GuestGroup& a, const GuestGroup& b) {
                return a.NumGuests > b.NumGuests;
            });

            // Remove up to MaxGroups
            if (_groups.size() > MaxGroups)
            {
                _groups.resize(MaxGroups);
            }
        }

        /**
         * Calculates a hash value (arguments) for comparing peep actions/thoughts
         */
        static FilterArguments GetArgumentsFromPeep(const Guest& peep, GuestViewType type)
        {
            FilterArguments result;
            Formatter ft(result.args);
            switch (type)
            {
                case GuestViewType::Actions:
                    peep.FormatActionTo(ft);
                    break;
                case GuestViewType::Thoughts:
                {
                    const auto& thought = peep.Thoughts[0];
                    if (thought.type != PeepThoughtType::None && thought.freshness <= 5)
                    {
                        PeepThoughtSetFormatArgs(&thought, ft);
                    }
                    break;
                }
            }
            return result;
        }

        static constexpr StringId GetViewName(GuestViewType type)
        {
            switch (type)
            {
                default:
                case GuestViewType::Actions:
                    return STR_ACTIONS;
                case GuestViewType::Thoughts:
                    return STR_THOUGHTS;
            }
        }

        static constexpr StringId GetFilterString(GuestFilterType type)
        {
            switch (type)
            {
                default:
                case GuestFilterType::Guests:
                    return STR_GUESTS_FILTER;
                case GuestFilterType::GuestsThinking:
                    return STR_GUESTS_FILTER_THINKING;
                case GuestFilterType::GuestsThinkingAbout:
                    return STR_GUESTS_FILTER_THINKING_ABOUT;
            }
        }

        template<bool TRealNames>
        static bool CompareGuestItem(const GuestItem& a, const GuestItem& b)
        {
            const auto* peepA = GetEntity<Peep>(a.Id);
            const auto* peepB = GetEntity<Peep>(b.Id);
            if (peepA != nullptr && peepB != nullptr)
            {
                // Compare types
                if (peepA->Type != peepB->Type)
                {
                    return static_cast<int32_t>(peepA->Type) < static_cast<int32_t>(peepB->Type);
                }

                // Compare name
                if constexpr (!TRealNames)
                {
                    if (peepA->Name == nullptr && peepB->Name == nullptr)
                    {
                        // Simple ID comparison for when both peeps use a number or a generated name
                        return peepA->PeepId < peepB->PeepId;
                    }
                }
            }
            return String::logicalCmp(a.Name, b.Name) < 0;
        }

        static GuestItem::CompareFunc GetGuestCompareFunc()
        {
            return GetGameState().Park.Flags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES ? CompareGuestItem<true>
                                                                                : CompareGuestItem<false>;
        }
    };

    WindowBase* GuestListOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::GuestList);
        if (window == nullptr)
        {
            window = windowMgr->Create<GuestListWindow>(WindowClass::GuestList, 350, 330, WF_10 | WF_RESIZABLE);
        }
        return window;
    }

    /**
     * @param index The number of the ride or index of the thought
     */
    WindowBase* GuestListOpenWithFilter(GuestListFilterType type, int32_t index)
    {
        auto* w = static_cast<GuestListWindow*>(GuestListOpen());
        if (w != nullptr)
        {
            w->SetFilter(type, index);
        }
        return w;
    }

    void WindowGuestListRefreshList()
    {
        auto* windowMgr = GetWindowManager();
        auto* w = windowMgr->FindByClass(WindowClass::GuestList);
        if (w != nullptr)
        {
            static_cast<GuestListWindow*>(w)->RefreshList();
        }
    }
} // namespace OpenRCT2::Ui::Windows
