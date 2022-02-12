/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cmath>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Guest.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Math.hpp>
#include <openrct2/util/Util.h>
#include <openrct2/world/Park.h>
#include <vector>

static constexpr const rct_string_id WINDOW_TITLE = STR_GUESTS;
static constexpr const int32_t WH = 330;
static constexpr const int32_t WW = 350;

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
static rct_widget window_guest_list_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  0, 43}, {350, 287}, WindowWidgetType::Resize,   WindowColour::Secondary                                                   ), // tab content panel
    MakeWidget({  5, 59}, { 80,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, STR_ARG_4_PAGE_X                                 ), // page dropdown
    MakeWidget({ 73, 60}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH                               ), // page dropdown button
    MakeWidget({120, 59}, {142,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary, 0xFFFFFFFF,         STR_INFORMATION_TYPE_TIP     ), // information type dropdown
    MakeWidget({250, 60}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_INFORMATION_TYPE_TIP     ), // information type dropdown button
    MakeWidget({273, 46}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_MAP,            STR_SHOW_GUESTS_ON_MAP_TIP   ), // map
    MakeWidget({297, 46}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_G2_SEARCH,      STR_GUESTS_FILTER_BY_NAME_TIP), // filter by name
    MakeWidget({321, 46}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_TRACK_PEEP,     STR_TRACKED_GUESTS_ONLY_TIP  ), // tracking
    MakeTab   ({  3, 17},                                                                        STR_INDIVIDUAL_GUESTS_TIP    ), // tab 1
    MakeTab   ({ 34, 17},                                                                        STR_SUMMARISED_GUESTS_TIP    ), // tab 2
    MakeWidget({  3, 72}, {344, 255}, WindowWidgetType::Scroll,   WindowColour::Secondary, SCROLL_BOTH                                      ), // guest list
    WIDGETS_END,
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

        rct_string_id GetFirstStringId()
        {
            rct_string_id firstStrId{};
            std::memcpy(&firstStrId, args, sizeof(firstStrId));
            return firstStrId;
        }

        bool operator==(const FilterArguments& other)
        {
            return std::memcmp(args, other.args, sizeof(args)) == 0;
        }
        bool operator!=(const FilterArguments& other)
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

    static constexpr const uint8_t SUMMARISED_GUEST_ROW_HEIGHT = SCROLLABLE_ROW_HEIGHT + 11;
    static constexpr const auto GUESTS_PER_PAGE = 2000;
    static constexpr const auto GUEST_PAGE_HEIGHT = GUESTS_PER_PAGE * SCROLLABLE_ROW_HEIGHT;
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
        widgets = window_guest_list_widgets;
        WindowInitScrollWidgets(this);

        _selectedTab = TabId::Individual;
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
                auto guestRide = get_ride(RideId::FromUnderlying(index));
                if (guestRide != nullptr)
                {
                    ft.Add<rct_string_id>(
                        guestRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IN_RIDE) ? STR_IN_RIDE : STR_ON_RIDE);
                    guestRide->FormatNameTo(ft);

                    _selectedFilter = GuestFilterType::Guests;
                    _highlightedIndex = {};
                    _selectedTab = TabId::Individual;
                    _selectedView = GuestViewType::Actions;
                }
                break;
            }
            case GuestListFilterType::GuestsInQueue:
            {
                auto guestRide = get_ride(RideId::FromUnderlying(index));
                if (guestRide != nullptr)
                {
                    ft.Add<rct_string_id>(STR_QUEUING_FOR);
                    guestRide->FormatNameTo(ft);

                    _selectedFilter = GuestFilterType::Guests;
                    _highlightedIndex = {};
                    _selectedTab = TabId::Individual;
                    _selectedView = GuestViewType::Actions;
                }
                break;
            }
            case GuestListFilterType::GuestsThinkingAboutRide:
            {
                auto guestRide = get_ride(RideId::FromUnderlying(index));
                if (guestRide != nullptr)
                {
                    ft.Add<rct_string_id>(STR_NONE);
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
                ft.Add<rct_string_id>(PeepThoughts[index & 0xFF]);

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
        if (_tabAnimationIndex >= (_selectedTab == TabId::Individual ? 24UL : 32UL))
            _tabAnimationIndex = 0;
        InvalidateWidget(WIDX_TAB_1 + static_cast<int32_t>(_selectedTab));

        gWindowMapFlashingFlags |= MapFlashingFlags::GuestListOpen;
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_MAP:
                context_open_window(WC_MAP);
                break;
            case WIDX_TRACKING:
                _trackingOnly = !_trackingOnly;
                SetWidgetPressed(WIDX_TRACKING, _trackingOnly);
                Invalidate();
                scrolls[0].v_top = 0;
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

    void OnMouseDown(rct_widgetindex widgetIndex) override
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
                scrolls[0].v_top = 0;
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

    void OnDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex) override
    {
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

    void OnTextInput(rct_widgetindex widgetIndex, std::string_view text) override
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

        widgets[WIDX_BACKGROUND].right = width - 1;
        widgets[WIDX_BACKGROUND].bottom = height - 1;
        widgets[WIDX_TAB_CONTENT_PANEL].right = width - 1;
        widgets[WIDX_TAB_CONTENT_PANEL].bottom = height - 1;
        widgets[WIDX_TITLE].right = width - 2;
        widgets[WIDX_CLOSE].left = width - 13;
        widgets[WIDX_CLOSE].right = width - 3;
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

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        // Filter description
        rct_string_id format;
        auto screenCoords = windowPos + ScreenCoordsXY{ 6, widgets[WIDX_TAB_CONTENT_PANEL].top + 3 };
        if (_selectedTab == TabId::Individual)
        {
            if (_selectedFilter)
            {
                if (_filterArguments.GetFirstStringId() != STR_NONE)
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
            DrawTextEllipsised(&dpi, screenCoords, 310, format, ft);
        }

        // Number of guests (list items)
        if (_selectedTab == TabId::Individual)
        {
            screenCoords = windowPos + ScreenCoordsXY{ 4, widgets[WIDX_GUEST_LIST].bottom + 2 };
            auto ft = Formatter();
            ft.Add<int32_t>(static_cast<int32_t>(_guestList.size()));
            DrawTextBasic(
                &dpi, screenCoords, (_guestList.size() == 1 ? STR_FORMAT_NUM_GUESTS_SINGULAR : STR_FORMAT_NUM_GUESTS_PLURAL),
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
                y = static_cast<int32_t>(_guestList.size()) * SCROLLABLE_ROW_HEIGHT;
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
        if (i < scrolls[0].v_top)
        {
            scrolls[0].v_top = i;
            Invalidate();
        }

        return { 447, y };
    }

    void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        auto i = screenCoords.y / (_selectedTab == TabId::Individual ? SCROLLABLE_ROW_HEIGHT : SUMMARISED_GUEST_ROW_HEIGHT);
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
                auto i = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
                i += static_cast<int32_t>(_selectedPage * GUESTS_PER_PAGE);
                for (const auto& guestItem : _guestList)
                {
                    if (i == 0)
                    {
                        auto guest = GetEntity<Guest>(guestItem.Id);
                        if (guest != nullptr)
                        {
                            WindowGuestOpen(guest);
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
                    scrolls[0].v_top = 0;
                    RefreshList();
                }
                break;
            }
        }
    }

    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        gfx_fill_rect(
            &dpi, { { dpi.x, dpi.y }, { dpi.x + dpi.width - 1, dpi.y + dpi.height - 1 } }, ColourMapA[colours[1]].mid_light);
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
                item.Id = peep->sprite_index;

                Formatter ft;
                peep->FormatNameTo(ft);
                format_string(item.Name, sizeof(item.Name), STR_STRINGID, ft.Data());
            }

            std::sort(_guestList.begin(), _guestList.end(), GetGuestCompareFunc());
        }
    }

