/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/ride/Construction.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>
#include <vector>

static constexpr const StringId WINDOW_TITLE = STR_SELECT_DESIGN;
static constexpr const int32_t WH = 441;
static constexpr const int32_t WW = 600;
static constexpr const int32_t DEBUG_PATH_HEIGHT = 12;
static constexpr const int32_t ROTATE_AND_SCENERY_BUTTON_SIZE = 24;
static constexpr const int32_t WINDOW_PADDING = 5;

// clang-format off
enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_BACK,
    WIDX_FILTER_STRING,
    WIDX_FILTER_CLEAR,
    WIDX_TRACK_LIST,
    WIDX_TRACK_PREVIEW,
    WIDX_ROTATE,
    WIDX_TOGGLE_SCENERY,
    WIDX_SORT_TYPE,
    WIDX_SORT_TYPE_DROPDOWN,
    WIDX_SORT_TRACKS,
};

validate_global_widx(WC_TRACK_DESIGN_LIST, WIDX_ROTATE);

static Widget window_track_list_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  4,  18}, {218,  13}, WindowWidgetType::TableHeader, WindowColour::Primary  , STR_SELECT_OTHER_RIDE                                       ),
    MakeWidget({  4,  32}, {124,  13}, WindowWidgetType::TextBox,     WindowColour::Secondary                                                              ),
    MakeWidget({130,  32}, { 92,  13}, WindowWidgetType::Button,       WindowColour::Primary  , STR_OBJECT_SEARCH_CLEAR                                     ),
    MakeWidget({  4,  61}, {218, 381}, WindowWidgetType::Scroll,       WindowColour::Primary  , SCROLL_VERTICAL,         STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP),
    MakeWidget({224,  18}, {372, 219}, WindowWidgetType::FlatBtn,      WindowColour::Primary                                                                ),
    MakeWidget({572, 405}, { ROTATE_AND_SCENERY_BUTTON_SIZE, ROTATE_AND_SCENERY_BUTTON_SIZE}, WindowWidgetType::FlatBtn,      WindowColour::Primary  , ImageId(SPR_ROTATE_ARROW),        STR_ROTATE_90_TIP                  ),
    MakeWidget({572, 381}, { ROTATE_AND_SCENERY_BUTTON_SIZE, ROTATE_AND_SCENERY_BUTTON_SIZE}, WindowWidgetType::FlatBtn,      WindowColour::Primary  , ImageId(SPR_SCENERY),             STR_TOGGLE_SCENERY_TIP             ),
    MakeWidget({  4,  46}, {162,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary), // current sort type WIDX_SORT_TYPE
    MakeWidget({155,  47}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH), // track sort dropdown button WIDX_SORT_TYPE_DROPDOWN
    MakeWidget({168,  46}, { 54,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_SORT_ARROW_UP), // sort button WIDX_SORT_TRACKS
    WIDGETS_END,
};

// clang-format on

enum
{
    /* Minigolf */
    SORT_MG_NAME,
    SORT_MG_EXCITEMENT,
    SORT_MG_INTENSITY,
    SORT_MG_NAUSEA,
    SORT_MG_HOLES,
    SORT_MG_RIDE_LENGTH,
    SORT_MG_SPACE_REQUIRED,
    DROPDOWN_LIST_COUNT_MG,

    /* Roller coasters, Ghost trains */
    SORT_RC_NAME,
    SORT_RC_EXCITEMENT,
    SORT_RC_INTENSITY,
    SORT_RC_NAUSEA,
    SORT_RC_MAX_SPEED,
    SORT_RC_AVERAGE_SPEED,
    SORT_RC_RIDE_LENGTH,
    SORT_RC_MAX_POSITIVE_VERTICAL_G,
    SORT_RC_MAX_NEGATIVE_VERTICAL_G,
    SORT_RC_MAX_LATERAL_G,
    SORT_RC_TOTAL_AIR_TIME,
    SORT_RC_DROPS,
    SORT_RC_HIGHEST_DROP_HEIGHT,
    SORT_RC_SPACE_REQUIRED,
    DROPDOWN_LIST_COUNT_RC,

    /* Roller Coasters with inversions */
    SORT_RCI_NAME,
    SORT_RCI_EXCITEMENT,
    SORT_RCI_INTENSITY,
    SORT_RCI_NAUSEA,
    SORT_RCI_MAX_SPEED,
    SORT_RCI_AVERAGE_SPEED,
    SORT_RCI_RIDE_LENGTH,
    SORT_RCI_MAX_POSITIVE_VERTICAL_G,
    SORT_RCI_MAX_NEGATIVE_VERTICAL_G,
    SORT_RCI_MAX_LATERAL_G,
    SORT_RCI_TOTAL_AIR_TIME,
    SORT_RCI_DROPS,
    SORT_RCI_HIGHEST_DROP_HEIGHT,
    SORT_RCI_INVERSIONS,
    SORT_RCI_SPACE_REQUIRED,
    DROPDOWN_LIST_COUNT_RCI,

    /* Water rides */
    SORT_W_NAME,
    SORT_W_EXCITEMENT,
    SORT_W_INTENSITY,
    SORT_W_NAUSEA,
    SORT_W_MAX_SPEED,
    SORT_W_AVERAGE_SPEED,
    SORT_W_RIDE_LENGTH,
    SORT_W_DROPS,
    SORT_W_HIGHEST_DROP_HEIGHT,
    SORT_W_SPACE_REQUIRED,
    DROPDOWN_LIST_COUNT_W,

    /* Transport, tower rides, tracked gentle/thrill, other water */
    SORT_T_NAME,
    SORT_T_EXCITEMENT,
    SORT_T_INTENSITY,
    SORT_T_NAUSEA,
    SORT_T_MAX_SPEED,
    SORT_T_AVERAGE_SPEED,
    SORT_T_RIDE_LENGTH,
    SORT_T_SPACE_REQUIRED,
    DROPDOWN_LIST_COUNT_T,

    /* Maze */
    SORT_M_NAME,
    SORT_M_EXCITEMENT,
    SORT_M_INTENSITY,
    SORT_M_NAUSEA,
    SORT_M_SPACE_REQUIRED,
    DROPDOWN_LIST_COUNT_M,

    /* Default list */
    SORT_TYPE_NAME,
    SORT_TYPE_EXCITEMENT,
    SORT_TYPE_INTENSITY,
    SORT_TYPE_NAUSEA,
    SORT_TYPE_MAX_SPEED,
    SORT_TYPE_AVERAGE_SPEED,
    SORT_TYPE_HOLES,
    SORT_TYPE_RIDE_LENGTH,
    SORT_TYPE_MAX_POSITIVE_VERTICAL_G,
    SORT_TYPE_MAX_NEGATIVE_VERTICAL_G,
    SORT_TYPE_MAX_LATERAL_G,
    SORT_TYPE_TOTAL_AIR_TIME,
    SORT_TYPE_DROPS,
    SORT_TYPE_HIGHEST_DROP_HEIGHT,
    SORT_TYPE_INVERSIONS,
    SORT_TYPE_SPACE_REQUIRED,
    DROPDOWN_LIST_COUNT,
};

static constexpr const StringId mg_sort_type_string_mapping[DROPDOWN_LIST_COUNT_MG - SORT_MG_NAME] = {
    /* Minigolf */
    STR_SORT_NAME,  STR_SORT_EXCITEMENT,  STR_SORT_INTENSITY,      STR_SORT_NAUSEA,
    STR_SORT_HOLES, STR_SORT_RIDE_LENGTH, STR_SORT_SPACE_REQUIRED,
};

static constexpr const StringId rc_sort_type_string_mapping[DROPDOWN_LIST_COUNT_RC - SORT_RC_NAME] = {
    /* Roller coasters, Ghost trains */
    STR_SORT_NAME,
    STR_SORT_EXCITEMENT,
    STR_SORT_INTENSITY,
    STR_SORT_NAUSEA,
    STR_SORT_MAX_SPEED,
    STR_SORT_AVERAGE_SPEED,
    STR_SORT_RIDE_LENGTH,
    STR_SORT_MAX_POSITIVE_VERTICAL_G,
    STR_SORT_MAX_NEGATIVE_VERTICAL_G,
    STR_SORT_MAX_LATERAL_G,
    STR_SORT_TOTAL_AIR_TIME,
    STR_SORT_DROPS,
    STR_SORT_HIGHEST_DROP_HEIGHT,
    STR_SORT_SPACE_REQUIRED,
};

static constexpr const StringId rci_sort_type_string_mapping[DROPDOWN_LIST_COUNT_RCI - SORT_RCI_NAME] = {
    /* Roller coasters with inversions */
    STR_SORT_NAME,
    STR_SORT_EXCITEMENT,
    STR_SORT_INTENSITY,
    STR_SORT_NAUSEA,
    STR_SORT_MAX_SPEED,
    STR_SORT_AVERAGE_SPEED,
    STR_SORT_RIDE_LENGTH,
    STR_SORT_MAX_POSITIVE_VERTICAL_G,
    STR_SORT_MAX_NEGATIVE_VERTICAL_G,
    STR_SORT_MAX_LATERAL_G,
    STR_SORT_TOTAL_AIR_TIME,
    STR_SORT_DROPS,
    STR_SORT_HIGHEST_DROP_HEIGHT,
    STR_SORT_INVERSIONS,
    STR_SORT_SPACE_REQUIRED,
};

static constexpr const StringId drs_sort_type_string_mapping[DROPDOWN_LIST_COUNT_W - SORT_W_NAME] = {
    /* Dinghy, River rapids, Splash boats */
    STR_SORT_NAME,           STR_SORT_EXCITEMENT, STR_SORT_INTENSITY,
    STR_SORT_NAUSEA,         STR_SORT_MAX_SPEED,  STR_SORT_AVERAGE_SPEED,
    STR_SORT_RIDE_LENGTH,    STR_SORT_DROPS,      STR_SORT_HIGHEST_DROP_HEIGHT,
    STR_SORT_SPACE_REQUIRED,
};

static constexpr const StringId tw_sort_type_string_mapping[DROPDOWN_LIST_COUNT_T - SORT_T_NAME] = {
    /* Transport, tower rides, tracked gentle/thrill, other water */
    STR_SORT_NAME,      STR_SORT_EXCITEMENT,    STR_SORT_INTENSITY,   STR_SORT_NAUSEA,
    STR_SORT_MAX_SPEED, STR_SORT_AVERAGE_SPEED, STR_SORT_RIDE_LENGTH, STR_SORT_SPACE_REQUIRED,
};

static constexpr const StringId fr_sort_type_string_mapping[DROPDOWN_LIST_COUNT_M - SORT_M_NAME] = {
    /* Flat rides */
    STR_SORT_NAME, STR_SORT_EXCITEMENT, STR_SORT_INTENSITY, STR_SORT_NAUSEA, STR_SORT_SPACE_REQUIRED,
};

static constexpr const StringId default_sort_type_string_mapping[DROPDOWN_LIST_COUNT - SORT_TYPE_NAME] = {
    /* Default list */
    STR_SORT_NAME,
    STR_SORT_EXCITEMENT,
    STR_SORT_INTENSITY,
    STR_SORT_NAUSEA,
    STR_SORT_MAX_SPEED,
    STR_SORT_AVERAGE_SPEED,
    STR_SORT_HOLES,
    STR_SORT_RIDE_LENGTH,
    STR_SORT_MAX_POSITIVE_VERTICAL_G,
    STR_SORT_MAX_NEGATIVE_VERTICAL_G,
    STR_SORT_MAX_LATERAL_G,
    STR_SORT_TOTAL_AIR_TIME,
    STR_SORT_DROPS,
    STR_SORT_HIGHEST_DROP_HEIGHT,
    STR_SORT_INVERSIONS,
    STR_SORT_SPACE_REQUIRED,
};

constexpr uint16_t TRACK_DESIGN_INDEX_UNLOADED = UINT16_MAX;

RideSelection _window_track_list_item;

class TrackListWindow final : public Window
{
private:
    std::vector<TrackDesignFileRef> _trackDesigns;
    utf8 _filterString[USER_STRING_MAX_LENGTH];
    std::vector<uint16_t> _filteredTrackIds;
    uint16_t _loadedTrackDesignIndex;
    std::unique_ptr<TrackDesign> _loadedTrackDesign;
    std::vector<uint8_t> _trackDesignPreviewPixels;
    int32_t _trackSortType;            // Current selected type to sort tracks on.
    int32_t _currentFirstType;         // Stores enum value that represents the first type in the current dropdown.
    int32_t _currentLastType;          // Stores enum value that represents the last + 1 type in the current dropdown.
    int32_t _currentDropDownSelection; // Index that maps correct string to currently selected type to sort on.
    bool _sortAscending;               // Decides if tracks are sorted in ascending or descending order.
    const StringId* _trackSortTypeStringMapping; // Pointer to array of StringId that is displayed in dropdown.

    void FilterList()
    {
        _filteredTrackIds.clear();

        // Nothing to filter, so fill the list with all indices
        if (String::LengthOf(_filterString) == 0)
        {
            for (uint16_t i = 0; i < _trackDesigns.size(); i++)
                _filteredTrackIds.push_back(i);

            return;
        }

        // Convert filter to uppercase
        const auto filterStringUpper = String::ToUpper(_filterString);

        // Fill the set with indices for tracks that match the filter
        for (uint16_t i = 0; i < _trackDesigns.size(); i++)
        {
            const auto trackNameUpper = String::ToUpper(_trackDesigns[i].name);
            if (trackNameUpper.find(filterStringUpper) != std::string::npos)
            {
                _filteredTrackIds.push_back(i);
            }
        }
    }

    void SelectFromList(int32_t listIndex)
    {
        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, this->windowPos.x + (this->width / 2));
        if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            if (listIndex == 0)
            {
                Close();
                ride_construct_new(_window_track_list_item);
                return;
            }
            listIndex--;
        }

        // Displays a message if the ride can't load, fix #4080
        if (_loadedTrackDesign == nullptr)
        {
            ContextShowError(STR_CANT_BUILD_THIS_HERE, STR_TRACK_LOAD_FAILED_ERROR, {});
            return;
        }

        if (_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE)
        {
            gTrackDesignSceneryToggle = true;
        }

        uint16_t trackDesignIndex = _filteredTrackIds[listIndex];
        TrackDesignFileRef* tdRef = &_trackDesigns[trackDesignIndex];
        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        {
            auto intent = Intent(WindowClass::ManageTrackDesign);
            intent.putExtra(INTENT_EXTRA_TRACK_DESIGN, tdRef);
            ContextOpenIntent(&intent);
        }
        else
        {
            if (_loadedTrackDesignIndex != TRACK_DESIGN_INDEX_UNLOADED
                && (_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE))
            {
                ContextShowError(STR_THIS_DESIGN_WILL_BE_BUILT_WITH_AN_ALTERNATIVE_VEHICLE_TYPE, STR_NONE, {});
            }

            auto intent = Intent(WindowClass::TrackDesignPlace);
            intent.putExtra(INTENT_EXTRA_TRACK_DESIGN, tdRef);
            ContextOpenIntent(&intent);
        }
    }

    int32_t GetListItemFromPosition(const ScreenCoordsXY& screenCoords)
    {
        size_t maxItems = _filteredTrackIds.size();
        if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            // Extra item: custom design
            maxItems++;
        }

        int32_t index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
        if (index < 0 || static_cast<uint32_t>(index) >= maxItems)
        {
            index = -1;
        }
        return index;
    }

    void LoadDesignsList(RideSelection item)
    {
        auto repo = OpenRCT2::GetContext()->GetTrackDesignRepository();
        std::string entryName;
        if (item.Type < 0x80)
        {
            if (GetRideTypeDescriptor(item.Type).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
            {
                entryName = get_ride_entry_name(item.EntryIndex);
            }
        }
        _trackDesigns = repo->GetItemsForObjectEntry(item.Type, entryName);

        FilterList();
    }

    bool LoadDesignPreview(const u8string& path)
    {
        _loadedTrackDesign = TrackDesignImport(path.c_str());
        if (_loadedTrackDesign != nullptr)
        {
            TrackDesignDrawPreview(_loadedTrackDesign.get(), _trackDesignPreviewPixels.data());
            return true;
        }
        return false;
    }

    /**
     * trackSortValues: a vector of pairs where the first value in each pair is the value that is to be sorted,
     * and the second value in each pair is the index of the TrackDesignFileRef from _trackDesigns that stored the value.
     *
     * std::sort is used to sort trackSortValues on the first value in each pair.
     *
     * sortedTrackDesigns is created as a temporary vector to hold the TrackDesignFileRef's.
     * Since the old index is known (second value of each pair in trackSortValues),
     * and the new (sorted) index is known (index of each pair after trackSortValues is sorted),
     * it is possible construct sortedTrackDesigns by iterating through trackSortValues once.
     *
     * Values in sortedTrackDesigns is then moved to _trackDesign.
     */
    template<typename T> void SortAlg(std::vector<std::pair<T, uint32_t>>& trackSortValues)
    {
        const bool sortAscending = _sortAscending;
        std::sort(
            std::begin(trackSortValues), std::end(trackSortValues), [sortAscending](const auto& p1, const auto& p2) -> bool {
                return sortAscending ? p1.first > p2.first : p1.first < p2.first;
            });

        std::vector<TrackDesignFileRef> sortedTrackDesigns{};
        for (uint32_t i{ 0 }; i < trackSortValues.size(); ++i)
            sortedTrackDesigns.push_back(_trackDesigns[trackSortValues[i].second]);
        std::move(sortedTrackDesigns.begin(), sortedTrackDesigns.end(), _trackDesigns.begin());
    }

    /**
     * In each case a vector called trackSortValues is constructed.
     * This vector holds pairs where the first value in each pair is the value that is to be sorted,
     * and the second value in each pair is the index of the TrackDesignFileRef from _trackDesigns that stored the value.
     * trackSortValues is sent to SortAlg to perform the actual sorting.
     *
     * Motivation:
     * By constructing trackSortValues it is possible to load each TrackDesign from disk only once.
     * This is desirable since std::sort is later used to sort trackSortValues.
     * std::sort sorts a vector in O(n*log(n)), where n is the size of the vector.
     * If std::sort was used directly on _trackDesigns, the game would have to load each TrackDesign several times from disk,
     * which is expensive.
     * ~hjort96
     */
    void SortList()
    {
        TrackDesign dummy{};
        switch (_trackSortType)
        {
            case SORT_TYPE_NAME:
            case SORT_MG_NAME:
            case SORT_RC_NAME:
            case SORT_RCI_NAME:
            case SORT_W_NAME:
            case SORT_T_NAME:
            case SORT_M_NAME:
            {
                std::vector<std::pair<decltype(dummy.name), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->name, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_EXCITEMENT:
            case SORT_RC_EXCITEMENT:
            case SORT_RCI_EXCITEMENT:
            case SORT_W_EXCITEMENT:
            case SORT_T_EXCITEMENT:
            case SORT_M_EXCITEMENT:
            {
                std::vector<std::pair<decltype(dummy.excitement), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->excitement, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_INTENSITY:
            case SORT_RC_INTENSITY:
            case SORT_RCI_INTENSITY:
            case SORT_W_INTENSITY:
            case SORT_T_INTENSITY:
            case SORT_M_INTENSITY:
            {
                std::vector<std::pair<decltype(dummy.intensity), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->intensity, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_NAUSEA:
            case SORT_RC_NAUSEA:
            case SORT_RCI_NAUSEA:
            case SORT_W_NAUSEA:
            case SORT_T_NAUSEA:
            case SORT_M_NAUSEA:
            {
                std::vector<std::pair<decltype(dummy.nausea), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->nausea, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_MAX_SPEED:
            case SORT_RC_MAX_SPEED:
            case SORT_RCI_MAX_SPEED:
            case SORT_W_MAX_SPEED:
            case SORT_T_MAX_SPEED:
            {
                std::vector<std::pair<decltype(dummy.max_speed), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->max_speed, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_AVERAGE_SPEED:
            case SORT_RC_AVERAGE_SPEED:
            case SORT_RCI_AVERAGE_SPEED:
            case SORT_W_AVERAGE_SPEED:
            case SORT_T_AVERAGE_SPEED:
            {
                std::vector<std::pair<decltype(dummy.average_speed), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->average_speed, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_HOLES:
            case SORT_MG_HOLES:
            {
                std::vector<std::pair<decltype(dummy.holes), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->holes, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_RIDE_LENGTH:
            case SORT_RC_RIDE_LENGTH:
            case SORT_RCI_RIDE_LENGTH:
            case SORT_W_RIDE_LENGTH:
            case SORT_T_RIDE_LENGTH:
            {
                std::vector<std::pair<decltype(dummy.ride_length), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->ride_length, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_MAX_POSITIVE_VERTICAL_G:
            case SORT_RC_MAX_POSITIVE_VERTICAL_G:
            case SORT_RCI_MAX_POSITIVE_VERTICAL_G:
            {
                std::vector<std::pair<decltype(dummy.max_positive_vertical_g), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->max_positive_vertical_g, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_MAX_NEGATIVE_VERTICAL_G:
            case SORT_RC_MAX_NEGATIVE_VERTICAL_G:
            case SORT_RCI_MAX_NEGATIVE_VERTICAL_G:
            {
                std::vector<std::pair<decltype(dummy.max_negative_vertical_g), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->max_negative_vertical_g, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_MAX_LATERAL_G:
            case SORT_RC_MAX_LATERAL_G:
            case SORT_RCI_MAX_LATERAL_G:
            {
                std::vector<std::pair<decltype(dummy.max_lateral_g), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->max_lateral_g, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_TOTAL_AIR_TIME:
            case SORT_RC_TOTAL_AIR_TIME:
            case SORT_RCI_TOTAL_AIR_TIME:
            {
                std::vector<std::pair<decltype(dummy.total_air_time), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->total_air_time, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_DROPS:
            case SORT_RC_DROPS:
            case SORT_RCI_DROPS:
            case SORT_W_DROPS:
            {
                std::vector<std::pair<decltype(dummy.drops), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr) // Have to & with magic number 0x3F for some reason
                        trackSortValues.push_back({ trackDesign.get()->drops & 0x3F, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_HIGHEST_DROP_HEIGHT:
            case SORT_RC_HIGHEST_DROP_HEIGHT:
            case SORT_RCI_HIGHEST_DROP_HEIGHT:
            case SORT_W_HIGHEST_DROP_HEIGHT:
            {
                std::vector<std::pair<decltype(dummy.highest_drop_height), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back({ trackDesign.get()->highest_drop_height, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_INVERSIONS:
            case SORT_RCI_INVERSIONS:
            {
                std::vector<std::pair<decltype(dummy.inversions), uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr) // Have to & with magic number 0x1F for some reason.
                        trackSortValues.push_back({ trackDesign.get()->inversions & 0x1F, i });
                }
                SortAlg(trackSortValues);
                break;
            }

            case SORT_TYPE_SPACE_REQUIRED:
            case SORT_RC_SPACE_REQUIRED:
            case SORT_RCI_SPACE_REQUIRED:
            case SORT_W_SPACE_REQUIRED:
            case SORT_T_SPACE_REQUIRED:
            case SORT_M_SPACE_REQUIRED:
            { // uint8_t is too small, so use uin32_t instead of decltype(dummy.space_required_x) (because of multiplication)
                std::vector<std::pair<uint32_t, uint32_t>> trackSortValues{};
                for (uint32_t i{ 0 }; i < _trackDesigns.size(); ++i)
                {
                    const auto& trackDesignFileRef{ _trackDesigns[i] };
                    const auto& trackDesign{ TrackDesignImport(trackDesignFileRef.path.c_str()) };
                    if (trackDesign != nullptr)
                        trackSortValues.push_back( // Compare area
                            { static_cast<uint32_t>(trackDesign.get()->space_required_x)
                                  * static_cast<uint32_t>(trackDesign.get()->space_required_y),
                              i });
                }
                SortAlg(trackSortValues);
                break;
            }
        }
    }

public:
    void OnOpen() override
    {
        String::Set(_filterString, sizeof(_filterString), "");
        window_track_list_widgets[WIDX_FILTER_STRING].string = _filterString;
        widgets = window_track_list_widgets;

        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        {
            widgets[WIDX_BACK].type = WindowWidgetType::Empty;
        }
        else
        {
            widgets[WIDX_BACK].type = WindowWidgetType::TableHeader;
        }

        WindowInitScrollWidgets(*this);
        track_list.track_list_being_updated = false;
        track_list.reload_track_designs = false;
        // Start with first track highlighted
        selected_list_item = 0;
        if (_trackDesigns.size() != 0 && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            selected_list_item = 1;
        }
        gTrackDesignSceneryToggle = false;
        window_push_others_right(*this);
        _currentTrackPieceDirection = 2;
        _trackDesignPreviewPixels.resize(4 * TRACK_PREVIEW_IMAGE_SIZE);

        _loadedTrackDesign = nullptr;
        _loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;

        _trackSortType = SORT_TYPE_NAME;
        _sortAscending = false;
    }

    void OnClose() override
    {
        // Dispose track design and preview
        _loadedTrackDesign = nullptr;
        _trackDesignPreviewPixels.clear();
        _trackDesignPreviewPixels.shrink_to_fit();

        // Dispose track list
        _trackDesigns.clear();

        // If gScreenAge is zero, we're already in the process
        // of loading the track manager, so we shouldn't try
        // to do it again. Otherwise, this window will get
        // another close signal from the track manager load function,
        // try to load the track manager again, and an infinite loop will result.
        if ((gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && gScreenAge != 0)
        {
            window_close_by_number(WindowClass::ManageTrackDesign, number);
            window_close_by_number(WindowClass::TrackDeletePrompt, number);
            Editor::LoadTrackManager();
        }
    }

    void OnMouseUp(const WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_ROTATE:
                _currentTrackPieceDirection++;
                _currentTrackPieceDirection %= 4;
                Invalidate();
                break;
            case WIDX_TOGGLE_SCENERY:
                gTrackDesignSceneryToggle = !gTrackDesignSceneryToggle;
                _loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;
                Invalidate();
                break;
            case WIDX_BACK:
                Close();
                if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
                {
                    ContextOpenWindow(WindowClass::ConstructRide);
                }
                break;
            case WIDX_FILTER_STRING:
                window_start_textbox(
                    *this, widgetIndex, STR_STRING, _filterString, sizeof(_filterString)); // TODO check this out
                break;
            case WIDX_FILTER_CLEAR:
                // Keep the highlighted item selected
                if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
                {
                    if (selected_list_item != -1 && _filteredTrackIds.size() > static_cast<size_t>(selected_list_item))
                        selected_list_item = _filteredTrackIds[selected_list_item];
                    else
                        selected_list_item = -1;
                }
                else
                {
                    if (selected_list_item != 0)
                        selected_list_item = _filteredTrackIds[selected_list_item - 1] + 1;
                }

                String::Set(_filterString, sizeof(_filterString), "");
                FilterList();
                Invalidate();
                break;
            case WIDX_SORT_TRACKS:
                _sortAscending = !_sortAscending;
                SortList();
                Invalidate();
                break;
        }
    }

    void OnMouseDown(WidgetIndex widgetIndex) override
    {
        if (widgetIndex == WIDX_SORT_TYPE_DROPDOWN)
        {
            const auto& widget = widgets[widgetIndex - 1];

            int32_t lastType = _currentLastType - 1;
            int32_t numItems = 0;
            int32_t selectedIndex = -1;
            int32_t strMapping = 0; // Is used to get correct string from dropdown

            for (int32_t type = _currentFirstType; type <= lastType; type++)
            {
                if (type == _trackSortType)
                {
                    selectedIndex = numItems;
                }

                gDropdownItems[numItems].Format = STR_DROPDOWN_MENU_LABEL;
                strMapping = type - _currentFirstType;
                gDropdownItems[numItems].Args = _trackSortTypeStringMapping[strMapping];
                numItems++;
            }
            WindowDropdownShowTextCustomWidth(
                { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height(), colours[1], 0,
                Dropdown::Flag::StayOpen, numItems, widget.width() - 3);
            if (selectedIndex != -1)
            {
                Dropdown::SetChecked(selectedIndex, true);
            }
        }
    }

    void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
    {
        if (widgetIndex == WIDX_SORT_TYPE_DROPDOWN)
        {
            if (dropdownIndex == -1)
                return;

            int32_t sortType = _currentFirstType;
            uint32_t arg = static_cast<uint32_t>(gDropdownItems[dropdownIndex].Args);
            int32_t strMapping = 0; // Is used to get correct string from dropdown

            for (int32_t type = _currentFirstType; type <= (_currentLastType - 1); type++)
            {
                strMapping = type - _currentFirstType;
                if (arg == _trackSortTypeStringMapping[strMapping])
                {
                    sortType = static_cast<int32_t>(type);
                    _currentDropDownSelection = strMapping;
                }
            }

            _trackSortType = sortType;
            SortList();
            Invalidate();
        }
    }

    ScreenSize OnScrollGetSize(const int32_t scrollIndex) override
    {
        size_t numItems = _filteredTrackIds.size();
        if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            // Extra item: custom design
            numItems++;
        }
        int32_t scrollHeight = static_cast<int32_t>(numItems * SCROLLABLE_ROW_HEIGHT);

        return { width, scrollHeight };
    }

    void OnScrollMouseDown(const int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (!track_list.track_list_being_updated)
        {
            int32_t i = GetListItemFromPosition(screenCoords);
            if (i != -1)
            {
                SelectFromList(i);
            }
        }
    }

    void OnScrollMouseOver(const int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (!track_list.track_list_being_updated)
        {
            int32_t i = GetListItemFromPosition(screenCoords);
            if (i != -1 && selected_list_item != i)
            {
                selected_list_item = i;
                Invalidate();
            }
        }
    }

    void OnTextInput(const WidgetIndex widgetIndex, std::string_view text) override
    {
        if (widgetIndex != WIDX_FILTER_STRING || text.empty())
            return;

        if (String::Equals(_filterString, std::string(text).c_str()))
            return;

        String::Set(_filterString, sizeof(_filterString), std::string(text).c_str());

        FilterList();

        scrolls->v_top = 0;

        Invalidate();
    }

    void OnPrepareDraw() override
    {
        // Draw correct string in dropdown
        widgets[WIDX_SORT_TYPE].text = _trackSortTypeStringMapping[_currentDropDownSelection];

        // Draw correct arrow on sort button
        widgets[WIDX_SORT_TRACKS].text = _sortAscending ? STR_SORT_ARROW_DOWN : STR_SORT_ARROW_UP;

        StringId stringId = STR_NONE;
        rct_ride_entry* entry = get_ride_entry(_window_track_list_item.EntryIndex);

        if (entry != nullptr)
        {
            RideNaming rideName = get_ride_naming(_window_track_list_item.Type, *entry);
            stringId = rideName.Name;
        }

        Formatter::Common().Add<StringId>(stringId);
        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        {
            window_track_list_widgets[WIDX_TITLE].text = STR_TRACK_DESIGNS;
            window_track_list_widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_RENAME_OR_DELETE_IT;
        }
        else
        {
            window_track_list_widgets[WIDX_TITLE].text = STR_SELECT_DESIGN;
            window_track_list_widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP;
        }

        if ((gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) || selected_list_item != 0)
        {
            pressed_widgets |= 1uLL << WIDX_TRACK_PREVIEW;
            disabled_widgets &= ~(1uLL << WIDX_TRACK_PREVIEW);
            window_track_list_widgets[WIDX_ROTATE].type = WindowWidgetType::FlatBtn;
            window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WindowWidgetType::FlatBtn;
            if (gTrackDesignSceneryToggle)
            {
                pressed_widgets &= ~(1uLL << WIDX_TOGGLE_SCENERY);
            }
            else
            {
                pressed_widgets |= (1uLL << WIDX_TOGGLE_SCENERY);
            }
        }
        else
        {
            pressed_widgets &= ~(1uLL << WIDX_TRACK_PREVIEW);
            disabled_widgets |= (1uLL << WIDX_TRACK_PREVIEW);
            window_track_list_widgets[WIDX_ROTATE].type = WindowWidgetType::Empty;
            window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WindowWidgetType::Empty;
        }

        // When debugging tools are on, shift everything up a bit to make room for displaying the path.
        const int32_t bottomMargin = gConfigGeneral.DebuggingTools ? (WINDOW_PADDING + DEBUG_PATH_HEIGHT) : WINDOW_PADDING;
        window_track_list_widgets[WIDX_TRACK_LIST].bottom = height - bottomMargin;
        window_track_list_widgets[WIDX_ROTATE].bottom = height - bottomMargin;
        window_track_list_widgets[WIDX_ROTATE].top = window_track_list_widgets[WIDX_ROTATE].bottom
            - ROTATE_AND_SCENERY_BUTTON_SIZE;
        window_track_list_widgets[WIDX_TOGGLE_SCENERY].bottom = window_track_list_widgets[WIDX_ROTATE].top;
        window_track_list_widgets[WIDX_TOGGLE_SCENERY].top = window_track_list_widgets[WIDX_TOGGLE_SCENERY].bottom
            - ROTATE_AND_SCENERY_BUTTON_SIZE;
    }

    void OnUpdate() override
    {
        if (gCurrentTextBox.window.classification == classification && gCurrentTextBox.window.number == number)
        {
            window_update_textbox_caret();
            widget_invalidate(*this, WIDX_FILTER_STRING); // TODO Check this
        }

        if (track_list.reload_track_designs)
        {
            LoadDesignsList(_window_track_list_item);
            selected_list_item = 0;
            Invalidate();
            track_list.reload_track_designs = false;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);

        int32_t listItemIndex = selected_list_item;
        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        {
            if (_trackDesigns.empty() || listItemIndex == -1)
                return;
        }
        else
        {
            if (listItemIndex == 0)
                return;

            // Because the first item in the list is "Build a custom design", lower the index by one
            listItemIndex--;
        }

        int32_t trackIndex = _filteredTrackIds[listItemIndex];

        // Track preview
        auto& tdWidget = widgets[WIDX_TRACK_PREVIEW];
        int32_t colour = ColourMapA[colours[0]].darkest;
        u8string path = _trackDesigns[trackIndex].path;

        // Show track file path (in debug mode)
        if (gConfigGeneral.DebuggingTools)
        {
            utf8 pathBuffer[MAX_PATH];
            const utf8* pathPtr = pathBuffer;
            shorten_path(pathBuffer, sizeof(pathBuffer), path.c_str(), width, FontStyle::Medium);
            auto ft = Formatter();
            ft.Add<utf8*>(pathPtr);
            DrawTextBasic(
                &dpi, windowPos + ScreenCoordsXY{ 0, height - DEBUG_PATH_HEIGHT - 3 }, STR_STRING, ft,
                { colours[1] }); // TODO Check dpi
        }

        auto screenPos = windowPos + ScreenCoordsXY{ tdWidget.left + 1, tdWidget.top + 1 };
        gfx_fill_rect(&dpi, { screenPos, screenPos + ScreenCoordsXY{ 369, 216 } }, colour); // TODO Check dpi

        if (_loadedTrackDesignIndex != trackIndex)
        {
            if (LoadDesignPreview(path))
            {
                _loadedTrackDesignIndex = trackIndex;
            }
            else
            {
                _loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;
            }
        }

        if (!_loadedTrackDesign)
        {
            return;
        }

        auto trackPreview = screenPos;
        screenPos = windowPos + ScreenCoordsXY{ tdWidget.midX(), tdWidget.midY() };

        rct_g1_element g1temp = {};
        g1temp.offset = _trackDesignPreviewPixels.data() + (_currentTrackPieceDirection * TRACK_PREVIEW_IMAGE_SIZE);
        g1temp.width = 370;
        g1temp.height = 217;
        g1temp.flags = G1_FLAG_HAS_TRANSPARENCY;
        gfx_set_g1_element(SPR_TEMP, &g1temp);
        drawing_engine_invalidate_image(SPR_TEMP);
        gfx_draw_sprite(&dpi, ImageId(SPR_TEMP), trackPreview);

        screenPos.y = windowPos.y + tdWidget.bottom - 12;

        // Warnings
        if ((_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE)
            && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            // Vehicle design not available
            DrawTextEllipsised(&dpi, screenPos, 368, STR_VEHICLE_DESIGN_UNAVAILABLE, {}, { TextAlignment::CENTRE });
            screenPos.y -= SCROLLABLE_ROW_HEIGHT;
        }

        if (_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE)
        {
            if (!gTrackDesignSceneryToggle)
            {
                // Scenery not available
                DrawTextEllipsised(
                    &dpi, screenPos, 368, STR_DESIGN_INCLUDES_SCENERY_WHICH_IS_UNAVAILABLE, {}, { TextAlignment::CENTRE });
                screenPos.y -= SCROLLABLE_ROW_HEIGHT;
            }
        }

        // Track design name
        auto ft = Formatter();
        ft.Add<const utf8*>(_trackDesigns[trackIndex].name.c_str());
        DrawTextEllipsised(&dpi, screenPos, 368, STR_TRACK_PREVIEW_NAME_FORMAT, ft, { TextAlignment::CENTRE });

        // Information
        screenPos = windowPos + ScreenCoordsXY{ tdWidget.left + 1, tdWidget.bottom + 2 };

        // Stats
        ft = Formatter();
        ft.Add<fixed32_2dp>(_loadedTrackDesign->excitement * 10);
        DrawTextBasic(&dpi, screenPos, STR_TRACK_LIST_EXCITEMENT_RATING, ft);
        screenPos.y += LIST_ROW_HEIGHT;

        ft = Formatter();
        ft.Add<fixed32_2dp>(_loadedTrackDesign->intensity * 10);
        DrawTextBasic(&dpi, screenPos, STR_TRACK_LIST_INTENSITY_RATING, ft);
        screenPos.y += LIST_ROW_HEIGHT;

        ft = Formatter();
        ft.Add<fixed32_2dp>(_loadedTrackDesign->nausea * 10);
        DrawTextBasic(&dpi, screenPos, STR_TRACK_LIST_NAUSEA_RATING, ft);
        screenPos.y += LIST_ROW_HEIGHT + 4;

        // Information for tracked rides.
        if (GetRideTypeDescriptor(_loadedTrackDesign->type).HasFlag(RIDE_TYPE_FLAG_HAS_TRACK))
        {
            const auto& rtd = GetRideTypeDescriptor(_loadedTrackDesign->type);
            if (!rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
            {
                if (_loadedTrackDesign->type == RIDE_TYPE_MINI_GOLF)
                {
                    // Holes
                    ft = Formatter();
                    ft.Add<uint16_t>(_loadedTrackDesign->holes & 0x1F);
                    DrawTextBasic(&dpi, screenPos, STR_HOLES, ft);
                    screenPos.y += LIST_ROW_HEIGHT;
                }
                else
                {
                    // Maximum speed
                    ft = Formatter();
                    ft.Add<uint16_t>(((_loadedTrackDesign->max_speed << 16) * 9) >> 18);
                    DrawTextBasic(&dpi, screenPos, STR_MAX_SPEED, ft);
                    screenPos.y += LIST_ROW_HEIGHT;

                    // Average speed
                    ft = Formatter();
                    ft.Add<uint16_t>(((_loadedTrackDesign->average_speed << 16) * 9) >> 18);
                    DrawTextBasic(&dpi, screenPos, STR_AVERAGE_SPEED, ft);
                    screenPos.y += LIST_ROW_HEIGHT;
                }

                // Ride length
                ft = Formatter();
                ft.Add<StringId>(STR_RIDE_LENGTH_ENTRY);
                ft.Add<uint16_t>(_loadedTrackDesign->ride_length);
                DrawTextEllipsised(&dpi, screenPos, 214, STR_TRACK_LIST_RIDE_LENGTH, ft);
                screenPos.y += LIST_ROW_HEIGHT;
            }

            if (GetRideTypeDescriptor(_loadedTrackDesign->type).HasFlag(RIDE_TYPE_FLAG_HAS_G_FORCES))
            {
                // Maximum positive vertical Gs
                ft = Formatter();
                ft.Add<int32_t>(_loadedTrackDesign->max_positive_vertical_g * 32);
                DrawTextBasic(&dpi, screenPos, STR_MAX_POSITIVE_VERTICAL_G, ft);
                screenPos.y += LIST_ROW_HEIGHT;

                // Maximum negative vertical Gs
                ft = Formatter();
                ft.Add<int32_t>(_loadedTrackDesign->max_negative_vertical_g * 32);
                DrawTextBasic(&dpi, screenPos, STR_MAX_NEGATIVE_VERTICAL_G, ft);
                screenPos.y += LIST_ROW_HEIGHT;

                // Maximum lateral Gs
                ft = Formatter();
                ft.Add<int32_t>(_loadedTrackDesign->max_lateral_g * 32);
                DrawTextBasic(&dpi, screenPos, STR_MAX_LATERAL_G, ft);
                screenPos.y += LIST_ROW_HEIGHT;

                if (_loadedTrackDesign->total_air_time != 0)
                {
                    // Total air time
                    ft = Formatter();
                    ft.Add<int32_t>(_loadedTrackDesign->total_air_time * 25);
                    DrawTextBasic(&dpi, screenPos, STR_TOTAL_AIR_TIME, ft);
                    screenPos.y += LIST_ROW_HEIGHT;
                }
            }

            if (GetRideTypeDescriptor(_loadedTrackDesign->type).HasFlag(RIDE_TYPE_FLAG_HAS_DROPS))
            {
                // Drops
                ft = Formatter();
                ft.Add<uint16_t>(_loadedTrackDesign->drops & 0x3F);
                DrawTextBasic(&dpi, screenPos, STR_DROPS, ft);
                screenPos.y += LIST_ROW_HEIGHT;

                // Drop height is multiplied by 0.75
                ft = Formatter();
                ft.Add<uint16_t>((_loadedTrackDesign->highest_drop_height * 3) / 4);
                DrawTextBasic(&dpi, screenPos, STR_HIGHEST_DROP_HEIGHT, ft);
                screenPos.y += LIST_ROW_HEIGHT;
            }

            if (_loadedTrackDesign->type != RIDE_TYPE_MINI_GOLF)
            {
                uint16_t inversions = _loadedTrackDesign->inversions & 0x1F;
                if (inversions != 0)
                {
                    ft = Formatter();
                    ft.Add<uint16_t>(inversions);
                    // Inversions
                    DrawTextBasic(&dpi, screenPos, STR_INVERSIONS, ft);
                    screenPos.y += LIST_ROW_HEIGHT;
                }
            }
            screenPos.y += 4;
        }

        if (_loadedTrackDesign->space_required_x != 0xFF)
        {
            // Space required
            ft = Formatter();
            ft.Add<uint16_t>(_loadedTrackDesign->space_required_x);
            ft.Add<uint16_t>(_loadedTrackDesign->space_required_y);
            DrawTextBasic(&dpi, screenPos, STR_TRACK_LIST_SPACE_REQUIRED, ft);
            screenPos.y += LIST_ROW_HEIGHT;
        }

        if (_loadedTrackDesign->cost != 0)
        {
            ft = Formatter();
            ft.Add<uint32_t>(_loadedTrackDesign->cost);
            DrawTextBasic(&dpi, screenPos, STR_TRACK_LIST_COST_AROUND, ft);
        }
    }

    void OnScrollDraw(const int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        uint8_t paletteIndex = ColourMapA[colours[0]].mid_light;
        gfx_clear(&dpi, paletteIndex);

        auto screenCoords = ScreenCoordsXY{ 0, 0 };
        size_t listIndex = 0;
        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        {
            if (_trackDesigns.empty())
            {
                // No track designs
                DrawTextBasic(&dpi, screenCoords - ScreenCoordsXY{ 0, 1 }, STR_NO_TRACK_DESIGNS_OF_THIS_TYPE);
                return;
            }
        }
        else
        {
            // Build custom track item
            StringId stringId;
            if (listIndex == static_cast<size_t>(selected_list_item))
            {
                // Highlight
                gfx_filter_rect(
                    &dpi, { screenCoords, { width, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1 } },
                    FilterPaletteID::PaletteDarken1);
                stringId = STR_WINDOW_COLOUR_2_STRINGID;
            }
            else
            {
                stringId = STR_BLACK_STRING;
            }

            auto ft = Formatter();
            ft.Add<StringId>(STR_BUILD_CUSTOM_DESIGN);
            DrawTextBasic(&dpi, screenCoords - ScreenCoordsXY{ 0, 1 }, stringId, ft);
            screenCoords.y += SCROLLABLE_ROW_HEIGHT;
            listIndex++;
        }

        for (auto i : _filteredTrackIds)
        {
            if (screenCoords.y + SCROLLABLE_ROW_HEIGHT >= dpi.y && screenCoords.y < dpi.y + dpi.height)
            {
                StringId stringId;
                if (listIndex == static_cast<size_t>(selected_list_item))
                {
                    // Highlight
                    gfx_filter_rect(
                        &dpi, { screenCoords, { width, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1 } },
                        FilterPaletteID::PaletteDarken1);
                    stringId = STR_WINDOW_COLOUR_2_STRINGID;
                }
                else
                {
                    stringId = STR_BLACK_STRING;
                }

                // Draw track name
                auto ft = Formatter();
                ft.Add<StringId>(STR_TRACK_LIST_NAME_FORMAT);
                ft.Add<const utf8*>(_trackDesigns[i].name.c_str());
                DrawTextBasic(&dpi, screenCoords - ScreenCoordsXY{ 0, 1 }, stringId, ft);
            }

            screenCoords.y += SCROLLABLE_ROW_HEIGHT;
            listIndex++;
        }
    }

    bool SetRideSelection(const RideSelection item)
    {
        _window_track_list_item = item;
        LoadDesignsList(item);
        return true;
    }

    void SelectDropDown(const RideSelection item)
    {
        switch (item.Type)
        {
            // Roller coasters, Ghost Train and Dinghy
            case 0:  // Spiral Roller Coaster
            case 1:  // Stand-up Roller Coaster
            case 4:  // Junior Roller Coasters
            case 7:  // Mini Suspended Coaster
            case 9:  // Wooden Wild Mouse
            case 15: // Looping Roller Coaster
            case 16: // Dinghy
            case 17: // Mine Train Coaster
            case 50: // Ghost Train
            case 52: // Wooden Roller Coasters
            case 54: // Steel Wild Mouse
            case 87: // Mini Roller Coasters
            case 88: // Mine Ride
            case 94: // Spinning Wild Mouse
                _trackSortTypeStringMapping = rc_sort_type_string_mapping;
                _currentFirstType = SORT_RC_NAME;
                _currentLastType = DROPDOWN_LIST_COUNT_RC;
                break;

            // Roller coasters with inversions
            case 19: // Corkscrew Roller Coaster
            case 66: // Heartline Twister
            case 73: // Compact Inverted Roller Coaster
                _trackSortTypeStringMapping = rci_sort_type_string_mapping;
                _currentFirstType = SORT_RCI_NAME;
                _currentLastType = DROPDOWN_LIST_COUNT_RCI;
                break;

            // Transport Rides and others
            case 5:  // Miniature Railway
            case 6:  // Monorail
            case 8:  // Boat hire
            case 11: // Car ride
            case 12: // Launched Freefall
            case 14: // Observation tower
            case 18: // Chairlift
            case 22: // Go Karts
            case 61: // Mini Helicopters
            case 72: // Monorail Cycles
            case 79: // River Rafts
                _trackSortTypeStringMapping = tw_sort_type_string_mapping;
                _currentFirstType = SORT_T_NAME;
                _currentLastType = DROPDOWN_LIST_COUNT_T;
                break;

            // Water Rides
            case 23: // Log Flume
            case 24: // River Rapids
            case 60: // Splash Boats
                _trackSortTypeStringMapping = drs_sort_type_string_mapping;
                _currentFirstType = SORT_W_NAME;
                _currentLastType = DROPDOWN_LIST_COUNT_W;
                break;

            case 20: // Maze
                _trackSortTypeStringMapping = fr_sort_type_string_mapping;
                _currentFirstType = SORT_M_NAME;
                _currentLastType = DROPDOWN_LIST_COUNT_M;
                break;

            case 67: // Minigolf
                _trackSortTypeStringMapping = mg_sort_type_string_mapping;
                _currentFirstType = SORT_MG_NAME;
                _currentLastType = DROPDOWN_LIST_COUNT_MG;
                break;

            // No match, show all sort types
            default:
                _trackSortTypeStringMapping = default_sort_type_string_mapping;
                _currentFirstType = SORT_TYPE_NAME;
                _currentLastType = DROPDOWN_LIST_COUNT;
                break;
        }
    }
};

rct_window* WindowTrackListOpen(const RideSelection item)
{
    window_close_construction_windows();
    ScreenCoordsXY screenPos{};
    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
    {
        int32_t screenWidth = ContextGetWidth();
        int32_t screenHeight = ContextGetHeight();
        screenPos = { screenWidth / 2 - 300, std::max(TOP_TOOLBAR_HEIGHT + 1, screenHeight / 2 - 200) };
    }
    else
    {
        screenPos = { 0, TOP_TOOLBAR_HEIGHT + 2 };
    }
    auto* w = WindowCreate<TrackListWindow>(WindowClass::TrackDesignList, WW, WH, 0);
    w->SetRideSelection(item);
    w->SelectDropDown(item);
    return w;
}
