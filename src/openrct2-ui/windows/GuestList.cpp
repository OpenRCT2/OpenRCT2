/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/SpriteIds.h>
#include <openrct2/core/Numerics.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/entity/EntityList.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Guest.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/peep/PeepThoughts.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>
#include <vector>

using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Numerics;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_GUESTS;
    static constexpr ScreenSize kWindowSize = { 350, 330 };

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
    static constexpr auto window_guest_list_widgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({  0, 43}, {350, 287}, WidgetType::resize,       WindowColour::secondary                                                        ), // tab content panel
        makeWidget({  5, 59}, { 80,  12}, WidgetType::dropdownMenu, WindowColour::secondary, STR_ARG_4_PAGE_X                                      ), // page dropdown
        makeWidget({ 73, 60}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH                                    ), // page dropdown button
        makeWidget({120, 59}, {142,  12}, WidgetType::dropdownMenu, WindowColour::secondary, 0xFFFFFFFF,              STR_INFORMATION_TYPE_TIP     ), // information type dropdown
        makeWidget({250, 60}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH,      STR_INFORMATION_TYPE_TIP     ), // information type dropdown button
        makeWidget({273, 46}, { 24,  24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_MAP),        STR_SHOW_GUESTS_ON_MAP_TIP   ), // map
        makeWidget({297, 46}, { 24,  24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_G2_SEARCH),  STR_GUESTS_FILTER_BY_NAME_TIP), // filter by name
        makeWidget({321, 46}, { 24,  24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_TRACK_PEEP), STR_TRACKED_GUESTS_ONLY_TIP  ), // tracking
        makeTab   ({  3, 17},                                                                                         STR_INDIVIDUAL_GUESTS_TIP    ), // tab 1
        makeTab   ({ 34, 17},                                                                                         STR_SUMMARISED_GUESTS_TIP    ), // tab 2
        makeWidget({  3, 72}, {344, 255}, WidgetType::scroll,       WindowColour::secondary, SCROLL_BOTH                                           ) // guest list
    );
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

        static constexpr uint8_t kSummarisedGuestsRowHeight = kScrollableRowHeight + 11;
        static constexpr auto kGuestsPerPage = 2000;
        static constexpr const auto kGuestPageHeight = kGuestsPerPage * kScrollableRowHeight;
        static constexpr size_t kMaxGroups = 240;

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
        void onOpen() override
        {
            setWidgets(window_guest_list_widgets);
            WindowInitScrollWidgets(*this);

            _selectedTab = TabId::Summarised;
            _selectedView = GuestViewType::Thoughts;
            _numPages = 1;
            widgets[WIDX_TRACKING].type = WidgetType::flatBtn;
            widgets[WIDX_FILTER_BY_NAME].type = WidgetType::flatBtn;
            widgets[WIDX_PAGE_DROPDOWN].type = WidgetType::empty;
            widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WidgetType::empty;

            WindowSetResize(*this, kWindowSize, { 500, 450 });

            RefreshList();
        }

        void setFilter(GuestListFilterType type, int32_t index)
        {
            _selectedPage = 0;
            _numPages = 1;
            _trackingOnly = false;
            _filterArguments = {};

            Formatter ft(_filterArguments.args);

            switch (type)
            {
                case GuestListFilterType::guestsOnRide:
                {
                    auto guestRide = GetRide(RideId::FromUnderlying(index));
                    if (guestRide != nullptr)
                    {
                        ft.Add<StringId>(
                            guestRide->getRideTypeDescriptor().HasFlag(RtdFlag::describeAsInside) ? STR_IN_RIDE : STR_ON_RIDE);
                        guestRide->formatNameTo(ft);

                        _selectedFilter = GuestFilterType::Guests;
                        _highlightedIndex = {};
                        _selectedTab = TabId::Individual;
                        _selectedView = GuestViewType::Thoughts;
                    }
                    break;
                }
                case GuestListFilterType::guestsInQueue:
                {
                    auto guestRide = GetRide(RideId::FromUnderlying(index));
                    if (guestRide != nullptr)
                    {
                        ft.Add<StringId>(STR_QUEUING_FOR);
                        guestRide->formatNameTo(ft);

                        _selectedFilter = GuestFilterType::Guests;
                        _highlightedIndex = {};
                        _selectedTab = TabId::Individual;
                        _selectedView = GuestViewType::Thoughts;
                    }
                    break;
                }
                case GuestListFilterType::guestsThinkingAboutRide:
                {
                    auto guestRide = GetRide(RideId::FromUnderlying(index));
                    if (guestRide != nullptr)
                    {
                        ft.Add<StringId>(kStringIdNone);
                        guestRide->formatNameTo(ft);

                        _selectedFilter = GuestFilterType::GuestsThinking;
                        _highlightedIndex = {};
                        _selectedTab = TabId::Individual;
                        _selectedView = GuestViewType::Thoughts;
                    }
                    break;
                }
                case GuestListFilterType::guestsThinkingX:
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

        void onUpdate() override
        {
            if (_lastFindGroupsWait != 0)
            {
                _lastFindGroupsWait--;
            }

            // Current tab image animation
            _tabAnimationIndex++;
            if (_tabAnimationIndex >= (_selectedTab == TabId::Individual ? 24uL : 32uL))
                _tabAnimationIndex = 0;
            invalidateWidget(WIDX_TAB_1 + static_cast<int32_t>(_selectedTab));
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_MAP:
                    ContextOpenWindow(WindowClass::map);
                    break;
                case WIDX_TRACKING:
                    _trackingOnly = !_trackingOnly;
                    setWidgetPressed(WIDX_TRACKING, _trackingOnly);
                    invalidate();
                    scrolls[0].contentOffsetY = 0;
                    RefreshList();
                    break;
                case WIDX_FILTER_BY_NAME:
                    if (!_filterName.empty())
                    {
                        // Unset the search filter.
                        _filterName.clear();
                        setWidgetPressed(WIDX_FILTER_BY_NAME, false);
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

        void onMouseDown(WidgetIndex widgetIndex) override
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
                    widgets[WIDX_TRACKING].type = WidgetType::empty;
                    if (_selectedTab == TabId::Summarised)
                    {
                        widgets[WIDX_FILTER_BY_NAME].type = WidgetType::empty;
                        setWidgetPressed(WIDX_FILTER_BY_NAME, false);
                        _filterName.clear();
                    }
                    else if (_selectedTab == TabId::Individual)
                    {
                        widgets[WIDX_TRACKING].type = WidgetType::flatBtn;
                        widgets[WIDX_FILTER_BY_NAME].type = WidgetType::flatBtn;
                    }
                    widgets[WIDX_PAGE_DROPDOWN].type = WidgetType::empty;
                    widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WidgetType::empty;
                    _tabAnimationIndex = 0;
                    _selectedFilter = {};
                    invalidate();
                    scrolls[0].contentOffsetY = 0;
                    RefreshList();
                    break;
                }
                case WIDX_PAGE_DROPDOWN_BUTTON:
                {
                    auto* widget = &widgets[widgetIndex - 1];

                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height(), colours[1], 0,
                        Dropdown::Flag::StayOpen, _numPages, widget->width() - 4);

                    for (size_t i = 0; i < _numPages; i++)
                    {
                        Formatter ft;
                        ft.Add<uint16_t>(i + 1);
                        gDropdown.items[i] = Dropdown::MenuLabel(STR_PAGE_X, ft);
                    }
                    gDropdown.items[static_cast<int32_t>(_selectedPage)].setChecked(true);
                    break;
                }
                case WIDX_INFO_TYPE_DROPDOWN_BUTTON:
                {
                    gDropdown.items[0] = Dropdown::MenuLabel(GetViewName(GuestViewType::Actions));
                    gDropdown.items[1] = Dropdown::MenuLabel(GetViewName(GuestViewType::Thoughts));

                    auto* widget = &widgets[widgetIndex - 1];
                    WindowDropdownShowTextCustomWidth(
                        { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height(), colours[1], 0,
                        Dropdown::Flag::StayOpen, 2, widget->width() - 4);

                    gDropdown.items[static_cast<int32_t>(_selectedView)].setChecked(true);
                    break;
                }
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (dropdownIndex == -1)
            {
                return;
            }
            switch (widgetIndex)
            {
                case WIDX_PAGE_DROPDOWN_BUTTON:
                    _selectedPage = dropdownIndex;
                    invalidate();
                    break;
                case WIDX_INFO_TYPE_DROPDOWN_BUTTON:
                    _selectedView = static_cast<GuestViewType>(dropdownIndex);
                    invalidate();
                    break;
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (!text.empty())
            {
                _filterName = text;
                setWidgetPressed(WIDX_FILTER_BY_NAME, true);
                RefreshList();
            }
        }

        void onPrepareDraw() override
        {
            setWidgetPressed(WIDX_TAB_1, false);
            setWidgetPressed(WIDX_TAB_2, false);
            setWidgetPressed(WIDX_TAB_1 + static_cast<int32_t>(_selectedTab), true);

            widgets[WIDX_INFO_TYPE_DROPDOWN].text = GetViewName(_selectedView);
            widgets[WIDX_MAP].type = WidgetType::empty;
            if (_selectedTab == TabId::Individual && _selectedFilter)
                widgets[WIDX_MAP].type = WidgetType::flatBtn;

            widgets[WIDX_GUEST_LIST].right = width - 4;
            widgets[WIDX_GUEST_LIST].bottom = height - 15;
            widgets[WIDX_MAP].left = 273 - kWindowSize.width + width;
            widgets[WIDX_MAP].right = 296 - kWindowSize.width + width;
            widgets[WIDX_FILTER_BY_NAME].left = 297 - kWindowSize.width + width;
            widgets[WIDX_FILTER_BY_NAME].right = 320 - kWindowSize.width + width;
            widgets[WIDX_TRACKING].left = 321 - kWindowSize.width + width;
            widgets[WIDX_TRACKING].right = 344 - kWindowSize.width + width;

            if (_numPages > 1)
            {
                widgets[WIDX_PAGE_DROPDOWN].type = WidgetType::dropdownMenu;
                widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WidgetType::button;
                auto ft = Formatter::Common();
                ft.Increment(4);
                ft.Add<uint16_t>(_selectedPage + 1);
            }
            else
            {
                widgets[WIDX_PAGE_DROPDOWN].type = WidgetType::empty;
                widgets[WIDX_PAGE_DROPDOWN_BUTTON].type = WidgetType::empty;
            }
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
            DrawTabImages(rt);

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
                DrawTextEllipsised(rt, screenCoords, 310, format, ft);
            }

            // Number of guests (list items)
            if (_selectedTab == TabId::Individual)
            {
                screenCoords = windowPos + ScreenCoordsXY{ 4, widgets[WIDX_GUEST_LIST].bottom + 2 };
                auto ft = Formatter();
                ft.Add<int32_t>(static_cast<int32_t>(_guestList.size()));
                DrawTextBasic(
                    rt, screenCoords, (_guestList.size() == 1 ? STR_FORMAT_NUM_GUESTS_SINGULAR : STR_FORMAT_NUM_GUESTS_PLURAL),
                    ft);
            }
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            int32_t y = 0;
            switch (_selectedTab)
            {
                case TabId::Individual:
                    // Count the number of guests
                    y = static_cast<int32_t>(_guestList.size()) * kScrollableRowHeight;
                    _numPages = (_guestList.size() + kGuestsPerPage - 1) / kGuestsPerPage;
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
                    y = static_cast<int32_t>(_groups.size() * kSummarisedGuestsRowHeight);
                    break;
            }

            y -= static_cast<int32_t>(kGuestPageHeight * _selectedPage);
            y = std::max(0, std::min(y, kGuestPageHeight));

            if (_highlightedIndex)
            {
                _highlightedIndex = {};
                invalidate();
            }

            auto i = std::max(0, y - widgets[WIDX_GUEST_LIST].bottom + widgets[WIDX_GUEST_LIST].top + 21);
            if (i < scrolls[0].contentOffsetY)
            {
                scrolls[0].contentOffsetY = i;
                invalidate();
            }

            return { 447, y };
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto i = screenCoords.y / (_selectedTab == TabId::Individual ? kScrollableRowHeight : kSummarisedGuestsRowHeight);
            i += static_cast<int32_t>(_selectedPage * kGuestsPerPage);
            if (static_cast<size_t>(i) != _highlightedIndex)
            {
                _highlightedIndex = i;
                invalidate();
            }
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (_selectedTab)
            {
                case TabId::Individual:
                {
                    auto i = screenCoords.y / kScrollableRowHeight;
                    i += static_cast<int32_t>(_selectedPage * kGuestsPerPage);
                    for (const auto& guestItem : _guestList)
                    {
                        if (i == 0)
                        {
                            auto guest = getGameState().entities.GetEntity<Guest>(guestItem.Id);
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
                    auto i = static_cast<size_t>(screenCoords.y / kSummarisedGuestsRowHeight);
                    if (i < _groups.size())
                    {
                        _filterArguments = _groups[i].Arguments;
                        _selectedFilter = _selectedView == GuestViewType::Actions ? GuestFilterType::Guests
                                                                                  : GuestFilterType::GuestsThinking;
                        _selectedTab = TabId::Individual;
                        widgets[WIDX_TRACKING].type = WidgetType::flatBtn;
                        invalidate();
                        widgets[WIDX_FILTER_BY_NAME].type = WidgetType::flatBtn;
                        scrolls[0].contentOffsetY = 0;
                        RefreshList();
                    }
                    break;
                }
            }
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            Rectangle::fill(
                rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } },
                getColourMap(colours[1].colour).midLight);
            switch (_selectedTab)
            {
                case TabId::Individual:
                    DrawScrollIndividual(rt);
                    break;
                case TabId::Summarised:
                    DrawScrollSummarised(rt);
                    break;
            }
        }

        void RefreshList()
        {
            // only the individual tab uses the GuestList so no point calculating it
            if (_selectedTab != TabId::Individual)
            {
                RefreshGroups();
            }
            else
            {
                _guestList.clear();

                for (auto peep : EntityList<Guest>())
                {
                    getGameState().entities.EntitySetFlashing(peep, false);
                    if (peep->OutsideOfPark)
                        continue;
                    if (_selectedFilter)
                    {
                        if (!IsPeepInFilter(*peep))
                            continue;
                        getGameState().entities.EntitySetFlashing(peep, true);
                    }
                    if (!GuestShouldBeVisible(*peep))
                        continue;

                    auto& item = _guestList.emplace_back();
                    item.Id = peep->Id;

                    Formatter ft;
                    peep->FormatNameTo(ft);
                    FormatStringLegacy(item.Name, sizeof(item.Name), STR_STRINGID, ft.Data());
                }

                std::sort(_guestList.begin(), _guestList.end(), GetGuestCompareFunc());
            }
        }

    private:
        void DrawTabImages(RenderTarget& rt)
        {
            // Tab 1 image
            auto i = (_selectedTab == TabId::Individual ? _tabAnimationIndex & ~3 : 0);
            auto* animObj = findPeepAnimationsObjectForType(AnimationPeepType::guest);
            i += animObj->GetPeepAnimation(PeepAnimationGroup::normal).baseImage + 1;
            GfxDrawSprite(
                rt, ImageId(i, Drawing::Colour::grey, Drawing::Colour::darkOliveGreen),
                windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].midX(), widgets[WIDX_TAB_1].bottom - 6 });

            // Tab 2 image
            i = (_selectedTab == TabId::Summarised ? _tabAnimationIndex / 4 : 0);
            GfxDrawSprite(
                rt, ImageId(SPR_TAB_GUESTS_0 + i),
                windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left, widgets[WIDX_TAB_2].top });
        }

        void DrawScrollIndividual(RenderTarget& rt)
        {
            size_t index = 0;
            auto y = static_cast<int32_t>(_selectedPage) * -kGuestPageHeight;
            for (const auto& guestItem : _guestList)
            {
                // Check if y is beyond the scroll control
                if (y + kScrollableRowHeight + 1 >= -0x7FFF && y + kScrollableRowHeight + 1 > rt.y && y < 0x7FFF
                    && y < rt.y + rt.height)
                {
                    // Highlight backcolour and text colour (format)
                    StringId format = STR_BLACK_STRING;
                    if (index == _highlightedIndex)
                    {
                        Rectangle::filter(rt, { 0, y, 800, y + kScrollableRowHeight - 1 }, FilterPaletteID::paletteDarken1);
                        format = STR_WINDOW_COLOUR_2_STRINGID;
                    }

                    // Guest name
                    auto peep = getGameState().entities.GetEntity<Guest>(guestItem.Id);
                    if (peep == nullptr)
                    {
                        continue;
                    }
                    auto ft = Formatter();
                    peep->FormatNameTo(ft);
                    DrawTextEllipsised(rt, { 0, y }, 113, format, ft);

                    switch (_selectedView)
                    {
                        case GuestViewType::Actions:
                            // Guest face
                            GfxDrawSprite(rt, ImageId(GetPeepFaceSpriteSmall(peep)), { 118, y + 1 });

                            // Tracking icon
                            if (peep->PeepFlags & PEEP_FLAGS_TRACKING)
                                GfxDrawSprite(rt, ImageId(STR_ENTER_SELECTION_SIZE), { 112, y + 1 });

                            // Action
                            ft = Formatter();
                            peep->FormatActionTo(ft);
                            DrawTextEllipsised(rt, { 133, y }, 314, format, ft);
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
                                DrawTextEllipsised(rt, { 118, y }, 329, format, ft, { FontStyle::small });
                                break;
                            }
                            break;
                    }
                }
                y += kScrollableRowHeight;
                index++;
            }
        }

        void DrawScrollSummarised(RenderTarget& rt)
        {
            size_t index = 0;
            auto y = 0;
            for (auto& group : _groups)
            {
                // Check if y is beyond the scroll control
                if (y + kSummarisedGuestsRowHeight + 1 >= rt.y)
                {
                    // Check if y is beyond the scroll control
                    if (y >= rt.y + rt.height)
                        break;

                    // Highlight backcolour and text colour (format)
                    StringId format = STR_BLACK_STRING;
                    if (index == _highlightedIndex)
                    {
                        Rectangle::filter(rt, { 0, y, 800, y + kSummarisedGuestsRowHeight }, FilterPaletteID::paletteDarken1);
                        format = STR_WINDOW_COLOUR_2_STRINGID;
                    }

                    // Draw guest faces
                    for (uint32_t j = 0; j < std::size(group.Faces) && j < group.NumGuests; j++)
                    {
                        GfxDrawSprite(
                            rt, ImageId(group.Faces[j] + SPR_PEEP_SMALL_FACE_VERY_VERY_UNHAPPY),
                            { static_cast<int32_t>(j) * 8, y + 12 });
                    }

                    // Draw action/thoughts
                    Formatter ft(group.Arguments.args);
                    // Draw small font if displaying guests
                    if (_selectedView == GuestViewType::Thoughts)
                    {
                        DrawTextEllipsised(rt, { 0, y }, 414, format, ft, { FontStyle::small });
                    }
                    else
                    {
                        DrawTextEllipsised(rt, { 0, y }, 414, format, ft);
                    }

                    // Draw guest count
                    ft = Formatter();
                    ft.Add<StringId>(STR_GUESTS_COUNT_COMMA_SEP);
                    ft.Add<uint32_t>(group.NumGuests);
                    DrawTextBasic(rt, { 326, y }, format, ft, { TextAlignment::right });
                }
                y += kSummarisedGuestsRowHeight;
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
                FormatStringLegacy(name, sizeof(name), STR_STRINGID, ft.Data());
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
            uint32_t tick256 = floor2(getGameState().currentTicks, 256);
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
            _lastFindGroupsTick = floor2(getGameState().currentTicks, 256);
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

            // Remove up to kMaxGroups
            if (_groups.size() > kMaxGroups)
            {
                _groups.resize(kMaxGroups);
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
            const auto* peepA = getGameState().entities.GetEntity<Peep>(a.Id);
            const auto* peepB = getGameState().entities.GetEntity<Peep>(b.Id);
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
            return getGameState().park.flags & PARK_FLAGS_SHOW_REAL_GUEST_NAMES ? CompareGuestItem<true>
                                                                                : CompareGuestItem<false>;
        }
    };

    WindowBase* GuestListOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::guestList);
        if (window == nullptr)
        {
            window = windowMgr->Create<GuestListWindow>(
                WindowClass::guestList, kWindowSize, { WindowFlag::higherContrastOnPress, WindowFlag::resizable });
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
            w->setFilter(type, index);
        }
        return w;
    }

    void WindowGuestListRefreshList()
    {
        auto* windowMgr = GetWindowManager();
        auto* w = windowMgr->FindByClass(WindowClass::guestList);
        if (w != nullptr)
        {
            static_cast<GuestListWindow*>(w)->RefreshList();
        }
    }
} // namespace OpenRCT2::Ui::Windows