private:
    void DrawTabImages(rct_drawpixelinfo& dpi)
    {
        // Tab 1 image
        auto i = (_selectedTab == TabId::Individual ? _tabAnimationIndex & ~3 : 0);
        i += GetPeepAnimation(PeepSpriteType::Normal).base_image + 1;
        i |= 0xA1600000;
        gfx_draw_sprite(
            &dpi, ImageId::FromUInt32(i),
            windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].midX(), widgets[WIDX_TAB_1].bottom - 6 });

        // Tab 2 image
        i = (_selectedTab == TabId::Summarised ? _tabAnimationIndex / 4 : 0);
        gfx_draw_sprite(
            &dpi, ImageId(SPR_TAB_GUESTS_0 + i),
            windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left, widgets[WIDX_TAB_2].top });
    }

    void DrawScrollIndividual(rct_drawpixelinfo& dpi)
    {
        size_t index = 0;
        auto y = static_cast<int32_t>(_selectedPage) * -GUEST_PAGE_HEIGHT;
        for (const auto& guestItem : _guestList)
        {
            // Check if y is beyond the scroll control
            if (y + SCROLLABLE_ROW_HEIGHT + 1 >= -0x7FFF && y + SCROLLABLE_ROW_HEIGHT + 1 > dpi.y && y < 0x7FFF
                && y < dpi.y + dpi.height)
            {
                // Highlight backcolour and text colour (format)
                rct_string_id format = STR_BLACK_STRING;
                if (index == _highlightedIndex)
                {
                    gfx_filter_rect(&dpi, { 0, y, 800, y + SCROLLABLE_ROW_HEIGHT - 1 }, FilterPaletteID::PaletteDarken1);
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
                DrawTextEllipsised(&dpi, { 0, y }, 113, format, ft);

                switch (_selectedView)
                {
                    case GuestViewType::Actions:
                        // Guest face
                        gfx_draw_sprite(&dpi, ImageId(get_peep_face_sprite_small(peep)), { 118, y + 1 });

                        // Tracking icon
                        if (peep->PeepFlags & PEEP_FLAGS_TRACKING)
                            gfx_draw_sprite(&dpi, ImageId(STR_ENTER_SELECTION_SIZE), { 112, y + 1 });

                        // Action
                        ft = Formatter();
                        peep->FormatActionTo(ft);
                        DrawTextEllipsised(&dpi, { 133, y }, 314, format, ft);
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
                            peep_thought_set_format_args(&thought, ft);
                            DrawTextEllipsised(&dpi, { 118, y }, 329, format, ft, { FontSpriteBase::SMALL });
                            break;
                        }
                        break;
                }
            }
            y += SCROLLABLE_ROW_HEIGHT;
            index++;
        }
    }

    void DrawScrollSummarised(rct_drawpixelinfo& dpi)
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
                rct_string_id format = STR_BLACK_STRING;
                if (index == _highlightedIndex)
                {
                    gfx_filter_rect(&dpi, { 0, y, 800, y + SUMMARISED_GUEST_ROW_HEIGHT }, FilterPaletteID::PaletteDarken1);
                    format = STR_WINDOW_COLOUR_2_STRINGID;
                }

                // Draw guest faces
                for (uint32_t j = 0; j < std::size(group.Faces) && j < group.NumGuests; j++)
                {
                    gfx_draw_sprite(
                        &dpi, ImageId(group.Faces[j] + SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY),
                        { static_cast<int32_t>(j) * 8, y + 12 });
                }

                // Draw action/thoughts
                Formatter ft(group.Arguments.args);
                // Draw small font if displaying guests
                if (_selectedView == GuestViewType::Thoughts)
                {
                    DrawTextEllipsised(&dpi, { 0, y }, 414, format, ft, { FontSpriteBase::SMALL });
                }
                else
                {
                    DrawTextEllipsised(&dpi, { 0, y }, 414, format, ft);
                }

                // Draw guest count
                ft = Formatter();
                ft.Add<rct_string_id>(STR_GUESTS_COUNT_COMMA_SEP);
                ft.Add<uint32_t>(group.NumGuests);
                DrawTextBasic(&dpi, { 326, y }, format, ft, { TextAlignment::RIGHT });
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
            format_string(name, sizeof(name), STR_STRINGID, ft.Data());
            if (strcasestr(name, _filterName.c_str()) == nullptr)
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
        if (_filterArguments.GetFirstStringId() == STR_NONE && _selectedFilter == GuestFilterType::GuestsThinking)
        {
            Formatter(peepArgs.args).Add<rct_string_id>(STR_NONE);
        }
        return _filterArguments == peepArgs;
    }

    bool IsRefreshOfGroupsRequired()
    {
        uint32_t tick256 = floor2(gCurrentTicks, 256);
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
        auto foundGroup = std::find_if(
            std::begin(_groups), std::end(_groups), [&arguments](GuestGroup& group) { return group.Arguments == arguments; });
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
        _lastFindGroupsTick = floor2(gCurrentTicks, 256);
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
                group.Faces[group.NumGuests] = get_peep_face_sprite_small(peep) - SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY;
            }
            group.NumGuests++;
        }

        // Remove empty group (basically guests with no thoughts)
        auto foundGroup = std::find_if(std::begin(_groups), std::end(_groups), [](GuestGroup& group) {
            return group.Arguments.GetFirstStringId() == STR_EMPTY;
        });
        if (foundGroup != std::end(_groups))
        {
            _groups.erase(foundGroup);
        }

        // Sort groups by number of guests
        std::sort(
            _groups.begin(), _groups.end(), [](const GuestGroup& a, const GuestGroup& b) { return a.NumGuests > b.NumGuests; });

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
                    peep_thought_set_format_args(&thought, ft);
                }
                break;
            }
        }
        return result;
    }

    static constexpr rct_string_id GetViewName(GuestViewType type)
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

    static constexpr rct_string_id GetFilterString(GuestFilterType type)
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

    template<bool TRealNames> static bool CompareGuestItem(const GuestItem& a, const GuestItem& b)
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
                    return peepA->Id < peepB->Id;
                }
            }
        }
        return strlogicalcmp(a.Name, b.Name) < 0;
    }

    static GuestItem::CompareFunc GetGuestCompareFunc()
    {
        return gParkFlags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES ? CompareGuestItem<true> : CompareGuestItem<false>;
    }
};

rct_window* WindowGuestListOpen()
{
    auto* window = window_bring_to_front_by_class(WC_GUEST_LIST);
    if (window == nullptr)
    {
        window = WindowCreate<GuestListWindow>(WC_GUEST_LIST, 350, 330, WF_10 | WF_RESIZABLE);
    }
    return window;
}

/**
 * @param index The number of the ride or index of the thought
 */
rct_window* WindowGuestListOpenWithFilter(GuestListFilterType type, int32_t index)
{
    auto* w = static_cast<GuestListWindow*>(WindowGuestListOpen());
    if (w != nullptr)
    {
        w->SetFilter(type, index);
    }
    return w;
}

void WindowGuestListRefreshList()
{
    auto* w = window_find_by_class(WC_GUEST_LIST);
    if (w != nullptr)
    {
        static_cast<GuestListWindow*>(w)->RefreshList();
    }
}
